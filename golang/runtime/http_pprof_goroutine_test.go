package runtime

import (
	"testing"
)

// go test http_pprof_goroutine* pprof_common.go -v
func TestPprofGoroutine(t *testing.T) {
	http_pprof_goroutine()
}
