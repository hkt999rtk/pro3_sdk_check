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

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <vip_hal_os_port.h>
#include <vip_drv_share.h>
#include <vip_drv_interface.h>
#include "FreeRTOS.h"
#include "semphr.h"


/*
@brief initialize OS env.
*/
vip_status_e viphal_os_init(
    void **os
    )
{
    return VIP_SUCCESS;
}

/*
@brief uninitialize OS env.
@param os, os object.
*/
vip_status_e viphal_os_uninit(
    void **os
    )
{
    return VIP_SUCCESS;
}

/*
@brief allocate memory from system.
@param size, the size of memory.
@param memory, memory object.
*/
vip_status_e viphal_os_allocate_memory(
    IN vip_size_t size,
    OUT void **memory
    )
{
    void *allco_memory = VIP_NULL;

    if (size == 0 || !memory) {
        return VIP_ERROR_INVALID_ARGUMENTS;
    }

    allco_memory = pvPortMalloc(size);
    if (!allco_memory) {
        return VIP_ERROR_OUT_OF_MEMORY;
    }

    *memory = allco_memory;

    return VIP_SUCCESS;
}

/*
@brief free memory.
@param memory, memory object.
*/
vip_status_e viphal_os_free_memory(
    IN void *memory
    )
{
    if (!memory) {
        return VIP_ERROR_INVALID_ARGUMENTS;
    }

    vPortFree(memory);

    return VIP_SUCCESS;
}

/*
@brief copy src to dst.
@param size the size of memory should be copy.
*/
vip_status_e viphal_os_copy_memory(
    IN void *dst,
    IN const void *src,
    IN vip_size_t size
    )
{
    if (!dst || !src || size == 0) {
        return VIP_ERROR_INVALID_ARGUMENTS;
    }

    memcpy(dst, src, size);

    return VIP_SUCCESS;
}

