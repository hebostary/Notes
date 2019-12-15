#include "Sort.hpp"
#include "../common/common.hpp"

int bucketsort(vector<int> &nums) {
    cout<<"***********[Begin]Bucket sort**********"<<endl;
    printVec(nums);
    int size = nums.size();
    if (size < 2) return 0;

    int max = nums[0];
    for (auto ite = nums.begin(); ite != nums.end(); ++ite)
    {
        if(*ite > max) max = *ite;
    }

    //TODO
    
    
    printVec(nums);
    cout<<"***********[End]Bucket sort**********"<<endl;
    return 0;
}

void bucketsortTest() {
    vector<int> vec = createRandomVec(RANDOM_VECTOR_SIZE, RANDOM_INT_MAX);
    recordRunTime();
    bucketsort(vec);
    recordRunTime(true);
}
