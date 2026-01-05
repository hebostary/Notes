# Kubevirt分析

## 核心组件

### virt-api  - TODO



### virt-controller - TODO

服务启动：

```text
# 启动各种资源的informer
STARTING informer kubeVirtInformer
STARTING informer CRDInformer


# 启动各种资源的controller
Starting migration controller
Starting VirtualMachine controller
Starting clone controller
Starting snapshot controller
Starting restore controller
Starting vmi controller
...
```



### virt-launcher-monitor

虚拟机virt-launcher pod的启动命令就是/usr/bin/virt-launcher-monitor，启动参数如下：

```bash
/usr/bin/virt-launcher-monitor --qemu-timeout 278s --name ubuntu-amd --uid 2852bf00-db39-4343-8c28-3104dfb84ba1 --namespace default --kubevirt-share-dir /var/run/kubevirt --ephemeral-disk-dir /var/run/kubevirt-ephemeral-disks --container-disk-dir /var/run/kubevirt/container-disks --grace-period-seconds 135 --hook-sidecars 0 --ovmf-path /usr/share/OVMF --run-as-nonroot
```

virt-launcher-monitor负责管理virt-launcher进程的生命周期：

1. 当virt-launcher进程退出时，virt-launcher-monitor收到SIGCHLD信号，如果qemu进程还存在则发送SIGTERM信号停止qemu进程后自行退出。
2. virt-launcher-monitor进程收到SIGINT、SIGTERM、SIGQUIT后，先发送SIGTERM信号停止virt-launcher进程后自行退出。

上面两种情况，虚拟机pod会自动被重新创建。



### virt-launcher

virt-launcher-monitor负责启动virt-launcher，并传递所有启动参数：

```bash
/usr/bin/virt-launcher --qemu-timeout 278s --name ubuntu-amd --uid 2852bf00-db39-4343-8c28-3104dfb84ba1 --namespace default --kubevirt-share-dir /var/run/kubevirt --ephemeral-disk-dir /var/run/kubevirt-ephemeral-disks --container-disk-dir /var/run/kubevirt/container-disks --grace-period-seconds 135 --hook-sidecars 0 --ovmf-path /usr/share/OVMF --run-as-nonroot
```

virt-launcher启动过程：

1. 初始化本地目录和磁盘目录:`/var/run/kubevirt-private`.

2. 启动`/usr/sbin/virtqemud -f /var/run/libvirt/virtqemud.conf`,`virtqemud`进程意外退出后,virt-launcher会自动将其重启.

3. 启动`/usr/sbin/virtlogd -f /etc/libvirt/virtlogd.conf`,`virtlogd `进程意外退出后,virt-launcher会自动将其重启.

4. 启动`QEMUSeaBiosLogging`,这个协程从管道文件`/var/run/kubevirt-private/QEMUSeaBiosDebugPipe`读取日志信息然后打印带有`SeaBios`的标准日志.

5. 初始化`LibvirtDomainManager`并启动CmdServer, CmdServer提供gRPC服务接口用于给虚拟机domain发送控制命令。但是此时，CmdServer暂不对外提供服务,处于初始化状态。

6. 启动`DomainEventMonitoring`组件, 首先调用 libvirt 的 `virEventRunDefaultImplWrapper`接口启动 libvirt 事件调度系统，监听和处理虚拟化系统中的事件（如虚拟机状态变化、网络状态变化等，这类事件通常由 libvirt 触发，并且需要通过事件驱动机制进行处理），然后启动DomainNotifier负责将libvirt事件发送到virt-handler，后面会有详细的解析。

   > 在实际使用中，virEventRunDefaultImplWrapper函数通常会用在需要定期检查事件并触发响应操作的场景。它是事件循环的一部分，目的是让 libvirt 持续运行，直到事件处理完成或被中断。

7. 标记pod ready, 把`/var/run/kubevirt/sockets/launcher-init-sock`重命名为`/var/run/kubevirt/sockets/launcher-sock`.至此,容器的*readiness*检查通过，这会告诉virt-controller组件virt-launcher已经准备好管理虚拟机。同节点上的`virt-handler`将通过这个socket调用virt-launcher的gRPC接口。

8. 等待虚拟机domain UUID,每10秒或者收到来自libvirtd的domainevents,就去librivtd去检查domain UUID是否就绪:

   1. 虚拟机domain就绪后的状态信息:

      ```bash
      sh-5.1$ virsh dominfo default_ubuntu-amd
      Authorization not available. Check if polkit service is running or see debug message for more information.
      Id:             1
      Name:           default_ubuntu-amd
      UUID:           d3c2dac2-71b8-5a49-a5a6-7f3bdb93e2c3
      OS Type:        hvm
      State:          running
      CPU(s):         4
      CPU time:       50.7s
      Max memory:     4194304 KiB
      Used memory:    4194304 KiB
      Persistent:     yes
      Autostart:      disable
      Managed save:   no
      Security model: none
      Security DOI:   0
      ```

   2. 到达`--qemu-timeout 278s`还是查询不到UUID后, virt-launcher进程就会直接退出.

9. 主协程开始监控qemu进程, 主要是根据`/run/libvirt/qemu/run/<domain name>.pid`里记录的qemu进程号去检查该进程是否不存在或者变成了僵尸进程,如果是, virt-launcher执行一些清理操作然后退出。正常情况下, virt-launcher在退出时会对domain执行gracefulShutdown, gracefulShutdown超时(`--grace-period-seconds 135`)后继续执行finalShutdown, 即调用对domain的`Destroy`操作，如果Destroy操作失败,virt-launcher直接发送`SIGTERM`信号.

virt-launcher核心数据结构：

```go
/* Launcher接口：
KillVirtualMachine
ShutdownVirtualMachine
DeleteVirtualMachine
...
*/
type Launcher struct {
	domainManager  virtwrap.DomainManager
	allowEmulation bool
}

type LibvirtDomainManager struct {
    // 用于访问libvirtd的连接
    virConn cli.Connection
    
    // ...
}
```

virt-launcher通过`CGO`调用`libvirt-qemu.so.0`提供的接口来管控qemu虚拟机。同时，virt-launcher暴露gRPC服务接口给其他工具调用，比如：

```go
//kubevirt/pkg/handler-launcher-com/cmd/v1/cmd.pb.go
SyncVirtualMachine 
FreezeVirtualMachine
SoftRebootVirtualMachine
ShutdownVirtualMachine
```

这些gRPC接口的客户端包括：

1. `virt-freezer`：通过给虚拟机domain发送QemuAgentCommand： `guest-fsfreeze-freeze`。
2. `virt-handler`：

#### DomainNotifier

DomainNotifier是virt-launcher中负责在libvirt和virt-handler之间中转虚拟机事件的组件，它的工作机制主要是通过 libvirt 连接注册一些回调函数处理如下感兴趣的虚拟机domain事件，这些回调函数收到 libvirt 发送的事件后通过 channel 发送给一个独立的协程触发事件更新，这个协程会通过libvirt查询虚拟机domain的最新状态信息，包括state和reason，然后调用virt-handler提供的gRPC接口发送domain事件和k8s事件（只有`ReasonPausedIOError`会发送k8s事件）。

```go
libvirt.DomainEventLifecycle
libvirt.DomainEventDeviceAdded
libvirt.DomainEventDeviceRemoved
libvirt.DomainEventMemoryDeviceSizeChange
libvirt.DomainEventAgentLifecycle
```

