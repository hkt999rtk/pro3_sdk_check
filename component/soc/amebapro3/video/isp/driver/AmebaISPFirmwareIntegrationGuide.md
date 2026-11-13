# Ameba ISP Firmware Integration Guide

代码分为四个目录：apps，drivers，isplib，kernel。

其中apps和kernel以及drivers下除isp外的其他部分，都是sample代码，可以由用户自己实现。只有isplib和drivers/isp是我们的lib代码。下面分别介绍。

## apps

start.c处理armv8m的启动以及中断处理。目前我们代码里isp中断接在armv8m的interrupt1中断上，可由用户依据自身配置修改。这里的isp中断包括：data start中断，frame end中断，以及statis done中断。

在main.c中，调用isp_mcu_apps_start，创建isp task。

在isp task中，我们完成以下内容：（请参考isp_task作为一个范例<a href="#sample">sample</a>）。

初始化isp，注册sensor和算法，启动isp，最后task进入睡眠状态，等待接收其他指令。在这个过程中需要调用rts_av接口，具体api的介绍在后面<a href="#isplib">isplib</a>章节介绍。

在printf.c中，isp_rtos_printf接口需要实现，在isplib/server/oslayer/freertos/isp_os_log.c中调用到，用于显示isp log。本sample是直接通过串口打印输出的log。

```c
/*  FIXME: need vendor implamented */
int isp_rtos_printf(const char *format, char *buf)
{
    int i;

    i = printf(format, buf);
    return i;
}
```

## drivers

此目录包括所有驱动代码，核心驱动isp，其余驱动是启动isp所需要的，有clock，i2c，gpio，pinctrl，reset，memory等，需要用户实现。其中的rtscam_rtos_XXX_t表明是用户自己实现的接口指针。下面介绍需要实现的驱动接口.

### isp

在isp task里，请先调用isp_mcu_driver_init，初始化isp driver。退出时请调用isp_mcu_driver_cleanup，清理isp driver。

其中rtscam_isp_pipeline.c里有一些需要客户自行开发的内容，有标注FIXME。

1. RTSISP_IOC_PIPE_SET_FRAME_INFO，RTSISP_IOC_PIPE_NOTIFY_DYN_FPS，RTSISP_IOC_PIPE_NOTIFY_COLOR_RANGE是需要客户自己实现具体功能。

2. 函数rtscam_isp_parse里的pipeline->is_fpga是因为在fpga开发阶段，fpga平台需要一些特殊配置，所以提供这个参数给各个模块使用。

3. 函数rtscam_isp_parse里的pipeline->bus_addr，是在fpga平台测试时获取物理地址用的。对于客户来说，只需要在isplib/server/core/isp_statis.c里能够获取g_statis的物理地址即可。

### common

一些通用的代码提供给所有驱动使用。包括以下：

1. atomic

2. barrier

3. completion

4. dev

5. kthread

6. mutex

7. spinlock

8. wait_queue

### clock

在isp driver里，我们需要在必要的时候控制isp clock，interface clock（mipi clock），sensor hclk等。所以需要实现下列接口。

```c
typedef void *rtscam_rtos_clk_t;

rtscam_rtos_clk_t rtscam_rtos_clk_get(const char *id);
void rtscam_rtos_clk_put(rtscam_rtos_clk_t clk);
int rtscam_rtos_clk_enable(rtscam_rtos_clk_t clk);
int rtscam_rtos_clk_disable(rtscam_rtos_clk_t clk);
int rtscam_rtos_clk_set_rate(rtscam_rtos_clk_t clk, unsigned long rate);
unsigned long rtscam_rtos_clk_get_rate(rtscam_rtos_clk_t clk);
long rtscam_rtos_clk_round_rate(rtscam_rtos_clk_t clk, unsigned long rate);
```

1. rtscam_rtos_clk_t rtscam_rtos_clk_get(const char *id);

get clock driver handle.

| Parameter  | Value | Description                 |
| ---------- | ----- | --------------------------- |
| param [in] | id    | clock name                  |
| return     |       | clock driver handle pointer |

2. void rtscam_rtos_clk_put(rtscam_rtos_clk_t clk);

put clock driver handle pointer

| Parameter  | Value | Description                 |
| ---------- | ----- | --------------------------- |
| param [in] | clk   | clock driver handle pointer |

