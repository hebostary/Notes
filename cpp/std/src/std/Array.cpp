#include "Array.hpp"

const int LEN = 3;
int g_a[LEN]; //全局数组元素会被初始化为默认值

void TestArray()
{
    cout<<"***********[Begin] "<<__FUNCTION__<<"**********"<<endl;
    //1.数组的长度只能是常量或者是常量表达式
    int a1[LEN]; //局部数组元素不会自动初始化
    int a2[3] = { 30 }; //首元素被初始化为指定值，其余元素初始化为默认值
    int a3[3] = { 11, 22, 33};
    //int a4[n] = { 3 }; //variable "a4" may not be initializedC/C++(145)
    cout<<__FUNCTION__<<" a1[2]="<<a1[2]<<endl;//a1[2]=4197413
    cout<<__FUNCTION__<<" a2[0]="<< a2[0] <<", a2[2]="<<a2[2]<<endl;// a2[0]=30, a2[2]=0
    cout<<__FUNCTION__<<" a3[2]="<<a3[2]<<endl;//33
    cout<<__FUNCTION__<<" g_a[2]="<<g_a[2]<<endl;//g_a[2]=0

    //2. 探索数组指针
    //对数组名取地址不会发生指针转换，得到的就是数组地址，
    //也可以叫做数组指针，这个指针的步长是所有元素的长度和，这里是12
    cout << __FUNCTION__ << " &a3: " << &a3 << endl;//0x7ffe8bd1c500
    cout << __FUNCTION__ << " *a3: " << *a3 << endl;//0x7ffe8bd1c500
    cout << __FUNCTION__ << " (*a3)+1: " << (*a3)+1 << endl;//12，单纯的取值加法运算
    cout << __FUNCTION__ << " *(a3+1): " << *(a3+1) << endl;//22，等价于a3[1]
    //&a3[0]：&的优先级不是最高的，所以&的直接作用对象是 a3[0] 这个子表达式，
    //此时a3转换为指针（int指针）后进行运算，得到数组的第一个元素，&作用于这个元素后取得其地址，
    //得到的最终结果指向数组首元素的指针。尽管这里的地址和上面的地址相同，但是它们有本质的区别
    //所谓数组的下标本质是指针运算
    cout << __FUNCTION__ << " &a3[0]: " << &a3[0] << endl;//0x7ffe8bd1c500
    //a3[1]等价于int指针加1，得到数组的第二个元素
    cout << __FUNCTION__ << " &a3[1]: " << &a3[1] << endl;//0x7ffe8bd1c504
    //发生了指针转换
    cout << __FUNCTION__ << " (&a3)[0]: " << (&a3)[0] << endl;//0x7ffe8bd1c500
    //这里实际上是在数组指针上进行运算，指针+1即移动12个字节
    //注意，这里已经发生了内存越界访问，有很大的安全隐患
    cout << __FUNCTION__ << " (&a3)[1]: " << (&a3)[1] << endl;//0x7ffe8bd1c50c

    //没有发生转换，所以得到的是数组的大小
    cout << __FUNCTION__ << " size of a3: " << sizeof(a3) <<endl;//12
    //a3被转换为指针，所以并不是数组a3与0相加，而是转换后得到的指针在参与运算
    cout << __FUNCTION__ << " size of a3+0: " << sizeof(a3+0) <<endl;//8
    TestArrayAsParam(a3);

    //3.动态数组
    int n = 10;
    int *pia1 = new int[n]; //每个元素都没有初始化
    int *pia2 = new int[n]();  // 每个元素初始化为默认值，这里是0
    //如果元素是class，始终都会调用默认构造函数进行初始化
    string *psa1 = new string[n];//调用每个元素的默认构造函数
    string *psa2 = new string[n]();//调用每个元素的默认构造函数

    //4。二维数组
    int value1[9][9];//未初始化
    int value2[9][9] = {{1,1},{2}}; //value2[0][0,1]和value2[1][0]的值初始化，其他初始化为0

    int m = 10;
    const int n1 = 10;
    int (*value3)[n1] = new int[m][n1];//n1必须是常量表达式，未初始化
    int** value4 = new int* [m];//未初始化

    cout<<"***********[End] "<<__FUNCTION__<<"**********"<<endl;
}

void TestArrayAsParam(int a[])
{
    //sizeof的值始终是8，数组作为参数时，编译器自动转换为指针，此处为指向int的指针
    cout << __FUNCTION__ << " size of array param: " << sizeof(a) <<endl;
    cout << __FUNCTION__ << " address of array param: " << a <<endl;//0x7ffe2f82d570
    cout << __FUNCTION__ << " (a+2)= " << a+2 << endl;//0x7ffe2f82d578，步长是int型size，即4个字节
    cout << __FUNCTION__ << " *(a+2)= " << *(a+2) << endl;//33
    cout << __FUNCTION__ << " a[2]= " << a[2] << endl;//33
}