其中，DomainEventAgentLifecycle事件里连接成功和断开连接两种事件会触发启动和停止agent poller，agent poller会周期性地调用下面这些qemu命令获取虚拟机domain的基本信息并存储到 AsyncAgentStore 里（sync.Map，key就是下面这些AgentCommand）。LibvirtDomainManager的相应接口会直接从AsyncAgentStore 查询相关信息，而不是每次都去调用 libvirt 的接口。

```go
const (
	GET_OSINFO          AgentCommand = "guest-get-osinfo"
	GET_HOSTNAME        AgentCommand = "guest-get-host-name"
	GET_INTERFACES      AgentCommand = "guest-network-get-interfaces"
	GET_TIMEZONE        AgentCommand = "guest-get-timezone"
	GET_USERS           AgentCommand = "guest-get-users"
	GET_FILESYSTEM      AgentCommand = "guest-get-fsinfo"
	GET_AGENT           AgentCommand = "guest-info"
	GET_FSFREEZE_STATUS AgentCommand = "guest-fsfreeze-status"

	pollInitialInterval = 10 * time.Second
)
```



### virt-handler

> virt-handler pod是共享主机PID namespace的。

virt-launcher 核心数据结构：

```go
type VirtualMachineController struct {}
```

VirtualMachineController 不断地从自己的队列里取出虚拟机key，然后根据key从`vmiSourceInformer`的cache里获取最新的`vmi`状态，从`domainInformer`获取domain状态，然后顺序检查如下虚拟机事件是否发生，并调用相应的`virt-launcher`的gRPC接口控制虚拟机：

```go
	switch {
	case forceIgnoreSync:
		log.Log.Object(vmi).V(3).Info("No update processing required: forced ignore")
	case shouldShutdown:
		log.Log.Object(vmi).V(3).Info("Processing shutdown.")
		syncErr = d.processVmShutdown(vmi, domain)
	case shouldDelete:
		log.Log.Object(vmi).V(3).Info("Processing deletion.")
		syncErr = d.processVmDelete(vmi)
	case shouldCleanUp:
		log.Log.Object(vmi).V(3).Info("Processing local ephemeral data cleanup for shutdown domain.")
		syncErr = d.processVmCleanup(vmi)
	case shouldUpdate:
		log.Log.Object(vmi).V(3).Info("Processing vmi update")
		syncErr = d.processVmUpdate(vmi, domain)
	default:
		log.Log.Object(vmi).V(3).Info("No update processing required")
	}
```

#### DomainWatcher

virt-handler内部的`DomainWatcher`结构体实现了k8s`ListerWatcher`接口，可以包装在k8s`cache.SharedInformer`里作为虚拟机domain信息的缓存，并注册相应的事件handler来处理虚拟机domain的状态变化。

1. List接口通过遍历主机上的socket文件（`/var/lib/kubelet/pods/<pod uid>/volumes/kubernetes.io~empty-dir/sockets/launcher-sock`）来查询当前节点上有哪些虚拟机domain及其详细信息。
2. Watch接口主要是传递virt-launcher通过notify-server发送的虚拟机domain事件。

#### notify-server

virt-handler的notify-server负责提供gRPC服务专门处理virt-launcher发送k8s事件和虚拟机domain事件，前者的接口直接调用k8s的`EventRecorder`生成k8s事件，后者的接口将虚拟机domain事件发送DomainWatcher的eventChannel，然后传递给调用watch接口的客户端。



## 管理工具

### virt-freezer

virt-freezer工具用于freeze和unfreeze虚拟机domain，这个命令需要在virt-launcher容器内执行，因为它是通过UDS（/run/kubevirt/sockets/launcher-sock）来调用virt-launcher的gRPC接口。

在使用velero对kubevirt虚拟机进行备份时，会在preback hook阶段执行`virt-freezer --freeze`命令先将虚拟机freeze，在postbackup hook阶段执行`virt-freezer --unfreeze`。

### container-disk





## 资源对象

### vm resource

```yaml
# kubectl get vm ubuntu-amd -o yaml
apiVersion: kubevirt.io/v1
kind: VirtualMachine
metadata:
  annotations:
    creator: admin
    kubevirt.io/latest-observed-api-version: v1
    kubevirt.io/storage-observed-api-version: v1
    limit-cpu: "4"
    limit-memory: 4Gi
    os: Linux
    request-cpu: "4"
    request-memory: 4Gi
    sys-disk-size: "25"
    vm-image: docker.io/virtual-machine-image/ubuntu20:amd64
  creationTimestamp: "2024-10-28T10:42:54Z"
  finalizers:
  - kubevirt.io/virtualMachineControllerFinalize
  generation: 2
  name: ubuntu-amd
  namespace: default
  resourceVersion: "363019"
  uid: dffd2d2d-8f12-4a77-ab00-2754e32abbaf
spec:
  running: true
  template:
    metadata:
      creationTimestamp: null
    spec:
      architecture: amd64
      domain:
        cpu:
          cores: 4
          sockets: 1
          threads: 1
        devices:
          disks:
          - bootOrder: 1
            disk:
              bus: virtio
            name: rootdisk
          - bootOrder: 2
            cdrom:
              bus: sata
            name: containerdisk
          inputs:
          - bus: usb
            name: tablet1
            type: tablet
        features:
          acpi: {}
          apic: {}
        machine:
          type: q35
        resources:
          limits:
            cpu: "4"
            memory: 4Gi
          requests:
            cpu: "4"
            memory: 4Gi
      nodeSelector:
        kubernetes.io/arch: amd64
      terminationGracePeriodSeconds: 120
      volumes:
      - name: rootdisk
        persistentVolumeClaim:
          claimName: ubuntu-amd-sys-disk
      - containerDisk:
          image: registry.cluster.local:30444/virtual-machine-image/ubuntu20:amd64
          imagePullPolicy: IfNotPresent
        name: containerdisk
status:
  conditions:
  - lastProbeTime: null
    lastTransitionTime: "2024-10-28T10:43:47Z"
    status: "True"
    type: Ready
  - lastProbeTime: null
    lastTransitionTime: null
    message: 'cannot migrate VMI: PVC ubuntu-amd-sys-disk is not shared, live migration
      requires that all PVCs must be shared (using ReadWriteMany access mode)'
    reason: DisksNotLiveMigratable
    status: "False"
    type: LiveMigratable
  created: true
  desiredGeneration: 2
  observedGeneration: 2
  printableStatus: Running
  ready: true
  runStrategy: Always
  volumeSnapshotStatuses:
  - enabled: false
    name: rootdisk
    reason: 'No VolumeSnapshotClass: Volume snapshots are not configured for this
      StorageClass [openebs-hostpath] [rootdisk]'
  - enabled: false
    name: containerdisk
    reason: Snapshot is not supported for this volumeSource type [containerdisk]
```



### vmi resource

