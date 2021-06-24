#include "Template.hpp"

void myPrint()
{
    cout << "myPrint END..." << endl;
}

void TestTemplate()
{
    EnterFunc(__FUNCTION__);

    cout<<__cplusplus<<endl;
    TestMemberTemplate();

    myPrint("Hunk", 100, 98.5);

    //编译器会使用字面量的大小来替代模板定义中的N和M，从而实例化模板
    //编译器会在一个字符串的末尾插入一个空字符作为终结符，因此编译器实例化出的compare版本：
    //int compare(const char (&p1)[3], const char (&p1)[5])
    compare("hi", "hunk");

    string str_arr[] = {"I'm", "hunk!"};
    printArray(str_arr);

    vector<int> vec_int = {1, 4, 34};
    myfind(vec_int.begin(), vec_int.end(), 4);
    myfind(vec_int.begin(), vec_int.end(), 6);

    list<string> list_str = {"hunk", "jack", "bob"};
    myfind(list_str.begin(), list_str.end(), "hunk");

    ExitFunc(__FUNCTION__);
}

void TestMemberTemplate()
{
    EnterFunc(__FUNCTION__);

    Pair<Derived1, Derived2> p;
    Pair<Base1, Base2> p2(p);
    cout << p.first.getName()<<endl;
    cout << p2.first.getName()<<endl;

    ExitFunc(__FUNCTION__);
}