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

#include <vip_hal_heap.h>
#include <vip_hal_os_port.h>
#include <vip_drv_share.h>
#include <vip_lite_version.h>
#include <vip_feature_database.h>

/*
@brief call vip-drv space functions.
@param os, os object create in viphal_os_init.
@param command, kenrel space command.
@param data, pass to vip-drv space function data or vip-drv space return data.
*/
vip_status_e viphal_os_call_drv(
    IN void *os,
    IN OUT vipdrv_intrface_e command,
    IN OUT void *data
    );

#if vpmdENABLE_MEMORY_PROFILING
struct node_list {
    void *handle;       /* profile memory handle */
    vip_size_t size;    /* the size of memory */
    struct node_list *prev;
    struct node_list *next;
};

typedef struct _gcvip_profiler {
    vip_size_t   system_memory;
    vip_size_t   system_peak;
    vip_uint32_t system_allocs;
    vip_uint32_t system_frees;

    vip_size_t   video_memory;
    vip_size_t   video_peak;
    vip_uint32_t video_allocs;
    vip_uint32_t video_frees;

    struct node_list node;
} viphal_profiler_t;

#define MEMORY_PROFILING_ADD(handle, size)  \
{                                           \
    profile_add_node(handle, size);         \
    profile_add_video_memory(size);         \
}
#define MEMORY_PROFILING_REMOVE(handle)               \
{                                                           \
    profile_remove_video_memory(profile_get_size(handle));  \
    profile_remove_node(handle);                            \
}
#else
#define MEMORY_PROFILING_ADD(handle, size)
#define MEMORY_PROFILING_REMOVE(handle)
#endif

/* vip-hal context. */
typedef struct _viphal_context
{
#if vpmdENABLE_SYS_MEMORY_HEAP
    heap_t     sys_mem_heap;
    void       *sys_mem_heap_addr;

    #if vpmdENABLE_MULTIPLE_TASK
    void       *heap_mutex;
    #endif
#endif

#if vpmdENABLE_MEMORY_PROFILING
    viphal_profiler_t profiler;
    void*            profiler_mutex;
#endif

#if vpmdENABLE_CAPTURE
    void *capture_file;
#endif

    struct _vip_hw_feature_db* feature_db;

    void *os_private; /* private data for vip-hal os */
} viphal_context_t;

/* User context object. */
viphal_context_t    uContext;
#define GET_CONTEXTU()  viphal_context_t *context = &uContext

#if vpmdENABLE_MEMORY_PROFILING
static vip_int32_t profile_add_node(
    void *handle,
    vip_size_t size
    )
{
    viphal_profiler_t *profiler = VIP_NULL;
    struct node_list *p = VIP_NULL;
    GET_CONTEXTU();
    if (VIP_NULL == context->profiler_mutex) {
        return 0;
    }
    viphal_os_lock_mutex(context->profiler_mutex);
    profiler = &context->profiler;
    p = &profiler->node;

    while (p->next) {
        p = p->next;
    }
    if (viphal_os_allocate_memory(sizeof(struct node_list), (void **)&p->next)) {
        return 1;
    }
    p->next->handle = handle;
    p->next->size = size;
    p->next->prev = p;
    p->next->next = VIP_NULL;
    viphal_os_unlock_mutex(context->profiler_mutex);
    return 0;
}

static void profile_remove_node(
    void *handle
    )
{
    viphal_profiler_t *profiler = VIP_NULL;
    struct node_list *p = VIP_NULL;
    GET_CONTEXTU();
    if (VIP_NULL == context->profiler_mutex) {
        return;
    }
    viphal_os_lock_mutex(context->profiler_mutex);
    profiler = &context->profiler;
    p = &profiler->node;

    while (p && p->handle != handle) {
        p = p->next;
    }

    if (p && p->handle == handle) {
        p->prev->next = p->next;
        if (p->next != VIP_NULL) {
            p->next->prev = p->prev;
        }
        viphal_os_free_memory(p);
    }
    else {
        vip_error("Doesn't find the node\n");
    }
    viphal_os_unlock_mutex(context->profiler_mutex);
}

static vip_size_t profile_get_size(
    void *handle
    )
{
    viphal_profiler_t *profiler = VIP_NULL;
    struct node_list *p = VIP_NULL;
    vip_size_t size = 0;
    GET_CONTEXTU();
    if (VIP_NULL == context->profiler_mutex) {
        return 0;
    }
    viphal_os_lock_mutex(context->profiler_mutex);
    profiler = &context->profiler;
    p = &profiler->node;

    while (p) {
        if (p->handle == handle) {
            break;
        }
        p = p->next;
    }

    if (p != VIP_NULL) {
        size = p->size;
    }
    viphal_os_unlock_mutex(context->profiler_mutex);
    return size;
}

static void profile_report(void)
{
    viphal_profiler_t *profiler = VIP_NULL;
    GET_CONTEXTU();

    profiler = &context->profiler;

    vip_print("========== Memory Statistics ========\n");
    vip_print("Current Video Memory Used: %lld bytes.\n", profiler->video_memory);
    vip_print("Video Peak Memory Used:    %lld bytes.\n", profiler->video_peak);
    vip_print("Video Memory Alloc Count:  %d times.\n", profiler->video_allocs);
    vip_print("Video Memory Free Count:  %d times.\n", profiler->video_frees);

    vip_print("\n");

#if vpmdENABLE_SYS_MEMORY_HEAP
    if (context->sys_mem_heap_addr != VIP_NULL) {
        vip_print("Current System Memory Used: %lld bytes.\n", profiler->system_memory);
        vip_print("System Peak Memory Used:    %lld bytes.\n", profiler->system_peak);
        vip_print("System Memory Alloc Count:  %d times.\n", profiler->system_allocs);
        vip_print("System Memory Free Count:  %d times.\n", profiler->system_frees);
    }
#endif
    vip_print("==============  End =================\n");
}

static void profile_add_video_memory(
    vip_size_t bytes
    )
{
    viphal_profiler_t *profiler = VIP_NULL;
    GET_CONTEXTU();
    if (VIP_NULL == context->profiler_mutex) {
        return;
    }
    viphal_os_lock_mutex(context->profiler_mutex);
    profiler = &context->profiler;

    profiler->video_allocs++;
    profiler->video_memory += bytes;
    if (profiler->video_memory > profiler->video_peak) {
        profiler->video_peak = profiler->video_memory;
    }
    viphal_os_unlock_mutex(context->profiler_mutex);
}

