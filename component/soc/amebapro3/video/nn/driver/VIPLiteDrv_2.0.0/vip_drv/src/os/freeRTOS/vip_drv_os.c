/******************************************************************************\
|* Copyright (c) 2017-2024 by Vivante Corporation.  All Rights Reserved.      *|
|*                                                                            *|
|* The material in this file is confidential and contains trade secrets of    *|
|* of Vivante Corporation.  This is proprietary information owned by Vivante  *|
|* Corporation.  No part of this work may be disclosed, reproduced, copied,   *|
|* transmitted, or used in any way for any purpose, without the express       *|
|* written permission of Vivante Corporation.                                 *|
|*                                                                            *|
\******************************************************************************/
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "timers.h"
#include "event_groups.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include "vip_drv_os_port.h"
#if vpmdENABLE_SYS_MEMORY_HEAP
#include "vip_hal.h"
#endif
#include "vip_drv_interface.h"
#include "vip_drv_device_driver.h"
#include "vip_drv_mem_heap.h"
#include "vip_drv_hardware.h"
#include "vip_drv_context.h"
// #include "printf.h"
#include "ameba_soc.h"


#define DEFAULT_AI_TASK_NAME            "NPU TASK"
#define DEFAULT_AI_TASK_STACK_SZ        2000
#define DEFAULT_AI_TASK_PRIO            5 //(configMAX_PRIORITIES <96> 1)
#define NPU_FREERTOS_WAIT_FOREVER       0xFFFFFFFF


typedef struct _vipdrv_signal_data
{
    void *wait_signal;
    vipdrv_mutex lock;
    volatile vip_bool_e value;
} vipdrv_signal_data_t;

/*
@brief Do some initialization works in VIPDRV_OS layer.
@param video_mem_size, the size of video memory heap.
*/
vip_status_e vipdrv_os_init(
    IN vip_uint32_t video_mem_size
    )
{
    vip_status_e status = VIP_SUCCESS;

    status = vipdrv_drv_init(video_mem_size);
    if (status != VIP_SUCCESS) {
        PRINTK_E("fail to drv init\n");
    }

    return status;
}

/*
@brief Do some un-initialization works in VIPDRV_OS layer.
*/
vip_status_e vipdrv_os_close(void)
{
    vip_status_e status = VIP_SUCCESS;

    status = vipdrv_drv_exit();
    if (status != VIP_SUCCESS) {
        PRINTK_E("fail to drv exit\n");
    }

    return status;
}


/*******************************NOTICE***********************************/
/************************************************************************/
/* *********************need to be ported functions**********************/
/************************************************************************/
/************************************************************************/
/************************************************************************/
/*
@brief Allocate system memory in vip drv space.
@param video_mem_size, the size of video memory heap.
@param size, Memory size to be allocated.
@param memory, Pointer to a variable that will hold the pointer to the memory.
*/
vip_status_e vipdrv_os_allocate_memory(
    IN vip_size_t size,
    OUT void **memory
    )
{
    vip_status_e status = VIP_SUCCESS;

    if (size == 0 || !memory) {
        return VIP_ERROR_INVALID_ARGUMENTS;
    }

#if vpmdENABLE_SYS_MEMORY_HEAP
    status = viphal_allocate_memory(size, memory);
    if (status != VIP_SUCCESS)
#endif
    {
        *memory = pvPortMalloc(size);
        if (*memory == VIP_NULL) {
            return VIP_ERROR_OUT_OF_MEMORY;
        }
    }

    return status;
}

/*
@brief Free system memory in vip drv space.
@param memory, Memory to be freed.
*/
vip_status_e vipdrv_os_free_memory(
    IN void *memory
    )
{
    vip_status_e status = VIP_SUCCESS;

    if (!memory) {
        return VIP_ERROR_INVALID_ARGUMENTS;
    }
#if vpmdENABLE_SYS_MEMORY_HEAP
    status = viphal_free_memory(memory);
    if (status != VIP_SUCCESS)
#endif
    {
        vPortFree(memory);
    }

    return VIP_SUCCESS;
}

/*
@brief Set memory to zero.
@param memory, Memory to be set to zero.
@param size, the size of memory.
*/
vip_status_e vipdrv_os_zero_memory(
    IN void *memory,
    IN vip_size_t size
    )
{
    if (!memory) {
        return VIP_ERROR_INVALID_ARGUMENTS;
    }
    memset(memory, 0, size);

    return VIP_SUCCESS;
}