```yaml
kubectl get vmi ubuntu-amd -o yaml
apiVersion: kubevirt.io/v1
kind: VirtualMachineInstance
metadata:
  annotations:
    kubevirt.io/latest-observed-api-version: v1
    kubevirt.io/storage-observed-api-version: v1
    kubevirt.io/vm-generation: "2"
  creationTimestamp: "2024-10-28T10:43:42Z"
  finalizers:
  - kubevirt.io/virtualMachineControllerFinalize
  - foregroundDeleteVirtualMachine
  generation: 8
  labels:
    kubevirt.io/nodeName: master
  name: ubuntu-amd
  namespace: default
  ownerReferences:
  - apiVersion: kubevirt.io/v1
    blockOwnerDeletion: true
    controller: true
    kind: VirtualMachine
    name: ubuntu-amd
    uid: dffd2d2d-8f12-4a77-ab00-2754e32abbaf
  resourceVersion: "363018"
  uid: 7ac26e32-a799-4eec-9a2a-cc8ae86621b0
spec:
  architecture: amd64
  domain:
    cpu:
      cores: 4
      model: host-model
      sockets: 1
      threads: 1
    devices:
      disks:
      - bootOrder: 1
        disk:
          bus: virtio
        name: rootdisk
      - bootOrder: 2
        cdrom:
          bus: sata
          readonly: true
          tray: closed
        name: containerdisk
      inputs:
      - bus: usb
        name: tablet1
        type: tablet
      interfaces:
      - bridge: {}
        name: default
    features:
      acpi:
        enabled: true
      apic:
        enabled: true
    firmware:
      uuid: d3c2dac2-71b8-5a49-a5a6-7f3bdb93e2c3
    machine:
      type: q35
    resources:
      limits:
        cpu: "4"
        memory: 4Gi
      requests:
        cpu: "4"
        memory: 4Gi
  evictionStrategy: None
  networks:
  - name: default
    pod: {}
  nodeSelector:
    kubernetes.io/arch: amd64
  terminationGracePeriodSeconds: 120
  volumes:
  - name: rootdisk
    persistentVolumeClaim:
      claimName: ubuntu-amd-sys-disk
  - containerDisk:
      image: registry.cluster.local:30444/virtual-machine-image/ubuntu20:amd64
      imagePullPolicy: IfNotPresent
    name: containerdisk
status:
  activePods:
    cfad06b0-b6ec-4b1b-9f88-10d009f4c3ab: master
  conditions:
  - lastProbeTime: null
    lastTransitionTime: "2024-10-28T10:43:47Z"
    status: "True"
    type: Ready
  - lastProbeTime: null
    lastTransitionTime: null
    message: 'cannot migrate VMI: PVC ubuntu-amd-sys-disk is not shared, live migration
      requires that all PVCs must be shared (using ReadWriteMany access mode)'
    reason: DisksNotLiveMigratable
    status: "False"
    type: LiveMigratable
  currentCPUTopology:
    cores: 4
    sockets: 1
    threads: 1
  guestOSInfo: {}
  interfaces:
  - infoSource: domain
    ipAddress: 10.233.97.174
    ipAddresses:
    - 10.233.97.174
    mac: 6a:07:67:28:e5:d7
    name: default
    queueCount: 1
  launcherContainerImageVersion: docker.io/virt-launcher:v1.2.2
  machine:
    type: pc-q35-rhel9.2.0
  memory:
    guestCurrent: 4Gi
  migrationMethod: BlockMigration
  migrationTransport: Unix
  nodeName: master
  phase: Running
  phaseTransitionTimestamps:
  - phase: Pending
    phaseTransitionTimestamp: "2024-10-28T10:43:42Z"
  - phase: Scheduling
    phaseTransitionTimestamp: "2024-10-28T10:43:42Z"
  - phase: Scheduled
    phaseTransitionTimestamp: "2024-10-28T10:43:47Z"
  - phase: Running
    phaseTransitionTimestamp: "2024-10-28T10:43:49Z"
  qosClass: Guaranteed
  runtimeUser: 107
  selinuxContext: none
  virtualMachineRevisionName: revision-start-vm-dffd2d2d-8f12-4a77-ab00-2754e32abbaf-2
  volumeStatus:
  - containerDiskVolume:
      checksum: 501717117
    name: containerdisk
    target: sda
  - name: rootdisk
    persistentVolumeClaimInfo:
      accessModes:
      - ReadWriteOnce
      capacity:
        storage: 25Gi
      filesystemOverhead: "0.055"
      requests:
        storage: 25Gi
      volumeMode: Filesystem
    target: vda
```



### vm pod instance

