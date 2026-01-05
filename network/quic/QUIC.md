# QUIC 协议
## 更快的链接握手

## 重传机制
## 改进的拥塞控制
## 多路复用解决队头阻塞
## 连接迁移

## Datagrams 特性


# quic-go

## quic 核心配置项


## 典型问题
### timer.Reset 导致 quic 连接 run loop 卡死
在 v0.55.0之前的版本，高负载场景下使用 http3.Transport 很容易出现一个问题，客户端发送请求会发现所有的请求流都会卡住，没有任何报文进入sendQueue，最终导致对端因为 `idle_timeout` 关闭了连接，但是本端又没做任何处理。从 qlog 观察本端长时间没有`transport::packet_sent` 事件，即使将 idle_timeout 设置的很长也会遇到此问题。大概率是 quic 连接的 `timer.Reset` 阻塞在读取 timer 里的通道（ `internal/utils/timer.go` 里的 `<-t.t.C` 调用），导致 quic 连接管理的核心 run() 函数卡住。曾尝试实现连接池自行管理http3.Transport 创建的 quic 连接，检测到连接异常后调用 conn.CloseWithError 也会卡住。参考 v0.55.0 里的这个修复： [#5339](https://github.com/quic-go/quic-go/pull/5339)。

quic-go 里自行实行的 conneciton_timer 原本是为了解决在 Go 1.23 之前标准库里的 timer 提供的 Reset() 方法在并发场景下不安全的问题，因为 GC 不会恢复没有到期或者已经被 Stop 的 timer，并且 timer 里的通道（`timer.C`）使用的是带缓冲的通道，调用 Reset 后但是原来的 timer 仍然活跃，所以可能从通道收到多个事件，导致程序错误，具体可以参考 `internal/utils/timer.go` 里 `Reset()` 方法的注释。但是 quic-go 这个 wrapper 实现仍然可能导致协程永久阻塞，Go 1.23 后将 `timer.C` 改成了无缓冲的，并且保证了 Reset() 调用是安全的。

## qlog tracer
Quic 协议是默认使用 TLS 1.3 加密的，很多控制流的报文也是加密的，加之多路复用的机制以及Quic 协议会交换很多辅助报文（`PADDING`），通过 tcpdump 和 wireshark 抓包分析不太方便。确实需要抓包分析，可以参考如下代码将 TLS key 打印出来然后导入到 Wireshark 中，这样可以把所有的 quic 报文解密出来。
```go
return &tls.Config{
	Certificates: []tls.Certificate{tlsCert},
	// Required by HTTP/3
	NextProtos: []string{"h3"},
	KeyLogWriter: openKeyLogFile(),
}

// openKeyLogFile opens the path from SSLKEYLOGFILE for TLS key logging.
// Returns nil if the env is unset or file can't be opened.
func OpenKeyLogFile() *os.File {
    path := os.Getenv("SSLKEYLOGFILE")
    if path == "" {
        return nil
    }
    f, err := os.OpenFile(path, os.O_CREATE|os.O_WRONLY|os.O_APPEND, 0o600)
    if err != nil {
        return nil
    }
    return f
}
```
Qlog tracer 是 quic-go 提供的用于跟踪 quic 连接的调试分析利器，quic-go 在涉及 quic 连接管理的关键路径会调用 tracer 打印调试信息，比如握手，发送接收元数据、连接metrics、连接关闭信息、发送队列积压等。
在服务端和客户端创建 quicConfig 的时候如下指定 Tracer 参数即可，可以使用默认的 ConnectionTracer，也可以添加自定义的打印甚至自己实现一个 ConnectionTracer。默认的 ConnectionTracer 只有设置了环境变量 `QLOGDIR` 才会生效。
```go
quicConfig = &quic.Config{
	Tracer: func(ctx context.Context, p logging.Perspective, connID quic.ConnectionID) *logging.ConnectionTracer {
		fmt.Printf("qlog connection ID %s", connID)
		return qlog.DefaultConnectionTracer(ctx, p, connID)
	}
}
```

### packet_sent
`transport::packet_sent` 事件在 **数据包被序列化并准备发送时记录**,而不是在 UDP 报文实际从网络接口发出时。`transport::packet_sent` 表示数据包已经被写入发送队列,但**不保证已经从 UDP socket 发出**。它更准确地说是"准备发送"而不是"已发送"。
在 quic-go 的实现中,发送数据包的流程如下:
1. **打包阶段**:在 `connection.go` 中,数据包被打包并记录日志[link to Repo quic-go/quic-go: connection_logging.go:83-135]
2. **tracer 回调**:在打包完成后,会调用 tracer 的 `SentShortHeaderPacket` 或 `SentLongHeaderPacket` 回调[link to Repo quic-go/quic-go: connection_logging.go:115-134]
3. **qlog 记录**:在 `qlog/connection_tracer.go` 中,这些回调会生成 `eventPacketSent` 事件[link to Repo quic-go/quic-go: qlog/connection_tracer.go:232-279]
4. **实际发送**:数据包随后通过 `sendQueue` 发送到网络。
### packet_received
`transport::packet_received` 事件在 **数据包被成功解析和解密后记录**,而不是在 UDP 报文刚到达时。`transport::packet_received` 表示数据包**已经到达进程内部并被成功处理**(解密、解析)。如果数据包在解密前就被丢弃,不会记录此事件。
接收数据包的流程如下:
1. **UDP 接收**：数据包从 UDP socket 读取
2. **初步处理**：在 `Transport.handlePacket()` 中进行初步处理[link to Repo quic-go/quic-go: transport.go:570-624]
3. **队列缓存**：数据包被放入连接的 `receivedPackets` 队列[link to Repo quic-go/quic-go: connection.go:1549-1567]
4. **解密和解析**：在连接的 run loop 中,数据包被取出、解密和解析
5. **tracer 回调**：解析成功后,调用 `ReceivedShortHeaderPacket` 或 `ReceivedLongHeaderPacket`
6. **qlog 记录**：生成 `eventPacketReceived` 事件[link to Repo quic-go/quic-go: qlog/connection_tracer.go:281-308]

**长时间没有 packet_received 的含义**：如果发送端有大量 `transport::packet_sent`,但接收端在密集接收一组 packet 后长时间(5分钟)没有任何 `transport::packet_received`,这强烈暗示后续的 UDP 包被丢弃了。可能的原因包括:
1. 接收队列满 (DoS Prevention)：最可能的原因是接收端的 `receivedPackets` 队列已满,触发 DoS 防护机制。[link to Repo quic-go/quic-go: connection.go:1552-1558]。当队列长度达到 `MaxConnUnprocessedPackets` (256) 时,新数据包会被丢弃并记录 `packet_dropped` 事件(trigger: `dos_prevention`)。这种情况下,数据包到达了 UDP socket,但在进入连接处理队列前就被丢弃,因此不会有 `packet_received` 事件。
2. 解密失败：如果数据包无法解密(例如密钥不匹配),也不会记录 `packet_received` 事件。无法解密的数据包会被缓存在 `undecryptablePackets` 队列中,但如果队列满了也会被丢弃。[link to Repo quic-go/quic-go: connection.go:2507-2523]。这种情况出现的概率非常低。
3. 网络层丢包：如果 UDP 包在网络层就丢失了(例如操作系统的 UDP 接收缓冲区满),数据包根本不会到达应用层,自然也不会有任何 qlog 记录。UDP 接收缓冲区是**操作系统内核级别的资源**,每个 UDP socket 都有独立的接收缓冲区。用户态的 QUIC 协议栈**不会主动触发维护或更新UDP 缓冲区的动作**，唯一的解决方案**是在系统级别调整内核参数`net.core.rmem_max`和`net.core.rmem_default`
4. 连接已关闭：如果连接已经关闭,新到达的数据包会被直接丢弃,不会记录 `packet_received`。

#诊断建议 
1. 要确认具体原因,建议检查:
2. **packet_dropped 事件**:查看接收端的 qlog 是否有 `transport::packet_dropped` 事件,特别是 `trigger: dos_prevention`
3. **连接状态**:确认连接是否仍然活跃,是否有 `connection_closed` 事件
4. **系统资源**:使用 `netstat -su` 或 `ss -u` 检查 UDP 缓冲区溢出统计
5. **CPU 使用率**:高 CPU 使用率可能导致数据包处理延迟,进而导致队列积压。