static void profile_remove_video_memory(
    vip_size_t bytes
    )
{
    viphal_profiler_t *profiler = VIP_NULL;
    GET_CONTEXTU();
    if (VIP_NULL == context->profiler_mutex) {
        return;
    }
    viphal_os_lock_mutex(context->profiler_mutex);
    profiler = &context->profiler;

    profiler->video_frees++;
    profiler->video_memory -= bytes;
    viphal_os_unlock_mutex(context->profiler_mutex);
}

static void profile_add_system_memory(
    vip_size_t bytes
    )
{
    viphal_profiler_t *profiler = VIP_NULL;
    GET_CONTEXTU();
    if (VIP_NULL == context->profiler_mutex) {
        return;
    }
    viphal_os_lock_mutex(context->profiler_mutex);
    profiler = &context->profiler;

    profiler->system_allocs++;
    profiler->system_memory += bytes;
    if (profiler->system_memory > profiler->system_peak) {
        profiler->system_peak = profiler->system_memory;
    }
    viphal_os_unlock_mutex(context->profiler_mutex);
}

static void profile_remove_system_memory(
    vip_size_t bytes
    )
{
    viphal_profiler_t *profiler = VIP_NULL;
    GET_CONTEXTU();
    if (VIP_NULL == context->profiler_mutex) {
        return;
    }
    viphal_os_lock_mutex(context->profiler_mutex);
    profiler = &context->profiler;

    profiler->system_frees++;
    profiler->system_memory -= bytes;
    viphal_os_unlock_mutex(context->profiler_mutex);
}
#endif

#if vpmdENABLE_SYS_MEMORY_HEAP
static vip_status_e heap_alloc_memory(
    IN vip_size_t size,
    OUT void **memory
    )
{
    vip_status_e status = VIP_SUCCESS;
    void *memory_alloc = VIP_NULL;
    void *handle = VIP_NULL;
    GET_CONTEXTU();

#if vpmdENABLE_MULTIPLE_TASK
    viphal_os_lock_mutex(context->heap_mutex);
#endif

    if (viphal_heap_max_size(&context->sys_mem_heap) > size) {
        handle = viphal_heap_alloc(&context->sys_mem_heap, size, &memory_alloc);
        if (handle == VIP_NULL) {
            vip_error("fail to allocate system memory from heap, \n");
            vipGoOnError(VIP_ERROR_OUT_OF_MEMORY);
        }
        *memory = memory_alloc;
    }
    else {
        vip_info("system memory heap is not enough, bypass for system allocte\n");
        status = viphal_os_allocate_memory(size, memory);
    }

onError:
#if vpmdENABLE_MULTIPLE_TASK
    viphal_os_unlock_mutex(context->heap_mutex);
 #endif
    return status;
}

static vip_status_e heap_free_memory(
    IN void *memory
    )
{
    vip_status_e status = VIP_SUCCESS;
    GET_CONTEXTU();

#if vpmdENABLE_MULTIPLE_TASK
    viphal_os_lock_mutex(context->heap_mutex);
#endif

    status = viphal_heap_free_mem(&context->sys_mem_heap, memory);

#if vpmdENABLE_MULTIPLE_TASK
    viphal_os_unlock_mutex(context->heap_mutex);
#endif

    return status;
}
#endif

/*
@Brief Initialize resource in vip hal layer.
@param sys_mem_size The size of system memory heap.
@param video_mem_size The size of video memory heap.
@param version The version of viplite.
*/
vip_status_e viphal_init(
    IN vip_uint32_t sys_mem_size,
    IN vip_uint32_t video_mem_size,
    IN vip_uint32_t version
    )
{
    vip_status_e status = VIP_SUCCESS;
    vipdrv_initialize_t init_data;
    vipdrv_query_database_t database;
    vip_uint32_t i = 0;
#if vpmdENABLE_SYS_MEMORY_HEAP
    void *pool = VIP_NULL;
    heap_t *heap = VIP_NULL;
#endif
    GET_CONTEXTU();

    if (version != ((VERSION_MAJOR << 16) | (VERSION_MINOR << 8) | (VERSION_SUB_MINOR))) {
        vip_error("lib version mismatch, NBGLinker.so=0x%08x, VIPHal.so=0x%08x.\n",
            version, (VERSION_MAJOR << 16) | (VERSION_MINOR << 8) | (VERSION_SUB_MINOR));
        vipGoOnError(VIP_ERROR_NOT_SUPPORTED);
    }

    viphal_os_zero_memory(context, sizeof(viphal_context_t));
#if vpmdENABLE_MEMORY_PROFILING
    vipOnError(viphal_os_create_mutex(&context->profiler_mutex));
#endif

    /*1. init vip-hal space OS */
    status = viphal_os_init(&context->os_private);
    if (status != VIP_SUCCESS) {
        vip_error("fail to initialize OS, status=%d.\n", status);
        vipGoOnError(status);
    }

    /*2. init vip-drv space */
    init_data.video_mem_size = video_mem_size;
    init_data.version = version;
    status = viphal_os_call_drv(context->os_private, VIPDRV_INTRFACE_INIT, &init_data);
    if (status != VIP_SUCCESS) {
        vip_error("fail to call vip-drv for cmd init, status=%d.\n", status);
        vipGoOnError(status);
    }

    viphal_query_database(&database);
    for (i = 0; i < sizeof(vip_chip_info) / sizeof(struct _vip_hw_feature_db); i++) {
        if (database.hw_cid == vip_chip_info[i].pid) {
            context->feature_db = &vip_chip_info[i];
            break;
        }
    }
    if (VIP_NULL == context->feature_db) {
        vip_error("fail to get hardware feature database pid=0x%x\n", database.hw_cid);
        vipGoOnError(VIP_ERROR_NOT_SUPPORTED);
    }

    /*3. create system memory heap, allocate a memory for using system memory heap */
#if vpmdENABLE_SYS_MEMORY_HEAP
    if (sys_mem_size < database.sys_heap_size) {
        sys_mem_size = database.sys_heap_size;
    }
    if (sys_mem_size > 0) {
        heap = &context->sys_mem_heap;
        status = viphal_os_allocate_memory(sys_mem_size, &pool);
        if (status != VIP_SUCCESS) {
            vip_error("fail to acquire system memory heap size=0x%x\n", sys_mem_size);
            sys_mem_size = 0;
            pool = VIP_NULL;
            vipGoOnError(status);
        }

        context->sys_mem_heap_addr = pool;
        vip_debug("system memory heap size=0x%xbytes, address=%p\n", sys_mem_size, pool);

        if (pool != VIP_NULL) {
            status = viphal_heap_construct(heap, sys_mem_size, pool);
            if (status != VIP_SUCCESS) {
                vip_error("fail to construce system memory heap\n");
                vipGoOnError(status);
            }
        }
        else {
            vip_error("system memory heap base addrss is NULL\n");
        }

        #if vpmdENABLE_MULTIPLE_TASK
        if (context->sys_mem_heap_addr != VIP_NULL) {
            status = viphal_os_create_mutex(&context->heap_mutex);
            if (status != VIP_SUCCESS) {
                vip_error("fail to create mutex for system memory heap\n");
                vipGoOnError(status);
            }
        }
        #endif
    }
    else
#endif
    {
        vip_info("allocate dynamic memory from system for system memory\n");
    }

onError:
    return status;
}

