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


static vip_status_e vipdrv_dyna_continue_map_kernel_logical(
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

static vip_status_e vipdrv_dyna_continue_unmap_kernel_logical(
    vipdrv_video_mem_handle_t* handle
    )
{
    vip_status_e status = VIP_SUCCESS;
    vipdrv_video_mem_handle_t* ptr = handle;

    ptr->kerl_logical = VIP_NULL;
    ptr->mem_flag &= ~VIPDRV_MEM_FLAG_MAP_KERNEL_LOGICAL;

    return status;
}

static vip_status_e vipdrv_dyna_continue_map_user_logical(
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

static vip_status_e vipdrv_dyna_continue_unmap_user_logical(
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
static vip_status_e vipdrv_dyna_continue_flush_cache(
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

    cpu_physical = allocator->callback.phy_vip2cpu(ptr->physical_table[0]);
    PRINTK_D("flush cache physical=0x%"PRIx64", size=0x%08X, type=%d\n", cpu_physical, size, type);

    status = vipdrv_os_flush_cache(cpu_physical, ptr->user_logical, size, type);
    if (status != VIP_SUCCESS) {
        PRINTK_E("fail to flush cache physical=0x%"PRIx64", size=0x%08X, type=%d\n", cpu_physical, size, type);
    }
#endif
    return status;
}
#endif

static vip_status_e vipdrv_dyna_continue_mem_alloc(
    vipdrv_video_mem_handle_t* handle,
    vipdrv_alloc_param_ptr param
    )
{
    vip_status_e status = VIP_SUCCESS;
    vipdrv_video_mem_handle_t* ptr = handle;
    /*vipdrv_allocator_alloc_param_t* alloc_param = (vipdrv_allocator_alloc_param_t*)param;*/

    ptr->mem_flag |= VIPDRV_MEM_FLAG_CONTIGUOUS;

    ptr->physical_num = 1;
    vipOnError(vipdrv_os_allocate_memory(sizeof(vip_physical_t) * ptr->physical_num,
                                        (void**)&ptr->physical_table));
    vipOnError(vipdrv_os_allocate_memory(sizeof(vip_size_t) * ptr->physical_num,
                                        (void**)&ptr->size_table));

    /* alloc contigous physical address from system */
    /*vipdrv_os_allocate_memory();*/

onError:
    return status;
}

static vip_status_e vipdrv_dyna_continue_mem_free(
    vipdrv_video_mem_handle_t* handle
    )
{
    vipdrv_video_mem_handle_t* ptr = handle;
    if (ptr->kerl_logical) {
        vipdrv_dyna_continue_unmap_kernel_logical(handle);
        ptr->kerl_logical = VIP_NULL;
    }

    if (ptr->user_logical) {
        vipdrv_dyna_continue_unmap_user_logical(handle);
        ptr->user_logical = VIP_NULL;
    }

    if (VIP_NULL != ptr->size_table) {
        vipdrv_os_free_memory((void*)ptr->size_table);
        ptr->size_table = VIP_NULL;
    }
    if (VIP_NULL != ptr->physical_table) {
        vipdrv_os_free_memory((void*)ptr->physical_table);
        ptr->physical_table = VIP_NULL;
    }

    return VIP_SUCCESS;
}

/*
@brief convert CPU physical to VIP physical.
@param cpu_physical. the physical address of CPU domain.
*/
static vip_physical_t vipdrv_dyna_continue_phy_cpu2vip(
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
static vip_physical_t vipdrv_dyna_continue_phy_vip2cpu(
    vip_physical_t vip_physical
    )
{
    vip_physical_t cpu_hysical = vip_physical;
    return cpu_hysical;
}

/*
dynamic allocator:
    A contigous physical address that is dynamic alloc from system.
*/
vip_status_e allocator_init_freertos_dyna_continue(
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
    allocator->callback.allocate = vipdrv_dyna_continue_mem_alloc;
    allocator->callback.free = vipdrv_dyna_continue_mem_free;
    allocator->callback.map_kernel_logical = vipdrv_dyna_continue_map_kernel_logical;
    allocator->callback.unmap_kernel_logical = vipdrv_dyna_continue_unmap_kernel_logical;
    allocator->callback.map_user_logical = vipdrv_dyna_continue_map_user_logical;
    allocator->callback.unmap_user_logical = vipdrv_dyna_continue_unmap_user_logical;
#if vpmdENABLE_FLUSH_CPU_CACHE
    allocator->callback.flush_cache = vipdrv_dyna_continue_flush_cache;
#endif
    allocator->callback.phy_vip2cpu = vipdrv_dyna_continue_phy_cpu2vip;
    allocator->callback.phy_cpu2vip = vipdrv_dyna_continue_phy_vip2cpu;

    return VIP_SUCCESS;
}
#endif
