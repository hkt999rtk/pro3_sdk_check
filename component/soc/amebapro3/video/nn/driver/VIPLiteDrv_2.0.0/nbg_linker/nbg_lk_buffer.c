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

#include <nbg_lk.h>
#include <nbg_lk_buffer.h>
#include <nbg_lk_network.h>
#include <nbg_lk_debug.h>
#include <vip_hal.h>
#include <nbg_fmt_util.h>
#include <vip_lite.h>
#include <nbg_lk_context.h>


#define NBGLK_CREATE_BUF_INIT() \
{   \
    buff->param.num_of_dims = create_param->num_of_dims;    \
    buff->param.data_format = create_param->data_format;    \
    buff->param.quant_format = create_param->quant_format;  \
    buff->param.memory_type = create_param->memory_type;    \
    for (i = 0; i < create_param->num_of_dims; i++) {   \
        buff->param.sizes[i] = create_param->sizes[i];  \
    }   \
    for (i = create_param->num_of_dims; i < MAX_NUM_DIMS; i++) {    \
        buff->param.sizes[i] = 1;   \
    }   \
    if (VIP_BUFFER_QUANTIZE_DYNAMIC_FIXED_POINT == create_param->quant_format) {    \
        buff->param.quant_data.dfp.fixed_point_pos = create_param->quant_data.dfp.fixed_point_pos; \
    }   \
    else if (VIP_BUFFER_QUANTIZE_TF_ASYMM == create_param->quant_format) {  \
        buff->param.quant_data.affine.scale = create_param->quant_data.affine.scale;    \
        buff->param.quant_data.affine.zeroPoint = create_param->quant_data.affine.zeroPoint;    \
    }   \
}

/*
@brief Create a buffer used by the network's input and output.
       CPU's cache will be flush in nbglk_set_input/output function.
@param create_param The parametes of buffer be created.
@param size_of_param The size of create paramters.
@param buffer The returns buffer object.
*/
VIP_API
vip_status_e vip_create_buffer(
    IN vip_buffer_create_params_t *create_param,
    IN vip_uint32_t size_of_param,
    OUT vip_buffer *buffer
    )
{
    vip_uint32_t buf_size = 0;
    vip_uint32_t alloc_size = 0;
    vip_buffer_t *buff = VIP_NULL;
    vip_status_e status = VIP_SUCCESS;
    vip_uint32_t i = 0;
    VIP_FUNC_ENTER

    if (VIP_NULL == create_param) {
        vip_error("create param is NULL.\n");
        VIP_FUNC_EXIT
        return VIP_ERROR_INVALID_ARGUMENTS;
    }

    if (VIP_NULL == buffer) {
        vip_error("buffer is NULL.\n");
        VIP_FUNC_EXIT
        return VIP_ERROR_INVALID_ARGUMENTS;
    }

    /* Allocate memory. */
    status = viphal_allocate_memory(sizeof(vip_buffer_t), (void **)&buff);
    if ((status != VIP_SUCCESS) || (VIP_NULL == buff)) {
        vip_error("fail to allocate memory for buffer.\n");
        *buffer = VIP_NULL;
        VIP_FUNC_EXIT
        return status;
    }

    NBGLK_CREATE_BUF_INIT();

    buf_size = nbg_get_buf_format_bit(create_param->data_format);
    for (i = 0; i < create_param->num_of_dims; i++) {
        buf_size *= create_param->sizes[i];
        if (0 == i) {
            /* round up */
            buf_size = (buf_size + BITS_PER_BYTE - 1) / BITS_PER_BYTE;
        }
    }

    if ((0 == create_param->num_of_dims) || (0 == buf_size)) {
        vip_error("fail to create buffer, dim_num=%d, shape[%d, %d, %d, %d] \n",
                     create_param->num_of_dims, create_param->sizes[0], create_param->sizes[1],
                     create_param->sizes[2], create_param->sizes[3]);
        vip_error("fail to create buffer, param is invalid\n");
        vipGoOnError(VIP_ERROR_INVALID_ARGUMENTS);
    }

    alloc_size = buf_size + 64;
    alloc_size = VIP_ALIGN(alloc_size, vpmdCPU_CACHE_LINE_SIZE);

    status = viphal_allocate_videomemory((vip_uint8_t **)&buff->memory.logical,
                                              &buff->memory.mem_id, &buff->memory.physical,
                                              alloc_size, vipdMEMORY_ALIGN_SIZE,
                                              VIPDRV_VIDEO_MEM_ALLOC_MMU_PAGE_WRITE);
    if (status == VIP_SUCCESS) {
        *buffer = buff;
        buff->allocated = NBGLK_BUFFER_ALLOCATE_FROM_INTERNAL;
        buff->memory.size = buf_size;
        buff->object.type = NBGLK_OBJ_BUFFER;
    }
    else {
        vip_error("fail to create buffer, alloc video memory fail status=%d\n", status);
        vipGoOnError(status);
    }

    /*
    physical: when MMU enabled, physical is NPU's virtual address. when MMU disable, physical is NPU's physical address
    logical: CPU's logical address.
    */
    vip_debug("dim_num=%d, dims[%d,%d,%d,%d], logical=%p, phyical=0x%llx, mem_id=0x%x, size=0x%x\n",
                create_param->num_of_dims, create_param->sizes[0], create_param->sizes[1],
                create_param->sizes[2], create_param->sizes[3], buff->memory.logical,
                buff->memory.physical, buff->memory.mem_id, buf_size);

    VIP_FUNC_EXIT
    return status;
onError:
    vip_error("fail to create vip buffer\n");
    if (buff != VIP_NULL) {
        viphal_free_memory(buff);
        *buffer = VIP_NULL;
    }
    VIP_FUNC_EXIT
    return status;
}

