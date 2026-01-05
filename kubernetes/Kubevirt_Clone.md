# VM 快照/克隆

## 方案概述

### 前置条件

1. Kubevirt 虚拟机克隆依赖于虚拟机快照和快照恢复能力，而快照和快照恢复功能要求虚拟机所有的`Volume/PVC`关联的存储类必须要支持`VolumeSnaptshot` 和 `VolumeSnaptshotRestore`，这里的`VolumeSnaptshotRestore`指的是创建快照克隆卷的能力，即相应的`PV provider`需要支持使用`VolumeSnapshotContent`作为数据源来创建新的`PV`。 
2. `OpenEBS Local PV LVM`仅支持`VolumeSnapshot`，没有实现基于快照卷的克隆机制，即创建基于`VolumeContentSource`的`LVMVolume`，`LVMVolume`也不支持`VolumeContentSource`扩展。所以， `OpenEBS Local PV LVM`不支持`VolumeSnaptshotRestore`，只能用于数据备份的场景，无法实现快照恢复和克隆。LVM 快照本身存在以下局限性，不适合支持这些高级场景：
	1. 基于LVM普通卷来实现快照和快照克隆卷，会极其浪费存储空间。比如一个10G的数据卷，创建一个快照就需要创建一个10G的快照卷（可以设置得更小一点），每执行一次快照恢复就需要创建一个新的10G快照克隆卷，并且由于要保留其它快照还无法删除原数据卷。
	2. LVM thin provision volume比较节省空间，但是存在如下的问题，如果针对thin volume创建多个快照，恢复（merge）其中一个快照时会让其他快照变得不可用（至少在快照没有数据的情况下如此，比如虚拟机关机连续创建快照）。
3. `OpenEBS Local PV ZFS`同时支持`VolumeSnapshot`和`VolumeSnaptshotRestore`，安装相应的`zfs operator` 即可，需要用户或者运维安装zfs工具包并创建好相应的`zpool`；
4. 其它存储类例如`ceph rbd`也支持`VolumeSnaptshot` & `VolumeSnaptshotRestore`，但开发尚未验证。
5. 目前仅在`Ubunut 20.04`系统上验证过基于`OpenEBS Local PV ZFS`的虚拟机快照和克隆。对于其它系统，如果没有现场的`zfs`工具包进行安装，可以从源码编译然后提供相应的安装包。

#### ZFS快照特性

1. ZFS实例可以创建多个快照，但是只能使用最新的快照直接执行回滚（`zfs rollback`）操作；想要回滚到非最新快照，需要先销毁（destroy）之后的快照，再执行rollback操作，或者直接使用`-r`参数强制删除之后的快照。
2. 基于ZFS实例的快照创建的克隆会共享源快照的数据块，即克隆卷的大部分数据块是直接引用zpool中已有的数据块。
3. 如果删除ZFS实例的快照时，存储基于该快照的克隆，则必须指定`-R`参数将克隆一起删除。

> 基于ZFS的这几个特性，我们可以理解`Kubevirt`的虚拟机恢复和虚拟机克隆为什么都是基于快照创建克隆来实现。

### 方案设计

> 创建虚拟机的时候支持指定`StorageClass`，缺省值为集群中的默认`StorageClass`（`OpenEBS hostpath-localpv`）。

#### 基本流程

**虚拟机快照后台流程（异步操作）**：

1. 检查虚拟机和快照是否已经存在。
2. 创建`VirtualMachineSnapshot`资源下发快照恢复任务，`POST`接口不会等待恢复结束，直接返回。因为快照操作一般是比较快的，后端接口也可以实现为：如果快照任务能在一定时间内（例如3秒）结束，则直接返回结果200或者失败错误码。否则，返回202，表示任务仍在执行中。
3. 前端通过虚拟机快照子页面查询恢复任务状态。

**虚拟机快照恢复后台流程（异步操作）**：

1. 检查虚拟机和快照是否存在。
2. 检查虚拟机是否关机。
3. 创建`VirtualMachineRestore`资源下发快照恢复任务，`POST`接口不会等待恢复结束，直接返回。
4. 前端通过虚拟机恢复记录子页面查询恢复任务状态。

**虚拟机克隆后台流程（异步操作）**：

