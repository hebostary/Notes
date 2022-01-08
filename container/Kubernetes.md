#  容器编排与Kubernetes作业管理

## 为什么我们需要Pod？



### 关联容器的成组调度问题

对于Linux容器而言：Namespace实现了容器资源的隔离；Cgroups实现了对容器资源的限制；联合文件系统实现了容器的rootfs。本质上，容器还是Linux系统中被隔离了的进程。

受限于容器的“单进程模型”，应用的多个模块进程必须被放到不同的容器里，而这些模块进程之间往往又需要通过local或者Unix域Socket进行通信，因此这些容器又必须同时运行在同一个宿主机上。

> 容器的“单进程模型”，并不是指容器里只能运行一个进程，而是指容器里没有类似于init或者systemd这样的设计，因此容器没有管理多个进程的能力。因为容器里的PID=1的进程就是应用本身，其它的进程都是这个PID=1的进程的子进程，而我们的应用程序并不具备管理进程的能力，当容器里的子进程异常退出时，应用程序任何知道？又如何处理垃圾收集工作？

Kubernetes就是容器世界里的分布式操作系统，当有多个宿主机时，如何调度这些“成组”的容器就成了问题。

假设一个应用场景：集群有2个节点，node-1 上有 3 GB 可用内存，node-2 有 2.5 GB 可用内存。我们的应用包含了3个容器：main、A、B，A和B依赖于同一个机器上的main容器，每个容器设置的内存配额都是1GB。

下面先来看看已有的方案是如何调度这些容器的。

Docker Swarm项目有一个容器亲密性的设计：给关联的容器A和B设置亲密性标签（比如：affinity=main），表示容器A和B必须和main容器运行在同一个机器上。当我们顺序创建这些容器时，它们就会进入Swarm的待调度队列。main容器和容器A先后出队并被调度到了node-2上，此使node-2上的内存只有0.5GB了，但是，当容器B出队开始被调度时，Swarm就不知道如何处理了：node-2上的可用资源并不满足B的配额要求，可是根据亲密性设置，B又必须运行在node-2上。

这就是一个典型的成组调度（gang scheduling）没有被妥善处理的例子。

Mesos 中就有一个资源囤积（resource hoarding）的机制，会在所有设置了 Affinity 约束的任务都达到时，才开始对它们统一进行调度。而在 Google Omega 论文中，则提出了使用乐观调度处理冲突的方法，即：先不管这些冲突，而是通过精心设计的回滚机制在出现了冲突之后解决问题。可是这些方法都谈不上完美。资源囤积带来了不可避免的调度效率损失和死锁的可能性；而乐观调度的复杂程度，则不是常规技术团队所能驾驭的。

Kubernetes对于上述容器成组调度的问题给出的方案正是Pod：**Pod 是 Kubernetes 里的原子调度单位。这就意味着，Kubernetes 项目的调度器，是统一按照 Pod 而非容器的资源需求进行计算的。**

对于上面假设的应用场景：main容器和容器A和B会被组成一个Pod，Kubenetes项目在调度时，自然会去选择可用内存等于3GB的node-1节点进行绑定，而不会考虑node-2。

Pod里的这些关联容器的典型特征包括但不限于：互相之间会发生直接的文件交换、使用 localhost 或者 Socket 文件进行本地通信、会发生非常频繁的远程调用、需要共享某些 Linux Namespace（比如，一个容器要加入另一个容器的 Network Namespace）等等。

### Pod的实现原理

首先，Pod只是Kubernetes里的一个逻辑概念。即：Kubernetes真正处理的是，还是宿主机操作系统上Linux容器的Namespace和Cgroups，而并不是一个所谓的Pod的边界或者隔离环境。

Pod本质上是一组共享了某些资源的容器。具体的说：Pod里的所有容器，共享的是同一个Network Namespace，并且可以声明共享同一个Volume。这些需求似乎用`docker run --net --volumes-from`这样的命令也能实现，比如：

```bash
$ docker run --net=B --volumes-from=B --name=A image-A
```

但是，基于这种实现，A和B这两个容器就成了拓扑关系而不是对等关系了，因为B容器必须先于A容器启动。

所以，在Kubernetes项目里，Pod的实现需要使用一个中间容器，这个容器叫做Infra容器，它的名字和Pod里的其它容器差不多，基本格式就是**k8s_POD_\<Pod name\>**。在创建Pod的时候，Infra容器永远都是第一个被创建的容器，而其它用户容器，则通过Join Network Namespace的方式，与Infra容器关联在一起，这个关系可以如下表达：

![image-20211101164543262](https://hunk-pic-store.oss-cn-beijing.aliyuncs.com/img/20211101164550.png)

如上图，这个Pod里包含了容器A和B，以及Infra容器。在 Kubernetes 项目里，Infra 容器一定要占用极少的资源，所以它使用的是一个非常特殊的镜像，叫作：k8s.gcr.io/pause。这个镜像是一个用汇编语言编写的、永远处于“暂停”状态的容器，解压后的大小也只有 100~200 KB 左右。

对于Pod里的容器A和容器B来说：

1. 它们可以直接使用localhost进行通信；
2. 它们看到的网络设备和Infra容器看到的万全一样；
3. 一个Pod只有一个IP地址，也就是这个Pod的Network Namespace对应的IP地址；
4. 其它的所有网络资源，都是一个Pod一份，并且被该Pod中的所有容器共享；
5. Pod的生命周期只跟Infra容器一致，而与容器A和B无关。

###  sidecar容器设计模式

考虑一种应用场景：我们在Tomcat容器里运行WAR包里的应用程序，WAR包里的应用程序经常在更新，而Tomcat本身是比较稳定的。先看看，在Docker的世界里，该如何处理这种关系呢？

1. 直接将Tomcat容器和WAR包放在同一个镜像里，那么更新WAR包或者升级Tomcat环境的时候，就要重新制作发布新的镜像，非常麻烦。
2. 永远只发布一个Tomcat容器，为这个容器的webapps目录声明一个hostPath类型的Volume，从而把宿主机上的WAR包挂载进容器运行起来。这需要解决一个难题：如何维护每台宿主机上的这个存放WAR包的目录，这就不得不引入复杂的分布式存储系统了。在实际生产环境中，我们的很多容器都是有状态的，因此配置分布式存储系统是很常见的。但是，在储存用户数据的存储系统上发布软件包的做法并不可取。

实际上，有了Pod后，这样的问题就好办了：我们可以把Tomcat容器和WAR包分别做成镜像，然后放到同一个Pod里，并通过Volume来共享WAR包。yaml配置如下：

```yaml

apiVersion: v1
kind: Pod
metadata:
  name: javaweb-2
spec:
  initContainers:
  - image: geektime/sample:v2
    name: war
    command: ["cp", "/sample.war", "/app"]
    volumeMounts:
    - mountPath: /app
      name: app-volume
  containers:
  - image: geektime/tomcat:7.0
    name: tomcat
    command: ["sh","-c","/root/apache-tomcat-7.0.42-v2/bin/start.sh"]
    volumeMounts:
    - mountPath: /root/apache-tomcat-7.0.42-v2/webapps
      name: app-volume
    ports:
    - containerPort: 8080
      hostPort: 8001 
  volumes:
  - name: app-volume
    emptyDir: {}
```

WAR包是一个**initContainer**类型的容器。在Pod中，所有Init Container容器会按顺序逐一启动，而直到它们都启动并且退出了，spec.containers里的容器才会启动。这就保证了，Tomcat容器启动时，webapps目录下的WAR总是已经ready。

