# Introduction

## Documents

MongoDB stores data records as `BSON` documents. BSON is a binary representation of [JSON](https://www.mongodb.com/docs/manual/reference/glossary/#std-term-JSON)documents, though it contains more data types than JSON.

![BSON document](https://www.mongodb.com/docs/manual/images/crud-annotated-document.bakedsvg.svg)

### Document Structure

The value of a field can be any of the BSON [data types](https://www.mongodb.com/docs/manual/reference/bson-types/), including other documents, arrays, and arrays of documents. For example, the following document contains values of varying types:

```json
var mydoc = {
               _id: ObjectId("5099803df3f4948bd2f98391"),
               name: { first: "Alan", last: "Turing" },
               birth: new Date('Jun 23, 1912'),
               death: new Date('Jun 07, 1954'),
               contribs: [ "Turing machine", "Turing test", "Turingery" ],
               views : NumberLong(1250000)
            }
```

The above fields have the following data types:

- `_id` holds an [ObjectId.](https://www.mongodb.com/docs/manual/reference/bson-types/#std-label-objectid)
- `name` holds an *embedded document* that contains the fields `first` and `last`.
- `birth` and `death` hold values of the *Date* type.
- `contribs` holds an *array of strings*.
- `views` holds a value of the *NumberLong* type.

### Document Limitations

The maximum BSON document size is 16 MB. To store documents larger than the maximum size, MongoDB provides the `GridFS` API.

#### Document字段顺序

Unlike JavaScript objects, the fields in a BSON document are ordered：

1. The `_id` field is always the first field in the document.
2. Updates that include [`renaming`](https://www.mongodb.com/docs/manual/reference/operator/update/rename/#mongodb-update-up.-rename) of field names may result in the reordering of fields in the document.

#### 关于_id字段

存储在collection中的每个document都需要一个唯一的`_id`字段，其表现为一个主键（primary key）。如果插入的document省略了`_id`字段，MongoDB将自动生成一个ObjectID赋给`_id`字段。

在创建一个collection的时候，MongoDB默认会在`_id`字段上创建一个唯一的索引。

# CURD opearations

关于MongoDB CURD的更多细节直接参考官网：[MongoDB CRUD Operations](https://www.mongodb.com/docs/rapid/crud/)

关于SQL和MongoDB的概念以及操作的映射关系可以参考：[SQL to MongoDB Mapping Chart](https://www.mongodb.com/docs/rapid/reference/sql-comparison/)

## Insert operations

```bash
test> db.setting.insertOne({"groupId":"User","subGroupId":"setting","key":"ALERTPOLICY","value":[{"id":"SNMP","deliveryTime":"n/a","target":{"address":"","protocol":"SNMP","locale":null,"properties":{"ip":"","port":"162","community":"public"}},"interval":1440,"rules":[{"type":"ALERT_ALWAYS","components":["Connections","ThermalSensor","Memory"],"suppressedUMIs":["M-409-820-26","M-409-820-2"],"alertOnceUMIs":null}],"enabled":false}]})
{
  acknowledged: true,
  insertedId: ObjectId("6445db30b1cc4a6b3c3829e7")
}
test> db.setting.find({key:"ALERTPOLICY"}).pretty()
[
  {
    _id: ObjectId("6445db30b1cc4a6b3c3829e7"),
    groupId: 'User',
    subGroupId: 'setting',
    key: 'ALERTPOLICY',
    value: [
      {
        id: 'SNMP',
        deliveryTime: 'n/a',
        target: {
          address: '',
          protocol: 'SNMP',
          locale: null,
          properties: { ip: '', port: '162', community: 'public' }
        },
        interval: 1440,
        rules: [
          {
            type: 'ALERT_ALWAYS',
            components: [ 'Connections', 'ThermalSensor', 'Memory' ],
            suppressedUMIs: [ 'M-409-820-26', 'M-409-820-2' ],
            alertOnceUMIs: null
          }
        ],
        enabled: false
      }
    ]
  }
]
```

## Delete operations

MongoDB删除操作的基本语句：

- db.inventory.deleteOne( { status: "D" } ) - 删除匹配到的第一个document，如果没有匹配到就不删除
- db.inventory.deleteMany({ status : "A" }) - 删除匹配的所有document
- db.inventory.deleteMany({})  - 删除collection里的所有document

## Query operations

下面的查询选择嵌套字段h小于15，嵌套字段uom等于“in”，状态字段等于“D”的所有文档:

```bash
db.inventory.find( { "size.h": { $lt: 15 }, "size.uom": "in", status: "D" } )
```

## Update operations

MongoDB更新操作的基本语句：

- db.collection.updateOne(<filter>, <update>, <options> )
- db.collection.updateMany(<filter>, <update>, <options> )
- db.collection.replaceOne(<filter>, <update>, <options> )

使用`update`操作符的基本格式：

```json
{
  <update operator>: { <field1>: <value1>, ... },
  <update operator>: { <field2>: <value2>, ... },
  ...
}
```

1. 可以存在多个`<filed>:<value>`对，意味着可以同时更新多个不同的字段。

2. 其中`field`可以用`.`跟数组下标或者字段名来索引想要更新的字段，但是整个`filed`必须用双引号括起来。
3. 支持的更新操作符：
   1. `$set`：更新指定filed的value，如果该field不存在会直接创建新的field。
4. MongoDB中的所有写操作在单个document级别上都是原子的。
5. document字段是有序的，并且`_id`字段永远是第一个。如果更新操作里包含重命名字段名，可能触发document字段的重新排序。
6. 不支持更新`_id`字段。

```bash
test> db.setting.updateOne({key:"ALERTPOLICY"}, {$set: { "value.0.rules.0.suppressedUMIs": [ "V-475-702-1007", "V-409-820-26", "V-409-820-2" ] }})
{
  acknowledged: true,
  insertedId: null,
  matchedCount: 1,
  modifiedCount: 1,
  upsertedCount: 0
}
test> db.setting.find({key:"ALERTPOLICY"}).pretty()
[
  {
    _id: ObjectId("6445db30b1cc4a6b3c3829e7"),
    groupId: 'User',
    subGroupId: 'setting',
    key: 'ALERTPOLICY',
    value: [
      {
        id: 'SNMP',
        deliveryTime: 'n/a',
        target: {
          address: '',
          protocol: 'SNMP',
          locale: null,
          properties: { ip: '', port: '162', community: 'public' }
        },
        interval: 1440,
        rules: [
          {
            type: 'ALERT_ALWAYS',
            components: [ 'Connections', 'ThermalSensor', 'Memory' ],
            suppressedUMIs: [ 'V-475-702-1007', 'V-409-820-26', 'V-409-820-2' ],
            alertOnceUMIs: null
          }
        ],
        enabled: false
      }
    ]
  }
]
```

## Bulk Operations

Bulk operations用于执行大量的操作，比如`BulkWrite`可以同时执行批量的不同类型的写操作。这种类型的批量操作不是对数据库的每个操作都进行调用，而是通过对数据库的一次调用来执行多个操作。

```go
models := []mongo.WriteModel{
  mongo.NewInsertOneModel().SetDocument(Book{Title: "Middlemarch", Author: "George Eliot", Length: 904}),
  mongo.NewInsertOneModel().SetDocument(Book{Title: "Pale Fire", Author: "Vladimir Nabokov", Length: 246}),
  mongo.NewReplaceOneModel().SetFilter(bson.D{{"title", "My Brilliant Friend"}}).
    SetReplacement(Book{Title: "Atonement", Author: "Ian McEwan", Length: 351}),
  mongo.NewUpdateManyModel().SetFilter(bson.D{{"length", bson.D{{"$lt", 200}}}}).
    SetUpdate(bson.D{{"$inc", bson.D{{"length", 10}}}}),
  mongo.NewDeleteManyModel().SetFilter(bson.D{{"author", bson.D{{"$regex", "Jam"}}}}),
}
//默认地，SetOrdered(true)：按定义顺序执行这些操作，发生错误时停止整个BulkWrite
//设置这些写操作是无序的，发生错误不会停止整个BulkWrite，其余写操作继续执行
opts := options.BulkWrite().SetOrdered(false)

results, err := coll.BulkWrite(context.TODO(), models, opts)
if err != nil {
   panic(err)
}

fmt.Printf("Number of documents inserted: %d\n", results.InsertedCount)
fmt.Printf("Number of documents replaced or updated: %d\n", results.ModifiedCount)
fmt.Printf("Number of documents deleted: %d\n", results.DeletedCount)
```

## Compound Operations

[Compound Operations](https://www.mongodb.com/docs/drivers/go/current/fundamentals/crud/compound-operations/#std-label-golang-find-and-delete)将读和写操作合并为一个原子操作。如果分别执行读和写操作，尽管单独的读操作和写操作都是原子操作，但是其他人可能会在这两个操作之间修改document，导致非预期的修改结果。MongoDB通过在Compound Operations期间对正在修改的的document设置写锁来防止这种情况。

MongoDB支持的Compound Operations：

1. `FindOneAndDelete()`：查找与指定查询筛选器匹配的第一个document并将其删除。
2. `FindOneAndUpdate()`：查找与指定查询过滤器匹配的第一个文档，并根据参数传入的更新文档对其进行更新。
3. `FindOneAndReplace()`：查找与指定查询筛选器匹配的第一个文档，并用参数传入的替换文档替换它。

## Write/Read Concern and Read Preference

MongoDB允许我们在下面不同对象级别里设置Write Concern, Read Concern和 Read Preference这三种option:

- Client level, which sets the *default for all operation executions* unless overridden
- Session level
- Transaction level
- Database level
- Collection level
- Query level

### Write Concern

`Write Concern`描述了MongoDB对独立mongod、副本集（replica sets）或分片集群（shared clusters）进行写操作时请求的确认级别。在分片集群中，mongos实例将Write Concern传递给分片。

了解了存储引擎的工作原理后，我们知道MongoDB的写操作在极端情况下仍然可能会丢失。对于安全性要求极高的数据，可以使用Write Concrn来保证写入的数据得到更高级别的承诺。

Write Concern的基本格式：

```jso
{ w: <value>, j: <boolean>, wtimeout: <number> }
```

`w`选项请求确认写操作已传播到指定数量的mongod实例或具有指定标记的mongod实例。`w`选项其实是一种数据复制策略，它的value可以是：

1. `"majority"`：请求确认写操作已持久地提交给大多数计算承载数据的投票成员(即具有成员[n]的主要和次要成员)。{w: "majority"}是大多数MongoDB部署的默认Write Concern。比如对于有3个投票成员（Primary-Secondary-Secondary (P-S-S)）的replica set，大多数就是2。写操作向客户端返回{w: "majority"}确认后，客户端可以通过"majority" readConcrn来读取结果。
2. `<number>`：请求确认写操作已持久地提交给指定数目的MongoDB实例。

`j`选项请求确认写操作已写入磁盘日志（[on-disk journal](https://www.mongodb.com/docs/manual/core/journaling/#std-label-journaling-internals)），而不仅仅是写入内存缓冲区。要求MongoDB先打开了journal，否则会产生错误。

`wtimeout`选项指定毫秒级的时间限制，以防止写操作无限期阻塞。

1. ​	即使所需的Write Concern最终会成功，Wtimeout也会导致写操作在指定的限制之后返回错误。当这些写操作返回时，MongoDB不会撤销在Write Concern超过wtimeout时间限制之前执行的成功数据修改。
2. 如果未指定wtimeout选项，且写操作级别无法实现，则写操作将无限期阻塞。指定wtimeout值为0相当于不带wtimeout选项的Write Concern。

```go
uri := "mongodb://<hostname>:<port>"
wc := writeconcern.W(2)
opts := options.Client().ApplyURI(uri).SetWriteConcern(writeconcern.New(wc))
client, err := mongo.Connect(context.TODO(), opts)
```

更多细节建议直接参考[Write Concern](https://www.mongodb.com/docs/manual/reference/write-concern/)

### Read Concern

[Read Concern](https://www.mongodb.com/docs/rapid/reference/read-concern/)选项允许我们明确客户端从查询返回哪些数据。MongoDB支持的Read Concern级别：

1. [ "available"](https://www.mongodb.com/docs/rapid/reference/read-concern-available/#mongodb-readconcern-readconcern.-available-)： 查询从实例返回数据，但不保证数据已写入大多数复制集成员（即读到的数据可能会被rollback）。**"available" read concern不可用于因果一致的会话和事务**。
   1. 对于分片集群（sharded cluster），“available”为分区提供了更大的容忍度，因为它不需要等待来确保一致性保证。即“available”不联系分片的主服务器，也不联系配置服务器更新元数据。这意味着：如果分片正在进行块迁移，使用“available” read concern的查询可能会返回孤立文档（[orphaned documents](https://www.mongodb.com/docs/rapid/reference/glossary/#std-term-orphaned-document)）。
   2. 对于未分片的collection(包括独立部署或复制集部署中的collection)，“local”和“available”的行为相同。
2. ["local"](https://www.mongodb.com/docs/rapid/reference/read-concern-local/#mongodb-readconcern-readconcern.-local-)：默认的Read Conern级别，这意味着客户端返回实例的最新数据，但不保证数据已被写入大多数副本集成员（即读到的数据可能会被rollback）。"local"和"available"非常类似，区别在于**“local”级别可用于有或没有因果一致的会话和事务**。
3. ["majority"](https://www.mongodb.com/docs/rapid/reference/read-concern-majority/#mongodb-readconcern-readconcern.-majority-)：查询返回实例最近的数据，这些数据被确认已写入副本集中的大多数成员。**“majority”可用于有或没有因果一致的会话和事务**。
   1. 为了实现“majority”，复制集成员从多数提交点的数据的内存视图返回数据。因此，“majority”在性能成本上与其他read concern相当。
   2. 为了使用"majority"，复制集必须使用`WiredTiger`存储引擎。
   3. 对于多文档事务（[multi-document transactions](https://www.mongodb.com/docs/rapid/core/transactions/)），"majority"仅在事务以"majority" write concern提交时提供保证。否则"majority" read concern无法保证事务中读取的数据。
4. ["linearizable"](https://www.mongodb.com/docs/rapid/reference/read-concern-linearizable/#mongodb-readconcern-readconcern.-linearizable-)：该查询返回的数据反映了所有成功的写操作，这些写操作在读操作开始之前以多数票的形式发出并得到确认。
5. ["snapshot"](https://www.mongodb.com/docs/rapid/reference/read-concern-snapshot/#mongodb-readconcern-readconcern.-snapshot-)：该查询返回mongod实例中数据在特定时间点的完整副本。仅可用于多文档事务中的操作。

```go
// majority
rc := readconcern.Majority()
opts := options.Collection().SetReadConcern(rc)
database := client.Database("myDB")
coll := database.Collection("myCollection", opts)
```



### Read Preference

Read Preference选项指定MongoDB客户端如何将读操作路由到副本集的成员。默认情况下，应用程序将其读操作定向到复制集中的主成员（Primary Member）。



# Data Models



# Storage

## Storage Engine

存储引擎主要负责管理数据如何在内存和硬盘中存储。

> Starting in version 4.2, MongoDB removes the deprecated MMAPv1 storage engine.
>
> [WiredTiger](https://www.mongodb.com/docs/v4.4/core/wiredtiger/) is the default storage engine starting in MongoDB 3.2
>
> With the WiredTiger storage engine, using XFS for data bearing nodes is recommended on Linux.

### [WiredTiger Storage Engine （Default）](https://www.mongodb.com/docs/v4.4/core/wiredtiger/)

WiredTiger存储引擎的特性：

#### Document Level Concurrency

WiredTiger对写操作使用文档级并发控制。因此，多个客户机可以同时修改集合的不同文档。

对于大多数读写操作，WiredTiger使用乐观并发控制。WiredTiger只在全局、数据库和集合级别使用意图锁。当存储引擎检测到两个操作之间存在冲突时，其中一个操作将引发写冲突，导致MongoDB透明地重试该操作。

一些全局操作(通常是涉及多个数据库的短期操作)仍然需要全局“实例范围”锁。其他一些操作，如`collMod`，仍然需要独占数据库锁。

#### Snapshots and Checkpoints

WiredTiger使用`多版本并发控制(MVCC`)。在操作开始时，WiredTiger为操作提供数据的某个时间点快照(point-in-time snapshot)。快照提供了内存中数据的一致视图。

当写入磁盘时，WiredTiger跨所有数据文件以一致的方式将`snapshot`中的所有数据写入磁盘。现在持久的数据充当数据文件中的`checkpoint`。checkpoint确保数据文件总是和最后一个checkpoint是一致的；所以，checkpoint可以作为恢复点。

从3.6版本开始，MongoDB配置WiredTiger每隔60秒创建checkpoint(即将快照数据写入磁盘)。在早期版本中，MongoDB在WiredTiger中对用户数据设置检查点，时间间隔为60秒，或者当2 GB的日志数据被写入时，以先发生的时间为准。

在写入新检查点期间，前一个检查点仍然有效。因此，即使MongoDB在写入新检查点时终止或遇到错误，在重新启动时，MongoDB也可以从上次有效的检查点恢复。

当WiredTiger的元数据表被自动更新以引用新的检查点时，新的检查点变得可访问和永久。一旦新的检查点可以访问，WiredTiger就会从旧的检查点中释放页面。

使用WiredTiger，即使没有`journaling`， MongoDB可以从上一个checkpoint恢复；但是，要恢复上次检查点之后所做的更改，请需要打开`journaling`。

#### Journal

WiredTiger使用`预写日志`(WAL, write-ahead log，即日志)与`checkpoint`相结合来确保数据的持久性。

WiredTiger日志在checkpoint之间持久化所有数据修改。如果MongoDB在checkpoint之间退出，它使用使用日志恢复自上次checkpoint以来修改的所有数据。

对于超过128字节的日志记录，WiredTiger使用使用`snappy`压缩库对其进行压缩。

#### Compression

默认情况下，WiredTiger对所有集合使用带有snappy压缩库的块压缩，对所有索引使用前缀压缩（prefix compression）。我们也可以为collection配置使用`zlib`或者`zstd`这些块压缩库。

#### Memory Use

使用WiredTiger存储引擎, MongoDB同时利用WiredTiger内部缓存和文件系统缓存。

从MongoDB 3.4开始，默认的WiredTiger内部缓存大小是两者中较大的:

1. 50%  x (RAM - 1 GB)，或者
2. 256 MB。

比如，系统有4GB的RAM，那么WiredTiger内部缓存大小就是50% x (4GB -1GB) = 1.5GB。如果计算出来小于256MB，那就是256MB。

### In-Memory Storage Engine

## Journaling

### 日志流程（Journaling Process）

使用`Journaling`，WiredTiger为每个客户端发起的写操作创建一个日志记录（journal record），每个记录有唯一的标识符，WiredTiger的最小日志记录大小是128字节。日志记录包括由客户端的初始写引起的所有内部写操作。例如，对集合中的文档的更新可能会导致索引的修改；WiredTiger创建一个日志记录，其中包括更新操作和相关的索引修改。

MongoDB配置WiredTiger使用`内存缓冲`来存储日志记录。线程协调分配和复制到它们的缓冲区部分。128 kB以内的所有日志记录都被缓冲在内存中。

下列任意条件满足，WiredTiger将缓冲的日志记录同步到磁盘上：

1. 对于复制集（replica set）成员(主成员和从成员)，
   1. 是否有等待`oplog`表项的操作。可以等待oplog条目的操作包括:
      1. 对oplog进行前向扫描查询。
      2. 作为因果一致性会话的一部分执行的读操作。
   2. 此外，对于次要成员，在每次批量应用oplog条目之后。
2. 如果写操作包含或暗示写关注点（`Write Concern`）为`j: true`（参考：[writeConcernMajorityJournalDefault](https://www.mongodb.com/docs/v4.4/reference/replica-configuration/#mongodb-rsconf-rsconf.writeConcernMajorityJournalDefault)）。
3. 每100毫秒（参考：[storage.journal.commitIntervalMs](https://www.mongodb.com/docs/v4.4/reference/configuration-options/#mongodb-setting-storage.journal.commitIntervalMs)）
4. 当WiredTiger创建一个新的日志文件时。由于MongoDB使用的日志文件大小限制为100 MB，因此WiredTiger大约每100 MB的数据创建一个新的日志文件。

### 日志文件（Journaling Files）

日志文件路径：<dbPath>/journal/WiredTigerLog.<sequence>，sequence从0000000001开始。

WiredTiger会预分配一些日志文件：<dbPath>/journal/WiredTigerPreLog.<sequence>。

MongoDB的WiredTiger日志文件的最大大小限制约为100 MB。一旦文件超过这个限制，WiredTiger就会创建一个新的日志文件。

WiredTiger自动删除旧的日志文件，只维护从上次检查点恢复所需的文件。

## GridFS

GridFS是一种多功能存储系统，适合处理大文件，例如超过`16MB`的文档大小限制的大文件。

# MongoDB Backup Methods



# Troubleshooting

## Collection file became corrupted

后端服务每次访问MongoDB的一个collection时，整个MongoDB进场都因为异常被shutdown了，导致服务端异常，MongoDB的主要日志如下：

```bash
{"t":{"$date":"2023-04-19T23:25:03.565-07:00"},"s":"E",  "c":"STORAGE",  "id":22435,   "ctx":"conn2","msg":"WiredTiger error","attr":{"error":-31802,"message":"[1681971903:565145][189599:0x7f789f78b700], file:collection-21-7863571732508414186.wt, WT_CURSOR.next: __wt_block_read_off, 293: collection-21-7863571732508414186.wt: fatal read error: WT_ERROR: non-specific WiredTiger error"}}{"t":{"$date":"2023-04-19T23:25:03.565-07:00"},"s":"E",  "c":"STORAGE",  "id":22435,   "ctx":"conn2","msg":"WiredTiger error","attr":{"error":-31804,"message":"[1681971903:565154][189599:0x7f789f78b700], file:collection-21-7863571732508414186.wt, WT_CURSOR.next: __wt_block_read_off, 293: the process must exit and restart: WT_PANIC: WiredTiger library panic"}}{"t":{"$date":"2023-04-19T23:25:03.565-07:00"},"s":"F",  "c":"-",        "id":23089,   "ctx":"conn2","msg":"Fatal assertion","attr":{"msgid":50853,"file":"src/mongo/db/storage/wiredtiger/wiredtiger_util.cpp","line":521}} {"t":{"$date":"2023-04-19T23:25:03.565-07:00"},"s":"F",  "c":"-",        "id":23090,   "ctx":"conn2","msg":"\n\n***aborting after fassert() failure\n\n"}{"t":{"$date":"2023-04-19T23:25:03.565-07:00"},"s":"F",  "c":"CONTROL",  "id":4757800, "ctx":"conn2","msg":"Writing fatal message","attr":{"message":"Got signal: 6 (Aborted).\n"}}
```

怀疑是`collection-21-7863571732508414186.wt`这个db文件已经坏掉了，根据官网手册[Recover a Standalone after an Unexpected Shutdown](https://www.mongodb.com/docs/v4.4/tutorial/recover-data-following-unexpected-shutdown/)执行下面自动恢复操作（注意切换到MongoDB的服务用户，其他用户可能会更改文件权限）：

```bash
$ sudo -u mongod mongod --dbpath /config/mongodb --repair
...
{"t":{"$date":"2023-04-20T00:42:23.226-07:00"},"s":"I",  "c":"STORAGE",  "id":21027,   "ctx":"initandlisten","msg":"Repairing collection","attr":{"namespace":"autosupportClient.telemetry"}}
{"t":{"$date":"2023-04-20T00:42:23.238-07:00"},"s":"I",  "c":"STORAGE",  "id":22431,   "ctx":"initandlisten","msg":"WiredTiger progress","attr":{"operation":"WT_SESSION.verify","progress":100}}
{"t":{"$date":"2023-04-20T00:42:23.240-07:00"},"s":"E",  "c":"STORAGE",  "id":22435,   "ctx":"initandlisten","msg":"WiredTiger error","attr":{"error":0,"message":"[1681976543:240389][269338:0x7fccc502e000], file:collection-21-7863571732508414186.wt, WT_SESSION.verify: __wt_block_read_off, 284: collection-21-7863571732508414186.wt: read checksum error for 16384B block at offset 1449984: block header checksum of 0x9ded6650 doesn't match expected checksum of 0x703028e1"}}
...
{"t":{"$date":"2023-04-20T00:42:23.242-07:00"},"s":"E",  "c":"STORAGE",  "id":22435,   "ctx":"initandlisten","msg":"WiredTiger error","attr":{"error":0,"message":"[1681976543:242476][269338:0x7fccc502e000], file:collection-21-7863571732508414186.wt, WT_SESSION.verify: __verify_ckptfrag_chk, 520: checkpoint ranges never verified: 14"}}
{"t":{"$date":"2023-04-20T00:42:23.242-07:00"},"s":"E",  "c":"STORAGE",  "id":22435,   "ctx":"initandlisten","msg":"WiredTiger error","attr":{"error":0,"message":"[1681976543:242491][269338:0x7fccc502e000], file:collection-21-7863571732508414186.wt, WT_SESSION.verify: __verify_filefrag_chk, 434: file ranges never verified: 14"}}
{"t":{"$date":"2023-04-20T00:42:23.242-07:00"},"s":"I",  "c":"STORAGE",  "id":22328,   "ctx":"initandlisten","msg":"Verify failed. Running a salvage operation.","attr":{"uri":"table:collection-21-7863571732508414186"}}
{"t":{"$date":"2023-04-20T00:42:23.247-07:00"},"s":"I",  "c":"STORAGE",  "id":22431,   "ctx":"initandlisten","msg":"WiredTiger progress","attr":{"operation":"WT_SESSION.salvage","progress":100}}
{"t":{"$date":"2023-04-20T00:42:23.285-07:00"},"s":"I",  "c":"STORAGE",  "id":22431,   "ctx":"initandlisten","msg":"WiredTiger progress","attr":{"operation":"WT_SESSION.salvage","progress":200}}
{"t":{"$date":"2023-04-20T00:42:23.295-07:00"},"s":"I",  "c":"INDEX",    "id":20384,   "ctx":"initandlisten","msg":"Index build: starting","attr":{"namespace":"autosupportClient.telemetry","buildUUID":null,"properties":{"v":2,"key":{"_id":1},"name":"_id_"},"method":"Hybrid","maxTemporaryMemoryUsageMB":100}}
{"t":{"$date":"2023-04-20T00:42:23.298-07:00"},"s":"I",  "c":"INDEX",    "id":20384,   "ctx":"initandlisten","msg":"Index build: starting","attr":{"namespace":"autosupportClient.telemetry","buildUUID":null,"properties":{"v":2,"key":{"expireAt":1},"name":"expire_At","expireAfterSeconds":0},"method":"Hybrid","maxTemporaryMemoryUsageMB":100}}
{"t":{"$date":"2023-04-20T00:42:23.299-07:00"},"s":"I",  "c":"STORAGE",  "id":20673,   "ctx":"initandlisten","msg":"Index builds manager starting","attr":{"buildUUID":{"uuid":{"$uuid":"83fa00b1-a544-4fae-b9a3-471267300380"}},"namespace":"autosupportClient.telemetry"}}
{"t":{"$date":"2023-04-20T00:42:23.355-07:00"},"s":"I",  "c":"INDEX",    "id":20685,   "ctx":"initandlisten","msg":"Index build: inserted keys from external sorter into index","attr":{"namespace":"autosupportClient.telemetry","index":"_id_","keysInserted":7269,"durationMillis":0}}
{"t":{"$date":"2023-04-20T00:42:23.359-07:00"},"s":"I",  "c":"INDEX",    "id":20685,   "ctx":"initandlisten","msg":"Index build: inserted keys from external sorter into index","attr":{"namespace":"autosupportClient.telemetry","index":"expire_At","keysInserted":7269,"durationMillis":0}}
{"t":{"$date":"2023-04-20T00:42:23.360-07:00"},"s":"I",  "c":"INDEX",    "id":20345,   "ctx":"initandlisten","msg":"Index build: done building","attr":{"buildUUID":null,"namespace":"autosupportClient.telemetry","index":"_id_","commitTimestamp":{"$timestamp":{"t":0,"i":0}}}}
{"t":{"$date":"2023-04-20T00:42:23.360-07:00"},"s":"I",  "c":"INDEX",    "id":20345,   "ctx":"initandlisten","msg":"Index build: done building","attr":{"buildUUID":null,"namespace":"autosupportClient.telemetry","index":"expire_At","commitTimestamp":{"$timestamp":{"t":0,"i":0}}}}
{"t":{"$date":"2023-04-20T00:42:23.361-07:00"},"s":"I",  "c":"STORAGE",  "id":20674,   "ctx":"initandlisten","msg":"Index builds manager completed successfully","attr":{"buildUUID":{"uuid":{"$uuid":"83fa00b1-a544-4fae-b9a3-471267300380"}},"namespace":"autosupportClient.telemetry","indexSpecsRequested":2,"numIndexesBefore":2,"numIndexesAfter":2}}
{"t":{"$date":"2023-04-20T00:42:23.364-07:00"},"s":"I",  "c":"STORAGE",  "id":22430,   "ctx":"initandlisten","msg":"WiredTiger message","attr":{"message":"[1681976543:364349][269338:0x7fccc502e000], WT_SESSION.checkpoint: [WT_VERB_CHECKPOINT_PROGRESS] saving checkpoint snapshot min: 38, snapshot max: 38 snapshot count: 0, oldest timestamp: (0, 0) , meta checkpoint timestamp: (0, 0) base write gen: 899969"}}
```

# References

[MongoDB Manual](https://www.mongodb.com/docs/v4.4/)

[MongoDB Architecture Guide](https://www.mongodb.com/collateral/mongodb-architecture-guide)

[MongoDB’s Pluggable Storage Engine Architecture](https://www.mongodb.com/collateral/storage-engine-white-paper)

[[MONGODB SCHEMA DESIGN](http://learnmongodbthehardway.com/)](http://learnmongodbthehardway.com/schema/introduction/)

https://www.elibaron.com/db/mongodb/mongo-x-overview.html