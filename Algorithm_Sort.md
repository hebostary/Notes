- [1. 经典排序算法](#1-%e7%bb%8f%e5%85%b8%e6%8e%92%e5%ba%8f%e7%ae%97%e6%b3%95)
  - [1.1. 冒泡排序(Bouble Sort)](#11-%e5%86%92%e6%b3%a1%e6%8e%92%e5%ba%8fbouble-sort)
  - [1.2. 插入排序(Insert Sort)](#12-%e6%8f%92%e5%85%a5%e6%8e%92%e5%ba%8finsert-sort)
  - [1.3. 选择排序(Select Sort)](#13-%e9%80%89%e6%8b%a9%e6%8e%92%e5%ba%8fselect-sort)
  - [1.4. 冒泡、插入和选择排序的比较](#14-%e5%86%92%e6%b3%a1%e6%8f%92%e5%85%a5%e5%92%8c%e9%80%89%e6%8b%a9%e6%8e%92%e5%ba%8f%e7%9a%84%e6%af%94%e8%be%83)
  - [1.5. 归并排序(Merge Sort)](#15-%e5%bd%92%e5%b9%b6%e6%8e%92%e5%ba%8fmerge-sort)
  - [1.6. 快速排序(Quick Sort)](#16-%e5%bf%ab%e9%80%9f%e6%8e%92%e5%ba%8fquick-sort)
  - [1.7. 堆排序(Heap Sort)](#17-%e5%a0%86%e6%8e%92%e5%ba%8fheap-sort)
  - [1.8. 希尔排序](#18-%e5%b8%8c%e5%b0%94%e6%8e%92%e5%ba%8f)
  - [1.9. 桶排序(Bucket Sort)](#19-%e6%a1%b6%e6%8e%92%e5%ba%8fbucket-sort)
  - [1.10. 计数排序(Counting Sort)](#110-%e8%ae%a1%e6%95%b0%e6%8e%92%e5%ba%8fcounting-sort)
  - [1.11. 基数排序(Radix Sort)](#111-%e5%9f%ba%e6%95%b0%e6%8e%92%e5%ba%8fradix-sort)
  - [1.12. 如何选择合适的排序算法](#112-%e5%a6%82%e4%bd%95%e9%80%89%e6%8b%a9%e5%90%88%e9%80%82%e7%9a%84%e6%8e%92%e5%ba%8f%e7%ae%97%e6%b3%95)
  - [参考链接](#%e5%8f%82%e8%80%83%e9%93%be%e6%8e%a5)

# 1. 经典排序算法

> 说明：本文内的部分配图来自极客时间王争老师的《数据结构和算法之美》专栏课程。

## 1.1. 冒泡排序(Bouble Sort)

冒泡排序图示

![冒泡排序图示](https://static001.geekbang.org/resource/image/92/09/9246f12cca22e5d872cbfce302ef4d09.jpg)

基本思路

- 每一次冒泡的过程就是将一个未排序元素中的最大或者最小的元素一步步地交换到正确的序列位置。

代码实现

```c++
int boublesort(vector<int> &nums) {
    cout<<"***********[Begin]Bouble sort**********"<<endl;
    printVec(nums);
    int size = nums.size();
    if (size < 2) return 0;

    for (int i = size - 1; i >= 0; --i)
    {
        bool flag = false;
        for (int j = 0; j < i; ++j)
        {
            if (nums[j] > nums[j+1]) {
                swap(nums[j], nums[j+1]);
                flag = true;
            }
        }

        //没有数据交换，说明数据已经是有序的，提前退出
        if (!flag) break;
    }

    printVec(nums);
    cout<<"***********[End]Bouble sort**********"<<endl;
    return 0;
}
```

复杂度分析

- 原地排序算法，空间复杂度O(1)。
- 最好情况时间复杂度O(n)：比如[ 1, 2, 3, 4, 5, 6 ]，只需要一次冒泡。
- 最坏情况时间复杂度O(n^2)：比如[ 6, 5, 4, 3, 2, 1 ]，需要n次冒泡。
- 平均情况时间复杂度O(n^2)。
- 稳定性：冒泡排序中，只有后面的元素大于前面的元素时才会做交换，因此不会破坏相同元素原有的相对位置关系，所以冒泡排序是稳定的排序算法。

## 1.2. 插入排序(Insert Sort)

插入排序图示

![插入排序图示](https://static001.geekbang.org/resource/image/fd/01/fd6582d5e5927173ee35d7cc74d9c401.jpg)

基本思路

- 将数组元素分为了两个部分：左边的**已排序区间**和右边的**未排序区间**，遍历时每次从未排序区间取出一个元素插入到已排序区间中的合适位置，每次插入时都可能会搬移大量的元素。

代码实现

```c++
int insertsort(vector<int> &nums) {
    cout<<"***********[Begin]Insert sort**********"<<endl;
    printVec(nums);
    int size = nums.size();
    if (size < 2) return 0;

    for (int i = 1; i < size; ++i)
    {
        int temp = nums[i]; //保存当前要插入的数据
        int j = i - 1;
        for ( ; j >= 0 ; --j) //从后向前遍历已排序区域查找插入位置，同时搬移元素
        {
            if (nums[j] > temp) {
                nums[j+1] = nums[j];
            } else {
                //如果元素相等则不变换已排序元素的位置，保证稳定性
                break;
            }
        }
        //j+1就是最终插入位置
        nums[j+1] = temp;
    }
    printVec(nums);
    cout<<"***********[End]Insert sort**********"<<endl;
    return 0;
}
```

复杂度分析

- 原地排序算法，空间复杂度O(1)。
- 最好情况时间复杂度O(n)：比如[ 1, 2, 3, 4, 5, 6 ]，只需要遍历n个元素，不需要搬移任何数据。
- 最坏情况时间复杂度O(n^2)：比如[ 6, 5, 4, 3, 2, 1 ]，插入每个元素时都需要搬移**已排序区**的所有数据。
- 平均情况时间复杂度O(n^2)。
- 稳定性：在上面的代码实现中，不会破坏相同元素的原来的相对位置，所以插入排序是稳定的排序算法。

## 1.3. 选择排序(Select Sort)

选择排序图示
![选择排序图示](https://static001.geekbang.org/resource/image/32/1d/32371475a0b08f0db9861d102474181d.jpg)

基本思路

- 和插入排序类似，将数组元素分为了两个部分：左边的**已排序区间**和右边的**未排序区间**，遍历时每次从未排序区间取出最小元素交换到已排序区间中的尾部。

代码实现

```c++
int selectsort(vector<int> &nums) {
    cout<<"***********[Begin]Select sort**********"<<endl;
    printVec(nums);
    int size = nums.size();
    if (size < 2) return 0;

    for (int i = 0; i < size - 1; ++i)
    {
        int minIndex = i + 1;
        for (int j = i + 2 ; j < size; ++j)
        {
            //找到未排序区里最小元素的索引
            if(nums[j] < nums[minIndex]) minIndex = j;
        }
        //将上面找到的元素交换到排序区的尾部
        if(nums[i] > nums[minIndex]) swap(nums[i], nums[minIndex]);
    }
    printVec(nums);
    cout<<"***********[End]Select sort**********"<<endl;
    return 0;
}
```

复杂度分析

- 原地排序算法，空间复杂度O(1)。
- 最好情况、最坏情况和平均情况的时间复杂度都是O(n^2)。
- 稳定性：因为在交换过程中可能打破相同元素原有的相对位置，所以它不是稳定的排序算法。比如[ 5, 8, 5, 2, 9 ]这样一组数据，使用选择排序算法来排序的话，第一次找到最小元素 2，与第一个 5 交换位置，那第一个 5 和中间的 5 顺序就变了，所以就不稳定了。

## 1.4. 冒泡、插入和选择排序的比较

理论分析比较
![冒泡、插入和选择排序的比较](https://static001.geekbang.org/resource/image/34/50/348604caaf0a1b1d7fee0512822f0e50.jpg)

运行效率比较

- 我用前面的代码实现去做10万个随机数的排序，运行时间统计如下，其实无论数据规模多大，三种排序相对时间开销都差不多。显然，在这三种平均时间复杂度均为O(n^2)的排序算法中，插入和选择排序更为高效，因为稳定性原因，插入排序是相对最好的选择。

```sh
***********[Begin]Bouble sort**********
***********[End]Bouble sort**********
Run time: [ 63.670406 ] seconds.
***********[Begin]Insert sort**********
***********[End]Insert sort**********
Run time: [ 17.872087 ] seconds.
***********[Begin]Select sort**********
***********[End]Select sort**********
Run time: [ 24.533143 ] seconds.
```

- 在理论分析上，插入排序和冒泡排序的复杂度是一样的，为什么实际运行效率又会有如此大的差距呢？参考代码实现，原因在于插入排序中的数据移动相比冒泡排序要简单很多，只是简单的将数组元素顺序后移，而冒泡过程中则是大量的swap交换操作，导致每个单元内的耗时更多。

- 即便如此，插入排序也仅适合于小规模数据的排序，因为具有代码实现简单，不占用额外空间的优点，但是随着数据规模增长到一定程度后，也不能再使用这种时间复杂度为O(n^2)的排序算法。

## 1.5. 归并排序(Merge Sort)

归并排序图示
![归并排序图示](https://static001.geekbang.org/resource/image/db/2b/db7f892d3355ef74da9cd64aa926dc2b.jpg)

基本思路

- 归并排序使用的是**分治思想**，对于这类可以不断分解为子问题的问题，一般用递归来实现，对于递归问题，最重要的是两大要素：**递归公式**和**终止条件**，参考代码实现。**分治是解决问题的处理思想，递归是一种编程技巧。**

代码实现

```c++
int mergesort(vector<int> &nums) {
    cout<<"***********[Begin]Merge sort**********"<<endl;
    printVec(nums);
    int size = nums.size();
    if (size < 2) return 0;

    mergesort_sep(nums, 0, size - 1);
    printVec(nums);
    cout<<"***********[End]Merge sort**********"<<endl;
    return 0;
}

void mergesort_sep(vector<int> &nums, int left, int right) {
    //递归终止条件
    if(left >= right) return;

    int mid = (left + right) / 2;
    mergesort_sep(nums, left, mid);
    mergesort_sep(nums, mid+1, right);
    merge(nums, left, mid+1, right);
}

//合并两段已经排序的子数组：nums[left, mid-1], nums[mid, right]
void merge(vector<int> &nums, int left, int mid, int right) {
    int size = right - left + 1;
    int oriLeft = left; //记录左边子数组的起始下标
    int oriMid = mid;  //记录右边子数组的起始下标
    int k = 0;
    vector<int> tempVec(size); //用一个临时数组来合并两个子数组
    while (left < oriMid && mid <= right)
    {
        tempVec[k++] = (nums[left] <= nums[mid]) ? nums[left++] : nums[mid++];
    }

    //检查两个子数组里哪个还有剩余的数据，如果left=oriMid，说明左边子数组已经全部入列
    int start = (left == oriMid) ? mid : left;
    while (k < size)
    {
        tempVec[k++] = nums[start++];
    }

    //将临时数组里已经排好序的元素写回到原数组中，完成合并
    for (k = 0; k < size; ++k)
    {
        nums[oriLeft + k] = tempVec[k];
    }
}
```

复杂度分析

- 非原地排序算法，在合并过程中用到了额外的数组空间，但是递归过程中使用的最大数组空间为排序数组的大小（注意：完成一次合并后，其中用到的临时数组空间在merge函数返回后就被立即释放了），因此空间复杂度为O(n)。
- 最好情况、最坏情况和平均情况的时间复杂度都是O(nlogn)，**待理解分析过程**。
- 稳定性：合并的过程中可以保证相等元素的原有相对位置，因此是稳定的。

## 1.6. 快速排序(Quick Sort)

快速排序图示

![快速排序图示](https://static001.geekbang.org/resource/image/4d/81/4d892c3a2e08a17f16097d07ea088a81.jpg)

基本思路

- 快速排序和归并排序类似，也是使用的是**分治思想**。快排先将数组中的所有元素进行分区，分区过程中会选择一个**中心点（pivot）**（比如选择数组的最后一个元素），中心点左边区域的元素都比中心点值小，右边都比它大，之后对中心点左右两个分区继续做类似的处理。
- 参考代码实现，在分区过程中可以通过双指针巧妙地完成元素交换，而不占用额外的空间。

归并快排对比

![归并快排对比](https://static001.geekbang.org/resource/image/aa/05/aa03ae570dace416127c9ccf9db8ac05.jpg)

快排在原理上和归并排序完全不一样:

- 归并排序是先将问题拆解为同等规模子问题，然后对子问题进行求解（排序），然后逐步合并子问题的解（排序好的子数组）。
- 快排并不是将简单的将问题拆解为同等规模子问题，通过分区操作拆解后的两个子问题具有不同的属性（大小也是随机的），并且分区过程中就逐步完成了数据的移动和排序。

代码实现

```c++
int quicksort(vector<int> &nums) {
    cout<<"***********[Begin]Quick sort**********"<<endl;
    printVec(nums);
    int size = nums.size();
    if (size < 2) return 0;

    quicksort_sep(nums, 0, size - 1);
    printVec(nums);
    cout<<"***********[End]Quick sort**********"<<endl;
    return 0;
}

void quicksort_sep(vector<int> &nums, int left, int right) {
    //递归终止条件
    if(left >= right) return;

    int p = partition(nums, left, right);
    quicksort_sep(nums, left, p - 1);
    quicksort_sep(nums, p + 1, right);
}

//对nums[left, right]区间的元素以nums[right]作为临界值pivot进行分区
//左边分区的元素都小于pivot，右边分区的元素都大于pivot
int partition(vector<int> &nums, int left, int right) {
    int pIndex = left;
    int pivot = nums[right];
    for (int i = left; i < right; ++i)
    {
        //其实这是一个双指针的应用，pIndex始终指向左边分区最后一个大于pivot的元素，
        //也是需要被交换到右边分区的元素，所以i往后遍历时，一旦发现比pivot小的值,
        //则和nums[pIndex]进行交换
        if(nums[i] < pivot) {
            swap(nums[pIndex], nums[i]);
            pIndex += 1;
        }
    }

    swap(nums[pIndex], nums[right]);
    return pIndex;  
}
```

复杂度分析

- 原地排序算法，空间复杂度为O(1)。
- 时间复杂度：在大部分情况下的时间复杂度都可以做到 O(nlogn)，只有在极端情况下，才会退化到 O(n2)。
- 稳定性：分区过程中相等元素的原有相对位置可能会改变，因此不是稳定的。

## 1.7. 堆排序(Heap Sort)

堆排序图示

![堆排序图示]()

基本思路

代码实现

```c++
```

复杂度分析

- 非原地排序算法，空间复杂度为O(nlogn)。
- 时间复杂度：O(n)。
- 稳定性：

适用场景

## 1.8. 希尔排序

希尔排序图示

![基数排序图示]()

基本思路

代码实现

```c++
```

复杂度分析

- 非原地排序算法，空间复杂度为O(n)。
- 时间复杂度：O(n)。
- 稳定性：

适用场景

## 1.9. 桶排序(Bucket Sort)

桶排序图示

![桶排序图示](https://static001.geekbang.org/resource/image/98/ae/987564607b864255f81686829503abae.jpg)

基本思路

代码实现

```c++
```

复杂度分析

- 非原地排序算法，空间复杂度为O(n)。
- 时间复杂度：O(n)。
- 稳定性：

适用场景
桶排序比较适合用在外部排序中。所谓的外部排序就是数据存储在外部磁盘中，数据量比较大，内存有限，无法将数据全部加载到内存中。

## 1.10. 计数排序(Counting Sort)

计数排序图示

![计数排序图示](https://static001.geekbang.org/resource/image/1d/84/1d730cb17249f8e92ef5cab53ae65784.jpg)

基本思路

- 根据上图进行说明，A[8]是需要排序的原数组，C[6]存放的是原数组中小于等于下标值的元素个数（理解数组C中元素值的意义很重要，比如C[2]=4，它代表了原数组中<=2的元素有4个，那么原数组中最后一个2在排序后数组中就应该是第4个，所以下标位置就应该是3），R[8]是用于存放排序结果的临时数组。排序过程：从后往前遍历原数组A[8]，遍历当前值为v，则C[v]-1的值就是v在排序后的数组中的下标位置i,然后将v写入到R[i]，写完值后C[v]需要减一。最后将临时数组的排序结果写会原数组位置即可。

代码实现

```c++
int countingsort(vector<int> &nums) {
    cout<<"***********[Begin]Counting sort**********"<<endl;
    printVec(nums);
    int size = nums.size();
    if (size < 2) return 0;

    //找出数组元素的最大值--确定值范围0 - max
    int max = nums[0];
    for (auto ite = nums.begin(); ite != nums.end(); ++ite)
    {
        if (*ite > max) max = *ite;
    }

    //统计原数组中每个值的个数，值就是countVec的下标
    vector<int> countVec(max + 1, 0);
    for (auto ite = nums.begin(); ite != nums.end(); ++ite)
    {
        countVec[*ite] += 1;
    }

    //累加countVec的值后，原数组中每个值v1作为countVec索引，
    //该位置的值v2代表的就是原数组中 <= v1的元素个数
    for (int i = 1; i < max + 1; ++i)
    {
        countVec[i] += countVec[i - 1];
    }

    //用于临时存放排序的结果
    vector<int> tempVec(size, 0);
    for (int i = size - 1; i >= 0; --i)
    {
        int v = nums[i];
        tempVec[countVec[v] - 1] = v;
        countVec[v] -= 1;
    }

    //将临时排序结果写回原数组
    for (int i = 0; i < size; ++i)
    {
        nums[i] = tempVec[i];
    }

    printVec(nums);
    cout<<"***********[End]Counting sort**********"<<endl;
    return 0;
}
```

复杂度分析

- 非原地排序算法，空间复杂度为O(n)。
- 时间复杂度：O(n)。
- 稳定性：在我们的代码实现中，因为是从后往前完成遍历来查找元素的合适位置，相同元素值的数组索引也是递减的，也就保证了原数组中多个相同的元素值其相对位置关系不会改变，因此是稳定的。

适用场景

- 计数排序只能用在数据范围不大的场景中，如果数据范围 k 比要排序的数据 n 大很多，就不适合用计数排序了。
- 计数排序只能给非负整数排序，如果要排序的数据是其他类型的，要将其在不改变相对大小的情况下，转化为非负整数再排序，但是这个过程又必然增加大量对于计算机比较耗时的乘除运算，所以要更加仔细考虑是否选择计数排序了。

## 1.11. 基数排序(Radix Sort)

基数排序图示
![基数排序图示](https://upload-images.jianshu.io/upload_images/13491454-1d3af296e1793427.gif?imageMogr2/auto-orient/strip|imageView2/2/w/1012)

基本思路

代码实现

```c++
```

复杂度分析

- 非原地排序算法，空间复杂度为O(n)。
- 时间复杂度：O(n)。
- 稳定性：

适用场景

- 基数排序对要排序的数据是有要求的，需要可以分割出独立的“位”来比较，而且位之间有递进的关系，如果 a 数据的高位比 b 数据大，那剩下的低位就不用比较了。除此之外，每一位的数据范围不能太大，要可以用线性排序算法来排序，否则，基数排序的时间复杂度就无法做到 O(n) 了。

## 1.12. 如何选择合适的排序算法

先来看看前面学习的这几种排序算法的对比：
![对比](https://img-blog.csdnimg.cn/20181210101518784.png?x-oss-process=image/watermark,type_ZmFuZ3poZW5naGVpdGk,shadow_10,text_aHR0cHM6Ly9ibG9nLmNzZG4ubmV0L015cm9uQ2hhbQ==,size_16,color_FFFFFF,t_70)

- 在时间复杂度为O(n^2) 的三种算法中，插入排序显然优于冒泡和选择排序。但是前面也提过，O(n^2)的算法只适用于小规模数据的排序。
- 桶排序、计数排序和基数排序属于线性排序算法，基本上都只有O(n)的时间复杂度，但是仅限于特殊的数据场景，所以不太适用于来写通用的排序函数。
- 为了兼顾任意场景下的数据排序，大多数场景下选择复杂度为O(nlogn)的几种排序算法更为合理。

## 参考链接

[visualgo-算法动态过程演示](https://visualgo.net/en)
