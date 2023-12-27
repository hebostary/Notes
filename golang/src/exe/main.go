package main

import (
	"flag"
	"fmt"
	"perf/gmp"
	"runtime"
	"time"

	// demo code for golang std library
	"std/error"
	"std/function"
	"std/goruntine"
	"std/interfaces"
	"std/method"
	"std/mutex"
	"std/reflection"
	"std/slice"
	structTest "std/struct"
	"std/unsafed"

	//"std/io"

	// other demo code
	"self/chat"
	"self/memo"
	"self/memo1"
	"self/walk"
	"self/word"

	//"self/crawl"

	// demo code for 3rd framework
	"3rd/muxdemo"

	"perf/mem"
)

/*
Test pointer
*/
func Pointer() {
	fmt.Println("**********Test - Test pointer*********")
	i, j := 100, 200

	p := &i
	fmt.Println("Adress: ", p, ", value: ", *p)
	*p = 1000
	fmt.Println("Adress: ", p, ", value: ", *p, ", ", i)

	p = &j
	*p = *p / 10
	fmt.Println(j)
}

/*
Test defer statement
*/
func Defer() {
	fmt.Println("**********Test - Test defer statement*********")
	for i := 0; i < 5; i++ {
		defer fmt.Println("The defer statement ", i, " executing.")
	}
	fmt.Println("The Defer function return.")
}

/*
Test switch statement
*/
func SwitchCase() {
	fmt.Println("**********Test - Test switch statement*********")
	switch os := runtime.GOOS; os {
	case "darwin":
		fmt.Println("OS X.")
	case "linux":
		fmt.Println("Linux.")
	default:
		fmt.Println("%s", os)
	}

	t := time.Now()
	switch {
	case t.Hour() < 12:
		fmt.Println("Good morning.")
	case t.Hour() < 17:
		fmt.Println("Good afternoon.")
	default:
		fmt.Println("Good evening.")
	}
}

func main() {
	var module string
	flag.StringVar(&module, "module", "default", "module name")
	flag.Parse()

	module_list := "module list: default, slice, mapTest, function, " +
		"structTest, method, interfaces, error, io, goruntine, crawl, walk, " +
		"chat, mutex, memo, memo1, word, reflect, unsafed, muxdemo, mem, " +
		"gmp"

	switch module {
	case "default":
		SwitchCase()
		Defer()
		Pointer()
	case "slice":
		slice.Test()
	case "mapTest":
		Test()
	case "function":
		function.Test()
	case "structTest":
		structTest.Test()
	case "method":
		method.Test()
	case "interfaces":
		interfaces.Test()
	case "error":
		error.Test()
	//case "io":
	//	io.Test()
	case "goruntine":
		goruntine.Test()
	//case "crawl":
	//	crawl.Test()
	case "walk":
		walk.Test()
	case "chat":
		chat.Test()
	case "mutex":
		mutex.Test()
	case "memo":
		memo.Test()
	case "memo1":
		memo1.Test()
	case "word":
		word.Test()
	case "reflect":
		reflection.Test()
	case "unsafed":
		unsafed.Test()
	case "muxdemo":
		muxdemo.Test()
	case "mem":
		mem.Test()
	case "gmp":
		gmp.Test()
	default:
		fmt.Println(module_list)
	}
}
