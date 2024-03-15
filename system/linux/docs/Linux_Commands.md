

# 参考资料

Linux命令网页工具：

1. [Linux工具快速教程](https://www.bookstack.cn/read/linuxtools/README.md)

2. [Linux命令搜索](https://wangchujiang.com/linux-command/)

# 基本命令

## alias

```bash
$ alias ll='ls -l'
```

## vim

```bash
:set fileformat #查看文件格式，在文件格式错误时，常常出现程序读取文件内容异常，多出来一些特殊字符
:set fileformat=unix/dos #设置文件格式
:set list/nolist #设置是否显示不可见字符，比如换行符
:%s/\n//g #删除所有的换行符
```

## grep

```bash
$ grep -E '^ENABLE|^BPCD' log.text    #查找出文件中以ENABLE或者BPCD开头的行
$ grep "ENABLE" log.text | grep BPCD  #查找同时包含ENABLE和BPCD的行
```

## awk

## sed

```bash
# 在匹配行后面插入新行
$ sed 's/^anothervalue=.*/replace=me/g' test.txt
$ sed "s/^anothervalue=.*/replace=${value}/g" test.txt
```

# 用户管理

## 创建删除用户

```bash
$ groupadd -g 200 nexus  		    #添加指定gid的组
$ useradd -u 200 -g nexus nexus   #添加指定uid的用户
$ usermod -G root nexus 		    #修改用户加入root组
```

## 查询用户信息



# 进程/服务管理

## ps

```bash
ps -H -p <pid>         #查看进程创建的线程
ps -p <pid> -o etime   #查看进程运行时间
```

## top

关于`top	`命令输出字段的解释和更多用法可以参考：[top linux下的任务管理器](https://www.bookstack.cn/read/linuxtools/22.md)

```bash
top -H                #查看系统所有线程
top -H -p <pid>       #查看进程内线程运行情况
```

## strace

`strace`常用来跟踪进程执行时的系统调用和所接收的信号。 在Linux世界，进程不能直接访问硬件设备，当进程需要访问硬件设备(比如读取磁盘文件，接收网络数据等等)时，必须由用户态模式切换至内核态模式，通过系统调用访问硬件设备。`strace`可以跟踪到一个进程产生的系统调用，包括参数、返回值、执行消耗的时间。参考[strace 跟踪进程中的系统调用](https://www.bookstack.cn/read/linuxtools/20.md)。

`stace`支持的参数：

```bash
-c 统计每一系统调用的所执行的时间,次数和出错的次数等.
-d 输出strace关于标准错误的调试信息.
-f 跟踪由fork调用所产生的子进程.
-ff 如果提供-o filename,则所有进程的跟踪结果输出到相应的filename.pid中,pid是各进程的进程号.
-F 尝试跟踪vfork调用.在-f时,vfork不被跟踪.
-h 输出简要的帮助信息.
-i 输出系统调用的入口指针.
-q 禁止输出关于脱离的消息.
-r 打印出相对时间关于,,每一个系统调用.
-t 在输出中的每一行前加上时间信息.
-tt 在输出中的每一行前加上时间信息,微秒级.
-ttt 微秒级输出,以秒了表示时间.
-T 显示每一调用所耗的时间.
-v 输出所有的系统调用.一些调用关于环境变量,状态,输入输出等调用由于使用频繁,默认不输出.
-V 输出strace的版本信息.
-x 以十六进制形式输出非标准字符串
-xx 所有字符串以十六进制形式输出.
-a column
设置返回值的输出位置.默认 为40.
-e expr
指定一个表达式,用来控制如何跟踪.格式如下:
[qualifier=][!]value1[,value2]...
qualifier只能是 trace,abbrev,verbose,raw,signal,read,write其中之一.value是用来限定的符号或数字.默认的 qualifier是 trace.感叹号是否定符号.例如:
-eopen等价于 -e trace=open,表示只跟踪open调用.而-etrace!=open表示跟踪除了open以外的其他调用.有两个特殊的符号 all 和 none.
注意有些shell使用!来执行历史记录里的命令,所以要使用\\.
-e trace=set
只跟踪指定的系统 调用.例如:-e trace=open,close,rean,write表示只跟踪这四个系统调用.默认的为set=all.
-e trace=file
只跟踪有关文件操作的系统调用.
-e trace=process
只跟踪有关进程控制的系统调用.
-e trace=network
跟踪与网络有关的所有系统调用.
-e strace=signal
跟踪所有与系统信号有关的 系统调用
-e trace=ipc
跟踪所有与进程通讯有关的系统调用
-e abbrev=set
设定 strace输出的系统调用的结果集.-v 等与 abbrev=none.默认为abbrev=all.
-e raw=set
将指 定的系统调用的参数以十六进制显示.
-e signal=set
指定跟踪的系统信号.默认为all.如 signal=!SIGIO(或者signal=!io),表示不跟踪SIGIO信号.
-e read=set
输出从指定文件中读出 的数据.例如:
-e read=3,5
-e write=set
输出写入到指定文件中的数据.
-o filename
将strace的输出写入文件filename
-p pid
跟踪指定的进程pid.
-s strsize
指定输出的字符串的最大长度.默认为32.文件名一直全部输出.
-u username
以username 的UID和GID执行被跟踪的命令
```

常见用法：

```bash
# 跟踪可执行程序，-f -F选项告诉strace同时跟踪fork和vfork出来的进程，-o选项把所有strace输出写到~/straceout.txt里
$ strace -f -F -o ~/straceout.txt <command path>

# 跟踪一个正在运行的进程（有些版本不支持）
$ strace -o output.txt -T -tt -e trace=all -p 28979

# 只跟踪指定的系统调用
$ strace -e trace=network,read,write 

# 统计指定系统调用的数据
$ strace -p 26308 -e trace=network,read,write -c
strace: Process 26308 attached
^Cstrace: Process 26308 detached
% time     seconds  usecs/call     calls    errors syscall
------ ----------- ----------- --------- --------- ----------------
 44.84    0.000317           9        32           accept
 28.43    0.000201           6        32           write
 26.73    0.000189           2        64           setsockopt
------ ----------- ----------- --------- --------- ----------------
100.00    0.000707                   128           total
```

## pstack 

`pstack`是 Linux 系统下的一个命令行工具，此命令可以显示指定进程每个线程的堆栈快照，便于排查程序异常和性能评估。`pstack` 是基于 gdb 实现的，通过 man pstack 可以发现，它其实是 gstack 的一个软链接。

在定位死锁问题时，我们可以多次执行` pstack` 命令查看线程的函数调用过程，多次对比结果，确认哪几个线程一直没有变化，且是因为在等待锁，那么大概率是由于死锁问题导致的。

具体用法可以参考[Debug 利器：pstack & strace](https://www.cnblogs.com/chenxinshuo/p/11986858.html)

## gdb

### pstack & gdb排查死锁问题

下面案例的原文链接：[利用工具排查死锁问题](https://xiaolincoding.com/os/4_process/deadlock.html#利用工具排查死锁问题)

```bash
// gdb 命令，已经通过pstack命令发现87746这个进程大概率发生了死锁问题
$ gdb -p 87746

// 打印所有的线程信息
(gdb) info thread
  3 Thread 0x7f60a610a700 (LWP 87747)  0x0000003720e0da1d in __lll_lock_wait () from /lib64/libpthread.so.0
  2 Thread 0x7f60a5709700 (LWP 87748)  0x0000003720e0da1d in __lll_lock_wait () from /lib64/libpthread.so.0
* 1 Thread 0x7f60a610c700 (LWP 87746)  0x0000003720e080e5 in pthread_join () from /lib64/libpthread.so.0
//最左边的 * 表示 gdb 锁定的线程，切换到第二个线程去查看

// 切换到第2个线程
(gdb) thread 2
[Switching to thread 2 (Thread 0x7f60a5709700 (LWP 87748))]#0  0x0000003720e0da1d in __lll_lock_wait () from /lib64/libpthread.so.0 

// bt 可以打印函数堆栈，却无法看到函数参数，跟 pstack 命令一样。threadB_proc 可以看出这是线程 B
(gdb) bt
#0  0x0000003720e0da1d in __lll_lock_wait () from /lib64/libpthread.so.0 
#1  0x0000003720e093ca in _L_lock_829 () from /lib64/libpthread.so.0
#2  0x0000003720e09298 in pthread_mutex_lock () from /lib64/libpthread.so.0
#3  0x0000000000400792 in threadB_proc (data=0x0) at dead_lock.c:25
#4  0x0000003720e07893 in start_thread () from /lib64/libpthread.so.0
#5  0x00000037206f4bfd in clone () from /lib64/libc.so.6

// 打印第三帧信息，每次函数调用都会有压栈的过程，而 frame 则记录栈中的帧信息
(gdb) frame 3
#3  0x0000000000400792 in threadB_proc (data=0x0) at dead_lock.c:25
27    printf("thread B waiting get ResourceA \n");
28    pthread_mutex_lock(&mutex_A);

// 打印mutex_A的值 ,  __owner表示gdb中标示线程的值，即LWP
// 可以看到它被 LWP 为 87747（线程 A） 的线程持有着；
(gdb) p mutex_A
$1 = {__data = {__lock = 2, __count = 0, __owner = 87747, __nusers = 1, __kind = 0, __spins = 0, __list = {__prev = 0x0, __next = 0x0}}, 
  __size = "\002\000\000\000\000\000\000\000\303V\001\000\001", '\000' <repeats 26 times>, __align = 2}

// 打印mutex_B的值 ,  __owner表示gdb中标示线程的值，即LWP
// 可以看到他被 LWP 为 87748 （线程 B） 的线程持有着；
(gdb) p mutex_B
$2 = {__data = {__lock = 2, __count = 0, __owner = 87748, __nusers = 1, __kind = 0, __spins = 0, __list = {__prev = 0x0, __next = 0x0}}, 
  __size = "\002\000\000\000\000\000\000\000\304V\001\000\001", '\000' <repeats 26 times>, __align = 2}  
```

从上面的过程可以看出，线程 B持有锁mutex_B在等待mutex_A，而线程 A持有锁mutex_A在等待mutex_B，这是典型的死锁问题。

## systemd

### systemd-analyze

`systemd-analyze`可用于确定系统启动性能统计数据，并从系统和服务管理器检索其他状态和跟踪信息。 

```bash
$ systemd-analyze blame

# 打印在内核中花费的时间
$ systemd-analyze time
Startup finished in 9.756s (kernel) + 25.122s (initrd) + 2min 41.906s (userspace) = 3min 16.785s
```

# 软件包管理

## rpm包

**基础操作 **

```bash
$ rpm2cpio <name>.rpm | cpio -div  #解压rpm
$ rpm -qf <file>     #查询文件属于哪个包
$ rpm -ql <package>  #查询rpm包安装了哪些文件
$ rpm -qi <package>  #查询rpm包属性信息

#查询包的依赖关系
$ rpm -qpR <package.rpm>  #查看包是否依赖其它包
$ rpm -e --test <package> #测试包是否被其它包依赖
```

**构建rpm包**

## xz包

```bash
#解压tar.xz包
$ xz -d xxx.tar.xz -> xxx.tar
$ tar xvf xxx.tar
```

## bz2包

```bash
$ bunzip2 all.bz2
$ tar -xjf valgrind-3.15.0.tar.bz2
```

## gz包

```bash
# 解压gz包
$ gunzip messages-20220731.gz 
```

## ISO管理



# 存储资源管理

## 存储设备

### lsblk 

Usage - list block devices

```bash
$ lsblk 
NAME              MAJ:MIN   RM   SIZE RO TYPE MOUNTPOINT
loop0               7:0      0   128M  0 loop /data/head
loop1               7:1      0   128M  0 loop /data/map
sda                 8:0      0  13.7T  0 disk 
└─sda3              8:3      0  13.7T  0 part 
sdb                 8:16     0 930.4G  0 disk 
├─sdb1              8:17     0   512G  0 part 
│ └─systemlog-log 253:8      0   512G  0 lvm  /log
└─sdb2              8:18     0    20G  0 part 
  └─systemiso-iso 253:7      0    20G  0 lvm  
sdc                 8:32     0 930.4G  0 disk 
├─sdc1              8:33     0   500M  0 part /boot
└─sdc2              8:34     0 929.9G  0 part 
  ├─system-root   253:0      0  90.9G  0 lvm  /
  ├─system-swap   253:1      0  63.6G  0 lvm  [SWAP]
  ├─system-home   253:2      0    10G  0 lvm  /home
  └─system-inst   253:6      0  48.8G  0 lvm  /inst
```

### blkid

```bash
$ blkid     #查看磁盘及其UUID
/dev/mapper/systemlog-log: UUID="a0267a0f-9bbd-44ce-a11c-bbe750a39005" BLOCK_SIZE="4096" TYPE="ext4"
/dev/sdb1: UUID="nMl6pe-HLDM-4qNC-x1Nr-eCeU-TQxt-StaGmF" TYPE="LVM2_member" PARTUUID="571f617b-01"
...
```

### lsscsi 

Usage - list SCSI devices (or hosts), list NVMe devices

```bash
$ lsscsi
[0:2:0:0]    disk    Intel    RMS3CC080        4.68  /dev/sda 
[1:0:0:0]    disk    SEAGATE  ST1000NM0023     E007  -        
[1:0:1:0]    disk    SEAGATE  ST1000NM0023     E007  -        
[1:0:2:0]    disk    SEAGATE  ST1000NM0023     E007  -        
[1:0:3:0]    disk    SEAGATE  ST1000NM0023     E007  -        
[1:1:0:0]    disk    LSI      Logical Volume   3000  /dev/sdc 
[1:1:1:0]    disk    LSI      Logical Volume   3000  /dev/sdb
```

### dd - vitual block file

```bash
# 创建指定大小的临时文件
$ dd if=/dev/zero of=/msdp/cat/test_file bs=4M count=1024

# 清空硬盘或者分区
$ dd if=dev/zero of=/dev/sdb

# 备份和还原整个硬盘
$ dd if=/dev/sda1 of=/root/sda1.bak
$ dd if=/dev/sda1.bak of=/root/sda1

# 备份和还原硬盘分区表
$ dd if=/dev/sda1 of=/root/sda1.mbr.bak bs=512 count=1
$ dd if=/dev/sda1.mbr.bak of=/root/sda1
```

### sg3_utils

**rescan-scsi-bus.sh**

### losetup - loop device

`loop device`是Linux提供的一种特殊的伪设备机制，它允许我们把普通文件（一般用`dd`命令创建）当做虚拟的块设备使用，既然是块设备，就可以在上面创建文件系统然后挂载使用，即实现了在文件系统中嵌套子文件系统。比如用于安装操作系统的ISO文件本身就包含一个文件系统，因此可以作为`loop device`直接挂载，然后我们就可以直接访问ISO里的所有文件。

`loop device`最主要的好处在于可以提供永久的数据隔离。

`losetup`命令用于管理loop device，下面是创建并使用loop device的demo：

```bash
$ dd if=/dev/zero of=VirtBlock.img bs=100M count=3
3+0 records in
3+0 records out
314572800 bytes (315 MB, 300 MiB) copied, 0.293769 s, 1.1 GB/s
$ du -sh VirtBlock.img 
300M    VirtBlock.img

# -P - will force the kernel to scan the partition table on the newly created loop device.
$ losetup -fP VirtBlock.img 
$ losetup -a
/dev/loop2: [64770]:6159 (/home/hunk/VirtBlock.img)

$ mkfs.ext4 /home/hunk/VirtBlock.img
mke2fs 1.45.6 (20-Mar-2020)
Discarding device blocks: done                            
Creating filesystem with 307200 1k blocks and 76912 inodes
Filesystem UUID: 297cfdae-4d6b-424d-b5da-d3bc3b3e39d8
Superblock backups stored on blocks: 
        8193, 24577, 40961, 57345, 73729, 204801, 221185

Allocating group tables: done                            
Writing inode tables: done                            
Creating journal (8192 blocks): done
Writing superblocks and filesystem accounting information: done 

$ mkdir /loopfs
$ mount -o loop /home/hunk/VirtBlock.img /loopfs
# lsblk命令也可以看到挂载的loop device
$ mount | grep loop
/dev/loop2 on /loopfs type ext4 (rw,relatime)
$ ls -al /home/hunk
...
-rw-r--r--   1 root root 314572800 Feb 20 01:31 VirtBlock.img
...
$ echo "hello" > /loopfs/test.txt
```

删除loop device：

```bash
$ umount /loopfs
$ rmdir /loopfs
$ losetup -d /dev/loop
loop0         loop1         loop2         loop3         loop4         loop5         loop-control  
$ losetup -d /dev/loop2 
$ rm /home/hunk/VirtBlock.img
```

### nvme

`nvme`命令是一个用户空间实用程序，为NVM-Express驱动器提供符合标准的工具。它是专门为Linux制作的，因为它依赖于主线内核驱动程序定义的`IOCTLS`。该实用程序具有用于规范中定义的所有admin和io命令以及用于显示控制器寄存器的子命令。

#### nvme-discover - Discover NVMeoF subsystems

Send one or more Get Log Page requests to a `NVMe-over-Fabrics` Discovery Controller.

The NVMe-over-Fabrics specification defines the concept of a `Discovery Controller`  that an NVMe Host can query on a fabric network to discover NVMe subsystems contained in NVMe Targets which it can connect to on the network. The Discovery Controller will return Discovery Log Pages that provide the NVMe Host with specific information (such as network address and unique subsystem NQN) the NVMe Host can use to issue an NVMe connect command to connect itself to a storage resource contained in that NVMe subsystem on the NVMe Target.

```bash
# nvme help discover
$ nvme discover -t rdma -a 192.168.30.21
```

#### nvme-connect - Connect to NVMeoF subsystem

Create a transport connection to a remote system (specified by --traddr and --trsvcid) and create a NVMe over Fabrics controller for the NVMe subsystem specified by the --nqn option.

```bash
# nvme help connect
$ nvme connect -t rdma -a 192.168.20.21 -s 4420 -n nqn.1992-05.com.wdc.afaapp:nvme.10 -i 1

$ nvme list-subsys
```

#### name-connect-all - Discover and Connect to NVMeoF subsystems

### StorCLI



## LVM

### lvextend

```bash
$ lvextend -L +42G /dev/mapper/vg-inst 
$ resize2fs -p /dev/mapper/vg-inst

# 也可以使用lvresize
$ lvresize --resizefs --size 50G /dev/mapper/vg-inst 
```

扩容时可能出现的问题：Snapshot origin volumes can be resized only while inactive: try lvchange -an Run `lvextend --help' for more information. 解决办法：

```bash
$ umount /dev/mapper/vg-inst
$ lvchange -an /dev/mapper/vg-inst
$ lvextend -L +30G /dev/mapper/vg-inst
$ lvchange -ay /dev/mapper/vg-inst
$ mount /dev/mapper/vg-inst
$ resize2fs /dev/mapper/vg-inst
```

### lvreduce

> 缩小逻辑卷是数据损坏的最高风险，尽量提前做好容量规划，避免卷缩容操作。

```bash
$ umount /int
$ e2fsck -f /dev/mapper/vg-inst
$ lvreduce -L -12G /dev/mapper/vg-inst
$ e2fsck -f /dev/mapper/vg-inst
$ mount /inst
```

## mdadm - Software RAID

[A guide to mdadm](**Linux Block IO: Introducing Multi-queue SSD Access on Multi-core Systems**)

[Using mdadm to send e-mail alerts for RAID failures](https://www.suse.com/support/kb/doc/?id=000016716)

## 文件系统

### findmnt

```bash
# 查找一个挂载的文件系统
$ findmnt -n -F /etc/fstab -o SOURCE,TARGET,FSTYPE,OPTIONS,FREQ,PASSNO /dev/mapper/system-vol
```

### 软/硬链接

```bash
$ ln -sf <source file> <target file>  #创建软链接
```

### NFS

`Ubuntu`系统参考文档：[Network File System (NFS)](https://ubuntu.com/server/docs/service-nfs)

```bash
$ systemctl status  nfs-kernel-server.service
$ systemctl start  nfs-kernel-server.service
$ mount host.cdc.domain.com:/home/nfs /Users/hunk/work/nfs
```

### split

```bash
$ split -l 200 systemlog #切割后的每个小文件包含200行
$ split  -b 200M /var/log/1.log  1.log.split # 将1.log切割成200MB的文件，1.log.split是切割后文件的前缀
$ split -d -b 200M httpd.log log # Split the file and name it with numbers
```

# 网络管理

## curl

```bash
$ curl POST --tlsv1.2 --insecure -H "Content-Type":"application/json" -v https://localhost:8446/ascws/collectordata -d '{"componentList":[{"id":"M.2 SATA SSD","properties":{"capacity":"64.02GB","device_path":"/dev/sdcg"},"protocol":"ATA","type":"SSD"}],"hostname":"eagapp0088.domain.com","pluginName":"generalSSDMon"}'

# via http proxy
$ curl -X POST -v --proxy http://nbpipeline-comn.domain.com:3128 http://localhost:8080/receiver/telemetry -d '{\"uid\":\"NNG05161410042\",\"type\":\"configuration\",\"timestamp\":\"2022-10-25T19:49:59.987-07:00\",\"data\":{\"HB_SEQ\":1,\"HB_TYPE\":\"CONFIG\"},\"metaData\":{\"productName\":\"NetBackup\",\"productVersion\":\"5.1.1\"}}'

# via https proxy
$ curl -X POST -v --proxy https://proxy.com:3129 --proxy-user genesis:P@ssw0rd --proxy-tlsv1 --proxy-insecure  http://10.10.1.1:8080/receiver/telemetry -d '{\"uid\":\"NNG05161410042\",\"type\":\"configuration\",\"timestamp\":\"2022-10-25T19:49:59.987-07:00\",\"data\":{\"HB_SEQ\":1,\"HB_TYPE\":\"CONFIG\"},\"metaData\":{\"productName\":\"NetBackup\",\"productVersion\":\"5.1.1\"}}'
```



## iptables

[全网最好文章：iptables详解（1）：iptables概念](http://www.zsythink.net/archives/1199/)

## DNS

## nmcli

## ethtool

## nmap

```bash
$ nmap 110.18.146.124
Starting Nmap 7.70 ( https://nmap.org ) at 2023-04-18 15:20 CST
Nmap scan report for net146-host124.domain.com (110.18.146.124)
Host is up (0.00016s latency).
Not shown: 998 closed ports
PORT    STATE SERVICE
22/tcp  open  ssh
111/tcp open  rpcbind
MAC Address: FF:50:56:9E:DC:BF (VMware)

Nmap done: 1 IP address (1 host up) scanned in 1.74 seconds

$ nmap 110.18.146.124 -p 22       #扫描特定TCP端口
$ nmap 110.18.146.124 -p 22 -sU   #扫描特定UDP端口
```





[nmap扫描结果的6种端口状态](https://www.cnblogs.com/Rain99-/p/12882677.html)

## tcpdump

参考：https://www.tcpdump.org

```bash
# Older versions of tcpdump truncate packets to 68 or 96 bytes. If this is the case, use -s to capture full-sized packets:
$ tcpdump -i <interface> -s 65535 -v -w <file>
$ tcpdump -vv -A -T snmp -s 0 "(dst port 162) and (host <ip address>)" -w /tmp/tcpdump.1

# Capture traffic from localhost to localhost
$ tcpdump -i lo src host localhost and dst host localhost and src port 8449 or dst port 8449 -v -w /tmp/tcpdump.2
```

## nc

```bash
$ nc -z -v [hostname/IP address] [port number] #测试tcp端口的连通性
$ nc -z -v -u [hostname/IP address] [port number] #测试udp端口的连通性
```

## netstat

```bash
$ netstat -n | awk '/^tcp/' #查看所有tcp连接及状态
```

## ssh

### 配置ssh数字签名登陆（免用户密码）

示例：host 1使用数字签名登录到host 2。

**Step 1**：生成rsa公钥和私钥：

```bash
[ansible@host1 ~]$ ssh-keygen -t rsa
Generating public/private rsa key pair.
Enter file in which to save the key (/home/ansible/.ssh/id_rsa): 
Created directory '/home/ansible/.ssh'.
Enter passphrase (empty for no passphrase): 
Enter same passphrase again: 
Your identification has been saved in /home/ansible/.ssh/id_rsa.
Your public key has been saved in /home/ansible/.ssh/id_rsa.pub.
The key fingerprint is:
SHA256:PNoGmN0aj/RYUbRRnbnIZEH4w6sS/mG7iPAX3MUnS7E ansible@host1.cdc.domain.com
```

**Step 2**：将host 1的rsa公钥添加到host 2的ssh配置文件`authorized_keys`中：

```bash
[ansible@host2 ~]$ scp ansible@10.200.8.27:/home/ansible/.ssh/id_rsa.pub .
[ansible@host2 ~]$ cat id_rsa.pub > /home/ansible/.ssh/authorized_keys
#要保证.ssh和authorized_keys都只有用户自己有写权限。否则验证无效。否则会报下面的错误：
#The authenticity of host 'host2 ' can't be established.
[ansible@host2 ~]$ chmod 700 .ssh
[ansible@host2 ~]$ chmod 600 .ssh/authorized_keys
```

**Step 3**：ssh登录

```bash
[ansible@host1 ~]$ ssh host2
```

# 密钥安全

## OpenSSL

用openssl命令启动一个TLS服务器：

```bash
$ openssl s_server -key /etc/pki/tls/certs/key.pem -cert /etc/pki/tls/certs/cert.pem -accept 8090 -cipher ALL
```

用openssl命令作为客户端访问TLS 服务器：

```bash
$ openssl s_client -connect <hostname|ip>:port
$ openssl s_client -starttls smtp -connect <hostname|ip>:port

# 用gdb调试openssl的源代码：
# 	需要先安装一个调试包： openssl-debugsource-1.1.1k-7.el8_6.x86_64 
$ gdb openssl
(gdb) set args s_client -starttls smtp -connect <SMTPS server>:587
```

用openssl命令统计当前系统计算MD6和SHA512的性能：

```bash
$ openssl speed md5 sha512
Doing md5 for 3s on 16 size blocks: 20441721 md5's in 2.98s
Doing md5 for 3s on 64 size blocks: 11590977 md5's in 2.99s
Doing md5 for 3s on 256 size blocks: 5129948 md5's in 2.99s
Doing md5 for 3s on 1024 size blocks: 1578697 md5's in 2.99s
Doing md5 for 3s on 8192 size blocks: 213110 md5's in 2.98s
Doing md5 for 3s on 16384 size blocks: 105312 md5's in 2.99s
Doing sha512 for 3s on 16 size blocks: 8035609 sha512's in 2.99s
Doing sha512 for 3s on 64 size blocks: 8044063 sha512's in 2.99s
Doing sha512 for 3s on 256 size blocks: 3512538 sha512's in 2.99s
Doing sha512 for 3s on 1024 size blocks: 1329922 sha512's in 2.98s
Doing sha512 for 3s on 8192 size blocks: 194753 sha512's in 2.99s
Doing sha512 for 3s on 16384 size blocks: 97766 sha512's in 2.99s
OpenSSL 1.1.1k  FIPS 25 Mar 2021
built on: Wed Feb  8 16:12:48 2023 UTC
options:bn(64,64) md2(char) rc4(16x,int) des(int) aes(partial) idea(int) blowfish(ptr) 
compiler: gcc -fPIC -pthread -m64 -Wa,--noexecstack -Wall -O3 -O2 -g -pipe -Wall -Werror=format-security -Wp,-D_FORTIFY_SOURCE=2 -Wp,-D_GLIBCXX_ASSERTIONS -fexceptions -fstack-protector-strong -grecord-gcc-switches -specs=/usr/lib/rpm/redhat/redhat-hardened-cc1 -specs=/usr/lib/rpm/redhat/redhat-annobin-cc1 -m64 -mtune=generic -fasynchronous-unwind-tables -fstack-clash-protection -fcf-protection -Wa,--noexecstack -Wa,--generate-missing-build-notes=yes -specs=/usr/lib/rpm/redhat/redhat-hardened-ld -DOPENSSL_USE_NODELETE -DL_ENDIAN -DOPENSSL_PIC -DOPENSSL_CPUID_OBJ -DOPENSSL_IA32_SSE2 -DOPENSSL_BN_ASM_MONT -DOPENSSL_BN_ASM_MONT5 -DOPENSSL_BN_ASM_GF2m -DSHA1_ASM -DSHA256_ASM -DSHA512_ASM -DKECCAK1600_ASM -DRC4_ASM -DMD5_ASM -DAESNI_ASM -DVPAES_ASM -DGHASH_ASM -DECP_NISTZ256_ASM -DX25519_ASM -DPOLY1305_ASM -DZLIB -DNDEBUG -DPURIFY -DDEVRANDOM="\"/dev/urandom\"" -DSYSTEM_CIPHERS_FILE="/etc/crypto-policies/back-ends/openssl.config"
The 'numbers' are in 1000s of bytes per second processed.
type             16 bytes     64 bytes    256 bytes   1024 bytes   8192 bytes  16384 bytes
md5             109754.21k   248101.18k   439219.63k   540664.12k   585837.96k   577067.49k
sha512           42999.91k   172180.61k   300739.04k   456993.33k   533584.14k   535718.44k
```



# 内核管理

## SystemTap



## crash - 调试vmcore

```bash
$ crash /usr/lib/debug/lib/modules/4.18.0-372.32.1.el8_6.x86_64/vmlinux vmcore

# 关于bt的用法细节可以参考https://www.cnblogs.com/pengdonglin137/p/16046329.html
$ crash> bt # 查看调用栈（默认查看的导致crash的进程的调用栈）
# bt     displays a task's kernel-stack backtrace.  If it is given
#              the -a option, it displays the stack traces of the active
#              tasks on all CPUs.  It is often used with the foreach
#              command to display the backtraces of all tasks with one command.

$ crash> rd <address> # displays the contents of memory, with the output formatted
                      # in several different manners.


$ crash> dis  #  disassembles memory, either entire kernel functions, from
              # a location for a specified number of instructions, or from
              # the start of a function up to a specified memory location.
$ crash> dis ffffffff87560b20 #反汇编命令
$ crash> dis -l __x64_sys_openat+5 # -l可以展示原代码行

$ crash> sym   # translates a symbol to its virtual address, or a static
               # kernel virtual address to its symbol -- or to a symbol-
               # plus-offset value, if appropriate.
$ crash> sys   # displays system-specific data.
$ crash> log / dmesg # 打印出故障现场的kmsg缓冲区log_buf中的内容
$ crash> struct # 展示结构体的定义，或者从指定的地址开始解析一个结构体
$ crash> union # 与struct类似，用于展示union
$ crash> p # print某个变量的值，实际上是调用gdb的p命令
$ crash> whatis # 展示结构体，联合体等定义
$ crash> ps #打印系统中的进程状态，和正常系统运行时的ps命令类似
$ crash> task <pid> # 打印某个pid的task_struct内容，不加pid则表示当前进程
$ crash> mount #展示当前挂载的文件系统的命令
$ crash> file <pid> # 查看某个进程中的所有打开文件
$ crash> net # 展示网络相关的信息
$ crash> search -t <value/symbol> # 子啊所有进程的stack页面中查找一个value或者一个symbol并打印结果

$ crash> list  # displays the contents of a linked list.

$ crash> vtop  # translates a user or kernel virtual address to its
               # physical address.
$ crash> kmem   displays information about the use of kernel memory.
```

# 主板管理

## flashupdt - Intel

如果是Intel的主板，可以用这个命令查询和修改主板上的一些信息，包括BIOS和Firmware：

```bash
# 显示BIOS和固件信息
$ /usr/bin/flashupdt/flashupdt -i

# 设置产品名称
$ /usr/bin/flashupdt/flashupdt -set product Pn "Company Appliance XXX"

# 设置产品制造商名字
$ /usr/bin/flashupdt/flashupdt -set product Mn "Company"
```

## ipmitool

```bash
$ ipmitool --help

$ ipmitool -I lanplus -H <IPMI host> -U <IPMI user> -P <password> power status
$ ipmitool -I lanplus -H <IPMI host> -U <IPMI user> -P <password> power on
$ ipmitool -I lanplus -H <IPMI host> -U <IPMI user> -P <password> power off

# User manageent
$ ipmitool user list 3
$ ipmitool -I lanplus -H <IPMI host> -U <IPMI user> -P <password> user list 3
$ ipmitool user set password `ipmitool user list 3|grep sysadmin | awk '{print $1}'` P@ssw0rd 20

# Configure LAN Channels
$ ipmitool lan print 1
$ ipmitool -I lanplus -H <IPMI host> -U <IPMI user> -P <password> lan print
$ ipmitool -I lanplus -H <IPMI host> -U <IPMI user> -P <password> lan print 1

$ ipmitool -I lanplus -H <IPMI host> -U <IPMI user> -P <password> bmc reset cold

# Print detailed sensor information
$ ipmitool -I lanplus -H <IPMI host> -U <IPMI user> -P <password> sensor
```

