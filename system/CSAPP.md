# C0 写在前面

这篇笔记的结构主要以《深入理解计算机系统第三版》作为主线，笔记内容也主要来自这本书及CMU的官方网站（链接参考文末），作为扩展，也参考了《UNIX环境高级编程第三版》和网络上的一些资料。

这篇笔记主要聚焦计算机体系结构和操作系统底层相关的知识点：
* C程序编译、链接的基本原理。
* 程序运行的基本原理，包括加载、栈帧、程序返回和跳转，以及进程、线程的基本概念等。
* 虚拟内存的基本原理。

# C7 链接
## Linux C程序的编译链接
```c
//main.c
int buf[2] = {1, 2};
   
int main()
{
   swap();
   return 0;
}
```

```c
//swap.c
extern int buf[]; 
 
int *bufp0 = &buf[0];
static int *bufp1;

void swap()
{
  int temp;

  bufp1 = &buf[1];
  temp = *bufp0;
  *bufp0 = *bufp1;
  *bufp1 = temp;
}
```


我们调用下面的gcc命令来编译这两个源文件：
```c
gcc -Og -o prog main.c swap.c -v
```
gcc实际上执行了下面几个动作：
```c
//0.预处理，处理头文件包含、宏定义替换等编译准备工作
//1.编译：调用c编译器cc1将main.c和swap.c翻译成ASCII汇编语言文件main.s和swap.s。
//必要时还会有C预处理器cpp先将.c文件翻译成ASCII码的中间文件.i。
/usr/libexec/gcc/x86_64-redhat-linux/4.8.5/cc1 -quiet -v main.c -quiet -dumpbase main.c 
-mtune=generic -march=x86-64 -auxbase main -Og -version -o /tmp/ccJSty5f.s

//2.汇编：调用汇编器as将.s文件翻译成可重定向目标文件（relocatable object file）。
as -v --64 -o /tmp/ccGUs9ue.o /tmp/ccJSty5f.s

//3.链接：调用链接器collect2将所有可重定位目标文件链接成可执行目标文件。
//collect2实际上是Linux静态链接程序ld的封装。
/usr/libexec/gcc/x86_64-redhat-linux/4.8.5/collect2 --build-id --no-add-needed 
--eh-frame-hdr --hash-style=gnu -m elf_x86_64 -dynamic-linker /lib64/ld-linux-x86-64.so.2 
-o prog /usr/lib/gcc/x86_64-redhat-linux/4.8.5/../../../../lib64/crt1.o /usr/lib/gcc/
x86_64-redhat-linux/4.8.5/../../../../lib64/crti.o /usr/lib/gcc/x86_64-redhat-linux/4.8.5/
crtbegin.o -L/usr/lib/gcc/x86_64-redhat-linux/4.8.5 -L/usr/lib/gcc/x86_64-redhat-linux/
4.8.5/../../../../lib64 -L/lib/../lib64 -L/usr/lib/../lib64 -L/usr/lib/gcc/
x86_64-redhat-linux/4.8.5/../../.. /tmp/ccGUs9ue.o /tmp/cc4u02Wc.o -lgcc --as-needed 
-lgcc_s --no-as-needed -lc -lgcc --as-needed -lgcc_s --no-as-needed /usr/lib/gcc/
x86_64-redhat-linux/4.8.5/crtend.o /usr/lib/gcc/x86_64-redhat-linux/4.8.5/../../../../
lib64/crtn.o
```

我们也可以手动地调用这些程序来依次编译链接（collect2参数过多，当我们执行gcc -o prog main.o swap.o时，gcc也会自动调用collect2完成链接）。
下面先看一下编译器的输出main.s，汇编器会根据.s中的符号（包括函数main、swap和变量buf）来构造符号表：

```c
[root@hunk c7]# cat main.s 
        .file   "main.i"
        .text
        .globl  main
        .type   main, @function
main:
.LFB0:
        .cfi_startproc
        subq    $8, %rsp
        .cfi_def_cfa_offset 16
        movl    $0, %eax
        call    swap
        movl    $0, %eax
        addq    $8, %rsp
        .cfi_def_cfa_offset 8
        ret
        .cfi_endproc
.LFE0:
        .size   main, .-main
        .globl  buf
        .data
        .align 4
        .type   buf, @object
        .size   buf, 8
buf:
        .long   1
        .long   2
        .ident  "GCC: (GNU) 4.8.5 20150623 (Red Hat 4.8.5-16)"
        .section        .note.GNU-stack,"",@progbits
```

这里我们讨论的重点是（静态）链接器，链接器负责将一组可重定位目标文件生成一个完全链接的、可以加载和运行的可执行目标文件。可重定位目标文件由各种不同的代码和数据节（section）组成，每一节都是一个连续的字节序列。为了构造可执行文件，链接器主要做两件事情：
* 符号解析(symbol resolution)。目标文件定义和引用符号（symbol），每个符号对应于一个函数、一个全局变量或者一个静态变量（C语言中任何以static属性声明的变量）。符号解析的目的是将每个符号引用和一个符号定义关联起来。
* 重定位(relocation)。编译器和汇编器生成从地址0开始的代码和数据节。链接器通过把每个符号定义与一个内存位置关联起来，从而重定位这些节，然后修改所有对这些符号的引用，使得它们指向这个内存位置。链接器使用汇编器产生的重定位条目（relocation entry）的详细指令，不加甄别地执行这样的重定位。

## 目标文件ELF
目标文件（模块）有3种形式：
* 可重定位目标文件。包含二进制代码和数据，其形式可以在链接时与其它可重定位目标文件结合起来，创建一个可执行目标文件。单个.o可重定位目标文件经常被叫做模块。
* 可执行目标文件。包含二进制代码和数据，并有着固定的虚拟内存结构布局，其形式可以直接被复制到内存运行。
* 共享目标文件。一种特殊类型的可重定位目标文件，可以在加载或者运行时被动态地加载进内存并链接。

目标文件是按照特定的目标文件格式组织的，各个系统都不太一样，现代x86-64 Linux和Unix系统使用的是可执行可链接格式（executable and linkable format，ELF）。

ELF文件的基本格式：

