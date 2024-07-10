# 一、C++ References

## C++语言

[Cpp references 中文版](https://zh.cppreference.com/w/cpp)

[C++语言参考手册](https://zh.cppreference.com/w/cpp/language)

[GeeksForGeeks C++专题](https://www.geeksforgeeks.org/virtual-functions-and-runtime-polymorphism-in-c-set-1-introduction/)

[hacking C++ ***](https://hackingcpp.com/)

[C++ Template 进阶指南](https://github.com/wuye9036/CppTemplateTutorial)

[C++ Core Guidelines](https://github.com/isocpp/CppCoreGuidelines/blob/master/CppCoreGuidelines.md)

## C++ Style

[C++ Tips of the Week - Google](https://abseil.io/tips/) or [C++ Tip of The Week - Github](https://github.com/QuantlabFinancial/cpp_tip_of_the_week/blob/master/README.md#)

https://clang.llvm.org/docs/UsersManual.html)

## C++ Debugger & Tools

### CMake

[Learn CMake's Scripting Language in 15 Minutes](https://preshing.com/20170522/learn-cmakes-scripting-language-in-15-minutes/)

[Introduction to CMake by Example](http://derekmolloy.ie/hello-world-introductions-to-cmake/)

### GDB

[GDB备忘清单](https://quickref.cn/docs/gdb.html) ***

[GDB QUICK REFERENCE](https://www.cs.utexas.edu/~dahlin/Classes/UGOS/reading/gdb-ref.pdf)

[Cmake和gdb调试程序](https://www.cnblogs.com/taolusi/p/9293290.html)

### Valgrind

1. https://zhuanlan.zhihu.com/p/92074597



# 二、C++语言基础

## 2.1 代码结构和布局

### Header（头文件）的防卫式声明

```c++
//Complex.hpp
#ifndef Complex_H_   //防卫式声明，防止同一个程序中重复去包含这个头文件里定义的内容
#define Complex_H_

class Complex
{
    ...
};

#endif
```

> 在C++中，成员函数一般是不可以在头文件中定义的，只能在头文件中声明。因为函数只能有一次定义，而可以有多次声明，当头文件被多次包含的时候，如果头文件中有函数定义就违背了这个原则。在编译链接时就可能报下面的错误：
>
> ```bash
> std/libstd.a(String.cpp.o): In function `String::~String()':
> String.cpp:(.text+0x0): multiple definition of `String::~String()'
> CMakeFiles/demo.dir/main.cpp.o:main.cpp:(.text+0x0): first defined here
> ```

## 2.2 编译和内存布局

### 编译 & 链接

C/C++ 程序的编译和链接过程如下图所示：

![image-20240423164716902](https://hunk-pic-store.oss-cn-beijing.aliyuncs.com/img/20240423164720.png)

#### 静态链接 vs 动态链接

https://www.geeksforgeeks.org/static-and-dynamic-linking-in-operating-systems/#

#### 程序去哪里加载动态库？

当一个C++程序在Linux系统中动态链接到一个共享库（`.so`文件）时，它在运行的时候需要知道去哪里找到这个共享库。常用的定义共享库路径的方式如下：

1. 系统标准库目录，比如 **/usr/lib, /lib, or /usr/local/lib**。

2. 库路径环境变量，通过设置 `LD_LIBRARY_PATH`来扩展库查找路径，例如：

   ```bash
   $ export LD_LIBRARY_PATH=/path/to/your/library:$LD_LIBRARY_PATH
   ```

3. `rpath`或者 RUNPATH，在编译程序时通过编译选项（ **-rpath** or **-rpath-link**）指定库查找路径，例如：

   ```bash
   $ g++ -o my_program my_program.cpp -L/path/to/your/library -lYourLibrary -Wl,-rpath,/path/to/your/library
   ```

4. 使用`ldconfig`添加自定以动态库路径和相应的配置文件，例如：

   ```bash
   $ echo "/path/to/your/library" | sudo tee /etc/ld.so.conf.d/your_library.conf
   $ sudo ldconfig
   ```



## 2.3 基础数据类型

### 数组

关于C++数组的一些总结：

1. 栈上分配数组长度必须是常量或者常量表达式，堆上可以分配动态数组。
2. 局部数组不会自动初始化，而全局数组会被自动初始化为元素默认值。
3. 理解这篇文章：[C/C++中数组与指针的关系探究](https://www.cnblogs.com/zhiheng/p/6683334.html)

下面通过一些使用案例探究C++数组的特性：

```c++
const int LEN = 3;
int g_a[LEN]; //全局数组元素会被初始化为默认值

void TestArray()
{
    cout<<"***********[Begin] "<<__FUNCTION__<<"**********"<<endl;
    //1.数组的长度只能是常量或者是常量表达式
    int a1[LEN]; //局部数组元素不会自动初始化
    int a2[3] = { 30 }; //首元素被初始化为指定值，其余元素初始化为默认值
    int a3[3] = { 11, 22, 33};
    //int a4[n] = { 3 }; //variable "a4" may not be initializedC/C++(145)
    cout<<__FUNCTION__<<" a1[2]="<<a1[2]<<endl;//a1[2]=4197413
    cout<<__FUNCTION__<<" a2[0]="<< a2[0] <<", a2[2]="<<a2[2]<<endl;// a2[0]=30, a2[2]=0
    cout<<__FUNCTION__<<" a3[2]="<<a3[2]<<endl;//33
    cout<<__FUNCTION__<<" g_a[2]="<<g_a[2]<<endl;//g_a[2]=0

    //2. 探索数组指针
    //对数组名取地址不会发生指针转换，得到的就是数组地址，
    //也可以叫做数组指针，这个指针的步长是所有元素的长度和，这里是12
    cout << __FUNCTION__ << " &a3: " << &a3 << endl;//0x7ffe8bd1c500
    cout << __FUNCTION__ << " *a3: " << *a3 << endl;//0x7ffe8bd1c500
    cout << __FUNCTION__ << " (*a3)+1: " << (*a3)+1 << endl;//12，单纯的取值加法运算
    cout << __FUNCTION__ << " *(a3+1): " << *(a3+1) << endl;//22，等价于a3[1]
    //&a3[0]：&的优先级不是最高的，所以&的直接作用对象是 a3[0] 这个子表达式，
    //此时a3转换为指针（int指针）后进行运算，得到数组的第一个元素，&作用于这个元素后取得其地址，
    //得到的最终结果指向数组首元素的指针。尽管这里的地址和上面的地址相同，但是它们有本质的区别
    //所谓数组的下标本质是指针运算
    cout << __FUNCTION__ << " &a3[0]: " << &a3[0] << endl;//0x7ffe8bd1c500
    //a3[1]等价于int指针加1，得到数组的第二个元素
    cout << __FUNCTION__ << " &a3[1]: " << &a3[1] << endl;//0x7ffe8bd1c504
    //发生了指针转换
    cout << __FUNCTION__ << " (&a3)[0]: " << (&a3)[0] << endl;//0x7ffe8bd1c500
    //这里实际上是在数组指针上进行运算，指针+1即移动12个字节
    //注意，这里已经发生了内存越界访问，有很大的安全隐患
    cout << __FUNCTION__ << " (&a3)[1]: " << (&a3)[1] << endl;//0x7ffe8bd1c50c

    //没有发生转换，所以得到的是数组的大小
    cout << __FUNCTION__ << " size of a3: " << sizeof(a3) <<endl;//12
    //a3被转换为指针，所以并不是数组a3与0相加，而是转换后得到的指针在参与运算
    cout << __FUNCTION__ << " size of a3+0: " << sizeof(a3+0) <<endl;//8
    TestArrayAsParam(a3);

    //3.动态数组
    int n = 10;
    int *pia1 = new int[n]; //每个元素都没有初始化
    int *pia2 = new int[n]();  // 每个元素初始化为默认值，这里是0
    //如果元素是class，始终都会调用默认构造函数进行初始化
    string *psa1 = new string[n];//调用每个元素的默认构造函数
    string *psa2 = new string[n]();//调用每个元素的默认构造函数

    //4。二维数组
    int value1[9][9];//未初始化
    int value2[9][9] = {{1,1},{2}}; //value2[0][0,1]和value2[1][0]的值初始化，其他初始化为0

    int m = 10;
    const int n1 = 10;
    int (*value3)[n1] = new int[m][n1];//n1必须是常量表达式，未初始化
    int** value4 = new int* [m];//未初始化

    cout<<"***********[End] "<<__FUNCTION__<<"**********"<<endl;
}

void TestArrayAsParam(int a[])
{
    //sizeof的值始终是8，数组作为参数时，编译器自动转换为指针，此处为指向int的指针
    cout << __FUNCTION__ << " size of array param: " << sizeof(a) <<endl;
    cout << __FUNCTION__ << " address of array param: " << a <<endl;//0x7ffe2f82d570
    cout << __FUNCTION__ << " (a+2)= " << a+2 << endl;//0x7ffe2f82d578，步长是int型size，即4个字节
    cout << __FUNCTION__ << " *(a+2)= " << *(a+2) << endl;//33
    cout << __FUNCTION__ << " a[2]= " << a[2] << endl;//33
}
```

### 字符串string

```c++
string s = string("asd"); //crash?
string s = string("asd\0"); //OK
```





## 2.4 基础语言特性

### 指针和引用

#### 指针和引用的区别

![image-20200902212618830](https://hunk-pic-store.oss-cn-beijing.aliyuncs.com/img/20240416164726.png)

**指针**：指针是一种变量类型，其值为另一个变量的地址，即内存位置的直接地址。就像其他变量或常量一样，必须在使用指针存储其他变量地址之前，对其进行声明。在 64 位计算机中，指针占 8 个字节空间。使用指针时可以用以下几个操作：定义一个指针变量、把变量地址赋值给指针、访问指针变量中可用地址的值。通过使用一元运算符 `*` 来返回位于操作数所指定地址的变量的值。

C 语言中定义了空指针为 `NULL`，实际是一个宏，它的值是 0，即 `#define NULL 0`。C++ 中使用 `nullptr` 表示空指针，它是 C++ 11 中的关键字，是一种特殊类型的字面值，可以被转换成任意其他类型。

指针的运算：

1. 两个同类型指针可以比较大小；
2. 两个同类型指针可以相减；
3. 指针变量可以和整数类型变量或常量相加；
4. 指针变量可以减去一个整数类型变量或常量；
5. 指针变量可以自增，自减；

```c++
int a[10];
int *p1 = a + 1; // 指针常量相加
int *p2 = a + 4;
bool greater = p2 > p1; // 比较大小
int offset = p2 - a; // 相减
p2++; // 自增
p1--; // 自减
```

**引用**：引用变量是别名，即已存在变量的另一个名称。对于编译器来说，引用和指针一样，也是通过存储对象的地址来实现的。实际可以将引用视为具有自动间接寻址的常量指针，编译器自动为引用使用 `*` 运算符。

指针和引用的区别：

1. 指针可以在定义后的任意地方重新赋值，指向新的对象，而引用所绑定的对象一旦初始化绑定就不能改变，即强制一一对应关系。
2. 指针本身在内存中占有内存空间，指针和其指向的对象各自占用不同的内存空间。引用相当于变量的别名，在内存中不占内存空间（实际底层编译器可能用指针实现的引用），当我们使用 & 对引用取地址时，将会得到绑定对象的地址，和直接对绑定对象取地址的结果是一样的。从效果上来看，对象和其引用的大小相同，地址也相同。
3. 引用通常不用于声明变量，而是常用于参数类型和返回类型的描述。

```c++
#include <iostream>
using namespace std;

int main() 
{ 
    int a = 10;
    int &b = a;
    cout<<&a<<endl;
    cout<<&b<<endl;
    return 0;
}
// &a 和 &b 的结果是一样的，因为引用相当于别名，给a起了个别名叫b而已。用&对引用取地址的结果，跟引用绑定对象的结果是一致的。
```



#### 常量指针和指针常量的区别

常量指针本质上是个指针，只不过这个指针指向的对象是常量。

1. 指针指向的对象不能通过这个指针来修改，也就是说常量指针可以被赋值为变量的地址，之所以叫做常量指针，是限制了通过这个指针修改变量的值。
2. 虽然常量指针指向的对象不能变化，可是因为常量指针本身是一个变量，因此，可以被重新赋值。

常量指针的特点：const 的位置在指针声明运算符 * 的左侧。只要 const 位于 * 的左侧，无论它在类型名的左边或右边，都表示指向常量的指针。（可以这样理解：* 左侧表示指针指向的对象，该对象为常量，那么该指针为常量指针。）

```c++
const int * p;
int const * p;
```

指针常量本质上是个常量，只不过这个常量的值是一个指针。指针常量的值是指针，这个值因为是常量，所以指针本身不能改变。指针的内容可以改变。

指针常量特点：const 位于指针声明操作符右侧，表明该对象本身是一个常量，* 左侧表示该指针指向的类型，即以 * 为分界线，其左侧表示指针指向的类型，右侧表示指针本身的性质。

```c++
const int var1 = 1;
int * const c_p1 = &var1;  // 指向常量的指针常量，指针的指向不可修改，指针所指的内存区域中的值也不可修改。

int var2 = 1;
int * const c_p = &var2;  // 指向常量的指针常量，指针的指向不可修改，指针指向的变量var2可以被修改
```

#### 函数指针

函数指针即指向函数的指针。函数指针本质是一个指针变量，只不过这个指针指向一个函数。我们知道所有的函数最终都编译生成代码段，每个函数的都只是代码段中一部分而已，每个函数在代码段中都有其调用的起始地址与结束地址，因此我们可以用指针变量指向函数的在代码段中的起始地址。

```c++
#include <iostream>
using namespace std;

int add(int a, int b){
    return a + b;}

typedef int (*fun_p)(int, int);

int main(void){
    fun_p fn = add;
    cout << fn(1, 6) << endl;
    return 0;}
```

#### 野指针和空悬指针

**空悬指针**：若指针指向一块内存空间，当这块内存空间被释放后，该指针依然指向这块内存空间，此时，称该指针为“悬空指针”。如果对悬空指针再次释放可能会出现不可预估的错误，比如可能该段内存被别的程序申请使用了，而此时对该段内存进行释放可能会产生不可预估的后果。

**野指针：**指不确定其指向的指针，未初始化的指针为“野指针”，未初始化的指针的初始值可能是随机的，如果使用未初始化的指针可能会导致段错误，从而程序会崩溃。如何避免野指针：指针在定义时即初始化，指针在释放完成后，需要将其置为空。

```c++
void *p = malloc(size);
free(p); // 此时，p 指向的内存空间已释放， p 就是悬空指针。
p = NULL;

void *p; 
// 此时 p 是“野指针”。
```

### 左值和右值

C++ 表达式中的 “值分类”（value categories）属性为左值（lvalue）或右值（rvalue）：

1. `左值（lvalue）`：指表达式结束后依然存在的持久对象；可以通过内置（不包含重载）取地址符 & 取地址；可以将一个右值赋给左值。

2. `右值（rvalue）`：表达式结束就不再存在的临时对象；不可通过内置（不包含重载）取地址符 & 取地址；由于右值不可取地址，所以不能将任何值赋给右值；右值可以是字面量、临时对象等表达式。

使用 `=` 进行赋值时，`=` 左边必须为左值，右值只能出现在 `=` 的右边。

```c++
// x 是左值，666 为右值
int x = 666;   // ok 
int *y = x; // ok
int *z = &666 // error
666 = x; // error
int a = 9; // a 为左值
int b = 4; // b 为左值
int c = a + b // c 为左值 , a + b 为右值
a + b = 42; // error

// 函数返回值可以是左值，也可以是右值
int setValue() { return 6;}
int global = 100;
int& setGlobal() { return global;  }
setValue() = 3; // error!
setGlobal() = 400; // OK
```

但是，能否被赋值不是区分 C++ 左值与右值的依据，C++ 的 const 左值是不可赋值的；而作为临时对象的右值可能允许被赋值。左值与右值的根本区别在于是否允许取地址 & 运算符获得对应的内存地址。

> 关于C++左值和右值更多细节讨论参考：[理解 C/C++ 中的左值和右值](https://nettee.github.io/posts/2018/Understanding-lvalues-and-rvalues-in-C-and-C/)



#### 左值引用

左值引用可以区分为 常量左值引用 和 非常量左值引用。左值引用的底层实现是指针实现。

1. **非常量左值引用**：只能绑定到非常量左值，不能绑定到常量左值和右值。如果绑定到非常量右值，就有可能指向一个已经被销毁的对象。
2. **常量左值引用**：能绑定到非常量左值、常量左值和右值。

```c++
int y = 10;
int& yref = y;  // ok
int& xref = 10; // error， 非常量左值引用绑定右值
const &xref = 10; // ok, 常量左值引用绑定右值
int a = 10;
int b = 20;
int& zref = a + b // error， a + b为右值

int &aref1 = a;  //ok, 非常量左值引用绑定非常量左值
const int &aRef2 = a; //ok, 常量左值引用绑定非常量左值
const int c = 4;   
int &cref1 = c;  // error，非常量左值不能绑定常量右值
const int &cref2 = c; //ok, 常量左值引用绑定常量左值
const int &ref2 = a + b;    //ok, 常量左值引用绑定到右值（表达式）
```

#### 右值引用（C++11）

`右值引用 （Rvalue Referene）` 即绑定到右值的引用，用 `&&` 来获得右值引用。它是 C++ 11 中引入的新特性 , 实现了**移动语义 （Move Sementics）**和**完美转发 （Perfect Forwarding）**。*右值引用只能绑定到一个将要销毁的对象上，因此可以自由地移动其资源并转移所有权。*

> C++ 03 在用临时对象或函数返回值给左值对象赋值时的**深度拷贝（deep copy）**，总是需要先复制资源并写入新的对象然后再销毁原有资源，造成性能低下。考虑到临时对象的生命期仅在表达式中持续，如果把临时对象的内容直接移动（move）给被赋值的左值对象（右值参数所绑定的内部指针复制给新的对象，然后把该指针置为空），效率改善将是显著的。
>
> 右值引用就是为了实现 move 与 forward 所需要而设计出来的新的数据类型。右值引用的实例对应于临时对象；右值引用并区别于左值引用，用作形参时能通过函数重载来区别对象是调用拷贝构造函数还是移动拷贝构造函数。实际上无论是左值引用还是右值引用，从编译后的反汇编层面上，都是对象的存储地址的引用。右值引用与左值引用的变量都不能悬空，也即定义时必须初始化从而绑定到一个对象上。
>
> C++ 11引入右值引用是C++  的一个重大革新，它解决了C++中大量的历史遗留问题，使C++标准库在多种场景下消除了不必要的额外开销。即使你并不直接使用右值引用，也可以通过标准库，间接从这一新特性中受益。为了更好的理解标准库结合右值引用带来的优化，我们有必要了解一下右值引用的重大意义。这个关于右值引用的回答讲的很好：https://www.zhihu.com/question/22111546/answer/30801982

从实践角度讲，它能够完美解决 C++ 中长久以来为人所诟病的临时对象效率问题。从语言本身讲，它健全了 C++ 中的引用类型在左值右值方面的缺陷。从库设计者的角度讲，它给库设计者又带来了一把利器。从使用者的角度来看，可以获得效率的提升，避免对象在传递过程中重复创建。

右值引用两个主要功能：

1. 消除两个对象交互时不必要的对象拷贝、节省运算存储资源，提高效率。
2. 能够更简介明确的定义泛型函数。

```c++
#include <iostream>
using namespace std;

int g_val = 10;
void ProcessValue(int &i) {                         // 左值引用
    cout << "lValue processed: " << i << endl;
}

void ProcessValue(int &&i) {                        // 右值引用
    cout << "rValue processed: " << i << endl;
}

int GetValue() { // 返回右值
    return 3; 
} 

int& getVal() { // 返回左值引用
    return g_val; 
}

int main() {
    int a = 0;
    int b = 1;
    int &alRef = a;             // 左值引用
    int &&rRef1 = 1;            // 临时对象是右值
    int &&rRef2 = GetValue();   // 调用的函数为右值
    ProcessValue(a);            // 左值，lValue processed: 0
    ProcessValue(getVal());     // 左值引用，lValue processed: 10
    ProcessValue(1);            // 临时对象是右值，rValue processed: 1
    ProcessValue(GetValue());   // 调用的函数为右值，rValue processed: 3
    ProcessValue(a+b);          // 表达式为右值，rValue processed: 1
    return 0;}
```

#### 左值转换成右值

我们可以通过 `std::move` 函数、`static_cast`、或者`std::forward` 将一个左值强制转化为右值或者直接转换为右值引用，继而可以通过右值引用使用该值，以用于移动语义，从而完成将资源的所有权进行转移。后面 STL 章节对 std::move 和 std::forward 的实现原理做了详细的介绍。

```c++
void fun(int&& tmp) { 
  cout << "fun rvalue bind:" << tmp << endl; 
} 

void fun1(int& tmp) { 
  cout << "fun1 lvalue bind:" << tmp << endl; 
} 

int main() { 
    int var = 11; 
    fun(12); // 右值引用，fun rvalue bind:12
    fun(var); // 左值引用，fun lvalue bind:11
    fun(std::move(var)); // 使用std::move转为右值引用，fun rvalue bind:11
    fun(static_cast<int&&>(var));  // 使用static_cast转为右值引用，fun rvalue bind:11
    fun((int&&)var); // 使用C风格强转为右值引用，fun rvalue bind:11
    fun(std::forward<int&&>(var)); // 使用std::forwad<T&&>为右值引用，fun rvalue bind:11
    fun1(12); // error
    return 0; }
```

#### 引用折叠

通过类型别名或者通过模板参数间接定义，多重引用最终折叠成左值引用或者右值引用。有两种引用（左值和右值），所以就有四种可能的引用+引用的组合（左值 + 左值，左值 + 右值，右值 + 左值，右值 + 右值）。如果引用的引用出现在允许的语境，该双重引用会折叠成单个引用，规则如下：

1. 所有的右值引用叠加到右值引用上仍然还是一个右值引用；T&& && 折叠成 T&&
2. 所有的其他引用类型之间的叠加都将变成左值引用。T& &&，T&& &， T& & 折叠成 T&。

```c++
typedef int&  lref;
typedef int&& rref;

void fun(int&& tmp) { 
    cout << "fun rvalue bind:" << tmp << endl; 
} 

void fun(int& tmp) { 
    cout << "fun lvalue bind:" << tmp << endl; 
} 

int main()  { 
    int n = 11; 
    fun((lref&)n);  // fun lvalue bind:11, T& & 被折叠成T&
    fun((lref&&)n); // fun lvalue bind:11，T& && 被折叠成T&
    fun((rref&)n);   //fun lvalue bind:11，T&& & 被折叠成T&
    fun((rref&&)n);  //fun rvalue bind:11，T&& && 被折叠成T&&
    return 0; }
```

#### 万能引用模型

基于前面介绍的引用折叠的基本行为，在模板中 T&& t 在发生自动类型推断的时候，它是未定的引用类型（universal references），它既可以接受一个左值又可以接受一个右值。如果被一个左值初始化，它就是一个左值；如果它被一个右值初始化，它就是一个右值，它是左值还是右值取决于它的初始化。参考资料：

1. [引用折叠和完美转发](https://zhuanlan.zhihu.com/p/50816420)

2. [谈谈C++的左值右值、左右引用、移动语义和完美转发](https://zhuanlan.zhihu.com/p/402251966)



### 参数传递：值/指针/引用传递

C++传递参数的三种方式：

1. **值传递**：形参是实参的拷贝，函数对形参的所有操作不会影响实参。从被调用函数的角度来说，值传递是单向的（实参->形参），参数的值只能传入。当函数内部可能需要改变参数具体的内容时，我们则采用形参，在组成原理上来说，对于值传递的方式我们采用直接寻址。
2. **指针传递**：**本质上是值传递，只不过拷贝的是指针的值，拷贝之后实参和形参是不同的指针，但指向的地址都相同**。通过指针可以间接的访问指针所指向的对象，从而可以修改它所指对象的值。在组成原理上来说，对于指针传递的方式一般采用间接寻址。
3. **引用传递**：当形参是引用类型时，我们说它对应的实参被引用传递。当然不同的编译器对于引用有不同的实现，部分编译器在底层也是使用指针来实现引用传递。

只有在值传递时，形参和实参的地址不一样，在函数体内操作的不是变量本身。引用传递和指针传递，在函数体内操作的是变量本身。

### 类型转换

#### static_cast

`static_cast` 是**静态转换**的意思，也即在编译期间转换，转换失败的话会抛出一个编译错误。一般用于如下场景：

1. 用于数据的强制类型转换，强制将一种数据类型转换为另一种数据类型。
2. 用于基本数据类型的转换。
3. 用于类层次之间的基类和派生类之间指针或者引用的转换（不要求必须包含虚函数、但必须是有相互联系的类）：
   1. 进行上行转换（派生类的指针或引用转换成基类表示）是安全的；
   2. 进行下行转换（基类的指针或者引用转换成派生类表示）由于没有动态类型检查，所以是不安全的，最好用 dynamic_cast 进行下行转换。
4. 可以将空指针转换成目标类型的空指针。
5. 可以将任意类型的表达式转换成 void 类型。
6. 不能用于在不同类型的指针之间相互转换，也不能用于整型和指针之间的互相转换、当然也不能用于不同类型的引用之间的转换。这些都属于风险比较高的转换。
7. 如果对象所属的类重载了强制类型转换运算符 T（如 T是 int、int* 或其他类型名），则 static_cast 也能用来进行对象到 T 类型的转换。

```c++
class A {
public:
    operator int() { return 1; }
    operator char*() { return NULL; }
};

int main() {
    A a;
    int n;
    const char* p = "This is a str for static_cast";
    n = static_cast <int> (3.14);           // n 的值变为 3 - 基本数据类型之间的转换
    n = static_cast <int> (a);              // 调用 a.operator int, n 的值变为 1
    p = static_cast <char*> (a);            // 调用 a.operator char*，p 的值变为 NULL
    // n = static_cast <int> (p);           // 编译错误，static_cast不能将指针转换成整型
    // p = static_cast <char*> (n);     // 编译错误，static_cast 不能将整型转换成指针
    return 0;
}
```

#### const_cast

`const_cast` 主要用于 const 和非const、volatile 和非volatile 之间的转换、强制去掉常量属性，不能用于去掉变量的常量性，只能用于去除指针或引用的常量性，即将常量指针/引用转换为非常量指针/引用（注意：表达式的类型和要转化的类型是相同的）。

#### reinterpret_cast

`reinterpret_cast`  用于进行各种不同类型的指针之间、不同类型的引用之间以及指针和能容纳指针的整数类型之间的转换。`reinterpret_cast` 转换时，执行的过程时逐个比特复制的操作。

```c++
class A {
public:
    int i;
    int j;
    A(int n) :i(n), j(n) { }
};

int main() {
    A a(100);
    int &r = reinterpret_cast<int&>(a);             // 强行让 r 引用 a
    r = 200;                                        // 把 a.i 变成了 200
    cout << a.i << "," << a.j << endl;              // 输出 200,100
    int n = 300;
    A *pa = reinterpret_cast<A*> (&n);              // 强行让 pa 指向 n
    pa->i = 400;                                    // n 变成 400
    pa->j = 500;                                    // 此条语句不安全，很可能导致程序崩溃
    cout << n << endl;                              // 输出 400
    long long la = 0x12345678abcdLL;
    pa = reinterpret_cast<A*>(la);                   // la太长，只取低32位0x5678abcd拷贝给pa
    unsigned int u = reinterpret_cast<unsigned int>(pa);    // pa逐个比特拷贝到u
    cout << hex << u << endl;                        // 输出 5678abcd
    typedef void(*PF1) (int);
    typedef int(*PF2) (int, char *);
    PF1 pf1 = nullptr; 
    PF2 pf2;
    pf2 = reinterpret_cast<PF2>(pf1);                   // 两个不同类型的函数指针之间可以互相转换
}
```

上面三种类型转换都是编译时完成的。

#### dynamic_cast

`dynamic_cast` 是在程序运行时处理的，运行时会进行类型检查。`dynamic_cast` 的用法如下：

1. 不能用于基本数据类型的转换。

2. 只能用于带有虚函数的基类或者派生类的指针或者引用对象的转换，转换成功返回指向类型的指针或者引用，转换失败返回NULL；

3. 在向上进行转换时，即派生类的指针转换成基类的指针和 `static_cast` 效果是一样的（注意：这里的转换只是改变了指针的类型、指针指向的对象的类型并未发生改变）。

   ```c++
       Base *p1 = new Derive();
       Derive *p2 = new Derive();
   
       //向上类型转换
       p1 = dynamic_cast<Base *>(p2);
       if (p1 == NULL) {
           cout << "NULL" << endl;
       } else {
           cout << "NOT NULL" << endl; //输出
       }
   ```

4. 在下行转换时，基类的指针类型转换为派生类的指针类型，只有当要转换的指针指向的对象类型和转换后的对象类型相同时，才会转化成功。

   ```c++
   class Base {  
   //有虚函数，因此是多态基类
   public:
       virtual ~Base() {}
   };
   
   class Derived : public Base { };
   
   int main() {
       Base b;
       Derived d;
       Derived* pd;
       pd = reinterpret_cast <Derived*> (&b);
       if (pd == NULL)
           //此处pd不会为 NULL。reinterpret_cast不检查安全性，总是进行转换
           cout << "unsafe reinterpret_cast" << endl; //不会执行
           
       pd = dynamic_cast <Derived*> (&b);
       if (pd == NULL)  //结果会是NULL，因为 &b 不指向派生类对象，此转换不安全
           cout << "unsafe dynamic_cast1" << endl;  //会执行
   
       pd = dynamic_cast <Derived*> (&d);  //安全的转换
       if (pd == NULL)  //此处 pd 不会为 NULL
           cout << "unsafe dynamic_cast2" << endl;  //不会执行
       return 0;
   }
   ```

   dynamic_cast 在进行引用的强制转换时，如果发现转换不安全，就会拋出一个异常，通过处理异常，就能发现不安全的转换。

[为什么说不要使用 dynamic_cast，需要运行时确定类型信息，说明设计有缺陷？](https://www.zhihu.com/question/22445339)

[(C++ 成长记录) —— C++强制类型转换运算符（static_cast、reinterpret_cast、const_cast和dynamic_cast）](https://zhuanlan.zhihu.com/p/368267441)

>**关于C++类型转换的一些简单经验：**
>
>1. 能够不要使用类型转换尽量不要使用类型转换，尽可能地只用原本的子类类型。
>2. 需要使用类型转换的时候，可以尽量避免使用 reinterpret_cast，因为它过于自由，安全性太差。
>3. 能用 static_cast 的地方，不要去使用 dynamic_cast。
>4. 非必要不要用 const_cast 转换去破坏常量的属性。

### 条件语句

#### switch的case里为何不建议定义变量？

switch 下面的这个花括号表示一块作用域，而不是每一个 case 表示一块作用域。如果在某一 case 中定义了变量，其作用域在这块花括号内，按理说在另一个 case 内可以使用该变量，但是在实际使用时，每一个 case 之间互不影响，是相对封闭的。参考如下实例，在 switch 的 case 中定义的变量，没有实际意义，仅为了解释上述原因。

```c++
int main()
{
    // 局部变量声明
    char var = 'D';

    switch (var) {
    case 'A':
        int cnt = 0; // 定义变量
        cout << "Excellent." << endl
             << cnt;
        break;
    case 'B':
    case 'C':
        ++cnt;
        cout << "Good." << endl
             << cnt;
        break;
    case 'D':
        cout << "Not bad." << endl
             << cnt;
        break;
    default:
        cout << "Bad." << endl
             << cnt;
    }

    return 0;
}
```





## 2.5 C++关键字

### const类型限定符

关于`const`的详细说明参考：[const类型限定符](https://zh.cppreference.com/w/c/language/const)

编译器可以把声明带`const` 限定类型的对象放到只读内存中，而且若程序决不取该 const 对象的地址，则可能完全不存储它。`const` 语义仅应用到左值表达式；只要在不要求左值的语境中使用 const 左值表达式，就会丢失其 `const` 限定符（注意不丢失 `volatile` 限定符，若它存在）。

下面列出const类型限定符的常见用途：

1. **声明常量**

```c++
// const关键字可以用于修饰普通变量，定义成const常量后不可更改
const int n = 1; // const 类型对象
n = 2; // 错误： n 的类型为 const 限定

// const 指针
int x = 2; // 无限定类型对下
const int* p = &x;
*p = 3; // 错误：左值 *p 的类型为 const 限定
```

​	需要注意的是，const修饰指针的时候的位置：

```c++
// const在`*`右边时，离指针变量名更近，所以修饰的是指针本身
char *const cp;  //到char的const指针，pc不能指向别的字符串，但可以修改其指向的字符串的内容

// const在`*`左边时，离变量类型更近，所以修饰的是指针指向的对象不可更改，但是指针本身可以指向其它的对象。
char const *pc1; //到const char的指针，pc2的内容不可以改变，但pc2可以指向别的字符串

int a = 8;
const int * const  p = &a; //指针和指针指向的变量都不可修改
```

​	指向非 const 类型的指针能隐式转换成指向同一或兼容类型的 const 限定版本的指针，并且能用转型表达式进行逆向转换：

```c++
int* p = 0;
const int* cp = p; // OK ：添加限定符（ int 到 const int ）
p = cp; // 错误：舍弃限定符（ const int 到 int ）
p = (int*)cp; // OK ：转型
```

​	任何修改有 const 限定类型的对象的尝试导致未定义行为：

```c++
const int n = 1; // const 限定类型对象
int* p = (int*)&n;
*p = 2; // 未定义行为
```

2. **修饰函数参数和返回值**

   将函数传入参数声明为const，以指明使用这种参数仅仅是为了效率的原因，而不是想让调用函数能够修改对象的值。所以，通常修饰指针参数和引用参数，函数将不修改由这个参数所指向的对象。

```c++
 void Fun(const A *in); //修饰指针型传入参数
 void Fun(const A &in); //修饰引用型传入参数
```

​	函数返回值为指针或者引用，可以用const指针或者引用接受返回值，此时指向的内容不可以修改。

3. **修饰类的成员变量**

   const成员变量只能在类内声明、定义、在构造函数初始化列表中初始化；const成员变量只在某个对象的生存周期内是常量，对于整个类而言却是可变的，因为类可以创建多个对象；不同对象的const成员变量的值是不同的。所以，**不能在类的声明中初始化const成员变量**。

4. **修饰类的成员函数**

   将const关键字放在成员函数的参数列表后面就是修饰成员函数。const成员函数只能访问成员变量，不能修改成员变量，除非由mutable修饰；不能调用非常量成员函数，以防修改成员变量

![image-20200910093423956](https://hunk-pic-store.oss-cn-beijing.aliyuncs.com/img/image-20200910093423956.png)

​	1  `const`对象只能访问const成员函数，而非const对象可以访问任意的成员函数，包括const成员函数。

​	2 `const`对象的成员是不能修改的，而通过指针维护的对象却是可以修改的。

​	3 `const`成员函数不可以修改对象的数据，不管对象是否具有const性质。编译时以是否修改成员数据为依据进行检查。

​	4 `mutable`关键字修饰的数据成员，在任何情况下可以通过任何手段修改，包括const成员函数也可以修改它。

#### define和const的区别

`#define`是预处理指令，`const`是C++关键字，它们在某些使用场景下达到了类似的效果，比如定义公共常量，所以做个简单对比。

![image-20240417173343124](https://hunk-pic-store.oss-cn-beijing.aliyuncs.com/img/20240417173343.png)

结论就是在C++程序里能用`const`的地方就尽量用`const`而不是`#define`。

### static关键字

在C语言中，static关键字可以定义静态局部变量、静态全局变量和静态函数，我们把这些称之为面向过程的static。

在C++语言中，static关键字不仅兼容上面三种面向过程的static，还支持与类有关的静态成员变量和静态成员函数，我们把这些称之为面向类的static。

#### 面向过程的static

1. **静态全局变量**：
   1. 内存：在全局数据区（静态数据存储区）分配内存空间。
   2. 初始化：静态全局变量在程序初始化时，则进行了初始化，并且只初始化一次，防止在其他文件中使用。未经初始化的静态全局变量会被程序自动初始化为0（自动变量的值时随机的，除非它被显示初始化）。
   3. 作用域：静态全局变量作用域仅限于定义它的源文件内，其它源文件中不能使用它。普通全局变量的作用域是整个源程序，如果源程序由多个源文件组成，普通全局变量在各个源文件都是有效的。

2. **静态局部变量**：

   1. 内存：在全局数据区（静态数据存储区）分配内存空间。

   2. 初始化：局部静态变量只能被初始化一次，即在程序执行到该对象的声明处被首次初始化，以后的函数调用不再进行初始化。静态局部变量一般在声明处初始化，如果没有显式初始化，会被程序自动初始化为0。

      > 注意：静态全局对象在程序初始化时，则进行了初始化。静态局部对象的初始化在第一次进入函数内部时，才会调用对象的构造函数进行初始化。程序退出时，先释放静态局部变量，再释放静态全局变量。

   3. 作用域：它始终驻留在全局数据区，直到程序运行结束。但是其作用域为局部作用域，与函数内部的局部变量相同，当定义它的函数或语句结束时，其作用域随之结束。

      如果在函数中返回静态变量的地址会发生什么呢？以下面代码为例：

      ```c++
      int * fun(int tmp){
          static int var = 10;
          var *= tmp;
          return &var;
      }
      
      int main() {
          cout << *fun(5) << endl; //50
          return 0;
      }
      // 因为局部静态变量的生命周期为程序的整个生命周期，所以函数里返回局部静态变量的地址后，该变量并不会被销毁
      // 函数外部可以依然可以正常访问该变量
      ```

3. **静态函数**：静态函数的作用域，仅可在定义该函数的文件内部调用，不能被其它文件所用，其它文件中可以定义相同名字的函数，不会发生命名冲突。

```c++
#include <iostream>
using namespace std;

int g_var = 0; // 普通全局变量，所有源文件可见
static char *gs_var;  // 静态全局变量，定义它的源文件可见

int main() {
    int var;                    // 普通局部变量
    static int s_var = 0;       // 局部静态变量，作用域仅限于函数内部
    return 0;
}
```



#### 面向类的static

1. **静态成员变量**：

   1. 非静态成员数据：每个类对象都有一份拷贝。静态成员数据属于类本身，不属于特定的类对象，所有类对象共享同一份数据。所以，在没有产生类对象时其作用域就可见。静态成员变量相当于类域中的全局变量，被类的所有对象所共享，包括派生类的对象，且该变量只能被初始化一次，不能在类的构造函数中对静态成员变量进行初始化。

   2. 静态数据成员存储在全局数据区。静态数据成员定义时要分配内存空间，所以不能在类声明中定义。**静态成员变量是在类内进行声明，在类外进行定义和初始化**，在类外进行定义和初始化的时候不要出现 static 关键字和 private、public、protected 访问规则。

   3. 静态成员变量可以作为成员函数的参数，而普通成员变量不可以。

   4. 静态数据成员的类型可以是所属类的类型，而普通数据成员的类型只能是该类类型的指针或引用。

      ```c++
      class A
      {
      public:
          static A s_var; // 正确，静态数据成员
          A var;          // error: field 'var' has incomplete type 'A'
          A *p;           // 正确，指针
          A &var1;        // 正确，引用
      };
      ```

   5. 静态数据成员主要用在各个对象都有相同的某项属性的时候。比如所有的银行账户共享相同的利率。**如果明确了数据只需要在某个类的所有对象之间共享，尽量设计成类的静态数据成员而不是全局变量**。既然都是可以共享的数据，使用静态数据成员相对于全局变量有两个优势：（1）静态数据成员没有进入程序的全局命名空间，因此降低了命名冲突的可能性。（2）可以实现信息隐藏，静态成员数据可以是`private`成员，而全局变量不能。

2. **静态成员函数**：
   1. 静态成员函数，为整个类服务而不是为某一个类的具体对象服务。**因为它不与任何的类对象相联系，所以不具有`this`指针**。所以，静态成员函数也不能访问非静态成员变量或者调用非静态成员函数。
   2. 静态成员函数做为类作用域的全局函数，在类对象产生之前就存在了，所以可以被对象的非静态成员函数调用。
   3. 静态成员之间可以相互访问：包括静态成员函数访问静态数据成员和访问静态成员函数。
   4. 静态成员函数不能声明成虚函数（virtual）、const 函数和 volatile 函数。
   5. 出现在类体外的函数定义不能指定static关键字。

### inline原理、作用及用法

`inline` 关键字用于定义内联函数。**内联函数像普通函数一样被调用，但是在调用时并不通过函数调用的机制而是直接在调用点处展开，这样可以大大减少由函数调用带来的开销，从而提高程序的运行效率**。在内联函数出现之前，在 C/C++ 的大型工程中常见用 #define 定义一些“函数”来消除调用这些函数的开销。内联函数设计的目的之一，就是取代 #define 的这项功能。由于使用 #define 定义的“函数”，编译器不会检查其参数的正确性等，而使用 inline 定义的函数，可以指定参数类型，则会被编译器校验）。内联函数可以在头文件中被定义，并被多个 .cpp 文件 include，而不会有重定义错误。这也是设计内联函数的主要目的之一。

为类定义内联函数的用法：

1. 类内定义的成员函数（比如构造函数、析构函数、普通成员函数）默认是内联函数，可以不用在函数头部加inline关键字，编译器自动会处理。虚函数除外，因为虚函数是在运行时决定的，在编译时还无法确定虚函数的实际调用。

2. 类外定义的成员函数，若想定义为内联函数，需要在函数定义体加上inline关键字，只是在函数声明处加 inline关键字不起作用。

   ```c++
   class A{
   public:
       int var;
       A(int tmp){ 
         var = tmp;
       }
       //默认编译成内联函数
       void fun1(){ 
           cout << var << endl;
       }
       void fun2();
   };
   
   inline void A::fun(){
       cout << var << endl;
   }
   ```

内联函数的工作原理及优点：

1. 内联函数不是在调用时发生控制转移关系，而是在编译阶段将函数体嵌入到每一个调用该函数的语句块中，编译器会将程序中出现内联函数的调用表达式用内联函数的函数体来替换。**函数调用时，需要切换栈帧寄存器，同时栈中压入参数、返回值，然后进行跳转，这些都需要开销，而内联函数则可以不要这些开销，直接将内联函数中函数体直接插入或者替换到该函数调用点**。
2. 普通函数是将程序执行转移到被调用函数所存放的内存地址，当函数执行完后，返回到执行此函数前的地方。转移操作需要保护现场，被调函数执行完后，再恢复现场，该过程需要较大的资源开销。
3. 虽然内联函数在调用时直接进行展开，但实际在编译后代码中存在内联函数的定义，可以供编译器进行调用。普通函数可以有指向它的函数指针，内联函数也可以有指向它的函数指针。

内联函数的缺点：

1. 从内联函数中添加的变量会消耗额外的寄存器，在内联函数之后，如果要使用寄存器的变量数量增加，则可能会在寄存器变量资源利用方面产生开销。在函数调用点替换内联函数体时，函数使用的变量总数也会增加，用于存储变量的寄存器数量也会增加。因此，如果在函数内联变量数量急剧增加之后，它肯定会导致寄存器利用率的开销。
2. 如果你使用太多的内联函数，那么二进制可执行文件的大小会很大，因为相同的代码重复。
3. 过多的内联也会降低指令缓存命中率，从而降低从缓存内存到主内存的指令获取速度。
4. 如果有人更改内联函数内的代码，内联函数可能会增加编译时间开销，那么所有调用位置都必须重新编译，因为编译器需要再次替换所有代码，否则它将继续使用旧功能。
5. 内联函数可能会导致抖动，因为内联可能会增加二进制可执行文件的大小。内存抖动会导致计算机性能下降。

内联函数的使用场景： 内联函数一般只适用于比较短小，处理较为简单的函数。内联只是对编译器的请求，而不是命令。编译器可以忽略内联请求。编译器可能不会在以下情况下执行内联：

1. 如果函数包含循环（for, while, do-while）；
2. 如果一个函数包含静态变量；
3. 如果一个函数是递归的；
4. 如果函数返回类型不是 void，并且函数体中不存在 return 语句；
5. 如果函数包含 switch 或 goto 语句；

#### 宏定义和内联函数的区别

1. 内联函数在编译时展开，而宏定义在预处理阶段展开；
2. 在编译的时候，内联函数是直接被嵌入到目标代码中，成为调用函数的一个代码块，执行的时候省去了函数调用压栈出栈的开销；而宏定义只是简单的文本替换。
3. 内联函数可以进行调试，宏定义的”函数“无法调试。
4. 由于类的成员函数全部为内联函数，通过内联函数，可以访问类的数据成员，而宏不能访问类的数据成员。

### extern关键字

C 和 C++ 对同一个函数经过编译后生成的函数名是不同的，由于 C++ 支持函数重载，因此编译器编译函数的过程中会将函数的参数类型也加到编译后的函数名中，而不仅仅是原始的函数名。比如以下函数，同一个函数 test 在 C++ 编译后符号表中生成的函数名可能为 _Z4testv，而 C 编译后符号表中生成的函数名可能就为 test。

由于 C 语言并不支持函数重载，在 C 语言中函数不能重名，因此编译 C 语言代码的函数时不会带上函数的参数类型，一般只包括函数名。如果在 C++ 中调用一个使用 C 语言编写的模块中的某个函数 test，C++ 是根据 C++ 的函数名称修饰方式来查找并链接这个函数，去在生成的符号表查找 _Z4testv 这个函数的代码，此时就会发生链接错误。而此时我们用 extern C 声明，那么在链接时，C++ 编译器则按照 C 语言的函数命名规则 test 去符号表中查找对应的函数。因此当 C++ 程序需要调用 C 语言编写的函数，C++ 使用链接指示，即 extern "C" 指出任意非 C++ 函数所用的语言。

`extern`提供以不同程序语言编写的模块间的链接。比如 `extern "C"` 使得以C程序语言编写的函数进行链接，以及在C++程序中定义能从C模块调用的函数成为可能，从而实现了C++与C及其它语言（符合C语言的编译和连接规约的任何语言，比如汇编语言）的混合编程。

```c++
extern "C" {
    int open(const char *pathname, int flags); // C 函数声明
}
 
int main()
{
    int fd = open("test.txt", 0); // 从 C++ 程序调用 C 函数
}
 
// 此 C++ 函数能从 C 代码调用
extern "C" void handler(int) {
    std::cout << "Callback invoked\n"; // 它能使用 C++
}
```

`extern "C"` 使得在 C++ 程序中包含（include）含有 C 库函数的声明的头文件成为可能，但如果与 C 程序共用相同的头文件，必须以适当的`#ifdef`隐藏 `extern "C"`（其在 C 中不被允许），通常为`__cplusplus`。

```c++
#ifdef __cplusplus
extern "C" int foo(int, int); // C++ 编译器所见
#else
int foo(int, int);            // C 编译器所见
#endif
```

如果有多个这样的函数声明需要被处理，可以写成：

```c++
#ifdef __cplusplus
extern "C" {
#endif
    
int foo1(int, int); 
int foo2(int, int); 
    
#ifdef __cplusplus
}
#endif
```

#### extern "C"的惯用用法

1. 在C++中引用C语言中的函数和变量时：（1）包含C语言头文件（cExample.h）时需进行以下处理：

   ```c++
   extern "C" {
   #include "cExample.h"
   }
   ```

   （2）在C语言的头文件中，对其外部函数只能指定为`extern`类型（C语言中不支持`extern "C"`声明，在.c文件中包含了`extern "C"`时会出现编译语法错误，所以这个声明一般写在C++源文件中）。

   下面给出C++中引用C函数案例的3个源文件：

   ```c
   // c语言头文件：cExample.h
   #ifndef C_EXAMPLE_H
   #define C_EXAMPLE_H
   
   extern int add(int x,int y);
   
   #endif
   ```

   ```c
   //c语言实现文件：cExample.c
   #include "cExample.h"
   
   int add( int x, int y ) {
       return x + y;
   }
   ```

   ```c++
   //C++实现文件，调用add函数
   #ifdef __cplusplus
   extern "C" {
   #endif
       
   #include "cExample.h"
       
   #ifdef __cplusplus
   }
   #endif
   
   int main() {
       add(2, 3);
       return 0;
   }
   ```

2. 在C语言中引用C++中的函数和变量时：（1）C++的头文件需要添加`extern "C"`声明，告诉编译器将其编译成C风格的函数；（2）在C语言中不能直接引用声明了`extern "C"`的C++头文件，应该仅在C文件中将C++定义的`extern "C"`函数声明为`extern`类型。

   下面给出C语言调用C++函数案例的3个源文件：

   ```c++
   // C++头文件 cppExample.h
   #ifndef CPP_EXAMPLE_H
   #define CPP_EXAMPLE_H
   
   extern "C" int add( int x, int y );
   
   #endif
   ```

   ```c++
   // C++实现文件 cppExample.cpp
   #include "cppExample.h"
   
   int add( int x, int y ) {
   	return x + y;
   }
   ```

   ```c
   // C实现文件 cFile.c
   // 加上这行会编译出错：#include "cExample.h"
   
   extern int add( int x, int y );
   
   int main( int argc, char* argv[] ) {
   	add( 2, 3 );
   	return 0;
   }
   ```

**参考**

[C和C++的互相调用](http://zjf30366.blog.163.com/blog/static/41116458201042425110860/)

[语言链接](https://zh.cppreference.com/w/cpp/language/language_linkage)



### define 和 typedef 

1. `#define` 作为预处理指令，在编译预处理时进行替换操作，不作正确性检查，只有在编译已被展开的源程序时才会发现可能的错误并报错。`typedef` 是关键字，在编译时处理，有类型检查功能，用来给一个已经存在的类型定义一个别名 。
2. `typedef` 用来定义类型的别名，方便使用。`#define` 不仅可以为类型取别名，还可以定义常量、变量、编译开关等。
3. `#define` 没有作用域的限制，只要是之前预定义过的宏，在以后的程序中都可以使用，如果在 .cpp 文件中定义了宏，则在整个文件中都可以使用该宏，如果在 .h 文件中定义了宏，则只要包含该头文件都可以使用；而 typedef 有自己的作用域，如果在函数之外定义了类型，则在整个文件中都可以使用该类型定义，如果在函数内部定义了该类型，则只能在函数内部使用该类型。

`typedef` 和 `#define` 在处理指针时不完全一样。如下程序可以清晰的说明它们之间的区别：

```c++
#include <stdio.h>

typedef char* ptr;
#define PTR char*

int main() {
    ptr a, b, c;
    PTR x, y, z;
    // typedef 定义了新的类型，因此变量 a, b, c 都属于指针类型;
    printf("sizeof a:%zu\n" ,sizeof(a) ); //8
    printf("sizeof b:%zu\n" ,sizeof(b) ); //8
    printf("sizeof c:%zu\n" ,sizeof(c) ); //8
    
    // define 只是做了简单的替换，被替换为语句 char *x, y, z 实际 y,z 为 char 类型。
    printf("sizeof x:%zu\n" ,sizeof(x) ); //8
    printf("sizeof y:%zu\n" ,sizeof(y) ); //1
    printf("sizeof z:%zu\n" ,sizeof(z) ); //1
    return 0;
}
```

### new/delete和malloc/free

用 `new` 创建对象时，首先从堆中申请相应的内存空间，然后调用对象的构造函数，最后返回指向对象的指针。new 操作符从自由存储区（free store）上为对象动态分配内存空间，而 malloc 函数从堆上动态分配内存。自由存储区是 C++ 基于 new 操作符的一个抽象概念，凡是通过 new 操作符进行内存申请，该内存即为自由存储区。而堆是操作系统中的术语，是操作系统所维护的一块特殊内存，用于程序的内存动态分配，C 语言使用 malloc 从堆上分配内存，使用 free 释放已分配的对应内存。

关于new的深入研究参考： https://cloud.tencent.com/developer/article/1177460

`new` 可以指定在内存地址空间创建对象，用法如下：

```c++
new (place_address) type
```

**new和malloc的对比：**

1. new是C++的一个操作符，malloc是C的一个函数。
2. new在申请内存的同时，会调用对象的构造函数进行初始化。malloc仅仅在堆中申请一块指定大小的内存空间，并不会对内存和对象进行初始化。
3. new可以指定内存空间初始化对象，malloc只能从堆中申请内存。
4. new的返回值是一个对象的指针类型；malloc统一返回void *指针，需要手动转换类型。
5. 分配内存失败后，new抛出bad_alloc异常；malloc返回NULL指针。
6. new申请的内存空间大小由编译器自动计算，malloc需要指定申请的空间大小。
7. new作为运算符可以重载，可以自定义申请内存的行为；malloc作为函数不可重载。
8. malloc 可以更改申请过的空间大小，我们可以 realloc 更改空间大小，而 new 一旦申请则无法更改。

**delete和free的对比：**

1. delete 是C++的一个操作符，可以进行重载，会执行对象的析构函数；

2. free 是C的一个函数，不能重载，只会释放指向的内存，不会执行对象的析构函数。

3. delete 首先执行该对象所属类的析构函数，进而通过调用 operator delete 的标准库函数来释放所占的内存空间。delete 用来释放单个对象所占的空间，只会调用一次析构函数；delete [] 用来释放数组空间，会对数组中的每个元素都调用一次析构函数。delete 只能用来释放 new 操作返回的指针，否则会产生不可预知的后果。

4. free 释放在堆中申请的动态内存空间，只能释放 malloc，calloc，realloc 申请的内存。需要注意的是，free 函数只是将参数指针指向的内存归还给操作系统，并不会把参数指针置 NULL，为了以后访问到被操作系统重新分配后的错误数据，所以在调用 free 之后，通常需要手动将指针置 NULL。

   > 内存资源都是由操作系统来管理的，而不是编译器，编译器只是向操作系统提出申请，所以 free 函数是没有能力去真正的 free 内存的，只是向内存管理模块归还了内存，其他模块还可以继续申请使用这些内存。free 后指针仍然指向原来的堆地址，实际还可以使用，但操作系统可能将这块内存已经分配给其他模块使用，一般建议在 free 以后将指针置为空。一个指针经过两次 free，也是比较危险的操作，因为可能该段内存已被别的内存使用申请使用了，free 之后会造成严重后果。

### volatile 的作用及应用场景

`volatile` 关键字的主要作用是防止编译器过度优化，用它声明的类型变量表示可以被某些编译器未知的因素更改，比如：操作系统、硬件或者其它线程等。**遇到这个关键字声明的变量，编译器对访问该变量的代码就不再进行优化，系统总是重新从它所在的内存读取数据，即使它前面的指令刚刚从该处读取过数据，而且读取的数据立刻被保存。所以说 volatile 可以保证对特殊地址的稳定访问。**

特别地，在多线程环境中，当两个线程都要用到某一个变量且该变量的值会被改变时，应该用`volatile`声明，该关键字的作用是防止优化编译器把变量从内存装入CPU寄存器中。如果变量被装入寄存器，那么两个线程有可能一个使用内存中的变量，一个使用寄存器中的变量，这会造成程序的错误执行。`volatile`的意思是让编译器每次操作该变量时一定要从内存中真正取出，而不是使用已经存在寄存器中的值。

> 但是，在解决多线程数据同步问题时，`volatile`也并不是正确地选择，而应该使用 `std::atomic` 或者互斥量、条件变量来保证多个线程之间对共享数据的操作行为是定义清晰的。

这篇文章讲的很好：https://zhuanlan.zhihu.com/p/62060524

### struct 和 union

1. union （联合体）和 struct（结构体）都是由若干个类型不同的数据成员组成。使用时，union 只有一个有效的成员，struct 所有的成员都有效。所以，union 中的所有成员变量共享同一段内存空间，它所占用的空间只要能容下最大的那个成员即可。struct 中的每个成员变量独占内存空间，在定义 struct 的时候最好妥善安排成员的顺序，尽量避免因为内存对齐造成的空间浪费。
2. 对 union的不同成员赋值，将会对覆盖其他成员的值，而对于struct的对不同成员赋值时，相互不影响。
3. union 的大小为其内部所有变量的最大值，按照最小类型的倍数进行分配大小；struct 分配内存的大小遵循内存对齐原则。
4. struct 可以定义变长数组成员变量 int a[]，union 中不能包含有这种不确定长度的变量。

#### union 和 struct 所占内存

```c++
#include <iostream>
using namespace std;

typedef union {
    char c[10];
    char cc1; // char 1 字节，按该类型的倍数分配大小
} u11;

typedef union {
    char c[10];
    int i; // int 4 字节，按该类型的倍数分配大小
} u22;

typedef union {
    char c[10];
    double d; // double 8 字节，按该类型的倍数分配大小
} u33;

typedef struct s1 {
    char c;   // 1 字节
    double d; // 1（char）+ 7（内存对齐）+ 8（double）= 16 字节
} s11;

typedef struct s2 {
    char c;   // 1 字节
    char cc;  // 1（char）+ 1（char）= 2 字节
    double d; // 2 + 6（内存对齐）+ 8（double）= 16 字节
} s22;

typedef struct s3 {
    char c;   // 1 字节
    double d; // 1（char）+ 7（内存对齐）+ 8（double）= 16 字节
    char cc;  // 16 + 1（char）+ 7（内存对齐）= 24 字节
} s33;

int main() {
    cout << sizeof(u11) << endl; // 10
    cout << sizeof(u22) << endl; // 12
    cout << sizeof(u33) << endl; // 16
    cout << sizeof(s11) << endl; // 16
    cout << sizeof(s22) << endl; // 16
    cout << sizeof(s33) << endl; // 24

    cout << sizeof(int) << endl;    // 4
    cout << sizeof(double) << endl; // 8
    return 0;
}
```

#### union的用法

```c++
#include <iostream>

// Define a union with different types of members
union MyUnion {
    int integerData;
    double doubleData;
    char charData;
};

int main() {
    // Create an instance of the union
    MyUnion myUnion;

    // Assign values to the members
    myUnion.integerData = 42;
    std::cout << "Integer data: " << myUnion.integerData << std::endl; // Integer data: 42

    // Change the member and print the value
    myUnion.doubleData = 3.14;
    std::cout << "Double data: " << myUnion.doubleData << std::endl; // Double data: 3.14

    // Access another member and print the value
    myUnion.charData = 'A';
    std::cout << "Char data: " << myUnion.charData << std::endl; // Char data: A

    // Since all members share the same memory space, changing one member affects the others
    // Integer data after changing char data: 1374389569，变成了随机值
    std::cout << "Integer data after changing char data: " << myUnion.integerData << std::endl;

    return 0;
}
```

#### C和C++ struct的区别

1. 在 C 语言中 `struct` 是用户自定义数据类型，没有访问权限的设置，是一些变量的集合体，不能定义成员函数。
2. `struct` 在C++ 中和 class几乎一样，可以继承、可以实现多态，它和 class 的主要区别：
   1. C++ struct的成员默认是public，而 class 默认是private。
   2. C++ struct 继承默认是public 继承，而 class 默认是 private 继承。
   3. C++ struct不能用于定义模板参数，而 class是可以的。 
3. C 语言中 struct 定义的自定义数据类型，在定义该类型的变量时，需要加上 struct 关键字，例如：struct A var;，定义 A 类型的变量；而 C++ 中，不用加该关键字，例如：A var。
4. C++中的 struct 主要是为了兼容C而做的保留字。

### explicit 避免隐式转换

`explict` 用来声明类构造函数是显式调用的，而非隐式调用，可以阻止调用构造函数时进行隐式转换和赋值初始化。**它只能用于单参构造函数，因为无参构造函数和多参构造函数本身就是显式调用的，加上explict 关键字也没有意义**。

```c++
class A {
public:
    int var;
    A(int tmp) {
        var = tmp;
    }
};
int main() {
    A ex = 10; // 发生了隐式转换
    return 0;
}
```

```c++
class A {
public:
    int var;
    explicit A(int tmp) {
        var = tmp;
        cout << var << endl;
    }
};
int main() {
    A ex(100);
    A ex1 = 10; // error: conversion from 'int' to non-scalar type 'A' requested
    return 0;
}
```

> 在工程实践中，一般情况下建议声明 `explicit` 的单参构造函数，从而可以阻止编译器执行非预期 (往往也不被期望) 的类型转换，因为某些非预期的类型转换可能会引起意向不到的错误。



## 2.6 常用库函数

### sizeof()和strlen()

二者不同之处：

- strlen() 是C头文件中的库函数，在程序运行过程中，用于计算字符串的实际长度，以 '\0' 结束；strlen() 的参数必须是 char * 类型的变量。
- sizeof() 是C++中的运算符，在编译时计算对象或者表达式类型占用的字节大小。sizeof() 接受的参数可以是对象也可以是表达式，但是 sizeof(expression) 在运行时不会对接受的表达式进行计算，**编译器只会推导表达式的类型从而计算占用的字节大小；而 strlen() 是一个函数，如果接受表达式则会对表达式进行运算。**

```c++
#include <iostream>
#include <cstring>

using namespace std;

//若字符数组 arr 作为函数的形参：
//	sizeof(arr) 中 arr 被当作字符指针(char*)来处理，所以得到的是指针的长度
//	strlen(arr) 中 arr 依然是字符数组
void size_of(char arr[]) {
    cout << sizeof(arr) << endl; // 8
    cout << strlen(arr) << endl; // 5 
}

int main() {
    char arr[10] = "hello";
    cout << strlen(arr) << endl; // 5
    cout << sizeof(arr) << endl; // 10

    size_of(arr);
    return 0;
}
```

使用 `std::string`的时候，最好使用 `std::string` 原生提供的 `length()` 方法去获取字符串长度：

```c++
string s = "hello";
s.push_back(0);
s.push_back('w');
cout << strlen(s.c_str()) << endl; // 5，中间插入的 0 元素会影响 strlen 计算底层 c_str 的实际长度
cout << sizeof(s) << endl;         // 32，计算的是string对象的所占空间的大小，永远是32
cout << s.length() << endl;        // 7
cout << s.capacity() << endl;      // 15
```

### memcmp()函数

`memcmp()`函数用于比较两块内存区域的内容是否相同（逐个字节比较），只要返回值不为0，即认为比较的 n 个字节内容是不同的。

```c++
SYNOPSIS
       #include <string.h>

       int memcmp(const void *s1, const void *s2, size_t n);

DESCRIPTION
       The  memcmp()  function  compares the first n bytes (each interpreted as unsigned char) of the memory areas s1
       and s2.
```



#### 可以用memcmp()判断结构体相等吗？

不能直接用 `memcmp()` 来判断两个结构体是否相等，因为 `memcmp()` 函数是逐个字节进行比较的，而结构体内部会自动做内存对齐，内存对齐时补的字节内容是随机的，会产生垃圾值，所以无法比较。正确的做法是重载操作符 `==` 来判断两个结构体是否相等：

```c++
#include <iostream>
using namespace std;

struct A {
    char c;
    int val;
    A(char c_tmp, int tmp) : c(c_tmp), val(tmp) {}

    // 如果是私有变量的话，友元函数能在类外获取。 所以这里用友元函数是规范化
    friend bool operator==(const A &tmp1, const A &tmp2); //  友元运算符重载函数
};

bool operator==(const A &tmp1, const A &tmp2){
    return (tmp1.c == tmp2.c && tmp1.val == tmp2.val);
}

int main() {
    A ex1('a', 90), ex2('b', 80);
    if (ex1 == ex2)
        cout << "ex1 == ex2" << endl;
    else
        cout << "ex1 != ex2" << endl; // 输出
    return 0;
}
```

### strcpy()和strncpy()

`strcpy()` 是 C++ 语言的一个标准函数 ，strcpy 把含有 '\0' 结束符的字符串复制到另一个地址空间，返回值的类型为 char*，返回值为拷贝后的字符串的首地址。其源代码实现如下：

```c
char * strcpy(char * strDest,const char * strSrc) {
    if ((NULL==strDest) || (NULL==strSrc)) 
    throw "Invalid argument(s)"; 
    char * strDestCopy = strDest; 
    while ((*strDest++=*strSrc++)!='\0'); 
    return strDestCopy;
}
```

strcpy() 函数的缺陷在于：strcpy 函数不检查目的缓冲区的大小边界，而是将源字符串逐一的全部赋值给目的字符串地址起始的一块连续的内存空间，同时加上字符串终止符，会导致其他变量被覆盖。例如：

```c++
int main() {
    int var = 0x11112222;
    char arr[10];
    cout << "Address : var " << &var << endl;
    cout << "Address : arr " << &arr << endl;
    strcpy(arr, "hello world!");
    cout << "var:" << hex << var << endl; // 将变量 var 以 16 进制输出
    cout << "arr:" << arr << endl;
    return 0;
}
/* 输出：
Address : var 0x23fe4c
Address : arr 0x23fe42
var:11002164
arr:hello world!
*/
```

从上述代码中可以看出，变量 var 的后六位被字符串 "hello world!" 的 "d!\0" 这三个字符改变，这三个字符对应的 ascii 码的十六进制为：\0(0x00)，!(0x21)，d(0x64)。因为变量 arr 只分配了 10 个字节长度的内存空间，通过上述程序中的地址可以看出 arr 和 var 在内存中是连续存放的，但是在调用 strcpy 函数进行拷贝时，源字符串 "hello world!" 所占的内存空间为 13，因此在拷贝的过程中会占用 var 的内存空间，导致 var 的后六位被覆盖。

*由于 strcpy 函数存在一定的安全风险，如果使用不当容易出现安全问题，利用 strcpy 的特性可以编写 shellcode 来进行缓冲区溢出攻击。在大多数工程代码中，为了保证代码的健壮性和安全性，一般会使用 `strncpy` 代替 strcpy。*

### assert()宏

`assert`是 C++ 定义的一个宏，用于检查指定的条件表达式是否成立，如果不成立则会终止程序执行，可以指定断言失败时输出的 message。如果在引入头文件位置前面定义了 `NDEBUG` 这个宏，`assert` 就会被禁掉，这也说明这个宏一般用于 debug 版本，release 版本不应该使用它。其用法如下：

```c++
#include <iostream>
// #define NDEBUG
#include <cassert>
 
// Use (void) to silence unused warnings.
#define assertm(exp, msg) assert(((void)msg, exp))
 
int main() {
    assert(2 + 2 == 4);
    std::cout << "Checkpoint #1\n";
 
    assert((void("void helps to avoid 'unused value' warning"), 2 * 2 == 4));
    std::cout << "Checkpoint #2\n";
 
    assert((010 + 010 == 16) && "Yet another way to add an assert message");
    std::cout << "Checkpoint #3\n";
 
    assertm((2 + 2) % 3 == 1, "Success");
    std::cout << "Checkpoint #4\n";
 
    assertm(2 + 2 == 5, "Failed"); // assertion fails
    // examples.bin: /home/hunk/workspace/src/github.com/Notes/cpp/examples/src/main.cpp:23: int main(): Assertion `((void)"Failed", 2 + 2 == 5)' failed.
    std::cout << "Execution continues past the last assert\n"; // No output
}
```



## 2.6 Class设计基础

### C++ class的经典分类

可以根据内部是否带有指针类型数据来将类分为两种：

1. 不带指针的类（Class without pointer members），比如后面的Complex类。
2. 带指针的类（Class with pointer members），比如后面的string类。

### 设计Complex类

```c++
class Complex
{
public:
    //构造函数，没有返回类型
    Complex (double r = 0, double i = 0)
        : re(r), im(i) //初值列
    { ... }
    
    //常量成员函数：const修饰函数，表示函数不会修改数据，注意const的位置
    double real() const { return re; }//返回符数实部
    double imag() const { return im; }//返回负数虚部
    
    //在传递函数参数和返回值时，尽量传递引用而不是传递值，效率更高
    //如果不希望传递的数据被修改，可以加上const修饰
    Complex& operator += (const Complex&);
    
    int func(const Complex& param) { return param.re + param.im; }
    
private:
    //数据一般放到private区，然后用合适的public函数来提供访问接口
    double re, im; //复的实部和虚部
    
    //友元函数
    friend Complex& __doapl (Complex*, const Complex&);
};

//在这个函数中，第一个参数会被改变，第二个参数不会被改变
//do assignment plus
inline Complex& __doapl (Complex* ths, const Complex& r)
{
    ths->re += r.re; //自由取得friend的private成员数据
    ths->im += r.im;
    
    return *ths;
}

inline double imag(const Complex& x) 
{
    return x.imag();
}

inline double real(const Complex& x) 
{
    return x.real();
}
```

```c++
//caller
{
    Complex c1(2, 1);
    Complex c2;
    
    //相同class的各个对象互为友元
    c2.func(c1);
    
    c2 += c1;
}
```

#### 传递值和传递引用

无论是参数传递还是返回值传递，**传递者都无需知道接收者是以引用形式接收**，因此直接传递值就可以了，比如Complex类中\_\_doapl函数中返回*ths是OK的，以及c2 += c1的运算也是OK的。

**什么情况下不能传递引用？**

是否可以用传递引用代替值传递，主要是考虑数据的生命周期，在函数返回数据时：

1. 如果返回的是一个局部变量甚至临时对象，那么就不能传递引用，因为函数调用结束后数据本身就会被销毁。
2. 如果返回的数据本身在函数返回后仍然存在，则可以传递引用。比如上面Complex类的\_\_doapl函数，第一个参数ths在调用\_\_doapl之前已经存在，调用结束后依然存在，因此可以传递引用。

创建临时对象的基本语法：typename()，例如：

```c++
{
    Complex();//临时对象
    Complex(2, 4);//临时对象
    //至此，上面两个临时对象的生命周期已经结束
    ...
}
```

#### 操作符重载

**操作符重载1 --成员函数**

重载操作符时，重载函数的参数依次代表了左右操作数。后面的这些成员函数都应该在Complex.cpp中实现。

```c++
//Complex.cpp
//操作符重载1 -- 成员函数，包含this
//实现+=这个操作符
inline Complex& Complex::operator += (const Complex& r)
{
    //每个成员函数都包含一个隐式参数：this指针
    return __doapl(this, r);
}
```

**操作符重载2 --非成员函数**

```c++
//操作符重载2 -- 非成员函数，没有this
//c2 = c1 + c2;
inline Complex operator + (const Complex& x, const Complex& y)
{
    return Complex (real(x) + real(y), imag(x) + imag(y));
}

//c2 = c1 + 5;
inline Complex operator + (const Complex& x, double y)
{
    return Complex (real(x) + y, imag(x));
}

//c2 = 7 + c1;
inline Complex operator + (double x, const Complex& y)
{
    return Complex (x + real(y), imag(y));
}

//cout << +c1;
inline Complex operator + (const Complex& x)
{
    return x;
}

//cout << -c1;
inline Complex operator - (const Complex& x)
{
    return Complex (-real(x), -imag(x));
}
```

> 注意，这几个重载函数返回的都是值类型，因为它们返回的是函数内部创建的临时对象，所以这里绝不能返回引用（return by reference）。

重载等于和不等于操作符：

```c++
inline bool operator == (const Complex& x, const Complex& y)
{
    return real(x) == real(y) && imag(x) == imag(y);
}

inline bool operator != (const Complex& x, const Complex& y)
{
    return real(x) != real(y) || imag(x) != imag(y);
}
```

重载输出操作符：

```c++
#include <iostream.h>

ostream& operator << (ostream& os, const Complex& x)
{
    return os << '(' << real(x) << ',' << imag(x) <<  ')';
}
```

对于输出单个对象，我们可以在重载函数中返回void。但是更普遍的场景是连续输出，比如cout << c1 << c2;，因此需要在重载函数里返回传入的ostream对象，使它可以继续作用于后面的输出对象。

在Complex类的设计中主要介绍了构造函数和操作符重载，以及在函数参数传递和返回时如何正确使用引用传递。下面介绍的字符串类属于成员数据带指针的类，以此引出类的其它几个重要成员函数。

### 设计String类

先通过字符串类型的基本用法看看一个最简单的字符串类需要设计哪些东西：

```c++
//string-test.cpp
int main {
    String s1();
    String s2("hello"); //调用构造函数
    
    String s3(s1); //调用拷贝构造函数
    cout << s3 << endl;//调用输出操作符
    s3 = s2; //调用拷贝赋值函数
    
    cout << s3 << endl;
    
    String* p = new String("hello");
    delete p;//调用析构函数
}
```

先给出String类的基本声明，包含以下几个重要成员函数：

1. 构造和析构函数
2. 拷贝构造函数（copy constructor operator）
3. 拷贝赋值函数（copy assignment operator）
4. 操作符重载

```c++
class String
{
public:
    String(const char* cstr = 0);
    String(const String& str);
    String& operator=(const String& str);
    ~String();//析构函数
	char* get_c_str() const { return m_data; }//用于字符串的输出

private:
	char* m_data;
};

//重载输出操作符
ostream& operator<<(ostream& os, const String& str)
{
    os << str.get_c_str();
    return os;
}
```

#### 构造和析构函数

编译器也会提供默认的构造和析构函数。

```c++
inline String::String(const char* cstr = 0)
{
    if (cstr) {
        m_data = new char[strlen(cstr)+1];
        strcpy(m_data, cstr);
	} else { // 未指定初值
        m_data = new char[1];
        *m_data = '\0';
	}
}

//析构函数用以释放对象占用的空间，在对象声明周期结束的时候会被自动调用
inline String::~String()
{
	delete[] m_data;
}
```

#### 拷贝构造/赋值函数

前面在 Complex 类中我们没有去实现拷贝构造函数和拷贝赋值函数也是OK的，是因为编译器会自动提供这些拷贝函数的默认版本。但是，默认版本只会完成最基础的成员数据拷贝，如果类包含指针成员，那么拷贝后的结果就是两个对象指向同一份底层数据，也就是发生了**浅拷贝**。比如下图中 m_data 指针的浅拷贝，这会带来两个问题：

1. 其中一份数据没有被正常释放，造成内存泄漏。
2. 两个对象的成员指针指向同一个底层数据，任何修改会相互影响。

![image-20200823202356309](https://hunk-pic-store.oss-cn-beijing.aliyuncs.com/img/image-20200823202356309.png)

我们的期望是在拷贝对象时，能够同时拷贝底层数组，拷贝后的对象互相不会影响，也就是**深拷贝**。所以，对于这种带指针成员的类，我们一定要实现自定义的**拷贝构造函数**和**拷贝赋值函数**。

**拷贝构造函数**相对简单，只需要申请空间然后拷贝对象里的数据即可。拷贝构造函数仍然属于构造函数，所以没有返回值。

```c++
inline String::String(const String& str)
{
	m_data = new char[ strlen(str.m_data) + 1 ];//申请合适的空间
    strcpy(m_data, str.m_data);//拷贝底层字符数组
}
```

**拷贝赋值函数**稍微复杂一些，在S2 = S1操作中，我们需要先释放S2原来分配的资源，然后重新分配和S1的数据相同大小的空间，最后再将S1的数据内容拷贝到S2。

```c++
inline String& String::operator = (const String& str)
{
    //0.检测自我赋值（self assignment）
    if(this == &str) return *this;
    
    //1.释放原有资源
    delete[] m_data;
    
    //2.申请新的空间
	m_data = new char[ strlen(str.m_data) + 1 ];
    
    //3.//拷贝底层字符数组
    strcpy(m_data, str.m_data);
    
    return *this;
}
```

拷贝赋值函数中最开始的自我赋值检测非常重要，否则可能产生内存访问的不确定行为。

![image-20200823203642663](https://hunk-pic-store.oss-cn-beijing.aliyuncs.com/img/image-20200823203642663.png)

### 深拷贝和浅拷贝

如果一个类拥有资源，该类的对象进行复制时，如果资源重新分配，就是深拷贝，否则就是浅拷贝。

1. 深拷贝（Deep Copy）：该对象和原对象占用不同的内存空间，既拷贝存储在栈空间中的内容，又拷贝存储在堆空间中的内容。
2. 浅拷贝（Shallow Copy）：该对象和原对象占用同一块内存空间，仅拷贝类中位于栈空间中的内容。

![image-20240418182642294](https://hunk-pic-store.oss-cn-beijing.aliyuncs.com/img/20240418182643.png)

当类的成员变量中有指针变量时，最好使用深拷贝。因为当两个对象指向同一块内存空间，如果使用浅拷贝，当其中一个对象的删除后，该块内存空间就会被释放，另外一个对象指向的就是垃圾内存。

编译器生成的默认拷贝函数均大部分都是浅拷贝，所以在特定场景下需要禁止编译器生成默认拷贝构造函数，即自定义拷贝构造函数，在前面的设计 String 类时也提到过。

> 在遇到需要使用堆内存的构造函数中，我们需要特别注意浅拷贝和深拷贝的使用方式，防止两个不同的对象指向同一块内存区域。

浅拷贝实例：

```c++
#include <iostream>
using namespace std;

class Test {
private:
    int *p;

public:
    Test(int tmp){
        this->p = new int(tmp);
        cout << "Test(int tmp)" << endl;
    }
    ~Test() {
        if (p != NULL) {
            delete p;
        }
        cout << "~Test()" << endl;
    }
};

int main() {
    Test ex1(10);    
    Test ex2 = ex1; // 类对象 ex1、ex2 实际上是指向同一块内存空间，对象析构时，ex2 先将内存释放了一次，
                        // 之后析构对象 ex1 时又将这块已经被释放过的内存再释放一次。
                        // 对同一块内存空间释放了两次，会导致程序崩溃。
     return 0;
}
```

深拷贝实例：

```c++
#include <iostream>
using namespace std;

class Test {
private:
    int *p;

public:
    Test(int tmp){
        p = new int(tmp);
        cout << "Test(int tmp)" << endl;
    }
    ~Test(){
        if (p != NULL) { delete p; }
        cout << "~Test()" << endl;
    }
    Test(const Test &tmp) { // 定义拷贝构造函数
        p = new int(*tmp.p);
        cout << "Test(const Test &tmp)" << endl;
    }
};

int main() {
    Test ex1(10);    
    Test ex2 = ex1; 
    return 0;
}
```







### 深入理解构造函数

#### 为什么拷贝构造函数必须声明为引用？

结论：*避免拷贝构造函数无限制的递归而导致栈溢出*。先来看一下什么情况下会调用拷贝构造函数：

1. 直接初始化和拷贝初始化的时候：

   ```c++
   string dots("zhang"); //直接初始化
   string dots = "zhang" //拷贝初始化
   ```

2. 将一个对象作为实参传递给一个非引用或者非指针类型的形参的时候。

3. 从一个返回类型为非引用或非指针的函数返回一个对象时。

4. 用花括号列表初始化一个数组的元素或者一个聚合类中的成员时。

来看下面的代码：

```c++
#include <iostream>
using namespace std;

class A {
private:
    int val;
public:
    A(int tmp) : val(tmp) { // 带参数构造函数
        cout << "A(int tmp)" << endl;
    }

    A(const A &tmp) { // 拷贝构造函数
        cout << "A(const A &tmp)" << endl;
        val = tmp.val;
    }

    A &operator=(const A &tmp) { // 赋值运算符重载
        cout << "A &operator=(const A &tmp)" << endl;
        val = tmp.val;
        return *this;
    }

    void fun(A tmp) {
    }
};

int main() {
    A ex1(1); // A(int tmp) 
    A ex2(2); // A(int tmp)
    A ex3 = ex1; // A(const A &tmp)，ex3还没初始化，所以调用的是拷贝构造函数，即拷贝构造函数的隐式调用
    ex2 = ex1; // A &operator=(const A &tmp)，对象ex2已经实例化了，不需要构造，此时只是将ex1赋值给ex2，所以调用赋值运算符重载
    ex2.fun(ex1); // A(const A &tmp)
    return 0;
}
```

如果拷贝构造函数中的形参不是引用类型，那么上面的代码  'A ex3 = ex1' 会出现什么问题？构造 ex3，实质上是 ex3.A(ex1);，假如拷贝构造函数参数不是引用类型，那么将使得 ex3.A(ex1); 相当于 ex1 作为函数 A(const A tmp) 的实参，在参数传递时相当于 A tmp = ex1，因为 tmp 没有被初始化，所以在 A tmp = ex1 中继续调用拷贝构造函数，接下来的是构造 tmp，也就是 tmp.A(ex1) ，必然又会有 ex1 作为函数 A(const A tmp); 的实参，在参数传递时相当于即 A tmp = ex1，那么又会触发拷贝构造函数，就这下永远的递归下去。

#### 如何禁止构造函数的使用？

为类的构造函数增加 `= delete` 修饰符，可以达到虽然声明了构造函数但禁止使用的目的：

```c++
#include <iostream>

using namespace std;

class A {
public:
    int var1, var2;
    A(){
        var1 = 10;
        var2 = 20;
    }
    A(int tmp1, int tmp2) = delete;
};

int main() {
    A ex1;    
    A ex2(12,13); // error: use of deleted function 'A::A(int, int)'
    return 0;
}
```

如果我们仅仅将构造函数设置为私有，类内部的成员和友元还可以访问，无法完全禁止。而在 C++11 以后，在成员函数声明后加 `= delete`则可以禁止该函数的使用，而需要保留的加 `= default`。

**哪些场景要禁止构造函数呢？**

1. 单例模式中只需要创建一个对象，不希望我们自己去创建对象，就会把构造函数私有，通过一个静态的方法获取到唯一一个实例对象，工厂模式中也是私有化特定的商品类的构造函数，只通过工厂类来创建对象，一般这种禁止构造函数都是设计模式的一种思想。

#### 默认构造函数

默认构造函数（default constructor）就是在没有显式提供初始化式时调用的构造函数。它由不带参数的构造函数，或者为所有的形参提供默认实参的构造函数定义。如果定义某个类的变量时没有提供初始化时就会使用默认构造函数。

1. 用户自定义的默认构造函数：

   ```c++
   class A
   {
   public:
       A(){ // 自定义默认构造函数1：不带任何参数
           var = 10;
           c = 'q';
       }
       A(int _var = 10, char _c = 'q'){ // 自定义默认构造函数2：为所有形参提供默认值
           var = _var;
           c = _c;
       }
       A(int val = 10)
       int var;
       char c;
   };
   ```

2. 如果用户定义的类中没有显式的定义任何构造函数，编译器在必要的情况下会自动为该类型生成默认构造函数，称为**合成默认构造函数**。

> 一般情况下，如果类中包含内置或复合类型的成员，则该类就不应该依赖于合成的默认构造函数，它应该定义自己的构造函数来初始化这些成员。

#### 如何减少构造函数开销？

在构造函数时尽量使用*类初始化列表*，会减少调用默认的构造函数产生的开销。*因为它允许在对象构造时直接对成员变量进行初始化，而不是在构造函数体内进行赋值操作。这样可以避免了先调用默认构造函数再进行赋值的过程，从而提高了效率*。此外，对于一些特定的数据类型，比如引用类型和const类型的成员变量，只有成员初始化列表才能进行初始化，因此使用成员初始化列表是必要的。

总的来说，使用成员初始化列表可以减少不必要的中间步骤，提高了代码的执行效率。

例如下面的演示代码，相比于Test1，Test2的初始化过程多调用了一次 A 的构造函数：

```c++
#include <iostream>
using namespace std;

class A {
private:
    int val;
public:
    A() { cout << "A()" << endl; }
    A(int tmp) {
        val = tmp;
        cout << "A(int " << val << ")" << endl;
    }
};

class Test1 {
private:
    A ex;
public:
    Test1() : ex(1) {} // 成员列表初始化方式 
};

class Test2 {
private:
    A ex;
public:
    Test2() { ex = A(2); }// 函数体中赋值的方式
};

int main(){
    Test1 ex1;
    cout << "------------------" << endl;
    Test2 ex2;
    return 0;
}
/* 运行结果：
A(int 1)
------------------
A()
A(int 2)
*/
```

## 2.7 对象内存管理

### 对象生命周期

```c++
Complex c3(1, 2);//全局对象
    
int main 
{
    //栈对象
    Complex c1(1, 2);//c1占用的空间来自栈
    
    //静态对象
    static Complex c2(1, 2);
    
    //Complex(3)是个临时对象，但是它所占用的空间是从堆上动态分配，并由p指向
    Complex* p = new Complex(1, 2);
    delete p；
}
```

1. 栈对象（stack objects）：c1便是栈对象，其生命在作用域结束之际结束。
2. 静态对象（static local objects）：c2便是静态对象，其生命在作用域结束之后仍然存在，直到整个程序结束。
3. 全局对象（global objects）：c3就是全局对象，其生命在整个程序结束后才结束，因此也可以将其看作一种静态对象。
4. 堆对象（heap objects）：p所指的就是堆对象，其生命在它被delete之后结束。如果没有恰当的执行delete，在作用域结束后，指针p的生命周期结束了，但是它指向的堆对象仍然存在，占用的资源不能被正确释放，导致内存泄漏。

### new/delete和malloc/free

`new`和`malloc`都用于申请堆上的动态内存，`delete`和`free`用于释放内存。`malloc` 和 `free` 是 C/C++ 语言的标准库函数，`new` 和 `delete` 是C++的运算符。

`malloc`分配时的大小是人为计算的，只能分配指定大小的堆内存空间，返回类型是`void*`，使用时需要类型转换，而`new`在分配时，编译器能够根据对象类型自动计算出大小，返回类型是指向对象类型的指针。如下图所示，`new`的底层操作实际上可以分为3步：

1. 通过调用  `operator new` 函数分配一块合适，原始的，未命名的内存空间，返回类型也是`void *`，而且可以通过重载 `operator new` 来自定义内存分配策略，甚至不做内存分配，甚至分配到非内存设备上，而`malloc`函数做不到。
2. 调用`static_cast`将`void *`转换成目标对象类型。
3. 调用构造函数构造对象，`new`将调用合适的构造函数实现，而`malloc`不能；对应的，释放对象资源时，`delete`将调用析构函数，而`free`不能。

![image-20200823220834552](https://hunk-pic-store.oss-cn-beijing.aliyuncs.com/img/image-20200823220834552.png)

1. Complex类只有两个double类型的成员数据，所以第一步为Complex对象分配内存的时候分配的就是2个double的空间。
2. 按大小为对象分配了空间，随后需要做类型转换。
3. 最后，调用对象的构造函数完成初始化。

![image-20200823220905752](https://hunk-pic-store.oss-cn-beijing.aliyuncs.com/img/image-20200823220905752.png)

**delete**可以看作**new**的逆向操作，先调用析构函数（一般是释放构造函数中为成员数据申请的堆内存），最后释放对象对象本身占用的堆内存。

### 堆对象内存布局

下面这张图描述了VC编译器下动态分配的堆对象的内存布局：

![image-20200823222220528](https://hunk-pic-store.oss-cn-beijing.aliyuncs.com/img/image-20200823222220528.png)

两组分别描述Complex对象和String对象，左边比较长的内存块都是带调试信息的。内存块顶部和底部的叫做cookie，每个占用4字节，它记录了这个内存块的大小和结束位置，以带调试信息的Complex对象为例：

1. 青色部分两个double成员数据占用8个字节。
2. 灰色部分调试信息占用32+4个字节。
3. 红色的cookie一共占用4*2个字节。

总共是52个字节，因为要16字节对齐，所以总共分配了64个字节（其余地方用0填满），64等于0x40，cookie最后一位表示内存已经被分配（这有助于操作系统管理内存），因为分配的内存块都是16的倍数，所以后面4位始终是0，借用最后一位标记分配是OK的。

下面这张图描述了VC编译器下动态分配的数组的内存布局：

![image-20200823222019777](https://hunk-pic-store.oss-cn-beijing.aliyuncs.com/img/image-20200823222019777.png)

### delete和delete[]

删除动态创建的数组时，如果只是用了delete而没有用delete[]，则可能导致成员指针指向的堆对象不能被释放，编译器只会调用第一个数组元素的析构函数。

![image-20200823221250104](https://hunk-pic-store.oss-cn-beijing.aliyuncs.com/img/image-20200823221250104.png)

当然，对象本身占用的空间始终是可以正常释放的，因为对象的存储空间里记录了整个对象的大小信息。如果是Complex对象数组，用delete去删除也不会发生内存泄漏。如果是String对象数组，就会发生内存泄漏。

> 为了良好的编程习惯，new出来的动态数组始终用delete[]去删除。

## 2.8 Class设计提高

### 静态成员数据和函数

前面介绍的Complex类和String类的成员数据都是非静态的，每个类对象都独有一份自己的数据。成员函数都有一个隐式参数this指针（指向类对象本身），它由编译器自动添加，我们不用写在函数参数列表里，但是可以在函数内直接使用。访问非静态成员数据时，实际上就是通过this指针访问一个具体类对象的数据。

![image-20200824201626970](https://hunk-pic-store.oss-cn-beijing.aliyuncs.com/img/20200825091629.png)

如果我们希望所有的类对象共享同一份成员数据，就需要使用static关键字将其定义成静态成员数据。比如每个银行客户都有自己的账户，但是所有账户使用相同的利率。相应的，静态函数就是专门（也只能）用于处理静态成员数据，我们可以通过类名直接调用静态成员函数来访问静态成员数据，也可以通过类对象访问。

![image-20200824201933836](https://hunk-pic-store.oss-cn-beijing.aliyuncs.com/img/20200825091630.png)

> Account类黄色部分代码是很重要的，因为类内部只是声明了一个静态成员数据m_rate，实际上没有获得内存空间，我们需要在类的外部对其进行定义和初始化（也可以不初始化）。否则，在程序链接阶段会遇到“undefined reference”的错误。

### 把构造函数放在私有区

对于单例模式，我们经常把构造函数放到private区，然后通过静态的getInstance方法返回静态实例。只要有地方首次调用了这个方法，之后所有的调用操作的都是同一份数据。

![image-20200824203549418](https://hunk-pic-store.oss-cn-beijing.aliyuncs.com/img/20200825091631.png)

但是这个实现有个问题，即便没有人调用getInstance方法，静态成员a仍然存在，可以考虑仅在首次调用getInstance后才会创建这个数据：

![image-20200824203959609](https://hunk-pic-store.oss-cn-beijing.aliyuncs.com/img/20200825091632.png)

### 不允许成员函数修改类的成员变量（const）

如果想达到一个类的成员函数不能修改类的成员变量，只需用 `const` 关键字来修饰该函数即可。同时 C++ 还存在与 `const` 相反的关键字 `mutable`。被 `mutable` 修饰的变量，将永远处于可变的状态，即使在一个 const 函数中。如果我们需要在 `const` 函数中修改类的某些成员变量，这时就需要用到 `mutable`。

```c++
#include <iostream>
using namespace std;

class A {
public:
    mutable int var1;
    int var2;
    A() {
        var1 = 10;
        var2 = 20;
    }
    void fun() const // 不能在 const 修饰的成员函数中修改成员变量的值，除非该成员变量用 mutable 修饰
    {
        var1 = 100; // ok
        var2 = 200; // error: assignment of member 'A::var1' in read-only object
    }
};

int main() {
    A ex1;
    return 0;
}
```

使用 `mutable` 的注意事项：

1. `mutable` 只能作用于类的非静态和非常量数据成员。
2. 应尽量避免在类中大量使用 `mutable`，大量使用 `mutable` 表示程序设计存在缺陷。

### 再谈操作符重载

#### 关于std::cout

为什么std::cout可以输出很多不同类型的数据呢？看完下面这张图就明白了，因为它的底层类**ostream**重载了不同类型参数的<<操作。

![image-20200824204441485](https://hunk-pic-store.oss-cn-beijing.aliyuncs.com/img/20200825091633.png)

#### 关于迭代器

迭代器是访问容器数据的常用工具，在迭代器的设计中同样利用了大量的操作符重载来实现迭代器的基本操作。下面的__list_iterator是作用于list这种数据结构的迭代器，它主要包含一个\_\_list_node类型的指针数据node，用于指向list中的一个元素。

![image-20200829184041089](https://hunk-pic-store.oss-cn-beijing.aliyuncs.com/img/image-20200829184041089.png)

对迭代器的解引用操作 *ite，其实是希望得到list节点里的数据，所以返回的是node里的数据对象data，其类型T是在创建list时指定。

在迭代器上调用方法 ite->method()，其实是调用list节点里的数据对象data的方法，因此等价于(\*ite).method() 或者 (&(*ite))->method();。

![image-20200829185405862](https://hunk-pic-store.oss-cn-beijing.aliyuncs.com/img/image-20200829185405862.png)

#### 关于重载new和delete操作符

![image-20200910093158810](https://hunk-pic-store.oss-cn-beijing.aliyuncs.com/img/image-20200910093158810.png)

### 常量成员函数和常量对象

![image-20200910093518468](https://hunk-pic-store.oss-cn-beijing.aliyuncs.com/img/image-20200910093518468.png)

![image-20200910093423956](https://hunk-pic-store.oss-cn-beijing.aliyuncs.com/img/image-20200910093423956.png)

### 转换函数（conversion function）

转换函数可以将类对象转换成指定的类型，其实就是特殊的操作符重载，只不过重载的操作符是类型，转换函数可以帮助编译器在需要的时候自动完成类型转换。来看下面的例子：

```c++
class BaseTask
{
private:
    string m_id;
    string m_type; 

public:
    BaseTask();
    BaseTask(string id, std::string type);
    ~BaseTask();

    //...
    virtual string serialize();

    //转换函数，编译器可以通过它将BaseTask对象转换成string
    //编译器允许不用写转换函数的返回类型，因为编译器可以通过
    //转化函数的名字自动推导出返回类型
    operator string() { return serialize(); }
    operator int() { return (int)serialize().size(); }
};
```

```c++
{
    BaseTask task2;
    string task2Str(task2);//调用operator string()
    cout << "task2Str: " << task2Str << endl;
    int total = 10 + task2; //调用operator int()
    cout << "task2Int: " << total << endl;
}

//输出：
//task2Str: [m_id:1B-14-3C-F8-D6-D8-41-B5-80-D7-A4-D4-64-1B-F6-42, m_type: Base]
//task2Int: 78
```

在编译 int total = 10 + task2 时，编译器会先尝试在全局去找是否有一个 + 操作符的重载版本：第一个参数是int，第二个参数类型是BaseTask。没有找到合适的+操作符版本后，编译器继续尝试将 task2 转换成int，因此去找 BaseTask 是否提供了合适的转换函数可以将 BaseTask 对象转换成int，最终调用operator int()完成转换。

### non-explicit-one-argument ctor

转换函数是将类对象转换成指定的类型，同样的，我们也可以将指定类型的数据转换成临时的类对象，然后继续参与运算。主要依靠一种叫做non-explicit-one-argument的构造函数提供这种转换，这种构造函数可以有多个参数，但是允许调用者只提供一个实参。看下面这个表示分数的例子：

![image-20200829175217860](https://hunk-pic-store.oss-cn-beijing.aliyuncs.com/img/image-20200829175217860.png)

注意，Fraction的构造函数第一个参数类型是int，如果是和int不兼容的类型也不行。此外，分数的意义里 4 / 1 = 4，因此第二个分母的参数默认值设置为1，符合数学意义。在设计这种构造函数的时候，应该仔细考虑其参数默认值的合理性。

既然在一个 + 操作符的运算中，我们可以让两边的操作数类型相互转换，那是否可以同时提供转换函数和non-explicit-one-argument构造函数呢？

![image-20200829180316791](https://hunk-pic-store.oss-cn-beijing.aliyuncs.com/img/image-20200829180316791.png)

答案是不可以，在上面的例子中，编译 d2 = f + 4 时，编译器有下面两种选择：

1. 先将 f 转换成 double 类型，然后执行 + 4，最后再将结果转换成一个Fraction对象。
2. 先将 4 转换成一个Fraction对象，然后执行 + 运算。

这两条编译路径都是合理的，因此产生了二义性，导致编译错误。

### explicit-one-argument ctor

对于non-explicit-one-argument构造函数提供的这种自动转换，有好处也有坏处，有时候我们不想这种转换行为自动发生，就可以通过 **explicit**关键字来告诉编译器不做这种转换。

![image-20200829181629426](https://hunk-pic-store.oss-cn-beijing.aliyuncs.com/img/image-20200829181629426.png)

> Effective C++：被声明为`explicit`的构造函数通常比其 non-explicit 兄弟更受欢迎, 因为它们禁止编译器执行非预期 (往往也不被期望) 的类型转换. 除非我有一个好理由允许构造函数被用于隐式类型转换, 否则我会把它声明为`explicit`. 我鼓励你遵循相同的政策.
>
> 关于`explicit`关键字的更多细节可以参考：https://zhuanlan.zhihu.com/p/52152355 和 https://en.cppreference.com/w/cpp/language/explicit

### 仿函数

仿函数也叫做function-like classes，像函数一样的类。既然有类似于函数的行为，就需要重载函数调用操作符()。比如下面这些标准库中的仿函数设计：

![image-20200829185937079](https://hunk-pic-store.oss-cn-beijing.aliyuncs.com/img/image-20200829185937079.png)

![image-20200829190059551](https://hunk-pic-store.oss-cn-beijing.aliyuncs.com/img/image-20200829190059551.png)

### 命名空间

![image-20200824213507486](https://hunk-pic-store.oss-cn-beijing.aliyuncs.com/img/20200825091639.png)

## 2.9 面向对象设计OOP

前面重点介绍的是如何设计单个类，叫做基于对象的编程/设计（Object Based Programing/Design）。本节更多的是探讨类与类之间的关系，叫做面向对象编程/设计（Object Oriented Programing/Design）。

### 面向对象3大特性（封装、继承和多态）

面向对象编程有封装、继承和多态三大特性：

1. 封装：将具体的实现过程和数据封装成一个对象，只能通过结构进行访问，降低耦合性。
2. 继承：子类继承父类的特征和行为，子类有父类的非private 方法和成员变量；子类可以父类的方法进行重写，但是当父类中的成员变量、成员函数或者类本身被 final 关键字修饰时，修饰的类不能继承，修饰的成员不能重写或修改。
3. 多态：用基类指针指向派生类的对象，使得基类指针呈现不同的表示方式。C++多态一般使用虚函数来实现，使用基类指针调用函数方法时：
   1. 如果该指针指向的是一个基类对象，则调用的是基类的虚函数；
   2. 如果该指针指向的是一个派生类对象，则调用的是派生类的虚函数；

#### 单继承和多继承的虚函数表结构

1. 编译器将虚函数表的指针放在类的实例对象的内存空间中，该对象调用该类的虚函数时，通过指针找到虚函数表，根据虚函数表中存放的虚函数的地址找到对应的虚函数。

   ![image-20240423110747698](https://hunk-pic-store.oss-cn-beijing.aliyuncs.com/img/20240423110749.png)

2. 如果派生类没有重新定义基类的虚函数 A，则派生类的虚函数表中保存的是基类的虚函数 A 的地址，也就是说基类和派生类的虚函数 A 的地址是一样的。

   ![image-20240423110846219](https://hunk-pic-store.oss-cn-beijing.aliyuncs.com/img/20240423110847.png)

3. 如果派生类重写了基类的某个虚函数 B，则派生的虚函数表中保存的是重写后的虚函数 B 的地址，也就是说虚函数 B 有两个版本，分别存放在基类和派生类的虚函数表中。例如下图中派生类的 func1 函数：

   ![image-20240423111001209](https://hunk-pic-store.oss-cn-beijing.aliyuncs.com/img/20240423111002.png)

4. 如果派生类重新定义了新的虚函数 C，派生类的虚函数表保存新的虚函数 C 的地址。例如下图，派生类继承于三个基类（则有3个虚表指针 vptr），并重新定义了3个新的虚函数： 

   ![image-20240423111019796](https://hunk-pic-store.oss-cn-beijing.aliyuncs.com/img/20240423111021.png)

> 更多细节参考：[单继承和多继承的虚函数表结构](https://leetcode.cn/leetbook/read/cmian-shi-tu-po/vwbet3/)

#### 多重/菱形继承的常见问题

参考：[C++菱形继承问题和虚继承分析](https://blog.csdn.net/c_base_jin/article/details/86036185)

#### override和final关键字有什么作用？

`override` 关键字告诉编译器，这个函数一定会重写父类的虚函数，如果父类没有这个虚函数，则无法通过编译。此关键字可省略，但不建议省略。

`final` 关键字告诉编译器，这个函数到此为止，如果后续有类继承当前类，也不能再重写此函数。使用 final 关键字修饰的类不能被继承。

```c++
#include <iostream>
using namespace std;
class Base final
{
};

class Derive: public Base{ // error: cannot derive from 'final' base 'Base' in derived type 'Derive'
};

int main() {
    Derive ex;
    return 0;
}
```

#### 重载、重写和隐藏

1. **函数重载（overload）**：是指同一可访问区内被声明几个具有不同参数（类型、个数、顺序）列表的同名函数，根据参数列表确定调用哪个函数，函数重载不关心函数返回类型。

   ```c++
   class A
   {
   public:
       void fun(int tmp);
       void fun(float tmp);        // 重载 参数类型不同（相对于上一个函数）
       void fun(int tmp, float tmp1); // 重载 参数个数不同（相对于上一个函数）
       void fun(float tmp, int tmp1); // 重载 参数顺序不同（相对于上一个函数）
       int fun(int tmp);            // error: 'int A::fun(int)' cannot be overloaded 错误：注意重载不关心函数返回类型
   };
   ```

2. **函数隐藏**：是指派生类的函数屏蔽了与其同名的基类函数，只要是与基类同名的成员函数，不管参数列表相同，基类函数都会被隐藏。

   ```c++
   class Base
   {
   public:
       void fun(int tmp, float tmp1) { cout << "Base::fun(int tmp, float tmp1)" << endl; }
   };
   
   class Derive : public Base
   {
   public:
       void fun(int tmp) { cout << "Derive::fun(int tmp)" << endl; } // 隐藏基类中的同名函数
   };
   
   int main()
   {
       Derive ex;
       ex.fun(1);       // Derive::fun(int tmp)
       ex.fun(1, 0.01); // error: candidate expects 1 argument, 2 provided
       return 0;
   }

3. **函数重写(override)**：是指派生类中存在重新定义的函数。*函数名、参数列表、返回值类型都必须同基类中被重写的函数一致，只有函数体不同*。派生类调用时会调用派生类的重写函数，不会调用基类的被重写函数。当然，基类中可以被重写的函数一定是虚函数。

**重写和重载的区别：**

1. 范围区别：对于类中函数的重载或者重写而言，重载发生在同一个类的内部，重写发生在不同的类之间（子类和父类之间）。
2. 参数区别：重载的函数需要与原函数有相同的函数名、不同的参数列表，不关注函数的返回值类型；重写的函数的函数名、参数列表和返回值类型都需要和原函数相同，父类中被重写的函数需要有 virtual 修饰。
3. virtual 关键字：重写的函数基类中必须有 virtual 关键字的修饰，重载的函数可以有 virtual 关键字的修饰也可以没有。

**隐藏和重写、重载的区别：**

1. 范围区别：隐藏与重载范围不同，隐藏发生在不同类中。
2. 参数区别：隐藏函数和被隐藏函数参数列表可以相同，也可以不同，但函数名一定相同；当参数不同时，无论基类中的函数是否被 virtual 修饰，基类函数都是被隐藏，而不是重写。
3. 利用重写可以实现多态，而隐藏不可以。如果使用基类指针 p 指向派生类对象，利用这个指针调用函数时，对于隐藏的函数，会根据指针的类型去调用函数；对于重写的函数，会根据指针所指对象的类型去调用函数。重写必须使用 virtual 关键字，此时会更改派生类虚函数表的表项。
4. 隐藏是发生在编译时，即在编译时由编译器实现隐藏，而重写一般发生运行时，即运行时会查找类的虚函数表，决定调用函数接口。

### 友元的作用及使用场景

**友元（`friend`）**提供了不同类的成员函数之间、类的成员函数与一般函数之间进行数据共享的机制。即，通过友元，一个普通的函数或另一个类中的成员函数可以访问类中的私有成员和保护成员。

#### 普通函数定义为类的友元函数

**场景1：**普通函数定义为类的友元函数，使得普通函数能够访问该类的私有成员和保护成员：

```c++
#include <iostream>
using namespace std;

class A {
    friend ostream &operator<<(ostream &_cout, const A &tmp); // 声明为类的友元函数

public:
    A(int tmp) : var(tmp) { }

private:
    int var;
};

ostream &operator<<(ostream &_cout, const A &tmp) {
    _cout << tmp.var;
    return _cout;
}

int main() {
    A ex(4);
    cout << ex << endl; // 4
    return 0;
}
```

#### 友元类

**场景2**：友元类。由于类的 private 和 protected 成员变量只能由类的成员函数访问或者派生类访问，友元类则提供提供一种通用的方法，使得不同类之间可以访问其 private 和 protected 成员变量，用于不同类之间共享数据。

```c++
#include <iostream>
using namespace std;

class A {
    friend class B;

public:
    A() : var(10){} // 自定义默认构造函数
    A(int tmp) : var(tmp) {}
    void fun() { cout << "fun():" << var << endl; }

private:
    int var;
};

class B {
public:
    B() {}
    void fun() { cout << "fun():" << ex.var << endl;} // 访问类 A 中的私有成员

private:
    A ex;
};

int main() {
    B ex;
    ex.fun(); // fun():10
    return 0;
}
```

### 复合（Composition）

复合表示的是一种包含或者拥有关系（*has a*），一个类实例同时包含了另一个类的实例。比如下面的queue类和deque类就是这种关系，每个queue包含一个deque。deque是一种两边都可以进出的双端队列，而queue只提供一种先进先出的队列，所以queue对deque的部分操作进行适当的包装即可实现，这也是一种叫做Adapter的设计模式，即 STL 里的容器适配器的原理。

![image-20200824213327781](https://hunk-pic-store.oss-cn-beijing.aliyuncs.com/img/20200825091637.png)

![image-20200824204626420](https://hunk-pic-store.oss-cn-beijing.aliyuncs.com/img/20200825091635.png)

#### 复合关系下的构造和析构

我们把外部类叫做 Container，被包含的类叫做 Component，那么在创建 Container 对象时，是先调用Container构造函数还是先调用Component的构造函数呢？

![image-20200824210637073](https://hunk-pic-store.oss-cn-beijing.aliyuncs.com/img/20200825091636.png)

红色部分的调用由编译器自动添加，析构函数是唯一的，但是构造函数可能有多个，编译器始终调用默认构造函数，如果希望调用其它的构造函数版本，需要我们自己显式调用。

> 对于复合关系中的 Container 和 Component，它们的生命周期是一致的，同时被创建，同时被销毁。

### 委托（Delegation）

委托其实也是一种复合关系，只不过是 Composition by reference，即类通过成员数据里的指针或引用指向另一个类的实例。比如下面字符串类String 的设计里，并不是把底层字符串直接放到 String 对象里，而是用一个 StringRep 对象来专门存放底层字符串，然后用一个指针 rep 指向StringRep 对象。

![image-20200827115543232](https://hunk-pic-store.oss-cn-beijing.aliyuncs.com/img/image-20200827115543232.png)

这也是一种叫做Handle/Body的设计模式，这样设计的好处是拥有相同字符串内容的不同String对象可以共享相同的底层StringRep对象，从而节省内存空间。

1. 在StringRep对象中包含引用计数count，来记录有多少个String对象指向自己，只有当最后一个指向它的String对象被释放时，StringRep对象才会被真正的释放。
2. 如果String对象需要修改字符串内容，就会创建并指向新的StringRep对象，这也叫写时复制（COW，copy on write）。

#### 关于智能指针

对于委托关系，另一个比较好的例子是标准库里智能指针类 `shared_ptr` 的设计：

![image-20200829182245219](https://hunk-pic-store.oss-cn-beijing.aliyuncs.com/img/image-20200829182245219.png)

`shared_ptr` 类包含了一个指向实际数据对象的指针 px，并且让对 `shared_ptr` 对象的指针操作间接作用在px上，这就需要 `shared_ptr` 类重载指针相关的操作符：

1. 解引用操作符 * ：期望返回的是px指向的对象。
2. 访问操作符 ->：在sp ->method()调用里，sp->首先得到的是px，px是一个指针类型，在指针类型上调用方法似乎还需要一个->操作符。实际上，这是C++对于->操作符的一个特殊处理，允许->操作符持续作用在数据上。

### 继承（Inheritance）

继承是一个归属 （*is a* ）的关系，在继承关系中，派生类就是一个基类，因为它继承了基类里的数据和方法，只是拥有更多自己的数据和方法。比如，人也是一种动物。

![image-20200827114532644](https://hunk-pic-store.oss-cn-beijing.aliyuncs.com/img/image-20200827114532644.png)

#### 继承关系下的构造和析构

```c++
class Base { ... };
class Derived : public Base { ... };
```

**构造由内而外**：Derived的构造函数首先调用Base的默认构造函数，然后才执行自己，等价于：

```c++
Derived::Derived(...) : Base() { ... };
```

**析构由外而内**：Derived的析构函数首先执行自己，再调用Base的析构函数，等价于：

```c++
Derived::~Derived(...) { ... ~Base() };
```

> 基类的析构函数必须是虚函数。

#### 继承中的虚函数

1. 非虚函数（non-vritual function）：不希望派生类去重新定义（override，覆写）它。
2. 虚函数（virtual function）：希望派生类重新定义（override，覆写）它，但是基类也会提供默认定义（实现）。
3. 纯虚函数（pure virtual function）：希望派生类一定要重新定义（override，覆写）它，基类中没有提供默认定义（实现）。

```c++
class Shape {
public:
	virtual void draw() const = 0; //纯虚函数
    virtual error(const std::string& msg); //虚函数
    int objectID() const; //非虚函数
}
```

比如下面文档操作类中，CMyDoc继承了CDocument，每种文档类型都应该支持基本的Open操作，在OnOpenFile函数中需要调用Serialize方法，但是CDocument类的Serialize方法就需要设计成虚函数，每种不同类型的文档应该自己提供序列化的操作。

![image-20200827113522256](https://hunk-pic-store.oss-cn-beijing.aliyuncs.com/img/image-20200827113522256.png)

调用myDoc.OnFileOpen的等价于调用CDocument::OnFileOpen(&myDoc)，在执行CDocument::OnFileOpen(&myDoc)时需要调用Serialize()方法，实际上是this->Serialize()方法，而此时this指针指的是myDoc这个对象，所以就调用了CMyDoc里的Serialize()方法。

### 继承 + 复合

当类之间同时包含继承和复合关系时，构造和析构的顺序可以直接通过前面的规则推导出来，比如下面的情况，派生类中复合包含了另一个类：

![image-20200827112829213](https://hunk-pic-store.oss-cn-beijing.aliyuncs.com/img/image-20200827112829213.png)

**构造由内向外**：Derived的构造函数首先调用Base的构造函数，然后调用Component的构造函数，最后才调用Derived自己的构造函数。等价于：

```c++
Derived::Derived(...): Base(), Component() { ... };
```

**析构由外向内**：Derived的析构函数首先执行自己，然后调用Component的析构函数，最后调用Base的析构函数。等价于：

```c++
Derived::~Derived(...) { ... ~Component(), ~Base() };
```

### 委托 + 继承

在下面的例子中就用到了委托+继承的关系，我们有一个保存数据实体的类Subject，但是有多个不同类对象来展示同一个Subject对象里的数据，这些展示类叫做观察者（Observer）。当Subject里的数据更新时，所有观察者都能得到通知，然后及时更新数据的展示情况。

![image-20200827122707499](https://hunk-pic-store.oss-cn-beijing.aliyuncs.com/img/image-20200827122707499.png)

这里体现的就是观察者设计模式。我们在Subject里包含一个指向不同观察者的指针列表m_views，通过attach方法将一个观察者注册到到观察者列表，当Subject对象数据更新时，调用notify方法通知所有的观察者即可。在notify方法里，依次调用所有观察者的update方法，这个方法在观察者基类Observer中是一个纯虚函数，意味着派生类都需要实现这个方法，也就是如何去更新展示数据。



## 2.10 C++ 对象模型

### 类的实例化

#### 实例化类对象的过程

1. **分配空间**：创建类对象首先要为该对象分配内存空间。不同的对象，为其分配空间的时机未必相同：
   1. 全局对象、静态对象、分配在栈区域内的对象，在编译阶段进行内存分配；
   2. 存储在堆空间的对象，是在运行阶段进行内存分配。
2. **初始化**：首先明确一点：初始化不同于赋值。初始化发生在赋值之前，初始化随对象的创建而进行，而赋值是在对象创建好后，为其赋上相应的值。这一点可以联想下上一个问题中提到：初始化列表先于构造函数体内的代码执行，初始化列表执行的是数据成员的初始化过程，这个可以从成员对象的构造函数被调用看的出来。
3. **赋值**：对象初始化完成后，可以对其进行赋值。对于一个类的对象，其成员变量的赋值过程发生在类的构造函数的函数体中。当执行完该函数体，也就意味着类对象的实例化过程完成了。（总结：构造函数实现了对象的初始化和赋值两个过程，对象的初始化是通过初始化列表来完成，而对象的赋值则才是通过构造函数的函数体来实现。）

对于拥有虚函数的类的对象，还需要给虚表指针赋值：

- 没有继承关系的类，分配完内存后，首先给虚表指针赋值，然后再列表初始化以及执行构造函数的函数体，即上述中的初始化和赋值操作。
- 有继承关系的类，分配内存之后，首先进行基类的构造过程，然后给该派生类的虚表指针赋值，最后再列表初始化以及执行构造函数的函数体，即上述中的初始化和赋值操作。

#### 类对象的初始化顺序

构造函数调用顺序：

1. 按照派生类继承基类的顺序，即派生列表中声明的继承顺序，依次调用基类的构造函数；
2. 在有虚继承和一般继承存在的情况下，优先虚继承。比如虚继承：class C: public B, virtual public A，此时应当先调用 A 的构造函数，再调用 B 的构造函数。
3. 按照派生类中成员变量的声明顺序，依次调用派生类中成员变量所属类的构造函数；
4. 执行派生类自身的构造函数。

类对象的初始化顺序：

1. 按照构造函数的调用顺序，调用基类的构造函数
2. 按照成员变量的声明顺序，调用成员变量的构造函数，成员变量的初始化顺序与声明顺序有关；
3. 调用该类自身的构造函数；

析构顺序和类对象的初始化顺序相反。

#### 如何禁止类被实例化？

有3种方案可以实现：

1. 在类中定义一个纯虚函数，使该类成为抽象基类，因为不能创建抽象基类的实例化对象：

   ```c++
   #include <iostream>
   using namespace std;
   class A {
   public:
       int var1, var2;
       A(){
           var1 = 10;
           var2 = 20;
       }
       virtual void fun() = 0; // 纯虚函数
   };
   
   int main(){
       A ex1; // error: cannot declare variable 'ex1' to be of abstract type 'A'
       return 0;
   }
   ```

2. 将类的所有构造函数声明为私有 private 。

3. C++ 11以后，可以将类的所有构造函数用 `=delete` 修饰。

### 类实例的大小

类的大小是指类的实例化对象的大小，用 `sizeof` 对类型名操作时，结果是该类型的对象的大小。计算原则如下：

1. 遵循结构体的成员变量的对齐原则。
2. 只与具体对象私有的普通成员变量有关，与静态成员和成员函数无关。即普通成员函数、静态成员函数、静态数据成员、静态常量数据成员均对类的大小无影响，因为静态数据成员被类的对象共享，存储在全局数据区，并不属于具体某个对象。
3. 虚函数对类的大小有影响，是因为虚函数表指针的影响。
4. 虚继承对类的大小有影响，是因为虚基表指针带来的影响。

下面看一些代码案例：

```c++
// 说明：程序是在 64 位编译器下测试的
#include <iostream>
using namespace std;
class A
{
private:
    static int s_var; // 不影响类的大小
    const int c_var;  // 4 字节
    int var;          // 8 字节 4 + 4 (int) = 8
    char var1;        // 12 字节 8 + 1 (char) + 3 (填充) = 12
public:
    A(int temp) : c_var(temp) {} // 不影响类的大小
    ~A() {}                    // 不影响类的大小
};

class B
{
};
int main()
{
    A ex1(4);
    B ex2;
    cout << sizeof(ex1) << endl; // 12 字节
    cout << sizeof(ex2) << endl; // 空类大小：1 字节
    return 0;
}
```

#### 带有虚函数的情况

虚函数的个数并不影响所占内存的大小，因为类对象的内存中只保存了指向虚函数表的指针。由于不同平台、不同编译器厂商所生成的虚表指针在内存中的布局是不同的，有些将虚表指针置于对象内存中的开头处，有些则置于结尾处。在 X64 GCC 编译器下，虚指针在类的开头出，我们可以通过偏移量获取。

如下程序示例，我们通过对象内存的开头处取出 `vptr`，并遍历对象虚函数表：

```c++
// 说明：程序是在 64 位编译器下测试的
#include <iostream>
using namespace std;
class A
{
private:
    static int s_var; // 不影响类的大小
    const int c_var;  // 4 字节
    int var;          // 8 字节 4 + 4 (int) = 8
    char var1;        // 12 字节 8 + 1 (char) + 3 (填充) = 12
public:
    A(int temp) : c_var(temp) {} // 不影响类的大小
    ~A() {}                      // 不影响类的大小
    virtual void f() { cout << "A::f" << endl; }
    virtual void g() { cout << "A::g" << endl; }
    virtual void h() { cout << "A::h" << endl; } // 24 字节 12 + 4 (填充) + 8 (指向虚函数的指针) = 24
};

typedef void (*func)(void);

void printVtable(unsigned long *vptr, int offset) {
     func fn = (func)*((unsigned long*)(*vptr) + offset);
     fn();    
}

int main()
{
    A ex1(4);
    A *p;
    cout << sizeof(p) << endl;   // 8 字节 注意：指针所占的空间和指针指向的数据类型无关
    cout << sizeof(ex1) << endl; // 24 字节
    unsigned long* vPtr = (unsigned long*)(&ex1);
    printVtable(vPtr, 0); // A::f
    printVtable(vPtr, 1); // A::g
    printVtable(vPtr, 2); // A::h
    return 0;
}
```

#### 空类字节数以及缺省成员函数

**空类声明时编译器不会生成任何成员函数**：对于空类，声明编译器不会生成任何的成员函数，只会生成 1 个字节的占位符。由于在实际程序中，空类同样可以被实例化，而每个实例在内存中都有一个独一无二的地址，为了达到这个目的，编译器往往会给一个空类隐含的加一个字节，这样空类在实例化后在内存得到了独一无二的地址，所以 `sizeof(A)` 的大小为 1。

**空类定义时编译器会生成6个成员函数：**当空类 A 定义对象时，sizeof(A) 仍是为 1，但编译器会在需要时生成 6 个缺省成员函数：缺省的构造函数、拷贝构造函数、析构函数、赋值运算符、两个取址运算符。

### 虚函数和纯虚函数

在前面加上 `virtual` 关键字修饰的类成员函数都是虚函数，这其中又分为两种：

1. **虚函数**：基类中的成员函数虽然有 `virtual` 关键字修饰，但是也有具体的函数实现，表示派生类可以重写这个虚函数，也可以选择不重写，直接使用基类的默认实现版本。
   1. **构造函数**不能定义为虚函数。
   2. 一般将基类的**析构函数**定义成虚函数，可以保证派生类被正确地释放。
   3. `static` 函数不能定义为虚函数。
2. **纯虚函数**：基类中的成员函数有 `virtual` 关键字修饰，只有接口定义，但是没有具体实现，并且末尾加上 `= 0`。派生类必须实现这个纯虚函数。
   1. 含有纯虚函数的类叫做**抽象类**。*抽象类对象不能实例化对象，所以不能作为函数的参数，不能作为函数返回类型。可以声明抽象类的指针和引用（不然多态咋玩？）*
   2. 继承抽象类的派生类，如果没有完全实现基类的纯虚函数，依然是抽象类，不能实例化对象。

>  关于虚函数的具体行为和约束，建议直接参考：[virtual函数说明符](https://zh.cppreference.com/w/cpp/language/virtual)

#### 非虚构造函数

构造函数是在实例化对象的时候进行调用，如果此时将构造函数定义成虚函数，需要通过访问该对象所在的内存空间才能进行虚函数的调用（因为需要通过指向虚表指针 `vptr` 调用虚函数表 `vtbl`，虽然虚函数表在编译时就有了，但是没有虚表指针，虚表指针 `vptr` 只有在创建了对象才有），但是此时该对象还未创建，便无法进行虚函数的调用。所以构造函数不能定义成虚函数。

#### 虚析构函数

*实际应用时，当基类的指针或者引用指向或绑定到派生类的对象时，如果没有将基类的析构函数定义成虚函数，当我们对基类指针执行 `delete` 操作时，此时只会调用基类的析构函数，将基类的成员所占的空间释放掉，而派生类中特有的资源就会无法释放而导致内存泄漏*。参考案例：https://zhuanlan.zhihu.com/p/148290103

虽然，析构函数是不继承的，但若基类声明其析构函数为 `virtual`，则派生的析构函数始终覆盖它。*这使得可以通过指向基类的指针 `delete` 动态分配的多态类型对象。*

```c++
class Base {
 public:
    virtual ~Base() { /* 释放 Base 的资源 */ }
};
 
class Derived : public Base {
    ~Derived() { /* 释放 Derived 的资源 */ }
};
 
int main()
{
    Base* b = new Derived;
    delete b; // 进行对 Base::~Base() 的虚函数调用
              // 由于它是虚函数，故它调用的是 Derived::~Derived()，
              // 这就能释放派生类的资源，然后遵循通常的析构顺序
              // 调用 Base::~Base()
}
```

此外，若类是*多态的*（声明或继承了至少一个虚函数），且其析构函数非虚，则删除它是*未定义行为*，无论不调用派生的析构函数时是否会导致资源泄漏。*一条有用的方针是，任何基类的析构函数必须为[公开且虚，或受保护且非虚](https://github.com/isocpp/CppCoreGuidelines/blob/master/CppCoreGuidelines.md#discussion-make-base-class-destructors-public-and-virtual-or-protected-and-nonvirtual)*。

### 虚函数表和虚表指针

`虚函数表`（vtbl，也叫虚表）和`虚表指针`（vptr）是C++实现多态的核心设计。

虚函数表是一个函数指针的查找表，用于在运行时动态地将虚函数绑定到对象，即动态绑定行为。它不打算由程序直接使用，因此没有访问它的标准化方法。*每个定义了虚函数的类（包括继承虚基类的类）都有自己的虚函数表，这个表由编译器在编译时添加*。

在实例化派生类对象时，先实例化基类，将基类的虚表入口地址赋值给基类的虚表指针，当基类构造函数执行完时，再将派生类的虚表入口地址赋值给基类的虚表指针（派生类和基类此时共享一个虚表指针，并没有各自都生成一个），再执行基类的构造函数。

>  对于纯虚函数，虚函数表里存储的是NULL指针。

虚表指针（vptr）是编译器为虚基类添加的一个隐藏指针，这个指针指向特定类的虚函数表。所有派生类都会继承这个虚表指针，因此每个带虚函数的类对象都会存储这个指针。通过类对象调用虚函数都会根据这个虚表指针来解析具体调用的函数。

虚函数表中有序放置了父类和子类中的所有虚函数，并且相同虚函数在类继承链中的每一个虚函数表中的偏移量都是一致的。所以确定的虚函数对应虚表中一个固定位置 `n`，`n` 是一个在编译时期就确定的常量，所以，使用 `vptr` 加上对应的 `n`，就可以得到对应的函数入口地址。C++采用的这种 绝对地址+偏移量 的方法调用虚函数，查找速度快执行效率高，时间复杂度为O(1)，这里概括一下虚函数的寻址过程：

1. 获取类型名和函数名。
2. 从符号表中获得当前虚函数的偏移量。
3. 利用偏移量得到虚函数的访问地址，并调用虚函数。

如下图所示，某个特定类的所有对象共享该类的同一个虚表，但是每个类对象都会存储自己的虚表指针。

![See the source image](https://hunk-pic-store.oss-cn-beijing.aliyuncs.com/img/48e48915cadcd)

来看一段代码：

```c++
#include<iostream.h>

class Base  
 {  
 public:  
    virtual void function1() {cout<<"Base :: function1()\n";};  
    virtual void function2() {cout<<"Base :: function2()\n";};  
    virtual ~Base(){};
};  
   
class D1: public Base  
{  
public:  
   ~D1(){};
   virtual void function1() { cout<<"D1 :: function1()\n";};
};  
  
class D2: public Base  
{  
public:  
   ~D2(){};
   virtual void function2() { cout<< "D2 :: function2\n";};  
};  
```

Base，D1，D2这几个类的虚表结构如下：

![虚表指针1](https://hunk-pic-store.oss-cn-beijing.aliyuncs.com/img/VirtualTable.JPG)

简单测试这些类：

```c++
int main()
{
  D1 *d = new D1;;
  Base *b = d; 

  b->function1();
  b->function2();

  delete (b);
  
  return (0);
}

/* 输出
D1 :: function1()
Base :: function2()
*/
```

在main函数里，指针b被分配给D1类的虚表指针，现在开始指向D1的虚函数表。然后调用function1()，使它的虚表指针直接调用虚函数表的function1()，这样反过来调用D1的方法，即function1()，因为D1这个类有它自己的function1()定义。

当指针b调用function2()时，它的虚表指针指向D1的虚函数表，而后者又指向基类Base的虚函数表里的function2()，因为D1类没有自己的function2())定义。

最后调用指针删除b，它的虚表指针是指向D1的虚函数表，调用它自己的类的析构函数即D1类的析构函数,然后调用基类的析构函数。当派生类对象被删除时，它会继续删除嵌入的基类对象，所以会调用基类的析构函数。这就是为什么我们必须总是使基类的析构函数为虚函数。

简而言之，动态绑定发生在调用不同派生类对象的虚函数时。最后，再贴一张侯捷老师课件的图片：

![image-20200910084600384](https://hunk-pic-store.oss-cn-beijing.aliyuncs.com/img/image-20200910084600384.png)

### 多态/动态绑定

多态就是不同继承类的对象，对同一消息做出不同的响应，基类的指针指向或绑定到派生类的对象，使得基类指针呈现不同的表现方式。在基类的函数前加上 virtual 关键字，在派生类中重写该函数，运行时将会根据对象的实际类型来调用相应的函数。在调用函数的时候，会通过虚指针转到虚表，并根据虚函数的偏移得到真实函数地址，从而实现多态：

1. 如果对象类型是派生类，就调用派生类的函数；
2. 如果对象类型是基类，就调用基类的函数。

多态是通过虚函数实现的，存在虚函数的类都有一个虚函数表 `vtbl`，当创建一个该类的对象时，该对象有一个指向类虚函数表的虚表指针 `vptr`（虚函数表和类对应的，如果派生类继承自多个基类，则会同时存在多个虚表指针分别指向不同基类的虚函数表，虚表指针是和对象对应）；虚表指针 `vptr`保存在含有虚函数的类的实例对象的内存空间中。

程序的编译过程中就会将虚函数的地址放在虚函数表中，即虚函数表 `vtbl` 是编译时确定的，但是虚表指针 `vptr` 是在创建对象之后才有的。虚表指针 `vptr`存放在对象的内存空间中最前面的位置，这是为了保证正确取到虚函数的偏移量。

当基类指针指向派生类对象，基类指针调用虚函数时，该基类指针指的虚表指针实际指向派生类虚函数表，通过遍历虚表，寻找相应的虚函数然后调用执行。

动态绑定是C++多态现象的底层行为，本质是一回事，都是建立在虚函数表和虚表指针的基础上。

![image-20200910090951805](https://hunk-pic-store.oss-cn-beijing.aliyuncs.com/img/image-20200910090951805.png)

上图里的 *a* 只是一个普通的变量，不是指针或者引用类型，所以通过 *a* 调用 *vfunc1* 时还是调用的A这个类定义的 *vfunc1*而不是 B 这个类定义的*vfunc1*，属于`静态绑定`。

![image-20200910091021449](https://hunk-pic-store.oss-cn-beijing.aliyuncs.com/img/image-20200910091021449.png)

这里的 *pa* 是一个虚基类的指针，但是指向了一个派生类对象，通过 *pa* 调用 *vfunc1* 时实际上调用了派生类的 *vfunc1*，这就是 `动态绑定`，动态绑定发生在运行时。

可以总结多态或者动态绑定的3个条件：

1. 要有继承，即一定是基类和派生类之间的行为。
2. 要有虚函数重写，即派生类要重新定义基类中的虚函数。
3. 向上转型，即用基类指针或引用去指向派生类对象，用普通变量不行，反过来不行。

#### 编译时多态和运行时多态的区别

**编译时多态**：在程序编译过程中出现，发生在**模板和函数重载**中（泛型编程）。实际在编译器内部看来不管是重载还是模板，编译器内部都会生成不同的函数，在代码段中分别装有两个函数的不同实现。

**运行时多态**：运行时多态也称动态绑定，在程序运行过程中出现，发生在继承体系中，是指通过基类的指针或引用访问派生类中的虚函数。

编译时多态和运行时多态的区别：

1. 时期不同：编译时多态发生在程序编译过程中，运行时多态发生在程序的运行过程中；
2. 实现方式不同：编译时多态运用泛型编程来实现，运行时多态借助虚函数表来实现。

### 参考资料

[C++对象模型](http://www.360doc.com/content/15/0909/19/7377734_498070757.shtml)

[How Virtual Table and _vptr works](https://www.go4expert.com/articles/virtual-table-vptr-t16544/)

[Virtual Functions and Runtime Polymorphism in C++ | Set 1 (Introduction)](https://www.geeksforgeeks.org/virtual-functions-and-runtime-polymorphism-in-c-set-1-introduction/)

[C++多态中的虚函数表](https://blog.csdn.net/code_peak/article/details/118883247)

[C++虚函数表深入探索(详细全面)](https://cloud.tencent.com/developer/article/1599283)

# 三、C++ 模板与泛型编程

## 3.1 模板基本概念

C++泛型编程的基础是模板。模板是 C++ 编程语言的一个特性，它允许函数和类使用泛型类型进行操作。这允许一个函数或类在许多不同的数据类型上工作，而无需为每个类型重写。所以，模板是创建泛型类或函数的蓝图或公式。STL容器、迭代器和算法，都是泛型编程的例子，它们都使用了模板的概念。

模板定义以关键字 `template` 开始，后跟一个模板参数列表（模板参数列表不能为空）。模板类型参数前必须使用关键字 `class` 或者 `typename`，在模板参数列表中这两个关键字含义相同，可相互替换。

C++共支持三种模板：

1. **函数模板**：函数模板的行为类似于函数，只是模板可以有许多不同类型的参数。一个函数模板代表一个函数族。*当调用一个模板函数时，编译器用函数实参来推断模板实参，从而使用实参的类型来确定绑定到模板参数的类型。*
2. **类模板**：类模板提供了基于参数生成类的规范。类模板通常用于实现容器。类模板通过将一组给定的类型作为模板参数传递给它来实例化。C++ 标准库包含许多类模板，特别是改编自标准模板库的容器，例如 `vector`，`list`。*但是，编译器不能为类模板推断模板参数类型，需要在使用该类模板时，在模板名后面的尖括号中指明类型。*
3. **变量模板**（C++ 14）：即变量也可以参数化为特定的类型。

### 函数重载与模板的区别

函数重载 和 模板 都是面向对象多态特性的例子。当多个函数执行非常相似（不相同）的操作时使用函数重载，当多个函数针对不同类型数据执行相同操作时使用模板，函数模板也可以重载。注意：当模板类或者模板函数中含有静态变量时，则每个模板的实例类型都含有一个静态成员。

```c++
template <class T>
class A { 
  public: 
    static T val; 
}; 
A<int> a; // 含有静态成员 int val;
A<string> b; // 含有静态成员 string val;
```

### 函数模板和类模板的区别

1. 实例化方式不同：函数模板实例化由编译器在处理函数调用时自动完成，类模板实例化需要在程序中显式指定。
2. 实例化的结果不同：函数模板实例化后是一个函数，类模板实例化后是一个类。
3. 默认参数：函数模板不允许有默认参数，类模板在模板参数列表中可以有默认参数。
4. 特化：函数模板只能全特化；类模板可以全特化，也可以偏特化。
5. 调用方式不同：函数模板可以进行类型推导，可以隐式调用，也可以显式调用；类模板只能显式调用。

参考：[函数模板与类模板](https://zhuanlan.zhihu.com/p/381299879)

### 泛型编程的优缺点

1. 通用性强：泛型算法时建立在语法一致性上，运用到的类型集是无限的。
2. 效率高：编译器能确定静态类型信息，其效率与针对特定数据类型的而设计的算法相同，即没有性能损耗。
3. 类型检查严格：静态类型信息被完整的保存在了编译器，在编译时可以发现更多潜在的错误。
4. 二进制复用性差：泛型算法是建立在语法一致性上，语法是代码层面的，语法上的约定无法体现在机器指令中。泛型算法实现的库，其源代码基本是必须公开的，引用泛型库都需要重新编译生成新的机器指令。传统的C库全是以二进制目标文件形式发布的，需要使用这些库时直接动态链接加载使用即可，不需要进行再次编译。

## 3.2 模板定义

### 函数模板

很多函数操作针对不同类型的处理是一样的，那么如何将函数抽象成一个通用版本呢？比如获取两个数据中较小值的min函数，对于基础数据类型，可以利用重载机制实现相应的min函数版本。但是对于复杂的自定义类类型，我们就没法通过重载min函数来做到了。类似于类模板，可以使用**函数模板**来编写min函数，将操作的数据类型抽象为T，min函数内部只包含最基础的比较操作。基本语法如下：

![image-20200824213345709](https://hunk-pic-store.oss-cn-beijing.aliyuncs.com/img/20200825091638.png)

编译器会自动对函数模板做实参类型推导，比如上图中编译 r3 = min(r1, r2)这条语句时，编译器会推导出T就是stone类型，就会去调用stone::operator<，这也就要求stone类重载了<操作符，否则就会编译报错。

这样设计的好处就是只需要提供一个最精简的min函数，作为参数传递的类重载比较操作符即可。实际上，也只有设计类的人最清楚应该怎么去定义比较操作。

### 类模板

类模板是用来生产类的蓝图，它允许我们设计更为通用的类，比如队列这种数据结构，实现一个双端队列类 `queue`，就可以用来装载不同类型的数据。在类的声明和定义中使用抽象化的数据类型 `T`，在实例化时用 `<>` 指定具体类型即可。对于前面介绍的复数类，它的实部和虚部也不一定是 double 类型，也可以是 float 或者 int 类型，采用类模板会让这个类更通用，使用者可以轻松实例化不同版本的复数类型。

![image-20200824204549290](https://hunk-pic-store.oss-cn-beijing.aliyuncs.com/img/20200825091634.png)

与函数模板的不同之处在于，编译器不能为类模板推断模板参数类型，使用类模板时，我们必须在模板名后的尖括号里提供显式模板实参列表，它们被绑定到模板参数，编译器使用这些模板实参来实例化出特定的类。

类模板Blob：

```c++
template <typename T> class Blob {
public:
    typedef T value_type;
    typedef typename vector<T>::size_type size_type;

    //构造函数
    Blob() : data(make_shared<vector<value_type>>()) { }
    Blob(initializer_list<T> il) : data(make_shared<vector<value_type>>(il)) { }

    size_type size() { return data->size(); }
    bool empty() { return data->empty(); }

    //添加和删除元素
    void push_back(const value_type &t) { data->push_back(t); }
    void push_back(value_type &&t) { data->push_back(std::move(t)); }
    void pop_back();
    
    //元素访问
    T& front();
    T& back();
    T& operator[](size_type i);

private:
    shared_ptr<vector<T>> data;
    //如果data[i]不合法，抛出一个异常
    void check(size_type i, const string &msg) const;
};
```

我们可以在类模板内部或者外部为其定义成员函数，并且定义在类模板内部的成员函数被隐式声明为内联函数，比如上面的`push_back`函数。

类模板的每个实例都有其自己版本的成员函数，因此类模板的成员函数具有和类模板相同的模板参数，所以定义在类模板外部的成员函数必须以`template`开始，后接类模板参数列表，比如下面的`check`等函数。

```c++
template <typename T>
void Blob<T>::check(size_type i, const string &msg ) const {
    if ( i >= data->size() ) {
        throw out_of_range(msg);
    }
}

template <typename T>
void Blob<T>::pop_back()
{
    check(0, "pop_back on empty Blob");
    data->pop_back();
}

template <typename T>
T& Blob<T>::front() {
    check(0, "front on empty Blob");
    return data->front();
}

template <typename T>
T& Blob<T>::back() {
    check(0, "back on empty Blob");
    return data->back();
}

template <typename T>
T& Blob<T>::operator[](size_type i) {
    check(i, "index out of range.");
    return *data[i];
}
```

两条重要原则：

1. 当我们在类模板外部定义其成员函数时，我们并不在类的作用域中，直到遇到类名才表示进入类的作用域。
2. 在类模板的的作用域内时，我们可以直接使用模板名而不必指定模板实参。

下面使用类模板Blob：

```c++
Blob<string> b1 = { "hunk", "jack" };
```

当编译器从Blob模板实例化出一个类时，它会重写Blob模板，将模板参数T的每个实例替换成给定的模板实参，这里是string。对于指定的每种模板实参，编译器都生成不同的并且独立的类，Blob<string>与任何其他Blob类型都没有关联，也不会对任何其他Blob类型的成员有特殊访问权限。

> 类模板的名字并不是一个类型名。
>
> 默认情况下，一个类模板的成员函数只有当程序用到它时才进行实例化。比如上面实例化的Blob<string>，仅仅用到了其中一个序列构造函数，其余的成员函数没有被使用，也就不会被实例化。这个特性使得即使某种类型不能完全符合模板操作的要求，我们仍然可以使用该类型实例化类，该类型只要满足需要使用的模板操作的要求即可。

**类模板和友元**

当一个类包含一个友元声明时，类与友元各自是否是模板是相互无关的。如果一个类模板包含一个非模板友元，则友元被授权可以访问类模板的所有实例。如果友元自身是模板，类可以授权给所有友元模板实例，也可以只授权给特定实例。

类模板与另一个（类或函数）模板间友好关系的最常见形式是建立对应实例及其友元间的友好关系，这样友好关系就被限定在用相同类型实例化的Blob与BlobPtr，以及相等运算符之间，比如：

```c++
//前置声明，在Blob中声明友元所有需要的
template <typename> class BlobPtr;
template <typename> class Blob; //运算符==中的参数所需要
template <typename T>
	bool operator==(const Blob<T>&, const Blob<T>&);

template <typename T> class Blob {
    friend class BlobPtr<T>;
    friend bool operator==<T>
        (const Blob<T>&, const Blob<T>&);
    //....
}
```

#### 类成员模板

成员模板可以在类模板中嵌套泛型定义，经常用于模板类的构造函数设计中，使类模板的构造更有弹性。来看下面的例子：

```c++
class Base1 {
public:
    virtual string getName() { return "Base1"; };
};
class Derived1 : public Base1 {
public:
    virtual string getName() { return "Derived1"; }
};

class Base2 {
public:
    virtual string getName() { return "Base2"; }
};
class Derived2 : public Base2 {
public:
    virtual string getName() { return "Derived2"; }
};

template<class T1, class T2>
class Pair
{
public:
    T1 first;
    T2 second;

    Pair() : first(T1()), second(T2()) {}
    Pair(const T1& a, const T2& b) : first(a), second(b) { }
};
```

```c++
{
    Pair<Derived1, Derived2> p;
    Pair<Base1, Base2> p2(p);
} 
```

对于上面的调用代码，将会遇到如下编译错误，编译器无法找到合适的构造函数，在编译 Pair<Base1, Base2> p2(p) 时，对于 p2 编译器推导的 T1 和 T2 类型分别是 Base1 和 Base2，但是参数里传递的是 Derived1 和 Derived2。

```bash
/home/workspace/Notes/cpp/std/src/std/Template.cpp:14:28: error: no matching function for call to ‘Pair<Base1, Base2>::Pair(Pair<Derived1, Derived2>&)’
     Pair<Base1, Base2> p2(p);
```

成员模板可以很好的处理这种类模板构造问题：

```c++
template<class T1, class T2>
class Pair
{
public:
    T1 first;
    T2 second;

    Pair() : first(T1()), second(T2()) {}
    Pair(const T1& a, const T2& b) : first(a), second(b) { }

    //成员模板
    template<class U1, class U2>
    Pair(const Pair<U1, U2>& p) : first(p.first), second(p.second) { }
};
```

### 模板参数

**使用类的类型成员**

我们用作用域运算符`::`来访问static成员和类型成员：

1. 在非模板代码中，编译器掌握类的定义。因此，它知道通过作用域运算符访问的名字是类型还是static成员。比如：string::size_type，编译器有string的定义，因此知道size_type是一个类型。

2. 在模板代码中，当编译器遇到类似T::mem时，它不知道mem是一个类型成员还是static数据成员，直至实例化时才知道。但是，为了处理模板，编译器必须知道名字是否表示一个类型。比如：

   ```c++
   T::size_type * p;
   ```

   编译器需要知道我们是在定义一个名为p的变量，还是将一个名为size_type的 static 数据成员与名为p的变量相乘。

   默认情况下，C++语言假定通过作用域运算符访问的是名字不是类型。因此，为了使用一个模板类型参数的类型成员，必须通过`typename`关键字显式告诉编译器该名字是一个类型（这里不能和`class`混用）：

   ```c++
   template <typename T>
   typename T::value_type top(const T& c) {
       if (!c.empty())
           return c.back();
       else
           return typename T::value_type();//没有元素时生成一个值初始化的元素返回给调用者
   }
   ```

**默认模板实参**

和普通函数一样，我们也可以提供默认模板实参：

```c++
template <typename T, typename F = less<T>>
int compare(const T &v1, const T &v2, F f = F()) {
    if(f(v1, v2)) return -1;
    if(f(v2, v1)) return 1;
    return 0;
}
```

我们为模板添加了第二个类型参数F，F表示可调用对象的类型，并定义了一个新的函数参数f，绑定到一个可调用对象上，该可调用对象的返回类型必须能转换为bool值。

默认模板实参指出compare将使用标准库的less函数对象类，它是使用与compare一样的类型参数实例化的。默认函数实参指出f将是类型F的一个默认初始化的对象。

**默认模板实参与类模板**

无论何时使用一个类模板，我们都必须在模板名之后接上尖括号：尖括号指出类必须从一个模板实例化而来。特别地，如果一个类模板为其所有模板参数都提供了默认实参，且我们希望使用这些默认实参，就必须在模板名后跟一个空尖括号对：

```c++
template <class T= int> class Numbers {
public:
    Numbers(T v = 0): val(v) { }
    //...
private:
    T val;
}
Numbers<long double> lots_of_precision;
Numbers<> average_precision; //空<>表示我们希望使用默认参数类型int
```

### 成员模板

无论是普通类还是类模板，都可以包含自身是模板的成员函数，这种成员被称为成员模板。成员模板不能是虚函数。

对于类模板，类和成员模板各自有自己的，独立的模板参数，比如：

```c++
template <typename T> class Blob {
    template <typename It> Blob(It b, It e);
    //...
}
```

我们为Blob类定义一个构造函数，它接受两个迭代器，表示要拷贝元素的范围。由于我们希望支持不同类型序列的迭代器，因此将该构造函数定义为模板函数。

当我们在类模板外定义一个成员模板时，必须按照顺序为类模板和成员模板提供模板参数列表：

```c++
template <typename T>
template <typename It>
	Blob<T>::Blob(It b, It e):
		data(std::make_shared<std::vector<T>>(b, e)) { }
```

为了实例化一个类模板的成员模板，我们必须同时提供类和函数模板的实参：

```c++
int ia[] = {0, 1, 2};
vector<long> vi = {0, 1, 2};
list<const char*> w = {"hunk", "jack", "biob"};
Blob<int> a1(begin(ia), end(ia));
//实例化Blob<long>及其接受两个vector<long>::iterator参数的构造函数
Blob<long> a2(vi.begin(), vi.end());
//实例化Blob<string>及其接受两个list<const char*>::iterator参数的构造函数
Blob<string> a3(w.begin(), w.end());
```

### 控制实例化

当模板被使用时才会进行实例化这一特性意味着，相同的实例可能出现在多个对象文件中。当两个或多个独立编译的源文件使用了相同的模板，并提供了相同的模板参数时，每个文件中就都会有该模板的一个实例，这可能会带来大量的额外开销。

在新标准中，我们可以通过`extern`关键字声明显式实例化，以此来避免这种开销：

```c++
extern template class Blob<string>; //实例化声明
extern template int compare(const int&, const int&);  //实例化声明
```

当编译器遇到`extern`模板声明时，它不会在本文件中生成实例化代码。`extern`声明表示承诺在程序的其它位置有该实例化的一个非extern声明。对于一个给定的实例化版本，可以有多个extern声明，但必须只有一个定义：

```c++
template class Blob<string>; //实例化定义
template int compare(const int&, const int&);  //实例化定义
```

当编译器遇到一个实例化定义时，它为其生成代码。当然，最后我们必须将这些源文件编译后的对象文件链接到一起。

> 一个类模板的实例化定义会实例化该模板的所有成员，包括内联的成员函数。因为，当编译器遇到一个实例化定义时，它不了解程序使用哪些成员函数。因此，在一个类模板的实例化定义中，所有类型必须能用于模板的所有成员函数。

## 3.3 模板特化和偏特化

### 模板特化

模板特化就是针对泛型 `T` 的某些具体类型提供特定的实现版本。

**为什么需要模板特化？**模板并非对任何模板实参都合适、都能实例化，某些情况下，通用模板的定义对特定类型不合适，可能会编译失败，或者得不到正确的结果。因此，当不希望使用通用模板版本时，可以定义类或者函数模板的一个特例化版本。

根据模板参数特化的范围，模板特化分为 *全特化* 和 *偏特化*：

1. 全特化：将模板中的全部模板参数进行特例化。*函数模板只能全特化；类模板可以全特化，也可以偏特化。*
2. 偏特化：将模板中的部分参数进行特例化，剩余部分需要在编译器编译时确定。

> 要区分下函数重载与函数模板特化。定义函数模板的特化版本，本质上是接管了编译器的工作，为原函数模板定义了一个特殊实例，而不是函数重载，函数模板特化并不影响函数匹配。

程序实例如下：

```c++
#include <iostream>
#include <cstring>

using namespace std;
//函数模板
template <class T>
bool compare(T t1, T t2) {
    cout << "通用版本：";
    return t1 == t2;
}

template <> //函数模板特化
bool compare(char *t1, char *t2) {
    cout << "特化版本：";
    return strcmp(t1, t2) == 0;
}

int main(int argc, char *argv[]) {
    char arr1[] = "hello";
    char arr2[] = "abc";
    cout << compare(123, 123) << endl;
    cout << compare(arr1, arr2) << endl;

    return 0;
}
/* 运行结果：
通用版本：1
特化版本：0 */
```



### 模板偏特化 --范围的偏

```c++
template <typename T>
class C
{
    ...
};

//模板偏特化版本
template <typename T>
class C<T*>
{
    ...
};
//或者用不同的泛型名称也可以
template <typename U>
class C<U*>
{
    ...
};
```

当提供了指针类型的偏特化版本后，如果在泛型实例化时使用的是指针，那么编译器就会使用相应的偏特化版本。

```c++
C<string> obj1; //使用第一个普通版本
C<string*> obj2;//使用第二个偏特化版本
```

### 模板偏特化 --个数的偏

```c++
template<typename T, typename Alloc=...>
class vector
{
    ...
};

//模板偏特化版本，如果没有提供任何类型，将会调用这个偏特化版本
template<typename Alloc=...>
class vector<bool, Alloc>
{
    ...
};
```

### 模板模板参数

```c++
template<typename T, 
		template <typename T> 
			class SmartPtr
		>
class XCls
{
private:
    SmartPtr<T> sp;
    
public:
    XCls() : sp(new T) {}
}

XCls<string, shared_ptr> p1;
```

## 3.4 可变参数模板（C++11）

C++中的模板也可以支持可变参数。可变参数模板，即接受可变数目参数的模板函数或模板类。将可变数目的参数被称为*参数包*，用省略号 `...` 来指出一个模板参数或函数参数表示一个包。

在模板参数列表中，`class...` 或 `typename...` 指出接下来的参数表示零个或多个类型的列表就是一个所谓的包（pack）：

1. 用于模板参数，就是模板参数包（template parameters pack）。
2. 用于函数参数类型，就是函数参数类型包（function parameter types pack）。
3. 用于函数参数，就是函数参数包（function parameter pack）。

一个类型名后面跟一个省略号表示零个或多个给定类型的非类型参数的列表。当需要知道包中有多少元素时，可以使用 `sizeof...` 运算符。程序示例如下：

```c++
//.hpp
void myPrint();

//数量不定的模板参数
template<typename T, typename... Types>
void myPrint(const T& firstArg, const Types&... args) {
    cout << firstArg <<", size of args:" << sizeof...(args) << endl;
    myPrint(args...);
}
```

```c++
//.cpp
//如果把这个实现直接定义在hpp文件中，链接时会遇到重复定义的问题
void myPrint() { cout << "END..." << endl; }
```

测试：

```c
{
    myPrint("Hunk", 100, 98.5);
}
/*输出
Hunk, size of args:2
100, size of args:1
98.5, size of args:0
END...
*/
```

在上面的例子中，myPrint递归调用到最后，98.5作为firstArg时，后面的参数包args已经为空，等价于调用myPrint()，因此需要额外提供一个没有参数的版本，否则编译器会检测到这种错误。

> 可变参数函数通常是递归的，第二个版本的 myPrint 负责终止递归并打印初始调用中的最后一个实参。第一个版本的 print_fun 是可变参数版本，打印绑定到 Types 的实参，并用来调用自身来打印函数参数包中的剩余值。

## 3.5 类型萃取（Type Traits）

类型萃取（type traits）是使用模板技术来萃取类型（包含自定义类型和内置类型）的某些特性，用以判断该类型是否含有某些特性，从而在泛型算法中来对该类型进行特殊的处理用来提高效率或者得到其他优化。简单的来说类型萃取即确定变量去除引用修饰以后的真正的变量类型或者 CV 属性。C++ 关于 type traits 的详细使用技巧可以参考头文件 #include 。

**为什么需要类型萃取？**对于普通的变量来说，确定变量的类型比较容易，比如 int a = 10; 可以很容易确定变量的实际类型为 int，但在使用模板时确定变量的类型就比较困难，模板传入的类型为不确定性。为什么需要确定变量的实际类型？因为模板函数针对传入的对不同的类型可能作出不同的处理，这就需要我们在处理函数模板对传入的参数类型和特性进行提取。比如自定义拷贝函数 copy(T *dest, const T *src) ，如果 T 此时为 int 类型，则此时我们只需要 *dest = *src 即可，但是如果我们此时传入的 T 为 char * 字符串类型时，则就不能简单进行指针赋值，所以函数在实际处理时则需要对传入的类型进行甄别，从而针对不同的类型给予不同的处理，这样才能使得函数具有通用性。

C++ 类型萃取一般用于模板中，当我们定义一个模板函数后，需要知道模板类型形参并加以运用时就奥数可以用类型萃取。通过确定变量的特征我们可以在模板中使用不同的处理方法。示例代码：

```c++
#include <iostream>
#include <type_traits>
#include <string>

// 定义一个基本的 TypeProcessor 结构体，用于默认情况下处理任意类型
template <typename T, typename = void>
struct TypeProcessor {
  static void process(const T& value) {
    std::cout << "Default processing: " << value << std::endl;
  }
};

// 部分特化 TypeProcessor 结构体来处理字符串类型
template <typename T>
struct TypeProcessor<T, typename std::enable_if<std::is_same<T, std::string>::value>::type> {
  static void process(const T& value) {
    std::cout << "Processing string: " << value << std::endl;
  }
};

// 部分特化 TypeProcessor 结构体来处理整数类型
template <typename T>
struct TypeProcessor<T, typename std::enable_if<std::is_integral<T>::value>::type> {
  static void process(const T& value) {
    std::cout << "Processing integer: " << value << std::endl;
  }
};

int main() {
  TypeProcessor<int>::process(42);             // 输出: Processing integer: 42
  TypeProcessor<double>::process(3.14);        // 输出: Default processing: 3.14
  TypeProcessor<std::string>::process("hello"); // 输出: Processing string: hello

  return 0;
}
```

# 四、C++ 11/14/17/20

每个编译器版本都会定义一个宏：__cplusplus，我们可以通过它来查看编译器版本：

```c++
cout<< __cplusplus <<endl; //201103 即C++11
```

## 4.1 C++ 11

### auto类型推导

`auto`关键字可以让编译器在编译期间通过初始值或者函数返回值推导出变量的类型，即通过`auto`定义的变量必须有初始值。

```c++
list<string> lst;
...
list<string>::iterator ite;
ite = lst.begin();

auto ite = lst.begin();
auto ite; //错误的写法
ite = lst.begin();

// 正确的用法
auto var = val1 + val2; // 根据 val1 和 val2 相加的结果推断出 var 的类型
auto ret = [](double x){return x*x;}; // 根据Lambda表达式返回值推导出 ret 的类型
auto al = { 10, 11, 12 }; //类型是std::initializer_list<int>
```

`auto`关键字做类型自动推导时，依次施加以下规则：

1. 如果初始化表达式是引用，先去除引用。如果 auto 关键字带上 & 则不进行去除。
2. 如果剩下的初始化表达式有顶层的`const`或者`volatile`限定符，去除掉。
3. `auto` 关键字的类型完美转发，比如 `auto&& var=initValue`，此时 `auto&&` 并不意味着这一定是右值引用类型的变量，而是类似于模板函数参数的类型推导，既可能是左值引用，也可能是右值引用。其目的在于把初始化表达式的值分类情况，完美转发给由auto 声明的变量：
   1. 如果初始化值是类型A的左值，则声明的变量类型为左值引用A&。
   2. 如果初始化值是类型A的右值，则声明的变量类型为右值引用A&&。

即，**使用 auto 关键字声明变量的类型，不能自动推导出顶层的 const 或者 volatile，也不能自动推导出引用类型，需要程序中显式声明**，比如：

```c++
const int v1 = 101;
auto v2 = v1;         // v2 类型是int，脱去初始化表达式的顶层const
v2 = 102；            // 可赋值

int a = 100;
int &b = a; 
auto c = b;          // c 类型为int，去掉了初始化表达式的 &
```

初始化表达式为数组，auto 关键字推导的类型为指针。数组名在初始化表达式中自动隐式转换为首元素地址的右值。

```c++
int a[9]; 
auto j = a; // 此时j 为指针为 int* 类型，而不是 int(*)[9] 类型
std::cout << typeid(j).name() << " "<<sizeof(j)<<" "<<sizeof(a)<< std::endl;
```

> 注意：编译器推导出来的类型和初始值的类型并不完全一样，编译器会适当地改变结果类型使其更符合初始化规则。

### decltype说明符

`decltype` 和 `auto` 的功能一样，都用来在编译时期进行自动类型推导。`decltype`用于检查实体的声明类型，或表达式的类型和值类别。区别在于，`decltype`的类型推导并不是像`auto`一样是从变量声明的初始化表达式获得变量的类型，而是总是**以一个普通表达式作为参数**，返回该表达式的类型，而且`decltype`并不会对表达式进行求值。因此，`auto` 要求变量必须初始化，而 `decltype` 不要求。这很容易理解，`auto` 是根据变量的初始值来推导出变量类型的，如果不初始化，变量的类型也就无法推导了。

如果希望从表达式中推断出要定义的变量的类型，但是不想用该表达式的值初始化变量，这时就不能用`auto`关键字。`decltype`的作用是选择并返回操作数的数据类型。它们的用法区别如下：

```c++
auto var = val1 + val2; 
decltype(val1 + val2) var1 = 0; //根据val1 + val2 表达式推导出变量的类型，变量的初始值和与表达式的值无关
```

`decltype` 的基本推导规则：

1. 如果实参exp是一个没有带括号的标记符表达式或者类成员访问表达式，那么`decltype（exp）`就是exp所命名的实体的类型。此外，如果exp是一个被重载的函数，则会导致编译错误。
2. 否则 ，假设exp的类型是T：
   1. 如果exp的值类别为亡值，那么`decltype（exp）`为T&&。
   2. 如果exp的值类别为左值，那么`decltype（exp）`为T&。
   3. 如果exp的值类别为纯右值，则`decltype（exp）`为T。

标记符指的是除去关键字、字面量等编译器需要使用的标记之外的程序员自己定义的标记，而单个标记符对应的表达式即为标记符表达式。例如：

```c++
int arr[4];
```

arr为一个标记表达式，而arr[3]+0则不是。

注意，如果对象的名字带有括号，则它被当做通常的左值表达式，从而 `decltype(exp)` 和 `decltype((exp))` 通常是不同的类型。

在难以或不可能以标准写法进行声明的类型时，`decltype` 很有用，例如 lambda 相关类型或依赖于模板形参的类型。

`decltype`的基本用法如下：

```c++
int i = 4;
decltype(i) a; //推导结果为int。a的类型为int。
decltype((i))b=i;//b推导为int&，必须为其初始化，否则编译错误
```

推导更为复杂的lambda表达式类型：

```c++
auto f = [](int a, int b) -> int
{
    return a * b;
};

decltype(f) g = f; // lambda 的类型是独有且无名的
i = f(2, 2);
j = g(3, 3);

std::cout << "i = " << i << ", " //i = 4
    << "j = " << j << '\n'; //j = 9
```

在泛型编程中，`decltype`经常和`auto`结合使用，用于追踪函数的返回值类型：

```c++
template <typename _Tx, typename _Ty>
auto multiply(_Tx x, _Ty y)->decltype(_Tx*_Ty)
{
    return x*y;
}
```

编译时类型推导的出现正是为了泛型编程，在非泛型编程中，我们的类型都是确定的，根本不需要再进行推导。因此，这也是`decltype`关键字最主要的应用场景。

参考：[C++11特性：decltype关键字](https://www.cnblogs.com/QG-whz/p/4952980.html)

### 范围for语句

范围for语句主要用于遍历各种容器包括数组的元素，基本格式和应用：

```c++
// expression必须是一个序列，例如用花括号括起来的初始值列表、数组、vector、string等
// 这些类型的共同特点是拥有能返回迭代器的begin和end成员。
for (dec1 : expression) { statement } 
```

![image-20200830213028503](https://hunk-pic-store.oss-cn-beijing.aliyuncs.com/img/image-20200830213028503.png)

```c++
#include <iostream>
#include <vector>
using namespace std;
int main() {
    char arr[] = "hello world!";
    for (auto &c : arr) {
        cout << c;
    }  
    return 0;
}
```

### RAII

`RAII（Resource acquisition is initialization，资源获取即初始化）`，是在一些[面向对象语言](https://zh.wikipedia.org/wiki/面向对象语言)中的一种习惯用法。RAII要求，资源的有效期与持有资源的对象的生命周期严格绑定，**即由对象的构造函数完成资源的分配（获取），同时由析构函数完成资源的释放**，在这种要求下，只要对象能正确的析构，就不会出现资源泄露问题，从而提高程序的健壮性和可维护性。

使用RAII可以做哪些事情？主要可以管理动态分配的内存而不需要手动申请和释放，管理锁不需要手动加锁和解锁，管理句柄不需要手动打开和关闭。

C++标准库中哪些类型已经使用了RAII技术？std::shared_ptr、std::unique_ptr、std::lock_guard和std::unique_lock。

关于RAII的具体解释参考以下链接：

1. https://zh.wikipedia.org/wiki/RAII
2. https://stackoverflow.com/questions/76796/general-guidelines-to-avoid-memory-leaks-in-c
3. [C++面试八股文：什么是RAII？](https://zhuanlan.zhihu.com/p/636891176)

### 智能指针

参考资料：

1. [深入实践C++11智能指针](https://blog.csdn.net/code_peak/article/details/119722167) ***
2. 《Effective Modern C++》- 第四章

### lambda表达式

`lambda` 表达式，也叫 lambda函数或 lambda匿名函数，其基本定义如下：

```c++
[capture list] (parameter list) -> return type
{
function body;
};
```

其中，`capture list`是捕获列表，指lambda表达式所在函数中定义的局部变量的列表。

```c++
[]      // 没有定义任何变量。使用未定义变量会引发错误。
[x, &y] // x以传值方式传入（默认），y以引用方式传入。
[&]     // 任何被使用到的外部变量都隐式地以引用方式加以引用。
[=]     // 任何被使用到的外部变量都隐式地以传值方式加以引用。
[&, x]  // x显式地以传值方式加以引用。其余变量以引用方式加以引用。
[=, &z] // z显式地以引用方式加以引用。其余变量以传值方式加以引用。
```

定义在与 lambda 函数相同作用域的参数引用也可以被使用，一般称作closure（闭包），下面是闭包的常见用法：

```c++
int main() {
    int a = 10;
    auto f = [&a](int x)-> int {
        a = 20;
        return a + x;
    };
    cout << a <<endl; // 10
    cout << f(10) <<endl; // 30
    cout << a << endl; // 20
    return 0;
}
```

需要注意的是 lambda 函数**以值方式捕获的环境中的变量，在 lambda 函数内部是不能修改的**。否则，编译器会报错。其值是 lambda 函数定义时捕获的值，不再改变。如果在 lambda 函数定义时加上 `mutable` 关键字，则该捕获的传值变量在 lambda 函数内部是可以修改的，对同一个 lambda 函数的随后调用也会累加影响该捕获的传值变量，但对外部被捕获的那个变量本身无影响。

```c++
#include <iostream> 
using namespace std;
int main() { 
      size_t t = 9;
      auto f = [t]() mutable{
          t++;
          return t; 
      };
      cout << f() << endl; // 10
      t = 100;
      cout << f() << endl; // 11
      cout << "t:" << t << endl; // t: 100
      return 0;
}
```

### constexpr 常量表达式

常量表达式对编译器来说是优化的机会，编译器时常在编译期执行它们并且将值存入程序中。同样地，在许多场合下，C++ 标准要求使用常量表示式。例如在数组大小的定义上，以及枚举值（enumerator values）都要求必须是常量表示式。常量表示式不能含有函数调用或是对象构造函数。所以像是以下的例子是不合法的：

```c++
int g() {return 5;}
int f[g() + 10]; // 不合法的C++ 写法
```

由于编译器无从得知函数 g() 的返回值为常量，因此表达式 g() + 10 就不能确定是常量。C++ 11 引进关键字 `constexpr` 允许用户保证函数或是对象构造函数是编译期常量，编译器在编译时将去验证函数返回常量。

```c++
constexpr int g() {return 5;}
int f[g() + 10]; // 合法
```

### 列表初始化

https://www.cnblogs.com/Galesaur-wcy/p/15325414.html

https://zhuanlan.zhihu.com/p/687748237

### nullptr

1. nullptr 是C++ 11 新增的关键字，用于表示空指针，是一种特殊类型的字面值，可以被转换成任意其它类型。
2. NULL 是预处理变量，是一个宏，它的值是 0，定义在头文件 中，即 #define NULL 0。

相比于传统的NULL，`nullptr` 的优势：

1. 有类型：类型是 typdef decltype(nullptr) nullptr_t;，使用 nullptr 提高代码的健壮性。
2. 函数重载：因为 NULL 本质上是 0，在函数调用过程中，若出现函数重载并且传递的实参是 NULL，可能会出现不知和哪一个函数匹配的情况；但是传递实参 nullptr 就不会出现这种情况。

```c++
#include <iostream>
#include <cstring>
using namespace std;

void fun(char const *p) {
    cout << "fun(char const *p)" << endl;
}

void fun(int tmp) {
    cout << "fun(int tmp)" << endl;
} 

int main() {
    fun(nullptr); // fun(char const *p)
    // fun(NULL); // error: call of overloaded 'fun(NULL)' is ambiguous
    return 0;
}
```



## 4.2 C++ 14

## 4.3 C++ 17

## 4.4 C++ 20
