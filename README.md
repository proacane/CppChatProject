# CppChatProject

基于 C++ / Boost.Asio / gRPC 的多人全栈聊天项目，将服务端拆分为多个独立微服务，并配合一个 Qt 桌面客户端完成端到端联调。
---

## 一、项目简介

本项目实现了一套轻量级的即时通讯后端 + 桌面端示例，核心能力包括：

- 邮箱验证码注册、登录、找回密码
- 好友搜索、好友申请、好友申请审核
- 单聊文本消息收发
- 多 ChatServer 之间的跨服消息转发
- 基于 Redis 的登录态、用户基础信息缓存与负载计数

后端采用微服务架构，按职责拆分如下：

| 服务 | 语言 / 技术栈 | 职责 |
| --- | --- | --- |
| GateServer | C++ / Boost.Beast + Boost.Asio | HTTP 网关，负责注册 / 登录 / 找回密码等 HTTP 接口，并协调下游 gRPC |
| VerifyServer | Node.js / gRPC + nodemailer | 邮箱验证码生成与发送 |
| StatusServer | C++ / gRPC | 登录态校验、为客户端分配负载最小的 ChatServer、签发 token |
| ChatServer | C++ / Boost.Asio (TCP) + gRPC | 维持客户端长连接，处理登录后所有聊天业务，跨服转发通过 gRPC 完成 |
| ChatUi (客户端) | Qt | 桌面端 UI，HTTP 注册 / 登录 + TCP 长连接聊天 |

服务之间通过 gRPC 通信，对外（客户端）只暴露 GateServer 的 HTTP 接口和 ChatServer 的 TCP 端口。

---

## 二、整体架构

```
                         +----------------------+
                         |       客户端          |
                         |  (Qt ChatUi 桌面端)  |
                         +----------+-----------+
                                    |
                 HTTP(注册/登录/找回) |  TCP 长连接(聊天业务)
                                    |
            +-----------------------+---------------------+
            |                                             |
   +--------v---------+                          +--------v---------+
   |    GateServer    |   --gRPC--> VerifyServer |    ChatServer    |
   | (HTTP 网关)      |   --gRPC--> StatusServer | (Asio TCP 服务)  |
   +------------------+                          +--------+---------+
                                                          |
                                          gRPC(跨服转发)  |
                                                          v
                                                 +-----------------+
                                                 |   ChatServer2   |
                                                 | (对端 ChatServer)|
                                                 +-----------------+

公共存储: MySQL(user/apply/friend) + Redis(token / 缓存 / 负载计数)
```

关键点：

1. 客户端注册、登录、找回密码走 GateServer 的 HTTP 接口。
2. 登录成功后 GateServer 通过 StatusServer 分配负载最低的 ChatServer，并返回 host/port + token。
3. 客户端使用返回的 host/port 建立 TCP 长连接，使用 token 在 ChatServer 完成二次登录校验。
4. 单聊消息若发现对方在另一台 ChatServer，则通过 ChatServer 之间的 gRPC（`ChatService`）转发。

---

## 三、核心流程

### 1. 注册流程

参考 [backEnd/GateServer/src/LogicSystem.cpp](backEnd/GateServer/src/LogicSystem.cpp) 中的 `/get_verifycode` 与 `/user_register`。

1. 客户端 POST `/get_verifycode`（带 email）→ GateServer 通过 gRPC 调 `VerifyService.GetVerifyCode`。
2. VerifyServer 生成 4 位验证码（uuid 截断），写入 Redis（key=`code_<email>`，180s 过期），并通过 nodemailer 发送邮件。
3. 客户端 POST `/user_register`（user/email/password/confirm/verifycode）→ GateServer：
   - 校验两次密码一致；
   - 从 Redis 取 `code_<email>` 比对验证码（不存在则视为过期，不一致则视为错误）；
   - 调 `MysqlDao::registerUser` 写入用户表，返回 uid；
   - 注册成功后删除 Redis 中的验证码。

### 2. 找回密码流程

入口 `/reset_pwd`。逻辑与注册类似：先校验 user/email 是否匹配（`checkEmailUserName`），再校验验证码，最后 `updatePassword` 更新密码（同时处理"新密码与原密码相同"的情况）。

### 3. 登录流程

入口 `/user_login`，参考 [backEnd/GateServer/src/LogicSystem.cpp](backEnd/GateServer/src/LogicSystem.cpp)。

1. GateServer 调 `MysqlDao::checkPassword` 校验账号密码，得到 uid。
2. 通过 `StatusGrpcClient` 调 `StatusService.GetChatServer`：
   - StatusServer 依据 Redis 中各 ChatServer 的 `logincount` 选取连接数最少的服务（参考 [backEnd/StatusServer/src/StatusServiceImpl.cpp](backEnd/StatusServer/src/StatusServiceImpl.cpp)）；
   - 生成 UUID token，写入 Redis（key=`utoken_<uid>`），返回 host/port/token。
