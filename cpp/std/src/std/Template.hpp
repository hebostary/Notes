#ifndef TEMPLATE_H_
#define TEMPLATE_H_

#include "../common/common.hpp"
#include <memory>
#include <type_traits>

void TestTemplate();
void testFunctionTemplate();
void testClassTemplate();
void testMemberTemplate();

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

//在模板中使用尾置返回类型
//编译器遇到函数的参数列表之前，beg都是不存在的
//为了定义此函数，必须使用尾置返回类型
template<typename It>
auto fcn1(It beg, It end) -> decltype(*beg)
{
    //处理序列
    return *beg;//解引用运算符符返回一个左值，
    //因此decltype推断的类型为beg表示元素的类型的引用
}

//使用类型转换的标准库模板类
//remove_reference是一个类型转换模板，用以获得元素类型
//它包含一个模板参数和一个名为type的public类型成员
//remove_reference<string&>::type就是string
template<typename It>
auto fcn2(It beg, It end) -> 
    typename remove_reference<decltype(*beg)>::type
{
    //处理序列
    return *beg;//直接返回序列首元素的拷贝
}

/*
    类模板
*/
template <typename T>
class Blob {
public:
    typedef T value_type;
    typedef typename vector<T>::size_type size_type;

    //构造函数
    Blob() : data(make_shared<vector<value_type>>()) { }
    Blob(initializer_list<T> il) : data(make_shared<vector<value_type>>(il)) { }

    size_type size() { return data->size(); }
    bool empty() { return data->empty(); }

    //添加和删除元素
    void push_back(const value_type &t) { data->push_back(t); }
    void push_back(value_type &&t) { data->push_back(std::move(t)); }
    void pop_back();
    
    //元素访问
    T& front();
    T& back();
    T& operator[](size_type i);

private:
    shared_ptr<vector<T>> data;
    //如果data[i]不合法，抛出一个异常
    void check(size_type i, const string &msg) const;

};

template <typename T>
void Blob<T>::check(size_type i, const string &msg ) const
{
    if ( i >= data->size() )
    {
        throw out_of_range(msg);
    }
}

template <typename T>
void Blob<T>::pop_back()
{
    check(0, "pop_back on empty Blob");
    data->pop_back();
}

template <typename T>
T& Blob<T>::front()
{
    check(0, "front on empty Blob");
    return data->front();
}

template <typename T>
T& Blob<T>::back()
{
    check(0, "back on empty Blob");
    return data->back();
}

template <typename T>
T& Blob<T>::operator[](size_type i)
{
    check(i, "index out of range.");
    return *data[i];
}

/*
    类模板： 成员模板
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