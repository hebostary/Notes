# Design

https://restic.readthedocs.io/en/latest/100_references.html#design

# Demonstrations

## 基本操作

### 管理备份仓库

先初始化一个备份仓库：

```bash
$ ./restic init --repo /home/hunk/restic/repo1
```

为了后续操作方便，不用在命令行里指定仓库信息和密码，我们通过环境变量指定默认备份仓库和仓库密码：

```bash
$ export RESTIC_PASSWORD=*******
$ export RESTIC_REPOSITORY=/home/hunk/restic/repo1
```

查看备份仓库的信息：

```bash
$ ./restic stats
repository 30f15cd9 opened (version 2, compression level auto)
[0:00] 100.00%  1 / 1 index files loaded
scanning...
Stats in restore-size mode:
     Snapshots processed:  1
        Total File Count:  3
              Total Size:  0 B
```

### 执行备份

```bash
$ ./restic backup /home/hunk/tmp
repository 30f15cd9 opened (version 2, compression level auto)
created new cache in /home/hunk/.cache/restic
lock repository
no parent snapshot found, will read all files
load index files
[0:00]          0 index files loaded
start scan on [/home/hunk/tmp]
start backup on [/home/hunk/tmp]
scan finished in 0.226s: 0 files, 0 B

Files:           0 new,     0 changed,     0 unmodified
Dirs:            3 new,     0 changed,     0 unmodified
Data Blobs:      0 new
Tree Blobs:      4 new
Added to the repository: 1.087 KiB (973 B stored)

processed 0 files, 0 B in 0:00
snapshot 37175f33 saved
```

对于每次备份生成的备份副本，一般备份软件都定义为`image`或者`snap`。`restic`把备份副本叫做`snapshot`，意为数据在备份时刻的快照，也是合理的。每个`snapshot`有一个64字节的的`snapshot id`，一般都使用截断的8字节的`short_id`，比如 0e05ca4c。

如果是首次备份，仓库里没有`parent snapshot`，所以执行全量备份。否则，执行增量备份，增量备份的`snapshot`会指向其`parent snapshot`。执行增量备份后，前面的`snapshot`也是可以直接删除的。

备份时默认会备份指定目录的所有文件，包含隐藏文件，可以通过指定`--exclude`参数排除需要备份的文件，也可以通过`--file-from`指定需要备份的文件列表。

`restic`支持在每次备份时指定一个或者多个标签，便于后期基于tag做快照检索。

### 管理快照