1. 检查源虚拟机和目标虚拟机是否已经存在。
2. 检查源虚拟机除了`containerdisk`外所有`Volume`是否配置了关联的`VolumeSnapshotClass`，如果没有则直接报错。
3. 创建`VirtualMachineClone`资源下发虚拟机克隆任务，`POST`接口返回。`VirtualMachineClone`控制器会依次执行虚拟机克隆、创建目标虚拟机、快照恢复（即用卷快照作为数据源创建新的卷，一般以`restore-`开头）来完成虚拟机的克隆。
4. 协程监视`VirtualMachineClone`资源的状态变化；
5. `VirtualMachineClone`执行成功后（如果失败则会删除目标虚拟机，不会删除`VirtualMachineClone`资源）:
   1. 更新克隆虚拟机的标签。
   2. 创建`service`和`ingress`资源。
   3. 启动虚拟机。

#### Kubevirt CDI
对于那些使用了不支持`VolumeSnapshot`的存储类的虚拟机，无法通过快照和快照恢复来实现虚拟机克隆。作为基于`VolumeSnapshot`的克隆方案的`Fallback`机制，可以使用基于`Kubevirt CDI`的克隆方案，其工作流程如下：

1. 检查源虚拟机是否处于关机状态，如果没有则直接抛出错误要求用户先执行关机。
2. 针对虚拟机每个`Volume`（`PVC`或者`DataVolume`）创建相同大小的`PVC`，这些新建的`PVC`将作为克隆虚拟机的`Volume`。
3. 将源虚拟机的`Volume`通过`Kubevirt CDI`克隆成`DataVolume`。这一步骤会非常耗时，因为`Kubevirt CDI`的工作原理就是启动一个负责数据拷贝的pod，并将源Volume和目标Volume都挂载到pod内，然后执行文件系统拷贝操作。
4. 基于源虚拟机的`spec`创建一个全新的目标虚拟机资源，必要时生成新的`MacAddresses`和`SMBiosSerial`，目标虚拟机使用的`Volume`均来源于前面克隆的`DataVolume`。

#### 快照恢复存储资源优化

参考Kubevirt虚拟机快照恢复原理，对于虚拟机磁盘的恢复实际是基于卷快照创建新的克隆卷，然后让虚拟机使用新的克隆卷。以openebs-zfs存储类为例，这个流程存在两个问题：

1. 虽然克隆卷不会额外消耗很多存储空间，克隆卷会直接引用原卷的数据，新的克隆卷实际上只占用不到20KB，只有写入新的数据才会消耗存储空间。也正因如此，原卷也不能被删除，这就导致原卷上自创建快照后写入的数据无法被删除，相应的存储空间也就不能被释放。

2. 执行快照恢复过程中会产生被弃用的PVC，随着执行快照恢复次数的增多，这种未被使用的PVC会越来越多，给后期集群运维引入复杂度。手动删除这些PVC的结果：
   1. PVC、PV资源会被删除（保留策略是Delete，如果是Retain，PV不会被删除）。
   
   2. 创建虚拟机快照生成的`VirtualMachineSnapshot`、`VirtualMachineSnapshotContent`、`VolumeSnapshot`、`VolumeSnapshotContent`、`ZFSSnapshot`这些集群资源不会被删除；`ZFSVolume`和zpool里的dataset&snapshot能否被删除，取决于dataset的快照是否存在任何链接克隆，因为ZFS Driver Node Server使用`zfs destroy -r xxx`删除dataset，如果只有快照，快照会被一起删除，如果存在克隆，需要使用`-R`才能强制删除。但是ZFS Driver Node Server会一直重试删除操作，即任何时候链接克隆被删除，相应的`ZFSVolume`也会立马被删除。
   
      > 如果执行虚拟机快照恢复完成，虚拟机还未启动，新的PVC实际上还处于Pending状态，这个时候删除了源PVC，那么底层zpool里的dataset也会被清理（PV provision的时候才会创建链接克隆），虚拟机会因为PV provision失败无法启动。

考虑在虚拟机控制面做一些优化处理（以openebs-zfs存储类为例）：在快照恢复任务结束后，直接启动虚拟机，并且后台等待虚拟机进入Running状态（新的PVC绑定完成）直接删除源PVCs，删除之前先检查源PVC底层的ZFS dataset是否有快照，如果有，则将其底层的ZFS dataset恢复到最新的快照（需要到节点上执行`zfs rollback`），以释放不再使用的存储空间。这个快照可能是创建虚拟机快照产生的，也可能是虚拟机克隆临时生成的快照，这些快照和dataset都不能被手动删除。虚拟机克隆完成后，临时快照相关的`VirtualMachineSnapshot`和`VolumeSnapshot`资源会自动被删除，但是底层的zpool dataset snapshot不会被删除。回滚快照后删除PVCs也主要是为了清理集群里的对象资源，避免反复执行快照回滚后残留大量无用的PVC资源。

