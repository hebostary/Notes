#ifndef BINARY_TREE_H
#define BINARY_TREE_H

#include <iostream>

using namespace std;

int BinaryTreeTest();

template<class T>
class BinaryTreeNode {
    //friend void Visit(BinaryTreeNode<T> *);
    //friend void InOrderTraversal(BinaryTreeNode<T> *);
    //friend void PreOrderTraversal(BinaryTreeNode<T> *);
    //friend void PostOrderTraversal(BinaryTreeNode<T> *);
    //friend void LevelOrderTraversal(BinaryTreeNode<T> *);
    //friend void main(void);
public:
    void visit();
public:
    BinaryTreeNode() { m_leftChild = m_rightChild = m_parent = 0; }
    BinaryTreeNode(const T& e) {
        m_data = e;
        m_leftChild = m_rightChild = m_parent = 0;
    }
    BinaryTreeNode(const T& e, BinaryTreeNode *l, BinaryTreeNode *r, BinaryTreeNode *p) {
        m_data = e;
        m_leftChild = l;
        m_rightChild = r;
        m_parent = p;
    }
    void operator=(const BinaryTreeNode<T>& rhs) {
        m_data = rhs.m_data;
        m_leftChild = rhs.m_leftChild;
        m_rightChild = rhs.m_rightChild;
        m_parent = rhs.m_parent;
    }

    T m_data;
    BinaryTreeNode<T> *m_leftChild, *m_rightChild, *m_parent;
};

template<class T>
void BinaryTreeNode<T>::visit()
{
    cout << "Current node data: " << m_data << endl;
}

template<class T>
class BinaryTree
{
public:
    BinaryTree():m_root(NULL){};

    //interfaces
    void Insert (T data);
    //BinaryTreeNode<T>* Search(T data);

    void PreOrder();
    void InOrder();
    void PostOrder();
 
    //BrnaryTreeNode<T>* MinimumNode();
    //BinaryTreeNode<T>* MaxmumNode();

    //T MinmumData();
    //T MaxmumData();

    //void Print();
    //void Remove(T data);

    //void Destory();

private:
    void insert(BinaryTreeNode<T>* &tree, BinaryTreeNode<T>* node);
    //BinaryTreeNode<T>* search(BinaryTreeNode<T>* &tree, T data) const;
    void preOrder(BinaryTreeNode<T>* &node) const;
    void inOrder(BinaryTreeNode<T>* &node) const;
    void postOrder(BinaryTreeNode<T>* &node) const;
    //BrnaryTreeNode<T>* minimumNode(BinaryTreeNode<T>* &tree);
    //BinaryTreeNode<T>* maxmumNode(BinaryTreeNode<T>* &tree);
    //void print(BinaryTreeNode<T>* &tree);
    //BinaryTreeNode<T>* remove(BinaryTreeNode<T>* &tree, BinaryTreeNode<T> *z);
    //void destory(BinaryTreeNode<T>* &tree);

private:
    BinaryTreeNode<T> *m_root; //root node
};

template<class T>
void BinaryTree<T>::insert(BinaryTreeNode<T>* &tree, BinaryTreeNode<T>* node)
{
    //this is a empty tree
    if(tree == NULL) {
        tree = node;
        return;
    }

    BinaryTreeNode<T> *parent = NULL;
    BinaryTreeNode<T> *temp = tree;

    while(temp != NULL) {
        parent = temp;
        if(node->m_data > temp->m_data) {
            temp = temp->m_rightChild;
        } else {
            temp = temp->m_leftChild;
        }
    }

    node->m_parent = parent;
    if(parent->m_data > node->m_data) {
        parent->m_leftChild = node;
    } else {
        parent->m_rightChild = node;
    }
}

template<class T>
void BinaryTree<T>::Insert(T data)
{
    BinaryTreeNode<T>* node = new BinaryTreeNode<T>(data);
    if(node == NULL) {
        return;
    }
    insert(m_root, node);
}

template<class T>
void BinaryTree<T>::preOrder(BinaryTreeNode<T>* &node) const
{
    if(node) {
        node->visit();
        preOrder(node->m_leftChild);
        preOrder(node->m_rightChild);
    }
}

template<class T>
void BinaryTree<T>::PreOrder()
{
    cout<<"=====PreOrder====="<<endl;
    preOrder(m_root);
}


template<class T>
void BinaryTree<T>::inOrder(BinaryTreeNode<T>* &node) const
{
    if(node) {
        inOrder(node->m_leftChild);
        node->visit();
        inOrder(node->m_rightChild);
    }
}

template<class T>
void BinaryTree<T>::InOrder()
{
    cout<<"=====InOrder====="<<endl;
    inOrder(m_root);
}

template<class T>
void BinaryTree<T>::postOrder(BinaryTreeNode<T>* &node) const
{
    if(node) {
        postOrder(node->m_leftChild);
        postOrder(node->m_rightChild);
        node->visit();
    }
}

template<class T>
void BinaryTree<T>::PostOrder()
{
    cout<<"=====PostOrder====="<<endl;
    postOrder(m_root);
}

#endif
