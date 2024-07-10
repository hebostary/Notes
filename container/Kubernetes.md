# 部署Kubernetes集群

## 准备环境

### 部署基本安装环境

```bash
$ kubeadm version
kubeadm version: &version.Info{Major:"1", Minor:"22", GitVersion:"v1.22.2", GitCommit:"8b5a19147530eaac9476b0ab82980b4088bbc1b2", GitTreeState:"clean", BuildDate:"2021-09-15T21:37:34Z", GoVersion:"go1.16.8", Compiler:"gc", Platform:"linux/amd64"}

$ kubectl version
Client Version: version.Info{Major:"1", Minor:"22", GitVersion:"v1.22.2", GitCommit:"8b5a19147530eaac9476b0ab82980b4088bbc1b2", GitTreeState:"clean", BuildDate:"2021-09-15T21:38:50Z", GoVersion:"go1.16.8", Compiler:"gc", Platform:"linux/amd64"}
```

## kubeadm部署Kubernetes集群

### kubeadm init & join

```bash
$ kubeadm init --config kubeadm-config.yaml --dry-run #最好先跑一下preflight checks，没有任何错误时再初始化集群
$ kubeadm init --config kubeadm-config.yaml
```

```bash
$ cat kubeadm-config.yaml 
# kubeadm-config.yaml
kind: ClusterConfiguration
apiVersion: kubeadm.k8s.io/v1beta3
kubernetesVersion: v1.22.2
networking:
  podSubnet: "192.168.0.0/16"
---
kind: KubeletConfiguration
apiVersion: kubelet.config.k8s.io/v1beta1
cgroupDriver: systemd
```

> 注意，在kubeadm-config.yaml 里面，我们指定了pod的子网，这个实际上是部署Calico这款网络插件的需要。

`kubeadm init`执行成功后，会打印一些需要我们手动执行的操作：

```bash
# 1. 用于添加更多节点到集群中。
kubeadm join 100.1.146.150:6443 --token 69qzjw.yiflppti049t6tvj \
        --discovery-token-ca-cert-hash sha256:21a5124fdfab915f1d4d8522b8fb129012e386c171c71bb579910b5b9fb80d4d

# 2. 拷贝k8s配置到用户目录
mkdir -p $HOME/.kube
sudo cp -i /etc/kubernetes/admin.conf $HOME/.kube/config
sudo chown $(id -u):$(id -g) $HOME/.kube/config
```

### Cgroup Driver不匹配问题

**问题现象：**

在执行`kubeadm init`过程中发现初始化master节点失败了，主要错误信息：

```bash
[wait-control-plane] Waiting for the kubelet to boot up the control plane as static Pods from directory "/etc/kubernetes/manifests". This can take up to 4m0s
[kubelet-check] Initial timeout of 40s passed.
[kubelet-check] It seems like the kubelet isn't running or healthy.
[kubelet-check] The HTTP call equal to 'curl -sSL http://localhost:10248/healthz' failed with error: Get "http://localhost:10248/healthz": dial tcp [::1]:10248: connect: connection refused.
```

执行`docker ps`命令发现没有任何k8s的容器在运行，执行`systemctl status kubelet`发现kubelet也没有运行，通过`journalctl -xeu kubelet`查看kubelet退出的详细信息，在最后发现如下信息：

```bash
misconfiguration: kubelet cgroup driver: \"systemd\" is different from docker cgroup driver: \"cgroupfs\""
```

这是因为kubelet和docker使用的cgroup driver不匹配导致的，做如下检查：

```bash
$ docker info | grep -i cgroup
 Cgroup Driver: cgroupfs
 Cgroup Version: 1

$ cat /etc/systemd/system/kubelet.service.d/10-kubeadm.conf | grep KUBELET_CONFIG_ARGS
Environment="KUBELET_CONFIG_ARGS=--config=/var/lib/kubelet/config.yaml"
ExecStart=/usr/local/bin/kubelet $KUBELET_KUBECONFIG_ARGS $KUBELET_CONFIG_ARGS $KUBELET_KUBEADM_ARGS $KUBELET_EXTRA_ARGS

$ grep "cgroup" /var/lib/kubelet/config.yaml
cgroupDriver: systemd
```

**处理方法：**

