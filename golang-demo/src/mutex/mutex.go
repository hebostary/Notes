package mutex

import (
	"fmt"
	"sync"
	"time"
)

var (
	//mu      sync.Mutex
	/* sync.RWMutex在获得锁的大部分goruntine都是读操作的时候才能带来最多的好处，
	因为RWMutex的内部记录更为复杂，所以相比普通的mutex更耗时
	*/
	mu      sync.RWMutex
	balance int
)

func Withdraw(amount int) bool {
	mu.Lock() //这里加锁后，避免内部调用的函数再次对mu加锁，所以实现一个deposit版本
	defer mu.Unlock()
	fmt.Println("Deposit, balance: ", balance)
	deposit(-amount)
	if balance < 0 {
		deposit(amount)
		return false
	}
	return true
}

func Balance() int {
	mu.RLock()
	defer mu.RUnlock()
	return balance
}

func Deposit(amount int) {
	mu.Lock()
	defer mu.Unlock()
	balance += amount
}

func deposit(amount int) {
	balance += amount
}

func Test() {
	balance = 100
	for index := 20; index > 0; index-- {
		go func(amount int) {
			Withdraw(amount)
		}(index)
	}

	for index := 0; index < 25; index++ {
		go func() {
			ba := Balance()
			fmt.Println("Query, balance: ", ba)
		}()
	}

	time.Sleep(2000 * time.Millisecond)
}
