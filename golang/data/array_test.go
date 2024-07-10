package compiler

import (
	"fmt"
	"testing"
	"time"
)

func array() {
	//声明数组的三种方式
	//声明数组时必须指定长度，且长度是数组类型的一部分；或者数组长度可以被推导出来
	var a0 [3]int
	var a1 = [3]int{1, 2, 3}
	var a2 = [...]int{1, 2, 3, 4}

	fmt.Printf("Type of a0: %T, address: %p\n", a0, &a0) //[3]int
	fmt.Printf("Type of a1: %T\n", a1)                   //[3]int
	fmt.Printf("Type of a2: %T, address: %p\n", a2, &a2) //[4]int, address: 0xc000112020

	//数组在赋值和函数调用时的形参传递都是值传递，即会拷贝一份副本
	var a3 = a2
	a3[1] = 100
	fmt.Println(a2)                                      //[1 2 3 4]
	fmt.Println(a3)                                      //[1 100 3 4]
	fmt.Printf("Type of a3: %T, address: %p\n", a3, &a3) //[4]int, address: 0xc0000241c0
}

func TestArray(t *testing.T) {
	array()

	time.Sleep(time.Second)
}