/*
@Brief Free resources at vip-hal layer.
*/
vip_status_e viphal_destroy(void)
{
    vip_status_e status = VIP_SUCCESS;
    GET_CONTEXTU();

    status = viphal_os_call_drv(context->os_private, VIPDRV_INTRFACE_DESTROY, VIP_NULL);
    if (status != VIP_SUCCESS) {
        vip_error("fail to call vip-drv cmd destroy\n");
    }

    status = viphal_os_uninit(&context->os_private);
    context->os_private = VIP_NULL;

#if vpmdENABLE_SYS_MEMORY_HEAP
    if (context->sys_mem_heap_addr != VIP_NULL) {
        viphal_heap_destroy(&context->sys_mem_heap);
    }
#endif

#if vpmdENABLE_MEMORY_PROFILING
    profile_report();
    viphal_os_destroy_mutex(context->profiler_mutex);
    context->profiler_mutex = VIP_NULL;
#endif

#if vpmdENABLE_SYS_MEMORY_HEAP
    /* free system memory heap */
    if (context->sys_mem_heap_addr != VIP_NULL) {
        viphal_os_free_memory(context->sys_mem_heap_addr);
        context->sys_mem_heap_addr = VIP_NULL;
    }
    #if vpmdENABLE_MULTIPLE_TASK
    if (context->heap_mutex != VIP_NULL) {
        viphal_os_destroy_mutex(context->heap_mutex);
    }
    #endif
#endif

    return status;
}

/*
@Brief Wait VIP to finsh the task_id task.
@param, task_id, the id of this task should be wait complete.
*/
vip_status_e viphal_wait_task(
    IN vip_uint32_t task_id,
    IN vip_uint32_t subtask_index
    )
{
    vip_status_e status = VIP_SUCCESS;
    vipdrv_wait_task_param_t wait_t;
    GET_CONTEXTU();

    wait_t.task_id = task_id;
    wait_t.subtask_index = subtask_index;

    status = viphal_os_call_drv(context->os_private, VIPDRV_INTRFACE_WAIT_TASK, &wait_t);

    if ((status != VIP_SUCCESS) && (status != VIP_ERROR_CANCELED)) {
        vip_error("fail to check status=%d\n", status);
        vipGoOnError(status);
    }

onError:
    return status;
}

/*
@Brief Cancle this command run on vip hardware.
@param, task_id, the id of task should be cancel.
@param, subtask_index, the id of subtask in task.
*/
#if vpmdENABLE_CANCELATION
vip_status_e viphal_cancel_task(
    IN vip_uint32_t task_id,
    IN vip_uint32_t subtask_index
    )
{
    vip_status_e status = VIP_SUCCESS;
    vipdrv_cancel_task_param_t cancel_t;
    GET_CONTEXTU();

    cancel_t.task_id = task_id;
    cancel_t.subtask_index = subtask_index;

    status = viphal_os_call_drv(context->os_private, VIPDRV_INTRFACE_CANCEL_TASK, &cancel_t);
    CHECK_STATUS("cancel");

onError:
    return status;
}
#endif

/*
@Brief Submit the commands (network) to HW to run.
@param param The submit param structure.
*/
vip_status_e viphal_submit_task(
    IN viphal_task_param_t *param
    )
{
    vip_status_e status = VIP_SUCCESS;
    vipdrv_submit_task_param_t task_param = { 0 };
    GET_CONTEXTU();
    vipIsNULL(param);

    task_param.task_id = param->task_id;
    task_param.device_index = param->device_index;
    task_param.core_index = param->core_index;
    task_param.core_cnt = param->core_cnt;
#if vpmdENABLE_PREEMPTION
    task_param.priority = param->priority;
#endif
    task_param.time_out = param->time_out;
    task_param.subtask_cnt = param->subtask_count;
    task_param.subtask_index = param->subtask_index;
    task_param.need_schedule = param->need_schedule;
    status = viphal_os_call_drv(context->os_private, VIPDRV_INTRFACE_SUBMIT_TASK, &task_param);
    CHECK_STATUS("submit task");

onError:
    return status;
}

