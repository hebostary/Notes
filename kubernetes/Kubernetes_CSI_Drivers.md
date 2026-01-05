# GlusterFS CSI Driver
## Kadalu vs Heketi

|      | Kadalu                                                              | Heketi                                   |
| ---- | ------------------------------------------------------------------- | ---------------------------------------- |
| 工作模式 | 完全基于k8s CSI开发，并减少了不必要的中间层，支持在k8s集群内部署glusterfs集群，也可以接入外部glusterfs集群 | 通过在GlusterFS之上提供一层rest api管理面然后接入k8s CSI |
| 开发语言 | Python                                                              | Go                                       |
| 状态   | 活跃中                                                                 | 深度维护中，只修一些bug，k8s社区建议从该项目迁移              |
| 维护人员 | GlusterFS团队成员                                                       | N/A                                      |

## 部署Kadalu Operator

下载源码后执行`kubectl apply -f kadalu/manifests/kadalu-operator.yaml`即可部署。
## 创建KadaluStorage CRD

我们可以通过以下存储资源创建KadaluStorage CRD（参考`kadalu/doc/storage-config-options.adoc`）：

1. 集群内节点上的硬盘/分区（Storage from device）；
2. 主机上的某个路径（Storage from path）；
3. PVC（Storage from another PVC），PVC必须在kadalu namespace可用；
4. 外部GlusterFS（External mode, type=`External`），用于接入已经部署好的Glusterfs（Glusterfs服务器可以在k8s集群内部或者外部）。

对于前面3种，我们都不需要部署`glusterd`服务，kadalu控制器自己会启动kadalu server来提供glusterfs服务。

### 设备作为存储

参考`kadalu/doc/storage-classes.adoc`，先准备好专用的硬盘或者分区，使用`wipefs -a -f /dev/sdb4`将硬盘/分区擦除干净，然后使用`mkfs.ext4 /dev/sdb4`将其格式化为ext4文件系统，然后如下创建`KadaluStorage`资源：

> Native mode is available with 5 `type` options. They are `Replica1`, `Replica2`, `Replica3`, `Disperse` and `Arbiter`. Replica1不提供任何副本能力。

```bash
root@master:~/hebo/kadalu# cat storage-config.yaml
apiVersion: kadalu-operator.storage/v1alpha1
kind: KadaluStorage
metadata:
  # This will be used as name of PV Hosting Volume
  name: storage-pool-1
spec:
  type: Replica1
  storage:
    - node: master
      device: /dev/sdb4
root@master:~/hebo/kadalu# kubectl apply -f storage-config.yaml      
root@master:~/hebo/kadalu# kubectl get pod -n kadalu
NAME                          READY   STATUS    RESTARTS      AGE
kadalu-csi-nodeplugin-85c7m   3/3     Running   0             174m
kadalu-csi-provisioner-0      5/5     Running   6 (24m ago)   174m
operator-6fd76b4897-7cvz5     1/1     Running   0             175m
server-storage-pool-1-0-0     1/1     Running   0             24m

# 创建KadaluStorage资源后，kadalu控制器会启动一个server pod，里面主要运行了glusterd进程用于提供存储服务
root@master:~/hebo/kadalu# ps -ef |grep "/kadalu/server.py"
root     1828380 1828359  0 06:22 ?        00:00:00 python3 /kadalu/server.py
root@master:~/hebo/kadalu# pstree -alTp 1828380
python3,1828380 /kadalu/server.py
  ├─glusterfsd,1828425 -N --volfile-id storage-pool-1.master.bricks-storage-pool-1-data-brick -p /var/run/gluster/glusterfsd-bricks-storage-pool-1-data-brick.pid -S /var/run/gluster/brick.socket --brick-name /bricks/storage-pool-1/data/brick -l - --xlator-option *-posix.glusterd-uuid=node-0 --process-name brick --brick-port 24007 --xlator-option storage-pool-1-server.listen-port=24007 -f /var/lib/kadalu/volfiles/storage-pool-1.master.bricks-storage-pool-1-data-brick.vol
  ├─python3,1828423 /kadalu/exporter.py
  └─python3,1828424 /kadalu/watch_volfile_changes.py
```

### 主机路径作为存储

```yaml
root@master:~/hebo/kadalu# cat storage-config-path.yaml
apiVersion: kadalu-operator.storage/v1alpha1
kind: KadaluStorage
metadata:
  # This will be used as name of PV Hosting Volume
  name: storage-pool-2
spec:
  type: Replica1
  storage:
    - node: master
      path: /var/lib/kadalu
```

