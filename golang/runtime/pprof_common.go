package runtime

import (
	"fmt"
	"log"
	"net/http"
	_ "net/http/pprof"
)

func run_pprof_server() {
	fmt.Println("Entering run_pprof_server")
	if err := http.ListenAndServe(":6060", nil); err != nil {
		log.Fatal(err)
	}
	fmt.Println("Exiting run_pprof_server")
}
