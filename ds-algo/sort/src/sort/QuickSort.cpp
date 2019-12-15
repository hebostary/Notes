#include "Sort.hpp"
#include "../common/common.hpp"

void quicksort_sep(vector<int> &nums, int left, int right);
int partition(vector<int> &nums, int left, int right);

int quicksort(vector<int> &nums) {
    cout<<"***********[Begin]Quick sort**********"<<endl;
    printVec(nums);
    int size = nums.size();
    if (size < 2) return 0;

    quicksort_sep(nums, 0, size - 1);
    
    printVec(nums);
    cout<<"***********[End]Quick sort**********"<<endl;
    return 0;
}

void quicksort_sep(vector<int> &nums, int left, int right) {
    //递归终止条件
    if(left >= right) return;

    int p = partition(nums, left, right);
    quicksort_sep(nums, left, p - 1);
    quicksort_sep(nums, p + 1, right);
}

//对nums[left, right]区间的元素以nums[right]作为临界值pivot进行分区
//左边分区的元素都小于pivot，右边分区的元素都大于pivot
int partition(vector<int> &nums, int left, int right) {
    int pIndex = left;
    int pivot = nums[right];
    for (int i = left; i < right; ++i)
    {
        //其实这是一个双指针的应用，pIndex始终指向左边分区最后一个大于pivot的元素，
        //也是需要被交换到右边分区的元素，所以i往后遍历时，一旦发现比pivot小的值,
        //则和nums[pIndex]进行交换
        if(nums[i] < pivot) {
            swap(nums[pIndex], nums[i]);
            pIndex += 1;
        }
    }

    swap(nums[pIndex], nums[right]);
    return pIndex;  
}

void quicksortTest() {
    vector<int> vec = createRandomVec(RANDOM_VECTOR_SIZE, RANDOM_INT_MAX);
    recordRunTime();
    quicksort(vec);
    recordRunTime(true);
}