3. int rtscam_rtos_clk_enable(rtscam_rtos_clk_t clk);

enable clock

| Parameter  | Value | Description                 |
| ---------- | ----- | --------------------------- |
| param [in] | clk   | clock driver handle pointer |
| return     | 0     | Function succeeded          |
| return     | minus | Function failed             |

4. int rtscam_rtos_clk_disable(rtscam_rtos_clk_t clk);

disable clock

| Parameter  | Value | Description                 |
| ---------- | ----- | --------------------------- |
| param [in] | clk   | clock driver handle pointer |
| return     | 0     | Function succeeded          |
| return     | minus | Function failed             |

5. int rtscam_rtos_clk_set_rate(rtscam_rtos_clk_t clk, unsigned long rate);

set clock rate

| Parameter  | Value | Description                 |
| ---------- | ----- | --------------------------- |
| param [in] | clk   | clock driver handle pointer |
| param [in] | rate  | clock rate to set           |
| return     | 0     | Function succeeded          |
| return     | minus | Function failed             |

6. unsigned long rtscam_rtos_clk_get_rate(rtscam_rtos_clk_t clk);

get current clock rate

| Parameter  | Value | Description                 |
| ---------- | ----- | --------------------------- |
| param [in] | clk   | clock driver handle pointer |
| return     | rate  | clock current rate value    |

7. long rtscam_rtos_clk_round_rate(rtscam_rtos_clk_t clk, unsigned long rate);

find the closest rate must be great than or equal to this rate

| Parameter  | Value | Description                 |
| ---------- | ----- | --------------------------- |
| param [in] | clk   | clock driver handle pointer |
| param [in] | rate  | clock rate want to set      |
| return     | rate  | appropriate rate            |

### gpio

在isp driver里，我们需要在必要的时候控制sensor的reset和power down。这是通过gpio来实现的。所以需要实现下列接口。

```c
typedef void *rtscam_rtos_gpiod_t;

rtscam_rtos_gpiod_t rtscam_rtos_gpiod_get(const char *id);
void rtscam_rtos_gpiod_put(rtscam_rtos_gpiod_t gpio);
void rtscam_rtos_gpiod_direction_output(rtscam_rtos_gpiod_t gpio, int value);
```

1. rtscam_rtos_gpiod_t rtscam_rtos_gpiod_get(const char *id);

get gpiod handle pointer

| Parameter  | Value | Description                |
| ---------- | ----- | -------------------------- |
| param [in] | id    | gpio name                  |
| return     |       | gpio driver handle pointer |

2. void rtscam_rtos_gpiod_put(rtscam_rtos_gpiod_t gpio);

put gpiod handle pointer

| Parameter  | Value | Description                |
| ---------- | ----- | -------------------------- |
| param [in] | gpio  | gpio driver handle pointer |

3. void rtscam_rtos_gpiod_direction_output(rtscam_rtos_gpiod_t gpio, int value);

set gpiod direction output and value

| Parameter  | Value | Description                |
| ---------- | ----- | -------------------------- |
| param [in] | gpio  | gpio driver handle pointer |
| param [in] | value | output direction value     |

### i2c

在isp driver里，sensor需要i2c和mcu进行通信。所以我们实现了以下接口。

```c
typedef void *rtscam_rtos_i2c_t;

rtscam_rtos_i2c_t rtscam_rtos_i2c_dev_get(void);
void rtscam_rtos_i2c_dev_put(rtscam_rtos_i2c_t i2c);
int rtscam_rtos_i2c_read(rtscam_rtos_i2c_t i2c, uint16_t addr, uint16_t len,
             uint8_t *buf);
int rtscam_rtos_i2c_write(rtscam_rtos_i2c_t i2c, uint16_t addr, uint16_t len,
              uint8_t *buf);
```

1. rtscam_rtos_i2c_t rtscam_rtos_i2c_dev_get(void);

get i2c driver handle pointer

| Parameter | Value | Description               |
| --------- | ----- | ------------------------- |
| return    |       | i2c driver handle pointer |

2. void rtscam_rtos_i2c_dev_put(rtscam_rtos_i2c_t i2c);

put i2c driver handle pointer

