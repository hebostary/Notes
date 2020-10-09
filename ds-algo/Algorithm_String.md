# 字符串匹配算法

## BF算法

BF 算法中的 BF 是 Brute Force 的缩写，中文叫作暴力匹配算法，也叫朴素匹配算法。

比方说，我们在字符串 A 中查找字符串 B，那字符串 A 就是主串，字符串 B 就是模式串。我们把主串的长度记作 n，模式串的长度记作 m。因为我们是在主串中查找模式串，所以 n > m。

作为最简单、最暴力的字符串匹配算法，BF 算法的思想可以用一句话来概括，那就是，我们在主串中，检查起始位置分别是 0、1、2…n-m 且长度为 m 的 n-m+1 个子串，看有没有跟模式串匹配的，如下图所示：

![BF算法](https://static001.geekbang.org/resource/image/f3/a2/f36fed972a5bdc75331d59c36eb15aa2.jpg)

在极端情况下，比如主串是“aaaaa…aaaaaa”（省略号表示有很多重复的字符 a），模式串是“aaaaab”。我们每次都比对 m 个字符，要比对 n-m+1 次，所以，这种算法的最坏情况时间复杂度是 O(n*m)。

尽管理论上，BF 算法的时间复杂度很高，是 O(n \* m)，但在实际的开发中，它却是一个比较常用的字符串匹配算法。为什么这么说呢？原因有两点。

1. 实际的软件开发中，大部分情况下，模式串和主串的长度都不会太长。而且每次模式串与主串中的子串匹配的时候，当中途遇到不能匹配的字符的时候，就可以就停止了，不需要把 m 个字符都比对一下。所以，尽管理论上的最坏情况时间复杂度是 O(n*m)，但是，统计意义上，大部分情况下，算法执行效率要比这个高很多。
2. 朴素字符串匹配算法思想简单，代码实现也非常简单，不易出错。

下面给出基于C++的实现：

```c++
int BFMatch(const string str, const string parten)
{
    cout<<"[BFMatch] finding "<< parten << " in " << str <<endl;
    int index = -1;
    const char* sChar = str.c_str();
    const int sLen = str.size();
    const char* pChar = parten.c_str();
    const int pLen = parten.size();
    for (size_t i = 0; i < sLen-pLen+1; i++)
    {
        size_t step = 0;
        for ( ; step < pLen; step++)
        {
            if (sChar[i+step] != pChar[step]) break;
        }
        if (step == pLen) //说明所有的字符比较都相等
        {
            index = i;
            break;
        }
    }

    cout<<"[BFMatch] found index "<< index <<endl;
    return index;
}
```

## RK算法

RK 算法的全称叫 Rabin-Karp 算法，是由它的两位发明者 Rabin 和 Karp 的名字来命名的。

RK 算法的思路：通过哈希算法对主串中的 n-m+1 个子串分别求哈希值，然后逐个与模式串的哈希值比较大小。如果某个子串的哈希值与模式串相等，那就说明对应的子串和模式串匹配了，考虑到哈希冲突，如果某个子串的哈希值与模式串的哈希值相等，则继续比较子串和模式串是否是真的相等。因为哈希值是一个数字，数字之间比较是否相等是非常快速的，所以模式串和子串比较的效率就提高了。

![RK算法比较哈希值](https://static001.geekbang.org/resource/image/01/ee/015c85a9c2a4adc11236f9a40c6d57ee.jpg)

不过，通过哈希算法计算子串的哈希值的时候，我们需要遍历子串中的每个字符。尽管模式串与子串比较的效率提高了，但是，算法整体的效率并没有提高。有没有方法可以提高哈希算法计算子串哈希值的效率呢？

我们假设要匹配的字符串的字符集中只包含 K 个字符，我们可以用一个 K 进制数来表示一个子串，这个 K 进制数转化成十进制数，作为子串的哈希值。比如要处理的字符串只包含 a～z 这 26 个小写字母，那我们就用二十六进制来表示一个字符串。我们把 a～z 这 26 个字符映射到 0～25 这 26 个数字，a 就表示 0，b 就表示 1，以此类推，z 表示 25。用常规的进制转换计算就可以将这样的长度为K的字符串转换成10进制整数：

![RK算法哈希值计算2](https://static001.geekbang.org/resource/image/d5/04/d5c1cb11d9fc97d0b28513ba7495ab04.jpg)

这个计算哈希值的方法依然需要遍历字符串中的每个字符，但是它有一个特点：在主串中，相邻两个子串的哈希值的计算公式有一定关系，相邻两个子串 s[i-1]和 s[i]（i 表示子串在主串中的起始位置，子串的长度都为 m），对应的哈希值计算公式有交集，也就是说，我们可以使用 s[i-1]的哈希值很快的计算出 s[i]的哈希值。

![](https://static001.geekbang.org/resource/image/f9/f5/f99c16f2f899d19935567102c59661f5.jpg)

用公式表示如下：

![RK算法哈希值计算优化公式](https://static001.geekbang.org/resource/image/c4/9c/c47b092408ebfddfa96268037d53aa9c.jpg)

还有一个可优化的点：那就是 26^(m-1) 这部分的计算，我们可以通过查表的方法来提高效率。我们事先计算好 26^0、26^1、26^2……26^(m-1)，并且存储在一个长度为 m 的数组中，公式中的“次方”就对应数组的下标。当我们需要计算 26 的 x 次方的时候，就可以从数组的下标为 x 的位置取值，直接使用，省去了计算的时间。

整个 RK 算法包含两部分：

1. 计算子串哈希值：可以通过设计特殊的哈希算法，只需要扫描一遍主串就能计算出所有子串的哈希值了，所以这部分的时间复杂度是 O(n)。
2. 模式串哈希值与子串哈希值之间的比较：模式串哈希值与每个子串哈希值之间的比较的时间复杂度是 O(1)，总共需要比较 n-m+1 个子串的哈希值，所以，这部分的时间复杂度也是 O(n)。

所以，RK 算法整体的时间复杂度就是 O(n)。

> 但是，基于这种哈希计算方法的RK算法有一个不容忽视的问题：如果模式串稍微长一点，通过上面的哈希算法计算得到的哈希值就可能很大，很容易超过计算机中整型数据可以表示的范围。比如我们用[在线工具](https://tool.oschina.net/hexconvert) 来计算26进制的41111111111111（对应长度为14的字符串ebbbbbbbbbbbbb），其10进制值为：10023857607743097000，这已经超过C++ long long类型的最大值9223372036854775807，不可小觑啊。
>
> 那该如何解决呢？可以选择新的哈希值计算方法，比如假设字符串中只包含 a～z 这 26 个英文字母，那我们每个字母对应一个数字，比如 a 对应 1，b 对应 2，以此类推，z 对应 26。我们可以把字符串中每个字母对应的数字相加，最后得到的和作为哈希值。这种哈希算法产生的哈希值的数据范围就相对要小很多了，也同样可以利用子串临接关系来进行优化，但是发生哈希冲突的概率大幅增加。解决方法很简单，当我们发现一个子串的哈希值跟模式串的哈希值相等的时候，我们只需要再对比一下子串和模式串本身就好了。

下面给出基于C++的实现，我选择了回避哈希值过大的问题，而是牺牲了一些性能去处理哈希冲突：

```c++
int RKMatch(const string str, const string parten)
{
    cout<<"[RKMatch] finding "<< parten << " in " << str <<endl;
    int index = -1;
    const char* sChar = str.c_str();
    const int sLen = str.size();
    const char* pChar = parten.c_str();
    const int pLen = parten.size();
    int partenHash = 0;
    int subStrHash = 0;
    for (size_t i = 0; i < pLen; i++) 
    {
        //首先计算出模式串和第一个子串的哈希值
        partenHash += pChar[i]-'a';
        subStrHash += sChar[i]-'a';
    }

    for (size_t i = 0; i < sLen-pLen+1; i++)
    {
        if (i > 0)
        {
            //每次向前查找子串时，只需要减去前一个字符的值，加上尾部字符的值，就可以减少很多重复计算
            subStrHash -= sChar[i-1]-'a';
            subStrHash += sChar[i+pLen-1]-'a';//i的最大取值为sLen-pLen，(sLen-pLen) + (pLen-1) = sLen-1
        }
        if (subStrHash == partenHash)
        {
            //为解决哈希冲突，哈希值相等时，继续比较子串和模式串
            size_t step = 0;
            for ( ; step < pLen; step++)
            {
                if (sChar[i+step] != pChar[step]) break;
            }
            if (step == pLen) //说明所有的字符比较都相等
            {
                index = i;
                break;
            }
        }
    }

    cout<<"[RKMatch] found index "<< index <<endl;
    return index;
}
```

## KMP算法

KMP算法原理参考：[KMP算法教程](https://ruanx.net/kmp/) 和[34 | 字符串匹配基础（下）：如何借助BM算法轻松理解KMP算法？](https://time.geekbang.org/column/article/71845)

```c++
int KMPMatch(const string str, const string parten)
{
    cout<<"[KMPMatch] finding "<< parten << " in " << str <<endl;
    int index = -1;
    const char* sChar = str.c_str();
    const int sLen = str.size();
    const char* pChar = parten.c_str();
    const int pLen = parten.size();

    vector<int> next = getNexts(pChar);
    int pIndex = 0; //表示在模式串中的索引（下标）
    for (int sIndex = 0; sIndex < sLen; sIndex++)
    {
        while (pIndex > 0 && sChar[sIndex] != pChar[pIndex])
        {
            //遇到了坏字符，模式串中索引0 -> pIndex-1的子串就是当前的好前缀
            //k = next[pIndex-1] 表示当前好前缀子串的前缀和后缀的最大公共子串长度为 k
            //因此直接从模式串索引等于 k 的地方开始继续比较
            pIndex = next[pIndex-1];
            cout << "To comparie str["<< sIndex<<"] and parten["<<pIndex<<"]"<<endl;
        }

        //字符相等，在模式串中的匹配比较位置向后移动 1 位，0 -> pIndex这个子串在母串中匹配成功
        if(sChar[sIndex] == pChar[pIndex]) ++pIndex;
        
        //在母串中找到匹配模式串
        /*
        a b a d d w r //母串,匹配成功时sIndex = 4
            a d d     //模式串。匹配成功时sIndex = 3
        */
        if(pIndex == pLen) {
            index = sIndex - pIndex + 1;
            break;
        }
    }
    
    cout<<"[KMPMatch] found index "<< index <<endl;
    return index;
}

//计算next数组是KMP算法的核心
vector<int> getNexts(const char* pChar)
{
    int len = strlen(pChar);
    vector<int> next(len);
    if(len < 1) return next;
    
    //这里一定要注意对next[0]的处理，
    //最好直接设置next[0] = 0，然后在后面的循环中跳过对next[0]的处理
    next[0] = 0;
    for(int pIndex = 1; pIndex < len; pIndex++)
    {
        //now是 0 -> pIndex-1这个子串的前后缀最大公共子串的长度
        //比如 abcab 这个模式串的前缀：
        //     a : now=0
        //    ab : now=0,pIndex=1,pChar[pIndex]='b',next[pIndex]=0
        //   abc : now=0,pIndex=2,pChar[pIndex]='c',next[pIndex]=0
        //  abca : now=0,pIndex=3,pChar[pIndex]='a',next[pIndex]=1
        // abcab : now=1,pIndex=4,pChar[pIndex]='b',next[pIndex]=2
        int now = next[pIndex-1];
        while (now != 0 && pChar[now] != pChar[pIndex])
        {
            now = next[now - 1];
        }
        next[pIndex] = pChar[now] != pChar[pIndex] ? 0 : now+1;
    }

    printVec(next);
    return next;
}
```

## 测试BF/RK/KMP

测试前面介绍的BF/RK/KMP算法实现：

```c++
void StringMatchTest()
{       
    string str("dfgsdfdsfgsabcabdabcabdddabcabcdfgdsf");
    string parten("abcabdddabcabc");                        
    BFMatch(str, parten);
    RKMatch(str, parten);
    KMPMatch(str, parten);
}
/*output
[BFMatch] finding abcabdddabcabc in dfgsdfdsfgsabcabdabcabdddabcabcdfgdsf
[BFMatch] found index 17
[RKMatch] finding abcabdddabcabc in dfgsdfdsfgsabcabdabcabdddabcabcdfgdsf
[RKMatch] found index 17
[KMPMatch] finding abcabdddabcabc in dfgsdfdsfgsabcabdabcabdddabcabcdfgdsf
Print Vector: [0 0 0 1 2 0 0 0 1 2 3 4 5 3 ]
To comparie str[17] and parten[0]
[KMPMatch] found index 17
*/
```

## 基于Trie树的前缀查找

在搜索引擎中输入搜索关键词的过程中，它会自动提示很多可能的搜索选项。实现这个功能，我们需要考虑的最基本的事情包括：

1. 搜索引擎存储了一个搜索关键词的词库，也就是一个字符串的集合。
2. 根据输入的字符串作为前缀，在词库中快速查找匹配的搜索关键词。这个查找过程需要非常高效，就必须利用公共前缀的特点来进行存储所有搜索关键字。

![img](https://static001.geekbang.org/resource/image/ce/9e/ceb8738453401d5fc067acd513b57a9e.png)

**Trie树**，也叫“字典树”，是一种多叉树。它是一种专门处理字符串匹配的数据结构，用来解决在一组字符串集合中快速查找某个字符串的问题，非常适合用于这种场景。先来看以下Trie树的基本结构，假设我们有 6 个字符串，它们分别是：how，hi，her，hello，so，see，基于这6个字符串构造储来的Trie树如下：

![img](https://static001.geekbang.org/resource/image/28/32/280fbc0bfdef8380fcb632af39e84b32.jpg)

根结点不存储有效前缀，一般存储一个'/'字符。

Trie树的结构还是比较简单的，插入和查找的过程和普通的二叉树类似，从根结点向下逐层查找的过程就是前缀匹配的过程。关键在于，对于公共前缀，Trie树只会存储一份，理论上可以节省很多空间。

### Trie树的实现

下面讨论一种基于数组的Trie树存储结构：每个Trie树结点除了存储一个字符数据之外，还包含一个数组，这个数组的长度要求可以表示所有潜在的字符，这里假设只有'a'-'z'这26个字符，以'a'为索引位置0可以快速计算出每个字符对应的索引位置，每个数组元素指向下一级结点。比如，前面的6个字符串首字母分别包含了'h'和's'，所以根结点的数组里这两个字符对应的索引位置不为空，而其它位置的元素为NULL。

![img](https://static001.geekbang.org/resource/image/f5/35/f5a4a9cb7f0fe9dcfbf29eb1e5da6d35.jpg)

下面给出JAVA版的实现：

```java

public class Trie {
  private TrieNode root = new TrieNode('/'); // 存储无意义字符

  // 往Trie树中插入一个字符串
  public void insert(char[] text) {
    TrieNode p = root;
    for (int i = 0; i < text.length; ++i) {
      int index = text[i] - 'a';
      if (p.children[index] == null) {
        TrieNode newNode = new TrieNode(text[i]);
        p.children[index] = newNode;
      }
      p = p.children[index];
    }
    p.isEndingChar = true;
  }

  // 在Trie树中查找一个字符串
  public boolean find(char[] pattern) {
    TrieNode p = root;
    for (int i = 0; i < pattern.length; ++i) {
      int index = pattern[i] - 'a';
      if (p.children[index] == null) {
        return false; // 不存在pattern
      }
      p = p.children[index];
    }
    if (p.isEndingChar == false) return false; // 不能完全匹配，只是前缀
    else return true; // 找到pattern
  }

  public class TrieNode {
    public char data;
    public TrieNode[] children = new TrieNode[26];
    public boolean isEndingChar = false;
    public TrieNode(char data) {
      this.data = data;
    }
  }
}
```

这里只是实现了基本的Trie树构造和查找功能，而且是字符串的精确查找。对于前面的搜索引擎自动提示功能，还需要做大量改进，比如在匹配成功时，需要继续遍历所有的子结点，返回以查找字符串为前缀的所有字符串，叫做潜在搜索关键词。实际上搜索引擎肯定还会有更多更复杂的设计，比如对于一个比较短的搜索字符串，必然存在大量的潜在搜索关键词，直接返回所有结果肯定是不合适的，可能返回搜索频率最高的潜在搜索关键词会比较合适。

### Trie树分析

很容易分析得到，在Trie树中查找字符串的复杂度是O(k)，k是查找字符串的长度，这个效率是非常高的。

前面这个版本的Trie树存在一些问题：每个结点都需要存储一个固定长度的数组。如果字符串中包含从 a 到 z 这 26 个字符，那每个节点都要存储一个长度为 26 的数组，并且每个数组元素要存储一个 8 字节指针（或者是 4 字节，这个大小跟 CPU、操作系统、编译器等有关），总共208个字节。即便一个节点只有很少的子节点，远小于 26 个，比如 3、4 个，我们也要维护一个长度为 26 的数组，浪费很多内存空间。而且实际上，我们不可能只考虑26个小写字母的情况。

因此，我们可以牺牲一些插入和查找的性能，来提供内存空间的利用率，比如用有序数组，散列表或者红黑树来代替上面的普通数组。

对于常规的在一组字符串中查找字符串的问题，我们更倾向于直接使用散列表或者红黑树。实际上，Trie 树并不适合精确匹配查找，这种问题更适合用散列表或者红黑树来解决。**Trie 树比较适合的是查找前缀匹配的字符串，也就是类似搜索引擎自动提示关键词的那种场景。**

## AC自动机 - 多模式串匹配算法

前面的BF/RK/KMP算法都是在字符串中去查找单个模式串的算法。基于Trie树的前缀查找算法算是一种多模式串匹配算法，但是仅适用于类似于搜索引擎关键词提示的这种场景。考虑另外一种典型场景：**敏感词过滤**，这种场景需要在字符串中同时查找多个模式串，按照前面的KMP算法，只能针对每个模式串都进行一次匹配，显然整体效率会比较差。

### AC自动机工作原理

AC自动机是一种经典的多模式串匹配算法，特别适合于上面的敏感词查找场景。

在KMP算法中，主要思路在于匹配过程中遇到坏字符时，母串和模式串的好前缀是相同的，利用好前缀的预置信息一次滑动多位，减少对于好前缀子串的重复比较，并且匹配过程中母串字符的索引不用回退。所以核心在于根据模式串提前计算好next数组，遇到坏字符时，直接从next数组中取出可以滑动的位数。

AC自动机借鉴了KMP中next数组的思想和Trie树的设计，其核心设计是`失效指针`。AC自动机首先将所有需要待查找的模式串构造成一颗Trie树，每个树结点都有一个失效指针，这个失效指针指向当前模式串的后缀子串与其它模式串的前缀子串的`最长公共子串`的末尾节点，在母串匹配的过程中，如果字符匹配失败，就不用从头开始查找其它模式串，而是根据提前构造好的模式串信息（Trie树），直接开始对另一个最有可能匹配成功的模式串的查找，而且跳过已经匹配过的最长公共子串，和`KMP算法`的思想如出一辙。这里需要理解的是：

1. 最长公共子串如果存在，那么它一定是唯一的，这是由Trie树的特点保证的。
2. 最长公共子串如果不存在，那么失效指针就指向root结点。

下面这幅图就是一个已经构造好的AC自动机，其中包含的模式串有四个：she, he, his, hers。其中，旁边标记了模式串的结点表示这是一个模式串的尾部，和Trie树里的isEndingChar标记类似，这些模式串信息会被存储在树结点中。虚线就代表了每个结点的失效指针的指向。

![image-20200911205409398](https://hunk-pic-store.oss-cn-beijing.aliyuncs.com/img/image-20200911205409398.png)

假设我们现在在字符串'abshersm'中同时查找这4个模式串：

1. 对于前面两个字符'ab'，Trie树里没有以它们开始的模式串，所以匹配直接失败。
2. 匹配到'abshe'时，即访问到结点5，成功匹配两个模式串'she'和'he'，但是下一个字符是'r'，结点5没有'r'子结点，匹配失败。结点5的失效指针指向结点2，因为'she'和后缀子串集{'e', 'he'}和'he'的前缀子串集{'h', 'he'}的最长公共子串是'he'。成功匹配模式串'she'意味着从根结点到结点2的模式串'he'在母串中肯定存在，现在就可以从失效指针直接跳到结点2继续匹配，下一个字符是'r'，结点2正好有'r'子结点，如此继续访问到了结点9，成功匹配了模式串'hers'。下一个字符是'm'，匹配失败通过结点9的失效指针跳到结点3，结点3也没有'm'子结点，继续跳转到根结点，根结点也没有'm'子结点，匹配结束。

**结点5为什么要同时存储'he'和'she'两个模式串呢？**实际上，'he'这个模式串仅在结点2存储就足够了。这是一个编码上的优化，如果每个结点存储了其失效指针指向的模式串，那么通过失效指针跳转时，就不用再去访问下一个结点里的模式串数据了，直接查找其子结点就可以了。因为失效指针跳转后得到的模式串是固定的，因此这种预置处理很有意义。

此外，我们也并不需要在结点里真正的存储模式串，因为在匹配母串的过程中，索引位置是确定的，跳转后的模式串也一定是当前模式串的后缀子串，所以只要用整型数组记录模式串的长度，很容易就计算得到匹配成功的模式串在母串中的起始位置，这也是字符串匹配最需要的数据。

注意，图中的结点编号只是方便描述，并不是AC自动机所需要的有效信息。

### 构造AC自动机

构造AC自动机的过程会用到BFS，即图算法中的广度优先：

1. 首先依次处理所有的模式串，构造一颗Trie树，并在模式串的尾节点记录模式串长度，比如上图中的结点5首先存储一个3，表示'she'这个模式串。
2. 从根结点开始，按照BFS的顺序，依次添加失效指针。每个结点的失效指针都会参考其父结点的失效指针：
   1. 因为字符串的后缀子串不包含自己本身，所以根结点的第一层子结点的失效指针都指向根结点。
   2. 访问到结点2时，因为结点1的失效指针指向根结点，所以直接到根结点查找是否有'e'子结点，这里根结点没有'e'子结点，所以结点2的失效指针也指向根结点。
   3. 访问到结点5时，其父结点是结点4，结点4的失效指针指向结点1，已经有最长公共子串'h'，那么直接从结点1的子结点里查找字符'e'即可，刚好存在，所以结点5的失效指针指向结点2，并且发现结点2是模式串'he'的尾节点，那就在结点5记录其长度2。此时，结点5就记录了两个模式串信息。

学习到这里，我们发现，AC自动机的原理和KMP算法的思想非常类似，对于模式串的前缀和后缀的最长公共子串的理解至关重要。

关于AC自动机原理，推荐B站视频：[[算法]轻松掌握ac自动机](https://www.bilibili.com/video/BV1uJ411Y7Eg)。

C++代码实现：[**aho_corasick.cpp**](https://github.com/xiaoyazi333/data-structure-and-algorithm/blob/master/aho_corasick.cpp)

# References

[32 | 字符串匹配基础（上）：如何借助哈希算法实现高效字符串匹配？](https://time.geekbang.org/column/article/71187)

