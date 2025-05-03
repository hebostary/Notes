# Linux性能分析工具

## tracepoint



## ftrace



# CPU性能分析及优化



# I/O性能分析及优化

## Page Cache相关











# 网络性能分析及优化

## TCP相关

###  影响TCP连接建立的系统配置

如下是客户端和服务端建立TCP连接的3次握手过程，红色标注是影响相关步骤的Linux系统配置：

![image-20250315214151797](https://hunk-pic-store.oss-cn-beijing.aliyuncs.com/img/tcp_conn1.png)

`net.ipv4.tcp_syn_retries`：客户端发送第一个 SYN 报文后，如果i没有收到服务端的 SYNACK 报文，则会触发超时重试机制，并且重试次数受到这个配置的限制，默认为6。SYN 重试策略是第 n 次重试等待时间为 2 ^ (n-1)秒，所以默认的6次重试总计会等待 1 + 2 + 4 + 8 + 16 + 32 + 64 = 127（秒），即127秒后产生才 ETIMEOUT 的错误。在网络质量很好的数据中心内部，这种情况一般是服务器本身出了问题，比如服务器下线但是没有通知到客户端，可以考虑将客户端 SYN 重试次数降低，让客户端尽快去尝试连接其它的服务器，减少阻塞时间。

`net.ipv4.tcp_max_syn_backlog`：客户端没有收到 SYNACK 报文，除了上面提到的服务器下线的原因，也有可能是服务端积压了太多的半连接（incomplete）无法及时处理导致的。TCP服务端每收到一个 SYN 报文，就会创建一个半连接，然后把该半连接加入到半连接队列（syn queue）里，syn queue 的长度就是 `tcp_max_syn_backlog` 这个配置项来决定的，当系统中积压的半连接个数超过了该值后，新的 SYN 包就会被丢弃。对于服务器而言，可能瞬间收到非常多的 SYN 报文，所以可以适当地调大该值，以免 SYN 包被丢弃而导致客户端收不到 SYNACK。

`net.ipv4.tcp_syncookies`：服务端积压的半连接较多，也有可能是因为有些恶意的客户端在进行 SYN Flood 攻击，比如客户端高频地向服务端发 SYN 包，并且这个 SYN 包的源 IP 地址不停地变换，那么服务端每次接收到一个新的 SYN 后，都会给它分配一个半连接，服务端的 SYNACK 根据之前的 SYN 包找到的是错误的客户端 IP， 所以也就无法收到客户端的 ACK 包，导致无法正确建立 TCP 连接，这就会让服务端的半连接队列耗尽，无法响应正常的 SYN 包。

为了防止 SYN Flood 攻击，Linux 内核引入了 SYN Cookies 机制。收到 SYN 包时，不去分配资源来保存客户端的信息，而是根据这个 SYN 包计算出一个 Cookie 值，然后将 Cookie 记录到 SYNACK 包中发送出去。对于正常的连接，该 Cookies 值会随着客户端的 ACK 报文被带回来。然后 服务端再根据这个 Cookie 检查这个 ACK 包的合法性，如果合法，才去创建新的 TCP 连接。通过这种处理，SYN Cookies 可以防止部分 SYN Flood 攻击。所以对于 Linux 服务器而言，推荐开启 SYN Cookies：

> net.ipv4.tcp_syncookies = 1

`net.ipv4.tcp_synack_retries`：服务端向客户端发送的 SYNACK 包也可能会被丢弃，或者因为某些原因而收不到客户端的响应，这个时候服务端也会重传 SYNACK 包。同样地，重传的次数也是由配置选项来控制的，该配置选项是 `tcp_synack_retries`，其重传策略和 `tcp_syn_retries` 是一致的。 `tcp_synack_retries`默认值为5，对于数据中心的服务器而言，通常都不需要这么大的值，推荐设置为 2。

`net.core.somaxconn`：客户端收到服务端发送的 SYNACK 报文后发送 ACK 报文，服务端收到客户端的ACK 报文后，TCP三次握手完成，即产生了一个 TCP 全连接（complete），它会被添加到全连接队列（accept queue）中，然后服务端就会调用 `accept() `来完成 TCP 连接的建立。全连接队列（accept queue）的长度也有限制，目的就是为了防止服务端不能及时调用 `accept()` 而浪费太多的系统资源。全连接队列的长度是由 `listen(sockfd, backlog)` 这个函数里的 backlog 控制的，而该 backlog 的最大值则是 somaxconn。somaxconn 在 5.4 之前的内核中，默认都是 128（5.4 开始调整为了默认 4096），建议将该值适当调大一些：

> net.core.somaxconn = 16384

`net.ipv4.tcp_abort_on_overflow`：当服务器中积压的全连接个数超过`net.core.somaxconn`后，新的全连接就会被丢弃掉。服务器在将新连接丢弃时，有的时候需要发送 reset 来通知客户端，这样客户端就不会再次重试了。不过，默认行为是直接丢弃不去通知客户端。至于是否需要给客户端发送 reset，是由 `tcp_abort_on_overflow` 这个配置项来控制的，该值默认为 0，即不发送 reset 给客户端，推荐也是将该值配置为 0。因为，如果服务端来不及调用 `accept()` 而导致全连接队列满，这往往是由瞬间有大量新建连接请求导致的，正常情况下服务端很快就能恢复，然后客户端再次重试后就可以建连成功了。即，将 `tcp_abort_on_overflow` 配置为 0，给了客户端一个重试的机会，也可以根据你的实际情况来决定是否要使能该选项。

`accept()`调用 成功返回后，一个新的 TCP 连接就建立完成了，TCP 连接进入到了 ESTABLISHED 状态：

![image-20250315222631713](https://hunk-pic-store.oss-cn-beijing.aliyuncs.com/img/tcp_conn2.png)

### 影响TCP连接断开的系统配置

如下是TCP连接断开的四次挥手的过程（由客户端主动发起）：

![image-20250315222745709](https://hunk-pic-store.oss-cn-beijing.aliyuncs.com/img/tcp_conn3.png)

首先调用 `close()` 的一侧是 active close（主动关闭），而接收到对端的 FIN 包后再调用 `close()` 来关闭的一侧，称之为 passive close（被动关闭）。在四次挥手的过程中，有三个 TCP 状态需要额外关注，就是上图中深红色的那三个状态：主动关闭方的 `FIN_WAIT_2` 和 `TIME_WAIT`，以及被动关闭方的 `CLOSE_WAIT` 状态，除了 `CLOSE_WAIT` 状态外，其余两个状态都有对应的系统配置项来控制。

`net.ipv4.tcp_fin_timeout`：首先来看 `FIN_WAIT_2` 状态，TCP 进入到这个状态后，如果本端迟迟收不到对端的 FIN 包，那就会一直处于这个状态，于是就会一直消耗系统资源。Linux 为了防止这种资源的开销，设置了这个状态的超时时间 `tcp_fin_timeout`，默认为 60s，超过这个时间后就会自动销毁该连接。至于本端为何迟迟收不到对端的 FIN 包，通常情况下都是因为对端机器出了问题，或者是因为太繁忙而不能及时 `close()`。所以，通常我们都建议将 `tcp_fin_timeout` 调小一些，以尽量避免这种状态下的资源开销。对于数据中心内部的机器而言，将它调整为 2s 足矣：

> net.ipv4.tcp_fin_timeout = 2

`net.ipv4.tcp_max_tw_buckets`：`TIME_WAIT` 状态存在的意义是，最后发送的这个 ACK 包可能会被丢弃掉或者有延迟，这样对端就会再次发送 FIN 包。如果不维持 `TIME_WAIT` 这个状态，那么再次收到对端的 FIN 包后，本端就会回一个 Reset 包，这可能会产生一些异常。所以维持 TIME_WAIT 状态一段时间，可以保障 TCP 连接正常断开。TIME_WAIT 的默认存活时间在 Linux 上是 60s（TCP_TIMEWAIT_LEN），这个时间对于数据中心而言可能还是有些长了，所以有的时候也会修改如下内核配置项将其减小：

> net.ipv4.tcp_max_tw_buckets = 10000

`net.ipv4.tcp_tw_reuse`：客户端关闭跟服务端的连接后，也有可能很快再次跟服务端之间建立一个新的连接，而由于 TCP 端口最多只有 65536 个，如果不去复用处于 `TIME_WAIT` 状态的连接，就可能在快速重启应用程序时，出现端口被占用而无法创建新连接的情况。所以建议打开复用 TIME_WAIT 的选项：

> net.ipv4.tcp_tw_reuse = 1

系统中没有对应的配置项控制 CLOSE_WAIT 状态，但是该状态也是一个危险信号，如果这个状态的 TCP 连接较多，那往往意味着应用程序有 Bug，在某些条件下没有调用 close() 来关闭连接。我们在生产环境上就遇到过很多这类问题。所以，如果我们的系统中存在很多 CLOSE_WAIT 状态的连接，最好去排查一下应用程序，看看哪里漏掉了 close()。