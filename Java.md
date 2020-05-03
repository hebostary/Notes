# 语言基础

## 认识Java

Java并不只是一个编程语言，而是一个完整的平台，有一个庞大的库，其中包含了很多可重用的代码和一个提供诸如安全性，跨操作系统的可移植性以及自动垃圾回收等服务的执行环境。

### Java与C++的一些比较

* **面向对象**：Java与C++的主要不同点在于多重继承，在Java中，取而代之的是更简单的接口概念。

* **健壮性**：Java和C++最大的不同在于java采用的指针模型可以消除重写内存和损坏数据的可能性。

* **安全性**：沙箱运行环境。
* **可移植性**：与C/C++不同，Java规范中没有“依赖具体实现”的地方。基本数据类型的大小以及有关运算都做了明确的说明。比如，Java的int永远是32位的整数，而C/C++中可能是16位或者32位的整数，也可能是编译器提供商指定的其它大小，只是做了一些类型大小约束：short int <= int <= long int。
* 跨平台性：一次编译，到处运行。

### Java源代码基础规约

* 标准的命名规范：骆驼命名法，比如FirstSample.java。
* 源代码的文件名必须与公共类的名字相同，并用.java作为扩展名。

### 注释

* // comments
* /* comments */ --这种注释不能嵌套，不能简单地把代码用/\*和\*/括起来，因为这段代码本身也可能包含一个\*/。
* /**  comments */  --可用于自动生成文档。

## 基础语法

### 基本数据类型

Java是一种强类型语言，意味着必须为每一个变量声明一种类型。在Java中，一共有8种基本类型：

|  type   | size (bytes) |                      value range                       | comments        |
| :-----: | :----------: | :----------------------------------------------------: | --------------- |
|   int   |      4       |     -2 147 483 648 ~ 2 147 483 647 (正好超过20亿)      |                 |
|  short  |      2       |                    -32 768 ~ 32 767                    |                 |
|  long   |      8       | -9 223 372 036 854 775 808 ~ 9 223 372 036 854 775 807 |                 |
|  byte   |      1       |                       -128 ~ 127                       |                 |
|  float  |      4       |      大约(-+)3.402 823 47E+38F（有效位为6 ~ 7位）      |                 |
| double  |      8       | 大约(-+)1.797 693 134 862 315 70E+308（有效位为15位）  |                 |
|  char   |      2       |                        比如'A'                         | 使用unicode编码 |
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

### 变量和常量

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

### 数值类型转换

Java数值类型之间的合法转换如下，实箭头表示无信息丢失，虚箭头表示可能有精度损失。

![自动转换](https://note.youdao.com/yws/api/personal/file/4AF129CE75D044C28C0902284C731B6D?method=download&shareKey=340be1fa5a56e497c78e9ffcb5643ae7)

#### 自动类型转换

两个不同类型的数值进行二元操作时，先要将两个操作数转换为同一类型，然后再进行计算。自动转换的优先级：double > float > long > int。 比如计算float + double时，先将float自动转换为double。

#### 强制类型转换

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

### 运算符

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

### 枚举类型

```java
enum Size { SMALL, MEDIUM, LARGE};
Size s = Size.MEDIUM;
```

Size的变量值只能存储这个类型声明中给定的某个枚举值，或者null，null表示这个变量还没设置任何值。

### 字符串

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

#### 字符串的比较

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

#### 空串与Null串

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

#### 码点与代码单元

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

#### 高效构建字符串

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

### 联机文档

[Java SE 11 API Documentation](https://docs.oracle.com/en/java/javase/11/docs/api/index.html)



### 输入输出

#### 读写文件

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

### 控制语句

Java的控制流程结构与C/C++的控制流程结构一样，只有很少的例外情况。没有*goto*语句，但*break*语句可以带标签，可以利用它实现从内层循环跳出的目的。

* *if* (condition) statement1 *else if* statement2 *else* statement3
* *while*(condition) { statement }
* *do* { statement } *while* (condition)
* *for* (int i = 1; i < 10;  i++) { statement }
* *for* ( variable : collection ) { statement } //for each循环。collection这一集合表达式必须是一个数组或者是一个实现了Iterable接口的类对象（支持迭代器访问），比如ArrayList。
* *switch* (choice) { *case* 1: ... *break*; *default*: ... *break* } //case标签可以是：类型为char/byte/short/int的常量表达式；枚举常量；字符串字面量。
* *break* label//通过执行带标签的break跳转到标签的语句块末尾。break也可以不跟标签，一般用于跳出循环语句。
* *continue* //中断正常的控制流程，continue语句将控制转移到最内层循环的首部。Java也支持带标签的continue语句，将跳转到与标签匹配的循环首部。

### 大数值

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

### 数组

#### 数组的初始化

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

#### 数组的拷贝

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

#### 数组排序

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

#### 二维数组

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

#### 不规则数组**

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



## 面向对象OOP

### 对象与类

### 封装

### 继承

## Java接口

## 异常处理

## 泛型编程