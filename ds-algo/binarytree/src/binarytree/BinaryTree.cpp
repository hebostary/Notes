#include "BinaryTree.hpp"

using namespace std;

int BinaryTreeTest() {
    cout << "*****[Enter]BinaryTree Test*****" << endl;
    BinaryTree<string> t1;
    t1.Insert("E");
    t1.Insert("A");
    t1.Insert("B");
    t1.Insert("F");
    t1.Insert("C");
    t1.Insert("D");
    t1.PreOrder();
    t1.InOrder();
    t1.PostOrder();
    cout << "*****[Exit]BinaryTree Test*****" << endl;
    return 0;
}
