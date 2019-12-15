#include <iostream>
#include <climits>

using namespace std;

/*
Determine whether arguments can be added without overflow
Return:
    0: with overflow
    1: without overflow
*/

/*
    求补码非的两种方法：
    1. -x = ~x+1 (~x: 对x的所有位取反)
    2. x的位级表示(b1,b2,b3,1,0,0,0,0),将最右边的1左边的所有位取反
        -4(1100) -> 4(0100)
        -8(1000) -> -8(1000)
        7(0111)  -> -7(1001)
*/

int tadd_ok(int x, int y)
{
    int sum = x + y;
    int neg_over = (x < 0) && (y < 0) && (sum > 0); //overflow
    int pos_over = (x > 0) && (y > 0) && (sum < 0); //overflow

    cout<< x << " + " << y << " = " <<sum << endl; //2147483647 + 1 = -2147483648

    return !neg_over && !pos_over;
}

int main()
{
    int x = INT_MAX;
    int y = 1;

    int z = INT_MIN;
    cout << "-(" << x <<") = " << -x <<endl;   //-(2147483647) = -2147483647
    cout << "-(" << y <<") = " << -y << ", ~" << y << "+1 = " << ~y+1 <<endl;   //-(1) = -1, ~1+1 = -1
    /*
    -INT_MIN = INT_MIN
    */
    cout << "-(" << z <<") = " << -z <<endl;   //-(-2147483648) = -2147483648  

    cout << (tadd_ok(x,y)==1?"without overflow":"with overflow") << endl;
}