/*
@brief create a buffer from user contiguous or scatter non-contiguous physical address.
        the vip_buffer created by this APi doesn't support flush CPU cache in driver.
        So the physical memory should be a non-cache buffer or flush CPU on Host control.
        not map user space logical on Linux.
@param create_param The parametes of buffer be created.
@param physical_table Physical address table of VIP. should be wraped for VIP hardware.
@param size_table The size of physical memory for each physical_table element.
@param physical_num The number of physical table element.
       physical_num is 1 when create buffer from contiguous phyiscal.
@param buffer vip lite buffer object.
*/
VIP_API
vip_status_e vip_create_buffer_from_physical(
    IN const vip_buffer_create_params_t *create_param,
    IN const vip_address_t *physical_table,
    IN const vip_uint32_t *size_table,
    IN vip_uint32_t physical_num,
    OUT vip_buffer *buffer
    )
{
    vip_uint32_t buf_size = 0;
    vip_uint32_t total_size = 0;
    vip_buffer_t *buff = VIP_NULL;
    vip_status_e status = VIP_SUCCESS;
    vip_uint32_t i = 0;
    vip_address_t physical = 0;
    vip_uint8_t *logical = VIP_NULL;
    vip_uint32_t alloc_flag = VIPDRV_VIDEO_MEM_ALLOC_NONE;
    vip_uint32_t mem_id = 0;
    VIP_FUNC_ENTER

    if ((VIP_NULL == create_param) || (VIP_NULL == buffer) ||
        (VIP_NULL == physical_table) || (VIP_NULL == size_table)) {
        vip_error("fail to create buffer from physical, parameter is NULL\n");
        VIP_FUNC_EXIT
        return VIP_ERROR_INVALID_ARGUMENTS;
    }

    /* check buffer size */
    for (i = 0; i < physical_num; i++) {
        total_size += size_table[i];
    }

    buf_size = nbg_get_buf_format_bit(create_param->data_format);
    for (i = 0; i < create_param->num_of_dims; i++) {
        buf_size *= create_param->sizes[i];
        if (0 == i) {
            /* round up */
            buf_size = (buf_size + BITS_PER_BYTE - 1) / BITS_PER_BYTE;
        }
    }

    if (buf_size > total_size) {
        vip_error("fail to create buffer. buffer size is bigger than physical total size."
                    "0x%x > 0x%x, shape [", buf_size, total_size);
        vip_print("data format=%d ", create_param->data_format);
        for (i = 0; i < create_param->num_of_dims; i++) {
            vip_print("%d ", create_param->sizes[i]);
        }
        vip_print("]\n");
        VIP_FUNC_EXIT
        return VIP_ERROR_INVALID_ARGUMENTS;
    }

    if ((0 == create_param->num_of_dims) || (0 == buf_size)) {
        vip_error("fail to create buffer, dim_num=%d, shape[%d, %d, %d, %d] \n",
                     create_param->num_of_dims, create_param->sizes[0], create_param->sizes[1],
                     create_param->sizes[2], create_param->sizes[3]);
        vip_error("fail to create buffer, param is invalid\n");
        VIP_FUNC_EXIT
        return VIP_ERROR_INVALID_ARGUMENTS;
    }

    status = viphal_allocate_memory(sizeof(vip_buffer_t), (void **)&buff);
    if (VIP_NULL == buff) {
        vip_error("fail to allocate memory for buffer.\n");
        *buffer = VIP_NULL;
        VIP_FUNC_EXIT
        return VIP_ERROR_OUT_OF_MEMORY;
    }

    NBGLK_CREATE_BUF_INIT();

    if (0 == physical_num) {
        vip_error("fail to create buffer from physical, physical number=%d\n", physical_num);
        vipGoOnError(VIP_ERROR_INVALID_ARGUMENTS);
    }
    status = viphal_wrap_userphysical(physical_table, size_table, physical_num, alloc_flag,
                                          create_param->memory_type, &physical, &logical, &mem_id);
    if (status != VIP_SUCCESS) {
        vip_error("fail to create buffer from physical. physical_num=%d, status=%d\n", physical_num, status);
        vipGoOnError(status);
    }
    /*
       MMU disabled, physical is CPU physical address. MMU enabled, physical is VIP virtual address.
    */
    buff->memory.physical = physical;
    buff->memory.logical = logical;
    buff->memory.size = total_size;
    buff->memory.mem_id = mem_id;
    buff->allocated = NBGLK_BUFFER_ALLOCATE_FROM_USER_PHYSICAL;
    buff->object.type = NBGLK_OBJ_BUFFER;
    *buffer = buff;

    /*
    physical[0]: the CPU's physical address.
    physical: when MMU enabled, physical is NPU's virtual address. when MMU disable, physical is NPU's physical address
    */
    vip_debug("from physical[0]=0x%" PRIx64", physical_num=%d, phyical(vip virtual)=0x%llx, "
                "logical=%p, mem_id=0x%x, total_size=0x%x, alloc_flag=0x%x\n",
                 physical_table[0], physical_num, physical, logical, mem_id, total_size, alloc_flag);

    VIP_FUNC_EXIT
    return status;

onError:
    vip_error("fail to create vip buffer for physical\n");
    if (buff != VIP_NULL) {
        viphal_free_memory(buff);
    }
    VIP_FUNC_EXIT
    return status;
}

