# 基础知识
## 依赖管理
### govendor
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

# Code Block
## path
```go
//获取可执行文件目录
dir, err := filepath.Abs(filepath.Dir(os.Args[0]))
if err != nil {
    log.Fatal(err)
}
```


# Framework
## Web
* [mux](https://github.com/gorilla/mux) //http router

## Plugins
* [go-plugins-helpers](https://github.com/docker/go-plugins-helpers)

# References
* Concurrency
    * [Handling 1 Million Requests per Minute with Go](http://marcio.io/2015/07/handling-1-million-requests-per-minute-with-golang/) 

* Documentation
    * http://legendtkl.com/categories/golang/