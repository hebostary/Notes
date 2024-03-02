package unsafed

import (
	"fmt"
	"unsafe"
)

type x struct {
	a bool
	b int16
	c []int
}

func unsafe_demo1() {
	fmt.Println("=========unsafed.Test()==========")
	z := x{
		a: false,
		b: 12,
		c: []int{1, 2},
	}
	//Sizeof函数返回的字节大小值包括数据结构中固定的部分，例如字符串对应结构体中的指针和字符串长度部分，不包括指针指向的字符串的实际内容
	//Alignof返回对应参数的类型需要对齐的倍数
	//Offsetof的参数必须是一个字段x.f，然后返回相对于x起始地址的字节偏移量，包括可能的内存空洞
	//这几个函数是安全的
	fmt.Printf("Sizeof(x)    = %d ,Alignof(x)      = %d\n", unsafe.Sizeof(z), unsafe.Alignof(z))
	fmt.Printf("Sizeof(x.a)  = %d ,Alignof(x.a)    = %d, Offsetoff(x.a) =  %d\n", unsafe.Sizeof(z.a), unsafe.Alignof(z.a), unsafe.Offsetof(z.a))
	fmt.Printf("Sizeof(x.b)  = %d ,Alignof(x.b)    = %d, Offsetoff(x.b) =  %d\n", unsafe.Sizeof(z.b), unsafe.Alignof(z.b), unsafe.Offsetof(z.b))
	fmt.Printf("Sizeof(x.c)  = %d ,Alignof(x.c)    = %d, Offsetoff(x.c) =  %d\n", unsafe.Sizeof(z.c), unsafe.Alignof(z.c), unsafe.Offsetof(z.c))
	/*
		Sizeof(x)    = 32 ,Alignof(x)      = 8
		Sizeof(x.a)  = 1 ,Alignof(x.a)    = 1, Offsetoff(x.a) =  0
		Sizeof(x.b)  = 2 ,Alignof(x.b)    = 2, Offsetoff(x.b) =  2
		Sizeof(x.c)  = 24 ,Alignof(x.c)    = 8, Offsetoff(x.c) =  8
		内存布局(64位系统)：
		---------------------------------
		| a |---|	b	|---------------|
		---------------------------------
		|			c(data)				|
		---------------------------------
		|			c(len)				|
		---------------------------------
		|			c(cap)				|
		---------------------------------
		总共占用32个字节，即4个机器字：
		a占用1个字节，bool类型对齐时的倍数为1，所以a后面有1个字节的内存空洞。
		b为int16类型，对齐倍数为2，所以后面有4个字节的内存空洞。
		slice的3个底层类型都是占用一个机器字

		如果将x中b和c的定义交换顺序，则Sizeof(x)=40，因为内存对齐占用5个机器字的空间，
		所以go(go version go1.11 darwin/amd64)暂时并没有对结构体中的成员的顺序进行优化，
		合理的定义结构体成员顺序可以使数据结构更紧凑，占用更少的空间
	*/
}