/*
@brief create a buffer from handle.
The vip_buffer can be used to input, output, memory pool and so on.
NOTE: driver will operation CPU cache when call vip_flush_buffer API.
after write data into this buffer, APP should call vip_flush_buffer(VIP_BUFFER_OPER_TYPE_FLUSH)
before CPU read date from this buffer. APP should call vip_flush_buffer(VIP_BUFFER_OPER_TYPE_INVALIDATE)
@param create_param The parametes of buffer be created.
@param handle_logical The logical address of the handle.
       create vip buffer from the logical address.
@param the handle_size should be aligned to 64byte(vpmdCPU_CACHE_LINE_SIZE) for easy flash CPU cache.
@param buffer vip lite buffer object.
*/
VIP_API
vip_status_e vip_create_buffer_from_handle(
    IN const vip_buffer_create_params_t *create_param,
    IN const vip_ptr handle_logical,
    IN vip_uint32_t handle_size,
    OUT vip_buffer *buffer
    )
{
    vip_status_e status = VIP_SUCCESS;
    vip_uint32_t buf_size = 0;
    vip_buffer_t *buff = VIP_NULL;
    vip_uint32_t i = 0;
    vip_address_t physical = 0;
    vip_uint32_t mem_id = 0;
    VIP_FUNC_ENTER

    if ((VIP_NULL == create_param) || (VIP_NULL == buffer) ||
        (VIP_NULL == handle_logical) || (0 == handle_size)) {
        vip_error("fail to create buffer from handle, parameter is NULL,"
                    "param=%p, buffer=%p, logical=%p\n",
                    create_param, buffer, handle_logical);
        VIP_FUNC_EXIT
        return VIP_ERROR_INVALID_ARGUMENTS;
    }

    if (((VIPPTR_TO_UINT64(handle_logical)) & (vpmdCPU_CACHE_LINE_SIZE - 1)) != 0) {
        vip_error("fail handle_logical=%p not align to 0x%x\n",
                    handle_logical, vpmdCPU_CACHE_LINE_SIZE);
        VIP_FUNC_EXIT
        return VIP_ERROR_INVALID_ARGUMENTS;
    }
    if ((handle_size & (vpmdCPU_CACHE_LINE_SIZE - 1)) != 0) {
        vip_error("fail handle_size=0x%x not align to 0x%x\n",
                    handle_size, vpmdCPU_CACHE_LINE_SIZE);
        VIP_FUNC_EXIT
        return VIP_ERROR_INVALID_ARGUMENTS;
    }

    buf_size = nbg_get_buf_format_bit(create_param->data_format);
    for (i = 0; i < create_param->num_of_dims; i++) {
        buf_size *= create_param->sizes[i];
        if (0 == i) {
            /* round up */
            buf_size = (buf_size + BITS_PER_BYTE - 1) / BITS_PER_BYTE;
        }
    }

    if (VIP_ALIGN(buf_size, vpmdCPU_CACHE_LINE_SIZE) > handle_size) {
        vip_error("fail to create buffer. buffer size is bigger than handle size."
                    "0x%x > 0x%x, shape [",
                     buf_size, handle_size);
        vip_print("data format=%d ", create_param->data_format);
        for (i = 0; i < create_param->num_of_dims; i++) {
            vip_print("%d ", create_param->sizes[i]);
        }
        vip_print("]\n");
        VIP_FUNC_EXIT
        return VIP_ERROR_INVALID_ARGUMENTS;
    }

    if ((0 == create_param->num_of_dims) || (0 == buf_size)) {
        vip_error("fail to create buffer, dim_num=%d, shape[%d, %d, %d, %d] \n",
                     create_param->num_of_dims, create_param->sizes[0], create_param->sizes[1],
                     create_param->sizes[2], create_param->sizes[3]);
        vip_error("fail to create buffer, param is invalid\n");
        VIP_FUNC_EXIT
        return VIP_ERROR_INVALID_ARGUMENTS;
    }

    status = viphal_allocate_memory(sizeof(vip_buffer_t), (void **)&buff);
    if (VIP_NULL == buff) {
        vip_error("fail to allocate memory for buffer.\n");
        *buffer = VIP_NULL;
        VIP_FUNC_EXIT
        return VIP_ERROR_OUT_OF_MEMORY;
    }

    NBGLK_CREATE_BUF_INIT();

    status = viphal_wrap_usermemory(handle_logical, handle_size,
                        create_param->memory_type | VIPDRV_VIDEO_MEM_ALLOC_MMU_PAGE_WRITE,
                        &physical, &mem_id);
    if (status != VIP_SUCCESS) {
        vip_error("fail to create buffer from handle=%p, size=0x%xbytes\n",
                     handle_logical, handle_size);
        vipGoOnError(status);
    }

    buff->memory.physical = physical;
    buff->memory.logical = (vip_uint8_t *)handle_logical;
    buff->memory.size = handle_size;
    buff->memory.mem_id = mem_id;
    buff->allocated = NBGLK_BUFFER_ALLOCATE_FROM_USER_MEMORY;
    buff->object.type = NBGLK_OBJ_BUFFER;
    *buffer = buff;

    /*
    physical: when MMU enabled, physical is NPU's virtual address. when MMU disable, physical is NPU's physical address
    logical: CPU's logical address.
    */
    vip_debug("from handle logical=%p, phyical=0x%llx, mem_id=0x%x, size=0x%x\n", buff->memory.logical,
                 buff->memory.physical, buff->memory.mem_id, handle_size);

    VIP_FUNC_EXIT
    return status;

onError:
    vip_error("fail to create vip buffer for handle\n");
    if (buff != VIP_NULL) {
        viphal_free_memory(buff);
    }
    VIP_FUNC_EXIT
    return status;
}

