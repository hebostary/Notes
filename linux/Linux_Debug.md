# GDB 调试工具
## GDB 分析多线程死锁
演示程序参考 `src/thread/deadlock.cpp`
```bash
# 一定要用sudo切换到root身份，否则 bt 输出都是空的
hunk@DESKTOP-JFUCNKU:~/workspace/src/github.com/hebostary/Notes/linux$ sudo gdb -p 29188
GNU gdb (Ubuntu 12.1-0ubuntu1~22.04.2) 12.1
Copyright (C) 2022 Free Software Foundation, Inc.
...
(gdb) thread apply all bt #查看所有线程栈顶快照，如果线程太多，可以先用 info threads
(gdb) info threads
  Id   Target Id                                  Frame 
* 1    Thread 0x72099a123740 (LWP 29188) "thread" __futex_abstimed_wait_common64 (private=128, cancel=true, abstime=0x0, op=265, 
    expected=29189, futex_word=0x720999dff910) at ./nptl/futex-internal.c:57
  2    Thread 0x720999dff640 (LWP 29189) "thread" futex_wait (private=0, expected=2, futex_word=0x5a289f0c7080 <mutex2>)
    at ../sysdeps/nptl/futex-internal.h:146
  3    Thread 0x7209995fe640 (LWP 29190) "thread" futex_wait (private=0, expected=2, futex_word=0x5a289f0c7040 <mutex1>)
    at ../sysdeps/nptl/futex-internal.h:146
(gdb) thread 2 # 切到2号线程
[Switching to thread 2 (Thread 0x720999dff640 (LWP 29189))]
#0  futex_wait (private=0, expected=2, futex_word=0x5a289f0c7080 <mutex2>) at ../sysdeps/nptl/futex-internal.h:146
146     ../sysdeps/nptl/futex-internal.h: No such file or directory.
(gdb) frame 3 # 看下栈帧，发现卡在 ___pthread_mutex_lock 这个函数了，函数参数是一个叫做 mutex2 的互斥锁
#3  ___pthread_mutex_lock (mutex=0x5a289f0c7080 <mutex2>) at ./nptl/pthread_mutex_lock.c:93
93      in ./nptl/pthread_mutex_lock.c
(gdb) print mutex2 # 打印这个互斥锁变量
$1 = {__data = {__lock = 2, __count = 0, __owner = 29190, __nusers = 1, __kind = 0, __spins = 0, __elision = 0, __list = {__prev = 0x0, 
      __next = 0x0}}, __size = "\002\000\000\000\000\000\000\000\006r\000\000\001", '\000' <repeats 26 times>, __align = 2}
(gdb) print mutex2.__data.__owner # 这个 29190 就是持有该锁的线程的LWP ID。现在使用 info threads 命令，看看线程ID为 29190 的线程
#正是3号线程，即持有mutex2在等待mutex1的线程
$2 = 29190
(gdb) info threads
  Id   Target Id                                  Frame 
  1    Thread 0x72099a123740 (LWP 29188) "thread" __futex_abstimed_wait_common64 (private=128, cancel=true, abstime=0x0, op=265, 
    expected=29189, futex_word=0x720999dff910) at ./nptl/futex-internal.c:57
* 2    Thread 0x720999dff640 (LWP 29189) "thread" futex_wait (private=0, expected=2, futex_word=0x5a289f0c7080 <mutex2>)
    at ../sysdeps/nptl/futex-internal.h:146
  3    Thread 0x7209995fe640 (LWP 29190) "thread" futex_wait (private=0, expected=2, futex_word=0x5a289f0c7040 <mutex1>)
    at ../sysdeps/nptl/futex-internal.h:146
(gdb) # 如果死锁不常发生，可以对关键的互斥量设置观察点，跟踪其状态变化，例如： watch -l mutex2.__data.__lock
```