> 那什么场景下快照恢复的源PVC没有快照呢？主要是快照恢复后再次执行快照恢复，这个时候源PVC是可能没有任何快照的。

检查源PVC快照的具体流程（以ubuntu-zfs1-sys-disk pvc为例）：

1. 从pvc获取volumeName：

   ```bash
   root@master:~# kubectl get pvc ubuntu-zfs1-sys-disk -o jsonpath='{.spec.volumeName}'
   pvc-3e340461-2450-4e8b-b86d-b1768e5a6fcf
   ```

2. 通过label selector查找是否有对应的ZFSSnapshot：

   ```bash
   root@master:~# kubectl get zfssnapshot -n openebs --selector openebs.io/persistent-volume=pvc-3e340461-2450-4e8b-b86d-b1768e5a6fcf
   NAME                                            AGE
   snapshot-8891cde1-843a-49bf-b5ce-f95dd6221a89   4h9m
   ```

3. 如果#2没有找到相关ZFSSnapshot，直接删除ubuntu-zfs1-sys-disk pvc；

4. 如果#2找到多个相关ZFSSnapshot，通过创建时间排序取最新的那个然后执行zfs rollback。rollback成功后直接删除ubuntu-zfs1-sys-disk pvc。
## 使用手册

### 部署使用

1. 部署 kubevirt operator。
2. 在集群节点上安装`zfs`工具包（对于无法支持`zfs`的节点可以暂时不安装）。
3. 在所有支持`zfs`的节点上创建相同名字的`zpool`，推荐使用独立的硬盘或者分区来创建`zpool`，示例：
   ```bash
   root@master:~/hebo/zfspv# apt-get install zfsutils-linux
   root@master:~/hebo/zfspv# zpool create zfspv-pool /dev/sdb3
   root@master:~/hebo/zfspv# zpool status
     pool: zfspv-pool
    state: ONLINE
     scan: none requested
   config:
   
           NAME        STATE     READ WRITE CKSUM
           zfspv-pool  ONLINE       0     0     0
             sdb3      ONLINE       0     0     0
   
   errors: No known data errors
   ```
1. 创建`openebs-zfspv`存储类（后续会将该存储类的支持做到存储管理里）。如果用户想用其它支持快照和恢复的存储类型，例如`ceph rbd`，还需要部署相应的`snapshotclass`，具体操作参考后面的Kubevirt调研部分：
   ```bash
   root@master:~/hebo/zfspv# cat zfspv-sc.yaml
   apiVersion: storage.k8s.io/v1
   kind: StorageClass
   metadata:
     name: openebs-zfspv
   parameters:
     recordsize: "128k"
     compression: "off"
     dedup: "off"
     fstype: "zfs"           # 也支持ext4
     poolname: "zfspv-pool"  # 使用上面创建好的zpool
   provisioner: zfs.csi.openebs.io
   volumeBindingMode: WaitForFirstConsumer
   allowedTopologies:
   - matchLabelExpressions:
     - key: kubernetes.io/hostname
       values:
         - master1
         - master2
   root@master:~/hebo/zfspv# kubectl create -f zfspv-sc.yaml
   storageclass.storage.k8s.io/openebs-zfspv created
   root@master:~/hebo/zfspv# kubectl get sc openebs-zfspv
   NAME            PROVISIONER          RECLAIMPOLICY   VOLUMEBINDINGMODE   ALLOWVOLUMEEXPANSION   AGE
   openebs-zfspv   zfs.csi.openebs.io   Delete          Immediate           false                  3s
   ```

1. 创建虚拟机，并指定存储类为`openebs-zfspv`。
2. 执行虚拟机快照、克隆、快照恢复等操作。
### 运维事项

#### zpool空间不足

执行`zfs list`查询`zpool`空间使用情况，如果空间不足需要及时扩容，否则`zpool`空间使用完后会导致虚拟机在写入数据时出现`IOError`，导致虚拟机处于挂起（`Pasued`）状态。

