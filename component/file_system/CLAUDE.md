# CLAUDE.md

本文件为 Claude Code (claude.ai/code) 在本仓库中工作时提供指引。

## 适用范围

本文档只覆盖 `component/file_system/`。仓库级的构建命令和 SoC 布局见 `/home/liang_cai/rtos_repo_master/CLAUDE.md`。

## 子树布局

```
file_system/
├── vfs/        # 中央抽象层 + 存储介质后端 (NAND FTL, SD-SPI, second-flash, USB host adapter 胶水代码)
├── littlefs/   # 内置 LittleFS r2.50 + littlefs_adapter.c (NOR / NAND / second flash 的 lfs_config)
├── fatfs/      # 内置 FatFs r0.14b + flash/SD/USB-host 的 disk_io adapter
├── kv/         # key-value 存储,以 "<prefix>:KV/<key>" 形式的文件实现
└── ftl/        # 基于 VFS+LittleFS 的 BLE 协议栈 flash-transport-layer 仿真 —— 不是 NAND FTL
```

两个 **`*_VER`** 占位符是刻意的间接层,使内置库可以升级版本而无需改动 adapter:
- `${v_LITTLEFS_VER}` → `r2.50`
- `${v_FATFS_VER}` → `r0.14b`

## 构建门控(顶层 `CMakeLists.txt`)

```cmake
if(CONFIG_VFS_ENABLED)
    ameba_add_subdirectory(littlefs)
    ameba_add_subdirectory(kv)
    ameba_add_subdirectory(vfs)
    ameba_add_subdirectory_if(CONFIG_BT ftl)
endif()
ameba_add_subdirectory_if(CONFIG_VFS_FATFS_INCLUDED fatfs)
ameba_add_subdirectory_if(CONFIG_VFS_REALFS_INCLUDED realfs)
```

含义:
- `CONFIG_VFS_ENABLED` 定义于各 SoC 的 `project/Kconfig`,为 `def_bool y depends on !MP_SHRINK && (WHC_HOST || WHC_NONE || (WHC_DEV && WHC_DEV_FLASH_BOOT))`。对 WiFi-host / 独立构建,**LittleFS 始终开启**;FatFs / REALFS 是可选的。
- 整个 VFS 核心子树在 **WHC_DEV + IPC 构建**或 **MP_SHRINK** 下不参与编译(IPC 构建通过 WHC 依赖 host CPU 的文件系统)。
- `ftl/` 需要 `CONFIG_BT` —— 它是 BLE 配对/参数存储,不是 NAND FTL。

Kconfig 按 **target** 组织:先选卷,再在卷下选它用哪种 FS(`Kconfig`):
- **平台门控**:`CONFIG_VFS_ENABLED` 定义于各 SoC 的 `project/Kconfig`;WHC_DEV + IPC 和 MP_SHRINK 下不编译 VFS 核心。
- **片上 flash(开机由 `vfs_auto_register()` 注册)**:`VFS1_LITTLEFS`(VFS1 目前只开放 LittleFS;`VFS1_REALFS` 是隐藏符号、恒为 n,待 KV/FTL 适配 REALFS 后再放回 choice,`.conf` 里写它会让配置应用报错);`VFS2_NONE`(默认)/`VFS2_FATFS`(`VFS2_LITTLEFS` 因与 VFS1 共用 LittleFS 实例,要求 `VFS1_REALFS`,在 REALFS 放开前不可选);只读的 `FATFS_WITHIN_APP_IMG`、`LITTLEFS_WITHIN_APP_IMG`(沿用原符号名,postbuild.cmake 在用)。
- **外部介质(由应用调用 `vfs_target_register()`)**:`VFS_SECOND_FLASH`(+`VFS_SECOND_FLASH_NAND`、FS 三选一 `VFS_SECOND_FLASH_{LITTLEFS,FATFS,REALFS}`)、`VFS_SD_CARD`(+`VFS_SD_FATFS`/`VFS_SD_REALFS`、`VFS_SD_CARD_HOTPLUG`)、`VFS_SD_SPI`、`VFS_USB_HOST`(这两个只能用 FatFs)、`VFS_RAM`(+`VFS_RAM_{LITTLEFS,FATFS,REALFS}`)。
- **全局 FatFs 选项**:`FATFS_READ_ONLY` 影响**所有** FatFs target(去掉 FatFs 写函数以省 code size),此时 FatFs 卷必须以 `VFS_RO` 注册;只想让某一个卷只读,不要开它,注册时传 `VFS_RO`。`FATFS_SUPPORT_EXFAT` 同为全局。