`systemd`是kubernetes官方推荐的cgroup driver（参见[Configuring the kubelet cgroup driver](https://kubernetes.io/docs/tasks/administer-cluster/kubeadm/configure-cgroup-driver/)），所以我们这里将docker的cgroup driver改成systemd，但是/etc/docker/daemon.json文件里docker的cgroup driver已经配置成systemd了，所以我们只需要重启一下docker服务：

```bash
$ cat /etc/docker/daemon.json
{
  "exec-opts": ["native.cgroupdriver=systemd"],
  "log-driver": "json-file",
  "log-opts": {
    "max-size": "100m"
  },
  "storage-driver": "overlay2"
}

$ systemctl restart docker.service
$ docker info | grep -i cgroup
 Cgroup Driver: systemd
 Cgroup Version: 1
```

### NotReady问题 & 部署网络插件

**问题现象：**

```bash
$ kubectl get nodes
NAME             STATUS     ROLES                  AGE   VERSION
net146-host150   NotReady   control-plane,master   18m   v1.22.2

$ kubectl describe node net146-host150 | grep NotReady
  Ready            False   Wed, 20 Oct 2021 10:35:13 -0400   Wed, 20 Oct 2021 10:19:40 -0400   KubeletNotReady              container runtime network not ready: NetworkReady=false reason:NetworkPluginNotReady message:docker: network plugin is not ready: cni config uninitialized
```

> 在调试Kubernetes集群时，最重要的手段就是用`kubectl describe`来查看这个节点（Node）对象的详细信息、状态和事件。

原因在于我们还没有部署任何网络插件。此外，我们可以通过kubectl命令检查master节点上各个Pod的状态。其中，kube-system是Kubernetes项目预留的系统Pod的工作空间（Namespace，注意它并不是Linux Namespace，它只是Kubernetes划分不同工作空间的单位）。

```bash
$ kubectl get pods -n kube-system
NAME                                     READY   STATUS    RESTARTS   AGE
coredns-78fcd69978-9zlxf                 0/1     Pending   0          22m
coredns-78fcd69978-dj8xt                 0/1     Pending   0          22m
etcd-net146-host150                      1/1     Running   1          22m
kube-apiserver-net146-host150            1/1     Running   1          22m
kube-controller-manager-net146-host150   1/1     Running   1          22m
kube-proxy-mzfvt                         1/1     Running   0          22m
kube-scheduler-net146-host150            1/1     Running   1          22m
```

可以看到，coredns这些依赖于网络的Pod都处于Pending状态，即调度失败，这是因为整个master节点的网络还未就绪。

**处理方法：**

参考[Installing Addons](https://kubernetes.io/docs/concepts/cluster-administration/addons/)部署网络插件（cni），慎用[Weave Net](https://www.weave.works/docs/net/latest/kubernetes/kube-addon/)这款插件，我在部署过程中遇到了无法解决的问题，社区里没有找到解决方案，官方网站访问时也不是很好用。最后切换到[Calico](https://docs.projectcalico.org/getting-started/kubernetes/quickstart)后就没有那些奇怪的问题了，最好是在用`kubeadm init`执行之前就先阅读Calico的部署过程，因为官网推荐在`kubeadm init`时先指定pod的网络：

```bash
$ kubeadm init --pod-network-cidr=192.168.0.0/16
```

具体部署过程参考[Quickstart for Calico on Kubernetes](https://docs.projectcalico.org/getting-started/kubernetes/quickstart)。为了防止**pull rate limit**的问题，我们先在每个node上手动拉取一下Calico镜像：

```bash
$ docker pull docker.io/calico/node:v3.20.2
$ docker pull docker.io/calico/kube-controllers:v3.20.2
$ docker pull docker.io/calico/cni:v3.20.2
$ docker pull docker.io/calico/typha:v3.20.2
$ docker pull docker.io/calico/pod2daemon-flexvol:v3.20.2
```

部署好Calico插件后，在calico-system这个命名空间里查看相关的pod资源，因为我一共部署了3个节点，所以pod会比较多：

```bash
$ kubectl get pods -n calico-system
NAME                                       READY   STATUS    RESTARTS   AGE
calico-kube-controllers-767ddd5576-xpdc5   1/1     Running   0          28m
calico-node-7928k                          0/1     Running   0          28m
calico-node-rjl4c                          0/1     Running   0          28m
calico-node-s7wt9                          0/1     Running   0          28m
calico-typha-6bf4f8f495-4bzq9              1/1     Running   0          28m
calico-typha-6bf4f8f495-nxhlt              1/1     Running   0          28m
calico-typha-6bf4f8f495-zl2vt              1/1     Running   0          28m
```

### 部署Dashboard

参见https://github.com/kubernetes/dashboard，执行下面的`kubectl apply`命令来部署dashboard：

```bash
$ kubectl apply -f https://raw.githubusercontent.com/kubernetes/dashboard/v2.4.0/aio/deploy/recommended.yaml
namespace/kubernetes-dashboard created
serviceaccount/kubernetes-dashboard created
service/kubernetes-dashboard created
secret/kubernetes-dashboard-certs created
secret/kubernetes-dashboard-csrf created
secret/kubernetes-dashboard-key-holder created
configmap/kubernetes-dashboard-settings created
role.rbac.authorization.k8s.io/kubernetes-dashboard created
clusterrole.rbac.authorization.k8s.io/kubernetes-dashboard created
rolebinding.rbac.authorization.k8s.io/kubernetes-dashboard created
clusterrolebinding.rbac.authorization.k8s.io/kubernetes-dashboard created
deployment.apps/kubernetes-dashboard created
service/dashboard-metrics-scraper created
deployment.apps/dashboard-metrics-scraper created
```

在这个v2.4.0版本的pod文件里，使用了如下的seccomp参数格式，官方文档[Deploy and Access the Kubernetes Dashboard](https://kubernetes.io/docs/tasks/access-application-cluster/web-ui-dashboard/)使用的v2.3.1里的seccomp用法已经在Kubernetes v1.19被弃用了，所以在部署时会产生warning。

```yaml
    spec:
      securityContext:
        seccompProfile:
          type: RuntimeDefault
```

此外，这个版本已经为master节点的Taint自动添加了tolerations，否则无法成功部署其他的pods，包括这些dashboard pods。

```yaml
      tolerations:
        - key: node-role.kubernetes.io/master
          operator: "Exists" 
          effect: NoSchedule
```

> 如果recommended.yaml中没有tolerations.operator这个参数的话，就先将yaml下载到本地，然后手动加上，再重新部署。因为tolerations.operator的默认值是'Equal'，但是master节点上的node-role.kubernetes.io/master这个Taint只有key，没有value，所以要用'Exists'。否则，kubernetes-dashboard这个pods里的两个容器一直处于ContainerCreating状态。

部署完后，可以通过`kubectl describe`去查看dashboard pods的状态：

```bash
$ kubectl get pods -n kubernetes-dashboard
NAME                                         READY   STATUS    RESTARTS   AGE
dashboard-metrics-scraper-6444d4b9c4-w6x4h   1/1     Running   0          21s
kubernetes-dashboard-576cb95f94-frtps        1/1     Running   0          21s
$ kubectl describe pods kubernetes-dashboard-576cb95f94-frtps -n kubernetes-dashboard
```

如果在部署dashboard后发现拉取dashboard镜像时遇到rate limit问题，可以用`docker pull`先手动拉取dashboard的两个镜像再重新部署：

```bash
$ kubectl describe pods kubernetes-dashboard-576cb95f94-frtps -n kubernetes-dashboard
...
Warning  Failed     15s (x2 over 36s)  kubelet            Failed to pull image "kubernetesui/dashboard:v2.4.0": rpc error: code = Unknown desc = Error response from daemon: toomanyrequests: You have reached your pull rate limit. You may increase the limit by authenticating and upgrading: https://www.docker.com/increase-rate-limit

$ docker pull kubernetesui/dashboard:v2.4.0
$ docker pull kubernetesui/metrics-scraper:v1.0.7
```

刚部署好的dashboard是无法从外网直接访问的，在启用proxy后可以在master节点上访问：

```bash
$ kubectl get services --namespace=kubernetes-dashboard
NAME                        TYPE        CLUSTER-IP       EXTERNAL-IP   PORT(S)    AGE
dashboard-metrics-scraper   ClusterIP   10.111.119.232   <none>        8000/TCP   13m
kubernetes-dashboard        ClusterIP   10.103.139.126   <none>        443/TCP    13m

$ kubectl proxy
Starting to serve on 127.0.0.1:8001

$ curl http://localhost:8001/api/v1/namespaces/kubernetes-dashboard/services/https:kubernetes-dashboard:/proxy/
<!--
Copyright 2017 The Kubernetes Authors.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at
...
```

我们一般都是在没有图形界面的服务器上部署Kebernetetes集群，所以这种对于dashboard的使用方式并不方便。



关于如何配置访问dashboard的不同方式，参见[Accessing Dashboard](https://github.com/kubernetes/dashboard/blob/master/docs/user/accessing-dashboard/README.md)。

### 访问Kubernetes集群



## 运行无状态应用 - Nginx Deployment

参见[使用 Deployment 运行一个无状态应用](https://kubernetes.io/zh/docs/tasks/run-application/run-stateless-application-deployment/)

### 运行Nginx Deployment

为了防止**pull rate limit**的问题，我们先在每个node上手动拉取一下NGINX镜像：

```bash
$ docker pull nginx:1.14.2
$ docker pull nginx:1.16.1
```

先部署一个1.14.2版本的NGINX应用，包含了两个副本，因此创建了两个pod，分别调度到了两个不同的Kubernetes节点：

```bash
$ kubectl apply -f apps/nginx/nginx-deployment.yaml
deployment.apps/nginx-deployment created
$ kubectl get pods
NAME                                READY   STATUS    RESTARTS   AGE
nginx-deployment-66b6c48dd5-8hwtn   1/1     Running   0          19s
nginx-deployment-66b6c48dd5-sl75z   1/1     Running   0          19s
$ kubectl describe pod nginx-deployment-66b6c48dd5-8hwtn
Name:         nginx-deployment-66b6c48dd5-8hwtn
Namespace:    default
Priority:     0
Node:         net146-host152/100.1.146.152
...
$ kubectl describe pod nginx-deployment-66b6c48dd5-sl75z
Name:         nginx-deployment-66b6c48dd5-sl75z
Namespace:    default
Priority:     0
Node:         net146-host151/100.1.146.151
...
Events:
  Type    Reason     Age        From               Message
  ----    ------     ----       ----               -------
  Normal  Scheduled  2m3s       default-scheduler  Successfully assigned default/nginx-deployment-66b6c48dd5-sl75z to net146-host151
  Normal  Pulled     <invalid>  kubelet            Container image "nginx:1.14.2" already present on machine
  Normal  Created    <invalid>  kubelet            Created container nginx
  Normal  Started    <invalid>  kubelet            Started container nginx
```

```yaml
# nginx-deployment.yaml 
apiVersion: apps/v1
kind: Deployment
metadata:
  name: nginx-deployment
spec:
  selector:
    matchLabels:
      app: nginx
  replicas: 2 # tells deployment to run 2 pods matching the template
  template:
    metadata:
      labels:
        app: nginx
    spec:
      containers:
      - name: nginx
        image: nginx:1.14.2
        ports:
        - containerPort: 80
```

### 升级Nginx Deployment

现在来将NGINX的版本升级到到1.16.1，K8s会采用**灰度发布**的形式升级这些pod：先停掉一个低版本pod，然后创建一个高版本pod，再停掉另一个低版本pod，最后再创建一个高版本pod。这样可以在升级过程中保证应用不中断。

```bash
$ kubectl apply -f apps/nginx/nginx-deployment-update.yaml 
deployment.apps/nginx-deployment configured
$ kubectl get pods
NAME                                READY   STATUS    RESTARTS   AGE
nginx-deployment-559d658b74-d4xcl   1/1     Running   0          105s
nginx-deployment-559d658b74-qz8mb   1/1     Running   0          102s
```

```yaml
# nginx-deployment-update.yaml
apiVersion: apps/v1
kind: Deployment
metadata:
  name: nginx-deployment
spec:
  selector:
    matchLabels:
      app: nginx
  replicas: 2
  template:
    metadata:
      labels:
        app: nginx
    spec:
      containers:
      - name: nginx
        image: nginx:1.16.1 # Update the version of nginx from 1.14.2 to 1.16.1
        ports:
        - containerPort: 80
```

我们也可以通过标签来过滤Pod：

```bash
$ kubectl get pods -l app=nginx
NAME                                READY   STATUS    RESTARTS   AGE
nginx-deployment-559d658b74-d4xcl   1/1     Running   0          5d10h
nginx-deployment-559d658b74-qz8mb   1/1     Running   0          5d10h
```

### 扩展Nginx Deployment

我们可以通过增加副本来扩展应用：

```yaml
# apps/nginx/nginx-deployment-scale.yaml
apiVersion: apps/v1
kind: Deployment
metadata:
  name: nginx-deployment
spec:
  selector:
    matchLabels:
      app: nginx
  replicas: 4 # Update the replicas from 2 to 4
  template:
    metadata:
      labels:
        app: nginx
    spec:
      containers:
      - name: nginx
        image: nginx:1.16.1
        ports:
        - containerPort: 80
```

将更新配置应用到集群后，Kubernetes会创再建两个新的Pod，最终nginx-deployment由四个Pod组成：

```bash
$ kubectl apply -f apps/nginx/nginx-deployment-scale.yaml
```

```bash
$ kubectl get pods -l app=nginx
NAME                                READY   STATUS    RESTARTS   AGE
nginx-deployment-559d658b74-d4xcl   1/1     Running   0          5d11h
nginx-deployment-559d658b74-mt4cz   1/1     Running   0          3m36s
nginx-deployment-559d658b74-qz8mb   1/1     Running   0          5d11h
nginx-deployment-559d658b74-r4dmn   1/1     Running   0          3m36s
```

### 删除Nginx Deployment

我们可以通过名称来删除整个Deployment，其中的Pod将会被自动删除：

```bash
$ kubectl delete deployment nginx-deployment
```



## 部署插件



## 常用工具手册

[kubectl-commands GETTING STARTED](https://kubernetes.io/docs/reference/generated/kubectl/kubectl-commands#-strong-getting-started-strong-)



# Kubernetes核心概念

## 容器

### 管理镜像

关于Kubernetes管理镜像的更多细节，参见[Images](https://kubernetes.io/zh/docs/concepts/containers/images/)。

**镜像拉取策略**

当你最初创建一个 [Deployment](https://kubernetes.io/zh/docs/concepts/workloads/controllers/deployment/)、 [StatefulSet](https://kubernetes.io/zh/docs/concepts/workloads/controllers/statefulset/)、Pod 或者其他包含 Pod 模板的对象时，如果没有显式设定的话，Pod 中所有容器的默认镜像 拉取策略是 `IfNotPresent`。这一策略会使得 [kubelet](https://kubernetes.io/docs/reference/generated/kubelet) 在镜像已经存在的情况下直接略过拉取镜像的操作。

容器的 `imagePullPolicy` 和镜像的标签会影响 [kubelet](https://kubernetes.io/zh/docs/reference/command-line-tools-reference/kubelet/) 尝试拉取（下载）指定的镜像。

当你（或控制器）向 API 服务器提交一个新的 Pod 时，你的集群会在满足特定条件时设置 `imagePullPolicy `字段：

- 如果你省略了 `imagePullPolicy` 字段，并且容器镜像的标签是 `:latest`， `imagePullPolicy` 会自动设置为 `Always`。
- 如果你省略了 `imagePullPolicy` 字段，并且没有指定容器镜像的标签， `imagePullPolicy` 会自动设置为 `Always`。
- 如果你省略了 `imagePullPolicy` 字段，并且为容器镜像指定了非 `:latest` 的标签， `imagePullPolicy`就会自动设置为 `IfNotPresent`。

> 在生产环境中部署容器时，你应该避免使用 `:latest` 标签，因为这使得正在运行的镜像的版本难以追踪，并且难以正确地回滚。因此，应指定一个有意义的标签，如 `v1.42.0`。

**ImagePullBackOff**

当 kubelet 使用容器运行时创建 Pod 时，容器可能因为 `ImagePullBackOff` 导致状态为 [Waiting](https://kubernetes.io/zh/docs/concepts/workloads/pods/pod-lifecycle/#container-state-waiting)。`ImagePullBackOff` 状态意味着容器无法启动， 因为 Kubernetes 无法拉取容器镜像（原因包括无效的镜像名称，或从私有仓库拉取而没有 `imagePullSecret`）。 `BackOff` 部分表示 Kubernetes 将继续尝试拉取镜像，并增加回退延迟。Kubernetes 会增加每次尝试之间的延迟，直到达到编译限制，即 300 秒（5 分钟）。

### 容器运行时接口(CRI)

参见[CRI: the Container Runtime Interface](https://github.com/kubernetes/community/blob/master/contributors/devel/sig-node/container-runtime-interface.md)

### 容器运行时类(RuntimeClass)

参见[容器运行时类（Runtime Class）](https://kubernetes.io/zh/docs/concepts/containers/runtime-class/)

## 工作负载(Workloads)

工作负载是在 Kubernetes 上运行的应用程序。取决于应用程序的复杂度，我们可以将其运行在一个或者多个Pod中。在 Kubernetes 中，Pod 代表的是集群上处于运行状态的一组 [容器](https://kubernetes.io/zh/docs/concepts/overview/what-is-kubernetes/#why-containers)。Kubernetes确保确保合适类型的、处于运行状态的 Pod 个数是正确的，与你所指定的状态相一致。

Kubernetes 提供若干种内置的工作负载资源：

- [Deployment](https://kubernetes.io/zh/docs/concepts/workloads/controllers/deployment/) 和 [ReplicaSet](https://kubernetes.io/zh/docs/concepts/workloads/controllers/replicaset/) （替换原来的资源 [ReplicationController](https://kubernetes.io/zh/docs/reference/glossary/?all=true#term-replication-controller)）。 `Deployment` 很适合用来管理你的集群上的无状态应用，`Deployment` 中的所有 `Pod` 都是相互等价的，并且在需要的时候被换掉。
- [StatefulSet](https://kubernetes.io/zh/docs/concepts/workloads/controllers/statefulset/) 让你能够运行一个或者多个以某种方式跟踪应用状态的 Pods。 例如，如果你的负载会将数据作持久存储，你可以运行一个 `StatefulSet`，将每个 `Pod` 与某个 [`PersistentVolume`](https://kubernetes.io/zh/docs/concepts/storage/persistent-volumes/) 对应起来。你在 `StatefulSet` 中各个 `Pod` 内运行的代码可以将数据复制到同一 `StatefulSet` 中的其它 `Pod` 中以提高整体的服务可靠性。

- [DaemonSet](https://kubernetes.io/zh/docs/concepts/workloads/controllers/daemonset/) 定义提供节点本地支撑设施的 `Pods`。这些 Pods 可能对于你的集群的运维是 非常重要的，例如作为网络链接的辅助工具或者作为网络 [插件](https://kubernetes.io/zh/docs/concepts/cluster-administration/addons/) 的一部分等等。每次你向集群中添加一个新节点时，如果该节点与某 `DaemonSet` 的规约匹配，则控制面会为该 `DaemonSet` 调度一个 `Pod` 到该新节点上运行。
- [Job](https://kubernetes.io/zh/docs/concepts/workloads/controllers/job/) 和 [CronJob](https://kubernetes.io/zh/docs/concepts/workloads/controllers/cron-jobs/)。 定义一些一直运行到结束并停止的任务。`Job` 用来表达的是一次性的任务，而 `CronJob`会根据其时间规划反复运行。

### Pods

*Pod* 是可以在 Kubernetes 中创建和管理的、最小的可部署的计算单元。它是一组容器，这些容器是相对紧密的耦合在一起的，并且共享存储、网络、以及怎样运行这些容器的声明。Pod 天生地为其成员容器提供了两种共享资源：[网络](https://kubernetes.io/zh/docs/concepts/workloads/pods/#pod-networking)和 [存储](https://kubernetes.io/zh/docs/concepts/workloads/pods/#pod-storage)。

通常我们不需要直接创建 Pod，甚至单实例 Pod。 而是使用诸如 [Deployment](https://kubernetes.io/zh/docs/concepts/workloads/controllers/deployment/) 或 [Job](https://kubernetes.io/zh/docs/concepts/workloads/controllers/job/) 这类工作负载资源 来创建 Pod。如果 Pod 需要跟踪状态， 可以考虑 [StatefulSet](https://kubernetes.io/zh/docs/concepts/workloads/controllers/statefulset/) 资源。

工作负载资源的控制器能够处理Pod副本的管理、上线，并在 Pod 失效时提供自愈能力。 例如，如果一个节点失败，控制器注意到该节点上的 Pod 已经停止工作， 就可以创建替换性的 Pod。调度器会将替身 Pod 调度到一个健康的节点执行。

下面是一些可以管理一个或者多个 Pod 的工作负载资源：

- [Deployment](https://kubernetes.io/zh/docs/concepts/workloads/controllers/deployment/)
- [StatefulSet](https://kubernetes.io/zh/docs/concepts/workloads/controllers/statefulset/)
- [DaemonSet](https://kubernetes.io/zh/docs/concepts/workloads/controllers/daemonset/)

#### Pod模板

工作负载的控制器会使用负载对象中的 `PodTemplate` 来生成实际的 Pod。 `PodTemplate` 是你用来运行应用时指定的负载资源的目标状态的一部分。比如，下面是一个描述一个Job的yaml，其中的 `template` 指示启动一个容器。

```yaml
apiVersion: batch/v1
kind: Job
metadata:
  name: hello
spec:
  template:
    # 这里是 Pod 模版
    spec:
      containers:
      - name: hello
        image: busybox
        command: ['sh', '-c', 'echo "Hello, Kubernetes!" && sleep 3600']
      restartPolicy: OnFailure
    # 以上为 Pod 模版
```

修改工作负载资源的 Pod 模版或者切换到新的 Pod 模版都不会对已经存在的 Pod 起作用。 Pod 不会直接收到模版的更新。相反， 新的 Pod 会被创建出来，与更改后的 Pod 模版匹配。

#### Pod内容器通信

每个 Pod 都在每个地址族中获得一个唯一的 IP 地址。 Pod 中的每个容器：

1. 共享网络名字空间，包括 IP 地址和网络端口。 *Pod 内* 的容器可以使用 `localhost` 互相通信。 当 Pod 中的容器与 *Pod 之外* 的实体通信时，它们必须协调如何使用共享的网络资源 （例如端口）。
2. 可以通过如 SystemV 信号量或 POSIX 共享内存这类标准的进程间通信方式互相通信。 
3. Pod 中的容器所看到的系统主机名与为 Pod 配置的 `name` 属性值相同。

#### 静态Pod

**静态 Pod（Static Pod）* 直接由特定节点上的 `kubelet` 守护进程管理， 不需要[API 服务器](https://kubernetes.io/zh/docs/reference/command-line-tools-reference/kube-apiserver/)看到它们。 尽管大多数 Pod 都是通过控制面（例如，[Deployment](https://kubernetes.io/zh/docs/concepts/workloads/controllers/deployment/)） 来管理的，对于静态 Pod 而言，`kubelet` 直接监控每个 Pod，并在其失效时重启之。

#### Pod生命周期

更多细节，参见[Pod 的生命周期](https://kubernetes.io/zh/docs/concepts/workloads/pods/pod-lifecycle/)。

Pod 遵循一个预定义的生命周期，起始于 `Pending` [阶段](https://kubernetes.io/zh/docs/concepts/workloads/pods/pod-lifecycle/#pod-phase)，如果至少 其中有一个主要容器正常启动，则进入 `Running`，之后取决于 Pod 中是否有容器以 失败状态结束而进入 `Succeeded` 或者 `Failed` 阶段。

Pod 在其生命周期中只会被[调度](https://kubernetes.io/zh/docs/concepts/scheduling-eviction/)一次。 一旦 Pod 被调度（分派）到某个节点，Pod 会一直在该节点运行，直到 Pod 停止或者 被[终止](https://kubernetes.io/zh/docs/concepts/workloads/pods/pod-lifecycle/#pod-termination)。在 Pod 运行期间，`kubelet` 能够重启容器以处理一些失效场景。 在 Pod 内部，Kubernetes 跟踪不同容器的[状态](https://kubernetes.io/zh/docs/concepts/workloads/pods/pod-lifecycle/#container-states) 并确定使 Pod 重新变得健康所需要采取的动作。

Pod 自身不具有自愈能力。如果一个[节点](https://kubernetes.io/zh/docs/concepts/architecture/nodes/)死掉了，调度到该节点 的 Pod 也被计划在给定超时期限结束后[删除](https://kubernetes.io/zh/docs/concepts/workloads/pods/pod-lifecycle/#pod-garbage-collection)。

任何给定的 Pod （由 UID 定义）从不会被“重新调度（rescheduled）”到不同的节点； 相反，这一 Pod 可以被一个新的、几乎完全相同的 Pod 替换掉。 如果需要，新 Pod 的名字可以不变，但是其 UID 会不同。

##### Pod的阶段

Pod 的 `status` 字段是一个 [PodStatus](https://kubernetes.io/docs/reference/generated/kubernetes-api/v1.22/#podstatus-v1-core) 对象，其中包含一个 `phase` 字段。Pod 的阶段（Phase）是 Pod 在其生命周期中所处位置的简单宏观概述。下面是 `phase` 可能的值：

1. `Pending`（悬决）：Pod 已被 Kubernetes 系统接受，但有一个或者多个容器尚未创建亦未运行。此阶段包括等待 Pod 被调度的时间和通过网络下载镜像的时间。
2. `Running`（运行中）：Pod 已经绑定到了某个节点，Pod 中所有的容器都已被创建。至少有一个容器仍在运行，或者正处于启动或重启状态。
3. `Succeeded`（成功）：Pod 中的所有容器都已成功终止，并且不会再重启。
4. `Failed`（失败）：Pod 中的所有容器都已终止，并且至少有一个容器是因为失败终止。也就是说，容器以非 0 状态退出或者被系统终止。
5. `Unknown`（未知）：因为某些原因无法取得 Pod 的状态。这种情况通常是因为与 Pod 所在主机通信失败。

一旦[调度器](https://kubernetes.io/docs/reference/generated/kube-scheduler/)将 Pod 分派给某个节点，`kubelet` 就通过 [容器运行时](https://kubernetes.io/zh/docs/setup/production-environment/container-runtimes) 开始为 Pod 创建容器。 容器的状态有三种：`Waiting`（等待）、`Running`（运行中）和 `Terminated`（已终止）。

##### Pod终止流程

使用`kubectl delete`手动删除Pod时的流程如下（该 Pod 的体面终止限期是默认值（30 秒））：

1. API 服务器中的 Pod 对象被更新（状态为Terminating），记录涵盖体面终止限期在内 Pod 的最终死期，超出所计算时间点则认为 Pod 已死（dead）。
2. 在 Pod 运行所在的节点上：`kubelet` 一旦看到 Pod 被标记为Terminating（已经设置了体面终止限期），`kubelet` 即开始本地的 Pod 关闭过程。
   1. 如果 Pod 中的容器之一定义了 `preStop` [回调](https://kubernetes.io/zh/docs/concepts/containers/container-lifecycle-hooks)， `kubelet` 开始在容器内运行该回调逻辑。如果超出体面终止限期时，`preStop` 回调逻辑 仍在运行，`kubelet` 会请求给予该 Pod 的宽限期一次性增加 2 秒钟。如果 `preStop` 回调所需要的时间长于默认的体面终止限期，你必须修改`terminationGracePeriodSeconds` 属性值来使其正常工作。
   2. `kubelet` 接下来触发容器运行时发送 TERM 信号给每个容器中的进程 1。Pod 中的容器会在不同时刻收到 TERM 信号，接收顺序也是不确定的。 如果关闭的顺序很重要，可以考虑使用 `preStop` 回调逻辑来协调。
3. 同时，`kubelet` 启动体面关闭逻辑，控制面会将 Pod 从对应的端点列表（以及端点切片列表， 如果启用了的话）中移除，过滤条件是 Pod 被对应的 [服务](https://kubernetes.io/zh/docs/concepts/services-networking/service/)以某 [选择算符](https://kubernetes.io/zh/docs/concepts/overview/working-with-objects/labels/)选定。 [ReplicaSets](https://kubernetes.io/zh/docs/concepts/workloads/controllers/replicaset/)和其他工作负载资源 不再将关闭进程中的 Pod 视为合法的、能够提供服务的副本。关闭动作很慢的 Pod 也无法继续处理请求数据，因为负载均衡器（例如服务代理）已经在终止宽限期开始的时候 将其从端点列表中移除。
4. 超出终止宽限期限时，`kubelet` 会触发强制关闭过程。容器运行时会向 Pod 中所有容器内 仍在运行的进程发送 `SIGKILL` 信号。 `kubelet` 也会清理隐藏的 `pause` 容器，如果容器运行时使用了这种容器的话。
5. `kubelet` 触发强制从 API 服务器上删除 Pod 对象的逻辑，并将体面终止限期设置为 0 （这意味着马上删除）。
6. API 服务器删除 Pod 的 API 对象，从任何客户端都无法再看到该对象。

如果，某个Worker节点上的kubelet服务停止，节点状态变成NotReady时，某个Deployment一共包含4个Pod副本，其中有两个运行在该Worker节点。那么Kubernetes会终止失效节点上的两个Pod，并重新调度一个Worker节点启动两个新的Pod，从而保证Pod的副本数量始终是4个。对于失效节点上的Pod，会一直处于Terminating状态，当失效节点恢复正常后，这些Pod会被立即彻底删除。

##### 强制终止Pod

默认情况下，所有的删除操作都会附有 30 秒钟的宽限期限。 `kubectl delete` 命令支持 `--grace-period=<seconds>` 选项，允许你重载默认值， 设定自己希望的期限值。将宽限期限强制设置为 `0` 意味着立即从 API 服务器删除 Pod，同时额外设置 `--force` 参数才能发起强制删除请求。

```bash
$ kubectl delete pod <pod name> --grace-period=0 --force
```

执行强制删除操作时，API 服务器不再等待来自 `kubelet` 的、关于 Pod 已经在原来运行的节点上终止执行的确认消息。 API 服务器直接删除 Pod 对象，这样新的与之同名的 Pod 即可以被创建。 在节点侧，被设置为立即终止的 Pod 仍然会在被强行杀死之前获得一点点的宽限时间。

> 对于某些工作负载及其 Pod 而言，尤其是有状态的工作负载，强制删除很可能会带来某种破坏。

#### 容器探针(Probe)

[Probe](https://kubernetes.io/docs/reference/generated/kubernetes-api/v1.22/#probe-v1-core) 是由 [kubelet](https://kubernetes.io/zh/docs/reference/command-line-tools-reference/kubelet/) 对容器执行的定期诊断。 要执行诊断，kubelet 调用由容器实现的 [Handler](https://kubernetes.io/docs/reference/generated/kubernetes-api/v1.22/#handler-v1-core) （处理程序）。有三种类型的处理程序：

- [ExecAction](https://kubernetes.io/docs/reference/generated/kubernetes-api/v1.22/#execaction-v1-core)： 在容器内执行指定命令。如果命令退出时返回码为 0 则认为诊断成功。
- [TCPSocketAction](https://kubernetes.io/docs/reference/generated/kubernetes-api/v1.22/#tcpsocketaction-v1-core)： 对容器的 IP 地址上的指定端口执行 TCP 检查。如果端口打开，则诊断被认为是成功的。
- [HTTPGetAction](https://kubernetes.io/docs/reference/generated/kubernetes-api/v1.22/#httpgetaction-v1-core)： 对容器的 IP 地址上指定端口和路径执行 HTTP Get 请求。如果响应的状态码大于等于 200 且小于 400，则诊断被认为是成功的。

每次探测都将获得以下三种结果之一：

- `Success`（成功）：容器通过了诊断。
- `Failure`（失败）：容器未通过诊断。
- `Unknown`（未知）：诊断失败，因此不会采取任何行动。

针对运行中的容器，`kubelet` 可以选择是否执行以下三种探针，以及如何针对探测结果作出反应：

- `livenessProbe`：存活态探针，指示容器是否正在运行。如果存活态探测失败，则 kubelet 会杀死容器， 并且容器将根据其[重启策略](https://kubernetes.io/zh/docs/concepts/workloads/pods/pod-lifecycle/#restart-policy)决定未来。如果容器不提供存活探针， 则默认状态为 `Success`。如果容器中的进程能够在遇到问题或不健康的情况下自行崩溃，则不一定需要存活态探针; `kubelet` 将根据 Pod 的`restartPolicy` 自动执行修复操作。
- `readinessProbe`：就绪态探针，指示容器是否准备好为请求提供服务。如果就绪态探测失败， 端点控制器将从与 Pod 匹配的所有服务的端点列表中删除该 Pod 的 IP 地址。 初始延迟之前的就绪态的状态值默认为 `Failure`。 如果容器不提供就绪态探针，则默认状态为 `Success`。如果要仅在探测成功时才开始向 Pod 发送请求流量，请指定就绪态探针。如果你的应用程序对后端服务有严格的依赖性，你可以同时实现存活态和就绪态探针。
- `startupProbe`: 启动探针，指示容器中的应用是否已经启动。如果提供了启动探针，则所有其他探针都会被 禁用，直到此探针成功为止。如果启动探测失败，`kubelet` 将杀死容器，而容器依其 [重启策略](https://kubernetes.io/zh/docs/concepts/workloads/pods/pod-lifecycle/#restart-policy)进行重启。 如果容器没有提供启动探测，则默认状态为 `Success`。如果你的容器需要在启动期间加载大型数据、配置文件或执行迁移，即容器启动需要较长时间，你可以使用 [启动探针](https://kubernetes.io/zh/docs/concepts/workloads/pods/pod-lifecycle/#when-should-you-use-a-startup-probe)。你不再需要配置一个较长的存活态探测时间间隔，只需要设置另一个独立的配置选定， 对启动期间的容器执行探测，从而允许使用远远超出存活态时间间隔所允许的时长。

#### Init容器

Init 容器是一种特殊容器，在 [Pod](https://kubernetes.io/docs/concepts/workloads/pods/pod-overview/) 内的应用容器启动之前运行。Init 容器可以包括一些应用镜像中不存在的实用工具和安装脚本。

如果为一个 Pod 指定了多个 Init 容器，这些容器会按顺序逐个运行。 每个 Init 容器必须运行成功，下一个才能够运行。当所有的 Init 容器运行完成时， Kubernetes 才会为 Pod 初始化应用容器并像平常一样运行。

如果 Pod 的 Init 容器失败，kubelet 会不断地重启该 Init 容器直到该容器成功为止。 然而，如果 Pod 对应的 `restartPolicy` 值为 "Never"，并且 Pod 的 Init 容器失败， 则 Kubernetes 会将整个 Pod 状态设置为失败。

如果 Pod [重启](https://kubernetes.io/zh/docs/concepts/workloads/pods/init-containers/#pod-restart-reasons)，所有 Init 容器必须重新执行。对 Init 容器规约的修改仅限于容器的 `image` 字段。 更改 Init 容器的 `image` 字段，等同于重启该 Pod。

因为 Init 容器可能会被重启、重试或者重新执行，所以 Init 容器的代码应该是幂等的。 特别地，基于 `emptyDirs`写文件的代码，应该对输出文件可能已经存在做好准备。

在查看Pod状态的时候，以 `Init:` 开头的 Pod 状态汇总了 Init 容器执行的状态：

| `Init:N/M`                     | Pod 包含 `M` 个 Init 容器，其中 `N` 个已经运行完成。 |
| ------------------------------ | ---------------------------------------------------- |
| `Init:Error`                   | Init 容器已执行失败。                                |
| `Init:CrashLoopBackOff`        | Init 容器执行总是失败。                              |
| `Pending`                      | Pod 还没有开始执行 Init 容器。                       |
| `PodInitializing` or `Running` | Pod 已经完成执行 Init 容器。                         |

#### Pod拓扑分布约束

可以使用 *拓扑分布约束（Topology Spread Constraints）* 来控制 [Pods](https://kubernetes.io/docs/concepts/workloads/pods/pod-overview/) 在集群内故障域 之间的分布，例如区域（Region）、可用区（Zone）、节点和其他用户自定义拓扑域。 这样做有助于实现高可用并提升资源利用率。

更多细节参见[Pod 拓扑分布约束](https://kubernetes.io/zh/docs/concepts/workloads/pods/pod-topology-spread-constraints/)。

### 工作负载资源



## 垃圾回收(Garbage Collection)

关于Kubernetes垃圾回收的更多细节，参见[Garbage Collection](https://kubernetes.io/docs/concepts/architecture/garbage-collection/)。

### 回收无用的容器和镜像

kubelet会周期性的回收无用的容器和镜像，默认地：

1. 回收无用镜像：每5分钟一次；
2. 回收无用容器：每1分钟一次。

当节点的磁盘使用率达到阈值时，为了节省磁盘空间，kubelet也会根据最近一次使用时间间隔来删除一些镜像。

### 失效 Pod 的垃圾收集

对于已失败的 Pod 而言，对应的 API 对象仍然会保留在集群的 API 服务器上，直到 用户或者[控制器](https://kubernetes.io/zh/docs/concepts/architecture/controller/)进程显式地 将其删除。

控制面组件会在 Pod 个数超出所配置的阈值 （根据 `kube-controller-manager` 的 `terminated-pod-gc-threshold` 设置）时 删除已终止的 Pod（阶段值为 `Succeeded` 或 `Failed`）。 这一行为会避免随着时间演进不断创建和终止 Pod 而引起的资源泄露问题。



## 存储

### 持久化存储（PV和PVC）

持久卷（PersistentVolume，PV）是集群中的一块存储，可以由管理员事先供应，或者 使用[存储类（Storage Class）](https://kubernetes.io/zh/docs/concepts/storage/storage-classes/)来动态供应。 持久卷是集群资源，就像节点也是集群资源一样。PV 持久卷和普通的 Volume 一样，也是使用 卷插件来实现的，只是它们拥有独立于任何使用 PV 的 Pod 的生命周期。

持久卷申领（PersistentVolumeClaim，PVC）表达的是用户对存储的请求。概念上与 Pod 类似。 Pod 会耗用节点资源，而 PVC 申领会耗用 PV 资源。

 PVC 申领与 PV 卷之间的绑定是一种一对一的映射，实现上使用 ClaimRef 来记述 PV 卷 与 PVC 申领间的双向绑定关系。Pod 将 PVC 申领当做存储卷来使用。集群会检视 PVC 申领，找到所绑定的卷，并 为 Pod 挂载该卷。对于支持多种访问模式的卷，用户要在 Pod 中以卷的形式使用申领 时指定期望的访问模式。

#### 保护使用中的存储对象

如果用户删除被某 Pod 使用的 PVC 对象，该 PVC 申领不会被立即移除。 PVC 对象的移除会被推迟，直至其不再被任何 Pod 使用。 此外，如果管理员删除已绑定到某 PVC 申领的 PV 卷，该 PV 卷也不会被立即移除。 PV 对象的移除也要推迟到该 PV 不再绑定到 PVC。

> 当使用某 PVC 的 Pod 对象仍然存在时，认为该 PVC 仍被此 Pod 使用。

### Projected Volumes



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



# Kubernetes 开发

##  存储插件 CSI

[技术分享 | 基于CSI Kubernetes 存储插件的开发实践](https://zhuanlan.zhihu.com/p/51757577)
