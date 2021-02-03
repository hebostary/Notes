#include "String.hpp"

//我们可以总是将这些成员函数标记为内联函数，
//最终是否成为内联函数由编译器决定，但总之没有坏处
inline String::String(const char* cstr)
{
    if (cstr) 
    {
        m_data = new char[ strlen(cstr) + 1 ];
        strcpy(m_data, cstr);
    } 
    else 
    { //未指定初始值
        m_data = new char[1];
        *m_data = '\0';
    }
}

inline String::String(const String& str)
{
    m_data = new char[ strlen(str.m_data) + 1 ];
    strcpy(m_data, str.m_data);
}

//赋值运算符通常组合了析构函数和构造函数的操作
//  类似析构函数，赋值操作会销毁左侧运算对象
//  类似拷贝构造函数，赋值操作会从右侧运算对象拷贝数据
inline String& String::operator=(const String& str)
{
    //检测自我赋值
    if(this == &str)
    {
        return *this;
    }

    //释放原有空间
    delete[] m_data;
    //申请新的空间
    m_data = new char[ strlen(str.m_data) + 1 ];
    //拷贝数据
    strcpy(m_data, str.m_data);
    return *this;
}

String::~String()
{
    //删除动态分配的字符数组，一定要用delete[]
    delete[] m_data;
}

//重载输出操作符
ostream& operator<<(ostream& os, const String& str)
{
    os << str.get_c_str();
    return os;
}

void TestStringClass()
{
    EnterFunc(__FUNCTION__);
    String s1("Hello hunk");
    cout<<"s1: "<<s1<<endl;

    String s2(s1);
    cout<<"s2: "<<s2<<endl;

    String s3("Hello jack");
    s2 = s3;
    cout<<"s2: "<<s2<<endl;

    int len = 10;
    char* cstr1 = new char[len]();
    for (int i = 0; i < len; i++) cstr1[i] = 'a';
    cout << " string(cstr1): " << string(cstr1) << endl;


    ExitFunc(__FUNCTION__);
}