| Parameter  | Value | Description               |
| ---------- | ----- | ------------------------- |
| param [in] | i2c   | i2c driver handle pointer |

3. int rtscam_rtos_i2c_read(rtscam_rtos_i2c_t i2c, uint16_t addr, uint16_t len, uint8_t *buf);

i2c read

| Parameter  | Value | Description                        |
| ---------- | ----- | ---------------------------------- |
| param [in] | i2c   | i2c driver handle pointer          |
| param [in] | addr  | i2c slave  id                      |
| param [in] | len   | i2c  data length                   |
| param [in] | buf   | buffer  that i2c read data to save |
| return     | 0     | Function succeeded                 |
| return     | minus | Function failed                    |

4. int rtscam_rtos_i2c_write(rtscam_rtos_i2c_t i2c, uint16_t addr, uint16_t len, uint8_t *buf);

i2c write

| Parameter  | Value | Description                 |
| ---------- | ----- | --------------------------- |
| param [in] | i2c   | i2c driver handle pointer   |
| param [in] | addr  | i2c slave id                |
| param [in] | len   | i2c  data length            |
| param [in] | buf   | buffer  that i2c write data |
| return     | 0     | Function succeeded          |
| return     | minus | Function failed             |

### memory

在isp driver里，有些模块需要大块的内存，因此需要用户实现这些内存的分配和释放等操作（但本sample并未实现）。所以我们需要实现以下接口。

```c
int rtscam_rtos_alloc_mem(uint32_t *phy_addr, uint32_t length, const char *info);
int rtscam_rtos_free_mem(uint32_t *phy_addr);
```

1. int rtscam_rtos_alloc_mem(uint32_t *phy_addr, uint32_t length, const char *info);

根据传入参数length，分配对应大小的内存，并把该内存的物理地址写入info里的phy_addr即可。其中info是需要分配内存的模块的名字。

分配成功返回0，失败返回minus。

| Parameter  | Value    | Description        |
| ---------- | -------- | ------------------ |
| param [in] | phy_addr | 成功分配的内存的物理地址       |
| param [in] | length   | 需要分配的内存长度          |
| param [in] | info     | 需要分配的内存模块的名字       |
| return     | 0        | Function succeeded |
| return     | minus    | Function failed    |

2. int rtscam_ispc_free_mem(uint32_t *phy_addr);

释放内存

| Parameter  | Value    | Description          |
| ---------- | -------- | -------------------- |
| param [in] | phy_addr | 需要释放的内存的物理地址phy_addr |
| return     | 0        | Function succeeded   |
| return     | minus    | Function failed      |

### pinctrl

在isp driver里，根据senso接口选择default，mipi，dvp状态，实际代表不同pins的复用功能。其中，default是指pins做gpio的状态；mipi代表pins做sensor的mipi接口状态，dvp代表pins做sensor的dvp接口状态。需要实现下列两个接口。

```c
typedef void *rtscam_rtos_pinctrl_t;

rtscam_rtos_pinctrl_t rtscam_rtos_pinctrl_lookup_state(const char *id);
int rtscam_rtos_pinctrl_select_state(rtscam_rtos_pinctrl_t state);
```

1. rtscam_rtos_pinctrl_t rtscam_rtos_pinctrl_lookup_state(const char *id);

根据pinctrl state的名字查找需要的pinctrl state handle 指针

| Parameter  | Value | Description                  |
| ---------- | ----- | ---------------------------- |
| param [in] | id    | pinctrl state name           |
| return     |       | pinctrl state handle pointer |

2. int rtscam_rtos_pinctrl_select_state(rtscam_rtos_pinctrl_t state);

根据pinctrl state handle 指针配置所需的寄存器。

| Parameter  | Value | Description                   |
| ---------- | ----- | ----------------------------- |
| param [in] | state | pinctrl driver handle pointer |
| return     | 0     | Function succeeded            |
| return     | minus | Function failed               |

### reset

在isp driver里，需要需要操作isp reset，mipi reset以及isp system memory。所以需要实现下列接口。

```c
typedef void *rtscam_rtos_reset_t;

rtscam_rtos_reset_t rtscam_rtos_reset_control_get(const char *id);
void rtscam_rtos_reset_control_put(rtscam_rtos_reset_t reset);
int rtscam_rtos_reset_control_reset(rtscam_rtos_reset_t reset);
int rtscam_rtos_reset_control_assert(rtscam_rtos_reset_t reset);
int rtscam_rtos_reset_control_deassert(rtscam_rtos_reset_t reset);
```

