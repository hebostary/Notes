package runtime

import (
	"fmt"
	_ "net/http/pprof"
)

var gMap = make(map[string]string)
var gSlice1 []byte
var gSlice2 []byte
var MiB = 1024 * 1024

func new_map_in_heap() {
	fmt.Println("Entering new_map_in_heap")
	for i := 0; i < 999999; i++ {
		gMap["KeyID"+fmt.Sprint(i)] = "value" + fmt.Sprint(i)
	}
	//让函数阻塞在这里不退出
	<-make(chan int)

	//避免分配的内存提前被回收
	fmt.Println("Size of gMap:", len(gMap))
	fmt.Println("Exiting new_map_in_heap")
}

func new_slice_in_heap1() {
	fmt.Println("Entering new_slice_in_heap1")
	gSlice1 = make([]byte, 12*MiB)
	new_slice_in_heap2()
	fmt.Println("Size of gSlice1:", len(gSlice1))
	fmt.Println("Exiting new_slice_in_heap1")
}

func new_slice_in_heap2() {
	fmt.Println("Entering new_slice_in_heap2")
	gSlice2 = make([]byte, 24*MiB)
	<-make(chan int)

	fmt.Println("Size of gSlice2:", len(gSlice2))
	fmt.Println("Exiting new_slice_in_heap2")
}

func http_pprof_heap() {
	go run_pprof_server()

	go new_map_in_heap()

	go new_slice_in_heap1()

	<-make(chan int)
}
