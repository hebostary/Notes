# 基本命令

## alias

```bash
alias ll='ls -l'
```

# 搜索过滤

## grep

```bash
grep -E '^ENABLE|^BPCD' log.text    #查找出文件中以ENABLE或者BPCD开头的行
grep "ENABLE" log.text | grep BPCD  #查找同时包含ENABLE和BPCD的行
```

## awk



# 用户管理

## 创建删除用户

```bash
groupadd -g 200 nexus  		    #添加指定gid的组
useradd -u 200 -g nexus nexus   #添加指定uid的用户
usermod -G root nexus 		    #修改用户加入root组
```

## 查询用户信息



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



# 包管理

## rpm包管理

### 基础操作

```bash
rpm2cpio <name>.rpm | cpio -div  #解压rpm
rpm -qf <file>     #查询文件属于哪个包
rpm -ql <package>  #查询rpm包安装了哪些文件
rpm -qi <package>  #查询rpm包属性信息

#查询包的依赖关系
rpm -qpR <package.rpm>  #查看包是否依赖其它包
rpm -e --test <package> #测试包是否被其它包依赖
```

### 构建rpm包

## xz包

```bash
#解压tar.xz包
xz -d xxx.tar.xz -> xxx.tar
tar xvf xxx.tar
```

## bz2包

```bash
bunzip2 all.bz2
tar -xjf valgrind-3.15.0.tar.bz2
```



## ISO管理



# 磁盘管理

## 磁盘命令

```bash
blkid     #查看磁盘及其UUID
```

## 文件系统

### 链接

```bash
ln -sf <source file> <target file>  #创建软链接
```

### NFS

`Ubuntu`系统：

参考文档：[Network File System (NFS)](https://ubuntu.com/server/docs/service-nfs)

```bash
systemctl status  nfs-kernel-server.service
systemctl start  nfs-kernel-server.service
mount host.cdc.domain.com:/home/nfs /Users/hunk/work/nfs
```



## lvm

### 卷扩容

```bash
lvextend -L +42G /dev/mapper/vg-inst 
resize2fs -p /dev/mapper/vg-inst
```

扩容时可能出现的问题：Snapshot origin volumes can be resized only while inactive: try lvchange -an Run `lvextend --help' for more information. 解决办法：

```bash
umount /dev/mapper/vg-inst
lvchange -an /dev/mapper/vg-inst
lvextend -L +30G /dev/mapper/vg-inst
lvchange -ay /dev/mapper/vg-inst
mount /dev/mapper/vg-inst
resize2fs /dev/mapper/vg-inst
```

# 网络管理

## iptables防火墙

[全网最好文章：iptables详解（1）：iptables概念](http://www.zsythink.net/archives/1199/)

## DNS

## netstat

```bash
netstat -n | awk '/^tcp/' #查看所有tcp连接及状态
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

**Step 3**：ssh登录z

```bash
[ansible@host1 ~]$ ssh host2
```