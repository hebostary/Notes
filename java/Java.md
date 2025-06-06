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

## 编译打包和运行

这一小节的内容可以在后面练习一些基础代码，对包有一些了解之后再看。

### 编译

```shell
javac -d ./bin -cp ./src:$CLASSPATH ./src/CoreJavaTest.java ./src/com/hebostary/corejava/*.java
```

### 打包jar包

```shell
cd bin
jar -cvfe  ../pkg/CoreJavaTest.jar CoreJavaTest .
cd -
```

### 解压jar包

```shell
jar -xvf pkg/CoreJavaTest.jar
```

### 运行jar包

```shell
java -jar pkg/CoreJavaTest.jar
```

### 目录结构

```shell
[root@izuf682lz6444cynn96up0z java]# tree
.
├── bin
│   ├── com
│   │   └── hebostary
│   │       └── corejava
│   │           ├── ArrayListTest$1.class
│   │           ├── ArrayListTest.class
│   │           ├── collection
│   │           │   ├── BitSetTest$1.class
│   │           │   ├── BitSetTest.class
│   │           │   ├── CollectionTest$1.class
│   │           │   ├── CollectionTest.class
│   │           │   ├── EnumSetMapTest$1.class
│   │           │   ├── EnumSetMapTest.class
│   │           │   ├── EnumSetMapTest$Weekday.class
│   │           │   ├── MapTest$1.class
│   │           │   └── MapTest.class
│   │           ├── Common.class
│   │           ├── Employee.class
│   │           ├── EmployeeSort.class
│   │           ├── EnumTest.class
│   │           ├── HelloWorld.class
│   │           ├── Manager.class
│   │           └── Size.class
│   └── CoreJavaTest.class
├── build.sh
├── pkg
│   └── CoreJavaTest.jar
└── src
    ├── com
    │   └── hebostary
    │       └── corejava
    │           ├── ArrayListTest.java
    │           ├── collection
    │           │   ├── BitSetTest.java
    │           │   ├── CollectionTest.java
    │           │   ├── EnumSetMapTest.java
    │           │   └── MapTest.java
    │           ├── Common.java
    │           ├── Employee.java
    │           ├── EmployeeSort.java
    │           ├── EnumTest.java
    │           ├── exception
    │           │   ├── ExceptionTest.java
    │           │   └── FileFormatException.java
    │           ├── generic
    │           │   ├── GenericMethod.java
    │           │   └── PairTest.java
    │           ├── HelloWorld.java
    │           ├── innerClass
    │           │   ├── InnerClass.java
    │           │   └── StaticInnerClass.java
    │           ├── lambda
    │           │   └── Lambda.java
    │           ├── Manager.java
    │           ├── proxy
    │           │   └── ProxyTest.java
    │           ├── reflect
    │           │   └── Reflection.java
    │           └── stackTrace
    │               ├── StackTrace$1.class
    │               ├── StackTrace.class
    │               └── StackTrace.java
    └── CoreJavaTest.java
```

# 基础语法

## 基本数据类型

Java是一种强类型语言，意味着必须为每一个变量声明一种类型。在Java中，一共有8种基本类型：

|  type  | size (bytes) |                      value range                      | comments        |
| :-----: | :----------: | :----------------------------------------------------: | --------------- |
|   int   |      4      |     -2 147 483 648 ~ 2 147 483 647 (正好超过20亿)     |                 |
|  short  |      2      |                    -32 768 ~ 32 767                    |                 |
|  long  |      8      | -9 223 372 036 854 775 808 ~ 9 223 372 036 854 775 807 |                 |
|  byte  |      1      |                       -128 ~ 127                       |                 |
|  float  |      4      |      大约(-+)3.402 823 47E+38F（有效位为6 ~ 7位）      |                 |
| double |      8      | 大约(-+)1.797 693 134 862 315 70E+308（有效位为15位） |                 |
|  char  |      2      |                        比如'A'                        | 使用Unicode编码 |
| boolean |      4      |                      true\| false                      |                 |

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

|                   运算符                   |      运算意义      | 说明                                  |
| :----------------------------------------: | :----------------: | ------------------------------------- |
|                     &&                     |  逻辑”与“运算符  |                                       |
|                    \|\|                    |  逻辑“或”运算符  |                                       |
|                     ！                     |  逻辑“非”运算符  |                                       |
|                <. >, <=, >=                |     比较运算符     |                                       |
|                    ? :                    |     三元运算符     | condition ? expression1 : expression2 |
| &（"and"）\|（"or"） ^（"xor"） ~（"not"） |      位运算符      |                                       |
|                >>, <<, >>>>                | 位模式左移或者右移 | >>>>运算符会用0填充高位，>>不会       |

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

> 码点（code point）是指与一个编码表中的某个字符对应的代码值。在Unicode标准中，码点采用十六进制书写，并加上前缀U+，例如U+0041就是拉丁字母A的码点。
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

> 构造器与类中的其它方法有一个重要的不同：构造器总是伴随着new操作符的执行被调用，而不能对一个已经存在的对象调用构造器来达到重新设置实例域（对象私有数据）的目的。

通常，希望构造的对象可以多次使用，因此，需要将对象存放在一个*对象变量*中，比如上面的birthday。需要注意的是，一个对象变量并没有实际包含一个对象，而仅仅引用一个对象。我们可以显示地将对象变量设置为null，表示这个对象变量目前没有引用任何对象。

```java
birthday = null;
```

> * 在Java中，任何对象变量的值都是对存储在另外一个地方的一个对象的引用。new操作符的返回值也是一个引用。
> * 可以将Java的对象变量看作是C++的对象指针，而不是C++的引用，因为C++中没有空引用（必须初始化），并且引用不能被赋值。
> * 局部变量不会自动地初始化为null，而必须通过调用new或者将它们设置为null进行初始化。
> * 所有Java对象都存储在堆中。当一个对象包含另一个对象变量时，这个变量依然包含着指向另一个堆对象的指针。
> * 在Java中使用一个没有初始化的指针，运行系统将会产生一个运行时错误，而不是像C++那样生成一个随机的结果，从而可能导致内存溢出。
> * C++通过拷贝构造器和复制操作符来实现对象的自动拷贝，而Java必须使用clone方法获得对象的完整拷贝。

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

> 当编译这段源代码的时候，编译器将在目录下创建两个类文件：EmployeeTest.class 和 Employee.class。将程序中包含main方法的类名提供给字节码解释器，以便启动这个程序。

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

这个例子说明，Java对对象采用的不是引用调用，实际上，对象引用调用是按**值传递**的。也就意味着，一个方法不能让对象参数引用一个新的对象。

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

> 采用这种方式使用this关键字非常有用，这样对公共的构造器代码部分只编写一次即可。

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

通过一个最简单的例子理解类继承：

```java
package com.hebostary.corejava;

public class Manager extends Employee {
    //子类继承父类的方法和域，也可以添加自己的方法和域
    private double bonus;

    public  Manager(String n, double s, double b) 
    {
        //通过super调用父类中的构造器方法
        //使用super调用构造器的语句必须是子类构造器的第一条语句
        super(n, s);
        bonus = b;
    }

    public void setBonus(double b) {
        bonus = b;
    }

    //覆盖父类中的方法实现
    public double getSalary()
    {
        //子类中无法直接访问父类中的private实例域
        //通过super来调用父类中的public方法
        double baseSalary = super.getSalary();
        return baseSalary + bonus;
    }

    public static void Test() {
        Common.PrintClassHeader("Manager");
        Employee[] staff = new Employee[3];
        staff[0] = new Manager("Hunk", 6000, 3000);
        staff[1] = new Employee("Jack", 7000);
        staff[2] = new Employee("Harry", 8000);
        for (Employee e : staff) {
            System.out.printf("%s's total salary: %8.2f.\n", e.getName(), e.getSalary());
        }
    }
}
```

请注意， 尽管在foreach循环中将 e 声明为 Employee 类型，但实际上 e 既可以引用 Employee 类型的对象，也可以引用 Manager 类型的对象。当 e 引用 Employee 对象时， e.getSalary( ) 调用的是 Employee 类中的 getSalary 方法；当e 引用 Manager 对象时，e.getSalary( ) 调用的是 Manager 类中的 getSalary 方法。虚拟机知道
e 实际引用的对象类型，因此能够正确地调用相应的方法。

> 关键字this有两个用途：1.引用隐式参数；2.调用该类的其它构造器。类似地，super关键字也有两个用途：1.调用超类的方法；2.调用超类的构造器。在调用构造器的时候， 这两个关键字的使用方式很相似。调用构造器的语句只能作为另一个构造器的第一条语句出现。 构造参数既可以传递给本类 （ this) 的其他构造器， 也可以传递给超类（super ) 的构造器。

### 继承层次

继承并不仅限于一个层次，而是可以任意派生。在继承层次中， 从某个特定的类到其祖先的路径被称为该类的继承链 ( inheritance chain) 。

> 和C++不同，Java 不支持多继承。

### 多态

一个对象变量（例如， 变量 e ) 可以指示多种实际类型的现象被称为**多态（ polymorphism)**。在运行时能够自动地选择调用哪个方法的现象称为**动态绑定（dynamic binding)**。

```java
Employee[] staff = new Employee[3];
Manager boss =  new Manager("Hunk", 6000, 3000);
staff[0] = boss;
```

需要注意的是，在上面的例子中，尽管staff[0]和boss变量引用了同一个Manager对象（实际上也是一个Employee对象），但编译器是将staff[0]看成Employee对象，这就意味着：

```java
boss.setBonus(2000); //OK
//but
staff[0].setBonus(2000); //Error
```

这是因为staff[0]声明的类型是Employee，而setBonus不是Employee类的方法。

此外，也不能将一个超类的引用赋给子类变量。例如，下面的赋值是非法的 ：

```java
Manager m = staff[i]; //Error
```

因为Manager is a Employee，但是Employee not is a Manager 。

在Java中，子类数组的引用可以转换成超类数组的引用，而不需要采用强制类型转换，例如：

```java
Manager[] managers = new Manager[10];
Employee[] staff = managers; //OK

staff[0] = new Employee(...); //编译器竟然允许这个赋值操作
```

**需要强调的是：staff[0]和managers[0]引用的是同一个对象**。所以，最后的赋值操作把一个普通的雇员归入经理的行列了。这可能导致严重的错误，比如调用manages[0].setBonus(1000);的时候，将会导致调用一个不存在的实例域，进而扰乱相邻存储空间的内容。

> 为了确保不发生这类错误，所有数组都要牢记创建它们的元素类型，并负责监督仅将类型兼容的引用存储到数组中。例如， 使用 new managers[10] 创建的数组是一个经理数组。如果试图存储一个 Employee 类型的引用就会引发 ArrayStoreException 异常。

### 阻止继承：final类和方法

有时候，可能希望阻止人们利用某个类定义子类。不允许扩展的类被称为 final 类。如果在定义类的时候使用了 final 修饰符就表明这个类是 final 类。 例如， 假设希望阻止人们定义Executive类的子类，就可以在定义这个类的时候使用 final 修饰符声明。声明格式如下所示：

```java
public final class Executive extends Manager
{
    //...
}
```

> 类中的特定方法也可以被声明为final。如果这样做，子类就不能覆盖这个方法（final类中的所有方法自动地成为final方法）。实际上，域也可以被声明为final，对于final域来说，构造对象之后就不允许改变它们的值了。不过，如果将一个类声明为final，只有其中的方法自动地成为final，而不包括域。

