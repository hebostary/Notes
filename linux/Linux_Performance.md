# Linux性能分析工具

## 常用工具

### mpstat

### pidstat - cpu

pidstat 是一个专门分析每个进程 CPU 使用情况的工具，它的输出如下所示：

```bash
# # 每隔1秒输出一组数据，共输出1组
root@ubuntu:~/perf# pidstat 1 1
Linux 5.4.0-174-generic (ubuntu)        05/20/2025      _x86_64_        (8 CPU)

12:26:46 PM   UID       PID    %usr %system  %guest   %wait    %CPU   CPU  Command
12:26:47 PM     0   2061391  199.02    0.00    0.00    0.00  199.02     5  cpu_extensive2
...

Average:      UID       PID    %usr %system  %guest   %wait    %CPU   CPU  Command
...
Average:        0   2061391  199.02    0.00    0.00    0.00  199.02     -  cpu_extensive2
Average:        0   2073716    0.00    0.98    0.00    0.00    0.98     -  pidstat
Average:        0   2073719    0.98    0.00    0.00    0.00    0.98     -  runc
```

其中，输出的各项指标含义：

1. 用户态 CPU 使用率 （%usr）；
2. 内核态 CPU 使用率（%system）；
3. 运行虚拟机 CPU 使用率（%guest）；
4. 等待 CPU 使用率（%wait）；
5. 以及总的 CPU 使用率（%CPU）。

相比于 top 命令，pidstat 会区分显示程序在内核态和在用户态分别消耗了多少 CPU ，这对于实际的性能分析更实用。

### vmstat - system

vmstat 是一个强大的 Linux 系统性能监控工具，可以报告整个系统的虚拟内存统计、进程、CPU 活动、内存使用、磁盘 I/O 等信息，基本用法如下：

```tex
vmstat [选项] [间隔时间] [次数]
```

常用选项：

- `-a`：显示活跃和非活跃内存
- `-d`：显示磁盘统计
- `-p`：显示指定磁盘分区的统计
- `-s`：显示内存相关统计和各种事件计数器的汇总
- `-w`：宽输出模式（更适合宽屏显示）

典型输出示例：

```bash
root@ubuntu:~/perf# vmstat -w
procs -----------------------memory---------------------- ---swap-- -----io---- -system-- --------cpu--------
 r  b         swpd         free         buff        cache   si   so    bi    bo   in   cs  us  sy  id  wa  st
 3  0            0      4950136       537380      6317340    0    0     5    28   32   15   9   2  89   0   0
```

1. **procs（进程）**
   - `r`：运行队列中的进程数，太多的话说明系统中有大量进程在运行队列中等待 CPU，系统可能存在 CPU 性能问题
   - `b`：等待I/O的阻塞进程数，太多的话说明系统可能存在I/O瓶颈，磁盘成为性能瓶颈
2. **memory（内存）**
   - `swpd`：使用的虚拟内存量（KB）
   - `free`：空闲内存量（KB）
   - `buff`：用作缓冲区的内存量（KB）
   - `cache`：用作缓存的内存量（KB）
3. **swap（交换分区）**- 显示是否有持续的swap内存交换活动
   - `si`：每秒从磁盘读入swap的内存大小（KB/s）
   - `so`：每秒写入swap的内存大小（KB/s）
4. **io（I/O）**
   - `bi`：每秒从块设备接收的块数（blocks/s）
   - `bo`：每秒发送到块设备的块数（blocks/s）
5. **system（系统）** - 中断数和上下文切换数过高的话说明可能系统调用过于频繁，或者存在大量网络小包之类的问题
   - `in`：每秒中断数
   - `cs`：每秒上下文切换数
6. **cpu（CPU）** - 反应系统整体的 CPU 性能
   - `us`：用户空间占用CPU百分比
   - `sy`：内核空间占用CPU百分比
   - `id`：空闲CPU百分比
   - `wa`：等待I/O的CPU百分比
   - `st`：被虚拟机偷取的时间百分比

其它常用用法：

```bash
# 查看内存详细信息：
vmstat -s

# 监控特定磁盘分区：
vmstat -p /dev/sda1 1 5
```

### dstat - system

dstat 是一个强大的 Linux 系统性能监控工具，可以实时显示 **CPU、内存、磁盘、网络、进程、系统负载** 等关键指标，比 `vmstat` 和 `iostat` 更直观，并且支持彩色输出和 CSV 导出。基础用法如下：

```bash
# 每秒输出一组数据，总共输出3组数据
root@ubuntu:~# dstat 1 3
You did not select any stats, using -cdngy by default.
--total-cpu-usage-- -dsk/total- -net/total- ---paging-- ---system--
usr sys idl wai stl| read  writ| recv  send|  in   out | int   csw 
 10   2  87   0   0|  15k  234k|   0     0 |   0     0 |  14k   24k
 13   4  83   0   0|   0   456k| 405k  441k|   0     0 |  16k   27k
  8   2  90   0   0|   0     0 |  90k   90k|   0     0 |  12k   19k
```

监控所有指标：

```bash
# CPU + 磁盘 + 网络 + 负载 + 系统统计 = dstat -a
root@ubuntu:~# dstat -cdngy
--total-cpu-usage-- -dsk/total- -net/total- ---paging-- ---system--
usr sys idl wai stl| read  writ| recv  send|  in   out | int   csw 
 10   2  87   0   0|  15k  234k|   0     0 |   0     0 |  14k   24k
 10   3  87   0   0|   0   128k| 113k  424k|   0     0 |  13k   22k
 13   4  82   0   0|   0    36k| 615k  641k|   0     0 |  20k   36k
```

显示特定设备的IO：

```bash
root@ubuntu:~# dstat -D sda,sdb  # 只监控 sda 和 sdb 的磁盘 I/O
root@ubuntu:~# dstat -N eth0,lo  # 只监控 eth0 和 lo 的网络流量
```

显示进程占用资源：

```bash
root@ubuntu:~# dstat -p -t --top-cpu    # 显示 CPU 占用最高的进程
root@ubuntu:~# dstat -p -t --top-mem    # 显示内存占用最高的进程
root@ubuntu:~# dstat -p -t --top-io     # 显示 I/O 占用最高的进程
```

