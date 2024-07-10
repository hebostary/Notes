package runtime

import (
	"log"
	"os"
	"runtime/pprof"
)

func busyLoop() {
	for i := 0; i < 1000000000; i++ {
	}
}

func pprof_cpu() {
	f, err := os.Create("cpu.prof")
	if err != nil {
		log.Fatal(err)
	}
	defer f.Close()

	if err := pprof.StartCPUProfile(f); err != nil {
		log.Fatal(err)
	}
	defer pprof.StopCPUProfile()
	busyLoop()
}
