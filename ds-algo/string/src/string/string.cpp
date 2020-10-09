#include "string.hpp"
#include <string.h>
#include "../common/common.hpp"

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

/**
 * RK 算法的全称叫 Rabin-Karp 算法，是由它的两位发明者 Rabin 和 Karp 的名字来命名的
 * RK 算法的思路：通过哈希算法对主串中的 n-m+1 个子串分别求哈希值，然后逐个与模式串的哈希值比较大小。
 * 如果某个子串的哈希值与模式串相等，那就说明对应的子串和模式串匹配了，考虑到哈希冲突，如果某个子串的哈希值与模式串的哈希值相等，
 * 则继续比较子串和模式串是否是真的相等。因为哈希值是一个数字，数字之间比较是否相等是非常快速的，所以模式串和子串比较的效率就提高了。
*/
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

void StringMatchTest()
{
    cout<<"***********[Begin] StringMatchTest**********"<<endl;
    //                                |matched here         
    string str("dfgsdfdsfgsabcabdabcabdddabcabcdfgdsf");
    string parten("abcabdddabcabc");                        
    BFMatch(str, parten);
    RKMatch(str, parten);
    KMPMatch(str, parten);
    cout<<"***********[End] StringMatchTest**********"<<endl;
}