```bash
# 查询仓库里的所有快照
$ ./restic snapshots
repository 30f15cd9 opened (version 2, compression level auto)
ID        Time                 Host             Tags        Paths
--------------------------------------------------------------------------
37175f33  2024-03-02 18:30:24  DESKTOP-JFUCNKU              /home/hunk/tmp
--------------------------------------------------------------------------
1 snapshots

# 查看指定快照的信息
$ ./restic snapshots 0e05ca4c
repository 30f15cd9 opened (version 2, compression level auto)
ID        Time                 Host             Tags        Paths
----------------------------------------------------------------------------------------------------
0e05ca4c  2024-03-02 21:33:49  DESKTOP-JFUCNKU              /home/hunk/workspace/src/github.com/ceph
----------------------------------------------------------------------------------------------------
1 snapshots

# 输出json格式的快照信息
$ ./restic snapshots --json
[{"time":"2024-03-02T21:33:49.139060166+08:00","tree":"11270637c3aeb99ae7e8c5976b6fa253a50d0ef65474786c4563c0921c584a0a","paths":["/home/hunk/workspace/src/github.com/ceph"],"hostname":"DESKTOP-JFUCNKU","username":"hunk","uid":1000,"gid":1000,"program_version":"restic 0.16.3-dev (compiled manually)","id":"0e05ca4cd1ed04633d5b4e76b28c497e256939c959f2f1602e790aa748611c37","short_id":"0e05ca4c"},{"time":"2024-03-02T21:35:10.60423227+08:00","parent":"0e05ca4cd1ed04633d5b4e76b28c497e256939c959f2f1602e790aa748611c37","tree":"122496ce6182c74d791d5f8745e5edf6a457755ea2d8e192a3633878f216b52b","paths":["/home/hunk/workspace/src/github.com/ceph"],"hostname":"DESKTOP-JFUCNKU","username":"hunk","uid":1000,"gid":1000,"program_version":"restic 0.16.3-dev (compiled manually)","id":"b36caa0eec1a90c2114f3d308e86ae4806b567eb3f50b2dfa360c9b2487960a8","short_id":"b36caa0e"}]

# 从仓库里删除指定快照
$ ./restic forget 37175f33
repository 30f15cd9 opened (version 2, compression level auto)
[0:00] 100.00%  1 / 1 files deleted

# 只保留最新的3个snapshot，删除其余snapshot，最好用--dry-run确认删除行为是否是预期的操作
$./restic forget --keep-last=3 --dry-run
repository 30f15cd9 opened (version 2, compression level auto)
Applying Policy: keep 3 latest snapshots
keep 2 snapshots:
ID        Time                 Host             Tags        Reasons        Paths
-------------------------------------------------------------------------------------------------------------------
b36caa0e  2024-03-02 21:35:10  DESKTOP-JFUCNKU              last snapshot  /home/hunk/workspace/src/github.com/ceph
830f5899  2024-03-02 21:54:34  DESKTOP-JFUCNKU              last snapshot  /home/hunk/workspace/src/github.com/ceph
-------------------------------------------------------------------------------------------------------------------
2 snapshots

# 只保留最新的一个小时的快照
$ date
Sat Mar  2 22:44:32 CST 2024
$ ./restic  forget --dry-run --keep-hourly 1
repository 30f15cd9 opened (version 2, compression level auto)
Applying Policy: keep 1 hourly snapshots
keep 1 snapshots:
ID        Time                 Host             Tags        Reasons          Paths
---------------------------------------------------------------------------------------------------------------------
830f5899  2024-03-02 21:54:34  DESKTOP-JFUCNKU              hourly snapshot  /home/hunk/workspace/src/github.com/ceph
---------------------------------------------------------------------------------------------------------------------
1 snapshots

remove 1 snapshots:
ID        Time                 Host             Tags        Paths
----------------------------------------------------------------------------------------------------
b36caa0e  2024-03-02 21:35:10  DESKTOP-JFUCNKU              /home/hunk/workspace/src/github.com/ceph
----------------------------------------------------------------------------------------------------
1 snapshots

Would have removed the following snapshots:
{b36caa0e}

# 对比两个快照
$ ./restic diff b36caa0e 830f5899
repository 30f15cd9 opened (version 2, compression level auto)
comparing snapshot b36caa0e to 830f5899:

[0:00] 100.00%  5 / 5 index files loaded
-    /home/hunk/workspace/src/github.com/ceph/go-implement-your-object-storage-master/
...
-    /home/hunk/workspace/src/github.com/ceph/go-implement-your-object-storage-master/tools/stoptestenv.sh

Files:           0 new,   185 removed,     0 changed
Dirs:            0 new,    67 removed
Others:          0 new,     0 removed
Data Blobs:      0 new,   153 removed
Tree Blobs:      7 new,    74 removed
  Added:   24.500 KiB
  Removed: 183.156 KiB
```

### 文件检索和恢复单个文件

```bash
# 列出snapshot里备份的文件列表
./restic ls b36caa0e

# 将snapshot里指定的文件dump到标准输出
# Usage:
#  restic dump [flags] snapshotID file
$ ./restic dump b36caa0e /home/hunk/workspace/src/github.com/ceph/udev/50-rbd.rules > /tmp/50-rbd.rules

# 在仓库里查找指定的文件。支持多种查找方式，参考help文档
$ ./restic find 50-rbd.rules
$ ./restic find 50-rbd.rules
repository 30f15cd9 opened (version 2, compression level auto)
[0:00] 100.00%  5 / 5 index files loaded
Found matching entries in snapshot b36caa0e from 2024-03-02 21:35:10
/home/hunk/workspace/src/github.com/ceph/udev/50-rbd.rules

Found matching entries in snapshot 830f5899 from 2024-03-02 21:54:34
/home/hunk/workspace/src/github.com/ceph/udev/50-rbd.rules
```



### 执行恢复

```bash
$ ./restic restore b36caa0e --target /
tmp
repository 30f15cd9 opened (version 2, compression level auto)
[0:00] 100.00%  5 / 5 index files loaded
restoring snapshot b36caa0e of [/home/hunk/workspace/src/github.com/ceph] at 2024-03-02 21:35:10.60423227 +0800 CST by hunk@DESKTOP-JFUCNKU to /tmp
Summary: Restored 58468 files/dirs (1.871 GiB) in 1:03

$ ls /tmp/home/hunk/workspace/src/github.com/ceph/
AUTHORS                          bin                                      make-dist
...
```