### 接入外部GlusterFS

> == Using external gluster in kadalu native way
>
> In this mode, we expect gluster volume to be created and is in 'Started' state.
> kadalu storage config takes one of the node IP/hostname, and gluster volume name
> to use it as the storage for PVs. The PVs would be provided as subdirectories -
> this is similar to how a PV is created in kadalu native way.

#### 部署glusterfs集群

以Ubuntu系统为例，准备3个节点：

1. glusternode-100.1  - 主节点
2. glusternode-25.109 - perr节点
3. glusternode-25.110 - perr节点

在每个节点上配置域名解析，然后执行如下命令安装glusterfs服务器：

```bash
root@master:~/hebo/kadalu# cat /etc/hosts |grep glusternode
192.168.100.1 glusternode-100.1
192.168.25.109 glusternode-25.109
192.168.25.110 glusternode-25.110

root@master:~/hebo# apt install glusterfs-server
root@master:~/hebo# systemctl enable glusterd
root@master:~/hebo# systemctl start glusterd

root@master:~/hebo# systemctl status glusterd
● glusterd.service - GlusterFS, a clustered file-system server
     Loaded: loaded (/lib/systemd/system/glusterd.service; enabled; vendor preset: enabled)
     Active: active (running) since Tue 2024-12-10 01:57:10 UTC; 2 days ago
       Docs: man:glusterd(8)
    Process: 2134720 ExecStart=/usr/sbin/glusterd -p /var/run/glusterd.pid --log-level $LOG_LEVEL $GLUSTERD_OPTIONS (code=exited, status=0/SUCCESS)
   Main PID: 2134721 (glusterd)
      Tasks: 9 (limit: 125532)
     Memory: 8.6M
     CGroup: /system.slice/glusterd.service
             └─2134721 /usr/sbin/glusterd -p /var/run/glusterd.pid --log-level INFO

Warning: journal has been rotated since unit was started, output may be incomplete.

# 默认的glusterfs服务器存储路径是/var/lib/glusterd/
root@master:~/hebo# ls /var/lib/glusterd/
bitd  geo-replication  glusterd.upgrade  glustershd  groups  hooks  nfs  options  peers  quotad  scrub  snaps  ss_brick  vols
```

在每个节点上创建目录：

```bash
root@master:~/hebo# mkdir -p /data/brick1
```

在主节点上探测另外两个peer节点：

```bash
root@master:~/hebo/kadalu# gluster peer probe glusternode-25.109
peer probe: success. 
root@master:~/hebo/kadalu# gluster peer probe glusternode-25.110
peer probe: success. 
root@master:~/hebo/kadalu# gluster peer status
Number of Peers: 2

Hostname: glusternode-25.109
Uuid: 81951017-3852-4020-a26c-c345d81f4d31
State: Peer in Cluster (Connected)

Hostname: glusternode-25.110
Uuid: 03739f0c-b102-424c-85cc-dbc9353351ad
State: Peer in Cluster (Connected)
```

创建并启动glusterfs volume：

```bash
# 因为/data/brick1源自根文件系统，所以需要加上 force 参数
root@master:~/hebo/kadalu# gluster volume create glusterfs_volume1 replica 2 arbiter 1 transport tcp glusternode-100.1:/data/brick1 glusternode-25.109:/data/brick1 glusternode-25.110:/data/brick1 force
volume create: glusterfs_volume1: success: please start the volume to access data

root@master:~/hebo/kadalu# gluster volume start glusterfs_volume1
volume start: glusterfs_volume1: success

# 检查volume状态是Started
root@master:~/hebo/kadalu# gluster volume info glusterfs_volume1
 
Volume Name: glusterfs_volume1
Type: Replicate
Volume ID: d1f3a4a5-3efc-41d8-81ce-542144317523
Status: Started
Snapshot Count: 0
Number of Bricks: 1 x (2 + 1) = 3
Transport-type: tcp
Bricks:
Brick1: glusternode-100.1:/data/brick1
Brick2: glusternode-25.109:/data/brick1
Brick3: glusternode-25.110:/data/brick1 (arbiter)
Options Reconfigured:
transport.address-family: inet
storage.fips-mode-rchecksum: on
nfs.disable: on
performance.client-io-threads: off

# 检查volume的所有Brick以及self-heal进程都是online状态
root@master:~/hebo# gluster volume status glusterfs_volume1
Status of volume: glusterfs_volume1
Gluster process                             TCP Port  RDMA Port  Online  Pid
------------------------------------------------------------------------------
Brick glusternode-100.1:/data/brick1        49152     0          Y       2273428
Brick glusternode-25.109:/data/brick1       49152     0          Y       126807
Brick glusternode-25.110:/data/brick1       49152     0          Y       127243
Self-heal Daemon on localhost               N/A       N/A        Y       2273449
Self-heal Daemon on glusternode-25.110      N/A       N/A        Y       127264
Self-heal Daemon on glusternode-25.109      N/A       N/A        Y       126828
 
Task Status of Volume glusterfs_volume1
------------------------------------------------------------------------------
There are no active volume tasks
```