```bash
root@master:~# zfs list
NAME                                                  USED  AVAIL     REFER  MOUNTPOINT
zfspv-pool                                           11.2G   277G       24K  /zfspv-pool
zfspv-pool/pvc-88c0cc79-ca78-4ddf-86be-19efc55299f9  3.31G  16.7G     3.26G  legacy
zfspv-pool/pvc-935c693a-6f48-4ccf-9c97-7118ea69c64f  4.30G  15.7G     7.01G  legacy
zfspv-pool/pvc-e7352a4d-ffdf-4c41-ad02-82c7fc3a2331  2.58G  17.4G     5.03G  legacy
zfspv-pool/pvc-faf1b4d1-fff9-4f0f-ac43-44cfb142b0f9  1.04G  19.0G     3.82G  legacy
```

#### zpool scrub

基于前面提到的ZFS快照特性，在使用虚拟机快照&快照恢复&克隆功能的过程中，`zpool`中可能会逐渐产生很多没有被引用的数据块。建议定期执行scrub操作清理没有被引用的数据块以释放存储空间。

> `zfs scrub <zpool name>`操作会扫描`zpool`里的所有数据块，对性能有影响。建议将节点上的虚拟机先关机，或者虚拟机没有业务运行的情况下执行该操作。

```bash
root@master:~# zpool status
  pool: zfspv-pool
 state: ONLINE
  scan: scrub repaired 0B in 0 days 00:00:02 with 0 errors on Sun Dec  8 00:24:03 2024
config:

        NAME        STATE     READ WRITE CKSUM
        zfspv-pool  ONLINE       0     0     0
          sdb3      ONLINE       0     0     0

errors: No known data errors

root@master:~# zpool scrub zfspv-pool

root@master:~# zpool status
  pool: zfspv-pool
 state: ONLINE
  scan: scrub in progress since Tue Dec 10 09:43:21 2024
        7.05G scanned at 802M/s, 1.01G issued at 115M/s, 7.05G total
        0B repaired, 14.31% done, 0 days 00:00:53 to go
config:

        NAME        STATE     READ WRITE CKSUM
        zfspv-pool  ONLINE       0     0     0
          sdb3      ONLINE       0     0     0
```
### ZFS性能评估
由于`Kubevirt`虚拟机是将k8s pvc作为volume挂载到`virt-launcher` pod里然后用`dd`命令创建`disk.img`虚拟机块设备作为硬盘挂载到`qemu`虚拟机里，然后安装过程中创建根文件系统提供给虚拟机内的应用使用。这就导致`Kubevirt`虚拟机硬盘使用的I/O路径比较复杂，涉及到以下几个关键I/O路径（自底向上）：

1. 存储设备：硬盘（`/dev/sdb`）、分区（`/dev/sdb3`）或者逻辑卷（`/dev/mapper/ubuntu--vg-ubuntu--lv`）。
2. zfs（`zfspv-pool/pvc-935c693a-6f48-4ccf-9c97-7118ea69c64f`）。
3. 虚拟机（`/var/lib/kubelet/pods/4cc746c8-3dd4-47f2-b191-e2427bcdf028/volumes/kubernetes.io~csi/pvc-935c693a-6f48-4ccf-9c97-7118ea69c64f/mount/test.img`）。

下面对虚拟机I/O路径上的关键节点做一些简单性能测试：

| 测试点             | 4K读/写（400MiB）       | 4M读/写（400MiB）     |
| ------------------ | ----------------------- | --------------------- |
| 存储设备（ext4）   | 1.3 GB/s     / 199 MB/s | 3.6 GB/s  /  234 MB/s |
| zfs                | 520 MB/s  / 138 MB/s    | 2.1 GB/s  / 221 MB/s  |
| 虚拟机应用（ext4） | 183 MB/s  /  114 MB/s   | 3.6 GB/s  / 178 MB/s  |

粗略观察一下虚拟机内的性能损耗（主要相比于直接使用裸设备）：

1. 4K读性能下降非常严重。
2. 4M读性能基本没有损耗，4M写性能下降也不算很严重。

