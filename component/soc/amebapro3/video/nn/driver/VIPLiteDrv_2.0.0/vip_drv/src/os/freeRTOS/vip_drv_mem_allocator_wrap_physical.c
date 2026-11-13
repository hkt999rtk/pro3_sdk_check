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

#include <vip_drv_mem_allocator.h>
#include <vip_drv_context.h>
#include "vip_drv_device_driver.h"

#undef VIPDRV_LOG_ZONE
#define VIPDRV_LOG_ZONE  VIPDRV_LOG_ZONE_VIDEO_MEMORY


static vip_status_e vipdrv_map_kernel_logical_wrapphy(
    vipdrv_video_mem_handle_t* handle
    )
{
    vip_status_e status = VIP_SUCCESS;
    vip_physical_t physical = 0;
    vipdrv_video_mem_handle_t* ptr = handle;
    vipdrv_allocator_t* allocator = ptr->allocator;
    physical = allocator->callback.phy_vip2cpu(ptr->physical_table[0]);

    ptr->kerl_logical = (vip_uint8_t*)physical;
    ptr->mem_flag |= VIPDRV_MEM_FLAG_MAP_KERNEL_LOGICAL;

    return status;
}

static vip_status_e vipdrv_unmap_kernel_logical_wrapphy(
    vipdrv_video_mem_handle_t* handle
    )
{
    vip_status_e status = VIP_SUCCESS;
    vipdrv_video_mem_handle_t* ptr = handle;

    ptr->kerl_logical = VIP_NULL;
    ptr->mem_flag &= ~VIPDRV_MEM_FLAG_MAP_KERNEL_LOGICAL;

    return status;
}

static vip_status_e vipdrv_map_user_logical_wrapphy(
    vipdrv_video_mem_handle_t* handle
    )
{
    vip_status_e status = VIP_SUCCESS;
    vip_physical_t physical = 0;
    vipdrv_video_mem_handle_t* ptr = handle;
    vipdrv_allocator_t* allocator = ptr->allocator;
    physical = allocator->callback.phy_vip2cpu(ptr->physical_table[0]);

    ptr->user_logical = (vip_uint8_t*)physical;
    ptr->mem_flag |= VIPDRV_MEM_FLAG_MAP_USER_LOGICAL;

    return status;
}

static vip_status_e vipdrv_unmap_user_logical_wrapphy(
    vipdrv_video_mem_handle_t* handle
    )
{
    vip_status_e status = VIP_SUCCESS;
    vipdrv_video_mem_handle_t* ptr = handle;

    ptr->user_logical = VIP_NULL;
    ptr->mem_flag &= ~VIPDRV_MEM_FLAG_MAP_USER_LOGICAL;

    return status;
}

#if vpmdENABLE_FLUSH_CPU_CACHE
static vip_status_e vipdrv_flush_cache_wrapphy(
    vipdrv_video_mem_handle_t* handle,
    vip_uint8_t type
    )
{
    vip_status_e status = VIP_SUCCESS;
    vipdrv_video_mem_handle_t* ptr = handle;
    vip_physical_t cpu_physical = 0;
    vip_size_t size = 0;
    vip_uint32_t i = 0;
    vipdrv_allocator_t* allocator = ptr->allocator;

    for (i = 0; i < ptr->physical_num; i++) {
        cpu_physical = allocator->callback.phy_vip2cpu(ptr->physical_table[i]);
        size = ptr->size_table[i];
        PRINTK_D("flush wrapphy cache physical=0x%"PRIx64", size=0x%08X, type=%d\n", cpu_physical, size, type);

        status = vipdrv_os_flush_cache(cpu_physical, ptr->user_logical, size, type);
        if (status != VIP_SUCCESS) {
            PRINTK_E("fail to flush cache physical=0x%"PRIx64", size=0x%08X, type=%d\n", cpu_physical, size, type);
        }
    }

    return status;
}
#endif