创建External KadaluStorage（`kadalu/doc/external-gluster-storage.adoc`）：

```yaml
root@master:~/hebo/kadalu# cat storage-config-external.yaml
apiVersion: kadalu-operator.storage/v1alpha1
kind: KadaluStorage
metadata:
  name: storage-pool-3
spec:
  type: External
  # Omitting 'single_pv_per_pool' or using 'false' as a value will create
  # 1 PV : 1 Subdir in external gluster volume
  single_pv_per_pool: false
  details:
    # gluster_hosts: [ gluster1.kadalu.io, gluster2.kadalu.io ]
    gluster_host: glusternode-100.1 # 单个host只能用这种写法
    gluster_volname: glusterfs_volume1
    gluster_options: log-level=DEBUG
root@master:~/hebo/kadalu# kubectl apply -f storage-config-external.yaml
kadalustorage.kadalu-operator.storage/storage-pool-3 created
```

也可以支持多个host：

```bash
root@master:~/hebo/kadalu# kubectl get kadalustorage storage-pool-3 -o yaml
apiVersion: kadalu-operator.storage/v1alpha1
kind: KadaluStorage
metadata:
  annotations:
    kubectl.kubernetes.io/last-applied-configuration: |
      {"apiVersion":"kadalu-operator.storage/v1alpha1","kind":"KadaluStorage","metadata":{"annotations":{},"name":"storage-pool-3","namespace":"default"},"spec":{"details":{"gluster_hosts":["192.168.100.1","192.168.25.109","192.168.25.110"],"gluster_options":"log-level=INFO","gluster_volname":"glusterfs_volume1"},"single_pv_per_pool":false,"type":"External"}}
  creationTimestamp: "2024-12-12T12:51:57Z"
  generation: 3
  name: storage-pool-3
  namespace: default
  resourceVersion: "12976909"
  uid: ef034f3f-ff6b-40ed-b897-bfab4fc43fb0
spec:
  details:
    gluster_hosts:
    - 192.168.100.1
    - 192.168.25.109
    - 192.168.25.110
    gluster_options: log-level=INFO
    gluster_volname: glusterfs_volume1
  pvReclaimPolicy: delete
  single_pv_per_pool: false
  type: External
root@master:~/hebo/kadalu# cat storage-config-external.yaml
apiVersion: kadalu-operator.storage/v1alpha1
kind: KadaluStorage
metadata:
  name: storage-pool-3
spec:
  type: External
  # Omitting 'single_pv_per_pool' or using 'false' as a value will create
  # 1 PV : 1 Subdir in external gluster volume
  single_pv_per_pool: false
  details:
    gluster_hosts: [ 192.168.100.1, 192.168.25.109, 192.168.25.110 ]
    # gluster_host: 192.168.100.1
    gluster_volname: glusterfs_volume1
    gluster_options: log-level=INFO
```

#### Name resolution failure

如上创建`KadaluStorage`并没有看到相应的存储类被创建出来，在operator opd里看到如下日志：

```text
[2024-12-12 11:24:24,876] ERROR [kadalulib - 118:is_host_reachable] - Failed to open socket connection   error=[Errno -3] Temporary failure in name resolution host=glusternode-100.1
Traceback (most recent call last):
  File "/kadalu/main.py", line 1089, in <module>
    main()
  File "/kadalu/main.py", line 1078, in main
    crd_watch(core_v1_client, k8s_client)
  File "/kadalu/main.py", line 920, in crd_watch
    watch_stream(core_v1_client, k8s_client)
  File "/kadalu/main.py", line 888, in watch_stream
    handle_added(core_v1_client, item)
  File "/kadalu/main.py", line 540, in handle_added
    if not validate_volume_request(obj):
  File "/kadalu/main.py", line 168, in validate_volume_request
    return validate_ext_details(obj)
  File "/kadalu/main.py", line 128, in validate_ext_details
    if not is_host_reachable(ghosts, gport):
  File "/kadalu/kadalulib.py", line 122, in is_host_reachable
    sock.close()
UnboundLocalError: local variable 'sock' referenced before assignment
```

