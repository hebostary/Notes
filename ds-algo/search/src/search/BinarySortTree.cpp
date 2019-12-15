#include "BinarySortTree.hpp"
#include "Search.hpp"

using namespace std;

int BinarySortTreeTest()
{
    cout << "*****[Enter]BinarySortTree Test*****" << endl;
    cout << "*****[Test1]BinarySortTree Test*****" << endl;
    BinarySortTree<string> t1;
    t1.Insert("E");
    t1.Insert("A");
    t1.Insert("B");
    t1.Insert("F");
    t1.Insert("C");
    t1.Insert("D");

    t1.InOrder();
    t1.Insert("D");
   
    cout <<"Remove B from tree" <<endl; 
    t1.Remove("B");
    t1.InOrder();
    
    cout<<"Insert B into tree"<<endl;
    t1.Insert("B");
    t1.InOrder();

    BinarySortTreeNode<string>* re = t1.Search("B");
    re = t1.Search("J");
   
    t1.Insert("M");
    t1.Insert("N");
    t1.Insert("H");
    t1.Insert("y");
    t1.Insert("L");

    t1.InOrder();
    t1.PreOrder(); 

    cout << "*****[Test2]BinarySortTree Test*****" << endl;
    BinarySortTree<string> t2;
    string array[10] = {"A", "B", "C", "D", "E", "F", "H", "I", "J", "K"};
    
    for(size_t i = 0; i < 10; i++)
    {
        t2.Insert(array[i]);
    }

    t2.InOrder();
    t2.PreOrder(); 
    cout << "*****[Exit]BinarySortTree Test*****" << endl;
    return 0;

}
