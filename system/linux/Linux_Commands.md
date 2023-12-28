# 基本命令

## alias

```bash
$ alias ll='ls -l'
```

# 三剑客grep|awk|sed

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

# 服务管理

## systemd

### systemd-analyze

`systemd-analyze`可用于确定系统启动性能统计数据，并从系统和服务管理器检索其他状态和跟踪信息。 

```bash
$ systemd-analyze blame

# 打印在内核中花费的时间
$ systemd-analyze time
Startup finished in 9.756s (kernel) + 25.122s (initrd) + 2min 41.906s (userspace) = 3min 16.785s
```



# 进程管理

## ps命令

```bash
ps -H -p <pid>         #查看进程创建的线程
ps -p <pid> -o etime   #查看进程运行时间
```

## top命令

```bash
top -H                #查看系统所有线程
top -H -p <pid>       #查看进程内线程运行情况
```

## strace命令

```bash
$ strace <command path>
$ strace -p <pid> # attach到一个进程上，有些版本不支持
$ strace -e trace=network,read,write # 只看指定的系统调用

$ strace -p 26308 -e trace=network,read,write -c # 统计系统调用的数据
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

# 软件包管理

## rpm包管理

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



# 存储管理

## 存储设备

### 磁盘命令

```bash
$ blkid     #查看磁盘及其UUID
$ lsscsi
```

### sg3_utils

**rescan-scsi-bus.sh**

### loop device

```bash
$ dd if=/dev/zero of=myfs bs=4M count=10
$ /sbin/mkfs.ext3 myfs
$ mount -t ext2 -o loop ./myfs /mnt
```

### nvme

这个nvme程序是一个用户空间实用程序，为NVM-Express驱动器提供符合标准的工具。它是专门为Linux制作的，因为它依赖于主线内核驱动程序定义的`IOCTLS`。该实用程序具有用于规范中定义的所有admin和io命令以及用于显示控制器寄存器的子命令。

**nvme-discover - Discover NVMeoF subsystems**

Send one or more Get Log Page requests to a `NVMe-over-Fabrics` Discovery Controller.

The NVMe-over-Fabrics specification defines the concept of a `Discovery Controller`  that an NVMe Host can query on a fabric network to discover NVMe subsystems contained in NVMe Targets which it can connect to on the network. The Discovery Controller will return Discovery Log Pages that provide the NVMe Host with specific information (such as network address and unique subsystem NQN) the NVMe Host can use to issue an NVMe connect command to connect itself to a storage resource contained in that NVMe subsystem on the NVMe Target.

```bash
# nvme help discover
$ nvme discover -t rdma -a 192.168.30.21
```

**nvme-connect - Connect to NVMeoF subsystem**

Create a transport connection to a remote system (specified by --traddr and --trsvcid) and create a NVMe over Fabrics controller for the NVMe subsystem specified by the --nqn option.

```bash
# nvme help connect
$ nvme connect -t rdma -a 192.168.20.21 -s 4420 -n nqn.1992-05.com.wdc.afaapp:nvme.10 -i 1
```

**name-connect-all - Discover and Connect to NVMeoF subsystems**

## 逻辑卷LVM

### Resize - 扩容

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

### Resize - 缩容

> 缩小逻辑卷是数据损坏的最高风险，尽量提前做好容量规划，避免卷缩容操作。

```bash
$ umount /int
$ e2fsck -f /dev/mapper/vg-inst
$ lvreduce -L -12G /dev/mapper/vg-inst
$ e2fsck -f /dev/mapper/vg-inst
$ mount /inst
```

## Software RAID - mdadm

[A guide to mdadm](**Linux Block IO: Introducing Multi-queue SSD Access on Multi-core Systems**)

[Using mdadm to send e-mail alerts for RAID failures](https://www.suse.com/support/kb/doc/?id=000016716)

# 文件系统

```bash
# 查找一个挂载的文件系统
$ findmnt -n -F /etc/fstab -o SOURCE,TARGET,FSTYPE,OPTIONS,FREQ,PASSNO /dev/mapper/system-vol
```

### 软/硬链接

```bash
$ ln -sf <source file> <target file>  #创建软链接
```

## NFS

`Ubuntu`系统：

参考文档：[Network File System (NFS)](https://ubuntu.com/server/docs/service-nfs)

```bash
$ systemctl status  nfs-kernel-server.service
$ systemctl start  nfs-kernel-server.service
$ mount host.cdc.domain.com:/home/nfs /Users/hunk/work/nfs
```

## split

```bash
$ split -l 200 systemlog #切割后的每个小文件包含200行
$ split  -b 200M /var/log/1.log  1.log.split # 将1.log切割成200MB的文件，1.log.split是切割后文件的前缀
$ split -d -b 200M httpd.log log # Split the file and name it with numbers
```

## dd

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



# 网络管理

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
$ openssl s_server -key /etc/pki/tls/certs/eca/private/key.pem -cert /etc/pki/tls/certs/eca/cert_chain.pem -accept 8090 -cipher ALL
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

# 内核管理

## crash - 调试vmcore

```bash
$ crash /usr/lib/debug/lib/modules/4.18.0-372.32.1.el8_6.x86_64/vmlinux vmcore
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

