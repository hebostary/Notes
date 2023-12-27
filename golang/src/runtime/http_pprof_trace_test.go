package runtime

import (
	"testing"
)

// go test http_pprof_trace* pprof_common.go -v
func TestPprofTrace(t *testing.T) {
	http_pprof_trace()
}
