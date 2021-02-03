#include "Message.hpp"

Message::Message(const Message& msg) :
    contents(msg.contents), folders(msg.folders)
{
    add_to_Folders(msg);
}

Message::~Message()
{
    //确认没有任何Folder保存正在销毁的Message指针
    remove_from_Folders();
}

Message& Message::operator=(const Message &rhs)
{
    //先从左侧运算对象的folders中删除当前Message的指针
    remove_from_Folders();
    //拷贝消息内容
    contents = rhs.contents;
    folders = rhs.folders;
    //将本Message添加到那些Fodler中
    add_to_Folders(rhs);
    return *this;
}

void Message::save(Folder &f)
{
    folders.insert(&f);
    f.addMsg(this);
}

void Message::remove(Folder &f)
{
    folders.erase(&f);
    f.remMsg(this);
}

void Message::add_to_Folders(const Message& msg)
{
    for (auto f : msg.folders)
    {
        f->addMsg(this);
    }
}

void Message::remove_from_Folders()
{
    for (auto f : folders)
    {
        f->remMsg(this);
    }
}

void swap(Message &lhs, Message &rhs)
{
    using std::swap;
    //先将每个消息的指针从它原来所在folder中删除
    for (auto f : lhs.folders) {
        f->remMsg(&lhs);
    }
    for (auto f : rhs.folders) {
        f->remMsg(&rhs);
    }

    swap(lhs.contents, rhs.contents);
    swap(lhs.folders, rhs.folders);

    //将每个Message添加到它的新的Folder中去
    for (auto f : lhs.folders) {
        f->addMsg(&lhs);
    }
    for (auto f : rhs.folders) {
        f->addMsg(&rhs);
    }
}


void Folder::addMsg(Message* msg)
{
    messages.insert(msg);
}

void Folder::remMsg(Message* msg)
{
    messages.erase(msg);
}