/*
@Brief Allocate a buffer, which could be used for command,
       intermediate pools, input/output and so on.
@param mem_id  the id video memory.
@param logical, user space logical address.
@param physical, the virtual address of VIP when .
@param size, the size of of user memory should be unwraped.
@param align, The alignment size of the video memory.
@param alloc_flag, The alloc flag of the video memory. see vipdrv_video_mem_alloc_flag_e.
*/
vip_status_e viphal_allocate_videomemory(
    OUT vip_uint8_t **logical,
    OUT vip_uint32_t *mem_id,
    OUT vip_address_t *physical,
    IN vip_size_t  size,
    IN vip_uint32_t align,
    IN vip_uint32_t alloc_flag
    )
{
    vip_status_e status = VIP_SUCCESS;
    vipdrv_allocation_t allocation;
    GET_CONTEXTU();
    if (0 == size) {
        vip_error("fail to alloc video memory size is 0\n");
        vipGoOnError(VIP_ERROR_INVALID_ARGUMENTS);
    }

    allocation.size = size;
    allocation.align = align;
    allocation.alloc_flag = alloc_flag | VIPDRV_VIDEO_MEM_ALLOC_MAP_USER;
#if !vpmdENABLE_VIDEO_MEMORY_CACHE
    allocation.alloc_flag |= VIPDRV_VIDEO_MEM_ALLOC_NONE_CACHE;
#endif

    if (size > (0x10000 / 2)) {
        allocation.alloc_flag |= VIPDRV_VIDEO_MEM_ALLOC_64K_CONTIGUOUS;
    }
    if (size > (0x100000/ 2)) {
        allocation.alloc_flag |= VIPDRV_VIDEO_MEM_ALLOC_1M_CONTIGUOUS;
    }
    if (size > (0x1000000 / 2)) {
        allocation.alloc_flag |= VIPDRV_VIDEO_MEM_ALLOC_16M_CONTIGUOUS;
    }

    status = viphal_os_call_drv(context->os_private, VIPDRV_INTRFACE_ALLOCATION, &allocation);
    if (status == VIP_SUCCESS) {
        *logical = VIPUINT64_TO_PTR(allocation.logical);
        *mem_id = allocation.mem_id;
        if (physical != VIP_NULL) {
            *physical = allocation.physical;
        }

        #if vpmdENABLE_MEMORY_PROFILING
        MEMORY_PROFILING_ADD(VIPUINT32_TO_PTR(*mem_id), size);
        #endif
    }
    else {
        *mem_id = 0;
        *logical = VIP_NULL;
        vipGoOnError(status);
    }

    vip_debug("video memory logical=%x, physical=0x%llx, size=0x%x, mem_id=0x%x, alloc_flag=0x%x\n",
                *logical, allocation.physical, allocation.size, allocation.mem_id, allocation.alloc_flag);
    return status;

onError:
    vip_error("fail to allocate video memory size=0x%x, status=%d\n", size, status);
    return status;
}

/*
@ Brief Free a video memory buffer.
@param mem_id the id of video memory.
*/
vip_status_e viphal_free_videomemory(
    IN vip_uint32_t mem_id
    )
{
    vip_status_e status = VIP_SUCCESS;
    vipdrv_deallocation_t allocation;
    GET_CONTEXTU();
    vipIsNULL(mem_id);

    allocation.mem_id = mem_id;
    status = viphal_os_call_drv(context->os_private, VIPDRV_INTRFACE_DEALLOCATION, &allocation);
    CHECK_STATUS("free video memory");

    MEMORY_PROFILING_REMOVE(VIPUINT32_TO_PTR(mem_id));

onError:
    return status;
}

/*
@Brief mapping user space logical address
@param mem_id the id of video memory.
@logical, OUT. the logical address of this video memory.
*/
vip_status_e viphal_map_userlogical(
    IN vip_uint32_t mem_id,
    OUT vip_uint8_t **logical
    )
{
    vip_status_e status = VIP_SUCCESS;
    vipdrv_map_user_logical_t map;
    GET_CONTEXTU();
    vipIsNULL(mem_id);
    vipIsNULL(logical);

    map.mem_id = mem_id;
    status = viphal_os_call_drv(context->os_private, VIPDRV_INTRFACE_MAP_USER_LOGICAL, &map);
    CHECK_STATUS("map userlogical");
    *logical = VIPUINT64_TO_PTR(map.logical);

    vip_debug("video memory map logical=%p, mem_id=0x%x\n", *logical, map.mem_id);

onError:
    return status;
}

/*
@Brief unmapping user space logical address
@param mem_id the id of video memory.
*/
vip_status_e viphal_unmap_userlogical(
    IN vip_uint32_t mem_id
    )
{
    vip_status_e status = VIP_SUCCESS;
    vipdrv_unmap_user_logical_t unmap;
    GET_CONTEXTU();
    vipIsNULL(mem_id);

    unmap.mem_id = mem_id;
    status = viphal_os_call_drv(context->os_private, VIPDRV_INTRFACE_UNMAP_USER_LOGICAL, &unmap);
    CHECK_STATUS("unmap user logical");

onError:
    return status;
}

/*
@brief create a buffer from user contiguous or scatter non-contiguous physical address.
@param physical_table Physical address table. should be wraped for VIP hardware.
@param size_table The size of physical memory for each physical_table element.
@param physical_num The number of physical table element.
       physical_num is 1 when create buffer from contiguous phyiscal.
@param alloc_flag the flag of this video memroy. see vipdrv_video_mem_alloc_flag_e.
@param memory_type The type of this VIP buffer memory. see vip_buffer_memory_type_e.
@param virtual_addr The VIP hardware virtual address mapped by MMU.
@param mem_id The id of video memory.
*/
vip_status_e viphal_wrap_userphysical(
    IN const vip_address_t *physical_table,
    IN const vip_uint32_t *size_table,
    IN vip_uint32_t physical_num,
    IN vip_uint32_t alloc_flag,
    IN vip_uint32_t memory_type,
    OUT vip_address_t *virtual_addr,
    OUT vip_uint8_t **logical,
    OUT vip_uint32_t *mem_id
    )
{
    vip_status_e status = VIP_SUCCESS;
    vip_uint32_t i = 0;
    vipdrv_wrap_user_physical_t *data = VIP_NULL;
    GET_CONTEXTU();
    vipIsNULL(logical);

    if (physical_num > MAX_WRAP_USER_PHYSICAL_TABLE_SIZE) {
        vip_error("fail to wrap user physical, only supports physical num is small than %d, actual=%d\n",
                     MAX_WRAP_USER_PHYSICAL_TABLE_SIZE, physical_num);
        vipGoOnError(VIP_ERROR_INVALID_ARGUMENTS);
    }

    vipOnError(viphal_allocate_memory(sizeof(vipdrv_wrap_user_physical_t), (void**)&data));

    for (i = 0; i < physical_num; i++) {
        data->physical_table[i] = physical_table[i];
        data->size_table[i] = size_table[i];
    }

    data->memory_type = memory_type;
    data->physical_num = physical_num;
    data->alloc_flag = alloc_flag;

    if (0 == physical_num) {
        vip_error("fail to wrap user physical, physical_num is 0\n");
        vipGoOnError(VIP_ERROR_INVALID_ARGUMENTS);
    }

    status = viphal_os_call_drv(context->os_private, VIPDRV_INTRFACE_WRAP_USER_PHYSICAL, data);
    CHECK_STATUS("wrap user physical");

    *virtual_addr = data->virtual_addr;
    *mem_id = data->mem_id;
    *logical = VIPUINT64_TO_PTR(data->logical);

    viphal_free_memory(data);

#if vpmdENABLE_MEMORY_PROFILING
    {
    vip_uint32_t size = 0;
    for (i = 0; i < physical_num; i++) {
        size += size_table[i];
    }
    MEMORY_PROFILING_ADD(VIPUINT32_TO_PTR(*mem_id), size);
    }
#endif

    return status;
onError:
    vip_error("fail to wrap user physical.\n");
    return status;
}