/*
@brief Create a vip buffer from user fd(file descriptor).
         only  support create buffer from dma-buf on Linux.
         the vip_buffer created by this APi doesn't support flush CPU cache in driver.
         So the dma-buf should be a non-cache buffer or flush CPU on Host control.
@param create_param The parametes of buffer be created.
@param memory_size the size of user memory.
    the handle_size should be aligned to 64byte(vpmdCPU_CACHE_LINE_SIZE) for easy flash CPU cache.
@param buffer vip lite buffer object.
*/
VIP_API
vip_status_e vip_create_buffer_from_fd(
    IN const vip_buffer_create_params_t *create_param,
    IN vip_uint32_t fd,
    IN vip_uint32_t memory_size,
    OUT vip_buffer *buffer
    )
{
#if vpmdENABLE_CREATE_BUF_FD
    vip_status_e status = VIP_SUCCESS;
    vip_uint32_t buf_size = 0;
    vip_buffer_t *buff = VIP_NULL;
    vip_uint32_t i = 0;
    vip_address_t physical = 0;
    vip_uint8_t *logical = VIP_NULL;
    vip_uint32_t mem_id = 0;
    VIP_FUNC_ENTER

    if ((VIP_NULL == create_param) || (VIP_NULL == buffer)) {
        vip_error("fail to create buffer from handle, parameter is NULL,"
                    "param=%p, buffer=%p, fd=%p\n",
                    create_param, buffer, fd);
        VIP_FUNC_EXIT
        return VIP_ERROR_INVALID_ARGUMENTS;
    }

    buf_size = nbg_get_buf_format_bit(create_param->data_format);
    for (i = 0; i < create_param->num_of_dims; i++) {
        buf_size *= create_param->sizes[i];
        if (0 == i) {
            /* round up */
            buf_size = (buf_size + BITS_PER_BYTE - 1) / BITS_PER_BYTE;
        }
    }

    if (buf_size > memory_size) {
        vip_error("fail to create buffer. buffer size is bigger than fd memory size."
                    "0x%x > 0x%x, shape [",
                     buf_size, memory_size);
        vip_print("data format=%d ", create_param->data_format);
        for (i = 0; i < create_param->num_of_dims; i++) {
            vip_print("%d ", create_param->sizes[i]);
        }
        vip_print("]\n");
        VIP_FUNC_EXIT
        return VIP_ERROR_INVALID_ARGUMENTS;
    }

    if ((0 == create_param->num_of_dims) || (0 == buf_size)) {
        vip_error("fail to create buffer, dim_num=%d, shape[%d, %d, %d, %d] \n",
                     create_param->num_of_dims, create_param->sizes[0], create_param->sizes[1],
                     create_param->sizes[2], create_param->sizes[3]);
        vip_error("fail to create buffer, param is invalid\n");
        VIP_FUNC_EXIT
        return VIP_ERROR_INVALID_ARGUMENTS;
    }

    status = viphal_allocate_memory(sizeof(vip_buffer_t), (void **)&buff);
    if ((VIP_NULL == buff) || (status != VIP_SUCCESS)) {
        vip_error("fail to allocate memory for buffer.\n");
        *buffer = VIP_NULL;
        VIP_FUNC_EXIT
        return VIP_ERROR_OUT_OF_MEMORY;
    }

    NBGLK_CREATE_BUF_INIT();

    status = viphal_wrap_userfd(fd, memory_size, create_param->memory_type,
                                        &physical, &logical, &mem_id);
    if (status != VIP_SUCCESS) {
        /* fail to wrap user memory, and allocate a video memory for it */
        vip_error("fail to create buffer from fd=%p, size=0x%xbytes\n",
                     fd, memory_size);
        vipGoOnError(status);
    }

    buff->memory.physical = physical;
    buff->memory.logical = logical;
    buff->memory.size = memory_size;
    buff->memory.mem_id = mem_id;
    buff->allocated = NBGLK_BUFFER_ALLOCATE_FROM_USER_DMA_BUF;
    buff->object.type = NBGLK_OBJ_BUFFER;
    *buffer = buff;

    vip_debug("from fd=%d, phyical=0x%llx, logical=%p, mem_id=0x%x, size=0x%x\n",
                 fd, physical, logical, mem_id, memory_size);

    VIP_FUNC_EXIT
    return status;

onError:
    vip_error("fail to create vip buffer for user fd\n");
    if (buff != VIP_NULL) {
        viphal_free_memory(buff);
    }
    VIP_FUNC_EXIT
    return status;
#else
    vip_error("not support this feature, please set vpmdENABLE_CREATE_BUF_FD to 1\n");
    return VIP_ERROR_FAILURE;
#endif
}


