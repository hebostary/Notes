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

测试前面介绍的BF和RK算法实现：

```c++
void StringMatchTest()
{
    //                                |matched here                                      
    BFMatch("asdhublasdbasfbkbghsabdgsaaabbccbgbasjgdajgldsl", "aabbcc");
    RKMatch("asdhublasdbasfbkbghsabdgsaaabbccbgbasjgdajgldsl", "aabbcc");
}

/*output
[BFMatch] finding aabbcc in asdhublasdbasfbkbghsabdgsaaabbccbgbasjgdajgldsl
[BFMatch] found index 26
[RKMatch] finding aabbcc in asdhublasdbasfbkbghsabdgsaaabbccbgbasjgdajgldsl
[RKMatch] found index 26
*/
```



# References

[32 | 字符串匹配基础（上）：如何借助哈希算法实现高效字符串匹配？](https://time.geekbang.org/column/article/71187)

