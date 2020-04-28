# 1. 基础知识
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