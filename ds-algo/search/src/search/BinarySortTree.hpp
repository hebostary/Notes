#ifndef BINARY_SORT_TREE_H
#define BINARY_SORT_TREE_H

#include <iostream>

using namespace std;

int BinarySortTreeTest();

template<class T>
class BinarySortTreeNode {
public:
    void visit();
public:
    BinarySortTreeNode() { m_leftChild = m_rightChild = m_parent = 0; }
    BinarySortTreeNode(const T& e) {
        m_data = e;
        m_leftChild = m_rightChild = m_parent = 0;
    }
    BinarySortTreeNode(const T& e, BinarySortTreeNode *l, BinarySortTreeNode *r, BinarySortTreeNode *p) {
        m_data = e;
        m_leftChild = l;
        m_rightChild = r;
        m_parent = p;
    }
    void operator=(const BinarySortTreeNode<T>& rhs) {
        m_data = rhs.m_data;
        m_leftChild = rhs.m_leftChild;
        m_rightChild = rhs.m_rightChild;
        m_parent = rhs.m_parent;
    }

    T m_data;
    BinarySortTreeNode<T> *m_leftChild, *m_rightChild, *m_parent;
};

template<class T>
void BinarySortTreeNode<T>::visit()
{
    cout << "Current node data: " << m_data << endl;
}

template<class T>
class BinarySortTree
{
public:
    BinarySortTree():m_root(NULL){};

    //interfaces
    void Insert (T data);
    BinarySortTreeNode<T>* Search(T data);

    void PreOrder();
    void InOrder();
    void PostOrder();
 
    //BrnaryTreeNode<T>* MinimumNode();
    //BinarySortTreeNode<T>* MaxmumNode();

    //T MinmumData();
    //T MaxmumData();

    //void Print();
    void Remove(T data);

    //void Destory();

private:
    void insert(BinarySortTreeNode<T>* &tree, BinarySortTreeNode<T>* node);
    BinarySortTreeNode<T>* search(BinarySortTreeNode<T>* &tree, T data) const;
    void preOrder(BinarySortTreeNode<T>* &node) const;
    void inOrder(BinarySortTreeNode<T>* &node) const;
    void postOrder(BinarySortTreeNode<T>* &node) const;
    //BrnaryTreeNode<T>* minimumNode(BinarySortTreeNode<T>* &tree);
    //BinarySortTreeNode<T>* maxmumNode(BinarySortTreeNode<T>* &tree);
    //void print(BinarySortTreeNode<T>* &tree);
    void remove(BinarySortTreeNode<T>* &tree, T data);
    void deleteNode(BinarySortTreeNode<T>* &node);
    //void destory(BinarySortTreeNode<T>* &tree);

private:
    BinarySortTreeNode<T> *m_root; //root node
};

template<class T>
void BinarySortTree<T>::insert(BinarySortTreeNode<T>* &tree, BinarySortTreeNode<T>* node)
{
    //this is a empty tree
    if(tree == NULL) {
        tree = node;
        return;
    }

    BinarySortTreeNode<T> *parent = NULL;
    BinarySortTreeNode<T> *temp = tree;

    while(temp != NULL) {
        parent = temp;
        if(node->m_data > temp->m_data) {
            temp = temp->m_rightChild;
        } else if (node->m_data < temp->m_data) {
            temp = temp->m_leftChild;
        } else {
            cout<<"Insert failed because of the data [" << node->m_data <<"] has been exists."<<endl;
            delete(node);
            return;
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
void BinarySortTree<T>::Insert(T data)
{
    BinarySortTreeNode<T>* node = new BinarySortTreeNode<T>(data);
    if(node == NULL) {
        cout<<"Insert failed because of create new node failed."<<endl;
        return;
    }
    insert(m_root, node);
}

template<class T>
BinarySortTreeNode<T>* BinarySortTree<T>::search(BinarySortTreeNode<T>* &tree, T data) const
{
    if(!tree) {
        return NULL;
    } else if (tree->m_data == data) {
        return tree;
    } else if (tree->m_data > data) {
        return search(tree->m_leftChild, data);
    } else {
        return search(tree->m_rightChild, data);
    }
}

template<class T>
BinarySortTreeNode<T>* BinarySortTree<T>::Search(T data)
{
    BinarySortTreeNode<T>* re = search(m_root, data);
    if(re){
        cout<<"Search [" << re->m_data << "] success."<<endl;
    } else {
        cout<<"Search [" << data << "] failed."<<endl;
    }
    return re;
}

template<class T>
void BinarySortTree<T>::preOrder(BinarySortTreeNode<T>* &node) const
{
    if(node) {
        node->visit();
        preOrder(node->m_leftChild);
        preOrder(node->m_rightChild);
    }
}

template<class T>
void BinarySortTree<T>::PreOrder()
{
    cout<<"=====PreOrder====="<<endl;
    preOrder(m_root);
}


template<class T>
void BinarySortTree<T>::inOrder(BinarySortTreeNode<T>* &node) const
{
    if(node) {
        inOrder(node->m_leftChild);
        node->visit();
        inOrder(node->m_rightChild);
    }
}

template<class T>
void BinarySortTree<T>::InOrder()
{
    cout<<"=====InOrder====="<<endl;
    inOrder(m_root);
}

template<class T>
void BinarySortTree<T>::postOrder(BinarySortTreeNode<T>* &node) const
{
    if(node) {
        postOrder(node->m_leftChild);
        postOrder(node->m_rightChild);
        node->visit();
    }
}

template<class T>
void BinarySortTree<T>::PostOrder()
{
    cout<<"=====PostOrder====="<<endl;
    postOrder(m_root);
}

template<class T>
void BinarySortTree<T>::remove(BinarySortTreeNode<T>* &tree, T data)
{
    if(!tree) {
        return;
    } else {
        if(data == tree->m_data) {
            return deleteNode(tree);
        } else if (data > tree->m_data) {
            return remove(tree->m_rightChild, data);
        } else {
            return remove(tree->m_leftChild, data);
        }
    }
}

template<class T>
void BinarySortTree<T>::deleteNode(BinarySortTreeNode<T>* &node)
{
    BinarySortTreeNode<T> *q, *s = NULL;
    if(node->m_leftChild == NULL){
        q = node;
        node = node->m_rightChild;
        delete q;
    } else if(node->m_rightChild == NULL) {
        q = node;
        node = node->m_leftChild;
        delete q;
    } else {
        q = node;
        s = node->m_leftChild;
        while(s->m_rightChild) {
            q = s; s = s->m_rightChild;
        }
        node->m_data = s->m_data;
        if(q != node) {
            q->m_rightChild = s->m_leftChild;
        } else {
            q->m_leftChild = s->m_leftChild;
        }
        delete(s);
    }
}

template<class T>
void BinarySortTree<T>::Remove(T data)
{
    remove(m_root, data);
}

#endif
