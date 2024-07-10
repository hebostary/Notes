#  TLS analysis

## TLS v1.2

###  ServerHandshake

```go
// serverHandshakeState contains details of a server handshake in progress.
// It's discarded once the handshake has completed.
type serverHandshakeState struct {
	c            *Conn
	ctx          context.Context
	clientHello  *clientHelloMsg
	hello        *serverHelloMsg
	suite        *cipherSuite
	ecdheOk      bool
	ecSignOk     bool
	rsaDecryptOk bool
	rsaSignOk    bool
	sessionState *sessionState
	finishedHash finishedHash
	masterSecret []byte
	cert         *Certificate
}
```

### processClientHello



### checkForResumption

基本流程：

1. 检查是否开启了SessionTicket配置；

2. 对ClientHello消息里携带的sessionTicket进行解密，并解析sessionTicket的明文；//crypto/tls/ticket.go -> decryptTicket()

3. 检查sessionTicket的生命周期：7 days；

   ```go
   const maxSessionTicketLifetime = 7 * 24 * time.Hour
   ```

4. 检查TLS版本：如果前面协商的TLS版本发生了变化，不会再复用sessionTicket；

5. 检查cipherSuite：首先看ClientHello消息里CipherSuites是否包含sessionTicket里的cipherSuite，然后看服务端是否仍然支持这个cipherSuite；

6. 检查客户端证书：如果当前服务端的配置（ClientAuthType）是`RequireAnyClientCert`或者`RequireAndVerifyClientCert`，而sessionTicket里没有，或者服务端的配置是`NoClientCert`而sessionTicket里包含了客户端证书， 都无法复用这个sessionTicket；

   ```go
   // ClientAuthType declares the policy the server will follow for
   // TLS Client Authentication.
   type ClientAuthType int
   
   const (
   	// NoClientCert indicates that no client certificate should be requested
   	// during the handshake, and if any certificates are sent they will not
   	// be verified.
   	NoClientCert ClientAuthType = iota
   	// RequestClientCert indicates that a client certificate should be requested
   	// during the handshake, but does not require that the client send any
   	// certificates.
   	RequestClientCert
   	// RequireAnyClientCert indicates that a client certificate should be requested
   	// during the handshake, and that at least one certificate is required to be
   	// sent by the client, but that certificate is not required to be valid.
   	RequireAnyClientCert
   	// VerifyClientCertIfGiven indicates that a client certificate should be requested
   	// during the handshake, but does not require that the client sends a
   	// certificate. If the client does send a certificate it is required to be
   	// valid.
   	VerifyClientCertIfGiven
   	// RequireAndVerifyClientCert indicates that a client certificate should be requested
   	// during the handshake, and that at least one valid certificate is required
   	// to be sent by the client.
   	RequireAndVerifyClientCert
   )
   ```

### FullHandshake

#### pickCipherSuite

我们先看一下CipherSuite的数据结构：

```go
// A cipherSuite is a TLS 1.0–1.2 cipher suite, and defines the key exchange
// mechanism, as well as the cipher+MAC pair or the AEAD.
type cipherSuite struct {
	id uint16
	// the lengths, in bytes, of the key material needed for each component.
	keyLen int
	macLen int
	ivLen  int
	ka     func(version uint16) keyAgreement
	// flags is a bitmask of the suite* values, above.
	flags  int
	cipher func(key, iv []byte, isRead bool) interface{}
	mac    func(key []byte) hash.Hash
	aead   func(key, fixedNonce []byte) aead
}
```

下面是是实现的CipherSuite：

