#include "Sort.hpp"
#include "../common/common.hpp"

int insertsort(vector<int> &nums) {
    cout<<"***********[Begin]Insert sort**********"<<endl;
    printVec(nums);
    int size = nums.size();
    if (size < 2) return 0;

    for (int i = 1; i < size; ++i)
    {
        int temp = nums[i]; //保存当前要插入的数据
        int j = i - 1;
        for ( ; j >= 0 ; --j) //从后向前遍历已排序区域查找插入位置，同时搬移元素
        {
            if (nums[j] > temp) {
                nums[j+1] = nums[j];
            } else {
                //如果元素相等则不变换已排序元素的位置，保证稳定性
                break;
            }
        }
        //j+1就是最终插入位置
        nums[j+1] = temp;
    }
    
    printVec(nums);
    cout<<"***********[End]Insert sort**********"<<endl;
    return 0;
}

void insertsortTest() {
    vector<int> vec = createRandomVec(RANDOM_VECTOR_SIZE, RANDOM_INT_MAX);
    recordRunTime();
    insertsort(vec);
    recordRunTime(true);
}