### 挂载仓库

`restic mount`命令可以将整个备份仓库通过FUSE以只读方式挂载到指定的挂载点，然后我们就可以像访问备份源文件/目录那样访问`snapshot`里的文件和目录。

```bash
$ mkdir -p /home/hunk/restic/snapshots
$ ./restic mount /home/hunk/restic/snapshots
repository 30f15cd9 opened (version 2, compression level auto)
[0:00] 100.00%  5 / 5 index files loaded
Now serving the repository at /home/hunk/restic/snapshots
Use another terminal or tool to browse the contents of this folder.
When finished, quit with Ctrl-c here or umount the mountpoint.
```

我们可以在另一个终端使用这个FUSE挂载，并拷贝任何想要恢复的文件或者目录：

```bash
$ mount | grep restic
/home/hunk/workspace/src/github.com/restic/restic/restic on /home/hunk/restic/snapshots type fuse (ro,nosuid,nodev,relatime,user_id=1000,group_id=1000)
$ ls -al /home/hunk/restic/snapshots/
total 4
dr-xr-xr-x 1 hunk hunk    0 Mar  2 22:19 .
drwxr-xr-x 4 hunk hunk 4096 Mar  2 22:18 ..
dr-xr-xr-x 1 hunk hunk    0 Mar  2 22:19 hosts
dr-xr-xr-x 1 hunk hunk    0 Mar  2 22:19 ids
dr-xr-xr-x 1 hunk hunk    0 Mar  2 22:19 snapshots
dr-xr-xr-x 1 hunk hunk    0 Mar  2 22:19 tags
$ ls -al /home/hunk/restic/snapshots/s
napshots/
total 1
dr-xr-xr-x 1 hunk hunk  0 Mar  2 22:19 .
dr-xr-xr-x 1 hunk hunk  0 Mar  2 22:19 ..
dr-xr-xr-x 2 root root  0 Mar  2 21:35 2024-03-02T21:35:10+08:00
dr-xr-xr-x 2 root root  0 Mar  2 21:54 2024-03-02T21:54:34+08:00
lrwxrwxrwx 1 hunk hunk 25 Mar  2 21:54 latest -> 2024-03-02T21:54:34+08:00
$ ls -al /home/hunk/restic/snapshots/snapshots/latest/home/hunk/workspace/src/github.com/ceph/
total 429
drwxrwxrwx 2 hunk hunk     0 Mar  2 21:54 .
drwxr-xr-x 2 hunk hunk     0 Mar  2 18:14 ..
drwxrwxrwx 2 hunk hunk     0 Mar  2 15:46 .git
-rwxrwxrwx 1 hunk hunk    57 Oct 14  2020 .gitattributes
drwxrwxrwx 2 hunk hunk     0 Oct 14  2020 .github
-rwxrwxrwx 1 hunk hunk  5802 Oct 14  2020 .githubmap
...
```

私以为这是`restic`一个很强大的功能。各种商业备份软件基本上都会支持这种`LiveMount`功能，实现方式也是通过FUSE将备份存储池里的备份副本挂载到指定的目录，并支持使用POSIX文件系统接口去访问，因为备份副本的存储格式和源文件是不一样的，一般都经过切片去重、压缩、加密等处理，无法直接读取。所以需要备份软件实现一个FUSE文件系统来提供对备份副本的I文件的/O接口。另外，备份软件还可以将这个挂载点通过NFS/SMB共享出来，这样用户可以在自己的生产测试环境中直接挂载NFS/SMB共享，然后拷贝想要恢复的任何文件。如果备份的是VMware虚拟机，还可以将NFS共享作为DataStore挂载到vCenter，然后在此基础上创建一个和备份VM相同的VM实例（vCenter支持从NFS启动VM），这个新的VM实例可以用于验证备份数据、测试补丁等。

## 并发方式备份文件/目录？

`--read-concurrency`这个参数用于在备份时指定并发读多少文件，默认是2个。但是，我在测试备份整个`cpeh`代码库（51683 files, 1.871 GiB）的时候，发现无论将`--read-concurrency`设置成10、20还是100，执行完备份所需时间都差不多。不知道是不是无法并发写备份仓库的问题，有待研究。

## 备份块设备文件