**`.conf` 的书写顺序:** `setconfig.py` 按行依次赋值,**父开关必须写在子选项之前**(如 `CONFIG_VFS_SD_CARD=y` 在 `CONFIG_VFS_SD_FATFS=y` 之前;`VFS_SECOND_FLASH`/`VFS_RAM` 同理),否则子项被判"依赖未满足"而整个配置应用失败。

**派生符号**:`VFS_LITTLEFS_INCLUDED`/`VFS_FATFS_INCLUDED`/`VFS_REALFS_INCLUDED`,以及 `LITTLEFS_SECOND_FLASH`、`FATFS_SECOND_FLASH`、`FATFS_SD_MODE`、`FATFS_SD_HOTPLUG`、`FATFS_SD_SPI_MODE`、`FATFS_USB_HOST`、`REALFS_FLASH`(仅 VFS1 用 REALFS)、`REALFS_SD_MODE`、`REALFS_SECOND_FLASH_NAND`、`REALFS_RAM` 是**无提示的隐藏 bool**,由上面的 target 选项推出。C 代码的 `#ifdef` 与 CMake 都按派生符号编译,所以没被选中的 FS 什么都不编进来。**不要 `select` 派生符号,也不要 `select` choice 的成员**(Kconfig 不允许);外部 Kconfig 要么 `select` 介质开关(如 `USBH_MSC_MENU` select `VFS_USB_HOST`),要么 `depends on` 某个 FS 选项(见 `usb/device/Kconfig` 的 MSC)。`.conf` 里写 target 选项,不要写派生符号(写了会被静默忽略)。

**同时挂载的冲突**(见 `VFS_TARGET_DESIGN.md` 2.7):VFS1/VFS2 不能同时用 LittleFS(共用 `g_lfs`);VFS2 与 APP 镜像不能同时用 FatFs(共用 `fatfs_flash_param`);REALFS 是单实例,选了 `VFS1_REALFS` 后其它 target 不能再选 REALFS。这几条在 Kconfig 里禁止。外部介质的 FatFs 共用 `fatfs_second_flash_param` + `fatfs2_mount_flag`,同时挂两个会"假成功",由 `vfs_target_register()` 在运行时拒绝。

## 架构(需要读多个文件才能理清的部分)

### 1. VFS 分发表

`vfs/vfs.h` 定义了 `vfs_opt` —— 一个 28 槽的函数表(open/read/write/seek/opendir/...)外加 `mount`/`unmount`/`get_interface`(参数都是 **target**)。存在三个实例:
- `vfs/vfs_littlefs.c` 中的 `littlefs_drv`
- `vfs/vfs_fatfs.c` 中的 `fatfs_drv`
- `vfs/vfs_realfs.c` 中的 `realfs_drv`

`vfs_drv vfs`(单一全局,在 `vfs.c` 中)持有:
- `drv[VFS_FS_MAX]` —— 已注册的 FS 驱动
- `user[VFS_USER_MAX]` —— 已挂载的 "user"(基础 4 个,启用 ROLFS +1、启用 RAM +1),每个把 `(prefix, vfs_type, vfs_target, ro_flag)` 绑定到一个驱动和一个 `fs` 句柄(如 `&g_lfs`、`&g_second_lfs`)。**同一个 target 可以占多个槽(别名)**:共用一个 FS 实例、只 mount 一次,各 prefix 的读写权限可以不同,加密回调按卷统一,最后一个 prefix 注销时才卸载。

生命周期:`vfs_init()`(创建 mutex + NAND FTL 启动)→ `vfs_auto_register()` 按 Kconfig 注册片上 flash 的 target(VFS1、VFS2、APP 镜像里的两个)→ `vfs_kv_init()`/`vfs_ftl_init()`。外部介质由应用在介质就绪后调用 `vfs_target_register("sd", VFS_TARGET_SD, VFS_RW)`:它用 `vfs_target_fs()` 取该 target 的 FS(由 Kconfig 决定,调用者不传),惰性注册驱动,分配槽位,调用 `vfs_target_setup()` 从 flash layout 填充 `LFS_FLASH_BASE_ADDR` / `FLASH_APP_BASE` 等,再 `drv->mount(target)`。注销用 `vfs_target_unregister(prefix)`,只传 prefix。