> 测试方法（以zfs为例，每个测试命令执行3次，分别读写不同文件，最后取中位数）：
>
> `dd if=/dev/random of=/var/lib/kubelet/pods/4cc746c8-3dd4-47f2-b191-e2427bcdf028/volumes/kubernetes.io~csi/pvc-935c693a-6f48-4ccf-9c97-7118ea69c64f/mount/test.img bs=4K count=102400` // 4K写
>
> ``dd of=/dev/null if=/var/lib/kubelet/pods/4cc746c8-3dd4-47f2-b191-e2427bcdf028/volumes/kubernetes.io~csi/pvc-935c693a-6f48-4ccf-9c97-7118ea69c64f/mount/test2.img bs=4K count=102400` // 4k读

> `dd if=/dev/random of=/var/lib/kubelet/pods/4cc746c8-3dd4-47f2-b191-e2427bcdf028/volumes/kubernetes.io~csi/pvc-935c693a-6f48-4ccf-9c97-7118ea69c64f/mount/test.img bs=4M count=100`  // 4M写
>
> `dd of=/dev/null if=/var/lib/kubelet/pods/4cc746c8-3dd4-47f2-b191-e2427bcdf028/volumes/kubernetes.io~csi/pvc-935c693a-6f48-4ccf-9c97-7118ea69c64f/mount/test.img bs=4M count=100` // 4M读

# Kubevirt 调研

关于虚拟机的存储设置参考：https://kubevirt.io/user-guide/storage/disks_and_volumes/#thick-and-thin-volume-provisioning

## Snapshot & Restore

### 前置条件

1. 在Kubevirt CR中开启`Snapshot`特性网关。
2. 想要使用Snapshot & Restore的完整功能，k8s集群中的CSI Driver（OpenEBS）需要先配置好匹配的`VolumeSnapshotClass`，可以通过`kubectl get volumesnapshotclass`查看。如果CSI Driver不支持`VolumeSnapshotClass`，创建`VirtualMachineSnapshots`时，Kubevirt只会备份`VirtualMachine`的配置信息，而不包含任何数据卷。

### Snapshot a VM

Kubevirt允许对在线/离线虚拟机执行Snapshot：

1. 在线虚拟机的snapshot逻辑如下：

   > When snapshotting a running vm the controller will check for qemu guest agent in the vm. If the agent exists it will freeze the vm filesystems before taking the snapshot and unfreeze after the snapshot. It is recommended to take online snapshots with the guest agent for a better snapshot, if not present a best effort snapshot will be taken.

   如果虚拟机没有安装`qemu guest agent`，先对虚拟机关机再执行snapshot更安全。

2. 离线虚拟机：安全稳妥的方式。

Snapshot demo：

```yaml
apiVersion: snapshot.kubevirt.io/v1beta1
kind: VirtualMachineSnapshot
metadata:
  name: snap-larry
spec:
  source:
    apiGroup: kubevirt.io
    kind: VirtualMachine
    name: larry
  # 默认snapshot超时时间为5分钟，超时后，snapshot被标记成Failed，虚拟机被unfrozen，创建好的快照内容会被清理
  failureDeadline: 1m
```

可用如下命令等待Snapshot完成，也可以查询VirtualMachineSnapshot的状态，结果为：InProgress、Succeeded或者Failed。
```bash
kubectl wait vmsnapshot snap-larry --for condition=Ready
```

### Restore a VM

从VirtualMachineSnapshot执行虚拟机Restore：

```yaml
apiVersion: snapshot.kubevirt.io/v1beta1
kind: VirtualMachineRestore
metadata:
  name: restore-larry
spec:
  target:
    apiGroup: kubevirt.io
    kind: VirtualMachine
    name: larry
  virtualMachineSnapshotName: snap-larry
```

可用如下命令等待虚拟机恢复完成：
```yaml
kubectl wait vmrestore restore-larry --for condition=Ready
```

## Clone - 原生方案

### Clone a VM

发起虚拟机克隆作业和快照类似，也是先创建一个如下的`VirtualMachineClone`对象（CRD）：

```yaml
kind: VirtualMachineClone
apiVersion: "clone.kubevirt.io/v1alpha1"
metadata:
  name: testclone

spec:
  # source & target definitions
  source:
    apiGroup: kubevirt.io
    kind: VirtualMachine
    name: vm-cirros
  target:
    apiGroup: kubevirt.io
    kind: VirtualMachine
    name: vm-clone-target

  # labels & annotations definitions
  labelFilters:
    - "*"
    - "!someKey/*"
  annotationFilters:
    - "anotherKey/*"

  # template labels & annotations definitions
  template:
    labelFilters:
      - "*"
      - "!someKey/*"
    annotationFilters:
      - "anotherKey/*"

  # other identity stripping specs:
  newMacAddresses:
    interfaceName: "00-11-22"
  newSMBiosSerial: "new-serial"