将方法或类声明为final的主要目的是：确保它们不会在子类中改变语义。比如标准库的String类就是final类，这意味着不允许任何人定义String的子类，换言之，如果有一个 String 的引用， 它引用的一定是一个 String 对象， 而不可能是其他类的对象。

### 强制类型转换

在 Java 中，每个对象变量都属于一个类型。类型描述了这个变量所引用的以及能够引用的对象类型。例如， staff[i] 引用一个 Employee 对象（因此它还可以引用 Manager对象。)

将一个值存人变量时， 编译器将检查是否允许该操作。将一个子-类的引用赋给一个超类变量， 编译器是允许的。但将一个超类的引用赋给一个子类变量， 必须进行类型转换， 这样才能够通过运行时的检査。

进行类型转换的唯一原因是：在暂时忽视对象的实际类型之后， 使用对象的全部功能。 比如，我们需要将数组中引用经理的元素复原成 Manager 类， 以便能够访问新增加的所有变量 。

```java
Employee[] staff = new Employee[3];
Manager boss =  new Manager("Hunk", 6000, 3000);
staff[0] = boss;

Manager boss = (Manager) staff[0]; //OK
```

但是，如果试图在继承链上进行向下的类型转换（超类 --> 子类），并且“ 谎报” 有关对象包含的内容，会发生什么情况呢？

```java
Manager boss = (Manager) staff[1]; //Error
```

运行这个程序时， Java 运行时系统将报告这个错误， 并产生一个 ClassCastException异常。 如果没有捕获这个异常，那么程序就会终止。因此，应该养成这样一个良好的程序设计习惯： 在进行类型转换之前，先查看一下是否能够成功地转换。这个过程简单地使用instanceof 操作符就可以实现。 例如：

```java
if (staff[1] instanceof Manager)
{
    boos = (Manager)staff[1];
    ...
}
```

> 实际上，通过类型转换调整对象的类型并不是一种好的做法。在我们列举的示例中，大多数情况并不需要将 Employee 对象转换成 Manager 对象， 两个类的对象都能够正确地调用getSalary 方法，这是因为实现多态性的动态绑定机制能够自动地找到相应的方法。只有在使用 Manager 中特有的方法时才需要进行类型转换， 例如， setBonus 方法。如果鉴于某种原因，发现需要通过 Employee 对象调用 setBonus 方法， 那么就应该检查一下超类的设计是否合理。重新设计一下超类，并添加 setBonus方法才是正确的选择。请记住，只要没有捕获*ClassCastException*异常，程序就会终止执行。 在一般情况下，应该尽量少用类型转换和*instanceof*运算符。
>
> Java的类型转换处理过程类似于C++的dynamic_cast操作，它们之间只有一点重要的区别： 当类型转换失败时， Java 不会生成一个 null 对象，而是抛出一个异常。
>
> ```c++
> Manager* boos = dynamic_cast<Manager*>(staff[1]);
> if (boss != NULL) ...
> ```

### 抽象类

考虑Employee类的更高层次抽象，我们设计它的超类Person，每个Employee都是一个Person。每个人都有名字这样的属性，因此我们可以把name和对应的getName()方法放到Person类中，这样就可以从Person类派生出Student这样的子类。

现在，再增加一个getDescription方法，它可以返回对一个人的简短描述。在Employee类中实现这个方法很容易，但是Person类中应该提供什么内容呢？除了name之外，Person类一无所知，最简单的方法是让Person类中的getDescription方法返回空字符串，但这不够优雅，我们可以采用*abstract*关键字来定义这样的方法和类：

```java
public abstract class Person
{
    private String name;
    public Person(String name)
    {
        this.name = name;
    }
  
    public abstract String getDescription();
  
    public String getName()
    {
        return name;
    }
}
```

为了提高程序的清晰度， **包含一个或多个抽象方法的类本身必须被声明为抽象的**。  除了抽象方法之外，抽象类还可以包含具体数据和具体方法。例如， Person 类还保存着姓名和一个返回姓名的具体方法。

下面总结一下抽象类的一些特点：

* 抽象方法充当着占位的角色， 它们的具体实现在子类中。扩展抽象类可以有两种选择：
  * 一种是在抽象类中定义部分抽象类方法或不定义抽象类方法，这样就必须将子类也标记为抽象类；
  * 另一种是定义全部的抽象方法，这样一来，子类就不是抽象的了。
* 类即使不含抽象方法，也可以将类声明为抽象类。
* 抽象类不能被实例化。也就是说，如果将一个类声明为 *abstract*, 就不能创建这个类的对象。
* 可以定义一个抽象类的对象变量， 但是它只能引用非抽象子类的对象。

```java
Person[] people = new Person[2];
people[0] = new Enp1oyee(...);
people[1] = new Student(...) ;

for (Person p : people)
	System.out.println(p.getName() + ", " + p.getDescription())；
```

考虑上面代码中的两个细节：

1. Person类没有定义有效的getDescription方法，p.getDescription()为什么可以被调用？因为，**由于变量p永远不会引用Person对象，而是引用Employee和Student这样的具体类对象**，而这些对象中都定义了这个方法。
2. 是否可以省略Person超类中的抽象方法，而仅在Employee和Student类中定义getDescription方法呢？答案是不能。因为**编译器只允许调用在类中声明的方法。**

### 受保护访问

任何声明为public的内容对其它类都是可见的，任何声明为private的内容对其它类都是不可见的。因此，出于封装性和安全性的考虑，建议最好将类中的域标记为private，而方法标记为public。

但是有些时候，我们希望超类中的某些方法允许被子类访问，或允许子类的方法访问超类的某个域。这就需要一种介于public和private之间的访问控制，即声明受保护的（protected）方法或域。

例如， 如果将超类 Employee中的 hireDay 声明为*protected*, 而不是私有的，Manager中的方法就可以直接地访问它。  不过， Manager 类中的方法只能够访问 Manager 对象中的 hireDay 域， 而不能访问其他Employee 对象中的这个域。 这种限制有助于避免滥用受保护机制，使得子类只能获得访问受保护域的权利。

受保护的方法更具有实际意义。 如果需要限制某个方法的使用， 就可以将它声明为protected。这表明子类（可能很熟悉祖先类）得到信任， 可以正确地使用这个方法， 而其他类则不行。  这种方法最好的示例就是Object类中的clone方法。

> 事实上，Java中的受保护部分对所有子类及同一个包中的所有其他类都可见。

下面，归纳一下Java用于控制可见性的4个访问修饰符：

1. private：仅对本类可见。
2. public：对所有类可见。
3. protected：对本包和所有子类可见。
4. 默认（没有修饰符时）：对本包可见。

## Object超类

Object类是Java中所有类的始祖，在Java中每个类都是由它扩展而来的。如果没有明确地指出超类，Object就被认为是这个类的超类。

可以使用Object类型的变量引用任何类型的对象：

```java
Object obj = new Employee(...);
```

但是，Object类型的变量只能用于作为各种值得通用持有者，要想对其中的内容进行具体的操作，还需要清楚对象的原始类型，并进行相应的类型转换：

```java
Employee e = (Employee)obj;
```

在Java中，只有基本数据类型（数值/字符/布尔）不是对象。但是，**所有的数组类型，不管是对象数组还是基本类型的数组都扩展了Object类。**

```java
Employee[] staff = new Employee[10];
obj = staff; //ok
obj = new int[10]; //OK
```

> 在C++中没有所有类的基类，只是每个指针都可以转换成void*指针。

### equals方法

Object类中的equals方法用于检测一个对象是否等于另外一个对象，即判断两个对象是否具有相同的引用。

Java 语言规范要求 equals 方法具有下面的特性：