旧的 `vfs_user_register`/`vfs_user_unregister`/`find_vfs_tag` 以及 `VFS_INF_*`/`VFS_REGION_*` 宏已删除,不保留兼容层;外部代码需改用 target 接口。

### 2. stdio 是怎么重定向的

`vfs/vfs_wrap.c` 定义了 `__wrap_fopen` / `__wrap_fread` / `__wrap_fseek` 等。符号重定向以注释块形式列在文件顶部,用的是 IAR 风格的 `--redirect fopen=__wrap_fopen` 指令(注意:**不是** GCC 的 `--wrap=` —— 构建里任何地方都没有 `--wrap=fopen` 标志;这些 wrapper 只是覆盖了弱的 libc stdio 符号)。它们把 libc stdio 重定向到 wrapper。`__wrap_fopen` 调用 `find_vfs_number(filename, ...)` 扫描 prefix 表 —— **路径中 `:`/`/` 之前的第一段用来选择 user**,所以路径形如:

```
"vfs:foo.txt"           # VFS1(LittleFS),默认 prefix "vfs"
"vfs2:bar/baz"          # VFS2,默认 prefix "vfs2"
"fat1:0:/log.txt"       # FatFs (由 vfs_build_filename 内部重建为 "<drive>:/...")
```

wrapper 在堆上分配一个 `vfs_file*`,存好 `vfs_id`/`user_id`,然后把它强转为 `FILE*` 返回。**在本子树内不要假设 `FILE*` 是一个 libc 句柄** —— 凡是把 `FILE*` 强转回 `vfs_file*` 的地方(见 `ftl/ftl.c` 通过 `lfs_file_t->ctz.size` 取当前文件大小)都依赖这一点。

### 3. Target 语义

`VFS_TARGET_*` 唯一对应一个物理卷,取代旧的 (interface, region) 两个参数。`vfs_target_setup()` 按 target 计算介质参数:
- `VFS_TARGET_FLASH_VFS1`/`FLASH_VFS2` → 片上 flash,base/size 由 `flash_get_layout_info(VFS1/VFS2, ...)`(bootloader 发布的 layout)查询;默认 prefix `"vfs"`/`"vfs2"`,读写
- `VFS_TARGET_FLASH_APP_FATFS` → 搭在 APP 镜像上的 FatFs(仅在 `CONFIG_FATFS_WITHIN_APP_IMG` 时有效);base 地址通过遍历镜像头并匹配 `PATTERN_VFS_1/2` 找到;默认 prefix `"fat1"`,只读;**挂载失败绝不 mkfs**
- `VFS_TARGET_FLASH_APP_LFS` → APP 镜像尾部的只读 LittleFS blob(原 ROLFS);默认 prefix `"rolfs"`,只读
- `VFS_TARGET_SECOND_FLASH`/`SD`/`SD_SPI`/`USBH`/`RAM` → 外部介质。对 second NOR flash,SPI 引脚映射 + 波特率在 `vfs/vfs_second_nor_flash.h`;有两种形态 —— *data flash*(吞吐更高,无 PSRAM)与 *normal external flash*(兼容 PSRAM)。

Sector 数和 LittleFS `block_count` 在 `vfs_target_register` 时计算,而非编译期 —— 同一份固件可挂载不同大小的 flash。

**Layout 约束(来自 flash layout,在 register/mount 时强制):**VFS1 分区**必须存在、必须 ≥128KB**;KV 和 BT FTL 固定使用 VFS1 且依赖其为 LittleFS(`VFS1_REALFS` 时二者不可用,以桩实现链接)。VFS2 可选(`VFS2_NONE` 为默认,很多 layout 里它大小为 0)。片上 VFS 分区最多 **两个**;APP 镜像里的两个 target 和外部设备不是额外的 flash 分区。

**查询 prefix:** 用 `vfs_get_prefix(VFS_TARGET_xxx)`,返回该 target 下标最小的已挂载槽的 prefix,未挂载返回 NULL。不要硬编码 prefix 字符串。

### 4. NAND vs NOR —— 运行时决定,而非编译期