3. GateServer 把 host/port/token 透传给客户端，客户端据此建立到 ChatServer 的 TCP 长连接。

### 4. ChatServer 二次登录

参考 [backEnd/ChatServer/src/LogicSystem.cpp](backEnd/ChatServer/src/LogicSystem.cpp) 的 `loginHandler`。

1. 客户端连接 ChatServer 后发送 `MSG_CHAT_LOGIN`(1005) 报文，带 uid + token。
2. ChatServer 从 Redis 取 `utoken_<uid>` 比对 token，校验通过后：
   - 取 `ubaseinfo_<uid>` 用户基础信息（Redis 未命中则回源 MySQL 并回写）；
   - 拉取好友申请列表、好友列表回包；
   - 在 Redis 中累加本 ChatServer 的 `logincount`；
   - 写入 `uid_<uid>` → server name，记录用户所在服务以便跨服查找；
   - 通过 `UserMgr` 将 uid 与 session 绑定，便于后续踢人 / 查找。

### 5. 聊天与跨服转发

参考 [backEnd/ChatServer/src/ChatGrpcClient.cpp](backEnd/ChatServer/src/ChatGrpcClient.cpp) 与 `dealChatTextMsg`。

- 收到 `ID_TEXT_CHAT_MSG_REQ`(1017) 后，根据 `uid_<touid>` 在 Redis 中查到对端所在 ChatServer 名。
- 若对端在本地（同一 ChatServer），直接通过 `UserMgr` 找到 session 推送；
- 若对端在另一台 ChatServer，通过预先建好的 gRPC 连接池 `ChatConnectionPool` 调用 `ChatService.NotifyTextChatMsg` 转发。
- 好友申请 / 好友审核同理，分别走 `NotifyAddFriend` 与 `NotifyAuthFriend`。

---

## 四、通信协议设计

### 1. 客户端 ↔ ChatServer 自定义二进制协议

参考 [backEnd/ChatServer/include/const.h](backEnd/ChatServer/include/const.h) 与 [backEnd/ChatServer/src/CSession.cpp](backEnd/ChatServer/src/CSession.cpp)。

每个报文由定长包头 + 变长消息体组成：

```
+--------+--------+-------------------+
| msg_id | msg_len |     msg_data     |
| 2 byte | 2 byte  |   msg_len byte   |
+--------+--------+-------------------+
\______  ______/
       \/
   HEAD_TOTAL_LEN = 4
```

- `msg_id` / `msg_len` 均按网络字节序传输，服务端读取后通过 `network_to_host_short` 转回主机序。
- 报文最大长度 `MAX_LENGTH = 2048` 字节。
- 读取采用 `asyncReadFull` → `asyncReadLen` 的链式异步读，先读满 4 字节包头，解析出 msg_id 与 msg_len 后再读对应长度的消息体。

主要消息 ID（节选自 `MSG_IDS`）：

| 消息 ID | 含义 |
| --- | --- |
| 1005 / 1006 | 登录请求 / 登录回包 |
| 1007 / 1008 | 用户搜索请求 / 回包 |
| 1009 / 1010 | 添加好友请求 / 回包 |
| 1011 | 通知对端"收到好友申请" |
| 1013 / 1014 | 好友审核请求 / 回包 |
| 1015 | 通知对端"好友申请已通过" |
| 1017 / 1018 | 文本聊天消息请求 / 回包 |
| 1019 | 通知对端"收到一条文本消息" |

### 2. gRPC 服务定义

统一在 [backEnd/ChatServer/proto/message.proto](backEnd/ChatServer/proto/message.proto)（各服务各自有一份相同 proto）中定义：

- `VerifyService.GetVerifyCode`：邮箱 → 验证码
- `StatusService.GetChatServer` / `Login`：分配 ChatServer、登录态校验
- `ChatService`：
  - `NotifyAddFriend` / `RplyAddFriend`
  - `NotifyAuthFriend`
  - `SendChatMsg`
  - `NotifyTextChatMsg`

---

## 五、数据存储设计

### 1. MySQL

通过 `MysqlDao` 管理，连接由 `MysqlPool` 维护（带定时心跳 `Select 1` 与超过 1 小时自动重连）。涉及的核心数据结构（参考 [backEnd/ChatServer/include/data.h](backEnd/ChatServer/include/data.h) 与 MysqlDao 接口）：

- `user` 表：账号、密码、邮箱、昵称、性别、头像、描述等
- 好友申请记录（apply）：发送方 uid、接收方 uid、申请描述、状态
- 好友关系：from_uid、to_uid、备注 back

主要 DAO 接口：`registerUser`、`checkEmailUserName`、`updatePassword`、`checkPassword`、`getUser`、`addFriendApply`、`getFriendApplyList`、`authFriendApply`、`addFriend`、`getFriendList`。

