#ifndef STR_VEC_H_
#define STR_VEC_H_

#include "../common/common.hpp"

using namespace std;

class StrVec
{
public:
    StrVec() : //alloactor成员进行默认初始化
        elements(nullptr), first_free(nullptr), cap(nullptr) {};
    StrVec(StrVec &&) noexcept;         //移动构造函数
    StrVec(const StrVec&);              //拷贝构造函数
    StrVec& operator=(const StrVec&);   //拷贝赋值运算符
    StrVec& operator=(StrVec&&) noexcept; //移动赋值运算符
    StrVec& operator=(initializer_list<string> il); //序列化赋值
    ~StrVec() { free(); }

    //重载下标运算符
    //下表运算符必须是成员函数
    //如果一个类包含下标运算符，通常会定义下面两个版本
    string& operator[](size_t n) 
        { return elements[n]; } //返回普通引用
    const string& operator[](size_t n) const 
        { return elements[n]; } //返回常量引用，元素无法被修改

    void push_back(const string&); //拷贝版本的push_back
    void push_back(string&&);      //移动版本的push_back
    size_t size() const { return first_free - elements; }
    size_t capacity() const { return cap - elements; }
    string* begin() const { return elements; }
    string* end() const { return first_free; }

private:
    //****需要注意的是：类的声明并不会分配内存空间，因此，类的静态成员需要类内声明，类外定义。
    //并且注意定义尽量不要出现在头文件中，以免造成重复定义。
    static allocator<string> alloc;//分配元素

    //添加元素的函数使用
    void chk_n_alloc() 
        { if ( size() == capacity() ) reallocate(); }
    //工具函数，被拷贝构造函数，赋值运算符和析构函数所使用
    pair<string *, string*> alloc_n_copy(const string*, const string*);
    void free();       //销毁元素并释放内存
    void reallocate(); //重新分配更多内存并拷贝已有元素

    string *elements;  //指向数组首元素的指针
    string *first_free; //指向数组第一个空闲元素的指针
    string *cap;  //指向数组尾后位置的指针
};


//函数对象
class StringPrinter
{
public:
    StringPrinter(ostream& o = cout, char c = '\n')
        : os(o), sep(c) { }
    void operator()(const string& str) const {
        os << str << sep;
    }

private:
    ostream& os;
    char sep;//打印分隔符
};

void TestStrVec();

#endif
