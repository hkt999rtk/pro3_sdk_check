# CLAUDE.md

本文件为 Claude Code (claude.ai/code) 在本仓库中工作时提供指引。

> 适用范围:本文件描述 `component/at_cmd/` 下的 **AT command 子系统**。仓库级的构建命令和 SoC 列表见根目录 `CLAUDE.md`。

## 本组件做什么

`component/at_cmd/` 实现了通过 UART/SPI/SDIO/USB 暴露的 AT command shell。它有两种模式,通过 Kconfig (`CONFIG_ATCMD_*`) 选择:

- **Manual Test Mode** (`ATCMD_MANUAL_TEST`) —— 操作者在与 log 共用的 UART 上手动输入 AT 命令。
- **Host Control Mode** (`ATCMD_HOST_CONTROL`) —— 由 host MCU 通过专用传输口(UART / SPI / SDIO / USB CDC-ACM device 或 host)驱动 Ameba。输出经 `out_buffer` 而非 log 路径送出,并提供 "transparent transmission" (TT) 模式,让大块数据绕过命令解析。

Menuconfig 路径是 **`CONFIG SHELL`** 菜单 (`component/at_cmd/Kconfig`):`CONFIG_SHELL` → `CONFIG_SUPPORT_ATCMD` → 二选一 `ATCMD_MANUAL_TEST` / `ATCMD_HOST_CONTROL`,再选各模块开关。大多数文件由 `CONFIG_SUPPORT_ATCMD` 加上一个 feature flag(如 `CONFIG_ATCMD_MQTT`、`CONFIG_ATCMD_WEBSOCKET`)门控。`CMakeLists.txt` 是"哪个文件在哪个配置下编译"的权威来源 —— 新增源文件前先读它。

**命令长度上限**(超出部分被截断,而非报错):Manual Test Mode 单行上限 **127 字节**(shell 输入缓冲);Host Control Mode 上限 **2000 字节** (`MAX_CMD_LEN`, `at_intf_usbh.c:32`)。转义序列 (`\,`、`\\`) 每个算 2 字节。2000 字节模式在 **AmebaGreen2 上不支持**。

## 架构

### 命令分发(核心技巧)

每个 AT feature 文件用 `ATCMD_APONLY_TABLE_DATA_SECTION` 宏声明一个静态 `log_item_t[]` 表,放进专门的 linker section:

```c
ATCMD_APONLY_TABLE_DATA_SECTION
const log_item_t at_sys_items[] = {
    {"+TEST", at_test},
    ...
};
```

运行时 `atcmd_action()` (`atcmd_parser.c:17`) 遍历连续区间 `__cmd_table_start__ .. __cmd_table_end__`(linker 符号,在各 SoC 的 `component/soc/<soc>/project/.../ld/*.ld` 中定义),匹配解析出的命令关键字。新增一个命令只需:定义 handler、在该文件的 `at_*_items[]` 里加一项,section 宏自动完成注册。**不要调用任何显式的 "register" 函数** —— 注册纯粹是 linker 把结构体放进 section 完成的。

一个重要后果:这同一个 section 与 shell monitor 的 `COMMAND_TABLE`(如 `monitor.c:419`)共用,所以 `log_item_t` 与 `COMMAND_TABLE` 必须保持 ABI 兼容(`{const char*, fn_ptr}`)。

### 顶层流程

1. `atcmd_service_init()` (`atcmd_service.c:714`) 由系统初始化调用。它逐项执行 `log_init_table[]`(各 feature 的 init 函数,如 `at_wifi_init`、`at_mqtt_init`)并创建 `at_printf_mutex`。在 Host Control Mode 下还会启动 `atcmd_host_control_mode_init_thread`。
2. 传输层 (`at_intf_*.c`) 把命令字节读入行缓冲,调用 `atcmd_service(line_buf)`(在 `at_intf_uart.c` 等中声明为 `extern`)。
3. `atcmd_handler(cmd)` (`atcmd_parser.c:35`) 校验 `AT` 前缀,按 `=` 切分,调用 `parse_param_advance` 解析 argv,再调用匹配到的 action。
4. handler 通过 `at_printf` / `at_printf_indicate`(带 mutex 保护、按模式区分)输出响应,以 `ATCMD_OK_END_STR` 或 `ATCMD_ERROR_END_STR` 结尾。

