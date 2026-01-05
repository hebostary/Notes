# Kubernetes 核心组件

## kube-scheduler*

### 启动过程

```go
//cmd/kube-scheduler/app/server.go:runCommand() 
```

1. 初始化进程级别的context，注册信号处理函数保证进程优雅退出。
2. 读取配置文件和命令行参数，初始化配置，还包括创建：informerFactory（pod informer）、EventBroadcaster。

```go
//cmd/kube-scheduler/app/config/config.go
type Config struct {
    //...
	// LeaderElection is optional. 单master部署可以禁用leader选举。该配置包含leader选举的一些关键信息，比如
    // Identity：节点id，格式：hostname_<UUID>
    // LeaseDuration：租约过期时间，默认为net.defaultTCPKeepAliveIdle (15000000000)
    // RenewDeadline：租约renew截止时间，默认为k8s.io/kubernetes/pkg/scheduler/internal/queue.DefaultPodMaxBackoffDuration (10000000000)
    // ResourceLock=leases, ResourceName=kube-scheduler, ResourceNamespace=kube-system, 
	LeaderElection *leaderelection.LeaderElectionConfig

	// PodMaxInUnschedulablePodsDuration is the maximum time a pod can stay in
	// unschedulablePods. If a pod stays in unschedulablePods for longer than this
	// value, the pod will be moved from unschedulablePods to backoffQ or activeQ.
	// If this value is empty, the default value (5min) will be used.
	PodMaxInUnschedulablePodsDuration time.Duration
}
```

3. 根据配置创建`Scheduler`对象

   ```go
   //pkg/scheduler/scheduler.go:New()
   ```

4. 启动事件处理管道

   ```go
   //cc.EventBroadcaster.StartRecordingToSink()
   ```

5. 等待informer本地缓存同步完成。

6. 如果没有配置Leader选举则直接启动调度器，否则（多master集群）就注册leader选举回调函数并在成功的回调函数里启动调度器。选举过程就是后面介绍的 `leaderelection`包提供的基础功能，选举失败或者成功后租约自动续租超时，进程直接退出。我们在调试kube-scheduler的时候可以添加参数 `--leader-elect=false` 来关闭leader 选举，避免调试过程中进程暂停而退出。

   ```go
   //pkg/scheduler/scheduler.go:Run()
   // 运行调度器的核心就是启动独立协程去维护SchedulingQueue，然后再启动独立协程循环执行Scheduler.scheduleOne()函数
   //pkg/scheduler/internal/queue/scheduling_queue.go:Run()
   ```

### 调度器对象（Scheduler）

`Scheduler` 是 kube-scheduler 的核心框架和数据对象，用于驱动整个调度系统的运转，下面是它的核心数据：

```go
// Scheduler watches for new unscheduled pods. It attempts to find
// nodes that they fit on and writes bindings back to the api server.
type Scheduler struct {
	// It is expected that changes made via Cache will be observed
	// by NodeLister and Algorithm.
	Cache internalcache.Cache
	// NextPod should be a function that blocks until the next pod
	// is available. We don't use a channel for this, because scheduling
	// a pod may take some amount of time and we don't want pods to get
	// stale while they sit in a channel.
	NextPod func(logger klog.Logger) (*framework.QueuedPodInfo, error)
	// FailureHandler is called upon a scheduling failure.
	FailureHandler FailureHandlerFn
	// SchedulePod tries to schedule the given pod to one of the nodes in the node list.
	// Return a struct of ScheduleResult with the name of suggested host on success,
	// otherwise will return a FitError with reasons.
	SchedulePod func(ctx context.Context, fwk framework.Framework, state *framework.CycleState, pod *v1.Pod) (ScheduleResult, error)
	StopEverything <-chan struct{} // 优雅关闭
	// 调度队列保存等待调度的pod，目前使用的是基于两个heap（activeQ和podBackoffQ）的优先级队列
	SchedulingQueue internalqueue.SchedulingQueue
	Profiles profile.Map // 维护了一个基于调度器名称和调度框架实例的map：map[string]framework.Framework
	nodeInfoSnapshot *internalcache.Snapshot
	percentageOfNodesToScore int32
	nextStartNodeIndex int
}
```

#### scheduleOne()

```go
//pkg/scheduler/schedule_one.go 
```

