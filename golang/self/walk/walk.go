package walk

import (
	"bufio"
	"fmt"
	"io/ioutil"
	"os"
	"path/filepath"
	"strings"
	"sync"
	"time"
)

//限制同时打开的目录文件数，防止启动过多的goruntine
var sema = make(chan struct{}, 20)

//用于取消整个磁盘扫描任务
var done = make(chan struct{})

func walkDir(dir string, n *sync.WaitGroup, fileSizes chan<- int64) {
	defer n.Done()
	//收到cancel信号后，goruntine直接退出
	if cancelled() {
		return
	}
	for _, entry := range dirents(dir) {
		if entry.IsDir() {
			n.Add(1)
			subdir := filepath.Join(dir, entry.Name())
			go walkDir(subdir, n, fileSizes)
		} else {
			fileSizes <- entry.Size()
		}
	}
}

func dirents(dir string) []os.FileInfo {
	select {
	case sema <- struct{}{}:
	case <-done:
		//发出cancel后，我们期望是这个工作能立即被终止，但是对于已经启动的goruntine，dirents会继续执行并耗费不少时间
		//所以这里添加对cancel信号的处理，可以减少cancel操作的时延
		return nil
	}

	defer func() {
		<-sema
	}()

	entries, err := ioutil.ReadDir(dir)
	if err != nil {
		fmt.Fprintf(os.Stderr, "du1: %v\n", err)
		return nil
	}
	return entries
}

func inputDir() string {
	var (
		inputReader *bufio.Reader
		dir         string
		err         error
	)

	inputReader = bufio.NewReader(os.Stdin)
	str := "Please input a directory:"
	fmt.Printf(str)
	dir, err = inputReader.ReadString('\n')
	if err != nil {
		os.Exit(1)
	}
	dir = strings.Replace(dir, "\n", "", -1)
	return dir
}

func printDiskUsage(nFiles int64, nBytes int64) {
	fmt.Printf("%d files %.1f GB\n", nFiles, float64(nBytes)/1e9)
}

func cancelled() bool {
	select {
	case <-done:
		return true
	default:
		return false
	}
}

func Test() {
	fmt.Println("=========Enter, walk.Test.()==========")
	defer fmt.Println("=========Exit, walk.Test.()==========")
	dir := inputDir()

	//从标准输入读到任何内容后，关闭done这个channel
	go func() {
		os.Stdin.Read(make([]byte, 1))
		close(done)
	}()

	fileSizes := make(chan int64)
	var n sync.WaitGroup
	n.Add(1)
	go walkDir(dir, &n, fileSizes)

	go func() {
		n.Wait()
		close(fileSizes)
	}()

	var nFiles, nBytes int64
	var tick <-chan time.Time
	tick = time.Tick(100 * time.Millisecond)

loop:
	for {
		select {
		case <-done:
			for range fileSizes {
				//do nothing，这个for循环的意义在于：收到cancel信号后，统计工作马上结束，我们会退出对fileSizes这个channel的接收操作，但是整个程序未必退出
				//这时将fileSizes排空可以防止还在运行walkDir的goruntine因为向fileSies发送数据被阻塞（没有buffer或者buffer已满），导致goruntine泄露
			}
			return
		case size, ok := <-fileSizes:
			if !ok { //这里必须显示的判断fileSizes是否已经被close
				break loop
			}
			nFiles++
			nBytes += size
		case <-tick: //每0.1S产生一次时钟信号，打印一次当前统计数字
			printDiskUsage(nFiles, nBytes)
		}
	}

	printDiskUsage(nFiles, nBytes)
}
