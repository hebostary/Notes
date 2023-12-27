package runtime

import (
	"fmt"
	"time"
)

func http_pprof_goroutine() {
	fmt.Println("Entering http_pprof_goroutine")
	go run_pprof_server()

	a := make(chan int)
	for {
		time.Sleep(2 * time.Second)
		go func() {
			<-a
		}()
	}
}