1. 调用 `SchedulingQueue.Pop()` 从优先级队列中获取下一个等待处理的pod，实际上是从 `activeQ` 中获取堆顶部的pod。
2. 根据 `pod.Spec.SchedulerName`  从 `Scheduler.Profiles` 中获取对应的调度框架，即后面的 `fwk`。
3. `skipPodSchedule()` - 检查需要跳过调度的特殊场景：（1）pod 已经被加上了删除时间戳；（2）pod 已经是 `assumed` 状态，即 pod 已经有了调度结果`SuggestedHost`并且在缓存中记录，只是还没有完成异步绑定周期（`bindCycle`）。这个 `assumed` 状态的设计主要是为了将 `schedule` 和 `bind`两个调度周期解耦，这样调度器可以快速执行下一个 pod 的调度。
4. `schedulingCycle()` - 核心调度逻辑：
   1. `schedulePod()` - 尝试将 pod 调度到集群的某个节点上，返回的调度结果包含建议运行 pod 的节点（`SuggestedHost`）。
      1. 先对整个调度器缓存加锁，获取节点信息（`NodeInfo`）缓存的快照，即在核心调度逻辑执行期间使用的节点信息是不会变更的。`NodeInfo`缓存里保存了所有节点上：已有的 pod、带亲和性的 pod、带反亲和性的 pod、节点使用的 `pvc`等信息。快照里没有任何节点就直接报错了：`"no nodes available to schedule pods`。
      2. `fwk.RunPreFilterPlugins()` - **预选**，依次运行 *prefilter* 插件（`framework.PreFilterPlugin`，每个插件实现 `PreFilter` 接口，默认12个），这些插件针对 pod 过滤出符合相关资源需求或者亲和性的节点列表，所以每个插件都会过滤出0或多个符合要求的节点，然后依次对这些节点列表取交集得到预选节点列表。目前的 *prefilter* 插件包括（`pkg/scheduler/framework/plugins/`）：
         1. `VolumeBinding` - 卷绑定插件，这个插件完成节点过滤后，还会将卷绑定需求写入`framework.CycleState`。 
         2. `NodeAffinity` - 节点亲和性插件
         3. `CSILimits`
         4. ...

      3. `findNodesThatPassFilters()` - 针对预选得到的每个节点开启并发任务执行`fwk.RunFilterPlugins()`，即依次调用框架里的 filter 插件（`framework.FilterPlugin`，每个插件都实现一个 Filter 函数，默认15个）的 Filter 函数，插件执行成功的节点加入`feasibleNodes`成为备选，插件运行失败的节点则被过滤掉。具体的 filter 插件包括：
         1. 

      4. `findNodesThatPassExtenders()` - 这个阶段会再对`feasibleNodes`进行一次过滤，某些节点可能会被踢出去。默认调度器没有 extender。
      5. 至此，剩下的这些`feasibleNodes`客观上都满足运行 pod 的硬性需求。如果没有备选节点或者只剩下一个节点，都直接返回了。否则，继续进行节点优先级处理，即**优选**。
      6. `fwk.RunPreScorePlugins()`
      7. `fwk.RunScorePlugins()`
   2. 更新 pod 的 `assumed` 状态：拷贝 pod 对象副本后将 `pod.spec.nodeName`设置为 `SuggestedHost` ，然后更新调度器缓存，包括记录 assumedpod 的集合（基于map实现的）和节点的 pod 列表缓存（LRU）。
   3. `fwk.RunReservePluginsReserve()` - 运行调度框架注册的 reserve 插件？
   4. `fwk.RunPermitPlugins()` - 运行调度框架注册的 *permit* 插件，每个插件主要实现一个 `Permit` 接口。任何一个 *permit* 插件返回状态为 rejected 或失败则调度直接失败；如果有任何插件返回 Wait 表示 pod 需要延迟绑定，这种情况也算作调度成功，但是会创建一个 waitingPod 并把所有需要 wait 的插件都加入 waitingPod  的 map 中，每个插件有自己的超时时间（max：15 分钟），最后把 waitingPod 加入调度框架的 waitingPods map中。
   5. 返回调度结果。如果调度失败调用 `handleSchedulingFailure()` 执行一些错误处理，包括产生事件，更新 pod 状态里的 condition。
5. `bindingCycle()`  - 异步绑定周期。
   1. `fwk.WaitOnPermit()`
   2. `fwk.RunPreBindPlugins()`
   3. `bind()`
   4. `fwk.RunPostBindPlugins()`


#### SchedulingQueue



### 调度器框架（Framework）

```go
//pkg/scheduler/profile/profile.go

// Map holds frameworks indexed by scheduler name.
type Map map[string]framework.Framework

// 根据调度器profile初始化调度器map，map的key就是调度器名称，value（framework.Framework）则是调度器的实现
// NewMap builds the frameworks given by the configuration, indexed by name.
func NewMap(ctx context.Context, cfgs []config.KubeSchedulerProfile, r frameworkruntime.Registry, recorderFact RecorderFactory,
	opts ...frameworkruntime.Option) (Map, error) {
	m := make(Map)
	v := cfgValidator{m: m}

	for _, cfg := range cfgs {
		p, err := newProfile(ctx, cfg, r, recorderFact, opts...)
		if err != nil {
			return nil, fmt.Errorf("creating profile for scheduler name %s: %v", cfg.SchedulerName, err)
		}
		if err := v.validate(cfg, p); err != nil {
			return nil, err
		}
		m[cfg.SchedulerName] = p
	}
	return m, nil
}
```



