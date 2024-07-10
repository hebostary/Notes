package runtime

import "runtime/debug"
import "runtime/pprof"
import "os"
import "testing"

func print_stack() {
	debug.PrintStack()
}

func pprof_lookup() {
	pprof.Lookup("goroutine").WriteTo(os.Stdout, 1)
}

// go test runtime_test.go -v
func TestPrintStack(t *testing.T) {
	print_stack()
}

func TestPprofLookup(t *testing.T) {
	pprof_lookup()
}
