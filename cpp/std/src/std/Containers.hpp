#ifndef CONTAINERS_H_
#define CONTAINERS_H_

#include "../common/common.hpp"
#include <array>
#include <vector>
#include <list>
#include <deque>
#include <forward_list>
#include <fstream>
#include <sstream>
#include <map>
#include <set>
#include <unordered_map>
#include <unordered_set>

void TestContainers();

void TestInit();
void TestSwap();
void TestCompare();

//测试顺序容器
void TestArray();
void TestVector();
void TestString();
void TestList(); //双向链表
void TestDeque(); //双端队列
void TestForwardList();

//测试关联容器
void TestMapAndSet();
//单词转换程序
void TestWordTransform();
map<string, string> buildMap(ifstream &mapFile);
string transform(string &word, map<string, string> &map);

//测试无序容器
void TestUnorderedMapAndSet();

template<typename T1, typename T2>
void printUnorderMapInfo(unordered_map<T1, T2> &unorderMap) {
    size_t keyNum = unorderMap.size();
    size_t bucketNum = unorderMap.bucket_count();
    size_t maxBucketNum = unorderMap.max_bucket_count();
    float loadFactor = unorderMap.load_factor();
    float maxLoadFactor = unorderMap.max_load_factor();
    cout << "map size = " << keyNum << ", bucket_count = " << bucketNum 
    << ", max_bucket_count = " << maxBucketNum 
    << ", load_factor = " << loadFactor 
    << ", max_load_factor = " << maxLoadFactor << "\n[";
    for (size_t i = 0; i < bucketNum; i++)
    {
        //打印每个桶内的元素数量
        cout << unorderMap.bucket_size(i);
        if ( i != bucketNum-1 ) cout << ", ";
    }
    cout << "]\n";
}    

#endif