```go
var cipherSuites = []*cipherSuite{ // TODO: replace with a map, since the order doesn't matter.
	{TLS_ECDHE_RSA_WITH_CHACHA20_POLY1305, 32, 0, 12, ecdheRSAKA, suiteECDHE | suiteTLS12, nil, nil, aeadChaCha20Poly1305},
	{TLS_ECDHE_ECDSA_WITH_CHACHA20_POLY1305, 32, 0, 12, ecdheECDSAKA, suiteECDHE | suiteECSign | suiteTLS12, nil, nil, aeadChaCha20Poly1305},
	{TLS_ECDHE_RSA_WITH_AES_128_GCM_SHA256, 16, 0, 4, ecdheRSAKA, suiteECDHE | suiteTLS12, nil, nil, aeadAESGCM},
	{TLS_ECDHE_ECDSA_WITH_AES_128_GCM_SHA256, 16, 0, 4, ecdheECDSAKA, suiteECDHE | suiteECSign | suiteTLS12, nil, nil, aeadAESGCM},
	{TLS_ECDHE_RSA_WITH_AES_256_GCM_SHA384, 32, 0, 4, ecdheRSAKA, suiteECDHE | suiteTLS12 | suiteSHA384, nil, nil, aeadAESGCM},
	{TLS_ECDHE_ECDSA_WITH_AES_256_GCM_SHA384, 32, 0, 4, ecdheECDSAKA, suiteECDHE | suiteECSign | suiteTLS12 | suiteSHA384, nil, nil, aeadAESGCM},
	{TLS_ECDHE_RSA_WITH_AES_128_CBC_SHA256, 16, 32, 16, ecdheRSAKA, suiteECDHE | suiteTLS12, cipherAES, macSHA256, nil},
	{TLS_ECDHE_RSA_WITH_AES_128_CBC_SHA, 16, 20, 16, ecdheRSAKA, suiteECDHE, cipherAES, macSHA1, nil},
	{TLS_ECDHE_ECDSA_WITH_AES_128_CBC_SHA256, 16, 32, 16, ecdheECDSAKA, suiteECDHE | suiteECSign | suiteTLS12, cipherAES, macSHA256, nil},
	{TLS_ECDHE_ECDSA_WITH_AES_128_CBC_SHA, 16, 20, 16, ecdheECDSAKA, suiteECDHE | suiteECSign, cipherAES, macSHA1, nil},
	{TLS_ECDHE_RSA_WITH_AES_256_CBC_SHA, 32, 20, 16, ecdheRSAKA, suiteECDHE, cipherAES, macSHA1, nil},
	{TLS_ECDHE_ECDSA_WITH_AES_256_CBC_SHA, 32, 20, 16, ecdheECDSAKA, suiteECDHE | suiteECSign, cipherAES, macSHA1, nil},
	{TLS_RSA_WITH_AES_128_GCM_SHA256, 16, 0, 4, rsaKA, suiteTLS12, nil, nil, aeadAESGCM},
	{TLS_RSA_WITH_AES_256_GCM_SHA384, 32, 0, 4, rsaKA, suiteTLS12 | suiteSHA384, nil, nil, aeadAESGCM},
	{TLS_RSA_WITH_AES_128_CBC_SHA256, 16, 32, 16, rsaKA, suiteTLS12, cipherAES, macSHA256, nil},
	{TLS_RSA_WITH_AES_128_CBC_SHA, 16, 20, 16, rsaKA, 0, cipherAES, macSHA1, nil},
	{TLS_RSA_WITH_AES_256_CBC_SHA, 32, 20, 16, rsaKA, 0, cipherAES, macSHA1, nil},
	{TLS_ECDHE_RSA_WITH_3DES_EDE_CBC_SHA, 24, 20, 8, ecdheRSAKA, suiteECDHE, cipher3DES, macSHA1, nil},
	{TLS_RSA_WITH_3DES_EDE_CBC_SHA, 24, 20, 8, rsaKA, 0, cipher3DES, macSHA1, nil},
	{TLS_RSA_WITH_RC4_128_SHA, 16, 20, 0, rsaKA, 0, cipherRC4, macSHA1, nil},
	{TLS_ECDHE_RSA_WITH_RC4_128_SHA, 16, 20, 0, ecdheRSAKA, suiteECDHE, cipherRC4, macSHA1, nil},
	{TLS_ECDHE_ECDSA_WITH_RC4_128_SHA, 16, 20, 0, ecdheECDSAKA, suiteECDHE | suiteECSign, cipherRC4, macSHA1, nil},
}
```



