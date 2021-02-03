#include "common.hpp"

const int GUID_LEN = 16;

static clock_t beginRunTime = clock();


void EnterFunc(string msg)
{
    cout<<"|-------------------[BEGIN] "<<msg<<" ---------------------|"<<endl;
}
void ExitFunc(string msg)
{
    cout<<"|-------------------[END] "<<msg<<" -----------------------|"<<endl;
}

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

#if defined(_WIN64)
string createUUID()
{
    return "AAAA0000BBBBBAAA";
}
#else
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
#endif