/*
@brief copy src memory to dst memory.
@param dst, Destination memory.
@param src. Source memory
@param size, the size of memory should be copy.
*/
vip_status_e vipdrv_os_copy_memory(
    IN void *dst,
    IN const void *src,
    IN vip_size_t size
    )
{
    if ((VIP_NULL == src) || (VIP_NULL == dst))  {
        PRINTK_E("fail to copy memory, parameter is NULL, size: %d bytes\n", size);
        return VIP_ERROR_INVALID_ARGUMENTS;
    }
    memcpy(dst, src, size);

    return VIP_SUCCESS;
}

/*
@brief Read a hardware register.
@param reg_base, the base address of hardware register.
@param address, read the register address.
@param data, the data of address be read.
*/
vip_status_e vipdrv_os_read_reg(
    IN volatile void *reg_base,
    IN vip_uint32_t address,
    OUT vip_uint32_t *data
    )
{
    vip_uint32_t *reg = (vip_uint32_t *)((vip_uint8_t *)reg_base + address);
    *data = *reg;

#if vpmdENABLE_AHB_LOG
    {
    vipdrv_hardware_info_t info = {0};
    vip_uint32_t i = 0;
    vipdrv_drv_get_hardware_info(&info);
    for (i = 0; i < info.core_count; i++) {
        if (reg_base == info.vip_reg[i]) {
            break;
        }
    }
    PRINTK("@[register.read %u 0x%05X 0x%08X]\n", i, address, *data);
    }
#endif

    return VIP_SUCCESS;
}

/*
@brief Write a hardware register.
@param reg_base, the base address of hardware register.
@param address, write the register address.
@param data, the data of address be wrote.
*/
vip_status_e  vipdrv_os_write_reg(
    IN volatile void *reg_base,
    IN vip_uint32_t address,
    IN vip_uint32_t data
    )
{
    vip_uint32_t *reg = (vip_uint32_t *)((vip_uint8_t *)reg_base + address);
    *reg = data;

#if vpmdENABLE_AHB_LOG
    {
    vipdrv_hardware_info_t info = {0};
    vip_uint32_t i = 0;
    vipdrv_drv_get_hardware_info(&info);
    for (i = 0; i < info.core_count; i++) {
        if (reg_base == info.vip_reg[i]) {
            break;
        }
    }
    PRINTK("@[register.write %u 0x%05X 0x%08X]\n", i, address, data);
    }
#endif

    return VIP_SUCCESS;
}

/*
@brief Waiting for the hardware interrupt. wait interrupt function depend on the type of RTOS
@param irq_queue, the wait queue of interrupt handle.
@param irq_value, a flag for interrupt.
@param mask, mask data for interrupt.
*/
vip_status_e vipdrv_os_wait_interrupt(
    IN void    *irq_queue,
    IN volatile vip_uint32_t* volatile irq_value,
    IN vip_uint32_t time_out,
    IN vip_uint32_t mask
    )
{
    vip_status_e status = VIP_SUCCESS;
    SemaphoreHandle_t *irq_queue_handle = (SemaphoreHandle_t *)irq_queue;
    vipIsNULL(irq_value);

    if ((mask & *irq_value) == 0) {
        if (xSemaphoreTake(*irq_queue_handle, time_out) != pdTRUE) {
            PRINTK_E("freeOS xTestSemaphore time out irq_value=0x%x\n", *irq_value);
            return VIP_ERROR_TIMEOUT;
        }
    }
    else {
        PRINTK_I("not wait, may interrupt is returned, irq_value=0x%x\n", *irq_value);
    }

onError:
    return status;
}

vip_status_e vipdrv_os_wakeup_interrupt(
    IN void *irq_queue
    )
{
    vip_status_e status = VIP_SUCCESS;
    static portBASE_TYPE xHigherPriorityTaskWoken;

    xHigherPriorityTaskWoken = pdFALSE;
    SemaphoreHandle_t *irq_queue_handle = (SemaphoreHandle_t *)irq_queue;

    xSemaphoreGiveFromISR(*irq_queue_handle, &xHigherPriorityTaskWoken);

    return status;
}

/*
@brief Delay execution of the current thread for a number of milliseconds.
@param ms, delay unit ms. Delay to sleep, specified in milliseconds.
*/
void vipdrv_os_delay(
    IN vip_uint32_t ms
    )
{
    if (ms > 0) {
        vTaskDelay(ms);
    }
}

