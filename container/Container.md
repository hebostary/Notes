#  Linux容器底层技术

## Namespace

### Mount Namespace

对于最原始的Linux容器，即使开启了 Mount Namespace，容器进程看到的文件系统也跟宿主机完全一样。因为Mount Namespace 修改的，是容器进程对文件系统“挂载点”的认知。这也就意味着，只有在“挂载”这个操作发生之后，进程的文件系统视图才会被改变。而在此之前，新创建的容器会直接继承宿主机的各个挂载点。

比如，默认地，在容器内部执行`ls /tmp`命令看到的内容和在宿主机上执行该命令看到的结果是一样的。如果在容器内部重新挂载/tmp目录，那么看到的`/tmp`目录将不再是宿主机上的`/tmp`目录。比如下面的例子，我们用`clone`这个系统调用来创建新的容器，`CLONE_NEWNS`参数表示开启了Mount Namespace。并在容器内部，以 tmpfs（内存盘）格式，重新挂载了 `/tmp` 目录。

```c
#define STACK_SIZE (1024 * 1024)
static char container_stack[STACK_SIZE];
char* const container_args[] = { "/bin/bash", NULL};

int container_main(void* arg)
{
  printf("Container - inside the container!\n");
  // 如果你的机器的根目录的挂载类型是shared，那必须先重新挂载根目录
  // mount("", "/", NULL, MS_PRIVATE, "");
  mount("none", "/tmp", "tmpfs", 0, "");
  execv(container_args[0], container_args);
  printf("Something's wrong!\n");
  return 1;
}

int main()
{ 
    printf("Parent - start a container!\n"); 
    int container_pid = clone(container_main, container_stack+STACK_SIZE, CLONE_NEWNS | SIGCHLD , NULL); 
    waitpid(container_pid, NULL, 0); 
    printf("Parent - container stopped!\n"); 
    return 0;
}
```

这个“挂载”只在容器内可见，在宿主上是不可见的。

```bash
# 在宿主机上
$ mount -l | grep tmpfs
```

> 这就是 Mount Namespace 跟其他 Namespace 的使用略有不同的地方：它对容器进程视图的改变，一定是伴随着挂载操作（mount）才能生效。

## Rootfs

容器的rootfs由如下图所示的三部分组成：

![img](https://hunk-pic-store.oss-cn-beijing.aliyuncs.com/img/20211007111913.png)

1. 只读层：它是这个容器的 rootfs 最下面的五层，对应的正是 ubuntu:latest 镜像的五层。可以看到，它们的挂载方式都是只读的（ro+wh，即 readonly+whiteout）。
2. 可读写层：
3. Init层：





## Cgroups

`Cgroups`（Control groups）是Linux内核的一个功能， 用于将进程组织成有层次结构的group，而这个group对于各种系统资源（CPU、Memory...）的使用情况可以被监控和限制。所以，本质上cgroup是一个基于进程组模型来管理系统资源的机制。内核里的Cgroups的接口通过一个叫做`cgroupfs`的文件系统来提供。

Cgroups的几个基本概念：

1. `cgroup`：`cgroup`是一个进程的集合，这些进程绑定到一组通过`cgroupfs`定义的限制或参数。
2. `subsystem（子系统）`：子系统是一个内核组件，它可以修改`cgroup`中的进程的行为。不同的子系统实现可以完成比如限制`cgroup`可用的CPU时间和内存数量、统计`cgroup`使用的CPU时间以及冻结和恢复`cgroup`中进程的执行等工作。子系统有时候也称为`resource controller（资源控制器）`或者`cgroup controller`。
3. `hierarchy（层次结构）`：一个子系统或者控制器的所有cgroups按照hierarchy排列，这个层次结构通过创建、删除和重命名`cgroupfs`中的子系统来定义。在层次结构的每一层，都可以定义属性(例如，限制)。cgroup提供的限制、控制和计算通常对cgroup下面的子层次结构的相关属性也有影响。例如，在高级别层次结构上对cgroup的限制不能被低级别的cgroup所超过。

### Cgroups v1 and Cgroups v2

Cgroups的初始实现版本从Linux  2.6.24引入。随着时间的发展，越来越多的cgroup控制器被添加进内核用于管理不同类型的资源，但是这些cgroup控制器的开发在很大程度上是不协调的，结果是cgroup控制器之间产生了许多不一致。从Linux 3.10便开始了新工作去解决Cgroup v1的问题，Cgroup v2在Linux 4.5正式发布。

Cgroups v1控制器中存在的`cgroup.sane_behavior`文件是挂载选项的遗物，这个文件总是报告"0"，主要是为了向后兼容而保留。

虽然Cgroups v2是Cgroups v1的替代品，但是旧的系统继续存在（由于兼容性原因不太可能被移除)。目前，Cgroups v2只实现了Cgroups v1中可用控制器的一个子集。Cgroups v1控制器和Cgroups v2控制器可以挂载在同一个系统上。所以，可以在使用Cgroups v2控制器时，同时也使用那些Cgroups v2还没有实现的Cgroups v1控制器，这里唯一的限制是控制器不能同时在Cgroups v1层次结构和Cgroups v2层次结构中使用，即一个cgroup控制器关联的所有cgroup应该是同一个版本。

