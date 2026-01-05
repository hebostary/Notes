# CSI 架构设计
Kubenrtes CSI 的基本架构如下图所示：
![csi-diagram-1](https://github.com/kubernetes/design-proposals-archive/blob/main/storage/container-storage-interface_diagram1.png?raw=true)

> CSI官方设计文档：https://github.com/kubernetes/design-proposals-archive/blob/main/storage/container-storage-interface.md
> [kubernetes/k8s CSI分析-容器存储接口分析](https://www.cnblogs.com/lianngkyle/p/15055552.html) - 这篇博客分析得很全面

## CSI 相关组件

| **组件**                     | **职责**                   | **位置**                  |
| -------------------------- | ------------------------ | ----------------------- |
| **CSI Controller Server**  | 管理集群范围的存储卷生命周期，创建、删除、调度卷 | 控制平面（API server）        |
| **CSI Node Server(Agent)** | 处理节点级别的存储操作，挂载和卸载存储卷     | 每个节点上                   |
| **CSI Provisioner**        | 根据 PVC 请求创建新的存储卷         | Controller sidecar      |
| **CSI Attacher**           | 将卷挂载到正确的节点               | Controller sidecar      |
| **CSI Snapshotter**        | 管理卷的快照功能                 | Controller sidecar      |
| **CSI Resizer**            | 支持卷的扩展                   | Controller sidecar      |
| PV Controller              |                          | kube-controller-manager |
| Volume Manager             |                          | kubelet                 |

## CSI API资源

### StorageClass

```yaml
# kubectl get StorageClass zfspv-pool -o yaml
allowedTopologies:
- matchLabelExpressions:
  - key: kubernetes.io/hostname
    values:
    - master
apiVersion: storage.k8s.io/v1
kind: StorageClass
metadata:
  creationTimestamp: "2025-01-04T05:53:50Z"
  labels:
    storage-type: zfs
  name: zfspv-pool
  resourceVersion: "3639564"
  uid: 1e07debd-aeb6-488b-9b28-f953b7fb3f3f
parameters:
  compression: "off"
  dedup: "off"
  fsType: zfs
  poolName: zfspv-pool
provisioner: zfs.csi.openebs.io
reclaimPolicy: Delete
volumeBindingMode: WaitForFirstConsumer
```
### CsiDriver

```yaml
# kubectl get CsiDriver zfs.csi.openebs.io -o yaml
apiVersion: storage.k8s.io/v1
kind: CSIDriver
metadata:
  annotations:
    kubectl.kubernetes.io/last-applied-configuration: |
      {"apiVersion":"storage.k8s.io/v1","kind":"CSIDriver","metadata":{"annotations":{},"name":"zfs.csi.openebs.io"},"spec":{"attachRequired":false,"podInfoOnMount":false,"storageCapacity":true}}
  creationTimestamp: "2024-12-26T01:35:54Z"
  name: zfs.csi.openebs.io
  resourceVersion: "523"
  uid: 1e0884bb-9210-4feb-8457-0727b34fc867
spec:
  attachRequired: false
  fsGroupPolicy: ReadWriteOnceWithFSType
  podInfoOnMount: false
  requiresRepublish: false
  seLinuxMount: false
  storageCapacity: true
  volumeLifecycleModes:
  - Persistent
```

### CsiNode

1. CsiNode 是节点级别的资源，和集群里的 Node 资源是一对一关系（CsiNode.Name等于Node.Name），记录了节点的拓扑信息。
2. 用于标识某个节点上可用的csidriver，提供有关csidriver的能力、拓扑和附加特性的详细信息，帮助kubernetes执行存储调度决策，例如卷和节点的亲和性。
3. 由CSI Driver的Node Server和kubelet自动创建和更新。

```yaml
# kubectl get CsiNode master -o yaml
apiVersion: storage.k8s.io/v1
kind: CSINode
metadata:
  annotations:
    storage.alpha.kubernetes.io/migrated-plugins: kubernetes.io/aws-ebs,kubernetes.io/azure-disk,kubernetes.io/azure-file,kubernetes.io/cinder,kubernetes.io/gce-pd,kubernetes.io/vsphere-volume
  creationTimestamp: "2024-12-26T01:35:09Z"
  name: master
  ownerReferences:
  - apiVersion: v1
    kind: Node
    name: master
    uid: 74ec78a5-b335-43ca-8bfb-dad1600cb761
  resourceVersion: "1470"
  uid: 86a52b03-2b72-4adb-93e9-33eb9760ff3a
spec:
  drivers:
  - name: zfs.csi.openebs.io
    nodeID: master
    topologyKeys:
    - beta.kubernetes.io/arch
    - beta.kubernetes.io/os
    - kubernetes.io/arch
    - kubernetes.io/hostname
    - kubernetes.io/os
    - node-role.kubernetes.io/master
    - openebs.io/nodeid
    - openebs.io/nodename
```

删除CsiNode后，关联的CsiStorageCapacities也会自动被删除。重启csi Node Service pod，其中的`node-driver-registrar`会向kubelet注册CSI driver（kubelet设备插件机制），kubelet插件注册函数RegisterPlugin()会调用CSI driver的`/csi.v1.Node/NodeGetInfo` rpc接口获取节点信息，然后更新etcd里的CsiNode资源。

> 参考 kubernetes/pkg/volume/csi/csi_plugin.go

CsiNode资源对应于源码里的CSINodeInfo struct：

```go
// CSINodeInfo holds information about status of all CSI drivers installed on a node.
type CSINodeInfo struct {
    metav1.TypeMeta
    // ObjectMeta.Name must be node name.
    metav1.ObjectMeta
    // List of CSI drivers running on the node and their properties.
    CSIDrivers []CSIDriverInfo
}

// Information about one CSI driver installed on a node.
type CSIDriverInfo struct {
    // CSI driver name.
    Name string
    // ID of the node from the driver point of view.
    NodeID string
    // Topology keys reported by the driver on the node.
    TopologyKeys []string
}
```
### CsiStorageCapacities

```yaml
# kubectl get csistoragecapacities -n openebs     csisc-ps5x6 -o yaml
apiVersion: storage.k8s.io/v1
capacity: "445199919616"
kind: CSIStorageCapacity
metadata:
  creationTimestamp: "2025-01-04T05:53:50Z"
  generateName: csisc-
  labels:
    csi.storage.k8s.io/drivername: zfs.csi.openebs.io
    csi.storage.k8s.io/managed-by: external-provisioner
  name: csisc-ps5x6
  namespace: openebs
  ownerReferences:
  - apiVersion: apps/v1
    controller: true
    kind: ReplicaSet
    name: openebs-zfs-controller-54d5dc654
    uid: 853fd601-ff3a-4b64-8e91-1a79aab54aa9
  resourceVersion: "3644328"
  uid: 9821fe8a-2018-4b4f-b933-a67b07c82076
nodeTopology:
  matchLabels:
    beta.kubernetes.io/arch: amd64
    beta.kubernetes.io/os: linux
    kubernetes.io/arch: amd64
    kubernetes.io/hostname: master
    kubernetes.io/os: linux
    node-role.kubernetes.io/master: "true"
    openebs.io/nodeid: master
    openebs.io/nodename: master
storageClassName: zfspv-pool
```
### VolumeAttachment

## CSI 与调度

## PV/PVC 访问模式

Kubernetes 定义了三种 PV/PVC 访问模式，用于描述存储卷的挂载方式：
1. ReadWriteOnce（RWO） - 卷可被单个节点以读写模式挂载。同一时间只能被一个节点挂载为读写模式，即使在同一节点，不同 Pod 也不能同时挂载同一 RWO 卷，属于最严格的访问控制。典型存储类型： AWS EBS、Ceph RBD。适用于数据库、消息队列等要求强一致性的单实例应用。
2. ReadOnlyMany（ROX） - 卷可被多个节点同时挂载，但都是只读模式。典型存储类型：文件存储、对象存储。适用于高效分发静态数据，比如配置文件、CSS/JS/图片静态资源、机器学习模型文件、只读数据库副本。
3. ReadWriteMany（RWX） - 卷可被多个节点以读写模式挂载，需要存储系统支持并发控制和一致性。典型存储类型：网络文件系统，例如 NFS、CephFS。
### ReadWriteMany
对于 ReadWriteMany 模式，需要存储系统支持并发控制和一致性，不同存储后端的并发控制方式：
1. NFS v4+，依靠文件锁（fcntl）来做并发控制，提供强一致性（同一个客户端）。
2. CephFS，依靠元数据服务器（MDS）来做并发控制，提供强一致性。
3. GlusterFS，依靠分散锁来做并发控制，提供最终一致性。

# 核心组件源码分析
## CSI Provisioner 

> 源码仓库：https://github.com/kubernetes-csi/external-provisioner

CSI Provisioner 也叫 External Provisioner，是kubernetes sig-storage组提供的一个用于存储资源制备的公共组件，它也是以sidecar的形式和CSI Driver运行在同一个pod内。它负责监视集群内感兴趣的（`volume.kubernetes.io/storage-provisioner`注解）PVC资源的变更事件，然后为其创建匹配的PV资源用于绑定。用户创建 PVC 后，PV controller 监视到 新的 PVC，会先尝试寻找合适的 PV与之绑定，如果没有找到则会给 PVC 添加 `volume.kubernetes.io/storage-provisioner`注解。
CSI Provisioner 包含两个核心组件：负责容量管理的 Capacity controller 和 负责存储资源制备的 Provision controller。
### Capacity controller

Capacity controller是 CSI Provisioner 里的一个负责存储类容量管理的内置控制器，其作用是通过节点拓扑关系（CsiNode）和CSI可用存储容量来维护etcd里的CsiStorageCapabities对象，它存储了存储类名称、节点拓扑标签、容量信息。kube-scheduler在调度pod过程中，对于动态绑定pvc的pod，便会查询CsiStorageCapabities 对象来过滤掉那些存储空间不足的节点，如果查询不到CsiStorageCapabities会导致相关节点被过滤掉。

```go
type Controller struct {
    // ...
    topologyInformer topology.Informer // 缓存topology
	scInformer       storageinformersv1.StorageClassInformer
	cInformer        storageinformersv1.CSIStorageCapacityInformer
	
    pollPeriod       time.Duration // 默认每分钟周期性刷新容量信息
	immediateBinding bool
	timeout          time.Duration

    // CSIStorageCapacity对象缓存
	capacities     map[workItem]*storagev1.CSIStorageCapacity
	capacitiesLock sync.Mutex
}

type workItem struct {
	segment          *topology.Segment //CSIStorageCapacity.NodeTopology标签里的key-value数组
	storageClassName string
}
```

**capacity controller的工作机制分两个阶段**：

New阶段：

1. 创建topology informer，topology有两种模式：

   2. nodedeployment：TODO。

   3. NewNodeTopology() - 这里的NodeTopology并不是etcd里存储的资源类型，它是在external-provisioner capacity内部实现的一种缓存资源类型，也提供了List和AddCallBack接口。NodeTopology通过注册nodes和csinodes informer handler处理nodes和csinodes对象的变更事件，如果node.Labels或者csinode.Spec.Driver.TopologyKeys发生变化，通过在queue里放入空字符串`""`触发sync函数来更新segments缓存（数组）。sync函数的工作流程：

      1. 刚开始缓存里所有的segment都会被放到removalCandidates map里，并被标记成删除。

      2. List所有csinodes，遍历所有csinodes：

         1. 获取csinode.Spec.Driver.TopologyKeys并根据csiNode.Name查询对应的node信息。排序后遍历TopologyKeys里的所有key，用key去查询node.Labels并将key-value组装成新的segment（**如果任何key在node.Labels里不存在，就跳过当前csinode，直接处理下一个csinode，原来过期的segment就会被删除）**。如果新组装的segment在segments缓存里已经存在，就在removalCandidates map里标记不用删除。
         2. 新的segment经过去重检查后upsert到segments缓存里。

      3. 调用回调函数 onTopologyChanges(addedSegments, removedSegments()，关键日志：

         ```text
         I1226 01:38:17.361067       1 capacity.go:339] Capacity Controller: topology changed: added [0xc00088ed98 = beta.kubernetes.io/arch: amd64+ beta.kubernetes.io/os: linux+ kubernetes.io/arch: amd64+ kubernetes.io/hostname: master+ kubernetes.io/os: linux+ node-role.kubernetes.io/master: true+ openebs.io/nodeid: master+ openebs.io/nodename: master], removed []
         ```

4. NewCentralCapacityController() - 通过注册informer handler处理topology和storageclass资源的变更事件。这两种资源发生变更，都是通过在`Controller.queue`里删除或添加workItem来触发`Controller.capacities`的更新，storageclass更新会List所有的topology，topology更新会List所有的storageclas。

Run阶段（goroutine）：

1. Controller.prepare()：

   2. 等待所有informer同步完成。
   3. 构建`Controller.capacities`缓存的所有key：List所有storageclass和topology，将所有topology和关联的storageclass（provisioner==csidriver&&绑定模式 != VolumeBindingImmediate）组成workItem添加到`Controller.capacities`里。topology资源发生变化，回调函数 onTopologyChanges() 就会重新执行一次这个流程。
   4. 在CSIStorageCapacity informer上注册回调函数`onCAddOrUpdate`和`onCDelete`。
      1. `onCAddOrUpdate`函数负责添加/更新`Controller.capacities`里缓存的CSIStorageCapacity对象：如果在缓存里找到UID相同的CSIStorageCapacity对象，或者找到相同的workItem（topology和storageClassName相同），则直接对其进行更新；否则表示是过期的CSIStorageCapacity对象，将其放到`Controller.queue`用于触发删除。
      2. `onCDelete`函数负责删除`Controller.capacities`里缓存的相同UID的CSIStorageCapacity对象并触发重建，即将workItem放到`Controller.queue`里。
   5. List所有CSIStorageCapacity，依次调用`onCAddOrUpdate`。

6. 启动worker协程负责依次处理`Controller.queue`里面的任务，主要包含两种任务：

   7. workItem：**Controller.syncCapacity()** - 同步CSIStorageCapacity资源，流程如下：

      1. 查找Controller.capacities里是否有这个workItem，没有的话表示它已经过期（例如topology或者storageclass已经发生变化），不做任何处理。

      2. 查询workItem里的storageclass，结合topology信息封装请求调用csidriver的rpc接口去获取capacity信息。

         ```text
         I0107 05:38:28.515240       1 connection.go:193] GRPC call: /csi.v1.Controller/GetCapacity
         I0107 05:38:28.515250       1 connection.go:194] GRPC request: {"accessible_topology":{"segments":{"beta.kubernetes.io/arch":"amd64","beta.kubernetes.io/os":"linux","kubernetes.io/arch":"amd64","kubernetes.io/hostname":"master","kubernetes.io/os":"linux","node-role.kubernetes.io/master":"true","openebs.io/nodeid":"master","openebs.io/nodename":"master"}},"parameters":{"compression":"off","dedup":"off","fsType":"zfs","poolName":"zfspv-pool"},"volume_capabilities":[{"AccessType":{"Mount":null},"access_mode":{}}]}
         I0107 05:38:28.521692       1 connection.go:200] GRPC response: {"available_capacity":445199919616}
         ```

      3. 如果查询到capacity信息，则创建或者更新集群中的`CSIStorageCapacity`对象，均以`csisc-`开头。

   8. CSIStorageCapacity：删除etcd里的CSIStorageCapacity对象。

9. 启动一个单独的协程负责周期性地（每分钟）将`Controller.capacities`里面的所有key（即workItem）加入`Controller.queue`用于触发更新。

### Provision Controller

#### Immediate vs WaitForFirstConsumer

存储类的VolumeBindingMode参数可以设置为Immediate 或者 WaitForFirstConsumer，这两种模式的区别如下：

| **特性**           | **Immediate**                                                | **WaitForFirstConsumer**                                     |
| ------------------ | ------------------------------------------------------------ | ------------------------------------------------------------ |
| **绑定时机**       | 创建时立即尝试绑定到 PV。PV Controller如果没有合适的PV，CSI Provisioner也会立即创建相应的PV。 | Pod 调度到节点后，kube-scheduler给PVC添加volume.kubernetes.io/selected-node注解，CSI Provisioner才会去动态创建相应的PV，然后PVC 才可以绑定到 PV。 |
| **节点亲和性支持** | 无法保证卷绑定与 Pod 调度的节点一致。                        | 支持节点亲和性（例如本地存储）。                             |
| **适用场景**       | 适用于共享存储或无节点依赖的存储。                           | 适用于本地存储或对节点位置敏感的存储。                       |

#### Dynamic Provision

```go
// syncClaim checks if the claim should have a volume provisioned for it and
// provisions one if so. Returns an error if the claim is to be requeued.
func (ctrl *ProvisionController) syncClaim(ctx context.Context, obj interface{}) error {
	claim, ok := obj.(*v1.PersistentVolumeClaim)
	if !ok {
		return fmt.Errorf("expected claim but got %+v", obj)
	}
	should, err := ctrl.shouldProvision(ctx, claim)
	if should {
		status, err := ctrl.provisionClaimOperation(ctx, claim)
    }
```

Provision Controller监视并处理和当前Provisioner匹配的PVC对象资源，先判断是否执行Provision操作（即创建相应的PV）：

1. 检查PVC的注解`volume.kubernetes.io/storage-provisioner`或者`volume.beta.kubernetes.io/storage-provisioner`是否存在并等于当前CSI provisioner。
2. 检查PVC的注解`volume.kubernetes.io/selected-node`：
   3. 如果值等于当前节点，则执行Provision。
   4. 如果存储类的VolumeBindingMode是`Immediate`，即使注解不存在，也会尝试在当前节点来做Provision。？
   5. 如果存储类的VolumeBindingMode是`WaitForFirstConsumer`，则必须等这个注解被kube-scheduler设置值之后才能执行Provision。

Provisoin操作的具体流程（`pkg/controller/controller.go -> Provision()`）：

1. 生成PV名字：`pvc-<pvc UUID>`
2. 从注解`volume.kubernetes.io/selected-node`获取节点。
3. 调用CSI Driver Controller server的`CreateVolume` RPC方法创建相应的Volume。
4. 在集群中创建关联的PV资源。保存到etcd之前已经更新了PV的`.spec`和注解，包括`spec.claimRef`也已经被更新，所以不用担心被绑定到其它PVC。

PV资源被创建出来后，kube-controller-manager里的 PV Controller就可以将PVC和PV进行绑定了，主要是将它们的`status.phase`更新为`Bound`。

## CSI Attacher


## CSI Snapshotter

关于卷快照的基础知识参考https://kubernetes.io/zh-cn/docs/concepts/storage/volume-snapshots/。

## CSI Node Driver Registrar

> 源码仓库：https://github.com/kubernetes-csi/node-driver-registrar

node-driver-registrar是Kubernetes CSI提供的公共组件，一般是作为sidecar容器和CSI Driver负责节点级存储资源管控的Node Server运行在一个Daemonset pod内，它主要负责利用kubelet的pluginregistration机制将CSI Driver Node Server注册到kubelet上，注册的内容主要是CSI driver的名字和socket端点。它主要通过两个UDS(Unix-domain socket)来完成和kubelet & CSI driver的通信：

1. Registration socket: 主机上路径`/var/lib/kubelet/plugins_registry/<csiDriverName>-reg.sock`，容器内挂载路径：`/registration`，socket文件由node-driver-registrar负责创建，名字格式是固定的。
2. CSI driver socket:主机上路径`/var/lib/kubelet/plugins/<csiDriverName>/csi.sock`，容器内挂载路径：`/plugin/csi.sock`。

node-driver-registrar二进制的关键启动参数：

1. `--csi-address`：指向CSI driver socket容器内路径。
2. `--kubelet-registration-path`：指向CSI driver socket主机路径。这个路径参数主要是用于health check，即检查同目录下的名为`registration`的lock文件，这个文件存在表示node-driver-registrar注册成功，即kubelet已经调用过`GetInfo`方法。

node-driver-registrar启动的时候通过<--csi-address>去连接CSI Driver Node Server，先调用RPC方法获取CSI driver名字，然后调用`nodeRegister()`执行注册流程：

1. 创建Registration socket并进行监听。
2. 清理`/var/lib/kubelet/plugins/<csiDriverName>/registration`文件。
3. 创建gRPC服务器。
4. 调用kubelet的pluginregistration框架接口注册registrar gRPC服务（`k8s.io/kubelet/pkg/apis/pluginregistration/v1.RegistrationServer`），提供两个RPC方法：
	1. `GetInfo`：创建`/var/lib/kubelet/plugins/<csiDriverName>/registration`文件；返回插件信息：
      ```go
      	return &registerapi.PluginInfo{
      		Type:              registerapi.CSIPlugin, // 插件类型，CSIPlugin；还有DevicePlugin等
      		Name:              e.driverName,
      		Endpoint:          e.endpoint, // /var/lib/kubelet/plugins/<csiDriverName>/csi.sock
      		SupportedVersions: e.version, // 支持的版本
      	}
      ```
	   2. `NotifyRegistrationStatus`：kubelet调用这个方法通知node-driver-registrar注册结果。
5. 启动http服务器暴露健康检查接口（`/healthz`）和调试接口（`/debug/pprof/xxx`）- 可选的，参数控制。
6. 启动协程，在程序收到SIGTERM后负责移除Registration socket文件。
7. 启动gRPC服务器。
# CSI Driver 开发
根据前面的描述，针对特定存储开发 Kubernetes CSI 驱动时，需要实现 CSI Controller Server 和CSI Node Server，最终部署的 CSI Operator 也主要包含这两部分，然后包含各自所需的 CSI Sidecar 容器。下面以 OpenEBS-zfs 为例，其 operator 包含如下两个组件：
1. openebs-zfs-controller deployment：包含5个容器：csi-resizer, csi-snapshotter, snapshot-controller, csi-provisioner, openebs-zfs-plugin(controller server)。
2. openebs-zfs-node deamonset：包含2个容器：csi-node-driver-registrar, openebs-zfs-plugin(node server)
CSI Driver本身主要包含3个部分：Identity server、Controller server 和 Node Server：
- Identity server是公共组件，作为Controller server和Node server进程的一部分，和 Controller server 和 Node server监听同一个UDS；
- Controller server一般在集群中部署为单副本deployment，负责处理集群级别的CSI资源；它监听`/var/lib/csi/sockets/pluginproxy/csi.sock`提供RPC服务。
- Node Server作为daemonset部署在每个节点上，负责处理节点级的存储资源操作，监听在`/var/lib/kubelet/plugins/<csiDriverName>/csi.sock`提供RPC服务。
### Identity Server APIs

Identity server需要实现如下3个RPC服务接口：

```go
// IdentityServer is the server API for Identity service.
type IdentityServer interface {
	GetPluginInfo(context.Context, *GetPluginInfoRequest) (*GetPluginInfoResponse, error)
	GetPluginCapabilities(context.Context, *GetPluginCapabilitiesRequest) (*GetPluginCapabilitiesResponse, error)
	Probe(context.Context, *ProbeRequest) (*ProbeResponse, error)
}
```

### Controller Server APIs

Controller Server需要实现如下RPC服务接口：

```go
type ControllerServer interface {
	CreateVolume(context.Context, *CreateVolumeRequest) (*CreateVolumeResponse, error)
	DeleteVolume(context.Context, *DeleteVolumeRequest) (*DeleteVolumeResponse, error)
	ControllerPublishVolume(context.Context, *ControllerPublishVolumeRequest) (*ControllerPublishVolumeResponse, error)
	ControllerUnpublishVolume(context.Context, *ControllerUnpublishVolumeRequest) (*ControllerUnpublishVolumeResponse, error)
	ValidateVolumeCapabilities(context.Context, *ValidateVolumeCapabilitiesRequest) (*ValidateVolumeCapabilitiesResponse, error)
	ListVolumes(context.Context, *ListVolumesRequest) (*ListVolumesResponse, error)
	GetCapacity(context.Context, *GetCapacityRequest) (*GetCapacityResponse, error)
	ControllerGetCapabilities(context.Context, *ControllerGetCapabilitiesRequest) (*ControllerGetCapabilitiesResponse, error)
	CreateSnapshot(context.Context, *CreateSnapshotRequest) (*CreateSnapshotResponse, error)
	DeleteSnapshot(context.Context, *DeleteSnapshotRequest) (*DeleteSnapshotResponse, error)
	ListSnapshots(context.Context, *ListSnapshotsRequest) (*ListSnapshotsResponse, error)
	ControllerExpandVolume(context.Context, *ControllerExpandVolumeRequest) (*ControllerExpandVolumeResponse, error)
}
```

这些RPC接口被各种不同的CSI sidecar容器调用。

以`CreateVolume`为例，这个RPC方法主要是被CSI Provisioner在创建PV时调用，下面是openebs-zfs CSI的controller server的日志：

```text
I0113 07:27:46.850763       1 grpc.go:72] GRPC call: /csi.v1.Controller/CreateVolume requests {"accessibility_requirements":{"preferred":[{"segments":{"kubernetes.io/arch":"amd64","kubernetes.io/hostname":"master","kubernetes.io/os":"linux","node-role.kubernetes.io/master":"true","openebs.io/nodeid":"master","openebs.io/nodename":"master"}}],"requisite":[{"segments":{"kubernetes.io/arch":"amd64","kubernetes.io/hostname":"master","kubernetes.io/os":"linux","node-role.kubernetes.io/master":"true","openebs.io/nodeid":"master","openebs.io/nodename":"master"}}]},"capacity_range":{"required_bytes":3221225472},"name":"pvc-a9de9b08-76a3-485c-a457-e44c4fd1f8b5","parameters":{"compression":"on","csi.storage.k8s.io/pv/name":"pvc-a9de9b08-76a3-485c-a457-e44c4fd1f8b5","csi.storage.k8s.io/pvc/name":"zfs-pvc3","csi.storage.k8s.io/pvc/namespace":"default","dedup":"on","fsType":"zfs","poolName":"zfspv-pool"},"volume_capabilities":[{"AccessType":{"Mount":{"fs_type":"zfs"}},"access_mode":{"mode":1}}]}
I0113 07:27:47.985432       1 grpc.go:81] GRPC response: {"volume":{"accessible_topology":[{"segments":{"openebs.io/nodeid":"master"}}],"capacity_bytes":3221225472,"volume_context":{"openebs.io/cas-type":"localpv-zfs","openebs.io/poolname":"zfspv-pool"},"volume_id":"pvc-a9de9b08-76a3-485c-a457-e44c4fd1f8b5"}}
```

CSI Provisioner调用`CreateVolume`方法的参数里，包含了访问拓扑信息`accessibility_requirements`。如果RPC调用参数里没有明确指定节点信息，zfs controller server会执行一个调度处理：

1. 计算所有节点的调度权重信息（一般默认是容量），得到一个节点和权重信息的map。以zfs  controller server为例，权重信息就是各个节点上指定的zpool已经分配给`ZFSVolume`的空间。

2. 如果RPC参数`accessibility_requirements.preferred`存在，则将其作为拓扑信息，否则以`accessibility_requirements.requisite`，则将其作为拓扑信息。如果两个都没有，则不再对#1得到的节点列表进行处理。

3. 根据拓扑信息筛选出所有符合要求的节点列表，即拓扑信息里所有标签都匹配的节点。然后将所有满足拓扑关系的节点按照权重信息排序，还没有创建过ZFSVolume或者已分配空间越小的节点优先级越高（要求所有节点的zpool容量是固定的）。

4. 按节点优先级创建和节点绑定的`ZFSVolume`，如下所示里的`spec.ownerNodeID`，如果失败则继续尝试下一个节点直到成功。

   ```yaml
   [root@master ~]# kubectl get zfsvolume -n openebs     pvc-7005d77a-fd88-4f35-8974-c32ae13d2125 -o yaml
   apiVersion: zfs.openebs.io/v1
   kind: ZFSVolume
   metadata:
     creationTimestamp: "2025-01-07T06:43:46Z"
     finalizers:
     - zfs.openebs.io/finalizer
     generation: 2
     labels:
       kubernetes.io/nodename: master
     name: pvc-7005d77a-fd88-4f35-8974-c32ae13d2125
     namespace: openebs
     resourceVersion: "2740957"
     uid: 793cc5fa-d252-4c70-9fb1-c7ecd3bedaf5
   spec:
     capacity: "26843545600"
     compression: "on"
     dedup: "on"
     fsType: zfs
     ownerNodeID: master
     poolName: zfspv-pool
     volumeType: DATASET
   status:
     state: Ready
   ```

### Node Server APIs

Node Server需要实现如下RPC服务接口：

```go
// NodeServer is the server API for Node service.
type NodeServer interface {
	NodeStageVolume(context.Context, *NodeStageVolumeRequest) (*NodeStageVolumeResponse, error)
	NodeUnstageVolume(context.Context, *NodeUnstageVolumeRequest) (*NodeUnstageVolumeResponse, error)
    // NodePublishVolume 由kubelet的volumemanager调用，在启动pod时处理，负责执行节点级的文件系统/设备挂载
	NodePublishVolume(context.Context, *NodePublishVolumeRequest) (*NodePublishVolumeResponse, error)
    // 执行NodePublishVolume的逆操作，pod被删除后执行
	NodeUnpublishVolume(context.Context, *NodeUnpublishVolumeRequest) (*NodeUnpublishVolumeResponse, error)
	NodeGetVolumeStats(context.Context, *NodeGetVolumeStatsRequest) (*NodeGetVolumeStatsResponse, error)
	NodeExpandVolume(context.Context, *NodeExpandVolumeRequest) (*NodeExpandVolumeResponse, error)
	NodeGetCapabilities(context.Context, *NodeGetCapabilitiesRequest) (*NodeGetCapabilitiesResponse, error)
	NodeGetInfo(context.Context, *NodeGetInfoRequest) (*NodeGetInfoResponse, error)
}
```

以openebs-zfs为例，它的Node Server会启动几个单独的协程watcher负责处理Controller server创建的CSI资源：

1. zfsnode resource watcher - 维护节点信息。
2. zfsvolume watcher - 监视集群中的`ZFSVolume`资源，并创建或者删除对应的zfs存储资源，比如zvol或者dataset。
3. snapshot watcher - 监视集群中的`ZFSSnapshot`资源，并创建或者删除对应的zfs快照。
4. backup controller
5. restore controller