/*
@brief Delay execution of the current thread for a number of microseconds.
@param ms, delay unit us. Delay to sleep, specified in microseconds.
*/
void vipdrv_os_udelay(
    IN vip_uint32_t us
    )
{
    /* please replace vTaskDelay to delay us */
    DelayUs(us);
}

/*
@brief Print string on console.
@param msg, which message to be print.
*/
vip_status_e vipdrv_os_print(
    vip_log_level level,
    IN const char *message,
    ...
    )
{
    vip_status_e status = VIP_SUCCESS;
    char strbuf[256] = {'\0'};
    (void)level;
    va_list varg;
    va_start (varg, message);
    vsnprintf(strbuf, 256, message, varg);
    va_end(varg);

    //iprintf("npu[%x] %s", vipdrv_os_get_tid(), strbuf);
    printf("npu[%x] %s", vipdrv_os_get_tid(), strbuf);

    return status;
}

/*
@brief Print string to buffer.
@param, size, the size of msg.
@param msg, which message to be print.
*/
vip_status_e vipdrv_os_snprint(
    IN vip_char_t *buffer,
    IN vip_uint32_t size,
    IN const vip_char_t *msg,
    ...
    )
{
    vip_status_e status = VIP_SUCCESS;
    va_list varg;
    va_start (varg, msg);
    vsnprintf(buffer, size, msg, varg);
    va_end(varg);

    return status;
}

/*
@brief get the current system time.
*/
vip_uint64_t  vipdrv_os_get_time(void)
{
    return (vip_uint64_t)xTaskGetTickCount();
}

/*
@brief Get process id
*/
vip_uint32_t vipdrv_os_get_pid(void)
{
    vip_uint32_t pid;
    pid = 0x12345678;  //xTaskGetCurrentTaskHandle()
    return pid;
}

/*
@brief Get thread id
*/
vip_uint32_t vipdrv_os_get_tid(void)
{
    vip_uint32_t tid;
    tid = 0x12345678;  //xTaskGetCurrentTaskHandle()
    return tid;
}

/*
@brief Memory barrier function for memory consistency.
*/
vip_status_e vipdrv_os_memorybarrier(void)
{
    /* please make sure the memory barrier ASM can work well on you platform */
    __asm__ __volatile__("dsb sy":::"cc");

    return VIP_SUCCESS;
}

/*
@brief Flush CPU cache for video memory which allocated from heap.
@param physical, the CPU physical address.
@param logical, the CPU logical address.
       loggical address only for Linux system or CPU MMU is enabled?.
       should be equal to physical address in RTOS?
@param size, the size of the memory should be flush.
@param type The type of operate cache. see vipdrv_cache_type_e.
*/
vip_status_e vipdrv_os_flush_cache(
    IN vip_physical_t physical,
    IN void *logical,
    IN vip_uint32_t size,
    IN vip_uint8_t type
    )
{
    vip_status_e status = VIP_SUCCESS;

	/* please implement flush cpu cache function if video memory heap has enabled cache */
	//dcache_clean_by_addr((uint32_t *)logical, size);
	uint32_t data = (uint32_t)logical;
	uint32_t data_end = data + size;
	uint32_t b0_start = data & (~(vpmdCPU_CACHE_LINE_SIZE - 1U));
	uint32_t b1_end = (data_end + (vpmdCPU_CACHE_LINE_SIZE - 1U)) & (~(vpmdCPU_CACHE_LINE_SIZE - 1U));
    
    PRINTK_D("flush cache physical=0x%llX, size=0x%08X, type=%d\n", physical, size, type);
    if ((0 == physical) && (VIP_NULL == logical)) {
        PRINTK_E("flush cache physical=0x%x, logical=%p\n", physical, logical);
        return VIP_ERROR_FAILURE;
    }

    /* please implement flush cpu cache function if video memory heap has enabled cache */
    switch (type) {
        case VIPDRV_CACHE_CLEAN:
            DCache_Clean(b0_start, b1_end - b0_start);
            break;
        case VIPDRV_CACHE_FLUSH:
            DCache_CleanInvalidate(b0_start, b1_end - b0_start);
            break;
        case VIPDRV_CACHE_INVALID:
            DCache_Invalidate(b0_start, b1_end - b0_start);
            break;
        default:
            PRINTK_E("not support this flush cache type=%d\n", type);
            status = VIP_ERROR_INVALID_ARGUMENTS;
            break;
    }

    //logical = logical;

    return status;
}

