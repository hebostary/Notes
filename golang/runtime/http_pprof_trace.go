package runtime

import (
	"fmt"
	"time"
)

func http_pprof_trace() {
	fmt.Println("Entering http_pprof_trace")
	go run_pprof_server()

	go new_map_in_heap()

	a := make(chan int)
	for {
		time.Sleep(2 * time.Second)
		go func() {
			<-a
		}()
	}
}