```yaml
apiVersion: v1
kind: Pod
metadata:
  annotations:
    kubectl.kubernetes.io/default-container: compute
    kubevirt.io/domain: ubuntu-amd
    kubevirt.io/migrationTransportUnix: "true"
    kubevirt.io/vm-generation: "2"
    post.hook.backup.velero.io/command: '["/usr/bin/virt-freezer", "--unfreeze", "--name",
      "ubuntu-amd", "--namespace", "default"]'
    post.hook.backup.velero.io/container: compute
    pre.hook.backup.velero.io/command: '["/usr/bin/virt-freezer", "--freeze", "--name",
      "ubuntu-amd", "--namespace", "default"]'
    pre.hook.backup.velero.io/container: compute
  creationTimestamp: "2024-10-28T10:43:42Z"
  generateName: virt-launcher-ubuntu-amd-
  labels:
    kubevirt.io: virt-launcher
    kubevirt.io/created-by: 7ac26e32-a799-4eec-9a2a-cc8ae86621b0
    kubevirt.io/nodeName: master
    vm.kubevirt.io/name: ubuntu-amd
  name: virt-launcher-ubuntu-amd-khwrs
  namespace: default
  ownerReferences:
  - apiVersion: kubevirt.io/v1
    blockOwnerDeletion: true
    controller: true
    kind: VirtualMachineInstance
    name: ubuntu-amd
    uid: 7ac26e32-a799-4eec-9a2a-cc8ae86621b0
  resourceVersion: "363001"
  uid: cfad06b0-b6ec-4b1b-9f88-10d009f4c3ab
spec:
  affinity:
    nodeAffinity:
      requiredDuringSchedulingIgnoredDuringExecution:
        nodeSelectorTerms:
        - matchExpressions:
          - key: node-labeller.kubevirt.io/obsolete-host-model
            operator: DoesNotExist
  automountServiceAccountToken: false
  containers:
  - command:
    - /usr/bin/virt-launcher-monitor
    - --qemu-timeout
    - 301s
    - --name
    - ubuntu-amd
    - --uid
    - 7ac26e32-a799-4eec-9a2a-cc8ae86621b0
    - --namespace
    - default
    - --kubevirt-share-dir
    - /var/run/kubevirt
    - --ephemeral-disk-dir
    - /var/run/kubevirt-ephemeral-disks
    - --container-disk-dir
    - /var/run/kubevirt/container-disks
    - --grace-period-seconds
    - "135"
    - --hook-sidecars
    - "0"
    - --ovmf-path
    - /usr/share/OVMF
    - --run-as-nonroot
    env:
    - name: XDG_CACHE_HOME
      value: /var/run/kubevirt-private
    - name: XDG_CONFIG_HOME
      value: /var/run/kubevirt-private
    - name: XDG_RUNTIME_DIR
      value: /var/run
    - name: POD_NAME
      valueFrom:
        fieldRef:
          apiVersion: v1
          fieldPath: metadata.name
    image: docker.io/virt-launcher:v1.2.2
    imagePullPolicy: IfNotPresent
    name: compute
    resources:
      limits:
        cpu: "4"
        devices.kubevirt.io/kvm: "1"
        devices.kubevirt.io/tun: "1"
        devices.kubevirt.io/vhost-net: "1"
        memory: "4687134721"
      requests:
        cpu: "4"
        devices.kubevirt.io/kvm: "1"
        devices.kubevirt.io/tun: "1"
        devices.kubevirt.io/vhost-net: "1"
        ephemeral-storage: 50M
        memory: "4687134721"
    securityContext:
      allowPrivilegeEscalation: false
      capabilities:
        add:
        - NET_BIND_SERVICE
        drop:
        - ALL
      privileged: false
      runAsGroup: 107
      runAsNonRoot: true
      runAsUser: 107
    terminationMessagePath: /dev/termination-log
    terminationMessagePolicy: File
    volumeMounts:
    - mountPath: /var/run/kubevirt-private
      name: private
    - mountPath: /var/run/kubevirt
      name: public
    - mountPath: /var/run/kubevirt-ephemeral-disks
      name: ephemeral-disks
    - mountPath: /var/run/kubevirt/container-disks
      mountPropagation: HostToContainer
      name: container-disks
    - mountPath: /var/run/libvirt
      name: libvirt-runtime
    - mountPath: /var/run/kubevirt/sockets
      name: sockets
    - mountPath: /var/run/kubevirt-private/vmi-disks/rootdisk
      name: rootdisk
    - mountPath: /var/run/kubevirt/hotplug-disks
      mountPropagation: HostToContainer
      name: hotplug-disks
  - args:
    - --copy-path
    - /var/run/kubevirt-ephemeral-disks/container-disk-data/7ac26e32-a799-4eec-9a2a-cc8ae86621b0/disk_1
    command:
    - /usr/bin/container-disk
    image: registry.cluster.local:30444/virtual-machine-image/ubuntu20:amd64
    imagePullPolicy: IfNotPresent
    name: volumecontainerdisk
    resources:
      limits:
        cpu: 10m
        memory: 40M
      requests:
        cpu: 10m
        ephemeral-storage: 50M
        memory: 40M
    securityContext:
      allowPrivilegeEscalation: false
      capabilities:
        drop:
        - ALL
      runAsNonRoot: true
      runAsUser: 107
    terminationMessagePath: /dev/termination-log
    terminationMessagePolicy: File
    volumeMounts:
    - mountPath: /var/run/kubevirt-ephemeral-disks/container-disk-data/7ac26e32-a799-4eec-9a2a-cc8ae86621b0
      name: container-disks
    - mountPath: /usr/bin
      name: virt-bin-share-dir
  - args:
    - --logfile
    - /var/run/kubevirt-private/7ac26e32-a799-4eec-9a2a-cc8ae86621b0/virt-serial0-log
    command:
    - /usr/bin/virt-tail
    env:
    - name: VIRT_LAUNCHER_LOG_VERBOSITY
      value: "2"
    image: docker.io/virt-launcher:v1.2.2
    imagePullPolicy: IfNotPresent
    name: guest-console-log
    resources:
      limits:
        cpu: 15m
        memory: 60M
      requests:
        cpu: 15m
        memory: 60M
    securityContext:
      allowPrivilegeEscalation: false
      capabilities:
        drop:
        - ALL
      runAsNonRoot: true
      runAsUser: 107
    terminationMessagePath: /dev/termination-log
    terminationMessagePolicy: File
    volumeMounts:
    - mountPath: /var/run/kubevirt-private
      name: private
      readOnly: true
  dnsPolicy: ClusterFirst
  enableServiceLinks: false
  hostname: ubuntu-amd
  imagePullSecrets:
  - name: registry-docker-config
  initContainers:
  - command:
    - /usr/bin/cp
    - /usr/bin/container-disk
    - /init/usr/bin/container-disk
    env:
    - name: XDG_CACHE_HOME
      value: /var/run/kubevirt-private
    - name: XDG_CONFIG_HOME
      value: /var/run/kubevirt-private
    - name: XDG_RUNTIME_DIR
      value: /var/run
    image: docker.io/virt-launcher:v1.2.2
    imagePullPolicy: IfNotPresent
    name: container-disk-binary
    resources:
      limits:
        cpu: 10m
        memory: 40M
      requests:
        cpu: 10m
        memory: 40M
    securityContext:
      allowPrivilegeEscalation: false
      capabilities:
        drop:
        - ALL
      privileged: false
      runAsGroup: 107
      runAsNonRoot: true
      runAsUser: 107
    terminationMessagePath: /dev/termination-log
    terminationMessagePolicy: File
    volumeMounts:
    - mountPath: /init/usr/bin
      name: virt-bin-share-dir
  - args:
    - --no-op
    command:
    - /usr/bin/container-disk
    image: registry.cluster.local:30444/virtual-machine-image/ubuntu20:amd64
    imagePullPolicy: IfNotPresent
    name: volumecontainerdisk-init
    resources:
      limits:
        cpu: 10m
        memory: 40M
      requests:
        cpu: 10m
        ephemeral-storage: 50M
        memory: 40M
    securityContext:
      allowPrivilegeEscalation: false
      capabilities:
        drop:
        - ALL
      runAsNonRoot: true
      runAsUser: 107
    terminationMessagePath: /dev/termination-log
    terminationMessagePolicy: File
    volumeMounts:
    - mountPath: /var/run/kubevirt-ephemeral-disks/container-disk-data/7ac26e32-a799-4eec-9a2a-cc8ae86621b0
      name: container-disks
    - mountPath: /usr/bin
      name: virt-bin-share-dir
  nodeName: master
  nodeSelector:
    kubernetes.io/arch: amd64
    kubevirt.io/schedulable: "true"
  preemptionPolicy: PreemptLowerPriority
  priority: 0
  readinessGates:
  - conditionType: kubevirt.io/virtual-machine-unpaused
  restartPolicy: Never
  schedulerName: default-scheduler
  securityContext:
    fsGroup: 107
    runAsGroup: 107
    runAsNonRoot: true
    runAsUser: 107
  serviceAccount: default
  serviceAccountName: default
  terminationGracePeriodSeconds: 150
  tolerations:
  - effect: NoExecute
    key: node.kubernetes.io/not-ready
    operator: Exists
    tolerationSeconds: 300
  - effect: NoExecute
    key: node.kubernetes.io/unreachable
    operator: Exists
    tolerationSeconds: 300
  volumes:
  - emptyDir: {}
    name: private
  - emptyDir: {}
    name: public
  - emptyDir: {}
    name: sockets
  - emptyDir: {}
    name: virt-bin-share-dir
  - emptyDir: {}
    name: libvirt-runtime
  - emptyDir: {}
    name: ephemeral-disks
  - emptyDir: {}
    name: container-disks
  - name: rootdisk
    persistentVolumeClaim:
      claimName: ubuntu-amd-sys-disk
  - emptyDir: {}
    name: hotplug-disks
```

虚拟机pod的启动流程：

1. initContainers：
   1. 从virt-launcher镜像里拷贝`/usr/bin/container-disk`工具到`virt-bin-share-dir`这个临时volume。
   2. 从虚拟机镜像启动容器并挂载`virt-bin-share-dir`和`container-disks`，执行`/usr/bin/container-disk --no-op`
2. containers:
   1. compute：virt-launcher-monitor进程，负责启动libvirtd。
   2. volumecontainerdisk：挂载`virt-bin-share-dir`和`container-disks`，执行`/usr/bin/container-disk --copy-path  -/var/run/kubevirt-ephemeral-disks/container-disk-data/7ac26e32-a799-4eec-9a2a-cc8ae86621b0/disk_1 `
   3. guest-console-log：执行`/usr/bin/virt-tail --logfile /var/run/kubevirt-private/7ac26e32-a799-4eec-9a2a-cc8ae86621b0/virt-serial0-log`



虚拟机pod内网络设备:

