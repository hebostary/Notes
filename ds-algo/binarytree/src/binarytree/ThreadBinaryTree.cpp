#include "ThreadBinaryTree.hpp"

using namespace std;

int ThreadBinaryTreeTest() {
    cout << "*****[Enter]ThreadBinaryTree Test*****" << endl;
    ThreadBinaryTree<string> t1;
    t1.Insert("E");
    t1.Insert("A");
    t1.Insert("B");
    t1.Insert("F");
    t1.Insert("D");
    t1.Insert("C");
    t1.InOrderToChain();
    t1.InOrderThreadChain();
    cout << "*****[Exit]ThreadBinaryTree Test*****" << endl;
    return 0;
}