#include "cmsis_compiler.h"
int vipdrv_in_interrupt(void)
{
#ifdef CONFIG_ARM_CORE_CA32
	return __get_mode() != CPSR_M_USR;
#else
#if defined(__ICCARM__)
	return (__get_PSR() & 0x1FF) != 0;
#elif defined(__GNUC__)
	return (__get_xPSR() & 0x1FF) != 0;
#endif
#endif
}

/*
@brief Create a mutex for multiple thread.
@param mutex, create mutex pointer.
*/
vip_status_e vipdrv_os_create_mutex(
    OUT vipdrv_mutex *mutex
    )
{
    *mutex = xSemaphoreCreateRecursiveMutex();
    if(NULL != *mutex) {
        return VIP_SUCCESS;
    }
    else {
        PRINTK_E("fail to create mutex\n");
        return VIP_ERROR_FAILURE;
    }
}

vip_status_e vipdrv_os_lock_mutex(
    IN vipdrv_mutex mutex
    )
{
    if (VIP_NULL == mutex) {
        PRINTK_E("fail to lock mutex, parameter is NULL\n");
        return VIP_ERROR_FAILURE;
    }

    if (pdTRUE == xSemaphoreTakeRecursive(mutex, NPU_FREERTOS_WAIT_FOREVER)) {
        return VIP_SUCCESS;
    }
    else {
        PRINTK_E("fail to lock mutex=0x%"PRPx"\n", mutex);
        return VIP_ERROR_FAILURE;
    }
}

vip_status_e vipdrv_os_unlock_mutex(
    IN vipdrv_mutex mutex
    )
{
    if (VIP_NULL == mutex) {
        PRINTK_E("fail to unlock mutex, parameter is NULL\n");
        return VIP_ERROR_FAILURE;
    }

    if (pdTRUE == xSemaphoreGiveRecursive(mutex)) {
        return VIP_SUCCESS;
    }
    else {
        PRINTK_E("fail to unlock mutex=0x%"PRPx"\n", mutex);
        return VIP_ERROR_FAILURE;
    }
}

/*
@brief Destroy a mutex which create in vipdrv_os_create_mutex..
@param mutex, create mutex pointer.
*/
vip_status_e vipdrv_os_destroy_mutex(
    IN vipdrv_mutex mutex
    )
{
    if (mutex != VIP_NULL) {
        vSemaphoreDelete(mutex);
    }
    return VIP_SUCCESS;
}

#if vpmdENABLE_MULTIPLE_TASK || vpmdPOWER_OFF_TIMEOUT
/*
@brief Create a thread.
@param func, the thread handle function.
@param parm, parameter for this thread.
@param name, the thread name.
@param handle, the handle for thread.
*/
typedef struct _vipdrv_thread_s {
	vipdrv_thread_func func;
	vip_ptr parm;
	vipdrv_thread *handle;
} vipdrv_thread_t;

static void vipdrv_thread_wrap(void *arg)
{
	if (arg) {
		vipdrv_thread_t *t = (vipdrv_thread_t *)arg;
		void (*func)(void *) = (void (*)(void *))t->func;
		func((void *)t->parm);

		*t->handle = NULL;
		free(arg);
	}
	vTaskDelete(NULL);
	while (1);
}

static int vip_task_count = 0;

vip_status_e vipdrv_os_create_thread(
    IN vipdrv_thread_func func,
    IN vip_ptr parm,
    IN vip_char_t *name,
    OUT vipdrv_thread *handle
    )
{
	char vip_task_name[16];
	if (name) {
		snprintf(vip_task_name, 16, "%s", name);
	} else {
		sprintf(vip_task_name, 16, DEFAULT_AI_TASK_NAME" %x", vip_task_count++);
	}

	vipdrv_thread_t *t = (vipdrv_thread_t *) malloc(sizeof(vipdrv_thread_t));
	if (!t) {
		return VIP_ERROR_OUT_OF_MEMORY;
	}
	t->func = func;
	t->parm = parm;
	t->handle = handle;
	if (pdTRUE == xTaskCreate(
			(TaskFunction_t) vipdrv_thread_wrap,
			vip_task_name,
			DEFAULT_AI_TASK_STACK_SZ,
			(void *)t,
			DEFAULT_AI_TASK_PRIO,
			(TaskHandle_t *)handle
		)) {
		return VIP_SUCCESS;
	} else {
		PRINTK_E("fail to create npu thread\n");
		return VIP_ERROR_FAILURE;
	}

}

