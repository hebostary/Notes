# 典型问题
## 调度相关

### not have enough free storage

在某个集群创建应用并挂载`openebs-zfs` csi存储类的pvc（dynamic binding）时，发现pod和pvc总是处于pending状态，pod因为存储空间不足调度失败：

```text
0/1 nodes are available: 1 node(s) did not have enough free storage. preemption: 0/1 nodes are available: 1 Preemption is not helpful for scheduling.
```

但是，检查节点上的zfs存储空间是很富裕的。检查`kube-scheduler`的日志如下（`v=4`）：

```bash
I0104 15:01:32.732254    1160 binder.go:1043] "Node has no accessible CSIStorageCapacity with enough capacity for PVC" node="master" PVC="default/windows-sys-disk" size=26843545600 storageClass="zfs"
I0104 15:01:32.732303    1160 preemption.go:223] "Preemption will not help schedule pod on any node" pod="default/virt-launcher-windows-8mwnr"
I0104 15:01:32.732365    1160 schedule_one.go:987] "Unable to schedule pod; no fit; waiting" pod="default/virt-launcher-windows-8mwnr" err="0/1 nodes are available: 1 node(s) did not have enough free storage. preemption: 0/1 nodes are available: 1 Preemption is not helpful for scheduling."
```

参考前面对kube-scheduler volumebinding plugin的分析，zfs存储类的provisioner为`zfs.csi.openebs.io`，相应的`csidriver`如下所示，因为其`STORAGECAPACITY`属性为true，所以需要基于关联的`CSIStorageCapacity`对象中的 capacity 和 nodeTopology 信息来检查csi容量是否能满足申请的资源量。

```bash
[root@master ~]# kubectl get csidriver zfs.csi.openebs.io
NAME                 ATTACHREQUIRED   PODINFOONMOUNT   STORAGECAPACITY   TOKENREQUESTS   REQUIRESREPUBLISH   MODES        AGE
zfs.csi.openebs.io   false            false            true              <unset>         false               Persistent   3d22h
```

用`kubectl get csistoragecapacities -A`检查发现集群中没有csidriver相关的`CSIStorageCapacity`对象，怀疑是csi控制器出问题了。

如下，在csi控制器的`csi-provisioner`容器可看到如下日志，相应的`csisc-sg7qj`被创建后，之前创建的过期的`csisc-sg7qj`立马也被删除了，重启整个zfs controller可以短暂的解决问题，但是很快问题又会出现，`CSIStorageCapacity`对象总是被删掉。

```bash
$ kubectl logs -n openebs openebs-zfs-controller-54d5dc654-mfkfx -c csi-provisioner
...
I0104 02:43:25.184005       1 capacity.go:339] Capacity Controller: topology changed: 
// ...
I0104 02:43:25.184112       1 capacity.go:518] Capacity Controller: enqueuing {segment:0xc0001ae3d8 storageClassName:zfs} for periodic update
I0104 02:43:25.184138       1 capacity.go:731] Capacity Controller: CSIStorageCapacity csisc-sg7qj with resource version 1203219 is obsolete, enqueue for remova
I0104 02:43:25.184173       1 connection.go:193] GRPC call: /csi.v1.Controller/GetCapacity
I0104 02:43:25.208100       1 connection.go:200] GRPC response: {"available_capacity":364798906368}
I0104 02:43:25.208112       1 connection.go:201] GRPC error: <nil>
I0104 02:43:25.208153       1 capacity.go:643] Capacity Controller: creating new object for {segment:0xc0001ae3d8 storageClassName:zfs}, new capacity 356248932Ki
I0104 02:43:25.404884       1 capacity.go:648] Capacity Controller: created csisc-zm5bl with resource version 1486284 for {segment:0xc0001ae3d8 storageClassName:zfs} with capacity 356248932Ki
I0104 02:43:25.405027       1 capacity.go:682] Capacity Controller: removing CSIStorageCapacity csisc-sg7qj
```

基于对capacity controller的源码分析，怀疑是topology出现了问题，导致`CSIStorageCapacity`对象总是被删除，如下检查了csinode的topologyKey果然发现了一个异常的标签：

```bash
[root@master ~]# kubectl get node master -o jsonpath='{.metadata.labels}' > /tmp/node.labels
[root@master ~]# cat check.sh 
#!/bin/bash

l=$1
grep $l /tmp/node.labels > /dev/null || echo "$l does not exists"
[root@master ~]# kubectl get csinode master -o yaml | grep "    - " | awk -F "- " '{print $2}' | xargs -i bash check.sh {}
scheduling.node.kubevirt.io/tsc-frequency-2592008000 does not exists
```

`scheduling.node.kubevirt.io/tsc-frequency`标签是kubevirt项目用于控制虚拟机时钟频率的，由virt-controller组件负责动态更新，单位是赫兹，后面的2592008000代表了2.59G HZ。在这个问题里，该标签在master节点的标签列表里不存在，master拥有类似的标签`scheduling.node.kubevirt.io/tsc-frequency-2591997000`。即，存在不匹配的topologyKey，这会导致缓存中没有相关的topology，自然`CSIStorageCapacity`对象也无法正常创建或者一直被删除。作为workaround，删除csi node service pod重启后会自动更新csinode信息，zfs csi恢复正常工作。

但是，csinode的topologyKey里包含了很多kubevirt项目添加的标签，而这些标签显然和csi没有什么关系，不应该被加到topologyKey里。csinode的topologyKey来源于csi node service的rpc接口`/csi.v1.Node/NodeGetInfo`，参考zfs-localpv/pkg/driver/agent.go -> NodeGetInfo()，这个接口查询集群中节点的标签，如果环境变量里的`ALLOWED_TOPOLOGIES==all`，则将所有节点标签作为topology，否则只使用指定的标签。

作为优化，在zfs node service的配置里通过`ALLOWED_TOPOLOGIES`环境变量指定zfs相关的标签，问题得到解决。

### local PVC 的节点亲和性

注意，如果使用OpenEBS localpath 这类 CSI 创建 local pvc 然后挂载给应用，在重启pod后，新的pod仍然可能会被调度到其它节点，这就导致之前写入 pvc 的数据无法被访问到。这是一个陷阱，使用时最好使用节点亲和性明确数据所在的节点：

```yaml
apiVersion: v1
kind: PersistentVolume
metadata:
  name: registry-data
  namespace: default
  annotations:
    pv.kubernetes.io/provisioned-by: openebs.io/local
  finalizers:
  - kubernetes.io/pv-protection
  labels:
    openebs.io/cas-type: local-hostpath
spec:
  accessModes:
  - ReadWriteOnce
  capacity:
    storage: 500Gi
  claimRef:
    apiVersion: v1
    kind: PersistentVolumeClaim
    name: registry-data
    namespace: default
  local:
    path: {{registryDataPathTemplate}}
  nodeAffinity:
    required:
      nodeSelectorTerms:
      - matchExpressions:
        - key: kubernetes.io/hostname
          operator: In
          values:
          - {{registryHostTemplate}}
  persistentVolumeReclaimPolicy: Delete
  storageClassName: openebs-hostpath
  volumeMode: Filesystem
```