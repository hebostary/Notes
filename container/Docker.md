# Docker基础知识

## Docker简介

Docker的主要目标是“Build，Ship and Run Any App,Anywhere”，也就是通过对应用组件的封装、分发、部署、运行等生命周期的管理，使用户的APP（可以是一个WEB应用或者数据库应用等等）及其运行环境能够做到“一次封装，到处运行”。

Docker引擎的基础是Linux自带的容器（Linux Containers,LXC）技术。我们可以将容器理解为一种沙盒，每个容器内运行一个应用，不同的容器相互隔离，容器之间可以建立通信机制。容器的创建和停止都十分快速（秒级），容器自身对资源的需求十分有限，远比虚拟机本身占用的资源少。

Docker基于Linux原有的 Namespace 和 Cgroups 实现。Namespace 的作用是“隔离”，它让应用进程只能看到该 Namespace 内的“世界”；而 Cgroups 的作用是“限制”，它给这个“世界”围上了一圈看不见的墙。

> 容器的本质是一种特殊的进程。一个正在运行的Docker容器，其实就是启用了多个Linux Namespace的应用进程，而这个进程能够使用的资源量，则受Cgroups配置的限制。

### Docker vs 虚拟机

下图左边部分描述的是Docker对资源的管理模型，右边是传统虚拟机技术的资源管理模型。

