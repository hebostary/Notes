#include "StrVec.hpp"

//类的静态成员类内声明，类外定义和初始化
allocator<string> StrVec::alloc;

//移动操作不应该抛出异常
//与拷贝构造函数不同，移动构造函数不分配任何新内存：
//  它接管给定的StrVec中的内存
StrVec::StrVec(StrVec &&sv) noexcept
    //成员初始化器接管sv中的资源
    : elements(sv.elements), first_free(sv.first_free), cap(sv.cap)
{
    cout << "Calling move constructor: StrVec::StrVec(StrVec &&sv) noexcept" << endl;
    //令sv进入这样一种状态：对其运行析构函数是安全的
    //注意：StrVec的析构函数在first_free上调用deallocate
    sv.elements = sv.first_free = sv.cap = nullptr;
}

StrVec::StrVec(const StrVec& sv)
{
    cout << "Calling copy constructor: StrVec::StrVec(const StrVec& sv) " << endl;
    auto newdata = alloc_n_copy(sv.begin(), sv.end());
    elements = newdata.first;
    first_free = cap = newdata.second;
}

StrVec& StrVec::operator=(const StrVec& rhs)
{
    cout << "Calling copy assignment: StrVec& StrVec::operator=(StrVec& rhs) " << endl;
    //分配新的内存并并拷贝元素
    auto data = alloc_n_copy(rhs.begin(), rhs.end());
    //释放本StrVec原来分配的内存
    free();
    elements = data.first;
    first_free = cap = data.second;
    return *this;
}

StrVec& StrVec::operator=(initializer_list<string> il)
{
    cout << "Calling initializer list assignment: StrVec& StrVec::operator=(initializer_list<string>& il) " << endl;
    auto data = alloc_n_copy(il.begin(), il.end());
    free();  //销毁对象中的元素并释放内存空间
    elements = data.first;
    first_free = cap = data.second;
    return *this;
}

StrVec& StrVec::operator=(StrVec&& rhs) noexcept
{
    cout << "Calling move assignment: StrVec& StrVec::operator=(StrVec&& rhs) " << endl;
    if (this != &rhs)
    {
        free(); //释放原有分配的元素空间
        elements = rhs.elements; //从rhs接管资源
        first_free = rhs.first_free;
        cap = rhs.cap;

        rhs.elements = rhs.first_free = rhs.cap = nullptr;
    }
    return *this;
}

//注意拷贝版本和下面的移动版本的参数类型，一个是const T&，一个是T&&
void StrVec::push_back(const string& s)
{
    cout << "Calling copy push_back: StrVec::push_back(const string& s)" << endl;
    chk_n_alloc(); //确保有空间容纳新元素
    //在first_free指向的元素中构造s的副本
    alloc.construct(first_free++, s);
}

void StrVec::push_back(string&& s)
{
    cout << "Calling move push_back: StrVec::push_back(string&& s)" << endl;
    chk_n_alloc();//确保有新空间容纳新元素
    //construct函数使用其第二个和随后的实参的类型来确定使用哪个构造函数
    //这里传递给construct的第二个参数是一个右值引用string&&
    //因此，会使用string的移动构造函数来构造新元素
    alloc.construct(first_free++, std::move(s));
}

//alloc_n_copy函数负责分配足够的内存来保存给定范围的元素，并将这些元素拷贝到新的内存中
pair<string*, string*> 
StrVec::alloc_n_copy(const string *b, const string *e)
{
    //分配空间保存黑顶范围中的元素
    auto data = alloc.allocate(e - b);
    //初始化并返回一个pair
    //uninitialized_copy负责将b->e中的元素拷贝到data指向的（未初始化的）内存中，
    //并返回最后一个构造元素之后的位置
    return {data, uninitialized_copy(b, e, data)};
}

void StrVec::free()
{
    if ( elements )
    {
        //逆序销毁旧元素
        for(auto p = first_free; p != elements; /*空*/)
        {
            //destroy会运行string的析构函数释放string自己分配的内存空间
            alloc.destroy(--p);
        }
        //deallocate释放本StrVec对象分配的内存空间
        //传递给deallocate的指针必须是之前某次allocate调用所返回的指针，所以上面先检查是否为空
        alloc.deallocate(elements, cap - elements);
    }
    
}

