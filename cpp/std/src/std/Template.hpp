#ifndef TEMPLATE_H_
#define TEMPLATE_H_

#include "../common/common.hpp"

void TestTemplate();
void TestMemberTemplate();

/*
    函数模板
*/
void myPrint();

//数量不定的模板参数
template<typename T, typename... Types>
void myPrint(const T& firstArg, const Types&... args)
{
    cout << firstArg <<", size of args:" << sizeof...(args) << endl;
    myPrint(args...);
}


//非类型模板参数
//一个非类型模板参数表示一个值而不是一个类型
//当一个模板被实例化时，非类型参数被一个用户提供的或编译器推断出的值所替代
//非类型模板参数的模板实参必须是常量表达式
template<unsigned N, unsigned M>
inline int compare(const char (&p1)[N], const char (&p2)[M])
{
    return strcmp(p1, p2);
}

//打印任意类型的数组
//同时包含了类型模板参数和非类型模板参数
template<typename T, unsigned N>
inline void printArray(const T (&p)[N])
{
    for (int i = 0; i != N; i++) {
        cout << p[i] << " ";
    }
    cout << endl;
}

//实现类似于标准库find算法的模板函数
template<typename T1, typename T2>
T1 myfind(T1 begin, T1 end, T2 value)
{
    while (begin != end) {
        if ( *begin == value ) {
            break;
        }
        begin++;
    }

    if (begin == end) {
        cout << "Not finded value:" << value << endl;
    } else {
        cout << "Finded value:" << value << endl;
    }
    
    return begin;
}



/*
    类模板
*/
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


#endif