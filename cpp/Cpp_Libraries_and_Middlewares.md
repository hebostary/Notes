# 一、内存池

## 为什么需要内存池？

## TCMalloc

`TCMalloc` (Thread-Caching Malloc）是 Google 开发的内存分配算法库，最初作为Google性能工具库 perftools 的一部分，提供高效的多线程内存管理实现，用于替代操作系统的内存分配相关的函数（malloc、free，new，new[]等），具有减少内存碎片、适用于多核、更好的并行性支持等特性。据称，它的内存分配速度是 glibc2.3 中实现的 malloc的数倍。

`TCMalloc` 在不少项目中都有使用，例如在 Golang 的内存分配算法。

### TCMalloc原理概述

如下图所示，TCMalloc 中每个线程都有独立的线程缓存 ThreadCache，线程的内存分配请求会向 ThreadCache 申请，ThreadCache内存不够用会向全局的 CentralCache 申请，CentralCache 里的内存不够用时会向 PageHeap 申请，PageHeap 不够用就会向OS操作系统申请。

![tcmalloc1](https://pic3.zhimg.com/80/v2-781e427be04352d232efd36e2398e76a_720w.webp)

下面介绍 TCMalloc guan管理内存的核心概念：Page、Span、ThreadCache、Sise Class、CentralCache、PageHeap。

1. Page：操作系统对内存管理的单位，TCMalloc 中以 `Page` 为单位管理内存，`Page` 默认大小为 8KB，通常为Linux系统中`Page`大小的倍数关系，如8、32、64，可以在编译选项配置时通过--with-tcmalloc-pagesize参数指定。默认Page大小通过减少内存碎片来最小化内存使用，使用更大的Page则会带来更多的内存碎片，占用更多的内存，但速度上会有所提升。

2. Span：`Span` 是 `PageHeap` 中管理内存`Page`的单位，由一个或多个连续的`Page`组成，比如2个`Page`组成的 `Span`，多个`Span`使用链表来管理，TCMalloc 以 `Span`为单位向操作系统申请内存。

   ![tcmalloc2](https://hunk-pic-store.oss-cn-beijing.aliyuncs.com/img/20240410200421.webp)

   如上图所示，第1个 Span 包含2个 Page，第2个和第4个 Span 包含3个 Page，第3个 Span 包含5个 Page。Span要么被拆分成多个相同 size-class 的小对象（Object）用于小对象分配，要么作为一个整体用于中对象或大对象分配。当作用作小对象分配时，Span的sizeclass成员变量记录了其对应的Size Class。Span中包含两个Span类型的指针（prev，next），用于将多个Span以链表的形式存储。

3. ThreadCache：`ThreadCache` 是每个线程独立拥有的Cache，包含多个空闲内存链表（size-classes），每一个链表（size-class）都有大小相同的object。线程可以从各自 ThreadCache 的 `FreeList` 获取对象，不需要加锁，所以速度很快。如果 ThreadCache 的FreeList 为空，需要从 CentralCache 中的 CentralFreeList 中获取若干个 Object 到 ThreadCache 对应的 size class列表中，然后再取出其中一个object返回。

4. Size Class：TCMalloc定义了很多个 Size Class，每个 Size Class都维护了一个可分配的 FreeList，FreeList 中的每一项称为一个Object，同一个 Size Class 的 FreeList 中每个 Object大小相同。在申请小内存时(小于 256K )，TCMalloc 会根据申请内存大小映射到某个size-class中。比如，申请0到8个字节的大小时，会被映射到size-class1中，分配8个字节大小；申请9到16字节大小时，会被映射到size-class2中，分配16个字节大小，以此类推。

   当申请的内存大于 256K 时，不再通过 ThreadCache 分配，而是通过 PageHeap 直接分配大内存。

   ![tcmalloc3](https://pic3.zhimg.com/80/v2-cdf0030a7c226a8a397f052b2357a1ea_720w.webp)

5. CentralCache：`CentralCache` 是`ThreadCache`的全局缓存，ThreadCache 内存不足时会向 CentralCache 申请。CentralCache 本质是一组 CentralFreeList，链表数量和 ThreadCache 数量相同。ThreadCache中内存过多时，可以放回CentralCache中。如果CentralFreeList 中的 Object不够，CentralFreeList 会向 PageHeap 申请一连串由 Page 组成的 Span，并将申请的 Span 切割成一系列的 Object 后，再将部分 Object 转移给 ThreadCache。

   CentralCache在TCMalloc中是一个逻辑上的概念，本质是CentralFreeList类型的数组。CentralCache简化结构如下：

   ![tcmalloc5](https://hunk-pic-store.oss-cn-beijing.aliyuncs.com/img/20240410203521.webp)

6. PageHeap：PageHeap 保存存储 Span 的若干链表，CentralCache 内存不足时，可以从 PageHeap 获取Span，然后把 Span 切割成 Object。PageHeap 申请内存时按照Page申请，但管理内存的基本单位是Span，Span代表若干连续Page。

   PageHeap 被所有线程共享(分配时需要全局锁定)，负责与操作系统的直接交互(申请及释放内存)，并且大尺寸的内存申请直接通过PageHeap进行分配。

   PageHeap内部根据内存块（Span）的大小采取了两种不同的缓存策略：128个Page以内的 Span，每个大小都用一个链表来缓存，超过128个Page的 Span，存储于一个有序 set（std::set）。

总结一下：PageHeap 从操作系统那里申请多个 Page，以 Span 的形式组织起来；PageHeap 又将 Span 按需分配给全局的 CentralCache，CentralCache 会将 Span 切割成一系列同等大小的 Object；CentralCache 又将这些 Object 分配给各个线程的 ThreadCache，并根据 Object 大小 通过 Size Class 组织起来，最终 ThreadCache 根据应用申请的内存大小从匹配的  Size Class 列表里分配 Object。即：**PageHeap（Span of n Pages）-> CentralCache（Object，CentralFreeList of Size Class）-> ThreadCache（Object，FreeList  of Size Class）-> Application（N bytes）**。如果是大于 265 K的内存申请需求，通过 PageHeap 直接分配。

![tcmalloc4](https://hunk-pic-store.oss-cn-beijing.aliyuncs.com/img/20240410203128.webp)





###  TCMalloc参考资料

[TCMalloc Overview](https://google.github.io/tcmalloc/overview.html)

[TCMalloc Quickstart](https://google.github.io/tcmalloc/quickstart.html)

[C++性能优化 —— TCMalloc的原理与使用](https://zhuanlan.zhihu.com/p/679559200)

[图解TCMalloc - 知乎](https://zhuanlan.zhihu.com/p/29216091)

## Jemalloc



参考资料：

[Jemalloc内存分配与优化实践](https://zhuanlan.zhihu.com/p/635155570)

[内存管理特性分析（十五）:内存分配器之jemalloc技术原理分析](https://zhuanlan.zhihu.com/p/642471269)

## mimalloc - microsoft

## nginx slab

参考：nginx/src/core/ngx_slab.c

# 二、线程池

# 三、数据库连接池

# 四、网络库

## brpc

## libevent

## workflow

