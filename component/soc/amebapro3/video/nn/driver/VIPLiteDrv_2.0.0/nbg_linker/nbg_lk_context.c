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

#include <nbg_lk_context.h>
#include <vip_hal_os_port.h>
#include <vip_lite_version.h>
#include <nbg_lk_debug.h>
#include <nbg_lk_capture.h>


/* A global context object. */
nbglk_context_t lite_context = {0};
nbglk_context_t *global_context = &lite_context;


#if vpmdENABLE_MULTIPLE_TASK
#if defined(LINUX)
pthread_mutex_t init_mutex = PTHREAD_MUTEX_INITIALIZER;
#endif
#endif

nbglk_context_t* nbglk_get_context(void)
{
    return global_context;
}

static vip_status_e nbglk_context_init(
    nbglk_context_t *context
    )
{
    vip_status_e status = VIP_SUCCESS;
    vip_virtual_t offset = 0;
    vip_uint32_t iter0 = 0, iter1 = 0;
    vipdrv_query_database_t database;

    status = viphal_query_database(&database);
    if (status != VIP_SUCCESS) {
        vip_error("fail to query database\n");
        vipGoOnError(status);
    }
    if (0 == database.device_count) {
        vip_error("fail to init, device count=%d\n", database.device_count);
        vipGoOnError(VIP_ERROR_FAILURE);
    }

    vipOnError(viphal_allocate_memory(sizeof(vip_uint32_t) * database.device_count,
                                        (void **)&context->axi_sram_size));
    vipOnError(viphal_allocate_memory(sizeof(vip_virtual_t) * database.device_count,
                                        (void **)&context->axi_sram_base));

    offset = database.axi_sram_base;
    context->func_config.enable_capture = database.func_config.enable_capture;
    context->func_config.enable_cnn_profile = database.func_config.enable_cnn_profile;
    context->func_config.enable_dump_nbg = database.func_config.enable_dump_nbg;
    context->device_count = database.device_count;
    vipOnError(viphal_allocate_memory(sizeof(nbglk_device_t) * database.device_count,
                                        (void **)&context->devices));
    for (iter0 = 0; iter0 < database.device_count; iter0++) {
        nbglk_device_t *device = &context->devices[iter0];
        device->dev_index = iter0;
        device->core_count = database.device_core_number[iter0];
        context->axi_sram_base[iter0] = offset;
        context->axi_sram_size[iter0] = database.axi_sram_size[iter0];
        offset += database.axi_sram_size[iter0];
        vip_info("device%d axi sram base=0x%x, size=0x%x\n",
            iter0, context->axi_sram_base[iter0], context->axi_sram_size[iter0]);
        vipOnError(viphal_allocate_memory(sizeof(vip_uint32_t) * device->core_count,
                                            (void **)&device->core_id));
        for (iter1 = 0; iter1 < device->core_count; iter1++) {
            device->core_id[iter1] = database.device_core_id[iter0][iter1];
        }
        #if vpmdENABLE_DEBUG_LOG >= 4
        {
            vip_char_t info_t[256];
            vip_char_t *tmp = info_t;
            for (iter1 = 0; iter1 < device->core_count; iter1++) {
                viphal_os_snprint(tmp, 255, "%02d ", device->core_id[iter1]);
                tmp += 3;
            }
            vip_debug("device%d core count=%d, core id[%s]\n",
                        iter0, device->core_count, info_t);
        }
        #endif

    }

    context->hw_target = database.hw_cid;
    context->vip_sram_base = database.vip_sram_base;
    context->vip_sram_size = database.vip_sram_size[0];
    vip_debug("vip sram size=0x%x, address=0x%08X, cid=0x%X, device cnt=%d\n",
                database.vip_sram_size[0], database.vip_sram_base, database.hw_cid, database.device_count);

onError:
    return status;
}

static vip_status_e nbglk_context_uninit(
    nbglk_context_t *context
    )
{
    vip_status_e status = VIP_SUCCESS;
    vip_uint32_t i = 0;

    if (context->axi_sram_base != VIP_NULL) {
        viphal_free_memory(context->axi_sram_base);
        context->axi_sram_base = VIP_NULL;
    }
    if (context->axi_sram_size != VIP_NULL) {
        viphal_free_memory(context->axi_sram_size);
        context->axi_sram_size = VIP_NULL;
    }

    if (context->devices != VIP_NULL) {
        for (i = 0; i < context->device_count; i++) {
            if (context->devices[i].core_id != VIP_NULL) {
                viphal_free_memory(context->devices[i].core_id);
                context->devices[i].core_id = VIP_NULL;
            }
        }
        viphal_free_memory(context->devices);
        context->devices = VIP_NULL;
    }

    return status;
}

