#ifndef BINARY_SORT_TREE_H
#define BINARY_SORT_TREE_H

#include <iostream>

using namespace std;

/*
二叉查找树 or 二叉搜索树，
因为中序遍历时可以得到所有元素的有序输出，所以又叫二叉排序树。
*/

int BinarySearchTreeTest();

template<class T>
class BinarySearchTreeNode {
public:
    void visit();
public:
    BinarySearchTreeNode() { m_leftChild = m_rightChild = m_parent = 0; }
    BinarySearchTreeNode(const T& e) {
        m_data = e;
        m_leftChild = m_rightChild = m_parent = 0;
    }
    BinarySearchTreeNode(const T& e, BinarySearchTreeNode *l, BinarySearchTreeNode *r, BinarySearchTreeNode *p) {
        m_data = e;
        m_leftChild = l;
        m_rightChild = r;
        m_parent = p;
    }
    void operator=(const BinarySearchTreeNode<T>& rhs) {
        m_data = rhs.m_data;
        m_leftChild = rhs.m_leftChild;
        m_rightChild = rhs.m_rightChild;
        m_parent = rhs.m_parent;
    }

    T m_data;
    BinarySearchTreeNode<T> *m_leftChild, *m_rightChild, *m_parent;
};

template<class T>
void BinarySearchTreeNode<T>::visit()
{
    cout << "Current node data: " << m_data << endl;
}

template<class T>
class BinarySearchTree
{
public:
    BinarySearchTree():m_root(NULL){};

    //interfaces
    void Insert (T data);
    BinarySearchTreeNode<T>* Search(T data);

    void PreOrder();
    void InOrder();
    void PostOrder();
 
    //BrnaryTreeNode<T>* MinimumNode();
    //BinarySearchTreeNode<T>* MaxmumNode();

    //T MinmumData();
    //T MaxmumData();

    //void Print();
    void Remove(T data);

    int High();

    //void Destory();

private:
    void insert(BinarySearchTreeNode<T>* &tree, BinarySearchTreeNode<T>* node);
    BinarySearchTreeNode<T>* search(BinarySearchTreeNode<T>* &tree, T data) const;
    void preOrder(BinarySearchTreeNode<T>* &node) const;
    void inOrder(BinarySearchTreeNode<T>* &node) const;
    void postOrder(BinarySearchTreeNode<T>* &node) const;
    //BrnaryTreeNode<T>* minimumNode(BinarySearchTreeNode<T>* &tree);
    //BinarySearchTreeNode<T>* maxmumNode(BinarySearchTreeNode<T>* &tree);
    //void print(BinarySearchTreeNode<T>* &tree);
    void remove(BinarySearchTreeNode<T>* &tree, T data);
    void deleteNode(BinarySearchTreeNode<T>* &node);
    int high(BinarySearchTreeNode<T>* &node);
    //void destory(BinarySearchTreeNode<T>* &tree);

private:
    BinarySearchTreeNode<T> *m_root; //root node
};

