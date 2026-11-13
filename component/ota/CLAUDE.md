# CLAUDE.md

本文件为 Claude Code（claude.ai/code）在本仓库中工作时提供指引。

## 模块概览

`component/ota/` 是 ameba-rtos 的固件 OTA（Over-The-Air，空中升级）引擎。它从服务器（或本地源）下载打包好的固件文件，解析其镜像头，并将镜像编程写入**非激活**的 flash slot。模块分为三层：

- **Core 层**（`ota_core.c`）—— 下载状态机：解析头、处理数据包、编排 flash 编程、上报进度。
- **Storage 层**（`ota_storage.c`）—— flash 地址解析、扇区擦/写/读、checksum 校验、manifest 更新。
- **Transport 层**（`transport/`）—— 可插拔的数据源：HTTP、HTTPS、VFS（本地文件）以及 USER（调用方提供回调）。

公开 API 在 `include/ota_api.h`；跨层共享类型与内部 API 在 `include/ota_internal.h`。

> **遇到本文未覆盖或拿不准的地方，优先用 `realmcu-ask-ai-docs` MCP 查最新的 OTA 文档知识**（该 MCP 检索官方文档，是文档类问题的首选）；查代码可配合 CodeGraph，构建/烧录可用 ameba-dev MCP。若 MCP 不可用，再回退到官方 OTA 文档：<https://aiot.realmcu.com/zh/latest/rtos/ota/index.html>。镜像格式、版本号与切换策略等细节以官方文档的最新说明为准，不要凭本文档的概括臆断。

## 目录结构

```
component/ota/
├── include/
│   ├── ota_api.h        # 公开 API（ota_init/start/deinit + register_* 回调）
│   └── ota_internal.h   # 共享类型 + storage/transport 内部 API + 可调参数
├── ota_core.c           # 下载状态机、头解析、固件编程
├── ota_storage.c        # flash slot 寻址、擦/写/读、checksum、manifest
├── transport/
│   ├── ota_transport.c  # Transport 分发器（按 ctx->type 分支）
│   ├── ota_http.c       # 纯 HTTP
│   ├── ota_https.c      # 基于 mbedTLS 的 HTTP
│   └── ota_vfs.c        # 从 VFS 文件读取镜像
├── Kconfig
└── CMakeLists.txt
```

## 公开 API 流程

调用方通过 `ota_context_t`（见 `ota_internal.h`）驱动一次升级：

```c
ota_context_t ctx;
ota_init(&ctx, host, port, resource, OTA_HTTP);   // 分配 otaCtrl + otaHdrManager，初始化 storage/transport
ota_register_progress_cb(&ctx, my_progress_cb);   // 可选
ota_start(&ctx);                                   // 连接 → 下载 → 编程 → 校验 → 切换 manifest
ota_deinit(&ctx);                                  // 释放 ota_init/transport 分配的所有资源
```

`ota_init` 会清零 context、设置 `type`，随后调用 `ota_storage_init()` 与 `ota_transport_init()`，并分配 `otaCtrl`（`ota_download_ctrl_t`）和 `otaHdrManager`（`ota_hdr_manager_t`）。任一步失败都会调用 `ota_deinit` 并返回 `OTA_ERR`。返回码为 `OTA_ERR (-1)`、`OTA_OK (0)`、`OTA_FINISH (1)`。

### 两种驱动模式

引擎支持两种互斥的驱动方式，**不可混用**：

| 模式 | 流程 | 数据形态 |
|---|---|---|
| transport 自驱 | `ota_init → ota_start（内部循环 read）→ ota_deinit` | 完整 OTA 文件（`[文件头][子头×N][manifest+镜像×N]`），由 `ota_core_parse_header` 解析头 |
| 调用方喂数据（push） | `ota_init → ota_set_image_info → ota_write × N → ota_finish → ota_deinit` | **已剥头的单镜像 payload**，首块以 `ota_manifest_t` 开头 |

push 模式用于 BLE GATT write、串口 DMA 等由调用方持有数据源的场景：

