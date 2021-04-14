# 仿CTP行情前置系统项目

## 一、项目实现目标

本仿CTP行情前置系统（下简称为**CMFS**）用于替换速度较慢的ctp行情前置， 在兼容ftdc协议的基础上， 将内部极速行情进行转发， 并提供鉴权功能

1. 功能
   + 仿CTP行情API协议, 实现接口完全兼容
   + 根据后端存储的客户号进行鉴权
   + 实现订阅接口的合约模糊匹配订阅
2. 性能
   + 单前置1000用户全量订阅
   + 转发速度优于CTP前置

## 二、功能列表


| 功能 | 对应api方法 | 描述 |
| - | - | - |
| 获取交易日 | GetTradingDay | 通过api实例查询交易日信息 |
| 初始化连接 | Init | 通过tcp的ftdc协议连接到CMFS |
| 客户登录 | ReqUserLogin | 客户登录， 提供客户号校验 |
| 客户登出 | ReqUserLogout | 客户登出 |
| 行情订阅 | SubscribeMarketData | 提供行情订阅接口， 可根据合约号模糊订阅 |
| 行情退订 | UnSubscribeMarketData | 提供行情退订接口， 可根据合约号模糊取消订阅 |
| 实时行情推送 | OnRtnDepthMarketData | 行情实时推送接口 |

## 三、数据流图

todo  待补充

## 四、工作计划

0. 调研阶段  （ftdc协议研究， 架构设计与讨论）
1. ftdc协议实现
2. 自有行情源平台接入
3. 功能编码完整实现与单元测试
4. 性能测试与调优
5. 整体测试与验证
6. 客户试用与预上线

![进度计划](./doc/images/进度计划gantt.png)

## 五、 架构设计
   见[架构设计文档](./doc/CMFS架构设计.md)


## 六、 代码结构

```
├── CMakeLists.txt     
├── compile.sh
├── doc           【设计文档】
├── Dockerfile
├── README.md
├── references    【参考文档】
├── dependencies  【三方依赖库】
│   ├── include
│   │   ├── asio
│   │   ├── asio.hpp
│   │   ├── gmock
│   │   ├── gtest
│   │   ├── nlohmann
│   │   └── spdlog
│   └── lib
├── include       【头文件】
│   ├── adapter
│   │   ├── CAdapterManager.h
│   │   ├── CCtpMarketAdapter.h
│   │   ├── CMockAdapter.h
│   │   ├── CUDPAdapter.h
│   │   └── IAdapter.h
│   ├── api
│   │   ├── ctp
│   │   │   ├── ThostFtdcMdApi.h
│   │   │   ├── ThostFtdcTraderApi.h
│   │   │   ├── ThostFtdcUserApiDataType.h
│   │   │   └── ThostFtdcUserApiStruct.h
│   │   ├── ctp_ext
│   │   │   ├── ctp_ext.h
│   │   │   └── ctp_ftdc_proto.h
│   │   └── udp_market
│   │       ├── GtjaMdStructV3.h
│   │       └── GtjaMdUserApi.h
│   ├── apiWrapper
│   │   └── ctp
│   │       ├── MyCtpApi.h
│   │       ├── MyCtpSpi.h
│   │       ├── MyMarketApi.h
│   │       └── MyMarketSpi.h
│   ├── bizData
│   │   ├── CMarketDataManager.h
│   │   └── CUserSessionManager.h
│   ├── CConfig.hpp
│   ├── CLog.h
│   ├── core   
│   ├── ds
│   │   ├── bizDataStruct.h
│   │   ├── bizDataType.h
│   │   ├── CDefine.h
│   │   └── common.h
│   ├── Init.h
│   ├── interface
│   │   ├── IPlugin.h
│   │   └── IQueue.h
│   ├── plugin
│   │   ├── CMarketPlugin.h
│   │   └── CUserManagePlugin.h
│   ├── tcp
│   │   ├── CTcpServer.h
│   │   └── CTcpSession.h
│   └── utils
│       ├── nettrans.h
│       ├── ProcSingletonCheck.h
│       └── Utils.h
├── src           【源码目录】
│   ├── adapter
│   │   ├── CAdapterManager.cpp
│   │   ├── CCtpMarketAdapter.cpp
│   │   ├── CMockAdapter.cpp
│   │   └── CUDPAdapter.cpp
│   ├── apiWrapper
│   │   └── ctp
│   │       ├── MyCtpApi.cpp
│   │       ├── MyCtpSpi.cpp
│   │       ├── MyMarketApi.cpp
│   │       └── MyMarketSpi.cpp
│   ├── bizData
│   │   ├── CMarketDataManager.cpp
│   │   └── CUserSessionManager.cpp
│   ├── core
│   │   └── semaphore.cpp
│   ├── main.cpp
│   ├── plugin
│   │   ├── CMarketPlugin.cpp
│   │   └── CUserManagePlugin.cpp
│   └── tcp
│       ├── CTcpServer.cpp
│       └── CTcpSession.cpp
├── test          【Gtest测试】
│   ├── InstrumentQryProcess.cpp
│   ├── test_base_qry.cpp
│   ├── test_base_qry.h
│   ├── test_cache.cpp
│   ├── test_marketdata.cpp
│   ├── test_md_sub.cpp
│   ├── test_md_sub.h
│   ├── test_suit.h
│   ├── test_suit_md.h
│   └── test_suit_trade.h
```

## 七、 ReleaseNotes

   见[发布说明](./doc/发布说明.md)


## 八、 后续计划
- 兼容 股票期权api
- 张江机房上线五所行情版本前置(需要申请操作员账号以及流量测算)

## 九、 参考文档
   + [FTD交易数据交换协议](./references/期货交易数据交换协议_FTD协议.pdf)
   + [ftd协议解析总结](./references/ftd协议解析.pdf)

