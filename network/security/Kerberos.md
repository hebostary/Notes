# Kerberos介绍

## 什么是Kerberos?

Kerberos是一个计算机网络认证协议（computer-network authentication protocol），由MIT设计和开发维护，属于开源项目，在github上也有源代码。Kerberos基于ticket的设计，允许计算机节点在不安全的网络中以安全的方式向对端提供身份信息，因此在一些涉及到用户身份认证和授权的程序中得到广泛应用，比如：

1. Active Directory（AD），这是微软非常著名的活动目录软件，在中大型企业的IT系统中非常常见，用于管理批量的计算机设备及用户（组）信息。从Win Server2003开始，AD就采用Kerberos作为其认证协议。微软并没有直接用MIT提供的Kerberos包，而是实现了自己的Kerberos版本，以便更好的与AD集成。
2. NFS，Linux上的文件共享服务，应用非常广泛的一种NAS存储。NFSv4支持客服端通过Kerberos来完成和服务端之间的身份认证，以此提供更为安全的存储访问。

Kerberos v1-v4这几个版本都主要是用于MIT的内部项目，被外界广泛使用的是Kerberos v5（krb5）。

## 解析Kerberos协议细节

如下图所示，Kerberos协议中包含3个主要角色：

1. Client，访问网络服务的客户端，比如挂载NFS共享存储的客户端。
2. Service Server（SS），提供网络服务的服务器，比如NFS服务器。
3. Key Distribution Center（KDC），Kerberos协议中的密钥分发中心，主要包含Authentication Server（AS）和Ticket-Granting Server（TGS）两个组件。