/*
@brief zero memory.
@param size the size of memory should be zero.
*/
vip_status_e viphal_os_zero_memory(
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
@brief call 'kernel space' functions entry. kernel spce is only for Linux system. freeRTOS is no kernel space concept.
       so, this function is entry for 'kernel function'. don't need change it on RTOS.
@param os, os object create in viphal_os_init.
@param command, kenrel space command.
@param data, pass to kernel space function data or kernel space return data.
*/
vip_status_e viphal_os_call_drv(
    IN void *os,
    IN OUT vipdrv_intrface_e command,
    IN OUT void *data
    )
{
    return vipdrv_interface_call(command, data);
}

/*
@brief print messago on console
@param message, which message should be print.
*/
vip_status_e viphal_os_print(
    vip_log_level level,
    IN const vip_char_t *message,
    ...
    )
{
    vip_status_e error = VIP_SUCCESS;

    (void)level;

    char strbuf[256] = {'\0'};
    va_list varg;
    va_start (varg, message);
    vsnprintf(strbuf, 256, message, varg);
    va_end(varg);

    //iprintf("%s", strbuf);
    printf("%s", strbuf);

    return error;
}

/*
@brief print string into a file(stream).
@param os object, create in viphal_os_init().
@param buffer, which memory should be print.
*/
vip_status_e viphal_os_snprint(
    OUT vip_char_t *buffer,
    IN const vip_uint32_t size,
    IN const vip_char_t *message,
    ...
    )
{
    vip_status_e error = VIP_SUCCESS;
    va_list arg;
    va_start(arg, message);
    vsnprintf(buffer, size, message, arg);
    va_end(arg);

    return error;
}

/*
@brief open a file.
@param name, file name.
@param mode, opem mode.
*/
#include "nn_file_op.h"
viphal_file_t viphal_os_open_file(
    vip_char_t *name,
    viphal_file_mode_e mode
    )
{
    return (viphal_file_t *)nn_f_open(name, mode);
    //return (viphal_file_t *)NULL;
}

vip_status_e viphal_os_read_file(
    void *dst,
    vip_size_t size,
    viphal_file_t file
    )
{
    vip_status_e status = VIP_SUCCESS;
    
    nn_f_read((void *)file, dst, size);
    
    return status;
}

vip_status_e viphal_os_write_file(
    void *data,
    vip_size_t size,
    viphal_file_t file
    )
{
    vip_status_e status = VIP_SUCCESS;

    return status;
}

void viphal_os_close_file(
    viphal_file_t file
    )
{
    nn_f_close((void *)file);
}

/*
typedef enum _gcvip_file_seek {
	GCVIP_FILE_CUR = 0,
	GCVIP_FILE_SET,
	GCVIP_FILE_END
} gcvip_file_seek_e;
*/
/*
@brief seek file location.
*/
vip_status_e viphal_os_seek_file(
    vip_size_t offset,
    viphal_file_seek_e pos,
    viphal_file_t file
    )
{
    int os_pos_map[] = {SEEK_CUR, SEEK_SET, SEEK_END};

    vip_status_e status = VIP_SUCCESS;
    nn_f_seek((void *)file, offset, os_pos_map[pos]);

    return status;
}

/*
@brief get process id.
*/
vip_uint32_t viphal_os_get_pid(void)
{
    vip_uint32_t pid;
    pid = 0x12345678;  //xTaskGetCurrentTaskHandle()
    return pid;
}

/*
@brief get thread id.
*/
vip_uint32_t viphal_os_get_tid(void)
{
    vip_uint32_t tid;
    tid = 0x12345678;  //xTaskGetCurrentTaskHandle()
    return tid;
}

/*
@brief print string into a file(stream).
@param os object, create in viphal_os_init() if user os lyaer neccessary this.
@param buffer, which memory should be print.
*/
vip_status_e viphal_os_print_string(
    IN void *os,
    IN void *stream,
    IN vip_char_t *buffer
    )
{
    /* without file system in freeRTOS, so print to screen */
    vip_print(buffer);

    return VIP_SUCCESS;
}

#if vpmdENABLE_MULTIPLE_TASK || vpmdENABLE_MEMORY_PROFILING
/*
@brief create mutex.
@param mutex. mutex object
*/
vip_status_e viphal_os_create_mutex(
    vip_ptr *mutex
    )
{
    *mutex = xSemaphoreCreateMutex();
    if (*mutex) {
        return VIP_SUCCESS;
    }
    else {
        vip_error("fail to create mutex\n");
        return VIP_ERROR_IO;
    }
}

/*
@brief lock mutex.
@param mutex. mutex object
*/
vip_status_e viphal_os_lock_mutex(
    vip_ptr mutex
    )
{
    if (VIP_NULL == mutex) {
        vip_error("fail to lock mutex, parameter is NULL\n");
        return VIP_ERROR_FAILURE;
    }
    if (xSemaphoreTake(mutex, (TickType_t)0xFFFFFFFF) == pdTRUE) {
        return VIP_SUCCESS;
    }
    else {
        vip_error("fail to lock mutex\n");
        return VIP_ERROR_IO;
    }
}

/*
@brief unlock mutex.
@param mutex. mutex object
*/
vip_status_e viphal_os_unlock_mutex(
    vip_ptr mutex
    )
{
    if (VIP_NULL == mutex) {
        vip_error("fail to unlock mutex, parameter is NULL\n");
        return VIP_ERROR_FAILURE;
    }
    if(xSemaphoreGive(mutex) == pdTRUE) {
        return VIP_SUCCESS;
    }
    else {
        return VIP_ERROR_IO;
    }
}

/*
@brief destroy mutex.
@param mutex. mutex object
*/
vip_status_e viphal_os_destroy_mutex(
    vip_ptr mutex
    )
{
    if (mutex != NULL) {
        vSemaphoreDelete(mutex);
    }
    return VIP_SUCCESS;
}
#endif

/*
@brief get the current system time.
*/
vip_uint64_t viphal_os_get_time(void)
{
    vip_uint64_t time = 0;
    time = (vip_uint64_t)xTaskGetTickCount();
    return time;
}

/*
@brief compare two strings.
@param s1, string one.
@param s2, string two.
*/
vip_int32_t viphal_os_strcmp(
    vip_char_t *s1,
    vip_char_t *s2
    )
{
    return strcmp(s1, s2);
}

/*
@brief copy string src to dest.
@param dest, destation buffer.
@param src, resource buffer.
*/
vip_char_t* viphal_os_strcpy(
    vip_char_t *dest,
    vip_char_t *src
    )
{
    vip_char_t *r = dest;

    if ((VIP_NULL == dest) || (VIP_NULL == src)) {
        vip_error("fail to strcpy, parameter is NULL\n");
        return VIP_NULL;
    }

    /* without strcpy() function in freeRTOS */
    while((*r++ = *src++)!='\0');

    return dest;
}
