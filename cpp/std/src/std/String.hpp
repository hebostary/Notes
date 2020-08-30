#ifndef STRING_H_
#define STRING_H_

#include "../common/common.hpp"

using namespace std;

class String
{
public:
    String(const char* cstr = 0);//构造函数
    String(const String& str);//拷贝构造函数
    String& operator=(const String& str);//拷贝赋值函数
    ~String();//析构函数
    char* get_c_str() const { return m_data; }
private:
    char* m_data;
};

//测试函数
void TestStringClass();

#endif
