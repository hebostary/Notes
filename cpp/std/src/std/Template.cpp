#include "Template.hpp"

void myPrint()
{
    cout << "END..." << endl;
}

void TestTemplate()
{
    cout<<"\n***********[Begin] TestTemplate**********"<<endl;
    cout<<__cplusplus<<endl;
    TestMemberTemplate();

    myPrint("Hunk", 100, 98.5);
    cout<<"***********[End] TestTemplate**********\n"<<endl;
}

void TestMemberTemplate()
{
    cout<<"\n***********[Begin] TestMemberTemplate**********"<<endl;
    Pair<Derived1, Derived2> p;
    Pair<Base1, Base2> p2(p);
    cout << p.first.getName()<<endl;
    cout << p2.first.getName()<<endl;
    cout<<"***********[End] TestMemberTemplate**********\n"<<endl;
}