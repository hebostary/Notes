#include "Sort.hpp"
#include "../common/common.hpp"

void mergesort_sep(vector<int> &nums, int left, int right);
void merge(vector<int> &nums, int left, int mid, int right); 

int mergesort(vector<int> &nums) {
    cout<<"***********[Begin]Merge sort**********"<<endl;
    printVec(nums);
    int size = nums.size();
    if (size < 2) return 0;

    mergesort_sep(nums, 0, size - 1);
    
    printVec(nums);
    cout<<"***********[End]Merge sort**********"<<endl;
    return 0;
}

void mergesort_sep(vector<int> &nums, int left, int right) {
    //递归终止条件
    if(left >= right) return;

    int mid = (left + right) / 2;
    mergesort_sep(nums, left, mid);
    mergesort_sep(nums, mid+1, right);
    merge(nums, left, mid+1, right);
}

//合并两段已经排序的子数组：nums[left, mid-1], nums[mid, right]
void merge(vector<int> &nums, int left, int mid, int right) {
    int size = right - left + 1;
    int oriLeft = left; //记录左边子数组的起始下标
    int oriMid = mid;  //记录右边子数组的起始下标
    int k = 0;
    vector<int> tempVec(size);
    while (left < oriMid && mid <= right)
    {
        tempVec[k++] = (nums[left] <= nums[mid]) ? nums[left++] : nums[mid++];
    }
    
    //检查两个子数组里哪个还有剩余的数据，如果left=oriMid，说明左边子数组已经全部入列
    int start = (left == oriMid) ? mid : left;
    while (k < size)
    {
        tempVec[k++] = nums[start++];
    }
    
    //将临时数组里已经排好序的元素写回到原数组中，完成合并
    for (k = 0; k < size; ++k)
    {
        nums[oriLeft + k] = tempVec[k];
    }
}

void mergesortTest() {
    vector<int> vec = createRandomVec(RANDOM_VECTOR_SIZE, RANDOM_INT_MAX);
    recordRunTime();
    mergesort(vec);
    recordRunTime(true);
}