看了下代码是建立到`glusternode-100.1:24007`端口的sock因为解析`glusternode-100.1`失败了，临时换成ip地址后OK，可能是Python代码的bug，后面再查具体原因。

#### Transport endpoint is not connected

基于外部接入的glusterfs创建的存储类创建PVC并挂载给应用，但是PV创建无法成功，`kadalu-csi-provisioner` pod内`kadalu-provisioner`容器的日志如下（`kubectl logs -n kadalu kadalu-csi-provisioner-0  -c kadalu-provisioner -f`）：

```txt
[2024-12-12 11:59:12,442] ERROR [_server - 508:_call_behavior] - Exception calling application: [Errno 107] Transport endpoint is not connected
Traceback (most recent call last):
  File "/kadalu/lib/python3.10/site-packages/grpc/_server.py", line 494, in _call_behavior
    response_or_iterator = behavior(argument, context)
  File "/kadalu/controllerserver.py", line 120, in CreateVolume
    volume = search_volume(request.name)
  File "/kadalu/volumeutils.py", line 782, in search_volume
    retry_errors(os.statvfs, [mntdir], [ENOTCONN])
  File "/kadalu/kadalulib.py", line 52, in retry_errors
    raise TimeoutOSError(err.errno, err.strerror) from None
kadalulib.TimeoutOSError: [Errno 107] Transport endpoint is not connected
```

进入容器后发现`storage-pool-3`挂载有问题：

```bash
root@kadalu-csi-provisioner-0:/kadalu# ls -al /mnt/
ls: cannot access '/mnt/storage-pool-3': Transport endpoint is not connected
total 16
drwxr-xr-x 1 root root 4096 Dec 12 11:38 .
drwxr-xr-x 1 root root 4096 Dec 12 03:51 ..
drwxr-xr-x 5 root root 4096 Dec 12 06:22 storage-pool-1
drwxr-xr-x 5 root root 4096 Dec 12 08:43 storage-pool-2
d????????? ? ?    ?       ?            ? storage-pool-3
```

```bash
root@kadalu-csi-provisioner-0:/# tail -f /var/log/gluster/gluster.log
....
[2024-12-12 12:59:36.346245 +0000] D [logging.c:1831:_gf_msg_internal] 0-logging-infra: Buffer overflow of a buffer whose size limit is 5. About to flush least recently used log message to disk
[2024-12-12 12:59:36.285873 +0000] D [MSGID: 0] [md-cache.c:1269:mdc_lookup_cbk] 0-stack-trace: stack-address: 0x7fad00000dc8, glusterfs_volume1-md-cache returned -1 [Transport endpoint is not connected]
[2024-12-12 12:59:36.346242 +0000] E [MSGID: 101073] [name.c:254:gf_resolve_ip6] 0-resolver: error in getaddrinfo [{family=2}, {ret=Temporary failure in name resolution}] 
[2024-12-12 12:59:36.346315 +0000] E [name.c:383:af_inet_client_get_remote_sockaddr] 0-glusterfs_volume1-client-1: DNS resolution failed on host glusternode-25.109
```

看起来还是`glusternode-25.109` host解析的问题，手动写入解析条目后，`/mnt/storage-pool-3`恢复正常：

```bash
root@kadalu-csi-provisioner-0:/# echo "192.168.100.1 glusternode-100.1" >> /etc/hosts
root@kadalu-csi-provisioner-0:/# echo "192.168.25.109 glusternode-25.109" >> /etc/hosts
root@kadalu-csi-provisioner-0:/# echo "192.168.25.110 glusternode-25.110" >> /etc/hosts

root@kadalu-csi-provisioner-0:/# ls -al /mnt/
total 20
drwxr-xr-x 1 root root 4096 Dec 12 12:54 .
drwxr-xr-x 1 root root 4096 Dec 12 12:54 ..
drwxr-xr-x 5 root root 4096 Dec 12 06:22 storage-pool-1
drwxr-xr-x 5 root root 4096 Dec 12 08:43 storage-pool-2
drwxr-xr-x 3 root root 4096 Dec 12 13:04 storage-pool-3

root@kadalu-csi-provisioner-0:/# ls -al /mnt/storage-pool-3/
total 28
drwxr-xr-x 3 root root  4096 Dec 13 01:12 .
drwxr-xr-x 1 root root  4096 Dec 12 12:54 ..
-rw-r--r-- 1 root root 20480 Dec 13 01:12 stat.db
root@kadalu-csi-provisioner-0:/# echo "hello" > /mnt/storage-pool-3/test.txt
root@kadalu-csi-provisioner-0:/# cat /mnt/storage-pool-3/test.txt
```

