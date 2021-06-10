#ifndef COMMON_H_
#define COMMON_H_

#include <iostream>
#include <time.h>
#include <vector>
#include <map>
#include <set>
#include <string.h>
#include <queue>
#include <array>
#include <algorithm>
#include <sstream>
#include <fstream>
#include <memory>
#include <new>
#include <utility>

#if defined(_WIN64)

#else
#include <uuid/uuid.h>
#endif

using namespace std;

#define ENTER_FUNC EnterFunc(__FUNCTION__);
#define EIXT_FUNC ExitFunc(__FUNCTION__);

void EnterFunc(string msg);
void ExitFunc(string msg);

/*
* function: 记录函数的运行时间，需要在函数运行前和结束时调用两次，
* 并且在第二次调用时传入参数finish=true，打印运行时间
*/
void recordRunTime(bool finish=false);

/*
* function：生成大小为size，元素值范围为0 - max的vector
*/
vector<int> createRandomVec(int size, int max);

/*
* function：遍历打印vector容器的所有元素
*/
template<class T>
void printVec(vector<T> &vec) {
    return;
    cout<<"Print Vector: [";
    for (auto ite = vec.begin(); ite != vec.end(); ++ite)
    {
        cout<<*ite<<" ";
    }
    cout<<"]"<<endl;
}

/*
* 生成UUID字符串
*/
string createUUID();

template<typename _RandomAccessIterator>
void printContainer(string title, _RandomAccessIterator begin, _RandomAccessIterator end)
{
    auto ite = begin;
    cout << title << ": [";
    while (ite != end)
    {
        cout << *ite;
        if( ++ite != end ) cout << ", ";
    }
    cout << "]\n";
}

#endif
