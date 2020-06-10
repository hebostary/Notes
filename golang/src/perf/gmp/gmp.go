package gmp

import (
	"fmt"
	"runtime"
)

func testGOMAXPROCS() {
	fmt.Printf("Number of CPU: %v \n", runtime.NumCPU())
	fmt.Printf("Number of Go routine: %v \n", runtime.NumGoroutine())
}

//Test ...
func Test() {
	testGOMAXPROCS()
}