```bash
sh-5.1$ ip link show
1: lo: <LOOPBACK,UP,LOWER_UP> mtu 65536 qdisc noqueue state UNKNOWN mode DEFAULT group default qlen 1000
    link/loopback 00:00:00:00:00:00 brd 00:00:00:00:00:00
2: tunl0@NONE: <NOARP> mtu 1480 qdisc noop state DOWN mode DEFAULT group default qlen 1000
    link/ipip 0.0.0.0 brd 0.0.0.0
4: eth0-nic@if544: <BROADCAST,MULTICAST,UP,LOWER_UP> mtu 1450 qdisc noqueue master k6t-eth0 state UP mode DEFAULT group default 
    link/ether 32:6c:de:16:c6:ad brd ff:ff:ff:ff:ff:ff link-netnsid 0
5: k6t-eth0: <BROADCAST,MULTICAST,UP,LOWER_UP> mtu 1450 qdisc noqueue state UP mode DEFAULT group default qlen 1000
    link/ether 32:6c:de:16:c6:ad brd ff:ff:ff:ff:ff:ff
6: tap0: <BROADCAST,MULTICAST,UP,LOWER_UP> mtu 1450 qdisc pfifo_fast master k6t-eth0 state UP mode DEFAULT group default qlen 1000
    link/ether 7a:a2:1e:cb:26:85 brd ff:ff:ff:ff:ff:ff
7: eth0: <BROADCAST,NOARP> mtu 1450 qdisc noop state DOWN mode DEFAULT group default qlen 1000
    link/ether ca:5f:48:41:7d:5d brd ff:ff:ff:ff:ff:ff
```

### vm pod资源监控

关于虚拟机资源分配，以分配request和limit都是`8Gi`的Windows虚拟机为例，实际上虚拟机pod分配的内存要更大一些，如下是从cgroup里看到的虚拟机pod的内存数据：

```bash
[root@master1 ~]# cat /sys/fs/cgroup/memory/kubepods.slice/kubepods-pod79bc48d0_783f_4c88_af9f_1717dadd89c4.slice/memory.max_usage_in_bytes 
8717844480
[root@master1 ~]# cat /sys/fs/cgroup/memory/kubepods.slice/kubepods-pod79bc48d0_783f_4c88_af9f_1717dadd89c4.slice/memory.limit_in_bytes 
8923688960
[root@master1 ~]# cat /sys/fs/cgroup/memory/kubepods.slice/kubepods-pod79bc48d0_783f_4c88_af9f_1717dadd89c4.slice/memory.usage_in_bytes
8691228672 //8288.6MiB
```

如下，从cadvisor的采样数据来看，虚拟机pod占用的内存是8.680710144e+09 bytes（8268.6 MiB），即pause（44Ki）、compute、volumecontainerdisk和volumevirtio4个容器占用内存之和。其中，只有compute的资源是随着用户设置变化的，其它容器资源都是固定的。

```bash
[root@master1 ~]# curl https://10.233.0.1:443/api/v1/nodes/master1/proxy/metrics/cadvisor -k|grep container_memory_rss |grep win container_memory_rss{container="",id="/kubepods.slice/kubepods-pod79bc48d0_783f_4c88_af9f_1717dadd89c4.slice",image="",name="",namespace="test",pod="virt-launcher-win-test-xhlkz"} 8.680710144e+09 1728485321848
container_memory_rss{container="POD",id="/kubepods.slice/kubepods-pod79bc48d0_783f_4c88_af9f_1717dadd89c4.slice/docker-2111ebb0ff1951bd6e9fa3c1d63f4fa3775adf802ac687acb8682239a0449f96.scope",image="registry.cluster.local:30443/google-containers/pause:3.1",name="k8s_POD_virt-launcher-win-test-xhlkz_test_79bc48d0-783f-4c88-af9f-1717dadd89c4_0",namespace="test",pod="virt-launcher-win-test-xhlkz"} 45056 1728485325826
container_memory_rss{container="compute",id="/kubepods.slice/kubepods-pod79bc48d0_783f_4c88_af9f_1717dadd89c4.slice/docker-a18e8197a7adaf65b49c67a8e8bc1a8e9fd433249a266652feb8e074b707c825.scope",image="sha256:e4411efd66c8ff7aaaf99b94600ae94603b0ae2b24f41bb1c03216017e55d23e",name="k8s_compute_virt-launcher-win-test-xhlkz_test_79bc48d0-783f-4c88-af9f-1717dadd89c4_0",namespace="test",pod="virt-launcher-win-test-xhlkz"} 8.680587264e+09 1728485314718
100 3367k    0 3367k    0     0  15.3M      0 --:--:-- --:--:-- --:--:-- 15.3M
container_memory_rss{container="volumecontainerdisk",id="/kubepods.slice/kubepods-pod79bc48d0_783f_4c88_af9f_1717dadd89c4.slice/docker-4ed9ccd2d0807b1444eaa4c663469a9f156141d96a1d9fd84a55e4c3eb427d39.scope",image="sha256:b79018148e896206891150fd75c9e76c167ee666e9d9f0e914096a1aa6946163",name="k8s_volumecontainerdisk_virt-launcher-win-test-xhlkz_test_79bc48d0-783f-4c88-af9f-1717dadd89c4_0",namespace="test",pod="virt-launcher-win-test-xhlkz"} 40960 1728485323247
container_memory_rss{container="volumevirtio",id="/kubepods.slice/kubepods-pod79bc48d0_783f_4c88_af9f_1717dadd89c4.slice/docker-528b7c545b06195771c85b06d128c3f46a90f5fe6db95f89d21f25eab567df62.scope",image="sha256:a0719164ae8d592a21b6284633d5880066f9719246949f0370215a24b4831cff",name="k8s_volumevirtio_virt-launcher-win-test-xhlkz_test_79bc48d0-783f-4c88-af9f-1717dadd89c4_0",namespace="test",pod="virt-launcher-win-test-xhlkz"} 36864 1728485319277
```

从pod yaml详情里看虚拟机pod的各个容器的资源限制（request等于limit）如下：

1. compute容器：memory: 8434Mi。
2. volumecontainerdisk容器：memory: 40M
3. volumevirtio容器：memory: 40M

另外3个init containers：container-disk-binary、volumecontainerdisk-init、volumevirtio-init都是40M。

### libvirt domain

