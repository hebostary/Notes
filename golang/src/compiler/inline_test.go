package compiler

import (
	"testing"
)

func max(a, b int) int {
	if a > b {
		return a
	}
	return b
}

var Result int

// go tool compile -m=2 inline_test.go
// go test inline_test.go -bench=. -count=10

func BenchmarkMaxInline(b *testing.B) {
	var r int
	for i := 0; i < b.N; i++ {
		r = max(-1, i)
	}
	Result = r
}

//go:noinline
func BenchmarkMaxNoInline(b *testing.B) {
	var r int
	for i := 0; i < b.N; i++ {
		r = max(-1, i)
	}
	Result = r
}
