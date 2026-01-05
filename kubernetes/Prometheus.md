# Prometheus Metrics

## cAdvisor

```yaml
    - job_name: 'kubernetes-cadvisor'
      scheme: https
      tls_config:
        ca_file: /var/run/secrets/kubernetes.io/serviceaccount/ca.crt
      bearer_token_file: /var/run/secrets/kubernetes.io/serviceaccount/token
      kubernetes_sd_configs:
      - role: node
      # 丢掉一些不适用的metrics，节省存储空间
      metric_relabel_configs:
      - source_labels: [__name__]
        action: drop
        regex: container_blkio_device_usage_total|container_tasks_state
      relabel_configs:
      - action: labelmap
        regex: __meta_kubernetes_node_label_(.+)
      - target_label: __address__
        replacement: kubernetes.default.svc:443
      - source_labels: [__meta_kubernetes_node_name]
        action: replace
        target_label: node
      - source_labels: [node]
        regex: (.+)
        target_label: __metrics_path__
        replacement: /api/v1/nodes/${1}/proxy/metrics/cadvisor
```

在prometheues console上可以通过`container_fs_writes_bytes_total{"pod"="nginx2-65744b8455-lqhkt"}`查询到pod内的文件系统写入数据的统计指标。pod监控指标数据的查询链路：promtheues -> kube-apiserver metrics api -> metrics-server api -> kubelet metrics api (cAdvisor)。

也可以直接调用kube-apiserver的metrics接口查询，下面4组数据是从2个维度产生的：

1. 不同设备：`/dev/dm-0`和`/dev/sda`
2. 不同cgroupfs id：一般只有pod/container这一级的scope文件里才有有效数据：

```bash
[root@master ~]# curl -k https://172.16.9.175:6443/api/v1/nodes/master/proxy/metrics/cadvisor | grep -v "^#" | grep container_fs_writes_bytes_total | grep nginx2-65744b8455-lqhkt
# 
container_fs_writes_bytes_total{container="",device="/dev/dm-0",id="/kubepods.slice/kubepods-pod0a2d3fc7_592e_46b5_8d02_a23af4b3de99.slice",image="",name="",namespace="default",pod="nginx2-65744b8455-lqhkt"} 0 1736152702701
# 
container_fs_writes_bytes_total{container="",device="/dev/sda",id="/kubepods.slice/kubepods-pod0a2d3fc7_592e_46b5_8d02_a23af4b3de99.slice",image="",name="",namespace="default",pod="nginx2-65744b8455-lqhkt"} 0 1736152702701
# 
container_fs_writes_bytes_total{container="nginx2",device="/dev/dm-0",id="/kubepods.slice/kubepods-pod0a2d3fc7_592e_46b5_8d02_a23af4b3de99.slice/docker-66a9f16df3c0d338394f80bfa8ee28561f64496541edcdbc5d021e17dfba1ec3.scope",image="sha256:c1d55261dc68855d3afd1b4d4d5ac6c45dd246f745b799cb3ade36f774cc17d6",name="k8s_nginx2_nginx2-65744b8455-lqhkt_default_0a2d3fc7-592e-46b5-8d02-a23af4b3de99_0",namespace="default",pod="nginx2-65744b8455-lqhkt"} 1.0738528256e+10 1736152696101
# 
container_fs_writes_bytes_total{container="nginx2",device="/dev/sda",id="/kubepods.slice/kubepods-pod0a2d3fc7_592e_46b5_8d02_a23af4b3de99.slice/docker-66a9f16df3c0d338394f80bfa8ee28561f64496541edcdbc5d021e17dfba1ec3.scope",image="sha256:c1d55261dc68855d3afd1b4d4d5ac6c45dd246f745b799cb3ade36f774cc17d6",name="k8s_nginx2_nginx2-65744b8455-lqhkt_default_0a2d3fc7-592e-46b5-8d02-a23af4b3de99_0",namespace="default",pod="nginx2-65744b8455-lqhkt"} 1.073741824e+10 1736152696101
```

### pod磁盘写速率

我们以如下的promtheues规则来统计pod的磁盘写速率，主要通过kubelet/cAdvisor从blkio cgroupfs里捕获的`container_fs_write_bytes_total`指标（磁盘同步写入数据总量（/sys/fs/cgroup/blkio/kubepods.slice/kubepods-pod0a2d3fc7_592e_46b5_8d02_a23af4b3de99.slice/docker-66a9f16df3c0d338394f80bfa8ee28561f64496541edcdbc5d021e17dfba1ec3.scope/blkio.throttle.io_service_bytes）计算得到。需要注意的是，`container_fs_write_bytes_total`指标只统计实际完成的磁盘IO操作，不包含异步IO写入的数据，这是cAdvisor的设计机制。应用程序一般的默认写入操作都是write back机制，即数据其实只写入了page cache，并没有真正落盘，os会有一些机制在合适的时机将数据落盘。比如使用`dd`命令写入文件时，如果没有使用`-oflag=direct`，大多数时候都看不到pod磁盘写数据。

```yaml
      - record: pod_fs_write_rate
        expr: label_replace(avg(rate(container_fs_writes_bytes_total{image!=""}[1m])) by(pod,namespace,node),"pod_name","$1","pod","(.*)")
```



