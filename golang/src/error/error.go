package error

import (
	"fmt"
	"math"
	"time"
)

/*
Newton's method to compute the square root most nearly.
*/

type ErrNegativeSqrt float64

func (e ErrNegativeSqrt) Error() string {
	return fmt.Sprintf("can't Sqrt negative number: %v", float64(e))
}

func sqrt(x float64) (float64, error) {
	if x < 0 {
		return 0, ErrNegativeSqrt(x)
	}
	z := float64(0.01)
	for i := 0; math.Abs(z*z-x) > 0.001; i++ {
		z -= (z*z - x) / (2 * z)
		fmt.Println("iterator order: ", i, ", x: ", z)
	}
	return z, nil
}

func errorTest2() {
	fmt.Println("=========Enter, ErrorTest.errorTest2()==========")
	defer fmt.Println("=========Exit, ErrorTest.errorTest2()==========")
	fmt.Println(sqrt(200))
	fmt.Println(sqrt(-12))
}

/*
Note:
Go programs express error state with error values.
The error type is a built-in interface similar to fmt.Stringer:

type error interface {
    Error() string
}
(As with fmt.Stringer, the fmt package looks for the error interface when printing values.)
Functions often return an error value, and calling code should handle errors by testing whether the error equals nil.
*/

type RunError struct {
	When time.Time
	What string
}

func (v *RunError) Error() string {
	return fmt.Sprintf("at %v, %s", v.When, v.What)
}

func run() error {
	return &RunError{
		time.Now(),
		"it didn't work",
	}
}

func errorTest1() {
	fmt.Println("=========Enter, ErrorTest.errorTest1()==========")
	defer fmt.Println("=========Exit, ErrorTest.errorTest1()==========")
	if err := run(); err != nil {
		fmt.Println(err)
	}
}

func Test() {
	errorTest1()
	errorTest2()
}