大概按照以下几个步骤挑选CipherSuite：

1. 根据默认的CipherSuite优先级列表和服务端配置的CipherSuite列表得出实际的服务端优先级列表：

   ```go
   //crypto/tls/cipher_suites.go
   var cipherSuitesPreferenceOrder = []uint16{
   	// AEADs w/ ECDHE
   	TLS_ECDHE_ECDSA_WITH_AES_128_GCM_SHA256, TLS_ECDHE_RSA_WITH_AES_128_GCM_SHA256,
   	TLS_ECDHE_ECDSA_WITH_AES_256_GCM_SHA384, TLS_ECDHE_RSA_WITH_AES_256_GCM_SHA384,
   	TLS_ECDHE_ECDSA_WITH_CHACHA20_POLY1305, TLS_ECDHE_RSA_WITH_CHACHA20_POLY1305,
   	// CBC w/ ECDHE
   	TLS_ECDHE_ECDSA_WITH_AES_128_CBC_SHA, TLS_ECDHE_RSA_WITH_AES_128_CBC_SHA,
   	TLS_ECDHE_ECDSA_WITH_AES_256_CBC_SHA, TLS_ECDHE_RSA_WITH_AES_256_CBC_SHA,
   	// AEADs w/o ECDHE
   	TLS_RSA_WITH_AES_128_GCM_SHA256,
   	TLS_RSA_WITH_AES_256_GCM_SHA384,
   	// CBC w/o ECDHE
   	TLS_RSA_WITH_AES_128_CBC_SHA,
   	TLS_RSA_WITH_AES_256_CBC_SHA,
   	// 3DES
   	TLS_ECDHE_RSA_WITH_3DES_EDE_CBC_SHA,
   	TLS_RSA_WITH_3DES_EDE_CBC_SHA,
   	// CBC_SHA256
   	TLS_ECDHE_ECDSA_WITH_AES_128_CBC_SHA256, TLS_ECDHE_RSA_WITH_AES_128_CBC_SHA256,
   	TLS_RSA_WITH_AES_128_CBC_SHA256,
   	// RC4
   	TLS_ECDHE_ECDSA_WITH_RC4_128_SHA, TLS_ECDHE_RSA_WITH_RC4_128_SHA,
   	TLS_RSA_WITH_RC4_128_SHA,
   }
   // disabledCipherSuites are not used unless explicitly listed in
   // Config.CipherSuites. They MUST be at the end of cipherSuitesPreferenceOrder.
   var disabledCipherSuites = []uint16{
   	// CBC_SHA256
   	TLS_ECDHE_ECDSA_WITH_AES_128_CBC_SHA256, TLS_ECDHE_RSA_WITH_AES_128_CBC_SHA256,
   	TLS_RSA_WITH_AES_128_CBC_SHA256,
   	// RC4
   	TLS_ECDHE_ECDSA_WITH_RC4_128_SHA, TLS_ECDHE_RSA_WITH_RC4_128_SHA,
   	TLS_RSA_WITH_RC4_128_SHA,
   }
   ```

2. 检查硬件以及客户端是否支持AES-GCM模式，如果不支持就排除相关的CipherSuite；

3. 根据服务端的优先级列表和客户端支持的CipherSuite列表挑选出最终使用的CipherSuite。

在我们的demo中，客户端和服务端都是golang 1.17，限定了协议版本是TLS1.2，最终被挑选的就是`TLS_ECDHE_RSA_WITH_AES_128_CBC_SHA`（0xc013 = 49171）：