```go
//pkg/scheduler/framework/runtime/framework.go
type frameworkImpl struct {
	registry             Registry
	snapshotSharedLister framework.SharedLister
	waitingPods          *waitingPodsMap
	scorePluginWeight    map[string]int
	preEnqueuePlugins    []framework.PreEnqueuePlugin
	enqueueExtensions    []framework.EnqueueExtensions
	queueSortPlugins     []framework.QueueSortPlugin
	preFilterPlugins     []framework.PreFilterPlugin
	filterPlugins        []framework.FilterPlugin
	postFilterPlugins    []framework.PostFilterPlugin
	preScorePlugins      []framework.PreScorePlugin
	scorePlugins         []framework.ScorePlugin
	reservePlugins       []framework.ReservePlugin
	preBindPlugins       []framework.PreBindPlugin
	bindPlugins          []framework.BindPlugin
	postBindPlugins      []framework.PostBindPlugin
	permitPlugins        []framework.PermitPlugin

	clientSet       clientset.Interface
	kubeConfig      *restclient.Config
	eventRecorder   events.EventRecorder
	informerFactory informers.SharedInformerFactory
	logger          klog.Logger

	metricsRecorder          *metrics.MetricAsyncRecorder
	profileName              string
	percentageOfNodesToScore *int32

	extenders []framework.Extender
	framework.PodNominator

	parallelizer parallelize.Parallelizer
}

// Registry is a collection of all available plugins. The framework uses a
// registry to enable and initialize configured plugins.
// All plugins must be in the registry before initializing the framework.
type Registry map[string]PluginFactory

// NewFramework initializes plugins given the configuration and the registry.
// 包括初始化调度器的插件
func NewFramework(ctx context.Context, r Registry, profile *config.KubeSchedulerProfile, opts ...Option) (framework.Framework, error) {
    //...
}
```



#### volumebinding plugin



### 调度器缓存

```go
//pkg/scheduler/internal/cache/interface.go
```

#### 节点信息缓存及快照



## kube-controller-manager

### HorizontalPodAutoscalerController

### PersistentVolumeController

### PersistentVolumeBinderController

### EndpointsController





## kubelet*

### volumemanager

Volume Manager协程每100毫秒执行一次`reconcile()`来处理pod volume的挂载/卸载等需求。

```go
// reconcilerLoopSleepPeriod = 100 * time.Millisecond

func (rc *reconciler) runOld(stopCh <-chan struct{}) {
	wait.Until(rc.reconciliationLoopFunc(), rc.loopSleepDuration, stopCh)
}

func (rc *reconciler) reconciliationLoopFunc() func() {
	return func() {
		rc.reconcile()

		// Sync the state with the reality once after all existing pods are added to the desired state from all sources.
		// Otherwise, the reconstruct process may clean up pods' volumes that are still in use because
		// desired state of world does not contain a complete list of pods.
		if rc.populatorHasAddedPods() && !rc.StatesHasBeenSynced() {
			klog.InfoS("Reconciler: start to sync state")
			rc.sync()
		}
	}
}
```







### cAdvisor

以CgroupV1为例，下面命令可以获取nginx pod的uid和容器id，其组成的cgroupfs id就是`kubepods.slice/kubepods-pod0a2d3fc7_592e_46b5_8d02_a23af4b3de99.slice/docker-66a9f16df3c0d338394f80bfa8ee28561f64496541edcdbc5d021e17dfba1ec3.scope`。

```bash
[root@master ~]# kubectl get pod nginx2-65744b8455-lqhkt -o yaml | egrep "uid:|containerID:"
    uid: 3f97f57d-e006-4ffe-8094-eefdeb32ad53 # deployment uid
  uid: 0a2d3fc7-592e-46b5-8d02-a23af4b3de99 # pod uid
  - containerID: docker://66a9f16df3c0d338394f80bfa8ee28561f64496541edcdbc5d021e17dfba1ec3 # docker id
```

`container_fs_writes_bytes_total`这个metric的数据来源就是cAdvisor读取的blkio cgroupfs子系统里的如下文件里的`Write`数据：

>  blkio cgroupfs是块设备层的IO统计数据，我们在容器内向文件系统写入数据时，默认都是write back机制（操作系统异步地将数据落盘），所以磁盘写（Write/Sync）数据几乎不变，Async会发生变化。如果应用程序设置写模式是`O_DIRECT`，Write/Sync会立即发生变化，并且新增数据量和实际写入一致。

