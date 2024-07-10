> 关于 C++ STL 建议在使用时多参考 https://hackingcpp.com/cpp/cheat_sheets.html 和 https://zh.cppreference.com/w/cpp

#  STL 函数

## move & forward

`std::move()` 函数用于*将任意类型的左值转换为其类型的右值引用*，其函数原型如下：

```c++
template <typename T>
typename remove_reference<T>::type&& move(T&& t){
    return static_cast<typename remove_reference<T>::type &&>(t);
}
```

首先回顾一下引用折叠的基本原理：

- 右值传递给上述函数的形参 T&& 依然是右值，即 T&& && 相当于 T&&。
- 左值传递给上述函数的形参 T&& 依然是左值，即 T&& & 相当于 T&。

通过引用折叠原理可以知道，`std::move()` 函数的形参既可以是左值也可以是右值。std::move 函数的处理流程：

1. 首先利用万能模板将传入的参数 t 进行处理，我们知道右值经过 T&& 传递类型保持不变还是右值，而左值经过 T&& 变为普通的左值引用，以保证模板可以传递任意实参，且保持类型不变；对参数 t 做一次右值引用，根据引用折叠规则，右值的右值引用是右值引用，而左值的右值引用是普通的左值引用。万能模板既可以接受左值作为实参也可以接受右值作为实参。
2. 通过 remove_refrence 移除引用，得到参数 t 具体的类型 type；
3. 最后通过 static_cast<> 进行强制类型转换，返回 type && 右值引用。

remove_reference 的主要作用是解除类型中引用并返回变量的实际类型，其具体实现如下：

```c++
//原始的，最通用的版本
template <typename T> struct remove_reference{
    typedef T type;  //定义 T 的类型别名为 type
};
 
//部分版本特例化，将用于左值引用和右值引用
template <class T> struct remove_reference<T&> //左值引用
{ typedef T type; }
 
template <class T> struct remove_reference<T&&> //右值引用
{ typedef T type; }   
  
//举例如下,下列定义的a、b、c三个变量都是int类型
int i;
remove_refrence<decltype(42)>::type a;             //使用原版本，
remove_refrence<decltype(i)>::type  b;             //左值引用特例版本
remove_refrence<decltype(std::move(i))>::type  b;  //右值引用特例版本 
```

`std::forward()` *保证了在转发时左值右值特性不会被更改，实现完美转发*。主要解决引用函数参数为右值时，传进来之后有了变量名就变成了左值。比如如下代码：

```c++
#include <iostream>
using namespace std;

template<typename T>
void fun(T&& tmp) { 
    cout << "fun rvalue bind:" << tmp << endl; 
} 

template<typename T>
void fun(T& tmp) { 
    cout << "fun lvalue bind:" << tmp << endl; 
} 

template<typename T>
void test(T&& x) {
    fun(x);
    fun(std::forward<T>(x));
}

int main() { 
    int a = 10;
    test(10); // fun lvalue bind:10；fun rvalue bind:10
    test(a); //fun lvalue bind:10； fun lvalue bind:10
    return 0;
}
```

参数 x 为右值，到了函数内部则变量名则变为了左值，我们使用 forward 即可保留参数 x 的属性。forward 函数实现如下:

```c++
  /**
   *  @brief  Forward an lvalue.
   *  @return The parameter cast to the specified type.
   *
   *  This function is used to implement "perfect forwarding".
   */
template<typename _Tp>
constexpr _Tp&&
forward(typename std::remove_reference<_Tp>::type& __t) noexcept
{ return static_cast<_Tp&&>(__t); }

/**
 *  @brief  Forward an rvalue.
 *  @return The parameter cast to the specified type.
 *
 *  This function is used to implement "perfect forwarding".
 */
template<typename _Tp>
constexpr _Tp&&
forward(typename std::remove_reference<_Tp>::type&& __t) noexcept
{
    static_assert(!std::is_lvalue_reference<_Tp>::value, "template argument"
        " substituting _Tp is an lvalue reference type");
    return static_cast<_Tp&&>(__t);
}
```

`std::forward()` 函数的处理流程:

1. forward 同样利用引用折叠的特性，对参数 t 做一次右值引用，根据引用折叠规则，右值的右值引用是右值引用，而左值的右值引用是普通的左值引用。forward 的实现有两个函数：
   1. 第一个，接受的参数是左值引用，只能接受左值。
   2. 第二个，接受的参数是右值引用，只能接受右值。
2. 根据引用折叠的原理：
   1. 如果传递的是左值，_Tp 推断为 T &，则返回变成static_cast，也就是 static_cast，所以返回的是左值引用。
   2. 如果传递的是右值，_Tp 推断为 T&& 或者 T，则返回变成 static_cast，所以返回的是右值引用。

*std::forward 与 std::move 最大的区别是，std::move 在进行类型转换时，利用 remove_reference 将外层的引用全部去掉，这样可以将 t 强制转换为指定类型的右值引用，而 std::forward 则利用引用折叠的技巧，巧妙的保留了变量原有的属性。*

> 参考：《Effective Modern C++》条款23

# STL IO流

为了支持不同类型的IO处理操作，标准库定义了3种用于IO的类，分别放在3个不同的头文件中：

| 头文件   | 类型                                                         | 用途                                             |
| -------- | ------------------------------------------------------------ | ------------------------------------------------ |
| iostream | istream, wistream / ostream, wostream / iostream, wiostream  | 从流读取数据 / 向流写入数据 / 读写流             |
| fstream  | ifstream, wifstream / ofstream, wofstream / fstream, wfstream | 从文件读取数据 / 向文件写入数据 / 读写文件       |
| sstream  | istringstream, wistringstream / ostringstream, wostringstream / stringstream, wstringstream | 从string读取数据 / 向string写入数据 / 读写string |

1. w开头的类型都是为了支持使用宽字符的语言，这些类型操纵的是wchar_t类型的数据。

2. iostream定义了用于读写流的基本类型，一般用于控制台的输入输出，比如常用的`cin`就是istream对象，用于从标准输入读取数据；`cout`和`cerr`都是ostream对象，前者用于向标准输出写入数据，后者用于向标准错误写入数据。

3. fstream定义了用于读写命名文件的流类型。

4. sstream定义了读写内存string对象的流类型。

非常重要的一点，这些不同的流类型，都支持统一的运算符：`>>`用于从一个流对象读取输入数据，`<<`用于向一个流对象写入输出数据。

>  *不能拷贝IO对象*，因此我们也不能将形参或者返回类型设置为流类型，进行IO操作的函数通常以引用的方式传递和返回流。

## 流的条件状态

IO操作一个与生俱来的问题是可能发生错误，部分错误可恢复，而那些系统深层次的错误，比如硬件故障，已经超出了应用程序的处理范围。IO库的流类型也定义了一些函数和标志，帮助应用程序访问和操纵流的条件状态。

