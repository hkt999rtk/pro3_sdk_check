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
#if vpmdENABLE_VIDEO_MEMORY_HEAP
#include <vip_drv_context.h>
#include "vip_drv_device_driver.h"

#undef VIPDRV_LOG_ZONE
#define VIPDRV_LOG_ZONE  VIPDRV_LOG_ZONE_VIDEO_MEMORY


static vip_status_e vipdrv_heap_map_kernel_logical(
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

static vip_status_e vipdrv_heap_unmap_kernel_logical(
    vipdrv_video_mem_handle_t* handle
    )
{
    vip_status_e status = VIP_SUCCESS;
    vipdrv_video_mem_handle_t* ptr = handle;

    ptr->kerl_logical = VIP_NULL;
    ptr->mem_flag &= ~VIPDRV_MEM_FLAG_MAP_KERNEL_LOGICAL;

    return status;
}

static vip_status_e vipdrv_heap_map_user_logical(
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

static vip_status_e vipdrv_heap_unmap_user_logical(
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
static vip_status_e vipdrv_heap_flush_cache(
    vipdrv_video_mem_handle_t* handle,
    vip_uint8_t type
    )
{
    vip_status_e status = VIP_SUCCESS;
#if vpmdENABLE_VIDEO_MEMORY_CACHE
    vipdrv_video_mem_handle_t* ptr = handle;
    vip_physical_t cpu_physical = 0;
    vip_size_t size = ptr->size_table[0];
    vipdrv_allocator_t* allocator = ptr->allocator;
    vip_uint8_t* flush_logical = VIP_NULL;

    cpu_physical = allocator->callback.phy_vip2cpu(ptr->physical_table[0]);
    PRINTK_D("flush cache physical=0x%"PRIx64", size=0x%08X, type=%d\n", cpu_physical, size, type);

    if ((ptr->mem_flag & VIPDRV_MEM_FLAG_MAP_KERNEL_LOGICAL) == VIPDRV_MEM_FLAG_MAP_KERNEL_LOGICAL) {
        flush_logical = ptr->kerl_logical;
    }
    else if ((ptr->mem_flag & VIPDRV_MEM_FLAG_MAP_USER_LOGICAL) == VIPDRV_MEM_FLAG_MAP_USER_LOGICAL) {
        flush_logical = ptr->user_logical;
    }

    status = vipdrv_os_flush_cache(cpu_physical, flush_logical, size, type);
    if (status != VIP_SUCCESS) {
        PRINTK_E("fail to flush cache physical=0x%"PRIx64", size=0x%08X, type=%d\n", cpu_physical, size, type);
    }
#endif
    return status;
}
#endif

static vip_status_e vipdrv_heap_mem_alloc(
    vipdrv_video_mem_handle_t* handle,
    vipdrv_alloc_param_ptr param
    )
{
    vipdrv_video_mem_handle_t* ptr = handle;
    ptr->mem_flag |= VIPDRV_MEM_FLAG_CONTIGUOUS;

    return VIP_SUCCESS;
}

static vip_status_e vipdrv_heap_mem_free(
    vipdrv_video_mem_handle_t* handle
    )
{
    vipdrv_video_mem_handle_t* ptr = handle;
    if (ptr->kerl_logical) {
        vipdrv_heap_unmap_kernel_logical(handle);
        ptr->kerl_logical = VIP_NULL;
    }

    if (ptr->user_logical) {
        vipdrv_heap_unmap_user_logical(handle);
        ptr->user_logical = VIP_NULL;
    }

    return VIP_SUCCESS;
}

/*
heap allocator:
    A contigous physical address that is not used by system and reserved for NPU used.
    The physical address manage by heap-management at xxx_kernel_heap.c
*/
vip_status_e allocator_init_freertos_heap(
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
    allocator->callback.allocate = vipdrv_heap_mem_alloc;
    allocator->callback.free = vipdrv_heap_mem_free;
    allocator->callback.map_kernel_logical = vipdrv_heap_map_kernel_logical;
    allocator->callback.unmap_kernel_logical = vipdrv_heap_unmap_kernel_logical;
    allocator->callback.map_user_logical = vipdrv_heap_map_user_logical;
    allocator->callback.unmap_user_logical = vipdrv_heap_unmap_user_logical;
#if vpmdENABLE_FLUSH_CPU_CACHE
    allocator->callback.flush_cache = vipdrv_heap_flush_cache;
#endif
    /* the phy_vip2cpu and phy_cpu2vip callback will be overwirte in vipdrv_alloc_mgt_heap_init() function
       if set heap_infos.ops.phy_cpu2vip/phy_vip2cpu in vipdrv_drv_get_memory_config().
    */
    allocator->callback.phy_vip2cpu = vipdrv_drv_get_cpuphysical;
    allocator->callback.phy_cpu2vip = vipdrv_drv_get_vipphysical;

    return VIP_SUCCESS;
}
#endif
