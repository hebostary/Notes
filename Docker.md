<!-- TOC -->autoauto- [1. Docker存储驱动（Storage Driver）](#1-docker存储驱动storage-driver)auto- [2. Docker应用存储](#2-docker应用存储)auto- [3. Docker网络模型](#3-docker网络模型)auto    - [3.1. CNM（Container Networking Model）](#31-cnmcontainer-networking-model)auto    - [3.2. 网络驱动（Network Drivers）](#32-网络驱动network-drivers)auto        - [3.2.1. Host Driver](#321-host-driver)auto        - [3.2.2. Bridge Driver](#322-bridge-driver)auto        - [3.2.3. Macvlan Driver](#323-macvlan-driver)auto        - [3.2.4. Overlay Driver](#324-overlay-driver)autoauto<!-- /TOC -->

# 1. Docker存储驱动（Storage Driver）

# 2. Docker应用存储

# 3. Docker网络模型

## 3.1. CNM（Container Networking Model）

## 3.2. 网络驱动（Network Drivers）

### 3.2.1. Host Driver

和后面介绍的Bridge、Macvlan、Overlay Driver不同，用host driver创建的网络没有独立的网络命名空间（network namespace）。对于配置了host driver网络的所有容器，都在相同的主机网络命名空间（host network namespace）中，都直接使用主机的网络接口和IP栈。从网络的角度来看，这些容器里的应用相当于在没有容器的主机上运行多个进程，所以这些容器也不能绑定到相同的主机端口。
![Host Driver](https://success.docker.com/api/images/.%2Frefarch%2Fnetworking%2Fimages%2Fhost-driver.png)


Docker自动创建叫host的host driver网络，并且它是唯一的，我们无法手动创建更多的host driver网络。
```sh
[root@hunk ~]# docker network ls |grep host
62c10382a30e        host                host                local
[root@hunk ~]# docker network create -d host host1
Error response from daemon: only one instance of "host" network is allowed
```

测试

### 3.2.2. Bridge Driver

### 3.2.3. Macvlan Driver

### 3.2.4. Overlay Driver