> 本文主要是Raft分布式共识协议论文（In Search of an Understandable Consensus Algorithm (Extended Version)）的阅读笔记。

# 介绍

## 复制状态机（Replicated state machines）

共识算法通常出现在复制状态机的上下文中。在这种方法中，一组服务器上的状态机计算相同状态的相同副本，并且即使某些服务器宕机，也可以继续运行。复制状态机用于解决分布式系统中的各种容错问题。 例如，具有单个 leader 的大规模系统，如 `GFS`，`HDFS`，通常使用单独的复制状态机来进行 leader 选举和存储 leader 崩溃后重新选举需要的配置信息。`Chubby` 和 `ZooKeeper` 都是复制状态机。

复制状态机通常使用复制日志实现，如下图所示。每个服务器存储一个包含一系列命令的日志，其状态机按顺序执行日志中的命令。 每个日志中命令都相同并且顺序也一样，因此每个状态机处理相同的命令序列， 这样就能得到相同的状态和相同的输出序列，即一致性的状态副本。

![image-20240409093228868](https://hunk-pic-store.oss-cn-beijing.aliyuncs.com/img/20240409093236.png)

共识算法的工作就是保证复制日志的一致性。 每台服务器上的共识模块（Consensus Module）接收来自客户端的命令，并将它们添加到其日志中。 它与其他服务器上的共识模块通信，以确保每个服务器上的日志最终以相同的顺序包含相同的命令，即使有一些服务器失败。 一旦命令被正确复制，每个服务器上的状态机按日志顺序处理它们，并将输出返回给客户端。 这样就形成了高可用的复制状态机。

实际系统中的共识算法通常具有以下属性：

- 它们确保在所有非拜占庭条件下（包括网络延迟，分区和数据包丢失，重复和乱序）的安全性（不会返回不正确的结果）。
- 只要任何多数服务器都可以运行，并且可以相互通信和与客户通信，共识算法就可用。 因此，五台服务器的典型集群可以容忍任何两台服务器的故障。假设服务器突然宕机，它们可以稍后从状态恢复并重新加入集群。
- 它们不依赖于时序来确保日志的一致性：错误的时钟和极端消息延迟可能在最坏的情况下导致可用性问题。
- 在通常情况下，只要集群的多数服务器已经响应了单轮 `RPC`，命令就可以完成;；少数慢服务器不影响整个系统性能。

# Raft 共识算法

## 服务器状态（Server state）

一个 Raft 集群包含若干个服务器节点；通常是 5 个，这样的系统可以容忍 2 个节点的同时故障。正常情况下 Raft 集群中所有服务器都处于三种状态之一：`Follower`，`Candidate`，`Leader`，后面也以这些状态来代指处于该状态的服务器。

![image-20240409093803056](https://hunk-pic-store.oss-cn-beijing.aliyuncs.com/img/20240409093803.png)

如上图所示，所有服务器在这种三种状态中进行转换：

1. Raft 保证任何时候最多只有一个 `Leader`服务器。
2. `Followers` 是集群中大多数服务器的常规状态，同时它也是一个完全被动的角色，只负责响应来自其它服务器的请求。
3. `Candidate`是一个中间临时状态，只出现在选举过程中。当集群状态稳定时，不会有服务器处于Candidate状态。

Raft 把时间分割成任意长度的`任期（term）`，如下图所示。`任期`用连续的整数标记，每一段`任期`从一次选举开始，一个或者多个 `Candidate` 尝试成为 `Leader` 。如果一个 `Candidate` 赢得选举，然后它就在该任期剩下的时间里充当 `Leader`，继续管理集群并为Client提供服务。在某些情况下，一次选举无法选出 `Leader` 。在这种情况下，这一任期会以没有 `Leader` 结束；一个新的任期（包含一次新的选举）会很快重新开始。**Raft 保证了在任意一个任期内，最多只有一个 Leader**。

![image-20240409094114629](https://hunk-pic-store.oss-cn-beijing.aliyuncs.com/img/20240409094114.png)

不同的服务器节点观察到的任期转换的次数可能不同，在某些情况下，一个服务器节点可能没有看到 Leader 选举过程或者甚至整个任期全程。任期在 Raft 算法中充当 **逻辑时钟** 的作用，这使得服务器节点可以发现一些过期的信息比如过时的 Leader 。每一个服务器节点存储一个当前`任期号(currentTerm)`，该编号随着时间单调递增。服务器之间通信的时候会交换当前任期号：

1. 如果一个服务器的当前任期号比其他的服务器小，该服务器会将自己的任期号更新为较大的那个值。
2. 如果一个 Candidate 或者 Leader 发现自己的任期号过期了，它会立即回到 Follower 状态。
3. 如果一个节点接收到一个包含过期的任期号的请求，它会直接拒绝这个请求。

Raft 算法中服务器节点之间使用 `RPC` 进行通信，并且基本的共识算法只需要两种类型的 `RPC`：

1. `请求投票（RequestVote） RPC` 由 Candidate 在选举期间发起。
2. `追加条目（AppendEntries）RPC` 由 Leader 发起，用来复制日志和提供一种心跳机制。

为了在服务器之间传输快照增加了第三种 `RPC`。当服务器没有及时的收到 `RPC` 的响应时，会进行重试， 并且他们能够并行的发起 `RPC` 来获得最佳的性能。

下图列出了在 Raft 协议中，各个服务器需要保存的一些重要状态数据（后面的Leader选举，日志复制等部分会用到）：

![image-20240409095006598](https://hunk-pic-store.oss-cn-beijing.aliyuncs.com/img/20240409095006.png)

## 领导者选举（Leader election）

### 为什么需要触发选举--Why？

Raft 算法通过集群中唯一的权威 Leader 来全权管理日志复制，从而简化日志复制。Leader负责从 Client 端接收`日志条目(Log entry)`，并把日志条目复制到其它服务器上。如果其它服务器接收到 Client 的请求，也会先重定向到 Leader，而不是直接处理。因此，如果集群中没有 Leader，便无法再为Client提供服务。当 Leader 和其它服务器断开时，比如网络中断或者服务器宕机，这个时候就需要在集群中选举出新的 Leader，保证集群的可用性。

### 何时触发选举过程--When？

Raft 算法通过心跳机制来触发选举，服务器以 `Follower` 状态启动，只要在既定的选举超时周期（**election timeout**）内能收到其它服务器（Leader | Candidate）的心跳（`AppendEntries RPC`），就会一直保持这种状态。**一旦心跳超时，Follower 就认为没有可用的Leader，也没有合格的 Candidate，然后自己进入 Candidate 状态，并向集群中的其它服务器发起投票选举**。

这里面其实包含了一些细节，Follower 收到 Candidate 的投票请求后，基于后面介绍的一些投票原则：

1. 如果同意给 Candidate 投票，则会重置自己的超时定时器，再等一个选举超时周期；
2. 如果拒绝了投票，那Follower在选举超时后就会自己进入Candidate状态并发起选举。

### 如何选举新Leader--How？

Follower -> Candidate 的过程：

1. Follower 增加自己的 term 值，然后转换到 Candidate状态。
2. Candidate先给自己投上第一票，然后给并行地给集群中其它服务器发送`投票请求(RequestVote RPC)`。

![image-20240409095523233](https://hunk-pic-store.oss-cn-beijing.aliyuncs.com/img/20240409095523.png)

集群中服务器收到投票请求后，遵循以下投票原则：

1. 在同一个任期内（对于一个指定的term），每个服务器最多给一个 Candidate 投票，遵循`先到先服务（first-come-first-served）`原则，当 Candidate's term < Voter's term，则拒绝投票。
2. 还没投过票（`State.votedFor` == null），或者已经给请求的 Candidate 投过票（`State.votedFor` == candidateId），并且 Candidate的日志比服务器的日志更新，则给 Candidate投票。选举约束章节会详细描述投票过程中检查日志的原因和方法。
3. 在后面集群配置变更的设计中，为了防止被移除的服务器扰乱集群，当服务器认为当前 Leader 存在时，服务器会忽略投票请求 。特别的，当服务器在最小选举超时时间内收到一个 投票请求，它不会更新任期号或者投票。这不会影响正常的选举，每个服务器在开始一次选举之前，至少等待最小选举超时时间。

多数原则保证每轮投票（对于给定的 term）最多有一个 Candidate 获得多数投票。一旦某个 Candidate 赢得选举，成为新的 Leader，它会开始给其它服务器发送心跳消息，在集群中宣告权威，那么同一轮参与竞选的其它Candidate认可新的Leader后，就会识相地回退到Follower，也就阻止了新的一轮选举。

每个 Candidate 发起选举投票后，会出现下面几种情况：

1. **赢得选举**：获得了集群中所有服务器的半数以上投票，成为新的 Leader。
2. **竞选彻底失败**：有其它服务器已经赢得了选举，回退到 Follower 状态。
3. **本任期竞选失败**：集群中没有 winner 赢得选举，Candidate 在超时后增加 term 值，发起新的一轮投票。

### Candidate如何处理Leader的心跳消息？

对于前面的投票结果 #2，正在等待其它服务器投票的 Candidate 收到新的Leader的心跳消息后，需要满足什么条件，才能承认新 Leader的权威性呢？那就是：**Leader's term >= Candidate's term**。如果Leader's term < Candidate's term，说明正在宣告权威的新Leader是上一轮甚至更早的选举结果，那么 Candidate 就会拒绝承认新 Leader 的地位，继续保持 Candidate 状态。

> 设想一下这样的场景：在某一轮任期选举的末尾，Candidate A 收到了多数投票，准备宣告成为新 Leader A，这时由于网络原因，发送给其它服务器的心跳消息延迟了很久，而其它服务器经历一轮 term 选举失败后（上面选举结果#3，投了票但是在新的选举超时周期内没有收到新 Leader 的心跳），已经进入了新一轮任期（term++）的选举，这时其它服务器收到 Leader A 的心跳消息后，会拒绝承认其 Leader 地位，并且继续完成新一轮选举。好比过期的县令任命，也是比较尴尬了。

### 如何避免频繁split votes？

如果有多个 Followers 几乎同时发起了投票选举，因为先给自己投票，这样就导致没有 Candidate 能获得多数票，出现`分票（split votes）`，这会导致每个 Candidate 进入选举结果 #3。如果没有额外的干预措施，Candidate进入新的一轮投票后，很可能重复出现split votes，出现谁都不服谁的尴尬场景，最终导致整个集群长时间无法提供服务。

Raft 算法最终采用了 `随机选举超时（randomized election timeouts）`机制来保证 `split votes` 问题极少出现，即使出现了也可以被快速解决：每个`Candidate`启动选举投票时，都会重启自己的`randomized election timeouts` 来等待选举超时，这个随机时间一般被限制在一个固定的间隔，典型的范围是150 ~ 300(ms)，在后面性能章节会论证为什么推荐的是这个超时范围。

## 日志复制（Log replication）

Raft 集群中每个服务器都维护着自己的日志和状态机，服务器上的日志结构如下：

![image-20240409101221542](https://hunk-pic-store.oss-cn-beijing.aliyuncs.com/img/20240409101221.png)

每个日志条目（Log entry）：

1. 按序标记，有唯一的数组索引。
2. 包含被 Leader 创建时的term值。
3. 包含需要被执行的状态机命令。

> 每个 Client 的请求都包含了一个需要被`复制状态机`执行的命令，比如 SET A = 5，Leader 把这个命令作为一条新的日志条目加入到自己的日志中，然后并发地用`AppendEntries RPCs`向其它服务器复制条目，当条目被安全地复制到其它服务器后，Leader 把条目应用到自己的复制状态机（执行 Client 的命令），此时这条命令已经在集群中安全地被执行，这样的条目称为已提交(Committed)，最后 Leader 向Client 返回执行结果。Raft 算法保证提交的条目是持久的，并且最终将由所有可用的状态机执行。

那什么时候才能提交日志条目呢？一旦新条目被 Leader 成功复制到集群中**多数**服务器后，该日志条目就会被提交，这样即使个别的Follower 的响应比较慢，也不会影响整个集群的性能。同时，Leader 日志中该日志条目之前的所有日志条目也都会被提交，包括由其他 Leader 创建的条目。

### 如何保证日志的一致性？

日志条目中的 term 值用于检测各个服务器上日志的不一致性，并用以保证后面的**日志匹配性质**。

Leader会跟踪被提交的日志条目的`最高索引（LeaderCommit）`，然后将这个索引通过`AppendEntries RPCs` 发送给 Followers（心跳消息中也包含这个索引），其它 Followers 在收到 Committed 日志条目后，就会按照日志顺序将其应用到自己的本地状态机。Raft维护了下面两条日志性质，这两条性质共同组成了所谓的**日志匹配性质**(Log Matching Property)：

1. **如果不同日志中的两个条目具有相同的 index 和 term，则它们存储相同的命令**。或者说是通过 index 和 term 共同组成条目的键，在不同日志中相同键一定得到相同的值。因为只有 Leader 可以创建新的日志条目，所以只要Leader 保证对于指定的 index 和 term最多创建一个条目，并且不改变条目在日志中的位置，集群中的所有日志即可满足这条性质。
2. **如果不同日志中的两个条目具有相同的 index 和 term，那么前面所有条目中的日志都是相同的**。这条性质需要通过在`AppendEntries RPC`加入一致性检查来保证。

为了维护性质 #2，Leader 在向 Followers 发送 `AppendEntries RPC`时，包含了下面的数据：

1. 新条目。
2. 日志中紧挨着新条目的前一个条目（`preEntry`）的 index（`preLogIndex`）和term（`preLogTerm`）。

如果Follower在自己的日志中没有找到相同 index 和 term的 `preEntry`，则拒绝新的条目，返回失败。一致性检查就像一个归纳步骤：一开始空的日志状态肯定是满足Log Matching Property的，然后一致性检查保证了日志扩展时的日志匹配特性。这样也就保证了只要`AppendEntries RPC`成功返回，Leader 就知道了新条目以及之前的 `preEntry` 都是一致的。正常情况下，Leader 和 Followers 的日志都是可以保持一致的，所以上面的一致性检查不会失败。继续考虑服务器 crash 的异常情况：

1. Leader down。新一轮选举产生了新的 Leader，但是旧的 Leader可能没有完全复制日志中的所有条目。
2. Follower down。Followers 重新加入集群后，相比于 Leader的日志，已经缺失了一些新的条目。

这两种情况也可能交替发生，最终出现下面这些日志不一致的场景：

![image-20240409104634897](https://hunk-pic-store.oss-cn-beijing.aliyuncs.com/img/20240409104635.png)

1. 场景a-b：简单的 Follower down，错过一些日志条目的复制。

2. 场景c-d：`Follower` 上包含了当前 Leader的所有日志条目，但是也包含了未提交的条目。

3. 场景e-f：既有缺失的条目，也有未提交的条目。设想场景 f 发生的过程：这个 Follower 原本是term 2 时期的 Leader，但是在提交term 2的3个新条目之前就 crash了，然后迅速重启了，又重新赢得选举成为term 3的Leader，还是来不及提交新的条目就又crash了，然后这次 down 的时间持续了好几个 term 的周期，新的 Leader 又加入了新的条目。

**处理这些日志条目不一致的场景，Raft的设计是让 Leader 强制 Followers 复制自己的日志条目，意味着有冲突的条目时，用Leader的条目覆盖Follower的条目，最终达成一致。注意，对于Follower上未提交的条目，相应的Client请求也并未返回成功，所以覆盖这些冲突的条目，并不会造成数据的丢失。**

首先，如何去检查日志出现了不一致？其实前面已经提到过了，因为集群中所有服务器都各自维护自己的日志，并且 Followers 只被动地处理 `AppendEntries RPC`，所以一致性的检查必须在 Followers 上完成。简化描述：`re = (entries[preLogIndex].term == preLogTerm)`，如果 re==true 说明是一致的则`AppendEntries RPC`返回成功，re==false 说明已经出现了不一致，则`AppendEntries RPC`返回失败，这里其实用到了日志匹配的性质1。

然后，如何快速让 Follower 的日志和 Leader 的日志保持一致？显然，直接复制 Leader 上的整个日志不是聪明的办法。我们需要找到Follower 中不一致的日志条目的最小索引，其左边显然都是一致的，而右边是需要被覆盖的。Leader为每个服务器都维护了一个`nextIndex`，它表示了 Leader 下一次发给该 Follower 的日志条目的索引，Leader 会将所有 `nextIndex` 初始化为其日志中的最大条目索引值+1，比如上图中的11。当`AppendEntries RPC`返回失败后，Leader就将对应的 `nextIndex`减1，然后重试`AppendEntries RPC`，直到找到日志匹配的位置。一旦`AppendEntries RPC`的一致性检查成功，将会直接移除冲突的日志条目，然后将 Leader 的日志条目append到该位置。当服务器上日志重新保持一致后，并且没有产生新条目时，Leader 给 Followers发送的`AppendEntries RPC`中就不再携带任何条目数据了，仅仅是作为心跳消息。

最后，考虑优化一致性的收敛速度？因为每次只是对`nextIndex` 减1，就会导致大量的 `rejected AppendEntries RPC`，可以考虑增大搜索的步长，每次搜索一个term，`AppendEntries RPC`每次发送的就是Leader上所有条目的一个term子集，这样需要的`AppendEntries RPC`数量取决于有冲突的 term 数量而不是所有冲突条目。但是在实际场景中，上述极端情况不易出现，也不太可能出现大量的日志条目冲突，所以根据具体场景决定是否优化。

至此，可以总览以下`AppendEntries RPC`的全貌了：

![image-20240409105615259](https://hunk-pic-store.oss-cn-beijing.aliyuncs.com/img/20240409105615.png)

关于日志复制的一致性，还有这些性质：

1. Leader Append-Only Property： Leader永远不会去覆盖或者删除自己的日志条目，只能追加条目。

## 安全性（Safety）

前面描述了 Raft 算法是如何进行Leader 选举和日志复制的。然而，到目前为止描述的机制并不能充分地保证每一个状态机会按照相同的顺序执行相同的指令。例如，一个Follower可能会进入不可用状态，在此期间，Leader 可能提交了若干的日志条目，然后这个 Follower 可能会被选举为 Leader 并且用新的日志条目覆盖这些日志条目；结果，不同的状态机可能会执行不同的指令序列。

下面通过对 Leader 选举增加一个约束来完善 Raft 算法。这一限制保证了对于给定的任意任期号， Leader 都包含了之前各个任期所有被提交的日志条目，这也是所谓的c**Leader完备性**(Leader Completeness Property)。

### 选举约束(Election restriction)

下面这个序列图说明了为什么Leader不能用旧任期内的日志条目来判断提交承诺（commitment）：

![image-20240409105853047](https://hunk-pic-store.oss-cn-beijing.aliyuncs.com/img/20240409105853.png)

- a - S1 被选举为term 2的Leader，并部分复制了term=index=2的条目到S2上。
- b - S1 crash，S5竞选获得S3和S4的投票成为term=3的Leader（S2会拒绝给S5投票），并且加入了一条新的条目index=2，term=3。
- c - S5 crash，S1重启并竞选成为term=4的Leader，然后继续复制条目，首先是term=2的条目被复制到多数服务器（S1 S2 S3），但是还没来得及提交，接下里考虑d和e的情况：
- d - S1 crash，S5重启并竞选成为Leader，然后按照上一节的日志复制策略向其它服务器复制了term=3，index=2的条目，这是典型的从 Leader -> Followers 合并日志冲突的过程。
- e：重新考虑d场景，如果S1 crash之前，已经把term=2和term=4的条目复制到多数服务器，那么这时候就不能再让 S5赢得选举，否则term=2和term=4的条目就会被覆盖和删除，违背了Raft对于已提交条目的保证：**提交的条目是持久的，并且最终将由所有可用的状态机执行。**

因此，`RequestVote RP`C实现了一个`选举约束（Election restriction）`：`RPC` 携带Candidate的日志信息，投票者（Follower）发现自己的日志比Candidate的日志更新（more up-to-date），便会拒绝投票给该 Candidate。判断日志more up-to-date的的方法是比较日志的最后一条条目（last entry）：

1. term值不同，那term更大的视为more up-to-date；
2. term值相同，更长的日志视为more up-to-date。

再次思考上图中 e 的场景，如果没有选举约束，在S1 crash后，S5赢得选举的可能性大吗？在 c 场景中，S1竞选成功后，S1，S2，S3，S4的 `currentTerm`==4。S1 crash后，S5 以当前 `currentTerm`==3发起选举，最多只能获得自己和 S4 的投票，本轮竞选必然失败，需要在随机选举超时后，增加 `currentTerm` 值后重新发起选举，而这时 S2 和 S3 同样很长时间没有接收到 Leader 的心跳，很可能会在 S5 发起第二轮选举之前也加入竞选，而且只要 S2 和 S3 早于 S5 发起选举，那 S5 是不可能竞选成功的，但是这种情况是否出现取决于下面两个不确定因素，所以很难评估准确概率，但是可能性的确是存在的。

1. S1 crash的具体时间点。
2. S2、S3和S5本轮的随机选举超时时间，范围150~300(ms)。

### 提交之前任期内的日志条目(Committing entries from previous terms)

Raft 永远不会通过计算副本数目的方式来提交之前任期内的日志条目。只有 Leader 当前任期内的日志条目才通过计算副本数目的方式来提交；一旦当前任期的某个日志条目以这种方式被提交，那么由于日志匹配特性，之前的所有日志条目也都会被间接地提交。

### 安全性质(Safety property)

除了日志匹配性质，Raft还保证下面几条重要性质：

1. **Leader Completeness Property**：如果在给定的term中提交了一个日志条目，那么该条目将出现在所有更高term的Leader的日志中，或者说，在选举产生的新Leader的日志中，总是包含了旧Leader提交过的日志条目。
2. **State Machine Safety Property**：如果服务器已经将给定index的日志条目应用到其状态机，则其他服务器将不会为相同的index应用不同的日志条目。此外，Raft要求服务器按日志索引顺序应用条目，使日志的一致性最终能够保证所有服务器上状态机的一致性。

### Follower和Candidate崩溃(Follower and Candidate crashes)

前面主要讨论的还是Leader宕机后的一些情况，如果Follower和Candidate出现宕机，后续的`RPCs`都会失败，Raft会让这些 `RPCs`无限期重试，并且所有的Raft `RPCs` 都是幂等的，即便是它们在已经执行完 `RPC`只是还没有响应 Leader 的时候crash，后续的 `RPC`也是安全的。

### 定时和可用性(Timing and availability)

通过下面这个不等式来说明各种定时和集群可用性（集群及时响应客户请求的能力）之间的理想关系，以此可以维护一个稳定的集群：

`broadcastTime <= electionTimeout <= MTBF`

1. `broadcastTime`：广播时间，服务器向集群中的每个服务器并行发送`RPC`并接收它们的响应所需的平均时间，依赖于底层系统，典型的 0.5 ms ~ 20 ms。
2. `electionTimeout` ：选举超时时间，典型地10 ms ~ 500 ms。
3. `MTBF`：单个服务器的平均故障间隔时间，也就是服务器平均多久出现一次故障，典型地几个月。

`broadcastTime`和`MTBF`是由系统决定的，但是`electionTimeout`是我们自己选择的。`broadcastTime`的大小应该比 `electionTimeout` 小几个数量级，以便 Leader 能够可靠地发送所需的心跳消息，防止 Followers发起选举。`electionTimeout`也应该比 `MTBF` 少几个数量级，这样系统才能稳定运行。

## 集群成员变更（Cluster membership changes）

到目前为止，我们都假设集群的配置（参与一致性算法的服务器集合）是固定不变的。但是在实践中，偶尔会改变集群的配置，例如替换那些宕机的机器或者改变复制程度。尽管可以通过使整个集群下线，更新所有配置，然后重启整个集群的方式来实现，但是在更改期间集群会不可用。另外，如果存在手工操作步骤，那么就会有操作失误的风险。为了避免这样的问题，我们决定将配置变更自动化并将其纳入到 Raft 一致性算法中来。

为了使配置变更机制能够安全，在转换的过程中不能够存在任何时间点使得同一个任期里可能选出两个 Leader 。**不幸的是，任何服务器直接从旧的配置转换到新的配置的方案都是不安全的。**一次性自动地转换所有服务器是不可能的，在转换期间整个集群可能划分成两个独立的大多数，如下图，同一个任期里两个不同的 Leader 会被选出。一个获得旧配置里过半机器的投票，一个获得新配置里过半机器的投票。

![image-20240409111531031](https://hunk-pic-store.oss-cn-beijing.aliyuncs.com/img/20240409111531.png)

为了保证安全性，配置变更必须采用一种两阶段方法。在 Raft 中，集群先切换到一个过渡的配置，我们称之为联合共识（joint consensus）；一旦联合共识已经被提交了，那么系统就切换到新的配置上。联合一致结合了老配置和新配置：

- 日志条目被复制给集群中新、老配置的所有服务器。
- 新、旧配置的服务器都可以成为 leader 。
- 达成共识（针对选举和提交）需要分别在两种配置上获得过半的支持。

联合共识允许独立的服务器在不妥协安全性的前提下，在不同的时刻进行配置转换过程。**此外，联合共识允许集群在配置变更期间依然响应客户端请求。**

![image-20240409111646623](https://hunk-pic-store.oss-cn-beijing.aliyuncs.com/img/20240409111646.png)

集群配置在复制日志中以特殊的日志条目来存储和通信；上图展示了配置变更过程。当一个 leader 接收到一个改变配置从 C-old 到 C-new 的请求，它就为联合一致将该配置存储为一个日志条目（图中的 C-old,new），并以前面描述的方式复制该条目。一旦某个服务器将该新配置日志条目增加到自己的日志中，它就会用该配置来做出未来所有的决策（服务器总是使用它日志中最新的配置，无论该配置日志是否已经被提交）。这就意味着 Leader 会使用 C-old,new 的规则来决定 C-old,new 的日志条目是什么时候被提交的。如果 Leader 崩溃了，新 Leader 可能是在 C-old 配置也可能是在 C-old,new 配置下选出来的，这取决于赢得选举的 Candidate 是否已经接收到了 C-old,new 配置。在任何情况下， C-new 在这一时期都不能做出单方面决定。

一旦 C-old,new 被提交，那么 C-old 和 C-new 都不能在没有得到对方认可的情况下做出决定，并且 Leader完备性保证了只有拥有 C-old,new 日志条目的服务器才能被选举为 Leader 。现在Leader创建一个描述 C-new 配置的日志条目并复制到集群其他节点就是安全的了。此外，新的配置被服务器收到后就会立即生效。当新的配置在 C-new 的规则下被提交，旧的配置就变得无关紧要，同时不使用新配置的服务器就可以被关闭了。如上图所示，任何时刻 C-old 和 C-new 都不能单方面做出决定；这保证了安全性。

在关于配置变更还有三个问题需要解决。第一个问题是，新的服务器开始时可能没有存储任何的日志条目。当这些服务器以这种状态加入到集群中，它们需要一段时间来更新来赶上其他服务器，这段它们无法提交新的日志条目。为了避免因此而造成的系统短时间的不可用，**Raft 在配置变更前引入了一个额外的阶段，在该阶段，新的服务器以没有投票权身份加入到集群中来（leader 也复制日志给它们，但是考虑过半的时候不用考虑它们）**。一旦该新的服务器追赶上了集群中的其他机器，配置变更就可以按上面描述的方式进行。

第二个问题是，集群的 Leader 可能不是新配置中的一员。在这种情况下，Leader 一旦提交了 C-new 日志条目就会回到Follower 状态。这意味着有这样的一段时间（leader 提交 C-new 期间），Leader 管理着一个不包括自己的集群；它复制着日志但不把自己算在过半里面。Leader 转换发生在 C-new 被提交的时候，因为这是新配置可以独立运转的最早时刻（将总是能够在 C-new 配置下选出新的领导人）。在此之前，可能只能从 C-old 中选出领导人。

第三个问题是，那些被移除的服务器（不在 C-new 中）可能会扰乱集群。这些服务器将不会再接收到心跳，所以当选举超时，它们就会进行新的选举过程。它们会发送带有新任期号的 `RequestVote RPCs` ，这样会导致当前的Leader 回到Follower 状态。新的Leader 最终会被选出来，但是被移除的服务器将会再次超时，然后这个过程会再次重复，导致系统可用性很差。

**为了防止这种问题，当服务器认为当前 Leader 存在时，服务器会忽略 `RequestVote RPCs` 。特别的，当服务器在最小选举超时时间内收到一个 `RequestVote RPC`，它不会更新任期号或者投票。这不会影响正常的选举，每个服务器在开始一次选举之前，至少等待最小选举超时时间。**相反，这有利于避免被移除的服务器的扰乱：如果 Leader 能够发送心跳给集群，那它就不会被更大的任期号废黜。

## 日志压缩（Log compaction）

随着日志的条目的增多，最终一定会达到系统资源边界，导致可用性问题，所以需要设计额外的机制来做日志压缩，而快照技术则是最简单实用的方法。

下图描述了Raft提供的日志压缩的快照方法：

![image-20240409112055488](https://hunk-pic-store.oss-cn-beijing.aliyuncs.com/img/20240409112055.png)

捕获快照时，服务器用一个新的快照来替换日志里已提交的条目（图中log index 1 ~ 5），快照里只存储状态机的当前状态（也就是快照所包含的条目被应用到状态机后的状态），以及快照中最后一个条目的 index 值（`last included index`）和 term 值（`last included term`），这两个值实际上标记了快照的位置：紧挨着 index=6，term=3 的日志前面。一旦捕获快照完成（快照数据写入到磁盘），服务器将会在日志中删除新快照所包含的条目，以及之前的快照。

原则上，所有服务器都可以独立的捕获快照，但是 Leader 也必须偶尔向日志落后的 Followers 发送快照数据，比如：

1. 速度很慢的Followers。
2. 宕机很久后重启或者新加入集群的 Followers。

Leader使用 `InstallSnapshot RPC`来向日志条目更新落后的 Followers 发送快照数据：

![image-20240409112507046](https://hunk-pic-store.oss-cn-beijing.aliyuncs.com/img/20240409112507.png)

如上图中Receiver implementation所描述，Followers需要自己判断是否接收 Leader 发送的快照数据。

为什么不限制只允许Leader来捕获快照然后将快照数据发送给Followers呢？

1. 发送大量快照数据浪费集群的网络带宽，其它Followers获取的快照的最廉价方式就是从本地获取。
2. 让Leader的实现更为复杂，比如需要不阻塞新的Client请求，所以需要并行地发送快照数据。

此外，讨论两个会影响快照性能的问题及解决方案：

1. 服务器需要决定什么时候去捕获快照，太频繁会浪费磁盘I/O的带宽，太不频繁就有耗尽存储容量的风险，还会增加服务器重启时重播日志所需的时间。一个简单的策略是在日志达到固定大小(以字节为单位)时捕获快照。如果将此大小设置为比快照的预期大小大得多，那么用于快照的磁盘带宽开销将很小
2. 捕获的快照数据需要写入磁盘，所以会需要大量的时间，我们不希望这个过程使正常的操作被延迟。解决方案就是利用写时复制技术（copy-on-write），在写快照的时候也可以处理新的更新。

## 客户端交互（Client interaction）

本节介绍客户端如何和 Raft集群进行交互，包括客户端如何找到 Leader，以及 Raft 是如何支持线性化语义的。这些问题对于所有基于一致性的系统都存在，并且 Raft 的解决方案和其他的也差不多。

Raft 的客户端发送所有的请求给 Leader 。当客户端第一次启动的时候，它会随机挑选一个服务器进行通信。如果客户端第一次挑选的服务器不是 Leader ，那么该服务器会拒绝客户端的请求并且提供关于它最近接收到的 Leader的信息（`AppendEntries RPC`请求包含了 Leader 的网络地址）。如果 Leader 已经崩溃了，客户端请求就会超时；客户端之后会再次随机挑选服务器进行重试。

我们 Raft 的目标是要实现**线性化语义**（每一次操作立即执行，只执行一次，在它的调用和回复之间）。但是，如上述，Raft 可能执行同一条命令多次：如果 Leader 在提交了该日志条目之后，响应客户端之前崩溃了，那么客户端会和新的 Leader 重试这条指令，导致这条命令被再次执行。**解决方案就是客户端对于每一条指令都赋予一个唯一的序列号。然后，状态机跟踪每个客户端已经处理的最新的序列号以及相关联的回复。如果接收到一条指令，该指令的序列号已经被执行过了，就立即返回结果，而不重新执行该请求。**

Raft 在不写入日志条目的前提下处理只读操作，但是这可能导致返回给 Client 的是过期数据(stale data)，比如正在响应 Client 的Leader A已经被废黜（只是自己还不知道），而新 Leader B上已经更新了 Leader A 正在查询的数据，这是在出现网络分区问题时常见的场景。因此，Raft设计了两条额外的措施来保证不会给 Client返回过期的数据。

1. 首先，Leader必须拥有提交条目的最新信息。Leader 完整性特性保证了 Leader 一定拥有所有已经被提交的日志条目，但是在它任期(term)开始的时候，它可能不知道哪些是已经被提交的。为了知道这些信息，它需要在它的任期里提交一个日志条目。Raft 通过让 Leader 在任期开始的时候提交一个空的没有任何操作的日志条目到日志中来处理该问题。？
2. 其次，在处理只读请求之前，Leader必须检查它是否已被废黜(如果最近的Leader已经当选，它的信息可能会过时)。在响应只读请求之前，Raft通过让 Leader 与集群大多数服务器交换心跳消息来处理这个问题，或者，Leader可以依赖心跳机制来提供一种租约形式，但是这将依赖于安全的定时(假设时间误差时有界的)。

## 实现和评估（Implementation and evaluation）

`Raft` 比 `Paxos` 更简单，更容易被理解。

### 性能（Performance）

下图给出了探测和选举替换 Crashed Leader所需时间的平均测试数据，依次衡量选举超时设置对集群可用性的影响：

![image-20240409113247996](https://hunk-pic-store.oss-cn-beijing.aliyuncs.com/img/20240409113248.png)

上图中的顶部图显示，在选举超时中进行少量的随机操作就足以避免在选举中出现分裂投票。在缺乏随机性的情况下（150 ~ 150 ms 的选举超时），在我们的测试中，由于多次出现分票(split votes)，Leader选举持续时间超过10秒。仅仅增加 5 ms的随机性就有很大的帮助，平均停机时间为 287 ms。使用更多的随机性可以改善最坏情况的行为：使用 50 ms 的随机性，最坏情况的完成时间(超过1000次试验)是 513 ms。

上图中的底部图显示，可以通过减少选举超时来减少停机时间。选举超时时间为12 ~ 24(ms)，平均只需要 35 ms就可以选出Leader(最长的试验需要 152 ms)。但是，选举超时也不是越短越好，需要考虑前面提到过的集群广播时间（`broadcastTime`，广播时间会影响到心跳超时的设置），当继续降低选举超时时，**Leader很难在其他服务器开始新的选举之前广播心跳**，这可能会导致不必要的Leader变更，降低整个系统的可用性。而广播时间又是不稳定的，取决于网络和系统性能等因素**。因此，我们建议使用保守的选举超时，如150 ~ 300(ms)，选举超时时间差不多大广播超时时间一个数量级，这样不太可能导致不必要的Leader变更，并且很好的提高了系统可用性。**

# References

[Raft 官网](https://raft.github.io/)

[Raft 论文](https://raft.github.io/raft.pdf)

[Designing for Understandability: the Raft Consensus Algorithm - Slide](https://raft.github.io/slides/uiuc2016.pdf)

[Raft 动态分析](http://thesecretlivesofdata.com/raft/)