### CGROUPS VERSION 1 

在Cgroups v1下，每个控制器都可以挂载到一个单独的cgroup文件系统上，该文件系统为系统上的进程提供了自己的层次结构。也有可能对同一个cgroup文件系统合并挂载（comount）多个(甚至所有)cgroup v1控制器，这意味着合并挂载的控制器管理相同的进程层次组织。

对于每个挂载的层次结构，目录树反映了控制组层次结构。每个cgroup由一个目录表示，其每个child cgroup表示为一个子目录。

####  Cgroups  v1控制器

每个Cgroups v1控制器都由一个内核配置选项管理(如下所列)。此外，cgroups特性的可用性由`CONFIG_CGROUPS`内核配置选项管理。

`/sys/fs/cgroup`下面的这些子目录实际上就是cgroup控制器的用户视图，关于每个cgroup控制器的具体作用参考[Cgroups version 1 controllers](https://www.man7.org/linux/man-pages/man7/cgroups.7.html#DESCRIPTION)。

```bash
$ ls -al /sys/fs/cgroup
total 0
drwxr-xr-x. 14 root root 360 Apr 13 16:15 .
drwxr-xr-x.  8 root root   0 Apr 13 16:15 ..
dr-xr-xr-x.  6 root root   0 Apr 13 16:15 blkio
lrwxrwxrwx.  1 root root  11 Apr 13 16:15 cpu -> cpu,cpuacct
lrwxrwxrwx.  1 root root  11 Apr 13 16:15 cpuacct -> cpu,cpuacct
dr-xr-xr-x.  6 root root   0 Apr 13 16:15 cpu,cpuacct
dr-xr-xr-x.  3 root root   0 Apr 13 16:15 cpuset
dr-xr-xr-x.  6 root root   0 Apr 13 16:15 devices
dr-xr-xr-x.  3 root root   0 Apr 13 16:15 freezer
dr-xr-xr-x.  3 root root   0 Apr 13 16:15 hugetlb
dr-xr-xr-x.  6 root root   0 Apr 13 16:15 memory
lrwxrwxrwx.  1 root root  16 Apr 13 16:15 net_cls -> net_cls,net_prio
dr-xr-xr-x.  3 root root   0 Apr 13 16:15 net_cls,net_prio
lrwxrwxrwx.  1 root root  16 Apr 13 16:15 net_prio -> net_cls,net_prio
dr-xr-xr-x.  3 root root   0 Apr 13 16:15 perf_event
dr-xr-xr-x.  6 root root   0 Apr 13 16:15 pids
dr-xr-xr-x.  2 root root   0 Apr 13 16:15 rdma
dr-xr-xr-x.  6 root root   0 Apr 13 16:15 systemd
```

挂载Cgroups v1控制器：

```bash
# 挂载cpu控制器
$ mount -t cgroup -o cpu none /sys/fs/cgroup/cpu

#合并挂载cpu和cpuacct两个控制器
$ mount -t cgroup -o cpu,cpuacct none /sys/fs/cgroup/cpu,cpuacct

#合并挂载所有的控制器
$ mount -t cgroup -o all cgroup /sys/fs/cgroup
```

#### 管理cgroups

一个cgroup文件系统包含单个root cgroup，即层次结构树的根`'/'`，所有的进程都属于这个cgroup。创建新的cgroup的操作就是在cgroup文件系统下创建目录，控制器定义的属性文件也会自动生成。

```bash
# 创建新的cgroup
$ mkdir /sys/fs/cgroup/cpu/cg1
$ ls -al /sys/fs/cgroup/cpu/cg1
total 0
drwxr-xr-x. 2 root root 0 May 30 16:50 .
dr-xr-xr-x. 7 root root 0 Apr 13 16:15 ..
-rw-r--r--. 1 root root 0 May 30 16:50 cgroup.clone_children
-rw-r--r--. 1 root root 0 May 30 16:50 cgroup.procs
-r--r--r--. 1 root root 0 May 30 16:50 cpuacct.stat
-rw-r--r--. 1 root root 0 May 30 16:50 cpuacct.usage
-r--r--r--. 1 root root 0 May 30 16:50 cpuacct.usage_all
-r--r--r--. 1 root root 0 May 30 16:50 cpuacct.usage_percpu
-r--r--r--. 1 root root 0 May 30 16:50 cpuacct.usage_percpu_sys
-r--r--r--. 1 root root 0 May 30 16:50 cpuacct.usage_percpu_user
-r--r--r--. 1 root root 0 May 30 16:50 cpuacct.usage_sys
-r--r--r--. 1 root root 0 May 30 16:50 cpuacct.usage_user
-rw-r--r--. 1 root root 0 May 30 16:50 cpu.cfs_period_us
-rw-r--r--. 1 root root 0 May 30 16:50 cpu.cfs_quota_us
-rw-r--r--. 1 root root 0 May 30 16:50 cpu.rt_period_us
-rw-r--r--. 1 root root 0 May 30 16:50 cpu.rt_runtime_us
-rw-r--r--. 1 root root 0 May 30 16:50 cpu.shares
-r--r--r--. 1 root root 0 May 30 16:50 cpu.stat
-rw-r--r--. 1 root root 0 May 30 16:50 notify_on_release
-rw-r--r--. 1 root root 0 May 30 16:50 tasks

# 移动进程到到新建的cgroup中
# 	1. 一次应该只写入一个进程ID到这个文件里
# 	2. 写入 0 会把执行写入的进程移动到关联的cgroup中
#   3. 当写入进程ID到这个文件的时候，进程的所有线程也会被移动到关联的cgroup中
$ cat /sys/fs/cgroup/cpu/cg1/cgroup.procs
$ echo $$
2569946
$ cat /sys/fs/cgroup/cpu/cgroup.procs |grep 2569946
$ echo $$ > /sys/fs/cgroup/cpu/cgroup.procs  # 先将当前进程ID写入root cgroup
$ cat /sys/fs/cgroup/cpu/cgroup.procs |grep 2569946
2569946
$ echo $$ > /sys/fs/cgroup/cpu/cg1/cgroup.procs   # 将当前进程ID移动到cg1 cgroup
$ cat /sys/fs/cgroup/cpu/cgroup.procs |grep 2569946
$ cat /sys/fs/cgroup/cpu/cg1/cgroup.procs
2569946
2570900
```

用`rmdir`命令删除关联的目录就可以删除cgroup（cgroup目录下那些文件不需要也不能被手动删除），但是有一些约束条件：

1. 该cgroup没有child cgroup。
2. 该cgroup不包含进程。

```bash
$ rmdir /sys/fs/cgroup/cpu/cg1
rmdir: failed to remove '/sys/fs/cgroup/cpu/cg1': Device or resource busy
$ cat /sys/fs/cgroup/cpu/cg1/cgroup.procs
2569946
2573272
$ echo $$ > /sys/fs/cgroup/cpu/cgroup.procs
$ cat /sys/fs/cgroup/cpu/cg1/cgroup.procs
$ rmdir /sys/fs/cgroup/cpu/cg1
$ echo $?
0
```

### Cgroups & systemd

总是基于进程ID去控制资源分配会面临很多问题，比如进程因为异常重启就需要更新`cgroup.procs`文件里的进程ID。在redhat/centos系统中，通过将cgroup层次结构域systemd单元树捆绑，可以把资源管理从进程级别平移到应用程序级别。

默认地，systemd会自动创建`slice`、`scope`和`service`单位的层级，来为cgroup树提供统一层次结构。如下图所示，如果我们将所有系统资源看出一块馅饼，那么每一种资源默认会被划分成3个cgroup：`system`、`user`和`machine`，每个cgroup都是一个slice，每个slice都可以有自己的子slice。

![slice](https://jsdelivr.icloudnative.io/gh/yangchuansheng/imghosting@master/img/20200723163321.jpg)

```bash
$ ls -al /sys/fs/cgroup/cpu/ |grep slice
drwxr-xr-x.  6 root root   0 May  4 16:24 machine.slice
drwxr-xr-x. 92 root root   0 May 23 14:25 system.slice
drwxr-xr-x.  2 root root   0 May 23 14:46 user.slice
$ ls -al /sys/fs/cgroup/memory/ |grep slice
drwxr-xr-x.  6 root root   0 May  4 16:24 machine.slice
drwxr-xr-x. 92 root root   0 May 23 14:25 system.slice
drwxr-xr-x.  3 root root   0 May 23 14:46 user.slice
```

以CPU资源为例，系统默认创建了3个顶级slice，每个slice都会获得相同的CPU使用时间（仅在CPU繁忙时生效），这3个顶级slice的含义如下：

1. `system.slice`：所有由systemd管理的系统service的默认cgroup，所有启动的systemd服务都会自动创建关联的child cgroup，比如`/sys/fs/cgroup/memory/system.slice/crond.service`；当这个服务被stop后，这个child cgroup就会被自动删除。
2. `user.slice`：所有用户session的默认cgroup。每个用户session都会在user.slice下面创建一个基于用户ID的子slice，如果同一个用户多次登录系统，它们使用同一个子slice。
3. `machine.slice`：所有虚拟机和Linux容器的默认cgroup。

关于更多细节参考：[Cgroup 如何对 CPU 资源进行控制？](https://icloudnative.io/posts/understanding-cgroups-part-1-basics/)

[第 26 章 使用带有 systemd 的控制组群版本 1](https://access.redhat.com/documentation/zh-cn/red_hat_enterprise_linux/8/html/managing_monitoring_and_updating_the_kernel/using-control-groups-version-1-with-systemd_managing-monitoring-and-updating-the-kernel)

### CGROUPS VERSION 1 Demos 

#### 限制用户内存使用

Demo环境信息：

```bash
$ cat /etc/redhat-release 
Red Hat Enterprise Linux release 8.6 (Ootpa)
$ uname -r
4.18.0-372.9.1.el8.x86_64
```

在做这个实验之前，先临时禁用内存swap：

```bash
$ cat /proc/swaps
Filename                                Type            Size            Used            Priority
/dev/dm-1                               partition       6205436         220024          -2
$ swapoff -v /dev/dm-1
swapoff /dev/dm-1
$ systemctl daemon-reload

$ cat /proc/swaps
Filename                                Type            Size            Used            Priority
$ free --mega
              total        used        free      shared  buff/cache   available
Mem:          12366        4234        4138         567        3993        7210
Swap:             0           0           0
```

现在通过`systemctl`命令限制hunk用户所占用的内存空间：

```bash
$ id hunk
uid=1000(hunk) gid=1000(hunk) groups=1000(hunk)

# 限制hunk用户使用内存不超过256MB
$ systemctl set-property user-1000.slice MemoryLimit=256M
# 至少需要有一个hunk登录的session，这个目录才会存在
$ cd /sys/fs/cgroup/memory/user.slice/user-1000.slice/
free --mega; egrep "swap|rss" memory.stat; cat memory.usage_in_bytes; cat memory.limit_in_bytes
              total        used        free      shared  buff/cache   available
Mem:          12366        4238        4134         567        3994        7206
Swap:             0           0           0
rss 0
rss_huge 0
swap 0
total_rss 4497408
total_rss_huge 0
total_swap 0
6012928   #在只有一个SSH session的情况下，用户占用不到6MB的内存空间
268435456 #限定的256MB
```

现在用hunk用户执行`stress-ng`命令：

```bash
[hunk] $ stress-ng --vm 2 --vm-bytes 500M
stress-ng: info:  [2888801] defaulting to a 86400 second (1 day, 0.00 secs) run per stressor
stress-ng: info:  [2888801] dispatching hogs: 2 vm, 2 mmap
```

可以动态地观察到hunk用户创建的stress-ng进程最多占用`250MB`的内存空间，当stress-ng想要申请超出的内存空间时就会被OOM-Killer直接干掉，然后stress-ng又会自动启动新的进程开始分配内存：

```bash
$ free --mega; egrep "swap|rss" memory.stat; cat memory.usage_in_bytes; cat memory.limit_in_bytes
              total        used        free      shared  buff/cache   available
Mem:          12366        4476        3881         586        4008        6949
Swap:             0           0           0
rss 0
rss_huge 0
swap 0
total_rss 253706240
total_rss_huge 243269632
total_swap 0
267362304
268435456

$ tail -f /var/log/messages | grep stress
Jun  1 11:47:49 localhost kernel: Memory cgroup out of memory: Killed process 698096 (stress-ng-vm) total-vm:210428kB, anon-rss:131332kB, file-rss:704kB, shmem-rss:20kB, UID:1000 pgtables:372kB oom_score_adj:1000
Jun  1 11:47:49 localhost kernel: oom_reaper: reaped process 698096 (stress-ng-vm), now anon-rss:0kB, file-rss:0kB, shmem-rss:20kB
Jun  1 11:47:49 localhost stress-ng[676994]: memory (MB): total 11793.88, free 3543.79, shared 599.41, buffer 1.04, swap 0.00, free swap 0.00
```

如果开启了内存swap功能，那么stress进程不会直接被干掉，但是占用的物理内存空间仍然不会超出限制。

### CGROUPS VERSION 2

关于如何从默认的Cgroup v1切换到Cgroups v2可以参考：[Enable cgroup v2 on RHEL8](https://access.redhat.com/solutions/6898151)

Cgroups v2相比于Cgroups v1有一些新的行为：

1. Cgroups v2提供了一个统一的层次结构，所有的控制器都可以在这个层次结构上挂载。
2. “内部”进程是不允许的。除了`root cgroup`之外，进程可能只驻留在cgroup树的叶节点（即不包含child cgroup的cgroup）中。
3. 激活的cgroup必须通过`cgroup.controllers`和`cgroup.subtree_control`这两个文件指定。详见后面的Cgroups v2子树控制部分。
4. `tasks`文件已经被删除，该文件在Cgroups v1里用于跟踪与由参数设置控制的层次结构相关联的进程，tasks文件包含分配给cgroup的所有进程id (pid)。此外，`cpuset`控制器使用的`cgroup.clone_children`文件已被删除。
5. `cgroup.events`文件提供了一种改进的用于通知空的cgroup的机制。
6. v2层次结构中的每个cgroup可以通过`cgroup.max.depth`文件来限制派生的cgroup树的深度，通过`cgroup.max.descendants`文件来限制派生的cgroup的数量。

#### Cgroups v2统一层次结构

相比于Cgroups v1里不同的控制器可以挂载在不同的层次结构上，即衍生出多个独立的cgroup树，Cgroups v2只有一颗统一的cgroup树，摒弃了一些应用设计的灵活性（这种灵活性也会带来复杂度），简化了层次结构。

所有可用的Cgroups v2控制器都会自动被挂载，所以在执行挂载操作时不再需要指定控制器，比如：

```bash
$ mount -t cgroup2 none /mnt/cgroup2
```

> 只要一个Cgroups v2控制当前没有挂载到一个Cgroups v1的层次结构上，这个控制器就是可用的。原则上，尽管Cgroups v2向后兼容，但是控制器不能同时挂载在v1的层次结构和v2的层次结构。

`systemd`是Cgroups v1的重度使用者，如果想禁掉一些v1的控制器，可以在内核启动命令行加一个参数`cgroup_no_v1=list`, list是逗号分隔的控制器名字列表。

#### Cgroups v2控制器

关于每个cgroup v2控制器的具体作用参考[Cgroups v2 controllers](https://www.man7.org/linux/man-pages/man7/cgroups.7.html#DESCRIPTION)。

#### Cgroups v2子树控制

v2层次结构下的每个cgroup都包含这`cgroup.controllers`和`cgroup.subtree_control`这两个文件：

1. `cgroup.controllers`：这是一个只读文件，它包含了这个cgroup可用的控制器的列表，文件内容和`parent cgroup`的`cgroup.subtree_control`文件相匹配。

2. `cgroup.subtree_control`：这个文件包含了这个cgroup激活的控制器列表，这个控制器列表是`cgroup.controllers`的子集，可以通过更新这个文件来修改激活的控制器列表。比如：

   ```bash
   # + 代表激活； - 代表禁掉
   $ echo '+pids -memory' > x/y/cgroup.subtree_control
   ```

简单说，就是`cgroup.controllers`文件记录可用的控制器，`cgroup.subtree_control`记录激活的控制器，并且`parent cgroup`激活的控制器列表就是`child chroup`的可用控制器列表。

### Cgroups相关的/proc文件

1. `/proc/cgroups`：描述内核中支持的控制器名字、层次结构ID、cgroup的数量以及控制器是否被激活。

   ```bash
   $ cat /proc/cgroups 
   #subsys_name    hierarchy       num_cgroups     enabled
   cpuset  8       1       1
   cpu     6       171     1
   cpuacct 6       171     1
   blkio   12      1       1
   memory  7       2013    1
   devices 2       168     1
   freezer 11      1       1
   net_cls 4       1       1
   perf_event      3       1       1
   net_prio        4       1       1
   hugetlb 10      1       1
   pids    5       243     1
   rdma    9       1       1
   ```

2. `/proc/[pid]/cgroup`：描述该进程属于的cgroup信息：

   ```bash
   # 行格式： hierarchy-ID:controller-list:cgroup-path
   $ cat /proc/$$/cgroup 
   12:blkio:/
   11:freezer:/
   10:hugetlb:/
   9:rdma:/
   8:cpuset:/
   7:memory:/user.slice/user-888.slice/session-3573.scope
   6:cpu,cpuacct:/user.slice
   5:pids:/user.slice/user-888.slice/session-3573.scope
   4:net_cls,net_prio:/
   3:perf_event:/
   2:devices:/system.slice/sshd.service
   1:name=systemd:/user.slice/user-888.slice/session-3573.scope
   ```

3. `/sys/kernel/cgroup/delegate`：描述可委派的（delegatable）cgroup v2文件。

   ```bash
   $  cat /sys/kernel/cgroup/delegate 
   cgroup.procs
   cgroup.threads
   cgroup.subtree_control
   memory.oom.group
   ```

4. `/sys/kernel/cgroup/features`：描述内核cgroup支持的功能。

   ```bash
   $ cat /sys/kernel/cgroup/features 
   nsdelegate
   memory_localevents
   memory_recursiveprot
   ```

### CGROUPS VERSION 2 Demos

### References

[cgroups(7) — Linux manual page](https://www.man7.org/linux/man-pages/man7/cgroups.7.html#DESCRIPTION)

[Understanding cgroups](https://www.grant.pizza/blog/understanding-cgroups/)

[Controll Groups](https://www.kernel.org/doc/html/latest/admin-guide/cgroup-v1/cgroups.html)

[IOCost: Block IO Control for Containers in Datacenters](https://www.cs.cmu.edu/~dskarlat/publications/iocost_asplos22.pdf) # Linux blkio-cgroup作者的论文

[Chapter 32. Stress testing real-time systems with stress-ng](https://access.redhat.com/documentation/en-us/red_hat_enterprise_linux_for_real_time/8/html/optimizing_rhel_8_for_real_time_for_low_latency_operation/assembly_stress-testing-real-time-systems-with-stress-ng_optimizing-rhel8-for-real-time-for-low-latency-operation#proc_generating-a-virtual-memory-pressure_assembly_stress-testing-real-time-systems-with-stress-ng)



##  理解容器单进程模型