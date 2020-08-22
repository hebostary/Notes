#include "string.hpp"

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

void StringMatchTest()
{
    cout<<"***********[Begin] StringMatchTest**********"<<endl;
    //                                |matched here                                      
    BFMatch("asdhublasdbasfbkbghsabdgsaaabbccbgbasjgdajgldsl", "aabbcc");
    RKMatch("asdhublasdbasfbkbghsabdgsaaabbccbgbasjgdajgldsl", "aabbcc");
    cout<<"***********[End] StringMatchTest**********"<<endl;
}