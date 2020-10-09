#include "common.hpp"

const int GUID_LEN = 16;

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

string createUUID()
{
    char buf[GUID_LEN] = { 0 };

    uuid_t uu;   
    uuid_generate( uu );   

    int32_t index = 0;
    for (int32_t i = 0; i < 16; i++)
    {
        int32_t len = i < 15 ? 
            sprintf(buf + index, "%02X-", uu[i]) : 
            sprintf(buf + index, "%02X", uu[i]);
        if(len < 0 )
            return std::move(std::string(""));
        index += len;
    }

    return std::move(std::string(buf));
}

void printIntArray(int a[])
{
    size_t size = sizeof(a);//始终都是8，指针的长度，数组作为参数传递时，
    //编译器自动将a调整为指向int的指针类型
    cout << __FUNCTION__ << " array size: " << size << endl;
}
