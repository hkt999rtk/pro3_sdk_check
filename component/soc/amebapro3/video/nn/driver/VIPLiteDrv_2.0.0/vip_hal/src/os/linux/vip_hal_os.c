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

#include <vip_hal_os_port.h>
#include <vip_drv_os.h>
#include <vip_drv_share.h>
#include "vip_hal_os.h"
#if defined(__ANDROID__)
#include <android/log.h>
#include <cutils/properties.h>
#endif

typedef struct _sem_linux
{
    /* The mutex. */
    pthread_mutex_t mutex;

    /* The condition. */
    pthread_cond_t cond;

    /* The value. */
    int value;
} sem_linux_t, *sem_linux_t_ptr;

typedef struct _viphal_os
{
    vip_int32_t device_fd;
} viphal_os_t;

/*
@brief get process id.
*/
vip_uint32_t viphal_os_get_pid(void)
{
    vip_uint32_t pid = 0;

    pid = (vip_uint32_t)getpid();
    return pid;
}

/*
@brief get thread id.
*/
vip_uint32_t viphal_os_get_tid(void)
{
    vip_uint32_t tid = 0;

    tid = (vip_uint32_t)pthread_self();
    return tid;
}

/*
@brief initialize OS env.
@param mem_size, the size of video memory.
*/
vip_status_e viphal_os_init(
    void **os
    )
{
    const char *device_name = "/dev/vipcore";
    vip_status_e status = VIP_SUCCESS;
    viphal_os_t *private = VIP_NULL;

    viphal_allocate_memory(sizeof(viphal_os_t), (void **)&private);
    if (VIP_NULL == private) {
        vip_error("fail to allocate meory for os private\n");
        return VIP_ERROR_OUT_OF_MEMORY;
    }
    viphal_os_zero_memory(private, sizeof(viphal_os_t));

    private->device_fd = open(device_name, O_RDWR);
    if (private->device_fd == -1) {
        vip_error("fail to open device %s\n", device_name);
        vipGoOnError(VIP_ERROR_IO);
    }
    else {
        vip_debug("open device %s successfully\n", device_name);
    }

    *os = private;

    return status;

onError:
    if (private->device_fd != -1) {
        close(private->device_fd);
        private->device_fd = -1;
    }

    if (private != VIP_NULL) {
        viphal_free_memory(private);
        private = VIP_NULL;
    }

    return status;
}

/*
@brief uninitialize OS env.
@param os, os object.
*/
vip_status_e viphal_os_uninit(
    void **os
    )
{
    viphal_os_t *private = (viphal_os_t*)(*os);

    if (VIP_NULL == private) {
        vip_error("failed os is NULL in uninit\n");
        return VIP_ERROR_OUT_OF_MEMORY;
    }

    if (private->device_fd != -1) {
        close(private->device_fd);
        private->device_fd = -1;
    }

    if (private != VIP_NULL) {
        viphal_free_memory(private);
        private = VIP_NULL;
    }

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
    void *allo_memory = VIP_NULL;

    if (size == 0 || !memory) {
        return VIP_ERROR_INVALID_ARGUMENTS;
    }

    allo_memory = malloc(size);
    if (!allo_memory) {
        return VIP_ERROR_OUT_OF_MEMORY;
    }

    *memory = allo_memory;

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

    free(memory);

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
    vip_uint8_t *data = (vip_uint8_t*)memory;

    if (!memory) {
        vip_error("fail to zero memory, memory is NULL\n");
        return VIP_ERROR_INVALID_ARGUMENTS;
    }

    #if 0
    {
    vip_size_t i = 0;
    for (i = 0; i < size; i++) {
        data[i] = 0;
    }
    }
    #else
    memset(data, 0, size);
    #endif

    return VIP_SUCCESS;
}