/*
@brief destroy viplite buffer object.
@param buffer vip_buffer object.
*/
VIP_API
vip_status_e vip_destroy_buffer(
    IN vip_buffer buffer
    )
{
    vip_status_e status = VIP_SUCCESS;
    VIP_FUNC_ENTER

    if (VIP_NULL == buffer) {
        vip_error("fail to destroy buffer, buffer is NULL\n");
        VIP_FUNC_EXIT
        return VIP_ERROR_IO;
    }
    if (buffer->object.type != NBGLK_OBJ_BUFFER) {
        vip_error("fail to destroy buffer, type=%d mismatched\n", buffer->object.type);
        VIP_FUNC_EXIT
        return VIP_ERROR_INVALID_ARGUMENTS;
    }

    if (NBGLK_BUFFER_ALLOCATE_FROM_INTERNAL == buffer->allocated) {
        status = viphal_free_videomemory(buffer->memory.mem_id);
        if (status != VIP_SUCCESS) {
            vip_error("fail to free video memory\n");
            VIP_FUNC_EXIT
            return status;
        }
    }
    else if (NBGLK_BUFFER_ALLOCATE_FROM_USER_MEMORY == buffer->allocated) {
        if (buffer->memory.mem_id != 0) {
            viphal_unwrap_usermemory(buffer->memory.mem_id);
        }
    }
    else if (NBGLK_BUFFER_ALLOCATE_FROM_USER_PHYSICAL == buffer->allocated) {
        if (buffer->memory.mem_id != 0) {
            viphal_unwrap_userphysical(buffer->memory.mem_id);
        }
    }
#if vpmdENABLE_CREATE_BUF_FD
    else if (NBGLK_BUFFER_ALLOCATE_FROM_USER_DMA_BUF == buffer->allocated) {
        viphal_unwrap_userfd(buffer->memory.mem_id);
    }
#endif

    buffer->memory.mem_id = 0;
    buffer->memory.logical = VIP_NULL;
    buffer->memory.physical = 0;
    buffer->memory.size = 0;
    if (NBGLK_BUFFER_ALLOCATE_NONE != buffer->allocated) {
        status = viphal_free_memory(buffer);
        if (status != VIP_SUCCESS) {
            vip_error("fail to free buffer memory\n");
            buffer->allocated = NBGLK_BUFFER_ALLOCATE_NONE;
        }
    }

    VIP_FUNC_EXIT
    return status;
}