vip_status_e vipdrv_os_destroy_thread(
    IN vipdrv_thread handle
    )
{
    if (handle != VIP_NULL) {
        vTaskDelete(handle);
    }
    return VIP_SUCCESS;
}
#endif

#include <stdatomic.h>
/*
@brief Create a atomic.
@param atomic, create atomic pointer.
*/
vip_status_e vipdrv_os_create_atomic(
    OUT vipdrv_atomic *atomic
    )
{
    vip_status_e status = VIP_SUCCESS;
    vip_uint32_t *c_atomic = VIP_NULL;
    vipIsNULL(atomic);

    /* please add atomic implement in here if system supports atomic
        otherwise use below function to instead atmoic */

    status = vipdrv_os_allocate_memory(sizeof(vip_uint32_t), (void**)&c_atomic);
    if (status != VIP_SUCCESS) {
        PRINTK_E("fail to allocate memory for atomic\n");
        return VIP_ERROR_IO;
    }

    atomic_store(c_atomic, 0);
    *atomic = (vip_ptr)c_atomic;

onError:
    return status;
}

/*
@brief Destroy a atomic which create in vipdrv_os_create_atomic
@param atomic, create atomic pointer.
*/
vip_status_e vipdrv_os_destroy_atomic(
    IN vipdrv_atomic atomic
    )
{
    vip_status_e status = VIP_SUCCESS;
    vipIsNULL(atomic);

    vipdrv_os_free_memory(atomic);

onError:
    return status;
}

/*
@brief Set the 32-bit value protected by an atomic.
@param atomic, create atomic pointer.
*/
vip_status_e vipdrv_os_set_atomic(
    IN vipdrv_atomic atomic,
    IN vip_uint32_t value
    )
{
    vip_status_e status = VIP_SUCCESS;
    //vip_uint32_t *data = (vip_uint32_t*)atomic;
    vipIsNULL(atomic);

    atomic_store((atomic_int *)atomic, value);

onError:
    return status;
}

/*
@brief Get the 32-bit value protected by an atomic.
@param atomic, create atomic pointer.
*/
vip_uint32_t vipdrv_os_get_atomic(
    IN vipdrv_atomic atomic
    )
{
    // vip_uint32_t value = 0xFFFFFFFE;
    // vip_uint32_t *data = (vip_uint32_t*)atomic;
    if (VIP_NULL == atomic) {
        PRINTK_E("get atomic invalid arguments\n");
        return(VIP_ERROR_INVALID_ARGUMENTS);
    }

    return atomic_load((atomic_int *)atomic);
}

/*
@brief Increase the 32-bit value protected by an atomic.
@param atomic, create atomic pointer.
*/
vip_status_e vipdrv_os_inc_atomic(
    IN vipdrv_atomic atomic
    )
{
    vip_status_e status = VIP_SUCCESS;
    //vip_uint32_t *data = (vip_uint32_t*)atomic;
    vipIsNULL(atomic);

	atomic_fetch_add((atomic_int *)atomic, 1);

onError:
    return status;
}

/*
@brief Decrease the 32-bit value protected by an atomic.
@param atomic, create atomic pointer.
*/
vip_status_e vipdrv_os_dec_atomic(
    IN vipdrv_atomic atomic
    )
{
    vip_status_e status = VIP_SUCCESS;
    //vip_uint32_t *data = (vip_uint32_t*)atomic;
    vipIsNULL(atomic);

	atomic_fetch_sub((atomic_int *)atomic, 1);

onError:
    return status;
}

#if vpmdENABLE_MULTIPLE_TASK || vpmdENABLE_SUSPEND_RESUME || vpmdPOWER_OFF_TIMEOUT
/*
@brief Create a signal.
@param handle, the handle for signal.
*/
vip_status_e vipdrv_os_create_signal(
    IN vipdrv_signal *handle
    )
{
	*handle = xEventGroupCreate();
	return VIP_SUCCESS;
}

vip_status_e vipdrv_os_destroy_signal(
    IN vipdrv_signal handle
    )
{
	vEventGroupDelete(handle);
	return VIP_SUCCESS;
}