```xml
<domain type='kvm' id='1'>
  <name>default_ubuntu-amd</name>
  <uuid>d3c2dac2-71b8-5a49-a5a6-7f3bdb93e2c3</uuid>
  <metadata>
    <kubevirt xmlns="http://kubevirt.io">
      <uid/>
    </kubevirt>
  </metadata>
  <memory unit='KiB'>4194304</memory>
  <currentMemory unit='KiB'>4194304</currentMemory>
  <vcpu placement='static'>4</vcpu>
  <iothreads>1</iothreads>
  <sysinfo type='smbios'>
    <system>
      <entry name='manufacturer'>KubeVirt</entry>
      <entry name='product'>None</entry>
      <entry name='uuid'>d3c2dac2-71b8-5a49-a5a6-7f3bdb93e2c3</entry>
      <entry name='family'>KubeVirt</entry>
    </system>
  </sysinfo>
  <os>
    <type arch='x86_64' machine='pc-q35-rhel9.2.0'>hvm</type>
    <smbios mode='sysinfo'/>
  </os>
  <features>
    <acpi/>
    <apic/>
  </features>
  <cpu mode='custom' match='exact' check='full'>
    <model fallback='forbid'>SapphireRapids</model>
    <vendor>Intel</vendor>
    <topology sockets='1' dies='1' cores='4' threads='1'/>
    <feature policy='require' name='ss'/>
    <feature policy='require' name='hypervisor'/>
    <feature policy='require' name='tsc_adjust'/>
    <feature policy='require' name='stibp'/>
    <feature policy='disable' name='la57'/>
    <feature policy='disable' name='rdpid'/>
    <feature policy='disable' name='bus-lock-detect'/>
    <feature policy='disable' name='serialize'/>
    <feature policy='disable' name='tsx-ldtrk'/>
    <feature policy='disable' name='amx-bf16'/>
    <feature policy='disable' name='avx512-fp16'/>
    <feature policy='disable' name='amx-tile'/>
    <feature policy='disable' name='amx-int8'/>
    <feature policy='disable' name='avx-vnni'/>
    <feature policy='disable' name='avx512-bf16'/>
    <feature policy='disable' name='xsaves'/>
    <feature policy='disable' name='xfd'/>
    <feature policy='disable' name='wbnoinvd'/>
    <feature policy='disable' name='rdctl-no'/>
    <feature policy='disable' name='ibrs-all'/>
    <feature policy='disable' name='skip-l1dfl-vmentry'/>
    <feature policy='disable' name='mds-no'/>
    <feature policy='disable' name='pschange-mc-no'/>
    <feature policy='disable' name='taa-no'/>
  </cpu>
  <clock offset='utc'/>
  <on_poweroff>destroy</on_poweroff>
  <on_reboot>restart</on_reboot>
  <on_crash>destroy</on_crash>
  <devices>
    <emulator>/usr/libexec/qemu-kvm</emulator>
    <disk type='file' device='disk' model='virtio-non-transitional'>
      <driver name='qemu' type='raw' cache='none' error_policy='stop' discard='unmap'/>
      <source file='/var/run/kubevirt-private/vmi-disks/rootdisk/disk.img' index='3'/>
      <backingStore/>
      <target dev='vda' bus='virtio'/>
      <boot order='1'/>
      <alias name='ua-rootdisk'/>
      <address type='pci' domain='0x0000' bus='0x08' slot='0x00' function='0x0'/>
    </disk>
    <disk type='file' device='cdrom'>
      <driver name='qemu' type='qcow2' cache='none' error_policy='stop' discard='unmap'/>
      <source file='/var/run/kubevirt-ephemeral-disks/disk-data/containerdisk/disk.qcow2' index='1'/>
      <backingStore type='file' index='2'>
        <format type='raw'/>
        <source file='/var/run/kubevirt/container-disks/disk_1.img'/>
      </backingStore>
      <target dev='sda' bus='sata'/>
      <readonly/>
      <boot order='2'/>
      <alias name='ua-containerdisk'/>
      <address type='drive' controller='0' bus='0' target='0' unit='0'/>
    </disk>
    <controller type='usb' index='0' model='qemu-xhci'>
      <alias name='usb'/>
      <address type='pci' domain='0x0000' bus='0x05' slot='0x00' function='0x0'/>
    </controller>
    <controller type='scsi' index='0' model='virtio-non-transitional'>
      <alias name='scsi0'/>
      <address type='pci' domain='0x0000' bus='0x06' slot='0x00' function='0x0'/>
    </controller>
    <controller type='virtio-serial' index='0' model='virtio-non-transitional'>
      <alias name='virtio-serial0'/>
      <address type='pci' domain='0x0000' bus='0x07' slot='0x00' function='0x0'/>
    </controller>
    <controller type='sata' index='0'>
      <alias name='ide'/>
      <address type='pci' domain='0x0000' bus='0x00' slot='0x1f' function='0x2'/>
    </controller>
    <controller type='pci' index='0' model='pcie-root'>
      <alias name='pcie.0'/>
    </controller>
    <controller type='pci' index='1' model='pcie-root-port'>
      <model name='pcie-root-port'/>
      <target chassis='1' port='0x10'/>
      <alias name='pci.1'/>
      <address type='pci' domain='0x0000' bus='0x00' slot='0x02' function='0x0' multifunction='on'/>
    </controller>
    <controller type='pci' index='2' model='pcie-root-port'>
      <model name='pcie-root-port'/>
      <target chassis='2' port='0x11'/>
      <alias name='pci.2'/>
      <address type='pci' domain='0x0000' bus='0x00' slot='0x02' function='0x1'/>
    </controller>
    <controller type='pci' index='3' model='pcie-root-port'>
      <model name='pcie-root-port'/>
      <target chassis='3' port='0x12'/>
      <alias name='pci.3'/>
      <address type='pci' domain='0x0000' bus='0x00' slot='0x02' function='0x2'/>
    </controller>
    <controller type='pci' index='4' model='pcie-root-port'>
      <model name='pcie-root-port'/>
      <target chassis='4' port='0x13'/>
      <alias name='pci.4'/>
      <address type='pci' domain='0x0000' bus='0x00' slot='0x02' function='0x3'/>
    </controller>
    <controller type='pci' index='5' model='pcie-root-port'>
      <model name='pcie-root-port'/>
      <target chassis='5' port='0x14'/>
      <alias name='pci.5'/>
      <address type='pci' domain='0x0000' bus='0x00' slot='0x02' function='0x4'/>
    </controller>
    <controller type='pci' index='6' model='pcie-root-port'>
      <model name='pcie-root-port'/>
      <target chassis='6' port='0x15'/>
      <alias name='pci.6'/>
      <address type='pci' domain='0x0000' bus='0x00' slot='0x02' function='0x5'/>
    </controller>
    <controller type='pci' index='7' model='pcie-root-port'>
      <model name='pcie-root-port'/>
      <target chassis='7' port='0x16'/>
      <alias name='pci.7'/>
      <address type='pci' domain='0x0000' bus='0x00' slot='0x02' function='0x6'/>
    </controller>
    <controller type='pci' index='8' model='pcie-root-port'>
      <model name='pcie-root-port'/>
      <target chassis='8' port='0x17'/>
      <alias name='pci.8'/>
      <address type='pci' domain='0x0000' bus='0x00' slot='0x02' function='0x7'/>
    </controller>
    <controller type='pci' index='9' model='pcie-root-port'>
      <model name='pcie-root-port'/>
      <target chassis='9' port='0x18'/>
      <alias name='pci.9'/>
      <address type='pci' domain='0x0000' bus='0x00' slot='0x03' function='0x0' multifunction='on'/>
    </controller>
    <controller type='pci' index='10' model='pcie-root-port'>
      <model name='pcie-root-port'/>
      <target chassis='10' port='0x19'/>
      <alias name='pci.10'/>
      <address type='pci' domain='0x0000' bus='0x00' slot='0x03' function='0x1'/>
    </controller>
    <interface type='ethernet'>
      <mac address='ca:5f:48:41:7d:5d'/>
      <target dev='tap0' managed='no'/>
      <model type='virtio-non-transitional'/>
      <mtu size='1450'/>
      <alias name='ua-default'/>
      <rom enabled='no'/>
      <address type='pci' domain='0x0000' bus='0x01' slot='0x00' function='0x0'/>
    </interface>
    <serial type='unix'>
      <source mode='bind' path='/var/run/kubevirt-private/9eae133d-5264-4040-9261-2ff5827e9540/virt-serial0'/>
      <log file='/var/run/kubevirt-private/9eae133d-5264-4040-9261-2ff5827e9540/virt-serial0-log' append='on'/>
      <target type='isa-serial' port='0'>
        <model name='isa-serial'/>
      </target>
      <alias name='serial0'/>
    </serial>
    <console type='unix'>
      <source mode='bind' path='/var/run/kubevirt-private/9eae133d-5264-4040-9261-2ff5827e9540/virt-serial0'/>
      <log file='/var/run/kubevirt-private/9eae133d-5264-4040-9261-2ff5827e9540/virt-serial0-log' append='on'/>
      <target type='serial' port='0'/>
      <alias name='serial0'/>
    </console>
    <channel type='unix'>
      <source mode='bind' path='/var/run/kubevirt-private/libvirt/qemu/channel/target/domain-1-default_ubuntu-amd/org.qemu.guest_agent.0'/>
      <target type='virtio' name='org.qemu.guest_agent.0' state='disconnected'/>
      <alias name='channel0'/>
      <address type='virtio-serial' controller='0' bus='0' port='1'/>
    </channel>
    <input type='tablet' bus='usb'>
      <alias name='ua-tablet1'/>
      <address type='usb' bus='0' port='1'/>
    </input>
    <input type='mouse' bus='ps2'>
      <alias name='input1'/>
    </input>
    <input type='keyboard' bus='ps2'>
      <alias name='input2'/>
    </input>
    <graphics type='vnc' socket='/var/run/kubevirt-private/9eae133d-5264-4040-9261-2ff5827e9540/virt-vnc'>
      <listen type='socket' socket='/var/run/kubevirt-private/9eae133d-5264-4040-9261-2ff5827e9540/virt-vnc'/>
    </graphics>
    <audio id='1' type='none'/>
    <video>
      <model type='vga' vram='16384' heads='1' primary='yes'/>
      <alias name='video0'/>
      <address type='pci' domain='0x0000' bus='0x00' slot='0x01' function='0x0'/>
    </video>
    <watchdog model='itco' action='reset'>
      <alias name='watchdog0'/>
    </watchdog>
    <memballoon model='virtio-non-transitional' freePageReporting='on'>
      <stats period='10'/>
      <alias name='balloon0'/>
      <address type='pci' domain='0x0000' bus='0x09' slot='0x00' function='0x0'/>
    </memballoon>
  </devices>
</domain>
```