/*
@Brief UnWrap a user physical.
@param mem_id the id of video memory.
@param physical, the virtual address of VIP.
*/
vip_status_e viphal_unwrap_userphysical(
    IN vip_uint32_t mem_id
    )
{
    vip_status_e status = VIP_SUCCESS;
    vipdrv_deallocation_t data;
    GET_CONTEXTU();

    data.mem_id = mem_id;

    status = viphal_os_call_drv(context->os_private, VIPDRV_INTRFACE_UNWRAP_USER_PHYSICAL, &data);
    CHECK_STATUS("unwrap user physical");

    MEMORY_PROFILING_REMOVE(VIPUINT32_TO_PTR(mem_id));

onError:
    return status;
}

/*
@Brief Wrap a user memory.
@param mem_id the id of video memory.
@param logical, user space logical address.
@param size, the size of of user memory should be unwraped.
@param memory_type The type of this VIP buffer memory.
        see vip_buffer_memory_type_e | vipdrv_video_mem_alloc_flag_e.
@param virtual_addr The VIP virtual address mapped from logical/physical address.
*/
vip_status_e viphal_wrap_usermemory(
    IN vip_ptr logical,
    IN vip_size_t size,
    IN vip_uint32_t memory_type,
    OUT vip_address_t *virtual_addr,
    OUT vip_uint32_t *mem_id
    )
{
    vip_status_e status = VIP_SUCCESS;
    vipdrv_wrap_user_memory_t data;
    GET_CONTEXTU();
    vipIsNULL(logical);

    data.logical = VIPPTR_TO_UINT64(logical);
    data.size = size;
    data.memory_type = memory_type;

    if (VIP_NULL == virtual_addr) {
        vip_error("fail to wrap user memory, parameter is NULL\n");
        vipGoOnError(VIP_ERROR_INVALID_ARGUMENTS);
    }

    status = viphal_os_call_drv(context->os_private, VIPDRV_INTRFACE_WRAP_USER_MEMORY, &data);
    CHECK_STATUS("wrap user memory");

    *virtual_addr = data.virtual_addr;
    *mem_id = data.mem_id;

    MEMORY_PROFILING_ADD(VIPUINT32_TO_PTR(*mem_id), size);

onError:
    return status;
}

/*
@Brief UnWrap a user memory.
@param mem_id the id of video memory.
*/
vip_status_e viphal_unwrap_usermemory(
    IN vip_uint32_t mem_id
    )
{
    vip_status_e status = VIP_SUCCESS;
    vipdrv_deallocation_t data;
    GET_CONTEXTU();

    data.mem_id = mem_id;

    status = viphal_os_call_drv(context->os_private, VIPDRV_INTRFACE_UNWRAP_USER_MEMORY, &data);
    CHECK_STATUS("unwrap user memory");

    MEMORY_PROFILING_REMOVE(VIPUINT32_TO_PTR(mem_id));

onError:
    return status;
}

/*
@Brief Wrap a user memory fd.
@param fd the user memory file descriptor.
@param size, the size of of user memory should be unwraped.
@param memory_type The type of this VIP buffer memory. see vip_buffer_memory_type_e.
@param virtual The VIP virtual address mapped from logical/physical address.
@param mem_id the id of video memory.
*/
#if vpmdENABLE_CREATE_BUF_FD
vip_status_e viphal_wrap_userfd(
    IN vip_uint32_t fd,
    IN vip_size_t size,
    IN vip_uint32_t memory_type,
    OUT vip_address_t *virtual_addr,
    OUT vip_uint8_t **logical,
    OUT vip_uint32_t *mem_id
    )
{
    vip_status_e status = VIP_SUCCESS;
    vip_int32_t tmp = (vip_int32_t)fd;

    vipdrv_wrap_user_fd_t data;
    GET_CONTEXTU();
    vipIsNULL(mem_id);
    vipIsNULL(logical);

    if (tmp < 0) {
        vip_error("failed wrap user memory fd=%d\n", fd);
        vipGoOnError(VIP_ERROR_INVALID_ARGUMENTS);
    }

    if (VIP_NULL == virtual_addr) {
        vip_error("fail to wrap user memory, parameter is NULL\n");
        vipGoOnError(VIP_ERROR_INVALID_ARGUMENTS);
    }

    data.fd = fd;
    data.size = size;
    data.memory_type = memory_type;

    status = viphal_os_call_drv(context->os_private, VIPDRV_INTRFACE_WRAP_USER_FD, &data);
    CHECK_STATUS("wrap userfd");

    *virtual_addr = data.virtual_addr;
    *mem_id = data.mem_id;
    *logical = VIPUINT64_TO_PTR(data.logical);

    MEMORY_PROFILING_ADD(VIPUINT32_TO_PTR(*mem_id), size);

onError:
    return status;
}

/*
@Brief UnWrap a user memory from fd(file descriptor).
@param mem_id the id of video memory.
*/
vip_status_e viphal_unwrap_userfd(
    IN vip_uint32_t mem_id
    )
{
    vip_status_e status = VIP_SUCCESS;
    vipdrv_deallocation_t data;
    GET_CONTEXTU();

    data.mem_id = mem_id;

    status = viphal_os_call_drv(context->os_private, VIPDRV_INTRFACE_UNWRAP_USER_FD, &data);
    CHECK_STATUS("unwrap userfd");

    MEMORY_PROFILING_REMOVE(VIPUINT32_TO_PTR(mem_id));

onError:
    return status;
}
#endif