组合监控：

```bash
root@ubuntu:~# dstat -cdngy --top-cpu --top-mem --top-io
--total-cpu-usage-- -dsk/total- -net/total- ---paging-- ---system-- -most-expensive- --most-expensive- ----most-expensive----
usr sys idl wai stl| read  writ| recv  send|  in   out | int   csw |  cpu process   |  memory process |     i/o process      
 10   2  87   0   0|  15k  234k|   0     0 |   0     0 |  14k   24k|venus-plugin-2.2|kube-apiserv 658M|dockerd     576k 1430k
  8   2  90   0   0|   0  8192B| 183k  184k|   0     0 |  12k   20k|venus-plugin-3.1|kube-apiserv 658M|bash       1161k   16k
```

**排查性能问题时**：

- CPU 高 → `dstat -c --top-cpu`
- 磁盘慢 → `dstat -d -D sda --top-io`
- 网络卡 → `dstat -n -N eth0`
- 内存不足 → `dstat -m --top-mem`





### perf - cpu

perf 是分析 CPU 性能的常用工具。perf top 可以实时展示系统的性能信息。perf top -p 可以查看指定进程的性能信息。例如下面的测试程序创建2个线程持续进行计算，导致占满2个 CPU 核心：

```c
// gcc -g -o cpu_extensive2 cpu_extensive2.c -lpthread
#include <stdio.h>
#include <pthread.h>

void *cpu_thread(void *arg) {
    volatile unsigned long long i = 0;
    while(1) i++;
    return NULL;
}

int main() {
    int cores = 2;
    printf("Starting %d CPU-intensive threads...\n", cores);
    
    pthread_t threads[cores];
    for (int i = 0; i < cores; i++) {
        pthread_create(&threads[i], NULL, cpu_thread, NULL);
    }
    
    for (int i = 0; i < cores; i++) {
        pthread_join(threads[i], NULL);
    }
    
    return 0;
}
```

 top 命令查看其CPU使用率接近200%：

```bash
top - 11:59:31 up 1 day,  5:12,  4 users,  load average: 3.16, 2.68, 1.72
Tasks: 551 total,   1 running, 548 sleeping,   0 stopped,   2 zombie
%Cpu(s): 27.7 us,  1.0 sy,  0.0 ni, 70.2 id,  0.0 wa,  0.0 hi,  1.0 si,  0.0 st
MiB Mem :  15996.5 total,   5348.2 free,   4422.5 used,   6225.8 buff/cache
MiB Swap:      0.0 total,      0.0 free,      0.0 used.  11302.2 avail Mem 

    PID USER      PR  NI    VIRT    RES    SHR S  %CPU  %MEM     TIME+ COMMAND                                                                     
2037495 root      20   0   19032    616    536 S 199.7   0.0   8:02.12 cpu_extensive2
```

执行 `perf top -g -p 2037495` 对其进行分析（ -g： 开启调用关系分析）：

```bash
Samples: 262K of event 'cpu-clock:pppH', 4000 Hz, Event count (approx.): 31954820866 lost: 0/0 drop: 0/0
  Children      Self  Shared Object       Symbol
-  100.00%    99.93%  cpu_extensive2      [.] cpu_thread                                                                                          ▒
     99.93% start_thread                                                                                                                          ◆
        cpu_thread                                                                                                                                ▒
+   12.69%     0.00%  libpthread-2.31.so  [.] start_thread                                                                                        ▒
     0.07%     0.04%  [kernel]            [k] __softirqentry_text_start                                                                           ▒
     0.01%     0.00%  [kernel]            [k] __run_time
```

可以看到最耗 CPU 资源的是 cpu_thread 这个函数，选中这个函数并进入 `Annotate cpu_thread`后可以看到如下 cpu_thread  函数的 CPU 占用统计，函数里的具体哪些语句哪些汇编指令的 CPU 使用率都很清晰。

```bash
Samples: 900K of event 'cpu-clock:pppH', 4000 Hz, Event count (approx.): 40310957513
cpu_thread  /root/perf/cpu_extensive2 [Percent: local period]
Percent│
       │
       │
       │    Disassembly of section .text:
       │
       │    00000000000011a9 <cpu_thread>:
       │    cpu_thread():
       │    #include <stdio.h>
       │    #include <pthread.h>
       │
       │    void *cpu_thread(void *arg) {
       │      endbr64
       │      push    %rbp
       │      mov     %rsp,%rbp
       │      mov     %rdi,-0x18(%rbp)
       │    volatile unsigned long long i = 0;
       │      movq    $0x0,-0x8(%rbp)
       │    while(1) i++;
  0.40 │14:   mov     -0x8(%rbp),%rax
 59.46 │      add     $0x1,%rax
 13.39 │      mov     %rax,-0x8(%rbp)
 26.76 │    ↑ jmp     14
```

如果gcc编译时没有开启 -g，则只能看到 cpu_thread 函数里的汇编指令的 CPU 占用率，无法看到具体对应的是哪条 C 代码语句。

> TODO: 对于在容器中运行的程序，在宿主机山使用 perf top 可能只能看到符号的内存地址，函数符号名都无法打印。最好是将 perf 工具及其依赖拷贝到容器内执行。

perf record & report 可以进行离线的 CPU 性能分析：

```bash
# perf record 负责性能数据采样，CTL + C 生成 perf.data 文件，用perf report 即可和 perf top 做相同的分析
root@ubuntu:~/perf# perf record
root@ubuntu:~/perf# ls
perf.data
root@ubuntu:~/perf# perf report
```



### sar



## 内核性能分析工具

### tracepoint

### ftrace



# CPU性能分析及优化

## 平均负载和CPU使用率

### 平均负载

平均负载是指单位时间内，系统处于可运行状态和不可中断状态的平均进程数，也就是平均活跃进程数，它不仅包括了正在使用 CPU 的进程，还包括等待 CPU 和等待 I/O 的进程。