static vip_status_e vipdrv_mem_alloc_wrapphy(
    vipdrv_video_mem_handle_t* handle,
    vipdrv_alloc_param_ptr param
    )
{
    vip_status_e status = VIP_SUCCESS;
    vipdrv_video_mem_handle_t* ptr = handle;
    vipdrv_allocator_t* allocator = ptr->allocator;
    vip_size_t total_size = 0;
    vip_uint32_t i = 0;
    vipdrv_allocator_wrapphy_param_t *wrap_param = (vipdrv_allocator_wrapphy_param_t*)param;
    vip_uint32_t physical_num = wrap_param->phy_num;

    if (1 != physical_num) {
        PRINTK_E("fail to wrap user physical, physical_num=%d\n", physical_num);
        vipGoOnError(VIP_ERROR_IO);
    }

    for (i = 0; i < physical_num; i++) {
        total_size += wrap_param->size_table[i];
    }
    ptr->size = total_size;
    ptr->physical_num = physical_num;

    vipOnError(vipdrv_os_allocate_memory(sizeof(vip_physical_t) * physical_num, (void**)&ptr->physical_table));
    vipOnError(vipdrv_os_allocate_memory(sizeof(vip_size_t) * physical_num, (void**)&ptr->size_table));

    for (i = 0; i < physical_num; i++) {
        ptr->physical_table[i] = allocator->callback.phy_cpu2vip(wrap_param->phy_table[i]);
        ptr->size_table[i] = wrap_param->size_table[i];
    }

    ptr->mem_flag |= VIPDRV_MEM_FLAG_NONE_CACHE;
    ptr->mem_flag |= VIPDRV_MEM_FLAG_CONTIGUOUS;
    return status;

onError:
    if (VIP_NULL != ptr->physical_table) {
        vipdrv_os_free_memory(ptr->physical_table);
        ptr->physical_table = VIP_NULL;
    }
    if (VIP_NULL != ptr->size_table) {
        vipdrv_os_free_memory(ptr->size_table);
        ptr->size_table = VIP_NULL;
    }
    return status;
}

static vip_status_e vipdrv_mem_free_wrapphy(
    vipdrv_video_mem_handle_t* handle
    )
{
    vipdrv_video_mem_handle_t* ptr = handle;

    if (VIP_NULL == ptr->physical_table) {
        return VIP_SUCCESS;
    }

    if (ptr->kerl_logical) {
        vipdrv_unmap_kernel_logical_wrapphy(handle);
    }

    if (ptr->user_logical) {
        vipdrv_unmap_user_logical_wrapphy(handle);
    }

    if (VIP_NULL != ptr->physical_table) {
        vipdrv_os_free_memory(ptr->physical_table);
        ptr->physical_table = VIP_NULL;
    }
    if (VIP_NULL != ptr->size_table) {
        vipdrv_os_free_memory(ptr->size_table);
        ptr->size_table = VIP_NULL;
    }

    return VIP_SUCCESS;
}

/*
wrap allocator:
    Wrap the physical address specified by application so that NPU can access the memory.
    Wrap the physical specified by vip_create_buffer_from_physical() API.
*/
vip_status_e allocator_init_freertos_wrap_physical(
    vipdrv_allocator_t* allocator,
    vip_char_t* name,
    vipdrv_allocator_type_e type
    )
{
    if (VIP_NULL == allocator) {
        return VIP_ERROR_INVALID_ARGUMENTS;
    }

    allocator->mem_flag = VIPDRV_MEM_FLAG_CONTIGUOUS |
                          VIPDRV_MEM_FLAG_4GB_ADDR_PA;
    allocator->name = name;
    allocator->alctor_type = type;
    allocator->callback.allocate = vipdrv_mem_alloc_wrapphy;
    allocator->callback.free = vipdrv_mem_free_wrapphy;
    allocator->callback.map_kernel_logical = vipdrv_map_kernel_logical_wrapphy;
    allocator->callback.unmap_kernel_logical = vipdrv_unmap_kernel_logical_wrapphy;
    allocator->callback.map_user_logical = vipdrv_map_user_logical_wrapphy;
    allocator->callback.unmap_user_logical = vipdrv_unmap_user_logical_wrapphy;
#if vpmdENABLE_FLUSH_CPU_CACHE
    allocator->callback.flush_cache = vipdrv_flush_cache_wrapphy;
#endif
    allocator->callback.phy_vip2cpu = vipdrv_drv_get_cpuphysical;
    allocator->callback.phy_cpu2vip = vipdrv_drv_get_vipphysical;

    return VIP_SUCCESS;
}
