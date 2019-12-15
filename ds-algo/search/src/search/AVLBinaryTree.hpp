#ifndef AVL_BINARY_TREE_H
#define AVL_BINARY_TREE_H

#include <iostream>

using namespace std;

int AVLBinaryTreeTest();

const std::int8_t LH = 1; //左子树高
const std::int8_t EH = 0; 
const std::int8_t RH = -1; //右子树高

typedef std::int8_t Status;

const std::int8_t OK    = 1;
const std::int8_t ERROR = 0;
const std::int8_t TRUE  = 1;
const std::int8_t FALSE = 0;

template<class T>
class AVLBinaryTreeNode {
public:
    void visit();
public:
    AVLBinaryTreeNode() { m_leftChild = m_rightChild = m_parent = 0; m_bf = EH;}
    AVLBinaryTreeNode(const T& e) {
        m_data = e;
        m_leftChild = m_rightChild = m_parent = 0;
        m_bf = EH;
    }
    AVLBinaryTreeNode(const T& e, AVLBinaryTreeNode *l, AVLBinaryTreeNode *r, AVLBinaryTreeNode *p, int8_t &bf) {
        m_data = e;
        m_leftChild = l;
        m_rightChild = r;
        m_parent = p;
        m_bf = bf;
    }
    void operator=(const AVLBinaryTreeNode<T>& rhs) {
        m_data = rhs.m_data;
        m_leftChild = rhs.m_leftChild;
        m_rightChild = rhs.m_rightChild;
        m_parent = rhs.m_parent;
        m_bf = EH;
    }

    T m_data;
    AVLBinaryTreeNode<T> *m_leftChild, *m_rightChild, *m_parent;
    int8_t m_bf; //结点的平衡因子，bf = 左子树高度 - 右子树高度
};

template<class T>
void AVLBinaryTreeNode<T>::visit()
{
    cout << "Current node data: " << m_data << endl;
}

template<class T>
class AVLBinaryTree
{
public:
    AVLBinaryTree():m_root(NULL){};

    //interfaces
    void Insert (T data);
    AVLBinaryTreeNode<T>* Search(T data);

    void PreOrder();
    void InOrder();
    void PostOrder();
 
    //BrnaryTreeNode<T>* MinimumNode();
    //AVLBinaryTreeNode<T>* MaxmumNode();

    //T MinmumData();
    //T MaxmumData();

    //void Print();
    void Remove(T data);

    //void Destory();

private:
    void L_Rotate(AVLBinaryTreeNode<T>* &P);
    void R_Rotate(AVLBinaryTreeNode<T>* &P);
    void leftBalance(AVLBinaryTreeNode<T>* &tree);
    void rightBalance(AVLBinaryTreeNode<T>* &tree);
    Status insert(AVLBinaryTreeNode<T>* &tree, AVLBinaryTreeNode<T>* &node, Status &taller);
    AVLBinaryTreeNode<T>* search(AVLBinaryTreeNode<T>* &tree, T data) const;
    void preOrder(AVLBinaryTreeNode<T>* &node) const;
    void inOrder(AVLBinaryTreeNode<T>* &node) const;
    void postOrder(AVLBinaryTreeNode<T>* &node) const;
    //BrnaryTreeNode<T>* minimumNode(AVLBinaryTreeNode<T>* &tree);
    //AVLBinaryTreeNode<T>* maxmumNode(AVLBinaryTreeNode<T>* &tree);
    //void print(AVLBinaryTreeNode<T>* &tree);
    void remove(AVLBinaryTreeNode<T>* &tree, T data);
    void deleteNode(AVLBinaryTreeNode<T>* &node);
    //void destory(AVLBinaryTreeNode<T>* &tree);

private:
    AVLBinaryTreeNode<T> *m_root; //root node
};

template<class T>
void AVLBinaryTree<T>::L_Rotate(AVLBinaryTreeNode<T>* &P)
{
    AVLBinaryTreeNode<T> *R;
    R = P->m_rightChild;  //R指向P的右子树根结点
    P->m_rightChild = R->m_leftChild;  //R的左子树挂接为P的右子树
    R->m_leftChild = P;  //R的左子树指向P结点
    P = R;
}

template<class T>
void AVLBinaryTree<T>::R_Rotate(AVLBinaryTreeNode<T>* &P)
{
    AVLBinaryTreeNode<T> *L;
    L = P->m_leftChild;
    P->m_leftChild = L->m_rightChild;
    L->m_rightChild = P;
    P = L;
}

/*
function : 对以tree为根结点的二叉树做左平衡旋转处理
paramter : tree是一颗不平衡的二叉树的根结点，本算法结束后，tree指向新的根结点A
*/
template<class T>
void AVLBinaryTree<T>::leftBalance(AVLBinaryTreeNode<T>* &tree)
{
    AVLBinaryTreeNode<T> *L, *Lr;
    L = tree->m_leftChild;  //L指向tree的左子树根结点
    
    //检查tree的左子树的平衡度，并做相应平衡处理
    switch (L->m_bf)
    {
        /*
        L的平衡因子为LH(1)，表明与根结点（tree）的平衡因子符号相同
        新结点插入在tree的左孩子的左子树上，做单右旋处理
        */
        case LH:
            tree->m_bf = L->m_bf = EH; //右旋转后，tree和其左子树的平衡因子都讲变成EH(0)
            R_Rotate(tree);
            break;
        /*
        新结点插入在tree的左孩子的右子树上，要做双旋处理
        */
        case RH:
            Lr = L->m_rightChild; //Lr指向tree的左孩子的右子树根结点  
            switch (Lr->m_bf)
            {
                case LH:
                    tree->m_bf = RH;
                    L->m_bf = EH;
                    break;
                case EH:
                    tree->m_bf = L->m_bf = EH;
                    break;
                case RH:
                    tree->m_bf = EH;
                    L->m_bf = LH;
                    break;
            }
            Lr->m_bf = EH;
            L_Rotate(tree->m_leftChild);  //对tree的左子树做左平衡处理
            R_Rotate(tree); //对tree做右旋平衡处理
    }
}