```c
ota_init(&ctx, NULL, 0, NULL, OTA_USER);
ota_set_image_info(&ctx, OTA_IMGID_APP, img_len);   // 提供 ImgID + 总长(含 manifest)，定位 slot 并做 layout 校验
/* 每收到一片： */  ota_write(&ctx, data, data_len); // 喂剥头 payload；失败返回 OTA_ERR 应中止
/* 可选校验：   */  ota_read(&ctx, off, buf, len);   // finish 前回读已写内容（开头部分在 RAM：NOR 一个 page、NAND 整个首块，其余在 flash）
/* 传输完成：   */  ota_finish(&ctx);                // 收完判断 + 最后写 manifest 提交
ota_deinit(&ctx);
```

push 模式的关键约束：

- **`ota_write` 前必须先 `ota_set_image_info`**；它构造单镜像 `FileImgHdr[0]`（`ImgID`/`ImgLen`/`Signature="OTA"`）、`ValidImgCnt=1`，并用 `ota_storage_check_layout` 校验长度能否放进目标 slot。当前**只支持单镜像**。
- **回读校验（`ota_read`）**：`ota_finish` 之前，调用方可用 `ota_read(ctx, offset, buf, len)` 把已写入的固件内容读回来自行校验（如算 hash）。`offset` 以镜像起点为 0（即 manifest 首字节）。`ota_finish` 前镜像的第一个擦除块（NOR 为一个 page，NAND 为一整块）只缓存在 RAM（`head_buf`），尚未落 flash；最后一个不满一页的尾巴还在 `page_buf`。`ota_read` 调用 `ota_storage_read_image` 透明地拼接这三处来源（RAM 首块、flash、`page_buf` 尾部），NAND 上按坏块映射定位；开头 24 字节的真 manifest 由 `ota_read` 用 `otaHdrManager->Manifest[Index]` 覆盖。`offset+len` 越过 `ImageLen` 返回 `OTA_ERR`。须在 `ota_write` 之后（`IsDnldInit==1`）调用。
- **不解析 OTA 文件头**：`ota_write` 直接走 `download_packet_process`，绕开 `ota_core_parse_header`。因此喂进来的数据必须**已剥掉** `[文件头][子头×N]`，首块从 manifest 开始。`ota_write` 会做防御性检查——若首块在 `OTA_HEADER_LEN` 偏移处仍能识别出子头 `"OTA"` 签名，判定为未剥头的完整文件包并返回 `OTA_ERR`。
- **不做 checksum**：镜像完整性由调用方在 `ota_finish` 前自行校验；`ota_finish` 仅检查 `RemainBytes` 是否收完，然后写 manifest 提交。掉电安全仍靠「manifest 最后写」保证。
- **提前中止**：未调 `ota_finish` 直接 `ota_deinit` 即可，该 slot 的 manifest 未提交、不可引导。

### Transport 类型（`ota_type_t`）

| 类型 | 数据源 | 说明 |
|---|---|---|
| `OTA_USER` | 调用方回调 | 注册 `user_read/open/close_func`；与具体传输无关 |
| `OTA_HTTP` | 普通 socket | `ota_http.c` |
| `OTA_HTTPS` | mbedTLS | `ota_https.c`，双向 TLS 需 `ca_cert`/`client_cert`/`private_key` |
| `OTA_VFS` | 本地文件 | `ota_vfs.c` |

使用 `OTA_USER` 时，read/open/close 函数须在 `ota_init` **之后**、`ota_start` 之前注册。`yield_func`（可选，所有类型通用）会在接收循环每次迭代时被调用一次，使长时间下载能够协作式让出 CPU。

## 固件文件格式

下载得到的文件结构为 `[文件头] [子头 × N] [镜像 + manifest × N]`：

- `ota_hdr_t`（`OTA_HEADER_LEN` = 8）：`FwVer`、`HdrNum`（镜像数量，必须 ≤ `OTA_IMGID_MAX`）。
- 每个镜像一个 `ota_sub_hdr_t`（`OTA_SUB_HEADER_LEN` = 24）：`Signature`（必须为 `"OTA"`）、`ImgHdrLen`、`Checksum`、`ImgLen`、`Offset`、`ImgID`。
- 每段镜像负载以一个 `ota_manifest_t`（即 `AuthHeader_TypeDef` 的别名，来自 `ameba_secure_boot.h`）开头。

