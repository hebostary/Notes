package runtime

import (
	"testing"
)

// go test http_pprof_heap*.go pprof_common.go -v
func TestPprofHeap(t *testing.T) {
	http_pprof_heap()
}