1. rtscam_rtos_reset_t rtscam_rtos_reset_control_get(const char *id);

根据id名字获取reset handle 指针。

| Parameter  | Value | Description          |
| ---------- | ----- | -------------------- |
| param [in] | id    | reset name           |
| return     |       | reset handle pointer |

2. void rtscam_rtos_reset_control_put(rtscam_rtos_reset_t reset);

释放reset handle pointer

| Parameter  | Value | Description                 |
| ---------- | ----- | --------------------------- |
| param [in] | reset | reset driver handle pointer |

3. int rtscam_rtos_reset_control_reset(rtscam_rtos_reset_t reset);

一次完整的复位过程

| Parameter  | Value | Description                |
| ---------- | ----- | -------------------------- |
| param [in] | reset | reset river handle pointer |
| return     | 0     | Function succeeded         |
| return     | minus | Function failed            |

4. int rtscam_rtos_reset_control_assert(rtscam_rtos_reset_t reset);

控制reset生效

| Parameter  | Value | Description                |
| ---------- | ----- | -------------------------- |
| param [in] | reset | reset river handle pointer |
| return     | 0     | Function succeeded         |
| return     | minus | Function failed            |

5. int rtscam_rtos_reset_control_deassert(rtscam_rtos_reset_t reset);

控制reset失效

| Parameter  | Value | Description                |
| ---------- | ----- | -------------------------- |
| param [in] | rset  | reset river handle pointer |
| return     | 0     | Function succeeded         |
| return     | minus | Function failed            |

## <span id="isplib">isplib</span>

主要介绍isplib/server/api/rts_isp.c里的api。

1. int rts_av_isp_init(void);

    初始化isp。需要在其他isp api之前就调用。

    返回0，成功；返回负数，失败。

2. int rts_av_isp_cleanup (void);

    清理释放ISP。需要和rts_av_isp_init在同一进程中使用。

    返回0，成功；返回负数，失败。

3. int rts_av_isp_start(void);

    开启isp event循环。需要和rts_av_isp_init在同一进程中使用。

    返回0，成功；返回负数，失败。

4. int rts_av_isp_stop(void);

    停止isp event循环。需要和rts_av_isp_init在同一进程中使用。

5. int32_t rts_av_isp_get_status(void);

        获取isp状态，分为4种：

        RTS_ISP_UNINITIALIZED    未初始化

        RTS_ISP_STOPPED              已停止

        RTS_ISP_RUNNING             运行中

        RTS_ISP_STREAMING         出流中

```c
/**
 * @brief ISP status
 */
enum rts_isp_status {
    RTS_ISP_UNINITIALIZED, /**< ISP uninitialized */
    RTS_ISP_STOPPED, /**< ISP preview loop stopped */
    RTS_ISP_RUNNING, /**< ISP preview loop is running but not previewing */
    RTS_ISP_STREAMING, /**< ISP preview loop is streaming */
};
```

6. isp 插件文件类型，目前只支持使用静态链接库文件类型，即RTS_ISP_PLUG_BIN_TYPE_STATIC。

```c
/**
 * @brief ISP plugin binary type
 */
enum rts_isp_plugin_bin_type {
    RTS_ISP_PLUG_BIN_TYPE_FILE,
    RTS_ISP_PLUG_BIN_TYPE_BLOB,
    RTS_ISP_PLUG_BIN_TYPE_STATIC,
};
```

7. isp插件结构体， 目前只支持entry作为静态链接库的入口地址。

```c
/**
 * @brief Structure of ISP plugin.
 */
struct rts_isp_plugin {
    int id; /**< ID of plugin */
    int bin_type; /**< binary type of plugin */
    union {
        const char *path; /**< path of plugin file */
        const void *blob; /**< pointer of plugin blob */
        const void *entry; /**< entry pointer of plugin static */
    };
};
```

8. int rts_av_isp_register_plugin(const struct rts_isp_plugin *plug);

    注册isp插件。包括sensor，算法，IQ都是通过这个api进行注册的。需要和rts_av_isp_init在同一进程中使用。

    返回0，成功；返回负数，失败。

