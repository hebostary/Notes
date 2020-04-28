package mem

import (
	"fmt"
	"runtime"
)

/*
FUNCTION: Go程序内存管理
*/
var stat runtime.MemStats

func outMemSize() {
	runtime.ReadMemStats(&stat)
	// HeapSys is bytes of heap memory obtained from the OS.
	// Init 63 MB
	fmt.Println("------------------------------------------------------------------")
	fmt.Printf("   The largest size the heap has had:  %v KB.\n", stat.HeapSys/1024)

	// HeapAlloc is bytes of allocated heap objects.
	fmt.Printf("          The allocated heap objects:  %v KB.\n", stat.HeapAlloc/1024)

	// HeapObjects is the number of allocated heap objects.
	fmt.Printf("The number of allocated heap objects:  %v.\n", stat.HeapObjects)

	// 225 bytes
	fmt.Printf("               The heap objects size:  %v bytes.\n", stat.HeapAlloc/stat.HeapObjects)

	fmt.Println("------------------------------------------------------------------")
	// HeapIdle is bytes in idle (unused) spans.
	fmt.Printf("  The size of in idle (unused) spans:  %v KB.\n", stat.HeapIdle/1024)

	// HeapInuse is bytes in in-use spans.
	fmt.Printf("         The size of in in-use spans:  %v KB.\n", stat.HeapInuse/1024)

	fmt.Printf("                    Total spans size:  %v KB.\n", (stat.HeapInuse+stat.HeapIdle)/1024)
	fmt.Println("------------------------------------------------------------------")
}

func Test() {
	outMemSize()
	outZeroSizeObj()
}