### 传输接口 (`at_intf_*.c`)

每种传输是单独的文件,由 CMakeLists 中的 Kconfig 选择:
- `at_intf_uart.c` —— host-control 模式下总是编译。默认波特率 **38400** (`u32 UART_BAUD = 38400`, `at_intf_uart.c:33`) —— 区别于 `monitor` 用的 1.5M LogUART。各 SoC 默认 TX/RX/RTS/CTS 引脚在 `atcmd_sys_common.c`;波特率和引脚都会在启动时从 `atcmd_config.json` (`UART_BAUD = baudrate_ob->valueint`, `atcmd_service.c:485`) 或 `atcmd_uart_baudrate` 这个 KV key 覆盖。RTS/CTS 默认关闭;**通过 UART 做 OTA 时必须开硬件流控**。
- `at_intf_spi.c` —— host-control 模式下总是编译。**不只是一个 ringbuffer** —— 它实现了带帧的从机协议:2 字节 `"AT"` magic (`0x41 0x54`) + 2 字节 payload 长度 + payload + 4 字节 `checksum_32_spi` (CRC32),按固定 `ATCMD_SPI_DMA_SIZE`(= `2048*8` = 16384 字节)的 DMA 包传输(payload 上限 = size − 8)。Master↔slave 握手用两个 GPIO(`master_sync_pin` 拉低请求发送;`slave_sync_pin` 在从机就绪/有数据时拉高)—— 从机无法主动发起总线传输,因此 SPI **不使用** TT watermark(见 TT 章节)。
- `at_intf_sdio.c` —— 由 `CONFIG_SUPPORT_SDIO_DEVICE` 门控。
- `at_intf_usbd.c` (CDC-ACM device) 和 `at_intf_usbh.c` (CDC-ACM host) —— 由 `CONFIG_SUPPORT_USB` 门控;在 `CONFIG_SUPPORT_USB_DEVICE_ONLY` 下排除 usbh。`atcmd_config.json` 的 `USB Mode` 字段(`0` = device,`1` = host)必须与 `CONFIG USB` menuconfig(`USB Mode` + `CDC ACM`)一致;AT 设备的角色与 host MCU 的角色相反。

当前生效的传输由 `g_host_control_mode` (`AT_HOST_CONTROL_UART/SPI/SDIO/USB`) 在运行时选择,并持久化在 `VFS_REGION_1` 下的 `wifi_config.json` / atcmd 配置中。

### Transparent Transmission (TT) 模式

当某命令需要传输任意二进制 payload(sockets、websocket 帧、OTA 镜像、文件写入)时使用:

- handler 计算出长度,调用 `atcmd_tt_mode_start(len)`,它输出 `ATCMD_ENTER_TT_MODE_STR` = `">>>\r\n"` 并创建 `atcmd_tt_mode_rx_ring_buf`(上限 `MAX_TT_HEAP_SIZE` = `1024*80` = **80KB**, `atcmd_service.h:47`)。
- 传输层 ISR / RX 任务把收到的字节推入 ringbuffer 并 signal `atcmd_tt_mode_sema`。
- handler 通过 `atcmd_tt_mode_get(buf, len)` 取字节(阻塞;单次取出上限 `MAX_TT_BUF_LEN` = 10KB)。
- **Watermark 背压**只对无流控的传输(UART/USB/SDIO)生效:占用到 `TT_MODE_HIGH_WATERMARK` (0.7) 时输出 `[$][TT]:High Watermark`,降到 `TT_MODE_LOW_WATERMARK` (0.2) 时输出 `[$][TT]:Low Watermark` (`atcmd_service.h:48-49`)。**SPI 从不使用 watermark** —— 它的 sync-pin 握手已能防止溢出。所以 <80KB 的 payload 可自由传;≥80KB 需要 host 遵守 watermark 消息。
- **主动退出**:host 连续发送 **三条内容为单字节 `<` 的消息** (`g_tt_mode_stop_char_cnt >= 3`,各 `at_intf_*.c` 中检查);否则在声明的长度收齐后 TT 自动结束。`atcmd_tt_mode_end()` 销毁 ringbuffer。

