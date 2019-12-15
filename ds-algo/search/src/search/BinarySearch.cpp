#include "Search.hpp"
#include "../common/common.hpp"

int binarysearch(int *a, int n, int key)
{
    cout<<"***********[Begin]Binary Search**********"<<endl;
    int re = -1;
    int low = 0, high = n - 1, ite = 0;
    while(low <= high) {
        cout<<"Search counter: " << ite++ << endl;
        //mid = (high + low)/2; //high和low都很大时求和容易溢出
        int mid = low + ((high - low)>>1);
        if(key == a[mid]) {
            re =  mid;
            break;
        } else if (key > a[mid]) {
            //注意low和high的更新，如果是low=mid和high=mid，则在low=high时会发生死循环
            low = mid + 1;
        } else {
            high = mid -1;
        }
    }
    if(re == -1) {
        cout<<"Search failed"<<endl;
    }
    cout<<"***********[End]Binary Search**********"<<endl;
    return re;
}

//二分查找变体问题1：
//查找第一个值等于给定值的元素
int binarysearch_first_euqal(int *a, int n, int key)
{
    int re = -1;
    int low = 0, high = n - 1, ite = 0;
    while(low <= high) {
        cout<<"Search counter: " << ite++ << endl;
        int mid = low + ((high - low)>>1);
        if(key == a[mid]) {
            //mid等于0的时候，意味着数组第一个元素就是我们查找的结果
            //a[mid-1]不等于key的时候，意味着mid位置的元素就是我们要找的结果
            if((mid == 0) || a[mid - 1] != key) {
                re = mid;
                break;
            } else {
                high = mid -1;
            }            
        } else if (key > a[mid]) {
            low = mid + 1;
        } else {
            high = mid -1;
        }
    }
    if(re == -1) {
        cout<<"Search failed"<<endl;
    }
    return re;
}

//二分查找变体问题2：
//查找第最后一个值等于给定值的元素
int binarysearch_last_euqal(int *a, int n, int key)
{
    int re = -1;
    int low = 0, high = n - 1, ite = 0;
    while(low <= high) {
        cout<<"Search counter: " << ite++ << endl;
        int mid = low + ((high - low)>>1);
        if(key == a[mid]) {
            //和第一个问题类似，只不过是向数组右边移动判断
            //注意此处，mid一定是和最后的位置n-1比较，而不是查找过程中变化的high
            if((mid == (n - 1)) || a[mid + 1] != key) {
                re = mid;
                break;
            } else {
                low = mid + 1;
            }   
        } else if (key > a[mid]) {
            low = mid + 1;
        } else {
            high = mid -1;
        }
    }
    if(re == -1) {
        cout<<"Search failed"<<endl;
    }
    return re;
}

//二分查找变体问题3：
//查找第一个大于等于给定值的元素
int binarysearch_first_euqal_or_large(int *a, int n, int key)
{
    int re = -1;
    int low = 0, high = n - 1, ite = 0;
    while(low <= high) {
        cout<<"Search counter: " << ite++ << endl;
        int mid = low + ((high - low)>>1);
        if (a[mid] >= key) {
            //继续检查mid左边一位的数组元素,注意数组左边界的检查
            if( (mid == 0) || a[mid - 1] < key) {
                re = mid;
                break;
            } else {
                //mid左边一位仍然大于等于key，则继续迭代查找
                high = mid - 1;
            }
        } else {
            low = mid + 1;
        }
    }
    if(re == -1) {
        cout<<"Search failed"<<endl;
    }
    return re;
}

//二分查找变体问题4：
//查找第最后一个值小于等于给定值的元素
int binarysearch_last_euqal_or_small(int *a, int n, int key)
{
    int re = -1;
    int low = 0, high = n - 1, ite = 0;
    while(low <= high) {
        cout<<"Search counter: " << ite++ << endl;
        int mid = low + ((high - low)>>1);
        if (a[mid] <= key) {
            //继续检查mid右边一位的数组元素,注意数组右边界的检查
            if( (mid == (n - 1)) || a[mid + 1] > key) {
                re = mid;
                break;
            } else {
                //mid右边一位仍然小等于key，则继续迭代查找
                low = mid + 1;
            }
        } else {
            high = mid - 1;
        }
    }
    if(re == -1) {
        cout<<"Search failed"<<endl;
    }
    return re;
}

void BinarySearchTest()
{
    int length = 10000;
    int key = 1234;
    int *a = generateSortedArray(length);
    int re = binarysearch(a, length, key);
    cout<<"Search: ["<<key<<" ], found index is: a["<<re<<"] = "<<a[re]<<endl;

    length = 10;
    key = 1;
    int a1[10] = {1, 1, 1, 2, 2, 2, 2, 7, 8, 9};
    cout<<"***********[Begin]Binary Search First Equal**********"<<endl;
    re = binarysearch_first_euqal(a1, length, key);
    cout<<"Search: ["<<key<<" ], found first index is: a1["<<re<<"] = "<<a1[re]<<endl;
    
    key = 2;
    re = binarysearch_first_euqal(a1, length, key);
    cout<<"Search: ["<<key<<" ], found first index is: a1["<<re<<"] = "<<a1[re]<<endl;
    cout<<"***********[End]Binary Search First Equal**********"<<endl;

    cout<<"***********[Begin]Binary Search Last Equal**********"<<endl;
    re = binarysearch_last_euqal(a1, length, key);
    cout<<"Search: ["<<key<<" ], found last index is: a1["<<re<<"] = "<<a1[re]<<endl;
    
    key = 1;
    re = binarysearch_last_euqal(a1, length, key);
    cout<<"Search: ["<<key<<" ], found last index is: a1["<<re<<"] = "<<a1[re]<<endl;
    cout<<"***********[End]Binary Search Last Equal**********"<<endl;

    cout<<"***********[Begin]Binary Search First Equal Or Large**********"<<endl;
    key = 4;
    re = binarysearch_first_euqal_or_large(a1, length, key);
    cout<<"Search: ["<<key<<" ], found first index is: a1["<<re<<"] = "<<a1[re]<<endl;

    key = 8;
    re = binarysearch_first_euqal_or_large(a1, length, key);
    cout<<"Search: ["<<key<<" ], found first index is: a1["<<re<<"] = "<<a1[re]<<endl;
    cout<<"***********[End]Binary Search First Equal Or Large**********"<<endl;

    cout<<"***********[Begin]Binary Search Last Equal Or Small**********"<<endl;
    key = 2;
    re = binarysearch_last_euqal_or_small(a1, length, key);
    cout<<"Search: ["<<key<<" ], found last index is: a1["<<re<<"] = "<<a1[re]<<endl;

    key = 6;
    re = binarysearch_last_euqal_or_small(a1, length, key);
    cout<<"Search: ["<<key<<" ], found last index is: a1["<<re<<"] = "<<a1[re]<<endl;

    key = 20;
    re = binarysearch_last_euqal_or_small(a1, length, key);
    cout<<"Search: ["<<key<<" ], found last index is: a1["<<re<<"] = "<<a1[re]<<endl;
    cout<<"***********[End]Binary Search Last Equal Or Small**********"<<endl;
}