`ota_core_parse_header` 会跨数据包边界重组头（通过 `NextImgBuf` 在上一包尾部与下一镜像之间携带残留字节），校验签名，并在任何 flash 写入之前调用 `ota_storage_check_layout`。

## 关键：掉电安全的编程顺序

`download_packet_process` 刻意**将镜像开头部分（含 manifest）缓存在 RAM 中、最后才写入**（NOR 缓存 page 0，NAND 缓存整个首块）——先把其余镜像体写进 flash，随后 `download_finish` 运行 `ota_storage_verify_checksum`，校验通过后才执行 `ota_storage_update_manifest`。若下载中途掉电，该 slot 的 manifest 仍为无效状态，bootloader 不会去启动一个写了一半的镜像。**不要把 manifest 的写入提前到镜像体之前，也不要在 checksum 通过之前写 manifest。**

## 双 Slot 切换

OTA 总是写**非激活**的 slot。`download_parameter_init` 据此挑选目标：`ota_get_cur_index(ImgId) == OTA_INDEX_1 ? OTA_INDEX_2 : OTA_INDEX_1`。flash 地址来自 `IMG_ADDR[OTA_IMGID_MAX][2]` 表，该表在 `ota_storage_init` 中经 `flash_get_layout_info` 填充（`IMG_BOOT`、`IMG_APP_OTA1/2`，以及 `CONFIG_LINUX_FW_EN` 下的 `IMG_APIMG_OTA1/2`）。

重启后 bootloader 选用哪个 slot 由 Kconfig（`Kconfig`）决定：
- `OTA_SWITCH_BY_VERSION`（默认）—— 启动版本号较高的镜像；两个 slot 都保留以便回滚。
- `OTA_SWITCH_BY_VALID_HEADER` —— 使旧镜像的头失效以强制启用新镜像（不可回滚）。
- `UPGRADE_BOOTLOADER` —— 同时 OTA bootloader（当 OTA2 bootloader 所在 slot 无效时，运行时会通过 `ota_check_bootloader_ota2()` 跳过，见 `SkipBootOTAFg`）。
- `COMPRESS_OTA_IMG` —— 支持压缩的 APP 镜像。

## 编译门控（重要）

本组件**并非总会被编译**。`CMakeLists.txt` 仅在 `CONFIG_OTA_ENABLED` 为 y 时构建，否则直接 `return()`，不产出 `lib_ota`。

`OTA_ENABLED` 定义于 `component/ota/Kconfig`（`def_bool y depends on WHC_HOST || WHC_NONE || (WHC_DEV && WHC_DEV_FLASH_BOOT)`），由各 SoC project Kconfig 通过 `rsource` 引入，不需要手动配置。它是开源组件，通过 `ameba_add_internal_library(ota ...)` 从源码构建。

另外，`ota_transport.c` 内部的 HTTP/HTTPS connect/read/disconnect 分支被 `#ifdef CONFIG_WHC_INTF_IPC` 包裹。改动 transport 分发逻辑时，须保证 `ota_transport_connect`、`ota_transport_read`、`ota_transport_disconnect` 三者的这些条件编译保持一致，否则某种 transport 类型会在一条路径里编译进来、却在另一条路径里被丢掉。

## 本模块编码约定

- **日志**：使用 `RTK_LOGI/W/E(OTA_TAG, ...)`，并带上与文件对应的 `[LAYER]` 前缀（`[CORE]`、`[STORAGE]`、`[TRANSPORT]`）。格式字符串风格与上下文保持一致。
- **内存**：使用 `rtos_mem_malloc/zmalloc/free` 包装器（声明于 `ota_internal.h`），绝不直接用 `malloc`。`ota_init`/`ota_transport_init` 中的每一次分配，在 `ota_deinit` 中都有对应的释放——新增 context 字段时要保持这种对称。
- **接收缓冲**：下载只用一个 `OTA_BUF_SIZE`（2048）缓冲区；状态机被设计成能容忍任意的数据包边界。不要假设一个数据包恰好包含一个完整的头或完整的镜像。
- **`_memcpy`/`_memset`**：现有代码使用这些 ROM 辅助函数；模块内沿用即可。

## 开发注意事项 / 易错点

