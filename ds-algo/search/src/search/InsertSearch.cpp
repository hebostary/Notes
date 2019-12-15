#include "Search.hpp"
#include "../common/common.hpp"

int insertsearch(int *a, int n, int key)
{
    cout<<"***********[Begin]Insert Search**********"<<endl;
    int re = -1;
    int low, mid, high;
    low = 0;
    high = n - 1;
    int ite = 0;
    while(low <= high) {
        cout<<"Search counter: " << ite++ << endl;
        mid = low + (high - low) * (key - a[low]) / (a[high] - a[low]);
        if(key == a[mid]) {
            re = mid;
            break;
        } else if (key > a[mid]) {
            low = mid + 1;
        } else {
            high = mid -1;
        }
    }
    if(re == -1) {
        cout<<"Search failed"<<endl;
    }
    cout<<"***********[End]Insert Search**********"<<endl;
    return re;
}

void InsertSearchTest()
{
    int length = 10000;
    int key = 1234;
    int *a = generateSortedArray(length);
    int re = insertsearch(a, length, key);
    cout<<"Search: ["<<key<<" ], found index is: "<<re<<endl;
}