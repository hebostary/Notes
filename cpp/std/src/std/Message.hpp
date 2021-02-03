#ifndef MESSAGE_H_
#define MESSAGE_H_

#include "../common/common.hpp"

/*
Chapter 13.4
*/

class Folder;
class Message
{
    friend class Folder;
    friend void swap(Message &lhs, Message &rhs);
public:
    //folders被隐式初始化为空集合
    explicit Message(const string &str = "") :
        contents(str) {};
    //拷贝控制成员
    Message(const Message&);
    //拷贝赋值
    Message& operator=(const Message&);
    //析构
    ~Message();

    //从给定Folder集合中添加/删除本消息
    void save(Folder&);
    void remove(Folder&);

private: 
    string contents;    //实际消息文本
    set<Folder*> folders; //包含本消息的folder
    //将本消息添加到指向参数的Folder中去
    void add_to_Folders(const Message&);
    //从folders中的每个Folder中删除本消息
    void remove_from_Folders();
};

void swap(Message &lhs, Message &rhs);

class Folder
{
    friend class Message;
public:
    Folder(/* args */);
    ~Folder();

    //将指向的消息添加到本目录中
    void addMsg(Message*);
    //从本目录中删除指向的消息
    void remMsg(Message*);

private:
    set<Message*> messages;
};

#endif