# 一致性哈希（Consistent Hashing）



## 一致性哈希的工作原理

关于一致性哈希的原理和应用场景，网络上已经有很多写的非常好的文章了：

1. [A Guide to Consistent Hashing](https://www.toptal.com/big-data/consistent-hashing)
2. [Consistent Hashing | System Design](https://www.geeksforgeeks.org/consistent-hashing/)
3. [Consistent Hashing Explained](https://systemdesign.one/consistent-hashing-explained/?ref=highscalability.com#references)

## C++ Example

下面的例子来自 [Consistent Hashing | System Design](https://www.geeksforgeeks.org/consistent-hashing/)，实现了一致性哈希的核心功能，即如何添加删除虚拟节点，以及如何确定给定的查找键应该落在哪个节点上。

```c++


#include <bits/stdc++.h>
 
using namespace std;
 
class ConsistentHashRing {
private:
    map<int, string> ring; // 哈希环：key是虚拟节点（例如 Server_A_1）的哈希值；value是物理节点（例如 Server_A）的名字
    set<int> sorted_keys; // 有序的虚拟节点哈希值
    int replicas;
 
    int get_hash(const string& value) {
        hash<string> hash_function;
        return hash_function(value);
    }
 
public:
    ConsistentHashRing(int replicas = 3) : replicas(replicas) {}
 
      // Function to add Node in the ring
    void add_node(const string& node) {
        for (int i = 0; i < replicas; ++i) {
            int replica_key = get_hash(node + "_" + to_string(i));
            ring[replica_key] = node;
            sorted_keys.insert(replica_key);
        }
    }
 
      // Function to remove Node from the ring
    void remove_node(const string& node) {
        for (int i = 0; i < replicas; ++i) {
            int replica_key = get_hash(node + "_" + to_string(i));
            ring.erase(replica_key);
            sorted_keys.erase(replica_key);
        }
    }
 
    // 查找指定的 key 应该被记录在哪个物理节点上
    string get_node(const string& key) {
        if (ring.empty()) {
            return "";
        }
 
        int hash_value = get_hash(key);
        // 返回 sorted_keys 里首个不小于（即大于或等于） hash_value 的元素的迭代器
        auto it = sorted_keys.lower_bound(hash_value);

        // lower_bound 没找到合适的节点则返回尾后迭代器
        if (it == sorted_keys.end()) {
            // 直接使用 sorted_keys 里的首节点，支持形成哈希环的查找路径
            it = sorted_keys.begin();
        }
 
        return ring[*it];
    }
};
 
int main() {
    ConsistentHashRing hash_ring;
 
    // Add nodes to the ring
    hash_ring.add_node("Node_A");
    hash_ring.add_node("Node_B");
    hash_ring.add_node("Node_C");
 
    // Get the node for a key
    string key = "first_key";
    string node = hash_ring.get_node(key);
 
    cout << "The key '" << key << "' is mapped to node: " << node << endl;
 
    return 0;
}
```

结合下面的调试信息就更清晰了，当查询 first_key 这个键时，`hash_value("first_key")`的计算结果是 1030243938，这个哈希值大于 `sorted_keys`里的最大值，因此直接将其落在首节点上，即 `sorted_keys[0]`对应的物理节点 `Node_C`。

![image-20240426162625500](https://hunk-pic-store.oss-cn-beijing.aliyuncs.com/img/20240426162625.png)



## 一致性哈希应用场景

一致性哈希常用于分布式系统，用于在多个节点或服务器之间有效地分布和平衡数据。它主要用于以下场景：

### 负载均衡（Load Balancing）

在负载均衡中，一致性哈希有助于以平衡的方式在多个服务器之间分配传入请求或数据。

- 每个服务器都被分配了一系列哈希值（通过虚拟节点，即前面代码里的 `replicas`），并对传入请求或数据键进行哈希处理以确定哪个服务器应该处理它们。
- 通过使用一致性哈希，系统可以添加或删除服务器，而不会显着影响请求或数据的分布，因为只有一部分密钥或请求需要重新映射到不同的服务器。

### 缓存（Caching）

在缓存系统中，一致性哈希用于确定哪个缓存节点应存储或检索数据。

- 每个数据键都经过哈希处理，以确定哪个缓存节点应负责存储或检索数据。
- 一致性哈希确保每条数据存储在一致的位置，减少缓存未命中并提高缓存效率。

### 分区（Partitioning）or 分片（Sharding）

一致性哈希用于在分布式数据库或存储系统中跨多个节点对数据进行分区：

- 每个节点负责一系列哈希值，并对数据键进行哈希处理以确定哪个节点应存储或检索数据。
- 通过使用一致性哈希，系统可以通过添加新节点来水平扩展，而无需重新哈希所有数据，因为只需将一部分数据重新映射到新节点。

### 韧性（Resilience）

一致性哈希提供了针对分布式系统中节点故障的弹性：

- 当某个节点发生故障或从系统中移除时，只需将一部分数据重新映射到其他节点，减少节点故障对整个系统的影响。
- 一致性哈希确保即使在存在节点故障的情况下数据的分布也保持平衡，从而维持系统性能和可用性。



# 布隆过滤器（Bloom Filters）

## References

[Bloom Filters Explained](https://systemdesign.one/bloom-filters-explained/)

[Bloom Filters in System Design](https://www.geeksforgeeks.org/bloom-filters-in-system-design/?ref=ml_lbp)