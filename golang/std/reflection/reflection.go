package reflection

import(
	"fmt"
	//"io"
	//"os"
	"reflect"
	"strconv"
	//"time"
	"strings"
)

/* 反射需要谨慎使用，主要有以下原因：
1.基于反射的代码比较脆弱。对于每一个会导致编译错误的问题，在反射中都有与之对应的问题，不同的是编译器会在build时报告错误，
而反射则在程序运行的时候才会跑出panic异常。
因此，在反射程序中需要小心的检查：每个reflect.Value的对于值的类型；是否可取地址；是否可被修改，如Test3()中的demo。
避免这种因反射而导致的脆弱性问题的最好方法是：最好不要在包或接口中直接暴露反射类型(reflect.Value)，而是将反射相关的使用
控制在包内部，这样可以限制一些非法输入。

2.反射操作不能做静态类型检查，而且大量的反射代码难以理解。

3.基于反射的代码通常比正常的代码运行速度慢1到2个数量级。
*/

type Move struct {
	Title, Subtitle string
	Year            int
	Color           bool
	Actor           map[string]string
	Oscars          []string
	Sequel          *string
}

var strangelove = Move{
	Title: "Dr. Strangelove",
	Subtitle: "How I learned to stop....",
	Year: 1964,
	Color: false,
	Actor: map[string]string{
		"Dr . Strangelove" : "Peter Sellers",
		"Maj . T.J King" : "Slim Pickens",
	},
	Oscars: []string{
		"Best Actor(Nomin)",
		"Best Adapted Screenplay (Nomin)",
	},
}

//尽量避免在一个包中暴露和反射相关的接口，所以用Display进程封装，只是接收interface类型的入参
func Display(name string, x interface{})  {
	fmt.Printf("Display %s (%T):\n", name , x)
	display(name, reflect.ValueOf(x))
}

func display(path string, v reflect.Value)  {
	switch v.Kind() {
	case reflect.Invalid:
		fmt.Printf("%s = invalid\n", path)
	case reflect.Slice, reflect.Array:
		for index := 0; index < v.Len(); index++ {
			display(fmt.Sprintf("%s[%d]", path, index), v.Index(index))
		}
	case reflect.Struct:
		for index := 0; index < v.NumField(); index++ { //NumField()给出结构体中成员的数量
			fieldPath := fmt.Sprintf("%s.%s", path, v.Type().Field(index).Name)
			display(fieldPath, v.Field(index))
		}
	case reflect.Map:
		for _, key := range v.MapKeys() {
			display(fmt.Sprintf("%s[%s]", path, formatAtom(key)), v.MapIndex(key))
		}
	case reflect.Ptr:
		if v.IsNil() {
			fmt.Printf("%s = nil\n", path)
		} else {
			display(fmt.Sprintf("(%s)", path), v.Elem())
		}
	case reflect.Interface:
		if v.IsNil() {
			fmt.Printf("%s = nil\n", path)
		} else {
			fmt.Printf("%s.type = %s\n", path, v.Elem().Type())
			display(path+".value", v.Elem())
		}
	default:
		fmt.Printf("%s = %s\n", path, formatAtom(v))
	}
}

func formatAtom(v reflect.Value) string {
	switch v.Kind() {
	case reflect.Invalid:
		return "invalid"
	case reflect.Int, reflect.Int8, reflect.Int16, reflect.Int32, reflect.Int64:
		return strconv.FormatInt(v.Int(), 10)
	case reflect.Uint, reflect.Uint8, reflect.Uint16, reflect.Uint32, reflect.Uint64:
		return strconv.FormatUint(v.Uint(), 10)
	case reflect.Bool:
		return strconv.FormatBool(v.Bool())
	case reflect.String:
		return strconv.Quote(v.String())
	case reflect.Chan, reflect.Func, reflect.Ptr, reflect.Slice, reflect.Map:
		return v.Type().String() + " 0x" + strconv.FormatUint(uint64(v.Pointer()), 16)
	default:
		return v.Type().String() + " value"
	}
}

type Student struct{
	name string
	age  int
}

func (s Student) Show()  {
	fmt.Printf("name:%s, age:%d\n", s.name, s.age)
}

//打印一个类型的方法集
func PrintMethod(x interface{})  {
	v := reflect.ValueOf(x)
	t := v.Type()
	fmt.Printf("type %s\n", t)
	
	for i := 0; i < v.NumMethod(); i++ {
		methType := v.Method(i).Type()
		fmt.Printf("func (%s) %s%s\n", t, t.Method(i).Name,
			strings.TrimPrefix(methType.String(), "func"))
		var in []reflect.Value
		//调用类型的方法
		fmt.Printf("Call func %s, result: ", t.Method(i).Name)
		v.Method(i).Call(in)
	}
}

func Test4()  {
	fmt.Println("=========reflection.Test4()==========")
	stu := Student{
		name:"hunk",
		age:25,
	}
	PrintMethod(stu)
}

func Test3()  {
	fmt.Println("=========reflection.Test3()==========")
	x := 2
	a := reflect.ValueOf(x)
	//CanAddr()可以反映一个变量是否可以被取地址，但不能反映是否能被修改,  CanSet()可以检查对应的relfect.Value是否可取地址并且可修改
	fmt.Println(a.CanAddr()) //false
	fmt.Println(a.CanSet()) //false
	b := reflect.ValueOf(&x)
	fmt.Println(b.CanAddr())  //false

	//通过反射修改变量的值
	c := reflect.ValueOf(&x).Elem() //获取变量可取地址的Value
	//方法1
	fmt.Println(c.CanAddr()) //true
	px := c.Addr().Interface().(*int)
	*px = 3
	fmt.Println(x) //3

	//方法2
	c.Set(reflect.ValueOf(4))
	fmt.Println(x) //4

	c.SetInt(6)    //SetInt,SetUint,SetString,SetFloat....
	fmt.Println(x) //6

	//**********必须保证底层类型一致
	//c.Set(reflect.ValueOf(int64(5))) //panic: reflect.Set: value of type int64 is not assignable to type int
	//a.Set(reflect.ValueOf(5))   syntax error: unexpected panic at end of statement
	//b.Set(reflect.ValueOf(5))  panic: reflect: reflect.Value.Set using unaddressable value

	//*********注意：对于一个引用interface{}类型的reflect.Value调用SetInt会导致panic异常，可以通过reflect.ValueOf来set新值
	var y interface{}
	ry := reflect.ValueOf(&y).Elem()
	//ry.SetInt(3)  panic: reflect: call of reflect.Value.SetInt on interface Value
	//py := ry.Addr().Interface().(*int)
	//*py = 3   panic: interface conversion: interface {} is *interface {}, not *int
	ry.Set(reflect.ValueOf(3))
	fmt.Println(y)  //3
	ry.Set(reflect.ValueOf("hello reflect"))
	fmt.Println(y) //hello reflect
}

func Test2()  {
	fmt.Println("=========reflection.Test2()==========")
	Display("strangelove", strangelove)
}

func Test1()  {
	fmt.Println("=========reflection.Test1()==========")
	t := reflect.TypeOf(3)
	fmt.Println(t.String())

	//var w io.Writer = os.Stdout
	s := "hello hunk" 
	fmt.Println(reflect.TypeOf(s)) //string
	v := reflect.ValueOf(s) //reflect.Value
	fmt.Println(v)

	t = v.Type() //a reflect.Type
	fmt.Println(t.String()) //string
}

func Test() {
	Test1()
	Test2()
	Test3()
	Test4()
}