package data

import (
	"fmt"
	"testing"
)

/*
在Go语言中，切片的复制也是值复制，只不过复制的是SliceHeader结构，底层指针任然指向相同的底层数组。
append操作会根据实际情况对底层数组进行扩容，扩容的基本规则：
 1. 如果新申请的容量大于2倍的旧容量，最终容量就是新申请的容量
 2. 如果新申请的容量小于等于于2倍的旧容量：
    2.1 如果旧切片的长度小于1024，最终容量就是旧容量的2倍
    2.2 如果旧切片的长度大于等于1024，最终容量就是旧容量的基础上循环增加旧容量的1/4，直到最终容量大于等于新申请的容量
 3. 如果最终容量过大导致计算溢出，则最终容量就是新申请的容量
*/
func slice() {
	var s0 []int
	var s1 = []int{1, 2, 3, 4}
	var s2 = make([]int, 4)
	var s3 []int = make([]int, 4, 7)
	fmt.Printf("Type of s0: %T, len: %d, cap: %d\n", s0, len(s0), cap(s0)) //[]int, len: 0, cap: 0
	fmt.Printf("Type of s1: %T, len: %d, cap: %d\n", s1, len(s1), cap(s1)) //[]int, len: 4, cap: 4
	fmt.Printf("Type of s2: %T, len: %d, cap: %d\n", s2, len(s2), cap(s2)) //[]int, len: 4, cap: 4
	fmt.Printf("Type of s3: %T, len: %d, cap: %d\n", s3, len(s3), cap(s3)) //[]int, len: 4, cap: 7

	//s3和s4共享底层数组，修改s4会影响s3
	s3[3] = 100
	var s4 = s3[1:3]
	s4[1] = 200
	fmt.Println(s3)                           //[0 0 200 100]
	fmt.Println(s4)                           //[0 200]
	fmt.Printf("s3[2] address: %p\n", &s3[2]) //0xc00001c110
	fmt.Printf("s4[1] address: %p\n", &s4[1]) //0xc00001c110

	//使用copy函数可以完整的拷贝一个切片，拷贝后的切片和原切片不再共享底层数组
	s5 := make([]int, len(s4), cap(s4)*2)
	count := copy(s5, s4)
	fmt.Printf("Copied %d elements\n", count) //Copied 2 elements
	fmt.Printf("s5[1] address: %p\n", &s5[1]) //s5[1] address: 0xc0000262a8

	//优雅的删除切片中的元素
	s3 = append(s3, 1, 2, 3, 4, 5, 6, 7, 8, 9)                             //底层数组扩容到2倍
	fmt.Printf("Type of s3: %T, len: %d, cap: %d\n", s3, len(s3), cap(s3)) //[]int, len: 13, cap: 14
	fmt.Println(s3)                                                        //[0 0 200 100 1 2 3 4 5 6 7 8 9]

	fmt.Printf("s3[1] address: %p\n", &s3[1]) //s3[1] address: 0xc00002a078
	s3 = s3[1:]                               //删除第一个元素
	fmt.Println(s3)                           //[0 200 100 1 2 3 4 5 6 7 8 9]
	s3 = s3[:len(s3)-1]                       //删除最后一个元素
	fmt.Println(s3)                           //[0 200 100 1 2 3 4 5 6 7 8]
	a := int(len(s3) / 2)
	fmt.Printf("Type of s3: %T, len: %d, cap: %d\n", s3, len(s3), cap(s3)) //[]int, len: 10, cap: 13
	s3 = append(s3[:a], s3[a+1:]...)                                       //删除中间元素
	fmt.Println(s3)                                                        //[0 200 100 1 2 4 5 6 7 8]
	fmt.Printf("Type of s3: %T, len: %d, cap: %d\n", s3, len(s3), cap(s3)) //[]int, len: 10, cap: 13
	fmt.Printf("s3[0] address: %p\n", &s3[0])                              //s3[0] address: 0xc00002a078，说明并未发生内存拷贝
}

func TestSlice(t *testing.T) {
	slice()
}