template<class T>
void AVLBinaryTree<T>::rightBalance(AVLBinaryTreeNode<T>* &tree)
{
    AVLBinaryTreeNode<T> *R, *Rl;
    R = tree->m_rightChild;  //R指向tree的右子树根结点
    
    switch (R->m_bf)
    {
        //新结点在tree的右子孩子的右子树上，只需要一次左旋处理
        case RH:
            tree->m_bf = R->m_bf = EH;
            L_Rotate(tree);
            break;
        case LH:
            Rl = R->m_rightChild;
            switch (Rl->m_bf)
            {
                case RH:
                    tree->m_bf = LH;
                    R->m_bf = EH;
                    break;
                case EH:
                    tree->m_bf = R->m_bf = EH;
                    break;
                case LH:
                    tree->m_bf = EH;
                    R->m_bf = RH;
                    break;
            }
            Rl->m_bf = EH;
            R_Rotate(tree->m_rightChild);
            L_Rotate(tree);
    }
}

template<class T>
Status AVLBinaryTree<T>::insert(AVLBinaryTreeNode<T>* &tree, AVLBinaryTreeNode<T>* &node, Status &taller)
{
    if(tree == NULL) { //插入新结点，树长高
        tree = node;
        taller = TRUE;
    } else {
        if(tree->m_data == node->m_data) {
            taller = FALSE;
            delete(node);
            cout<<"The data ["<<tree->m_data<<"] has been exists."<<endl;
            return FALSE;
        }
        
        if (node->m_data < tree->m_data) {
            //继续从tree的左子树寻找插入点
            if (!insert(tree->m_leftChild, node, taller)) { //未插入
                return FALSE;
            }
            if (taller) { //已插入到tree的左子树中且左子树"长高"
                switch (tree->m_bf)
                {
                    case LH:  //原本左子树比右子树高，需要做左平衡处理
                        leftBalance(tree);
                        taller = FALSE;
                        break;
                
                    case EH:  //原本左、右子树等高，现因左子树长高而使树长高
                        tree->m_bf = LH;
                        taller = TRUE;
                        break;
                    case RH:  //原本右子树比左子树高，现在左、右子树等高
                        tree->m_bf = EH; 
                        taller = FALSE;
                        break;
                }
            }   
        } 
        else
        { 
            if (!insert(tree->m_rightChild, node, taller)) {
                return FALSE;
            }
            
            if (taller) {
                
                switch (tree->m_bf)
                {
                    case LH:  //原本左子树比右子树高，现在左、右子树等高
                        tree->m_bf = EH;
                        taller = FALSE;
                        break;
                    case EH:  //原本左右子树等高，现在右子树增高而使树增高
                        tree->m_bf = RH;
                        taller = TRUE;
                        break;
                    case RH:
                        rightBalance(tree);
                        taller = FALSE;
                        break;
                }
            } 
        }
    }
    return TRUE;
}

template<class T>
void AVLBinaryTree<T>::Insert(T data)
{
    AVLBinaryTreeNode<T>* node = new AVLBinaryTreeNode<T>(data);
    if(node == NULL) {
        cout<<"Insert failed because of create new node failed."<<endl;
        return;
    }

    Status taller = FALSE;
    insert(m_root, node, taller);
}

template<class T>
AVLBinaryTreeNode<T>* AVLBinaryTree<T>::search(AVLBinaryTreeNode<T>* &tree, T data) const
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
AVLBinaryTreeNode<T>* AVLBinaryTree<T>::Search(T data)
{
    AVLBinaryTreeNode<T>* re = search(m_root, data);
    if(re){
        cout<<"Search [" << re->m_data << "] success."<<endl;
    } else {
        cout<<"Search [" << data << "] failed."<<endl;
    }
    return re;
}

template<class T>
void AVLBinaryTree<T>::preOrder(AVLBinaryTreeNode<T>* &node) const
{
    if(node) {
        node->visit();
        preOrder(node->m_leftChild);
        preOrder(node->m_rightChild);
    }
}

template<class T>
void AVLBinaryTree<T>::PreOrder()
{
    cout<<"=====PreOrder====="<<endl;
    preOrder(m_root);
}


template<class T>
void AVLBinaryTree<T>::inOrder(AVLBinaryTreeNode<T>* &node) const
{
    if(node) {
        inOrder(node->m_leftChild);
        node->visit();
        inOrder(node->m_rightChild);
    }
}

template<class T>
void AVLBinaryTree<T>::InOrder()
{
    cout<<"=====InOrder====="<<endl;
    inOrder(m_root);
}

template<class T>
void AVLBinaryTree<T>::postOrder(AVLBinaryTreeNode<T>* &node) const
{
    if(node) {
        postOrder(node->m_leftChild);
        postOrder(node->m_rightChild);
        node->visit();
    }
}

template<class T>
void AVLBinaryTree<T>::PostOrder()
{
    cout<<"=====PostOrder====="<<endl;
    postOrder(m_root);
}

template<class T>
void AVLBinaryTree<T>::remove(AVLBinaryTreeNode<T>* &tree, T data)
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
void AVLBinaryTree<T>::deleteNode(AVLBinaryTreeNode<T>* &node)
{
    AVLBinaryTreeNode<T> *q, *s = NULL;
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
void AVLBinaryTree<T>::Remove(T data)
{
    remove(m_root, data);
}

#endif
