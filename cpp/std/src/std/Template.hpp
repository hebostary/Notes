#ifndef TEMPLATE_H_
#define TEMPLATE_H_

#include "../common/common.hpp"

void TestTemplate();
void TestMemberTemplate();

class Base1 {
public:
    virtual string getName() { return "Base1"; };
};
class Derived1 : public Base1 {
public:
    virtual string getName() { return "Derived1"; }
};

class Base2 {
public:
    virtual string getName() { return "Base2"; }
};
class Derived2 : public Base2 {
public:
    virtual string getName() { return "Derived2"; }
};

template<class T1, class T2>
class Pair
{
public:
    T1 first;
    T2 second;

    Pair() : first(T1()), second(T2()) {}
    Pair(const T1& a, const T2& b) : first(a), second(b) { }

    //成员模板
    template<class U1, class U2>
    Pair(const Pair<U1, U2>& p) : first(p.first), second(p.second) { }

/*
TODO:如果把first和second数据放到private区，并用public方法访问，
则下面这个成员模板的构造函数始终无法编译通过
    template<class U1, class U2>
    Pair(const Pair<U1, U2>& p) 
        : first((U1)p.getFirst()), second((U2)p.getSecond()) { }

    T1& getFirst() { return first; }
    T2& getSecond() { return second; }
    */
};


void myPrint();

//数量不定的模板参数
template<typename T, typename... Types>
void myPrint(const T& firstArg, const Types&... args)
{
    cout << firstArg <<", size of args:" << sizeof...(args) << endl;
    myPrint(args...);
}


#endif