1. 所谓可运行状态的进程，是指正在使用 CPU 或者正在等待 CPU 的进程，也就是我们常用 ps 命令看到的，处于 R 状态（Running 或 Runnable）的进程。
2. 不可中断状态的进程则是正处于内核态关键流程中的进程，并且这些流程是不可打断的，比如最常见的是等待硬件设备的 I/O 响应，也就是我们在 ps 命令中看到的 D 状态（Uninterruptible Sleep，也称为 Disk Sleep）的进程。不可中断状态实际上是系统对进程和硬件设备的一种保护机制。

使用`uptime`和`top`命令都可以观察系统的平均负载，三个不同时间间隔（依次是1分钟，5分钟和15分钟）的平均值，其实给我们提供了，分析系统负载趋势的数据来源，让我们能更全面、更立体地理解目前的负载状况：

1. 如果 1 分钟、5 分钟、15 分钟的三个值基本相同，或者相差不大，那就说明系统负载很平稳。

2. 如果 1 分钟的值远小于 15 分钟的值，就说明系统最近 1 分钟的负载在减少，而过去 15 分钟内却有很大的负载。

3. 如果 1 分钟的值远大于 15 分钟的值，就说明最近 1 分钟的负载在增加，这种增加有可能只是临时性的，也有可能还会持续增加下去，所以就需要持续观察。一旦 1 分钟的平均负载接近或超过了 CPU 的个数，就意味着系统正在发生过载的问题，这时就得分析调查是哪里导致的问题，并要想办法优化了。

平均负载是否过高取决于机器上的 CPU 数量（可以通过 `lscpu` 查看），一般地，当平均负载高于 CPU 数量 70% 的时候，就应该分析排查负载高的问题了。一旦负载过高，就可能导致进程响应变慢，进而影响服务的正常功能。可以使用 `mpstat`、`pidstat` 等工具，辅助分析负载的来源。

> mpstat 是一个常用的多核 CPU 性能分析工具，用来实时查看每个 CPU 的性能指标，以及所有 CPU 的平均指标。
>
> pidstat 是一个常用的进程性能分析工具，用来实时查看进程的 CPU、内存、I/O 以及上下文切换等性能指标。

CPU 使用率，是单位时间内 CPU 繁忙情况的统计，跟平均负载并不一定完全对应。比如：

1. CPU 密集型进程，使用大量 CPU 会导致平均负载升高，此时这两者是一致的；
2. I/O 密集型进程，等待 I/O 也会导致平均负载升高，但 CPU 使用率不一定很高；
3. 大量等待 CPU 的进程调度也会导致平均负载升高，此时的 CPU 使用率也会比较高。

所以，平均负载和 CPU 使用率并没有直接关系。

### CPU 使用率

CPU 使用率，就是除了空闲时间外的其他时间占总 CPU 时间的百分比。为了计算 CPU 使用率，性能工具一般都会取 间隔一段时间（比如 3 秒）的两次值，作差后，再计算出这段时间内的平均 CPU 使用率，即：

