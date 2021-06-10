#include "FunctionObject.hpp"

//普通函数
int add(int i, int j)
{
    return i + j;
}

void TestFuncitonObject()
{
    ENTER_FUNC

    // function<int(int, int)>表示可以接受
    //两个int，返回一个int的可调用对象
    map<string, function<int(int, int)>> binops = {
        {"+", add},                                 //函数指针
        {"-", std::minus<int>()},                   //标准库函数对象
        {"/", divide()},                            //用户自定义的函数对象
        {"*", [](int i, int j) { return i*j; }},    //未命名的lambda对象
        {"%", mod}                                  //命名了的lambda对象
    };

    int a = 10, b = 5;
    cout << "10 + 5 = " << binops["+"](a, b) << endl;
    cout << "10 - 5 = " << binops["-"](a, b) << endl;
    cout << "10 / 5 = " << binops["/"](a, b) << endl;
    cout << "10 * 5 = " << binops["*"](a, b) << endl;
    cout << "10 % 5 = " << binops["%"](a, b) << endl;

    EIXT_FUNC
}