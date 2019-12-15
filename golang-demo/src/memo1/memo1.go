package memo1

import (
	"fmt"
	"io/ioutil"
	"log"
	"net/http"
	"sync"
	"time"
)

/*
FUNCTION: 完全基于channel实现的无阻塞缓存
*/

type result struct {
	value interface{}
	err   error
}

type Func func(key string) (interface{}, error)

type entry struct {
	res   result
	ready chan struct{}
}

type request struct {
	key      string
	response chan<- result
}

type Memo struct {
	requests chan request
}

func New(f Func) *Memo {
	memo := &Memo{requests: make(chan request, 3)}
	go memo.server(f)
	return memo
}

func (memo *Memo) Get(key string) (interface{}, error) {
	response := make(chan result)
	memo.requests <- request{key, response}
	res := <-response
	return res.value, res.err
}

func (memo *Memo) Close() {
	close(memo.requests)
}

/*缓存请求监控函数，运行在单独的goruntine中，处理获取key的value的请求，
无论是新的key的value的获取，还是已有key的value的交付都用单独的goruntine来完成
不会阻塞处理请求的主线goruntine
*/
func (memo *Memo) server(f Func) {
	cache := make(map[string]*entry)
	for req := range memo.requests {
		e := cache[req.key]
		if e == nil {
			e = &entry{ready: make(chan struct{})}
			cache[req.key] = e
			go e.call(f, req.key)
		}
		go e.deliver(req.response)
	}
}

/*
对于固定key,第一个进来的request，会在entry上调用f获取value，
最后关闭ready来广播消息表示缓存中已经有该key的value
*/
func (e *entry) call(f Func, key string) {
	e.res.value, e.res.err = f(key)
	close(e.ready)
}

/*
对于固定key,第二个和之后进来的request会等待ready被close后取出value
*/
func (e *entry) deliver(response chan<- result) {
	<-e.ready
	response <- e.res
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
