#include "StrVec.hpp"

StrVec::StrVec(const StrVec& sv)
{
    auto newdata = alloc_n_copy(sv.begin(), sv.end());
    elements = newdata.first;
    first_free = cap = newdata.second;
}

StrVec& StrVec::operator=(const StrVec& rhs)
{
    //分配新的内存并并拷贝元素
    auto data = alloc_n_copy(rhs.begin(), rhs.end());
    //释放本StrVec原来分配的内存
    free();
    elements = data.first;
    first_free = cap = data.second;
    return *this;
}

void StrVec::push_back(const string& s)
{
    chk_n_alloc(); //确保有空间容纳新元素
    //在first_free指向的元素中构造s的副本
    alloc.construct(first_free++, s);
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

    free();//释放旧数组

    //更新指针
    elements = newdata;
    first_free = elem;
    cap = elements + newcapacity;
}


void TestStrVec()
{
    EnterFunc(__FUNCTION__);

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

    ExitFunc(__FUNCTION__);
}