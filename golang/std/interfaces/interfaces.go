package interfaces

import (
	"fmt"
	"math"
)

/*
Note: Stringers
1.One of the most ubiquitous interfaces is Stringer defined by the fmt package.
type Stringer interface {
    String() string
}
A Stringer is a type that can describe itself as a string. The fmt package (and many others) look for this interface to print values.
*/

type Person struct {
	Name string
	Age  int
}

func (p Person) String() string {
	return fmt.Sprintf("%v (%v years)", p.Name, p.Age)
}

type IPAddr [4]byte

func (p IPAddr) String() string {
	return fmt.Sprintf("%v.%v.%v.%v", p[0], p[1], p[2], p[3])
}

func stringersTest() {
	fmt.Println("=========Enter, InterfaceTest.stringersTest()==========")
	defer fmt.Println("=========Exit, InterfaceTest.stringersTest()==========")
	a := Person{"Arthur Dent", 42}
	z := Person{"Zaphod Beeblebrox", 9001}
	fmt.Println(a, z)

	hosts := map[string]IPAddr{
		"loopback":  {127, 0, 0, 1},
		"googleDNS": {8, 8, 8, 8},
	}
	for name, ip := range hosts {
		fmt.Printf("%v: %v\n", name, ip)
	}
}

/*
Note:
1.A type switch is a construct that permits several type assertions in series.
A type switch is like a regular switch statement, but the cases in a type switch specify types (not values), and those values are compared against the type of the value held by the given interface value.
The declaration in a type switch has the same syntax as a type assertion i.(T), but the specific type T is replaced with the keyword type.
*/

func do(i interface{}) {
	switch v := i.(type) {
	case int:
		fmt.Printf("Twice %v is %v\n", v, v*2)
	case string:
		fmt.Printf("%q is %v bytes long\n", v, len(v))
	default:
		fmt.Printf("Unknown type %T!\n", v)
	}
}

func typeSwitchesTest() {
	fmt.Println("=========Enter, InterfaceTest.typeSwitchesTest()==========")
	defer fmt.Println("=========Exit, InterfaceTest.typeSwitchesTest()==========")
	do(1)
	do("hello")
	do(true)
}

/*
Note:
1.Type assertions: A type assertion provides access to an interface value's underlying concrete value.
	t := i.(T)
This statement asserts that the interface value i holds the concrete type T and assigns the underlying T value to the variable t.
If i does not hold a T, the statement will trigger a panic.
To test whether an interface value holds a specific type, a type assertion can return two values: the underlying value and a boolean value that reports whether the assertion succeeded.
	t, ok := i.(T)
If i holds a T, then t will be the underlying value and ok will be true.
If not, ok will be false and t will be the zero value of type T, and no panic occurs.
Note the similarity between this syntax and that of reading from a map.
*/
func typeAssertionTest() {
	fmt.Println("=========Enter, InterfaceTest.typeAssertionTest()==========")
	defer fmt.Println("=========Exit, InterfaceTest.typeAssertionTest()==========")
	var i interface{} = "hello"

	s := i.(string)
	fmt.Println(s) //hello

	s, ok := i.(string)
	fmt.Println(s, ok) //hello true

	f, ok := i.(float64)
	fmt.Println(f, ok) //0 false

	//f = i.(float64)
	//fmt.Println(f) //panic: interface conversion: interface {} is string, not float64
}

/*
Note:
1.An interface type is defined as a set of method signatures.
2.A value of interface type can hold any value that implements those methods.
3.Interface values can be thought of as a tuple of a value and a concrete type: (value, type)
4.A nil interface value holds neither value nor concrete type. Calling a method on a nil interface is a run-time error because there is no type inside the interface tuple to indicate which concrete method to call.
5.An empty interface may hold values of any type. (Every type implements at least zero methods.) Empty interfaces are used by code that handles values of unknown type. For example, fmt.Print takes any number of arguments of type interface{}.
*/

type I interface {
	M()
}

type T struct {
	S string
}

type F float64

func (v *T) M() {
	if v == nil {
		fmt.Println("<nil>")
		return
	}
	fmt.Println("Value of S: ", v.S)
}

func (f F) M() {
	fmt.Println("Value of f: ", f)
}

func describe(i interface{}) {
	fmt.Printf("(%v, %T)\n", i, i)
}

func interfaceTest() {
	fmt.Println("=========Enter, InterfaceTest.interfaceTest()==========")
	defer fmt.Println("=========Exit, InterfaceTest.interfaceTest()==========")

	var i I
	i = &T{"hello hunk"}
	describe(i)
	i.M()

	i = F(math.Pi)
	describe(i)
	i.M()

	var t *T
	i = t
	describe(i)
	i.M()

	fmt.Println("Test empty interface")
	var a interface{}
	describe(a)

	a = 42
	describe(a)

	a = "hello"
	describe(a)
}

func Test() {
	interfaceTest()
	typeAssertionTest()
	typeSwitchesTest()
	stringersTest()
}