```c++
//stream是一种IO类型
stream::iostate   //iostate是一种机器相关的类型，提供了表达条件状态的完整功能
stream::badbit    //用来指出流已经崩溃，表示发生系统级错误，如不可恢复的读写错误
stream::failbit   //用来指出一个IO操作失败了，表示发生可恢复错误，比如期望读取数值确读到字符
stream::eofbit    //用来支持流到达了文件结束，如果到达文件结束位置，eofbit和failbit都会被置位
stream::goodbit   //用来指出流未处于错误状态
    
//s是一个流对象
s.eof()              //若流s的eofbit置位，则返回true
s.fail()             //若流s的failbit置位，则返回true
s.bad()              //若流s的badbit置位，则返回true
s.good()             //若流s处于有效状态，则返回true
s.clear()            //将流s的所有条件状态位复位，将流的状态设置为有效。
s.setstate(flags)    //根据给定的flags标志位，将流s中对应条件状态位置位，flags类型为stream::iostate
s.rdstate()          //返回流s的当前条件状态，返回值类型为stream::iostate
```

只有当一个流处于无错状态时，才可以从它读取数据，向它写入数据。一个流一旦发生错误，其上后续的IO操作都会失败。由于流可能处于错误状态，因此代码通常应该在使用流之前检查它是否处于良好状态。确定一个流对象的状态的最简单的方法是将它当作一个条件来使用：

```c++
while ( cin >> word) {
    //ok; 读操作成功
}
//如果badbit，failbit和eofbit任意一个被置位，则检测流状态的条件会失败
```

复位指定的两个状态位 failbit 和 badbit，但保持eofbit不变：

```c++
cin.clear(cin.rdstate() & ~cin.failbit & ~cin.badbit);
```

下面的demo中，readFromIstream 函数从给定流中读取数据，直至遇到文件结束标识时停止，最后将流复位成有效状态返回。

```c++
istream& readFromIstream(istream& in )
{
    //注意不同系统的文件结束符是不一样的
    cout << "Please input some words and end with (ctl + z) in windows or (ctl + d) in linux:" << endl;
    vector<string> vs;
    string word;
    while ( in >> word )
    {
        vs.push_back(word);
    }
    if ( in.eof() ) {
        cout << "Reached EOF!" << endl;
    }

    printContainer("Readed words: ", vs.begin(), vs.end());

    in.clear();//对流进行置位，使其处于有效状态
    return in;
}
```

## 输出缓冲

每个输出流都管理一个缓冲区，用来保存程序读写的数据。有了缓冲机制，操作系统就可以将程序的多个输出操作组合成单一的系统级写操作，从而带来性能提升。 

导致缓冲刷新（即，数据真正写到输出设备或文件）的原因有很多：

1. 程序正常结束，作为 main 函数的return语句操作的一部分，缓冲刷新被执行。

2. 缓冲区满时，需要刷新缓冲，之后新的数据才能继续写入缓冲区。

3. 可以使用操作符来显示刷新缓冲区。比如`endl`操作符完成换行并刷新缓冲区的工作；`flush`操作符刷新缓冲区但不输出额外的字符；`ends`操作符向缓冲区插入一个空字符，然后刷新缓冲区。

   ```c++
   cout << "hi!" << endl;   //输出hi！和一个换行符，然后刷新缓冲区
   cout << "hi!" << flush;  //输出hi！然后刷新缓冲区
   cout << "hi!" << ends;   //输出hi！和一个空字符，然后刷新缓冲区
   ```

4. 如果想在每次输出操作后都刷新缓冲区，可以使用`unitbuf`操作符设置流的内部状态，它告诉流在接下来的每次写操作之后都进行一次`flush`操作。`nounitbuf`操作符用以重置流，使其恢复使用正常的系统管理的缓冲区刷新策略。

   ```c++
   cout << unitbuf;
   cout << nounitbuf;
   ```

   默认情况下，对cerr是设置`unitbuf`的，因此写道cerr的内容都是立刻刷新的。

5. 一个输出流可能被关联到另一个流。在这种情况下，当读写被关联的流时，关联到的流的缓冲区会被刷新。默认情况下，cin 和 cerr都关联到 cout。因此，读 cin 或写 cerr 都会导致 cout 的缓冲区被刷新。

> 如果程序异常终止，输出缓冲区是不会被刷新的。当一个程序崩溃后，它所输出的数据很可能停留在输出缓冲区中等待打印。

## 文件流 fstream

头文件fstream定义了三个流类型来支持文件IO：ifstream用于从一个给定文件读取数据；ofstream用于向一个给定文件写入数据；fstream可以读写给定文件。

当我们读写一个文件时，可以定义一个文件流对象，并将对象与文件关联起来。每个文件流类型都定义了一个名为 open 的成员函数，它完成一些系统相关的操作，来定位给定的文件，并视情况打开为读或写模式。

创建文件流对象时，如果提供了一个文件名，则open会自动被调用。如果没有指定文件名，创建了空文件流对象，可以随后调用 open 来将它与文件关联起来。为了将文件流关联到另外一个文件，必须首先调用 close 函数关闭已经关联的文件，之后可以用 open 关联新的文件。

每个流都有一个关联的文件模式，用来指出如何使用文件。这些文件模式包含:

```c++
in     //以读方式打开，只可以对ifstream或fstream对象设定out模式
out    //以写方式打开，只可以对ofstream或fstream对象设定in模式
       //注意：默认情况下，即使没有指定trunc，以out模式打开的文件也会被截断
       //为了保留以out模式打开的文件的内容，必须同时指定app模式，或者同时指定in模式（即打开文件同时进行读写操作）
       //demo：out.open("file", ofstream::app);
app    //append，每次写操作前均定位到文件末尾，只要trunc没被设定，就可以设定app模式
       //在app模式下，即使没有指定out模式，文件也总是以输出方式被打开。
ate    //打开文件后立即顶定位到文件末尾
trunc  //截断文件
binary //以二进制方式进行IO，ate和binary模式可用于任何类型的文件流对象，且可以与其它任何文件模式组合使用
```

每个文件流类型都定义了默认的文件模式（ifstream -> in模式， ofstream -> out模式，fstream -> in和out模式），当我们未指定文件模式时，就是用默认模式。所以，默认情况下，用ofstream打开一个文件时，文件的内容会被丢弃。

使用IO流完成文件输入输出的基本用法：