/*
@brief get the logical address of buffer.
@param buffer vip_buffer object.
*/
VIP_API
void * vip_map_buffer(
    IN vip_buffer buffer
    )
{
    vip_uint8_t *logi = VIP_NULL;
    VIP_FUNC_ENTER
    if (VIP_NULL != buffer) {
        if (VIP_NULL == buffer->memory.logical) {
            if (VIP_SUCCESS != viphal_map_userlogical(buffer->memory.mem_id, &buffer->memory.logical)) {
                vip_error("buffer %p map user logical fail\n", buffer);
            }
        }
        logi = buffer->memory.logical;
    }
    else {
        vip_error("error, buffer is NULL\n");
    }
    vip_debug("map buffer=%p\n", logi);
    VIP_FUNC_EXIT
    return (void*)logi;
}

/*
@brief unmap buffer, NOT use.
@param buffer vip_buffer object.
*/
VIP_API
vip_status_e vip_unmap_buffer(
    IN vip_buffer buffer
    )
{
    VIP_FUNC_ENTER
    vip_debug("unmap buffer=%p\n", buffer);
    VIP_FUNC_EXIT
    return VIP_SUCCESS;
}

/*
@brief get the size of buffer.
@param buffer vip_buffer object.
*/
VIP_API
vip_uint32_t vip_get_buffer_size(
    IN vip_buffer buffer
    )
{
    vip_size_t size = 0;
    VIP_FUNC_ENTER
    if (buffer != VIP_NULL) {
        size = buffer->memory.size;
    }
    else{
        vip_error("error, buffer is NULL\n");
    }
    VIP_FUNC_EXIT
    return SIZE_CAST(vip_uint32_t, size);
}