1. 自反性： 对于任何非空引用 x, x.equals(?0 应该返回 true。
2. 对称性: 对于任何引用 x 和 y, 当且仅当 y.equals(x) 返回 true, x.equals(y) 也应该返回true。
3. 传递性： 对于任何引用 x、 y 和 z, 如果 x.equals(y) 返 N true， y.equals(z) 返回 true，x.equals(z) 也应该返回 true。
4. 一致性： 如果 x 和 y 引用的对象没有发生变化，反复调用 x.eqimIS(y) 应该返回同样的结果。
5. 对于任意非空引用 x, x.equals(null) 应该返回 false。

在决定两个类对象是否相等时，不同的相等语义需要不同的处理，比如：

1. **如果子类能够拥有自己的相等概念，则对称性需求将强制采用getClass进行检测，意味着不是同一个类的对象一定不相等。**比如两个Manager对象的姓名，薪水和雇佣日期均相等，而奖金不相等，我们就认为它们是不同的。在这种相等语义中，就可以使用getClass检测。
2. **如果由超类决定相等的概念，那么就可以使用instanceof进行检测，这样可以在不同子类对象之间进行相等的比较。**比如使用雇员ID作为相等的检测标准，并且这个相等的概念适用于所有的子类，就可以使用instanceof进行检测，并应该将Employee.equals方法声明为final。

标准的equals方法实现：

```java
public class Employee
{
    // 1.显示参数命名为otherObject
    // 显示参数类型不能声明成Employee，否则无法覆盖Object类的equals方法，而是定义了一个完全无关的方法
    // 为了避免发生类型错误，可以使用@Override对覆盖超类的方法进行标记
    @Override public boolean equals(Object otherObject)
	{
        // 2.检测this与otherObject是否引用同一个对象，实际上这是一个优化
        if (this == otherObject) return true;
    
        // 3.检测otherObject是否为null
        if (otherObject == null) return false;
    
        // 4.1 比较this与otherObject是否属于同一个类。如果equals的语义在每个子类中都有改变，
        // 就使用getClass检测，也就是上面的相等语义1的处理.
        if (getClass() != otherObject.getClass()) return false;
    
        // or 4.2 如果所有子类都有统一的相等语义，就使用instanceof检测
        // 也就是上面的相等语义2的处理：
        if(!(otherObject instanceof Employee)) return false;
    
        // 5.将otherObject转换为相应的类类型变量：
        Employee other = (Employee) otherObject;
    
        // 6.如果在子类中重新定义equals，就要在其中包含调用super.equals(other)。
        // 并对所有需要比较的域进行比较，基本类型域使用==比较，对象域使用equals比较：
        return Objects.equals(name, other.name)
        	&& salary == other.salary;
    }
}
```

### hashCode方法

hashCode方法定义在Object类中，因此每个对象都有一个默认的散列码，其值为对象的存储地址。

如果重新定义equals方法，就必须重新定义hashCode方法，以便用户可以将对象插入对象散列表中。

hashCode方法应该返回一个整型数值（也可以是负数），并合理地组合实例域的散列码，以便能让各个不同的对象产生的散列码更加均匀，比如:

```java
public int hashCode()
{
    return 7 * Objects.hashCode(name) // Objects.hashCode是null安全的，如果参数为null，返回0
        + 11 * Double.hashCode(salary) //使用Double.hashCode避免创建Double对象
        + 13 * Objects.hashCode(hireDay);
}
```

还有更好的方法，需要组合多个散列值时，可以调用*Objects.hash*并提供多个参数，这个方法会对各个参数调用Objects.hashCode，并组合这些散列值：

```java
public int hashCode()
{
    return Objects.hash(name, salary, hireDay);
}
```

equals和hashCode的定义必须一致：如果x.equals(y)返回true，那么x.hashCode()就必须域y.hashCode()具有相同的值。例如， 如果用定义的 Employee.equals 比较雇员的 ID， 那么 hashCode 方法就需要散列 ID，而不是雇员的姓名或存储址。

> 如果存在数组类型的域，那么可以使用静态的Arrays.hashCode方法计算一个散列码，这个散列码由数组元素的散列码组成。

### toString方法

```java
public String toString()
{
    return getClass().getName() + "[name=" + name + ",id=" + id +
        ", salary=" + salary + "]";
}
```

只要对象与一个字符串通过操作符“+”连接起来，Java编译器就会自动地调用toString方法，以便获得这个对象地字符串描述。如果x是任意一个对象，并调用：*System.out.println(x)*，println方法就会直接地调用x.toString()，并打印输出得到的字符串。

使用静态方法打印数组：

```java
[]int numbers = {1, 4, 5};
String s = Arrays.toString(numbers);
```

> 强烈建议为自定义的每个类增加toString方法。

## 泛型数组列表ArrayList

在C++中，必须在编译时就确定整个数组的大小。在Java中，允许在运行时确定数组的大小：

```java
int actualSize = ...;
Employee[] staff = new Employee[actualSize];
```

上面的数组在确定大小并分配数组空间后，要想改变也不容易，需要我们自己重新分配新的数组空间并拷贝所有元素。

数组列表ArrayList是一个采用类型参数的泛型类（generic class），可以自动地帮助我们管理数组空间的动态分配。

```java
package com.hebostary.corejava;

import java.util.ArrayList;

public class ArrayListTest {
    public static void Test() {
        Common.PrintClassHeader("ArrayList");
        //构造一个空的数组列表
        ArrayList<Employee> staff = new ArrayList<>();
        //等价于ArrayList<Employee> staff = new ArrayList<Employee>();

        //用指定容量构造一个数组列表
        ArrayList<Employee> staff1 = new ArrayList<>(2);
        staff1.add(new Employee("Hunk", 10000));
        staff1.add(new Employee("Bob", 10000));
        System.out.printf("Current address of staff1: 0x%x\n", System.identityHashCode(staff1));
        Common.printAllArrayEleAddr(staff1);

        staff1.add(new Employee("Jack", 10000));
        System.out.printf("Current address of staff1: 0x%x\n", System.identityHashCode(staff1));
        Common.printAllArrayEleAddr(staff1);

        //确保数组列表在不重新分配存储空间的情况下就能够保存给定数量的元素。
        //可能会重新分配存储空间
        staff1.ensureCapacity(10);

        //将数组列表的存储容量削减到当前尺寸。
        //可能会重新分配存储空间，仅在确定数组的大小不会再改变时才使用
        staff1.trimToSize();

//当数组元素的数量超出capacity后，ArrayList会先自动扩容，创建新的底层数组，并拷贝现有数组的所有元素
//实际上底层数组的每个元素都是对对象的引用，因此数组元素引用的实际对象并不会重新分配
        /*
        Current address of staff1: 0xea4a92b
        Current address of index[0]: 0x3c5a99da
        Current address of index[1]: 0x47f37ef1
        Current address of staff1: 0xea4a92b
        Current address of index[0]: 0x3c5a99da
        Current address of index[1]: 0x47f37ef1
        Current address of index[2]: 0x5a01ccaa
        */

        //在指定位置插入元素，后面的所有元素都需要向后移动
        //如果插人新元素后，数组列表的大小超过了容量， 数组列表就会被重新分配存储空间。
        staff1.add(0, new Employee("Grice", 10000));
        //删除指定位置元素，后面的所有元素都需要向前移动
        staff1.remove(0);
    }
}
```

> 对数组列表实施插人和删除元素的操作其效率比较低。如果数组存储的元素数比较多， 又经常需要在中间位置插入、删除元素，就应该考虑使用链表了。

## 对象包装器与自动装箱

有时， 需要将 int 这样的基本类型转换为对象。 所有的基本类型都有一个与之对应的类。例如，Integer 类对应基本类型 int。通常，这些类称为包装器 （ wrapper )。 这些对象包装器类拥有很明显的名字：Integer、Long、Float、Double、Short、Byte、Character 、 Void 和 Boolean (前6 个类派生于公共的超类 Number)。**对象包装器类是不可变的，即一旦构造了包装器，就不允许更改包装在其中的值。**同时， 对象包装器类还是 final , 因此不能定义它们的子类。

 比如，ArrayList尖括号中的类型参数不允许是基本类型：

```java
//ArrayList<int> list = new ArrayList<>(); //NOT OK
ArrayList<Integer> list = new ArrayList<>();
```

> 由于每个值分别包装在对象中，所以ArrayList `<Integer>`的效率远低于int[]数组。因此，应该用它构造小型集合，依次获得操作的方便性，同时对执行性能没有太多的要求。

### 自动装箱autoboxing

举例说明：

```java
//添加元素，下面这个调用将自动变换成：
//list.add(Integer.valueOf(3)); 
//这种变换称为自动装箱（autoboxing）
list.add(3);
```

> 自动装箱规范要求boolean、byte、char <= 127，介于-128 ~ 127之间的 short 和int 被包装到固定的对象中。
>
> ```java
> Integer a = 127;
> Integer b = 127;
> Integer c = 128;
> Integer d = 128;
> System.out.println((a == b) ? "a equals to b." : "a not equals to b."); //a equals to b.
> System.out.println((c == d) ? "c equals to d." : "c not equals to d."); //c not equals to d.
> ```

### 自动拆箱

相反地，当将一个 Integer 对象赋给一个 int 值时， 将会自动地拆箱。

```java
//编译器将下面地语句翻译成：
//int n = list.get(i).intValue();
int n = list.get(i);
```

甚至在算术表达式中也能够自动地装箱和拆箱。例如，可以将自增操作符应用于一个包装器引用：

```java
Integer n = 3;
n++; //编译器将自动地插人一条对象拆箱的指令， 然后进行自增计算， 最后再将结果装箱。
```

> 装箱和拆箱是编译器认可的，而不是虚拟机。编译器在生成类的字节码时，插入必要的方法调用。虚拟机只是执行这些字节码。

另外，包装器类放置了很多通用的基本方法，这些方法基本都是静态的：

```java
int x = Integer.parseInt(s); //将字符串转换成整型，
String s = Integer.toString(x);//以一个新 String 对象的形式返回给定数值 i 的十进制表示。
...
```

## 参数数量可变的方法

以System.out.printf方法为例：

```java
public PrintStream printf(String format, Object ... args) {
    return format(format, args);
}
```

这里的省略号...是Java代码的一部分，它表明这个方法可以接收任意数量的对象（除format参数之外）。

实际上，printf 方法接收两个参数，一个是格式字符串， 另一个是 Object []数组， 其中保存着所有的参数（如果调用者提供的是整型数组或者其他基本类型的值， 自动装箱功能将把它们转换成对象 )。现在将扫描 format字符串， 并将第 i 个格式说明符与 args[i] 的值匹配起来。
换句话说，对于 printf 的实现者来说，Object… 参数类型与 Object[ ] 完全一样。

用户自定义变参函数 :

```java
public static double max(double... values)
{
    double largest = Double.NEGATIVE_INFINITY;
    for (double d : values) {
        if (d > largest) {
            largest = d;
        }
    }
    return largest;
}

//caller
System.out.println(ArrayListTest.max(3.1, 300, -43));
```

## 枚举类

实际上，下面的枚举声明定义的类型是一个类，它刚好有4个实例，在此尽量不要构造新对象。

```java
public enum Size { SMALL, MEDIUM, LARGE };
```

因此， 在比较两个枚举类型的值时， 永远不需要调用 equals, 而直接使用“ = =” 就可以了。
如果需要的话， 可以在枚举类型中添加一些构造器、 方法和域。 当然， 构造器只是在构造枚举常量的时候被调用。

```java
enum Size
{
    //括号里可以存放实例的缩写
    SMALL("S"), MEDIUM("M"), LARGE("L");

    private Size(String abbreviation) {
        this.abbreviation = abbreviation;
    }
    public String getAbbreviation() {
        return abbreviation;
    }

    private String abbreviation;
}
```

所有的枚举类型都是 *Enum* 类的子类，它们继承了这个类的许多方法：

```java
public class EnumTest {
    public static void Test() {
       Common.PrintClassHeader("EnumTest");
       Scanner in = new Scanner(System.in);
       System.out.println("Enter a size: (SMALL, MEDIUM, LARGE)");
       String input = in.next().toUpperCase();//input: small

       //静态方法Enum.valueOf是toString方法的逆方法
       //将size设置成Size.SMALL
       Size size = Enum.valueOf(Size.class, input);

       //自动调用size.toString()，返回枚举常量名
       System.out.println("size=" + size);//size=SMALL
       System.out.println("abbreviation=" + size.getAbbreviation());//S
   
       //每个枚举类型都有一个静态的values方法，返回一个包含全部枚举值的数组
       Size[] values = Size.values();
       System.out.println(Arrays.toString(values));//[SMALL, MEDIUM, LARGE]

       //ordinal方法返回enum声明中枚举常量的位置，位置从0开始计数
       System.out.println(size.ordinal());//0

    }
}
```

## 反射

### Class类

如同Employee/Manager对象表示一个特定的雇员属性一样，一个Class对象将表示一个特定类的属性。**虚拟机为每个类型（包括基础类型）管理一个Class对象，用以存储类的域和方法等属性信息，这些类属性和具体类对象无关**。

下面的代码将展示获取Class对象的3种方法，以及如何通过Class对象的newInstance方法动态创建类对象：

```java
Employee employee = new Employee("Grace", 10000);
Manager manager= new Manager("Hunk", 10000, 5000);

//1.从具体的类对象获得相应的Class对象 
//Object类中的getClass()方法将会返回一个Class类型的实例
Class class1 = employee.getClass();
Class class2 = manager.getClass();

//Class对象最常用的方法是getName()，它将返回类的名字
//如果类在一个包里，包的名字也作为类的一部分
System.out.println(class1.getName() + " " + employee.getName());
//com.hebostary.corejava.Employee Grace
System.out.println(class2.getName() + " " + manager.getName());
//com.hebostary.corejava.Manager Hunk

//2. 调用Class的静态方法forName获得类名对应的Class对象
//只有className是类名或接口时才能执行，否则将会抛出已检查异常（checked exception）
//无论何时使用这个方法，都应该提供异常处理器
String className = "com.hebostary.corejava.Manager";
try {
    Class class3 = Class.forName(className);

    //可以通过Class对象的newInstance()方法动态地创建类实例：
    //newInstance()方法调用默认的构造器初始化新创建的对象
    //如果这个类没有默认的构造器，就会抛出一个异常
    Employee employee2 = (Employee) class3.newInstance();
    System.out.println(employee2.getName());//NoneName

    //虚拟机为每个类型管理一个Class对象，因此，可以利用==运算符
    //比较两个类对象
    System.out.println(class2 == class3);//true
} catch (Exception e) {
    System.out.println(e);
}

//3. T是任意的Java类型（或void关键字），T.class将代表匹配的类对象
//注意，一个Class对象实际上表示的是一个类型，而这个类型不一定是类
//也可以是int这种基本类型
Class class4 = int.class;
Class class5 = Manager.class;
System.out.println(class4.getName());//int
System.out.println(class5.getName());
```