## Snapshot & Clone



# Libvirtd

## virsh工具

###  Domain Management

```bash
bash-4.4# virsh list --all
 Id   Name           State
-------------------------------
 -    default_test   shut off

bash-4.4# virsh dumpxml default_test > /tmp/1.xml
bash-4.4# vi /tmp/1.xml

bash-4.4# virsh undefine default_test
Domain 'default_test' has been undefined

bash-4.4# virsh destory default_test

bash-4.4# virsh create /tmp/1.xml 
Domain 'default_test' created from /tmp/1.xml

bash-4.4# virsh list --all
 Id   Name           State
------------------------------
 18   default_test   running
```

###  Host and Hypervisor

```bash
# for x86_64
# virttype是kvm还是qemu取决于主机时候支持kvm，即/dev/kvm设备存在或者kvm驱动被加载（`grep kvm /proc/misc`）
$ virsh domcapabilities --machine q35 --arch x86_64 --virttype kvm/qemu


# for arm64
$ virsh domcapabilities --machine virt --arch aarch64 --virttype kvm/qemu
```



```bash

```



###  Domain Monitoring

> 下面的虚拟机domain `default_ubuntu`在使用dd命令写入数据的时候遇到了IO错误，使用的CSI为OpenEBS ZFS。

```bash
 Domain Monitoring (help keyword 'monitor')
    domblkerror                    Show errors on block devices
    domblkinfo                     domain block device size information
    domblklist                     list all domain blocks
    domblkstat                     get device block stats for a domain
    domcontrol                     domain control interface state
    domif-getlink                  get link state of a virtual interface
    domifaddr                      Get network interfaces' addresses for a running domain
    domiflist                      list all domain virtual interfaces
    domifstat                      get network interface stats for a domain
    dominfo                        domain information
    dommemstat                     get memory statistics for a domain
    domstate                       domain state
    domstats                       get statistics about one or multiple domains
    domtime                        domain time
    list                           list domains
```

```bash
sh-5.1$ virsh domblklist default_ubuntu
Authorization not available. Check if polkit service is running or see debug message for more information.
 Target   Source
--------------------------------------------------------------------------------
 vda      /var/run/kubevirt-private/vmi-disks/rootdisk/disk.img
 sda      /var/run/kubevirt-ephemeral-disks/disk-data/containerdisk/disk.qcow2

sh-5.1$ virsh domblkinfo default_ubuntu /var/run/kubevirt-private/vmi-disks/rootdisk/disk.img
Authorization not available. Check if polkit service is running or see debug message for more information.
Capacity:       26843283456
Allocation:     264245248
Physical:       18004852224

sh-5.1$ virsh domblkerror default_ubuntu                                                      
Authorization not available. Check if polkit service is running or see debug message for more information.
vda: unspecified error

sh-5.1$ virsh domblkstat default_ubuntu
Authorization not available. Check if polkit service is running or see debug message for more information.
 rd_req 122
 rd_bytes 1958400
 wr_req 0
 wr_bytes 0
 flush_operations 0
 rd_total_times 11479667
 wr_total_times 0
 flush_total_times 0
```



###  Domain Events

```bash
sh-5.1$ virsh event --list
Authorization not available. Check if polkit service is running or see debug message for more information.
lifecycle
reboot
rtc-change
watchdog
io-error
graphics
io-error-reason
control-error
block-job
disk-change
tray-change
pm-wakeup
pm-suspend
balloon-change
pm-suspend-disk
device-removed
block-job-2
tunable
agent-lifecycle
device-added
migration-iteration
job-completed
device-removal-failed
metadata-change
block-threshold
memory-failure
memory-device-size-change
```



```bash
# 这两操作都很慢
sh-5.1$ virsh event --event io-error       
Authorization not available. Check if polkit service is running or see debug message for more information.
event 'io-error' for domain 'default_ubuntu': /var/run/kubevirt-private/vmi-disks/rootdisk/disk.img (ua-rootdisk) pause
events received: 1
sh-5.1$ virsh event --event io-error-reason
Authorization not available. Check if polkit service is running or see debug message for more information.
event 'io-error' for domain 'default_ubuntu': /var/run/kubevirt-private/vmi-disks/rootdisk/disk.img (ua-rootdisk) pause due to 
events received: 1
```





# Qemu

## qemu-kvm

### cpu类型

```bash
# 查看qemu-kvm支持的CPU类型，下面是对Intel Xeon Processor的支持
bash-4.4# /usr/libexec/qemu-kvm -cpu help |grep Cascadelake-Server
x86 Cascadelake-Server    (alias configured by machine type)                        
x86 Cascadelake-Server-noTSX  (alias of Cascadelake-Server-v3)                          
x86 Cascadelake-Server-v1  Intel Xeon Processor (Cascadelake)                        
x86 Cascadelake-Server-v2  Intel Xeon Processor (Cascadelake) [ARCH_CAPABILITIES]    
x86 Cascadelake-Server-v3  Intel Xeon Processor (Cascadelake) [ARCH_CAPABILITIES, no TSX]
x86 Cascadelake-Server-v4  Intel Xeon Processor (Cascadelake) [ARCH_CAPABILITIES, no TSX]
x86 Cascadelake-Server-v5  Intel Xeon Processor (Cascadelake) [ARCH_CAPABILITIES, EPT switching, XSAVES, no TSX]
```



### machine类型

下面的实验基于kubevirt 0.48.1，可以看出arm节点仅支持`machine=virt`的虚拟机，而amd节点上主要支持`machine=q35`的虚拟机。

