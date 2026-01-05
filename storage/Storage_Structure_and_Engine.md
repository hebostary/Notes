# 存储结构概述

存储引擎是数据库系统的核心组件，它主要负责高效的管理磁盘上的存储空间，以提高数据增删改查的性能。存储结构则是存储引擎管理磁盘存储空间所使用的数据结构。一般的，存储引擎操作的数据有两种类型：

1. 数据文件：用于存储用户数据（数据库行）的文件。
2. 索引文件：用于快速查询数据所在的位置。组织形式有：索引组织表、堆组织表、哈希组织表。

索引文件的组织形式其实和存储引擎的性能关系更加密切。

## 存储结构分类

如下图所示，根据更新数据的不同策略，可以将存储结构分为下面两类：

1. **In-place update structure**：就地更新结构，即直接覆盖旧记录来存储更新的数据，例如`B树`、`B+树`。这些结构通常是**读优化**的，因为只存储每个记录的最新版本。然而，这种设计**牺牲了写性能**，因为更新会导致随机I/O。
2. **Out-of-place update structure**：异位更新结构，即将更新的数据存储到新的位置，而不是原地覆盖旧数据、例如`LSM树`采用追加写的方式更新数据。这种设计提高了写性能，因为它可以**利用顺序I/O来处理写**。它还可以通过不覆盖旧数据来简化恢复过程。然而，这种设计的主要问题是**牺牲了读取性能**，因为记录可能存储在多个位置中的任何一个。此外，这些结构通常需要一个独立的数据重组过程，以不断提高存储和查询效率。

