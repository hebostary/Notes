#include "Algorithm.hpp"
#include <list>
#include <numeric>
#include <functional>
#include <iostream>

using namespace std::placeholders;

void TestAlgorithms()
{
    EnterFunc(__FUNCTION__);
    // 只读算法
    TestFind();
    TestCount();
    TestAccumulate();

    //写容器元素的算法
    TestFill();
    TestCopy();

    //重排容器元素的算法
    TestSortAndUnique();
    TestPartition();

    //测试lambda表达式
    TestLambda();

    TestBind();

    //测试插入器
    TestInserter();

    //测试流迭代器
    TestStreamIterator();

    ExitFunc(__FUNCTION__);
}

void TestFind()
{
    EnterFunc(__FUNCTION__);
    //find返回指向第一个等于给定值的元素的迭代器
    //如果无匹配元素，则find返回第二个参数来表示搜索失败
    vector<int> vi = {1, 2, 4, 1, 3, 3 };
    int target = 10;
    auto result = find(vi.begin(), vi.end(), target);
    if ( result == vi.end() ) {
        cout << "find " << target << " failed." << endl;
    } else {
        cout << "find " << target << " success." << *result << endl;
    }
    
    target = 4;
    result = find(vi.begin(), vi.end(), target);
    if ( result == vi.end() ) {
        cout << "find " << target << " failed." << endl;
    } else {
        cout << "find " << target << " success." << *result << endl;
    }

    //查找长度大于5的元素，将返回第一个查找到的元素的迭代器
    vector<string> vs = {"asda", "asdasdas", "asd"};
    auto res = find_if(vs.begin(), vs.end(), longer);
    cout << "res: " << *res << endl;//asdasdas
    
    ExitFunc(__FUNCTION__);
}

void TestCount()
{
    EnterFunc(__FUNCTION__);
    //
    vector<int> vi = {1, 2, 4, 1, 3, 3 };
    int target = 10;
    auto result = count(vi.begin(), vi.end(), target);
    cout << "count 10: " << result << endl; //0

    list<string> ls = { "hunk", "jack", "bob", "hunk"};
    result = count(ls.begin(), ls.end(), "hunk");
    cout << "count hunk: " << result << endl; //2
    
    ExitFunc(__FUNCTION__);
}

//对迭代器范围内元素求和
void TestAccumulate()
{
    EnterFunc(__FUNCTION__);

    //accumulate的第三个参数的类型决定了函数中使用哪个加法运算符以及返回值的类型
    //因此，迭代器范围中的元素必须与第三个参数匹配，或者能够转换为第三个参数的类型

    vector<int> vi = {1, 2, 12};
    int sum = accumulate(vi.cbegin(), vi.cend(), 0);
    cout << "sum: " << sum << endl;//15

    vector<double> vd = {1.2, 2.33, 34.2};
    double sumd = accumulate(vd.cbegin(), vd.cend(), 0);
    cout << "sumd: " << sumd << endl;//37 注意丢失了精度，每个double元素都被转换成了int

    sumd = accumulate(vd.cbegin(), vd.cend(), double(0));
    cout << "sumd: " << sumd << endl;//37.73

    //string定义了 + 运算符，所以可以通过accumulate来连接容器内的字符串
    list<string> ls = { "Hello", " ", "Hunk", "!!!"};
    string res = accumulate(ls.cbegin(), ls.cend(), string(""));
    cout << "res: " + res << endl;

    //但是下面这种写法存在编译错误，因为空串将被当作字符串字面值传递，
    //用于保存和的对象类型将是const char*，但是const char*上没有定义+运算符
    //res = accumulate(ls.cbegin(), ls.cend(), "");

    ExitFunc(__FUNCTION__);
}

void TestFill()
{
    EnterFunc(__FUNCTION__);

    vector<string> vs = {"hunk", "df", "bob"};
    fill(vs.begin(), vs.end(), "A");
    cout << accumulate(vs.cbegin(), vs.cend(), string("")) << endl;//AAA

    fill_n(vs.begin(), 2, "B");//指定起始位置，修改固定个数的元素
    cout << accumulate(vs.cbegin(), vs.cend(), string("")) << endl;//BBA
    
    vector<int> vi;
    //ERROR: 下面这种写法是错误的
    //即在空容器上调用fill_n或类似的算法，编译检查通过但是将导致程序crash
    //fill_n(vi.begin(), 3, 1);
    fill_n(vi.begin(), vi.size(), 2);//这种调用是安全的

    //对于fill_n及类似的向目的位置迭代器写入数据的算法假定目的位置足够大，
    //能容纳要写入的元素，编程人员应该提供这种保证，否则算法的结果将是未定义的

    //back_inserter创建一个插入迭代器，因此fill_n的每次赋值都会在vi上调用push_back
    //所以最终效果就是在vi尾部插入了100个1
    fill_n(back_inserter(vi), 100, 1);
    cout << accumulate(vi.cbegin(), vi.cend(), 0) << endl;//100

    ExitFunc(__FUNCTION__);
}