> Class类实际上是一个泛型类：Manager.class的类型是Class `<Manager>`。

### 利用反射分析类

在 java.lang.reflect 包中有三个类 Field、 Method 和 Constructor 分别用于描述类的域、 方法和构造器的详细信息：

1. **名称**：这三个类都有一个叫做 getName 的方法， 用来返回域，方法或构造器的名称。
2. **域的Class对象**：Field类有一个 getType 方法， 用来返回描述域所属类型的 Class 对象。
3. **返回值及参数类型**：Method 和 Constructor 类有能够报告参数类型的getParameterTypes方法， Method 类还有一个可以报告返回类型的getReturnType方法。
4. **访问修饰符**：这 三个类还有一个叫做 getModifiers 的方法， 它将返回一个整型数值， 用不同的位开关描述 public 和 static 这样的修饰符使用状况。另外， 还可以用 java.lang.refleCt 包中的 Modifier类的静态方法分析getModifiers 返回的整型数值。 例如， 可以使用 Modifier 类中的 isPublic、 isPrivate 或 isFinal判断方法或构造器是否是 public、 private 或 final。 我们需要做的全部工作就是调用 Modifier类的相应方法， 并对返回的整型数值进行分析， 另外，还可以利用 Modifier.toString方法将修饰符打印出来。

Class类中的 getFields、 getMethods 和 getConstructors 方 法 将 分 别 返 回 类 提 供 的public 域、 方法和构造器数组， 其中包括超类的公有成员。

Class 类种的 getDeclareFields、getDeclareMethods 和 getDeclaredConstructors 方法将分别返回类中声明的全部域、 方法和构造器， 其中包括私有和受保护成员，但不包括超类的成员。

下面用例子说明这些类的基本用法：

1. 获得对应的Class对象。
2. 通过Class对象调用相应的方法获取域，方法和构造器。

```java
//打印类的所有构造器，方法和域信息
public static void printMetadataOfClass() {
    Scanner in = new Scanner(System.in);
    System.out.println("Enter class name (e.g. com.hebostary.corejava.Manager):");
    String name = in.next();

    try {
        //打印类和超类的名字
        Class cl = Class.forName(name);
        Class superCl = cl.getSuperclass();
        String modifiers = Modifier.toString(cl.getModifiers());
        if (modifiers.length() > 0) System.out.print(modifiers + " ");
        System.out.print("class "+ name);

        if (superCl != null && superCl != Object.class) 
        {
            System.out.print(" extends " + superCl.getName());
        }

        System.out.print("\n{");
        printConstructors(cl);

        System.out.println();
        printMethods(cl);

        System.out.println();
        printFiels(cl);
        System.out.println("\n}");
    } catch (ClassNotFoundException e) {
        e.printStackTrace();
    }
}
```

```java
/**
     * 1.打印一个类的所有构造器方法
    */
public static void printConstructors(Class cl) {
    //获取类中声明的全部构造器，其中包括私有和受保护成员，但不包括超类成员
    Constructor[] constructors = cl.getDeclaredConstructors();

    for (Constructor constructor : constructors) {
        System.out.print("\n    ");
        //打印构造器的访问修饰符
        String modifiers = Modifier.toString(constructor.getModifiers());
        if (modifiers.length() > 0) System.out.print(modifiers + " ");
        System.out.print(constructor.getName() + "(");

        //打印参数类型
        Class[] paramTypes = constructor.getParameterTypes();
        for (int i = 0; i < paramTypes.length; i++) {
            if (i > 0) System.out.print(",");
            System.out.print(paramTypes[i].getName());
        }
        System.out.print(");");
    }
}
```

```java
/**
     * 2.打印一个类的所有方法
    */
public static void printMethods(Class cl) {
    //获取类中声明的全部方法，其中包括私有和受保护成员，但不包括超类成员
    Method[] methods = cl.getDeclaredMethods();

    for (Method method : methods) {
        Class returnType = method.getReturnType();

        System.out.print("\n    ");
        String modifiers = Modifier.toString(method.getModifiers());
        if (modifiers.length() > 0) System.out.print(modifiers + " ");
        System.out.print(returnType.getName() + " " + method.getName() + "(");

        //打印参数类型
        Class[] paramTypes = method.getParameterTypes();
        for (int i = 0; i < paramTypes.length; i++) {
            if (i > 0) System.out.print(",");
            System.out.print(paramTypes[i].getName());
        }
        System.out.print(");");
    }
}
```

```java
/**
     * 3.打印类的所有域
    */
public static void printFiels(Class cl) {
    //获取类中声明的全部域，其中包括私有和受保护成员，但不包括超类成员
    Field[] fields = cl.getDeclaredFields();

    for (Field field : fields) {
        //获取域的Class对象
        Class type = field.getType();
        System.out.print("\n    ");
        String modifiers = Modifier.toString(field.getModifiers());
        if (modifiers.length() > 0) System.out.print(modifiers + " ");
        System.out.print(type.getName() + field.getName() + ";");
    }
}
```

测试printMetadataOfClass函数：

```sh
Enter class name (e.g. com.hebostary.corejava.Manager):
com.hebostary.corejava.Manager
public class com.hebostary.corejava.Manager extends com.hebostary.corejava.Employee
{
    public com.hebostary.corejava.Manager();
    public com.hebostary.corejava.Manager(java.lang.String,double,double);

    public static void Test();
    public void setBonus(double);
    public double getSalary();

    private doublebonus;
}
```

### 利用反射分析对象

上一节中，我们只是利用反射分析了类的基本属性，基本与具体的类对象无关。利用反射机制还可以查看在编译时还不清楚的具体对象的数据域（值）。

 查看对象域的关键方法是 Field类中的 get 方法。 如果 f 是一个 Field 类型的对象（例如，通过 getDeclaredFields 得到的对象)， obj 是某个包含 f 域的类的对象，f.get(obj) 将返回一个对象，其值为 obj 域的当前值。

```java
Employee employee = new Employee("Grace", 10000);
Manager manager= new Manager("Hunk", 10000, 5000);

Class class1 = employee.getClass();
Class class2 = manager.getClass();

try {
    //获取类中指定名字的域（不能获取超类的成员）
    Field field = class1.getDeclaredField("name");

    //反射机制的默认行为受限于 Java 的访问控制。
    //因此需要调用setAccessible方法覆盖访问控制
    //setAccessible 方法是 AccessibleObject 类中的一个方法， 
    //它是 Field、 Method 和 Constructor类的公共超类。
    field.setAccessible(true);
    Object object = field.get(manager);
    System.out.println(object);//Hunk
} catch (Exception e) {
    e.printStackTrace();
}
```

当然，可以获得就可以设置。 调用 f.set(obj ，value) 可以将 obj 对象的 f 域设置成新值。

### 使用反射编写泛型数组代码

Arrays包中的copyOf函数可以用来扩展任意类型的数组，甚至可以在扩展时完成数组元素类型的转换：

```java
public static <T> T[] copyOf(T[] original, int newLength) {
    return (T[]) copyOf(original, newLength, original.getClass());
}

public static <T,U> T[] copyOf(U[] original, int newLength, Class<? extends T[]> newType) {
    @SuppressWarnings("unchecked")
    T[] copy = ((Object)newType == (Object)Object[].class)
        ? (T[]) new Object[newLength]
        : (T[]) Array.newInstance(newType.getComponentType(), newLength);
    System.arraycopy(original, 0, copy, 0,
                     Math.min(original.length, newLength));
    return copy;
}
```

这里的关键在于通过数组的Class对象的getComponentType方法确定数组对应的类型。

### 调用任意方法

类似于C/C++中的方法指针，在Method类中有一个invoke方法，它允许调用包装在当前Method对象中的方法，invoke方法的的签名：

```java
Object invoke(Object obj, Object... args)
```

如果invoke方法执行的是非静态方法，那么它一定需要在某个类对象的基础上操作，因此第一个参数就是隐式参数，而对于静态方法，第一个参数可以被忽略，即可以将它设置为null。

如果在调用方法的时候提供了一个错误的参数，那么 invoke 方法将会抛出一个异常  。

```java
Method ml = Employee.class.getMethod("getName");
Method m2 = Employee.class.getMethod("raiseSalary", double.class);

String n = (String) ml.invoke(harry);

//如果返回类型是基本类型， invoke方法会返回其包装器类型。
//必须相应地完成类型转换。可以使用自动拆箱将它转换为一个 double
double s = (Double) m2.invoke(harry);
```

> invoke 的参数和返回值必须是 Object 类型的。这就意味着必须进行多次的类型转换。这样做将会使编译器错过检查代码的机会。因此， 等到测试阶段才会发现这些错误， 找到并改正它们将会更加困难。不仅如此， 使用反射获得方法指针的代码要比仅仅直接调用方法明显慢一些。

## 继承的设计技巧

下面总结一些对设计继承关系很有帮助的建议：

1. **将公共操作和域放在超类**。
2. **不要使用受保护的（protected）域**。protected 机制并不能够带来更好的封装保护， 其原因主要有两点：（1）子类集合是无限制的， 任何人都能够由某个类派生一个子类，并编写代码以直接访问 protected 的实例域， 从而破坏了封装性。（2）在 Java 中，在同一个包中的所有类都可以访问 proteced 域，而不管它是否为这个类的子类。
3. **使用继承实现“is-a”关系**。比如钟点工的信息包含姓名和雇佣日期，但是没有薪水。他们按小时计薪，并且不会因为拖延时间而获得加薪。 钟点工与雇员之间不属于“ is-a” 关系。钟点工不是特殊的雇员。 因此，直接从Employee派生出Contractor 类然后再增加一个 hourlyWage域并不是一个好的设计。因为这样一来， 每个钟点工对橡中都包含了薪水和计时工资这两个域。在实现打印支票或税单方法耐候， 会带来诞的麻烦， 并且与不采用继承，会多写很制戈码。
4. **除非所有继承的方法都有意义，否则不要使用继承**。
5. **在覆盖方法时，不要改变预期的行为**。
6. **使用多态，而非类型信息**。比如下面的代码，都应该考虑多态性：

   ```java
   if (x is of type 1)
       action1(x);
   else if (x is of type 2)
       action2(x);
   ```

   如果action1和action2表示相同的概念，就应该为它们定义一个方法，并将其放到两个类的超类或接口中，然后简化调用：

   ```java
   x.action();
   ```
7. **不要过多的使用反射**。反射是很脆弱的，意味着编译器很难帮助发现程序中的错误，只有运行时才能发现有些错误并导致异常。

# 接口/lambda表达式/内部类

## 接口

### 接口基本概念

在Java中，接口（interface）不是类，而是主要用来描述类具有什么功能，接口并不给出每个功能的具体实现。一个类可以实现（implement）一个或多个接口，并在需要接口的地方，随时使用实现了相应接口的对象。

比如Arrays类中的sort方法承诺可以对对象数组进行排序，但要求对象所属的类必须实现了Comparable接口：

```java
//JAVA SE 5.0中，这个接口被改进为泛型类型
public interface Comparable<T> {
    public int compareTo(T o);
}
```

任何实现Comparable接口的类都需要包含compareTo方法，并且这个方法的参数必须是一个Object对象，返回一个整型数值。比如下面的Employee类：

