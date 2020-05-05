# 认识Java

Java并不只是一个编程语言，而是一个完整的平台，有一个庞大的库，其中包含了很多可重用的代码和一个提供诸如安全性，跨操作系统的可移植性以及自动垃圾回收等服务的执行环境。

## Java与C++的一些比较

* **面向对象**：Java与C++的主要不同点在于多重继承，在Java中，取而代之的是更简单的接口概念。

* **健壮性**：Java和C++最大的不同在于java采用的指针模型可以消除重写内存和损坏数据的可能性。

* **安全性**：沙箱运行环境。
* **可移植性**：与C/C++不同，Java规范中没有“依赖具体实现”的地方。基本数据类型的大小以及有关运算都做了明确的说明。比如，Java的int永远是32位的整数，而C/C++中可能是16位或者32位的整数，也可能是编译器提供商指定的其它大小，只是做了一些类型大小约束：short int <= int <= long int。
* 跨平台性：一次编译，到处运行。

## Java源代码基础规约

* 标准的命名规范：骆驼命名法，比如FirstSample.java。
* 源代码的文件名必须与公共类的名字相同，并用.java作为扩展名。

## 注释

* // comments
* /* comments */ --这种注释不能嵌套，不能简单地把代码用/\*和\*/括起来，因为这段代码本身也可能包含一个\*/。
* /**  comments */  --可用于自动生成文档。

# 基础语法

## 基本数据类型

Java是一种强类型语言，意味着必须为每一个变量声明一种类型。在Java中，一共有8种基本类型：

|  type   | size (bytes) |                      value range                       | comments        |
| :-----: | :----------: | :----------------------------------------------------: | --------------- |
|   int   |      4       |     -2 147 483 648 ~ 2 147 483 647 (正好超过20亿)      |                 |
|  short  |      2       |                    -32 768 ~ 32 767                    |                 |
|  long   |      8       | -9 223 372 036 854 775 808 ~ 9 223 372 036 854 775 807 |                 |
|  byte   |      1       |                       -128 ~ 127                       |                 |
|  float  |      4       |      大约(-+)3.402 823 47E+38F（有效位为6 ~ 7位）      |                 |
| double  |      8       | 大约(-+)1.797 693 134 862 315 70E+308（有效位为15位）  |                 |
|  char   |      2       |                        比如'A'                         | 使用Unicode编码 |
| boolean |      4       |                     true \| false                      |                 |

关于整型的说明：

* 前面4种整型的范围与运行Java代码的机器无关，并且Java没有任何无符号（unsigned）形式的int，long，short或byte类型。
* 从Java7开始：前缀0b或者0B表示二进制数，比如0b1001就是9；为了提高代码易读性，数字字面量可以加下划线，比如用1_000_000（或0b1111_0100_0010_0100_0000）表示一百万，Java编译器会去除这些下划线。

关于浮点类型的说明：

* double表示这种类型的数值精度是float类型的两倍，绝大部分应用程序都应该使用double类型，也称为双精度浮点类型。
* float类型的数值有一个后缀f或者F（例如3.14F），没有后缀F的浮点值默认为double类型。
* 所有的浮点值计算都遵循IEEE 754规范，有三个表示溢出和出错情况的特殊值（常量）：
  * Double.POSITIVE_INFINITY（正无穷大）。
  * Doule.NEGATIVE_INFINITY（负无穷大）。
  * Double.NaN（不是一个数字，比如计算0/0或者符数的平方根）。检测一个特定值是否等于Double.NaN的正确方法：if (Double.isNaN(x))，而不是if (x == Double.NaN) //is never true，因为所有“非数值”都认为是不相同的。
* 浮点数值不适用于无法接受舍入误差的金融计算中，而应该使用*BigDecimal*类。

关于char类型的说明：

* 在Java中，char数据类型是一个采用UTF-16编码表示Unicode码点的代码单元。
* 强烈建议不要在程序中使用char类型，除非确实需要处理UTF-16代码单元，最好将字符串作为抽象数据类型处理。

关于布尔值的说明：

* 在C++中，数值甚至指针可以代替boolean值。在Java中，整型值和布尔值之间不能进行相互转换。

## 变量和常量

在Java中：

* 变量名对大小写敏感。

* 不区分变量的声明和定义。

* 声明一个变量后，必须用赋值语句对变量进行显示初始化，千万不要使用未初始化的变量，否则编译器会报错：

```java
public static void main(String[] args) {
    int a;
    System.out.println(a); //The local variable a may not have been initialized
}
```