void StrVec::reallocate()
{
    cout << "StrVec::reallocate(), current size: " << size() << endl;
    //将分配当前大小两倍的内存空间
    auto newcapacity = size() ? size() * 2 : 1;
    //分配新的未初始化内存
    auto newdata = alloc.allocate(newcapacity);

    auto dest = newdata;  //指向新数组中下一个空闲位置
    auto elem = elements; //指向旧数组中的下一个元素

    //移动旧数组中的元素到新数组中
    for(size_t i = 0; i != size(); ++i) {
        //****调用move的返回结果会令construct使用string的移动构造函数
        //使用移动构造函数，这些string管理的内存将不会被拷贝
        //相反，构造的每个string都会从elem指向的string哪里接管内存的所有权
        //move后，我们不知道旧的StrVec内存中的string包含什么值，
        //但是可以保证对它们执行string的析构函数是安全的
        alloc.construct(dest++, move(*elem++));
    }
    //这里的移动操作也可以写成下面这样：
    //make_move_iterator函数将一个普通迭代器转换为一个移动迭代器****
    //移动迭代器通过改变给定迭代器的解引用运算符的行为来适配此迭代器
    //uninitialized_copy使用解引用运算符提取输入序列的每个元素，
    //并调用construct将元素‘拷贝’到目的位置
    //因为这里传递的时移动迭代器，解引用运算符生成的是右值引用
    //意味着construct将使用移动构造函数来构造元素，减少内存分配的开销
    /*
    auto last = uninitialized_copy(make_move_iterator(begin()),
                            make_move_iterator(end()),
                            dest);
    elements = dest;
    first_free = last;
    */


    free();//释放旧数组

    //更新指针
    elements = newdata;
    first_free = dest;
    cap = elements + newcapacity;
}


void TestStrVec()
{
    ENTER_FUNC

    StrVec sv;
    cout << "sv size: " << sv.size() << ", cap: " << sv.capacity() << endl;
    sv.push_back("hello");
    cout << "sv size: " << sv.size() << ", cap: " << sv.capacity() << endl;
    sv.push_back("jack");
    cout << "sv size: " << sv.size() << ", cap: " << sv.capacity() << endl;
    sv.push_back("hunk");
    sv.push_back("bob");
    sv.push_back("jim");
    cout << "sv size: " << sv.size() << ", cap: " << sv.capacity() << endl;

    StrVec sv1(sv);
    cout << "sv1 size: " << sv1.size() << ", cap: " << sv1.capacity() << endl;

    StrVec sv2 = sv1;
    cout << "sv2 size: " << sv2.size() << ", cap: " << sv2.capacity() << endl;

    //测试右值引用
    //std::move返回一个右值，因此使用移动赋值
    StrVec sv3 = std::move(sv2);
    sv3.push_back("amos"); 
    cout << "sv2 size: " << sv2.size() << ", cap: " << sv2.capacity() << endl;//sv2 size: 0, cap: 0
    cout << "sv3 size: " << sv3.size() << ", cap: " << sv3.capacity() << endl;//sv3 size: 6, cap: 10

    string str = "last string";
    //这些调用的差别在于实参是一个左值还是一个右值
    sv3.push_back(str);  //调用StrVec::push_back(const string& s)
    sv3.push_back("jim"); //调用Calling move push_back: StrVec::push_back(string&& s)
    
    //测试序列化赋值
    sv3 = {"monkey", "snake", "pig"};

    //测试下标运算符重载
    const StrVec sv4 = sv3;
    sv3[1] = "cat";
    //sv4[1] = "cat"; //错误，不能为const修饰的sv4元素赋值

    //测试函数调用运算符重载
    //第三个参数是StringPrinter的一个临时对象，程序调用for_each时
    //将会把sv3中的每个元素依次作为参数调用这个临时对象完成打印
    for_each(sv3.begin(), sv3.end(), StringPrinter(cout, ','));

    EIXT_FUNC
}