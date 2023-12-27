goroutine:
	go test http_pprof_goroutine* pprof_common.go -v

heap:
	go test http_pprof_heap*.go pprof_common.go -v

cpu:
	go test pprof_cpu*.go -v

runtime:
	go test runtime_test.go -v

trace:
	go test http_pprof_trace* pprof_common.go http_pprof_heap.go -v

all: runtime

