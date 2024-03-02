# Demonstrations

参考[Reading data from a command](https://restic.readthedocs.io/en/latest/040_backup.html#reading-data-from-a-command), restic支持从命令输出流直接备份数据，这在一些特殊场景中非常实用，比如在传统的数据库备份流程中，我们一般是先执行数据库工具将数据dump到某个文件，然后再将文件备份到其它地方，这个过程包含一次完全没必要的读写操作。restic的`--stdin-from-command`参数用于备份命令行输出流，具体用法如下：

```bash
$ restic -r /srv/restic-repo backup --stdin-filename production.sql --stdin-from-command mysqldump [...]
# 或者
# Will not notice failures, read the warning above
$ mysqldump [...] | restic -r /srv/restic-repo backup --stdin
```

`--stdin-filename`用来指定备份文件的名字，默认名字是`stdin`。但是最好不要用第二种方法，官方手册给出的原因如下：

> **Important**: while it is possible to pipe a command output to restic using `--stdin`, doing so is discouraged as it will mask errors from the command, leading to corrupted backups. For example, in the following code block, if `mysqldump` fails to connect to the MySQL database, the restic backup will nevertheless succeed in creating an _empty_ backup:

这个用法不禁让我想到可以用restic来备份块设备文件。

## Backup block device file

### Create block device file and file system then write some files

```bash
[root@net146-host122 srcfiles]# dd if=/dev/zero of=./1.img bs=1G count=1
1+0 records in
1+0 records out
1073741824 bytes (1.1 GB, 1.0 GiB) copied, 3.67359 s, 292 MB/s
[root@net146-host122 srcfiles]# ll
total 1048576
-rw-r--r--. 1 root root 1073741824 Feb  4 12:05 1.img
[root@net146-host122 srcfiles]# mkfs.ext4 ./1.img 
mke2fs 1.45.6 (20-Mar-2020)
Discarding device blocks: done                            
Creating filesystem with 262144 4k blocks and 65536 inodes
Filesystem UUID: 212f0ff9-4734-4d55-8866-55b3fa4dcb52
Superblock backups stored on blocks: 
        32768, 98304, 163840, 229376

Allocating group tables: done                            
Writing inode tables: done                            
Creating journal (8192 blocks): done
Writing superblocks and filesystem accounting information: done

[root@net146-host122 srcfiles]# ll
total 33424
-rw-r--r--. 1 root root 1073741824 Feb  4 12:05 1.img

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

### Backup block device file

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

可以看到，虽然备份的是一个1GB大小的块设备文件，实际只备份了17.639 MiB数据到repo里去。

### Update file system contents

继续对`1.img`这个文件系统做了一些更新：删除了一个文件consoleFull，在1.txt里追加写了一行数据。

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

### Backup block device file again

再次备份1.img这个块设备文件，一共备份了7.852 MiB有效数据。

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

### Restore block device file via first snapshot

用第一次备份的snapshot将1.img恢复到其它目录后直接挂载，发现里面的内容正是我们第一次创建的那些数据。

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

### Restore block device file via second snapshot

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

### Backup real block device file

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

