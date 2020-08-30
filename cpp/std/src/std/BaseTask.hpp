#ifndef BASETASK_H_
#define BASETASK_H_

#include "../common/common.hpp"

using namespace std;

class BaseTask
{
private:
    string m_id;
    string m_type; 

public:
    BaseTask();
    BaseTask(string id, std::string type);
    ~BaseTask();

    string getID() { return m_id; }
    string getType() { return m_type; }

    virtual void printTask();
    virtual string serialize();

    //转换函数，编译器可以通过它将BaseTask对象转换成string
    //编译器允许不用写转换函数的返回的类型，因为编译器可以通过
    //转化函数的名字自动推导出返回类型
    operator string() { return serialize(); }
    operator int() { return (int)serialize().size(); }
};

#endif