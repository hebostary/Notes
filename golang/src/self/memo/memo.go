package memo

import (
	"fmt"
	"io/ioutil"
	"log"
	"net/http"
	"sync"
	"time"
	//
)

/*
FUNCTION: 基于互斥锁和channel实现的无阻塞缓存
*/

type Memo struct {
	f     Func
	mu    sync.Mutex
	cache map[string]*entry
}

type Func func(key string) (interface{}, error)

type result struct {
	value interface{}
	err   error
}

type entry struct {
	res   result
	ready chan struct{}
}

func New(f Func) *Memo {
	return &Memo{f: f, cache: make(map[string]*entry)}
}

func (memo *Memo) Get(key string) (interface{}, error) {
	memo.mu.Lock()
	e := memo.cache[key]
	if e == nil {
		//对于一个key，第一个获得锁的goruntine取到的e是nil
		e = &entry{ready: make(chan struct{})}
		memo.cache[key] = e
		memo.mu.Unlock() //插入条目后立即释放互斥锁
		e.res.value, e.res.err = memo.f(key)
		close(e.ready)
	} else {
		//第一个goruntine调用f还没有完成，但是e已经有内容了，只是e.res.value可能还没有，等待e.ready被close后就可以取了
		memo.mu.Unlock()
		<-e.ready
	}
	return e.res.value, e.res.err
}

func httpGetBody(url string) (interface{}, error) {
	resp, err := http.Get(url)
	if err != nil {
		return nil, err
	}
	defer resp.Body.Close()
	return ioutil.ReadAll(resp.Body)
}

func Test() {
	var urls = []string{
		"http://www.baidu.com",
		"http://www.baidu.com",
		"http://tencent.com",
		"http://tencent.com",
		"http://www.baidu.com",
		"http://tencent.com",
	}

	m := New(httpGetBody)
	var n sync.WaitGroup
	for _, url := range urls {
		n.Add(1)
		go func(url string) {
			start := time.Now()
			value, err := m.Get(url)
			if err != nil {
				log.Print(err)
			}
			fmt.Printf("%s, %s, %d bytes\n", url, time.Since(start), len(value.([]byte)))
			n.Done()
		}(url)
	}
	n.Wait()
}