![ELF格式](https://note.youdao.com/yws/api/personal/file/10FBEC087B7B4249BBFB77F8F2FB3733?method=download&shareKey=340be1fa5a56e497c78e9ffcb5643ae7)


## 可重定位目标文件
我们先单独执行编译和汇编得到两个可重定向文件main.o和swap.o，下面用readelf命令读取这两个可重定位目标文件的详细信息：
```c
[root@hunk c7]# readelf -a main.o 
//ELF头部
ELF Header:
  Magic:   7f 45 4c 46 02 01 01 00 00 00 00 00 00 00 00 00 
  Class:                             ELF64
  Data:                              2's complement, little endian
  Version:                           1 (current)
  OS/ABI:                            UNIX - System V
  ABI Version:                       0
  Type:                              REL (Relocatable file) //可重定位目标文件
  Machine:                           Advanced Micro Devices X86-64
  Version:                           0x1
  Entry point address:               0x0
  Start of program headers:          0 (bytes into file)
  Start of section headers:          624 (bytes into file) //节头部表的文件偏移
  Flags:                             0x0
  Size of this header:               64 (bytes)
  Size of program headers:           0 (bytes)
  Number of program headers:         0
  Size of section headers:           64 (bytes) //节头部表中条目的大小
  Number of section headers:         12 //节头部表中条目的数目
  Section header string table index: 9

//节头部表，描述了不同节的位置和大小
Section Headers:
  [Nr] Name              Type             Address           Offset
       Size              EntSize          Flags  Link  Info  Align
  [ 0]                   NULL             0000000000000000  00000000
       0000000000000000  0000000000000000           0     0     0
  [ 1] .text             PROGBITS         0000000000000000  00000040 //已编译程序的机器代码
       0000000000000018  0000000000000000  AX       0     0     1
  [ 2] .rela.text        RELA             0000000000000000  00000240
       0000000000000018  0000000000000018   I      10     1     8
  [ 3] .data             PROGBITS         0000000000000000  00000058 //已经初始化的全局和静态C变量
       0000000000000008  0000000000000000  WA       0     0     4
  [ 4] .bss              NOBITS           0000000000000000  00000060 //未初始化的全局和静态C变量
       0000000000000000  0000000000000000  WA       0     0     1
  [ 5] .comment          PROGBITS         0000000000000000  00000060
       000000000000002e  0000000000000001  MS       0     0     1
  [ 6] .note.GNU-stack   PROGBITS         0000000000000000  0000008e
       0000000000000000  0000000000000000           0     0     1
  [ 7] .eh_frame         PROGBITS         0000000000000000  00000090
       0000000000000030  0000000000000000   A       0     0     8
  [ 8] .rela.eh_frame    RELA             0000000000000000  00000258
       0000000000000018  0000000000000018   I      10     7     8
  [ 9] .shstrtab         STRTAB           0000000000000000  000000c0
       0000000000000059  0000000000000000           0     0     1
  [10] .symtab           SYMTAB           0000000000000000  00000120 //符号表
       0000000000000108  0000000000000018          11     8     8
  [11] .strtab           STRTAB           0000000000000000  00000228
       0000000000000016  0000000000000000           0     0     1
Key to Flags:
  W (write), A (alloc), X (execute), M (merge), S (strings), l (large)
  I (info), L (link order), G (group), T (TLS), E (exclude), x (unknown)
  O (extra OS processing required) o (OS specific), p (processor specific)

There are no section groups in this file.

There are no program headers in this file.

//当汇编器生成一个目标模块时，它并不知道数据和代码最终将放在内存中的什么位置，
//它也不知道这个模块引用的任何外部定义的函数或者全局变量的位置。所以：
//汇编器遇到对最终位置未知的目标引用，就会生成一个重定位条目（relocation entry），
//告诉链接器在将目标文件合并成可执行文件时如何修改这个引用：
//  1.代码的重定位条目就放在这个叫.rela.text的section中，这里可以看到关于swap函数的重定位条目。
//  2.已初始化数据的重定位条目就放在叫.rela.data的section中，在后面swap.o中可以看到buf的重定位条目
Relocation section '.rela.text' at offset 0x240 contains 1 entries:
  Offset          Info           Type           Sym. Value    Sym. Name + Addend
00000000000a  000900000002 R_X86_64_PC32     0000000000000000 swap - 4

Relocation section '.rela.eh_frame' at offset 0x258 contains 1 entries:
  Offset          Info           Type           Sym. Value    Sym. Name + Addend
000000000020  000200000002 R_X86_64_PC32     0000000000000000 .text + 0

The decoding of unwind sections for machine type Advanced Micro Devices X86-64 is not currently supported.

//符号表各字段含义：
//  name：字符串表中的字节偏移，指向符号的以null结尾的字符串名字
//  Value: 符号地址
//  Size： 目标大小（字节）
//  Type: 通常要么是数据，要么是函数
//  Bind：表示符号是本地的还是全局的
//  Ndx：表示符号在哪个数据节，通过节头部表来索引，readelf用整数索引来标识每个节，
//      UDN(EF)代表未定义的符号，也就是在本模块引用。但是在其它地方定义的模块，比如swap函数。
//      Ndx=1表示.text节，Ndx=3表示.data节
Symbol table '.symtab' contains 11 entries:
   Num:    Value          Size Type    Bind   Vis      Ndx Name
     0: 0000000000000000     0 NOTYPE  LOCAL  DEFAULT  UND 
     1: 0000000000000000     0 FILE    LOCAL  DEFAULT  ABS main.i //ABS代表不该被重定位的符号
     2: 0000000000000000     0 SECTION LOCAL  DEFAULT    1 
     3: 0000000000000000     0 SECTION LOCAL  DEFAULT    3 
     4: 0000000000000000     0 SECTION LOCAL  DEFAULT    4 
     5: 0000000000000000     0 SECTION LOCAL  DEFAULT    6 
     6: 0000000000000000     0 SECTION LOCAL  DEFAULT    7 
     7: 0000000000000000     0 SECTION LOCAL  DEFAULT    5 
     8: 0000000000000000    24 FUNC    GLOBAL DEFAULT    1 main //.text节偏移量为0的24字节函数
     9: 0000000000000000     0 NOTYPE  GLOBAL DEFAULT  UND swap
    10: 0000000000000000     8 OBJECT  GLOBAL DEFAULT    3 buf //.data节偏移量为0处的8字节目标

No version information found in this file.
```


```c
[root@hunk c7]# readelf -a swap.o 
ELF Header:
  Magic:   7f 45 4c 46 02 01 01 00 00 00 00 00 00 00 00 00 
  Class:                             ELF64
  Data:                              2's complement, little endian
  Version:                           1 (current)
  OS/ABI:                            UNIX - System V
  ABI Version:                       0
  Type:                              REL (Relocatable file)
  Machine:                           Advanced Micro Devices X86-64
  Version:                           0x1
  Entry point address:               0x0
  Start of program headers:          0 (bytes into file)
  Start of section headers:          792 (bytes into file)
  Flags:                             0x0
  Size of this header:               64 (bytes)
  Size of program headers:           0 (bytes)
  Number of program headers:         0
  Size of section headers:           64 (bytes)
  Number of section headers:         13
  Section header string table index: 10

Section Headers:
  [Nr] Name              Type             Address           Offset
       Size              EntSize          Flags  Link  Info  Align
  [ 0]                   NULL             0000000000000000  00000000
       0000000000000000  0000000000000000           0     0     0
  [ 1] .text             PROGBITS         0000000000000000  00000040
       0000000000000023  0000000000000000  AX       0     0     1
  [ 2] .rela.text        RELA             0000000000000000  00000270
       0000000000000078  0000000000000018   I      11     1     8
  [ 3] .data             PROGBITS         0000000000000000  00000068
       0000000000000008  0000000000000000  WA       0     0     8
  [ 4] .rela.data        RELA             0000000000000000  000002e8
       0000000000000018  0000000000000018   I      11     3     8
  [ 5] .bss              NOBITS           0000000000000000  00000070
       0000000000000008  0000000000000000  WA       0     0     8
  [ 6] .comment          PROGBITS         0000000000000000  00000070
       000000000000002e  0000000000000001  MS       0     0     1
  [ 7] .note.GNU-stack   PROGBITS         0000000000000000  0000009e
       0000000000000000  0000000000000000           0     0     1
  [ 8] .eh_frame         PROGBITS         0000000000000000  000000a0
       0000000000000030  0000000000000000   A       0     0     8
  [ 9] .rela.eh_frame    RELA             0000000000000000  00000300
       0000000000000018  0000000000000018   I      11     8     8
  [10] .shstrtab         STRTAB           0000000000000000  000000d0
       000000000000005e  0000000000000000           0     0     1
  [11] .symtab           SYMTAB           0000000000000000  00000130
       0000000000000120  0000000000000018          12     9     8
  [12] .strtab           STRTAB           0000000000000000  00000250
       000000000000001d  0000000000000000           0     0     1

Key to Flags:
  W (write), A (alloc), X (execute), M (merge), S (strings), l (large)
  I (info), L (link order), G (group), T (TLS), E (exclude), x (unknown)
  O (extra OS processing required) o (OS specific), p (processor specific)

There are no section groups in this file.

There are no program headers in this file.

Relocation section '.rela.text' at offset 0x270 contains 5 entries:
  Offset          Info           Type           Sym. Value    Sym. Name + Addend
000000000003  000400000002 R_X86_64_PC32     0000000000000000 .bss - 8
000000000007  000a0000000b R_X86_64_32S      0000000000000000 buf + 4
00000000000e  000b00000002 R_X86_64_PC32     0000000000000000 bufp0 - 4
000000000016  000a00000002 R_X86_64_PC32     0000000000000000 buf + 0
00000000001e  000a00000002 R_X86_64_PC32     0000000000000000 buf + 0

Relocation section '.rela.data' at offset 0x2e8 contains 1 entries:
  Offset          Info           Type           Sym. Value    Sym. Name + Addend
000000000000  000a00000001 R_X86_64_64       0000000000000000 buf + 0

Relocation section '.rela.eh_frame' at offset 0x300 contains 1 entries:
  Offset          Info           Type           Sym. Value    Sym. Name + Addend
000000000020  000200000002 R_X86_64_PC32     0000000000000000 .text + 0

The decoding of unwind sections for machine type Advanced Micro Devices X86-64 is not currently supported.

Symbol table '.symtab' contains 12 entries:
   Num:    Value          Size Type    Bind   Vis      Ndx Name
     0: 0000000000000000     0 NOTYPE  LOCAL  DEFAULT  UND 
     1: 0000000000000000     0 FILE    LOCAL  DEFAULT  ABS swap.i
     2: 0000000000000000     0 SECTION LOCAL  DEFAULT    1 
     3: 0000000000000000     0 SECTION LOCAL  DEFAULT    3 
     4: 0000000000000000     0 SECTION LOCAL  DEFAULT    5 
     5: 0000000000000000     8 OBJECT  LOCAL  DEFAULT    5 bufp1
     6: 0000000000000000     0 SECTION LOCAL  DEFAULT    7 
     7: 0000000000000000     0 SECTION LOCAL  DEFAULT    8 
     8: 0000000000000000     0 SECTION LOCAL  DEFAULT    6 
     9: 0000000000000000    35 FUNC    GLOBAL DEFAULT    1 swap
    10: 0000000000000000     0 NOTYPE  GLOBAL DEFAULT  UND buf
    11: 0000000000000000     8 OBJECT  GLOBAL DEFAULT    3 bufp0

No version information found in this file.
```

## 符号和符号解析
如我们在前面看到的，每个可重定位目标模块m都有一个符号表（.symtab section包含ELF符号表），它包含m定义和引用的符号的信息。在链接器的上下文中，有三种不同的符号：
* 由模块m定义并能被其它模块引用的全局符号，包括非静态的C函数和全局变量。
* 由其它模块定义并被模块m引用的全局符号。这些符号称为外部符号，对应于在其它模块定义的非静态C函数的全局变量（比如swap.c中的buf，在.symtab中buf的Ndx=UND(DEF)，因为它定义在模块main.o中）。
* 只被模块m定义和引用的本地局部符号，包括带static属性的C函数和全局变量（比如swap.c中的static变量bufp1）。这些符号在模块中的任何位置都可见，但不能被其它模块引用。

Notes
>* .symtab中的符号表不包含对应于本地非静态程序变量的任何符号（比如swap.c中的temp变量），这些符号在运行时在栈中被管理，链接器对它们不感兴趣。
>* 符号表是由汇编器基于编译器输出到汇编语言.s文件中的符号构造的。


链接器解析符号引用的方法是将每个引用与它输入的可重定位目标文件中的符号表中的一个确定的符号定义关联起来。当编译器遇到一个不在当前模块定义的符号时，会假设该符号是在其它某个模块中定义的，生成一个链接器符号表条目（观察main.o中.symtab中关于swap函数的条目），并把它交给链接器处理。如果链接器在它的任何输入模块中都找不到这个被引用符号的定义，就会报错。

![符号解析](https://note.youdao.com/yws/api/personal/file/41550823C95F47A0A39A9F16C99B9349?method=download&shareKey=340be1fa5a56e497c78e9ffcb5643ae7)

## 重定位
链接器完成符号解析后，就把代码中的每个符号引用和正好一个符号定义（即它的一个输入目标模块中的一个符号表条目）关联起来。并且链接器知道了它的输入目标模块中的代码节和数据节的确切大小，现在就可以开始重定位了，重定位步骤将合并输入模块，并为每个符号分配运行时地址。重定位分两个步骤：
* 重定位节和符号定义。链接器将所有相同类型的节合并为同一类型的聚合节（比如.data），然后将运行时内存地址赋给新的聚合节，赋给输入模块定义的每个节以及每个符号。这一步完成时，程序中的每条指令和全局变量都有唯一的运行时内存地址了。
* 重定位节中的符号引用。链接器修改代码节和数据节中对每个符号的引用，使得它们指向正确地运行时地址。链接器的这一步依赖于可重定位目标模块中称为重定位条目的数据结构（在前面我们用readelf解析main.o和swap.o的时候可以看到）。

![重定位](https://note.youdao.com/yws/api/personal/file/54AF907415D644D79BCED355857FA6D5?method=download&shareKey=340be1fa5a56e497c78e9ffcb5643ae7)

## 可执行目标文件
可执行目标文件包含程序加载到内存并运行它所需的所有信息：
* ELF头部包含了程序的入口点（entry point），也就是程序运行时执行的第一条指令的地址。
* 可执行目标文件中的.text、.rodata和.data节和可重定位目标文件中的节是类似的，只是这些节已经被重定位到它们最终运行时内存地址。
* .init节定义了一个_init函数，程序的初始化代码会调用它。
* ELF可执行文件被设计得很容易加载到内存。可执行文件的连续的片（chunk）被映射到连续的内存段。程序头部表（program header table）描述了这种映射关系。

因为可执行文件时完全链接的（已重定位），所以它不再需要.rel节这些重定位信息。我们用readelf来分析一个典型的ELF可执行文件的格式：

```c
[root@hunk c7]# readelf -a prog 
ELF Header:
  Magic:   7f 45 4c 46 02 01 01 00 00 00 00 00 00 00 00 00 
  Class:                             ELF64
  Data:                              2's complement, little endian
  Version:                           1 (current)
  OS/ABI:                            UNIX - System V
  ABI Version:                       0
  Type:                              EXEC (Executable file) //可执行ELF
  Machine:                           Advanced Micro Devices X86-64
  Version:                           0x1
  Entry point address:               0x400400 //程序入口点，指向_start函数。
  Start of program headers:          64 (bytes into file)
  Start of section headers:          6752 (bytes into file)
  Flags:                             0x0
  Size of this header:               64 (bytes)
  Size of program headers:           56 (bytes)
  Number of program headers:         9
  Size of section headers:           64 (bytes)
  Number of section headers:         30
  Section header string table index: 27

Section Headers:
  [Nr] Name              Type             Address           Offset
       Size              EntSize          Flags  Link  Info  Align
  [ 0]                   NULL             0000000000000000  00000000
       0000000000000000  0000000000000000           0     0     0
  [ 1] .interp           PROGBITS         0000000000400238  00000238 //包含动态链接器的路径名
       000000000000001c  0000000000000000   A       0     0     1
  [ 2] .note.ABI-tag     NOTE             0000000000400254  00000254
       0000000000000020  0000000000000000   A       0     0     4
  [ 3] .note.gnu.build-i NOTE             0000000000400274  00000274
       0000000000000024  0000000000000000   A       0     0     4
  [ 4] .gnu.hash         GNU_HASH         0000000000400298  00000298
       000000000000001c  0000000000000000   A       5     0     8
  [ 5] .dynsym           DYNSYM           00000000004002b8  000002b8
       0000000000000048  0000000000000018   A       6     1     8
  [ 6] .dynstr           STRTAB           0000000000400300  00000300
       0000000000000038  0000000000000000   A       0     0     1
  [ 7] .gnu.version      VERSYM           0000000000400338  00000338
       0000000000000006  0000000000000002   A       5     0     2
  [ 8] .gnu.version_r    VERNEED          0000000000400340  00000340
       0000000000000020  0000000000000000   A       6     1     8
  [ 9] .rela.dyn         RELA             0000000000400360  00000360
       0000000000000018  0000000000000018   A       5     0     8
  [10] .rela.plt         RELA             0000000000400378  00000378
       0000000000000030  0000000000000018  AI       5    12     8
  [11] .init             PROGBITS         00000000004003a8  000003a8
       000000000000001a  0000000000000000  AX       0     0     4
  [12] .plt              PROGBITS         00000000004003d0  000003d0
       0000000000000030  0000000000000010  AX       0     0     16
  [13] .text             PROGBITS         0000000000400400  00000400
       00000000000001a2  0000000000000000  AX       0     0     16
  [14] .fini             PROGBITS         00000000004005a4  000005a4
       0000000000000009  0000000000000000  AX       0     0     4
  [15] .rodata           PROGBITS         00000000004005b0  000005b0
       0000000000000010  0000000000000000   A       0     0     8
  [16] .eh_frame_hdr     PROGBITS         00000000004005c0  000005c0
       000000000000003c  0000000000000000   A       0     0     4
  [17] .eh_frame         PROGBITS         0000000000400600  00000600
       0000000000000104  0000000000000000   A       0     0     8
  [18] .init_array       INIT_ARRAY       0000000000600e10  00000e10
       0000000000000008  0000000000000000  WA       0     0     8
  [19] .fini_array       FINI_ARRAY       0000000000600e18  00000e18
       0000000000000008  0000000000000000  WA       0     0     8
  [20] .jcr              PROGBITS         0000000000600e20  00000e20
       0000000000000008  0000000000000000  WA       0     0     8
  [21] .dynamic          DYNAMIC          0000000000600e28  00000e28
       00000000000001d0  0000000000000010  WA       6     0     8
  [22] .got              PROGBITS         0000000000600ff8  00000ff8
       0000000000000008  0000000000000008  WA       0     0     8
  [23] .got.plt          PROGBITS         0000000000601000  00001000
       0000000000000028  0000000000000008  WA       0     0     8
  [24] .data             PROGBITS         0000000000601028  00001028
       0000000000000018  0000000000000000  WA       0     0     8
  [25] .bss              NOBITS           0000000000601040  00001040
       0000000000000010  0000000000000000  WA       0     0     8
  [26] .comment          PROGBITS         0000000000000000  00001040
       000000000000005a  0000000000000001  MS       0     0     1
  [27] .shstrtab         STRTAB           0000000000000000  0000109a
       0000000000000108  0000000000000000           0     0     1
  [28] .symtab           SYMTAB           0000000000000000  000011a8
       0000000000000678  0000000000000018          29    47     8
  [29] .strtab           STRTAB           0000000000000000  00001820
       0000000000000240  0000000000000000           0     0     1
Key to Flags:
  W (write), A (alloc), X (execute), M (merge), S (strings), l (large)
  I (info), L (link order), G (group), T (TLS), E (exclude), x (unknown)
  O (extra OS processing required) o (OS specific), p (processor specific)

There are no section groups in this file.

//程序头部表
Program Headers:
  Type           Offset             VirtAddr           PhysAddr
                 FileSiz            MemSiz              Flags  Align
  PHDR           0x0000000000000040 0x0000000000400040 0x0000000000400040
                 0x00000000000001f8 0x00000000000001f8  R E    8
  INTERP         0x0000000000000238 0x0000000000400238 0x0000000000400238
                 0x000000000000001c 0x000000000000001c  R      1
      [Requesting program interpreter: /lib64/ld-linux-x86-64.so.2]
  LOAD           0x0000000000000000 0x0000000000400000 0x0000000000400000
                 0x0000000000000704 0x0000000000000704  R E    200000
  LOAD           0x0000000000000e10 0x0000000000600e10 0x0000000000600e10
                 0x0000000000000230 0x0000000000000240  RW     200000
  DYNAMIC        0x0000000000000e28 0x0000000000600e28 0x0000000000600e28
                 0x00000000000001d0 0x00000000000001d0  RW     8
  NOTE           0x0000000000000254 0x0000000000400254 0x0000000000400254
                 0x0000000000000044 0x0000000000000044  R      4
  GNU_EH_FRAME   0x00000000000005c0 0x00000000004005c0 0x00000000004005c0
                 0x000000000000003c 0x000000000000003c  R      4
  GNU_STACK      0x0000000000000000 0x0000000000000000 0x0000000000000000
                 0x0000000000000000 0x0000000000000000  RW     10
  GNU_RELRO      0x0000000000000e10 0x0000000000600e10 0x0000000000600e10
                 0x00000000000001f0 0x00000000000001f0  R      1

 Section to Segment mapping:
  Segment Sections...
   00     
   01     .interp 
   02     .interp .note.ABI-tag .note.gnu.build-id .gnu.hash .dynsym .dynstr .gnu.version .gnu.version_r .rela.dyn .rela.plt .init .plt .text .fini .rodata .eh_frame_hdr .eh_frame 
   03     .init_array .fini_array .jcr .dynamic .got .got.plt .data .bss 
   04     .dynamic 
   05     .note.ABI-tag .note.gnu.build-id 
   06     .eh_frame_hdr 
   07     
   08     .init_array .fini_array .jcr .dynamic .got 

Dynamic section at offset 0xe28 contains 24 entries:
  Tag        Type                         Name/Value
 0x0000000000000001 (NEEDED)             Shared library: [libc.so.6]
 0x000000000000000c (INIT)               0x4003a8
 0x000000000000000d (FINI)               0x4005a4
 0x0000000000000019 (INIT_ARRAY)         0x600e10
 0x000000000000001b (INIT_ARRAYSZ)       8 (bytes)
 0x000000000000001a (FINI_ARRAY)         0x600e18
 0x000000000000001c (FINI_ARRAYSZ)       8 (bytes)
 0x000000006ffffef5 (GNU_HASH)           0x400298
 0x0000000000000005 (STRTAB)             0x400300
 0x0000000000000006 (SYMTAB)             0x4002b8
 0x000000000000000a (STRSZ)              56 (bytes)
 0x000000000000000b (SYMENT)             24 (bytes)
 0x0000000000000015 (DEBUG)              0x0
 0x0000000000000003 (PLTGOT)             0x601000
 0x0000000000000002 (PLTRELSZ)           48 (bytes)
 0x0000000000000014 (PLTREL)             RELA
 0x0000000000000017 (JMPREL)             0x400378
 0x0000000000000007 (RELA)               0x400360
 0x0000000000000008 (RELASZ)             24 (bytes)
 0x0000000000000009 (RELAENT)            24 (bytes)
 0x000000006ffffffe (VERNEED)            0x400340
 0x000000006fffffff (VERNEEDNUM)         1
 0x000000006ffffff0 (VERSYM)             0x400338
 0x0000000000000000 (NULL)               0x0

Relocation section '.rela.dyn' at offset 0x360 contains 1 entries:
  Offset          Info           Type           Sym. Value    Sym. Name + Addend
000000600ff8  000200000006 R_X86_64_GLOB_DAT 0000000000000000 __gmon_start__ + 0

Relocation section '.rela.plt' at offset 0x378 contains 2 entries:
  Offset          Info           Type           Sym. Value    Sym. Name + Addend
000000601018  000100000007 R_X86_64_JUMP_SLO 0000000000000000 __libc_start_main + 0
000000601020  000200000007 R_X86_64_JUMP_SLO 0000000000000000 __gmon_start__ + 0

The decoding of unwind sections for machine type Advanced Micro Devices X86-64 is not currently supported.

Symbol table '.dynsym' contains 3 entries:
   Num:    Value          Size Type    Bind   Vis      Ndx Name
     0: 0000000000000000     0 NOTYPE  LOCAL  DEFAULT  UND 
     1: 0000000000000000     0 FUNC    GLOBAL DEFAULT  UND __libc_start_main@GLIBC_2.2.5 (2)
     2: 0000000000000000     0 NOTYPE  WEAK   DEFAULT  UND __gmon_start__

// 对于可执行目标文件，Value符号地址是一个绝对运行时地址。
Symbol table '.symtab' contains 69 entries:
   Num:    Value          Size Type    Bind   Vis      Ndx Name
     0: 0000000000000000     0 NOTYPE  LOCAL  DEFAULT  UND 
     1: 0000000000400238     0 SECTION LOCAL  DEFAULT    1 
     2: 0000000000400254     0 SECTION LOCAL  DEFAULT    2 
     3: 0000000000400274     0 SECTION LOCAL  DEFAULT    3 
     4: 0000000000400298     0 SECTION LOCAL  DEFAULT    4 
     5: 00000000004002b8     0 SECTION LOCAL  DEFAULT    5 
     6: 0000000000400300     0 SECTION LOCAL  DEFAULT    6 
     7: 0000000000400338     0 SECTION LOCAL  DEFAULT    7 
     8: 0000000000400340     0 SECTION LOCAL  DEFAULT    8 
     9: 0000000000400360     0 SECTION LOCAL  DEFAULT    9 
    10: 0000000000400378     0 SECTION LOCAL  DEFAULT   10 
    11: 00000000004003a8     0 SECTION LOCAL  DEFAULT   11 
    12: 00000000004003d0     0 SECTION LOCAL  DEFAULT   12 
    13: 0000000000400400     0 SECTION LOCAL  DEFAULT   13 
    14: 00000000004005a4     0 SECTION LOCAL  DEFAULT   14 
    15: 00000000004005b0     0 SECTION LOCAL  DEFAULT   15 
    16: 00000000004005c0     0 SECTION LOCAL  DEFAULT   16 
    17: 0000000000400600     0 SECTION LOCAL  DEFAULT   17 
    18: 0000000000600e10     0 SECTION LOCAL  DEFAULT   18 
    19: 0000000000600e18     0 SECTION LOCAL  DEFAULT   19 
    20: 0000000000600e20     0 SECTION LOCAL  DEFAULT   20 
    21: 0000000000600e28     0 SECTION LOCAL  DEFAULT   21 
    22: 0000000000600ff8     0 SECTION LOCAL  DEFAULT   22 
    23: 0000000000601000     0 SECTION LOCAL  DEFAULT   23 
    24: 0000000000601028     0 SECTION LOCAL  DEFAULT   24 
    25: 0000000000601040     0 SECTION LOCAL  DEFAULT   25 
    26: 0000000000000000     0 SECTION LOCAL  DEFAULT   26 
    27: 0000000000000000     0 FILE    LOCAL  DEFAULT  ABS crtstuff.c
    28: 0000000000600e20     0 OBJECT  LOCAL  DEFAULT   20 __JCR_LIST__
    29: 0000000000400430     0 FUNC    LOCAL  DEFAULT   13 deregister_tm_clones
    30: 0000000000400460     0 FUNC    LOCAL  DEFAULT   13 register_tm_clones
    31: 00000000004004a0     0 FUNC    LOCAL  DEFAULT   13 __do_global_dtors_aux
    32: 0000000000601040     1 OBJECT  LOCAL  DEFAULT   25 completed.6354
    33: 0000000000600e18     0 OBJECT  LOCAL  DEFAULT   19 __do_global_dtors_aux_fin
    34: 00000000004004c0     0 FUNC    LOCAL  DEFAULT   13 frame_dummy
    35: 0000000000600e10     0 OBJECT  LOCAL  DEFAULT   18 __frame_dummy_init_array_
    36: 0000000000000000     0 FILE    LOCAL  DEFAULT  ABS main.c
    37: 0000000000000000     0 FILE    LOCAL  DEFAULT  ABS swap.c
    38: 0000000000601048     8 OBJECT  LOCAL  DEFAULT   25 bufp1
    39: 0000000000000000     0 FILE    LOCAL  DEFAULT  ABS crtstuff.c
    40: 0000000000400700     0 OBJECT  LOCAL  DEFAULT   17 __FRAME_END__
    41: 0000000000600e20     0 OBJECT  LOCAL  DEFAULT   20 __JCR_END__
    42: 0000000000000000     0 FILE    LOCAL  DEFAULT  ABS 
    43: 0000000000600e18     0 NOTYPE  LOCAL  DEFAULT   18 __init_array_end
    44: 0000000000600e28     0 OBJECT  LOCAL  DEFAULT   21 _DYNAMIC
    45: 0000000000600e10     0 NOTYPE  LOCAL  DEFAULT   18 __init_array_start
    46: 0000000000601000     0 OBJECT  LOCAL  DEFAULT   23 _GLOBAL_OFFSET_TABLE_
    47: 00000000004005a0     2 FUNC    GLOBAL DEFAULT   13 __libc_csu_fini
    48: 0000000000000000     0 NOTYPE  WEAK   DEFAULT  UND _ITM_deregisterTMCloneTab
    49: 0000000000601028     0 NOTYPE  WEAK   DEFAULT   24 data_start
    50: 0000000000601040     0 NOTYPE  GLOBAL DEFAULT   24 _edata
    51: 00000000004005a4     0 FUNC    GLOBAL DEFAULT   14 _fini
    52: 0000000000000000     0 FUNC    GLOBAL DEFAULT  UND __libc_start_main@@GLIBC_
    53: 0000000000601028     0 NOTYPE  GLOBAL DEFAULT   24 __data_start
    54: 0000000000601038     8 OBJECT  GLOBAL DEFAULT   24 bufp0
    55: 0000000000000000     0 NOTYPE  WEAK   DEFAULT  UND __gmon_start__
    56: 00000000004005b8     0 OBJECT  GLOBAL HIDDEN    15 __dso_handle
    57: 00000000004005b0     4 OBJECT  GLOBAL DEFAULT   15 _IO_stdin_used
    58: 0000000000400530   101 FUNC    GLOBAL DEFAULT   13 __libc_csu_init
    59: 0000000000601050     0 NOTYPE  GLOBAL DEFAULT   25 _end
    60: 0000000000400400     0 FUNC    GLOBAL DEFAULT   13 _start
    61: 000000000060102c     8 OBJECT  GLOBAL DEFAULT   24 buf
    62: 0000000000601040     0 NOTYPE  GLOBAL DEFAULT   25 __bss_start
    63: 00000000004004ed    24 FUNC    GLOBAL DEFAULT   13 main
    64: 0000000000000000     0 NOTYPE  WEAK   DEFAULT  UND _Jv_RegisterClasses
    65: 0000000000601040     0 OBJECT  GLOBAL HIDDEN    24 __TMC_END__
    66: 0000000000000000     0 NOTYPE  WEAK   DEFAULT  UND _ITM_registerTMCloneTable
    67: 0000000000400505    35 FUNC    GLOBAL DEFAULT   13 swap
    68: 00000000004003a8     0 FUNC    GLOBAL DEFAULT   11 _init

Version symbols section '.gnu.version' contains 3 entries:
 Addr: 0000000000400338  Offset: 0x000338  Link: 5 (.dynsym)
  000:   0 (*local*)       2 (GLIBC_2.2.5)   0 (*local*)    

Version needs section '.gnu.version_r' contains 1 entries:
 Addr: 0x0000000000400340  Offset: 0x000340  Link: 6 (.dynstr)
  000000: Version: 1  File: libc.so.6  Cnt: 1
  0x0010:   Name: GLIBC_2.2.5  Flags: none  Version: 2

Displaying notes found at file offset 0x00000254 with length 0x00000020:
  Owner                 Data size       Description
  GNU                  0x00000010       NT_GNU_ABI_TAG (ABI version tag)
    OS: Linux, ABI: 2.6.32

Displaying notes found at file offset 0x00000274 with length 0x00000024:
  Owner                 Data size       Description
  GNU                  0x00000014       NT_GNU_BUILD_ID (unique build ID bitstring)
    Build ID: 9198c4e6d32faf4ac71dcd52bc45879a0838db23
```


我们用objdump这个工具来看prog的program header似乎更方便一点：
```c
[root@hunk c7]# objdump -x prog 
...
Program Header:
         目标文件中偏移               虚拟内存地址               物理内存地址               对齐要求
    PHDR off    0x0000000000000040 vaddr 0x0000000000400040 paddr 0x0000000000400040 align 2**3
         目标文件中的段大小           内存中的段大小             运行时访问权限
         filesz 0x00000000000001f8 memsz 0x00000000000001f8 flags r-x
  INTERP off    0x0000000000000238 vaddr 0x0000000000400238 paddr 0x0000000000400238 align 2**0
         filesz 0x000000000000001c memsz 0x000000000000001c flags r--
    LOAD off    0x0000000000000000 vaddr 0x0000000000400000 paddr 0x0000000000400000 align 2**21
         filesz 0x0000000000000704 memsz 0x0000000000000704 flags r-x
    LOAD off    0x0000000000000e10 vaddr 0x0000000000600e10 paddr 0x0000000000600e10 align 2**21
         filesz 0x0000000000000230 memsz 0x0000000000000240 flags rw-
 DYNAMIC off    0x0000000000000e28 vaddr 0x0000000000600e28 paddr 0x0000000000600e28 align 2**3
         filesz 0x00000000000001d0 memsz 0x00000000000001d0 flags rw-
    NOTE off    0x0000000000000254 vaddr 0x0000000000400254 paddr 0x0000000000400254 align 2**2
         filesz 0x0000000000000044 memsz 0x0000000000000044 flags r--
EH_FRAME off    0x00000000000005c0 vaddr 0x00000000004005c0 paddr 0x00000000004005c0 align 2**2
         filesz 0x000000000000003c memsz 0x000000000000003c flags r--
   STACK off    0x0000000000000000 vaddr 0x0000000000000000 paddr 0x0000000000000000 align 2**4
         filesz 0x0000000000000000 memsz 0x0000000000000000 flags rw-
   RELRO off    0x0000000000000e10 vaddr 0x0000000000600e10 paddr 0x0000000000600e10 align 2**0
         filesz 0x00000000000001f0 memsz 0x00000000000001f0 flags r--
...
```
以LOAD开始的两个条目意味着可执行目标文件初始化两个内存段：
* 第一个LOAD条目代表着代码段有读/执行权限，开始于内存地址0x400000处，总的内存大小为0x704字节，并且被初始化为可执行目标文件的头0x704个字节（从0x0开始），其中包含了ELF头、程序头部表以及.init、.text和.rodata节。
* 第二个LOAD条目代表着第二个段（数据段）有读写访问权限，开始于内存地址0x600e10，总的内存大小为0x240字节，并用从目标文件中偏移0xe10处开始的.data中的0x230个字节初始化。该段中剩下的字节对应于运行时将被初始化为0的.bss数据。


## 加载可执行目标文件
```c
[root@hunk c7]# ./prog 
```
当我们在shell里运行上面的命令时，因为prog不是内置的shell命令，所以shell会把它作为执行可执行目标文件时处理：
* 复制进程后在子进程中通过execve函数来调用某个驻留在存储器中的称为加载器（loader）的操作系统代码来运行它。
* 加载器删除子进程现有的虚拟内存段，并创建一组新的代码、数据、堆和栈段，新的栈和堆段都被初始化为0。根据ELF程序头部表，通过将虚拟地址空间的页映射到可执行文件的页大小的片（chunk），新的代码和数据段都被初始化为可执行文件的内容（根据后面虚拟内存的知识，在内核维护的进程页表中，最开始虚拟地址映射的物理地址其实是磁盘上的地址，直到CPU引用一个被映射的虚拟页时才会进行页复制，操作系统利用它的页面调度机制自动将页面从磁盘传回内存）。
* 然后通过跳转到ELF头部里的程序入口点（entry point，指向_start函数）来运行程序。
* _start函数在系统目标文件ctrl.o中定义，对所有的C程序都一样。_start函数调用系统启动函数__libc_start_main（定义在libc.so中）。它初始化执行环境，调用用户层的main函数，处理main函数的返回值，并在需要的时候把控制返回给内核。

![加载可执行目标文件](https://note.youdao.com/yws/api/personal/file/06D55B784EED482993D2F6BB5E573368?method=download&shareKey=340be1fa5a56e497c78e9ffcb5643ae7)


## 动态链接共享库
共享库（shared library）是一个目标模块，所以也叫共享目标（shared object），在运行或加载时可以加载到任意的内存位置，并和一个在内存中的程序链接起来。这个过程称为动态链接（dynamic linking），由动态链接器（dynamic linker）程序执行，动态链接器本身也是一个共享目标（比如Linux上的ld-linux.so）。

比如下面的例子：

![动态链接](https://note.youdao.com/yws/api/personal/file/DC3EAA4E9E8C4AA48ACD6B90A1BDC8B5?method=download&shareKey=340be1fa5a56e497c78e9ffcb5643ae7)

基本的思路是当创建可执行文件时，静态执行一些链接，然后在程序加载时，由动态链接器执行重定位来动态完成链接过程。此时，没有任何libvector.so的代码和数据节真的被复制到可执行文件中。反之，链接器复制了一些重定位和符号表信息，它们使得运行时可以解析对libvector.so中代码和数据的引用。

> 我们可以通过 `cat /proc/<pid>/maps` 或者 `pmap <pid>` 来查看某个进程的实际虚拟内存布局。


# C8 异常控制流
## 系统调用
## 进程
### 进程的基本概念
进程的经典定义就是一个执行中程序的实例，系统中的每个程序都运行在某个进程的上下文环境（context）中。上下文是由程序正确运行所需的状态组成的。这个状态包括存放在内存中的程序的代码段和数据，它的栈、通用目的寄存器的内容、程序计数器、环境变量以及打开文件描述符的集合。
每次用户通过向shell输入一个可执行目标文件的名字，运行程序时，shell就会创建一个新的进程，然后通过调用execve函数在这个新进程的上下文中运行这个可执行目标文件。进程提供给应用程序的关键抽象：
> * 一个独立的逻辑控制流，它提供一个假象，好像我们的程序独占地使用处理器。
实际上，进程是轮流使用处理器的。每个进程执行它的流的一部分，然后被抢占（暂时挂起），然后轮到其它进程，对于一个运行在这些进程之一的上下文中的程序，它看上去就是在独占的使用处理器。
> * 一个私有的地址空间，它提供一个假象，好像我们的程序独占地使用内存系统。尽管和每个私有地址空间相关联的内存的内容是不一样的，但是每个这样的空间都有相同的通用结构，具体参考[Linux虚拟内存系统](#linux虚拟内存系统)。

### 进程状态
参考： [Linux进程状态解析](https://blog.csdn.net/wukery/article/details/79296129)

# C9 虚拟内存
计算机系统的主存被组织成一个由M个连续的字节大小的单元组成的数组，每字节都有一个唯一的物理地址（Physical Address, PA）。CPU使用这个物理地址访问内存的方式称为物理寻址。
早期的PC使用物理寻址，数字信号处理器、嵌入式微控制器等系统仍然继续使用这种寻址方式。而现代处理器则使用下图所展示的虚拟寻址方式访问内存，使用虚拟寻址，CPU通过生成一个虚拟地址（Virtual Address, VA）来访问主存，这个地址被送到内存之前先被MMU（Memory Management Unit, MMU）翻译成适当的物理地址。

![虚拟寻址](https://note.youdao.com/yws/api/personal/file/2B1CCFF472DA4573A6F75C27FD42882A?method=download&shareKey=340be1fa5a56e497c78e9ffcb5643ae7)

支持虚拟寻址方式的则是虚拟内存系统，它是处理器、MMU、主存等硬件和操作系统相互配合的一个系统，并不是系统中单独的某个硬件或者软件。

## 地址空间
地址空间就是非负整数地址的有序集合。比如主存的所有物理地址的集合就是物理地址空间（Physical Adress Space, PAS）。在包含虚拟内存的系统中，CPU从一个由N=2^n个地址的地址空间中生成虚拟地址，这个地址空间称为虚拟地址空间(（Virtual Adress Space, VAS）)。
主存中的每个字节都有一个选自虚拟地址空间的虚拟地址和选自物理地址空间的物理地址。

## 虚拟内存
### SRAM和DRAM
对于CPU而言，DRAM的访问速度大概比SRAM慢10倍，而磁盘的访问速度又比DRAM慢大概10000倍。我们一般说的主存是指DRAM，而SRAM用于缓存DRAM中的数据，在计算机系统中一般叫做高速缓存（L1, L2...）。

### 虚拟页和物理页
为了方便理解，我们可以将虚拟内存(VM)看做一个由存放在磁盘上的N个连续的字节大小的单元组成的数组，每个字节都有一个唯一的虚拟地址，作为到数组的索引。磁盘上数组的内容被缓存在主存中。虚拟内存系统将虚拟内存分割为固定大小（P字节）的虚拟页（Virtual Page, VP），作为磁盘和主存之间的传输单元。类似地，物理内存被分割为P字节大小的物理页（Physical Page, PP）。
在任意时刻，虚拟页面的集合都被分为三个不相交的子集：
* 未分配的：没有相关联的数据，不占用任何磁盘空间。
* 缓存的：当前已缓存在物理内存中的已分配页。
* 未缓存的：未缓存在物理内存中的已分配页。

### 如何维护虚拟页和物理页关系？
#### 页表
虚拟内存系统通过一个常驻在物理内存中的叫做页表（Page Table）的数据结构来管理虚拟页和物理页，它将虚拟页映射到物理页。每次MMU中的地址翻译硬件将一个虚拟地址转换成物理地址时，都会读取页表。操作系统内核负责维护这个页表的内容，以及在磁盘与DRAM之间来回传送页。
下图是页表的基本组织结构：

![页表](https://note.youdao.com/yws/api/personal/file/55E0B53D191B450381C4A5A7438C1729?method=download&shareKey=340be1fa5a56e497c78e9ffcb5643ae7)

页表就是一个页表条目（Page Table Entry, PTE）的数组。虚拟地址空间的每个页在页表中都有一个PTE。在这张图中，假设了每个PTE是由一个有效位和n位地址字段组组成：
* 有效位标识虚拟页是否被缓存在DRAM中。如果有效位没有被设置，空地址（null）表示虚拟页还未被分配（未分配的），否则就是虚拟页在磁盘上的起始位置（未缓存的）。
* 如果有效位被设置，n位地址字段表示DRAM中相应的物理页的起始位置（缓存的）。

#### 页面命中（page hit）
如下图中有效位为1的几个PTEs，CPU访问的虚拟地址（落在的虚拟页面）对应的物理页面已经被缓存到物理内存中，CPU可以直接从物理内存中读到数据字，也叫做DRAM缓存命中（DRAM cache hit）
![页表命中](https://note.youdao.com/yws/api/personal/file/C1C45A6803874CE4A4096557DAC16624?method=download&shareKey=340be1fa5a56e497c78e9ffcb5643ae7)

#### 页面未命中（page miss）
如下图，如果CPU访问的虚拟地址（落在的虚拟页面）对应的物理页面没有被缓存在物理内存中（PTE有效位为0且后面的内容不为NULL，则指向一个磁盘上的地址），则出现了页面未命中，也叫做DRAM缓存未命中（DRAM cache miss）。

![缺页1](https://note.youdao.com/yws/api/personal/file/2D7882D37AFE443E9C7E89ED742ED9C9?method=download&shareKey=340be1fa5a56e497c78e9ffcb5643ae7)

页面未命中会触发一个缺页异常（page fault exception），内核的缺页处理程序会选择一个牺牲页（下图中的VP4对应的物理页面，如果已经修改则先换出到磁盘），然后将需要访问的物理页面从磁盘换入到物理内存，并更新页表中的PTE（需要更新两条PTE）。

![缺页2](https://note.youdao.com/yws/api/personal/file/D06F522EAC314026BDB7E2198D734B1D?method=download&shareKey=340be1fa5a56e497c78e9ffcb5643ae7)

缺页处理程序处理完成后，CPU会重启开始执行的指令，不过这次就是page hit的流程了。

![缺页3](https://note.youdao.com/yws/api/personal/file/90E1A71D36954D97842741E786D9F6F5?method=download&shareKey=340be1fa5a56e497c78e9ffcb5643ae7)

后面会再次描述缺页处理的详细过程。

如下图所示，除了有效位，页表还带有一些许可位来控制页的访问（实际的页表条目包含更多的内容）：
![带许可位的页表](https://note.youdao.com/yws/api/personal/file/F3DE1353B9DD4888B2C2A5E18207C58F?method=download&shareKey=340be1fa5a56e497c78e9ffcb5643ae7)

>* 运行在内核模式的进程可以访问任何页面，运行在用户模式的进程只允许访问部分页面。
>* 控制用户模式进程对页面的读写权限。


如果一条指令违反了这些许可条件，那么CPU就触发一个一般保护故障，将控制传递给内核中的异常处理程序。Linux shell一般将这种异常报告为段错误（segmentation fault）。

Notes:
>* DRAM缓存是全相联的，所以任意物理页都可以包含任意虚拟页。
>* 操作系统为每个进程提供了一个独立的页表，因而每个进程都拥有一个独立的地址空间。多个虚拟页面可以映射到同一个共享物理页面上。

### 虚拟内存的设计有哪些优点？
虚拟内存简化了简化了链接和加载、代码和数据共享以及应用程序的内存分配，同时也为物理内存提供了保护机制：
> * 简化链接。独立的地址空间允许每个进程的内存映像使用相同的基本格式，而不管代码和数据实际存放在物理内存的何处，比如代码段总是从虚拟地址0X400000（x86-64）开始，数据段跟在代码段之后，栈占据用户进程地址空间最高的部分。这样的一致性极大的简化了链接器的设计和实现，允许链接器生成完全链接的可执行文件，这些可执行文件是独立于物理内存中代码和数据的最终位置的。
> * 简化加载。要把目标文件中的.text和.data节加载到一个新创建的进程中，Linux加载器为代码段和数据段分配虚拟页，并把它们标记为无效的（未被缓存的），将页表条目指向目标文件中适当的位置（磁盘位置）。但是，加载器从不从磁盘向内存复制任何实际数据。在每个页初次被引用时（可能是CPU取指令时引用，也可能是一条正在执行的指令引用一个内存位置），虚拟内存系统会根据需要自动地调入数据页（也就是将磁盘数据缓存到主存中）。将一组连续的虚拟页映射到任意一个文件中的任意位置称作内存映射（mmap提供这样的功能）。
> * 简化共享。独立地址空间为操作系统提供了一个管理用户进程和操作系统自身之间共享的一致机制。一般而言，每个进程都有自己私有的代码、数据、堆栈，这些不和其它进程共享，则操作系统为每个进程创建页表，将相应的虚拟页映射到不连续的物理页。对于进程共享的代码和数据，比如每个进程都会调用相同的操作系统内核代码，每个C程序都会调用标准库中的程序（比如printf），操作系统通过将不同进程中的适当的虚拟页面映射到相同的物理页面来实现共享，而不是在内存中为每个进程保存一份单独的副本。
> * 简化内存分配。当一个运行中的用户程序申请额外的堆空间时（比如调用malloc），操作系统分配一个适当数字（比如k个）个连续的虚拟页面，并且将它们映射到物理内存中任意位置的k个任意的物理页面。由于页表的工作方式，操作系统没有必要分配k个连续的物理内存页面，页面可以随机的分散在物理内存中。
> * 保护物理内存。参考前面提到的带许可位的页表。

### 虚拟地址如何翻译成物理地址？
地址翻译是一个N元素的虚拟地址空间（VAS）中的元素和一个M元素的物理地址空间（PAS）中元素之间的映射，虚拟地址地址翻译的结果可能是：
* 如果虚拟地址的数据已经缓存到物理内存，则得到物理地址空间的一个地址。
* 如果虚拟地址的数据不在物理内存中，则无法得到一个物理地址。

下图展示了从虚拟地址到物理地址最简单的翻译过程：

![地址翻译](https://note.youdao.com/yws/api/personal/file/153FE65720AD4AD6A5FE8860CE90588D?method=download&shareKey=340be1fa5a56e497c78e9ffcb5643ae7)

CPU中的页表基址寄存器（Page Table Base Register, PTBR，属于控制寄存器）指向当前（进程）页表。n位的虚拟地址包含两部分：
* p位的虚拟页偏移（Virtual Page Offset, VPO）。
* n-p位的虚拟页号（Virtual Page Number, VPN）。

另外，m位的物理地址也有类似的结构，由物理页面偏移（PPO）和物理页面号（PPN）组成。因为物理页面和虚拟页面都是P字节的，所以PPO和VPO是相同的。比如页面大小为2^6=64字节，则虚拟地址和物理地址的低6位分别作为VPO和PPO，剩下的高位作为VPN和PPN。

页面命中时（已缓存），如下图所示，地址翻译完全由硬件完成处理。

![命中翻译步骤](https://note.youdao.com/yws/api/personal/file/1C38FB5EBC5349DE9D3A9F39DF27416C?method=download&shareKey=340be1fa5a56e497c78e9ffcb5643ae7)

详细执行的步骤：
* 1. 处理器生成一个虚拟地址，并把它传给MMU。
* 2. MMU生成PTE地址（利用VPN来选择适当的PTE），然并从高速缓存/主存请求得到它。
* 3. 高速缓存/主存向MMU还回PTE。
* 4. MMU构造物理地址，并把它传给高速缓存/主存。
* 5. 高速缓存/主存返回所请求的数据字给处理器。


页面未命中时（page miss），如下图所示，其处理要求硬件和操作系统内核协作完成。

![未命中翻译步骤](https://note.youdao.com/yws/api/personal/file/1ED17CDD9CDF4F9888B903D22AFB83D4?method=download&shareKey=340be1fa5a56e497c78e9ffcb5643ae7)

详细执行的步骤：
* 1. 处理器生成一个虚拟地址，并把它传给MMU。
* 2. MMU生成PTE地址（利用VPN来选择适当的PTE），然并从高速缓存/主存请求得到它。
* 3. 高速缓存/主存向MMU还回PTE。
* 4. PTE中的有效位为0，所以MMU触发了一次异常，传递CPU中的控制到OS内核中的缺页异常处理程序。
* 5. 缺页处理程序确定出物理内存中的牺牲页，如果这个页面已经被修改了，则把它换出到磁盘(Swaping)。
* 6. 缺页处理程序调入新的页面，并更新内存中的PTE。
* 7. 缺页处理程序返回原来的进程，再次执行导致缺页的指令。处理器将引起缺页的虚拟地址重新发送给MMU，这次就和页面命中的流程一样了。



### 高速缓存（SRAM）和虚拟内存如何配合？
在任何同时使用虚拟内存和SRAM高速缓存的系统中，都有应该使用虚拟地址还是物理地址来访问SRAM的问题，现实中大多数系统选择了物理寻址，原因主要有：
* 使用物理寻址，多个进程同时在高速缓存中有存储块和共享来自相同虚拟页面的块更为简单。
* 高速缓存无须处理保护问题，访问权限的检查是地址翻译的一部分。

下图展示了物理寻址的高速缓存和虚拟内存相结合的情况，其关键思路：地址翻译发生在高速缓存查找之前。需要注意的是，和其他数据字一样，PTE也可以被缓存在SRAM中。
![高速缓存结合虚拟内存](https://note.youdao.com/yws/api/personal/file/A7E8CD2BA22F4C14AAC0FE548821DB58?method=download&shareKey=340be1fa5a56e497c78e9ffcb5643ae7)

### TLB如何加速地址翻译？
每次CPU产生一个虚拟地址，MMU就必须从高速缓存/内存查询一个PTE，其时间开销分为两种情况：
> * 如果要查询的PTE在L1高速缓存（SRAM）命中，那么只需要1-2个周期，这是比较理想的情况。
> * 如果要查询的PTE没有在高速缓存命中，就需要从内存（DRAM）中读取，需要几十到几百个周期，这是最糟糕的情况。

MMU中包含一个关于PTE的小缓存，称为翻译后备缓冲器（Translation Lookaside Buffer, TLB）。TLB是一个小的、超级快的、虚拟寻址的硬件缓存，其每一行都保存着一个由单个PTE组成的块。

TLB中的PTE条目通过组选择的索引和行匹配的标记来查询，而索引和标记字段均从虚拟地址的VPN中提取。如果TLB有T=2^t个组，那么TLB索引（TLBI）是由VPN中的t个最低位组成，而TLB标记（TLBT）是由VPB中剩余的位组成。

下图展示了MMU从TLB查询PTE命中时的情况：
![TLB命中](https://note.youdao.com/yws/api/personal/file/37171C641C4E45C5B6D0F985A40EFB7D?method=download&shareKey=340be1fa5a56e497c78e9ffcb5643ae7)

有了TLB之后，MMU查询PTE的顺序就变为：TLB缓存 -> SRAM高速缓存 -> DRAM主存。如果PTE在TLB中命中，则所有的地址翻译都是在芯片上的MMU中执行的，效率非常高。

注意：在进程上下文切换后，TLB的缓存内容将会过期，因此系统会做如下事情：
* 上下文切换时刷新TLB。
* 将进程ID和每个TLB条目关联起来。

总结：地址翻译最主要的工作就是查询在主存中维护的PTE，而高速缓存可以缓存主存中的PTE，从而提升PTE查询效率。TLB也是用于缓存PTE，不过因为是MMU自带的硬件缓存，在命中的情况下查询效率比SRAM高速缓存更高。

[TLB的作用及工作原理](https://www.cnblogs.com/alantu2018/p/9000777.html)

### 用多级页表解决页表过大问题
我们假设：
* 页面大小为2^12=4KB
* 32位地址空间（2^32个地址的集合）
* 4字节的PTE

则我们一共需要(2^32 / 2^12) * 2^2 = 2^22（字节）= 4MB，也就是需要一个4MB的页表驻留在内存中。对于地址空间为64位的系统而言，问题将更为复杂。

解决页表占用空间的问题需要引入具有层次结构的多级页表，比如下面的2级页表：
![2级页表](https://note.youdao.com/yws/api/personal/file/ED86D991C77B492EB44A4E33091B0B66?method=download&shareKey=340be1fa5a56e497c78e9ffcb5643ae7)
图中假设了此时虚拟地址空间中的结构：内存前2K个页面分配给了代码和数据，接下来的6K个页面还未分配，接下来的1个页面分配给了用户栈。
一级页表的每个PTE负责映射虚拟地址空间中的一个4MB的片（chunk），每个片由1024个连续的页面组成。假设地址空间为4GB，则1024个PTE足够。

> * 如果片i中的每个页面都未被分配，则一级PTEi则为空（图中片2-7）。
> * 如果片i中至少有一个页是分配的，那么一级PTEi就指向一个二级页表的基址（图中片0、1、8）。
二级页表中的每个PTE负责映射一个4KB的虚拟内存页面。

这种方法从两个方面减少了维护页表的内存需求：
> * 如果一级页表的一个PTE为空，则对应的二级页表根本不会存在，而对于一个典型的程序，4GB的虚拟地址空间大部分会是未分配的。
> * 只有一级页表才总是驻留在内存中。虚拟内存系统再需要时创建、页面调入或调出二级页表，这就减少了主存压力，只有最经常使用的二级页表才需要缓存在主存中。

下图展示了k级页表层次结构的地址翻译，虚拟地址被划分成k个VPN和1个VPO，每个VPNi都是到第i级页表的索引（或者说偏移量），而前面的页表（1到k-1）中的PTE都指向下一级某个页表的基址，只有第k级的页表中的PTE包含某个物理页面的PPN，或者一个磁盘块的地址。
![多级页表](https://note.youdao.com/yws/api/personal/file/FA2AF03E5B4A460C8C9A9E51F176054E?method=download&shareKey=340be1fa5a56e497c78e9ffcb5643ae7)
为了构造物理地址，在确定PPN之前，MMU必须访问k个PTE。对于只有一级的页表结构，PPO和VPO是相同的。由于TLB将不同层次上页表的PTE缓存起来，所以实际上带多级页表的地址翻译并不比单级页表慢很多。

### 案例分析-Core i7/Linux地址翻译
当前Core i7实现支持48位（256TB）虚拟地址空间和52位（4PB）的物理地址空间，同时兼容32位虚拟和物理地址空间。Core i7的内存系统：
* 页大小可以在启动时配置为4KB或者4MB，Linux使用的是4KB的页。
* 采用4级页表层次结构。每个进程有它自己私有的页表层次结构，当一个Linux进程在运行时，虽然Core i7体系结构允许页表换进换出，但是已经分配了的页相关联的页表都是驻留在内存中的。
* CR3控制寄存器指向第一级页表（L1）的起始位置，CR3的值是每个进程上下文的一部分，每次上下文切换时，CR3的值都会被恢复。

下图展示了Core i7内存系统地址翻译的情况：
![Core i7](https://note.youdao.com/yws/api/personal/file/3F910E12B5B34FFDB227E1BEE30B14A6?method=download&shareKey=340be1fa5a56e497c78e9ffcb5643ae7)

> * CO: Byte offset within cache line
> * CI: Cache index
> * CT: Cache tag

## Linux虚拟内存系统***
Linux为每个进程维护了一个单独的虚拟地址空间，下图是x86-64 Linux系统进程地址空间的组织结构：地址空间底部保留给用户程序，包括通常的代码、数据、堆和栈段，代码段总是从虚拟地址0x400000开始。地址空间顶部（从2^48-1开始）保留给内核（操作系统常驻内存部分），这部分对用户代码不可见。

![linux_virtual_memory](https://note.youdao.com/yws/api/personal/file/4313DD729BA5414EB21C1F35A88CEAF4?method=download&shareKey=340be1fa5a56e497c78e9ffcb5643ae7)

* 内核虚拟内存包含内核中的代码和数据，内核虚拟内存的某些区域被映射到所有进程共享的物理页面。
* 内核虚拟内存的其它区域包含每个进程都不相同的数据，比如页表、内核在进程的上下文中执行代码时使用的栈，以及记录虚拟地址空间的各种数据结构。

### Linux虚拟内存区域结构
Linux将虚拟内存组织成一些区域（area，也叫做段）的集合，也就是上图中的代码段、数据段、堆、共享库段，以及用户栈。一个区域就是已经分配的虚拟内存的连续片（chunk），这些页都是以某种方式相关联的。每个存在的虚拟页面都保存在某个区域中，而不属于某个区域的虚拟页面是不存在的，并且不能被进程引用。区域允许虚拟地址空间有间隙，内核不用记录那些不存在的虚拟页，而这样的页也不占用内存、磁盘等额外资源。

内核为系统中的每个进程维护一个单独的任务结构（源码中的：task_struct），这个任务结构包含或者指向内核运行该进程所需要的所有信息，包括PID、指向用户栈的指针、可执行目标文件的名字，以及程序计数器（PC）等。
下图强调了其中关于进程虚拟内存区域的数据结构：

![linux_vm_area](https://note.youdao.com/yws/api/personal/file/EFF5A0BC4ED444A7AEE7A151665CE8FF?method=download&shareKey=340be1fa5a56e497c78e9ffcb5643ae7)

task_struct中的一个条目指向mm_struct，它描述了虚拟内存的当前状态。这里重点关注pgd和mmap两个字段：
* pgd指向第一级页表（页全局目录）的基址。当内核运行这个进程时，就将pgd存放在CR3控制寄存器中。
* mmap指向一个vm_area_structs（区域结构）的链表，其中每个vm_area_structs都描述了当前虚拟地址空间的一个区域。一个区域结构包含以下几个字段：
vm_start：指向区域起始处。
vm_end：指向区域结束处。
vm_prot：描述区域内包含的所有页的读写许可权限。
vm_flags：描述区域内的页面是否与其它进程共享。
vm_next：指向链表中下一个区域结构。

### Linux缺页异常处理
如果MMU翻译一个虚拟地址A时触发了一个缺页，这个异常导致控制转移到内核的缺页处理程序，其处理流程如下：
* 判断A是合法的虚拟地址吗？具体做法是搜索区域结构的链表，把A和每个区域结构中的vm_start和_vm_end做比较。如果A是不合法的，则触发段错误，从而终止这个进程。（为了提升搜索链表的效率，Linux在链表中构建了一棵树）
* 判断试图进行的内存访问是否合法？即是否有读、写或者执行这个区域内页面的权限。如果是不合法的访问，则触发保护异常，从而终止这个进。
* 开始处理缺页：选择一个牺牲页面（如果这个牺牲页面被修改过，则需要先交换出去），换入新的页面并更新页表。当缺页处理程序返回时，CPU重新启动引起缺页的指令，这条指令将再次发送A到MMU做地址翻译，只是这次可以翻译成可用的物理地址，而不会再引起缺页中断。

下图展示了其中三种情况：
![linux_page_fault_handling](https://note.youdao.com/yws/api/personal/file/C889F58C5D0D47F19E7064FC85091D0A?method=download&shareKey=340be1fa5a56e497c78e9ffcb5643ae7)


## 内存映射***
Linux通过将一个虚拟内存区域与一个磁盘上的对象（object）关联起来，以初始化这个虚拟内存区域的内容，这个过程称为内存映射（memory mapping）。虚拟内存区域可以映射到下面两种类型的对象中的一种：
> * Linux文件系统中的普通文件：一个区域可以映射到一个普通磁盘文件的连续部分，比如可执行目标文件。文件区被分成页大小的片，每一片包含一个虚拟页面的初始内容。因为按需进行页面调度，所以这些虚拟页面没有实际交换进入物理内存，直到CPU第一次引用到页面（即发送一个虚拟地址，落在地址空间这个页面范围之内）。如果区域比文件区要大，则用零填充这个区域的其余部分。
> * 匿名文件。匿名文件由内核创建，包含的全是二进制零。CPU第一次引用这样一个区域内的虚拟页面时，内核就在物理内存中找到一个合适的牺牲页面（脏页面则先换出），用二进制零覆盖牺牲页面并更新页表，将这个页面标记为是驻留在内存中的。注意，与映射普通文件对象相比，映射匿名文件在磁盘和内存之间并没有实际的数据传送。映射到匿名文件的区域中的页面有时也叫做请求二进制零的页（demand-zero page）。

无论哪种映射，一旦一个虚拟页面被初始化了，它就在一个由内核维护的专门的交换文件（swap file）之间换来换去。交换文件也叫做交换空间（swap space）或者交换区域（swap area）。需要理解的是：任何时刻，交换空间都限制着当前运行着的进程能够分配的虚拟页面的总数。

后面可以看到内存映射的概念提供了一种简单而高效的把程序和数据加载到内存中的方法。

### 共享对象和私有对象
进程这一抽象能为每个进程提供自己私有的虚拟地址空间，可以免受其它进程的错误读写。不过，许多进程都有同样的只读代码区域，比如每个运行bash的进程都有相同的代码区域。而且许多程序只需要访问只读运行时库代码的相同副本，比如C库中的printf函数。对于这些公共的部分，如果我们只在物理内存中保存一份，那么可以大大节省内存资源。
一个对象可以被映射到虚拟内存的一个区域，要么作为共享对象，要么作为私有对象：
> * 共享对象：进程对映射了共享对象的区域（共享区域）的任何写操作，对于其他也映射了该共享对象的进程是可见的，而且这些变化会反映在磁盘上的原始对象中。因为每个对象都有唯一的文件名，
内核可以迅速的判断是否已经有某个进程（进程1）映射了一个共享对象，需要时则直接将后面的进程（进程2）的页表条目指向相应的物理页面即可。
> * 私有对象：进程对于映射到私有对象的区域（私有区域）做的改变，对于其他进程不可见，并且相应的写操作不会反映到磁盘上的原始对象中。

私有对象使用写时复制（COW, Copy-On-Write）技术被映射到虚拟内存中。如下面两张图所示：对于映射私有对象的进程，相应私有区域的页表条目被标记为只读，并且区域结构被标记为"私有的写时复制"。即使对于多个进程，一个私有对象最开始在物理内存中也只保留一份副本。只要没有进程试图写它自己的私有区域，它们就可以继续共享物理内存对象中的一个单独副本。只要某个进程试图写私有区域内的某个页面，那么这个写操作就会触发一个保护故障。故障处理程序注意到保护异常是因为进程写私有区域的一个页面导致的，就会在物理内存中创建该页面的一个新副本，更新页表条目指向新副本，然后恢复这个页面的可写权限，故障处理程序返回时，CPU重新执行这个写操作。
![linux_cow1](https://note.youdao.com/yws/api/personal/file/ABA6521745B546448D4121D0A40C86FC?method=download&shareKey=340be1fa5a56e497c78e9ffcb5643ae7)
![linux_cow2](https://note.youdao.com/yws/api/personal/file/1CE44C911B14487E8BBA3D5F7A22A271?method=download&shareKey=340be1fa5a56e497c78e9ffcb5643ae7)

### 从内存映射角度理解fork函数
fork函数被当前进程调用时，内核为新进程创建各种数据结构，并分配唯一的PID。为了给新进程创建虚拟内存，它创建了当前进程的mm_struct、区域结构（vm_area_structs）和页表的原样副本。它将两个进程中的每个页面都标记为只读，并将两个进程中的每个区域结构都标记为私有的写时复制。
fork在新进程中返回时，新进程现在的虚拟内存刚好和调用fork时存在的虚拟内存相同。当这两个进程中的任一个进行写操作时，写时复制机制就会创建新页面。
当然，实际上会创建新页面的都只是堆、栈等在程序运行时会被更改的区域，代码段和共享库这些只读区域仍然是共享的，除非进程调用execve函数执行新的可执行目标文件。


### 从内存映射角度理解execve函数
```c
execve("a.out", NULL, NULL);
```
execve函数在当前进程中加载并运行包含在可执行目标文件a.out中的程序，用a.out程序有效地替代了当前程序，加载a.out的步骤：
> * 释放已存在的用户区域和页表。删除当前进程虚拟地址的用户部分中的已经存在的区域结构以及页表。
> * 映射私有区域。为新程序的代码、数据、bss和栈区域创建新的区域结构和页表。所有这些区域都是私有的、写时复制的。代码和数据区被映射为a.out文件中的.text和.data区。bss区域是请求二进制零的（demand-zero），映射到匿名文件，其大小包含在a.out中。栈区和堆区域也是请求二进制零的，初始长度为0。
> * 映射共享区域。如果a.out程序与共享对象（或目标）链接，比如C库lic.so，那么这些对象都是动态链接到这个程序的，然后再映射到用户虚拟地址空间中的共享区域内。
> * 设置程序计数器（PC）。 execve做的最后一件事就是设置当前进程上下文中的程序计数器，使之指向代码区域的入口点。下一次调度这个进程时，将从这个入口开始执行。Linux将根据需要换入代码和数据页面（从磁盘加载到物理内存）。

![linux_execve](https://note.youdao.com/yws/api/personal/file/E8A9CA1401144E7CB605C31EB03B96F4?method=download&shareKey=340be1fa5a56e497c78e9ffcb5643ae7)


### 用mmap函数创建用户级内存映射
```c
#include <sys/mman.h>

void *mmap(void *addr, size_t length, int prot, int flags,
                  int fd, off_t offset);
int munmap(void *addr, size_t length);
```
mmap函数要求内核创建一个新的虚拟内存区域，最好是从地址start开始的一个区域，并将文件描述符fd指定的对象的一个连续的片（chunk）映射到这个新的区域。连续的对象片大小为length字节，从距离问阿金开始处offset字节的地方开始。start仅仅是一个暗示，通常被定义为NULL。
![mmap](https://note.youdao.com/yws/api/personal/file/42DEC76CEB544DEC8C48848070AFC810?method=download&shareKey=340be1fa5a56e497c78e9ffcb5643ae7)

munmap函数删除从虚拟地址start开始的，长度为length字节的区域。

## 扩展资料

[深入理解 Linux 虚拟内存管理  - 小林coding ****](https://xiaolincoding.com/os/3_memory/linux_mem.html#_4-6-深入理解-linux-虚拟内存管理)


# C10 系统级I/O
## Unix内核如何管理打开的文件？
如下面这几张图所示，内核通过3张表来管理打开的文件：
>* 每个进程维护一张文件描述符表（Descriptor table）。
>* 所有进程共享一张打开文件表（Open file table）。
>* 所有进程共享一张v-node表（v-node table）。

图一：进程打开两个不同的文件。
![open_files1](https://note.youdao.com/yws/api/personal/file/8563E0DEC0C142EB85883DF622C8DEDD?method=download&shareKey=340be1fa5a56e497c78e9ffcb5643ae7)
图二：进程对同一个文件调用open两次，产生两个文件描述符，它们在打开文件表里各有一个条目，但是共享v-node表条目
![open_files2](https://note.youdao.com/yws/api/personal/file/77662A572ECA4BDD8A73BDEE17D2D5B5?method=download&shareKey=340be1fa5a56e497c78e9ffcb5643ae7)
图三：fork调用前
![open_files3](https://note.youdao.com/yws/api/personal/file/30AA0A7298DC408B8DB8AF2893516834?method=download&shareKey=340be1fa5a56e497c78e9ffcb5643ae7)
图四：fork调用后，子进程继承父进程打开的文件，共享打开文件表和v-node表中的条目。
![open_files4](https://note.youdao.com/yws/api/personal/file/4E1576D891B94888BD5755A865F08DF1?method=download&shareKey=340be1fa5a56e497c78e9ffcb5643ae7)


## I/O重定向的原理
以下面的输出重定向为例：
```shell
$ ls > out.text
```
类Unix系统中I/O重定向的一种实现就是基于dup2这个系统调用：
```c
#include <unistd.h>

int dup2(int oldfd, int newfd); 
//复制描述符表表项oldfd到描述符表表项newfd，覆盖描述符表表项newfd以前的内容。
//如果newfd已经打开了，dup2会在复制oldfd之前先close掉newfd。
```
Step1：shell在执行ls程序时，会创建用于执行ls的子进程p1，然后在p1中打开需要重定向的文件out.text，得到下图中的fd4。
![io_redirection1](https://note.youdao.com/yws/api/personal/file/5F66DCD256604C78931DAE96BD87E9DE?method=download&shareKey=340be1fa5a56e497c78e9ffcb5643ae7)
Step2：调用dup2(4, 1)将进程描述符表中的fd4的表项复制到fd1的表项，所以fd1和fd4指向了相同的磁盘文件。此后，原本写入fd1（标准输出）的内容就被写入重定向的磁盘文件了。
![io_redirection2](https://note.youdao.com/yws/api/personal/file/E5748420909948F9A84BFDD018D2167B?method=download&shareKey=340be1fa5a56e497c78e9ffcb5643ae7)

注意，这两个步骤都应该发生在shell在p1中执行execve("ls", NULL， NULL)之前。

## Unix I/O和标准I/O对比
![io](https://note.youdao.com/yws/api/personal/file/B7177D1B8A9B4BD8AA429B0C5623B6F9?method=download&shareKey=340be1fa5a56e497c78e9ffcb5643ae7)

Unix I/O：
>* 通过系统调用来访问，I/O操作的对象是int型的文件描述符。
>* 提供访问文件元数据的接口。
>* 系统I/O接口是异步信号安全的，可以安全地在信号处理函数中使用。
>* 不提供缓冲。

标准I/O：
>* 在用户空间提供了缓冲区，通过减少调用系统级I/O接口的次数来提升I/O效率。
>* 标准I/O操作的对象是FILE类型的流，FILE类型的流就是对文件描述符和流缓冲区的抽象。

>* 标准I/O不是异步信号安全的，不适合作为信号处理函数。
>* 因为标准I/O的一些限制，导致它不适合用于网络套接字的输入和输出操作。
>* 没有提供访问文件元数据的接口。

标准I/O和RIO都是基于底层的Unix I/O实现，RIO是CSAPP实现的一组I/O接口，参考[RIO源码](http://www.cs.cmu.edu/afs/cs/academic/class/15213-f14/www/code/15-io/)。

C程序在开始运行时就拥有3个打开的流对象：stdin, stdout, stderr。
```c
#include <stdio.h>
extern FILE *stdin;  /* standard input  (descriptor 0) */
extern FILE *stdout; /* standard output (descriptor 1) */
extern FILE *stderr; /* standard error  (descriptor 2) */

int main() {
    fprintf(stdout, "Hello, world\n");
}
```

在大多数场景，不建议直接使用底层的Unix I/O，而是使用标准I/O。

### 标准I/O的缓冲类型
>* 全缓冲（Fully buffered）：只有在标准I/O缓冲区填满的时候才会发生实际的底层I/O操作（read, write）。
>* 行缓冲（Line buffered）：在输入输出的数据中检测到换行符时则执行底层I/O操作。
>* 不缓冲（Unbuffered）：和Unix I/O一样不提供缓冲。

Notes：
>标准错误一般都是不缓冲的，指向字符设备文件的流一般是行缓冲的，而其他形式的流比如普通文件的流，基本上都是采用全缓冲。

任何时候，我们都可以调用fflush来强制刷新指定流fd的缓冲区里的内容到磁盘，fp为NULL时所有输出流都会被刷新。
```c
#include <stdio.h>
/* Returns: 0 if OK, EOF on error */
int fflush(FILE *fp);
```

[Unix Programming: Standard I/O Library](https://medium.com/hungys-blog/unix-programming-standard-io-librar-f59f46a1b364)



# C12 并发编程
如果逻辑控制流在时间上重叠，那么它们就是并发的。操作系统内核交替运行多个应用程序也是一种并发，这也是并发应用程序的基础。
三种基本的构造并发程序的方法：
>* 多进程。每个逻辑控制流都是一个进程，由内核来调度和维护。因为进程有独立的虚拟地址空间，所以进程间通信必须使用某种显示的进程间通信（IPC）机制。
>* I/O多路复用。应用程序在一个进程的上下文中显示地调度自己的逻辑流，共享地址空间。比如常见的基于select和epoll的异步I/O模型。
>* 多线程。线程是运行在一个单一进城上下文中的逻辑流，由内核进行调度。多线程技术可以看做是多进程技术和I/O多路复用技术的结合体。

## 基于I/O多路复用的并发编程
### 理解select
### 理解epoll

## 基于多线程的并发编程
### 理解线程
理解线程一定是在理解进程的基础上，下面是最简化的进程视图（不引入线程概念）：进程 = 进程上下文 + 代码、数据、和栈
![process_view1](https://note.youdao.com/yws/api/personal/file/46C9DA727B0A46E6B4058BA37A328DCD?method=download&shareKey=340be1fa5a56e497c78e9ffcb5643ae7)

下面是引入线程概念后的进程视图：进程 = 线程 + 代码、数据和内核上下文
![process_view2](https://note.youdao.com/yws/api/personal/file/19BCD444E4824AB49CE724484EE66716?method=download&shareKey=340be1fa5a56e497c78e9ffcb5643ae7)

* 线程有自己独立的逻辑控制流。
* 每个进程开始生命周期时都是单一线程（主线程），在某一时刻，主线程创建一个对等线程（peer thread），从这个时间点开始，两个线程就并发的运行。

下面是多线程进程的视图：
![process_view3](https://note.youdao.com/yws/api/personal/file/1C129EF72FB54FB18295C8210878511A?method=download&shareKey=340be1fa5a56e497c78e9ffcb5643ae7)

* 运行在一个进程内的所有线程共享该进程的整个地址空间的内容，包括代码、数据、堆、共享库和打开的文件。
* 每个线程都有自己的线程上下文，包括唯一的整数线程ID、栈、栈指针SP、程序计数器PC、通用目的寄存器和条件码、调度优先级和策略、信号屏蔽字以及errno变量。尽管每个线程都有自己的栈用于保存本地变量，但是不能阻止其它线程访问线程的栈。
* 一个进程中运行着1-n个线程，线程由内核调度，内核通过线程ID来识别线程。

#### 线程和进程的对比

* 进程控制（创建和回收）的开销差不多是线程控制（创建和回收）的两倍。在Linux系统上，前者大约花费20K个周期，后者开销大约为10K个周期。
* 一个线程的上下文比进程的上下文小得多，所以线程的上下文切换要比进程的上下文切换快得多。
* 如下图，线程不像进程那样按照严格的父子层次来组织，和一个进程相关的线程组成一个对等的线程池，独立于其它线程创建的线程，主线程和其它线程的区别仅在于它总是进程中第一个运行的线程。
![process_thread](https://note.youdao.com/yws/api/personal/file/50624EF8FA0E40D4A636E8F9E91F6D41?method=download&shareKey=340be1fa5a56e497c78e9ffcb5643ae7)

Notes:
>* 一个线程可以杀死它的任何对等线程，或者等待它的任意对等线程终止。
>* 每个对等线程都能读写相同的共享数据。

#### 创建线程
```c
int pthread_create(pthread_t *thread, const pthread_attr_t *attr,
                          void *(*start_routine) (void *), void *arg);
```
Notes
>* 线程创建时不能保证哪个线程会先运行：是新创建的线程，还是调用线程。所以需要谨慎地处理调用线程和新线程之间的竞争关系。
>* 新创建的线程继承调用线程的浮点环境和信号屏蔽字，但是该线程的挂起信号集会被清除。

#### 初始化线程
once_control变量是一个全局或者静态变量，总是被初始化为PTHREAD_ONCE_INIT。第一次用参数once_control调用pthread_once函数的时候，它会调用init_route函数。接下来的以once_control作为参数调用pthread_once的时候不做任何事情。
pthread_once常用于动态初始化多个线程共享的全局变量。
```c
pthread_once_t once_control = PTHREAD_ONCE_INIT;

int pthread_once(pthread_once_t *once_control, void (*init_routine)(void));
```

#### 终止线程
一个线程可以通过以下几种方式来终止：
* 默认终止行为。线程例程返回时（线程的逻辑流执行完毕），线程会隐式地终止。
* 调用pthread_exit函数。调用这个函数，线程会显示的终止。如果主线程调用这个函数，它会等待所有其它对等线程终止，然后再终止主线程和整个进程，返回值为retval。
```c
void pthread_exit(void *retval);
```
* 调用exit函数。某个对等线程调动Linux的exit函数，该函数终止进程及所有与该进程相关的线程。
* 调用pthread_cancel函数。某个对等线程通过以当前线程ID作为参数调用pthread_cancel函数来终止目标线程。实际上pthread_cancel调用并不等待目标线程终止，只是给目标线程发送取消请求（cancellation request），目标线程收到请求后的行为取决于它的可取消状态（cancelability state）和取消类型（cancellation type），这两个线程的属性分别通过pthread_setcancelstate函数和pthread_setcanceltype函数设置，具体用法和行为参考相关的man page。
```c
int pthread_cancel(pthread_t thread);
```

Notes
> pthread_create和pthread_exit函数的无类型指针参数可以传递的值不止一个，这个指针可以传递包含复杂信息的结构的地址。但是，这个结构所使用的内存在调用者完成调用后必须仍然是有效的。例如，在调用线程的栈上分配了该结构，那么其它的线程在使用这个结构的时候内存里的内容已经改变了。又比如线程在自己的栈上分配了一个结构，然后把指向这个结构的指针传给pthread_exit，那么调用pthread_join的线程视图使用该结构时，这个栈可能已经被撤销，这块内存也已经另作他用。在类似场景中，用malloc函数从堆上分配内存是更为合理的处理。


线程可以调用pthread_cleanup_push函数安排它退出时需要调用的函数，称为线程清理处理程序（thread cleanup handler），这与进程退出时可以用atexit函数安排退出类似。一个线程可以建立多个清理处理程序，处理程序记录在栈中，因此执行顺序与注册顺序相反。
```c
void pthread_cleanup_push(void (*routine)(void *),
                            void *arg);

void pthread_cleanup_pop(int execute);
```

当线程执行以下动作时，清理函数routine是由pthread_cleanup_push函数调度的，调用时只有一个参数arg：
* 调用pthread_exit。
* 响应取消请求时。
* 用非零execute参数调用pthread_cleanup_pop时（显示的调用清理函数）。如果execute参数设置为0，清理函数将不被调用。


#### 回收线程
线程通过调用pthread_join函数等待其他线程终止，该调用会阻塞直到目标线程终止（调用pthread_exit、从线程例程中返回或者被取消），然后回收已终止线程占用的所有内存资源。如果线程简答地从线程例程返回，retval就包含返回码。如果线程被取消，由retval指定的内存单元就设置为PTHREAD_CANCELED。
```c
//The  pthread_join()  function waits for the thread specified by thread to terminate. 
//If that thread has already terminated, then pthread_join() returns immediately.  
//The thread specified by thread must be joinable.
int pthread_join(pthread_t thread, void **retval);
```
pthread_join只能等待一个指定的线程终止。

#### 分离线程
在任何时间点，线程是可结合的（joinable）或者是分离的（detached）：
* 一个可结合的线程能够被其它线程回收和杀死，在被其它线程回收（调用pthread_join）之前，它的内存资源是不释放的。
* 一个分离的线程是不能被其它线程回收或者杀死的，它的内存资源在它终止后由系统自动释放。

默认创建的线程是可结合的，为了避免内存泄露，每个可结合线程都应该要么被其它线程显示地回收，要么通过调用pthread_detach函数被分离：
```c
int pthread_detach(pthread_t thread);
```
>* 多数情况下推荐使用分离的线程，避免内存泄露。


### 线程同步
在多线程并发模型中，线程间共享数据非常容易，但是操作系统在调度这些线程时，并不默认提供对共享数据访问的保护机制。如果多个线程都只是单纯的读取共享数据，那么没有任何问题。如果有线程需要写入新数据，那么多个线程之间的数据访问就存在竞争关系。考虑变量增量操作的情况，增量操作通常分解为下面三个步骤：
* 1.从内存单元读入寄存器。
* 2.在寄存器中对变量做增量操作。
* 3.把新的值写回内存单元。

这几步操作并不是原子操作，多个线程同时访问时变量更新的最终结果取决于线程调度时这几个步骤的交叉执行顺序，也就意味着存在竞争条件，很可能出现数据不一致。因此我们需要做线程间的同步，来保证多个线程对共享数据的访问是安全一致的。


## 基于多线程的并发服务器案例
参考：https://github.com/hebostary/Notes/blob/master/csapp_v3/c12/echoservert_pre.c


# 参考链接
[Schedule, 15-213: Intro to Computer Systems, Fall 2014](http://www.cs.cmu.edu/afs/cs/academic/class/15213-f14/www/schedule.html)

[Course, 2015 Fall: 15-213	Introduction to Computer Systems](https://scs.hosted.panopto.com/Panopto/Pages/Sessions/List.aspx?#folderQuery=%2215213%22&folderID=%22b96d90ae-9871-4fae-91e2-b1627b43e25e%22&sortColumn=1&sortAscending=true&maxResults=10)

[Lab, Computer Systems: A Programmer's Perspective, 3/E](http://csapp.cs.cmu.edu/3e/labs.html)

[Attack lab implement](https://www.jianshu.com/p/db731ca57342)

[APUE](http://www.apuebook.com)