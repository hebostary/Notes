
#  Flagger 介绍

## Canary CRD

### Canary 状态转移

如下是Canary CRD 的状态转移图：

![image-20250509142312158](https://hunk-pic-store.oss-cn-beijing.aliyuncs.com/img/20250509142320.png)

下面是对 Canary CRD 各个状态的解释：

1. Initializing
   - Flagger detects a new Canary CRD
   - Creates the primary deployment from the target deployment
   - Sets up the necessary services (primary, canary, apex)
2. Initialized
   - Initial setup is complete
   - The primary deployment is running
   - The target deployment is scaled to zero
   - All traffic is routed to the primary
3. Waiting
   - Canary is waiting for changes to the target deployment
   - Flagger continuously checks for changes in:
     - Deployment spec (container image, resources, etc.)
     - ConfigMaps mounted as volumes or environment variables
     - Secrets mounted as volumes or environment variables
4. Progressing
   - A change was detected in the target deployment
   - Canary analysis has started
   - Traffic is gradually shifted from primary to canary
   - Metrics are analyzed at each step
   - Webhooks are executed for testing
5. WaitingPromotion
   - Analysis is complete and successful
   - Waiting for manual approval (if confirm-promotion webhook is configured)
6. Promoting
   - Canary is being promoted to primary
   - Primary deployment is updated with canary spec
   - ConfigMaps and Secrets are copied from canary to primary
7. Finalising
   - Primary deployment is being updated
   - Waiting for primary rollout to complete
8. Succeeded
   - Promotion completed successfully
   - All traffic is routed to the primary
   - Canary deployment is scaled to zero
9. Failed
   - Canary analysis failed (metrics threshold exceeded or webhook failed)
   - Traffic is routed back to primary
   - Canary deployment is scaled to zero
10. Terminating/Terminated
    - Canary resource is being deleted
    - Resources are cleaned up

## Canary Webhooks

Flagger 的 Canary CRD 中的 `webhooks` 是金丝雀发布流程中的关键扩展点（蓝绿部署也支持这些webhook），允许用户在发布的不同阶段触发自定义操作。以下是各阶段的详细解释和具体使用案例：

---

### **Webhook 阶段总览**

| ID   | ***\*阶段类型\****                       | ***\*触发时机\****                                       | ***\*结果处理\****                                           | ***\*典型用途\****                                           | ***\*是否阻塞流程\**** |
| ---- | ---------------------------------------- | -------------------------------------------------------- | ------------------------------------------------------------ | ------------------------------------------------------------ | ---------------------- |
| 1    | confirm_rollout（确认开始阶段）          | 检测到新版本                                             | Webhook返回200则继续发布流程；否则设置状态为 Waiting，暂停整个流程。 | 手动门控，决定是否开启金丝雀发布                             | 是                     |
| 2    | pre-rollout（预发布检查）                | 金丝雀发布开始前（流量切换前）                           | Webhook返回 HTTP 200 才继续发布；**失败则增加失败计数，达到阈值触发回滚** | 前置检查（数据库迁移、依赖服务验证）；生产环境建议配置       | 是                     |
| 3    | confirm-traffic-increase（流量增加确认） | 每次增加流量前需确认                                     | Webhook返回 HTTP 200 后继续增加流量；否则暂停流量增加，等待人工确认 | 人工审批或外部系统审批，例如等待运维团队审批。生产环境建议人工确认 | 是                     |
| 4    | rollout（发布期间测试）                  | 每次流量权重调整后                                       | Webhook返回 HTTP 200 则测试通过；**如果失败增加失败计数，达到阈值触发回滚** | 负载测试、自动化验证；可以用于自动生成负载测试流量，Flagger会持续监控应用指标（如错误率） | 否                     |
| 5    | confirm_promotion（确认推广）            | 在分析成功完成（达到设置的最高流量权重）后，推广到生产前 | Webhook返回 HTTP 200 后推广金丝雀版本，即更新主部署并切换流量到主部署；如果失败设置状态为 `WaitingPromotion`，继续运行指标检查 | 最后的手动确认，决定是否推广                                 | 是                     |
| 6    | post-rollout（发布后处理）               | 发布完成（成功或失败后）                                 | N/A                                                          | 发送成功/失败通知，清理临时资源、发送最终报告                | 否                     |
| 7    | rollback（回滚控制）                     | 在分析过程中持续检查                                     | Webhook返回 HTTP 200 后立即触发回滚                          | 主要用于手动触发回滚                                         | 否                     |
| 8    | event（事件通知）                        | 发布过程中发生特殊事件时                                 | N/A                                                          | 在发布过程中实时发送告警通知（Slack/PagerDuty）到外部系统    | 否                     |

关键差异总结：

| Webhook 类型               | 执行阶段       | 暂停行为           | 主要用途     |
| -------------------------- | -------------- | ------------------ | ------------ |
| `confirm-rollout`          | 开始前         | 暂停整个流程       | 批准开始发布 |
| `confirm-promotion`        | 推广前         | 暂停推广，继续监控 | 批准最终推广 |
| `confirm-traffic-increase` | 每次增加流量前 | 暂停流量增加       | 控制流量节奏 |

### **部分阶段详解与案例**

** `pre-rollout`（预发布检查）**

**用途**：确保发布前的系统状态满足条件  
**案例**：检查数据库迁移是否完成  

```yaml
webhooks:
  - name: "check-db-migration"
    url: "http://db-migration-checker.default.svc.cluster.local/status"
    timeout: "30s"
    type: "pre-rollout"
    metadata:
      service: "order-service"
```
**逻辑**：
1. Flagger 调用该 Webhook
2. 服务返回 HTTP 200 才继续发布，否则中止
3. 响应示例（需返回 JSON）：
   ```json
   { "status": "success", "details": "Database schema is compatible" }
   ```

**rollout`（流量调整后触发）**

**用途**：每次流量变化后执行验证  
**案例**：自动生成负载测试流量  

```yaml
webhooks:
  - name: "load-test"
    url: "http://flagger-loadtester.default/"
    type: "rollout"
    metadata:
      cmd: "hey -z 60s -q 10 -c 2 http://canary.order-service:8080/api/v1/health"
```
**效果**：
- 当流量切换到 10% 时，启动 60 秒的负载测试（10 QPS）
- Flagger 会持续监控应用指标（如错误率）

**`confirm-traffic-increase`（流量增加确认）**

**用途**：人工或外部系统审批  
**案例**：等待运维团队确认  

```yaml
webhooks:
  - name: "approval-gate"
    url: "http://approval-service.default/"
    type: "confirm-traffic-increase"
    metadata:
      approvers: "team-a,team-b"
```
**交互流程**：

1. Flagger 暂停发布，调用 Webhook
2. 人工在审批系统中点击通过
3. Webhook 返回 200 后继续增加流量

**confirm-rollout`（发布成功确认）**

**用途**：发布成功后执行后续操作  
**案例**：刷新 CDN 缓存  
```yaml
webhooks:
  - name: "purge-cdn"
    url: "https://api.cdn-provider.com/v1/purge"
    type: "confirm-rollout"
    metadata:
      zones: "eu,us"
```
**触发条件**：所有流量切换完成且指标达标

**`post-rollout`（发布后处理）**

**用途**：无论成功/失败都执行  
**案例**：发送发布报告  
```yaml
webhooks:
  - name: "report"
    url: "http://report-generator.default/"
    type: "post-rollout"
    metadata:
      format: "markdown"
```
**响应示例**：

```json
{
  "status": "failed",
  "reason": "High error rate detected",
  "timestamp": "2023-08-20T12:00:00Z"
}
```

**`event`（事件通知）**：

**用途**：实时通知发布状态变化  
**案例**：发送 Slack 告警  

```yaml
webhooks:
  - name: "slack-alert"
    url: "https://hooks.slack.com/services/TXXX/BYYY"
    type: "event"
    metadata:
      channel: "#alerts"
```
**触发事件**：
- `"type": "rollout"` → 开始发布
- `"type": "rollback"` → 回滚发生
- `"type": "analysis"` → 指标异常

---

### **完整示例**
```yaml
apiVersion: flagger.app/v1beta1
kind: Canary
metadata:
  name: order-service
spec:
  analysis:
    webhooks:
      # 前置检查
      - name: "pre-check"
        url: "http://pre-check.default/"
        type: "pre-rollout"
        timeout: 10s
      
      # 自动化测试
      - name: "load-test"
        url: "http://flagger-loadtester.default/"
        type: "rollout"
        metadata:
          cmd: "hey -z 30s http://order-service-canary:8080"
      
      # 人工审批
      - name: "approval"
        url: "http://approval-gate.default/"
        type: "confirm-traffic-increase"
      
      # 最终通知
      - name: "slack-notify"
        url: "https://hooks.slack.com/services/..."
        type: "post-rollout"
```

---

### **调试技巧**
1. **查看 Webhook 调用日志**：
   ```bash
   kubectl logs -f deploy/flagger -n flagger-system | grep webhook
   ```
2. **模拟 Webhook 响应**：
   ```bash
   kubectl port-forward svc/flagger-loadtester 8080:80 &
   curl -X POST http://localhost:8080/ -d '{"metadata": {"cmd": "echo test"}}'
   ```
3. **强制重试 Webhook**：
   ```bash
   kubectl annotate canary/order-service flagger.app/webhook-restart="true"
   ```

---

通过合理配置这些 Webhook，可以实现从 **自动化测试** 到 **人工审批** 的全流程控制，显著提升发布的可靠性和安全性。

#  基于 Flagger + Istio 实现灰度发布

## 金丝雀发布

基于 Flagger + Istio 的金丝雀发布参考： 

1. https://docs.flagger.app/tutorials/istio-progressive-delivery

2. https://docs.flagger.app/usage/how-it-works

下面的 Canary 资源可以实现金丝雀发布（meshProvidor 已经设置为 istio）：

```yaml
apiVersion: flagger.app/v1beta1
kind: Canary
metadata:
  name: my-app
spec:
  targetRef:
    apiVersion: apps/v1
    kind: Deployment
    name: my-app
  # 可选的
  autoscalerRef:
    apiVersion: autoscaling/v2
    kind: HorizontalPodAutoscaler
    name: my-app
    primaryScalerReplicas:
      minReplicas: 2
      maxReplicas: 5
  # 标识在集群中如何暴露应用
  service:
    name: my-app
    port: 8080
    portName: http
    appProtocol: http
    targetPort: 80
    portDiscovery: false
  analysis:
    interval: 1m
    threshold: 10
    maxWeight: 50
    stepWeight: 5
    metrics:
      - name: request-success-rate
        thresholdRange:
          min: 99
        interval: 1m
      - name: request-duration
        thresholdRange:
          max: 500
        interval: 1m
    # 定义在发布流程的关键阶段触发的 HTTP 回调（即 Webhook），其中的 url 字段指定了接收这些回调的端点地址
    # 这些webhook端点需返回 HTTP 200 表示成功，其他状态码会中止发布
    webhooks:
      # Flagger 会将cmd 作为payload 发送 POST 请求到 url，即由flagger-loadtester pod来执行负载测试
      # 没有测试流量，可能导致请求成功率指标分析没有数据而导致发布失败
      - name: load-test
        url: http://flagger-loadtester.istio-system/
        metadata:
          cmd: "hey -z 1m -q 10 -c 2 http://my-app-canary.default.svc.cluster.local:8080/"
```

如果 targetRef 指向的 Deployment 挂载了secrets 或者 configmap，Flagger 默认会创建一个带 `-primary` 后缀的副本，除非 在secrets 和 configmap 资源上加了注解`flagger.app/config-tracking: disabled`。

根据如上spec.service 的配置，Flagger 会创建如下的 Kubernetes Service：

- `<service.name>.<namespace>.svc.cluster.local`，selector `app=<name>-primary`
- `<service.name>-primary.<namespace>.svc.cluster.local`，selector `app=<name>-primary`
- `<service.name>-canary.<namespace>.svc.cluster.local`，selector `app=<name>`

`spec.analysis.interval` 参数定义了金丝雀分析的调度间隔，即 Flagger 控制器执行分析检查的频率 。在不同的部署策略中，这个参数的作用不同：

1. **金丝雀发布** - 控制流量权重递增的频率
2. **A/B 测试** - 控制迭代检查的间隔 
3. **蓝绿部署** - 控制测试和验证的执行频率

我们可以使用 `interval` 来估算金丝雀发布的总时间：

- **金丝雀发布**: `interval × (maxWeight / stepWeight)`
- **失败回滚**: `interval × threshold`

例如，如果 `interval: 1m`，`maxWeight: 50`，`stepWeight: 5`，那么完整的金丝雀发布需要 10 分钟。

指标配置里的 `request-success-rate` 这个指标由 Flagger 从Prometheus 查询请求监控指标计算得到，以 Istio 场景为例，Flagger 从 Istio生成的`istio_requests_total` 这个指标计算得到，计算方式如下：

```sql
sum(  
    rate(  
        istio_requests_total{  
          reporter="destination",  
          destination_workload_namespace="default",  
          destination_workload="nginx2",  
          response_code!~"5.*"  
        }[1m]  
    )  
)  
/  
sum(  
    rate(  
        istio_requests_total{  
          reporter="destination",  
          destination_workload_namespace="default",  
          destination_workload="nginx2"  
        }[1m]  
    )  
)
```



在标准的 Flagger 金丝雀发布流程中，两个 Deployment 的副本设置如下：

| Deployment       | 副本数管理方        | 典型副本数设置                  | 作用                                                               |
| :--------------- | :------------ | :----------------------- | :--------------------------------------------------------------- |
| `my-app`         | 用户 && Flagger | 通常设为 0，作为配置模板和 Canary 版本 | 作为用户管理的配置入口（GitOps 友好）；解耦用户操作和 Flagger 的内部状态管理                   |
| `my-app-primary` | Flagger       | 100% 生产流量所需的副本数          | 稳定版本（旧或新）；保证任何时候都有一个稳定的、全量服务的版本；避免直接修改运行中的 Deployment 导致的不可控滚动更新 |

- 当用户更新 my-app Deployment 的镜像版本时，Flagger 会检测到变更并开始发布流程，即更新`my-app` 作为 Canary 版本并切换逐步切换流量到 Canary  版本上。
- *经实践（Flagger v1.41.0），在触发金丝雀发布流程后，Flagger 并没有创建新的 my-app-canary deployment，而是直接更新原 my-app deployment 并将其副本从 0 设置为 1，以此 作为金丝雀发布的验证版本*。并且 my-app-canary service 里的 selector 也是 `app: my-app`。

**Flagger金丝雀发布流量切换过程 **

| 阶段     | my-app-primary (旧版本) | my-app (canary 新版本) | 流量分配                         |
| :------- | :---------------------- | :--------------------- | :------------------------------- |
| 发布前   | 旧版本运行中            | 不存在                 | 100% → primary                   |
| 发布开始 | 保持旧版本              | 新版本创建             | 90% → primary, 10% → canary      |
| 渐进发布 | 保持旧版本              | 新版本测试中           | 比例逐步变化(如 80/20, 70/30...) |
| 发布完成 | **被更新为新版本**      | 被删除                 | 100% → primary (现在已是新版本)  |

- `-primary` 始终代表当前应该接收生产流量的"主"版本，由 Flagger 在初始化 Canary 过程中创建；`-canary` 始终代表正在测试的"候选"新版本。这两个后缀是固定标识。
- 如果金丝雀发布完全成功，Flagger 会将新版本的 spec 应用到 `my-app-primary`，然后将 `my-app` 副本数重新设置为 0。此时， `my-app-primary`就是新版本。
- 如果金丝雀发布失败，Flagger 会回滚 `my-app`，然后保留 `my-app-primary`。

在 Istio 服务网格中， Flagger 可以自动创建、更新虚拟服务和目的规则等资源以实现流量的迁移。上游服务只需访问统一的服务地址 `my-app.prod.svc.cluster.local`，由 Istio 自动按权重分发请求。

>  如果未使用服务网格，Flagger 会修改原始 Service (`my-app`) 的 `selector`，逐步将流量从 `primary` 切换到 `canary`。

在基于 Flagger 实现灰度发布时，为已有 my-app deployment 创建 Canary 资源后，Flagger 控制器将对其进行初始化，完成初始化的前提条件包括：

1. Flagger 要能正常访问 meshProvider，一般是 服务网格里的 Prometheus 实例的访问端口。
2. Canary 资源里如果指定了 HPA，那么原来的 my-app 必须创建了名为 my-app  的 HPA 资源。

如果不满足这些条件，Canary 会一直处于正在初始化的状态。初始化过程中，Flagger 会执行以下动作：

1. 将原本的 my-app 副本数修改成 0，创建相应的 my-app-primary deployment用于承接所有访问流量。

2. 创建 my-app-primary 和 my-app-canary 两个 Service：

   | Service 名称     | 类型      | 选择器标签            | 用途                 |
   | :--------------- | :-------- | :-------------------- | :------------------- |
   | `my-app-primary` | ClusterIP | `app: my-app-primary` | 指向稳定版本的 Pod   |
   | `my-app-canary`  | ClusterIP | `app: my-app-canary`  | 指向金丝雀版本的 Pod |
   | `my-app`         | ClusterIP | `app: my-app`         | 旧版兼容             |

3. 创建 my-app-primary 和 my-app-canary 两个 DestinationRule 分别指向上面两个 Service。Flagger 已经创建了  primary 和 canary 版本的 DestinationRule，所以不会在 DestinationRule 内部定义 primary 和 canary 版本的 subset。

4. 创建一个 my-app VirtualService，路由规则里包含两个 host：my-app-primary 和 my-app-canary，流量权重分别为100和0。流量路径均为：client -> my-app server -> my-app-primary deployement。

当我们更新 my-app deployment 模板后，例如修改镜像或者 spec 里的其它字段，Flagger 会自动探测到变更（通过资源的 revision），然后触发金丝雀发布流程：

1. Flagger 根据 Canary 里配置的灰度发布参数，逐步更新  my-app VirtualService 的路由规则里的权重。my-app-canary 的流量权重逐步增多，my-app-primary 的流量权重逐步减少，直到 my-app-canary 的权重达到 Canary 里设置的最大权重，并维持一段时间。
2. 发布成功后，同步primary 版本的 configmap、secrets资源，然后更新 my-app-primary 并将其流量权重设置为100，将 my-app-canary 的流量权重设置为0，并将原 my-app 副本数再次设置为 0。 
3. 如果发布失败，则直接将 my-app-primary 流量权重设置为100，将 my-app-canary 的流量权重设置为0.
4. 有时候也会碰到一个问题，触发金丝雀发布后，Flagger 没能自动将原 my-app 并将其副本从 0 设置为 1，导致发布过程卡主。

**金丝雀发布图示 **

下图描述了金丝雀发布过程中，所有资源的关系：

![image-20250509154058139](https://hunk-pic-store.oss-cn-beijing.aliyuncs.com/img/20250509154058.png)

下图描述了金丝雀发布过程中，访问流量的迁移过程：

<img src="https://hunk-pic-store.oss-cn-beijing.aliyuncs.com/img/20250509170610.png" alt="image-20250509170610325"  />



>  删除 Canary 资源后，Flagger 自动删除创建的 Deployment、VirtualService 和 DestinationRule 资源。
>
> Flagger pod日志会打印整个过程。

**Flagger 如何处理已有资源 **

参考 Flagger 最新的v1.41.0 版本 `pkg/router/istio.go` 里的 `Reconcile` 函数，Flagger 总是会创建新的  VirtualService 和 DestinationRule，如果同名的资源已经存在，也会用基于 Canary 资源生成的 spec 对其进行更新。

**HPA 和 replicas**

1. 如果灰度发布的应用绑定了 HPA 资源，即通过 HPA 来对应用做弹性伸缩，Flagger 在灰度发布过程中，不会将用户在 my-app 中设置的 replicas 应用到 my-app-primary 。也推荐这种方式。

2. 如果灰度发布的应用没有绑定 HPA 资源，Flagger 在灰度发布过程中，会将用户在 my-app 中设置的 replicas 应用到 my-app-primary 。

## 蓝绿部署

Flagger 的蓝绿部署仍然是基于 Canary CRD 资源来进行处理。