9. int rts_av_isp_unregister_plugin(int plug_id);

    取消注册isp插件。需要和rts_av_isp_init在同一进程中使用。

    返回0，成功；返回负数，失败。

10. int rts_av_isp_get_plugin(struct rts_isp_plugin *plug);

    通过插件id获取当前插件的结构体里的其他内容。需要和rts_av_isp_init在同一进程中使用。

    返回0，成功；返回负数，失败。

11. int rts_av_isp_check_plugin(uint32_t isp_id, int plug_id);

    检查插件是否可以正常使用。尤其当注册sensor的时候，需要确认该sensor是否连接正常。需要和rts_av_isp_init在同一进程中使用。

    参数 isp id和插件id

    返回0，成功；返回负数，失败。

12. int rts_av_isp_bind_plugin(uint32_t isp_id, int plug_id);

    给指定的isp绑定指定的插件。可以在任何进程中使用。

    参数 isp id和插件id

    返回0，成功；返回负数，失败。

13. int rts_av_isp_unbind_plugin(uint32_t isp_id, int plug_id);

    对指定的isp取消绑定的插件。可以在任何进程中使用。

    参数 isp id和插件id

    返回0，成功；返回负数，失败。

14. 在isplib/common/api/rts_isp_define.h里的enum rts_isp_plugin_id定义plugin id的定义。其中包括了算法algo，sensor和iq的全部类型。注册后该ID作为相应插件的唯一标识。

```c
/**
 * @brief ISP plugin ID
 */
enum rts_isp_plugin_id {
    RTS_ISP_PLUG_ID_MIN,

    RTS_ISP_PLUG_ALGO_ID_MIN = RTS_ISP_PLUG_ID_MIN,
    RTS_ISP_PLUG_ALGO_AE_ID_MIN = RTS_ISP_PLUG_ALGO_ID_MIN,
    RTS_ISP_PLUG_ALGO_AE_ID0 = RTS_ISP_PLUG_ALGO_AE_ID_MIN,
    RTS_ISP_PLUG_ALGO_AE_ID1,
    RTS_ISP_PLUG_ALGO_AE_ID2,
    RTS_ISP_PLUG_ALGO_AE_ID3,
    RTS_ISP_PLUG_ALGO_AE_ID_MAX = RTS_ISP_PLUG_ALGO_AE_ID3,

    RTS_ISP_PLUG_ALGO_AWB_ID_MIN,
    RTS_ISP_PLUG_ALGO_AWB_ID0 = RTS_ISP_PLUG_ALGO_AWB_ID_MIN,
    RTS_ISP_PLUG_ALGO_AWB_ID1,
    RTS_ISP_PLUG_ALGO_AWB_ID2,
    RTS_ISP_PLUG_ALGO_AWB_ID3,
    RTS_ISP_PLUG_ALGO_AWB_ID_MAX = RTS_ISP_PLUG_ALGO_AWB_ID3,

    RTS_ISP_PLUG_ALGO_AF_ID_MIN,
    RTS_ISP_PLUG_ALGO_AF_ID0 = RTS_ISP_PLUG_ALGO_AF_ID_MIN,
    RTS_ISP_PLUG_ALGO_AF_ID1,
    RTS_ISP_PLUG_ALGO_AF_ID2,
    RTS_ISP_PLUG_ALGO_AF_ID3,
    RTS_ISP_PLUG_ALGO_AF_ID_MAX = RTS_ISP_PLUG_ALGO_AF_ID3,

    RTS_ISP_PLUG_ALGO_OTHER_ID_MIN,
    RTS_ISP_PLUG_ALGO_OTHER_ID0 = RTS_ISP_PLUG_ALGO_OTHER_ID_MIN,
    RTS_ISP_PLUG_ALGO_OTHER_ID1,
    RTS_ISP_PLUG_ALGO_OTHER_ID2,
    RTS_ISP_PLUG_ALGO_OTHER_ID3,
    RTS_ISP_PLUG_ALGO_OTHER_ID_MAX = RTS_ISP_PLUG_ALGO_OTHER_ID3,
    RTS_ISP_PLUG_ALGO_ID_MAX = RTS_ISP_PLUG_ALGO_OTHER_ID_MAX,

    RTS_ISP_PLUG_SENSOR_ID_MIN,
    RTS_ISP_PLUG_SENSOR_ID0 = RTS_ISP_PLUG_SENSOR_ID_MIN,
    RTS_ISP_PLUG_SENSOR_ID1,
    RTS_ISP_PLUG_SENSOR_ID2,
    RTS_ISP_PLUG_SENSOR_ID3,
    RTS_ISP_PLUG_SENSOR_ID4,
    RTS_ISP_PLUG_SENSOR_ID5,
    RTS_ISP_PLUG_SENSOR_ID6,
    RTS_ISP_PLUG_SENSOR_ID7,
    RTS_ISP_PLUG_SENSOR_ID_MAX = RTS_ISP_PLUG_SENSOR_ID7,

    RTS_ISP_PLUG_EXEC_ID_MAX = RTS_ISP_PLUG_SENSOR_ID_MAX,

    RTS_ISP_PLUG_IQ_ID_MIN,
    RTS_ISP_PLUG_IQ_ID0 = RTS_ISP_PLUG_IQ_ID_MIN,
    RTS_ISP_PLUG_IQ_ID1,
    RTS_ISP_PLUG_IQ_ID2,
    RTS_ISP_PLUG_IQ_ID3,
    RTS_ISP_PLUG_IQ_ID4,
    RTS_ISP_PLUG_IQ_ID5,
    RTS_ISP_PLUG_IQ_ID6,
    RTS_ISP_PLUG_IQ_ID7,
    RTS_ISP_PLUG_IQ_ID_MAX = RTS_ISP_PLUG_IQ_ID7,

    RTS_ISP_PLUG_ID_MAX = RTS_ISP_PLUG_IQ_ID_MAX,

    _RTS_ISP_PLUG_ID_NUM,
};
```

