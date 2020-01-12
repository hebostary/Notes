- [查找算法](#%e6%9f%a5%e6%89%be%e7%ae%97%e6%b3%95)
  - [二分查找](#%e4%ba%8c%e5%88%86%e6%9f%a5%e6%89%be)
    - [基础二分查找算法](#%e5%9f%ba%e7%a1%80%e4%ba%8c%e5%88%86%e6%9f%a5%e6%89%be%e7%ae%97%e6%b3%95)
    - [二分查找的变体问题](#%e4%ba%8c%e5%88%86%e6%9f%a5%e6%89%be%e7%9a%84%e5%8f%98%e4%bd%93%e9%97%ae%e9%a2%98)
      - [变体问题1：查找第一个值等于给定值的元素](#%e5%8f%98%e4%bd%93%e9%97%ae%e9%a2%981%e6%9f%a5%e6%89%be%e7%ac%ac%e4%b8%80%e4%b8%aa%e5%80%bc%e7%ad%89%e4%ba%8e%e7%bb%99%e5%ae%9a%e5%80%bc%e7%9a%84%e5%85%83%e7%b4%a0)
      - [变体问题2：查找最后一个值等于给定值的元素](#%e5%8f%98%e4%bd%93%e9%97%ae%e9%a2%982%e6%9f%a5%e6%89%be%e6%9c%80%e5%90%8e%e4%b8%80%e4%b8%aa%e5%80%bc%e7%ad%89%e4%ba%8e%e7%bb%99%e5%ae%9a%e5%80%bc%e7%9a%84%e5%85%83%e7%b4%a0)
      - [变体问题3：查找第一个大于等于给定值的元素](#%e5%8f%98%e4%bd%93%e9%97%ae%e9%a2%983%e6%9f%a5%e6%89%be%e7%ac%ac%e4%b8%80%e4%b8%aa%e5%a4%a7%e4%ba%8e%e7%ad%89%e4%ba%8e%e7%bb%99%e5%ae%9a%e5%80%bc%e7%9a%84%e5%85%83%e7%b4%a0)
      - [变体问题4：查找最后一个小于等于给定值的元素](#%e5%8f%98%e4%bd%93%e9%97%ae%e9%a2%984%e6%9f%a5%e6%89%be%e6%9c%80%e5%90%8e%e4%b8%80%e4%b8%aa%e5%b0%8f%e4%ba%8e%e7%ad%89%e4%ba%8e%e7%bb%99%e5%ae%9a%e5%80%bc%e7%9a%84%e5%85%83%e7%b4%a0)
  - [散列表](#%e6%95%a3%e5%88%97%e8%a1%a8)
  - [二叉树基础](#%e4%ba%8c%e5%8f%89%e6%a0%91%e5%9f%ba%e7%a1%80)
    - [二叉树存储结构](#%e4%ba%8c%e5%8f%89%e6%a0%91%e5%ad%98%e5%82%a8%e7%bb%93%e6%9e%84)
      - [链式存储](#%e9%93%be%e5%bc%8f%e5%ad%98%e5%82%a8)
      - [顺序存储](#%e9%a1%ba%e5%ba%8f%e5%ad%98%e5%82%a8)
    - [二叉树遍历](#%e4%ba%8c%e5%8f%89%e6%a0%91%e9%81%8d%e5%8e%86)
  - [二叉查找树(Binary Search Tree)](#%e4%ba%8c%e5%8f%89%e6%9f%a5%e6%89%be%e6%a0%91binary-search-tree)
  - [AVL平衡树](#avl%e5%b9%b3%e8%a1%a1%e6%a0%91)
  - [参考链接](#%e5%8f%82%e8%80%83%e9%93%be%e6%8e%a5)

# 查找算法

> 说明：本文内的部分配图来自极客时间王争老师的《数据结构和算法之美》专栏课程。

## 二分查找

### 基础二分查找算法

以从有序数组[8，11，19，23，27，33，45，55，67，98]中找出19为例：
![二分查找图示](https://static001.geekbang.org/resource/image/8b/29/8bce81259abf0e9a06f115e22586b829.jpg)

基本原理

- 二分查找的原理很简单（类似于快排中的分区思想，只不过查找的是有序序列），通过与查找区间的中间值比较，不断对半缩小查找区间，使得查找和比较次数大幅减少。

代码实现

```c++
int binarysearch(int *a, int n, int key)
{
    cout<<"***********[Begin]Binary Search**********"<<endl;
    int re = -1;
    int low = 0, high = n - 1, ite = 0;
    while(low <= high) {
        cout<<"Search counter: " << ite++ << endl;
        //mid = (high + low)/2; //high和low都很大时求和容易溢出
        int mid = low + ((high - low)>>1);
        if(key == a[mid]) {
            re =  mid;
            break;
        } else if (key > a[mid]) {
            //注意low和high的更新，如果是low=mid和high=mid，则在low=high时会发生死循环
            low = mid + 1;
        } else {
            high = mid -1;
        }
    }
    if(re == -1) {
        cout<<"Search failed"<<endl;
    }
    cout<<"***********[End]Binary Search**********"<<endl;
    return re;
}
```

复杂度分析

- 二分查找的时间复杂度为**O(logn)**，查找效率非常高。

适用场景

- **因为需要支持根据下标随机访问元素，二分查找依赖于顺序存储结构（数组）。** 那二分查找能否依赖其他数据结构呢？比如链表。NO，主要原因是二分查找算法需要按照下标随机访问元素。数组按照下标随机访问数据的时间复杂度是 O(1)，而链表随机访问的时间复杂度是 O(n)。所以，如果数据使用链表存储，二分查找的时间复杂就会变得很高。
- **查找的数组必须是有序的，如果数组是无序的则需要先进行排序。** 如果我们的数据集合有频繁的插入和删除操作，要想用二分查找，要么每次插入、删除操作之后保证数据仍然有序，要么在每次二分查找之前都先进行排序。针对这种动态数据集合，无论哪种方法，维护有序的成本都是很高的。
- **需要谨慎地考虑存储空间是否够用。** 尽管数组是最节省内存空间的数据结构（除了元素外不占用额外地空间），但是为了支持随机访问的特性，数组依赖于连续的内存空间。考虑内存使用过程中空闲内存会逐渐碎片化，剩余内存空间并不一定是连续的，所以太大的数据用数组存储就比较吃力了，也就不能用二分查找了。

### 二分查找的变体问题

#### 变体问题1：查找第一个值等于给定值的元素

代码实现

```c++
int binarysearch_first_euqal(int *a, int n, int key)
{
    int re = -1;
    int low = 0, high = n - 1, ite = 0;
    while(low <= high) {
        cout<<"Search counter: " << ite++ << endl;
        int mid = low + ((high - low)>>1);
        if(key == a[mid]) {
            //mid等于0的时候，意味着数组第一个元素就是我们查找的结果
            //a[mid-1]不等于key的时候，意味着mid位置的元素就是我们要找的结果
            if((mid == 0) || a[mid - 1] != key) {
                re = mid;
                break;
            } else {
                high = mid -1;
            }
        } else if (key > a[mid]) {
            low = mid + 1;
        } else {
            high = mid -1;
        }
    }
    if(re == -1) {
        cout<<"Search failed"<<endl;
    }
    return re;
}
```

#### 变体问题2：查找最后一个值等于给定值的元素

代码实现

```c++
int binarysearch_last_euqal(int *a, int n, int key)
{
    int re = -1;
    int low = 0, high = n - 1, ite = 0;
    while(low <= high) {
        cout<<"Search counter: " << ite++ << endl;
        int mid = low + ((high - low)>>1);
        if(key == a[mid]) {
            //和第一个问题类似，只不过是向数组右边移动判断
            //注意此处，mid一定是和最后的位置n-1比较，而不是查找过程中变化的high
            if((mid == (n - 1)) || a[mid + 1] != key) {
                re = mid;
                break;
            } else {
                low = mid + 1;
            }
        } else if (key > a[mid]) {
            low = mid + 1;
        } else {
            high = mid -1;
        }
    }
    if(re == -1) {
        cout<<"Search failed"<<endl;
    }
    return re;
}
```

#### 变体问题3：查找第一个大于等于给定值的元素

代码实现

```c++
int binarysearch_first_euqal_or_large(int *a, int n, int key)
{
    int re = -1;
    int low = 0, high = n - 1, ite = 0;
    while(low <= high) {
        cout<<"Search counter: " << ite++ << endl;
        int mid = low + ((high - low)>>1);
        if (a[mid] >= key) {
            //继续检查mid左边一位的数组元素,注意数组左边界的检查
            if( (mid == 0) || a[mid - 1] < key) {
                re = mid;
                break;
            } else {
                //mid左边一位仍然大于等于key，则继续迭代查找
                high = mid - 1;
            }
        } else {
            low = mid + 1;
        }
    }
    if(re == -1) {
        cout<<"Search failed"<<endl;
    }
    return re;
}
```

#### 变体问题4：查找最后一个小于等于给定值的元素

代码实现

```c++
int binarysearch_last_euqal_or_small(int *a, int n, int key)
{
    int re = -1;
    int low = 0, high = n - 1, ite = 0;
    while(low <= high) {
        cout<<"Search counter: " << ite++ << endl;
        int mid = low + ((high - low)>>1);
        if (a[mid] <= key) {
            //继续检查mid右边一位的数组元素,注意数组右边界的检查
            if( (mid == (n - 1)) || a[mid + 1] > key) {
                re = mid;
                break;
            } else {
                //mid右边一位仍然小等于key，则继续迭代查找
                low = mid + 1;
            }
        } else {
            high = mid - 1;
        }
    }
    if(re == -1) {
        cout<<"Search failed"<<endl;
    }
    return re;
}
```

## 散列表

## 二叉树基础

### 二叉树存储结构

二叉树的存储结构主要有两种：基于链表的链式存储和基于数组的顺序存储。

#### 链式存储

![二叉树链式存储](https://static001.geekbang.org/resource/image/12/8e/12cd11b2432ed7c4dfc9a2053cb70b8e.jpg)

链式存储：每个节点有三个字段，其中一个存储数据，另外两个是指向左右子节点的指针。我们只要拎住根节点，就可以通过左右子节点的指针，把整棵树都串起来。大部分二叉树代码都是通过这种结构来实现的，根据实际需要，每个结点上可能还包含更多的数据，比如指向父结点的指针。

#### 顺序存储

![二叉树顺序存储](https://static001.geekbang.org/resource/image/14/30/14eaa820cb89a17a7303e8847a412330.jpg)

基于数组的顺序存储法：把根节点存储在下标 i = 1 的位置，那左子节点存储在下标2 \* i = 2的位置，右子节点存储在2 * i + 1 = 3 的位置。以此类推，B 节点的左子节点存储在 2 * i = 2 * 2 = 4 的位置，右子节点存储在 2 * i + 1 = 2 * 2 + 1 = 5 的位置。

- 如果节点 X 存储在数组中下标为 i 的位置，下标为 2 \* i 的位置存储的就是左子节点，下标为 2 * i + 1 的位置存储的就是右子节点。反过来，下标为 i/2 的位置存储就是它的父节点。通过这种方式，我们只要知道根节点存储的位置（一般情况下，为了方便计算子节点，根节点会存储在下标为 1 的位置），这样就可以通过下标计算，把整棵树都串起来。
- 对于一颗完全二叉树而言，使用顺序存储可以很好的利用空间。

但是，对于如下图的普通二叉树而言，数组中很多空间将会被浪费。
![二叉树顺序存储1](https://static001.geekbang.org/resource/image/08/23/08bd43991561ceeb76679fbb77071223.jpg)

### 二叉树遍历

![二叉树遍历](https://static001.geekbang.org/resource/image/ab/16/ab103822e75b5b15c615b68560cb2416.jpg)

## 二叉查找树(Binary Search Tree)

二叉查找树要求，在树中的任意一个节点，其左子树中的每个节点的值，都要小于这个节点的值，而右子树节点的值都大于这个节点的值。

![二叉查找树](https://static001.geekbang.org/resource/image/f3/ae/f3bb11b6d4a18f95aa19e11f22b99bae.jpg)

查找元素

- 在二叉搜索树中查找元素，其实和二分查找原理是一样的。

插入元素
![二叉查找树插入](https://static001.geekbang.org/resource/image/da/c5/daa9fb557726ee6183c5b80222cfc5c5.jpg)

- 如果要插入的数据比节点的数据大，并且节点的右子树为空，就将新数据直接插到右子节点的位置；如果不为空，就再递归遍历右子树，查找插入位置。同理，如果要插入的数据比节点数值小，并且节点的左子树为空，就将新数据插入到左子节点的位置；如果不为空，就再递归遍历左子树，查找插入位置。

删除元素
![二叉查找树删除](https://static001.geekbang.org/resource/image/29/2c/299c615bc2e00dc32225f4d9e3490e2c.jpg)

删除元素时有3中情况需要处理：

1. 如果要删除的节点没有子节点，我们只需要直接将父节点中，指向要删除节点的指针置为 null。比如图中的删除节点 55。

2. 如果要删除的节点只有一个子节点（只有左子节点或者右子节点），我们只需要更新父节点中，指向要删除节点的指针，让它指向要删除节点的子节点就可以了。比如图中的删除节点 13。

3. 如果要删除的节点有两个子节点，我们需要找到这个节点的右子树中的最小节点（或者是左子树的最大节点），把它替换到要删除的节点上。然后再删除掉这个最小节点（或者是最大节点），因为最小节点肯定没有左子节点，所以，我们可以应用上面两条规则来删除这个最小节点。比如图中的删除节点 18。

复杂度分析

![二叉查找树性能](https://static001.geekbang.org/resource/image/e3/d9/e3d9b2977d350526d2156f01960383d9.jpg)

- 对于一颗完全二叉树，不管操作是插入、删除还是查找，查找元素的时间复杂度和有序数组的二分查找是类似的，也就是O(lgn)，也就是取决于树的高度。遗憾的是，普通的二叉查找树的性能是很不稳定的，十分依赖于元素的插入删除顺序，很容易退化到链表的时间复杂度O(n)。因此，在实际应用中基本上不会使用这种普通的二叉查找树，而是使用后面介绍的自平衡二叉树。

代码实现
[BinarySearchTree.hpp](https://github.com/hebostary/Notes/blob/master/ds-algo/search/src/search/BinarySearchTree.hpp)

## AVL平衡树

[AVL平衡树插入删除结点过程平衡操作图示](https://blog.csdn.net/u012299594/article/details/84924674)

## 参考链接

[visualgo-算法动态过程演示](https://visualgo.net/en)