- **物理地址 vs flash 偏移。** `IMG_ADDR[][]` 以及 `ota_storage_verify_checksum` 中直接解引用（`*(u8*)addr`）所用的地址都是**物理地址**（含 `SPI_FLASH_BASE`）。而 `ota_storage_write/read/erase_sector` 期望的是 **flash 偏移**（物理地址 − `SPI_FLASH_BASE`）。Core 层将 `otaCtrl->FlashAddr` 保存为偏移，`ota_storage_update_manifest` 在写入前会减去 `SPI_FLASH_BASE`。操作 flash 时务必清楚手里拿的是哪一种——混用会静默地写到错误位置。

- **下载引擎是单实例、非可重入的。** `download_packet_process` 与 `download_finish` 通过 **`static` 局部变量**保存跨包状态（`write_sector`、`next_erase_sector`、`size`、`manifest`、`percent_bak`）。同一时刻只能跑一个 OTA，这些变量在每个镜像开始时经 `otaCtrl->IsDnldInit == 0` 分支重新初始化——不要挪动该复位逻辑，也不要并发跑两个 `ota_start` 流程。

- **manifest 写入本身也有掉电安全的顺序。** 在 `ota_storage_update_manifest` 内部，先写 manifest 主体（`+8` 偏移处），最后才写 8 字节的 `Pattern`（bootloader 的有效性标记），作为伪原子提交。在 `CONFIG_OTA_SWITCH_BY_VALID_HEADER` 下，随后会把*旧* slot 的 pattern 清零以强制使用新镜像。请保持「先主体、后 pattern」的顺序。这只适用于 NOR。NAND 的同一 block 内必须按页地址升序编程，且一页只能编程一次，因此 NAND 把整个首块（page 0..N）缓存在 RAM，`update_manifest_nand` 在 checksum 通过后按 page 0、1、2… 顺序写出；提交窗口是「写整个首块」而不是「写一页」，中途掉电会留下 page 0 有效、后面页缺失的 slot，该 slot 能否被拒绝取决于加载方是否校验镜像：APP 由 bootloader 校验；Bootloader slot 由 ROM 加载，未开 secure boot 时 ROM 只检查镜像头 pattern 和地址范围、不做整镜像 hash，缺页只能靠 boot count 回退到另一个 slot（前提：旧 slot 的 header 仍有效）。因此 NAND 复位测试只覆盖 APP 镜像。

- **checksum 是逐字节累加和，不是 CRC。** `ota_storage_verify_checksum` 把 manifest 字节与镜像体字节相加，再与 `FileImgHdr.Checksum` 比较。它必须与主机端打包工具的计算方式完全一致——不要只在一边把它「升级」成 CRC。

- **layout 检查依赖初始化顺序。** `ota_storage_check_layout` 读取 `IMG_ADDR`，而该表只由 `ota_storage_init`（在 `ota_init` 中调用）填充。Core 层已按正确顺序调用；重构时请保持这一顺序。

- **日志格式符因宏而异。** 本模块使用带 tag 的 `RTK_LOGI/W/E`，可接受 `%lu`/`%lx`（见现有调用）。而项目级的 `RTK_LOGS` 变体**不支持**——它只支持 `%d %u %s %08x %c`。若你在此处新增 `RTK_LOGS` 行，请限制在该子集内，并对 `%08x` 做 `(unsigned int)` 强转，与现有写法一致。两者都不支持浮点。

- **`ota_init` 会清零 context。** 它会 `_memset` 整个 `ota_context_t`，因此 `type` 要通过 `ota_init` 的参数设置，回调（`ota_register_*`）须在 `ota_init` **之后**注册，绝不能在之前——更早设置的内容都会被清掉。

- **重启是调用方的责任。** `ota_start` 只负责编程与校验，不会复位；何时 `sys_reset()` 由应用决定（见示例）。`ota_start` 成功仅意味着非激活 slot 已有效。

- **大缓冲区不要放栈上。** 2 KB 接收缓冲在堆上（`rtos_mem_malloc`），`NextImgBuf[OTA_BUF_SIZE]` 也位于堆分配的 `otaCtrl` 之中。新增临时缓冲同样要避开栈——OTA task 的栈不大（示例中为 4 KB）。