## kernel

主要是freertos kernel源码。

1. 在kernel/portable/cache.c里，有一个需要用户实现的接口，在isplib中有使用到。

```c
void dcache_invalidate_by_addr(uint32_t addr, size_t dsize);
```

    使指定地址的D_Cache失效

    D-Cache以32字节粒度从32字节对齐的地址开始失效。

    D-Cache内存块是给定地址+给定大小的一部分无效。

2. FreeRTOSConfig.h里有需要配置的config。其中以下配置是必需的。

```c
#define configUSE_MUTEXES 1
#define configUSE_TIMERS 1
```

```c
#define configTIMER_TASK_PRIORITY (configMAX_PRIORITIES - 1)
```

    timer task拥有最高优先级。

```c
#define configIRQ_THREAD_TASK_PRIORITY (configMAX_PRIORITIES - 2)
```

中断后处理task需要一个较高的优先级。

```c
#define configSUPPORT_STATIC_ALLOCATION 1
#define configSUPPORT_DYNAMIC_ALLOCATION 1
#define INCLUDE_vTaskPrioritySet 1
#define INCLUDE_vTaskDelete 1
#define INCLUDE_vTaskSuspend 1
#define INCLUDE_vTaskDelay 1
#define INCLUDE_xTaskGetCurrentTaskHandle 1
```

## <span id="sample">sample</span>

下面介绍一下具体isp task的内容。

