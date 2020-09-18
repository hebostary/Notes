

# 贪心算法

# 分治算法

# 回溯算法

回溯算法实际上一个类似枚举的搜索尝试过程，主要是在搜索尝试过程中寻找问题的解，当发现已不满足求解条件时，就 “回溯” 返回，尝试别的路径。回溯法是一种选优搜索法，按选优条件向前搜索，以达到目标。但当探索到某一步时，发现原先选择并不优或达不到目标，就退回一步重新选择，这种走不通就退回再走的技术为回溯法，而满足回溯条件的某个状态的点称为 “回溯点”。许多复杂的，规模较大的问题都可以使用回溯法，有“通用解题方法”的美称。

深度优先搜索（DFS）实际上就是利用的回溯算法思想。回溯法本质上还是穷举所有可能性的暴力解法，但是它的确也是暴力解法的一个主要实现手段，尤其在暴力解法中发现使用`for`循环很难去编码实现的时候，就可以考虑回溯法了。

回溯算法一般都会用到递归解法（回溯是一种算法思想，递归是一种编码技巧），有人总结了一个比较通用的回溯法模板：

```c++
backtracking() { //递归函数
    if (终止条件) {
        存放结果;
    }

    for (枚举同一个位置的所有可能性，可以想成节点孩子的数量) {
        递归，处理节点;
        backtracking();
        回溯，撤销处理结果
    }
}

作者：carlsun-2
链接：https://leetcode-cn.com/problems/letter-combinations-of-a-phone-number/solution/17-dian-hua-hao-ma-de-zi-mu-zu-he-jing-dian-hui-su/
来源：力扣（LeetCode）
著作权归作者所有。商业转载请联系作者获得授权，非商业转载请注明出处。
```

## leetcode 17 - 电话号码的字母组合

### 题目

给定一个仅包含数字 `2-9` 的字符串，返回所有它能表示的字母组合。给出数字到字母的映射如下（与电话按键相同）。注意 1 不对应任何字母。

