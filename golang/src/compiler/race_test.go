package compiler

import (
	"fmt"
	"testing"
	"time"
)

var count int = 0

func add() {
	count++
}

/*
这是一个典型的数据竞争，因为count++不是一个原子操作，它至少包含三个步骤：
1. 读取count的值
2. 将count的值加1
3. 将结果写入count

执行两次的结果：
=== RUN   TestRaceAdd
993
--- PASS: TestRaceAdd (0.10s)

=== RUN   TestRaceAdd
969
--- PASS: TestRaceAdd (0.10s)
*/
func race_add() {
	for i := 0; i < 1000; i++ {
		go add()
	}
	time.Sleep(100 * time.Millisecond)
	fmt.Println(count)
}

// go test race_test.go -v
func TestRaceAdd(t *testing.T) {
	race_add()
}

/*
Golang提供race工具用于数据竞争检测，它可以检测到上面的数据竞争，执行命令：go test -race race_test.go -v会看到WARNING: DATA RACE的提示。
但是race工具也有一些局限性：
1. 它只能检测到读写竞争，不能检测到写写竞争
2. 它只能检测到同一个变量的数据竞争，不能检测到不同变量之间的数据竞争
3. race工具会产生大量的误报，这是因为race工具只是简单的检测到了数据竞争，但是并不知道这些数据竞争是否会导致程序出错。

Golang race工具可以用于多个指令当中，比如：
	go test -race xxx
	go run -race xxx
	go build -race xxx
	go install -race xxx

注意：对于典型的程序，race检测会使程序内存使用量增加5-10倍，执行时间会增加2-20倍。
*/
