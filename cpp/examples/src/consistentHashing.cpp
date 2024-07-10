

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