![image-20200916171809520](https://hunk-pic-store.oss-cn-beijing.aliyuncs.com/img/image-20200916171809520.png)

**示例**（任意选择答案输出顺序）：

```bash
输入："23"
输出：["ad", "ae", "af", "bd", "be", "bf", "cd", "ce", "cf"].
```

### 思路分析

以上面的输入`23`为例，数字'2'代表了'abc'，数字'3'代表了'def'。那么每个数字取任意一个可能字符，组合得到的就是输出字符串之一，比如'ad'，'ae'等，这也是数学中最简单的排列问题，一共有9种不同的字符串组合。

搜索所有可能性的路径其实就是下图中的这棵树，或者说在树中执行深度优先搜素：第一个数字'2'取字符'a'的时候，第二个数字有3中可能性，依次是'd'， 'e'，'f'，即左边最底层的3个叶子结点，访问完这3个叶子结点就得到了3个结果字符串'ad'，'ae'和'af'。然后就应该返回到树的上一层，继续探索数字'2'的下一个可能字符'b'，搜索过程和前面一样。

从编码实现的角度，整个探索过程就是递归遍历数字字符串的每一位数字，索引来到数字串末尾就是递归终止条件，在这个题目种意味着得到了一个答案字符串，可以将结果保存起来，然后回退一位数字继续探索，当倒数第二位数字的所有可能性都探索完成后，就再回退一位，以此类推。

![image-20200916172514679](https://hunk-pic-store.oss-cn-beijing.aliyuncs.com/img/image-20200916172514679.png)

### 代码实现

```c++
class Solution {
private:
    //存储每个数字所代表的可能字符组成的字符串
    const string letterMap[10] = {
        "",     //0
        "",     //1
        "abc",  //2
        "def",  //3
        "ghi",  //4
        "jkl",  //5
        "mno",  //6
        "pqrs", //7
        "tuv",  //8
        "wxyz"  //9
    };

    vector<string> res;

    void findLetterCombinations(string digits, int index, const string& str) {
        //index等于digits长度的时候，表示已经递归访问到了最后一个数字对应的其中一个字符。
        //即得到了一个可能的字符串答案，存到容器里，return后则继续处理最后一个数字的下一个可能字符。
        //所以回溯算法就是一种类似于深度优先搜索（DFS）的穷举思路，
        //把整个递归访问路径看作一棵树，那么每次访问到树的叶子结点都需要return
        if(index == digits.size()) {
            res.push_back(str);
            return;
        }

        assert(digits[index] >= '0' & digits[index] <= '9');

        //这个函数每次处理特定的index，下一个index上的数字有多种可能的字符，所以循环递归调用自己
        string letterMapStr = letterMap[ digits[index] -'0' ];
        for(int i = 0; i < letterMapStr.size(); i++) {
            findLetterCombinations(digits, index+1, str+letterMapStr[i]);
        }
    }

public:
    vector<string> letterCombinations(string digits) {
        if(digits.size() == 0) return res;

        findLetterCombinations(digits, 0, "");
        return res;
    }
};
```

### 复杂度

每位数字有3种可能性，所以复杂度就是3^n，可以表示为O(2^n)。

## Leetcode 93 - 复原IP地址

### 题目

给定一个只包含数字的字符串，复原它并返回所有可能的 IP 地址格式。

有效的 IP 地址 正好由四个整数（每个整数位于 0 到 255 之间组成，且不能含有前导 0），整数之间用 '.' 分隔。

例如："0.1.2.201" 和 "192.168.1.1" 是 有效的 IP 地址，但是 "0.011.255.245"、"192.168.1.312" 和 "192.168@1.1" 是 无效的 IP 地址。

**示例**：

```bash
输入：s = "25525511135"
输出：["255.255.11.135","255.255.111.35"]
```

### 思路分析

因为IP地址是固定的4段，所以这个问题的主要思路就在于每次选取一个子串作为IP地址的一个段，直到字符串最后一位刚好选取出4段，且选取的每个子串都要满足一些要求：

1. 长度大于等于1，但小于等于<3和剩下的子串长度的较小值>。
2. 数值大于1且小于等于255。
3. 开头不能包含连续两个'0'。

所以，每次选取一个子串时都需要检查是否是一个合法的IP地址段。刚好把整个字符串切割成4段，且每段都是合法的IP地址段，就得到一个合法IP地址，保存即可。

### 代码实现

下面`V1`是自己在Leetcode里第一次提交的版本，有很多优化的空间，Leetcode上有更多高效的实现。

```c++
//V1
class Solution {
private:
    vector<string> res;

public:
    vector<string> restoreIpAddresses(string s) {
        if(s.size() < 4) return res;

        findValidIpAddresses(s, 0, "", 0);

        return res;
    }

    void findValidIpAddresses(string& s, int index, const string& ipStr, int unitNum)
    {
        //这一行属于优化错误的切割：
        //1.前面切割得太短，将整个数字串切成了大于4段，
        //2.前面切割得太长，剩下的数字位数不够分段，
        //这两种情况都不可能得到合法的IP，提前返回
        if(unitNum > 4 || s.size()-index < 4-unitNum) return; 
        
        if(unitNum == 4 && index == s.size()) 
        {
            //将整个数字串刚好切成了4段，且每段都是合法的IP地址单元，保存结果
            res.push_back(ipStr);
            return;
        }

        int len = s.size()-index > 3 ? 3 : s.size()-index;
        for(int n =  1; n <= len; n++) //n表示此次选择的子串长度，应该大于等于1，并且小于等于3或者剩下的长度
        {
            string ipUnit = s.substr(index, n);
            if(isValidIpAddressUnit(ipUnit)) //检查子串是否是一个合法的IP段
            {
                string newIpStr = ipStr == "" ? ipUnit : ipStr+'.'+ipUnit;
                findValidIpAddresses(s, index+n, newIpStr, unitNum+1);
            }
        }
    }

    bool isValidIpAddressUnit(string& ipUnit)
    {
        bool result = false;
        int intUnit = atoi(ipUnit.c_str());
        if((ipUnit.size() <= 3) && (intUnit >= 0) && (intUnit <=255)) 
        {
            result = true;
        }
        
        if(ipUnit.size() > 1 && ipUnit[0] == '0') 
        {
            result = false;
        }
        return result;
    }
};
```

## Leetcode 51 - N皇后问题

### 题目

*n* 皇后问题研究的是如何将 *n* 个皇后放置在 *n*×*n* 的棋盘上，并且使皇后彼此之间不能相互攻击。下图为 8 皇后问题的一种解法：

![image-20200918200521608](https://hunk-pic-store.oss-cn-beijing.aliyuncs.com/img/image-20200918200521608.png)

给定一个整数 n，返回所有不同的 n 皇后问题的解决方案。

每一种解法包含一个明确的 n 皇后问题的棋子放置方案，该方案中 'Q' 和 '.' 分别代表了皇后和空位。

**示例**（4 皇后问题存在两个不同的解法）：

输入：4
输出：[
 [".Q..",  // 解法 1
  "...Q",
  "Q...",
  "..Q."],

 ["..Q.",  // 解法 2
  "Q...",
  "...Q",
  ".Q.."]
]

### 思路分析

首先n x n的棋盘里，必然每行每列里都会有一个皇后。否则，一定在某行或者某列存在冲突。根据回溯法的思想，逐行处理即可：

1. 首先处理第一行，皇后可能占据第一行的各个列的任意位置，针对每一列的情况，递归处理后面的行。
2. 对于每行的每个列位置，需要检查和前面已经放置过的皇后位置是否存在冲突：包括**列的正上方**，**左上对角线**以及**右上对角线**。

### 代码实现

```c++
class Solution {
private:
    //探索指定行的皇后位置
    void findPotentialSolution(const int n, const int row, vector<string>& chessboard, vector<vector<string>>& res) {
        if(row == n) {
            //最后一行都找到合适位置，说明已经找到一个合理的位置solution
            res.push_back(chessboard);
            return;
        }

        //对于指定行，探索在各个列放置皇后的合法位置
        for(int col = 0; col < n; col++) {
            if(isValidColLocation(n, row, col, chessboard)) {
                chessboard[row][col] = 'Q';
                findPotentialSolution(n, row+1, chessboard, res);
                chessboard[row][col] = '.'; //Important：因为棋盘容器是共享的，递归返回后需要清除之前的探索结果
            }
        }
    }

    bool isValidColLocation(const int n, const int row, const int col, vector<string>& chessboard) {
        //检查同一列上面各行的位置
        for(int i = row-1; i >= 0; i--) {
            if(chessboard[i][col] == 'Q') return false;
        }

        //检查左上对角线
        for(int i = row-1, j = col-1; i >= 0 && j >= 0; i--, j--) {
            if(chessboard[i][j] == 'Q') return false;
        }

        //检查右上对角线，注意row始终是减
        for(int i = row-1, j = col+1; i >= 0 && j < n; i--, j++) {
            if(chessboard[i][j] == 'Q') return false;
        }

        return true;
    }

public:
    vector<vector<string>> solveNQueens(int n) {
        vector<vector<string>> res;
        vector<string> chessboard(n, string(n, '.'));

        findPotentialSolution(n, 0, chessboard, res);

        return res;
    }
};
```







# 动态规划（Dynamic Programming）