/*
@brief call kernel space functions.
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
    vip_status_e status = VIP_SUCCESS;
    viphal_os_t *private = (viphal_os_t*)os;
    struct ioctl_data ioctl_data;

    vip_uint32_t cmd_bytes[VIPDRV_INTRFACE_MAX] = {
        sizeof(vipdrv_initialize_t),          /*    VIPDRV_INTRFACE_INIT = 0,               */
        0,                                    /*    VIPDRV_INTRFACE_DESTROY,                */
        sizeof(vipdrv_reg_t),                 /*    VIPDRV_INTRFACE_READ_REG,               */
        sizeof(vipdrv_reg_t),                 /*    VIPDRV_INTRFACE_WRITE_REG,              */
        sizeof(vipdrv_wait_task_param_t),     /*    VIPDRV_INTRFACE_WAIT_TASK,              */
        sizeof(vipdrv_submit_task_param_t),   /*    VIPDRV_INTRFACE_SUBMIT_TASK,            */
        sizeof(vipdrv_allocation_t),          /*    VIPDRV_INTRFACE_ALLOCATION,             */
        sizeof(vipdrv_deallocation_t),        /*    VIPDRV_INTRFACE_DEALLOCATION,           */
        0,                                    /*    NOT USED,                          */
        sizeof(vipdrv_query_address_info_t),  /*    VIPDRV_INTRFACE_QUERY_ADDRESS_INFO      */
        sizeof(vipdrv_query_database_t),      /*    VIPDRV_INTRFACE_QUERY_DATABASE          */
        sizeof(vipdrv_operate_cache_t),       /*    VIPDRV_INTRFACE_OPERATE_CACHE           */
        sizeof(vipdrv_wrap_user_memory_t),    /*    VIPDRV_INTRFACE_WRAP_USER_MEMORY        */
        sizeof(vipdrv_deallocation_t),        /*    VIPDRV_INTRFACE_UNWRAP_USER_MEMORY      */
        0,                                    /*    NOT USED,                          */
        sizeof(vipdrv_power_management_t),    /*    VIPDRV_INTRFACE_POWER_MANAGEMENT        */
        sizeof(vipdrv_wrap_user_physical_t),  /*    VIPDRV_INTRFACE_WRAP_USER_PHYSICAL      */
        sizeof(vipdrv_deallocation_t),        /*    VIPDRV_INTRFACE_UNWRAP_USER_PHYSICAL    */
        sizeof(vipdrv_wrap_user_fd_t),
        sizeof(vipdrv_deallocation_t),        /*    VIPDRV_INTRFACE_UNWRAP_USER_FD          */
        sizeof(vipdrv_query_profiling_t),
        sizeof(vipdrv_query_driver_status_t),
        sizeof(vipdrv_query_mmu_info_t),
        sizeof(vipdrv_map_user_logical_t),
        sizeof(vipdrv_unmap_user_logical_t),
        sizeof(vipdrv_cancel_task_param_t),   /*    VIPDRV_INTRFACE_CANCEL_TASK                  */
        sizeof(vipdrv_query_register_dump_t), /*    VIPDRV_INTRFACE_QUERY_REGISTER_DUMP     */
        sizeof(vipdrv_create_task_param_t),   /*    VIPDRV_INTRFACE_CREATE_TASK             */
        sizeof(vipdrv_set_task_property_t),   /*    VIPDRV_INTRFACE_SET_TASK_PROPERTY       */
        sizeof(vipdrv_destroy_task_param_t),  /*    VIPDRV_INTRFACE_DESTROY_TASK            */
    };

    if (VIP_NULL == private) {
        vip_error("failed os is NULL, command: %s\n", ioctl_cmd_string[command]);
        return VIP_ERROR_OUT_OF_MEMORY;
    }

    if (command >= VIPDRV_INTRFACE_MAX) {
        vip_error("command is %d, the maximun comand is %d\n", command, VIPDRV_INTRFACE_MAX);
        return VIP_ERROR_OUT_OF_MEMORY;
    }

    ioctl_data.command = command;
    ioctl_data.buffer = VIPPTR_TO_UINT64(data);
    ioctl_data.bytes = cmd_bytes[command];

    if (ioctl(private->device_fd, VIPDRV_IOCTL, &ioctl_data) < 0) {
        if (ioctl_data.error != VIP_SUCCESS) {
            status = ioctl_data.error;
        }
        if ((status != VIP_SUCCESS) && (status != VIP_ERROR_CANCELED)) {
            vip_error("fail to ioctl vipcore, command[%d]:%s, status=%d\n",
                        command, ioctl_cmd_string[command], status);
        }
        return status;
    }

    return status;
}