![image-20210908123507500](https://hunk-pic-store.oss-cn-beijing.aliyuncs.com/img/20210908123608.png)

#### doFullHandshake

基本流程：

1. 发送Server Hello消息；

2. **发送服务端Certificate**；

3. 发送Certificate Status记录，如果开启了OCSP；

4. **生成并发送ServerKeyExchange**；

5. 发送Certificate Request；这个是可选的，取决于服务端的Client Auth配置要大于等于 RequestClientCert，默认是NoClientCert。

   ```go
   type ClientAuthType int
   
   const (
   	// NoClientCert indicates that no client certificate should be requested
   	// during the handshake, and if any certificates are sent they will notbe verified.
   	NoClientCert ClientAuthType = iota
   	// RequestClientCert indicates that a client certificate should be requested
   	// during the handshake, but does not require that the client send any certificates.
   	RequestClientCert
   	// RequireAnyClientCert indicates that a client certificate should be requested
   	// during the handshake, and that at least one certificate is required to be
   	// sent by the client, but that certificate is not required to be valid.
   	RequireAnyClientCert
   	// VerifyClientCertIfGiven indicates that a client certificate should be requested
   	// during the handshake, but does not require that the client sends a
   	// certificate. If the client does send a certificate it is required to be
   	// valid.
   	VerifyClientCertIfGiven
   	// RequireAndVerifyClientCert indicates that a client certificate should be requested
   	// during the handshake, and that at least one valid certificate is required
   	// to be sent by the client.
   	RequireAndVerifyClientCert
   )
   ```

6. 发送Server Hello Done消息；

   > 至此，这6个步骤都是服务端在解析Client Hello消息之后的TLS握手行为，此时，客户端在发送Client Hello消息后一直在等待服务端的响应。TLS连接有独立的发送缓冲区，上面发送的几条消息的size都不大，因此实际上并没有立刻发送给客户端，在发送Server Hello Done消息后，服务端会执行一个`flush`刷一次缓冲区。如果我们用抓包软件，这几个服务端消息一般在同一个TCP报文里面。这个时候刷新缓冲区是因为轮到客户端作为了，flush执行完之后，服务端就会等待读取来自客户端的握手消息。

7. 根据配置处理客户端证书。

8. **接收并处理clientKeyExchangeMsg**，计算得到`preMasterSecret`（32 Bytes）。

9. TODO：计算`masterSecret`（48 Bytes）。计算`masterSecret`需要用到CipherSuite，preMasterSecret，Client Hello里的随机数，以及Server Hello里的随机数。

下面对其中部分重要的握手协议步骤进行更详细的剖析：

##### generateServerKeyExchange

以目前比较通用的ECDHE这种KeyAgreement为例，其基本流程如下：

1. 遍历ClientHello消息里支持的Curve和服务端支持的Curve来挑选合适的Curve。客户端和服务端默认的Curve的优先级都是X25519, CurveP256, CurveP384, CurveP521。因此，绝大多数情况下挑选出来的曲线就是X25519了。如果挑选到后面3种Curve之一，将会立即尝试创建对应类型的该曲线实例。

2. 生成ECDHE参数（generateECDHEParameters）：

   1. X25519： 首先生成32字节的随机数作为private key；然后以X25519内置的基本曲线点（[32]byte{9, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}）生成public key。最后返回的ECDHE参数就是private key和public key。
   2. TODO：其它3种曲线的参数生成方式

3. 组装serverECDHEParams，格式如下（从左至右），这里一共36个字节：

   | Params     | Named curve | curveID      | len (public key) | public key    |
   | ---------- | ----------- | ------------ | ---------------- | ------------- |
   | len (byte) | 1           | 2            | 1                | 32            |
   | value      | 3           | 29（X25519） | 32               | ${public key} |

4. 根据协议版本，服务端private key的签名算法，以及ClientHello里携带的签名算法集挑选本次使用的签名算法（signatureAlgorithm）；我们demo里使用的RSA算法签发的证书，如下所示，golang支持的RSA签名算法很多，这些都是复合签名算法。首先在这其中根据服务端private key的RSA签名算法的长度和TLS协议版本选择候选的RSA签名算法集。

   ```go
   // SignatureScheme identifies a signature algorithm supported by TLS. See
   // RFC 8446, Section 4.2.3.
   type SignatureScheme uint16
   const (
   	// RSASSA-PKCS1-v1_5 algorithms.
   	PKCS1WithSHA256 SignatureScheme = 0x0401
   	PKCS1WithSHA384 SignatureScheme = 0x0501
   	PKCS1WithSHA512 SignatureScheme = 0x0601
   	// RSASSA-PSS algorithms with public key OID rsaEncryption.
   	PSSWithSHA256 SignatureScheme = 0x0804
   	PSSWithSHA384 SignatureScheme = 0x0805
   	PSSWithSHA512 SignatureScheme = 0x0806
   	// ECDSA algorithms. Only constrained to a specific curve in TLS 1.3.
   	ECDSAWithP256AndSHA256 SignatureScheme = 0x0403
   	ECDSAWithP384AndSHA384 SignatureScheme = 0x0503
   	ECDSAWithP521AndSHA512 SignatureScheme = 0x0603
   	// EdDSA algorithms.
   	Ed25519 SignatureScheme = 0x0807
   	// Legacy signature and hash algorithms for TLS 1.2.
   	PKCS1WithSHA1 SignatureScheme = 0x0201
   	ECDSAWithSHA1 SignatureScheme = 0x0203
   )
   ```

   最终挑选出的候选签名算法有7个：

   1. PSSWithSHA256 (2052)
   2. PSSWithSHA384 (2053)
   3. PSSWithSHA512 (2054)
   4. PKCS1WithSHA256 (1025)
   5. PKCS1WithSHA384 (1281)
   6. PKCS1WithSHA512 (1537)
   7. PKCS1WithSHA1 (513)

   最后根据 ClientHello 消息里携带的签名算法集挑选出优先级最高的 PSSWithSHA256 (2052)，这个算法在ClientHelo里也是排在第一个。

5. 从前面挑选出的（复合）签名算法里解析出具体的签名类型（sigType）和哈希算法（sigHash）；

   PSSWithSHA256  -> signatureRSAPSS （226） + crypto.SHA256 （5）

6. 用上面的sigHash对（clientHello.random, hello.random, serverECDHEParams）组成的字节序列进行哈希。

7. 对第6步中得到的哈希值进行签名，得到signature。TODO：这个签名的原理和过程比较复杂，暂不深究。

8. 组装serverKeyExchangeMsg，格式如下：

   | Params     | serverECDHEParams    | signatureAlgorithm     | signature    |
   | ---------- | -------------------- | ---------------------- | ------------ |
   | len (byte) | 36                   | 2                      | 256          |
   | value      | ${serverECDHEParams} | 2052 ( PSSWithSHA256 ) | ${signature} |

   signatureAlgorithm这个字段只有TLS版本大于等于1.2的时候才会加进去。

最后，服务端在serverKeyExchangeMsg前面加上标准格式的消息类型和消息长度，发送给客户端。

##### processClientKeyExchange

处理clientKeyExchange主要是得到`preMasterSecret`，先看一下clientKeyExchangeMsg的数据结构：

```go
type clientKeyExchangeMsg struct {
	raw        []byte
	ciphertext []byte
}
```

其中，ciphertext一般是33个字节，第一个字节是客户端public key的长度（32），后面32个字节正是客户端的public key。通过服务端的private key和客户端的public key进行曲线点运算得到`preMasterSecret`，客户端通过自己的private key和服务端的public key计算得到相同的`preMasterSecret`，这里利用的是椭圆曲线点在有限域内的运算性质：(a * G) * b = a * ( G * b)，这个G就是我们前面提到的内置基本曲线点。

#### establishKeys

这一步非常重要，主要是基于前面的cipherSuite，masterSecret，clientRandom, serverRandom这些数据来计算下面这个6元组：

1. clientMAC
2. serverMAC
3. clientKey
4. serverKey
5. clientIV
6. serverIV

实际上分为3组，分别是MAC，Key和IV，每组服务端和客户端各一个数据。

下面是一组服务端调试数据：

#### ![image-20210911232157658](https://hunk-pic-store.oss-cn-beijing.aliyuncs.com/img/20210911232157.png)

clientKey和clientIV计算得到clientCipher，serverKey和serverIV计算得到serverCipher，clientCipher和serverCipher分别被设置到服务端和客户端的半连接里，用于后续数据交换的加解密。

```go
	var clientCipher, serverCipher interface{}
	var clientHash, serverHash hash.Hash

	if hs.suite.aead == nil {
		clientCipher = hs.suite.cipher(clientKey, clientIV, true /* for reading */)
		clientHash = hs.suite.mac(clientMAC)
		serverCipher = hs.suite.cipher(serverKey, serverIV, false /* not for reading */)
		serverHash = hs.suite.mac(serverMAC)
	} else {
		clientCipher = hs.suite.aead(clientKey, clientIV)
		serverCipher = hs.suite.aead(serverKey, serverIV)
	}

	c.in.prepareCipherSpec(c.vers, clientCipher, clientHash)
	c.out.prepareCipherSpec(c.vers, serverCipher, serverHash)
```

我们前面挑选的cipherSuite是TLS_ECDHE_RSA_WITH_AES_128_CBC_SHA（49171），它不是AEAD模式的，所以还要额外通过serverMAC和clientMAC来计算serverHash和ClientHash。

> 注意：这里的clientCipher和serverCipher是初始化过的函数，用于数据的加密和解密。serverHash 和 ClientHash也是初始化过的函数，用于计算MAC。

#### readFinished

读取客户端发送的ChangeCipherSpec消息和Finshed消息。

#### sendSessionTicket

如果配置支持SessionTicket，就将握手交换的这些数据打包并加密后发给客户端。

#### sendFinished

先向客户端发送一个 ChangeCipherSpec 消息，然后向客户端发送Finished消息，发送完后会立即执行`flush`刷新缓冲区。

至此，TLS连接握手完成，后面就是双方通过`write`和`read`交换应用数据了。

#### flush

强制将缓存里的消息发送到客户端。

### ResumeHandshake

#### doResumeHandshake

1. 发送 ServerHello 消息，并在消息里携带 sessionID，告诉客户端服务端正在处理 resumption，而不是完整的握手流程。
2. 处理客户端证书。如果服务端配置要求验证客户端证书，那session里就必须有合法的客户端证书，否则resumption失败。
3. 复用session里的`masterSecret`，这就省去了KeyAgreement的那些步骤。

#### establishKeys

这里和FullHandshake是一样的，主要是计算6元组。

#### sendSessionTicket

给客户端发送一个新的sessionTicket。

#### sendFinished

在sendSessionTicket后，不用再读取客户端发送的Finshed消息了，因为截至目前，客户端只发送了且仅需要发送一个ClientHello消息。

#### flush

强制将缓存里的消息发送到客户端。

#### readFinished

读取客户端发送的ChangeCipherSpec消息和Finshed消息。



在TLS握手的过程中，服务端和客户端最后都一定会向彼此发送一个ChangeCipherSpec消息和一个Finished消息：

1. FullHandshake：最后是服务端先读取客户端的ChangeCipherSpec/Finished消息，然后发送sessionTicket，最后向客户端发送ChangeCipherSpec/Finished消息。

2. ResumeHandshake：最后是服务端先发送ChangeCipherSpec/Finished消息，然后等待客户端的ChangeCipherSpec/Finished消息。

## TLS v1.3

```go
type serverHandshakeStateTLS13 struct {
	c               *Conn
	ctx             context.Context
	clientHello     *clientHelloMsg
	hello           *serverHelloMsg
	sentDummyCCS    bool
	usingPSK        bool
	suite           *cipherSuiteTLS13
	cert            *Certificate
	sigAlg          SignatureScheme
	earlySecret     []byte
	sharedKey       []byte
	handshakeSecret []byte
	masterSecret    []byte
	trafficSecret   []byte // client_application_traffic_secret_0
	transcript      hash.Hash
	clientFinished  []byte
}
```

