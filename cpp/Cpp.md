# C++语言基础

## C++代码结构和布局

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

## Class设计基础

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

### 传递值和传递引用

#### 引用的本质

无论是参数传递还是返回值传递，**传递者都无需知道接收者是以引用形式接收**，因此直接传递值就可以了，比如Complex类中\_\_doapl函数中返回*ths是OK的，以及c2 += c1的运算也是OK的。

#### 什么情况下不能传递引用？

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

### 操作符重载

#### 操作符重载1 --成员函数

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

#### 操作符重载2 --非成员函数

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

注意，这几个重载函数返回的都是值类型，因为它们返回的是函数内部创建的临时对象，所以这里绝不能返回引用（return by reference）。

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
int main 
{
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

### 构造和析构函数

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

### 浅拷贝和深拷贝

前面在Complex中我们没有去实现拷贝构造函数和拷贝赋值函数也是OK的，是因为编译器会自动提供这些拷贝函数的默认版本。但是，默认版本只会完成最基础的成员数据拷贝，如果类包含指针成员，那么拷贝后的结果就是两个对象指向同一份底层数据，也就是发生了**浅拷贝**。比如下图中m_data指针的浅拷贝，这会带来两个问题：

1. 其中一份数据没有被正常释放，造成内存泄漏。
2. 两个对象的成员指针指向同一个底层数据，任何修改会相互影响。

![image-20200823202356309](https://hunk-pic-store.oss-cn-beijing.aliyuncs.com/img/image-20200823202356309.png)

我们的期望是在拷贝对象时，能够同时拷贝底层数组，拷贝后的对象互相不会影响，也就是**深拷贝**。**所以，对于这种带指针成员的类，我们一定要实现自定义的拷贝构造函数和拷贝赋值函数。**

### 拷贝构造函数

拷贝构造函数相对简单，只需要申请空间然后拷贝对象里的数据即可。拷贝构造函数仍然属于构造函数，所以没有返回值。

```c++
inline String::String(const String& str)
{
	m_data = new char[ strlen(str.m_data) + 1 ];//申请合适的空间
    strcpy(m_data, str.m_data);//拷贝底层字符数组
}
```

### 拷贝赋值函数

拷贝赋值函数稍微复杂一些，在S2 = S1操作中，我们需要先释放S2原来分配的资源，然后重新分配和S1的数据相同大小的空间，最后再将S1的数据内容拷贝到S2。

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

## C++对象内存管理

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

### new/delete操作符

![image-20200823220834552](https://hunk-pic-store.oss-cn-beijing.aliyuncs.com/img/image-20200823220834552.png)

1. Complex类只有两个double类型的成员数据，所以第一步为Complex对象分配内存的时候分配的就是2个double的空间。
2. 按大小为对象分配了空间，随后需要做类型转换。
3. 最后，调用对象的构造函数完成初始化。

![image-20200823220905752](https://hunk-pic-store.oss-cn-beijing.aliyuncs.com/img/image-20200823220905752.png)

**delete**可以作**new**的逆向操作，先调用析构函数（一般是释放构造函数中为成员数据申请的堆内存），最后释放对象对象本身占用的堆内存。

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

![image-20200823221250104](https://hunk-pic-store.oss-cn-beijing.aliyuncs.com/img/image-20200823221250104.png)当然，对象本身占用的空间始终是可以正常释放的，因为对象的存储空间里记录了整个对象的大小信息。如果是Complex对象数组，用delete去删除也不会发生内存泄漏。如果是String对象数组，就会发生内存泄漏。

> 为了良好的编程习惯，new出来的动态数组始终用delete[]去删除。

## Class设计提高

### 静态成员数据和函数

前面介绍的Complex类和String类的成员数据都是非静态的，每个类对象都独有一份自己的数据。成员函数都有一个隐式参数this指针（指向类对象本身），它由编译器自动添加，我们不用写在函数参数列表里，但是可以在函数内直接使用。访问非静态成员数据时，实际上就是通过this指针访问一个具体类对象的数据。

![image-20200824201626970](https://hunk-pic-store.oss-cn-beijing.aliyuncs.com/img/20200825091629.png)

如果我们希望所有的类对象共享同一份成员数据，就需要使用static关键字将其定义成静态成员数据。比如每个银行客户都有自己的账户，但是所有账户使用相同的利率。相应的，静态函数就是专门（也只能）用于处理静态成员数据，我们可以通过类名直接调用静态成员函数来访问静态成员数据，也可以通过类对象访问。

![image-20200824201933836](https://hunk-pic-store.oss-cn-beijing.aliyuncs.com/img/20200825091630.png)

> Account类黄色部分代码是很重要的，因为类内部只是声明了一个静态成员数据m_rate，实际上没有获得内存空间，我们需要在类的外部对其进行定义和初始化（也可以不初始化）。

### 把构造函数放在私有区

对于单例模式，我们经常把构造函数放到private区，然后通过静态的getInstance方法返回静态实例。只要有地方首次调用了这个方法，之后所有的调用操作的都是同一份数据。

![image-20200824203549418](https://hunk-pic-store.oss-cn-beijing.aliyuncs.com/img/20200825091631.png)

但是这个实现有个问题，即便没有人调用getInstance方法，静态成员a仍然存在，可以考虑仅在首次调用getInstance后才会创建这个数据：

![image-20200824203959609](https://hunk-pic-store.oss-cn-beijing.aliyuncs.com/img/20200825091632.png)

### 操作符重载的威力

#### 关于std::cout

为什么std::cout可以输出很多不同类型的数据呢？看完下面这张图就明白了，因为它的底层类**ostream**重载了不同类型参数的<<操作。

![image-20200824204441485](https://hunk-pic-store.oss-cn-beijing.aliyuncs.com/img/20200825091633.png)

#### 关于迭代器

迭代器是访问容器数据的常用工具，在迭代器的设计中同样利用了大量的操作符重载来实现迭代器的基本操作。下面的__list_iterator是作用于list这种数据结构的迭代器，它主要包含一个\_\_list_node类型的指针数据node，用于指向list中的一个元素。

![image-20200829184041089](https://hunk-pic-store.oss-cn-beijing.aliyuncs.com/img/image-20200829184041089.png)

对迭代器的解引用操作 *ite，其实是希望得到list节点里的数据，所以返回的是node里的数据对象data，其类型T是在创建list时指定。

在迭代器上调用方法 ite->method()，其实是调用list节点里的数据对象data的方法，因此等价于(*ite).method() 或者 (&(*ite))->method();。

![image-20200829185405862](https://hunk-pic-store.oss-cn-beijing.aliyuncs.com/img/image-20200829185405862.png)



### 类模板

类模板允许我们设计更为通用的类，比如队列这种数据结构，实现一个queue类，就可以用来装载不同类型的数据。在类的声明和定义中使用抽象化的数据类型T，在实例化时用<>指定具体类型即可。对于前面介绍的复数类，它的实部和虚部也不一定是double类型，也可以是float或者int类型，采用类模板会让这个类更通用，使用者可以轻松实例化不同版本的复数类型。

![image-20200824204549290](https://hunk-pic-store.oss-cn-beijing.aliyuncs.com/img/20200825091634.png)

后面将会专题介绍C++模板编程。

### 函数模板

很多函数操作针对不同类型的处理是一样的，那么如何将函数抽象成一个通用版本呢？比如获取两个数据中较小值的min函数，对于基础数据类型，可以利用重载机制实现相应的min函数版本。但是对于复杂的自定义类类型，我们就没法通过重载min函数来做到了。类似于类模板，可以使用**函数模板**来编写min函数，将操作的数据类型抽象为T，min函数内部只包含最基础的比较操作。基本语法如下：

![image-20200824213345709](https://hunk-pic-store.oss-cn-beijing.aliyuncs.com/img/20200825091638.png)

编译器会自动对函数模板做实参类型推导，比如上图中编译 r3 = min(r1, r2)这条语句时，编译器会推导出T就是stone类型，就会去调用stone::operator<，这也就要求stone类重载了<操作符，否则就会编译报错。

这样设计的好处就是只需要提供一个最精简的min函数，作为参数传递的类重载比较操作符即可。实际上，也只有设计类的人最清楚应该怎么去定义比较操作。

### 成员模板

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

对于上面的调用代码，将会遇到如下编译错误，编译器无法找到合适的构造函数，在编译Pair<Base1, Base2> p2(p)时，对于p2编译器推导的T1和T2类型分别是Base1和Base2，但是参数里传递的是Derived1和Derived2。

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

### 仿函数

仿函数也叫做function-like classes，像函数一样的类。既然有类似于函数的行为，就需要重载函数调用操作符()。比如下面这些标准库中的仿函数设计：

![image-20200829185937079](https://hunk-pic-store.oss-cn-beijing.aliyuncs.com/img/image-20200829185937079.png)

![image-20200829190059551](https://hunk-pic-store.oss-cn-beijing.aliyuncs.com/img/image-20200829190059551.png)

### 命名空间

![image-20200824213507486](https://hunk-pic-store.oss-cn-beijing.aliyuncs.com/img/20200825091639.png)

## 面向对象OOP

前面重点介绍的是如何设计单个类，叫做基于对象的编程/设计（Object Based Programing/Design）。本节更多的是探讨类与类之间的关系，叫做面向对象编程/设计（Object Oriented Programing/Design）。

### 复合（Composition）

复合表示的是一种包含关系（has a），一个类实例同时包含了另一个类的实例。比如下面的queue类和deque类就是这种关系，每个queue包含一个deque。deque是一种两边都可以进出的双端队列，而queue只提供一种先进先出的队列，所以queue对deque的部分操作进行适当的包装即可实现，这也是一种叫做Adapter的设计模式。

![image-20200824213327781](https://hunk-pic-store.oss-cn-beijing.aliyuncs.com/img/20200825091637.png)

![image-20200824204626420](https://hunk-pic-store.oss-cn-beijing.aliyuncs.com/img/20200825091635.png)

#### 复合关系下的构造和析构

我们把外部类叫做的Container，被包含的类叫做Component，那么在创建Container对象时，是先调用Container构造函数还是先调用Component的构造函数呢？

![image-20200824210637073](https://hunk-pic-store.oss-cn-beijing.aliyuncs.com/img/20200825091636.png)

红色部分的调用由编译器自动添加，析构函数是唯一的，但是构造函数可能有多个，编译器始终调用默认构造函数，如果希望调用其它的构造函数版本，需要我们自己显式调用。

> 对于复合关系中的Container和Component，它们的生命周期是一致的，同时被创建，同时被销毁。

### 委托（Delegation）

委托其实也是一种复合关系，只不过是Composition by reference，即类通过成员数据里的指针或引用指向另一个类的实例。比如下面字符串类String的设计里，并不是把底层字符串直接放到String对象里，而是用一个StringRep对象来专门存放底层字符串，然后用一个指针rep指向StringRep对象。

![image-20200827115543232](https://hunk-pic-store.oss-cn-beijing.aliyuncs.com/img/image-20200827115543232.png)

这也是一种叫做Handle/Body的设计模式，这样设计的好处是拥有相同字符串内容的不同String对象可以共享相同的底层StringRep对象，从而介绍内存空间。

1. 在StringRep对象中包含引用计数count，来记录有多少个String对象指向自己，只有当最后一个指向它的String对象被释放时，StringRep对象才会被真正的释放。
2. 如果String对象需要修改字符串内容，就会创建并指向新的StringRep对象，这也叫写时复制（COW，copy on write）。

#### 关于智能指针

对于委托关系，另一个比较好的例子是标准库里智能指针类**shared_ptr**的设计：

![image-20200829182245219](https://hunk-pic-store.oss-cn-beijing.aliyuncs.com/img/image-20200829182245219.png)

**shared_ptr**类包含了一个指向实际数据对象的指针px，并且让对 shared_ptr 对象的指针操作间接作用在px上，这就需要**shared_ptr**类重载指针相关的操作符：

1. 解引用操作符 * ：期望返回的是px指向的对象。
2. 访问操作符 ->：在sp ->method()调用里，sp->首先得到的是px，px是一个指针类型，在指针类型上调用方法似乎还需要一个->操作符。实际上，这是C++对于->操作符的一个特殊处理，允许->操作符持续作用在数据上。

### 继承（Inheritance）

继承是一个is a的关系，在继承关系中，派生类就是一个基类，因为它继承了基类里的数据和方法，只是拥有更多自己的数据和方法。比如，人也是一种动物。

![image-20200827114532644](https://hunk-pic-store.oss-cn-beijing.aliyuncs.com/img/image-20200827114532644.png)

#### 继承关系下的构造和析构

![image-20200825090526833](https://hunk-pic-store.oss-cn-beijing.aliyuncs.com/img/20200825091640.png)

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

### 继承+复合

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

### 委托+继承

在下面的例子中就用到了委托+继承的关系，我们有一个保存数据实体的类Subject，但是有多个不同类对象来展示同一个Subject对象里的数据，这些展示类叫做观察者（Observer）。当Subject里的数据更新时，所有观察者都能得到通知，然后及时更新数据的展示情况。

![image-20200827122707499](https://hunk-pic-store.oss-cn-beijing.aliyuncs.com/img/image-20200827122707499.png)

这里体现的就是观察者设计模式。我们在Subject里包含一个指向不同观察者的指针列表m_views，通过attach方法将一个观察者注册到到观察者列表，当Subject对象数据更新时，调用notify方法通知所有的观察者即可。在notify方法里，依次调用所有观察者的update方法，这个方法在观察者基类Observer中是一个纯虚函数，意味着派生类都需要实现这个方法，也就是如何去更新展示数据。





# C++ 泛型编程

## 模板特化和偏特化

### 模板特化

模板特化就是针对泛型T的某些具体类型提供特定的实现版本，比如：

![image-20200830204354980](https://hunk-pic-store.oss-cn-beijing.aliyuncs.com/img/image-20200830204354980.png)

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

## 数量不定的模板参数（since C++11）

```c++
//.hpp
void myPrint();

//数量不定的模板参数
template<typename T, typename... Types>
void myPrint(const T& firstArg, const Types&... args)
{
    cout << firstArg <<", size of args:" << sizeof...(args) << endl;
    myPrint(args...);
}
```

```c++
//.cpp
//如果把这个实现直接定义在hpp文件中，链接时会遇到重复定义的问题
void myPrint()
{
    cout << "END..." << endl;
}
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

... 就是一个所谓的包（pack）：

1. 用于模板参数，就是模板参数包（template parameters pack）。
2. 用于函数参数类型，就是函数参数类型包（function parameter types pack）。
3. 用于函数参数，就是函数参数包（function parameter pack）。

在上面的例子中，myPrint递归调用到最后，98.5作为firstArg时，后面的参数包args已经为空，等价于调用myPrint（），因此需要额外提供一个没有参数的版本，否则编译器会检测到这种错误。

# C++ STL

# C++ 设计模式

# C++ 新标准11/14

每个编译器版本都会定义一个宏：__cplusplus，我们可以通过它来查看编译器版本：

```c++
cout<< __cplusplus <<endl; //201103 即C++11
```

## C++ 11

### auto关键字

```c++
list<string> lst;
...
list<string>::iterator ite;
ite = lst.begin();
```

auto关键字可以让编译器帮助我们做类型推导：

```c++
auto ite = lst.begin();

//错误的写法
auto ite;
ite = lst.begin();
```

### ranged-based for

ranged-based for语句主要用于遍历各种容器包括数组的元素，基本格式和应用：

```c++
for (dec1 : coll) { statement }
```

![image-20200830213028503](https://hunk-pic-store.oss-cn-beijing.aliyuncs.com/img/image-20200830213028503.png)

## C++ 14

# 系统设计与实践