- **代码风格须与 OTA 现有代码保持一致**（提交前 pre-commit 的 AStyle / CppCheck 也会检查）：
  - **缩进用 Tab**，不要用空格。
  - **大括号 K&R 风格**：`{` 跟在语句同一行，`} else {` 写在一行，所有控制语句即使单条也带大括号。
  - **函数命名** `ota_<层>_<动作>` 全小写下划线（如 `ota_core_recv_header`、`ota_storage_write`）；模块内部辅助函数用 `static`。
  - **类型** 用 `u8`/`u16`/`u32`/`int`，类型别名以 `_t` 结尾（如 `ota_context_t`、`ota_download_ctrl_t`）。
  - **结构体字段沿用现有混合命名**（如 `FwVer`、`FlashAddr`、`ValidImgCnt`、`slotIdx`、`IsDnldInit`、`otaCtrl`）——给现有结构体加字段时跟随该结构体的既有风格，不要另起一套命名。
  - **函数注释用 Doxygen 块**（`/** @brief ... @param ... @retval ... */`），与现有头/源文件一致。
  - 每个新文件顶部加 `Copyright ... SPDX-License-Identifier: Apache-2.0` 版权头。

## 示例

参考应用位于 `example/ota/`，每种 transport 类型一个。每个都是自包含示例（`app_example.c`、`example_ota_<type>.c/.h`、`CMakeLists.txt`、`README.md`），会创建一个 `ota_task`，依次执行 `ota_init → ota_start → ota_deinit`，成功后 `sys_reset()`。

| 示例 | 类型 | 入口 | 说明 |
|---|---|---|---|
| `example/ota/ota_http/` | `OTA_HTTP` | `example_ota_http()` | 等待 STA 联网，从 HTTP 服务器下载 `ota_all.bin`；在 `example_ota_http.c` 中修改 `HOST`/`PORT`/`RESOURCE` |
| `example/ota/ota_https/` | `OTA_HTTPS` | `example_ota_https()` | 同样流程，走 mbedTLS |
| `example/ota/ota_vfs/` | `OTA_VFS` | `example_ota_vfs()` | 从 VFS 路径读取镜像（`"<prefix>:/ota_all.bin"`）；不使用 `host`/`port`（传 `NULL`/`0`） |
| `example/ota/ota_user/` | `OTA_USER` | `example_ota_user()` | 演示回调路径：`ota_init(ctx, NULL, 0, NULL, OTA_USER)` 后再 `ota_register_user_read_func` / `ota_register_user_open_func` / `ota_register_user_close_func` |

标准范式（取自 `ota_http`）：
```c
ctx = rtos_mem_malloc(sizeof(ota_context_t));     // ctx 由调用方分配
memset(ctx, 0, sizeof(ota_context_t));
ota_init(ctx, HOST, PORT, RESOURCE, OTA_HTTP);
ota_start(ctx);                                   // 成功返回 OTA_OK
ota_deinit(ctx);                                  // 释放内部资源（不含 ctx 本身）
rtos_mem_free(ctx);                               // ctx 由调用方释放
```
注意：`ota_deinit` 释放的是 `ota_init` 在 context **内部**分配的资源；`ctx` 结构体本身由调用方持有并释放。

编译示例（在仓库根目录执行）：
```bash
./ameba.py build -a ota_http     # 这里是 example/ota/ 下的 <dir_name>，不是完整路径
```
示例的 README 记录了 HTTP 下载服务器的搭建（`tools/DownloadServer(HTTP)`）以及镜像切换策略的选择。改动示例前先阅读其自带 README。

## 新增一种 Transport

1. 在 `ota_internal.h` 的 `ota_type_t` 中增加一个枚举值。
2. 在 `transport/ota_<name>.c` 中实现 `ota_<name>_open/read/close`，并在 `ota_internal.h` 中声明。
3. 把该源文件加入 `CMakeLists.txt` 的 `private_sources`。
4. 在 `ota_transport.c` 的 `ota_transport_connect/read/disconnect` 中增加 `case OTA_<NAME>:` 分支（注意 `CONFIG_WHC_INTF_IPC` 的条件编译）。
5. Core 层无需改动——它只调用 `ota_transport_*`。