* 利用关键字*final*指示常量。关键字*final*表示这个变量只能被赋值一次，一旦被赋值，则不能再更改。习惯上，常量名使用全大写。
* 在一个类的多个方法中使用的常量称为类常量，用*static final*声明。需要注意，类常量的定义位于main方法的外部。因此，同一个类的其它方法也可以使用。并且，如果一个常量被声明位*public*，那么其他类的方法也可以使用这个变量。

```java
public class HelloWorld {
    public static final String HELLO_STRING = "Hello Java!!!";
    public static void main(String[] args) {
        System.out.println(HELLO_STRING);
    }
}
```

## 数值类型转换

Java数值类型之间的合法转换如下，实箭头表示无信息丢失，虚箭头表示可能有精度损失。

![自动转换](https://note.youdao.com/yws/api/personal/file/4AF129CE75D044C28C0902284C731B6D?method=download&shareKey=340be1fa5a56e497c78e9ffcb5643ae7)

### 自动类型转换

两个不同类型的数值进行二元操作时，先要将两个操作数转换为同一类型，然后再进行计算。自动转换的优先级：double > float > long > int。 比如计算float + double时，先将float自动转换为double。

### 强制类型转换

```java
double x = 9.99;
int nx = (int)x; //nx = 9；强制类型转换通过截断小数部分将浮点数转换为整型
```

如果相对浮点数进行舍入运算，以得到最接近的整数，就需要使用Math.round方法：

```java
doubld x = 9.99;
int nx = (int)Math.round(x); //nx = 10, Math.round返回结果为long类型，所以前面还得加上(int)
```

> 如果试图将一个数值从一种类型强制转换为另一种类型，而又超出了目标类型的表示范围，结果就会截断成一个完全不同的值。
>
> 不要在boolean类型与任何数值类型之间进行强制类型转换，这样可以防止发生错误。极少数情况可以用条件表达式b ? 1 : 0。

## 运算符

Java的逻辑运算符和C++基本相同：

|                    运算符                    |      运算意义      | 说明                                  |
| :------------------------------------------: | :----------------: | ------------------------------------- |
|                      &&                      |   逻辑”与“运算符   |                                       |
|                     \|\|                     |   逻辑“或”运算符   |                                       |
|                      ！                      |   逻辑“非”运算符   |                                       |
|                 <. >, <=, >=                 |     比较运算符     |                                       |
|                     ? :                      |     三元运算符     | condition ? expression1 : expression2 |
| &（"and"）  \|（"or"） ^（"xor"） ~（"not"） |      位运算符      |                                       |
|                 >>, <<, >>>>                 | 位模式左移或者右移 | >>>>运算符会用0填充高位，>>不会       |

* 移位运算符的右操作数要完成模32的运算（除非做操作数为long类型，做模64运算），即1 << 35等价于1 << 3等价于8。

## 枚举类型

```java
enum Size { SMALL, MEDIUM, LARGE};
Size s = Size.MEDIUM;
```

Size的变量值只能存储这个类型声明中给定的某个枚举值，或者null，null表示这个变量还没设置任何值。

## 字符串

Java没有内置的字符串类型，而是在标准Java类库中提供了一个预定义类String。

当将一个字符串与一个非字符串的值进行拼接时，后者被转换成字符串。

```java
int age = 13;
String user = "hunk" + age;
System.out.println(user); //hunk13
```

> **不可变字符串**：String类没有提供用于修改字符串的方法，字符串”HELLO“永远包含字符H,E,L,L,O的代码单元（Unicode编码中的概念）序列，而不能修改其中的任何一个字符。当然，可以修改字符串变量，让它引用另外一个字符串。不可变字符串的优点：**编译器可以让字符串共享**。
>
> Java的设计者认为共享带来的高效率远胜过于提取，拼接字符串所带来的低效率。对于不再被使用的字符串，将由GC自动完成回收。

### 字符串的比较

```java
int age = 13;
String user = "hunk" + age;
System.out.println(user);

System.out.println("hunk13".equals(user)); //true
System.out.println("HUNK13".equalsIgnoreCase(user)); //true，比较时忽略大小写

System.out.println(user == "hunk13"); //false

String user1 = "hunk13";
String user2 = "hunk13";
System.out.println(user == user1); //false
System.out.println(user1 == user2); //true
```

> 一定不要使用==运算符检测两个字符串是否相等！这个运算符只能够确定两个字符串是否在同一个位置上（内存中的同一份数据）。因此，只有内存中共享的字符串比较结果才会是true。实际上，只有字符串常量是共享的，而+运算符或者substring等操作产生的结果并不是共享的。

### 空串与Null串

空串""是长度为0的字符串对象，有自己的串长度（0）和内容（空），判断方法：

```java
if (str.length() == 0) //或者
if (str.equals(""))
```

String变量还可以存放一个特殊的值，名为null，表示目前没有任何关联对象，判断方法：

```java
if (str == null)
```

如果要检查一个字符串既不是null也不是空串，需要依次检查：

```java
if  (str != null && str.length() != 0)
```

### 码点与代码单元

>  码点（code point）是指与一个编码表中的某个字符对应的代码值。在Unicode标准中，码点采用十六进制书写，并加上前缀U+，例如U+0041就是拉丁字母A的码点。
>
> Unicode的码点可以分成17个代码级别（code plane），第一个代码级别称为*基本的多语言级别*，码点从U+0000到U+FFFF，其中包括经典的Unicode代码；其余的16个级别码点从U+10000到U+10FFFF，其中包括一些辅助字符。
>
> UTF-16编码采用不同长度的编码表示所有的Unicode码点。在*基本的多语言级别*中（包含了大多数常用Unicode字符），每个字符用16位表示，通常被称为代码单元（code unit），而辅助字符采用一对连续的代码单元进行编码。

Java字符串由char值序列组成，char数据类型是一个采用UTF-16编码表示Unicode码点的代码单元。length方法返回采用UTF-16编码表示的给定字符串所需要的代码单元数量。比如：

```java
String user2 = "hunk13";
System.out.println(user2.length());//6，字符数量，可能包含辅助字符
System.out.println(user2.codePointCount(0, user2.length()));//6，实际长度，即码点数量
```

返回位置指定位置的代码单元，n介于0 ~ length()-1之间：

```java
System.out.println(user2.charAt(0));//h
```

### 高效构建字符串

有些时候，需要由较短的字符串来构建字符串，其实就是字符串拼接。但是，采用字符串连接的方式效率比较低。每次连接字符串，都会构建一个新的String对象，既耗时，又浪费空间。使用StringBuilder类就可以避免这个问题：

```java
String str = "tringbuilder";
StringBuilder builder = new StringBuilder();
builder.append('S');
builder.append(str);
String compString = builder.toString();
System.out.println(compString); //Stringbuilder
```

> StringBuilder类是单线程环境中编辑字符串的最佳选择。还有一个效率低一些的StringBuffer类，允许采用多线程的方式执行添加或者删除字符的操作。

## 联机文档

[Java SE 11 API Documentation](https://docs.oracle.com/en/java/javase/11/docs/api/index.html)



## 输入输出

### 读写文件

* Scanner(File f)：构造一个从给定文件读取数据的Scanner，比如Scanner(System.in)，从标准输入读取数据。
* Scanner(String data)：构造一个从给定字符串读取数据的Scanner。
* PrintWriter(String fileName)：构造一个将数据写入文件的PrintWriter。

```java
try {
    String fileString = "myFile.txt";
    //写文件
    PrintWriter out = new PrintWriter(fileString, "UTF-8");
    out.println(message);
    out.println("Good morning!");
    out.close();

    //读文件
    Scanner in = new Scanner(Paths.get(fileString), "UTF-8"); //Paths.get()根据给定的路径名构造一个Path
    System.out.println(in.nextLine()); //Hello java 11，读取一行
    System.out.println(in.next()); //Good，只读取一个单词，以空白符作为分隔符
    in.close();
} catch (Exception e) {
    //TODO: handle exception
}
```

## 控制语句

Java的控制流程结构与C/C++的控制流程结构一样，只有很少的例外情况。没有*goto*语句，但*break*语句可以带标签，可以利用它实现从内层循环跳出的目的。

* *if* (condition) statement1 *else if* statement2 *else* statement3
* *while*(condition) { statement }
* *do* { statement } *while* (condition)
* *for* (int i = 1; i < 10;  i++) { statement }
* *for* ( variable : collection ) { statement } //for each循环。collection这一集合表达式必须是一个数组或者是一个实现了Iterable接口的类对象（支持迭代器访问），比如ArrayList。
* *switch* (choice) { *case* 1: ... *break*; *default*: ... *break* } //case标签可以是：类型为char/byte/short/int的常量表达式；枚举常量；字符串字面量。
* *break* label//通过执行带标签的break跳转到标签的语句块末尾。break也可以不跟标签，一般用于跳出循环语句。
* *continue* //中断正常的控制流程，continue语句将控制转移到最内层循环的首部。Java也支持带标签的continue语句，将跳转到与标签匹配的循环首部。

## 大数值

如果基本的整数和浮点数精度不能满足需求，可以使用*java.math*包中的两个很有用的类：*BigInteger*和*BigDecimal*。这两个类可以处理任意长度数字序列的数值。BigInteger类实现了任意精度的整数运算，BigDecimal实现了任意精度的浮点数运算。

但是，我们不能使用常规的运算符+和*等处理大数值，而需要使用类中的add和multiply等方法：

```java
BigInteger a = BigInteger.valueOf(100);
BigInteger b = BigInteger.valueOf(200);
BigInteger c = a.add(b); //a + b
BigInteger d = c.multiply(a.add(BigInteger.valueOf(2))); //d = c * (a + 2)
System.out.println(c); //300
System.out.println(d); //30600
```

## 数组

### 数组的初始化

```java
//声明一个数组变量，但是并未将其初始化为一个真正的数组，在调用new初始化之前不能使用
int[] a0;
//使用new运算符创建数组：
//数值元素都被初始化为0，布尔元素都被初始化为false；
//对象数组则初始化为null，表示这些元素还未存放任何对象
int[] a1 = new int[5];
String[] a2 = new String[5];
System.out.println(Arrays.toString(a1)); //[0, 0, 0, 0, 0]
System.out.println(Arrays.toString(a2)); //[null, null, null, null, null]
//对数组元素逐个进行初始化
for (int i = 0; i < a1.length; i++) {
    a1[i] = i;
}

//创建数组对象并同时赋予初始值的简写形式，这种概况不需要使用new
int[] arr1 = {1, 2, 3, 4};

//在不创建新变量的情况下重新初始化一个数组
//new int[] { ... }实际上是创建了一个匿名数组并完成初始化
//这条语句实际上等价于下面两条：
//int[] anonymous = {10, 20, 30, 40, 50};
//arr1 = anonymous;
arr1 = new int[] {10, 20, 30, 40, 50};
```

### 数组的拷贝

```java
//数组浅拷贝：java允许将一个数组变量拷贝给另一个数组变量。
//这时，两个变量将引用同一个底层数组
int[] arr2 = arr1;
arr2[3] = 400;
System.out.println(Arrays.toString(arr1)); //[10, 20, 30, 400, 50]

//数组深拷贝：如果希望将数组的所有值拷贝到一个新的数组中，
//就使用Arrays类的copyOf方法：
//第二个参数是数组的长度，这个方法通常用于增加数组大小
//如果数组元素是数值型，多余元素被赋值为0；布尔型赋值为false。
//如果长度小于原始数组长度，则只拷贝最前面的数据元素。
int[] arr3 = Arrays.copyOf(arr1, arr1.length);
arr3[3] = 4000;
System.out.println(Arrays.toString(arr1)); //[10, 20, 30, 400, 50]
```

### 数组排序

```java
//数组排序：这个方法采用了优化的快排算法
Arrays.sort(arr1);
System.out.println(Arrays.toString(arr1)); //[10, 20, 30, 50, 400]
```



> int[] a = new int[100]; // Java  等同于 int* a = new int[100]; //C++（堆分配） 而不同于 int a[100]; //C++（栈分配）。
>
> Java中的[]运算符被预定义为检查数组边界，而且没有指针运算，即不能通过a加1得到数组的下一个元素。
>
> 在Arrays类中还提供了很多操作数组的方法，比如二分查找和数组比较等。
>
> Java应用程序的main方法中，程序名并没有存储在args数组中。

### 二维数组

```java
//声明二维数组，在调用new初始化之前不能使用
double[][] balances;
//new初始化
balances = new double[3][3];
//知道数组元素的初始化
int[][] magicSquare = {
    {1, 2, 3},
    {4, 5, 6},
    {7, 8, 9}
};

//for each循环蹦年自动处理二维数组中的每一个元素，需要两个嵌套的循环
for (int[] row : magicSquare) {
    for (int ele : row) {
        System.out.printf("%d ", ele);
    }
}
System.out.println(Arrays.deepToString(magicSquare));//[[1, 2, 3], [4, 5, 6], [7, 8, 9]]
```

### 不规则数组

Java实际上没有多维数组，只有一维数组。多维数组被解释为“数组的数组”。这也大大提高了Java数组的灵活性：

* 可以单独的存储数组的某一行，因此可以交换不同的行。
* 可以构造*不规则数组*，即每行都有不同的长度。

```java
//可以交换数组的不同行
int[] temp = magicSquare[0];
magicSquare[0] = magicSquare[1];
magicSquare[1] = temp;
System.out.println(Arrays.deepToString(magicSquare));//[[4, 5, 6], [1, 2, 3], [7, 8, 9]]

//不规则数组
int MAX = 5;
//先分配一个具有所含行数的数组
int[][] odds = new int[MAX + 1][];
//依次分配各个行
for (int i = 0; i <= MAX; i++) {
    odds[i] = new int[i + 1];
}
System.out.println(Arrays.deepToString(odds));
//[[0], [0, 0], [0, 0, 0], [0, 0, 0, 0], [0, 0, 0, 0, 0], [0, 0, 0, 0, 0, 0]]
```

```java
double[][] balances = new doule[10][6]; //Java
//不同于
double balances[10][6]; //C++
//也不同于
double (*balances)[6] = new double[10][6]; //C++
//而是分配了一个包含10个指针的数组：
double** balances = new double*[10]; //C++
//然后，指针数组的每一个元素被填充了一个包含6个数字的数组：
    for(i = 0; i < 10; i++)
        balances[i] = new double[6];
//不过，当创建new double[10][6]时，这个循环将自动地执行
//当需要不规则数组时，只能按前面地例子单独地创建行数组
```



# 面向对象OOP

## 对象与类

类（class）是构造对象的模板或蓝图。由类构造对象的过程称为创建类的实例（instance）。

 识别类的简单规则是在分析问题的过程中寻找名词，而方法对应着动词。

封装（encapsulation），其实就是将数据和行为组合在一个包中，并对对象的使用者隐藏了数据的实现方式。对象中的数据称为*实例域*（instance field）， 操纵数据的过程称为*方法*。实现封装的关键在于绝对不能让类中的方法直接地访问其他类的实例域。程序仅通过对象的方法与对象数据进行交互。封装给对象赋予了”黑盒“特征，这是提高重用性和可靠性的关键。

在Java中，使用*构造器*构造新实例。*构造器*是一种特殊的方法，用来构造并初始化对象，并且没有返回值。构造器的名字应该与类相同。比如Date类的构造器名为Date，在构造器前面加上new操作符，就可以构造一个对象：

```java
Data birthday = new Date();
```

>  构造器与类中的其它方法有一个重要的不同：构造器总是伴随着new操作符的执行被调用，而不能对一个已经存在的对象调用构造器来达到重新设置实例域（对象私有数据）的目的。

通常，希望构造的对象可以多次使用，因此，需要将对象存放在一个*对象变量*中，比如上面的birthday。需要注意的是，一个对象变量并没有实际包含一个对象，而仅仅引用一个对象。我们可以显示地将对象变量设置为null，表示这个对象变量目前没有引用任何对象。

```java
birthday = null;
```

>* 在Java中，任何对象变量的值都是对存储在另外一个地方的一个对象的引用。new操作符的返回值也是一个引用。
>
>* 可以将Java的对象变量看作是C++的对象指针，而不是C++的引用，因为C++中没有空引用（必须初始化），并且引用不能被赋值。
>
>* 局部变量不会自动地初始化为null，而必须通过调用new或者将它们设置为null进行初始化。
>
>* 所有Java对象都存储在堆中。当一个对象包含另一个对象变量时，这个变量依然包含着指向另一个堆对象的指针。
>* 在Java中使用一个没有初始化的指针，运行系统将会产生一个运行时错误，而不是像C++那样生成一个随机的结果，从而可能导致内存溢出。
>* C++通过拷贝构造器和复制操作符来实现对象的自动拷贝，而Java必须使用clone方法获得对象的完整拷贝。

### 用户自定义类

主力类（workhorse class）：这些类没有main方法，却有自己的实例域和实例方法。要想创建一个完整的程序，应该将若干类组合在一起，其中只有一个类有main方法。

源文件名必须与**public**类的名字相匹配，在一个源文件中，只能有一个公有类，但可以有任意数目的非公有类。

```java
//EmployeeTest.java

public class EmployeeTest
{
    public static void main(String[] args)
    {
        //...
    }
}

class Employee
{
    //实例域
    //可以用public标记实例域，即允许程序的任何方法对其进行读取和修改，完全破坏了封装。
    //因此，强烈建议将实例域标记成private
    private String name;
    
    //构造器
    public Employee(String n)
    {
        //...
    }
    //...
}
```

>当编译这段源代码的时候，编译器将在目录下创建两个类文件：EmployeeTest.class 和 Employee.class。将程序中包含main方法的类名提供给字节码解释器，以便启动这个程序。

在Java中，所有的方法都必须在类的内部定义，但并不表示它们是内联方法。是否将某个方法设置为内联方法是Java虚拟机的任务。即时编译器会监视调用那些简洁，经常被调用，没有被重载以及可优化的方法。

### 多个源文件的使用

有时候我们希望一个源文件只包含一个class。比如将上面的EmployeeTest.java拆分成EmployeeTest.java和Employee.java，有两种编译方法：

* *javac Employee.java*。所有与通配符匹配的源文件都将被编译成类文件。
* *javac EmployeeTest.java*。当Java编译器发现EmployeeTest.java使用了Employee类时会查找名为Employee.class的文件。如果没有找到则自动搜索Employee.java进行编译。如果Employee.java有更新，也会自动重新编译。

### 隐式参数和显示参数

```java
public void reaiseSalary(double byPercent)
{
    double raise = this.salary * byPercent / 100;
    this.salary += raise;
}
```

以上面的raiseSalary方法为例，它有两个参数：

* 第一个参数称为*隐式参数*，是出现在方法名前的Employee类对象。在每一个方法中，关键字*this*表示隐士参数。在实例域前加上this可以将实例域与局部变量明显区分开来。
* 第二个参数就是方法名后面括号里的数值，称为*显示参数*。

### 封装的优点

注意不要编写返回引用可变对象的访问器方法。比如下面这个糟糕的设计，getHireDay方法返回了一个Date类对象，而Date类有一个更改器方法setTime可以设置毫秒数。Date对象是可变的，这一点就破坏了封装性。

```java
class Employee
{
    private Date hireDay;
    
    //...
    public Date getHireDay()
    {
        return hireDay;
    }
    //...
}
```

继续看下面的代码：

```java
Employee harry = ...;
Date d = harry.getHireDay();
double tenYearsInMs = 10 * 365 * 24 * 60 * 1000；
d.setTime(d.getTime() - (long)tenYearsInMs);
```

出错的原因很微妙，d和harry.hireDay引用同一个对象，对d调用更改器方法就可以自动改变这个雇员对象的私有状态。如果需要返回一个可变对象的引用，应该首先对它进行克隆（clone）。对象clone是指存放在另一个位置上的对象副本。

```java
class Employee
{
    private Date hireDay;
    
    //...
    public Date getHireDay()
    {
        return (Date) hireDay.clone();
    }
    //...
}
```

### 基于类的访问权限

来看下面的代码：

```java
class Employee
{
   //...
    public boolean equals(Employee other)
    {
        return name.equals(other.name);
    }
}
```

典型的调用方式：

```java
if (harry.equals(boos)) ...
```

这个方法不仅访问了harry的私有域，它还访问了boos的私有域。这是合法的，其原因是boos是Employee类对象，而Employee类的方法可以直接访问Employee类的任何一个对象的私有域。

### 静态域与静态方法(static)

对*static*关键字的解释：属于类且不属于类对象的变量和函数。

#### 静态域

```java
class Employee
{
    //静态域（field），每个类中只有一个这样的域。
    //每一个雇员对象都有一个自己的id域，但这个类的所有实例将共享一个nextId域。
    private static int nextId = 1;
    private int id;
    
    public void setId()
    {
        id = nextId;
        nextId++;
    }
}
```

#### 静态常量

```java
public class Math
{
    //静态常量：程序中，可以采用Math.PI的形式获得这个常量。
    //如果关键字static被省略，PI就变成了Math类的一个实例域，需要通过Math类的对象访问PI，
    //并且每个Math对象都有自己的一份PI拷贝。
    public static final double PI = 3.141592;
}
```

#### 静态方法

```java
//静态方法：
//1.静态方法是一种不能向对象实施操作的方法，或者说没有隐式的参数（this），比如下面的getNextId不能访问Id实例域。
//2.静态方法可以访问自身类中的静态域，比如下面的静态方法getNextId访问静态域nextId。
public static int getNextId()
{
    return nextId;
}

//可以通过类名调用类的静态方法，也可以使用对象调用静态方法来代替类的调用（容易混淆）。
//建议使用类名，而不是对象来调用静态方法。
int n = Employee.getNextId;
int n = harry.getNextId;
```

一般地，我们在下面两种情况下使用静态方法：

1. 一个方法不需要访问对象状态，其所需参数都是通过显示参数提供（例如Math.pow）。
2. 一个方法只需要访问类的静态域。

#### main方法

main方法也是一个静态方法。main方法不对任何对象进行操作。事实上，在启动程序时还没有任何一个对象，静态的main方法将执行并创建程序所需要的对象。

```java
public static void main(String[] args) 
{
    //...
}
```

> 每个类可以有一个main方法，这是一个常用于对类进行单元测试的技巧。

### 方法参数

程序设计语言设计中对于调用方法的参数传递，一般分为*按值调用（call by value）*和*按引用调用（call by reference）*，C++中这两种调用方式都支持。

Java总是采用按值调用。也就是说，方法得到的是所有参数值的一个拷贝，特别是，方法不能修改传递给它的任何参数变量的内容。考虑下面的代码：

```java
public static void tripleValue(double x)
{
    x = 3 * x;
}

//调用代码
double percent = 10;
tripleValue(percent);
```

上面的调用结束后，percent得值仍然是10，执行的具体过程：

1. x被初始化为percent值得一个拷贝。
2. x被乘以3后等于30，但是percent仍然是10.
3. 调用方法结束后，参数变量x不再使用。

但是，方法参数共有两种类型，这两种参数类型有一些区别：

1. 基本数据类型（数值，布尔等）。一个方法不可能修改一个基本数据类型的参数。
2. 对象引用。当传递对象引用作为方法参数时，方法得到的是对象引用的拷贝，对象引用及其它的拷贝同时引用同一个对象。因此，在方法中改变对象参数状态是可以做到的。

注意，即便是第二种对用引用类型的参数传递，也仍然是值调用，而不是引用调用。继续看下面的例子：

```java
public static void swap(Employee x, Employee y)
{
    Employee temp = x;
    x = y;
    y = temp;
}

//调用代码
Employee a = new Employee("Jack", ...);
Employee b = new Employee("Bob", ...);
swap(a, b);
```

实际上，上面的调用代码并没有改变存储在变量a和b中的对象引用，无法达到交换a和b的目的。swap方法的参数x和y被初始化为两个对象引用的拷贝，这个方法交换的是这两个拷贝，方法调用结束后，这两个拷贝也就被自动释放。

这个例子说明，Java对对象采用的不是引用调用，实际上，对象引用调用是按值传递的。也就意味着，一个方法不能让对象参数引用一个新的对象。

### 对象构造

#### 重载

如果有多个方法有相同的名字，不同的参数，便产生了重载（overloading）。编译器通过用各个方法给出的参数类型与特定方法调用所使用的值类型进行匹配，来挑选出相应的方法。如果编译器招不到匹配的参数，就会产生编译时错误。这个过程被称为重载解析（overloading resolution）。

Java允许重载任何方法，而不只是构造器方法。因此，要完整地描述一个方法，需要指出方法名及参数类型，这叫方法的签名（signature）。比如String类有4个称为indexOf的公有方法，它们的签名是：

```java
indexOf(int)
indexOf(int, int)
indexOf(String)
indexOf(String, int)
```

注意，返回类型不是方法签名的一部分。

#### 无参数的构造器

当且仅当类没有提供任何构造器的时候，系统才会提供一个默认的无参数构造器。

#### 调用另一个构造器

关键字this引用方法的隐式参数。然而，它还有另外一个含义，如果构造器的第一个语句形如this(...)，这个构造器将调用同一个类的另一个构造器。

```java
public class Employee {
    private static int nextId = 0;
    private String name;
    private double salary;

    public Employee(String n, double s)
    {
        name = n;
        salary = s;
    }

    public Employee(double s)
    {
        // calls Employee(String, double)
        this("Employee #" + nextId, s);
        nextId++;
    }

    public static void main(String[] args) {
        Employee employee1 = new Employee(6000);
        System.out.println(employee1.name); //Employee #0
        System.out.println(employee1.salary); //6000.0
    }
}
```

>  采用这种方式使用this关键字非常有用，这样对公共的构造器代码部分只编写一次即可。

#### 初始化数据域

Java提供下面三种机制来初始化数据域：

```java
public class Employee {
    //1.在声明中赋值初始化
    private static int nextId = 0;
    private int id;
    private String name;
    private double salary;

    //2.初始化块（initialization block），这种方法不常用。
    //在一个类中可以包含多个代码块，只要构造类的对象，这些块就会被执行。
    //首先运行初始化块，然后才运行构造器的主体部分，观察id的变化。
    {
        id = nextId;
    }

    //3.在构造器中设置初始值，这也是最常用的方法。
    public Employee(String n, double s)
    {
        name = n;
        salary = s;
        nextId++;
    }

    public static void main(String[] args) {
        Employee employee1 = new Employee("hunk", 6000);
        Employee employee2 = new Employee("jack", 6000);
        System.out.println(employee1.id); //0
        System.out.println(employee2.id); //1
    }
}
```

由于初始化数据域有多种途径，所以列出构造过程的所有路径可能相当混乱。下面是调用构造器的具体处理步骤：

1. 所有数据域被初始化为默认值（0，false或null）。
2. 按照在类声明中出现的次序，依次执行所有域初始化语句和初始化块。
3. 如果构造器第一行调用了第二个构造器，则执行第二个构造器主体。
4. 执行这个构造器主体。

如果对类的静态域进行初始化的代码比较复杂，那么可以使用如下所示的*静态的初始化块*。

```java
//static initialization block
static
{
    Random generator = new Random();
    nextId = generator.nextInt(1000);
}
```

在类第一次加载的时候，将会进行静态域的初始化。所有的静态初始化语句以及静态初始化块都将按照类定义的顺序执行。

### 对象析构与finalize方法

在C++中，析构器方法最常见的操作就是回收分配给对象的存储空间。由于Java有自动GC，不需要人工回收内存，所以Java不支持析构器。但是，有些对象使用了内存之外的其它资源，比如文件句柄。在这种情况下，当资源不再需要时，将其回收和再利用将显得十分重要。

可以为任何一个类添加finalize方法，它将在垃圾回收器清除对象之前调用。在实际应用中，不要依赖于使用finalize方法回收任何资源，这是因为很难知道这个方法什么时候才能够调用。

如果某个资源需要在使用完毕后立刻被关闭，那么就需要人工来管理。对象用完时，可以应用一个close方法来完成相应的清理操作。

## 包

Java允许使用包（package）将类组织起来，借助于包可以方便地组织自己的代码，并将自己的代码与别人提供的代码库分开管理。

标准的Java类库分布在多个包中，包括java.lang, java.util和java.net等。Sun公司建议将公司的因特网域名以逆序的形式作为包名，比如com.hebostary.corejava。

从编译器的角度来看，嵌套的包之间没有任何关系。例如，java.util与java.util.jar包毫无关系。

定义包：

```java
//./com/hebostary/corejava/Employee.java
package com.hebostary.corejava;
...
```

引用包：

```java
import com.hebostary.corejava.*;
...
```

> 类存储在文件系统的子目录中，而类的路径必须与包名匹配。
>
> 在C++中，与Java包机制类似的是命名空间（namespace）。在Java中，package与import语句类似于C++中的namespace和using指令。

### 静态导入

import语句不仅可以导入类，还增加了导入静态方法和静态域的功能。

比如我们在com.hebostary.corejava包中提供了Employee类，它有一个静态的Test方法：

```java
//Employee.java
package com.hebostary.corejava;

public class Employee {
    //...
    public static void Test() 
    {
        //...
    }
```

使用Employee类的静态方法：

```java
//CoreJavaTest.java
import com.hebostary.corejava.*;

public class CoreJavaTest
{
    public static void main(String[] args) {
        Employee.Test();
    }
}
```

### 包作用域

前面已经接触过访问修饰符public和private，下面做一些总结：

* 标记为public的部分可以被任意的类使用；
* 标记为private的部分只能被定义它们的类使用；
* 如果没有指定public或private，这个部分（类，方法或变量）可以被同一个包中的所有方法访问

## 文档注释

javadoc工具用于从源文件生成一个HTML文档。javadoc从以下几个特征中抽取信息：

* 包
* 公有类与接口
* 公有类的和受保护的构造器及方法
* 公有的和受保护的域

```java
package com.hebostary.corejava;

/**
 * 类注释：This ia employ class.
 * 通用注释：
 * @author hunk
 * @version 1.0
 * ...
*/
public class Employee {
    //...

    /**
     * 方法注释：Execute test case of this class
     * @param None
     * @return void
    */
    public static void Test() {
        //...
    }
}
```

## 类设计技巧

1. 一定要保证数据私有。这是最重要的，绝对不要破坏封装性。

2. 一定要对数据初始化，而不是依赖于系统的默认值。

3. 不要在类中使用过多的基本类型，比如用Address类替换一个Customer类中以下的实例域：

   ```java
   private String street;
   private String city;
   private String state;
   private int zip;
   ```

4. 不是所有的域都需要独立的域访问器和域更改器。

5. 将职责过多的类进行分解。

6. 类名和方法名要能体现它们的职责。

7. 优先使用不可变的类。

## 继承

# Java接口

# 异常处理

# 泛型编程