```

其中：

1. source/target定义了克隆的源/目标对象的API group，kind和name。
   1. 目前支持的kind除了`VirtualMachine`，还有`VirtualMachineSnapshot`（`snapshot.kubevirt.io` api group），即可以克隆的对象包含虚拟机或者虚拟机快照。
   2. target名字可以不指定，克隆控制器会自动生成一个名字。
   3. source和target必须在相同的namespace。
2. labels & annotations过滤器定义了克隆虚拟机时需要拷贝的标签和注解，支持模糊匹配；如果不设置默认拷贝所有标签和注解。
3. template labels & annotations过滤器用来过滤虚拟机模板里的标签和注解，一些网络配置信息可能被注入到虚拟机的标签和注解里（例如Kube-OVN / OVN-Kubernetes），这个过滤器可以避免克隆的虚拟机使用相同的网络。

创建虚拟机克隆并等待克隆完成：

```bash
kubectl create -f clone.yaml
kubectl wait vmclone testclone --for condition=Ready
```

`VirtualMachineClone`对象的状态包含了克隆任务执行的阶段，前3个代表了克隆任务执行的3个关键步骤：

- SnapshotInProgress
- CreatingTargetVM
- RestoreInProgress
- Succeeded
- Failed
- Unknown

克隆完成后，可以通过`kubectl get vm vm-clone-target -o yaml`查看目标虚拟机的信息。
### Cancel a Cloning
正在执行的克隆作业允许被Stop/Cancel。
关于克隆操作的UI设计，可以参考[clone vm](https://kubevirt.io/web-ui-design/ui-design/virtual-machines/clone-vm/clone-vm.html)。
## Clone - CDI

CDI的架构设计：[Architectue](https://github.com/kubevirt/containerized-data-importer/blob/main/doc/design.md#components)。

参考https://kubevirt.io/user-guide/storage/containerized_data_importer/ 和 https://github.com/kubevirt/containerized-data-importer：

>The [Containerized Data Importer](https://github.com/kubevirt/containerized-data-importer) (CDI) project provides facilities for enabling [Persistent Volume Claims](https://kubernetes.io/docs/concepts/storage/persistent-volumes/) (PVCs) to be used as disks for KubeVirt VMs by way of [DataVolumes](https://github.com/kubevirt/containerized-data-importer/blob/main/doc/datavolumes.md). The three main CDI use cases are:
>
>- Import a disk image from a web server or container registry to a DataVolume
>- Clone an existing PVC to a DataVolume
>- Upload a local disk image to a DataVolume
>
>CDI provides the ability to populate PVCs with VM images or other data upon creation. The data can come from different sources: a URL, a container registry, another PVC (clone), or an upload from a client.

如上，CDI支持将已经存在的PVC克隆到一个DataVolume里，然后让虚拟机以DataVolume的方式将其作为disk使用。DataVolume是CDI引入的一个K8s CRD，它是基于k8s PVC之上的一个抽象，用于快捷地创建和发布带有数据的PVC。

基于DataVolume以及后面介绍的DataVolume克隆（Host-Assisted Cloning）可以实现不依赖于`VolumeSnapshotClass`的虚拟机克隆方案，即：

1. 将source VM的PVC通过CDI克隆成DataVolume；
2. 基于source VM的spec创建一个全新的target VM，然后target VM使用的disk均来源于前面克隆的DataVolume。

See [Data Volumes](https://github.com/kubevirt/containerized-data-importer/blob/main/doc/datavolumes.md) fro more details。基于DataVolume的虚拟机disk实例：

```yaml
cat <<EOF | kubectl apply -f -
apiVersion: kubevirt.io/v1
kind: VirtualMachineInstance
metadata:
  name: cirros-vm
spec:
  domain:
    devices:
      disks:
      - disk:
          bus: virtio
        name: dvdisk
    machine:
      type: ""
    resources:
      requests:
        memory: 64M
  terminationGracePeriodSeconds: 0
  volumes:
  - name: dvdisk
    dataVolume:
      name: cirros-vm-disk
