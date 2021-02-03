#include "Template.hpp"

void myPrint()
{
    cout << "END..." << endl;
}

void TestTemplate()
{
    EnterFunc(__FUNCTION__);

    cout<<__cplusplus<<endl;
    TestMemberTemplate();

    myPrint("Hunk", 100, 98.5);

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