

# 0. 语言基础

## 数据类型

### array&slice

通过array的切片可以切出slice，也可以使用make创建slice，此时golang会生成一个匿名的数组。

```go
//创建数组
letters := [...]string{"a", "b", "c", "d"}
letters := [4]string{"a", "b", "c", "d"}

//从匿名数组创建切片
letters := []string{"a", "b", "c", "d"}

//通过make创建切片
var s []byte
s = make([]byte, 5, 5)
// s == []byte{0, 0, 0, 0, 0}

//从数组创建切片
x := [3]string{"Лайка", "Белка", "Стрелка"}
s := x[:] // a slice referencing the storage of x
```

array是值类型，slice则是复合类型，同时slice是基于array实现的。slice的第一个内容为指向数组的指针，然后是其长度和容量。

![slice sturcture](https://upload-images.jianshu.io/upload_images/7179784-42e4070017fea073.png?imageMogr2/auto-orient/strip|imageView2/2/w/517)

因为slice依赖其底层的array，修改slice本质是修改array，而array又是有大小限制，当超过slice的容量，即数组越界的时候，需要通过动态规划的方式创建一个新的底层数组，把原有的数据复制到新数组，然后slice的指针指向新的数组，这也是append函数的基本原理。所以array是定长的（初始化的时候指定长度，长度也是数组类型的一部分），而slice则是数据长度可变的，支持动态扩容。

因为array是值类型，作为函数参数传递时，会对整个数组进行拷贝，性能较差。

```go
func main() {
 var arr = [10]int{4,5,7,11,8,9}
 fmt.Println(arr) //[4,5,7,11,8,9,0,0,0,0]
 //验证数组是值拷贝传递
 AddOne(arr)
 fmt.Println(arr) //[4,5,7,11,8,9,0,0,0,0]
}
func AddOne(arr [10]int){
 arr[9] = 999999
 fmt.Println(arr) //[4,5,7,11,8,9,0,0,0,999999]
}
```

而slice是一种引用类型，但是，在函数中作为参数传递时实际上也不是严格意义上的传递引用，因为slice本身也发生了拷贝，只不过它们都指向了同一个底层数组，函数内的操作即对切片的修改操作了底层数组。

```go
    ...
    slice := make([]int, 2, 3)
    for i := 0; i < len(slice); i++ {
        slice[i] = i
    }

    fmt.Printf("slice %v %p \n", slice, &slice)  //slice [0 1] 0xc42000a1e0

    ret := changeSlice(slice)//func s [0 1] 0xc42000a260
    //slice [0 1] 0xc42000a1e0, ret [0 1 3]
   fmt.Printf("slice %v %p, ret %v \n", slice, &slice, ret)

   ret[1] = 1111

   //slice [0 1111] 0xc42000a1e0, ret [0 1111 3]
   fmt.Printf("slice %v %p, ret %v \n", slice, &slice, ret)
}

func changeSlice(s []int) []int {
    fmt.Printf("func s %v %p \n", s, &s)
    s = append(s, 3)
    return s
}
```

不过，如果在函数内，append操作超过了原始切片的容量，将会有一个新建底层数组的过程，那么此时再修改函数返回切片，则不会再影响原始切片。

```go
func main() {
    slice := make([]int, 2, 2)
    for i := 0; i < len(slice); i++ {
        slice[i] = i
    }
  
    fmt.Printf("slice %v %p \n", slice, &slice)    //slice [0 1] 0xc42000a1a0

    ret := changeSlice(slice)//func s [0 1] 0xc42000a200
    //slice [-1 1] 0xc42000a1a0, ret [-1 1111 3]
    //函数内的修改没有影响到slice的底层数组
    fmt.Printf("slice %v %p, ret %v \n", slice, &slice, ret)

    ret[1] = -1111

    //slice [-1 1] 0xc42000a1a0, ret [-1 -1111 3]
    fmt.Printf("slice %v %p, ret %v \n", slice, &slice, ret)
}

func changeSlice(s []int) []int {
    fmt.Printf("func s %v %p \n", s, &s)
    s[0] = -1
    s = append(s, 3)
    s[1] =  1111
    return s
}
```

当然，如果为了修改原始变量，可以指定参数的类型为指针类型。传递的就是slice的内存地址。函数内的操作都是根据内存地址找到变量本身。

```go
func main() {
    slice := []int{0, 1}
    fmt.Printf("slice %v %p \n", slice, &slice)//slice [0 1] 0xc42000a1e0

    changeSlice(&slice)//func s [0 1] 0xc42000a1e0
    fmt.Printf("slice %v %p \n", slice, &slice)//slice [-1 1111 3] 0xc42000a1e0 

    slice[1] = -1111
    fmt.Printf("slice %v %p \n", slice, &slice)//slice [-1 -1111 3] 0xc42000a1e0
}

func changeSlice(s *[]int) {
    fmt.Printf("func s %v %p \n", *s, s)
    (*s)[0] = -1
    *s = append(*s, 3)
    (*s)[1] =  1111
}
```

### map







# 1. 项目工程

## 1.1. 项目结构
golang工程典型目录结构
```shell
-- go_project     // go_project为GOPATH目录
  -- bin
     -- myApp1  // 编译生成 go build -o ./bin/myApp1 myApp1
     -- myApp2  // 编译生成
     -- myApp3  // 编译生成
  -- pkg
  -- src
     -- myApp1     // project1
        -- models
        -- controllers
        -- others
        -- main.go 
     -- myApp2     // project2
        -- models
        -- controllers
        -- others
        -- main.go 
     -- myApp3     // project3
        -- models
        -- controllers
        -- others
        -- main.go 
```

## 1.2. 插件管理
在VS Code里配置golang开发环境的时候，因为“墙”的存在，经常会遇到下载插件失败的问题，可以参考[VS Code配置Go语言开发环境](https://www.liwenzhou.com/posts/Go/00_go_in_vscode/?tdsourcetag=s_pcqq_aiomsg)配置国内的GOPROXY，比手动安装插件方便很多。
VS Code关于golang的settings：
```shell
{
"go.buildOnSave": "workspace",
"go.lintOnSave": "package",
"go.vetOnSave": "package",
"go.buildTags": "",
"go.buildFlags": [],
"go.lintFlags": [],
"go.vetFlags": [],
"go.coverOnSave": false,
"go.autocompleteUnimportedPackages": true,
"go.inferGopath": true,
"go.gotoSymbol.includeImports": true,  
"go.useCodeSnippetsOnFunctionSuggest": true,
"go.formatOnSave": true,
"go.formatTool": "goreturns",
"go.goroot": "/usr/local/go",
"go.gopath": "/home/workspace/Notes/golang",
"go.gocodeAutoBuild": false,
"files.autoSave": "onFocusChange",
"go.docsTool": "gogetdoc",
"go.toolsGopath": "/usr/local/go/pkg/tool/linux_amd64"
}
```

### 1.2.1. 备选：手动安装go插件
```shell
$ echo $GOPATH
/home/workspace/Notes/golang
$ mkdir -p $GOPATH/src/golang.org/x/
$ cd $GOPATH/src/golang.org/x/

# 直接从github上下载插件源码
$ git clone https://github.com/golang/tools.git tools
$ git clone https://github.com/golang/lint.git lint

# 手动安装插件
$ go install golang.org/x/lint/golint
```

## 1.3. 依赖管理
### 1.3.1. govendor
* 安装
```shell
$ go get -u github.com/kardianos/govendor
```
* 基础用法
```shell
$ cd $GOPATH/src
#初始化vendor目录
$ govendor init 

#添加依赖包
$ govendor fetch github.com/gorilla/mux 
#更新依赖包
$ govendor update github.com/gorilla/mux
#移除依赖包
$ govendor remove github.com/gorilla/mux 

#列出本地所有依赖包
$ govendor list
```

# 2. Framework
## 2.1. Web
* [mux](https://github.com/gorilla/mux) //http router

## 2.2. Plugins
* [go-plugins-helpers](https://github.com/docker/go-plugins-helpers)

# 3. References

* 数据类型
  * array&slice
    * [Golang Slice详解](https://www.jianshu.com/p/843aa553b461)
    * [Golang切片与函数参数“陷阱”](https://www.jianshu.com/p/7439e7ae3c4c)
* 协程模型
  * [Golang源码探索(二) 协程的实现原理](https://www.cnblogs.com/zkweb/p/7815600.html)
  * [Go 语言调度（一）: 系统调度](https://www.jianshu.com/p/db0aea4d60ed)
* 内存管理
  * [Golang源码探索(三) GC的实现原理](https://www.cnblogs.com/zkweb/p/7880099.html)
  * [Go 语言内存管理（一）：系统内存管理](https://www.jianshu.com/p/1ffde2de153f)
  * [Go 语言内存管理（二）：Go 内存管理](https://www.jianshu.com/p/7405b4e11ee2)
  * [Go 语言内存管理（三）：逃逸分析](https://www.jianshu.com/p/518466b4ee96)
  * [Go 语言内存管理（四）：垃圾回收](https://www.jianshu.com/p/0083a90a8f7e)
  * [Go内存分配机制总结](https://www.jianshu.com/p/34984105175c)
  * [高性能 Go 服务的内存优化(译)](https://www.jianshu.com/p/63404461e520)
* 并发编程
    * [Handling 1 Million Requests per Minute with Go](http://marcio.io/2015/07/handling-1-million-requests-per-minute-with-golang/) 
* Documentation
    * http://legendtkl.com/categories/golang/