package data

import (
	"fmt"
	"math/big"
	"testing"
)

/*
Go语言默认浮点数打印出的值是8位
*/
func float_print() {
	fmt.Println("Entering float_print")
	var f1 float32 = 1.123456789
	fmt.Println(f1) //1.1234568
}

/*
浮点数的加法运算由于需要调整指数，从而导致精度损失
*/
func float_sum() {
	fmt.Println("Entering float_sum")
	var sum float32
	for i := 0; i < 1000; i++ {
		sum += 0.1
	}
	fmt.Println(sum) //99.99905

	sum = 0
	for i := 0; i < 1000; i++ {
		sum += 0.01
	}
	fmt.Println(sum) //10.0001335
}

/*
加法运算和乘除法运算同时存在时，应该先进行乘除法运算，再进行加法运算，即减少加法运算的次数，可以避免精度损失
*/
func float_multi() {
	fmt.Println("Entering float_multi")
	var f1 float32 = 1.00231
	var f2 float32 = 0.0032
	var f3 float32 = 0.103
	var result float32 = f1*f2 + f1*f3
	fmt.Println(fmt.Sprintf("%f * %f + %f * %f = %f", f1, f2, f1, f3, result)) //0.106445

	result = f1 * (f2 + f3)
	fmt.Println(fmt.Sprintf("%f * (%f + %f) = %f", f1, f2, f3, result)) //0.106445
}

/*
计算一个大于10^99的斐波那契数
 1. 大数运算和普通int64相加或相乘不同的是，大数运算需要保留并处理进位
*/
func big_int() {
	fmt.Println("Entering big_int")
	a := big.NewInt(0)
	b := big.NewInt(1)
	var limit big.Int
	limit.Exp(big.NewInt(10), big.NewInt(99), nil)
	for a.Cmp(&limit) < 0 {
		a.Add(a, b)
		a, b = b, a
	}
	fmt.Println(fmt.Sprintf("a = %d", a)) //a = 1344719667586153181419716641724567886890850696275767987106294472017884974410332069524504824747437757
}

/*
big.Float的核心思想是把浮点数抓换位大整数运算
 1. big.Flaot仍然会损失精度，因为有限的位数无法表达无限的小数，可以通过设置prec存储数字的位数来提高精度
 2. prec设置的越大，精度越高，但是相应的计算花费也越高
 3. big.Float的运算结果是一个指针，需要通过new(big.Float)来创建
*/
func big_float() {
	fmt.Println("Entering big_float")
	var x1, y1 float64 = 10, 3
	z1 := x1 / y1
	fmt.Println(x1, y1, z1) //10 3 3.3333333333333335

	x2, y2 := big.NewFloat(10), big.NewFloat(3)
	z2 := new(big.Float).Quo(x2, y2)
	fmt.Println(x2, y2, z2) //10 3 3.3333333333333335

	//当prec设置为53的时候，其精度和float64相同
	//不设置prec的时候，默认精度和float64相同
	x2.SetPrec(53)
	y2.SetPrec(53)
	z2 = new(big.Float).Quo(x2, y2)
	fmt.Println(x2, y2, z2) //10 3 3.3333333333333335

	x2.SetPrec(100)
	y2.SetPrec(100)
	z2 = new(big.Float).Quo(x2, y2)
	fmt.Println(x2, y2, z2) //10 3 3.333333333333333333333333333332

}

// go test float_test.go -v
func TestFloat(t *testing.T) {
	float_print()
	float_sum()
	float_multi()
}

func TestBig(t *testing.T) {
	big_int()
	big_float()
}