参考[Reading data from a command](https://restic.readthedocs.io/en/latest/040_backup.html#reading-data-from-a-command), `restic`支持从命令输出流直接备份数据，这在一些特殊场景中非常实用，比如在传统的数据库备份流程中，我们一般是先执行数据库工具将数据dump到某个文件，然后再将文件备份到其它地方，这个过程包含一次完全没必要的读写操作。restic的`--stdin-from-command`参数用于备份命令行输出流，具体用法如下：

```bash
$ restic -r /srv/restic-repo backup --stdin-filename production.sql --stdin-from-command mysqldump [...]
# 或者
# Will not notice failures, read the warning above
$ mysqldump [...] | restic -r /srv/restic-repo backup --stdin
```

`--stdin-filename`用来指定备份文件的名字，默认名字是`stdin`。但是最好不要用第二种方法，官方手册给出的原因如下：

> **Important**: while it is possible to pipe a command output to restic using `--stdin`, doing so is discouraged as it will mask errors from the command, leading to corrupted backups. For example, in the following code block, if `mysqldump` fails to connect to the MySQL database, the restic backup will nevertheless succeed in creating an _empty_ backup:

这个用法不禁让我想到可以用`restic`来备份块设备文件。

### 创建块设备文件及文件系统

```bash
[root@net146-host122 srcfiles]# dd if=/dev/zero of=./1.img bs=1G count=1
1+0 records in
1+0 records out
1073741824 bytes (1.1 GB, 1.0 GiB) copied, 3.67359 s, 292 MB/s
[root@net146-host122 srcfiles]# ll
total 1048576
-rw-r--r--. 1 root root 1073741824 Feb  4 12:05 1.img
[root@net146-host122 srcfiles]# mkfs.ext4 ./1.img 
[root@net146-host122 srcfiles]# mkdir 1.mp
[root@net146-host122 srcfiles]# mount 1.img 1.mp/
[root@net146-host122 srcfiles]# cd 1.mp/
[root@net146-host122 1.mp]# ll
total 16
drwx------. 2 root root 16384 Feb  4 12:05 lost+found
[root@net146-host122 1.mp]# echo "hello world" > 1.txt
[root@net146-host122 1.mp]# cp /home/dev/tmp/consoleFull .
[root@net146-host122 1.mp]# ll
total 13592
-rw-r--r--. 1 root root       12 Feb  4 12:06 1.txt
-rw-r--r--. 1 root root 13893647 Feb  4 12:06 consoleFull
drwx------. 2 root root    16384 Feb  4 12:05 lost+found
[root@net146-host122 1.mp]# cd ..
[root@net146-host122 srcfiles]# umount ./1.mp
```

我们只在这个`1.img`文件系统里写入了十几MB用户数据，加上文件系统本身的一些元数据也不过几十MB数据，整个块设备文件的大部分块里存储的都是连续的零值。

### 备份块设备文件

```bash
[root@net146-host122 restic]# export RESTIC_BIN_PATH="/home/dev/workspace/github/restic/restic/restic"
[root@net146-host122 restic]# export RESTIC_REPO1_PATH="/home/dev/restic/repo1"
```

```bash
[root@net146-host122 restic]# .${RESTIC_BIN_PATH} --repo ${RESTIC_REPO1_PATH} --verbose backup --stdin-filename 1.img --stdin-from-command cat /home/dev/restic/srcfiles/1.img
open repository
enter password for repository: 
repository b832e2de opened (version 2, compression level auto)
lock repository
using parent snapshot c514f472
load index files
[0:00] 100.00%  4 / 4 index files loaded
read data from stdin
start scan on [/1.img]
start backup on [/1.img]
scan finished in 0.339s: 1 files, 0 B

Files:           1 new,     0 changed,     0 unmodified
Dirs:            0 new,     0 changed,     0 unmodified
Data Blobs:     18 new
Tree Blobs:      1 new
Added to the repository: 17.639 MiB (637.268 KiB stored)

processed 1 files, 1.000 GiB in 0:01
snapshot c572a07b saved
```

可以看到，虽然备份的是一个 1 GiB 大小的块设备文件，实际只备份了17.639 MiB 数据到仓库里去。

### 更新文件系统里的文件

我们继续对`1.img`这个文件系统做了一些更新：删除了一个文件 consoleFull，在 1.txt 里追加写了一行数据。

```bash
[root@net146-host122 srcfiles]# mount 1.img 1.mp/
[root@net146-host122 srcfiles]# cd 1.mp/
[root@net146-host122 1.mp]# ls -al
total 13596
drwxr-xr-x. 3 root root     4096 Feb  4 12:06 .
drwxr-xr-x. 3 root root       31 Feb  4 15:05 ..
-rw-r--r--. 1 root root       12 Feb  4 12:06 1.txt
-rw-r--r--. 1 root root 13893647 Feb  4 12:06 consoleFull
drwx------. 2 root root    16384 Feb  4 12:05 lost+found
[root@net146-host122 1.mp]# rm consoleFull 
rm: remove regular file 'consoleFull'? y
[root@net146-host122 1.mp]# cat 1.txt 
hello world
[root@net146-host122 1.mp]# echo "bloack file backup test" >> 1.txt 
[root@net146-host122 1.mp]# cd ..
[root@net146-host122 srcfiles]# umount 1.mp/
```

### 再次备份块设备文件

再次备份`1.img`这个块设备文件，一共备份了7.852 MiB有效数据。

```bash
[root@net146-host122 srcfiles]# ${RESTIC_BIN_PATH} --repo ${RESTIC_REPO1_PATH} --verbose backup --stdin-filename 1.img --stdin-from-command cat /home/dev/restic/srcfiles/1.img
open repository
enter password for repository: 
repository b832e2de opened (version 2, compression level auto)
lock repository
using parent snapshot c572a07b
load index files
[0:00] 100.00%  5 / 5 index files loaded
read data from stdin
start scan on [/1.img]
start backup on [/1.img]
scan finished in 0.314s: 1 files, 0 B

Files:           0 new,     1 changed,     0 unmodified
Dirs:            0 new,     0 changed,     0 unmodified
Data Blobs:      6 new
Tree Blobs:      1 new
Added to the repository: 7.852 MiB (337.422 KiB stored)

processed 1 files, 1.000 GiB in 0:01
snapshot 6def1dc1 saved
```

至此，我们两次备份分别创建了两个snapshot：`c572a07b`和`6def1dc1`。

### 用第一个备份快照做恢复

用第一次备份的snapshot将`1.img`恢复到其它目录后直接挂载，发现里面的内容正是我们第一次创建的那些数据。

```bash
[root@net146-host122 srcfiles]# ${RESTIC_BIN_PATH} --repo ${RESTIC_REPO1_PATH} --verbose restore c572a07b --target /tmp/c572a07b
enter password for repository: 
repository b832e2de opened (version 2, compression level auto)
[0:00] 100.00%  6 / 6 index files loaded
restoring snapshot c572a07b of [/1.img] at 2024-02-04 15:09:11.545477013 +0800 CST by root@net146-host122 to /tmp/c572a07b
Summary: Restored 1 files/dirs (1.000 GiB) in 0:01
[root@net146-host122 srcfiles]# ls -al /tmp/c572a07b
total 1048580
drwx------.  2 root root         19 Feb  4 15:34 .
drwxrwxrwt. 37 root root       4096 Feb  4 15:34 ..
-rw-r--r--.  1 root root 1073741824 Feb  4 15:09 1.img
[root@net146-host122 srcfiles]# mount /tmp/c572a07b/1.img ./1.mp/
[root@net146-host122 srcfiles]# ls -al ./1.mp/
total 13596
drwxr-xr-x. 3 root root     4096 Feb  4 12:06 .
drwxr-xr-x. 3 root root       31 Feb  4 15:05 ..
-rw-r--r--. 1 root root       12 Feb  4 12:06 1.txt
-rw-r--r--. 1 root root 13893647 Feb  4 12:06 consoleFull
drwx------. 2 root root    16384 Feb  4 12:05 lost+found
[root@net146-host122 srcfiles]# cat ./1.mp/1.txt 
hello world
[root@net146-host122 srcfiles]# umount 1.mp/
```

### 用第二个备份快照做恢复

用第一次备份的snapshot将`1.img`恢复到其它目录后直接挂载，可以看到里面的内容正是我们更新后的那些数据。

```bash
[root@net146-host122 srcfiles]# ${RESTIC_BIN_PATH} --repo ${RESTIC_REPO1_PATH} --verbose restore 6def1dc1 --target /tmp/6def1dc1
enter password for repository: 
repository b832e2de opened (version 2, compression level auto)
[0:00] 100.00%  6 / 6 index files loaded
restoring snapshot 6def1dc1 of [/1.img] at 2024-02-04 15:15:42.959723054 +0800 CST by root@net146-host122 to /tmp/6def1dc1
Summary: Restored 1 files/dirs (1.000 GiB) in 0:00
[root@net146-host122 srcfiles]# ls -al /tmp/6def1dc1
total 1048580
drwx------.  2 root root         19 Feb  4 15:38 .
drwxrwxrwt. 38 root root       4096 Feb  4 15:38 ..
-rw-r--r--.  1 root root 1073741824 Feb  4 15:15 1.img
[root@net146-host122 srcfiles]# mount /tmp/6def1dc1/1.img ./1.mp/
[root@net146-host122 srcfiles]# ls -al ./1.mp/
total 24
drwxr-xr-x. 3 root root  4096 Feb  4 15:10 .
drwxr-xr-x. 3 root root    31 Feb  4 15:05 ..
-rw-r--r--. 1 root root    36 Feb  4 15:10 1.txt
drwx------. 2 root root 16384 Feb  4 12:05 lost+found
[root@net146-host122 srcfiles]# cat 1.mp/1.txt 
hello world
bloack file backup test
[root@net146-host122 srcfiles]# umount 1.mp/
```

### 备份硬盘分区

我们前面备份的`1.img`本质上还是一个regular file，再尝试备份一下真正的块设备文件，为了方便，我直接备份了/dev/sda的两个partition：

```bash
[root@net146-host122 srcfiles]# df -h |grep boot
/dev/sda2             1014M  208M  807M  21% /boot
/dev/sda1              599M  5.9M  594M   1% /boot/efi
```

其中，备份和恢复`dev/sda1`是没有问题的，过程如下：

```bash
[root@net146-host122 srcfiles]# ${RESTIC_BIN_PATH} --repo ${RESTIC_REPO1_PATH} --verbose backup --stdin-filename /dev/sda1 --stdin-from-command cat /dev/sda1
open repository
enter password for repository: 
repository b832e2de opened (version 2, compression level auto)
lock repository
using parent snapshot 2a4b56b3
load index files
[0:00] 100.00%  7 / 7 index files loaded
read data from stdin
start scan on [/dev/sda1]
start backup on [/dev/sda1]
scan finished in 0.327s: 1 files, 0 B

Files:           1 new,     0 changed,     0 unmodified
Dirs:            0 new,     1 changed,     0 unmodified
Data Blobs:     14 new
Tree Blobs:      2 new
Added to the repository: 7.577 MiB (2.113 MiB stored)

processed 1 files, 600.000 MiB in 0:01
snapshot c47c1c63 saved
[root@net146-host122 srcfiles]# ${RESTIC_BIN_PATH} --repo ${RESTIC_REPO1_PATH} --verbose restore c47c1c63 --target /tmp/c47c1c63
enter password for repository: 
repository b832e2de opened (version 2, compression level auto)
[0:00] 100.00%  8 / 8 index files loaded
restoring snapshot c47c1c63 of [/dev/sda1] at 2024-02-04 16:10:34.341712649 +0800 CST by root@net146-host122 to /tmp/c47c1c63
Summary: Restored 2 files/dirs (600.000 MiB) in 0:00
[root@net146-host122 srcfiles]# mount /tmp/c47c1c63/dev/sda1 ./sda1/
[root@net146-host122 srcfiles]# ls -al ./sda1/
total 8
drwxr-xr-x. 3 root root 4096 Jan  1  1970 .
drwxr-xr-x. 5 root root   55 Feb  4 16:11 ..
drwxr-xr-x. 4 root root 4096 Mar 17  2023 EFI
```

但是恢复`/dev/sda2`之后，恢复的设备文件无法挂载成功。TODO：后面有时间再研究这个问题。

```bash
[root@net146-host122 srcfiles]# mount /tmp/2a4b56b3/dev/sda2 ./sda2/
mount: /home/dev/restic/srcfiles/sda2: wrong fs type, bad option, bad superblock on /dev/loop0, missing codepage or helper program, or other error.
```

# 基于restic二次开发

## Velero - Kubernetes应用备份容灾

`Velero`是VMware公司提供的一个云原生的Kubernetes灾难恢复和迁移工具，在GitHub上的地址是https://github.com/vmware-tanzu/velero。`Velero`的前身是Heptio公司的Ark工具，后被VMware公司收购，底层数据卷的备份用的正是`restic`。

关于`Velero`备份Kubernetes的应用实践可以参考：https://www.zhihu.com/people/int32bit