当设置了 `CONFIG_SUPPORT_NAND_FLASH` 时,`vfs_init()` 查询 `SHOULD_USE_NAND()`(== `!SYSCFG_OTP_BootFromNor()`),在 NAND 启动时:
- 调用 `NAND_FTL_Init()` (`vfs/vfs_nand_ftl.c` + `vfs_nand_ftl_mfg.c`)
- 用 `vfs_nand_flash_pagesize`/`vfs_nand_flash_pagenum` **patch** `SECTOR_SIZE_FLASH`/`SECTOR_NUM`/`FLASH_BLOCK_SIZE`(FatFs 用)以及 `g_nand_lfs_cfg.read_size/prog_size/cache_size`(LittleFS 用)。
- 随后 `rt_lfs_init()` 根据 `SHOULD_USE_NAND()` 选 `g_nand_lfs_cfg` 或 `g_nor_lfs_cfg`。

所以*同一个* `g_lfs` 实例会根据运行时 OTP 配成 NOR 或 NAND。**改动 block layout 常量时,要同时审查 `littlefs_adapter.c`(LFS)和 `fatfs/fatfs_flash.c`(FatFs)以及这条 NAND override 路径。** 同样的 `block_size`/`block_count` 还必须与离线 `tools/image_scripts/vfs.py -s/-c` 参数(用于预生成分区镜像)匹配 —— 不一致会导致 mount 失败并静默重格式化(见 Common Pitfalls)。

### 5. "两个 FTL" 的坑

"FTL" 这个名字被重载了。两者毫无关系:

| 路径 | 是什么 | 依赖 |
|------|-----------|------------|
| `component/file_system/vfs/vfs_nand_ftl.c` | 真正的 NAND Flash Translation Layer(page/block/ECC/坏块管理) | `CONFIG_SUPPORT_NAND_FLASH` |
| `component/file_system/ftl/ftl.c` | BLE 配对信息存储;通过读写 VFS1(`VFS_TARGET_FLASH_VFS1`)下的两个 LittleFS 文件(`ftl_for_ble`、`ftl_trace_for_ble`)来仿真蓝牙协议栈的 FTL API (`ftl_save_to_storage(offset,size)`) | `CONFIG_BT` |

grep `ftl_*` 时别把两者混为一谈。

### 6. Mount-flag 模式

每个可挂载后端都有一个 `volatile uint8_t *_mount_flag` 全局变量(`lfs_mount_flag`、`lfs2_mount_flag`、`fatfs_mount_flag`、`fatfs2_mount_flag`)。每个 `__wrap_*` 顶部都会调用 `vfs_check_mount_flag(user_id, op)`(按该槽的 target 选 mount flag),在用户没成功 mount 就调用文件 API 时快速失败并给出清晰错误。**新增后端时,加一个新的 mount flag 并扩展 `vfs.c` 里的 `vfs_mount_flag()` —— 不要绕过它。**

### 7. 加密钩子(按 user)

`vfs_set_user_encrypt_callback(prefix, enc, dec, iv_len)` 把 `(vfs_enc_callback_t, vfs_dec_callback_t, enc_iv_len)` 挂到匹配的 `user[]` 槽上。实际调用点在各 FS 自己的 read/write 路径里 —— 加密是**按卷(target)**生效的:通过任一 prefix 设置,会写入该 target 的所有槽,之后注册的别名也继承;所以同一个卷不会出现"一个 prefix 写密文、另一个 prefix 读出乱码"。不同 target(如 VFS1 与 SD)可以用不同密钥或一个明文一个加密。已经设置过回调的卷再次设置会被忽略。

### 8. KV 层

`kv/kv.c` 是 VFS 之上的薄便利层:`rt_kv_set/get/delete/list/size` 打开 `"<VFS1 的 prefix>:KV/<key>"`(prefix 来自 `vfs_get_prefix(VFS_TARGET_FLASH_VFS1)`)。它依赖 `lfs_mount_flag == 1`,所以只存在于 VFS1 的 LittleFS 上;`VFS1_REALFS` 时编成失败桩。**TODO:** 后续应改为与 VFS1 这个 target 绑定而不是与 LittleFS 绑定(需先处理 REALFS 与 LittleFS 在 fopen 模式上的差异:`"+"` 是否创建、`"wx"` 排他、流式写不支持 seek、`opendir` 不支持)。`MAX_KEY_LENGTH = 256`(key + 路径的预算)。

### 9. FatFs disk-IO adapter 模式

FatFs 上游期望每个 drive number 有一套 `disk_*()` 回调;本树中每个后端改为导出一个 `ll_diskio_drv` 结构体(`disk_initialize / disk_deinitialize / disk_status / disk_read / disk_write / disk_ioctl` + `TAG`),由 `vfs_fatfs.c` 根据 Kconfig 注册对应那一个。随附的四个 adapter:

