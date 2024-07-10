package goruntine

import (
	"fmt"
	"time"

	"golang.org/x/tour/tree"
)

//Equivalent Binary Trees
// Walk walks the tree t sending all values from the tree to the channel ch.
func Walk(t *tree.Tree, ch chan int) {
	if t == nil {
		fmt.Println("Tree is null.")
		return
	}

	if t.Left != nil {
		Walk(t.Left, ch)
	}
	ch <- t.Value
	if t.Right != nil {
		Walk(t.Right, ch)
	}
}

// Same determines whether the trees t1 and t2 contain the same values.
func Same(t1, t2 *tree.Tree) bool {
	ch1 := make(chan int, 10)
	ch2 := make(chan int, 10)
	go Walk(t1, ch1)
	go Walk(t2, ch2)
	for i := 0; i < 10; i++ {
		v1 := <-ch1
		v2 := <-ch2
		fmt.Println("v1: ", v1, " v2: ", v2)
		if v1 != v2 {
			fmt.Println("end: v1: ", v1, " v2: ", v2)
			return false
		}
	}
	return true
}

func goruntineTest5() {
	fmt.Println("=========Enter, goruntine.goruntineTest5()==========")
	defer fmt.Println("=========Exit, goruntine.goruntineTest5()==========")
	fmt.Println(Same(tree.New(1), tree.New(1)))
	fmt.Println(Same(tree.New(1), tree.New(2)))
}

/*
Note:
The select statement lets a goroutine wait on multiple communication operations.
A select blocks until one of its cases can run, then it executes that case. It chooses one at random if multiple are ready.
*/

func fibonacci(c, pause, quit chan int) {
	x, y := 0, 1
	for {
		select {
		case c <- x:
			x, y = y, x+y
		case <-pause:
			fmt.Println("pause")
			time.Sleep(3 * time.Second)
		case <-quit:
			fmt.Println("quit")
			return
		}
	}
}

func goruntineTest4() {
	fmt.Println("=========Enter, goruntine.goruntineTest4()==========")
	defer fmt.Println("=========Exit, goruntine.goruntineTest4()==========")
	c := make(chan int)
	pause := make(chan int)
	quit := make(chan int)
	go func() {
		for i := 0; i < 10; i++ {
			fmt.Println(<-c)
		}
		pause <- 0
		quit <- 0
	}()
	fibonacci(c, pause, quit)
}

/*
Note:
A sender can close a channel to indicate that no more values will be sent. Receivers can test whether a channel has been closed by assigning a second parameter to the receive expression: after
	v, ok := <-ch
ok is false if there are no more values to receive and the channel is closed.
The loop for i := range c receives values from the channel repeatedly until it is closed.
Only the sender should close a channel, never the receiver. Sending on a closed channel will cause a panic.
 Channels aren't like files; you don't usually need to close them. Closing is only necessary when the receiver must be told there are no more values coming, such as to terminate a range loop.
*/

func fibonacci1(n int, c chan int) {
	x, y := 0, 1
	for i := 0; i < n; i++ {
		c <- x
		x, y = y, x+y
	}
	close(c)
}

func fibonacci2(n int, c chan int) {
	x, y := 0, 1
	for i := 0; i < n; i++ {
		c <- x
		x, y = y, x+y
	}
	//close(c)
}

func goruntineTest3() {
	fmt.Println("=========Enter, goruntine.goruntineTest3()==========")
	defer fmt.Println("=========Exit, goruntine.goruntineTest3()==========")

	//use range
	fmt.Println("==========Print fibonacci1=========")
	c := make(chan int, 7)
	go fibonacci1(cap(c), c)
	for i := range c {
		fmt.Println(i)
	}

	fmt.Println("==========Print fibonacci2=========")
	ch := make(chan int, 7)
	go fibonacci2(cap(ch), ch)
	/*
		if this: for { ...  }, will report run error: fatal error: all goroutines are asleep - deadlock!
		if the sender will not close the channel, the receiver need to control the loop times, if the channel has been empty, the receiver
		need to stop receive date immediately.
	*/
	for i := 0; i < cap(ch); i++ {
		v, ok := <-ch
		if ok == false {
			break
		}
		fmt.Println(v)
	}
}

/*
Note:
Channels are a typed conduit through which you can send and receive values with the channel operator, <-.
	ch <- v    // Send v to channel ch.
	v := <-ch  // Receive from ch, and
	           // assign value to v.
(The data flows in the direction of the arrow.)
Like maps and slices, channels must be created before use:
	ch := make(chan int)
By default, sends and receives block until the other side is ready. This allows goroutines to synchronize without explicit locks or condition variables.
The example code sums the numbers in a slice, distributing the work between two goroutines. Once both goroutines have completed their computation, it calculates the final result.

Channels can be buffered. Provide the buffer length as the second argument to make to initialize a buffered channel:
	ch := make(chan int, 100)
Sends to a buffered channel block only when the buffer is full. Receives block when the buffer is empty.
*/
func sum(s []int, c chan int) {
	sum := 0
	for _, v := range s {
		sum += v
	}
	c <- sum // send sum to c
}

func goruntineTest2() {
	fmt.Println("=========Enter, goruntine.goruntineTest2()==========")
	defer fmt.Println("=========Exit, goruntine.goruntineTest2()==========")
	s := []int{7, 2, 8, -9, 4, 0}

	c := make(chan int)
	go sum(s[:len(s)/2], c)
	go sum(s[len(s)/2:], c)
	x, y := <-c, <-c // receive from c

	fmt.Println(x, y, x+y)

	ch := make(chan int, 2)
	ch <- 1
	ch <- 2
	//ch <- 3  //if open this line, run error: fatal error: all goroutines are asleep - deadlock!
	fmt.Println(<-ch)
	fmt.Println(<-ch)
	ch <- 4
	fmt.Println(<-ch)
}

func say(s string) {
	for i := 0; i < 5; i++ {
		time.Sleep(100 * time.Millisecond)
		fmt.Println(s)
	}
}

func goruntineTest1() {
	fmt.Println("=========Enter, goruntine.goruntineTest1()==========")
	defer fmt.Println("=========Exit, goruntine.goruntineTest1()==========")
	go say("hello")
	say("hunk")
}

func Test() {
	goruntineTest1()
	goruntineTest2()
	goruntineTest3()
	goruntineTest4()
	goruntineTest5()
}
