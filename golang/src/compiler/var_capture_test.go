package compiler

import (
	"fmt"
	"testing"
	"time"
)

func var_capture() {
	a := 0
	b := 1
	go func() {
		fmt.Println(a, b)
	}()
	a = 99

	time.Sleep(100 * time.Millisecond)
}

//go tool compile -m=2 var_capture_test.go
//go test var_capture_test.go -v

func TestVarCapture(t *testing.T) {
	var_capture()
}