| 文件 | Driver 符号 | 由谁选择 |
|------|---------------|-------------|
| `fatfs/fatfs_flash.c` | `Flash_disk_Driver` | 总是编译(片上 flash);second flash 部分由 `CONFIG_FATFS_SECOND_FLASH` 控制 |
| `fatfs/fatfs_sdcard.c` | `SD_disk_Driver`, `SD_disk_spi_Driver` | `CONFIG_FATFS_SD_MODE` / `CONFIG_FATFS_SD_SPI_MODE`(CMake 级门控) |
| `fatfs/fatfs_usbh_api.c` | (USB-MSC adapter) | `CONFIG_FATFS_USB_HOST`(CMake 级门控) |
| `fatfs/fatfs_ram.c` | `RAM_disk_Driver` | `CONFIG_VFS_RAM_FATFS` 且 FatFs 非只读 |

所有 adapter 形态一致 —— 新增后端时,复制 `ll_diskio_drv` 骨架并通过 `vfs_fatfs.c` 注册,**不要** patch 上游 `r0.14b/src/diskio.c`。FatFs 配置(`r0.14b/include/ffconf.h`)里的 `_USE_WRITE` / `_USE_IOCTL` 宏包住可选方法 —— 保留它们,这样只读构建也能编译。

`fatfs_sdcard.c` 导出一个全局 `int fatfs_init_status`,由 `disk_initialize/deinitialize` 切换;`fatfs_sdcard_api.c` 中的 SD card API 轮询这个标志而不再调进驱动。重命名它前要审查 `fatfs/` 与 `vfs/`。

### 10. LittleFS 块分配、擦除时机与掉电语义

(来源:内置 `littlefs/r2.50/lfs.c` —— 调试 "丢块" / NOSPC / 掉电问题时有用。)

- **flash 上没有持久的空闲表。** LittleFS 在磁盘上*不保存*空闲块的 bitmap/FAT。"空闲"是按需重算的:通过 `lfs_fs_rawtraverse()` 遍历已提交的结构(dir pair + CTZ skip-list)。唯一的空闲状态持有者是仅存在于 RAM 的 **lookahead buffer** `struct lfs_free {off,size,i,ack,*buffer}`(`lfs.h`),一个滑动窗口 bitmap(1 bit/block),在 `lfs_alloc`(`lfs.c:602`)内逐窗口重建。一个块当且仅当有已提交结构指向它时才算*已用*;`lfs_alloc_lookahead`(`lfs.c:569`)置位其 bit。Bit=0 ⇒ 空闲。
- **擦除是惰性的、由分配驱动,从不由释放驱动。** `lfs_bd_erase` 在一个块被取得*之后*、被编程*之前*调用 —— 见 `lfs_ctz_extend`(`lfs.c:2830`,文件数据)、`lfs_dir_compact`(`lfs.c:1869`,元数据)、`lfs_file_relocate`(`lfs.c:3170`)、`lfs_fs_rawgrow`(`lfs.c:5417`)。删除文件/目录**不会**擦除任何东西 —— 那个块只是变成无引用,等被复用时才擦除。
- **在某次元数据 commit 引用它之前,分配并不持久。** 如果在 `lfs_alloc` 与记录该块的 commit 之间掉电(包括擦除途中),重启后遍历找不到指向该块的指针 ⇒ 它再次被判为**空闲**,会被重新分配,并在复用前重新擦除。半擦除的块无害:每次复用都先重擦。这正是 LittleFS copy-on-write 掉电一致性的核心 —— 一次尚未提交的分配,等于从未发生。
- **`free.ack` 只在本次运行内有效,不是崩溃恢复机制。** `lfs_alloc_ack`(`lfs.c:586`,在一次成功 commit 后调用)防止在一个多块操作内把同一个未提交块发出两次。整个 `free` 结构都在 RAM,mount 时被丢弃/重建(`lfs_alloc_drop`,`lfs.c:593`),所以它在重启之间不起作用。
- 实用调参提示:`lookahead_size` 越大 ⇒ 窗口越宽 ⇒ 每轮分配突发中全 FS 遍历次数越少(`lfs.c:635-642`)。

## 本子树内的局部约定