![img](https://hunk-pic-store.oss-cn-beijing.aliyuncs.com/img/20211006201814.png)

虚拟化的核心是对资源进行抽象，目标往往是为了在同一个机器上运行多个系统或应用，从而提高系统资源的利用率。虚拟化分为很多类型，比如常见的硬件辅助虚拟化（VMware workstation、 KVM等）。Docker所代表的容器虚拟化技术属于操作系统级虚拟化：内核通过创建多个虚拟的操作系统实例（内核和库）来隔离不同的进程。

传统虚拟化和容器技术结构比较：传统虚拟化技术是在硬件层面实现虚拟化，增加了系统调用链路的环节，有性能损耗；容器虚拟化技术以共享Kernel的方式实现，几乎没有性能损耗。

### Docker能为我们带来什么？

在提到Docker带来的好处时，我想举几个在企业中常见的Docker使用场景：

场景一：

曾经参与的一个C++项目，每个开发人员都有自己的编译服务器（虚拟机），在拿到一个编译服务器开始工作时，我们需要做如下事情：

1. 安装编译依赖包。
2. 下载三方库代码 -> 编译（30分钟左右）。
3. 下载平台库代码 -> 编译（30分钟左右）。
4. 下载产品代码 -> 编译（首次全量编译30分钟左右）。

如果编译依赖包、三方库和平台库代码有任何更新，那么每个开发人员都需要重新安装和编译它们，还是比较麻烦的。实际上，编译依赖包、三方库和平台库的编译成果（二进制或者动态库）是所有开发人员可以共享的，显然不需要每个人都去反复安装和编译。当然，也可以把它们做到虚拟机的模板或者镜像中，而实际上虚拟机的更新也很麻烦。

场景二：

在我现在参与的项目中，项目构建成果是超过5G的系统ISO，依赖项目非常多，但是开发构建流程却非常顺畅。大致流程如下：

1. 准备Docker虚拟机。

2. 从内源仓库拉取用于构建的镜像（build image）。

3. 下载产品代码到Docker主机。

4. 启动容器（同时将产品代码mount到容器内），执行make开始构建。

我们将项目里很多只需要编译一次的组件都放到了用于构建的镜像中，如果这些组件有更新，开发人员只需要更新镜像就可以了，效率非常高。

这就是Docker在软件开发过程中给我们带来的便利。实际上，在软件的整个生命周期，包括开发、测试、部署、运维各个阶段，Docker都能发挥巨大的作用。

## 容器镜像（image）

类比于虚拟机镜像或者模板，Docker镜像则是作为Docker容器的模板，它是由一组上下有序的只读layer构建出来的，可以根据需要动态的构建。	

### 用Dockerfile构建镜像

官网链接：https://docs.docker.com/engine/reference/builder/

类比于我们用.spec文件来定义如何构建一个rpm包，用makefile（make）或者pom.xml（maven）来定义如何构建一个软件项目，我们通过Dockerfile来定义Docker镜像的构建规则。

Dockerfile由一序列的构建器指令（builder instruction）组成，这些指令不对大小写敏感，但是为了统一风格，我们一般习惯都使用大写。

使用docker build命令启动构建时Docker顺序执行Dockerfile中的指令，每个指令就代表着一个layer。每一个layer都只包含了相比于下一层的layer不同的内容，最后所有的layer堆叠起来就是一个新的镜像。

下面我们先用Dockerfile来构建一个自己的镜像，构建上下文环境根目录结构如下：

.├── app.py

 ├── Dockerfile

 └── requirements.txt

Dockerfile：

```bash
# 指定基础镜像
FROM python:2.7-slim

# 设置工作目录/app
WORKDIR /app

# 拷贝当前目录内容到容器内/app目录
COPY . /app

# 安装依赖包
RUN pip install --trusted-host pypi.python.org -r requirements.txt

# 对外暴露80端口
EXPOSE 80

# 定义环境变量
ENV NAME World

# 指定容器启动时运行的程序
CMD ["python", "app.py"]
```

app.py：

```bash
from flask import Flask
from redis import Redis, RedisError
import os
import socket

# Connect to Redis
redis = Redis(host="redis", db=0, socket_connect_timeout=2, \
socket_timeout=2)

app = Flask(__name__)

@app.route("/")
def hello():
    try:
        visits = redis.incr("counter")
    except RedisError:
        visits = "<i>cannot connect to Redis, counter disabled</i>"

    html = "<h3>Hello {name}!</h3>" \
           "<b>Hostname:</b> {hostname}<br/>" \
           "<b>Visits:</b> {visits}"
    return html.format(name=os.getenv("NAME", "world"), \
    hostname=socket.gethostname(), visits=visits)

if __name__ == "__main__":
    app.run(host='0.0.0.0', port=80)
```

requirements.txt：

```bash
Flask
Redis
```

`docker build`命令基于Dockerfile构建镜像，除了dockerfile还需要指定一个构建上下文环境（build context），构建上下文环境可以是：

- 1. 包含构建过程所需文件的本地目录。
  2. URL路径（比如Git仓库地址）。

我们可以执行下面的任一命令来构建我们的镜像：

```bash
docker build -t demo1 . #以当前目录作为构建目录
docker build -f /path/Dockerfile . #指定Dokcerfile路径
docker build -t user/demo1:1.0 .   #指定标准格式镜像标签
docker build -t user/demo1:1.0 -t user/demo1:latest . #指定多个标签
```

构建过程实际由Docker daemon完成，而不是CLI。如下所示，执行docker build命令时，首先会把整个上下文环境发送给Docker daemon。

```bash
[root@hunk demo1]# docker build -t image-demo1 .
Sending build context to Docker daemon   5.12kB
Step 1/7 : FROM python:2.7-slim
...
```

>构建上下文环境最好不要包含无关的文件和内容，更不要将根目录/作为上下文环境。

### 细说Dockerfile

#### .dockerignore文件

上一节提到，`docker build`执行的第一步就是把上下文环境打包发送到Docker daemon。那如果目录下包含了与构建无关的文件，但是又不方便移除怎么办呢，这个时候`.dockerignore`文件就派上用场了。

CLI在将上下文环境发送到daemon之前，会先检查上下文环境根目录下是否有名叫`.dockerignore`的文件，然后根据这个文件里的表达式来排除掉无用的文件目录或者敏感信息，从而改变发送到daemon的上下文环境。注意表达式匹配上的文件是不被发送到daemon。

`.dockerignore`文件里的过滤表达式都是相对于构建上下文环境的根目录（docker build指定的目录），下面简称为根目录，注意和系统根目录的区分。比如下面的例子：

```bash
# 添加注释，实际上仅此一行有效，后面的仅为表达式解释
# 过滤掉根目录的子目录下任意以temp开头的文件和目录，
# 比如/somedir/temp.txt
*/temp*

# 过滤掉根目录的二级子目录下任意以temp开头的文件和目录，
# 比如/somedir/subdir/temp.txt
*/*/temp*

# 过滤掉根目录包括所有子目录下.go文件，**表示递归匹配所有目录
**/*.go

# 过滤掉根目录下任何以temp做单字符扩展的文件和目录，比如/tempa和/tempb
temp?
```

以!开头的行可以用来定义一些过滤条件的例外情况，比如：

```bash
# 过滤掉除README.md以外的所有.md文件。
*.md
!README.md
```

```bash
# 过滤掉.md文件，但是README文件里除了README-secret.md都被发送到daemon
*.md
!README*.md
README-secret.md
```

```bash
# 过滤掉除README以外的所有文件，第三行的影响实际上会被第四行覆盖
*.md
README-secret.md
!README*.md
```

.dockerignore文件里甚至可以包含Dockerfile，不过CLI还是会将Dockerfile发送到daemon，因为daemon需要它来完成构建，只是后面的指令比如ADD,COPY就会忽略掉Dockerfile了。

> 从18.09版本开始，Docker支持一个新的构建后端（backend）实现，叫做BuildKit，这个功能由[moby/buildkit](https://github.com/moby/buildkit)项目提供。我们可以在运行docker build命令之前，通过在CLI设置环境变量DOCKER_BUILDKIT=1来启用BuildKit。

#### 构建器指令

构建器指令（builder instructoin）是Dockerfile中的基础指令，daemon正是顺序执行Dockfile中的构建器指令来层层构建镜像，每条指令就代表了一个layer。

**ENV**

ENV指令用于设置环境变量，在Dockerfile中，部分后续指令可以使用这些环境变量，用法主要有两种：`$variable_name`和`${variable_name}`。后者可以用于拼接没有空格的字符，默认用\转义$。

ENV指令支持两种写法：

```bash
ENV <key> <value>
ENV <key1>=<value1> <key2>=<value2>
```

对于第一种写法，key后面的空格后所有内容都被解析为value，包括空格本身。第二种写法允许在单行设置多个变量，这时候空格用于区分不同的键值对。双引号会被自动去掉，除非加上\转义。来看下面的例子吧：

```bash
ENV name hunk he #"name"="hunk he"
ENV name="hunk he" address=Beijing\ Haidian \
    work=coder
```

其中第二行的写法等价于下面的写法，注意其中对空格解析的不同：

```bash
ENV name hunk he 
ENV address Beijing Haidian
ENV work coder
```

下面来看看后续指令如何使用ENV设置的环境变量：

```bash
FROM busybox
ENV foo /bar
WORKDIR ${foo}/app   # WORKDIR /bar/app
ADD . $foo       # ADD . /bar
COPY \$foo /quux # COPY $foo /quux
```

`${variable}`也支持一些标准bash修饰符：

1. ${variable:-word}表示如果设置了variable，那么结果将是该值。如果没有设置变量，那么word将是结果。word可以是任意字符串。
2. ${variable:+word}表示如果设置了variable，那么word将是结果，否则结果是空字符串。

环境变量替换将在整个指令中对每个变量使用相同的值。比如下面的例子：

```bash
ENV abc=hello
ENV abc=bye def=$abc
ENV ghi=$abc
```

第二行中def的值为hello而不是bye，第三行中的ghi的值为bye。也就是说，这条指令开始时变量替换的值将用到指令结束，指令中间对变量的修改不影响指令后面的取值。

>  ENV指令设置的环境变量将被持久化，从镜像启动容器时这些环境变量的值保持不变，可以用docker inspect查看，在启动容器时也可以用如下方式修改环境变量：
>
> ```bash
> docker run --env <key>=<value>
> ```
>
> 支持环境变量的构建器指令包括：ADD, COPY, ENV, EXPOSE, FROM, LABLE, STOPSIGNAL, USER, VOLUME, WORKDIR。

**FROM**

Dockerfile必须以一个FROM指令开始，以此来初始化一个新的构建阶段（build stage），并为后续其它指令设置基础映像（base image）。FROM指令的基本格式：

```bash
# tag和digest是可选的，默认使用latest
FROM <image> [AS <name>]
FROM <image>[:<tag>] [AS <name>]
FROM <image>[@<digest>] [AS <name>]
```

FROM指令可以在单个Dockerfile中出现多次，用以创建多个镜像，或者使用一个构建阶段作为另一个构建阶段的依赖项，只需要在每条新的FROM指令之前通过提交记录镜像ID输出。每个FROM	指令自动清除之前的所有状态，可以通过将AS名称添加到FROM指令中，为新的构建阶段指定一个名称，可以在后续的FROM和COPY --from=<name | index>指令中使用该名称，以引用此阶段构建的镜像。

ARG指令是唯一可能出现在FROM之前的构建器指令，因为FROM指令支持ARG声明的环境变量，比如下面的例子：

```bash
ARG  CODE_VERSION=latest
FROM base:${CODE_VERSION}
CMD  /code/run-app

FROM extras:${CODE_VERSION}
CMD  /code/run-extras
```

ARG指令是独立于构建阶段之外的，所以FROM指令之后的其他指令要想使用ARG申明的变量值，必须在FROM后面再申明一次变量名字。

```bash
ARG VERSION=latest
FROM busybox:$VERSION
ARG VERSION
RUN echo $VERSION > image_version
```

**RUN**

RUN指令用于在构建镜像过程中运行一些命令来准备应用程序所需的运行环境，比如安装依赖包、修改配置文件等。RUN指令在当前镜像（上一条指令的提交结果）之上的新layer中执行任意命令并提交结果，提交结果又作为下一个指令的基础。分层运行指令和生成提交符合Docker的核心概念，其中提交很廉价（几乎每条指令都在做提交），允许可以从镜像构建过程中的任何位置创建容器。

RUN指令有两种基本形式：

```bash
RUN <command>  #shell形式
RUN ["executable", "param1", "param2"] #exec形式
```

这两种格式在大多数时候可以交换使用，但我们也要理解它们的一些使用区别：

1. shell形式在Linux上默认运行的是/bin/sh -c。如果不想用/bin/sh，可以用SHELL指令改变默认shell，或者写成RUN /bin/bash -c 'source $HOME/.bashrc; echo $HOME'。
2. 与shell形式不同，exec形式RUN指令不会触发标准shell命令处理流程。比如RUN [ "echo", "$HOME" ]不会对$HOME执行变量替换。如果确实需要执行标准shell命令流程，可以使用shell格式或者使用RUN [ "sh", "-c", "echo $HOME" ]，这样的话就由shell来做环境变量替换，而不是docker。
3. 如果基础镜像环境不包含shell可执行程序，那么就得用exec形式。

> 对于exec形式，[ ]内的参数实际上是被解析成JSON数组，所以每个元素都必须用双引号括起来。此外，在JSON格式中，有必要转义反斜杠。这在以反斜杠作为路径分隔符的Windous系统中很有必要。由于不是有效的JSON，下面第一行代码将被视为shell形式，并以一种意想不到的方式失败： 
>
> ```bash
> # 错误写法
> RUN ["c:\windows\system32\tasklist.exe"]
> 
> # 正确写法
> RUN ["c:\\windows\\system32\\tasklist.exe"] 
> ```

RUN指令的缓存（cache）不会自动失效，所以其运行结果会在下一层继续生效。如果希望其缓存失效，构建时可以加上--no-cache参数：

```bash
docker build --no-cache
```

**CMD**

CMD指令用于指定当用镜像启动容器时默认运行的命令，比如容器内应用程序的启动命令。CMD指令在Dockerfile中应该只出现一次，如果有CMD指令多个同时存在，只有最后一个生效。需要注意的是，RUN指令在构建过程中执行并提交结果，而CMD指令在构建镜像时并不运行，而是在启动容器时运行。

CMD指令有三种形式：

```shell
CMD ["executable","param1","param2"] #exec形式，推荐
CMD ["param1","param2"] #不包含可执行文件，作为ENTRYPOINT的默认参数
CMD command param1 param2 #shell形式
```

其实，基本形式也就exec和shell两种，用法细节和RUN指令里的两种形式几乎是一样的。第二种形式是和ENTRYPOINT指令配合使用的，用于指定其默认参数。

看一个简单例子，用下面的Dockerfile制作镜像并启动两个容器：

```shell
FROM ubuntu
CMD echo $HOME
```

```bash
[root@hunk demo2]# docker run e4c09dad6561 
/root
[root@hunk demo2]# docker run e4c09dad6561 echo $PATH
/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/opt/ibutils/bin
```

启动第一个容器时，CMD指令被执行了，并且实际上执行的是/bin/sh -c echo $HOME（shell形式），所以对$HOME执行了变量替换。如果把CMD指令写成CMD ["echo", "$HOME"]，将不会对$HOME执行变量替换。

启动第二个容器时，CMD指令并没有被执行，因为我们用docker run指定了运行参数，那么CMD里的默认参数将会被覆盖。docker run后面跟的运行参数实际上也是shell形式，所以会做变量替换。

**ENTRYPOINT**

ENTRYPOINT指令也用于设置容器启动时要执行的命令，并且比CMD指令优先级更高。我们可能会思考是不是Dockerfile中就一定需要ENTRYPOINT或者CMD指令呢？其实也不一定，如果容器本身是作为可执行程序使用，启动时需要运行固定的程序，那么设置ENTRYPOINT或者CMD是很有意义的。否则，可以不用设置。

在Dockerfile中，只能有一个ENTRYPOINT指令，如果有多个ENTRYPOINT指令则只有最后一个生效。

ENTRYPOINT指令也有exec和shell两种形式：

```bash
ENTRYPOINT ["executable", "param1", "param2"] #exec形式, 推荐
ENTRYPOINT command param1 param2 #shell形式
```

对于exec格式，不包含可执行文件的CMD指令参数以及docker run后跟的参数都会被追加到ENTRYPOINT指令后面，并且CMD指令的参数会被docker run后跟的参数覆盖。

对于shell格式，实际上执行的是/bin/sh -c  <cmd>，但是不允许任何追加参数，意味着CMD指令参数和docker run后跟的参数都会被忽略掉。

ENTRYPOINT指令和CMD指令最为推荐的用法：ENTRYPOINT指令设置容器启动执行命令<cmd>，CMD指令设置<cmd>的默认运行参数，动态参数则用docker run来指定。

先看一个推荐的exec格式的ENTRYPOINT指令例子：

```bash
 FROM ubuntu
 CMD ["-b"] 
 ENTRYPOINT ["top"]
```

制作好镜像后如下启动第一个容器，我们发现CMD指令作为了ENTRYPOINT指令的追加参数：

```shell
[root@hunk demo2]# docker run -dit 11b70268833c
5968f1b3b8293acb847d095977b94757a86978452b429c761db316e4f99e2012
[root@hunk demo2]# docker exec -ti 5968f1b3b82 ps -aux
... PID %CPU %MEM    VSZ   RSS TTY      STAT START   TIME COMMAND
...   1  0.0  0.0  36472  1604 pts/0    Ss+  10:03   0:00 top -b
...   6  0.0  0.0  34388  1456 pts/1    Rs+  10:03   0:00 ps -aux
```

启动第二个容器时，我们在docker run命令后也跟了-H参数，发现此时容器内运行的是top -H，而不是top -b或者top -b -H，说明CMD指令的参数会被docker run后跟的参数覆盖。

```bash
[root@hunk demo2]# docker run -dit 11b70268833c -H
62c9dc77b49b8840636aa9cd6e7e2e1a1c1c56a484fa3d001ac0925691890d08
[root@hunk demo2]# docker exec -ti 62c9dc77b49 ps -aux
... PID %CPU %MEM    VSZ   RSS TTY      STAT START   TIME COMMAND
...  1  0.2  0.0  36604  1688 pts/0    Ss+  10:05   0:00 top -H
...  6  0.0  0.0  34388  1456 pts/1    Rs+  10:06   0:00 ps -aux
```

ENTRYPOINT的设置也可以在执行`docker run`时用`--entrypoint`来覆盖，但是`--entrypoint`只能设置exec形式的参数。

下面再看一个shell形式的ENTRYPOINT指令例子：

```bash
 FROM ubuntu
 CMD ["-b"] 
 ENTRYPOINT top
```

启动容器后发现CMD和docker run后跟的参数都被忽略了。

```bash
[root@hunk demo2]# docker run -dit 13c74330e4dd -H
aacb2e05e2438d2c00a5bfc77005b67da19e3f976975900fd05e9d8211134601
[root@hunk demo2]# docker exec -ti aacb2e05e2438 ps -aux
... PID %CPU %MEM    VSZ   RSS TTY  STAT START   TIME COMMAND
... 1  0.2  0.0   4616   660 pts/0  Ss+  10:23   0:00 /bin/sh -c top 
... 6  0.0  0.0  36584  1704 pts/0  S+   10:23   0:00 top
... 7  0.0  0.0  34388  1460 pts/1  Rs+  10:23   0:00 ps -aux
```

使用shell格式ENTRYPOINT还有额外的问题：因为实际执行的是/bin/sh -c  <cmd>，所以<cmd>就不是容器中的1号进程（PID 1），在停止容器的时候容器无法干净的退出，因为无法收到SIGTERM信号所以<cmd>进程被残留，只有在超时后docker stop再次强制发送SIGKILL，整个过程需要超过10S的时间。更多细节可以参考：[理解Docker容器的进程管理](https://www.cnblogs.com/ilinuxer/p/6188303.html)。

```bash
[root@hunk demo2]# time docker stop aacb2e05e2438
aacb2e05e2438

real    0m10.298s
user    0m0.030s
sys     0m0.017s
```

因此，在使用shell形式时，建议加上shell内置的exec命令来使得<cmd>成为容器1号进程。

```bash
ENTRYPOINT exec top
```

下图来自官网，展示了ENTRYPOINT指令和CMD指令的组合情况，可以作为参考：

![img](https://hunk-pic-store.oss-cn-beijing.aliyuncs.com/img/20211006204503.png)

**LABLE**

LABLE指令用于给镜像烙上一些标签，比如作者，版本，描述等元数据信息。其内容就是一个个的键值对，看下面的例子：

```bash
LABEL metadata.author="Company" \
      metadata.version="1.0" \
      metadata.description="This is a test image."
```

我们可以用`docker inspect`命令来查看镜像的标签数据。

```bash
"Labels": {
    "metadata.author": "Company",
    "metadata.version": "1.0",
    "metadata.description": "This is a test image."
},
```

**VOLUME**

VOLUME指令用于为容器创建位于主机上的挂载点（mountpoint），其本质就是将主机上docker统一管理下的本地卷（local volume）映射到容器内。

VOLUME指令的两种基本格式：

```bash
VOLUME ["/data1", "data2"]
VOLUME /data1 /data2
```

`docker run`命令创建容器时，会为容器创建VOLUME指定的挂载点（先在docker主机上创建卷，然后再挂载到容器内的指定目录），并拷贝该目录已有的内容，所以声明卷之前对挂载目录的修改会被保留。但是，如果任何构建步骤（不包括CMD和ENTRYPOINT这些运行时指令）在声明卷之后更改了其中的数据，那么这些更改将被丢弃。

有两点需要特别理解：

1. VOLUME指令创建的挂载点目录可以实现对容器内数据的持久化，即使容器被删除后docker主机上的卷也仍然存在。
2. VOLUME指令不支持在Dockerfile中直接挂载主机上的现有目录，毕竟我们无法保证Dockerfile里定义的目录在每个主机上都存在，所以这需要在运行`docker run`命令时用-v参数指定，这也是为了镜像的可移植性。

在启动容器后，可以用`docker inspect <container id>`或者`docker volume inspect <volume id>`来查询volume的更多细节。

比如下面的Dockerfile例子：

```bash
  FROM ubuntu
  RUN mkdir /myvol
  RUN echo "hello world1" > /myvol/greeting
  VOLUME /myvol
  RUN echo "hello world2" > /myvol/greeting
```

制作好镜像后启动一个容器，我们发现最后一条RUN指令的修改没有生效。

```bash
[root@hunk demo3]# docker run -ti --name con-demo3 4cc9b9e8537c
root@c8b1dc951a45:/# cat /myvol/greeting 
hello world1
```

**USER**

USER指令主要用于设置后续指令（比如CMD，ENTRYPOINT，RUN等）以什么用户运行，主要格式：

```bash
USER <user>[:<group>]
USER <UID>[:<GID>]
```

如果USER指定的用户在基础镜像中并不存在，我们需要在Dockerfile中用RUN指令去创建，否则构建镜像时会报错：

```bash
unable to find user hunk: no matching entries in passwd file
```

如果指定的用户没有用户组，则会默认使用root组。

**WORKDIR**

WORKDIR用于为Dockerfile中的后续指令（CMD,RUN,ENTRYPOINT,COPY,ADD）设置工作目录。其基本格式如下：

```bash
WORKDIR /workdir
```

WORKDIR指令可以在Dockerfile中出现多次，效果是拼接相对路径。比如下面的例子，pwd命令的结果是将是/a/b/c。实际上我们不推荐这种写法，给构建过程和镜像增加了不必要的开销。

```bash
WORKDIR /a
WORKDIR b
WORKDIR c
RUN pwd
```

此外，WORKDIR也支持支持用ENV指令设置的环境变量。

**ARG**

ARG指令用于定义构建时（build-time）变量，这些变量的值我们可以在构建时通过`--build-arg <varname>=<value>`来传递给构建器，当然也可以在Dockerfile中设置默认值。其基本格式：

```bash
ARG <name>[=<default value>]
```

`docker build`命令传递给构建器的变量在Dockerfile中必须先用ARG声明过，否则会报错。在Dockerfile中构建时变量从ARG指令声明的地方开始生效，对于ARG前面的指令并不生效。因为ARG指令是独立于构建阶段之外的，构建时变量的作用域随着构建阶段结束而结束，如果需要在新的构建阶段中继续使用，需要重新声明一次，这点在FROM指令部分也提到过。

如果存在ENV指令和ARG指令定义的同名变量，ENV指令的优先级更高，也就是说后续指令会使用ENV指令中的变量值。

```bash
FROM busybox
ARG SETTINGS
RUN ./run/setup $SETTINGS

FROM busybox
ARG SETTINGS
RUN ./run/other $SETTINGS
```

如果说ENV指令定义的是Dockerfile内的静态变量，那么ARG定义的就是动态变量，它提供了Docker CLI和构建器之间的一种交互方式。使用ARG定义的变量的方式和ENV指令类似。

另外，不建议使用构建时变量来传递秘密，如密钥、用户凭证等，因为构建时变量值对于使用docker history <image>命令的任何用户都是可见的。

> docker定义了一些内置的构建时变量：HTTP_PROXY，http_proxy， HTTPS_PROXY， https_proxy， FTP_PROXY， ftp_proxy， NO_PROXY， no_proxy。这些变量我们可以直接使用，不用单独在Dockerfile中用ARG指令来声明，并且这些变量对于docker history命令也不可见，所以是安全的。

**ONBUILD**

ONBUILD指令用于设置在后续构建过程中执行的触发器指令。这个指令在我们制作基础镜像时可能会被用到。其基本格式：

```bash
ONBUILD [INSTRUCTION]
```

使用举例：

```bash
[...]
ONBUILD ADD . /app/src
ONBUILD RUN /usr/local/bin/python-build --dir /app/src
[...]
```

ONBUILD指令的工作流程是：

1. 在构建时，构建器看到一个ONBUILD指令，就会向镜像的元数据中添加一个触发器。这个指令不影响当前镜像的构建。
2. 镜像构建结束时，前面生成的触发器列表将会被添加到镜像属性清单（image manifest），key是OnBuild。用docker image inspect命令可以查询。
3. 随后在构建新的镜像时，用FROM指令引用前面的镜像作为基础镜像。作为FROM指令的的处理流程，构建器会查询基础镜像中的ONBUILD触发器指令并按添加的顺序执行，如果这些触发器指令都运行成功了，继续执行后续指令，如果任何ONBUILD触发器执行失败，构建过程将会失败退出。
4. 触发器指令只能被继承执行一次，构建过程中执行过的触发器指令将会被清理，以此镜像再作为基础镜像时，最初镜像的ONBUILD触发器指令已经不生效了。

**STOPSIGNAL**

STOPSIGNAL指令用于设置容器退出时发给容器的系统调用信号。基本格式：

```shell
STOPSIGNAL signal
```

signal可以是无符号整数（9）或者字符串格式（SIGKILL），只要在内核系统调用表里能匹配到就可以。

**HEALTHCHECK**

HEALTHCHECK指令用于设置容器的健康检查策略，包括检查周期，重试策略以及如何检查等。其基本指令格式如下：

```shell
HEALTHCHECK [OPTIONS] CMD command 
HEALTHCHECK NONE
```

第一种指令格式指定用于容器健康检查的命令，也可以是运行一个脚本。第二种指令格式用于禁掉所有基础镜像中的健康检查设置。

其中，[OPTIONS]可以是：

```shell
--interval=DURATION (默认值: 30s) # 设置检查命令的执行周期。
--timeout=DURATION (默认值: 30s) # 设置检查命令的超时时间，如果超时则视为检查失败。
--start-period=DURATION (默认值: 0s) 
--retries=N (默认值: 3) # 设置当检查到容器状态不健康时，连续多少次出现这种情况才更改容器状态为unhealthy。
```

对于检查命令，其返回值约定如下，和标准shell命令返回值类似：

```shell
0: success - healthy
1: unhealthy - unhealthy
2: reserved
```

检查命令打印到stdout和stderr的内容都会作为健康状态记录保存下来，可以通过`docker insect <contaienr id>`查询到。

#### 解释器指令

Dockerfile中除了必要的构建器指令，还可以包含可选的解释器指令（parser directive），解释器指令只能出现在Dockerfile的顶部，因为一旦注释、空行或构建器指令被处理，解释器指令就会被当做普通的注释处理。解释器指令不向构建中添加新的layer，也不会显示为构建步骤，并且只能使用一次。

其基本格式为：

```bash
# directive=value1
```

目前支持的解释器指令有两个：syntax和escape。

syntax用于指定Dockerfile构建器的位置，它可以让我们在不更新Docker daemon的情况下使用不同版本的构建器，这些构建器也以镜像的形式被发布到Docker Hub上的docker/dockerfile仓库。syntax指令只能在启用BuildKit时才能真正被启用。它的基本格式为：

```shell
# syntax=[remote image reference]
```

可用 的remote image reference：

```shell
# syntax=docker/dockerfile
# syntax=docker/dockerfile:1.0
# syntax=docker.io/docker/dockerfile:1
# syntax=docker/dockerfile:1.0.0-experimental
# syntax=example.com/user/repo:tag@sha256:abcdef...
```

escape指令用于设置Dockerfile中的转义字符，默认地转义字符是'\'。转义字符既用于转义行中字符，也用于转义换行。这个指令主要是在Windous系统上比较有用，因为Windous上的路径分隔符也是'\'，所以需要设置特殊的转义字符，比如：

```shell
# escape=`

FROM microsoft/nanoserver
COPY testfile.txt c:\
RUN dir c:\
```

无论Dockerfile中是否有escape指令，RUN命令中都不会执行转义，除非是在行尾。

### 发布镜像

先到https://hub.docker.com/上注册账户，账户就是Registry，然后创建一个自己的Repository。

![img](https://hunk-pic-store.oss-cn-beijing.aliyuncs.com/img/20211006210103.png)

完成后，我们就可以在Docker CLI上登陆这个账户：

```bash
docker login -u <username> --password-stdin
```

现在我们可以用docker tag命令来为刚刚制作的本地镜像打上唯一的镜像标签。docker tag的基本格式：

```bash
docker tag SOURCE_IMAGE[:TAG] TARGET_IMAGE[:TAG]
```

```bash
docker tag image-demo1:latest hebostary/image-demo1:v1
```

此时，在本地看起来有image-demo1:latest和hebostary/image-demo1:v1两个镜像，实际上只是对一个镜像打上了两个不同的标签，它们的镜像ID是一样的。我们试图删除image-demo1:latest时，也只是解除这个标签，并没有真正的删除镜像。

```bash
$ docker image ls
REPOSITORY              TAG      IMAGE ID        CREATED         SIZE
hebostary/image-demo1   v1       24a550909b3f    3 weeks ago     148MB
image-demo1             latest   24a550909b3f    3 weeks ago     148MB
$ docker image rm image-demo1:latest
Untagged: image-demo1:latest
$ docker image ls
REPOSITORY              TAG     IMAGE ID      CREATED            SIZE
hebostary/image-demo1   v1     24a550909b3f  3 weeks ago        148MB
```

最后，我们用docker push命令将这个镜像推送到docker hub上：

```bash
$ docker push hebostary/image-demo1:v1
The push refers to repository [docker.io/hebostary/image-demo1]
06b157080b2e: Pushed 
e50cb3a9f387: Pushed 
20db57dc9b41: Pushed 
efe67e69ee90: Mounted from library/python 
837c29e379a8: Mounted from library/python 
19e4018f673e: Mounted from library/python 
1c95c77433e8: Mounted from library/python 
v1: digest: sha256:e068f4cb1f11...86c02c65a86f7 size: 1787
```

从上面push的过程可以看到，部分layer（基础镜像部分）在docker hub上是共享的，所以实际上不需要推送，标记为Mounted from library/python。

![img](https://hunk-pic-store.oss-cn-beijing.aliyuncs.com/img/20211006210313.png)

发布成功后，如果这个repository是公开的，其他人就可以用docker pull或者docker run拉取这个镜像并运行新的容器。

```bash
$ docker pull hebostary/image-demo1:v1
$ docker run -p 4000:80 hebostary/image-demo1:v1
```

### 打包镜像文件

除了通过registry和repository来管理镜像的发布和部署，Docker还支持以tar包的形式来发布和导入镜像。

docker image save命令用于打包镜像：

```bash
$ docker image save hebostary/image-demo1:v1 -o image-demo1-v1.tar
$ ll
total 150252
-rw-r--r-- 1 root root       665 Aug 29 17:24 app.py
-rw-r--r-- 1 root root       560 Aug 29 17:24 Dockerfile
-rw------- 1 root root 153845760 Sep 22 16:32 image-demo1-v1.tar
-rw-r--r-- 1 root root        12 Aug 29 17:24 requirements.txt
```

删除本地现有的镜像hebostary/image-demo1:v1：

```bash
$ docker image rm hebostary/image-demo1:v1
```

docker image load命令用于将tar包加载成镜像：

```bash
$ docker image load -i image-demo1-v1.tar 
20db57dc9b41: Loading layer [=======================>]  1.536kB/1.536kB
e50cb3a9f387: Loading layer [=======================>]  5.632kB/5.632kB
06b157080b2e: Loading layer [=======================>]  11.55MB/11.55MB
Loaded image: hebostary/image-demo1:v1
```

因为实验机器上存在构建image-demo1:v1时用的python:2.7-slim镜像，并且其镜像ID没有改变，所以Docker只需要加载基础镜像之上的这些layer。如果基础镜像的标签相同，但是现有的镜像ID和构建时用到的镜像ID不同（意味着基础镜像被更新了），那么Docker将会重新拉取或者加载所有的layer。

以tar包的形式发布镜像看起来是一种比较传统的方式，但是在一些部署企业级应用的场景下非常有用。比如客户的网络是私有网络，无法直接从镜像提供商的仓库拉取镜像，此时以tar包的形式发布镜像会是不错的选择。

### 如何让镜像更小？

更小的镜像意味着在发布、拉取和部署镜像各个阶段都花费更少的时间，获得更好的性能体验。这里总结了一些最佳实践经验：

1. 从一个比较合适的基础镜像（base image）开始构建应用的镜像。比如我们的应用基于Apache Tomcat，选择一个合适的Tomcat版本作为基础镜像要比用直接用JDK作为基础镜像更高效。

2. 使用多阶段构建（multistage build）。如果你的Docker版本不支持多阶段构建，那就在Dockerfile中尽量减少layer的数量，比如把多个RUN命令合并成一条：

   ```shell
   RUN apt-get -y update
   RUN apt-get install -y python
   = >
   RUN apt-get -y update && apt-get install -y python
   ```

3. 如果多个镜像有很多相同的地方，考虑使用共享组件构建自己的基础镜像。Docker只需要加载一次公共layer，并且缓存起来。这意味着派生镜像在docker主机上使用内存的效率更高，加载速度更快。

4. 保持产品镜像精简，但同时可调试。可以考虑使用生产镜像作为调试镜像的基础镜像，然后在基础镜像之上添加其他调试和测试工具。

## 容器存储驱动（Storage Driver）

AUFS，Overlay FS这些都属于联合文件系统，它们依赖于特定的底层磁盘文件系统，比如xfs和extfs，所以处理的最小粒度是文件，而不是文件块。

Docker默认的驱动是Overlay2：

```bash
[root@izuf682lz6444cynn96up0z ~]# docker info
...
 Storage Driver: overlay2
  Backing Filesystem: extfs
  Supports d_type: true
  Native Overlay Diff: false
```

### Overlay2

Overlay FS只支持两个layer：镜像层和 容器层，如果镜像层有更多的layer，就需要通过硬链接来引用更底层共享的数据，比如最底层layer有一个文件aa，那么在所有的上层layer都会有aa的硬链接，这就会导致对inode的过度使用。

如果要选用Overlay FS作为Docker存储驱动，就使用overlay2而不是overlay，因为overlay2在inode利用率方面更有效。

#### Overlay2如何管理容器和镜像？

我们先来看一下Overlay2管理docker镜像的基本文件结构：

```bash
# image及其layer的元数据
$ tree -L 3 /var/lib/docker/image
`-- overlay2
    |-- distribution
    |   |-- diffid-by-digest
    |   `-- v2metadata-by-diffid
    |-- imagedb # 根据image ID存储image的元数据
    |   |-- content 
    |   `-- metadata
    |-- layerdb # 根据layer ID存储image layer的元数据
    |   |-- mounts
    |   |-- sha256
    |   `-- tmp
    `-- repositories.json
# image layer的有效数据
$ tree -L 1 /var/lib/docker/overlay2/
|-- 3e9b5c4a51cc3fe1c2fdf808f19786b036af40853757541833c80e1871b2b2cd
|-- 5fef3a761ef8bf683307f8534d0278f8b4558fc36bd5774ac1e816a3197c251d
|-- 65eaa13fee6e0f39410b70b6aa4bc376e2c0878e5801ff73763b6d98d8d32c7e
|-- 98a91e947ec4b0a7ae0da3a5cfb33e5aa9f33a0dbd3de2d4a0715bad8ff23668
|-- b40e51222410781d1c27a8ca595d5ec2474fa18b774dbe9f7e62a1d0ab8c939a
|-- f890d662b6763783f55e7f326e844de7ee96c47d23c766766f404a3f4ff1b022
`-- l
```

其中，元数据管理最重要的是`imagedb`和`layerdb`两个目录，它们分别存储了镜像的元数据信息和组成镜像的layer的元数据信息。

下面根据nginx镜像来分析overlay2是如何管理镜像的，先拉取最新的nginx镜像：

```bash
$ docker pull nginx:latest
latest: Pulling from library/nginx
07aded7c29c6: Pull complete
bbe0b7acc89c: Pull complete
44ac32b0bba8: Pull complete
91d6e3e593db: Pull complete
8700267f2376: Pull complete
4ce73aa6e9b0: Pull complete
Digest: sha256:06e4235e95299b1d6d595c5ef4c41a9b12641f6683136c18394b858967cd1506
Status: Downloaded newer image for nginx:latest
docker.io/library/nginx:latest
```

先看一下imagedb里的数据，根据镜像ID可以找到镜像的元数据文件，里面的内容很多，这里只摘取了rootfs部分：

```bash
$ docker image ls
REPOSITORY          TAG                 IMAGE ID            CREATED             SIZE
nginx               latest              f8f4ffc8092c        8 days ago          133MB
$ ll /var/lib/docker/image/overlay2/imagedb/content/sha256/
total 8
-rw------- 1 root root 7729 Oct  7 11:41 f8f4ffc8092c956ddd6a3a64814f36882798065799b8aedeebedf2855af3395b
$ cat /var/lib/docker/image/overlay2/imagedb/content/sha256/f8f4ffc8092c956ddd6a3a64814f36882798065799b8aedeebedf2855af3395b
{"architecture":"amd64",...,"os":"linux","rootfs":{"type":"layers","diff_ids":[
"sha256:476baebdfbf7a68c50e979971fcd47d799d1b194bcf1f03c1c979e9262bcd364",
"sha256:5259501115588b1be0b1bb6eee115422d2939f402137979603cea9d9f1e649ec",
"sha256:0772cb25d5cae1b4e6e47ff15af95fa1d2640c3b7c74cb4c008d61e2c8c28559",
"sha256:6e109f6c2f99fdfa436dd66299d2ed87a18fee00b5f22fbd761dbacac27b76a6",
"sha256:88891187bdd7d71eeaa5f468577eb253eca29f57e3577ea0a954f6991313fd71",
"sha256:65e1ea1dc98ccb565bf8dd0f7664fc767796d3a6eecaf29b79ce7e9932517ae5"
]}
```

rootfs部分记录了镜像的各个layer，第一个layer ID（476baebdfbf7）是这个镜像最底层的layer，以此类推。这些layer的具体信息都存放在`layerdb`目录，但是查看`layerdb`目录，我们仅仅能找到第一个layer ID的目录（476baebdfbf7）：

```bash
$ tree -L 2 /var/lib/docker/image/overlay2/layerdb/
|-- mounts
|-- sha256
|   |-- 3b06b30cf952c2f24b6eabdff61b633aa03e1367f1ace996260fc3e236991eec
|   |-- 476baebdfbf7a68c50e979971fcd47d799d1b194bcf1f03c1c979e9262bcd364 # 最底层layer
|   |-- c47815d475f74f82afb68ef7347b036957e7e1a1b0d71c300bdb4f5975163d6a
|   |-- c9958d4f33715556566095ccc716e49175a1fded2fa759dbd747750a89453490
|   |-- ed6dd2b44338215d30a589d7d36cb4ffd05eb28d2e663a23108d03b3ac273d27
|   `-- f208904eecb00a0769d263e81b8234f741519fefa262482d106c321ddc9773df
`-- tmp

9 directories, 0 files
# 每个layer ID目录下面的结构如下：
$ tree -L 1 /var/lib/docker/image/overlay2/layerdb/sha256/476baebdfbf7a68c50e979971fcd47d799d1b194bcf1f03c1c979e9262bcd364/
├── cache-id
├── diff
├── size
└── tar-split.json.gz
```

这是因为Docker使用了`chainID`的方式去记录这些layer信息：**chainID=sha256sum(H(chainID) diffid)**，我们可以通过最底层的layer ID手动依次计算上层的layer ID：

```bash
$ echo -n "sha256:476baebdfbf7a68c50e979971fcd47d799d1b194bcf1f03c1c979e9262bcd364 sha256:5259501115588b1be0b1bb6eee115422d2939f402137979603cea9d9f1e649ec" | sha256sum -
3b06b30cf952c2f24b6eabdff61b633aa03e1367f1ace996260fc3e236991eec  - # 第2层layer ID
$ echo -n "sha256:3b06b30cf952c2f24b6eabdff61b633aa03e1367f1ace996260fc3e236991eec sha256:0772cb25d5cae1b4e6e47ff15af95fa1d2640c3b7c74cb4c008d61e2c8c28559" | sha256sum -
c47815d475f74f82afb68ef7347b036957e7e1a1b0d71c300bdb4f5975163d6a  -
$ echo -n "sha256:c47815d475f74f82afb68ef7347b036957e7e1a1b0d71c300bdb4f5975163d6a sha256:6e109f6c2f99fdfa436dd66299d2ed87a18fee00b5f22fbd761dbacac27b76a6" | sha256sum -
c9958d4f33715556566095ccc716e49175a1fded2fa759dbd747750a89453490  -
```

计算后得到的这些`chainID`，我们都可以在`/var/lib/docker/image/overlay2/layerdb/`下面找到对应的子目录。

但是，`layerdb`下面只有image layer的元数据，其有效数据需要通过`layerdb`下面的`cache-id`作为索引到`/var/lib/docker/overlay2/`下面去查找：

```bash
$ cat /var/lib/docker/image/overlay2/layerdb/sha256/476baebdfbf7a68c50e979971fcd47d799d1b194bcf1f03c1c979e9262bcd364/cache-id 
5fef3a761ef8bf683307f8534d0278f8b4558fc36bd5774ac1e816a3197c251d
```

下面就以最底层的layer（476baebdfbf7）的`cache-id`来追踪其有效数据：

```bash
$ tree -L 3 /var/lib/docker/overlay2/
├── 3e9b5c4a51cc3fe1c2fdf808f19786b036af40853757541833c80e1871b2b2cd
│   ├── diff
│   │   ├── docker-entrypoint.d
│   │   ├── etc
│   │   ├── lib
│   │   ├── tmp
│   │   ├── usr
│   │   └── var
│   ├── link
│   ├── lower
│   └── work
├── 5fef3a761ef8bf683307f8534d0278f8b4558fc36bd5774ac1e816a3197c251d
│   ├── diff #diff目录包含了这个layer的有效数据，最底层的layer基本上包含了完整的rootfs目录结构
│   │   ├── bin
│   │   ├── boot
│   │   ├── dev
│   │   ├── etc
│   │   ├── home
...
│   │   ├── usr
│   │   └── var
│   └── link
├── 65eaa13fee6e0f39410b70b6aa4bc376e2c0878e5801ff73763b6d98d8d32c7e
│   ├── diff
│   │   └── docker-entrypoint.sh
│   ├── link
│   ├── lower
│   └── work
├── 98a91e947ec4b0a7ae0da3a5cfb33e5aa9f33a0dbd3de2d4a0715bad8ff23668
│   ├── diff
│   │   └── docker-entrypoint.d
│   ├── link
│   ├── lower
│   └── work
├── b40e51222410781d1c27a8ca595d5ec2474fa18b774dbe9f7e62a1d0ab8c939a
│   ├── diff
│   │   └── docker-entrypoint.d
│   ├── link
│   ├── lower
│   └── work
├── f890d662b6763783f55e7f326e844de7ee96c47d23c766766f404a3f4ff1b022
│   ├── diff
│   │   └── docker-entrypoint.d
│   ├── link
│   ├── lower
│   └── work
└── l
    ├── 4TPQJK7G2ALUFMEKOSSPI6QVAQ -> ../5fef3a761ef8bf683307f8534d0278f8b4558fc36bd5774ac1e816a3197c251d/diff
    ├── 7FFMHVYKMXHNTECOHHSMYWB4G6 -> ../3e9b5c4a51cc3fe1c2fdf808f19786b036af40853757541833c80e1871b2b2cd/diff
    ├── G6TZLR42VZ3R3XJKXNRMO4ADX6 -> ../65eaa13fee6e0f39410b70b6aa4bc376e2c0878e5801ff73763b6d98d8d32c7e/diff
    ├── NRJ4UR5YY2GOR2UOC4VRDFW3PF -> ../b40e51222410781d1c27a8ca595d5ec2474fa18b774dbe9f7e62a1d0ab8c939a/diff
    ├── TIQ56PJDQGMOXGGSSHLLOK37DF -> ../98a91e947ec4b0a7ae0da3a5cfb33e5aa9f33a0dbd3de2d4a0715bad8ff23668/diff
    └── URSLIJXJMIPTOTPOGTSBUDJJMA -> ../f890d662b6763783f55e7f326e844de7ee96c47d23c766766f404a3f4ff1b022/diff
```

最底层的layer只有`diff`目录和一个`link`文件，而其它layer还包含了`lower`和`work`两个文件。

```bash
$ cat /var/lib/docker/overlay2/5fef3a761ef8bf683307f8534d0278f8b4558fc36bd5774ac1e816a3197c251d/link 
4TPQJK7G2ALUFMEKOSSPI6QVAQ
```

`link`文件里面存放着一个26个字符组成的`linkID`，那么这个`linkID`的意义又是什么呢？在`/var/lib/docker/overlay2`下面有一个很特殊的`l`目录，它正好存放着与`linkID`关联的软链接，这些软链接指向对应的layer下面的diff目录，即这个`linkID`的目录存储着这个layer的数据，它的绝对路径也会被用于联合挂载。这个 linkID 的主要原因是用于避免mount 命令参数的大小限制。

```bash
$ tree -L 2 /var/lib/docker/overlay2/l/
/var/lib/docker/overlay2/l/
├── 4TPQJK7G2ALUFMEKOSSPI6QVAQ -> ../5fef3a761ef8bf683307f8534d0278f8b4558fc36bd5774ac1e816a3197c251d/diff
├── 7FFMHVYKMXHNTECOHHSMYWB4G6 -> ../3e9b5c4a51cc3fe1c2fdf808f19786b036af40853757541833c80e1871b2b2cd/diff
├── G6TZLR42VZ3R3XJKXNRMO4ADX6 -> ../65eaa13fee6e0f39410b70b6aa4bc376e2c0878e5801ff73763b6d98d8d32c7e/diff
├── NRJ4UR5YY2GOR2UOC4VRDFW3PF -> ../b40e51222410781d1c27a8ca595d5ec2474fa18b774dbe9f7e62a1d0ab8c939a/diff
├── TIQ56PJDQGMOXGGSSHLLOK37DF -> ../98a91e947ec4b0a7ae0da3a5cfb33e5aa9f33a0dbd3de2d4a0715bad8ff23668/diff
└── URSLIJXJMIPTOTPOGTSBUDJJMA -> ../f890d662b6763783f55e7f326e844de7ee96c47d23c766766f404a3f4ff1b022/diff
```

`lower`文件记录着该layer的所有下层layer的顺序关系和相对位置，所以最底层的layer没有这个文件。

```bash
$ cat /var/lib/docker/overlay2/98a91e947ec4b0a7ae0da3a5cfb33e5aa9f33a0dbd3de2d4a0715bad8ff23668/lower 
l/NRJ4UR5YY2GOR2UOC4VRDFW3PF:l/URSLIJXJMIPTOTPOGTSBUDJJMA:l/G6TZLR42VZ3R3XJKXNRMO4ADX6:l/7FFMHVYKMXHNTECOHHSMYWB4G6:l/4TPQJK7G2ALUFMEKOSSPI6QVAQ
```

当前nginx image一共包含了6个只读layer，这些layer在宿主机上是共享的。我们现在启动一个nginx容器：

```bash
$ docker run -dti --name nginx-c1 nginx
c41b411ab88a1f9faca0b9dad1f24b2b1ac60d02ab1177cc8f523873bd49a992
```

启动名为nginx-c1的容器后，docker在`/var/lib/docker/image/overlay2/layerdb/mounts/`下创建了一个以容器ID命名的目录，我们看一下这些文件里面都存放着什么：

```bash
$ ll /var/lib/docker/image/overlay2/layerdb/mounts/c41b411ab88a1f9faca0b9dad1f24b2b1ac60d02ab1177cc8f523873bd49a992/
total 12
-rw-r--r-- 1 root root 69 Oct  8 22:04 init-id
-rw-r--r-- 1 root root 64 Oct  8 22:04 mount-id
-rw-r--r-- 1 root root 71 Oct  8 22:04 parent
$ cat /var/lib/docker/image/overlay2/layerdb/mounts/c41b411ab88a1f9faca0b9dad1f24b2b1ac60d02ab1177cc8f523873bd49a992/init-id 
9c56824cddab9da90d6a360d0f309fab6e9bcef7692d92b9b1be5511ffb5be8f-init
$ cat /var/lib/docker/image/overlay2/layerdb/mounts/c41b411ab88a1f9faca0b9dad1f24b2b1ac60d02ab1177cc8f523873bd49a992/mount-id 
9c56824cddab9da90d6a360d0f309fab6e9bcef7692d92b9b1be5511ffb5be8f
$ cat /var/lib/docker/image/overlay2/layerdb/mounts/c41b411ab88a1f9faca0b9dad1f24b2b1ac60d02ab1177cc8f523873bd49a992/parent 
sha256:f208904eecb00a0769d263e81b8234f741519fefa262482d106c321ddc9773df
```

我们先来探究一下这个`mount-id`，也就是9c56824cddab9da。容器启动后，在`/var/lib/docker/overlay2/`下面多出了下面两个子目录9c56824cddab9da 和 9c56824cddab9da-init。

```bash
├── 9c56824cddab9da90d6a360d0f309fab6e9bcef7692d92b9b1be5511ffb5be8f
│   ├── diff
│   │   ├── etc
│   │   ├── run
│   │   ├── usr
│   │   └── var
│   ├── link
│   ├── lower
│   ├── merged # 所有底层layer的数据都被联合挂载到merged目录，它将作为容器的rootfs
│   │   ├── bin
│   │   ├── boot
│   │   ├── dev
│   │   ├── docker-entrypoint.d
│   │   ├── docker-entrypoint.sh
│   │   ├── etc
...
│   │   ├── usr
│   │   └── var
│   └── work
│       └── work
├── 9c56824cddab9da90d6a360d0f309fab6e9bcef7692d92b9b1be5511ffb5be8f-init
│   ├── diff
│   │   ├── dev
│   │   └── etc
│   ├── link
│   ├── lower
│   └── work
│       └── work
```

实际上，9c56824cddab9da 就是位于容器顶层的可读写层，也叫容器层，9c56824cddab9da-init是位于次顶层的Init层，它处于只读层和读写层之间。

> Init层是Docker项目单独生成的一个内部层，专门用来存放/etc/hosts，/etc/resolv.conf等信息，因为这些配置文件本来属于只读layer的一部分，但是用户往往在启动容器时就会写入一些指定的值，比如hostname，所以就需要在可读写层对/etc/hosts进行修改。但是，这些修改往往只对当前的容器有意义，我们并不希望执行`docker commit`时将这些信息一起提交到image。因此Docker的方案是，这些文件被修改后，被放到一个单独的init层来管理，并且`docker commit`不会提交这个Init层。

结合下面这张aufs的图来理解就很清楚了：

![img](https://hunk-pic-store.oss-cn-beijing.aliyuncs.com/img/20211008222911.png)

因此，当前这个nginx-c1容器一共包含了8个layer，这8个layer的内容都被合并到读写层的`merged`目录，形成文件系统的最终挂载点。这个`merged`目录就是容器的rootfs，容器内部的根目录试图实际上就是merged目录的内容：

```bash
$ ls /var/lib/docker/overlay2/9c56824cddab9da90d6a360d0f309fab6e9bcef7692d92b9b1be5511ffb5be8f/merged
bin   dev                  docker-entrypoint.sh  home  lib64  mnt  proc  run   srv  tmp  var
boot  docker-entrypoint.d  etc                   lib   media  opt  root  sbin  sys  usr
$docker exec -ti nginx-c1 bash 
root@c41b411ab88a:/# ls /
bin   dev                  docker-entrypoint.sh  home  lib64  mnt  proc  run   srv  tmp  var
boot  docker-entrypoint.d  etc                   lib   media  opt  root  sbin  sys  usr
```

下面我们再来看一下overlay文件系统是如何合并挂载这些layer的（为了方便阅读，做了一些格式化）：

```bash
$ mount -l | grep 9c56824cddab9da
overlay on /var/lib/docker/overlay2/9c56824cddab9da90d6a360d0f309fab6e9bcef7692d92b9b1be5511ffb5be8f/merged type overlay (rw,relatime,lowerdir=
/var/lib/docker/overlay2/l/Q5R4UR4WUJBR44BI25HP4BUCER: # Init层
/var/lib/docker/overlay2/l/TIQ56PJDQGMOXGGSSHLLOK37DF:
/var/lib/docker/overlay2/l/NRJ4UR5YY2GOR2UOC4VRDFW3PF:
/var/lib/docker/overlay2/l/URSLIJXJMIPTOTPOGTSBUDJJMA:
/var/lib/docker/overlay2/l/G6TZLR42VZ3R3XJKXNRMO4ADX6:
/var/lib/docker/overlay2/l/7FFMHVYKMXHNTECOHHSMYWB4G6:
/var/lib/docker/overlay2/l/4TPQJK7G2ALUFMEKOSSPI6QVAQ, # 只读层
upperdir= #读写层
/var/lib/docker/overlay2/9c56824cddab9da90d6a360d0f309fab6e9bcef7692d92b9b1be5511ffb5be8f/diff, 
workdir=
/var/lib/docker/overlay2/9c56824cddab9da90d6a360d0f309fab6e9bcef7692d92b9b1be5511ffb5be8f/work)
```

> 只读层和Init层都是通过`lnikID`来直接访问相关镜像layer的数据，而读写层没有通过这个`linkID`来访问，而是直接用的diff目录的路径。
>
> Overlay2原生支持最多128个layer，linkID 时26个字符，而layer ID的长度是64个字符，挂载时就可以显示出使用 `linkID` 的好处了。

下面这张图描述了overlay的分层文件结构，虽然其只支持两个layer，但是overlay2的基本原理也是一样的：

![overlayfs lowerdir, upperdir, merged](https://hunk-pic-store.oss-cn-beijing.aliyuncs.com/img/20211009220503.jpeg)

#### Overlay FS如何处理文件/目录访问请求？

考虑Overlay FS读文件时的3种情况：

1. 文件不存在于upper层：先访问upper层，发现文件不存在，继续从lower层访问该文件，这里会造成一点性能损失。
2. 文件只存在于读写层：直接从upper层访问该文件。
3. 文件同时存在与读写层和只读层：访问upper层的该文件。

考虑Overlay FS修改文件或者目录的3种情况：

1. 第一次写入一个文件：用户以写方式打开lower层的文件时，如果这个文件在upper层不存在，overlay/overlay2驱动就先执行`copy_up`操作将该文件从lower层拷贝到upper层，然后将更新写入upper层的副本。尽管写入的内容很少，而文件本身很大，也无法避免一次拷贝操作，因为OverlayFS处理的最小粒度是文件而不是块。`copy_up`操作只在第一次写入该文件时发生，后续继续写入时就可以直接操作upper层的文件了。
2. 删除文件或者目录：在容器内删除一个文件时，在upper层会创建一个`whiteout`文件（块设备文件类型），而lower层的该文件保持不变，但是`whiteout`文件可以阻止容器继续访问该文件。在容器内删除目录时，也是类似的机制，只不过在upper层创建是一个`opaque directory`（也是块设备文件类型）。
3. 重命名目录：只有源路径和目标路径都在upper层的时候，才允许对目录执行`rename`调用。否则，将会返回 EXDEV 错误（“cross-device link not permitted”）。应用程序需要处理这个错误。

>  从Overlay到Docker的世界里，upper层就是容器层，或者叫读写层；lower层可指代镜像等，或者叫只读层。

作为实验，我们到前面创建的容器里做一些文件操作：

```bash
root@c41b411ab88a:/# mkdir -p /home/hunk; touch /home/hunk/test.txt #创建新文件
root@c41b411ab88a:/# echo "# test layer change"  >> /docker-entrypoint.sh #修改只读层文件
```

那么在可读写层可以直接看到这些修改，而下面的只读层并不会被修改：

```bash
$ tree -L 3 /var/lib/docker/overlay2/9c56824cddab9da90d*/diff/
├── etc
│   └── nginx
│       └── conf.d
├── home
│   └── hunk
│       └── test.txt
...
$ cat /var/lib/docker/overlay2/9c56824cddab9da90d*/diff/docker-entrypoint.sh  | grep test
# test layer change
```

如果直接在宿主机上的读写层目录下修改来源于只读层的文件，结果是一样的，因为这些IO请求最终都会落到overlay/overlay2文件系统驱动上。

现在，我们在容器里删除一个文件和一个目录，它们都来源于镜像层：

```bash
root@c41b411ab88a:/# rm -rf /docker-entrypoint.d # 删除镜像层的目录
root@c41b411ab88a:/# rm -rf /etc/ssl/openssl.cnf #删除镜像层的文件
root@c41b411ab88a:/# ls /etc/ssl/openssl.cnf
ls: cannot access '/etc/ssl/openssl.cnf': No such file or directory
root@c41b411ab88a:/# ls /etc/ssl/openssl.cnf /docker-entrypoint.d
ls: cannot access '/etc/ssl/openssl.cnf': No such file or directory
```

执行删除后，在容器层就无法再访问该文件和目录了。这是因为，在容器层的`whiteout`文件和`opaque`目录阻止了容器对它们的访问。

```bash
$ ls -al  diff/docker-entrypoint.d 
c--------- 1 root root 0, 0 Oct  9 21:53 diff/docker-entrypoint.d
$ ls -al  diff/etc/ssl/openssl.cnf 
c--------- 1 root root 0, 0 Oct  9 21:54 diff/etc/ssl/openssl.cnf
```

总之，overlay文件系统在处理这些IO操作时，发现如果是更新下面只读层的文件，就会先将这些文件拷贝到读写层的diff目录，然后更新读写层的文件副本，从而使只读层的内容保持不变。在容器内读取文件时，Overlay将遵循从上往下查找layer的原则，容器内看到的就始终是自己更新过的读写层的文件副本。这个机制也叫写时复制（COW，Copy-On-Write）。

#### 再探联合挂载点merged目录

我们在前面看到，当启动容器时，所有layer的文件都会联合挂载到`merged`目录，看起来这些文件都是从各个layer的diff目录拷贝到了`merged`目录，那每启动一个容器，就要拷贝一次所有镜像层的所有文件？这个显然是不可能的，但是新人在这里也很容易困惑，下面我们在容器层查找一个来源于镜像层的文件 ca-certificates.conf：

```bash
$ find . -name ucf.conf |xargs ls -i
1838644 ./3e9b5c4a51cc3fe*/diff/etc/ucf.conf
1838644 ./9c56824cddab9da90d*/merged/etc/ucf.conf
```

这就很显然了，容器层merged目录和镜像层的该文件实际上是共享的文件系统里的同一个inode，本质上指向的是同一个文件，并没有拷贝文件。所以，当我们在同一个主机上用一个镜像同时启动多个容器，看到对磁盘空间的消耗并不会呈倍数增长。

我们再回顾一下Linux 文件里的硬链接和软链接（符号链接）：

1. 硬链接：`ln <src> <des>`；硬链接和其源文件共享同一个inode，inode信息中有一项叫做**链接数**，记录指向该inode的文件名总数。创建一个文件名时，这个链接数就增加1，删除一个文件名（可以直接删除源文件），就会使得inode节点中的"链接数"减1。当这个值减到0，表明没有文件名指向这个inode，系统就会回收这个inode号码，以及其所对应文件块。

   ```bash
   $ echo "hunk test" > inode_test
   $ ln inode_test inode_test1
   $ stat inode_test inode_test1 | grep "Inode"
   Device: fd01h/64769d    Inode: 1319099     Links: 2
   Device: fd01h/64769d    Inode: 1319099     Links: 2
   ```

   硬链接和源文件之间是平等的关系，无论先删除哪个文件对其它的都没有影响。

   ```bash
   $ rm -f inode_test #直接删除最开始建的文件
   $ stat inode_test1 | grep "Inode"
   Device: fd01h/64769d    Inode: 1319099     Links: 1
   $ cat inode_test1 
   hunk test
   ```

2. 软链接：`ln -s <src> <des>`；创建软链接时直接分配了新的inode，并且新的inode保存了源文件的名字，因此访问软链接文件时，本质上访问的还是源文件。软链接本身类似于一个"快捷方式"，和源文件之间有严格依赖关系。

   ```bash
   $ echo "hunk test" > inode_test 
   $ ln -s inode_test inode_test1
   $ stat inode_test inode_test1 | grep "Inode"
   Device: fd01h/64769d    Inode: 1319099     Links: 1
   Device: fd01h/64769d    Inode: 1319101     Links: 1
   ```

   修改源文件时，源文件的大小增加了，但是软链接文件的大小并不会变化，因为它的inode只存了源文件的文件名：

   ```bash
   $ stat inode_test inode_test1 | grep Size
     Size: 60           Blocks: 8          IO Block: 4096   regular file
     Size: 10           Blocks: 0          IO Block: 4096   symbolic link
   ```

   因此，删除源文件后，所有的软链接都将失效，不能再访问：

   ```bash
   $ rm -f inode_test
   $ stat inode_test1 | grep "Inode"
   Device: fd01h/64769d    Inode: 1319101     Links: 1
   $ cat inode_test1
   cat: inode_test1: No such file or directory
   ```

硬链接共享了inode，通过“链接数”来管理对inode的引用。软链接分配了新的inode，也就不再需要“链接数”。但是，Overlay FS里面，容器层和联合挂载点下的文件共享了inode，类似于硬链接，但是却没有增加"链接数"。我们尝试去修改镜像层的/etc/ucf.conf，修改被写入容器层的副本，联合挂载点的该文件指向upper层的副本。

```bash
$ echo "# hunk test" >> ./3e9b5c4a51cc3fe1*/diff/etc/ucf.conf
$ find . -name ucf.conf |xargs ls -i
1838644 ./3e9b5c4a51cc3fe*/diff/etc/ucf.conf
1839776 ./9c56824cddab9da90d*/diff/etc/ucf.conf
1839776 ./9c56824cddab9da90d*/merged/etc/ucf.conf
```

merged目录下的文件并不是通过创建任何形式的链接文件来实现的，这个目录不能当作普通的Ext FS目录来看待它的内容，它是Overlay FS提供的一个统一的前端视图，目录下的文件实际上就是lower层或者upper层的文件本身，这也是为何叫联合挂载点的原因。

#### Overlay FS和Docker性能

Overlay FS的性能要优于aufs和devicemapper：

1. OverlayFS支持页面缓存共享（Page Caching）。访问同一文件的多个容器共享该文件的单个页面缓存项（page cache entry）。这使得overlay和overlay2驱动程序在内存方面更高效。
2. copy_up操作是可能造成比较大性能延迟的，比如将很小的改动写入一个镜像层很大的文件，但是这种延迟仅限于第一次写入时出现。相比之下，overlay的copy_up操作的效率还是要高于aufs，因为aufs支持的layer还要更多一些。

性能最佳实践：

1. 使用更快的存储设备，比如SSD。
2. 对于写负载重的工作负载使用volume：volume为写负载重的工作负载提供最佳和最可预测的性能。这是因为它们绕过了存储驱动程序，并且不会产生任何由精简配置和写时复制带来的潜在开销。volume还有其他好处，比如允许在容器之间共享数据，即使没有正在运行的容器使用数据，也可以将数据持久化。

## 容器应用存储

Docker主要为容器里的应用提供了下图中的3种存储类型，以满足不同场景的需求：

![img](https://hunk-pic-store.oss-cn-beijing.aliyuncs.com/img/20211006210828.png)

### Bind mounts

1. Bind mounts may be stored *anywhere* on the host system. They may even be important system files or directories. Non-Docker processes on the Docker host or a Docker container can modify them at any time.

2. When you use a bind mount, a file or directory on the *host machine* is mounted into a container. The file or directory is referenced by its full path on the host machine. The file or directory does not need to exist on the Docker host already. It is created on demand if it does not yet exist. 
3. You can’t use Docker CLI commands to directly manage bind mounts.
4. One side effect of using bind mounts, for better or for worse, is that you can change the **host** filesystem via processes running in a **container**, including creating, modifying, or deleting important system files or directories. This is a powerful ability which can have security implications, including impacting non-Docker processes on the host system.

volume所在的路径/var/lib/docker/volume是由docker统一管理的，Bind mounts是直接将host上已经存在的目录或者文件挂载到container中的文件或者目录，也通过-v或者--mount来实现bind mount挂载。

```bash
docker run -d -it --name devtest --mount type=bind,source="$(pwd)"/target,target=/app nginx:latest
```

If you bind-mount into a non-empty directory on the container, the directory’s existing contents are obscured by the bind mount. 

note：实际上即使挂载的是host上的空目录，container里的挂载目录里的内容也会被冲掉。

### Volume

1. Volumes are stored in a part of the host filesystem which is *managed by Docker*(/var/lib/docker/volumes/ on Linux). Non-Docker processes should not modify this part of the filesystem. Volumes are the best way to persist data in Docker.
2. A given volume can be mounted into multiple containers simultaneously.
3. Volumes also support the use of *volume drivers*, which allow you to store your data on remote hosts or cloud providers, among other possibilities.

If you start a container which creates a new volume, as above, and the container has files or directories in the directory to be mounted (such as /app above), the directory’s contents are copied into the volume. The container then mounts and uses the volume, and other containers which use the volume also have access to the pre-populated content.

Volumes have several advantages over bind mounts:

- Volumes are easier to back up or migrate than bind mounts.
- **You can manage volumes using Docker CLI commands or the Docker API.** 
- Volumes work on both Linux and Windows containers.
- **Volumes can be more safely shared among multiple containers.**
- Volume drivers let you store volumes on remote hosts or cloud providers, to encrypt the contents of volumes, or to add other functionality.
- New volumes can have their content pre-populated by a container.
- volume does not increase the size of the containers using it, and the volume’s contents exist outside the lifecycle of a given container.

### tmpfs mount

1. tmpfs mounts are stored in the host system's memory only, and are never written to the host system's filesystem.
2. It can be used by a container during the lifetime of the container, to store non-persistent state or sensitive information.
3. tmpfs mounts are best used for cases when you do not want the data to persist either on the host machine or within the container. This may be for security reasons or to protect the performance of the container when your application needs to write a large volume of non-persistent state data.



### Tips for using bind mounts or volumes

- If you mount an **empty volume** into a directory in the container in which files or directories exist, these files or directories are propagated (copied) into the volume. Similarly, if you start a container and specify a volume which does not already exist, an empty volume is created for you. This is a good way to pre-populate data that another container needs.
- If you mount a **bind mount or non-empty volume** into a directory in the container in which some files or directories exist, these files or directories are obscured by the mount, just as if you saved files into /mnt on a Linux host and then mounted a USB drive into /mnt. The contents of /mnt would be obscured by the contents of the USB drive until the USB drive were unmounted. The obscured files are not removed or altered, but are not accessible while the bind mount or volume is mounted.

### Share data among machines

There are several ways to achieve this when developing your applications. 

1. One is to add logic to your application to store files on a cloud object storage system like Amazon S3. 
2. Another is to create volumes with a driver that supports writing files to an external storage system like NFS or Amazon S3.

```bash
docker volume prune  //remove all unused volumes and free up space
```



## Docker网络模型

### CNM（Container Networking Model）

### Host Driver
和后面介绍的Bridge、Macvlan、Overlay Driver不同，用host driver创建的网络没有独立的网络命名空间（network namespace）。对于配置了host driver网络的所有容器，都在相同的主机网络命名空间（host network namespace）中，都直接使用主机的网络接口和IP栈。**从网络的角度来看，这些容器里的应用相当于在没有容器的主机上运行多个进程，所以这些容器也不能绑定到相同的主机端口。**


Docker自动创建叫host的host driver网络，并且它是唯一的，我们无法手动创建更多的host driver网络。
```sh
[root@hunk ~]# docker network ls |grep host
62c10382a30e        host                host                local
[root@hunk ~]# docker network create -d host host1
Error response from daemon: only one instance of "host" network is allowed
```

### Bridge Driver

https://docs.docker.com/network/bridge/

桥接网络作为一种链接层设备，它转发网络之间的流量，桥接器可以是内核中的硬件设备或者软件设备。

docker的桥接网络是一种软桥接网络，允许连接到同一桥接网络的容器进行通信，同时提供未连接该桥接网络之间的隔离。所以，不同docker主机上的容器间无法通过桥接网络来通信。

**default bridge network**

创建容器时，如果没有指定--network参数，而是指定了network driver，容器将会连接到默认的桥接网络。

```bash
/etc/docker/daemon.json #配置默认桥接网络，修改后需要重启docker host使配置生效
```

**user-defined bridge network**

自定义的桥接网络可以在容器运行时动态连接和断开连接，而从默认桥接网络移除容器时需要停掉容器并重新创建。

```bash
docker network connect my-net my-nginx #将已经running的容器连接到自定义桥接网络
docker network disconnect my-net my-nginx #将已经running的容器从自定义桥接网络端口
```

连接到默认桥接网络的容器只能通过IP地址通信，而连接到相同自定义桥接网络的容器间可以通过IP地址和主机名来通信（支持DNS解析）

考虑到管理的诸多便捷性，推荐尽量使用自定义桥接网络。管理桥接网络的基本命令：

```bash
docker network create --help
docker network create <name>  #创建自定义桥接网络 更多参数参考：https://docs.docker.com/engine/reference/commandline/network_create/#specify-advanced-options
docker network rm <name> #删除网络
```

连接到相同user-defined bridge network的容器会将所有端口对彼此公开。要使不同网络上的容器或者非docker主机能够访问端口，必须使用`-p`或者`--public`标签公开该端口。

```bash
docker create --name my-nginx \
  --network my-net \ #指定网络
  --publish 8080:80 \ #将容器的80端口暴露到docker host的8080端口
  nginx:latest
```

### Macvlan Driver

### Overlay Driver

https://docs.docker.com/network/overlay/

`overlay`网络可以覆盖多个docker主机的网络，允许在不同docker主机上的容器之间通信，包括swarm服务中的容器，这也是overlay网络的主要用途。Docker负责将网络包从正确的docker主机路由到正确的目标容器里。

当我们初始化一个swarm或者将docker host加入到swarm中时，docker会在docker host中自动创建下面两个网络：

```bash
root@hunk-virtual-machine:/home/hunk# docker network ls
NETWORK ID          NAME                DRIVER              SCOPE
39bfe3d4d350        docker_gwbridge     bridge              local
#docker_gwbridge网络负责将单个docker后台进程连接打破swarm中的其他守护进程。
#docker_gwbridge其实就是一个网桥，将overlay网络连接到docker守护进程的物理网络。它存在于docker主机的kernel中。
g7n68xgvfxyn        ingress             overlay             swarm 
#ingress负责处理swarm service相关的控制和数据流量，当我们创建swarm service时没有将它连接到自定义的overlay网络上，
#docker默认将它连接到ingress网络。

#注：ingress和docker_gwbridge网络支持用户自定义，实际操作就是先删除再创建。
```

service或者container可以同时连接到多个网络中，但是只能通过它们共同连接的网络进行通信。

允许将swarm service和独立容器连接到一个overlay网络，但是默认地行为和配置关注点不太一样，所以分为应用于所有overlay网络的操作、应用于swarm service的操作和应用于独立容器的操作。

使用overlay网络的前提：

1. docker host需要打开以下端口：

   ```bash
   root@hunk-virtual-machine:/home/hunk# netstat -npl|grep 2377  #集群管理通信需要
   tcp6       0      0 :::2377                 :::*                    LISTEN      1309/dockerd    
   root@hunk-virtual-machine:/home/hunk# netstat -npl|grep 4789   #overlay网络流量
   udp        0      0 0.0.0.0:4789            0.0.0.0:*                           -               
   root@hunk-virtual-machine:/home/hunk# netstat -npl|grep 7946  #节点之间通信
   tcp6       0      0 :::7946                 :::*                    LISTEN      1309/dockerd    
   udp6       0      0 :::7946                 :::*          
   ```

2. 创建overlay网络之前必须使用docker swarm init创建swarm集群或者使用docker swarm join加入swarm集群。

`overlay`网络支持数据加密，但是在windous节点上不支持。

**应用于独立容器的操作**

只有在创建自定义`overlay`网络时加上`--attachable`参数，才能将独立的容器连接到overlay网络，默认地ingress网络只支持swarm service，不支持独立容器。

**应用于swarm service的操作**

连接到相同`overlay`网络的swarm services自动相互暴露所有端口，服务外部访问端口需要在`docker service create`或者`docker service update`时使用`-p`或者`--publish`来公开端口。

默认情况下，swarm service使用路由网格发布端口。当您连接到swarm节点上的已发布端口时(无论它是否运行给定的服务)，您将被重定向到正在透明地运行该服务的worker。实际上，Docker是swarm service的负载平衡器。使用路由网格的服务以虚拟IP (VIP)模式运行。甚至在每个节点上运行的service(通过`--mode`全局标志)也使用路由网格。在使用路由网格时，无法保证哪个Docker节点服务客户机请求。

要绕过路由网格，可以使用DNS轮询(DNSRR)模式启动service，方法是将`--endpointmode`标志设置为DNSRR。您必须在service前面运行自己的负载均衡器。对Docker主机上的服务名称的DNS查询将返回运行该服务的节点的IP地址列表。配置负载均衡器以使用此列表并平衡节点间的流量。

默认情况下，swarm集群控制流量和application之间的流量运行在相同的网络上，支持使用单独的网络来处理两种不同的流量，分别用`-advertising-addr`和`--datapath-addr`来指定，并且需要对加入swarm集群中的每个节点都执行这种操作。

# Docker应用实践

## Docker在生产实践中的问题

### 性能

### 安全

### 监控

1. 如何修复容器中的 top 指令以及 /proc 文件系统中的信息呢？（提示：lxcfs）