/*
@brief Initialize VIPLite software and hardware.
@param sys_mem_size The size of system memory heap.
@pram video_mem_size The size of video memory heap.
*/
vip_status_e nbglk_init(
    IN vip_uint32_t sys_mem_size,
    IN vip_uint32_t video_mem_size
    )
{
    vip_status_e status = VIP_SUCCESS;
    nbglk_context_t *context = nbglk_get_context();

    vip_info("VIPLite driver software version %d.%d.%d.%s\n",
                VERSION_MAJOR, VERSION_MINOR, VERSION_SUB_MINOR, VERSION_PATCH);

#if vpmdENABLE_MULTIPLE_TASK
#if defined(LINUX)
    /* some risk, only one initialize_mutex lock if many thread call vip_init() at the same time
    1. If Linux/Android, please define LINUX for enable static initialize a init_mutex
    2. If rtos, please only call vip_init() once */
    pthread_mutex_lock(&init_mutex);
#endif
    if (VIP_NULL == context->initialize_mutex) {
        vip_info("start create initialize mutex...\n");
        viphal_os_zero_memory(context, sizeof(nbglk_context_t));
        status = viphal_os_create_mutex(&context->initialize_mutex);
        if (status != VIP_SUCCESS) {
            vip_error("fail to create mutex for initialize\n");
            vipGoOnError(status);
        }
    }
    viphal_os_lock_mutex(context->initialize_mutex);
#if vpmdENABLE_CAPTURE
    if (VIP_NULL == context->capture_mutex.mutex) {
        vip_info("start create capture mutex...\n");
        status = viphal_os_create_mutex(&context->capture_mutex.mutex);
        if (status != VIP_SUCCESS) {
            vip_error("fail to create mutex for capture\n");
            vipGoOnError(status);
        }
    }
#endif
#endif

    if (context->initialize > 0) {
        vip_info("vip has been initialized, don't need initialize again\n");
        #if vpmdENABLE_MULTIPLE_TASK
        context->initialize++;
        viphal_os_unlock_mutex(context->initialize_mutex);
        #if defined(LINUX)
        pthread_mutex_unlock(&init_mutex);
        #endif
        #endif

        return VIP_SUCCESS;
    }

    /* init underlying software layers and HW. */
    status = viphal_init(sys_mem_size, video_mem_size,
        (VERSION_MAJOR << 16) | (VERSION_MINOR << 8) | (VERSION_SUB_MINOR));
    if (status != VIP_SUCCESS) {
        vip_error("fail to vip hal init\n");
        vipGoOnError(status);
    }

    status = nbglk_context_init(context);
    if (status != VIP_SUCCESS) {
        vip_error("fail to init context\n");
        vipGoOnError(status);
    }

#if vpmdENABLE_CAPTURE
    vipOnError(nbglk_capture_init(VIP_NULL, VIP_NULL, NBGLK_CAPTURE_TYPE_CAP));
#endif
    context->initialize = 1;

onError:
#if vpmdENABLE_MULTIPLE_TASK
    viphal_os_unlock_mutex(context->initialize_mutex);
#if defined(LINUX)
    pthread_mutex_unlock(&init_mutex);
#endif
#endif

    return status;
}

/*
@brief Destroy VIPLite software and hardware.
*/
vip_status_e nbglk_destroy(void)
{
    vip_status_e status = VIP_SUCCESS;
    nbglk_context_t *context = nbglk_get_context();

#if vpmdENABLE_MULTIPLE_TASK
#if defined(LINUX)
     pthread_mutex_lock(&init_mutex);
#endif

    if (context->initialize_mutex != VIP_NULL) {
        viphal_os_lock_mutex(context->initialize_mutex);
    }
    else {
        vip_error("error initialize mutex is NULL in do destroy\n");
    }
#endif

#if vpmdENABLE_MULTIPLE_TASK
    context->initialize--;

    vip_debug("vip destroy initialize count=%d\n", context->initialize);
    if (context->initialize <= 0) {
#else
    if (context->initialize > 0) {
        context->initialize = 0;
#endif
        #if vpmdENABLE_CAPTURE
        nbglk_capture_uninit(VIP_NULL, VIP_NULL);
        #endif

        nbglk_context_uninit(context);

        status = viphal_destroy();
        if (status != VIP_SUCCESS) {
            vip_error("fail to vip hal destroy\n");
        }

        #if vpmdENABLE_MULTIPLE_TASK
        #if vpmdENABLE_CAPTURE
        if (VIP_NULL != context->capture_mutex.mutex) {
            vip_debug("destroy capture mutex\n");
            viphal_os_destroy_mutex(context->capture_mutex.mutex);
        }
        #endif
        /* unlock and destroy initialize mutex last */
        if (context->initialize_mutex != VIP_NULL) {
            viphal_os_unlock_mutex(context->initialize_mutex);
            vip_debug("destroy initialize mutex\n");
            viphal_os_destroy_mutex(context->initialize_mutex);
            context->initialize_mutex = VIP_NULL;
        }
        #endif

        viphal_zero_memory(context, sizeof(nbglk_context_t));
    }
#if vpmdENABLE_MULTIPLE_TASK
    else {
        if (context->initialize_mutex != VIP_NULL) {
            viphal_os_unlock_mutex(context->initialize_mutex);
        }
    }
#if defined(LINUX)
    pthread_mutex_unlock(&init_mutex);
#endif
#endif

    vip_debug("destroy vip done..\n");
    return status;
}