/*
@Brief Allocate a system memory.
@param size The size of memory.
@param memory Allocate memory object.
*/
vip_status_e viphal_allocate_memory(
    IN vip_size_t size,
    OUT void **memory
    )
{
    vip_status_e status = VIP_ERROR_FAILURE;
#if vpmdENABLE_SYS_MEMORY_HEAP
    GET_CONTEXTU();
#endif
    if (0 == size) {
        vip_error("fail to allocate memory size is 0\n");
        vipGoOnError(VIP_ERROR_INVALID_ARGUMENTS);
    }
#if vpmdENABLE_SYS_MEMORY_HEAP
    if ((context->sys_mem_heap_addr != VIP_NULL) && (context->sys_mem_heap.free_bytes > size) &&
        ((context->sys_mem_heap.node_count + 1) < context->sys_mem_heap.node_capacity)) {
        status = heap_alloc_memory(size, memory);
    }
    if (VIP_SUCCESS != status)
#endif
    {
        status = viphal_os_allocate_memory(size, memory);
    }

    if (status != VIP_SUCCESS) {
        vip_error("fail to allocate system memory size=0x%x\n", size);
        status = VIP_ERROR_OUT_OF_MEMORY;
    }
#if vpmdENABLE_MEMORY_PROFILING
    else {
        profile_add_node(*memory, size);
        profile_add_system_memory(size);
    }
#endif

onError:
    return status;
}

/*
@Brief Free system memory.
@param memory The memory object should be free.
*/
vip_status_e viphal_free_memory(
    IN void *memory
    )
{
    vip_status_e status;
#if vpmdENABLE_SYS_MEMORY_HEAP
    GET_CONTEXTU();
#endif
    vipIsNULL(memory);

#if vpmdENABLE_SYS_MEMORY_HEAP
    if (context->sys_mem_heap_addr != VIP_NULL) {
        status = heap_free_memory(memory);
        if (status != VIP_SUCCESS) {
            /* try to free system memory */
            status = viphal_os_free_memory(memory);
        }
    }
    else
#endif
    {
        status = viphal_os_free_memory(memory);
    }
    CHECK_STATUS("free memory");
#if vpmdENABLE_MEMORY_PROFILING
    profile_remove_system_memory(profile_get_size(memory));
    profile_remove_node(memory);
#endif

onError:
    return status;
}

/*
@Brief Copy memory data from src to dst.
@param dst Destination memory.
@param src Source memory.
@param size The size of memory shoule be copy.
*/
vip_status_e viphal_copy_memory(
    IN void *dst,
    IN const void *src,
    IN vip_size_t size
    )
{
    vip_status_e status = VIP_SUCCESS;

    vipIsNULL(dst);
    status = viphal_os_copy_memory(dst, src, size);

onError:
    return status;
}

/*
brief fill zero to memory
param memory
param size The size of memory should be fill zero.
*/
vip_status_e viphal_zero_memory(
    IN void *memory,
    IN vip_size_t size
    )
{
    vip_status_e status = VIP_SUCCESS;

    vipIsNULL(memory);
    status = viphal_os_zero_memory(memory, size);

onError:
    return status;
}

/*
brief read a hardware register
param dev_index, the index of device.
param core_index the index of hardware core belong to device.
param address the address read.
param data data read to address.
*/
vip_status_e viphal_read_reg(
    IN  vip_uint32_t dev_index,
    IN  vip_uint32_t core_index,
    IN  vip_uint32_t address,
    OUT vip_uint32_t *data
    )
{
    vipdrv_reg_t reg;
    vip_status_e status = VIP_SUCCESS;
    GET_CONTEXTU();
    vipIsNULL(data);

    reg.reg = address;
    reg.core_index = core_index;
    reg.dev_index = dev_index;

    status = viphal_os_call_drv(context->os_private, VIPDRV_INTRFACE_READ_REG, &reg);
    CHECK_STATUS("read register");

    if (data != VIP_NULL) {
        *data = reg.data;
    }

onError:
    return status;
}

/*
brief write a hardware register
param dev_index, the index of device.
param core_index the index of hardware core belong to device.
param address the address write.
param data data write to address.
*/
vip_status_e viphal_write_reg(
    IN  vip_uint32_t dev_index,
    IN  vip_uint32_t core_index,
    IN  vip_uint32_t address,
    IN  vip_uint32_t data
    )
{
    vipdrv_reg_t reg;
    vip_status_e status = VIP_SUCCESS;
    GET_CONTEXTU();

    reg.dev_index = dev_index;
    reg.core_index = core_index;
    reg.reg = address;
    reg.data = data;

    status = viphal_os_call_drv(context->os_private, VIPDRV_INTRFACE_WRITE_REG, &reg);
    CHECK_STATUS("write register");

onError:
    return status;
}

/*
brief dump string to file.
param stream the object stream.  FILE * pointer.
param buffer string store in buffer object.
*/
vip_status_e viphal_print_string(
    IN void *stream,
    IN vip_char_t *buffer
    )
{
    vip_status_e status = VIP_SUCCESS;
    GET_CONTEXTU();

    status = viphal_os_print_string(context->os_private, stream, buffer);

    return status;
}

/*
brief Query address infomation. wait-link buffer address for Agent driver.
param data, query address info stucture.
*/
#if vpmdENABLE_USE_AGENT_TRIGGER
vip_status_e viphal_query_address_info(
    IN void *data
    )
{
    vip_status_e status = VIP_SUCCESS;
    GET_CONTEXTU();
    vipIsNULL(data);

    status = viphal_os_call_drv(context->os_private, VIPDRV_INTRFACE_QUERY_ADDRESS_INFO, data);
    CHECK_STATUS("query address info");

onError:
    return status;
}
#endif

/*
brief Query MMU page table infomation for dumping capture file.
param data, query mmu info stucture.
*/
#if vpmdENABLE_CAPTURE_MMU
vip_status_e viphal_query_mmu_info(
    IN void *data
    )
{
    vip_status_e status = VIP_SUCCESS;
    GET_CONTEXTU();
    vipIsNULL(data);

    status = viphal_os_call_drv(context->os_private, VIPDRV_INTRFACE_QUERY_MMU_INFO, data);
    CHECK_STATUS("mmu info");

onError:
    return status;
}
#endif