也可以用 `thread apply all bt`
```bash
(gdb) thread apply all bt

Thread 3 (Thread 0x7209995fe640 (LWP 29190) "thread"):
#0  futex_wait (private=0, expected=2, futex_word=0x5a289f0c7040 <mutex1>) at ../sysdeps/nptl/futex-internal.h:146
#1  __GI___lll_lock_wait (futex=futex@entry=0x5a289f0c7040 <mutex1>, private=0) at ./nptl/lowlevellock.c:49
#2  0x0000720999e98002 in lll_mutex_lock_optimized (mutex=0x5a289f0c7040 <mutex1>) at ./nptl/pthread_mutex_lock.c:48
#3  ___pthread_mutex_lock (mutex=0x5a289f0c7040 <mutex1>) at ./nptl/pthread_mutex_lock.c:93
#4  0x00005a289f0c44b6 in thread2_func (arg=0x0) at src/thread/deadlock.cpp:50
#5  0x0000720999e94ac3 in start_thread (arg=<optimized out>) at ./nptl/pthread_create.c:442
#6  0x0000720999f26850 in clone3 () at ../sysdeps/unix/sysv/linux/x86_64/clone3.S:81

Thread 2 (Thread 0x720999dff640 (LWP 29189) "thread"):
#0  futex_wait (private=0, expected=2, futex_word=0x5a289f0c7080 <mutex2>) at ../sysdeps/nptl/futex-internal.h:146
#1  __GI___lll_lock_wait (futex=futex@entry=0x5a289f0c7080 <mutex2>, private=0) at ./nptl/lowlevellock.c:49
#2  0x0000720999e98002 in lll_mutex_lock_optimized (mutex=0x5a289f0c7080 <mutex2>) at ./nptl/pthread_mutex_lock.c:48
#3  ___pthread_mutex_lock (mutex=0x5a289f0c7080 <mutex2>) at ./nptl/pthread_mutex_lock.c:93
#4  0x00005a289f0c43ce in thread1_func (arg=0x0) at src/thread/deadlock.cpp:25
#5  0x0000720999e94ac3 in start_thread (arg=<optimized out>) at ./nptl/pthread_create.c:442
#6  0x0000720999f26850 in clone3 () at ../sysdeps/unix/sysv/linux/x86_64/clone3.S:81

Thread 1 (Thread 0x72099a123740 (LWP 29188) "thread"):
#0  __futex_abstimed_wait_common64 (private=128, cancel=true, abstime=0x0, op=265, expected=29189, futex_word=0x720999dff910) at ./nptl/futex-internal.c:57
#1  __futex_abstimed_wait_common (cancel=true, private=128, abstime=0x0, clockid=0, expected=29189, futex_word=0x720999dff910) at ./nptl/futex-internal.c:87
--Type <RET> for more, q to quit, c to continue without paging--
#2  __GI___futex_abstimed_wait_cancelable64 (futex_word=futex_word@entry=0x720999dff910, expected=29189, clockid=clockid@entry=0, abstime=abstime@entry=0x0, private=private@entry=128) at ./nptl/futex-internal.c:139
#3  0x0000720999e96624 in __pthread_clockjoin_ex (threadid=125385561863744, thread_return=0x7fff67add158, clockid=0, abstime=0x0, block=<optimized out>) at ./nptl/pthread_join_common.c:105
#4  0x00005a289f0c46d5 in pthread_deadlock () at src/thread/deadlock.cpp:104
#5  0x00005a289f0c47db in main (argc=2, argv=0x7fff67add2a8) at src/thread/main.cpp:14
(gdb) 
(gdb) 
```

## GDB 生成coredump 文件
有时候我们不方便用gdb长期attach到生产环境的进程上进行分析，可以如下先生成coredump文件后进行离线分析：
```bash
$ gdb -p [PID]

# 在gdb中生成coredump文件，这两种方式差不多，都不会导致进程退出：
(gdb) generate-core-file
warning: target file /proc/29188/cmdline contained unexpected null characters
Saved corefile core.29188
# 或
(gdb) gcore core.[PID]

(gdb) quit

# 使用gdb分析coredump，这里的程序路径必须准确，即使不正确GDB也不会报错，只是后面看不到详细的调试符号信息
$ gdb [程序路径] [coredump文件]

# 例如，我们用这种方式调试前面的死锁程序，后面info threads打印的线程信息和直接 gdb attach 操作是完全一样的
hunk@DESKTOP-JFUCNKU:~/workspace/src/github.com/hebostary/Notes/linux$ sudo gdb bin/thread core.29188 
...
Reading symbols from bin/thread...
[New LWP 29188]
[New LWP 29189]
[New LWP 29190]
[Thread debugging using libthread_db enabled]
Using host libthread_db library "/lib/x86_64-linux-gnu/libthread_db.so.1".
--Type <RET> for more, q to quit, c to continue without paging--
Core was generated by `./bin/thread'.
#0  __futex_abstimed_wait_common64 (private=128, cancel=true, abstime=0x0, op=265, expected=29189, futex_word=0x720999dff910)
    at ./nptl/futex-internal.c:57
57      ./nptl/futex-internal.c: No such file or directory.
[Current thread is 1 (Thread 0x72099a123740 (LWP 29188))]
(gdb) info threas
Undefined info command: "threas".  Try "help info".
(gdb) info threads
  Id   Target Id                         Frame 
* 1    Thread 0x72099a123740 (LWP 29188) __futex_abstimed_wait_common64 (private=128, cancel=true, abstime=0x0, op=265, expected=29189, 
    futex_word=0x720999dff910) at ./nptl/futex-internal.c:57
  2    Thread 0x720999dff640 (LWP 29189) futex_wait (private=0, expected=2, futex_word=0x5a289f0c7080 <mutex2>)
    at ../sysdeps/nptl/futex-internal.h:146
  3    Thread 0x7209995fe640 (LWP 29190) futex_wait (private=0, expected=2, futex_word=0x5a289f0c7040 <mutex1>)
    at ../sysdeps/nptl/futex-internal.h:146
(gdb) 
```
