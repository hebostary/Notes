#include <iostream>
#include <climits>

using namespace std;

/*
Determine whether arguments can be multiplied without overflow
Return:
    0: with overflow
    1: without overflow
*/

int tmult_ok(int x, int y)
{
    int p = x * y;
    
    return !x || p/x == y;
}

int tmult_ok_way2(int x, int y)
{
    int64_t p = (int64_t)x * y; //这里做类型强转很重要，否则会先用32位值计算（可能会溢出），然后再做符号扩展
    
    return p == (int)p;
    //or
    //return (p & 0xFFFFFFFF00000000) == 0x0000000000000000;
}

int main()
{
    int x = INT_MIN;
    int y = -10;

    cout << (tmult_ok(10,10000)==1?"without overflow":"with overflow") << endl;
    cout << (tmult_ok(x,y)==1?"without overflow":"with overflow") << endl;
    cout << (tmult_ok_way2(10,10000)==1?"without overflow":"with overflow") << endl;
    cout << (tmult_ok_way2(x,y)==1?"without overflow":"with overflow") << endl;
}