/*
brief Query feature database.
@param data, query database stucture.
*/
vip_status_e viphal_query_database(
    IN void *data
    )
{
    vip_status_e status = VIP_SUCCESS;
    GET_CONTEXTU();
    vipIsNULL(data);

    status = viphal_os_call_drv(context->os_private, VIPDRV_INTRFACE_QUERY_DATABASE, data);
    CHECK_STATUS("query db");

onError:
    return status;
}

/*
brief Query profiling data, such as inference time, total cycle.
@param data, query profiling stucture. vipdrv_query_profiling_t.
*/
vip_status_e viphal_query_profiling(
    IN void *data
    )
{
    vip_status_e status = VIP_SUCCESS;
    GET_CONTEXTU();
    vipIsNULL(data);

    status = viphal_os_call_drv(context->os_private, VIPDRV_INTRFACE_QUERY_PROFILING, data);
    CHECK_STATUS("query profile");

onError:
    return status;
}

/*
brief Query vip-drv space driver status.
@param data, query driver status stucture. vipdrv_query_driver_status_t.
*/
vip_status_e viphal_query_driver_status(
    IN void *data
    )
{
    vip_status_e status = VIP_SUCCESS;
    GET_CONTEXTU();
    vipIsNULL(data);

    status = viphal_os_call_drv(context->os_private, VIPDRV_INTRFACE_QUERY_DRIVER_STATUS, data);
    CHECK_STATUS("query driver");

onError:
    return status;
}

/*
@brief Flush video memory CPU cache.
@param mem_id, the video memory id be flush.
@param type, the type of flush cache. see vipdrv_cache_type_e.
*/
#if vpmdENABLE_FLUSH_CPU_CACHE
vip_status_e viphal_flush_cache(
    IN vip_uint32_t mem_id,
    IN vip_uint8_t type
    )
{
    vip_status_e status = VIP_SUCCESS;
    vipdrv_operate_cache_t data;
    GET_CONTEXTU();
    vipIsNULL(mem_id);

    if ((type >= VIPDRV_CACHE_MAX) || (type == VIPDRV_CACHE_NONE)) {
        vip_error("invalid flush cache type=%d\n", type);
        return VIP_ERROR_FAILURE;
    }

    data.mem_id = mem_id;
    data.type = (vipdrv_cache_type_e)type;

    status = viphal_os_call_drv(context->os_private, VIPDRV_INTRFACE_OPERATE_CACHE, &data);
    CHECK_STATUS("flush cache");

onError:
    return status;
}
#endif

/*
@brief Give user applications more control over power management for VIP cores.
@perperty Control VIP core frequency and power status by property. see vip_power_property_e.
@value The value for vip_power_property_e property.
@device_index, The index of device.
*/
vip_status_e viphal_power_management(
    IN vip_enum property,
    IN void *value,
    IN vip_uint32_t device_index
    )
{
    vip_status_e status = VIP_SUCCESS;
    vipdrv_power_management_t power;
    GET_CONTEXTU();

    power.property = property;
    power.device_index = device_index;
    #if vpmdENABLE_USER_CONTROL_POWER
    if (VIPDRV_POWER_PROPERTY_SET_FREQUENCY == property) {
        vip_uint8_t *freq = (vip_uint8_t*)value;
        if (VIP_NULL == freq) {
            vip_error("fail to power management control, set frequency value is NULL\n");
            return VIP_ERROR_FAILURE;
        }
        power.fscale_percent = *freq;
    }
    #endif
    status = viphal_os_call_drv(context->os_private, VIPDRV_INTRFACE_POWER_MANAGEMENT, &power);
    CHECK_STATUS("pm");

onError:
    return status;
}

/*
@brief dump buffer in hex format. only dump buffer for debugging.
@param buffer, data buffer address.
@param size, the size of the buffer.
*/
#if vpmdENABLE_DEBUG_LOG >= 4 || vpmdENABLE_MULTIPLE_VIP
vip_status_e viphal_dump_data(
    IN vip_uint32_t *buffer,
    IN vip_uint32_t size
    )
{
    vip_uint32_t i = 0;
    vip_uint32_t line = size / 32;
    vip_uint32_t left = size % 32;
    vip_uint32_t *data = buffer;

    vip_print("Dump HEX data size=0x%x\n", size);
    for (i = 0; i < line; i++) {
        vip_print("%08X %08X %08X %08X %08X %08X %08X %08X\n",
                data[0], data[1], data[2], data[3], data[4], data[5], data[6], data[7]);
        data += 8;
    }
    switch(left) {
        case 28:
          vip_print("%08X %08X %08X %08X %08X %08X %08X\n",
                data[0], data[1], data[2], data[3],  data[4], data[5], data[6]);
          break;
        case 24:
          vip_print("%08X %08X %08X %08X %08X %08X\n",
                data[0], data[1], data[2], data[3], data[4], data[5]);
          break;
        case 20:
          vip_print("%08X %08X %08X %08X %08X\n",
                data[0], data[1], data[2], data[3], data[4]);
          break;
        case 16:
          vip_print("%08X %08X %08X %08X\n",
                data[0], data[1], data[2], data[3]);
          break;
        case 12:
          vip_print("%08X %08X %08X\n", data[0], data[1], data[2]);
          break;
        case 8:
          vip_print("%08X %08X\n", data[0], data[1]);
          break;
        case 4:
          vip_print("%08X\n", data[0]);
          break;

        default:
          break;
    }
    return VIP_SUCCESS;
}
#endif

#if vpmdENABLE_CAPTURE || (vpmdENABLE_HANG_DUMP > 1)
vip_status_e viphal_query_register_dump(
    OUT vip_uint32_t* mem_id,
    OUT vip_physical_t* physical,
    OUT vip_uint32_t* count
    )
{
    vip_status_e status = VIP_SUCCESS;
    vipdrv_query_register_dump_t info;
    GET_CONTEXTU();

    status = viphal_os_call_drv(context->os_private, VIPDRV_INTRFACE_QUERY_REGISTER_DUMP, &info);
    if (status == VIP_SUCCESS) {
        *mem_id = info.mem_id;
        *physical = info.physical;
        *count = info.count;
    }
    else {
        *mem_id = 0;
        *physical = VIP_NULL;
        *count = VIP_NULL;
        vipGoOnError(status);
    }

onError:
    return status;
}
#endif

