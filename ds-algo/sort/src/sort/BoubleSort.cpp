#include "Sort.hpp"
#include "../common/common.hpp"

int boublesort(vector<int> &nums) {
    cout<<"***********[Begin]Bouble sort**********"<<endl;
    printVec(nums);
    int size = nums.size();
    if (size < 2) return 0;

    for (int i = size - 1; i >= 0; --i)
    {
        bool flag = false;
        for (int j = 0; j < i; ++j)
        {
            if (nums[j] > nums[j+1]) {
                swap(nums[j], nums[j+1]);
                flag = true;
            }
        }

        //没有数据交换，说明数据已经是有序的，提前退出
        if (!flag) break;
    }

    printVec(nums);
    cout<<"***********[End]Bouble sort**********"<<endl;
    return 0;
}

void boublesortTest() {
    vector<int> vec = createRandomVec(RANDOM_VECTOR_SIZE, RANDOM_INT_MAX);
    recordRunTime();
    boublesort(vec);
    recordRunTime(true);
}