```c++
void Testiofstream()
{
    //创建一个文件流，并写入几行数据
    //与ofstream关联的文件默认以 out 和 trunc的模式打开，文件的内容会被丢弃
    //阻止ofstream清空给定文件内容的方法是同时指定 app 模式
    string fn = "output.txt";
    ofstream ofs(fn);
    if ( ofs )
    {   //检查打开文件流是否成功，因为open调用可能失败
        ofs << "hunk\njack\nbob" << endl;
        ofs.close();
    }

    //一旦一个文件流已经打开，它就保持与对应文件的关联
    //对一个已经打开的文件流调用会失败，failbit被置位
    //调用close关闭文件后，流对象可以重新打开的新的文件
    ofs.open( fn + ".copy");
    if ( ofs )
    {   //打开文件流成功
        ofs << "hunk\njack\nbob" << endl;
        ofs.close();
    }

    //从文件流读取数据
    //与ifstream关联的文件默认以 in 模式打开
    ifstream ifs( fn + ".copy");
    if ( ifs )
    {
        vector<string> vs;
        string buf;
        while ( getline( ifs, buf ) ) //每次读取一行
        {
            vs.push_back(buf);
        }
        ifs.close();
        printContainer("Read data from file: ", vs.cbegin(), vs.cend());
    }
}
```

## 字符串流 sstream

sstream头文件定义了三个流类型来支持内存IO：istringstream从string读取数据，ostringstream向string写入数据，stringstream既可以从 string 读数据也可以向 string 写数据。这些类型都继承自前面iostream头文件中定义的流类型，然后添加了一些特有操作：

```c++
sstream strm;    //strm是一个未绑定的stringstream对象
sstream strm(s); //strm是一个sstream对象，保存string s的一个拷贝，此构造函数时explicit的
strm.str();      //返回strm保存的string的拷贝
strm.str(s);     //将string s拷贝到strm中
```

下面给出一个字符串流的使用场景：用一个 istringstream 对象作为从文件中读取的一行数据的缓冲，然后再从这个缓冲里处理逐个字符串。

```c++
void Teststringstream()
{
    string fn("persons.record");
    ofstream ofs( fn );
    if ( ofs )
    {
        ofs << "jack 1234567 12323543543\n";
        ofs << "hunk 34436456 12323543\n";
        ofs << "bob 39444567 1233543\n";
        ofs.close(); //关闭文件并将缓存刷到文件
    }

    ifstream ifs( fn );
    if ( ifs )
    {
        string line, word;
        vector<PersonInfo> people;
        while ( getline(ifs, line) )
        {
            PersonInfo info;
            istringstream record(line); //将记录绑定到刚读入的行
            record >> info.name;
            while ( record >> word )
            {
                info.phones.push_back(word);
            }
            people.push_back(info);
        }
        cout << "read people nmber: " << people.size() << endl; //3
    }
}
```

# STL 容器

