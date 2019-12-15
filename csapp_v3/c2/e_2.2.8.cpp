#include <iostream>

using namespace std;

float sum_elements(float a[], unsigned len)
{
    int i;
    float re = 0;
    cout<<(0 - 1)<<endl; //-1
    cout<<(len - (unsigned)1)<<endl; //4294967295 -> 11111111 11111111 11111111 11111111 -> -1(complement)
    cout<<(len - 1)<<endl; //4294967295 
    
    for(i = 0; i <= len-1; i++)
    {
        re += a[i]; //Segmentation fault
    }
    return re;
}

int main()
{
    float a[1];
    cout<<sum_elements(a, 0)<<endl;
}