![img](https://hunk-pic-store.oss-cn-beijing.aliyuncs.com/img/kerberos-protocol.png)

在Kerberos协议中，客户端向 AS 进行身份验证，AS 将用户名转发到密钥分发中心 (KDC)。 KDC 发出一张Ticket-Granting Ticket (TGT)，该Ticket带有时间戳并使用 TGS 的密钥对其进行加密，并将加密结果返回给用户的机器。这个过程很少进行，通常在用户登录客户端时进行；尽管用户的会话管理器可能会在用户登录时透明地更新它，但 TGT 会在某个时候到期。

当客户端需要与另一个节点（Kerberos 用语中的principal）上的服务通信时，客户端将 TGT 发送到 TGS，后者通常与 KDC 共享同一主机。服务必须已经使用SPN (Service Princial Name，服务主体名称) 向 TGS 注册。客户端使用 SPN 请求访问此服务。在验证 TGT 有效并且允许用户访问请求的服务后，TGS 向客户端发出Ticket和Session key。然后，客户端将Ticket连同其服务请求一起发送到SS。

Kerberos协议主要包含下面几个过程：

1. User Client-based Login without Kerberos - 用户登录客户端，Kerberos不参与。
2. Client Authentication - 客户端认证，此过程主要验证客户端身份信息的合法性。
3. Client Service Authorization - 客户端服务授权，此过程主要验证客户端是否可以访问请求的服务。
4. Client Service Request - 客户端向服务端发起请求，服务端开始提供服务。

### User Client-based Login without Kerberos

1. 用户在客户端机器上输入用户名和密码。 pkinit (RFC 4556) 等其他凭证机制允许使用公钥代替密码。客户端将密码转换为对称密码的密钥。这要么使用内置密钥调度，要么使用单向散列，具体取决于所使用的cipher suite。

2. 服务器接收用户名和对称密码，并将其与数据库中的数据进行比较。如果密码与为用户存储的密码匹配，则登录成功。

### Client Authentication

1. 客户端代表用户向 AS（认证服务器）发送用户 ID 的明文消息请求服务。 （注意：密钥和密码都不会发送到 AS。）

2. AS 检查客户端是否在其数据库中。如果是，AS 通过散列在数据库中找到的用户密码（例如，Windows Server 中的 Active Directory）生成`Client secret key`（图中绿色钥匙），并将以下两条消息发送回客户端：
   1. Msg A：使用`Client secret key`加密的`Client/TGS session key`（图中红色钥匙）。
   2. Msg B：`Ticket-Granting-Ticket`（TGT，包括客户端 ID、客户端网络地址、Ticket有效期和`Client/TGS session key`），使用 `TGS secret key`（图中黄色钥匙，AS和TGS共享）加密。

3. 一旦客户端收到Msg A 和 Msg B，它就会尝试用用户输入的密码生成的`Client secret key`解密消息 A。如果用户输入的密码与AS数据库中的密码不匹配，客户端的secret key就会不同，从而无法解密消息A。 客户端使用有效的密码和密钥解密消息A以获得`Client/TGS Session Key`。该会话密钥用于与 TGS 的进一步通信。 （注意：客户端无法解密消息 B，因为它是使用 `TGS secret key`加密的）。此时，客户端有足够的信息向 TGS 验证自己。

### Client Service Authorization

1. 当请求服务时，客户端向TGS发送以下消息：
   1. Msg C：由消息 B 和所请求服务的 ID 组成。
   2. Msg D：身份验证器（Authenticator，由客户端 ID 和时间戳组成），使用`Client/TGS session key`加密。

2. 在收到消息 C 和 D 后，TGS 从消息 C 中检索消息 B。它使用  `TGS secret key`解密消息 B，得到了TGT，TGT包含了`Client/TGS session key`和客户端 ID。使用这个`Client/TGS session key`，TGS 解密消息 D（身份验证器）并比较消息 B 和 D 中的客户端 ID；这两个客户端ID，其中一个来自AS，一个来自客户端自己，如果它们匹配，说明当前客户端是合法的，服务器将向客户端发送以下两条消息：
   1. Msg E：使用`Service secret key`（图中黑色钥匙）加密的`Client-to-Server ticket`（包括客户端 ID、客户端网络地址、有效期和`Client/Server session key`（图中蓝色钥匙））。
   2. Msg F：使用`Client/TGS session key`加密的`Client/Server session key`。

这个过程主要是TGS通过AS和客户端各自提供的客户端ID，验证了客户端的合法性，然后向客户端签发了用于访问SS的Ticket，Ticket包含了后面Client和SS通信使用的session key。

### Client Service Request

1. 从 TGS 收到消息 E 和 F 后，客户端有足够的信息向服务服务器 (SS) 验证自己。客户端连接到 SS 并发送以下两条消息：
   1. Msg E：来自上一步中Client从TGS那里接受到的Msg E，这是客户端到服务器的Ticket，使用`Service secret key`加密。
   2. Msg G：一个新的 Authenticator，它包括客户端 ID、时间戳，并使用`Client/Server session key`加密。
2. SS 使用自己的`Service secret key`解密Msg E，以检索`Client/Server session key`。使用这个session key，SS 解密 Authenticator 并比较消息 E 和 G 中的客户端 ID，如果它们匹配，服务器将向客户端发送以下消息以确认其真实身份并愿意为客户端提供服务：
   1. Msg H：在客户端Authenticator中找到的时间戳（在版本 4 中加 1，但在版本 5[6][7] 中不需要），并使用`Client/Server session key`加密。

3. 客户端使用`Client/Server session key`解密确认（消息 H）并检查时间戳是否正确。如果是这样，那么客户端可以信任服务器并可以开始向服务器发出服务请求。

4. 服务器向客户端提供请求的服务。

Kerberos协议看起来的确不是那么容易理解，这个过程中生成的各种key和ticket很容易让人混淆。但是，本质上只有两种key，secret key和session key。

Client -> AS -> TGS -> SS这几个服务形成了一条单向链表，两个相邻的节点间共享一个secret key。链表中相邻两个节点可以直接安全地交换交换消息，但是跨节点就不可以了，就需要借助session key。比如Client和AS共享了`Client secret key`，它基于用户密码加上盐salt后散列生成，因此AS和Client之间可以安全的交换消息。但是，整个过程就是Client分别和AS,TGS,SS去沟通。Client和TGS是两个不相邻的节点，它们之间没有共享secret key，所以不能安全地交换消息。因此，就需要借助session key的机制：

1. AS给Client签发一个需要直接转发给TGS的Ticket（Ticket里面包含了`Client/TGS session key`），这个Ticket被AS和TGS之间共享的`TGS secret key`加密，因此只有AS和TGS可以解开它。
2. Client和TGS沟通时，就需要转发AS提供的Ticket和一个用session key加密的消息。
3. TGS依次解密两条消息，得到了用于和Client通信的`Client/TGS session key`，并解析其中的Client ID完成授权验证，之后TGS和Client就可以用`Client/TGS session key`来安全通信了。
4. Client和SS沟通的时候也是相同的套路。

最终，Client和SS之间得到一个共享的并且安全的`Client/Server session key`，这个session key在用户每次登录client的时候就会被刷新，也可以强制刷新，它可以在后续的通信中用于数据的对称加解密。

至此，我们可以看到Kerberos协议的一些设计特点：

1. 没有在客户端存储用户的密码，解决了客户端密码可能泄露的问题。
2. 没有在网络中传输用户密码，解决了密码可能被网络攻击者窃取的问题。
3. 引入了专门的密钥中心KDC来动态签发Ticket。

对比参考Kerberos和SSL/TLS的设计，我们会发现一些有意思的地方：

1. 在SSL/TLS握手过程中，客户端和服务器先通过非对称加解密的机制（PKI证书）协商得到了一个对称加解密的密钥用于数据的安全传输，而PKI证书的安全性又由第三方权威CA机构来保证。
2. 在Kerberos认证的过程中，客户端和服务器也是通过第三方密钥分发中心KDC得到了一个可以用作数据传输加密的对称会话密钥。

# Kerberos应用

## NFS v4 + Krb5提供更安全的NAS存储

默认的NFS共享的认证方式是sys，这种方式并不安全，NFS v4 + Krb5的组合可以提供更为安全的NFS存储。下面演示最基础的NFS v4 + Krb5的配置，实际上NFS还支持krb5p等更严格的认证方式，可以在涉及到这块时参考相关的官方文档。

### 一些准备工作

在NFS v4 + Krb5的模型中，NFS server就是上图中的SS，我们这里把NFS server和KDC server部署在了同一台机器上。Kerberos协议的时间戳机制要求各个机器间的时间要严格同步，因此需要准备一台ntp服务器。下面就是准备好的服务器环境：

1. KDC server + NFS server：kdcnfsserver.domain.com

   needed rpms：krb5-server krb5-libs krb5-workstation nfs ...

2. NFS client：nfsclient.domain.com

   needed rpms：krb5-server krb5-libs krb5-workstation nfs-utils

3. NTP server：ntpserver.domain.com

这些机器的OS版本都是`RHEL 7.9`。

配置ntp服务器的大致步骤，最好是开始就先在几个机器上配置好ntp服务。

````bash
$ systemctl enable ntpd
$ systemctl start ntpd

# 修改/etc/ntp.conf
# server ntpserver.domain.com

$ ntpq -p
$ ntpstat

# 手动快速同步时间
$ systemctl stop ntpd
$ ntpdate ntpserver.domain.com
$ systemctl start ntpd
````

### 搭建KDC server

**1. 配置/etc/krb5.conf**

按照默认的`EXAMPLE.COM`配置即可，realm不需要DNS可解析。

```bash
$ cat /etc/krb5.conf
# Configuration snippets may be placed in this directory as well
includedir /etc/krb5.conf.d/

[logging]
 default = FILE:/var/log/krb5libs.log
 kdc = FILE:/var/log/krb5kdc.log
 admin_server = FILE:/var/log/kadmind.log

[libdefaults]
 dns_lookup_realm = false
 ticket_lifetime = 24h
 renew_lifetime = 7d
 forwardable = true
 rdns = false
 pkinit_anchors = FILE:/etc/pki/tls/certs/ca-bundle.crt
 default_ccache_name = KEYRING:persistent:%{uid}

 default_realm = HUNKKDC.COM

[realms]
HUNKKDC.COM = {
  kdc = kdcnfsserver.domain.com
  admin_server = kdcnfsserver.domain.com
}

[domain_realm]
.veritas.com = HUNKKDC.COM
veritas.com = HUNKKDC.COM
```

**2.配置/var/kerberos/krb5kdc/kdc.conf**

```bash
$ cat /var/kerberos/krb5kdc/kdc.conf
[kdcdefaults]
 kdc_ports = 88
 kdc_tcp_ports = 88

[realms]
 HUNKKDC.COM = {
  #master_key_type = aes256-cts
  acl_file = /var/kerberos/krb5kdc/kadm5.acl
  dict_file = /usr/share/dict/words
  admin_keytab = /var/kerberos/krb5kdc/kadm5.keytab
  supported_enctypes = aes256-cts:normal aes128-cts:normal des3-hmac-sha1:normal arcfour-hmac:normal camellia256-cts:normal camellia128-cts:normal des-hmac-sha1:normal des-cbc-md5:normal des-cbc-crc:normal
 }
```

**3. 配置/var/kerberos/krb5kdc/kadm5.acl**

```bash
$ cat /var/kerberos/krb5kdc/kadm5.acl
*/admin@HUNKKDC.COM     *
```

**4. 初始化principal数据库/var/kerberos/krb5kdc/principal**

```bash
$ kdb5_util create -s -r HUNKKDC.COM
Loading random data
Initializing database '/var/kerberos/krb5kdc/principal' for realm 'HUNKKDC.COM',
master key name 'K/M@HUNKKDC.COM'
You will be prompted for the database Master Password.
It is important that you NOT FORGET this password.
Enter KDC database master key: 
Re-enter KDC database master key to verify:
```

**5. 启动kdc和kadmin**

```bash
$ systemctl enable krb5kdc kadmin
$ systemctl start krb5kdc kadmin
```

**6. 创建Principal**

```bash
$ kadmin.local 
Authenticating as principal root/admin@HUNKKDC.COM with password.
kadmin.local:  addprinc root/admin                      # 1
WARNING: no policy specified for root/admin@HUNKKDC.COM; defaulting to no policy
Enter password for principal "root/admin@HUNKKDC.COM": 
Re-enter password for principal "root/admin@HUNKKDC.COM": 
Principal "root/admin@HUNKKDC.COM" created.
kadmin.local:  addprinc root                            # 2
WARNING: no policy specified for root@HUNKKDC.COM; defaulting to no policy
Enter password for principal "root@HUNKKDC.COM": 
Re-enter password for principal "root@HUNKKDC.COM": 
Principal "root@HUNKKDC.COM" created.
kadmin.local:  addprinc -randkey host/kdcnfsserver.domain.com    # 3
WARNING: no policy specified for host/kdcnfsserver.domain.com@HUNKKDC.COM; defaulting to no policy
Principal "host/kdcnfsserver.domain.com@HUNKKDC.COM" created.
kadmin.local:  ktadd host/kdcnfsserver.domain.com     # 创建keytab文件
Entry for principal host/kdcnfsserver.domain.com with kvno 2, encryption type aes256-cts-hmac-sha1-96 added to keytab FILE:/etc/krb5.keytab.
Entry for principal host/kdcnfsserver.domain.com with kvno 2, encryption type aes128-cts-hmac-sha1-96 added to keytab FILE:/etc/krb5.keytab.
...
kadmin.local:  exit
```

**7. 开启防火墙端口**

```bash
$ firewall-cmd --permanent --add-service=kerberos
$ firewall-cmd --permanent --add-port=749/tcp
$ firewall-cmd --reload
```

### 配置NFS server

因为和KDC server共享了机器，NFS server不需要再加入Kerebros domain里，否则也得配置/etc/krb5。

**1. 打开防火墙**

```bash
$ firewall-cmd --permanent --add-service=nfs  # only for NFS v4
$ firewall-cmd --reload 
```

**2. 启动nfs服务**

```bash
$ systemctl enable nfs
$ systemctl start nfs
```

**3. 创建nfs共享**

```bash
$ cat /etc/exports
#default export for logs
/log *(rw,no_root_squash,sec=sys:krb5) #支持默认的认证方式（sys）和krb5的认证方式

$ systemctl restart nfs

# 查看NFS共享
$ showmount -e localhost
$ cat /proc/fs/nfs/exports 
# Version 1.1
# Path Client(Flags) # IPs
/log    *(rw,no_root_squash,sync,wdelay,no_subtree_check,uuid=ee9d955a:d7544d55:b30498b7:4ec7f772,sec=1:390003)
/       *(ro,no_root_squash,sync,no_wdelay,no_subtree_check,v4root,fsid=0,uuid=b4938e62:cedb46ab:97a07397:3ba2fde2,sec=390003:390004:390005:1:1:0:0)
```

**4. 创建nfs principal**

```bash
$ kadmin -p root/admin
Authenticating as principal root/admin with password.
Password for root/admin@HUNKKDC.COM: 
kadmin:  
kadmin:  
kadmin:  addprinc -randkey nfs/kdcnfsserver.domain.com
WARNING: no policy specified for nfs/kdcnfsserver.domain.com@HUNKKDC.COM; defaulting to no policy
Principal "nfs/kdcnfsserver.domain.com@HUNKKDC.COM" created.
kadmin:  list_principals
K/M@HUNKKDC.COM
host/kdcnfsserver.domain.com@HUNKKDC.COM
kadmin/admin@HUNKKDC.COM
kadmin/changepw@HUNKKDC.COM
kadmin/kdcnfsserver.domain.com@HUNKKDC.COM
kiprop/kdcnfsserver.domain.com@HUNKKDC.COM
krbtgt/HUNKKDC.COM@HUNKKDC.COM
nfs/kdcnfsserver.domain.com@HUNKKDC.COM
root/admin@HUNKKDC.COM
root@HUNKKDC.COM
kadmin:  ktadd nfs/kdcnfsserver.domain.com
Entry for principal nfs/kdcnfsserver.domain.com with kvno 2, encryption type aes256-cts-hmac-sha1-96 added to keytab FILE:/etc/krb5.keytab.
Entry for principal nfs/kdcnfsserver.domain.com with kvno 2, encryption type aes128-cts-hmac-sha1-96 added to keytab FILE:/etc/krb5.keytab.
...
kadmin:  exit
```

因为KDC server和NFS server部署在一起，所以它们共享/etc/krb5.conf和/etc/krb5.keytab：

```bash
$ ktutil /etc/krb5.keytab
ktutil:  l -e
slot KVNO Principal
---- ---- ---------------------------------------------------------------------
ktutil:  rkt /etc/krb5.keytab
ktutil:  l -e
slot KVNO Principal
---- ---- ---------------------------------------------------------------------
   1    2 host/kdcnfsserver.domain.com@HUNKKDC.COM (aes256-cts-hmac-sha1-96) 
   2    2 host/kdcnfsserver.domain.com@HUNKKDC.COM (aes128-cts-hmac-sha1-96) 
,,,
   9    2 nfs/kdcnfsserver.domain.com@HUNKKDC.COM (aes256-cts-hmac-sha1-96) 
  10    2 nfs/kdcnfsserver.domain.com@HUNKKDC.COM (aes128-cts-hmac-sha1-96) 
...
ktutil:  exit
```

### 配置NFS client

**1. 加入Kerberos domain**

为了方便，可以直接将KDC server上的/etc/krb5.conf拷贝到NFS client上。

**2. 创建principal**

```bash
$ kadmin 
Authenticating as principal root/admin@HUNKKDC.COM with password.
Password for root/admin@HUNKKDC.COM: 
kadmin:  addprinc -randkey nfs/nfsclient.domain.com
WARNING: no policy specified for nfs/nfsclient.domain.com@HUNKKDC.COM; defaulting to no policy
Principal "nfs/nfsclient.domain.com@HUNKKDC.COM" created.
kadmin:  ktadd nfs/nfsclient.domain.com
Entry for principal nfs/nfsclient.domain.com with kvno 2, encryption type aes256-cts-hmac-sha1-96 added to keytab FILE:/etc/krb5.keytab.
Entry for principal nfs/nfsclient.domain.com with kvno 2, encryption type aes128-cts-hmac-sha1-96 added to keytab FILE:/etc/krb5.keytab.
...
kadmin:  exit
```

```bash
$ klist 
Ticket cache: KEYRING:persistent:0:krb_ccache_ZOs3ski
Default principal: nfs/nfsclient.domain.com@HUNKKDC.COM

Valid starting       Expires              Service principal
12/31/1969 16:00:00  12/31/1969 16:00:00  Encrypted/Credentials/v1@X-GSSPROXY:
```

**3. 挂载NFS共享**

```bash
$ mkdir /mnt/nfs1
$ chmod 777 /mnt/nfs1 #不设置成777，开启krb5认证后，nfs client读写文件会出现permission denied问题
```

```bash
$ systemctl enable nfs-client.target && systemctl start nfs-client.target
mount -t nfs -o sec=krb5 kdcnfsserver.domain.com:/log /home/maintenance/nfs_mp/ -v
```

完成以上配置后，在client上挂载NFS共享时，client就会先到KDC server上完成认证授权。

### 参考文章

[RHEL7: Use Kerberos to control access to NFS network shares](https://www.certdepot.net/rhel7-use-kerberos-control-access-nfs-network-shares/)

[Errors Setting Up Kerberos](http://research.imb.uq.edu.au/~l.rathbone/ldap/kerberos.shtml)

[MIT Kerberos Troubleshooting](https://web.mit.edu/kerberos/krb5-latest/doc/admin/troubleshoot.html)