#### PV Provision failure

上面的问题规避后，遇到如下问题：

```bash
[2024-12-13 02:02:34,530] ERROR [_server - 508:_call_behavior] - Exception calling application: 'gluster_volname'
Traceback (most recent call last):
  File "/kadalu/lib/python3.10/site-packages/grpc/_server.py", line 494, in _call_behavior
    response_or_iterator = behavior(argument, context)
  File "/kadalu/controllerserver.py", line 213, in CreateVolume
    ext_volume = check_external_volume(request, host_volumes)
  File "/kadalu/volumeutils.py", line 1148, in check_external_volume
    and vol["g_volname"] == params["gluster_volname"]
KeyError: 'gluster_volname'
```

怀疑是删除默认的存储类重新创建存储类导致，后面创建相同的glusterfs volume2的时候，直接创建`KadaluStorage`，不重新创建存储类，没有再遇到这个问题。



#### 删除glusterfs volume

```bash
root@master:~/hebo/kadalu# gluster volume stop glusterfs_volume1
Stopping volume will make its data inaccessible. Do you want to continue? (y/n) y
volume stop: glusterfs_volume1: success
root@master:~/hebo/kadalu# gluster volume delete glusterfs_volume1
Deleting volume will erase all information about the volume. Do you want to continue? (y/n) y
volume delete: glusterfs_volume1: success
```

## 创建StorageClass

创建`KadaluStorage`后，Kadalu会自动创建如下默认的存储类`kadalu.storage-pool-1`，默认的volumeBindingMode是`Immediate`，我们也可以将其删除后自行创建：

> 警告：对于接入外部GlusterFS集群，不能去删除默认存储类，所以对应的存储类绑定模式只能是`Immediate`。

```bash
root@master:~/hebo/kadalu# kubectl get sc
NAME                         PROVISIONER            RECLAIMPOLICY   VOLUMEBINDINGMODE      ALLOWVOLUMEEXPANSION   AGE
kadalu.storage-pool-1        kadalu                 Delete          Immediate              true                   2m51s

root@master:~/hebo/kadalu# kubectl delete sc kadalu.storage-pool-1
storageclass.storage.k8s.io "kadalu.storage-pool-1" deleted

root@master:~/hebo/kadalu# cat glusterfs-sc.yaml
kind: StorageClass
apiVersion: storage.k8s.io/v1
metadata:
  name: glusterfs
provisioner: kadalu
reclaimPolicy: Delete
volumeBindingMode: WaitForFirstConsumer
parameters:
  storage_name: "storage-pool-1"
root@master:~/hebo/kadalu# kubectl apply -f glusterfs-sc.yaml
storageclass.storage.k8s.io/glusterfs created

```

## 创建PVC

指定前面创建的存储类创建PVC即可：

```bash
root@master:~/hebo/kadalu# cat glusterfs-pvc.yaml 
---
kind: PersistentVolumeClaim
apiVersion: v1
metadata:
  name: glusterfs-pvc1
spec:
  storageClassName: glusterfs
  accessModes:
    - ReadWriteOnce
  resources:
    requests:
      storage: 1Gi
root@master:~/hebo/kadalu# kubectl apply -f glusterfs-pvc.yaml
persistentvolumeclaim/glusterfs-pvc1 created
```

## 检查Glusterfs brick

创建应用并挂载这个glusterfs PVC后，写入一些测试数据，然后将主机上的硬盘/分区直接挂载到某个目录，可以看到里面的glusterfs的drick目录经创建出来了：

```bash
root@master:~/hebo/kadalu# mount /dev/sdb4 /mnt/sdb4/
root@master:~/hebo/kadalu# ls /mnt/sdb4/brick/
.glusterfs/ info/       stat.db     subvol/ 
root@master:~/hebo/kadalu# umount /mnt/sdb4
```

如果是Storage from path，目录结构如下：

```bash
root@master:~/hebo/kadalu# ls -al /var/lib/kadalu/data/brick/
total 56
drwxr-xr-x   5 root root  4096 Dec 12 08:43 .
drwxr-xr-x   3 root root  4096 Dec 12 08:41 ..
drwxr-xr-x 262 root root  4096 Dec 12 08:41 .glusterfs
drwxr-xr-x   3 root root  4096 Dec 12 08:43 info
-rw-r--r--   2 root root 20480 Dec 12 08:43 stat.db
drwxr-xr-x   3 root root  4096 Dec 12 08:43 subvol
```