/*
@brief Waiting for a signal.
@param handle, the handle for signal.
@param timeout, the timeout of waiting signal. unit ms. timeout is 0 or vpmdINFINITE, infinite wait signal.
*/
#include "event_groups.h"
#define BIT_ALL ( 0xffffffff )
#define BIT_0	( 1 << 0 )
#define BIT_1	( 1 << 1 )
vip_status_e vipdrv_os_wait_signal(
    IN vipdrv_signal handle,
    IN vip_uint32_t timeout
    )
{
	EventBits_t bit_state = xEventGroupWaitBits(handle, BIT_1, pdTRUE, pdFALSE, timeout);

	if (bit_state != BIT_1) {
		return VIP_ERROR_TIMEOUT;
	}

	return VIP_SUCCESS;
}

/*
@brief wake up wait signal.
     state is true, wake up all waiting signal and chane the signal state to SET.
     state is false, change the signal to RESET so the vipdrv_os_wait_signal will block until
     the signal state changed to SET. block waiting signal function.
@param handle, the handle for signal.
*/
vip_status_e vipdrv_os_set_signal(
    IN vipdrv_signal handle,
    IN vip_bool_e state
    )
{
	if (state == vip_false_e) {
		xEventGroupClearBits(handle, BIT_1);
	} else {
		if (vipdrv_in_interrupt()) {
			xEventGroupSetBitsFromISR(handle, BIT_1, pdFALSE);
		} else {
			xEventGroupSetBits(handle, BIT_1);
		}
	}

	return VIP_SUCCESS;
}
#endif

#if vpmdPOWER_OFF_TIMEOUT
/*
@brief create a system timer callback.
@param func, the callback function of this timer.
@param param, the paramete of the timer.
*/
typedef struct freertos_timer_id_s {
	void (*timer_cb)(void *param);
	void *param;
} freertos_timer_id_t;

void freertos_timer_wrapper(TimerHandle_t  xTimer)
{
	freertos_timer_id_t *id = (freertos_timer_id_t *)pvTimerGetTimerID(xTimer);

	id->timer_cb(id->param);
}

vip_status_e vipdrv_os_create_timer(
    IN vipdrv_timer_func func,
    IN vip_ptr param,
    OUT vipdrv_timer *timer
    )
{
	freertos_timer_id_t *id = malloc(sizeof(freertos_timer_id_t));
	if (!id)	{
		return VIP_ERROR_OUT_OF_MEMORY;
	}

	id->timer_cb = (void (*)(void *))func;
	id->param = param;

	TimerHandle_t tmr = xTimerCreate("viptmr", 600000, pdFALSE, (void *)id, (TimerCallbackFunction_t)freertos_timer_wrapper);
	*timer = (vip_ptr)tmr;
	return VIP_SUCCESS;
}

vip_status_e vipdrv_os_start_timer(
    IN vipdrv_timer timer,
    IN vip_uint32_t time_out
    )
{
	vip_status_e vip_ret = VIP_SUCCESS;
	BaseType_t  ret = xTimerChangePeriod((TimerHandle_t)timer, time_out, 10);

	if (ret == pdPASS) {
		ret = xTimerStart((TimerHandle_t)timer, 10);
	}

	if (ret == pdFAIL) {
		vip_ret = VIP_ERROR_TIMEOUT;
	}

	return vip_ret;
}

vip_status_e vipdrv_os_stop_timer(
    IN vipdrv_timer timer
    )
{
	vip_status_e vip_ret = VIP_SUCCESS;

	BaseType_t ret = xTimerStop((TimerHandle_t)timer, 10);
	if (ret == pdFAIL) {
		vip_ret = VIP_ERROR_TIMEOUT;
	}

	return vip_ret;
}

/*
@brief destroy a system timer
*/
vip_status_e vipdrv_os_destroy_timer(
    IN vipdrv_timer timer
    )
{
	vip_status_e vip_ret = VIP_SUCCESS;

	freertos_timer_id_t *id = (freertos_timer_id_t *)pvTimerGetTimerID(timer);
	if (id) {
		free(id);
	}

	BaseType_t ret = xTimerDelete((TimerHandle_t)timer, 10);
	if (ret == pdFAIL) {
		vip_ret = VIP_ERROR_TIMEOUT;
	}

	return vip_ret;
}
#endif

vip_int32_t vipdrv_os_strcmp(
    const vip_char_t* s1,
    const vip_char_t* s2
    )
{
    return strcmp(s1, s2);
}