- **日志**:用 `VFS_DBG(VFS_ERROR/WARNING/INFO, ...)`(见 `vfs.h`)。注意 `VFS_DBG_ON 0` 是默认值 —— 想看调试输出必须先打开它。提交的代码里保持 `VFS_DBG_LEVEL` 为 `VFS_ERROR`。
- **堆**:任何可能在启动期运行的分配,用 `rtos_mem_zmalloc` / `rtos_mem_free`(不是 `malloc`/`free`)—— 让本层保持跨 SoC 可移植,并遵守各核配置的堆区域。
- **内置源码保持原样**:`littlefs/r2.50/lfs.c`、`fatfs/r0.14b/src/ff.c`、`ffunicode.c`、`ffsystem.c` 都是上游代码。**不要就地 patch 它们** —— 把修复放进 adapter 文件(`littlefs_adapter.c`、`fatfs/fatfs_flash.c` 等)。升级内置版本时,新建一个 `r<x.y>` 目录并 bump `v_LITTLEFS_VER`/`v_FATFS_VER`,而不是覆盖原文件。**例外:**`fatfs/r0.14b/include/ffconf.h` *就是*预期的配置入口 —— 应该改它(如 `FF_CODE_PAGE`,默认 `999` = Realtek 体积优化值,CJK 文件名改为 `936`/`950` 等),而不是动 `ff.c`。
- 本子树的路径 API 是 `dirent` 形状的,但 `vfs.h` 里的结构体**不是** libc 那个 —— `d_name[VFS_PATH_MAX+1]` 是按带 prefix 的完整路径来定大小的。

## Common Pitfalls(常见坑)

- **不能在中断上下文调用任何 VFS/文件 API**(含 `vfs_target_register`/`unregister`)。调用路径上有 `rtos_mutex_take`(`vfs_mutex`、`lfs_diskio_lock`、`rfs_lock`;`os/freertos/os_wrapper/os_wrapper_mutex.c` 注明它不走 FromISR,ISR 中调用会触发 configASSERT 或行为未定义)、`rtos_mem_zmalloc`(`__wrap_fopen` 等)以及 FatFs `FF_FS_REENTRANT` 的信号量等待。ISR 里要记录数据,先写环形缓冲区,再由任务落盘。
- **并发只保证单次调用**:FS 锁只保证单次 `fread`/`fwrite` 不交错、卷结构不损坏;多次调用组成的操作序列需应用自己加锁,多个任务不要共用同一个 `FILE*`。FatFs `FF_FS_LOCK=0` 不检测重复打开 —— 同一文件同时多个写句柄、或删除/改名正在打开的文件,可能损坏卷。
- 别忘了 `fopen("foo.txt", ...)`(无 prefix)会返回 NULL 并报 `It can't find the file system` —— 每个路径**必须**以已注册的 prefix 开头。新写的示例常踩这个。
- **`fseek` 遵循 POSIX 语义**:成功返回 `0`、出错返回 `-1`(`littlefs_seek` / `fatfs_seek` 把后端内部 seek 结果映射为 `0`/`-1`;`vfs/vfs_littlefs.c:140`、`vfs/vfs_fatfs.c`)。`__wrap_fseek` wrapper 转发该返回值,所以 `if (fseek(...) != 0)` 是正确的判错方式。(历史上这些曾返回结果偏移量 —— 不要再依赖"正返回值=新位置"。)
- **LittleFS mount 失败会静默重格式化(抹掉)该分区。** `rt_lfs_init`(`littlefs/littlefs_adapter.c:42-57`)不仅在空 flash(`LFS_ERR_NOTFMT`)时重格式化,在 `LFS_ERR_INVAL` / `LFS_ERR_CORRUPT` 时也会。所以改 flash layout(移动/缩放 VFS1/VFS2)或盘上损坏会在下次启动抹掉用户数据,而不是让 mount 失败 —— 改 layout 时务必考虑这一点。
- 把 `vfs_file->file` 强转为 `lfs_file_t*` 只在 LittleFS 后端的 user 上成立(见 `ftl.c`);在 FatFs 上它是 `FIL*`。如果必须这么做,按 `vfs.user[user_id].vfs_type` 分支。
- `CONFIG_FATFS_WITHIN_APP_IMG` 用 OTA 感知逻辑解析正在运行的镜像 —— 改 OTA layout(`OTA_IMGID_APP` vs `OTA_IMGID_IMG2`)时,要验证 `VFS_TARGET_FLASH_APP_FATFS` 在 `vfs_target_setup()` 里的镜像定位。
