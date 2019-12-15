#include <iostream>

using namespace std;

int main()
{
    int x = 53191;
    short sx = (short)x;
    int y = sx;

    cout<<sx<<", "<<y<<endl; //-12345, -12345
}
