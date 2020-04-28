package main

/*
go build -gcflags '-m -m' -o ./bin/escape perf/escape
# perf/escape
src/perf/escape/main.go:3:6: can inline stack as: func() int { i := 10; return i }
src/perf/escape/main.go:8:6: can inline heap as: func() *int { j := 10; return &j }
src/perf/escape/main.go:14:6: can inline main as: func() { stack(); heap() }
src/perf/escape/main.go:15:7: inlining call to stack func() int { i := 10; return i }
src/perf/escape/main.go:16:6: inlining call to heap func() *int { j := 10; return &j }
src/perf/escape/main.go:10:2: j escapes to heap:
src/perf/escape/main.go:10:2:   flow: ~r0 = &j:
src/perf/escape/main.go:10:2:     from &j (address-of) at src/perf/escape/main.go:11:9
src/perf/escape/main.go:10:2:     from return &j (return) at src/perf/escape/main.go:11:2
src/perf/escape/main.go:10:2: moved to heap: j
*/

func stack() int {
	// 变量 i 会在栈上分配
	i := 10
	return i
}
func heap() *int {
	//逃逸， 变量 j 会在堆上分配
	j := 10
	return &j
}

/*
多级间接赋值容易导致逃逸。这里的多级间接指的是，对某个引用类对象中的引用类成员进行赋值。
Go 语言中的引用类数据类型有 func, interface, slice, map, chan, *Type(指针)。
记住公式 Data.Field = Value，如果 Data, Field 都是引用类的数据类型，则会导致 Value 逃逸。
这里的等号 = 不单单只赋值，也表示参数传递。

作者：达菲格
链接：https://www.jianshu.com/p/518466b4ee96
来源：简书
著作权归作者所有。商业转载请联系作者获得授权，非商业转载请注明出处。
*/
func testEscape1(i int)  {}
func testEscape2(i *int) {}

func main() {
	stack()
	heap()

	k, l, m, n := 1, 1, 1, 1
	t1, t2 := testEscape1, testEscape2

	//直接调用
	testEscape1(k)  //不逃逸
	testEscape2(&l) //不逃逸

	//间接调用
	t1(m) //不逃逸，m在栈上分配
	/*
		t2的类型是 func(*int)，属于引用类型，参数 *int 也是引用类型，
		则调用 t2(&n) 形成了为 t2 的参数(成员) *int 赋值的现象，即 t2.i = &j 会导致逃逸。
	*/
	t2(&n) //逃逸，n在堆上分配

}