关于各类STL容器支持的操作类型参考： [Containers library](https://en.cppreference.com/w/cpp/container)

## 迭代器

C++的迭代器是一种抽象的设计概念，在设计模式中有迭代器模式，即提供一种方法，使之能够依序寻访某个容器所含的各个元素，而无需暴露该容器的内部表述方式。迭代器只是一种概念上的抽象，具有迭代器通用功能和方法的对象都可以叫做迭代器。迭代器有很多不同的能力，可以把抽象容器和通用算法有机的统一起来。迭代器基本分为下面五种：

1. 输入迭代器（Input Iterator）：只能向前单步迭代元素，不允许修改由该迭代器所引用的元素；
2. 输出迭代器（Output Iterator）：只能向前单步迭代元素，对由该迭代器引用的元素只有写权限；
3. 向前迭代器(Forward Iterator)：该迭代器可以在一个区间中进行读写操作，它拥有输入迭代器的所有特性和输出迭代器的部分特性，以及向前单步迭代元素的能力；
4. 双向迭代器(Bidirectional Iterator)：在向前迭代器的基础上增加了向后单步迭代元素的能力；
5. 随机访问迭代器(Random Access Iterator)：不仅综合以上 4 种迭代器的所有功能，还可以像指针那样进行算术计算；

![image-20240416163558553](https://hunk-pic-store.oss-cn-beijing.aliyuncs.com/img/20240509175834.png)

在C++ STL中：容器 vector、 deque 提供随机访问迭代器； list 提供双向迭代器；set 和 map 提供向前迭代器；而 stack、queue、priority-queue 不支持迭代器。

**使用迭代器的优点：**

1. 代码编写方便：迭代器提供了统一接口来遍历元素，不用担心容器的大小，使用迭代器我们可以简单地使用成员函数 end() 来判断容器的结尾，遍历内容方便而简洁；
2. 代码可重用性高：：迭代器提供了一组通用的接口访问和遍历容器中的元素。迭代器支持代码的可重用性，它们可以被使用访问任何容器的元素。
3. 容器可以动态处理：迭代器能够在需要时方便地从容器中动态添加或删除元素。

### 迭代器失效问题及建议

只读方法决不会使迭代器或引用[失效](https://zh.cppreference.com/w/cpp/iterator#.E5.8F.AF.E8.A7.A3.E5.BC.95.E7.94.A8.E6.80.A7.E4.B8.8E.E6.9C.89.E6.95.88.E6.80.A7)。修改容器内容的方法可能会使迭代器和/或引用失效，如下表所示：

![image-20240509203354110](https://hunk-pic-store.oss-cn-beijing.aliyuncs.com/img/20240509203354.png)

**插入**指代任何添加一或多个元素到容器的方法，而**擦除**指代任何从容器移除一或多个元素的方法。

- 插入方法的例子是 [std::set::insert](https://zh.cppreference.com/w/cpp/container/set/insert)、[std::map::emplace](https://zh.cppreference.com/w/cpp/container/map/emplace)、[std::vector::push_back](https://zh.cppreference.com/w/cpp/container/vector/push_back) 和 [std::deque::push_front](https://zh.cppreference.com/w/cpp/container/deque/push_front)。
- 擦除方法的例子是 [std::set::erase](https://zh.cppreference.com/w/cpp/container/set/erase)、[std::vector::pop_back](https://zh.cppreference.com/w/cpp/container/vector/pop_back)、[std::deque::pop_front](https://zh.cppreference.com/w/cpp/container/deque/pop_front) 和 [std::map::clear](https://zh.cppreference.com/w/cpp/container/map/clear)。`clear` 会使所有迭代器和引用失效。因为它会擦除所有元素，这在技术上遵照上述规则。

> 使用失效的迭代器，指针或引用是严重的运行时错误，很可能引起与使用未初始化指针一样的问题。

关于使用迭代器的一些建议：

1. 尽量最小化要求迭代器必须保持有效的代码段。

2. 由于向迭代器添加和从迭代器删除元素的代码可能会使迭代器失效，因此必须保证每次改变容器的操作之后都正确地重新定位迭代器，对 `vector`，`string` 和 `deque`尤为重要。如果循环中调用的是 insert 或 erase，都会返回一个新的迭代器，那么更新迭代器就很容易，比如：

   ```c++
   //在循环中删除偶数元素，复制每个奇数元素
   vector<int> vi = {0, 1, 2, 3 ,4};
   auto ite = vi.begin();
   while( ite != vi.end() ) {
       if(*ite % 2) { 
           ite = vi.insert(ite, *ite); //复制当前元素并更新迭代器
           ite += 2;//记得跳过刚插入的新元素
       } else {
           ite = vi.erase(ite); //删除偶数元素
           //不用向前移动迭代器，ite指向我们删除的元素之后的元素
       }
   }
   ```

3. 在一个循环中插入或删除`vector`，`string` 和 `deque` 中的元素，**不要缓存 end() 返回的迭代器**，比如下面的错误写法：

   ```c++
   auto begin = v.begin(); end = v.end();
   while (begin != end ) {
       //处理begin指向的元素
       ++begin();//向前移动begin，因为我们想在其后面插入元素
       begin = v.insert(begin, 42); //插入新值
       ++begin; //跳过刚加入的元素
   }
   ```

   安全的做法（通常C++标准库的实现中 end() 操作都很快，不用担心性能问题）：

   ```c++
   auto begin = v.begin();
   while (begin != v.end()) {
       //处理begin指向的元素
       ++begin();//向前移动begin，因为我们想在其后面插入元素
       begin = v.insert(begin, 42); //插入新值
       ++begin; //跳过刚加入的元素
   }
   ```

## 顺序容器

### 顺序容器概览

顺序容器提供控制元素存储和访问顺序的能力，这种顺序不依赖于元素的值，而是与元素加入容器时的位置相对应。

| 类型                 | 特点                                                         | 底层数据结构 |
| -------------------- | :----------------------------------------------------------- | ------------ |
| vector               | 动态连续数组；支持快速（O(1)）随机访问（元素保存在连续的内存空间中）；在尾部之外的位置插入或删除元素可能很慢，实践复杂度为O(n)；通过push_back增加元素时，如果初始分配空间已满，则以2倍空间扩容（GCC）：首先申请一个2倍大的内存空间，然后将原空间的内容拷贝过来，最后释放原空间内存。 | 数组         |
| array（C++11）       | 静态连续数组（编译时确定，不能添加或删除元素）；支持快速随机访问；与内置数组相比，array是一种更安全，更容易使用的数组类型。 | 数组         |
| deque                | 双端队列；支持快速随机访问。*在两端位置插入/删除速度很快*，在中间位置添加或删除元素的代价很高。 | 双向队列     |
| list                 | 双链表；不支持随机访问，支持双向顺序访问。在list中任何位置进行插入/删除速度都很快。空间的额外开销比较多。 | 双向链表     |
| forward_list C++11） | 单链表；不支持随机访问，只支持单向顺序访问。在链表任何位置进行插入/删除操作都很快。空间的额外开销比较多。 | 单向链表     |

### 顺序容器CURD

#### 添加元素

| 操作               | 功能                                                         | 返回值                                          | 注释                                                    |
| ------------------ | ------------------------------------------------------------ | ----------------------------------------------- | ------------------------------------------------------- |
| push_back(t)       | 在容器尾部创建一个值为t的元素                                | void                                            | array和forward_list不支持                               |
| emplace_back(args) | 在容器尾部创建一个由args构造的元素                           | void                                            | 元素类型需要支持和args匹配的构造函数                    |
| push_front(t)      | 在容器头部创建一个值为t的元素                                | void                                            |                                                         |
| push_front(args)   | 在容器头部创建一个由args构造的元素                           | void                                            | 元素类型需要支持和args匹配的构造函数                    |
| insert(p, t)       | 在迭代器p指向的元素之前创建一个值为t的元素                   | 指向新添加元素的迭代器                          | c.push_front("hello")等价于c.insert(c.begin(), "hello") |
| emplace(p, args)   | 在迭代器p指向的元素之前创建一个由args构造的元素              | 指向新添加元素的迭代器                          |                                                         |
| insert(p, n, t)    | 在迭代器p指向的元素之前插入n个值为t的元素                    | 指向新添加的第一个元素的迭代器，n等于0则返回p   |                                                         |
| insert(p, b, e)    | 将迭代器b和e指定范围内的元素插入到p指向的元素之前。b和e不能指向容器中的元素 | 指向新添加的第一个元素的迭代器，范围为空则返回p |                                                         |
| insert(p, il)      | il是一个花括号包围的元素值列表，将列表元素插入到p指向的元素之前 | 指向新添加的第一个元素的迭代器，列表为空则返回p |                                                         |

> 向一个vector，string 或 deque 插入元素会使所有指向容器的迭代器，引用和指针失效。

将元素插入到 vector，deque 和string中的任何位置都是合法的，但是在一个vector或string的尾部之外的任何位置，或是一个deque的首尾之外的任何位置添加元素，都需要移动元素，这样做可能很耗时。此外，向一个vector或string添加元素可能引起整个对象存储空间的重新分配。重新分配一个对象的存储空间需要分配额外的内存，并将元素从旧的空间移动到新的空间中。

#### 访问元素

| 操作    | 功能                                                         | 注释               |
| ------- | ------------------------------------------------------------ | ------------------ |
| back()  | 返回容器尾元素的引用。若c为空，函数行为未定义                | forward_list不支持 |
| front() | 返回容器首元素的引用。若c为空，函数行为未定义                |                    |
| c[n]    | 返回容器下标为n的元素的引用。若 n >= size()，函数行为未定义  |                    |
| at(n)   | 返回容器下标为n的元素的引用。若下标越界，抛出`out_of_range`异常 |                    |

在容器中访问元素的4个成员函数返回的都是引用。如果容器是一个`const`对象，则返回值是`const`的引用，不能修改元素。

提供快速随机访问的容器（`string`、`vector`、`deque` 和 `array`）也都提供下标运算符，其它顺序容器不支持下标操作。*保证下标有效是程序员的责任，下标运算符并不检查是否在合法范围内，使用越界的下标是一种严重的程序设计错误，而且编译器不检查这种错误。*

*对一个空容器调用 front() 和 back()，就像使用一个越界的下标一样，也是一种严重的程序设计错误*。因此，使用它们的时候注意检查容器是否为空，比如下面的例子：

```c++
if (!c.empty()) {
    c.front() = 42;
    auto &v = c.back();  //获得指向最后一个元素的引用
    v = 24;              //改变c中的元素
    auto v2 = c.back();  //v2是c.back()的一个拷贝
    v2 = 0;              //未改变c中的元素
}
```

#### 删除元素

| 操作        | 功能                                                     | 返回值                                                       |
| ----------- | -------------------------------------------------------- | ------------------------------------------------------------ |
| pop_back()  | 删除容器尾元素；若容器为空，则函数行为未定义             | void                                                         |
| pop_front() | 删除容器首元素；若容器为空，则函数行为未定义             |                                                              |
| erase(p)    | 删除迭代器p所指定的元素，若p是尾后迭代器，函数行为未定义 | 返回一个指向被删元素之后元素的迭代器。若p指向尾元素，则返回尾后迭代器。 |
| erase(b, e) | 删除迭代器b和e所指定范围内的元素                         | 返回一个指向最后一个被删元素之后元素的迭代器。若e本身就是尾后迭代器，则函数也返回尾后迭代器 |
| clear()     | 删除容器所有元素                                         |                                                              |

> 删除deque中除首尾元素之外的任何元素都会使所有迭代器，引用和指针失效。指向vector或string中的删除点之后位置的迭代器，引用和指针都会失效。

删除元素的成员函数并不检查其参数，在删除元素之前，必须确保它（们）是存在的，比如：

```c++
while (!ilist.empty()) {
    process(ilist.front()); //对ilist的首元素进行一些处理
    ilist.pop_front(); //完成处理后删除首元素
}
```

顺序容器的删除操作都是基于迭代器位置的删除，不支持根据给定元素值来直接删除。

#### 容量管理

| 操作            | 功能                                                         | 返回值 | 注释 |
| --------------- | ------------------------------------------------------------ | ------ | ---- |
| empty()         | 检查容器是否为空。                                           |        |      |
| size()          | 返回元素数。                                                 |        |      |
| max_size()      | 返回根据系统或库实现限制的容器可保有的元素最大数量，即对于最大容器的 [std::distance](http://zh.cppreference.com/w/cpp/iterator/distance)(begin(), end())；和实际RAM大小有关，更多细节参考[max_size](https://zh.cppreference.com/w/cpp/container/deque/max_size)。 |        |      |
| shrink_to_fit() | 请求释放未使用的内存空；非强制性，请求是否达成取决于实现；所有迭代器和引用都会失效。 |        |      |
| resize(n)       | 调整容器大小为n个元素。若 n < size()，多出的元素被丢弃。若必须添加新元素，对新元素进行值初始化。 |        |      |
| resize(n, t)    | 调整容器大小为n个元素。任何新添加元素都初始化为t。           |        |      |

`array`是固定大小容器，不支持resize操作。

> 如果 resize() 缩小容器，则指向被删除元素的迭代器、引用和指针都会失效；对 `vector`，`string` 或 `deque` 进行resize() 可能导致迭代器、指针和引用失效。

### vector

#### vector自动扩容机制

为了支持快速随机访问，`vector`和`string`将元素连续存储，即每个元素挨着前一个元素存储。另一方面，它们的大小是可变的，考虑向`vector`和`string`中插入新元素时，如果剩余空间不足，容器需要首先分配一块更大的存储空间，然后拷贝所有已有元素，最后插入新元素。当容器元素很多时，重新分配内存空间的性能开销会很大。

为了防止频繁的重新分配内存空间，`vector`和`string`会采用预留空间的策略：当不得不获取新的内存空间时，`vector`和`string`的实现通常会一次性分配比新的空间需求更大的内存空间。容器预留这些空间作为备用，可用来保存更多的新元素。

关于容量管理的一些操作：

| 操作            | 功能                                           | 返回值 | 注释                                                  |
| --------------- | ---------------------------------------------- | ------ | ----------------------------------------------------- |
| shrink_to_fit() | 将 capacity() 减少为与 size() 相同大小         |        | 调用shrink_to_fit只是一个请求，标准库并不保证退还内存 |
| capacity()      | 不重新分配内存空间的话，容器可以保存的元素数目 |        |                                                       |
| reserve(n)      | 分配至少能容纳n个元素的内存空间                |        |                                                       |

`reserve`并不改变容器中元素的数量，它仅影响 `vector` 预先分配多大的内存空间。只有当需要的内存空间超过当前容量时，reserve 调用才会改变 `vector` 的容量：

1. 如果需求大小大于当前容量，reserve至少分配与需求一样大的内存空间（可能更大）。
2. 如果需求大小小于或等于当前容量，reserve什么也不做。**特别是，当需求大小小于当前容量时，容器不会退回内存空间**。因此，在调用reserve之后，实际capacity将会大于或等于传递给reserve的参数。

一些简单测试：

```c++
vector<int> vi;
cout<<"size: "<<vi.size()<<", capacity: " << vi.capacity() << " \n";//0, 0
vi.push_back(1);
cout<<"size: "<<vi.size()<<", capacity: " << vi.capacity() << " \n";//1, 1
vi.push_back(3); vi.push_back(5); vi.push_back(7);
cout<<"size: "<<vi.size()<<", capacity: " << vi.capacity() << " \n";//4, 4
vi.push_back(8);
cout<<"size: "<<vi.size()<<", capacity: " << vi.capacity() << " \n";//5, 8
vi.reserve(12);
cout<<"size: "<<vi.size()<<", capacity: " << vi.capacity() << " \n";//5, 12
vi.shrink_to_fit();
cout<<"size: "<<vi.size()<<", capacity: " << vi.capacity() << " \n";//5, 5
```

每个 `vector` 实现都可以选择自己的内存分配策略，一般是分配当前空间2倍大小的空间。

### deque

`std::deque`（double-ended queue，双端队列）是有索引的序列容器，允许在它的首尾两端快速插入及删除。另外，在 `deque` 任一端的插入或删除不会使指向其余元素的指针或引用失效。

与 [std::vector](https://zh.cppreference.com/w/cpp/container/vector) 相反，`deque` 的元素不是连续存储的：典型实现采用一系列单独分配的固定尺寸数组，外加额外的簿记数据，这表示对 `deque` 的索引访问必须进行二次指针解引用，与之相比 `vector` 的索引访问只进行一次。

`deque` 的存储按需自动扩张及收缩。扩张 `deque` 比扩张 [std::vector](https://zh.cppreference.com/w/cpp/container/vector) 更轻便，因为它不涉及将既存元素复制到新内存位置。

> 关于 `deque` 的底层原理可以参考：[C++ deque/queue/stack的底层原理](https://blog.csdn.net/TABE_/article/details/122830968)

### list

`std::list` 是支持从容器任何位置进行常数时间的元素插入和移除的容器。不支持快速随机访问。它通常实现为双向链表。与 [std::forward_list](https://zh.cppreference.com/w/cpp/container/forward_list) 相比，此容器提供双向迭代但在空间上效率稍低。

在 `list` 内或在数个 `list` 间添加、移除和移动元素不会使迭代器或引用失效。迭代器只有在对应元素被删除时才会失效。

### forward_list

`std::forward_list` 是支持从容器中的任何位置快速插入和移除元素的容器。不支持快速随机访问。它实现为单链表，且实质上与其在 C 中的实现相比无任何开销。与 [std::list](https://zh.cppreference.com/w/cpp/container/list) 相比，此容器在不需要双向迭代时提供更好的存储空间效率。

对于链表而言，添加或者删除一个元素时，添加或删除的元素之前的那个元素的后继会发生改变。为了添加或删除一个元素，我们需要访问其前驱。但是`forward_list`是单链表，没有简单的办法获取一个单链表元素的前驱。因此，在`forward_list`中添加或者删除元素的操作是通过改变给定元素之后的元素来完成的。由于这些操作和其它容器上的操作的实现方式不同，`forward_list`并未定义 insert，emplace和erase操作，而是定义了下面这些操作。

| 操作                       | 功能                                                         | 返回值                       | 注释 |
| -------------------------- | ------------------------------------------------------------ | ---------------------------- | ---- |
| lst.before_begin()         | 返回指向链表首元素之前不存在的元素的迭代器，此迭代器不能解引用 |                              |      |
| lst.cbefore_begin()        | 返回一个const_iterator                                       |                              |      |
| lst.insert_after(p, t)     | 在迭代器p之后的位置插入元素t。如果范围为空，则返回p；如果p是尾后迭代器，函数行为未定义，这也适用于后面几个重载版本。 |                              |      |
| lst.insert_after(p, n, t)  | 在迭代器p之后的位置插入n个元素t，如果p是尾后迭代器，函数行为未定义 | 指向最后一个插入元素的迭代器 |      |
| lst.insert_after(p, b, e)  | 在迭代器p之后的位置插入迭代器范围元素，b和e不能指向lst内     | 指向最后一个插入元素的迭代器 |      |
| lst.insert_after(p, il)    | 在迭代器p之后的位置插入花括号列表内的元素                    | 指向最后一个插入元素的迭代器 |      |
| lst.emplace_after(p, args) | 在迭代器p之后的位置创建一个元素，如果p是尾后迭代器，函数行为未定义 | 指向新元素的迭代器           |      |
| lst.erase_after(p)         | 删除p指向的位置之后的元素，如果p指向尾元素或者是尾后迭代器，函数行为未定义 | 指向被删元素之后元素的迭代器 |      |
| lst.erase_after(b, e)      | 删除迭代器范围内元素，如果p指向尾元素或者是尾后迭代器，函数行为未定义 | 指向被删元素之后元素的迭代器 |      |

当在forward_list中添加或删除元素时，必须关注两个迭代器，一个指向要处理的元素，另一个指向其前驱，比如下面从链表中删除奇数的例子：

```c++
forward_list<int> flist = {1, 3, 6, 7, 12, 34, 37};
auto prev = flist.before_begin(); //指向首前元素
auto curr = flist.begin();
cout << "flist: ["; for (auto &&w : flist) cout << w << ", "; cout << "]\n";
while (curr != flist.end())
{
    //删除奇数元素
    if (*curr % 2) {
        curr = flist.erase_after(prev);
    } else {
        prev = curr;
        ++curr;
    }
}
cout << "flist: ["; for (auto &&w : flist) cout << w << ", "; cout << "]\n";
```

### string

string 是一种与 vector 相似的容器，专门用于保存字符，底层也是数组，随机访问快；在尾部插入/删除速度快。

**字符串的初始化**

```c++
/*
    字符串的初始化
    */
const char* cstr = "Hello World";//C style字符串，以'\0'结束
char noNull[] = {'H', 'i'}; //字符数组，不是以'\0'结束

//拷贝cstr中的字符直到遇到空字符
string s1(cstr);
cout<<"s1: "<< s1 <<endl;

string s2(noNull, 2);//只拷贝两个字符
cout<<"s2: "<< s2 <<endl;

string s3(noNull);//未定义：noNull不是以空字符结束
cout<<"s3: "<< s3 <<endl;//Hi

string s4(s1, 6);
cout<<"s4: "<< s4 <<endl;//World
```

**翻转字符串**

```c++
cout<<"s4: "<< s4 <<endl;//World

reverse(s4.begin(), s4.end());
cout<<"s4: "<< s4 <<endl;//dlroW
```

**截取字符串**

```c++
string subs1 = s1.substr(0, 5);
string subs2 = s1.substr(6);
cout<<"subs1: "<< subs2 <<endl;//World
cout<<"subs2: "<< subs2 <<endl;//World
```

**修改字符串**

```c++
/*
    修改string的一些操作
    */
string s11 = "Hello World";
s11.insert(s11.size(), 5, '!'); //在字符串尾部插入5个感叹号
cout<<"s11: "<< s11 <<endl;//Hello World!!!!!
s11.erase(s11.size()-5, 5);//删除尾部5个字符
cout<<"s11: "<< s11 <<endl;//Hello World

//string还提供了接受C风格字符串的insert版本和assign版本
const char* cp = "Stanly, plump Buck";
string s12 = "Hello World";
s12.assign(cp, 7);//从cp里拷贝7个字符来替换s12的内容
cout<<"s12: "<< s12 <<endl;//Stanly,
s12.insert(s12.size(), cp + 7);
cout<<"s12: "<< s12 <<endl;//Stanly, plump Buck
ExitFunc(__FUNCTION__);

//apend操作是在字符串尾部进行插入操作的一种的简写形式
string s13("C++ Primer"), s14 = s13;
s13.insert(s13.size(), " 5th Ed.");
s14.append(" 5th Ed.");//这两种追加字符串的效果是一样的
cout<<"s13: "<< s13 <<endl;
cout<<"s14: "<< s14 <<endl;

//replace操作是调用erase和insert的一种简写形式
s13.erase(11, 3);
cout<<"s13: "<< s13 <<endl;//C++ Primer  Ed.
s13.insert(11, "6th");
cout<<"s13: "<< s13 <<endl;//C++ Primer 6th Ed.
//等价的replace操作
s14.replace(11, 3, "Fifth");//允许替换不同长度的子串
cout<<"s14: "<< s14 <<endl;//C++ Primer Fifth Ed.
```

**字符串查找**

```c++
/*
    string查找操作
        1.查找操作是大小写敏感的
    */
string s21 = "Hello World";
auto pos1 = s21.find("Hunk");
auto pos2 = s21.find("World");
cout << "Find Hunk position: " << int(pos1) << endl; //-1
cout << "Find World position: " << pos2 << endl; //6

//查找与给定字符串中任何一个字符串匹配的位置
string numbers("0123456789"), name("aqzwsxr2d2");
auto pos3 = name.find_first_of(numbers);
auto pos4 = name.find_first_not_of(name.substr(0, 7));
cout << "pos3: " << pos3 << endl;//7
cout << "pos4: " << pos4 << endl;//7
```

**数值转换**

```c++
/*
    数值转换
        1.如果string不能转换为一个数值，这些函数抛出一个invalid_argument异常
        2.如果转换得到的额数值无法用任何类型来表示，则抛出一个out_of_range异常
    */   
int i = 12345;
//将整形转换为字符串
string s31 = to_string(i);
cout << "s31: " << s31 << endl;// 12345
//将字符串转为浮点值
double d = stod(s31);
cout << "(d + 0.2) = " << d + 0.2 << endl;// 12345.2
string s32 = "pi = 3.14";
d = stod(s32.substr(s32.find_first_of("+-0123456789")));
cout << "d = " << d << endl;//3.14
```

### 如何选择顺序容器？

![flowchart for selecting an appropriate standard library container](https://hackingcpp.com/cpp/design/which_std_sequence_container_crop.svg)

## 容器适配器 Adapter

适配器是标准库的通用概念，容器，迭代器和函数都有适配器。本质上，一个适配器是一种封装机制，能使某种事物的行为看起来像另外一种事物一样。

基于最基础的顺序容器，标准库还定义了三个顺序容器适配器：`stack`, `queue`, `priority_queue`。vector/deque/list等基础顺序容器，都和底层存储结构紧密关联，比如使用数组或者链表，以及实现动态的空间管理。容器适配器表达的是纯逻辑结构，比如stack表达的是一种先进后出（FILO）的结构，queue表达的是先进先出(FIFO)的结构，priority_queue表达的是元素优先级，而它们可以直接基于不同的基础顺序容器来实现，或者说对满足需求的顺序容器进行一层包装即可。

默认情况下，stack 和 queue是基于deque实现的，priority_queue 是在 vector 之上实现的。

```c++
stack<int> stk(deq); //从deq拷贝元素到stk
```

同时也支持在创建适配器时将一个命名的顺序容器作为第二个类型参数，来重载默认容器类型。

```c++
stack<string, vector<string>> str_stk;
```

但是对于给定的适配器，可以使用哪些基础容器是有限制的。**所有适配器都要求容器具有添加和删除元素以及访问尾元素的能力**。因此，适配器不能构造在array和forward_list之上。

1. stack只要求push_back，pop_back和back操作，因此可以使用除array和forward_list之外的任何容器类型来构造。
2. queue要求back，front和push_front，因此可以基于list或者deque构造，但不能基于vector。
3. priority_queue除了front，push_back和pop_back操作之外还要求随机访问能力，因此可以构造于vector或deque之上，但不能基于list构造。

### stack

`stack` 类是一种容器适配器，主要提供 `栈` 这种先进后出（FILO）的数据结构。`stack` 模板类的定义如下，它表现为底层容器的包装器，只提供特定函数集合。

```c++
template<
    class T,
    class Container = std::deque<T>
> class stack;
```

模板形参里的 Container 是用于存储元素的底层容器类型，它必满足顺序容器的要求，并且需要提供一些通常语义的函数，包括 back()、push_back()、pop_back()。标准容器 [std::vector](https://zh.cppreference.com/w/cpp/container/vector)（包括 [`std::vector`](https://zh.cppreference.com/w/cpp/container/vector_bool)）、[std::deque](https://zh.cppreference.com/w/cpp/container/deque) 和 [std::list](https://zh.cppreference.com/w/cpp/container/list) 满足这些要求。如果没有为特定的 `stack` 类特化指定容器类，那么就会使用标准容器 [std::deque](https://zh.cppreference.com/w/cpp/container/deque)。

`stach` 提供的常用访问接口：

1. top()：访问栈顶元素。
2. pop()：移除栈顶元素。
3. push()：向栈顶插入元素。
4. emplace()：在顶部原味构造元素。
5. size()：返回元素数量。
6. empty()：检查 stack 是否为空。

### queue

`std::queue` 类也是一种[容器适配器](https://zh.cppreference.com/w/cpp/container#.E5.AE.B9.E5.99.A8.E9.80.82.E9.85.8D.E5.99.A8)，它提供[队列](https://en.wikipedia.org/wiki/Queue_(abstract_data_type))的功能——尤其是 FIFO（先进先出）数据结构。`queue` 模板类定义如下：

```c++
template<
    class T,
    class Container = std::deque<T>
> class queue;
```

模板形参里的 Container 是用于存储元素的底层容器类型，它必满足顺序容器的要求，并且提供一些通常语义的函数，包括 back()、front()、push_back()、pop_front()。标准容器 [std::deque](https://zh.cppreference.com/w/cpp/container/deque) 和 [std::list](https://zh.cppreference.com/w/cpp/container/list) 满足这些要求。如果没有为特定的 `queue` 类特化指定容器类，那么就会使用标准容器 [std::deque](https://zh.cppreference.com/w/cpp/container/deque)。

### priority_queue

`priority_queue`相当于一个有权值的单向队列`queue`，在这个队列中，所有元素都是按照优先级排列的。`priority_queue ` 根据`堆(Heap)`的处理规则来调整元素之间的位置。根据堆的特性，优先级队列实现了取出最大最小元素的时间复杂度为 O(1)。对于插入和删除元素，其最坏情况为 O(lgn)。

> `priority_queue` 默认是最大堆，即 top() 返回最大元素。

标准容器 [std::vector](https://zh.cppreference.com/w/cpp/container/vector)（包括 [`std::vector`](https://zh.cppreference.com/w/cpp/container/vector_bool)）和 [std::deque](https://zh.cppreference.com/w/cpp/container/deque) 可以用作 `priority_queue` 的底层容器，默认是用 vector。

`priority_queue` 的使用案例参考：https://zh.cppreference.com/w/cpp/container/priority_queue

## 关联容器 map & set

关联容器和顺序容器有着本质的不同，关联容器中的元素是按键（关键字）来保存和访问的，而顺序容器中的元素是按照元素在容器中的位置来顺序保存和访问的。

### 关联容器概览

标准库提供下面8个关联容器类型:

| 容器类型           | 功能                                    | 底层数据结构 | 复杂度                       | 头文件        |
| ------------------ | --------------------------------------- | ------------ | ---------------------------- | ------------- |
| map                | 关联数组，保存键-值对，键是唯一且有序的 | 红黑树       | 查找、插入、删除均为O(lgn)   | map           |
| set                | 键即值，即只保存键的容器，键唯一且有序  | 红黑树       | 查找、插入、删除均为O(lgn)   | set           |
| multimap           | 键可重复出现的map，键是有序的           | 红黑树       | 查找、插入、删除均为O(lgn)   | map           |
| multiset           | 键可重复出现的set，键是有序的           | 红黑树       | 查找、插入、删除均为O(lgn)   | set           |
| unordered_map      | 用哈希函数组织的map，键唯一但无序       | 哈希表       | 查找：O(n)，考虑哈希碰撞另说 | unordered_map |
| unordered_set      | 用哈希函数组织的set，键唯一但无序       | 哈希表       |                              | unordered_set |
| unordered_multimap | 哈希组织的map，键可以重复出现           | 哈希表       |                              | unordered_map |
| unordered_multiset | 哈希组织的set，键可以重复出现           | 哈希表       |                              | unordered_set |

这8个容器间的不同体现在三个维度上：

1. 是一个map，或者是一个set。map存储键值对，set只存储键。
2. 要求不重复的键，或许允许重复的键。
3. 顺序保存或者无序保存元素。
   1. 对于有序容器（map，set，multimap，multiset），关联容器关键字类型必须定义元素比较的方法。默认情况下，标准库使用关键字类型的`<`运算符来比较两个关键字。也可以提供自定义操作来代替关键字的`<`运算符，但是所提供的操作必须在关键字类型上定义一个*严格弱序（strict wreak ordering）*，可以看作小于等于。在实际编程中，如果一个类型定义了“行为正常”的`<`运算符，则它可以用作关键字类型。
   2. 对于无序容器，这些容器不是使用比较运算符来组织元素，而是使用一个哈希函数（hash<key_type>）和关键字类型的`==`运算符。标准库为内置类型（包括指针）和一些标准库类型（比如string和智能指针）提供了hash模板。如果想要用自定义类型作为无需容器关键字类型，必须提供自己的hash模板版本。

关联容器支持前面顺序容器部分介绍的大部分操作，但不支持顺序容器的位置相关的操作，比如 push_front() 或者push_back()。

### 有序关联容器

下面通过demo来学习有序关联容器的一些常用操作：

*初始化，遍历和插入元素*

```c++
string word, words = "Hunk always have his own idea! He need more chance to get success";
istringstream iss(words);

map<string, size_t> word_count;
set<string> black_list = {"have", "his", "He"};
while ( iss >> word)
{
    if ( black_list.find(word) == black_list.end() )
    {   //确认单词不在黑名单后才对它进行统计
        ++word_count[word];
    }
}

for (const auto &w : word_count)
{
    cout << w.first << " occurs " << w.second
        << ((w.second > 1) ? " times." : " time.") << endl;
}

//topic: 迭代器遍历关联容器
auto map_it = word_count.cbegin();
while ( map_it != word_count.cend() )
{
    cout << map_it->first << " occurs " << map_it->second
        << (( map_it->second > 1) ? " times." : " time.") << endl;
    ++map_it;
}

//topic: 不同的方式向map中插入元素--pair类型
//对于map的insert方法，返回类型也是pair：
//          pair的first成员是一个迭代器，指向具有给定关键字的元素（注意：也是一个pair）
//          pair的second成员是一个bool值，指出元素是插入成功还是已经存在
word = "good";
cout << word_count.insert( { word, 1 } ).second << endl;//1
cout << word_count.insert( make_pair(word, 1) ).second << endl;//0
cout << word_count.insert( pair<string, size_t>(word, 1) ).second << endl;//0
cout << word_count.insert( map<string, size_t>::value_type(word, 1) ).second << endl;//0

if ( black_list.find(word) == black_list.end() )
{
    cout << "the " << word << " not int black list" << endl;
}
```

*拷贝关联容器*

```c++
multiset<string> mss= {"good", "new", "yes"};
vector<string> vs;

//将顺序容器中的元素拷贝到关联容器中
copy(mss.begin(), mss.end(), inserter(vs, vs.end()));
copy(mss.begin(), mss.end(), back_inserter(vs));
printContainer("Copied vector: ", vs.begin(), vs.end());//[good, new, yes, good, new, yes]

//将关联容器中的元素拷贝到顺序容器中
multiset<string> mss2;
copy(vs.begin(), vs.end(), inserter(mss2, mss2.end()));
//copy(vs.begin(), vs.end(), back_inserter(mss2));//非法掉用，因为multiset不支持push_back操作
printContainer("Copied multiset: ", mss2.begin(), mss2.end());//[good, good, new, new, yes, yes]
```

*范围查询*

```c++
//Topic: 关联容器的范围查询
//因为multimap和multiset允许多个相同键值的元素存在，这些相同键值的元素被排序后是逻辑上连续的
//如果针对某个键值，想要获取所有的元素，可以用下面的方法达到范围查询的效果
multimap<string, string> mmss = {
    { "Hunk", "A"}, {"Alice", "A"}, {"Jack", "A"}, {"Bob", "D"},
    {"Hunk", "D"}, {"Alice", "D"}, {"Alice", "C"}, {"Hunk", "B"}
};
string tgt_name = "Hunk";
//方法1：count和find结合
cout << "\nRange search method 1:" << endl;
auto cnt = mmss.count(tgt_name);
auto ite = mmss.find(tgt_name);
while ( cnt )
{
    cout << "Found " << tgt_name << " = " << ite->second << endl;
    ++ite;
    --cnt;
}
//方法2：使用lower_bound和upper_bound获得查询的迭代器范围
//  lower_bound返回的迭代器指向第一个具有给定键值的元素
//  upper_bound返回的迭代器指向最后一个匹配给定键值的元素之后的位置
cout << "Range search method 2:" << endl;
for (auto begin = mmss.lower_bound(tgt_name),
     end = mmss.upper_bound(tgt_name); 
     begin != end; ++begin)
{
    cout << "Found " << tgt_name << " = " << begin->second << endl;
}
//方法3：直接调用equal_range
//equal_range返回一个迭代器pair：
//  若关键字存在，第一个迭代器指向第一个匹配元素。第二个元素指向最后一个匹配元之后的位置
//  若关键字不存在，两个迭代器都指向关键字可以插入的相同位置
cout << "Range search method 3:" << endl;
for (auto pos = mmss.equal_range(tgt_name); 
     pos.first != pos.second; ++pos.first)
{
    cout << "Found " << tgt_name << " = " << pos.first->second << endl;
}
```

*查找和删除元素*

```c++
multimap<string,string> authors = {
    {"Hunk", "Book A"}, {"Alice", "Book C"},
    {"Hunk", "Book B"}, {"Alice", "Book D"},
    {"Bob", "Book E"}
};
cout << "Author numbers: " << authors.size() << "\n";//5
for (auto ite = authors.begin(); ite != authors.end(); ite++)
{
    //以字典序打印所有元素
    cout << "Author name: " << ite->first << ", book name: " << ite->second << "\n";
}

string rm_author = "Alice";
auto res = authors.find(rm_author);
if ( res != authors.end() )
{
    //对于multimap，erase会删除所有key匹配的元素
    authors.erase(rm_author);
}
rm_author = "Jack";
res = authors.find(rm_author);
if ( res != authors.end() )
{
    authors.erase(rm_author);
}
cout << "Author numbers: " << authors.size() << "\n";//3
```

### 无序关联容器

无序关联容器在存储上组织为一组哈希桶（bucket），每个桶保存零个或多个元素，并用哈希函数将元素映射到桶。

为了访问一个元素，容器首先计算元素的哈希值，它指出应该搜索哪个桶。容器将具有一个特定哈希值的所有元素保存在相同的桶中。因此，无序关联容器的性能依赖于哈希函数的质量和桶的数量和大小。

TODO：无序容器哈希桶内元素如何保存？再哈希（rehash）的细节？

# STL 算法

泛型算法本身不会执行容器的操作，它们只会运行于迭代器之上，执行迭代器的操作，这意味着算法永远不会改变底层容器的大小。

泛型算法可能改变容器中保存的元素的值，也可能在容器内移动元素，但永远不会直接添加或删除元素。

关于 C++ STL 算法可以参考：[Standard Library Algorithms - hackingcpp](https://hackingcpp.com/cpp/std/algorithms.html)