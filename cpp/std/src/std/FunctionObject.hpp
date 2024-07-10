#ifndef FUNCTION_OBJECT_H
#define FUNCTION_OBJECT_H

#include "../common/common.hpp"
#include <functional>

//普通函数
int add(int i, int j);

//命名lambda表达式
//auto mod = [](int i, int j) { return i % j; };

//重载了调用运算符的可调用对象
struct divide
{
    int operator()(int denominator, int divisor)
    {
        return denominator / divisor;
    }
};

void TestFuncitonObject();

#endif