void TestCopy()
{
    EnterFunc(__FUNCTION__);

    //用copy函数实现内置函数的拷贝
    int a1[] = {1, 3, 5, 7, 8};
    int a2[sizeof(a1) / sizeof(*a1)];
    copy(begin(a1), end(a1), a2);
    printContainer("a2", begin(a2), end(a2)); //1 3 5 7 8
    
    vector<int> vec; 
    list<int> lst = {1, 4, 5};
    //ERROR: 直接这样调用copy函数是一个错误的demo，问题和前面的fill_n函数一样
    //    程序员负责保证copy函数中目标容器有足够的空间存储拷贝的元素
    //    这里vec是没有初始化的，编译器无法发现这种问题，将在运行时导致程序crash
    //copy(lst.begin(), lst.end(), vec.begin());

    //用reserve函数使容器预留足够的空间便可以解决这个问题
    vec.reserve(lst.size());
    copy(lst.begin(), lst.end(), vec.begin());

    ExitFunc(__FUNCTION__);
}


//标准库算法对迭代器而不是容器进行操作。因此，算法不能直接添加或者删除元素
//删除元素可以通过调用容器本身的erase方法来做到
//添加元素可以用前面的back_inserter间接调用容器的插入函数来做到
void TestSortAndUnique()
{
    EnterFunc(__FUNCTION__);

    //利用sort函数和unique函数对容易元素去重
    vector<string> vs = {"andy", "this", "is", "this", "andy", "yum", "andy"};
    //对元素按字典序排序,sort算法默认使用元素类型的 < 运算符
    sort(vs.begin(), vs.end());
    printContainer("sorted", vs.begin(), vs.end());//andy, andy, andy, is, this, this, yum
    //unique重排输入范围，使得每个单词只出现一次
    //返回值unique_end指向最后一个不重复的元素
    auto unique_end = unique(vs.begin(), vs.end());
    printContainer("uniqued", vs.begin(), vs.end());//andy, is, this, yum, , this, 
    //调用容器的erase方法删除尾部重复的元素
    //即使最初vs里没有重复字符串，调用erase也是安全的，unique函数返回vs.end()
    vs.erase(unique_end, vs.end());
    printContainer("deduplicated", vs.begin(), vs.end());//andy, is, this, yum

    //按字符串元素的长度排序
    vector<string> vs1 = {"aaa", "aaaa", "ddd", "bbb", "cc"};
    vector<string> vs2 = {"aaa", "aaaa", "ddd", "bbb", "cc"};
    sort(vs1.begin(), vs1.end());//先字典序排序
    printContainer("sorted vs1: ", vs1.cbegin(), vs1.cend());//[aaa, aaaa, bbb, cc, ddd]
    sort(vs1.begin(), vs1.end(), isShorter);//按元素长度排序
    printContainer("sorted vs1: ", vs1.cbegin(), vs1.cend());//[cc, aaa, bbb, ddd, aaaa]

    //按长度排序的同时，相同长度的元素按字典序排列
    //stable_sort意为“稳定排序”，在排序的同时不会改变相同长度元素原有的相对位置
    sort(vs2.begin(), vs2.end());////[aaa, aaaa, bbb, cc, ddd]
    stable_sort(vs2.begin(), vs2.end(), isShorter);
    printContainer("stable_sort vs2: ", vs2.cbegin(), vs2.cend());//[cc, aaa, bbb, ddd, aaaa]

    ExitFunc(__FUNCTION__);
}

bool isShorter(const string &s1, const string &s2)
{
    return s1.size() < s2.size();
}

//partition函数将容器内满足谓词条件为true的元素移动到前面
void TestPartition()
{
    EnterFunc(__FUNCTION__);

    vector<string> vs = {"aa", "bbbbbbb", "sssss", "we"};

    //返回值指向最后一个使谓词为true的元素之后的位置
    //下面的例子将长度等于大于5的元素移动到容器的前面
    auto partition_pos = partition(vs.begin(), vs.end(), longer);
    printContainer("first partition elements: ", vs.begin(), partition_pos);//[sssss, bbbbbbb]
    printContainer("second partition elements: ", partition_pos, vs.end());//[aa, we]

    ExitFunc(__FUNCTION__);
}

bool longer(const string &str)
{
    return str.size() >= 5;
}