```java
public class Employee implements Comparable<Employee>
{
    ...

    //实现Comparable接口的compareTo方法
    public int compareTo(Employee other)
    {
        return Double.compare(salary, other.salary);
    }
```

**为什么不能在 Employee 类直接提供一个 compareTo 方法，而必须实现 Comparable 接口呢？**主要原因在于 Java 程序设计语言是一种强类型（ strongly typed）语言。在调用方法的时候， 编译器将会检查这个方法是否存在。在 sort 方法中可能存在下面这样的语句：

```java
if (a[i].compareTo(a[j]) > 0) { ... }
```

为此， 编译器必须确认 a[i]—定有 compareTo 方法。 如果 a 是一个 Comparable 对象的数组， 就可以确保拥有compareTo 方法，因为每个实现 Comparable 接口的类都必须提供这个方法的定义。

> 语言标 准 规 定： 对 于 任 意 的 x 和 y, 实现必须能够保证 sgn(x.compareTo(y)) = -sgn(y.compareTo(x))。（也就是说， 如 果 y.compareTo(x) 抛出一个异常， 那么 x.compareTo(y)也应该抛出一个异常。这里的“ sgn” 是一个数值的符号：如果n是负值，sgn(n)等于-1 ; 如果n是0, sgn(n)等于0; 如果n是正值，sgn(n)等于1。 简单地讲，如果调换compareTo 的参数，结 果的符号也应该调换。

### 接口的基本特性

1. **接口中的所有方法自动地属于public**。因此，在接口中声明方法时，可以不提供关键字public。但是，在实现接口时， 必须把方法声明为public。否则， 编译器将认为这个方法的访问属性是包可见性， 即类的默认访问属性，之后编译器就会给出试图提供更严格的访问权限的警告信息。
2. **接口可以包含常量（将被自动设为public static final），但绝不能含有实例域**。提供实例域和方法实现的任务应该由实现接口的那个类来完成。Java SE 8以前，也不能在接口中实现方法，新版本允许在接口中增加静态方法。
3. **接口不是类，因此不能用new运算符实例化一个接口对象。但是，我们可以声明接口的变量，接口变量必须引用实现了接口的类对象:**

   ```java
   Comparable x; //OK
   x = new Employee(...);
   ```

   如同使用instanceof检查一个对象是否属于某个特定类一样，也可以用instanceof检查一个对象是否实现了某个特定的接口：

   ```java
   if (anObject instanceof Comparable) { ... }
   ```
4. **类似于类的继承关系，接口也可以被扩展**。即允许存在多条从具有较高通用性的接口到较高专用性的接口的链。

   ```java
   public interface Moveable{
       void move(double x, double y);
   }

   public interface Powered extends Moveable {
       double milesPerGallon();
       double SPEED_LIMIT = 95; //a public static final constant
   }
   ```
5. **每个类只能拥有一个超类，但却可以实现多个接口**，这就为定义类的行为提供了极大的灵活性。 这也是为什么不直接将接口设计为抽象类的原因：每个类只能扩展于一个类。

   ```java
   //如果某个类实现了这个Cloneable接口，Object类中的clone方法就可以创建类对象的一个拷贝。
   class Employee implements Cloneable, Comparable {...}

   abstract class Comparable //why not
   {
       public abstract int compareTo(Object other);
   }
   ```
6. 可以为接口方法提供一个默认实现，即**默认方法**，必须用default修饰符进行标记：

   ```java
   public interface Comparable<T> {
       default int compareTo(T other) {return 0;}
   }
   ```

   这个例子不太实际，因为Comparable的每一个实现都要覆盖这个方法。但是，在有些场景下，接口为自己声明的方法提供默认实现，则允许具体类只实现接口中感兴趣的部分方法。

   默认方法的一个重要用法是**接口演化**。比如新版本为某个接口添加了新的方法，同时提供这个方法的默认实现，就可以保证实现该接口的现有类“源代码兼容”。
7. **解决默认方法冲突。**如果先在一个接口中将一个方法定义为默认方法， 然后又在超类或另一个接口中定义了同样的方法（同名且参数类型相同），会发生什么情况？  规则很简单：（1）超类优先。接口的所有默认方法都会被忽略。（2）接口冲突，实现接口的类必须覆盖方法来解决冲突。

### 比较器接口Comparator

我们可以用Arrays.sort方法对字符串数组进行排序，因为String类实现了Comparable `<String>`接口，而且String.compareTo方法可以按照字典序比较字符串。

现在假设我们希望根据长度对字符串进行排序，而不是字典序。我们无法修改String类的compareTo方法的实现，那如何才能做到呢？要处理这种情况，ArrayS.Sort 方法还有第二个版本， 有一个数组和一个比较器 ( comparator ) 作为参数， 比较器是实现了 Comparator 接口的类的实例。

```java
public interface Comparator<T> {
    int compare(T first, T second);
}
```

要实现按长度比较字符串，可以如下定义一个实现Comparator `<String>`的类：

```java
class lengthComparator implements Comparator<String> {
    public int compare(String first, String second) {
        return first.length() - second.length();
    }
}
```

这个compare方法要在比较器对象上调用，而不是在字符串本身上调用。因此，具体完成比较时，需要建立一个实例（非静态方法）：

```java
Comparator<String> comp = new lengthComparator();
if (comp.compare(word[i], word[j]) > 0) {...}
```

现在，我们可以用lengthComparator来实现字符串数组的长度排序了：

```java
String[] friends = { "Peter", "Paul", "Mary" };
Arrays,sort(friends, new LengthComparatorO):
```

### 对象克隆Cloneable

#### 浅拷贝和深拷贝

浅拷贝只是复制了对象的引用，或者只拷贝了部分实例域（基本类型（int）和不变类型（String）的实例域），而不会拷贝所有子对象，修改其中一个时会影响另一个。

![](https://note.youdao.com/yws/api/personal/file/0D40BECDF98F40D6B230E92993A83C65?method=download&shareKey=340be1fa5a56e497c78e9ffcb5643ae7)

在下面这个典型的浅拷贝例子中，赋值操作仅仅复制了一个Employee对象的引用，所有实例域都是共享的，包括基本类型的域。

```java
Employee employee1 = new Employee("hunk", 6000);

Employee copy = employee1; //严格的说，这个赋值操作并没有执行任何真正的拷贝操作
copy.raiseSalary(10); //修改基本类型的域
copy.setName("hunk.copy");//修改对象域
//com.hebostary.corejava.Employee[name=hunk.copy,id=0, salary=6600.0]
System.out.println(copy); 
//com.hebostary.corejava.Employee[name=hunk.copy,id=0, salary=6600.0]
System.out.println(employee1); 
```

深拷贝则会拷贝所有的实例域（包括实例域中引用的子对象），深拷贝需要通过实现后面介绍的clone方法来实现。

```java
Employee copy = orignal.clone();//深拷贝，前提是Employee类实现了可以完成深拷贝的clone方法
```

浅拷贝会有什么影响吗？ 这要看具体情况。如果原对象和浅克隆对象共享的子对象是不可变的， 那么这种共享就是安全的。如果子对象属于一个不可变的类， 如 String，就是这种情况。或者在对象的生命期中，子对象一直包含不变的常量，没有更改器方法会改变它，也没有方法会生成它的引用，这种情况下同样是安全的。

遗憾的是，通常子对象都是可变的，必须重新定义 clone 方法来建立一个深拷贝， 同时克隆所有子对象。

#### Cloneable接口

Cloneable 接口的出现与接口的正常使用并没有关系。具体来说，它没有指定clone 方法，这个方法是从 Object 类继承的。这个接口只是作为一个标记，指示类设计者了解克隆过程，称为标记接口（tagging interface）。标记接口不包含任何方法，唯一作用就是允许在类型查询中使用instanceof。

**Object 类中 clone 方法声明为 protected ,  重新定义clone方法时，需要指定public访问修饰符**。实际上，即使clone的默认实现能够满足要求，还是需要实现Cloneable接口，将clone重新定义为public，再调用super.clone()。类实现了clone方法就一定可以实现深拷贝吗？继续看下面的例子：

```java
class Employee implements Cloneable { 
    private int id;
    private String name;
    private double salary;
    private Date hireDay;
  
	public Employee clone() throws CloneNotSupportedException {
        Employee cloned = (Employee) super.clone();

        return cloned;
    }
}
```

```java
Employee employee2 = new Employee("jack", 6000);
//为了简化，构造函数里会把hireDay设置为2020年

try {
    Employee copy2 = employee2.clone();
    copy2.raiseSalary(10);
    copy2.setName("hunk.copy2");
    copy2.setHireDay(2050, 5, 24);
    System.out.println(copy2); 
    System.out.println(employee2); 
} catch (CloneNotSupportedException e) {
    //TODO: handle exception
}
```

测试代码的输出：

```shell
com.hebostary.corejava.Employee[name=hunk.copy2,id=1, salary=6600.0, hireDay=Tue May 24 00:00:00 CST 2050]
com.hebostary.corejava.Employee[name=jack,id=1, salary=6000.0, hireDay=Tue May 24 00:00:00 CST 2050]
```

我们发现，如果只调用Object类默认的clone函数，仅仅拷贝了整数类型的实例域（salary和id）和不可变的String类型的实例域（name），而可变对象的实例域（hireDay）仍然被共享，这依然是浅拷贝。

这就说明，如果类中有可变类型的实例域，我们需要依次调用它们的clone方法拷贝子对象：

```java
//如果在一个对象上调用clone，但这个对象的类并没有实现Cloneable接口
//Object类的clone方法就会抛出一个CloneNotSupportedException
//在final类中可以选择捕获这个异常，否则，最好还是如下保留 throws 说明符，
//这样就允许子类在不支持克隆时选择抛出一个 CloneNotSupportedException。
class Employee implements Cloneable {
    public Employee clone() throws CloneNotSupportedException {
        //调用Object.clone()
        Employee cloned = (Employee) super.clone();

        //拷贝可变实例域
        cloned.hireDay = (Date) hireDay.clone();

        return cloned;
    }
}
```

再次执行前面的测试，输出如下：

```shell
com.hebostary.corejava.Employee[name=hunk.copy2,id=1, salary=6600.0, hireDay=Tue May 24 00:00:00 CST 2050]
com.hebostary.corejava.Employee[name=jack,id=1, salary=6000.0, hireDay=Wed Jun 24 00:00:00 CST 2020]
```

这个版本的clone方法实现了真正的深拷贝。

## Lambda表达式

前面为了实现按长度对字符串数组进行排序，我们实现了Comparator接口，其中compare方法的主要工作就是计算下面这个表达式：

```java
first.length() - second.length()
```

compare 方法不是立即调用。 实际上， 在数组完成排序之前， sort 方法会一直调用compare 方法， 只要元素的顺序不正确就会重新排列元素。  这里，为了传递这么一个代码块，必须构造一个对象，这个对象的类需要有一个方法能包含所需的代码块。

Lambda表达式提供了更简洁的传递代码块的方案。Lambda表达式本身就是一个可传递的代码块，以及必须传入代码的变量规范，可以在以后执行一次或多次。

### Lambda表达式的语法

最简单的Lambda表达式写法：

```java
(String first, String second)
   -> first.length() - second.length()
```

Java 是一种强类型语言， 所以我们还要指定参数的类型(String)，但是无需指定 lambda 表达式的返回类型。 lambda 表达式的返回类型总是会由上下文推导得出。 比如上面的表达式可以在需要 int类型结果的上下文中使用。

 如果表达式需要完成更复杂的工作，可以如下编写代码：

```java
(String first, String second) -> 
  {
     if (first.length() < second.length()) return -1;
    else if (first.length() > second.length()) return 1;
    else return 0;
  }
```

即使Lambda表达式没有参数，仍然需要提供括号，就像无参数方法一样：

```java
() -> {for (inti = 100;i >= 0;i ) System.out.println(i);}
```

如果可以推导出一个 lambda 表达式的参数类型，则可以忽略其类型。例如：

```java
//在这里，编译器可以推导出first和 second 必然是字符串，因为这个lambda表达式将赋给一个字符串比较器。
Comparator<String> comp
  = (first, second) // Same as (String first, String second)
    -> first.length() - second.length();
```

如果方法只有一个参数， 而且这个参数的类型可以推导得出，那么甚至还可以省略小括号：

```java
ActionListener listener =  event ->
    System.out.println("The time is " + new Date());
//等价于
ActionListener listener =  (event) ->
    System.out.println("The time is " + new Date());
```

> 如果一个 lambda 表达式只在某些分支返回一个值， 而在另外一些分支不返回值，这是不合法的。 例如，（int x)-> { if (x >= 0) return 1; } 就不合法。

