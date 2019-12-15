#include "Sort.hpp"
#include "../common/common.hpp"

int selectsort(vector<int> &nums) {
    cout<<"***********[Begin]Select sort**********"<<endl;
    printVec(nums);
    int size = nums.size();
    if (size < 2) return 0;

    for (int i = 0; i < size - 1; ++i)
    {
        int minIndex = i + 1;
        for (int j = i + 2 ; j < size; ++j)
        {
            //找到未排序区里最小元素的索引
            if(nums[j] < nums[minIndex]) minIndex = j;
        }
        
        //将上面找到的元素交换到排序区的尾部
        if(nums[i] > nums[minIndex]) swap(nums[i], nums[minIndex]);
    }
    
    printVec(nums);
    cout<<"***********[End]Select sort**********"<<endl;
    return 0;
}

void selectsortTest() {
    vector<int> vec = createRandomVec(RANDOM_VECTOR_SIZE, RANDOM_INT_MAX);
    recordRunTime();
    selectsort(vec);
    recordRunTime(true);
}