/*
@brief Operate the vip buffer CPU cache. flush, invalidate cache.
       You should call vip_flush_buffer API to flush buffer cache for input.
       and invalidate buffer for network's output if these memories with CPU cache.
@param buffer The vip buffer object.
@param type The type of this operation. see vip_buffer_operation_type_e.
*/
VIP_API
vip_status_e vip_flush_buffer(
    IN vip_buffer buffer,
    IN vip_buffer_operation_type_e type
    )
{
    vip_status_e status = VIP_SUCCESS;
#if vpmdENABLE_FLUSH_CPU_CACHE
    nbglk_videomemory_t *memory = VIP_NULL;
    VIP_FUNC_ENTER

    if (VIP_NULL == buffer) {
        vip_error("fail to flush vip buffer, buffer is NULL\n");
        VIP_FUNC_EXIT
        return VIP_ERROR_FAILURE;
    }

    if ((buffer->allocated <= NBGLK_BUFFER_ALLOCATE_NONE) ||
        (buffer->allocated >= NBGLK_BUFFER_ALLOCATE_MAX)) {
        vip_error("fail to flush vip buffer, buffer doesn't allocated\n");
        VIP_FUNC_EXIT
        return VIP_ERROR_FAILURE;
    }

    memory = &buffer->memory;

    vip_debug("flush buffer logical=%p, physical=0x%llx, size=0x%x, mem_id=0x%x, type=%s\n",
                 memory->logical, memory->physical, memory->size, memory->mem_id,
                 (type == VIP_BUFFER_OPER_TYPE_FLUSH) ? "FLUSH" : "INVALIDATE");

    switch (type) {
    case VIP_BUFFER_OPER_TYPE_FLUSH:
    {
        status = viphal_flush_cache(memory->mem_id, VIPDRV_CACHE_FLUSH);
    }
    break;
    case VIP_BUFFER_OPER_TYPE_INVALIDATE:
    {
        status = viphal_flush_cache(memory->mem_id, VIPDRV_CACHE_INVALID);
    }
    break;
    default:
        vip_error("flush buffer cache not supports this type=%d\n", type);
        status = VIP_ERROR_FAILURE;
        break;
    }

    VIP_FUNC_EXIT
#endif
    return status;
}