vip_status_e viphal_task_create(
    IN vip_char_t_ptr task_name,
    IN vip_uint32_t subtask_count,
    OUT vip_uint32_t* task_id
    )
{
    vip_status_e status = VIP_SUCCESS;
    vipdrv_create_task_param_t tsk_param = { 0 };
    GET_CONTEXTU();

    viphal_os_snprint(tsk_param.task_name, TASK_NAME_LENGTH, task_name);
    tsk_param.subtask_count = subtask_count;
    status = viphal_os_call_drv(context->os_private, VIPDRV_INTRFACE_CREATE_TASK, &tsk_param);
    if (status == VIP_SUCCESS) {
        *task_id = tsk_param.task_id;
    }
    else {
        *task_id = 0;
    }
    CHECK_STATUS("task create");

onError:
    return status;
}

vip_status_e viphal_task_set_property(
    IN vip_uint32_t task_id,
    IN vip_uint32_t type,
    IN vip_uint32_t subtask_index,
    IN vip_uint32_t subtask_id,
    IN vip_uint32_t size,
    IN vip_uint32_t patch_cnt
    )
{
    vip_status_e status = VIP_SUCCESS;
    vipdrv_set_task_property_t property = { 0 };
    GET_CONTEXTU();

    property.task_id = task_id;
    property.type = type;
    property.index = subtask_index;
    property.patch_type = VIPDRV_TASK_PATCH_NONE;
    property.param.subtask_info.subtask_id = subtask_id;
    property.param.subtask_info.size = size;
    property.param.subtask_info.patch_count = patch_cnt;
    status = viphal_os_call_drv(context->os_private, VIPDRV_INTRFACE_SET_TASK_PROPERTY, &property);
    CHECK_STATUS("task set property");

onError:
    return status;
}

#if vpmdTASK_SCHEDULE
vip_status_e viphal_task_set_patch_info(
    IN vip_uint32_t task_id,
    IN vip_uint32_t type,
    IN vip_uint32_t subtask_index,
    IN vip_uint32_t patch_index,
    IN vip_uint32_t patch_type,
    IN vip_uint32_t offset,
    IN vip_uint32_t size,
    IN vip_uint32_t core_index,
    IN vip_uint32_t core_count
    )
{
    vip_status_e status = VIP_SUCCESS;
    vipdrv_set_task_property_t property = { 0 };
    GET_CONTEXTU();

    property.task_id = task_id;
    property.type = type;
    property.index = subtask_index;
    property.patch_type = patch_type;
    property.param.patch_info.patch_index = patch_index;
    property.param.patch_info.size = size;
    property.param.patch_info.offset = offset;
    property.param.patch_info.core_index = core_index;
    property.param.patch_info.core_count = core_count;
    status = viphal_os_call_drv(context->os_private, VIPDRV_INTRFACE_SET_TASK_PROPERTY, &property);
    CHECK_STATUS("task set patch info");

onError:
    return status;
}

vip_status_e viphal_task_dup_info(
    IN vip_uint32_t task_id,
    IN vip_uint32_t type,
    IN vip_uint32_t subtask_index,
    IN vip_uint32_t task_id_dup,
    IN vip_uint32_t type_dup,
    IN vip_uint32_t subtask_index_dup
    )
{
    vip_status_e status = VIP_SUCCESS;
    vipdrv_set_task_property_t property = { 0 };
    GET_CONTEXTU();

    property.task_id = task_id;
    property.type = type;
    property.index = subtask_index;
    property.patch_type = VIPDRV_TASK_PATCH_DUP;
    property.param.dup_info.task_id = task_id_dup;
    property.param.dup_info.type = type_dup;
    property.param.dup_info.index = subtask_index_dup;
    status = viphal_os_call_drv(context->os_private, VIPDRV_INTRFACE_SET_TASK_PROPERTY, &property);
    CHECK_STATUS("task dup info");

onError:
    return status;
}
#endif

vip_status_e viphal_task_destroy(
    IN vip_uint32_t task_id
    )
{
    vip_status_e status = VIP_SUCCESS;
    vipdrv_destroy_task_param_t tsk_param = { 0 };
    GET_CONTEXTU();
    vipIsNULL(task_id);

    tsk_param.task_id = task_id;
    status = viphal_os_call_drv(context->os_private, VIPDRV_INTRFACE_DESTROY_TASK, &tsk_param);
    CHECK_STATUS("task destroy");

onError:
    return status;
}

vip_uint32_t viphal_query_hw_feature(
    IN vip_uint32_t feature
    )
{
    vip_uint32_t value = 0;
    GET_CONTEXTU();

    switch (feature)
    {
    case VIP_HW_FEATURE_KENERL_DESC:
        value = context->feature_db->kernel_descriptor;
        break;
    case VIP_HW_FEATURE_VIPSRAM_WIDTH:
        value = context->feature_db->vip_sram_width;
        break;
    case VIP_HW_FEATURE_NN_TRANS_PHASE2:
        value = context->feature_db->nn_transpose_phase2;
        break;
    case VIP_HW_FEATURE_NN_CORE_CNT:
        value = context->feature_db->nn_core_count;
        break;
    case VIP_HW_FEATURE_TP_CORE_CNT:
        value = context->feature_db->tp_core_count;
        break;
    case VIP_HW_FEATURE_LATENCY_HIDING_FULL_AXI_BW:
        value = context->feature_db->latencyHiding_AtFullAxiBw;
        break;
    case VIP_HW_FEATURE_MIN_AXI_BURST_SIZE:
        value = context->feature_db->min_axi_burst_size;
        break;
    case VIP_HW_FEATURE_AXI_BUS_WIDTH:
        value = context->feature_db->axi_bus_width;/* bytes */
        break;
    case VIP_HW_FEATURE_DDR_KERNEL_BURST_SIZE:
        value = context->feature_db->ddr_kernel_burst_size;
        break;
    case VIP_HW_FEATURE_OCB_COUNTER:
        value = context->feature_db->ocb_counter;
        break;
    case VIP_HW_FEATURE_NN_PROBE:
        value = context->feature_db->nn_probe;
        break;
    case VIP_HW_FEATURE_MMU_PDMODE:
        value = context->feature_db->mmu_pd_mode;
        break;
    default:
        break;
    }

    return value;
}