```c
static int register_plugin(int id, void *entry, int bind, int check)
{
    int ret;
    struct rts_isp_plugin plug;

    plug.id = id;
    plug.bin_type = RTS_ISP_PLUG_BIN_TYPE_STATIC;
    plug.entry = entry;

    ret = rts_av_isp_register_plugin(&plug);

    if (ret)
        return ret;
    if (check) {
        ret = rts_av_isp_check_plugin(ISP0, id);
        if (ret < 0) {
            rts_av_isp_unregister_plugin(id);
            return ret;
        }
    }
    if (bind) {
        ret = rts_av_isp_bind_plugin(ISP0, id);
        if (ret) {
            rts_av_isp_unregister_plugin(id);
            return ret;
        }
    }

    return 0;
}

static int register_sensor_iq(void)
{
    int ret;

    /* register sensor
     * entry: rts_isp_get_sc401ai_ops
     */
    ret = register_plugin(RTS_ISP_PLUG_SENSOR_ID0, rts_isp_get_sc401ai_ops,
                  RTS_ISP_TRUE, RTS_ISP_TRUE);
    if (ret)
        goto out;
    /* register iqs
     * entry: __isp_iq_table_start__, iq table array pointer
     */
    ret = register_plugin(RTS_ISP_PLUG_IQ_ID0,
                          (void *)__isp_iq_table_start__, RTS_ISP_TRUE,
                          RTS_ISP_TRUE);
out:
    if (ret)
        printf("register sensor iq fail ret = %d\n", ret);
    return ret;
}

static int register_all_algos(void)
{
    int ret;

    /* register ae algo
     * entry: rts_isp_get_ae_algo
     */
    ret = register_plugin(RTS_ISP_PLUG_ALGO_AE_ID0, rts_isp_get_ae_algo,
                  RTS_ISP_TRUE, RTS_ISP_TRUE);
    if (ret)
        goto out;
    /* register awb algo
     * entry: rts_isp_get_awb_algo
     */
    ret = register_plugin(RTS_ISP_PLUG_ALGO_AWB_ID0, rts_isp_get_awb_algo,
                  RTS_ISP_TRUE, RTS_ISP_TRUE);
    if (ret)
        goto out;
    /* register af algo
     * entry: rts_isp_get_af_algo
     */
    ret = register_plugin(RTS_ISP_PLUG_ALGO_AF_ID0, rts_isp_get_af_algo,
                  RTS_ISP_TRUE, RTS_ISP_TRUE);
    if (ret)
        goto out;
    /* register other algo
     * entry: rts_isp_get_other_algo
     */
    ret = register_plugin(RTS_ISP_PLUG_ALGO_OTHER_ID0,
                  rts_isp_get_other_algo, RTS_ISP_TRUE,
                  RTS_ISP_TRUE);
out:
    if (ret)
        printf("register algos fail ret = %d\n", ret);
    return ret;
}

static void isp_task(void *pvParameters)
{
    int ret;

    ret = driver_init(); /* init other drivers except isp */
    if (ret)
        goto exit_noclean;
    ret = isp_mcu_driver_init(); /* init isp driver */
    if (ret)
        goto exit_clean_driver;
    ret = rts_av_isp_init(); /* init isp */
    if (ret)
        goto exit_clean_driver;
    ret = register_sensor_iq(); /* register sensor and iq */
    if (ret)
        goto exit;
    ret = register_all_algos(); /* register algos */
    if (ret)
        goto exit;
    ret = rts_av_isp_start(); /* start isp loop */
    if (ret)
        goto exit;
exit:
    rts_av_isp_cleanup(); /* cleanup isp */
    isp_mcu_driver_cleanup(); /* cleanup isp driver */
exit_clean_driver:
    driver_cleanup(); /* cleanup other drivers except isp */
exit_noclean:
    if (ret)
        printf("rts isp fail %d\n", ret);
    vTaskDelete(NULL); /* delete current task when exit */
}

static void isp_preview_task(void *pvParameters)
{
    int ret;

    ret = rts_isp_test_preview(0, 10);
    if (ret)
        vTaskDelete(NULL);
    while (1) {
        vTaskDelay(pdMS_TO_TICKS(500));
    }
}

void isp_mcu_apps_start(void)
{
    /* create isp task*/
    xTaskCreate(isp_task, "isp", configMINIMAL_STACK_SIZE * 16, NULL,
            (portPRIVILEGE_BIT | (configMAX_PRIORITIES / 2)), NULL);
    if (0) {
        /* test code for preview */
        xTaskCreate(isp_preview_task, "isp_preview",
                configMINIMAL_STACK_SIZE * 16, NULL,
                (portPRIVILEGE_BIT |
                 ((configMAX_PRIORITIES / 2) - 2)),
                NULL);
    }
}
```

# 编译方法

1. 首先需要在isp_mcu下建立一个toolchain软连接：
   
   toolchain -> /toolchain/asdk-10.3.1/linux/newlib/

2. 需要安装python3以及grpcio_tools
   
   python3 -m pip install --user grpcio_tools

3. 编译isp.fw
   
   mkdir build
   
   cd build
   
   cmake -DCMAKE_TOOLCHAIN_FILE=../toolchainfile.cmake ../
   
   make

4. 生成isp.fw：build/apps/isp.fw