### 函数式接口

Java中已经有很多封装代码块的接口， 如 ActionListener 或Comparator。lambda 表达式与这些接口是兼容的。对于只有一个抽象方法的接口，需要这种接口的对象时，就可以提供一个lambda表达式，这种接口称为**函数式接口**。

> 为什么函数式接口必须有一个抽象方法，不是接口中的所有方法都是抽象的吗？ 实际上， 接口完全有可能重新声明 Object 类的方法， 如 toString 或 clone，这些声明有可能会让方法不再是抽象的。

最好把 lambda 表达式看作是一个函数， 而不是一个对象， 另外要接受 lambda 表达式可以传递到函数式接口。

```java
Arrays.sort(planets, (first, second) -> first.length() - second.length());
```

lambda表达式可以转换为接口，这个特性使得Lambda表达式的应用更加灵活。比如下面的写法：

```java
Timer t = new Timer(1000, event ->
    System.out.println("Time time is " + new Date()));
t.start();

//or

public static void thraderFunc(){
    new Thread(
            () -> {
                System.out.println("1");
            }
    ).start();
}
```

Timer类和Thread类的声明如下：

```java
javax.swing.Timer.Timer(int delay, ActionListener listener)
java.lang.Thread.Thread(Runnable target)
```

实际上，在 Java 中， 对 lambda 表达式所能做的也只是能转换为函数式接口。

### 方法引用

```java
Timer t = new Timer(1000, event -> System.out.println(event)):
//等价于
Timer t = new Timer(1000, Systei.out::println);//直接把println方法传递到Timer构造器

//对字符串排序，而不考虑字母的大小写
Arrays.sort(strings，String::conpareToIgnoreCase)
```

表达式 System.out::println 是一个方法引用（method reference )，它等价于 lambda 表达式x -> System.out.println(x)。

注意，要用::操作符分隔方法名与对象或类名，主要有下面3种情况：

1. object::instanceMethod
2. Class::staticMethod
3. Class::instanceMethod

前2种情况，方法引用等价于提供方法参数的lambda表达式。System.out::println 等价于 x -> System.out.println(x)。 类似地， Math::pow 等价于（x，y) -> Math.pow(x, y)。

对于第3种情况，第1个参数会成为方法的目标。String::conpareToIgnoreCase等同于(x, y) -> x.conpareToIgnoreCase(y)。

> **类似于lambda表达式，方法引用不能独立存在，总是会转换为函数式接口的实例。**

### 构造器引用

### 变量作用域（闭包）

观察下面的例子：

```java
private static void repeatPrint(int delay, String text) {
    for (int i = 0; i < 3; i++) {
        Timer t = new Timer(delay, event -> {
            System.out.println(text);
            //delay--; //ERROR:Local variable delay defined in an enclosing scope must be final or effectively finalJava(536871575)
            System.out.println(delay);

            //System.out.println(i); //ERROR:Local variable i defined in an enclosing scope must be final or effectively finalJava(536871575)
        });

        t.start();
    }
}
```

我们在lmabda表达式中打印了text和delay两个变量，而这两个变量是repeatPrint的方法参数，并不是在lambda表达式中定义的。lambda 表达式的代码可能会在repeatPrint调用返回很久以后才运行， 而那时这些参数变量已经不存在了。 如何保留 text和delay变量呢？

继续理解lambda表达式的3个组成部分：

1. 一个代码块，{ ... }。
2. 参数，( event )。
3. 自由变量的值，这是指非参数而且不在代码中定义的变量。

在上面的代码中，lambda表达式有两个自由变量text和delay。表示lambda表达式的数据结构必须存储自由变量的指，在这里就是text和delay的值，我们说它们被lambda表达式**捕获（captured）**了。

> 关于代码块以及自由变量值有一个术语：闭包（closure）。在Java中，lambda表达式就是闭包。

可以看到， lambda 表达式可以捕获外围作用域中变量的值。 在 Java 中， 要确保所捕获的值是明确定义的，这里有一个重要的限制。**在 lambda 表达式中， 只能引用值不会改变的变量**。  因此，在上面代码中我们试图在表达式中修改delay时，编译器就会报错。这个限制有一定原因，如果在lambda表达式中改变变量，并发执行多个动作时就会不安全。

此外，**如果在 lambda 表达式中引用变量， 而这个变量可能在外部改变，这也是不合法的**。比如上面代码中，我们试图去引用循环变量 i 的值，也会触发编译器错误。

实际上，这里有一条规则：lambda表达式中捕获的变量必须实际上是最终变量（effectively final）。实际上的最终变量是指，这个变量初始化之后就不会再为它赋新值。

lambda 表达式的体与嵌套块有相同的作用域。因此，在 lambda 表达式中声明与一个局部变量同名的参数或局部变量是不合法的。

在一个lambda表达式中使用*this*关键字时，是指创建这个lambda表达式的方法的this参数。在下面的代码中，this.toString()将会调用Application对象的toString方法，而不是ActionListener实例的方法。

```java
public class Application() {
    public void init(){
        ActionListener listener = event ->
        {
            System.out.println(this.toString());
        }
    }
}
```

### 再谈Comparator

Comparator 接口包含很多方便的静态方法来创建比较器。 这些方法可以用于 lambda 表达式或方法引用。
静态 comparing 方法取一个“ 键提取器” 函数， 它将类型 T 映射为一个可比较的类型( 如 String )。 对要比较的对象应用这个函数， 然后对返回的键完成比较。 例如， 假设有一个Person 对象数组，可以如下按名字对这些对象排序：

```java
Arrays.sort(people, Comparator.comparing(Person::getName));
```

可以把比较器与thenComparing方法串起来，如果两个人的姓相同，就会使用第二个比较器。

```java
Arrays.sort(people, Comparator.comparing(Person::getLastName)
           .thenComparing(Person::getFirstName));
```

还可以为comparing和thenComparing方法提取的键指定一个比较器，这里我们就可以再次使用lambda表达式来实现按字符串长度排序：

```java
Arrays.sort(people, Comparator.comparing(Person::getName,
            (s, t) -> Integer.compare(s.length(), t.length())));
```

## 内部类

内部类（inner class）是定义在另一个类中的类，使用内部类的主要原因：

1. 内部类方法可以访问该类定义所在的作用域中的数据，包括私有的数据。内部类既可以访问自身的数据，也可以访问创建它的外围类对象的数据域。 因为内部类的对象总有一个隐式引用，它指向了创建它的外部类对象。
2. 内部类可以对同一个包中的其他类隐藏起来。
3. 当想要定义一个回调函数且不想编写大量代码时，使用匿名内部类比较便捷。

内部类的一些约束：

1. 内部类中声明的所有静态域都必须是final。因为我们希望静态域只有一个实例，而每个外部对象都可能创建单独的内部类实例，如果这个域不是final的，它可能就不是唯一的。
2. 内部类不能有static方法。

内部类是一种编译器现象，与虚拟机无关。编译器会把内部类翻译成用$分割外部类名与内部类名的常规类文件，虚拟机对此一无所知。

### 局部内部类

创建在外部类的某个方法中的内部类，称为局部内部类。它的作用域被限定在声明这个局部类的块中。

局部内部类不能用public或private访问修饰符进行说明，它的作用域被限定在这个局部类的块中。比如这里只能在start函数里使用LocalTimerPrinter。
与其他内部类相比较，局部类不仅能访问包含它们的外部类，还可以访问局部变量。不过，这些局部变量必须事实上为final。

```java
public class InnerClass {
    public static void Test() {
        TestTalkingClock();
    }

    public static void TestTalkingClock() {
        TalkingClock clock = new TalkingClock(1000, true);
        clock.start();

        //弹窗保持程序运行
        JOptionPane.showMessageDialog(null, "Quit program?");
        System.exit(0);
    }
}

class TalkingClock
{
    private int interval;
    private boolean beep;

    /**
     * 构造一个talking clock
    */
    public TalkingClock(int interval, boolean beep) {
        this.interval = interval;
        this.beep = beep;
    }

    public void start() {
        ActionListener listener = new TimerPrinter();
        Timer t = new Timer(interval, listener);
        t.start();

        /*
            局部内部类
        */
        class LocalTimerPrinter implements ActionListener
        {
            //内部类中声明的所有静态域都必须是final
            private static final String message = "LocalTimerPrinter";
  
            public void actionPerformed (ActionEvent event) {
                System.out.println("At the tone, the time is " + new Date());
                //注意，这里的beep是start方法的参数变量，而start方法会提前退出，
                //beep参数变量也随之消失，为了保证局部内部类的访问
                //编译器必须检测对局部变量的访问，为每一个变量建立相应的数据域
                //并将局部变量拷贝到构造器中，以便将这些数据域初始化为局部变量的副本
                if (beep) {
                    System.out.println("Beeping 3... " + message);
                }
            }
        }

        ActionListener listener1 = new LocalTimerPrinter();
        Timer t1 = new Timer(interval, listener1);
        t1.start();
    }
}
```

### 匿名内部类

```java
class TalkingClock
{

    private int interval;
    private boolean beep;

    /**
     * 构造一个talking clock
    */
    public TalkingClock(int interval, boolean beep) {
        this.interval = interval;
        this.beep = beep;
    }

    public void start() {
        /*
            匿名内部类
        */
        //写法的含义：创建一个实现 ActionListener 接口的类的新
        //对象，需要实现的方法 actionPerformed 定义在括号内。
        //由于构造器的名字必须与类名相同，而匿名类没有类名，所以匿名类不能有构造器。
        //相比之下，其实Lambda表达式更有吸引力
        ActionListener listener2 = new ActionListener() {
            private static final String message1 = "AnonymousTimerPrinter";
            public void actionPerformed (ActionEvent event) {
                System.out.println("At the tone, the time is " + new Date());
                if (beep) {
                    System.out.println("Beeping 4... " + message1);
                }
            }
        }; //注意这里的分号

        Timer t2 = new Timer(interval, listener2);
        t2.start();
    }
}
```