状态(`g_tt_mode`、watermark 标志、`g_tt_mode_stop_char_cnt`、stop 标志、timer)在 `atcmd_service.c` 与每个 `at_intf_*.c` 之间共享 —— 改动需谨慎。

### 配置持久化

`atcmd_host_control_config_setting()` 和 `atcmd_wifi_config_setting()` (`atcmd_service.c`) 在启动时从 VFS region 1 加载 JSON 配置文件(`<prefix>:atcmd_config.json`、`<prefix>:wifi_config.json`)。随 SDK 提供的模板是 `tools/image_scripts/atcmd_config.json`;它被(通过 `vfs.py`)预载入 LittleFS VFS1 镜像,因此改传输口/引脚**无需重新编译 app 镜像**。引脚重映射宏:`PIN_VAL_TO_NAME_STR` 与 `atcmd_get_pin_from_json` 在 JSON 字符串(`"PA22"`)与 `_PA_*` 引脚枚举值之间转换。安全证书放在 VFS1 上固定的 `/CERT/` 目录(套件 1–10,命名 `client_*`/`server_*`);命令通过 `<cert_index>` 参数选择某一套。

## 热点与坑

- **超大单文件模块**:`atcmd_sockets.c`(约 2.7k 行)、`atcmd_wifi.c`(约 2.2k)、`atcmd_websocket.c`(约 2k)、`atcmd_mqtt.c`(约 1.7k)。用 `grep -n` 定位并按需读取片段;非必要不要整文件读入。
- **`g_host_control_mode` 与全局缓冲**(`atcmd_wifi.c` 中的 `global_buf`、`pin_name`、`password[129]`)在各命令间无锁共享。并发 AT 命令可能竞争 —— 写新 handler 时务必留意。
- **Host 与 device 构建矩阵**:大多数文件在 CMakeLists 里包在 `if(CONFIG_WHC_HOST OR CONFIG_WHC_NONE)` 中。WHC = "WiFi Host Communication";在仅 device 的构建(如 inic device)中只有 `atcmd_service.c`、`atcmd_parser.c`、`atcmd_sys_common.c`、`monitor.c` 参与编译。
- **`at_printf` vs `RTK_LOGS`**:在 Host Control Mode 下 `at_printf` 是真实函数,经 `out_buffer`(当前传输的 TX)送出;在 manual 模式下它宏展开为 `RTK_LOGS(...RTK_LOG_ALWAYS...)`。**绝不要在 ISR 中调用 `at_printf`** —— 它会获取一个 recursive mutex。
- **`parse_param` vs `parse_param_advance`**:前者按 `,` 切分;后者额外处理 `\,` 和 `\\` 转义。分发器用 `_advance`;handler 内部重新解析 argv 时应保持一致。

## 新增一个 AT 命令

1. 选定或新建一个 feature 文件 (`atcmd_<feature>.c`),在 `CMakeLists.txt` 中放到正确的 `CONFIG_*` 守卫下。
2. 定义 handler `void at_<name>(u16 argc, char **argv)`。
3. 在该文件的 `at_<feature>_items[]` 表(前面带 `ATCMD_APONLY_TABLE_DATA_SECTION` 的那个)中加一项。
4. 若该 feature 需要初始化,加一个 `at_<feature>_init` 函数,并在 `atcmd_service.c:75` 的 `log_init_table[]` 中、相应的 `CONFIG_*` 守卫下注册。
5. 用 `at_printf(ATCMD_OK_END_STR)` / `at_printf(ATCMD_ERROR_END_STR, code)` 结尾;异步主动上报用 `at_printf_indicate`(会加 `[$]` 前缀)。

## 相关位置

- 定义命令表 section 的 linker 脚本:`component/soc/<soc>/project/*/ld/*.ld`(搜 `__cmd_table_start__`)。
- 共用同一 section 的 shell monitor 实现:`component/soc/<soc>/app/monitor/ram/shell_ram.c`。