/*
@brief print message on console
@param message, which message should be print.
*/
vip_status_e viphal_os_print(
    vip_log_level level,
    IN const vip_char_t *message,
    ...
    )
{
    vip_status_e status = VIP_SUCCESS;
#define DEFAULT_ENABLE_LOG     1

#if (vpmdENABLE_DEBUG_LOG > 2)
    if ((level == VIP_LOG_DEBUG) || (level == VIP_LOG_INFO)) {
        static vip_uint32_t log_val = DEFAULT_ENABLE_LOG;
        static vip_uint8_t read_env = 0;
        if (0 == read_env) {
            #if defined(__ANDROID__)
            char env[PROPERTY_VALUE_MAX];
            if (property_get("vendor.vip_log_level", env, VIP_NULL) != 0) {
                log_val = atoi(env);
            }
            #else
            char *env = VIP_NULL;
            env = getenv("VIP_LOG_LEVEL");
            if (env) {
                log_val = atoi(env);
            }
            #endif
            read_env = 1;
        }

        if (0 == log_val) {
            return VIP_SUCCESS;
        }
    }
#endif

#if defined(__ANDROID__)
    #define MAX_MSG_LENGTH 1024
    vip_char_t arg_buffer[MAX_MSG_LENGTH] = {0};
    va_list arg;
    va_start(arg, message);
    vsnprintf(arg_buffer, MAX_MSG_LENGTH, message, arg);
    va_end(arg);
    __android_log_print(ANDROID_LOG_INFO, "NPU-viplite", "%s", arg_buffer);
#else
    va_list arg;
    va_start(arg, message);
    vprintf(message, arg);
    va_end(arg);
#endif

    return status;
}

/*
@brief print string into a file(stream).
@param os object, create in viphal_os_init().
@param buffer, which memory should be print.
*/
vip_status_e viphal_os_snprint(
    OUT vip_char_t  *buffer,
    IN vip_uint32_t size,
    IN  const vip_char_t *message,
    ...
    )
{
    vip_status_e status = VIP_SUCCESS;

    va_list varg;
    va_start(varg, message);
    vsnprintf(buffer, size, message, varg);
    va_end(varg);

    return status;
}

/*
@brief open a file.
@param name, file name.
@param mode, opem mode.
*/
viphal_file_t viphal_os_open_file(
    vip_char_t *name,
    viphal_file_mode_e mode
    )
{
    static const vip_char_t *mode_string[] = {
        "wb",
        "ab+",
        "rb",
        "w",
        "a",
        "r",
    };

    FILE *file = fopen(name, mode_string[mode]);

    return (viphal_file_t *)file;
}

vip_status_e viphal_os_read_file(
    void *dst,
    vip_size_t size,
    viphal_file_t file
    )
{
    vip_status_e status = VIP_SUCCESS;
    FILE *ffile = (FILE *)file;
    vip_int32_t value = 0;

    value = fread(dst, size, 1, ffile);
    if (value != 1) {
        vip_uint32_t* tmp = (vip_uint32_t*)dst;
        *tmp = 0;
        vip_error("fail to read file\n");
        status = VIP_ERROR_IO;
    }

    return status;
}

vip_status_e viphal_os_write_file(
    void *data,
    vip_size_t size,
    viphal_file_t file
    )
{
    vip_status_e status = VIP_SUCCESS;
    FILE *ffile = (FILE *)file;
    vip_int32_t value = 0;

    value = fwrite(data, size, 1, ffile);
    if (value == 0) {
        status = VIP_ERROR_IO;
        vip_error("fail to write file\n");
    }

    return status;
}

void viphal_os_close_file(
    viphal_file_t file
    )
{
    fflush((FILE *)file);
    fclose((FILE *)file);
}