```bash
# Kylin v10 ( arm64, Kupeng 920)
(base) [root@node virt-handler]# kubectl exec -ti virt-launcher-kylinv10-fhwdj sh
bash-4.4# /usr/libexec/qemu-kvm -machine help
Supported machines are:
virt                 RHEL 8.5.0 ARM Virtual Machine (alias of virt-rhel8.5.0)
virt-rhel8.5.0       RHEL 8.5.0 ARM Virtual Machine (default)
virt-rhel8.4.0       RHEL 8.4.0 ARM Virtual Machine
virt-rhel8.3.0       RHEL 8.3.0 ARM Virtual Machine
virt-rhel8.2.0       RHEL 8.2.0 ARM Virtual Machine
none                 empty machine

# Kylin v5 ( amd64)
(base) [root@node virt-handler]# kubectl exec -ti virt-launcher-kylinv5-drdzq -- sh
sh-4.4#  /usr/libexec/qemu-kvm -machine help
Supported machines are:
pc-i440fx-4.2        Standard PC (i440FX + PIIX, 1996) (deprecated)
pc-i440fx-2.11       Standard PC (i440FX + PIIX, 1996) (deprecated)
pc                   RHEL 7.6.0 PC (i440FX + PIIX, 1996) (alias of pc-i440fx-rhel7.6.0)
pc-i440fx-rhel7.6.0  RHEL 7.6.0 PC (i440FX + PIIX, 1996) (default)
pc-i440fx-rhel7.5.0  RHEL 7.5.0 PC (i440FX + PIIX, 1996)
pc-i440fx-rhel7.4.0  RHEL 7.4.0 PC (i440FX + PIIX, 1996)
pc-i440fx-rhel7.3.0  RHEL 7.3.0 PC (i440FX + PIIX, 1996)
pc-i440fx-rhel7.2.0  RHEL 7.2.0 PC (i440FX + PIIX, 1996)
pc-i440fx-rhel7.1.0  RHEL 7.1.0 PC (i440FX + PIIX, 1996)
pc-i440fx-rhel7.0.0  RHEL 7.0.0 PC (i440FX + PIIX, 1996)
q35                  RHEL-8.5.0 PC (Q35 + ICH9, 2009) (alias of pc-q35-rhel8.5.0)
pc-q35-rhel8.5.0     RHEL-8.5.0 PC (Q35 + ICH9, 2009)
pc-q35-rhel8.4.0     RHEL-8.4.0 PC (Q35 + ICH9, 2009)
pc-q35-rhel8.3.0     RHEL-8.3.0 PC (Q35 + ICH9, 2009)
pc-q35-rhel8.2.0     RHEL-8.2.0 PC (Q35 + ICH9, 2009)
pc-q35-rhel8.1.0     RHEL-8.1.0 PC (Q35 + ICH9, 2009)
pc-q35-rhel8.0.0     RHEL-8.0.0 PC (Q35 + ICH9, 2009)
pc-q35-rhel7.6.0     RHEL-7.6.0 PC (Q35 + ICH9, 2009)
pc-q35-rhel7.5.0     RHEL-7.5.0 PC (Q35 + ICH9, 2009)
pc-q35-rhel7.4.0     RHEL-7.4.0 PC (Q35 + ICH9, 2009)
pc-q35-rhel7.3.0     RHEL-7.3.0 PC (Q35 + ICH9, 2009)
none                 empty machine
```

所以在arm64 + amd64异构集群中，如果virt-api pod运行在amd64节点上，在arm64节点上创建的虚拟机会报如下错误，因为针对kubevirt 0.48.1这个版本：x86版本的virt-api里支持的虚拟机machine类型只有`q35`；arm64版本的virt-api里支持的虚拟机machine类型只有`virt`。

```txt
command SyncVMI failed: "LibvirtError(Code=67, Domain=10, Message='unsupported configuration: Emulator '/usr/libexec/qemu-kvm' does not support machine type 'q35'')"
```

这是被 virt-api里的`**validateEmulatedMachine**`函数拦截了。参考`**NewClusterConfigWithCPUArch**`函数，emulatedMachines是和CPU架构相关的（cpuArch = runtime.GOARCH）。

```go
// getCPUArchSpecificDefault get arch specific default config
func getCPUArchSpecificDefault(cpuArch string) (string, string, []string) {
	// get arch specific default config
	switch cpuArch {
	case "arm64":
		emulatedMachinesDefault := strings.Split(DefaultAARCH64EmulatedMachines, ",")
		return DefaultAARCH64OVMFPath, DefaultAARCH64MachineType, emulatedMachinesDefault
	case "ppc64le":
		emulatedMachinesDefault := strings.Split(DefaultPPC64LEEmulatedMachines, ",")
		return DefaultARCHOVMFPath, DefaultPPC64LEMachineType, emulatedMachinesDefault
	default:
		emulatedMachinesDefault := strings.Split(DefaultAMD64EmulatedMachines, ",")
		return DefaultARCHOVMFPath, DefaultAMD64MachineType, emulatedMachinesDefault
	}
}
```

 需要升级到kubevit 1.0 RC1之后的版本并开启`MultiArchitecture`特性解决此问题。

## 磁盘格式raw & qcow2

安装相关工具包：

```bash
root@master1:~# apt-get install qemu-utils
```

### 检测虚磁盘文件格式

```bash
root@master1:~# qemu-img info /root/disk.img
image: /root/disk.img
file format: raw
virtual size: 25 GiB (26843545600 bytes)
disk size: 25 GiB
```

### 挂载/卸载磁盘文件

```bash
# 加载nbd内核模块
root@master1:~# modprobe nbd max_part=8

# 将raw格式磁盘连接到nbd设备
root@master1:~# qemu-nbd --connect=/dev/nbd0 /root/disk.img 
WARNING: Image format was not specified for '/root/disk.img' and probing guessed raw.
         Automatically detecting the format is dangerous for raw images, write operations on block 0 will be restricted.
         Specify the 'raw' format explicitly to remove the restrictions.

# 将qcow2格式磁盘连接到nbd设备
root@master1:~# qemu-nbd --connect=/dev/nbd1 /root/disk.qcow2 

# 连接成功后的nbd设备就是一块盘，fdisk可以看到其中的分区
root@master1:~# fdisk -l /dev/nbd0
Disk /dev/nbd0: 25 GiB, 26843545600 bytes, 52428800 sectors
Units: sectors of 1 * 512 = 512 bytes
Sector size (logical/physical): 512 bytes / 512 bytes
I/O size (minimum/optimal): 512 bytes / 512 bytes
Disklabel type: gpt
Disk identifier: 0917EBBB-5A4F-4874-93B6-8D55D533BA08

Device        Start      End  Sectors Size Type
/dev/nbd0p1    2048     4095     2048   1M BIOS boot
/dev/nbd0p2    4096  2101247  2097152   1G Linux filesystem # boot分区，可以直接挂载
/dev/nbd0p3 2101248 52426751 50325504  24G Linux filesystem # LVM PV，不能直接挂载

# 断开nbd设备
root@master1:~# qemu-nbd --disconnect /dev/nbd0
/dev/nbd0 disconnected

# 卸载nbd模块 - optional
root@master1:~# modprobe -r nbd
```

### 磁盘格式转换

将raw格式磁盘转换为qcow2格式：

```bash
root@master1:~# qemu-img convert -f raw -O qcow2 /root/disk.img /root/disk.qcow2
root@master1:~# qemu-img info /root/disk.qcow2 
image: /root/disk.qcow2
file format: qcow2
virtual size: 25 GiB (26843545600 bytes)
disk size: 2.46 GiB
cluster_size: 65536
Format specific information:
    compat: 1.1
    lazy refcounts: false
    refcount bits: 16
    corrupt: false
```

将qcow2格式磁盘转换为raw格式：

```bash
root@master1:~# qemu-img convert -f qcow2 -O raw /root/disk.qcow2 /root/disk.img
```

qcow2是压缩格式，磁盘文件比较小，而raw格式一次性分配所需磁盘空间。如果使用qcow2格式磁盘文件，但是给qemu的参数格式是raw，就会导致虚拟机的实际可用空间很小。