//一个lambda表达式表示一个可调用的代码单元
//可以将其理解为一个未命名的内联函数
void TestLambda()
{
    EnterFunc(__FUNCTION__);


    //一个lambda只有在其捕获列表中捕获一个它所在函数中的局部变量，才能在函数体中使用该变量
    //lambda表达式的返回类型可以省略，编译器会自动推导返回类型，但是如果有多个return语句，就需要显示指定返回类型
    vector<string> vs = {"q", "aa", "bbbbbbb", "sssss", "wwe"};
    size_t baseSize = 3;
    auto partition_pos = partition(vs.begin(), vs.end(), 
        [/*捕获列表*/baseSize](/*参数列表*/const string &s) /*返回值*/ -> int {
            return s.size() >= baseSize;
        });

    printContainer("first partition elements: ", vs.begin(), partition_pos);//[wwe, sssss, bbbbbbb]
    printContainer("second partition elements: ", partition_pos, vs.end());//[aa, q]

    //for_each算法接受一个可调用对象，并对输入序列中的每个元素调用此对象
    //捕获列表只用于局部非static变量，lambda可以直接使用局部static变量和
    //在它所在函数之外声明的名字，比如下面例子中的cout，定义在iostream头文件中
    //打印第一个分区内的元素
    for_each(vs.begin(), partition_pos,
        [](const string &s) { cout << s << " "; });//wwe sssss bbbbbbb
    cout << endl;

    //lambda表达式捕获变量时默认捕获的是值拷贝，但是也可以捕获引用
    //lambda表达式默认不修改捕获的变量，也可以通过mutable关键字来修改
    int v = 42;
    auto f = [&v]() mutable {
        cout << "print v: " << v << endl;
    };
    f(); //42

    v++;
    f(); //43

    ExitFunc(__FUNCTION__);
}

//在前面测试 partition 函数的例子中，我们给 partition 函数传入了longer函数，
//因为partition函数的第三个参数只能是一元谓词，即longger函数也只能有一个参数，所以比较的长度是硬编码的
//下面新的longerThanFlag函数接收一个新的长度参数，就不能再被partition函数直接使用
//这个时候就可以用bind函数来对longerThanFlag函数进行适配改造，达到和上面lambda捕获变量相同的效果
void TestBind()
{
    EnterFunc(__FUNCTION__);

    vector<string> vs = {"aa", "bbbbbbb", "sssss", "we"};

    //_n是占位符，可以有多个，定义在placeholders中
    int flagLen = 6;
    //这里longer是生成的一个可调用对象，它只接受一个参数，也就是_1位置的意义
    //默认情况下，那些不是占位符的参数（flagLen）被拷贝到这个可调用对象里
    //当调用longer的时候，这个对象会将flagLen参数封装在一起去调用longerThanFlag函数
    //最终效果和上面lambda表达式捕获参数的实现一样
    auto longer = bind(longerThanFlag, _1, flagLen);
    //bind函数也可以调整参数顺序：比如 auto caller = bind(func, _2,  _1, flagLen);
    auto partition_pos = partition(vs.begin(), vs.end(),  longer);
    printContainer("first partition elements: ", vs.begin(), partition_pos);//[bbbbbbb]
    printContainer("second partition elements: ", partition_pos, vs.end());//[aa, sssss, we]

    ExitFunc(__FUNCTION__);
}

bool longerThanFlag(const string &str, int flag)
{
    return str.size() >= flag;
}

void TestInserter()
{
    EnterFunc(__FUNCTION__);

    vector<string> vs = {"hunk", "bob", "alice"};

    auto ite = inserter(vs, vs.begin());//ite的类型为 std::insert_iterator<std::vector<std::__cxx11::string>>
    *ite = "jack";
    *ite = "amos";
    printContainer("Current vector: ", vs.begin(), vs.end());//[jack, amos, hunk, bob, alice]

    printContainer("Reversed vector: ", vs.crbegin(),  vs.crend());//[alice, bob, hunk, amos, jack]


    // 测试反向迭代器
    string str = "FIRST,MIDDLE,LAST";
    //查找逗号分隔的第一个子串
    auto commma = find(str.cbegin(), str.cend(), ',');
    cout << "fist sub string: " << string(str.cbegin(), commma) << endl;//FIRST

    auto rcommma = find(str.crbegin(), str.crend(), ',');//std::reverse_iterator
    //默认从后往前处理字符
    cout << "last sub string: " << string(str.crbegin(), rcommma) << endl;//TSAL
    //反向迭代器的base方法将其转换成普通迭代器
    cout << "last sub string: " << string(rcommma.base(), str.cend()) << endl;//LAST
    /*
    如下： [str.crbegin(), rcomma)和[rcomma.base(), str.cend())指向str中相同的元素范围
    cbegin()   comma     rcomma.base()  cend()
    |           |               |        |   
    v           v               v        v
    F I R S T   , M I D D L E , L A S T  
                              ^       ^
                              |       |
                            rcomma  crbegin()
    */

    ExitFunc(__FUNCTION__);
}

void TestStreamIterator()
{
    EnterFunc(__FUNCTION__);

    //TODO: 完成10.4.2的练习题

    ExitFunc(__FUNCTION__);
}