### 静态内部类

有时使用内部类只是为了把一个类隐藏在另外一个类的内部，并不需要内部类引用外围类对象。为此，可以将内部类声明为 static, 以便取消产生的引用。只有内部类可以声明为static。

静态内部类的对象除了没有对生成它的外围类对象的引用特权外，与其它所有内部类完全一样。在内部类不需要访问外部类对象的时候，应该使用静态内部类。

```java
public class StaticInnerClass {
    public static void Test() {
        double[] values = new double[20];
        for (int i = 0; i < values.length; i++) {
            values[i] = 100 * Math.random();
        }
        ArrayAlg.Pair p = ArrayAlg.mixmax(values);
        System.out.println(Arrays.toString(values));
        System.out.println("Min: " + p.getFirst());
        System.out.println("Max: " + p.getSecond());
    }
}

class ArrayAlg
{
    /*
        静态内部类
    */
    public static class Pair
    {
        private double first;
        private double second;

        //Pair构造器
        public Pair(double f, double s) {
            first = f;
            second = s;
        }

        public double getFirst() {
            return first;
        }

        public double getSecond() {
            return second;
        }
    }

    //提供一个ArrayAlg的静态方法，返回数组中的最大值和最小值
    public static Pair mixmax(double[] values) {
        double min = Double.POSITIVE_INFINITY;
        double max = Double.NEGATIVE_INFINITY;

        for (double d : values) {
            if (d > max) max = d;
            if (d < min) min = d;
        }

        //将最大值和最小值封装在一个Pair中
        return new Pair(min, max);
    }
}
```

## 代理类

利用代理可以在运行时创建一个实现了一组给定接口的新类 : 这种功能只有在编译时无法确定需要实现哪个接口时才有必要使用。  代理类具有以下方法：

* 指定接口所需要的全部方法。
* Object类中的全部方法，例如toString, equals等。

因为无法在运行时定义这些方法的新代码，需要提供一个调用处理器（invocation handler），调用处理器是实现了InvocationHandler接口的类对象，这个接口中只有一个方法：

```java
public Object invoke(Object proxy, Method method, Object[] args)
        throws Throwable;
```

无论何时调用代理对象的方法，调用处理器的invoke方法都会被调用，并向其传递Method对象和原始的调用参数。调用处理器必须给出处理调用的方式。下面来看例子：

```java
public class ProxyTest {
    public static void Test(){
        Common.PrintClassHeader(new Object(){}.getClass()
            .getEnclosingClass().toString());

        Object[] elements = new Object[1000];

        for (int i = 0; i < elements.length; i++) {
            //1.创建一个整型对象
            Integer value = i + 1;

            //2.创建一个用于处理value对象的调用处理器
            InvocationHandler handler = new TraceHandler(value);
        
            //3.构造实现指定接口的代理类的一个新实例
            //代理对象属于在运行时定义的类（比如 $Proxy0）
            //这个类也实现了Comparable接口
            //但是它的compareTo方法实际上是调用了代理对象处理器（handler）的invoke方法
            Object proxy = Proxy.newProxyInstance(
                null, //类加载器，null表示使用默认的类加载器
                new Class[] { Comparable.class }, //Class对象数组，每个元素都是需要实现的接口 
                handler //调用处理器
            );

            //4.在数组中填充代理对象，
            //所以 compareTo 调用了 TraceHander 类中的 invoke 方法。
            //这个方法打印出了方法名和参数， 
            //之后用包装好的 Integer 对象调用 compareTo
            elements[i] = proxy;
        }

        //创建一个随机整数
        Integer key = new Random().nextInt(elements.length) + 1;

        //对key执行二分查找
        int result = Arrays.binarySearch(elements, key);

        //打印查找结果
        if (result >= 0) System.out.println(result);
    }
}

/**
 * 调用处理器：用于跟踪方法调用，先打印方法名和参数，然后再执行原来的方法
*/
class TraceHandler implements InvocationHandler
{
    private Object targeObject;

    public TraceHandler(Object t)
    {
        targeObject = t;
    }

    public Object invoke(Object proxy, Method m, Object[] args) throws Throwable
    {
        //打印隐式参数
        System.out.print(targeObject);

        //打印方法名字
        System.out.print("." + m.getName() + "(");
  
        //打印显式参数
        if (args != null)
        {
            for (int i = 0; i < args.length; i++) {
                System.out.print(args[i]);
                if (i < args.length - 1) System.out.print(",");
            }
        }
        System.out.println(")");

        //执行实际的方法
        return m.invoke(targeObject, args);
    }
}
```

调用Test()的输出：

```shell
500.compareTo(632)
750.compareTo(632)
625.compareTo(632)
687.compareTo(632)
656.compareTo(632)
640.compareTo(632)
632.compareTo(632)
631
```

> 代理类是在程序运行过程中创建的，它一旦被创建，就变成了常规类，与虚拟机中的任何其它类没有什么区别。
>
> 所有的代理类都扩展于 Proxy 类，一个代理类只有一个实例域—调用处理器，它定义在 Proxy 的超类中。 为了履行代理对象的职责， 所需要的任何附加数据都必须存储在调用处理器中。比如上面的例子，代理 Comparable 对象时，TraceHandler 包装了实际的对象。
>
> 对于特定的类加载器和预设的一组接口来说，只能有一个代理类。如果使用同一个类加载器和接口数组调用两次 newProxylustance 方法的话， 那么只能够得到同一个类的两个对象 。

# 异常处理

# 泛型编程

# 集合

## Java集合框架

