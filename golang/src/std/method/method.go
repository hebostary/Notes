package method

import (
	"fmt"
	"math"
)

/*
Note:
1.Go does not have classes, but we can define methods on types such as struct and non-struct types. A method is a function with a special receiver argument.
2.We can only declare a method with a receiver whose type is defined in the same package as the method.
3.We can declare methods with pointer receivers. Methods with pointer receivers can modify the value to which the receiver points. Since methods often need to modify their receiver, pointer receivers are more common than value receivers.
4.With a value receiver, the Scale method operates on a copy of the original Vertex value. (This is the same behavior as for any other function argument.)
5.For the statement v.Scale(10), even though v is a value and not a pointer, the method with the pointer receiver is called automatically. Ifit is just a function with a pointer argument, we must pass a pointer.
6.Two reasons to use a pointer receiver:
	(1).The first is so that the method can modify the value that its receiver points to.
	(2).The second is to avoid copying the value on each method call. This can be more efficient if the receiver is a large struct, for example.
*/

type Vertex struct {
	X, Y float64
}

func (v Vertex) Abs() float64 {
	return math.Sqrt(v.X*v.X + v.Y*v.Y)
}

func (v Vertex) ScaleValue(f float64) {
	v.X = v.X * f
	v.Y = v.Y * f
}

func (v *Vertex) Scale(f float64) {
	v.X = v.X * f
	v.Y = v.Y * f
}

func ScaleFunc(v *Vertex, f float64) {
	v.X = v.X * f
	v.Y = v.Y * f
}

func vertexMethod() {
	fmt.Println("=========Enter, MethodTest.vertexMethod()==========")
	defer fmt.Println("=========Exit, MethodTest.vertexMethod()==========")
	v := Vertex{5, 12}
	fmt.Println(v.Abs()) //13
	v.Scale(10)
	fmt.Println(v.Abs()) //130, if the Scale receiver is not a pointer, the result is 13.

	v1 := &Vertex{3, 4}
	v1.Scale(10)
	fmt.Println(v1.Abs()) //50

	//ScaleFunc(v, 10)  //compile error: cannot use v (type Vertex) as type *Vertex in argument to ScaleFunc

	v2 := &Vertex{3, 4}
	v2.ScaleValue(10)     //ScaleValue's receiver is value type, but we can use it with a pointer
	fmt.Println(v2.Abs()) //5
}

func Test() {
	vertexMethod()
}