![image-20240411175512104](https://hunk-pic-store.oss-cn-beijing.aliyuncs.com/img/20240411175519.png)

## 存储结构发展历史

1. 1970年，Rudolf Bayer教授在“Organization and Maintenance of Large Ordered Indices”一文中提出了`B树`。
2. 1996年，“The Log-Structured Merge-Tree(LSM-Tree)”论文发布，`LSM树`的概念正式诞生。
3. 2003年，“The Google File System”Google分布式文件系统`GFS`发布，对追加的一致性保证强于改写，此后的分布式文件系统基本都更推进啊使用追加写文件。
4. 2006年，“Bigtable: A Distributed Storage System for Structured Data”Google `Bigtable`发布。
5. 2014年左右，`SSD`开始进入大规模商用阶段，而`LSM树`在`SSD`上有两个显著的优势：
   1. 相比`HDD`，`SSD`读性能的提升（随机读减少了寻道时间）很大程度上弥补了`LSM树`读性能的弱点。
   2. `SSD`是基于闪存存储的，闪存不能覆盖写，只能先擦除才能写入，而`LSM树`追加写的模式天然符合`SSD`的这种特性，从而很大程度上节省了擦除的损耗，并提升了`SSD`的使用寿命。

`B树`作为 In-place update 模式的存储结构，在早期机械硬盘结构上的表现最好，所以占据了主流。但是，随着分布式存储（大数据）和`SSD`技术的发展，Out-of-place update模式的优势逐渐凸显。因此，`LSM树`的应用也越来越广泛，尤其是在现代NoSQL系统的存储引擎，包括BigTable、Dynamo、HBase、Cassandra、LevelDB、RocksDB和AsterixDB。

## 存储结构的共性特点

因为存储引擎是要负责将数据持久化到磁盘的，所以需要考虑磁盘I/O和内存I/O之间的巨大性能差距（尽量减少磁盘I/O的次数）。所以，存储结构需要具备一些必要的特性：

1. 适合磁盘存储（粒度尽可能大），IO尽量少且一次读取连续的区域。
2. 允许并发操作（粒度尽可能小），增删改操作对存储结构的影响尽量小。注意，事务并发不等于存储引擎并发操作。
3. 最好支持范围查询。

因此，常见的大多数内存数据结构并不适合做存储结构，比如：数组、链表、哈希表、跳跃表（[MySQL的索引为什么使用B+树而不使用跳表？](https://zhuanlan.zhihu.com/p/499607419)）、二叉搜索树、平衡二叉树、红黑树。适合做存储结构的数据结构主要就是`B树`、`B+树`及其衍生版本和`LSM树`。

`B树`和`B+树`都是多叉树，具有低高度和高扇出的特点，也就是“矮胖型”树，在做索引查询时，可以最大程度减少磁盘IO次数。但是，增删改操作可能造成树节点的分裂和合并，也就是`SMO(Structure Modification Operation)`操作，这会使得增删改操作的修改范围变大（写放大），并且给并发操作带来更多的约束。

## 索引分类

索引类型的基本分类（其中B+树索引和B树索引实际上也属于顺序索引）：

- 顺序索引--按索引值的顺序排序
- B+树索引
- B树索引
- 散列索引--将索引值均匀分布到多个散列通里
  - 静态散列索引
  - 动态散列索引

顺序索引实际上包含两层意思：

1. 索引有序：将搜索码的值按顺序存储，并将搜索码值与包含该搜索码值的记录关联起来。
2. 记录有序：数据库文件中的记录也按照搜索码的顺序存储。

`聚集索引（主索引）`：一个数据库文件可以有多个索引，分别基于不同的搜索码。但是文件中的记录只能按照其中一个搜索码的顺序来排序，而这个搜索码对应的索引就是聚集索引。聚集索引的搜索码一般是主码。

`非聚集索引（辅助索引）`：很显然，搜索码值顺序不代表文件记录的索引就是非聚集索引。

本文主要讨论B+树和B树这类顺序索引。关于散列索引可以参考 [数据库系统笔记 | 索引](https://blog.csdn.net/u012299594/article/details/86559671?spm=1001.2014.3001.5502)。

# B树及其变种

## B树和B+树



`B+`树是B树的进化版本，目前在工业界主流应用的是B+树，所以我们先介绍B+树。下图是[数据库系统概念官网](https://db-book.com/)给出的关于B+树的案例，这颗B+树是以表的第二列的名字数据作为搜索码来构建的：

![image-20240411200834073](https://hunk-pic-store.oss-cn-beijing.aliyuncs.com/img/20240411200834.png)

其主要特点如下：

1. B+树也是平衡树：从根节点到任意叶子节点的路径长度是一样的。
2. 叶节点：叶节点的指针指向具有搜索码值（例如上图中的 Brandt）的一条文件记录；尾部的指针指向下一个叶节点；各叶节点中的搜索码值的范围互不重合，除非有重复的搜索码值，在这种情况下，一个值可能出现在多个叶节点中。
3. 内部节点：B+树的非叶结点（也称内部结点）形成叶结点上的多级稀疏索引，内部节点上的连续两个搜索码值代表了一个范围（例如 Einstein -> Gold），当查询的搜索码刚好落在这个范围里，只需要继续查询 Einstein 后面的指针指向的下级节点即可，否则就继续 Einstein 前面或者 Gold 后面的指针指向的下级节点；非叶结点的结构和叶结点结构相同，只是其中的指针都是指向树中的结点。

一般地，**一个节点占用一个文件I/O块**（`4KiB`），如果搜索码值和指针占用20个字节，则n大约为200。假设文件中有100 0000条记录，一次查找也只需要访问3个结点（100^3>=100 0000）。通常根结点常驻在主存中，所以只需要读取最多2个磁盘块。相比前面的顺序文件索引，B+树的查询效率确实高了很多，而且这种性能足够稳定，以前面的假设考虑，只有记录的数量扩大100倍后磁盘块访问数才会增加一次。

TODO: 节点里存储的指针到底是什么？



下图是[数据库系统概念官网](https://db-book.com/)给出的关于B树的案例，以及B树和B+树的对比：

![image-20240411205025628](https://hunk-pic-store.oss-cn-beijing.aliyuncs.com/img/20240411205025.png)

B树和B+树的区别在于：

1. B树的内部节点和叶子节点上存储的搜索码值不会重复，内部节点上的搜索码值的左边直接存储指向对应的文件记录的指针。而B+树只会在叶子节点存储指向文件记录的指针

2. 相比B+树，B树的内部结点多了一个指针域，能够存储的项也更少。**对于相同数目的记录，B树的高度明显高于B+树，而且这一点也几乎抵消了搜索码值唯一性带来的空间利用率的提升**。所以，对于部分值的查找B树快于B+树，部分查找慢于B+树。

3. B树的删除操作相比B+树也更复杂。

> 这里描述的只是B+树和B树的原始结构，在实际存储引擎实现中，会有一些细节改进，具体参考后面对 MySQL InnoDB的介绍。



更多关于 B树 和 B+树 参考资料：

[数据库系统笔记 | 索引](https://blog.csdn.net/u012299594/article/details/86559671?spm=1001.2014.3001.5502)

[B+树看这一篇就够了（B+树查找、插入、删除全上）](https://zhuanlan.zhihu.com/p/149287061)

[Chapter 14: Indexing](https://db-book.com/slides-dir/PDF-dir/ch14.pdf)

[Chapter 18 : Concurrency Control](https://db-book.com/slides-dir/PDF-dir/ch18.pdf)

[B-Tress Visualizations](https://www.cs.usfca.edu/~galles/visualization/BTree.html)

## B*树

## B-Link树

## COW B树

## 惰性B树

## Bw树

`Bw树`更像是`B-Link树`和`LSM树`的结合，从而兼具无Latch的特性，又在相当程度上具有B树一族的读优势。目前Bw树在学术界有不少的探索和关注。在工业界暂无成功的实践。

# LSM树

## LSM树应用

`LSM树`的应用越来越广泛，尤其是在现代NoSQL系统的存储引擎，包括BigTable、DynamoDB、HBase、Cassandra、LevelDB、RocksDB和AsterixDB、influxdb、SQLite、Oceanbase、TiDB（based on RocksDB）。

## LSM树参考资料

[论文翻译：LSM-based storage techniques: a survey （上）](https://zhuanlan.zhihu.com/p/400293980)

[论文翻译：LSM-based storage techniques: a survey （下）](https://zhuanlan.zhihu.com/p/403396976)

[LSM-based Storage Techniques: A Survey](https://arxiv.org/pdf/1812.07527.pdf)

[MINI-LSM](https://skyzh.github.io/mini-lsm/)

# MySQL InnoDB（B+树）

## InnoDB 索引

`InnoDB`存储引擎的存储单元是`16KiB`大小的页（默认设置），而不是文件系统的I/O页大小`4KiB`。我们也可以通过设置 `innodb_page_size` 来调整页的大小，支持 `64KiB`、 `32KiB`、`16KiB` 、 `8KiB` 和 `4KiB`。

InnoDB 的索引类型分为 `主键索引`和`非主键索引`：

1. `主键索引`的叶子节点存的是整行数据。在 InnoDB 里，主键索引也被称为`聚簇索引（clustered index）`。因此，整张表的数据其实就是存储在聚簇索引中的，聚簇索引就是表。

   如果没有设置主键怎么办呢？MySQL会自动选择一个可以唯一标识数据记录的列作为主键，如果不存在这种列，则 MySQL 自动为InnoDB表生成一个隐含字段作为主键。

2. `非主键索引`的叶子节点内容是主键的值。在 InnoDB 里，非主键索引也被称为二级索引（secondary index）。二级索引的叶子节点中存的是主键的值，不是原始的数据（否则就太浪费存储空间了），所以二级索引找到主键的值之后，需要用该主键再去主键索引上查找一次，才能获取到最终的数据，这个过程叫做 `回表`，这也是“二级”的含义。

下面这张图展示了主键索引（主索引树）和非主键索引（辅助索引树）之间的关系：

![innodb index1](https://hunk-pic-store.oss-cn-beijing.aliyuncs.com/img/20240412103744.png)

1. 在执行 `select * from user_info where id = 5` 这个语句时，InnoDB 就会直接查询主索引树，然后从相应的节点上读取用户的信息。

2. 在执行 `select * from user_info where "user_name" = "Bob"`这个语句时，InnoDB 就会先查询右边的辅助索引树，得到 Bob 这个索引记录里的主键值，即15，然后再从主索引树中查询并读取 id 等于 15的用户信息。

### 创建 InnoDB 索引的建议

由于二级索引中保存了主键值，所以**索引主键值的 size 越小越好（这样单个B+树节点可以存储的索引条目越多），以避免二级索引占用的空间过大，一般建议使用 `int` 的自增列作为主键**。这样可以保证数据行是按顺序写入的，对于根据主键做关联操作的性能也会更好。因为主键是顺序增长的，所以每一条记录都保存在上一条记录的后面，当前的页写满的时候，下一条记录就写在新的页中。在读取数据的时候，就可以按顺序读取，充分利用了局部性的优势，大大提高了读取效率。

建议避免使用UUID作为聚簇索引，它使得聚簇索引的插入变得完全随机，使得数据没有任何聚集特性。因为UUID主键是随机生成的，新的主键不一定比上一个主键大，所以无法每次都把新的主键插入到最后面，需要为新的主键寻找合适的位置，通常在已有数据的中间位置。在页中间插入数据需要重新分配空间，以及移动旁边的数据，这样会导致频繁的页分裂操作同时会产生碎片。

### 并发控制(Lock和Latch)

## InnoDB 参考资料

[Chapter 17 The InnoDB Storage Engine](https://dev.mysql.com/doc/refman/8.0/en/innodb-storage-engine.html)

[MySQL InnoDB索引的存储结构](https://zhuanlan.zhihu.com/p/110178282)

[MySQL索引：图文并茂，深入探究索引的原理和使用](https://zhuanlan.zhihu.com/p/335734145)

[InnoDB 中B+Tree索引树有多高，以及能存多少行数据？](https://mp.weixin.qq.com/s/QmlVJJhHLVzjM7XMCOGPmg)  

# MongoDB WiredTiger（B+树）

# LevelDB（LSM引擎）

Google的两位专家基于`Bigtable`的经验实现了`LevelDB`，一个单机`LSM树`存储引擎，并开源。LevelDB是一个写性能十分优秀的存储引擎，是典型的LSM树(Log Structured-Merge Tree)实现。LSM树的核心思想就是放弃部分读的性能，通过尽量减少随机写的次数，换取最大的写入能力。对于每次写入操作，并不是直接将最新的数据驻留在磁盘中，而是将其拆分成（1）一次日志文件的顺序写（2）一次内存中的数据插入。

## 基本架构

<img src="https://leveldb-handbook.readthedocs.io/zh/latest/_images/leveldb_arch.jpeg" style="zoom:80%" />

LevelDB中主要由以下几个核心组件构成：

1. MemTable
2. Immutable MemTable
3. Log（WAL，Write Ahead Log）
4. SSTable（Sorted String Table）
5. MANIFEST
6. CURRENT

### MemTable

LevelDB 的一次写入操作并不是直接将数据刷新到磁盘文件，而是首先写入到内存中的 `MemTable`。在 `MemTable` 中，所有的数据按**用户定义的排序方法**排序之后按序存储，等到其存储内容的容量达到阈值时（默认为`4MiB`），便将其冻结转换成一个**不可修改**的 MemTable（即 `Immutable MemTable`），与此同时创建一个新的 `MemTable`，供用户继续进行读写操作。

> 内存中的 `MemTable` 使用了[跳表](https://zh.wikipedia.org/wiki/跳跃列表)数据结构，这种数据结构效率可以比拟二叉查找树，绝大多数操作的时间复杂度为O(log n)。

 `Immutable MemTable`和 `MemTable`的结构定义完全一样，唯一区别是  `Immutable MemTable` 是只读的。当一个  `Immutable MemTable` 被创建时，LevelDB的后台 `Compaction` 进程便会利用其中其中的内容，创建一个 `SSTable`持久化到磁盘文件中，这个过程叫做`Minor Compaction`。设计Minor Compaction的目的是为了：

1. 有效地降低内存地使用率；
2. 避免 WAL 日志文件过大，系统恢复实践过长。

### WAL日志

为了对数据做持久化，LevelDB 在写内存之前会首先将所有的写操作写到日志文件中，这种日志文件叫做 `WAL`。当以下异常情况发生时，均可以通过日志文件进行恢复：

1. 写日志期间进程异常 -  数据库重启读取log时，发现异常日志数据，抛弃该条日志数据，即视作这次用户写入失败，保障了数据库的一致性。
2. 写日志完成，写内存未完成 - 通过重做日志文件中记录的写入操作完成数据库的恢复。
3. Write 动作完成（即 log、内存写入都完成）后，进程异常 - 通过重做日志文件中记录的写入操作完成数据库的恢复。
4. Immutable MemTable 持久化过程中进程异常 - 通过 redo 日志文件中记录的写入操作完成数据库的恢复。
5. 其他压缩异常（较为复杂，首先不在这里介绍）；

每次日志的写操作都是一次顺序写，因此写效率高，整体写入性能较好。同时，LevelDB 写操作的原子性也是由`WAL`来保证的。

### SSTable

内存不是无限的，不可能将所有数据都存放在 `MemTable`中，日志文件也主要是为了顺序写和保证异常场景下的数据一致性。最终，大部分数据都需要持久化到磁盘上。LevelDB 使用 SSTable（Sorted String Table）来存储持久化数据，意思是数据按照键的顺序存储在磁盘上。既然键是有序的，那么查找键可以采用二分搜索，但是磁盘的二分搜索会读取多个磁盘块。这时候只需要给每个磁盘块一个索引，告诉这个磁盘块里面存储键的范围，那么在查找时，可以先通过对索引进行二分搜索找到键所在的磁盘块，只需要读取这个磁盘块，便可以找到这个键。索引是一个稀疏索引，比较小，可以放在内存中缓存。

当数据被成功写入SSTable后，日志里的相应数据就可以被淘汰了，保证异常情况下的恢复性能。但是，`MemTable`里只维护部分数据就导致读操作必须得去查询SSTable，随着SSTable不断地写入，SSTable会越来越多，当查找一个键时，可能需要读取多个SSTable，这就涉及了多次随机读，读取效率会很低。

SSTable 是分层存储的，即每个SSTable都属于不同的 Level，比如 Level 0，Level 1等等，**Level 0 的SSTable之间是存在键值重叠的，而 Level 0以上的SSTable是有序不重叠的。Compaction实际上就是对输入的多个SSTable进行多路归并的过程。**随着数据的增长，Compaction 过程不断将数据推向更高 level 的SSTable。随着 Level 的增大，每一层的文件总大小会以10倍增大，这样不但可以有大量的存储空间，而且每一次 Compaction 涉及的SSTable的数量都是可控的。

> SSTable 文件本身是不可修改的，即 Compaction 过程中只会创建新的 SSTable，这依然在遵循 Out-of-place update的思想。

### MANIFEST & CURRENT

随着 Level的增多，读取更复杂了。要先读取 MemTable，再读取 Level 0的文件（Level 0可能有多个文件的键范围包括这个查找键），还需要读取 Level 0 以上的文件，每一Level最多有一个文件的键范围包括查找键。不过Level的数量有限，Level 0的文件数量也有限，所以需要读取的SSTable的数量依然是常数级，配合缓存、布隆过滤器等优化技术，可以提高读的性能。这是在读取性能和后台操作性能之间的折中，为了让写操作成为顺序写，而做的牺牲。

磁盘上有多个SSTable，每个SSTable属于哪一层以及文件大小，每一个 SSTable 的键范围，把这些元数据信息都缓存在内存中可以有效改善查询效率。但是如果数据库重启了，就丢失了这些元信息，所以需要将它们持久化到磁盘上的`MANIFEST`文件中。

随着Compaction的进行， 元数据会改变，所以每次还需要将改变的元数据写到 MANIFEST 文件中。恢复元数据时，使用初始的元数据和各个改变恢复出最终的元数据。但是如果改变太多，MANIFEST太大，恢复就会太耗时，这时可以将当前的元数据写入到有一个新的 MANIFEST 中，而舍弃旧的MANIFEST。而 CURRENT 文件则存储了当前使用的 MANIFEST 文件是哪一个，写完 MANIFEST 后， 需要将 CURRENT 指向新的 MANIFEST。

## 基本读写流程

### 基本写操作(Put & Batch）

LevelDB整个架构设计就是为提高写性能而生的，它的大致读写流程如下图所示：

<img src="https://leveldb-handbook.readthedocs.io/zh/latest/_images/write_op.jpeg" style="zoom:40%" />

LevelDB的写操作分为两个部分：

1. Append 模式写日志 - 保证用户写入的数据不丢失。

2. 写内存里的 MemTable。

> 如果写模式设置为非同步，在系统宕机发生时，LevelDB仍然存在写入丢失的隐患（Page Cache里的脏页还未落盘）。

LevelDB 支持两种基本写操作：（1）`Put`；（2）`Delete`。这两种本质对应同一种操作，Delete操作同样会被转换成一个value为空的Put操作。此外，LevelDB 还提供了一个批量处理的工具 `Batch`，用户可以依据 `Batch`来完成批量的数据库更新操作，且这些操作是`原子性`的。无论是Put/Delete操作，还是批量操作，底层都会为这些操作创建一个 `batch` 实例作为一个数据库操作的最小执行单元。

`batch` 的格式如下：

<img src="https://leveldb-handbook.readthedocs.io/zh/latest/_images/batch.jpeg" style="zoom:40%" />

在 `batch` 中，每一条数据项都按照上图格式进行编码。每条数据项编码后的第一位是这条数据项的类型（更新还是删除），之后是数据项key的长度，数据项key的内容；若该数据项不是删除操作，则再加上value的长度，value的内容。

`batch` 中会维护一个size值，用于表示其中包含的数据量的大小。该size值为所有数据项key与value长度的累加，以及每条数据项额外的8个字节，这8个字节用于存储一条数据项额外的一些信息。

#### key值编码 - internalKey

当数据项从 `batch` 中写入到内存数据库中时，需要将用户提供的 key 值进行转换，即在 leveldb 内部，所有数据项的 key 是经过特殊编码的，这种格式称为 `internalKey`，其格式如下：

<img src="https://leveldb-handbook.readthedocs.io/zh/latest/_images/internalkey.jpeg" style="zoom:40%" />

`internalkey`在用户key的基础上，尾部追加了8个字节，用于存储（1）该操作对应的 `sequence number`（2）该操作的类型。

其中，每一个操作都会被赋予一个`sequence number` 来标识这些数据项的新旧情况。该计数器是在 LevelDB 内部维护，每进行一次操作就做一个累加。因为更新和删除操作都采用的是追加写模式，所以同样一个 key，会有多个版本的数据记录，而最大的`sequence number`对应的数据记录就是最新的。

`MemTable` 中的所有数据项都是按照键值比较规则（支持自定义）进行排序的，系统默认的比较规则如下：

- 首先按照字典序比较用户定义的 key（即上图中的 uKey），若 uKey 值大，整个 internalKey 就大；
- 若 uKey 相同，则序列号大的 internalKey 值就小；

通过这样的比较规则，则所有的数据项首先按照 uKey 进行升序排列；当 uKey 相同时，按照序列号进行降序排列，这样可以保证首先读到序列号大（最新）的数据项。

此外，LevelDB 的快照（snapshot）也是基于这个`sequence number`实现的，即每一个`sequence number`代表着数据库的一个版本。

#### 合并写

LevelDB 针对并发写入做了一个优化处理，即同一时刻 ，只允许一个写入操作将内容写入到日志文件以及内存数据库中。为了在写入进程较多的情况下，减少日志文件的*小写入*，增加整体的写入性能，leveldb 将一些*小写入*合并成一个*大写入*，称之为**合并写**。

合并写的流程如下（左边为先拿到锁的写操作，即进程/线程）：

<img src="https://leveldb-handbook.readthedocs.io/zh/latest/_images/write_merge.jpeg" style="zoom:40%" />

#### 原子性

在前面介绍 WAL log 部分已经提到过，LevelDB 的任意写操作的原子性都是由日志文件实现的。一个写操作中所有的内容会以一个日志中的一条记录，作为最小单位写入。考虑两种典型异常场景：

1. 写日志未开始，或写日志完成一半，进程异常退出；- 可能存储一个写操作的部分写已经被记载到日志文件中，仍然有部分写未被记录，这种情况下，当数据库重新启动恢复时，读到这条日志记录时，发现数据异常，直接丢弃或退出，实现了写入的原子性保障。
2. 写日志完成，进程异常退出；- 写日志已经完成，写入日志的数据未真正持久化，数据库启动恢复时通过redo日志实现数据写入，仍然保障了原子性。

### 快照

快照代表着数据库某一个时刻的状态，在 LevelDB 中，作者巧妙地用一个整型数来代表一个数据库状态。

在 LevelDB 中，用户对同一个key的若干次修改（包括删除）是以维护多条数据项的方式进行存储的（直至进行 Compaction 时才会合并成同一条记录），每条数据项都会被赋予一个序列号（即 `internalKey` 里的 `sequence number`），代表这条数据项的新旧状态。一条数据项的序列号越大，表示其中代表的内容为最新值。

```c++
// Snapshots are kept in a doubly-linked list in the DB.
// Each SnapshotImpl corresponds to a particular sequence number.
class SnapshotImpl : public Snapshot {
 public:
  SnapshotImpl(SequenceNumber sequence_number)
      : sequence_number_(sequence_number) {}

  SequenceNumber sequence_number() const { return sequence_number_; }

 private:
  friend class SnapshotList;
//...
  const SequenceNumber sequence_number_;
//,,,
};
```

**因此，每一个序列号，其实就代表着leveldb的一个状态**。换句话说，每一个序列号都可以作为一个状态快照。

当用户主动或者被动地创建一个快照时，LevelDB 会以当前最新的序列号对快照进行赋值。例如，图中用户在序列号为 98 的时刻创建了一个快照，并且基于该快照读取key为“name”的数据时，即便此刻用户已经将 "name" 的值修改为 "dog" ，再删除，用户读取到的内容仍然是“cat”。

<img src="https://leveldb-handbook.readthedocs.io/zh/latest/_images/snapshot.jpeg" style="zoom:40%" />

所以，利用快照能够保证数据库进行并发的读写操作。

### 读操作

LevelDB 提供的读操作接口是 `Get`，但是有两种方式调用 `Get`接口：

1. 直接调用 `Get` 接口；

   ```c++
   leveldb::Status s = db->Get(leveldb::ReadOptions(), key1, &value);
   ```

2. 基于快照调用 `Get` 接口。

   ```c++
   leveldb::ReadOptions options;
   // options.snapshot = it->second.get();
   options.snapshot = it->second;
   
   leveldb::Status status = db->Get(options, key, &value);
   ```

这两种方式本质是一样的，直接调用 `Get` 时默认地以当前数据库的状态创建了一个快照，并基于此快照进行读取。

**对于每个读操作，在获取到一个快照之后，LevelDB 会为本次查询的 key 构建一个 `internalKey`（格式如上文所述），其中 `internalKey` 的 Seq 字段使用的便是快照对应的 Seq。通过这种方式可以过滤掉所有 Seq 大于快照号的数据项，Seq 最新的那个相同 key 就是读取结果**。

`Get` 接口的源代码实现如下：

```c++
Status DBImpl::Get(const ReadOptions& options, const Slice& key,
                   std::string* value) {
  Status s;
  MutexLock l(&mutex_);
  SequenceNumber snapshot;
  if (options.snapshot != nullptr) {
    snapshot =
        static_cast<const SnapshotImpl*>(options.snapshot)->sequence_number();
  } else {
    snapshot = versions_->LastSequence();
  }
//...
}
```

LevelDB 的读操作流程如下图所示：

<img src="https://leveldb-handbook.readthedocs.io/zh/latest/_images/readop.jpeg" style="zoom:45%" />

> leveldb 在每一层  SSTable  中查找数据时，都是按序依次查找  SSTable  的。
>
> 0层的文件比较特殊。由于0层的文件中可能存在key重合的情况，因此在0层中，文件编号大的 SSTable 优先查找。理由是文件编号较大的 SSTable 中存储的总是最新的数据。
>
> 非0层文件，一层中所有文件之间的key不重合，因此leveldb可以借助 SSTable 的元数据（一个文件中最小与最大的key值）进行快速定位，每一层只需要查找一个 SSTable 文件的内容。

## WAL日志 - 崩溃恢复

为了处理进程异常、系统掉电等异常场景，LevelDB 的写操作在更新内存中的 MemTable 之前会先将写操作的内容写入 WAL 日志中。

<img src="https://hunk-pic-store.oss-cn-beijing.aliyuncs.com/img/20240428153808.jpeg" style="zoom:40%" />

如上图所示，当 `MemTable` 里的数据达到一定大小（默认4 MiB）后，当前 `MemTable` 会被转换成只读的 `Immutable MemTable`，同时当前 WAL 日志文件也会转换成一份 `Frozen Log`。 `Immutable MemTable`由后台的 Minor Compaction 进程将其转换成一个 `SSTable`  进行持久化，持久化完成后，删除与之对应的  `Frozen Log` 。

### 日志结构

WAL 日志文件按照 `block` （32 KiB）进行划分，每个 `block` 包含若干个完整的 `chunk`。

<img src="https://leveldb-handbook.readthedocs.io/zh/latest/_images/journal.jpeg" style="zoom:40%" />

如上图所示，一条日志记录由一个或者多个 `chunk`组成，每个 `chunk` 包含一个7字节大小的 Header：

1. Checksum：前4字节是该 `chunk` 的校验码 `Checksum`，其校验的范围包括 chunk 的类型以及随后的data数据。

2. Length：紧接的2字节是该 `chunk` 数据的长度；

3. Full/Middle/Last：最后一个字节是该 `chunk`的类型。chunk 共有四种类型：

   ```c++
   FULL == 1 //若一条日志记录只包含一个chunk，则类型为 FULL
   FIRST == 2 //若一条日志记录包含多个chunk，则这些chunk的第一个类型为first,
   MIDDLE == 3 //中间包含大于等于0个middle类型的chunk
   LAST == 4 // 最后一个类型为last
   ```

   可以看出，chunk 的设计主要是为了跨 `block` 存储日志记录的问题。因为 `block` 的为固定的 32 KiB，在 `block` 临近尾部位置开始写入的日志记录就必须被拆分成多个 `chunk`，然后分别写入不同的 `block` 中，`chunk` 的这几种类型就代表了这些 `chunk` 之间的链接关系。

   *虽然上图里每个 `Data` 段都是一样长的，实际上每个 `chunk` 里存的数据长度可能是不同的。*

### 日志内容

日志的内容（即上图中的 Data 段）为写入的 `batch` 编码后的信息，具体格式如下：

<img src="https://leveldb-handbook.readthedocs.io/zh/latest/_images/journal_content.jpeg" style="zoom:50%" />

一条日志记录的内容包含 Header（注意区分上面的 chunk header） 和 Data：其中 Header 中有（1）当前db的 `sequence number`（2）本次日志记录中所包含的 Put/Del 操作的个数 `entry number`；紧接着写入所有 `batch` 编码后的内容。

### 日志读写

LevelDB 内部实现了一个日志 writer，写入时先调用 Next 函数获取一个 singleWriter 用于写入一条日志记录。singleWriter 开始写入时，标志着第一个 chunk 开始写入，在写入过程种，不断判断 writer 中 block buffer 的大小，如果超过 32 KiB，则标志着 chunk 的结束， 为其计算 header 后将整个 chunk 写入文件。同时，重置 buffer，开始新的 chunk 的写入。

为了避免频繁的IO读取，每次从 WAL 文件中读取数据时，以 32 KiB 的`block` 为单位进行块读取，可以很好的利用内核中的缓存优化机制。每次读取一条日志记录，reader 调用 Next 函数返回一个 singleReader，singleReader 每次调用 Read 函数就返回一个 chunk 的数据。每次读取一个 chunk，都会检查其校验码、数据类型、数据长度等信息是否正确，若不正确，且用户要求严格的正确性，则返回错误，否则丢弃整个chunk的数据。当读取到一个类型为 Full 或者 Last 的 chunk，就代表读取到了一条完整的日志记录。

## SSTable - 数据落盘

前面提到，当 LevelDB 达到 checkpoint 点（MemTable中的数据量超过了预设的阈值），会将当前的 MemTable 冻结成一个只读的 Immutable MemTable，并且创建一个新的 MemTable 供系统继续使用。Immutable MemTable 会在后台进行一次 minor compaction，即将内存表中的数据持久化到磁盘文件中，即 SSTable。

设计 minor compaction 的主要目的是：（1）有效降低内存的使用率；（2）避免日志文件过大，导致系统恢复时间过长。

### SSTable 文件格式

SSTable 文件的逻辑结构如下图所示：

![LevelDB_Diagram.drawio](https://hunk-pic-store.oss-cn-beijing.aliyuncs.com/img/20240505212914.png)



### SSTable 写流程

SSTable 的写操作主要发生在两个场景：（1）minor compaction；（2）major compaction。

### SSTable 读流程

下图为在一个 SSTable 中查找某个数据项的流程图：

<img src="https://hunk-pic-store.oss-cn-beijing.aliyuncs.com/img/20240505214242.jpeg" style="zoom:50%" />

大致流程如下：

1. 如果 cache 中没有指定 SSTable 文件的句柄，则打开该文件，按规则读取该文件的元数据，将新打开的句柄存储至 cache 中。

   > LevelDB cache 用来缓存两类数据：（1）SSTable 文件句柄及其元数据；（2）Data Block 中的数据。 

   读取SSTable元数据的流程如下：

   <img src="https://hunk-pic-store.oss-cn-beijing.aliyuncs.com/img/20240505215615.jpeg" style="zoom:40%" />

2. 利用 SSTable 中的 `Index Block` 记录的 `Data Block n` 的 `Max Key` 对数据项进行快速定位，得到该数据项可能存在的两个 Data Block。

   > 和 Data Block一样， Index Block 中的索引信息同样也进行了 key 值截取，即第二个索引信息的 key 并不存储完整的 key，而是存储与前一个索引信息的 key 不共享的部分，区别在于 Data Block 中 `restart_interval` 的默认值为16，而 Index Block 中为 2，即Index Block 连续两条索引信息会被作为一个最小的“比较单元“。
   >
   > 如下图所示，在查找的过程中，若第一个索引信息的key小于目标数据项的key，则紧接着会比较第三条索引信息的key。这就导致最终目标数据项的范围区间为某”两个“Data Block。
   >
   > <img src="https://hunk-pic-store.oss-cn-beijing.aliyuncs.com/img/20240505220452.jpeg" style="zoom:40%" />

3. 利用 Index Block 中的索引信息，首先打开第一个可能的 Data Block。

4. 如果已经创建了布隆过滤器，则利用 filter Data Block 中的过滤器信息判断指定的数据项是否存在，如果存在，则创建一个迭代器对 Data Block 中的数据进行迭代遍历，寻找数据项；如果不存在，则结束第一个 Data Block的查找；

   > 在 Data Block 中查找目标数据项是一个简单的迭代遍历过程，虽然 Data Block 中所有数据项都是按序排序的，但是作者并没有采用“二分查找”来提高查找的效率，而是使用了更大的查找单元进行快速定位。
   >
   > 如下图所示，与 Index Block 的查找类似，Data Block中，以16条记录为一个查找单元，若entry 1的key小于目标数据项的key，则下一条比较的是entry 17。因此查找的过程中，利用更大的查找单元快速定位目标数据项可能存在于哪个区间内，之后依次比较判断其是否存在与data block中。
   >
   > <img src="https://hunk-pic-store.oss-cn-beijing.aliyuncs.com/img/20240505220904.jpeg" style="zoom:40%" />

5. 若在第一个 Data Block 中找到了目标数据，则返回结果；若未查找成功，则打开第二个 Data Block，重复步骤4；

6. 若在第二个 Data Block 中找到了目标数据，则返回结果；若未查找成功，则返回`Not Found`错误信息；

### Bloom Filter

鉴于 `Bloom Filter` 提供的结果具有以下特点：

1. “真阴性”：判断元素不存在的结论是可靠的。
2. “假阳性”：判断元素存在的结论是不可靠的。

LevelDB 中主要是利用 `Bloom Filter` 来快速判断指定的 key 值是否存在于一个 SSTable 文件中。若过滤器表示 key 存在，需要进一步遍历 SSTable 中的 Data Block 里的数据来确认；若过滤器表示 key 不存在，则该 key 一定不存在，直接结束对当前 SSTable 的查找，由此加快了查找效率。

### SSTable 文件特点

1. *只读性*：SSTable 文件为compaction 原子性产生的结果，在其余时间是只读的。
2. *完整性*：SSTable 文件的辅助数据（索引数据和过滤数据）都直接存储在同一个文件中；当需要使用这些辅助数据时，无须额外的磁盘读取；当 SSTable 文件需要删除时，无须额外的数据删除。简要地说，辅助数据随着文件一起创建和销毁。
3. *并发访问友好性*：由于 SSTable 文件具有只读性，因此不存在同一个文件的读写冲突。leveldb 采用引用计数维护每个文件的引用情况，当一个文件的计数值大于0时，对此文件的删除动作会等到该文件被释放时才进行，因此实现了无锁情况下的并发访问。
4. *Cache一致性*： SSTable  文件为只读的，因此 cache 中的数据永远和 SSTable 文件中的数据保持一致。



## Cache - 访问加速

LevelDB 实现了 Cache 来缓存数据，其中：

1. `cache`：缓存已经被打开的 SSTable 文件句柄及其元数据（默认上限为500个）。
2. `bcache`：缓存被读过的 SSTable 中的 Data Block的数据（默认上限为8 MiB）。

Cache 的设计使得发生读取热数据时，尽量在 cache 中命中，避免I/O读取，这在一定程度上优化了 LevelDB 的读性能。

当需要读取一个 SSTable 文件时，首先从 `cache` 中寻找是否已经存在相关的文件句柄，若存在则无需重复打开。否则，打开该文件，并将 SSTable 中的 Index Block 数据和 Meta Index Block 数据进行预读。

### Cache 结构

LevelDB 的 Cache 是一种 LRU Cache，其结构由两部分组成：

1. Hash Table：用来存储缓存数据。基于 Yujie Liu 等人的论文《Dynamic-Sized Nonblocking Hash Table》实现。
2. LRU（Last Recently Used）：用来维护数据项的新旧信息。

<img src="https://hunk-pic-store.oss-cn-beijing.aliyuncs.com/img/20240506093743.jpeg" style="zoom:40%" />

### Dynamic-sized NonBlocking Hash Table

在对 Hash Table 进行 resize 的过程中，保持 Lock-Free是一件非常困难的事情。一个 Hash Table 通常由若干个 bucket 组成，每一个bucket中会存储若干条被散列至此的数据项。当对 Hash Table 进行resize时，需要将“旧”桶中的数据读出，并且重新散列至另外一个“新”桶中。假设这个过程不是一个原子操作，那么会导致此刻其他的读、写请求的结果发生异常，甚至导致数据丢失的情况发生。

`Dynamic-sized NonBlocking Hash Table` 的设计思想是： Hash Table 中单个 bucket 的数据是可以冻结的。这个特点极大的简化了 Hash Table 在 resize 过程中在不同 bucket 之间转移数据的复杂度。

那 LevelDB 什么时候会对 cache 中的 Hash Table 进行 resize 呢？主要有以下3种情况：

1.  cache 中数据项（node）的个数超过预定的阈值（默认初始化状态下bucket 的个数为16，每个 bucket 中可存储 32 个数据项，即总量的阈值为：bucket 数量乘以 bucket 可以存储的数据项个数），需要对 Hash Table 进行扩张。
2. cache 中出现了数据项不平衡的情况。当某些桶的数据量超过了32个数据，即被视作数据发生散列不平衡。当这种不平衡累积值超过预定的阈值（128）个时，需要对 Hash Table 进行扩张；
3. 当 Hash table 中数据项的个数少于 bucket 的个数时，需要进行收缩。

`Dynamic-sized NonBlocking Hash Table` 的扩张过程如下图所示：

<img src="https://hunk-pic-store.oss-cn-beijing.aliyuncs.com/img/20240506094439.jpeg" style="zoom:30%" />

1. 计算新的 Hash Table 的 bucket 的个数（扩大一倍）。
2. 创建一个新的 Hash Table，并将旧的 Hash table 转换为一个 *过渡期* 的Hash table，其中每个 bucket 都被冻结；
3. 后台利用 *过渡期* Hash Table中的被冻结的 bucket 信息来构建新的 Hash Table 中的 bucket。

*在完成新的 Hash table 构建的整个过程中，Hash Table 的读写操作仍然可以进行。Hash Table 扩张过程中，最小的锁粒度是 bucket 级别，而不是整个 Hash Table。*

当有新的读写请求发生时，若被散列之后数据落在的 bucket 仍然未构建完成，则“主动”进行构建，并将构建后的 bucket 填入新的 Hash Table中。后台进程构建到该 bucket 时，发现已经被构建了，则无需重复构建。

> 对 Hash Table 进行扩张时，构建一个新的 bucket 其实就是将一个旧的 bucket 中的数据拆分成两个新的 bucket。拆分时仅需要将数据项key的散列值对新的哈希桶个数取余即可。

因此如上图所示，Hash Table 扩张结束，bucket 的个数增加了一倍，于此同时仍然可以对外提供读写服务，仅仅需要 bucket 级别的封锁粒度就可以保证所有操作的一致性跟原子性。

`Dynamic-sized NonBlocking Hash Table` 的收缩过程和扩张过程类似，只是新的 Hash table 的 bucket 的数量变成原来的一半，即两个旧的 bucket 被合并成一个新的 bucket。

### LRU

LevelDB 中的 LRU 利用双向循环链表来实现，每一个链表项称之为 `LRUNode`。一个 `LRUNode`除了维护一些链表中前后节点信息以外，还存储了一个 Hash Table 中数据项的指针，通过该指针，当某个节点由于 LRU 策略被驱逐时，从 Hash Table 中“安全的”删除数据内容。

LRU 提供了以下几个接口：

1. `Promote`：
   1. 若 Hash table 中的节点是首次创建，则为其创建一个 `LRUNode`，并将该 `LRUNode` 置于链表的头部，表示为最新访问的热数据；如果因为新增一个 `LRUNode` 导致超出了容量上限，就需要根据策略清除部分 `LRUNode`。
   2. 若 Hash table 中的节点在链表中已经有相关的 `LRUNode`，并将该 `LRUNode` 置于链表的头部。
2. `Ban`：从 LRU 链表中删除对应的 `LRUNode`，并尝试删除 Hash Table 中对应的数据。由于该 Hash Table 节点的数据可能被其他线程正在使用，因此需要查看该数据的引用计数，只有当引用计数为0时，才可以真正地从 Hash Table 中进行删除。

## Compaction

## 版本控制



## LevelDB参考资料

[leveldb-handbook](https://leveldb-handbook.readthedocs.io/zh/latest/basic.html)

[LevelDB源码剖析](https://www.zhihu.com/column/c_1282795241104465920)

[透过leveldb感悟存储工程实践](https://zhuanlan.zhihu.com/p/516566364)

# RocksDB（LSM引擎）

Facebook基于LevelDB开发了`RocksDB`，`RocksDB`做了相当多的迭代演进，如多线程、column family，compaction策略等，目前`RocksDB`已经是LSM树领域的事实标准。









# References

[Database System Concepts](https://db-book.com/slides-dir/index.html)

B站视频 -戍米的论文笔记