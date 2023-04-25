

# firewalld

firewalld主要分为两层，核心层和最上面的D-Bus层：核心层负责处理配置和后端，如iptables、ip6tables、ebtables、ipset和模块加载器。

![firewalld layers](https://firewalld.org/documentation/firewalld-structure+nftables.png)

1. RHEL7/CentOS7及以下版本：firewalld在Backends层使用`iptables`操作内核里的`netfilter`，

2. RHEL8/CentOS8及以上版本：firewalld在Backends层使用更先进的`nftables`操作内核里的`netfilter`，同时也提供了iptables命令工具，以保留一定的向后兼容能力，但是这个iptables命令是基于nftables库做的封装，不能查询/管理所`nftables`的所有规则，因此不推荐直接使用iptables。

   ```bash
   $ iptables --version
   iptables v1.8.4 (nf_tables)
   ```

3. 不管Backends层用的是iptables还是nftables，最终到内核层都是基于`netfilter`网络过滤器。

## 管理firewalld zone

```bash
$ firewall-cmd --get-zones # 查看可用zone的列表
$ firewall-cmd --permanent --list-ports #查看打开的端口
$ firewall-cmd --list-all-zones #查看所有zone的详细信息
$ firewall-cmd --zone=zone-name --list-all #查看指定zone的详细信息

$ firewall-cmd --get-default-zone #查询默认zone
$ firewall-cmd --set-default-zone zone-name #设置默认zone
$ firewall-cmd --get-active-zones #查询活跃的zone以及分配的网卡
$ firewall-cmd --add-service=ssh --zone=public #在public zone允许ssh服务
```

## 使用firewalld控制网络流量

### 在紧急情况下禁掉所有网络流量

```bash
$ firewall-cmd --panic-on #打开panic mode，立即断开所有网络流量，最好在有物理访问方式时使用
$ firewall-cmd --panic-off #关闭panic mode
$ firewall-cmd --query-panic #查询panic mode
```

### 使用预定义的服务控制网络流量

https://firewalld.org/documentation/howto/add-a-service.html

```bash
$ firewall-cmd --list-services #列出还没有被允许的服务？
cockpit dhcpv6-client https mountd nfs nfs3 samba ssh
$ firewall-cmd --get-services #列出所有预定义的服务
... git grafana gre high-availability http https ...
```

以`https`服务为例，默认地，我们可以从其他host去访问443端口：

```bash
$ firewall-cmd --permanent --service=https --get-ports
443/tcp

# 在其它host上发起请求
$ curl -v https://100.188.146.136:443
* About to connect() to 100.188.146.136 port 443 (#0)
*   Trying 100.188.146.136...
* Connected to 100.188.146.136 (100.188.146.136) port 443 (#0)
* Initializing NSS with certpath: sql:/etc/pki/nssdb
*   CAfile: /etc/pki/tls/certs/ca-bundle.crt
  CApath: none
* Server certificate:
*     ...
* NSS error -8172 (SEC_ERROR_UNTRUSTED_ISSUER)
* Peer's certificate issuer has been marked as not trusted by the user.
* Closing connection 0
curl: (60) Peer's certificate issuer has been marked as not trusted by the user.
More details here: http://curl.haxx.se/docs/sslcerts.html
```

如果将`https`服务移除并重新加载规则：

```bash
$ firewall-cmd --permanent --remove-service=https #从默认zone里移除服务
success
$ firewall-cmd --reload
success
$ firewall-cmd --query-service=https [--zone=public] #查询服务是否被添加到某个zone
no
```

我们将无法再从其它host访问443端口，但是仍然可以从本机访问443端口：

```bash
$ curl -v https://100.188.146.136:443
* About to connect() to 100.188.146.136 port 443 (#0)
*   Trying 100.188.146.136...
* No route to host
* Failed connect to 100.188.146.136:443; No route to host
* Closing connection 0
curl: (7) Failed connect to 100.188.146.136:443; No route to host
```

### 创建新的服务

```bash
$ firewall-cmd --permanent --new-service=test-svc
$ firewall-cmd --permanent --service=test-svc --set-description="Test Service Demo"
$ firewall-cmd --permanent --service=test-svc --set-short="Test Service"
$ firewall-cmd --permanent --service=test-svc --add-port=13701-13724/tcp

$ firewall-cmd --add-service=test-svc --zone=public
$ firewall-cmd --reload
```

firewalld也支持从xml文件创建新的服务，创建成功后<service-name>.xml会被拷贝到`/etc/firewalld/services/`目录下。

```bash
$ cat /root/test-svc.xml
<?xml version="1.0" encoding="utf-8"?>
<service>
  <short>Hun Service Used for Test</short>
  <description>HTTPS is a modified HTTP used to serve Web pages when security is important. Examples are sites that require logins like stores or web mail. This option is not required for viewing pages locally or developing Web pages. You need the httpd package installed for this option to be useful.</description>
  <port protocol="tcp" port="445"/>
</service>

$ firewall-cmd --new-service-from-file=/root/test-svc.xml --permanent
success
$ firewall-cmd --permanent --service=test-svc --add-port=13701-13724/tcp
success
$ firewall-cmd --add-service=test-svc
success
$ firewall-cmd --list-services
cockpit dhcpv6-client https mountd nfs nfs3 samba ssh test-svc
```

## 端口转发

```bash
$ firewall-cmd --permanent --add-forward-port=port=13784:proto=tcp:toaddr=127.0.0.1:toport=13784
```

## 参考文档

[RHEL8 - Using and configuring firewalld](https://access.redhat.com/documentation/en-us/red_hat_enterprise_linux/8/html/configuring_and_managing_networking/using-and-configuring-firewalld_configuring-and-managing-networking#listing-zones_working-with-firewalld-zones)

https://firewalld.org/documentation/

# nftables

## 通过nft直接管理netfilter

### 管理tables

```bash
$ nft list tables [<family>]
$ nft list table [<family>] <name> [-n] [-a]
$ nft (add | delete | flush) table [<family>] <name>
```

### 管理chains

```bash
$ nft (add | create) chain [<family>] <table> <name> [ { type <type> hook <hook> [device <device>] priority <priority> \; [policy <policy> \;] } ]
$ nft (delete | list | flush) chain [<family>] <table> <name>
$ nft rename chain [<family>] <table> <name> <newname>
```

### 管理rules

```bash
$ nft list ruleset #列出所有rules
$ nft add rule [<family>] <table> <chain> <matches> <statements>
$ nft insert rule [<family>] <table> <chain> [position <position>] <matches> <statements>
$ nft replace rule [<family>] <table> <chain> [handle <handle>] <matches> <statements>
$ nft delete rule [<family>] <table> <chain> [handle <handle>]

# examples
$ nft --handle list chain ip filter INPUT
table ip filter {
        chain INPUT { # handle 1
                type filter hook input priority filter; policy accept;
                ...
                tcp dport 13724 counter packets 0 bytes 0 accept # handle 29
        }
}

$ nft delete rule filter INPUT handle 29
```

### 调试rules

要确定规则是否匹配，可以使用计数器。

```bash
# examples
# 添加新的rule并使用counter参数
$ nft add rule inet example_table example_chain tcp dport 22 counter accept
# or 在已有rule的基础上添加计数器
$ nft --handle list chain inet example_table example_chain
table inet example_table {
  chain example_chain { # handle 1
    type filter hook input priority filter; policy accept;
    tcp dport ssh accept # handle 4
  }
}
$ nft replace rule inet example_table example_chain handle 4 tcp dport 22 counter accept
$ nft list ruleset
table inet example_table {
  chain example_chain {
    type filter hook input priority filter; policy accept;
    tcp dport ssh counter packets 6872 bytes 105448565 accept
  }
}
```

nftables中的跟踪特性与nft monitor命令相结合，使管理员能够显示匹配规则的数据包。通过在rule里替换`meta nftrace set 1`参数来添加跟踪特性。

```bash
# examples
$ nft --handle list chain ip filter INPUT
table ip filter {
        chain INPUT { # handle 1
                type filter hook input priority filter; policy accept;
								...
                meta l4proto tcp tcp dport 13724 counter packets 16 bytes 1056 accept # handle 122
        }
}
$ nft replace rule ip filter INPUT handle 122 tcp dport 13724 meta nftrace set 1 accept
$ nft --handle list chain ip filter INPUT
table ip filter {
        chain INPUT { # handle 1
                type filter hook input priority filter; policy accept;
                ...
                tcp dport 13724 meta nftrace set 1 accept # handle 122
        }
}
$ nft monitor | grep "ip filter INPUT"
trace id 188eadfb ip filter INPUT packet: iif "eth1" ether saddr 00:50:56:9e:6b:da ether daddr 00:50:56:9e:43:55 ip saddr 100.88.146.133 ip daddr 100.88.146.131 ip dscp cs0 ip ecn not-ect ip ttl 64 ip id 14795 ip length 60 tcp sport 59776 tcp dport 13724 tcp flags == syn tcp window 29200 
trace id 188eadfb ip filter INPUT rule tcp dport 13724 meta nftrace set 1 accept (verdict accept)
trace id 104c2038 ip filter INPUT packet: iif "eth1" ether saddr 00:50:56:9e:6b:da ether daddr 00:50:56:9e:43:55 ip saddr 100.88.146.133 ip daddr 100.88.146.131 ip dscp cs0 ip ecn not-ect ip ttl 64 ip id 14796 ip length 52 tcp sport 59776 tcp dport 13724 tcp flags == ack tcp window 29 
trace id 104c2038 ip filter INPUT rule tcp dport 13724 meta nftrace set 1 accept (verdict accept)
```

### 备份和恢复nftables rule set

```bash
# 备份
$ nft list ruleset > file.nft
 # or
$ nft -j list ruleset > file.json

# 恢复
$ nft -f file.nft
# or
$ nft -j -f file.json
```

## 参考文档

[Getting started with nftables](https://access.redhat.com/documentation/en-us/red_hat_enterprise_linux/8/html/configuring_and_managing_networking/getting-started-with-nftables_configuring-and-managing-networking#monitoring-packets-that-match-an-existing-rule_debugging-nftables-rules)

[Quick reference-nftables in 10 minutes](https://wiki.nftables.org/wiki-nftables/index.php/Quick_reference-nftables_in_10_minutes)

https://netfilter.org/index.html
