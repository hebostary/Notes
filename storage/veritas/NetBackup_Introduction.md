# NetBackup host roles

![img](https://hunk-pic-store.oss-cn-beijing.aliyuncs.com/img/%E6%88%AA%E5%9B%BE.png)



NetBackup accommodates multiple servers that work together under the
administrative control of one NetBackup master server in the following ways:  

1. **Master server**: the master server manages backups, archives, and restores. The master server is responsible for media and device selection for NetBackup. Typically, the master server contains the NetBackup catalog. The catalog contains the internal databases that contain information about NetBackup backups and configuration.
2. **Media server**s: the media servers provide additional storage by allowing NetBackup to use the
   storage devices that are attached to them. Media servers can also increase
   performance by distributing the network load. Media servers can also be referred to by using the following terms:  
   1. Device hosts (when tape devices are present)  
   2. Storage servers (when I/O is directly to disk)  
   3. Data movers (when data is sent to independent, external disk devices like OpenStorage appliances)  
3. **Clients**: During a backup or archive, the client sends backup data across the network to a NetBackup server.   

# NetBackup concepts

## Storage unit

A storage unit is a label that NetBackup associates with physical storage. The label can identify a robot, a path to a volume, or a disk pool.

### Storage unit types

![img](https://hunk-pic-store.oss-cn-beijing.aliyuncs.com/img/clipboard.png)

### Disk storage model

The NetBackup model for disk storage accommodates all Enterprise Disk Options. That is, it is the model for all disk types except for the `BasicDisk` type.

The following items describe components of the disk storage model:

**Data mover**

A data mover transfers data from primary storage (a NetBackup client) to secondary storage during backups. It also can move data back to primary storage during restores and from secondary storage to tertiary storage during duplication. **NetBackup media servers function as data movers.** Depending on the disk option, a NetBackup media server also may function as a storage server.

**Storage server**

An entity that writes data to and reads data from the disk storage. A storage server is the entity that has a mount on the file system on the storage. Depending on the NetBackup option, the storage server is one of the following:

1. A computer that hosts the storage. The computer may be embedded in the storage device.

2. A storage vendor's host on the Internet that exposes cloud storage to NetBackup. Alternatively, private cloud storage can be hosted within your private network. 

   For example,  when we adding an storage server based on Cloud storage, we need to tell NetBackup below things:

   1. Cloud storage vendor, such as Alibaba cloud OSS and Amazon S3.
   2. Cloud region, the cloud region always have related service host such as oss-cn-beijing.aliyuncs.com.
   3. Credential including Access Key and Secret Key.

   Based on above information, NetBackup can get the available `buckets` on cloud storage and these buckets can be used for creating new cloud disk pool.

3. A NetBackup media server that hosts storage. Such as NBA.

**Disk pool**

A collection of `disk volumes` that are administered as an entity. NetBackup aggregates the `disk volumes` into pools of storage (a disk pool) you can use for backups.

**A disk pool is a storage type in NetBackup**. When you create a storage unit, you select the disk type(storage category) and then you select a specific disk pool.

> A disk pool related to a specific storage server and one or multiple disk volumes with same storage type and located on same storage server.

Use below command to query disk pool and disk volume information:

```bash
$ nbdevquery -listdp -stype BasicDisk|AdvancedDisk|PureDisk|CloudStorage|OpenStorage -U
$ nbdevquery -listdv -stype BasicDisk|AdvancedDisk|PureDisk|CloudStorage|OpenStorage -U
```

### Disk storage unit

#### AdvancedDisk

An AdvancedDisk disk type storage unit is used for a dedicated disk that is directly attached to a NetBackup media server. This mean that AdvancedDisk storage can be direct attached storage (DAS), network attached storage (NAS), or storage area network (SAN) storage.  

> NetBackup cannot discover Windows CIFS disk volumes. For CIFS storage, we must use the Windows Universal Naming Convention (UNC) notation to specify the volumes.  

NetBackup assumes the exclusive ownership of the disk resources that comprise an AdvancedDisk disk pool. If the resources are shared with other users, NetBackup cannot manage disk pool capacity or storage lifecycle policies correctly.

For AdvancedDisk, the NetBackup media servers function as both **data movers** and **storage servers**, and data movers are configured when you configure storage servers.  

![image-20210622221906452](https://hunk-pic-store.oss-cn-beijing.aliyuncs.com/img/20210622221906.png)

**AdvancedDisk benefits**

1. Easy to deploy and use.
2. Easy to extend storage. Only add the capacity required, then update the NetBackup disk pools.
3. Multiple storage units can share a disk pool. Space is allocated dynamically as required. 
4. Load balancing, can occur at two levels:
   1. Storage servers. If you have more than one storage server, NetBackup chooses the least busy media server for each backup. 
   2. Storage unit groups. If you have more than one storage unit, you can choose the storage unit group Load Balance option. If the destination of each storage unit is a separate disk pool, NetBackup chooses the least busy disk pool.
5. And so on...

**Configuring an AdvancedDisk disk type STU**

Use nbdevconfig command as example:

1. Configuring an AdvancedDisk storage server, run the following command on the NetBackup master server or media server:

   ```bash
   $ /usr/openv/netbackup/bin/admincmd/nbdevconfig -creatests
   -storage_server hostname -stype server_type -st 5 -media_server
   hostname [-setattribute attribute]
   # server_type: AdvancedDisk | AdvancedDisk_crypt
   ```

2. Verifying AdvancedDisk storage server:

   ```bash
   $ /usr/openv/netbackup/bin/admincmd/nbdevquery -liststs -u
   ```

3. Querying disk volume list. On the host that has a file system mount on the storage, discover the volumes that are available and write them to a text file. If you intend to use multiple hosts to read to and write from the storage, use any of the hosts. 

   ```bash
   $ /usr/openv/netbackup/bin/admincmd/nbdevconfig -previewdv
   -storage_server hostname -media_server hostname -stype server_type > filename 
   # TODO: need to copy above file to master serevr?
   ```

   Delete the line for each volume that you do not want to be in
   the disk pool. Do not delete the blank line at the end of the file.

   NetBackup does not filter out common file system mount points, such as / and /usr. Therefore, carefully choose the volumes to include in a disk pool.  

4. Configuring disk pool by using the following command:

   TODO: Where to execute below command if multiple storage servers.

   ```bash
   $ /usr/openv/netbackup/bin/admincmd/nbdevconfig -createdp -dp
   disk_pool_name -stype server_type -storage_servers hostname...
   -dvlist filename [-reason "string"] [-lwm low_watermark_percent]
   [-max_io_streams n] [-comment comment] [-M master_server] [-reason "string"]
   ```

   nbdevconfig command options for AdvancedDisk disk pools:

   - `-dvlist`: The name of the file that contains the information about the volumes for the disk pool.

     - **If selected more than one storage server, `dvlist` should be the disk volumes that are common to all of them.** TODO: NetBackup how to balance backup job between these disk volume on different storage servers.

   - `-hwm`: The high_watermark setting (default: 98%) is a threshold that triggers the following actions:

     - When an individual volume reaches the high_watermark , NetBackup considers the volume full and chooses a different volume in the disk pool to write backup images to.
     - When all volumes in the disk pool reach the high_watermark , the disk pool is considered full. NetBackup fails any backup jobs that are assigned to a storage unit in which the disk pool is full. NetBackup also does not assign new jobs to a storage unit in which the disk pool is full.
     - NetBackup begins image cleanup when a volume reaches the high_watermark. NetBackup again assigns jobs to the storage unit when image cleanup reduces any disk volume's capacity to less than the high_watermark .

   - `-lwm`: The low_watermark is a threshold at which NetBackup stops image cleanup. The default is 80%.

   - `-max_io_streams`: Limit the number of read and write streams (that is, jobs) for each volume in the disk pool. A job may read backup images or write backup images. **By default, there is no limit.** When the limit is reached, NetBackup chooses another volume, if available. If not available, NetBackup queues jobs until a volume is available.

     Too many jobs that read and write data may degrade disk performance because of disk thrashing. (Disk thrashing is when the read and write heads move between the cylinders excessively as they seek the data for competing jobs.) Fewer streams may improve throughput, which may increase the number of jobs that complete in a specific time period. 

     A starting point is to divide the **Maximum concurrent jobs** of all of the storage units by the number of volumes in the disk pool.

5. Configuring AdvancedDisk disk type STU:

   ```bash
   $ /usr/openv/netbackup/bin/admincmd/bpstuadd -lable stu_name -dp disk_pool -cj max_concurrent_jobs -odo on_demand_only_flag -okrt ok_on_root -nodevhost -M master_server 
   ```

   bpstuadd command options for AdvancedDisk STU:

   - `-cj`:  Specify the maximum number of **write jobs** that NetBackup can send to a disk storage unit at one time. A Maximum concurrent jobs setting of 0 disables the storage unit.
   - `-odo`: 
   - `-okrt`:

More details please refer to **Veritas NetBackup™ AdvancedDisk Storage Solutions Guide**

#### BasicDisk

A BasicDisk type storage unit consists of a directory on a locally-attached disk or a network-attached disk that is exposed as a file system to a NetBackup media server. NetBackup stores backup data in the specified directory.

Notes about the BasicDisk type storage unit:

- Do not include the same volume or file system in multiple BasicDisk storage units.
- BasicDisk storage units cannot be used in a storage lifecycle policy.
- BasicDisk storage units not totally follow above disk storage model, it does not have relationship with storage server and disk pool.

#### Cloud Storage

A Cloud Storage disk type storage unit is used for storage in a cloud, usually provided by a third-party vendor. The actual name of the disk type depends on the cloud storage vendor. 

The cloud storage provided by storage vendor partners is integrated into NetBackup via the API.

A vendor host on the Internet is the storage server. The NetBackup media servers function as the data movers.

#### OpenStorage

An OpenStorage disk type storage unit is used for disk storage, usually provided by a third-party vendor. The actual name of the disk type depends on the vendor. The storage provided by storage vendor partners is integrated into NetBackup via the API.

The storage host is the storage server. The NetBackup media servers function as the data movers. The storage vendor's plug-in must be installed on each media server that functions as a data mover. The login credentials to the storage server must be configured on each media server.

#### PureDisk

A PureDisk disk type storage unit is used for deduplicated data for a Media Server Deduplication Pool (MSDP).

### NDMP storage unit

Media Manager controls NDMP storage units but the units attach to NDMP hosts.

## Backup policy

A policy consist of below components:

### Attributes

**Policy Type**

The Policy type attribute determines the purpose of the policy. The policy type you select typically depends on the type of client to be backed up. Some policy types are not used for client backups. For example, NBU-Catalog is not used for client backups.  

1. MS-Windows

2. Standard

3. MS-SQL-Server

4. Oracle

5. NDMP

**Data classification**

The `Data classification` setting allows the NetBackup administrator to classify data based on relative importance. A classification represents a set of backup requirements. When data must meet different backup requirements, consider assigning different classifications. For example, email backup data can be assigned to the silver data classification and financial data backup may be assigned to the platinum classification.

The` Data Classification` attribute specifies the classification of the storage lifecycle policy that stores the backup. For example, a backup with a gold classification must go to a storage unit with a gold data classification.   

By default, NetBackup provides below four data classifications:   

1. Platinum

2. Gold

3. Silver

4. Bronze

> This attribute is optional and applies only when the backup is to be written to a storage lifecycle policy. If the list displays No data classification, the policy uses the storage selection that is displayed in the Policy storage list. If a data classification is selected, all the images that the policy creates are tagged with the classification ID.  

**Policy Storage - STU**

### Schedules

**Backup type**

1. Full Backup

2. Differential Incremental Backup

3. Cumulative Incremental Backup

4. User Backup

5. User Archive

### Clients

### Backup selections

Choose a list of files and directories on selected clients.



## About staging backups

In the staged backups process, NetBackup writes a backup to a storage unit and then duplicates it to a second storage unit. Eligible backups are deleted on the initial storage unit when space is needed for more backups.  

`TODO`This two-stage process allows a NetBackup environment to leverage the advantages of disk-based backups for recovery in the short term.
Staging also meets the following 

objectives:

1. Allows for faster restores from disk.
2. Allows the backups to run when tape drives are scarce.
3. Allows the data to be streamed to tape without image multiplexing.  

NetBackup offers the following methods for staging backups:

1. Basic disk staging.

   The following storage unit types are available for basic disk staging: BasicDisk and tape.

2. Staging using the `Storage Lifecycle Policies` utility.

   > No BasicDisk or disk staging storage unit can be used in an SLP.

### About basic disk staging

![img](https://hunk-pic-store.oss-cn-beijing.aliyuncs.com/img/20210616163133.png)

The image continues to exist on both the disk staging storage unit and the final destination storage units until the image expires or until space is needed on the disk staging storage unit.

![img](https://hunk-pic-store.oss-cn-beijing.aliyuncs.com/img/20210616163243.png)

When the relocation schedule runs, NetBackup creates a data management job. The job looks for any data that can be copied from the disk staging storage unit to the final destination. The Job Details in the Activity Monitor identify the job as one associated with basic disk staging. The Job Details list displays Disk Staging in the job's Data Movement field.

When NetBackup detects a disk staging storage unit that is full, it pauses the backup. Then, NetBackup finds the oldest images on the storage unit that successfully copied onto the final destination. NetBackup expires the images on the disk staging storage unit to create space.

## Storage Life Policy - SLP

A storage lifecycle policy (SLP) is a storage plan for a set of backups. An SLP is
configured within the Storage Lifecycle Policies utility. An SLP contains instructions in the form of storage operations, to be applied to the data that is backed up by a backup policy. Operations are added to the SLP that determine how the data is stored, copied, replicated, and retained. NetBackup retries the copies as necessary to ensure that all copies are created.  



## Auto Image Replication - AIR



## Open Storage Technology - OST

### What is OST?

1. Provide an abstract interface between data protection applications (DPA) and a wide range of storage appliances.

2. Plugin architecture that allow third party vendors to integrate into NetBackup as a storage destination.

3. Plugins for all other non-cloud hardware/software type storage appliances are written and supported by the manufacturer.

4. Vendors sign up for the OST developers program and are supplied an SDK from which they can develop a plugin.
5. Veritas writes and supports the cloud-based plugins.

### OST overview

To NetBackup anything OST appears to be a disk. Uses the general structure of AdvancedDisk:

1. Storage Unit

2. Disk Pool

3. Disk Volume

4. Storage Server

### Infrastructure

1. Storage Server = hostname of the OST appliance or cloud storage endpoint.
   1. Some vendors have specific formats.
   2. Sometimes hard-coded within the plugin(cloud).

2. SType
   1. Unique name that identifies the vendor's plugin.
   2. Cannot be used by any other vendor.

3. LSU = Logical Storage Unit

   A disk volume or area of storage defined within the appliance, or a bit-bucket / storage blob / etc. within the cloud.

4. OST Plugin

   Plugins (including the version) installed on a media server can listed with **bpstsinfo -pi**.

5. Credentials
   1. Storage appliances will typically require a user name and password to allow access via the plugin.
   2. Cloud storage is conceptually the same but in general uses AK(Access Keys) and SK(Secret Keys).

6. Disk Pools
   1. NetBackup combines one or more LSU's into a disk pool.
   2. Basis of a storage unit.



## Performance & Scalability

### Incremental backup - Client side

增量备份：备份新增的文件B -如果已经备份过的文件A发生改变，也会备份整个文件A。

### Accelerator - Client side

1. Changed block backup -只备份文件内部变化的块，默认块粒度是128 KB，可调整块大小。类似于MSDP在客户端的dedupe。

2. 在客户端维护每个块的校验和数据？

3. 不同文件之间即便是有相同的块，也会同时备份，这里也是和MSDP在后端dedupe的差别。

### Snapshot

Leverage storage system with Replication Director.

1. 如果客户端不支持快照功能：为了保持数据的一致性，在备份之前需要先将应用程序暂停，将内存中的数据刷新到磁盘上，然后等待备份任务完成，应用程序才能继续运行提供业务。
2. 如果客户端支持快照功能：NetBackup可以先对客户端打一个快照（时间很快），然后对快照数据进行备份，备份任务就可以不影响应用程序。

### Near-continuous Data Protection

Schedule Accelerator or snapshot backup every few minutes.

### Scale-up with High Speed Interconnects

FC, ISCSI, Infiniband

### Scale-out

Multi-streaming, multiplexing, automatic load balancing.

### Backup over WAN

1. Accelerator and Dedupe for extreme data reduction.
2. WAN optimization as much as ten times.

### Recovery

**Off-site Disaster Recovery**

# References

https://www.veritas.com/content/support/en_US/doc/27801100-130821038-0/id-SF890399007-130821038