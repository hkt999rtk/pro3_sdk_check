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


static vip_status_e vipdrv_map_kernel_logical_wrapmem(
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

static vip_status_e vipdrv_unmap_kernel_logical_wrapmem(
    vipdrv_video_mem_handle_t* handle
    )
{
    vip_status_e status = VIP_SUCCESS;
    vipdrv_video_mem_handle_t* ptr = handle;

    ptr->kerl_logical = VIP_NULL;
    ptr->mem_flag &= ~VIPDRV_MEM_FLAG_MAP_KERNEL_LOGICAL;

    return status;
}

static vip_status_e vipdrv_map_user_logical_wrapmem(
    vipdrv_video_mem_handle_t* handle
    )
{
    vip_status_e status = VIP_SUCCESS;

    return status;
}

static vip_status_e vipdrv_unmap_user_logical_wrapmem(
    vipdrv_video_mem_handle_t* handle
    )
{
    vip_status_e status = VIP_SUCCESS;

    return status;
}

#if vpmdENABLE_FLUSH_CPU_CACHE
static vip_status_e vipdrv_flush_cache_wrapmem(
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

static vip_status_e vipdrv_mem_alloc_wrapmem(
    vipdrv_video_mem_handle_t* handle,
    vipdrv_alloc_param_ptr param
    )
{
    vip_status_e status = VIP_SUCCESS;
    vipdrv_video_mem_handle_t* ptr = handle;
    vipdrv_allocator_t* allocator = ptr->allocator;
    vipdrv_allocator_wrapmem_param_t *wrap_param = (vipdrv_allocator_wrapmem_param_t*)param;
    vip_uint32_t physical_num = 1;

    ptr->size = wrap_param->size;
    ptr->user_logical = wrap_param->user_logical;

    vipOnError(vipdrv_os_allocate_memory(sizeof(vip_physical_t) * physical_num, (void**)&ptr->physical_table));
    vipOnError(vipdrv_os_allocate_memory(sizeof(vip_size_t) * physical_num, (void**)&ptr->size_table));

    ptr->physical_table[0] = allocator->callback.phy_cpu2vip((vip_physical_t)((uint32_t)ptr->user_logical));
    ptr->size_table[0] = ptr->size;
    ptr->physical_num = physical_num;

    ptr->mem_flag |= VIPDRV_MEM_FLAG_NONE_CACHE;
    ptr->mem_flag |= VIPDRV_MEM_FLAG_CONTIGUOUS;
    ptr->mem_flag |= VIPDRV_MEM_FLAG_MAP_USER_LOGICAL;
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

static vip_status_e vipdrv_mem_free_wrapmem(
    vipdrv_video_mem_handle_t* handle
    )
{
    vipdrv_video_mem_handle_t* ptr = handle;

    if (VIP_NULL == ptr->physical_table) {
        return VIP_SUCCESS;
    }

    if (ptr->kerl_logical) {
        vipdrv_unmap_kernel_logical_wrapmem(handle);
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
@brief convert CPU physical to VIP physical.
@param cpu_physical. the physical address of CPU domain.
*/
static vip_physical_t vipdrv_mem_phy_cpu2vip_wrapmem(
    vip_physical_t cpu_physical
    )
{
    vip_physical_t vip_hysical = cpu_physical;
    return vip_hysical;
}

/*
@brief convert VIP physical to CPU physical.
@param vip_physical. the physical address of VIP domain.
*/
static vip_physical_t vipdrv_mem_phy_vip2cpu_wrapmem(
    vip_physical_t vip_physical
    )
{
    vip_physical_t cpu_hysical = vip_physical;
    return cpu_hysical;
}

/*
wrap allocator:
    Wrap the memory specified by application so that NPU can access the memory.
    Wrap the memory specified by vip_create_buffer_from_handle() API.
    Only can be used when NPU's MMU is enabled.
*/
vip_status_e allocator_init_freertos_wrap_memory(
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
    allocator->callback.allocate = vipdrv_mem_alloc_wrapmem;
    allocator->callback.free = vipdrv_mem_free_wrapmem;
    allocator->callback.map_kernel_logical = vipdrv_map_kernel_logical_wrapmem;
    allocator->callback.unmap_kernel_logical = vipdrv_unmap_kernel_logical_wrapmem;
    allocator->callback.map_user_logical = vipdrv_map_user_logical_wrapmem;
    allocator->callback.unmap_user_logical = vipdrv_unmap_user_logical_wrapmem;
#if vpmdENABLE_FLUSH_CPU_CACHE
    allocator->callback.flush_cache = vipdrv_flush_cache_wrapmem;
#endif
    allocator->callback.phy_vip2cpu = vipdrv_mem_phy_vip2cpu_wrapmem;
    allocator->callback.phy_cpu2vip = vipdrv_mem_phy_cpu2vip_wrapmem;

    return VIP_SUCCESS;
}