/*
@brief seek file location.
*/
vip_status_e viphal_os_seek_file(
    vip_size_t offset,
    viphal_file_seek_e pos,
    viphal_file_t file
    )
{
#define ORIGIN(pos) ((pos) & VIPHAL_FILE_SET) ? SEEK_SET : \
                    ((pos) & VIPHAL_FILE_END) ? SEEK_END : SEEK_CUR
    vip_status_e status = VIP_SUCCESS;
    int origin = ORIGIN(pos);
    long tmp = (unsigned long) - 1 >> 1;
    if (offset > (vip_size_t)tmp) {
        vip_error("fseek overlap\n");
        return VIP_ERROR_IO;
    }
    tmp = (VIPHAL_FILE_BACKWARD & pos) ? -(long)offset : (long)offset;
    if (0 != fseek((FILE*)file, tmp, origin)) {
        status = VIP_ERROR_IO;
    }

    return status;
#undef ORIGIN
}

/*
@brief print string into a file(stream).
@param os object, create in viphal_os_init().
@param buffer, which memory should be print.
*/
vip_status_e viphal_os_print_string(
    IN void *os,
    IN void *stream,
    IN vip_char_t *buffer
    )
{
    vip_status_e status = VIP_SUCCESS;

    if (stream != VIP_NULL) {
        fprintf((FILE*)stream, "%s", buffer);
        fflush((FILE *)stream);
    }
    else {
        vip_error("stream is NULL\n");
        vipGoOnError(VIP_ERROR_IO);
    }

onError:
    return status;
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
    vip_status_e error = VIP_SUCCESS;
    vip_int32_t ret = 0;
    pthread_mutex_t *pmutex = VIP_NULL;

    pmutex = (pthread_mutex_t *)malloc(sizeof(pthread_mutex_t));
    if (pmutex == VIP_NULL) {
        vip_error("fail to allocate for pmutex\n");
        return VIP_ERROR_IO;
    }

    ret = pthread_mutex_init(pmutex, VIP_NULL);
    if(ret != 0) {
        vip_error("create mutex failed.\n");
        error = VIP_ERROR_NOT_SUPPORTED;
    }

    *mutex = pmutex;

    return error;
}

/*
@brief lock mutex.
@param mutex. mutex object
*/
vip_status_e viphal_os_lock_mutex(
    vip_ptr mutex
    )
{
    vip_status_e error = VIP_SUCCESS;
    vip_int32_t ret = 0;

    if (mutex == VIP_NULL) {
        vip_error("fail to lock, mutex is NULL\n");
        return VIP_ERROR_OUT_OF_MEMORY;
    }

    ret = pthread_mutex_lock((pthread_mutex_t*)mutex);
    if(ret != 0) {
        vip_error("a thread lock mutex failed.\n");
        error = VIP_ERROR_NOT_SUPPORTED;
    }

    return error;
}

/*
@brief unlock mutex.
@param mutex. mutex object
*/
vip_status_e viphal_os_unlock_mutex(
    vip_ptr mutex
    )
{
    vip_status_e error = VIP_SUCCESS;
    vip_int32_t ret = 0;

    if (mutex == VIP_NULL) {
        vip_error("fail to unlock, mutex is NULL\n");
        return VIP_ERROR_OUT_OF_MEMORY;
    }

    ret = pthread_mutex_unlock((pthread_mutex_t*)mutex);
    if(ret != 0) {
        vip_error("a thread unlock mutex failed.\n");
        error = VIP_ERROR_NOT_SUPPORTED;
    }

    return error;
}

/*
@brief destroy mutex.
@param mutex. mutex object
*/
vip_status_e viphal_os_destroy_mutex(
    vip_ptr mutex
    )
{
    if (mutex == VIP_NULL) {
        vip_error("fail to destory, mutex is NULL\n");
        return VIP_ERROR_OUT_OF_MEMORY;
    }

    pthread_mutex_destroy((pthread_mutex_t*)mutex);
    free(mutex);

    return VIP_SUCCESS;
}
#endif

/*
@brief get the current system time.
return the time of day in microseconds.
*/
vip_uint64_t viphal_os_get_time(void)
{
    struct timeval tv;
    vip_uint64_t time = 0;

    gettimeofday(&tv, 0);
    time = (tv.tv_sec * (vip_uint64_t)1000000) + tv.tv_usec;

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
    return strcpy(dest, src);
}