### 2. Redis

通过 `RedisMgr` 单例访问，主要 key：

| Key 前缀 | 含义 |
| --- | --- |
| `code_<email>` | 邮箱验证码（180s 过期） |
| `utoken_<uid>` | 登录 token（StatusServer 签发，ChatServer 二次校验） |
| `uid_<uid>` | 用户当前所在 ChatServer 名（跨服路由依据） |
| `ubaseinfo_<uid>` | 用户基础信息缓存（JSON 串，未命中回源 MySQL 并回写） |
| `logincount` (hash) | 各 ChatServer 当前登录连接数，StatusServer 据此选最小负载 |

---

## 六、客户端

客户端 `frontEnd/ChatUi` 基于 Qt 实现，主要承担两类职责：

- 通过 `HttpMgr` 走 GateServer 的 HTTP 接口完成注册、登录、找回密码；
- 通过自定义 TCP 长连接与 ChatServer 交互，承载登录后所有聊天业务。

> 客户端部分不再展开，重点放在后端服务与协议设计上。如需了解 Qt 端的窗口、聊天页、好友列表等 UI 实现细节，请直接阅读 `frontEnd/ChatUi` 下源码。

---

## 七、目录结构

```
.
├── backEnd
│   ├── GateServer        # HTTP 网关（注册 / 登录 / 找回密码）
│   ├── VerifyServer      # Node.js 邮箱验证码服务
│   ├── StatusServer      # gRPC 状态服务，分配 ChatServer + token
│   ├── ChatServer        # C++ TCP 聊天服务（单实例）
│   └── ChatServer2       # 用于演示跨服转发的第二个 ChatServer 实例
└── frontEnd
    └── ChatUi            # Qt 桌面客户端
```

每个 C++ 服务目录结构基本一致：`include/` 头文件、`src/` 实现、`proto/` 生成的 gRPC 代码与 `.proto`、`config.ini` 配置、`main.cpp` 入口、`CMakeLists.txt`。

---

## 八、配置与构建

### 1. 配置

各服务通过自身目录下的 `config.ini` 加载配置（由 `ConfigMgr` 单例解析），关键配置项示例见 [backEnd/ChatServer/config.ini](backEnd/ChatServer/config.ini)：

```ini
[GateServer]
Port = 8080
[VerifyServer]
Host = 127.0.0.1
Port = 50051
[StatusServer]
Host = 127.0.0.1
Port = 50052
[SelfServer]          # ChatServer 自身
Name = chatServer1
Host = 0.0.0.0
Port = 8090
RPCPort = 50055
[PeerServer]          # 对端 ChatServer 列表，用于跨服转发
Servers = chatServer2
[chatServer2]
Host = 127.0.0.1
Name = chatServer2
Port = 50056
[Redis] / [Mysql]     # 公共存储连接信息
```

> ⚠️ 仓库中的 `config.ini` / `config.json` 内含本地或示例性的连接信息（含明文密码 / 邮箱授权码），实际部署请务必替换为自有环境，并避免将敏感配置提交到版本库。

### 2. C++ 服务构建

使用 CMake 构建，参考 [backEnd/ChatServer/CMakeLists.txt](backEnd/ChatServer/CMakeLists.txt)。主要第三方依赖：

- Boost（asio / beast / filesystem / uuid / property_tree）
- jsoncpp
- gRPC + Protobuf
- hiredis
- spdlog
- mysql-connector-c++（`mysqlcppconn`）

> 注：`ChatServer/CMakeLists.txt` 中 MySQL Connector 的 include / lib 路径写死为 Windows 路径示例，跨平台构建时需自行替换。

### 3. VerifyServer 构建

进入 `backEnd/VerifyServer`，`npm install` 后 `node server.js` 即可启动（gRPC 监听 `127.0.0.1:50051`）。

---

## 九、启动顺序建议

依赖外部组件：MySQL、Redis。就绪后按以下顺序启动：

1. `VerifyServer`（验证码服务）
2. `StatusServer`（状态服务，需先于 ChatServer 启动以提供 `GetChatServer`）
3. `ChatServer` / `ChatServer2`（聊天服务，会向 Redis 写入初始 `logincount=0`）
4. `GateServer`（HTTP 网关）
5. `ChatUi` 客户端

---

## 十、已知限制

本项目目前存在若干限制：

- 报文最大长度仅 2048 字节，不支持大消息（如长文本、图片附件）的分片传输；
- 客户端断线重连、消息可靠投递（ACK / 离线消息）等生产级特性未完整实现；
- `config.ini` 中包含明文密码与邮箱授权码，部署前需替换；
- `ChatGrpcClient::GetBaseInfo` 等部分接口存在 `TODO` 占位实现，未完整落地；
- 跨服路由依赖 Redis 中的 `uid_<uid>` 记录，Redis 故障或键过期会影响转发正确性。

---