[ProcessOn：Java Collection Framwork](https://www.processon.com/view/link/5ed7adfe637689186214f6a1)

### Collection接口

### Iterator迭代器

### Map接口

### Queue接口

## Java库中的具体集合

| 集合类型        | 描述                                                                                          | 底层数据结构       |
| --------------- | --------------------------------------------------------------------------------------------- | ------------------ |
| ArrayList       | 数组列表：一种可以动态增长和缩减的索引序列                                                    |                    |
| LinkedList      | 链接列表：一种可以在任何位置进行高效地插人和删除操作的有序序列，也是实现了Deque接口的双端队列 | 链表               |
| ArrayDeque      | 数组队列：一种用循环数组实现的双端队列                                                        |                    |
| HashSet         | 散列集合：一种没有重复元素的无序集合                                                          | 链表数组（哈希桶） |
| TreeSet         | 树集：—种有序集，元素插入后自动排序                                                          | 红黑树             |
| EnumSet         | 枚举集合：一种包含枚举类型值的集                                                              |                    |
| LinkedHashSet   | 链接散列集合：一种可以记住元素插人次序的散列集合                                              |                    |
| PriorityQueue   | 优先队列：一种允许高效删除最小元素的集合                                                      | 堆                 |
| HashMap         | 散列映射：一种存储键 / 值关联的数据结构                                                       |                    |
| TreeMap         | 树映射：—种键值有序排列的映射表                                                              | 红黑树             |
| EnumMap         | 枚举映射：一种键值属于枚举类型的映射表                                                        |                    |
| LinkedHashMap   | 链接散列映射：一种可以记住键 / 值项添加次序的映射表                                           |                    |
| WeakHashMap     | 弱(引用)散列映射：一种其值无用武之地后可以被垃圾回收器回收的映射表                            |                    |
| IdentityHashMap | 标识散列映射：一种用 == 而不是用 equals 比较键值（即比较内存地址而不是hashCode值）的映射表，  |                    |

## 视图

# Java SE8 流库

流（Stream）提供了一种可以在比集合更高的概念级别上指定计算的数据视图。通过使用流，我们可以说明想要完成什么任务，而不是说明如何去实现它。

流表面上看起来和集合很类似，但是它们之间存在着显著的差异：

1. 流并不存储其元素。这些元素可能是存储在底层的集合中，或者是按需生成的。
2. 流的操作不会修改其数据源。
3. 流的操作是尽可能惰性执行的。这意味着直至需要其结果时，操作才会执行。

流操作的典型流程可以总结为下面3个步骤：

1. 创建一个流。
2. 指定将初始流转换为其它流的中间操作，可能包含多个步骤。
3. 应用终止操作，从而产生结果。这个操作会强制执行之前的惰性操作，从此之后，这个流就再也不能用了。

## 流的创建

常见的用于创建流的操作：

1. 用Collection接口的stream方法将任何集合转换为一个流。
2. Stream.of：将数组转换为一个流。
3. Stream.empty：方法创建不包含任何元素的流。
4. Arrays.stream：由数组中指定范围的元素产生一个流。
5. Stream.generate：不断调用参数函数产生无限流。
6. Stream.iterate：从种子值迭代产生无限流。
7. 还可以从文件Files和正则表达式Pattern产生流，用法类似。

下面给出部分创建流操作的例子：

```java
String[] strings = {"a","bb","ccc","dddd","eeeee","ffffff", "hhhhhhh"};

//从数组创建流
Stream<String> words = Stream.of(strings);
show("words", words);
//从数组指定范围元素创建流
Stream<String> subWords = Arrays.stream(strings, 2, 5);
show("subWords", subWords);
Stream<String> song = Stream.of("gently", "down", "the", "stream");
show("song", song);

//创建一个空的流
Stream<String> silence = Stream.empty();
show("silence", silence);

//generate函数产生一个无限流，它的元素是通过不断调用传入的参数函数生成的
Stream<String> echos = Stream.generate(() -> { return "Echo"; });
show("echos", echos);

Stream<Double> randoms = Stream.generate(Math::random);
show("randoms", randoms);

//iterate函数也是产生无限流，它的元素包含一个种子seed（BigInteger.ONE），以及不断调用f(seed)，
//然后继续调用f(f(seed))，从而可以产生无限元素
Stream<BigInteger> integers = Stream.iterate(BigInteger.ONE, n -> n.add(BigInteger.ONE));
show("integers", integers); 
```

show函数：

```java
private static <T> void show(String title, Stream<T> stream) {
  final int LIMIT_SIZE = 10;
  List<T> firstElements = stream
    .limit(LIMIT_SIZE + 1)
    .collect(Collectors.toList());

  System.out.print(title + ":");
  for (int i = 0; i < firstElements.size(); i++) {
    if (i > 0) System.out.print(",");
    if (i < LIMIT_SIZE) System.out.print(firstElements.get(i));
    else System.out.print("...");
  }

  System.out.println();
}
```

我们也可以通过创建流来惰性处理大文件的所有行：

```java
try(Stream<String> lines = Files.lines(path, charset))
{
  ...
}
```

## 流的转换

本节介绍的流转换操作都是在原有流的元素基础上，通过过滤、排序等操作产生一个新的流。

### 流的filter/map/flatMap操作

```java
//过滤长度大于3的元素，生成一个新的流
Stream<String> filterWords = Stream.of(strings).filter(w -> w.length() > 3);
show("filterWords", filterWords);//filterWords:dddd,eeeee,ffffff,hhhhhhh

//将所有元素从小写转换成大写，生成一个新的流
Stream<String> upperWords = Stream.of(strings).map(String::toUpperCase);
show("upperWords", upperWords);//upperWords:A,BB,CCC,DDDD,EEEEE,FFFFFF,HHHHHHH

//对words流的每个元素调用letters方法都产生了一个流，
//因此最终的结果mapWords时一个流中流，也就是每个元素都是一个流
Stream<Stream<String>> mapWords = Stream.of(strings).map(w -> letters(w));
show("mapWords", mapWords);
//而flatMap方法可以把这种流中流摊平合并成一个结果流
Stream<String> flatMapWords = Stream.of(strings).flatMap(w -> letters(w));
show("flatMapWords", flatMapWords);//flatMapWords:a,b,b,c,c,c,d,d,d,d,...
```

### 抽取子流和连接流

stream.limit(n)：返回一个新的流，它在n个元素之后结束。这个方法对于裁剪无限流的尺寸会显得特别有用。

stream.skip(n)：与limit方法相反，skip方法是丢弃前n个元素。

Stream.contact(stream1, stream2)：将两个流连接起来，但是第一个流不应该是无限的，否则第二个流永远都不会被处理。

### 流的去重

stream.distinct()：返回一个流，元素从原有流中产生，重复元素被剔除。

### 流的排序

stream.sorted()：产生一个新流，元素按照顺序排序

### 并发流

```java
Stream<String> words = Stream.of(strings);
words = words.parallel();
```

### 流的调试

stream.peek(func)：peek方法会产生另一个流，它的元素与原来流中的元素相同，但是在每次获取一个元素时，都会调用一个函数func。这对于调试非常有帮助。

比如：

```java
Stream<String> subWords = Arrays.stream(strings, 2, 5).peek(w -> System.out.println("Fetching: " + w));
```

## 流的约简

约简是一种终结操作（terminal operation），它们会将流约简为可以在程序中使用的非流值，或者说得到从数据流中得到答案。

stream.count()：返回流中元素的数量。

stream.max()：返回最大值。

stream.min()：返回最小值。

stream.findFirst()：返回非空集合中的第一个值，通常与filter组合使用，比如获得第一个以Q开头的元素：

```java
Optional<String> startWithQ = words.filter(s -> s.startWith("Q")).findFirst();
```

类似的操作还有anyMatch，findAny等，可以在具体使用时查看手册。

这些方法返回的是一个类型Optional `<T>`的值，Optional `<T>`对象是一种包装器对象，它要么在其中包装了类型T的对象，要么没有包装任何对象，表示没有任何值（流为空）。

## Optional类型

### 如何正确的使用Optional值

```java
//没有任何匹配时，使用默认值
String result = optionalString.orElse("");

//没有任何值时抛出异常
String result = optionalString.orElseThrow(IllegalStateException::new);

//可选值存在则传递给参数函数处理，否则，不会发生任何事情
optionalValue.ifPresent(v -> results.add(v));
//或者直接写成
optionalValue.ifPresent(results::add);
//或者，需要处理返回值
Optional<Boolean> added = optionalValue.map(results::add);
```

### 创建Optional值

```java
//obj不为null的情况下返回Optional.of(obj)，否则返回Optional.empty()
Optional.ofNullable(obj);
```

## 收集结果

当处理完流之后，通常会想要查看其元素，可以将结果收集到指定的数据结构中，比如数组，集合以及映射。

### 收集到数组集合

```java
public class CollectingStream {
    public static void Test() {
        Common.PrintClassHeader(new Object(){}.getClass().getEnclosingClass().toString());
        String[] strings = {"a","bb","ccc","dddd","eeeee","ffffff", "hhhhhhh"};

        Stream<String> words = Stream.of(strings);
        //将流的结果收集到一个集合中并打印
        //Collectors类提供了很多用于生成公共收集器的工厂方法
        show("words", words.collect(Collectors.toSet()));//words:java.util.HashSet

        words = Stream.of(strings);
        //将流的结果保存到指定的类型集合中
        show("words", words.collect(Collectors.toCollection(TreeSet::new)));//words:java.util.TreeSet

        //将流结果收集到列表
        words = Stream.of(strings);
        List<String> list = words.collect(Collectors.toList());
        System.out.println(list);

        //将流结果收集到数组
        //注意：因为无法在运行时创建泛型数组，因此toArray()返回的是Object[]数组
        //想要得到正确类型的数组，传入指定的数组构造器
        words = Stream.of(strings);
        String[] array = words.toArray(String[]::new);
        System.out.println(array);

        //从流生成一个普通的迭代器
        Iterator<Integer> iter = Stream.iterate(0, n -> n + 1).limit(10).iterator();
        while (iter.hasNext()) {
            System.out.print(iter.next());
        }
        System.out.println();
    }

    private static <T> void show(String label, Set<T> set) {
        System.out.println(label + ":" + set.getClass().getName());
        //打印集合元素时，通过流处理来加上分隔符
        System.out.println("[" + set.stream().limit(10)
            .map(Object::toString).collect(Collectors.joining(",")) + "]");
    }
}
```

### 收集到映射

实际上，我们还可以将流元素收集到映射表甚至并发映射表中，更灵活的用法可以在使用时再研究。

```java
private static void collectToMap() {
  Common.PrintMethodHeader("collectToMap");
  Stream<Locale> locales = Stream.of(Locale.getAvailableLocales());
  //默认Locale中的名字为键，而其本地化的名字为值
  //这里我们不关心同一种语言是否会出现两次，比如很多国家都用英语，因此只记录第一项
  Map<String, String> localesMap = locales.collect(Collectors.toMap(Locale::getDisplayLanguage, 
                                                                    l -> l.getDisplayLanguage(l), (existingValue, newValue) -> existingValue));
  System.out.println(localesMap.get("Chinese"));
}
```

### 群组/分区以及下游收集器

```java
private static void grouping() {
  Common.PrintMethodHeader("grouping");
  Stream<Locale> locales = Stream.of(Locale.getAvailableLocales());
  //通过群组方法收集给定国家的语言，并输出到映射表中
  //Locale::getCountry是群组的分类函数
  Map<String, List<Locale>> countryToLocales = locales.collect(
    Collectors.groupingBy(Locale::getCountry));
  System.out.println(countryToLocales.get("CH")); //[gsw_CH, de_CH, pt_CH, fr_CH, rm_CH, it_CH, wae_CH, en_CH]

  //groupingBy会产生一个映射表，它的每个值都是一个列表
  //如果想要一某种方式处理这些列表，就需要提供“下游处理器”
  //Collectors包提供了很多“下游收集器”方法来获取不同类型的映射表结果
  //群组分类的值是集合而不是默认的列表
  locales = Stream.of(Locale.getAvailableLocales());
  Map<String, Set<Locale>> countryToLocalesSet = locales.collect(
    Collectors.groupingBy(Locale::getCountry, Collectors.toSet()));
  System.out.println(countryToLocalesSet.get("CH")); 

  //群组分类的值是计数
  locales = Stream.of(Locale.getAvailableLocales());
  Map<String, Long> countryToLocalesCount = locales.collect(
    Collectors.groupingBy(Locale::getCountry, Collectors.counting()));
  System.out.println(countryToLocalesCount.get("CH")); //8
}
```

## 约简操作

reduce方法是一种用于从流中计算某个值的通用机制。通常，如果reduce方法有一项约简操作op，那么该约简操作就会产生v0 *op* v1 *op* v2 *op* ..。这个操作op应该是可结合的：即组合元素时使用的顺序不应该成为问题，这使得在使用并行流时，可以执行高效的约简。来看下面的例子：

```java
private static void reduce()
{
  Common.PrintMethodHeader("reduce");
  //计算所有元素的和，参数函数称作累积器（accumulator）
  Stream<Integer> iStream = Stream.iterate(0, n -> n + 1).limit(10);
  Optional<Integer> sum = iStream.reduce((x, y) -> x + y);
  System.out.println(sum.orElse(999));//45等价于将流中的所有元素相加

  //计算所有字符串的长度总和
  //reduce的第一个参数函数叫累积器（accumulator），这个函数会被反复调用，产生累积的总和
  //第二个参数函数叫组合器（combiner），因为并行计算时，会有多个累积器的运算结果，需要通过组合器来合并结果
  String[] words = {"a","bb","ccc","dddd","eeeee","ffffff", "hhhhhhh"};
  int result = Stream.of(words).reduce(0, 
                                       (total, w) -> total + w.length(), 
                                       (total1, total2) -> total1 + total2);
  System.out.println("Total length: " + result);//28
  //对于这个特例，使用下面的方法更加高效，因为不涉及装箱操作
  //stream.mapToInt返回一个IntStream，它的元素就是将后面的参数函数应用到原有流的每个元素所得到的结果
  result = Stream.of(words).mapToInt(String::length).sum();
  System.out.println("Total length: " + result);//28
}
```

## 基本类型流

前面介绍的各种流操作中，大多是将整数收集到Stream `<Integer>`中。实际上，将每个整数都包装到包装器对象中是很低效的。流库中有专门的类型IntStream，LongStream，DoubleStream，用来直接存储基本类型值，而无需使用包装器：

1. IntStream可以用来存储short，char，byte和boolean。
2. DoubleStream可以用来存储double。

构造基本类型流：

```java
IntStream stream = IntStream.of(1,2,3,4);
stream = Arrays.stream(values, from, to);//values是一个int[]类型的数组
```

当有一个对象流时，可以用mapToInt，mapToLong和mapToDouble将其转换为基本类型流。比如：

```java
IntStream stream = Stream.of(words).mapToInt(String::length);
```

也可以使用boxed方法将基本类型流转换成对象流：

```java
//range生成步长为1的整数范围
Stream<Integer> integers = IntStream.range(0, 100).boxed();
```

## 并行流

先通过简单的例子了解并行流的基本用法：

```java
public class ParallelStream {
    public static void Test() {
        Common.PrintClassHeader(new Object(){}.getClass().getEnclosingClass().toString());
        String[] strings = {"a","bb","ccc","dddd","eeee","ffffff", "hhhhhhh"};
        //先根据元素长度进行群组，再收集到映射表，这个并行流操作是安全的
        Map<Integer, Long> result = Stream.of(strings).parallel()
            .filter(w -> w.length() < 10)
            .collect(Collectors.groupingBy(String::length, Collectors.counting()));
        System.out.println(result);

        //groupingByConcurrent使用了共享的并发映射表
        Map<Integer, List<String>> rMap = Stream.of(strings).parallel()
            .filter(w -> w.length() < 10)
            .collect(Collectors.groupingByConcurrent(String::length));
        System.out.println(rMap);
    }
}
```

为了让并行流正常工作，需要满足下面条件（这也是我们计划使用并行流时应该考虑的问题）：

1. 数据应该在内存中。必须等到数据到达是非常低效的。
2. 流应该可以被高效地分成若干个子部分。由数组或平衡二叉树支撑的流都可以工作得很好，但是Stream.iterate返回的结果不行。
3. 流操作的工作量应该具有较大的规模。如果总工作负载不是很大，那么搭建并行计算时所付出的代价就没有什么意义。
4. 流操作不应该被阻塞。并行流使用fork-join池来操作流的各个部分，如果多个流操作被阻塞，那么池可能就无法做任何事情了。

> 不要修改在执行某项流操作后会将元素返回到流中的集合（即使这种修改是线程安全的）。记住，流并不会收集它们的数据，数据总是在单独的集合中。如果修改了这样的集合，那么流操作的结果就是未定义的。比如下面的错误代码：
>
> ```java
> Stream<String> words = wordList.stream();
> words.forEach(s -> if(s.length() < 12) wordList.remove(s));
> ```

# Java IO
