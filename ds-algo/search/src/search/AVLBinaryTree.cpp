#include "AVLBinaryTree.hpp"
#include "Search.hpp"

using namespace std;

int AVLBinaryTreeTest()
{
    cout << "*****[Enter]AVLBinaryTree Test*****" << endl;
    AVLBinaryTree<string> t1;
    string array[10] = {"A", "B", "C", "D", "E", "F", "H", "I", "J", "K"};
    
    for(size_t i = 0; i < 10; i++)
    {
        t1.Insert(array[i]);
    }
    /*
                         D
                       /   \
                      B     I
                    /  \   /  \
                   A   C  F   J
                         / \   \
                        E   H   K
    */

    t1.InOrder(); 
    t1.PreOrder();
    cout << "*****[Exit]AVLBinaryTree Test*****" << endl;
    return 0;

}
