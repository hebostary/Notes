#include "common.hpp"

static clock_t beginRunTime = clock();

void recordRunTime(bool finish) {
    clock_t currentTime = clock();
    if (!finish) {
        beginRunTime = currentTime;
    } else {
        double duration = (double)(currentTime - beginRunTime) / CLOCKS_PER_SEC;
        printf("Run time: [ %f ] seconds.\n", duration);
    }
}

vector<int> createRandomVec(int size, int max) {
    vector<int> vec(size);
    int i = 0;
    srand(unsigned(time(NULL)));
    while (i < size)
    {
        vec[i++] = rand() % max+1;
    }

    return vec;
}
