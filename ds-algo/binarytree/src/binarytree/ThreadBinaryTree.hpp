#ifndef THREAD_BINARY_TREE_H
#define THREAD_BINARY_TREE_H

#include <iostream>

using namespace std;

int ThreadBinaryTreeTest();

typedef enum {Link, Thread} PointerTag;

template<class T>
class ThreadBinaryTreeNode {
public:
    void visit();
public:
    ThreadBinaryTreeNode() { m_leftChild = m_rightChild = m_parent = 0; }
    ThreadBinaryTreeNode(const T& e) {
        m_data = e;
        m_leftChild = m_rightChild = m_parent = 0;
    }
    ThreadBinaryTreeNode(const T& e, ThreadBinaryTreeNode *l, ThreadBinaryTreeNode *r, ThreadBinaryTreeNode *p) {
        m_data = e;
        m_leftChild = l;
        m_rightChild = r;
        m_parent = p;
    }
    void operator=(const ThreadBinaryTreeNode<T>& rhs) {
        m_data = rhs.m_data;
        m_leftChild = rhs.m_leftChild;
        m_rightChild = rhs.m_rightChild;
        m_parent = rhs.m_parent;
    }

    T m_data;
    ThreadBinaryTreeNode<T> *m_leftChild, *m_rightChild, *m_parent;
    PointerTag m_lTag;
    PointerTag m_rTag;
};

template<class T>
void ThreadBinaryTreeNode<T>::visit()
{
    cout << "Current node data: " << m_data << endl;
}

template<class T>
class ThreadBinaryTree
{
public:
    ThreadBinaryTree():m_root(NULL), m_pre(NULL), m_head(NULL){};

    //interfaces
    void Insert (T data);
    //ThreadBinaryTreeNode<T>* Search(T data);

    void InOrderToChain();
    void InOrderThreadChain();
 
    //BrnaryTreeNode<T>* MinimumNode();
    //ThreadBinaryTreeNode<T>* MaxmumNode();

    //T MinmumData();
    //T MaxmumData();

    //void Print();
    //void Remove(T data);

    //void Destory();

private:
    void insert(ThreadBinaryTreeNode<T>* &tree, ThreadBinaryTreeNode<T>* node);
    //ThreadBinaryTreeNode<T>* search(ThreadBinaryTreeNode<T>* &tree, T data) const;
    void inOrderToChain(ThreadBinaryTreeNode<T>* &node);
    void inOrderThreadChain(ThreadBinaryTreeNode<T>* &head);
    //BrnaryTreeNode<T>* minimumNode(ThreadBinaryTreeNode<T>* &tree);
    //ThreadBinaryTreeNode<T>* maxmumNode(ThreadBinaryTreeNode<T>* &tree);
    //void print(ThreadBinaryTreeNode<T>* &tree);
    //ThreadBinaryTreeNode<T>* remove(ThreadBinaryTreeNode<T>* &tree, ThreadBinaryTreeNode<T> *z);
    //void destory(ThreadBinaryTreeNode<T>* &tree);

private:
    ThreadBinaryTreeNode<T> *m_root; //root node
    //for traverse a tree and serialize it to a chain
    ThreadBinaryTreeNode<T> *m_pre;
    ThreadBinaryTreeNode<T> *m_head;
};

template<class T>
void ThreadBinaryTree<T>::insert(ThreadBinaryTreeNode<T>* &tree, ThreadBinaryTreeNode<T>* node)
{
    //this is a empty tree
    if(tree == NULL) {
        tree = node;
        return;
    }

    ThreadBinaryTreeNode<T> *parent = NULL;
    ThreadBinaryTreeNode<T> *temp = tree;

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
void ThreadBinaryTree<T>::Insert(T data)
{
    ThreadBinaryTreeNode<T>* node = new ThreadBinaryTreeNode<T>(data);
    if(node == NULL) {
        return;
    }
    insert(m_root, node);
}

template<class T>
void ThreadBinaryTree<T>::inOrderToChain(ThreadBinaryTreeNode<T>* &node)
{
    if(node) {
        if(node->m_lTag == Link) {
            inOrderToChain(node->m_leftChild);
        }

        node->visit();

        if(node->m_leftChild == NULL) {
            node->m_lTag = Thread;
            if(m_pre == NULL) { //current node is the first node by inorder
                node->m_leftChild = m_head; 
            } else {
                node->m_leftChild = m_pre;
            }
        }
        if(m_pre != NULL && m_pre->m_rightChild == NULL) {
            m_pre->m_rTag = Thread;
            m_pre->m_rightChild = node;
        }

        m_pre = node;
        if(node->m_rTag == Link) {
            inOrderToChain(node->m_rightChild);
        }
    }
}

template<class T>
void ThreadBinaryTree<T>::InOrderToChain()
{
    cout<<"=====InOrderToChain====="<<endl;
    string hData;
    m_head = new ThreadBinaryTreeNode<T>(hData);
    inOrderToChain(m_root);
    m_pre->m_rightChild = m_head;
    m_head->m_leftChild = m_root;
    m_head->m_rightChild = m_pre;
}
template<class T>
void ThreadBinaryTree<T>::inOrderThreadChain(ThreadBinaryTreeNode<T>* &head)
{
    ThreadBinaryTreeNode<T>* p = NULL;
    p = head->m_leftChild;
    while(p != head){
        while(p->m_lTag == Link) {
            p = p->m_leftChild;
        }
        p->visit();
        while(p->m_rTag == Thread && p->m_rightChild != head) {
            p = p->m_rightChild;
            p->visit();
        }
        p = p->m_rightChild;
    }
}

template<class T>
void ThreadBinaryTree<T>::InOrderThreadChain()
{
    cout<<"=====InOrderTheadChain====="<<endl;
    inOrderThreadChain(m_head);
}

#endif