![cpu usage compute](https://static001.geekbang.org/resource/image/84/5a/8408bb45922afb2db09629a9a7eb1d5a.png?wh=569*85)

系统的 CPU 统计数据记录在 `/proc/stat`  Linux 也给每个进程提供了运行情况的统计信息，也就是 `/proc/[pid]/stat`。

系统的 CPU 使用率升高，可以使用 top 命令查看是哪个进程的 CPU 占用率最高，但是 top 并没有细分进程的用户态 CPU 和内核态 CPU，可以使用 pidstat 分析程序是在用户态还是在内核态占用 CPU 过高。更进一步可以用 perf 工具来分析进程内哪个函数哪行指令占用 CPU 过高。

## CPU 上下文切换

Linux 是一个多任务操作系统，它支持远大于 CPU 数量的任务同时运行。每个任务运行前，CPU 都需要知道任务从哪里加载、又从哪里开始运行，也就是说，需要系统事先帮它设置好 CPU 寄存器和程序计数器（Program Counter，PC）。

CPU 寄存器，是 CPU 内置的容量小、但速度极快的内存。而程序计数器，则是用来存储 CPU 正在执行的指令位置、或者即将执行的下一条指令位置。它们都是 CPU 在运行任何任务前，必须的依赖环境，因此也被叫做 **CPU 上下文**。

![cpu_arch1](https://static001.geekbang.org/resource/image/98/5f/98ac9df2593a193d6a7f1767cd68eb5f.png?wh=438*345)

*CPU 上下文切换*，就是先把前一个任务的 CPU 上下文（也就是 CPU 寄存器和程序计数器）保存起来，然后加载新任务的上下文到这些寄存器和程序计数器，最后再跳转到程序计数器所指的新位置，运行新任务。这些保存下来的上下文，会存储在系统内核中，并在任务重新调度执行时再次加载进来。这样就能保证任务原来的状态不受影响，让任务看起来还是连续运行。

CPU 上下文切换本身只是更新一些寄存器和程序计数器，速度是非常快的，但是触发 CPU 上下文切换的源头是任务切换，而任务本身还会携带更多的上下文信息，导致切换成本变高。*过多的上下文切换，会把 CPU 时间消耗在寄存器、内核栈以及虚拟内存等数据的保存和恢复上，缩短进程真正运行的时间，成了系统性能大幅下降的一个元凶。*

根据任务的不同，CPU 的上下文切换就可以分为几个不同的场景，也就是进程上下文切换、线程上下文切换以及中断上下文切换。下面依次介绍这几种场景。

### 进程上下文切换

Linux 按照特权等级，把进程的运行空间分为内核空间和用户空间，分别对应着 CPU 特权等级的 Ring 0 和 Ring 3：

1. 内核空间（Ring 0）具有最高权限，可以直接访问所有资源；
2. 用户空间（Ring 3）只能访问受限资源，不能直接访问内存等硬件设备，必须通过系统调用陷入到内核中，才能访问这些特权资源。

进程既可以在用户空间运行，又可以在内核空间中运行。进程在用户空间运行时，被称为进程的用户态，通过*系统调用*陷入内核空间的时候，被称为进程的内核态。

在执行系统调用过程中，CPU 寄存器里原来用户态的指令位置，需要先保存起来。接着，为了执行内核态代码，CPU 寄存器需要更新为内核态指令的新位置。最后才是跳转到内核态运行内核任务。系统调用结束后，CPU 寄存器需要恢复原来保存的用户态，然后再切换到用户空间，继续运行进程。所以，*一次系统调用的过程，会发生两次 CPU 上下文切换*。

> 系统调用过程中，并不会涉及到虚拟内存等进程用户态的资源，也不会切换进程。这跟我们通常所说的进程上下文切换是不一样的：进程上下文切换，是指从一个进程切换到另一个进程运行。系统调用过程中一直是同一个进程在运行。所以，系统调用过程通常称为特权模式切换，而不是上下文切换。

那么，**进程上下文切换跟系统调用又有什么区别呢？**

进程是由内核来管理和调度的，进程的切换只能发生在内核态。所以，进程的上下文不仅包括了虚拟内存、栈、全局变量等用户空间的资源，还包括了内核堆栈、寄存器等内核空间的状态。因此，进程的上下文切换就比系统调用时多了一步：在保存当前进程的内核状态和 CPU 寄存器之前，需要先把该进程的虚拟内存、栈等保存下来；而加载了下一进程的内核态后，还需要刷新进程的虚拟内存和用户栈。

保存上下文和恢复上下文的过程并不是“免费”的，需要内核在 CPU 上运行才能完成。根据 Tsuna 的测试报告，每次上下文切换都需要几十纳秒到数微秒的 CPU 时间。这个时间还是相当可观的，特别是在进程上下文切换次数较多的情况下，很容易导致 CPU 将大量时间耗费在寄存器、内核栈以及虚拟内存等资源的保存和恢复上，进而大大缩短了真正运行进程的时间。

Linux 通过 TLB（Translation Lookaside Buffer）来管理虚拟内存到物理内存的映射关系。当虚拟内存更新后，TLB 也需要刷新，内存的访问也会随之变慢。特别是在多处理器系统上，缓存是被多个处理器共享的，刷新缓存不仅会影响当前处理器的进程，还会影响共享缓存的其他处理器的进程。

**什么时候会发生进程上下文切换呢？**

进程切换时才需要切换上下文，换句话说，只有在内核执行进程调度或者响应硬件中断的时候，才需要切换上下文。Linux 为每个 CPU 都维护了一个就绪队列，将活跃进程（即正在运行和正在等待 CPU 的进程）按照优先级和等待 CPU 的时间排序，然后选择最需要 CPU 的进程，也就是优先级最高和等待 CPU 时间最长的进程来运行。

那么，什么时候会触发内核执行进程调度呢？可以分为*主动调度 (Voluntary Scheduling)*和*被动调度 (Preemptive Scheduling，抢占)*两种类型：

1. 主动调度场景：

   1. 系统调用导致阻塞。例如，进程执行可能阻塞的系统调用（如`read()`, `write()`, `sleep()`, `wait()`等）。内核将进程状态设为`TASK_INTERRUPTIBLE`或`TASK_UNINTERRUPTIBLE`，并调用`schedule()`主动让出CPU。
   2. 进程显示调用`sched_yield()`主动让出CPU（但可能立即被再次调度）。例如，一些协程库的设计会用到。
   3. 进程执行`exit()`或主函数返回时。内核调用`schedule()`切换到下一个进程。

2. 被动调度（内核**强制剥夺当前进程的CPU**，通常由中断或条件触发）：

   1. 时钟中断（Scheduler Tick）：周期性时钟中断（如`CONFIG_HZ=1000`时，每1ms一次）。内核会更新当前进程的时间片（`task_struct->time_slice`）；检查是否需要抢占（如时间片耗尽或更高优先级任务就绪）；设置`TIF_NEED_RESCHED`标志。

      ```c
      scheduler_tick() → task_tick_fair() → check_preempt_tick()
      ```

   2. 进程时间片耗尽：例如 CFS 调度器中，进程的`vruntime`超过其他可运行进程。内核在`scheduler_tick()`中检测并触发抢占。

   3. 更高优先级任务就绪：例如实时进程（`SCHED_FIFO`/`SCHED_RR`）被唤醒。内核在`wake_up_process()`中调用`check_preempt_curr()`。

   4. 硬件中断：发生硬件中断时，CPU 上的进程会被中断挂起，转而执行内核中的中断服务程序。

   5. 中断返回路径（内核态 -> 用户态）：硬件中断或系统调用返回用户态前。内核检查`TIF_NEED_RESCHED`标志，若置位则调用`schedule()`。

   6. CPU 负载均衡：在多核系统中，调度器会迁移任务到空闲CPU。主要通过`SCHED_SOFTIRQ`软中断调用`run_rebalance_domains()`。

这些只是触发进程调度的常见场景，并不是全部。

### 线程上下文切换

线程与进程最大的区别在于，*线程是调度的基本单位，而进程则是资源拥有的基本单位*。说白了，所谓内核中的任务调度，实际上的调度对象是线程；而进程只是给线程提供了虚拟内存、全局变量等资源。所以，线程的上下文切换可以分为两个场景：

1. 前后两个线程属于同一个进程。此时，因为虚拟内存是共享的，所以在切换时，虚拟内存这些资源就保持不动，只需要切换线程的私有数据、寄存器等不共享的数据。
2. 前后两个线程属于不同的进程。这些资源在上下文切换时是不需要修改的。此时，因为资源不共享，所以切换过程就跟进程上下文切换是一样。

### 中断上下文切换

为了快速响应硬件的事件，中断处理会打断进程的正常调度和执行，转而调用中断处理程序，响应设备事件。而在打断其他进程时，就需要将进程当前的状态保存下来，这样在中断结束后，进程仍然可以从原来的状态恢复运行。

跟进程上下文不同：*中断上下文切换并不涉及到进程的用户态，即便中断过程打断了一个正处在用户态的进程，也不需要保存和恢复这个进程的虚拟内存、全局变量等用户态资源*。所以，中断上下文只包括内核态中断服务程序执行所必需的状态，包括 CPU 寄存器、内核堆栈、硬件中断参数等。

对同一个 CPU 来说，中断处理比进程拥有更高的优先级，所以中断上下文切换并不会与进程上下文切换同时发生。同样道理，由于中断会打断正常进程的调度和执行，所以大部分中断处理程序都短小精悍，以便尽可能快的执行结束。跟进程上下文切换一样，中断上下文切换也需要消耗 CPU，切换次数过多也会耗费大量的 CPU，甚至严重降低系统的整体性能。所以，当发现中断次数过多时，就需要注意去排查它是否会给你的系统带来严重的性能问题。

### 分析上下文切换

前面提到，过多的上下文切换，会把 CPU 时间消耗在寄存器、内核栈以及虚拟内存等数据的保存和恢复上，缩短进程真正运行的时间，成了系统性能大幅下降的一个元凶。那么如何分析上下文切换呢？我们可以使用 vmstat 这个工具，来查询系统的上下文切换情况。

> vmstat 是一个常用的系统性能分析工具，主要用来分析系统的内存使用情况，也常用来分析 CPU 上下文切换和中断的次数。

```bash
# 每5秒输出一组数据：
root@ubuntu:~# vmstat -w 5
procs -----------------------memory---------------------- ---swap-- -----io---- -system-- --------cpu--------
 r  b         swpd         free         buff        cache   si   so    bi    bo   in   cs  us  sy  id  wa  st
 1  0            0      7884736       244356      4303200    0    0    26    30  174  108  10   1  89   0   0
```

我们重点关注 procs 和 system 两组数据：

1. r（Running or Runnable）是就绪队列的长度，也就是正在运行和等待 CPU 的进程数。
2. b（Blocked）则是处于不可中断睡眠状态的进程数。
3. cs（context switch）是每秒上下文切换的次数。
4. in（interrupt）则是每秒中断的次数。

vmstat 只给出了系统总体的上下文切换情况，要想查看每个进程的详细情况，可以使用前面提到过的 pidstat：

```bash
# pidstat 默认显示进程的指标数据，加上 -t 参数后，才会输出线程的指标。可能进程（也就是主线程）的上下文切换次数看起来并不多，但它的子线程的上下文切换次数却有很多。
# 间隔5秒输出一组数据，只输出一组数据
root@ubuntu:~# pidstat -wt 5 1
Linux 5.4.0-174-generic (ubuntu)        05/04/2025      _x86_64_        (8 CPU)

08:16:56 AM   UID      TGID       TID   cswch/s nvcswch/s  Command
08:17:01 AM     0         1         -      0.99      0.00  systemd
08:17:01 AM     0         -         1      0.99      0.00  |__systemd
08:17:01 AM     0         9         -      1.38      0.00  ksoftirqd/0
08:17:01 AM     0         -         9      1.38      0.00  |__ksoftirqd/0
```

pidstat 的输出里，cswch 表示每秒*自愿上下文切换*（voluntary context switches）的次数；nvcswch 表示每秒*非自愿上下文切换*（non voluntary context switches）的次数。

1. 自愿上下文切换，是指进程无法获取所需资源，导致的上下文切换。比如说， I/O、内存等系统资源不足时，就会发生自愿上下文切换。自愿上下文切换变多了，说明进程都在等待资源，有可能发生了 I/O 等其他问题；
2. 非自愿上下文切换，则是指进程由于时间片已到等原因，被系统强制调度，进而发生的上下文切换。非自愿上下文切换变多了，说明进程都在被强制调度，也就是都在争抢 CPU，说明 CPU 的确成了瓶颈；

## 中断性能问题

### 硬中断和软中断

中断其实是一种异步的事件处理机制，可以提高系统的并发处理能力。中断事件发生，会触发执行中断处理程序。Linux 中的中断处理程序分为上半部和下半部：

1. 上半部对应硬件中断，用来快速处理中断，它在中断禁止模式下运行，主要处理跟硬件紧密相关的或时间敏感的工作。
2. 下半部对应软中断，用来异步处理上半部未完成的工作。通常以内核线程的方式运行。每个 CPU 都对应一个软中断内核线程，名字为 “ksoftirqd/CPU 编号”，比如说， 0 号 CPU 对应的软中断内核线程的名字就是 ksoftirqd/0。

软中断不只包括了刚刚所讲的硬件设备中断处理程序的下半部，一些内核自定义的事件也属于软中断，比如内核调度、 RCU 锁（Read-Copy Update 的缩写，RCU 是 Linux 内核中最常用的锁之一）、网络收发、定时等各种类型，可以通过查看 /proc/softirqs 来观察软中断的运行情况。

以网卡接收数据为例，网卡接收到数据包后，会通过硬件中断的方式，通知内核有新的数据到了。这时，内核就应该调用中断处理程序来响应它。这种情况下的上半部和下半部分别负责什么工作呢？对上半部来说，既然是快速处理，其实就是要把网卡的数据读到内存中，然后更新一下硬件寄存器的状态（表示数据已经读好了），最后再发送一个软中断信号，通知下半部做进一步的处理。而下半部被软中断信号唤醒后，需要从内存中找到网络数据，再按照网络协议栈，对数据进行逐层解析和处理，直到把它送给应用程序。

### 分析软中断异常

软中断 CPU 使用率（softirq）升高是一种很常见的性能问题。虽然软中断的类型很多，但实际生产中，我们遇到的性能瓶颈大多是网络收发类型的软中断，特别是网络接收的软中断。

非常典型的是 SYN FLOOD 网络攻击，服务端响应变慢，但是 top 命令看平均负载和 CPU 的使用率都挺低，也没有CPU占用高的进程，类似下面的输出所示：

```bash
# top运行后按数字1切换到显示所有CPU
$ top
top - 10:50:58 up 1 days, 22:10,  1 user,  load average: 0.00, 0.00, 0.00
Tasks: 122 total,   1 running,  71 sleeping,   0 stopped,   0 zombie
%Cpu0  :  0.0 us,  0.0 sy,  0.0 ni, 96.7 id,  0.0 wa,  0.0 hi,  3.3 si,  0.0 st
%Cpu1  :  0.0 us,  0.0 sy,  0.0 ni, 95.6 id,  0.0 wa,  0.0 hi,  4.4 si,  0.0 st
...

  PID USER      PR  NI    VIRT    RES    SHR S  %CPU %MEM     TIME+ COMMAND
    7 root      20   0       0      0      0 S   0.3  0.0   0:01.64 ksoftirqd/0
   16 root      20   0       0      0      0 S   0.3  0.0   0:01.97 ksoftirqd/1
 2663 root      20   0  923480  28292  13996 S   0.3  0.3   4:58.66 docker-containe
 3699 root      20   0       0      0      0 I   0.3  0.0   0:00.13 kworker/u4:0
 3708 root      20   0   44572   4176   3512 R   0.3  0.1   0:00.07 top
    1 root      20   0  225384   9136   6724 S   0.0  0.1   0:23.25 systemd
    2 root      20   0       0      0      0 S   0.0  0.0   0:00.03 kthreadd
...
```

仔细看 top 的输出，两个 CPU 的使用率虽然分别只有 3.3% 和 4.4%，但都用在了软中断上；而从进程列表上也可以看到，CPU 使用率最高的也是软中断进程 ksoftirqd。怀疑是软中断导致的问题，通过`watch -d cat /proc/softirqs` 可以观察哪些软中断的数据变化异常。

通过 sar 命令分析网卡数据收发情况：

```bash
# -n DEV 表示显示网络收发的报告，间隔1秒输出一组数据
$ sar -n DEV 1
15:03:46        IFACE   rxpck/s   txpck/s    rxkB/s    txkB/s   rxcmp/s   txcmp/s  rxmcst/s   %ifutil
15:03:47         eth0  12607.00   6304.00    664.86    358.11      0.00      0.00      0.00      0.01
15:03:47      docker0   6302.00  12604.00    270.79    664.66      0.00      0.00      0.00      0.00
15:03:47           lo      0.00      0.00      0.00      0.00      0.00      0.00      0.00      0.00
15:03:47    veth9f6bbcd   6302.00  12604.00    356.95    664.66      0.00      0.00      0.00      0.05
```

第三、四列：rxpck/s 和 txpck/s 分别表示每秒接收、发送的网络帧数，也就是 PPS。第五、六列：rxkB/s 和 txkB/s 分别表示每秒接收、发送的千字节数，也就是 BPS。

可以发现，对网卡 eth0 来说，每秒接收的网络帧数比较大，达到了 12607，而发送的网络帧数则比较小，只有 6304；每秒接收的千字节数只有 664 KB，而发送的千字节数更小，只有 358 KB。我们稍微计算一下，664*1024/12607 = 54 字节，说明 eth0 平均收到的网络帧只有 54 字节，这显然是很小的网络帧，也就是我们通常所说的小包问题。使用 tcpdump 抓取 eth0 上的包就可以进一步确认了。



**/proc/interrupts **

`/proc/interrupts` 文件记录了系统中所有硬件中断（IRQ）的详细统计信息，包括每个中断号的发生次数、触发CPU核心以及关联的设备或事件。这些数据对分析系统性能（尤其是CPU使用率和中断负载）至关重要。

```bash
root@ubuntu:~# cat /proc/interrupts 
           CPU0       CPU1       CPU2       CPU3       CPU4       CPU5       CPU6       CPU7       
  0:          4          0          0          0          0          0          0          0   IO-APIC   2-edge      timer # 系统定时器中断。IO-APIC、PCI-MSI：中断控制器类型（APIC/MSI为现代中断方式）。
  1:          0          0          0          0          0          0          0          9   IO-APIC   1-edge      i8042 # edge/fasteoi：中断触发模式（边沿触发/电平触发）。
  8:          1          0          0          0          0          0          0          0   IO-APIC   8-edge      rtc0
  9:          0          0          0          0          0          0          0          0   IO-APIC   9-fasteoi   acpi
 12:          0          0          0          0          0          0         15          0   IO-APIC  12-edge      i8042
 14:          0          0          0          0          0          0          0          0   IO-APIC  14-edge      ata_piix
 15:          0          0          0          0          0          0          0          0   IO-APIC  15-edge      ata_piix
 16:          0          0        117      88228          0          0          0          0   IO-APIC  16-fasteoi   vmwgfx
 17:          0          0          0       8530          0     318973          0          0   IO-APIC  17-fasteoi   ioc0
 18:          0          0          0          0          0         63          0          0   IO-APIC  18-fasteoi   uhci_hcd:usb2
 19:          0          0          0          0          0          0          0          0   IO-APIC  19-fasteoi   ehci_hcd:usb1 # USB控制器中断。
...
 56:          0          0    1406483          0          0          0        876          0   PCI-MSI 1572864-edge      ens160-rxtx-0
 57:          7         81          5         45        560        392        657       1192   PCI-MSI 1572865-edge      ens160-rxtx-1
 58:         16       9649          0         13       3334          0       2495        475   PCI-MSI 1572866-edge      ens160-rxtx-2
 59:          2        137          9         73        484        319        514       1086   PCI-MSI 1572867-edge      ens160-rxtx-3
 60:          2         93         12         51        922        272       1510       2316   PCI-MSI 1572868-edge      ens160-rxtx-4
 61:         65        712          5         62        354        232       2719       1622   PCI-MSI 1572869-edge      ens160-rxtx-5
 62:          2        125          9         60        575        179        748       1391   PCI-MSI 1572870-edge      ens160-rxtx-6
 63:      15929          0          7          0          0         34          6        151   PCI-MSI 1572871-edge      ens160-rxtx-7
 64:          0          0          0          0          0          0          0          0   PCI-MSI 1572872-edge      ens160-event-8
 65:          0        197          0          0       4375         10       3820       1540   PCI-MSI 1097728-edge      ahci[0000:02:03.0]
 66:          0        165         25          0       2508        209        271        409   PCI-MSI 129024-edge      vmw_vmci
 67:          0          0          0          0          0          0          0          0   PCI-MSI 129025-edge      vmw_vmci
NMI:          0          0          0          0          0          0          0          0   Non-maskable interrupts  # 不可屏蔽中断（硬件错误等）。
LOC:   15127653   15020192   15013790   14974722   14888020   15055181   15031877   15040645   Local timer interrupts
SPU:          0          0          0          0          0          0          0          0   Spurious interrupts
PMI:          0          0          0          0          0          0          0          0   Performance monitoring interrupts
IWI:          0          0          0          0          0          0          0          0   IRQ work interrupts
RTR:          0          0          0          0          0          0          0          0   APIC ICR read retries
RES:   11090372   11038755   10930158   10893615   10720820   10764828   10820971   10799650   Rescheduling interrupts
CAL:     489090     490382     476821     466129     412115     411556     419411     418516   Function call interrupts
TLB:    1114992    1133312    1120373    1114920    1111730    1125841    1117412    1130362   TLB shootdowns
TRM:          0          0          0          0          0          0          0          0   Thermal event interrupts
THR:          0          0          0          0          0          0          0          0   Threshold APIC interrupts
DFR:          0          0          0          0          0          0          0          0   Deferred Error APIC interrupts
MCE:          0          0          0          0          0          0          0          0   Machine check exceptions
MCP:         63         64         64         64         64         64         64         64   Machine check polls
ERR:          0
MIS:          0
PIN:          0          0          0          0          0          0          0          0   Posted-interrupt notification event
NPI:          0          0          0          0          0          0          0          0   Nested posted-interrupt event
PIW:          0          0          0          0          0          0          0          0   Posted-interrupt wakeup event
```

第一列代表了中断号，其余 CPUx 列代表了每个 CPU 的统计数据，最后几列表示中断控制器类型和触发方式等。

在性能分析中需关注的关键数据：

1. 中断分布不均（CPU负载倾斜），例如某个CPU核心处理的中断次数远高于其他核心。可以尝试启用**中断亲和性（IRQ Balance）**来解决：`systemctl start irqbalance`。
2. 高频率中断（中断风暴）：例如某个设备的中断次数异常高（如网卡小包处理）。可以启用**中断合并（Interrupt Coalescing）**来解决：`ethtool -C eth1 rx-usecs 100   # 每100μs合并一次中断`。
3. 硬件设备中断异常：特定设备（如磁盘、USB）中断频率激增，可能指示硬件故障或驱动问题。可以结合`dmesg`查看硬件错误日志，更新驱动程序或更换硬件。
4. 定时器中断 LOC ：表示每个CPU核心的本地定时器中断次数，用于任务调度和时间统计。如果某个CPU的`LOC`显著偏高：可能该核心承担了过多调度负载。

**/proc/softirqs**



## CPU 性能分析案例

### 分析思路总结

碰到常规问题无法解释的 CPU 使用率情况时，例如系统 CPU 使用率很高，但是用 top 、pidstat 这些命令却看不到 CPU 使用率高的进程，首先要想到有可能是短时应用导致的问题，比如有可能是下面这两种情况：

1. 应用里直接调用了其他二进制程序，这些程序通常运行时间比较短，通过 top 等工具也不容易发现。
2. 应用本身在不停地崩溃重启，而启动过程的资源初始化，很可能会占用相当多的 CPU。

短时应用的运行时间比较短，很难在 top 或者 ps 这类展示系统概要和进程快照的工具中发现。对于这类问题，可以用 perf top -g 来分析系统热点函数和占用 CPU 高的进程。

# I/O性能分析及优化

## Page Cache相关











# 网络性能分析及优化

## TCP相关

###  影响TCP连接建立的系统配置

如下是客户端和服务端建立TCP连接的3次握手过程，红色标注是影响相关步骤的Linux系统配置：

![image-20250315214151797](https://hunk-pic-store.oss-cn-beijing.aliyuncs.com/img/tcp_conn1.png)

`net.ipv4.tcp_syn_retries`：客户端发送第一个 SYN 报文后，如果i没有收到服务端的 SYNACK 报文，则会触发超时重试机制，并且重试次数受到这个配置的限制，默认为6。SYN 重试策略是第 n 次重试等待时间为 2 ^ (n-1)秒，所以默认的6次重试总计会等待 1 + 2 + 4 + 8 + 16 + 32 + 64 = 127（秒），即127秒后产生才 ETIMEOUT 的错误。在网络质量很好的数据中心内部，这种情况一般是服务器本身出了问题，比如服务器下线但是没有通知到客户端，可以考虑将客户端 SYN 重试次数降低，让客户端尽快去尝试连接其它的服务器，减少阻塞时间。

`net.ipv4.tcp_max_syn_backlog`：客户端没有收到 SYNACK 报文，除了上面提到的服务器下线的原因，也有可能是服务端积压了太多的半连接（incomplete）无法及时处理导致的。TCP服务端每收到一个 SYN 报文，就会创建一个半连接，然后把该半连接加入到半连接队列（syn queue）里，syn queue 的长度就是 `tcp_max_syn_backlog` 这个配置项来决定的，当系统中积压的半连接个数超过了该值后，新的 SYN 包就会被丢弃。对于服务器而言，可能瞬间收到非常多的 SYN 报文，所以可以适当地调大该值，以免 SYN 包被丢弃而导致客户端收不到 SYNACK。

`net.ipv4.tcp_syncookies`：服务端积压的半连接较多，也有可能是因为有些恶意的客户端在进行 SYN Flood 攻击，比如客户端高频地向服务端发 SYN 包，并且这个 SYN 包的源 IP 地址不停地变换，那么服务端每次接收到一个新的 SYN 后，都会给它分配一个半连接，服务端的 SYNACK 根据之前的 SYN 包找到的是错误的客户端 IP， 所以也就无法收到客户端的 ACK 包，导致无法正确建立 TCP 连接，这就会让服务端的半连接队列耗尽，无法响应正常的 SYN 包。

为了防止 SYN Flood 攻击，Linux 内核引入了 SYN Cookies 机制。收到 SYN 包时，不去分配资源来保存客户端的信息，而是根据这个 SYN 包计算出一个 Cookie 值，然后将 Cookie 记录到 SYNACK 包中发送出去。对于正常的连接，该 Cookies 值会随着客户端的 ACK 报文被带回来。然后 服务端再根据这个 Cookie 检查这个 ACK 包的合法性，如果合法，才去创建新的 TCP 连接。通过这种处理，SYN Cookies 可以防止部分 SYN Flood 攻击。所以对于 Linux 服务器而言，推荐开启 SYN Cookies：

> net.ipv4.tcp_syncookies = 1

`net.ipv4.tcp_synack_retries`：服务端向客户端发送的 SYNACK 包也可能会被丢弃，或者因为某些原因而收不到客户端的响应，这个时候服务端也会重传 SYNACK 包。同样地，重传的次数也是由配置选项来控制的，该配置选项是 `tcp_synack_retries`，其重传策略和 `tcp_syn_retries` 是一致的。 `tcp_synack_retries`默认值为5，对于数据中心的服务器而言，通常都不需要这么大的值，推荐设置为 2。

`net.core.somaxconn`：客户端收到服务端发送的 SYNACK 报文后发送 ACK 报文，服务端收到客户端的ACK 报文后，TCP三次握手完成，即产生了一个 TCP 全连接（complete），它会被添加到全连接队列（accept queue）中，然后服务端就会调用 `accept() `来完成 TCP 连接的建立。全连接队列（accept queue）的长度也有限制，目的就是为了防止服务端不能及时调用 `accept()` 而浪费太多的系统资源。全连接队列的长度是由 `listen(sockfd, backlog)` 这个函数里的 backlog 控制的，而该 backlog 的最大值则是 somaxconn。somaxconn 在 5.4 之前的内核中，默认都是 128（5.4 开始调整为了默认 4096），建议将该值适当调大一些：

> net.core.somaxconn = 16384

`net.ipv4.tcp_abort_on_overflow`：当服务器中积压的全连接个数超过`net.core.somaxconn`后，新的全连接就会被丢弃掉。服务器在将新连接丢弃时，有的时候需要发送 reset 来通知客户端，这样客户端就不会再次重试了。不过，默认行为是直接丢弃不去通知客户端。至于是否需要给客户端发送 reset，是由 `tcp_abort_on_overflow` 这个配置项来控制的，该值默认为 0，即不发送 reset 给客户端，推荐也是将该值配置为 0。因为，如果服务端来不及调用 `accept()` 而导致全连接队列满，这往往是由瞬间有大量新建连接请求导致的，正常情况下服务端很快就能恢复，然后客户端再次重试后就可以建连成功了。即，将 `tcp_abort_on_overflow` 配置为 0，给了客户端一个重试的机会，也可以根据你的实际情况来决定是否要使能该选项。

`accept()`调用 成功返回后，一个新的 TCP 连接就建立完成了，TCP 连接进入到了 ESTABLISHED 状态：

![image-20250315222631713](https://hunk-pic-store.oss-cn-beijing.aliyuncs.com/img/tcp_conn2.png)

### 影响TCP连接断开的系统配置

如下是TCP连接断开的四次挥手的过程（由客户端主动发起）：

![image-20250315222745709](https://hunk-pic-store.oss-cn-beijing.aliyuncs.com/img/tcp_conn3.png)

首先调用 `close()` 的一侧是 active close（主动关闭），而接收到对端的 FIN 包后再调用 `close()` 来关闭的一侧，称之为 passive close（被动关闭）。在四次挥手的过程中，有三个 TCP 状态需要额外关注，就是上图中深红色的那三个状态：主动关闭方的 `FIN_WAIT_2` 和 `TIME_WAIT`，以及被动关闭方的 `CLOSE_WAIT` 状态，除了 `CLOSE_WAIT` 状态外，其余两个状态都有对应的系统配置项来控制。

`net.ipv4.tcp_fin_timeout`：首先来看 `FIN_WAIT_2` 状态，TCP 进入到这个状态后，如果本端迟迟收不到对端的 FIN 包，那就会一直处于这个状态，于是就会一直消耗系统资源。Linux 为了防止这种资源的开销，设置了这个状态的超时时间 `tcp_fin_timeout`，默认为 60s，超过这个时间后就会自动销毁该连接。至于本端为何迟迟收不到对端的 FIN 包，通常情况下都是因为对端机器出了问题，或者是因为太繁忙而不能及时 `close()`。所以，通常我们都建议将 `tcp_fin_timeout` 调小一些，以尽量避免这种状态下的资源开销。对于数据中心内部的机器而言，将它调整为 2s 足矣：

> net.ipv4.tcp_fin_timeout = 2

`net.ipv4.tcp_max_tw_buckets`：`TIME_WAIT` 状态存在的意义是，最后发送的这个 ACK 包可能会被丢弃掉或者有延迟，这样对端就会再次发送 FIN 包。如果不维持 `TIME_WAIT` 这个状态，那么再次收到对端的 FIN 包后，本端就会回一个 Reset 包，这可能会产生一些异常。所以维持 TIME_WAIT 状态一段时间，可以保障 TCP 连接正常断开。TIME_WAIT 的默认存活时间在 Linux 上是 60s（TCP_TIMEWAIT_LEN），这个时间对于数据中心而言可能还是有些长了，所以有的时候也会修改如下内核配置项将其减小：

> net.ipv4.tcp_max_tw_buckets = 10000

`net.ipv4.tcp_tw_reuse`：客户端关闭跟服务端的连接后，也有可能很快再次跟服务端之间建立一个新的连接，而由于 TCP 端口最多只有 65536 个，如果不去复用处于 `TIME_WAIT` 状态的连接，就可能在快速重启应用程序时，出现端口被占用而无法创建新连接的情况。所以建议打开复用 TIME_WAIT 的选项：

> net.ipv4.tcp_tw_reuse = 1

系统中没有对应的配置项控制 CLOSE_WAIT 状态，但是该状态也是一个危险信号，如果这个状态的 TCP 连接较多，那往往意味着应用程序有 Bug，在某些条件下没有调用 close() 来关闭连接。我们在生产环境上就遇到过很多这类问题。所以，如果我们的系统中存在很多 CLOSE_WAIT 状态的连接，最好去排查一下应用程序，看看哪里漏掉了 close()。