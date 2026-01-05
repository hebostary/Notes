## ZFS快照&克隆

### 基本原理

### Demonstration

```bash
root@master:~# zpool create zfspv-pool /dev/sdb1
root@master:~# zfs create zfspv-pool/dataset1
root@master:~# mkdir -p /mnt/zfspv-pool/dataset1

# 查看dataset所有property
root@master:~# zfs get all zfspv-pool/dataset1

# 设置挂载点，不需要单独挂载
root@master:~# zfs set mountpoint=/mnt/zfspv-pool/dataset1 zfspv-pool/dataset1
root@master:~# mount -l |grep zfspv-pool/dataset1
zfspv-pool/dataset1 on /mnt/zfspv-pool/dataset1 type zfs (rw,xattr,noacl)

root@master:~# zfs list -t filesystem
NAME                                                  USED  AVAIL     REFER  MOUNTPOINT
zfspv-pool                                           2.42G   412G       24K  /zfspv-pool
zfspv-pool/dataset1                                    24K   412G       24K  /mnt/zfspv-pool/dataset1

# 创建快照snap1
root@master:~# echo "snap1" > /mnt/zfspv-pool/dataset1/snap_num.txt
root@master:~# zfs snapshot zfspv-pool/dataset1@snap1
root@master:~# zfs list -t snapshot zfspv-pool/dataset1
NAME                        USED  AVAIL     REFER  MOUNTPOINT
zfspv-pool/dataset1@snap1     0B      -     24.5K  -

# 创建快照snap2
root@master:~# echo "snap2" > /mnt/zfspv-pool/dataset1/snap_num.txt
root@master:~# zfs snapshot zfspv-pool/dataset1@snap2
root@master:~# echo "latest" > /mnt/zfspv-pool/dataset1/snap_num.txt
root@master:~# zfs list -t snapshot
NAME                        USED  AVAIL     REFER  MOUNTPOINT
zfspv-pool/dataset1@snap1  12.5K      -     24.5K  -
zfspv-pool/dataset1@snap2  12.5K      -     24.5K  -

# 往dataset写入400MB大文件
root@master:~# dd if=/dev/random bs=4M count=100 of=/mnt/zfspv-pool/dataset1/random.img
root@master:~# ls /mnt/zfspv-pool/dataset1/
random.img  snap_num.txt
root@master:~# zfs list -t snapshot zfspv-pool/dataset1
NAME                        USED  AVAIL     REFER  MOUNTPOINT
zfspv-pool/dataset1@snap1  12.5K      -     24.5K  -
zfspv-pool/dataset1@snap2  12.5K      -     24.5K  -
root@master:~# zfs list -t filesystem
NAME                                                  USED  AVAIL     REFER  MOUNTPOINT
zfspv-pool                                           3.10G   412G       24K  /zfspv-pool
zfspv-pool/dataset1                                   400M   412G      400M  /mnt/zfspv-pool/dataset1

# 回滚到最新快照snap2，创建snap2之后写入文件所占用的空间也被释放
# 回滚快照不需要umount
root@master:~# zfs rollback zfspv-pool/dataset1@snap2
root@master:~# cat /mnt/zfspv-pool/dataset1/snap_num.txt 
snap2
root@master:~# zfs list -t filesystem
NAME                                                  USED  AVAIL     REFER  MOUNTPOINT
zfspv-pool                                           2.70G   412G       24K  /zfspv-pool
zfspv-pool/dataset1                                    50K   412G     24.5K  /mnt/zfspv-pool/dataset1
root@master:~# zfs list -t snapshot zfspv-pool/dataset1
NAME                        USED  AVAIL     REFER  MOUNTPOINT
zfspv-pool/dataset1@snap1  12.5K      -     24.5K  -
zfspv-pool/dataset1@snap2    11K      -     24.5K  -

# 不允许直接回滚到非最新快照
root@master:~# zfs rollback zfspv-pool/dataset1@snap1
cannot rollback to 'zfspv-pool/dataset1@snap1': more recent snapshots or bookmarks exist
use '-r' to force deletion of the following snapshots and bookmarks:
zfspv-pool/dataset1@snap2

# 不允许直接删除存在快照的dataset，需要加-r参数强制删除快照
# 如果存在基于快照的链接克隆，删除dataset或者snapshot都需要用-R参数将克隆一起删除
root@master:~# zfs destroy zfspv-pool/dataset1
cannot destroy 'zfspv-pool/dataset1': filesystem has children
use '-r' to destroy the following datasets:
zfspv-pool/dataset1@snap1
zfspv-pool/dataset1@snap2

# 强制回滚到非最新快照snap1
root@master:~# zfs rollback zfspv-pool/dataset1@snap1 -r
root@master:~# zfs list -t snapshot zfspv-pool/dataset1
NAME                        USED  AVAIL     REFER  MOUNTPOINT
zfspv-pool/dataset1@snap1     0B      -     24.5K  -
root@master:~# cat /mnt/zfspv-pool/dataset1/snap_num.txt 
snap1

# 查看快照的创建时间，默认按时间先后排序
root@master:~# zfs list -t snapshot zfspv-pool/dataset1 -o creation,name,used,available,clones
CREATION               NAME                        USED  AVAIL  CLONES
Tue Jan 14  1:23 2025  zfspv-pool/dataset1@snap1  14.5K      -  
Tue Jan 14  1:57 2025  zfspv-pool/dataset1@snap2  12.5K      -  

# 从快照创建克隆，也可以通过-o指定更多option
# zfs clone -o quota=16106127360 -o mountpoint=legacy -o dedup=off -o compression=off <snap> <clone>
root@master:~# zfs clone zfspv-pool/dataset1@snap1  zfspv-pool/dataset1-snap1-clone
root@master:~# zfs list
NAME                                                  USED  AVAIL     REFER  MOUNTPOINT
zfspv-pool                                           3.04G   412G       24K  /zfspv-pool
zfspv-pool/dataset1                                    39K   412G     24.5K  /mnt/zfspv-pool/dataset1
zfspv-pool/dataset1-snap1-clone                         0B   412G     24.5K  /zfspv-pool/dataset1-snap1-clone
```