```bash
cat /sys/fs/cgroup/blkio/kubepods.slice/kubepods-pod0a2d3fc7_592e_46b5_8d02_a23af4b3de99.slice/docker-66a9f16df3c0d338394f80bfa8ee28561f64496541edcdbc5d021e17dfba1ec3.scope/blkio.throttle.io_service_bytes
8:0 Read 9430933504 # 从设备读取的字节数
8:0 Write 0 # 向设备写入的字节数
8:0 Sync 0 #同步IO字节数
8:0 Async 9430933504 #异步IO字节数
8:0 Total 9430933504 #总字节数：Read + Write
253:0 Read 9430933504
253:0 Write 1101824
253:0 Sync 1101824
253:0 Async 9430933504
253:0 Total 9432035328
Total 18862968832
```

其中，8:0和253:0是块设备id，可通过`lsblk`命令查看：

```bash
[root@master ~]# lsblk 
NAME            MAJ:MIN RM   SIZE RO TYPE MOUNTPOINT
sda               8:0    0 931.5G  0 disk 
├─sda1            8:1    0   200M  0 part /boot/efi
├─sda2            8:2    0     1G  0 part /boot
├─sda3            8:3    0   500G  0 part 
│ └─centos-root 253:0    0   500G  0 lvm  /
```

## kube-proxy





# Kubernetes SDK

## client-go

### tools

#### cache

#### leaderelection

leaderelection是k8s提供的一个客户端包，用于在集群中多个服务实例中选举leader，目前主要是`kube-scheduler`和`kube-controller-manager`内部的`PersistentVolumeExpanderController`在使用。核心数据结构是下面的`LeaderElector strcut`，可以在LeaderElector.config中注册两个回调函数：

1. OnStartedLeading：成为leader时调用的回调函数，例如kube-scheduler开启调度作业。
2. OnStoppedLeading：leader选举失败或者从leader降级时调用的回调函数。

```go
// LeaderElector is a leader election client.
type LeaderElector struct {
	config LeaderElectionConfig
	// internal bookkeeping
	observedRecord    rl.LeaderElectionRecord
	observedRawRecord []byte
	observedTime      time.Time
	// used to implement OnNewLeader(), may lag slightly from the
	// value observedRecord.HolderIdentity if the transition has
	// not yet been reported.
	reportedLeader string

	// clock is wrapper around time to allow for less flaky testing
	clock clock.Clock

	// used to lock the observedRecord
	observedRecordLock sync.Mutex

	metrics leaderMetricsAdapter
}
```

```go
// Run starts the leader election loop. Run will not return
// before leader election loop is stopped by ctx or it has
// stopped holding the leader lease
func (le *LeaderElector) Run(ctx context.Context) {
	defer runtime.HandleCrash()
    //竞选失败或者renew失败，程序重启后可以再次参与竞选leader
	defer le.config.Callbacks.OnStoppedLeading()

    // 不断尝试竞选leader，即获取租约资源（指定namespace里的Lease对象）
    // 如果该Lease对象不存在、或者HolderIdentity字段为空、或者已经过期，则可以申请成功。
	if !le.acquire(ctx) {
		return // ctx signalled done
	}
	ctx, cancel := context.WithCancel(ctx)
	defer cancel()
    // 竞选leader成功，调用回调函数执行leader业务
	go le.config.Callbacks.OnStartedLeading(ctx)
    // 周期性地对Lease对象进行renew，一旦超时没有进行renew或者renew失败则会被其它节点抢占。
	le.renew(ctx)
}
```

如果renew失败还会对Lease执行release操作，即将Lease对象里的`HolderIdentity`字段清空。

Kubernetes `kube-system/kube-scheduler`Lease对象详情如下：

```yaml
root@master:~# kubectl get lease -n kube-system       kube-scheduler -o yaml
apiVersion: coordination.k8s.io/v1
kind: Lease
metadata:
  creationTimestamp: "2024-12-26T01:35:08Z"
  name: kube-scheduler
  namespace: kube-system
  resourceVersion: "12512359"
  uid: 0f33cce9-51a8-44d7-8b87-51e85b2fdf4c
spec:
  acquireTime: "2025-02-06T09:12:24.561243Z"
  holderIdentity: master_9ca3a9c7-610d-4b47-91bb-2d916d1307ed
  leaseDurationSeconds: 15
  leaseTransitions: 10
  renewTime: "2025-02-06T11:40:57.200780Z"
```

### utils

#### workqueue

这个包提供了一些工作队列的封装，包括并发执行任务的 ParallelizeUntil。

```go
//vendor/k8s.io/client-go/util/workqueue/parallelizer.go
```





