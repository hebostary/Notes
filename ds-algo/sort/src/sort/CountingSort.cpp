#include "Sort.hpp"
#include "../common/common.hpp"

int countingsort(vector<int> &nums) {
    cout<<"***********[Begin]Counting sort**********"<<endl;
    printVec(nums);
    int size = nums.size();
    if (size < 2) return 0;

    //找出数组元素的最大值--确定值范围0 - max
    int max = nums[0];
    for (auto ite = nums.begin(); ite != nums.end(); ++ite)
    {
        if (*ite > max) max = *ite;
    }

    //统计原数组中每个值的个数，值就是countVec的下标
    vector<int> countVec(max + 1, 0);
    for (auto ite = nums.begin(); ite != nums.end(); ++ite)
    {
        countVec[*ite] += 1;
    }

    //累加countVec的值后，原数组中每个值v1作为countVec索引，
    //该位置的值v2代表的就是原数组中 <= v1的元素个数
    for (int i = 1; i < max + 1; ++i)
    {
        countVec[i] += countVec[i - 1];
    }
    
    //用于临时存放排序的结果
    vector<int> tempVec(size, 0);
    for (int i = size - 1; i >= 0; --i)
    {
        int v = nums[i];
        tempVec[countVec[v] - 1] = v;
        countVec[v] -= 1;
    }

    //将临时排序结果写回原数组
    for (int i = 0; i < size; ++i)
    {
        nums[i] = tempVec[i];
    }
    
    printVec(nums);
    cout<<"***********[End]Counting sort**********"<<endl;
    return 0;
}

void countingsortTest() {
    vector<int> vec = createRandomVec(RANDOM_VECTOR_SIZE, RANDOM_INT_MAX);
    recordRunTime();
    countingsort(vec);
    recordRunTime(true);
}