status: {}
EOF
```

### DataVolume cloning

参考：https://github.com/kubevirt/containerized-data-importer/blob/main/doc/efficient-cloning.md

> Data Volumes cloning a PVC source support multiple forms of cloning. Based on the prerequisites listed [here](https://github.com/kubevirt/containerized-data-importer/blob/main/doc/efficient-cloning.md#Prerequisites), a cloning method will be utilized with varying degrees of efficiency. These forms are [CSI volume cloning](https://github.com/kubevirt/containerized-data-importer/blob/main/doc/csi-cloning.md), [smart cloning](https://github.com/kubevirt/containerized-data-importer/blob/main/doc/smart-clone.md), and [host-assisted cloning](https://github.com/kubevirt/containerized-data-importer/blob/main/doc/clone-datavolume.md). CSI volume cloning and smart cloning employ CSI driver features in order to more efficiently clone a source PVC, but have certain limitations, while host-assited cloning provides a slower means with little limitation.

1. CSI Volume Cloning：**The csi driver backing the storage class of the PVC supports volume cloning**, and corresponding StorageProfile has the cloneStrategy set to CSI Volume Cloning (see [here](https://github.com/kubevirt/containerized-data-importer/blob/main/doc/csi-cloning.md#Prerequisites) for more details)
2. Smart Cloning：A Snapshot Class associated with the Storage Class exists，see [here](https://github.com/kubevirt/containerized-data-importer/blob/main/doc/clone-from-volumesnapshot-source.md) for more details.
3. Host-Assisted Cloning：Whenever the above cloning prerequisites are not met, we fallback to host-assisted cloning (`cloneType: copy`), which is the least efficient method of cloning. **It uses a source pod and a target pod to copy data from the source volume to the target volume.**  see [here](https://github.com/kubevirt/containerized-data-importer/blob/main/doc/clone-datavolume.md) for more details.

#### Host-Assisted Cloning

前置条件：

- You have a Kubernetes cluster up and running with CDI installed, source DV/PVC, and at least one available PersistentVolume to store the cloned disk image.
- The target PV is equal or larger in size than the source DV/PVC.
- When cloning from block to file system, content type must be kubevirt in both source and target, and host-assisted clone is used.
- When cloning across namespaces, the user must have the ability to create pods or have 'datavolumes/source' permission in the source namespace. You can give a user the appropriate permissions to a namespace by specifying [RBAC](https://github.com/kubevirt/containerized-data-importer/blob/main/doc/RBAC.md) rules.

Host-Assisted Cloning demo:

```yaml
apiVersion: cdi.kubevirt.io/v1beta1
kind: DataVolume
metadata:
  name: cloned-datavolume
spec:
  source:
    pvc:
      namespace: source-ns
      name: source-datavolume
  storage: {}
```



### Storage Profiles

https://github.com/kubevirt/containerized-data-importer/blob/main/doc/storageprofile.md

```yaml
apiVersion: cdi.kubevirt.io/v1beta1
kind: StorageProfile
metadata: 
  name: hostpath-provisioner
spec:
  claimPropertySets: 
  - accessModes:
    - ReadWriteOnce
    volumeMode: 
      Filesystem
  cloneStrategy: snapshot # 默认值，还支持 copy 和 csi-clone
status:
    storageClass: hostpath-provisioner
    provisioner: kubevirt.io/hostpath-provisioner
    claimPropertySets: 
    - accessModes: 
      - ReadWriteOnce
      volumeMode: Filesystem
    cloneStrategy: snapshot
```

## Clone - Velero Plugin

> GitHub仓库：https://github.com/kubevirt/kubevirt-velero-plugin。

这个插件注册了几个用于备份和恢复的actions用于操作DataVolume, PersistentVolumeClaim, Pod, VirtualMachine, VirtualMachineInstance这些资源。其中**VMBackupItemAction**会备份虚拟机包含的所有资源。对于备份虚拟机的 PVC/DataVolume，Kubevirt Velero Plugin底层还是用到了Kubevir CDI。
使用Velero plugin对虚拟机执行 Backup/Restore做备份恢复的流程如下：
1. 创建虚拟机：

   ```yaml
   kubectl create namespace demo
   kubectl create -f example/datavolume.yaml -n demo
   kubectl create -f example/vm.yaml -n demo
   ```

2. 创建虚拟机备份

   ```bash
   ./velero backup create demobackup1 --include-namespaces demo --wait
   ```

3. 恢复备份副本

   ```bash
   ./velero restore create --from-backup demobackup1 --wait
   ```

潜在问题：Kubevirt velero plugin可能无法将虚拟机备份恢复成一个全新的虚拟机，即只支持原位恢复。这和克隆的场景需求不太一样，暂不做跟更多调研。