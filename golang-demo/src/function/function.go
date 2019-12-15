package function

import (
	"fmt"
	"math"
)

/*
Go function may be closures. A closures is a function value that references vaiables from outside its body.
The function may access and assign the referenced variables; in this sense the function is "bound" to the variables.
*/
func adder() func(int) int {
	sum := 0
	return func(x int) int {
		sum += x
		return sum
	}
}

func funcClosures() {
	fmt.Println("=========Enter, funcClosures()==========")
	defer fmt.Println("=========Exit, funcClosures()==========")

	pos, neg := adder(), adder()
	for i := 0; i < 10; i++ {
		fmt.Println(
			pos(i),
			neg(-2*i),
		)
	}

	fmt.Println("Print fibonacci...")
	f := fibonacci()
	for i := 0; i < 10; i++ {
		fmt.Println(f())
	}
}

func fibonacci() func() int {
	lastvalue, currvalue := -1, 0
	return func() int {
		if currvalue == 0 {
			currvalue = 1
			return 0
		} else if currvalue == 1 && lastvalue == -1 {
			lastvalue = 0
			return 1
		} else if currvalue == 1 && lastvalue == 0 {
			lastvalue = currvalue
			return 1
		} else {
			tmp := lastvalue
			lastvalue = currvalue
			currvalue += tmp
			return currvalue
		}
	}
}

/*
Function values may be used as function arguments and return values.
*/
func compute(fn func(float64, float64) float64) float64 {
	return fn(3, 4)
}

func funcTest1() {
	fmt.Println("=========Enter, FuncTest1()==========")
	defer fmt.Println("=========Exit, FuncTest1()==========")

	hypot := func(x, y float64) float64 {
		return math.Sqrt(x*x + y*y)
	}

	fmt.Println(hypot(5, 12))
	fmt.Println(compute(hypot))
	fmt.Println(compute(math.Pow))

}

func Test() {
	funcTest1()
	funcClosures()
}