template<class T>
void BinarySearchTree<T>::insert(BinarySearchTreeNode<T>* &tree, BinarySearchTreeNode<T>* node)
{
    //this is a empty tree
    if(tree == NULL) {
        tree = node;
        return;
    }

    BinarySearchTreeNode<T> *parent = NULL;
    BinarySearchTreeNode<T> *temp = tree;

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
void BinarySearchTree<T>::Insert(T data)
{
    BinarySearchTreeNode<T>* node = new BinarySearchTreeNode<T>(data);
    if(node == NULL) {
        cout<<"Insert failed because of create new node failed."<<endl;
        return;
    }
    insert(m_root, node);
}

template<class T>
BinarySearchTreeNode<T>* BinarySearchTree<T>::search(BinarySearchTreeNode<T>* &tree, T data) const
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
BinarySearchTreeNode<T>* BinarySearchTree<T>::Search(T data)
{
    BinarySearchTreeNode<T>* re = search(m_root, data);
    if(re){
        cout<<"Search [" << re->m_data << "] success."<<endl;
    } else {
        cout<<"Search [" << data << "] failed."<<endl;
    }
    return re;
}

template<class T>
void BinarySearchTree<T>::preOrder(BinarySearchTreeNode<T>* &node) const
{
    if(node) {
        node->visit();
        preOrder(node->m_leftChild);
        preOrder(node->m_rightChild);
    }
}

template<class T>
void BinarySearchTree<T>::PreOrder()
{
    cout<<"=====PreOrder====="<<endl;
    preOrder(m_root);
}


template<class T>
void BinarySearchTree<T>::inOrder(BinarySearchTreeNode<T>* &node) const
{
    if(node) {
        inOrder(node->m_leftChild);
        node->visit();
        inOrder(node->m_rightChild);
    }
}

template<class T>
void BinarySearchTree<T>::InOrder()
{
    cout<<"=====InOrder====="<<endl;
    inOrder(m_root);
}

template<class T>
void BinarySearchTree<T>::postOrder(BinarySearchTreeNode<T>* &node) const
{
    if(node) {
        postOrder(node->m_leftChild);
        postOrder(node->m_rightChild);
        node->visit();
    }
}

template<class T>
void BinarySearchTree<T>::PostOrder()
{
    cout<<"=====PostOrder====="<<endl;
    postOrder(m_root);
}

template<class T>
void BinarySearchTree<T>::remove(BinarySearchTreeNode<T>* &tree, T data)
{
    if(!tree) {
        return;
    } else {
        //先找到需要删除的结点
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
void BinarySearchTree<T>::deleteNode(BinarySearchTreeNode<T>* &node)
{
    BinarySearchTreeNode<T> *q, *s = NULL;
    if(node->m_leftChild == NULL){
        //1 待删除结点的左子树为空，直接用右子树的根结点替换当前节点，最后删除结点
        q = node;
        node = node->m_rightChild;
        delete q;
    } else if(node->m_rightChild == NULL) {
        //2 待删除结点的右子树为空，直接用左子树的根结点替换当前节点，最后删除结点
        q = node;
        node = node->m_leftChild;
        delete q;
    } else {
        //3 待删除结点的左右子树都存在
        //注：可以删除左子树的最大值结点，也可以删除右子树的最小结点
        q = node;
        //3.1 找到待删除结点左子树的最大值D所在结点n
        s = node->m_leftChild;
        while(s->m_rightChild) {
            q = s; 
            s = s->m_rightChild;
        }
        //3.2 将D搬到待删除结点的位置
        node->m_data = s->m_data;
        //下面的处理需要谨慎：因为结点n必然没有右子树，但是还可能有左子树
        //这个左子树的嫁接位置取决于上面的结点n是否是原待删除结点的左孩子
        //下面用两个例子说明两种情况：
        if(q != node) {
            /*      6
                3       9
              2    5   7 10 
                 4
            例：我们要删除的结点6，则q->3, s->5,需要把5的左子树嫁接到结点3，作为右子树
            */
            q->m_rightChild = s->m_leftChild;
        } else {
            /*      6
                3       9
              2       7  10 
            例：我们要删除的结点6，则q->6, s->3,需要把3的左子树嫁接到结点6，作为左子树
            */
            q->m_leftChild = s->m_leftChild;
        }
        //3.3 最后删除D原来所在的结点
        delete(s);
    }
}

template<class T>
void BinarySearchTree<T>::Remove(T data)
{
    remove(m_root, data);
}

template<class T>
int BinarySearchTree<T>::High()
{
    return high(m_root);
}

template<class T>
int BinarySearchTree<T>::high(BinarySearchTreeNode<T>* &node)
{
    //递归法：子树的高度 = 左右子树高度较大的值加1
    if (node == NULL) {
        return 0;
    }
    return max(high(node->m_leftChild), high(node->m_rightChild)) + 1;   
}

#endif
