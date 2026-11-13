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

#include <nbg_lk_node.h>
#if vpmdENABLE_NODE
#include <vip_lite_node.h>
#include <vip_lite.h>
#include <nbg_lk_network.h>
#include <nbg_lk_context.h>
#include <nbg_lk_group.h>
#include <nbg_lk_debug.h>
#include <vip_hal.h>
#include <nbg_fmt_util.h>
#include <nbg_lk_relocate.h>
#include <vip_hal_os_port.h>
#include <nbg_lk_capture.h>


#define NBGLK_CREATE_IMAGE_INIT() \
{   \
    buf->param.height = height; \
    buf->param.width = width;   \
    buf->param.depth = is_grey ? 1 : 3; \
    buf->param.color = color;   \
    switch(color) { \
        case VIP_IMAGE_IYUV:    \
        case VIP_IMAGE_U8:  \
            buf->param.data_format = VIP_BUFFER_FORMAT_UINT8;   \
            break;  \
        default:    \
            vip_info("create image not support this color type %d\n", color); \
            break;  \
    }   \
    buf->object.type = NBGLK_OBJ_IMAGE; \
    size_table[0] = VIP_ALIGN(height * width * nbg_get_buf_format_byte(buf->param.data_format), 64); \
    size_table[1] = VIP_ALIGN(size_table[0] / 4, 64); \
    size_table[2] = VIP_ALIGN(size_table[0] / 4, 64); \
}

#if (vpmdENABLE_HANG_DUMP > 1) || (vpmdENABLE_CAPTURE > 1)
vip_status_e nbglk_capture_node_inputs(
    vip_node node,
    viphal_file_t capture_file
    )
{
    vip_uint32_t i = 0;
    vip_char_t buffer[256];
    nbglk_object_t *object = VIP_NULL;

    for (i = 0; i < node->input_num; i++) {
        object = (nbglk_object_t*)node->inputs[i];
        vip_info("dump input\n");
        NBGLK_GET_STRING(buffer, 255, "#[input %d dump]\n", i);
        NBGLK_DUMP_STRING(capture_file, buffer);

        if (NBGLK_OBJ_IMAGE == *((vip_uint32_t*)object)) {
            vip_image image = (vip_image)node->inputs[i];
            vip_uint32_t j = 0;
            for (j = 0; j < image->param.depth; j++) {
                NBGLK_GET_STRING(buffer, 255, "#[input %d depth_%d]\n", i, j);
                NBGLK_DUMP_STRING(capture_file, buffer);
                if (image->memories[j].logical != VIP_NULL) {
                    nbglk_capture_buffer(capture_file, image->memories[j].logical,
                                        image->memories[j].physical,
                                        image->memories[j].size, NBGLK_CAPTURE_BUF_MEMORY);
                }
                else {
                    vip_error("capture input %d logical address is NULL\n", i);
                }
            }
        }
    }

    return VIP_SUCCESS;
}

vip_status_e nbglk_capture_node_outputs(
    vip_node node,
    viphal_file_t capture_file
    )
{
    vip_uint32_t i = 0;
    vip_char_t buffer_st[256];
    nbglk_object_t *object = VIP_NULL;

    for (i = 0; i < node->input_num; i++) {
        object = (nbglk_object_t*)node->outputs[i];
        vip_info("dump output\n");
        NBGLK_GET_STRING(buffer_st, 255, "#[output %d dump]\n", i);
        NBGLK_DUMP_STRING(capture_file, buffer_st);

        if (NBGLK_OBJ_BUFFER == *((vip_uint32_t*)object)) {
            vip_buffer buffer = (vip_buffer)node->outputs[i];
            if (buffer->memory.logical != VIP_NULL) {
                nbglk_capture_buffer(capture_file, buffer->memory.logical,
                                    buffer->memory.physical,
                                    buffer->memory.size, NBGLK_CAPTURE_BUF_VERIFY);
            }
            else {
                vip_error("capture output %d logical address is NULL\n", i);
            }
        }
    }

    return VIP_SUCCESS;
}

static vip_status_e nbglk_capture_node(
    vip_node node
    )
{
    vip_status_e status = VIP_SUCCESS;
    nbglk_context_t *context = nbglk_get_context();
    viphal_file_t capture_file = VIP_NULL;
    vip_char_t buffer[256];
    vip_uint32_t i = 0;
#if vpmdENABLE_HANG_DUMP > 1
    viphal_file_t file= VIP_NULL;
#endif

    if (context->func_config.enable_capture == vip_false_e) {
        return VIP_SUCCESS;
    }

    if (node->capture_done != 0) {
        return VIP_SUCCESS;
    }

#if vpmdENABLE_CAPTURE
    capture_file = context->capture_file;
#endif
#if vpmdENABLE_HANG_DUMP > 1
    {
        vip_char_t file_name[256];
        vip_uint32_t pid = viphal_os_get_pid();
        vip_uint32_t tid = viphal_os_get_tid();
        vip_uint32_t time = (vip_uint32_t)(viphal_os_get_time() / 100);/* 0.1ms unit */
        viphal_os_snprint(file_name, 255, "%s/viplite_hang_capture_node_%x_%x_%x.log",
                         vpmdSAVE_FILE_PATH, time, pid, tid);
        file = viphal_os_open_file(file_name, FILE_APPENDTEXT);
        if (file != VIP_NULL) {
            capture_file = file;
        }
        vip_error("catpure file name %s\n", file_name);
    }
#endif

    nbglk_capture_node_inputs(node, capture_file);

    NBGLK_DUMP_STRING(capture_file, "#[scaler states]\n");
    nbglk_capture_buffer(capture_file, node->states_buffer.logical,
                         node->states_buffer.physical,
                         node->command_size,  NBGLK_CAPTURE_BUF_COMMAND);
    for (i = 0; i < context->devices[node->device_index].core_count; i++) {
        vip_uint32_t core_id = context->devices[node->device_index].core_id[i];
        NBGLK_GET_STRING(buffer, 255, "@[register.write %d 0x%05X 0x%08X]\n",
            core_id, 0x00654, node->states_buffer.physical);
        NBGLK_DUMP_STRING(capture_file, buffer);
        NBGLK_GET_STRING(buffer, 255, "@[register.write %d 0x%05X 0x%08X]\n",
            core_id, 0x003A4, 0x00010000 | ((node->command_size + 7) >> 3));
        NBGLK_DUMP_STRING(capture_file, buffer);
    }
    NBGLK_DUMP_STRING(capture_file, "@[stall]\n");

    nbglk_capture_node_outputs(node, capture_file);
    node->capture_done = 1;

#if vpmdENABLE_HANG_DUMP > 1
    if (file != VIP_NULL) {
        viphal_os_close_file(file);
    }
#endif

    return status;
}
#endif

vip_status_e nbglk_create_node(
    vip_node_t **node_ptr
    )
{
    vip_status_e status = VIP_SUCCESS;
    vip_char_t name[256];
    if (!node_ptr) {
        vip_error("fail to create node, parameter is NULL\n");
        return VIP_ERROR_INVALID_ARGUMENTS;
    }

    viphal_allocate_memory(sizeof(vip_node_t), (void **)node_ptr);
    if (!(*node_ptr)) {
        vip_error("fail to create node in allocate memory\n");
        return VIP_ERROR_OUT_OF_MEMORY;
    }
    viphal_zero_memory(*node_ptr, sizeof(vip_node_t));

    viphal_os_snprint(name, 255, "node_0x%x", *node_ptr);
    viphal_task_create(name, 1, &(*node_ptr)->task_id);

    return status;
}

static vip_status_e nbglk_run_node(
    vip_node_t *node
    )
{
    vip_status_e status = VIP_SUCCESS;
#if vpmdENABLE_CNN_PROFILING
    nbglk_context_t *context = nbglk_get_context();
#endif
    viphal_task_param_t param = { 0 };

#if vpmdENABLE_CNN_PROFILING
    if (context->func_config.enable_cnn_profile == vip_true_e) {
        node->start_time = viphal_os_get_time();
    }
#endif

    viphal_task_set_property(node->task_id, VIPDRV_TASK_CMD_SUBTASK,
        0, node->states_buffer.mem_id, node->states_buffer.size, 0);

    param.task_id = node->states_buffer.mem_id;
    param.subtask_count = 1;
    param.subtask_index = 0;
    param.device_index = 0;
    param.core_index = 0;
    param.core_cnt = 1;
    param.priority = 0;
    param.time_out = vpmdHARDWARE_TIMEOUT;
    status = viphal_submit_task(&param);
    if (status != VIP_SUCCESS) {
        vip_error("fail to commit node commands, status=%d.\n", status);
        vipGoOnError(status);
    }

    status = viphal_wait_task(node->task_id, 0);
    if (status != VIP_SUCCESS) {
        vip_error("user wait node complete time out, status=%d.\n", status);
        #if vpmdENABLE_HANG_DUMP > 1
        nbglk_capture_node(node);
        if (VIP_ERROR_RECOVERY == status) {
            node->capture_done = 0;
        }
        #endif
        vipGoOnError(status);
    }

#if vpmdENABLE_CNN_PROFILING
    if (context->func_config.enable_cnn_profile == vip_true_e) {
        vip_uint64_t diff_time = 0;
        vip_char_t string[NBGLK_NODE_TYPE_MAX][48] = {
          "NONE",
          "Scaler",
        };
        node->end_time = viphal_os_get_time();
        if (node->end_time > node->start_time) {
            diff_time = node->end_time - node->start_time;
        }
        vip_print("=== %s node process total time=%lld us or %lld ms===\n",
            string[node->type], diff_time, diff_time / 1000);
    }
#endif

#if (vpmdENABLE_CAPTURE > 1)
    nbglk_capture_node(node);
#endif

onError:
    return status;
}

/*
@brief destroy viplite image object.
@param image vip_image object.
*/
VIP_API
vip_status_e vip_destroy_image(
    IN vip_image image
    )
{
    vip_status_e status = VIP_SUCCESS;
    vip_uint32_t i = 0;
    VIP_FUNC_ENTER

    if (VIP_NULL == image) {
        vip_error("fail to destroy image, pointer is NULL\n");
        VIP_FUNC_EXIT
        return VIP_ERROR_IO;
    }

    if (NBGLK_BUFFER_ALLOCATE_FROM_INTERNAL == image->allocated) {
        for (i = 0; i < image->param.depth; i ++) {
            if (image->memories[i].logical) {
                viphal_free_videomemory(image->memories[i].mem_id);
            }
        }
    }

#if vpmdENABLE_MMU
    if (NBGLK_BUFFER_ALLOCATE_FROM_USER_MEMORY == image->allocated) {
        for (i = 0; i < image->param.depth; i ++) {
            if (image->memories[i].logical) {
                viphal_unwrap_usermemory(image->memories[i].mem_id);
            }
        }
    }
    else if (NBGLK_BUFFER_ALLOCATE_FROM_USER_PHYSICAL == image->allocated) {
        for (i = 0; i < image->param.depth; i ++) {
            if (image->memories[i].physical) {
                viphal_unwrap_userphysical(image->memories[i].mem_id);
            }
        }
    }
#endif

    for (i = 0; i < image->param.depth; i ++) {
        image->memories[i].mem_id = 0;
        image->memories[i].logical = VIP_NULL;
        image->memories[i].physical = 0;
        image->memories[i].size = 0;
    }
    if (NBGLK_BUFFER_ALLOCATE_NONE != image->allocated) {
        status = viphal_free_memory(image);
        if (status != VIP_SUCCESS) {
            vip_error("fail to free image memory\n");
        }
    }

    VIP_FUNC_EXIT
    return status;
}

/*! \brief Create a image buffer from physical
 *\param [in] height Image height.
 *\param [in] width Image Width.
 *\param [in] color Image color, see vip_image_color_e.
 *\param [in] addrs Logical address for each channel, index 0=Y, 1=U, 2=V.
 *\param [out] image vip lite image buffer object.
 *\return <tt>\ref vip_status_e </tt>
 */
VIP_API
vip_status_e vip_create_image_from_physical(
    IN vip_uint32_t height,
    IN vip_uint32_t width,
    IN vip_image_color_e color,
    IN vip_address_t addrs[],
    OUT vip_image *image
    )
{
    vip_status_e status = VIP_SUCCESS;
    vip_image_t *buf = VIP_NULL;
    vip_uint32_t i = 0;
    const vip_bool_e is_grey = (color == VIP_IMAGE_U8);
    vip_uint32_t mem_id = 0;
    vip_uint32_t size_table[3] = { 0 };
    vip_address_t physical = 0;
    vip_uint8_t *logical;
    VIP_FUNC_ENTER

    if (color != VIP_IMAGE_IYUV && color != VIP_IMAGE_U8) {
        vip_error("not support this color\n");
        VIP_FUNC_EXIT
        return VIP_ERROR_INVALID_ARGUMENTS;
    }
    status = viphal_allocate_memory(sizeof(vip_image_t), (void **)&buf);
    if (VIP_NULL == buf) {
        vip_error("fail to allocate memory for buffer.\n");
        *image = VIP_NULL;
        VIP_FUNC_EXIT
        return VIP_ERROR_OUT_OF_MEMORY;
    }
    viphal_zero_memory(buf, sizeof(vip_image_t));
    buf->param.memory_type = VIP_BUFFER_MEMORY_TYPE_HOST;
    buf->allocated = NBGLK_BUFFER_ALLOCATE_FROM_USER_PHYSICAL;
    NBGLK_CREATE_IMAGE_INIT();

    for (i = 0; i < buf->param.depth; i ++) {
        if (addrs[i] == VIP_NULL) {
            vip_error("Addr %d is NULL.\n", i);
            status = VIP_ERROR_INVALID_ARGUMENTS;
            break;
        }
        status = viphal_wrap_userphysical(&addrs[i], &size_table[i], 1, VIPDRV_VIDEO_MEM_ALLOC_MAP_USER,
                                              buf->param.memory_type, &physical, &logical, &mem_id);
        if (status != VIP_SUCCESS) {
            vip_error("fail to create buffer from physical.\n");
            break;
        }
        #if vpmdENABLE_MMU
        buf->memories[i].physical = physical;
        #else
        if (addrs[i] > 0xFFFFFFFF) {
            vip_error("fail to create buffer from physical mmu is disabled, physical address=0x%"PRIx64"\n",
                        addrs[i]);
            break;
        }
        buf->memories[i].physical = (vip_uint32_t)addrs[i];
        #endif
        buf->memories[i].logical = logical;
        buf->memories[i].size = size_table[i];
        buf->memories[i].mem_id = mem_id;

        vip_debug("logical=%p, phyical(vip virtual)=0x%llx, mem_id=0x%x, size=0x%x\n",
                buf->memories[i].logical, buf->memories[i].physical,
                buf->memories[i].mem_id, size_table[i]);
    }

    if (status != VIP_SUCCESS ) {
        vip_destroy_image(buf);
        buf = VIP_NULL;
    }

    *image = buf;
    VIP_FUNC_EXIT
    return status;
}

/*! \brief Create a image buffer from handle
 *\param [in] height Image height.
 *\param [in] width Image Width.
 *\param [in] color Image color, see vip_image_color_e.
 *\param [in] addrs Logical address for each channel, index 0=Y, 1=U, 2=V.
 *\param [out] image vip lite image buffer object.
 *\return <tt>\ref vip_status_e </tt>
 */
VIP_API
vip_status_e vip_create_image_from_handle(
    IN vip_uint32_t height,
    IN vip_uint32_t width,
    IN vip_image_color_e color,
    IN void* const addrs[],
    OUT vip_image *image
    )
{
    vip_status_e status = VIP_SUCCESS;
#if vpmdENABLE_MMU
    vip_image_t *buf = VIP_NULL;
    vip_uint32_t i = 0;
    const vip_bool_e is_grey = (color == VIP_IMAGE_U8);
    vip_uint32_t size_table[3] = { 0 };
    VIP_FUNC_ENTER

    if (color != VIP_IMAGE_IYUV && color != VIP_IMAGE_U8) {
        vip_error("not support this color\n");
        VIP_FUNC_EXIT
        return VIP_ERROR_INVALID_ARGUMENTS;
    }
    status = viphal_allocate_memory(sizeof(vip_image_t), (void **)&buf);
    if (VIP_NULL == buf) {
        vip_error("fail to allocate memory for buffer.\n");
        *image = VIP_NULL;
        VIP_FUNC_EXIT
        return VIP_ERROR_OUT_OF_MEMORY;
    }
    viphal_zero_memory(buf, sizeof(vip_image_t));
    buf->param.memory_type = VIP_BUFFER_MEMORY_TYPE_HOST;
    buf->allocated = NBGLK_BUFFER_ALLOCATE_FROM_USER_MEMORY;
    NBGLK_CREATE_IMAGE_INIT();

    for (i = 0; i < buf->param.depth; i ++) {
        buf->memories[i].size = size_table[i];
        if (addrs[i] == VIP_NULL) {
            vip_error("Addr %d is NULL.\n", i);
            status = VIP_ERROR_INVALID_ARGUMENTS;
            break;
        }
        status = viphal_wrap_usermemory(addrs[i], size_table[i],
                        buf->param.memory_type | VIPDRV_VIDEO_MEM_ALLOC_MMU_PAGE_WRITE,
                        &buf->memories[i].physical, &buf->memories[i].mem_id);
        if (status != VIP_SUCCESS) {
            vip_error("fail to wrap user memory channel=%d\n", i);
            break;
        }
        buf->memories[i].logical = (vip_uint8_t *)addrs[i];

        vip_debug("logical=%p, phyical=0x%llx, mem_id=0x%x, size=0x%x\n",
                buf->memories[i].logical, buf->memories[i].physical,
                buf->memories[i].mem_id, size_table[i]);
    }

    if (status != VIP_SUCCESS) {
        vip_destroy_image(buf);
        buf = VIP_NULL;
    }

    *image = buf;
    VIP_FUNC_EXIT
    return status;
#else
    vip_error("fail to create buffer from memory, NOT support. Please enable MMU first\n");
    status = VIP_ERROR_FAILURE;
#endif
    return status;
}


/*! \brief Create a image buffer. Memory is allocated from driver management.
 *\param [in] height Image height.
 *\param [in] width Image Width.
 *\param [in] color Image color, see vip_image_color_e.
 *\param [out] image vip lite image buffer object.
 *\return <tt>\ref vip_status_e </tt>
 */
VIP_API
vip_status_e vip_create_image(
    IN vip_uint32_t height,
    IN vip_uint32_t width,
    IN vip_image_color_e color,
    OUT vip_image *image
    )
{
    vip_status_e status = VIP_SUCCESS;
    vip_image_t *buf = VIP_NULL;
    vip_uint32_t i = 0;
    const vip_bool_e is_grey = (color == VIP_IMAGE_U8);
    vip_uint32_t size_table[3] = { 0 };
    VIP_FUNC_ENTER

    if (color != VIP_IMAGE_IYUV && color != VIP_IMAGE_U8) {
        vip_error("not support this color\n");
        VIP_FUNC_EXIT
        return VIP_ERROR_INVALID_ARGUMENTS;
    }
    status = viphal_allocate_memory(sizeof(vip_image_t), (void **)&buf);
    if (VIP_NULL == buf) {
        vip_error("fail to allocate memory for buffer.\n");
        *image = VIP_NULL;
        VIP_FUNC_EXIT
        return VIP_ERROR_OUT_OF_MEMORY;
    }
    viphal_zero_memory(buf, sizeof(vip_image_t));
    buf->param.memory_type = VIP_BUFFER_MEMORY_TYPE_DEFAULT;
    buf->allocated = NBGLK_BUFFER_ALLOCATE_FROM_INTERNAL;
    NBGLK_CREATE_IMAGE_INIT();

    for (i = 0; i < buf->param.depth; i ++) {
        buf->memories[i].size = size_table[i];
        status = viphal_allocate_videomemory((vip_uint8_t **)&buf->memories[i].logical,
                                                  &buf->memories[i].mem_id, &buf->memories[i].physical,
                                                  size_table[i], vipdMEMORY_ALIGN_SIZE,
                                                  VIPDRV_VIDEO_MEM_ALLOC_MMU_PAGE_WRITE);
        if (status != VIP_SUCCESS) {
            vip_error("fail to allocate video memory channel=%d\n", i);
            break;
        }
        vip_debug("logical=%p, phyical=0x%llx, mem_id=0x%x, size=0x%x\n",
                buf->memories[i].logical, buf->memories[i].physical,
                buf->memories[i].mem_id, size_table[i]);
    }
    if (status != VIP_SUCCESS) {
        vip_destroy_image(buf);
        buf = VIP_NULL;
    }

    *image = buf;
    VIP_FUNC_EXIT
    return status;
}

/*
@brief get the logical address of image buffer.
@param buffer vip_image object.
@param depth_count the number of image depath getting the logical.
@param logical the logical address array of vip_image.
*/
VIP_API
vip_status_e vip_map_image(
    IN vip_image image,
    IN vip_uint32_t depth_count,
    OUT void **logical
    )
{
    vip_status_e status = VIP_SUCCESS;
    vip_uint32_t i = 0;
    VIP_FUNC_ENTER
    if (VIP_NULL == image) {
        vip_error("image is null\n");
        VIP_FUNC_EXIT
        return VIP_ERROR_INVALID_ARGUMENTS;
    }
    if (depth_count > image->param.depth) {
        vip_error("failed , image depth=%d, depth_count=%d\n", image->param.depth, depth_count);
        VIP_FUNC_EXIT
        return VIP_ERROR_FAILURE;
    }
    for (i = 0; i < depth_count; i++) {
        logical[i] = (void*)image->memories[i].logical;
    }

    VIP_FUNC_EXIT
    return status;
}

/*
@brief put the logical address of image buffer.
@param buffer vip_buffer object.
*/
VIP_API
vip_status_e vip_unmap_image(
    IN vip_image image
    )
{
    return VIP_SUCCESS;
}

/*! \brief Destroy a node object
 *\details Release all resources allocated for this node.
 *\param [in] node The opaque handle to the node to be destroyed
 *\return <tt>\ref vip_status_e </tt>
 */
VIP_API
vip_status_e vip_destroy_node(
    IN vip_node node
    )
{
    VIP_FUNC_ENTER
    if (node) {
        if (node->inputs != VIP_NULL) {
            viphal_free_memory(node->inputs);
            node->inputs = VIP_NULL;
        }
        if (node->outputs != VIP_NULL) {
            viphal_free_memory(node->outputs);
            node->outputs = VIP_NULL;
        }

        if (node->states_buffer.mem_id != 0) {
            viphal_free_videomemory(node->states_buffer.mem_id);
            node->states_buffer.mem_id = 0;
        }
        viphal_task_destroy(node->task_id);
        viphal_free_memory(node);
    }

    VIP_FUNC_EXIT
    return VIP_SUCCESS;
}

/*! \brief. Kick off the node execution and send command buffer of this node to VIP hardware.
 *\details This function can be called multiple times.
           Every time it's called it would do inference with current attached
 *\param [in] node The opaque handle to the node to be executed.
 *\return <tt>\ref vip_status_e </tt>
 */
VIP_API
vip_status_e vip_run_node(
    IN vip_node node
    )
{
    vip_status_e status = VIP_SUCCESS;
    VIP_FUNC_ENTER
    if (VIP_NULL == node) {
        vip_error("fail to run node, node is NULL\n");
        VIP_FUNC_EXIT
        return VIP_ERROR_INVALID_ARGUMENTS;
    }
    /* check node's input/output before running */
    if (NBGLK_NODE_TYPE_SCALER == node->type) {
        vip_image image = (vip_image)node->inputs[0];
        vip_buffer buffer = (vip_buffer)node->outputs[0];
        if (VIP_NULL == image) {
            vip_error("fail to run node input%d is NULL\n");
            return VIP_ERROR_INVALID_ARGUMENTS;
        }
        else {
            if (0 == image->memories[0].size) {
                vip_error("fail to run node input size is  0\n");
                return VIP_ERROR_INVALID_ARGUMENTS;
            }
        }
        if (VIP_NULL == buffer) {
            vip_error("fail to run node output is NULL\n");
            return VIP_ERROR_INVALID_ARGUMENTS;
        }
        else {
            if (0 == buffer->memory.size) {
                vip_error("fail to run node output%d size is  0\n");
                return VIP_ERROR_INVALID_ARGUMENTS;
            }
        }
    }

    status = nbglk_run_node(node);
    VIP_FUNC_EXIT
    return status;
}

/*
@brief Operate the vip buffer CPU cache. flush, invalidate cache.
       You should call vip_flush_buffer API to flush buffer cache for input.
       and invalidate buffer for network's output if these memories with CPU cache.
@param image The vip image buffer object.
@param type The type of this operation. see vip_buffer_operation_type_e.
*/
VIP_API
vip_status_e vip_flush_image(
    IN vip_image image,
    IN vip_buffer_operation_type_e type
    )
{
    vip_status_e status = VIP_SUCCESS;
    nbglk_videomemory_t *memory = VIP_NULL;
    vip_uint32_t i = 0;
    VIP_FUNC_ENTER

    if (VIP_NULL == image) {
        vip_error("fail to flush vip image buffer, image is NULL\n");
        VIP_FUNC_EXIT
        return VIP_ERROR_FAILURE;
    }

    if ((image->allocated <= NBGLK_BUFFER_ALLOCATE_NONE) ||
        (image->allocated >= NBGLK_BUFFER_ALLOCATE_MAX)) {
        vip_error("fail to flush vip image buffer, buffer doesn't allocated\n");
        VIP_FUNC_EXIT
        return VIP_ERROR_FAILURE;
    }

    for (i = 0; i < image->param.depth; i ++) {
        memory = &image->memories[i];
        vip_info("flush buffer logical=%p, physical=0x%llx, size=0x%x, mem_id=0x%x, type=%s\n",
                     memory->logical, memory->physical, memory->size, memory->mem_id,
                     (type == VIP_BUFFER_OPER_TYPE_FLUSH) ? "FLUSH" : "INVALIDATE");
        switch (type) {
#if vpmdENABLE_FLUSH_CPU_CACHE
            case VIP_BUFFER_OPER_TYPE_FLUSH:
                status = viphal_flush_cache(memory->mem_id, VIPDRV_CACHE_FLUSH);
                break;

            case VIP_BUFFER_OPER_TYPE_INVALIDATE:
                status = viphal_flush_cache(memory->mem_id, VIPDRV_CACHE_INVALID);
                break;
#endif
            default:
                vip_error("flush buffer cache not supports this type=%d\n", type);
                status = VIP_ERROR_FAILURE;
                break;
        }
    }
    VIP_FUNC_EXIT
    return status;
}

/*! \brief Generate perprocess command stream
 *\param [in] type Preprocess type, see vip_preprocess_type_e.
 *\param [in] param Preprocess parameters. see vip_preprocess_yuv2rgb_t.
 *\param [in] param_size the Size of preprocess parameters.
 *\param [in] input_buffer Input buffer object.
 *\param [in] output_buffer Output buffer object.
 *\param [out] node Return the generated command stream.
 *\return <tt>\ref vip_status_e </tt>
 */
VIP_API
vip_status_e vip_create_preprocess(
    const vip_preprocess_type_e type,
    const vip_preprocess_yuv2rgb_t *param,
    const vip_uint32_t param_size,
    const vip_image input_buffer,
    const vip_buffer output_buffer,
    vip_node *out_node
    )
{
    vip_status_e status = VIP_ERROR_FAILURE;
    VIP_FUNC_ENTER
    status = nbglk_create_preprocess(type, param, param_size,
                                     input_buffer, output_buffer, out_node);
    VIP_FUNC_EXIT
    return status;
}

/*! \brief Generate perprocess command stream
 *\param [in] node Preprocess node to update the command stream.
 *\param [in] type Preprocess type, see vip_preprocess_type_e.
 *\param [in] param Preprocess parameters.
 *\param [in] param_size Size of preprocess parameters.
 *\param [in] input_buffer Input buffer object.
 *\param [in] output_buffer Output buffer object.
 *\return <tt>\ref vip_status_e </tt>
 */
VIP_API
vip_status_e vip_update_preprocess(
    vip_node node,
    const vip_preprocess_type_e type,
    const vip_preprocess_yuv2rgb_t *param,
    const vip_uint32_t param_size,
    const vip_image input_buffer,
    const vip_buffer output_buffer
    )
{
    vip_status_e status = VIP_ERROR_FAILURE;
    VIP_FUNC_ENTER
    status = nbglk_update_preprocess(node, type, param, param_size,
                                     input_buffer, output_buffer);
    VIP_FUNC_EXIT
    return status;
}

/*! \brief Add a vip_node object into group.
*\param group vip_group object, network be added into group.
*\param node vip_node added into group.
*\return <tt>\ref vip_status_e </tt>
*/
VIP_API
vip_status_e vip_add_node(
    IN vip_group group,
    IN vip_node node
    )
{
    vip_status_e status = VIP_SUCCESS;
#if vpmdENABLE_GROUP_MODE
    nbglk_videomemory_t* cmd = VIP_NULL;
    vip_uint32_t i = 0;
    vip_uint32_t count = 0;
#if vpmdENABLE_POLLING
    vip_bool_e with_event = vip_false_e;
#else
    vip_bool_e with_event = vip_true_e;
#endif
    VIP_FUNC_ENTER

#if vpmdENABLE_MULTIPLE_TASK
    viphal_os_lock_mutex(group->mutex);
#endif
    vip_info("add node=%p into group=%p\n", node, group);
    count = group->add_task_num;
    if (count > (group->max_task_num - 1)) {
        vip_error("fail to add network, task count is more than max=%d\n",
                     group->max_task_num);
        vipGoOnError(VIP_ERROR_FAILURE);
    }

    if (0 < count) {
        if (NBGLK_OBJ_NETWORK == group->tasks[0].object.type) {
            if (1 != ((vip_network)group->tasks[0].task)->nbg_desc.net_info.core_count) {
                vip_error("fail to add node, cur node's core cnt %d != group's %d\n",
                    1, ((vip_network)group->tasks[0].task)->nbg_desc.net_info.core_count);
                vipGoOnError(VIP_ERROR_FAILURE);
            }
        }
    }

    /* check command buffer is not duplicated */
    for (i = 0; i < group->add_task_num; i++) {
        nbglk_videomemory_t *buffer = &node->states_buffer;
        nbglk_group_item_get_command(&group->tasks[i], &cmd);
        if (buffer == cmd) {
            vip_error("faied to add node, not support the same node in a group\n");
            vipGoOnError(VIP_ERROR_FAILURE);
        }
    }

    group->tasks[count].object.type = NBGLK_OBJ_NODE;
    group->tasks[count].task = (vip_ptr)node;
    if (count > 0) {
        status = nbglk_group_item_get_command(&group->tasks[count-1], &cmd);
        if (status != VIP_SUCCESS) {
            vip_error("fail to get pre cmd for node, count=%d\n", count);
            vipGoOnError(status);
        }
        status = nbglk_link_command(cmd, &node->states_buffer);
        if (status != VIP_SUCCESS) {
            vip_error("Fail to link commands\n");
            group->tasks[count].object.type = NBGLK_OBJ_NONE;
            group->tasks[count].task = VIP_NULL;
            vipGoOnError(status);
        }

        node->command_size = node->states_buffer.size;
        status = nbglk_append_cmds(&node->states_buffer, &node->command_size, with_event, vip_false_e, 0);
        if (status != VIP_SUCCESS) {
            vip_error("fail to append commands in add node\n");
            vipGoOnError(status);
        }
    }

    group->add_task_num++;
    group->run_task_num = group->add_task_num;

    nbglk_group_item_get_command(&group->tasks[count], &cmd);
    viphal_task_set_property(group->task_id, VIPDRV_TASK_CMD_SUBTASK,
        count, cmd->mem_id, cmd->size, 0);

onError:
    VIP_FUNC_EXIT
#if vpmdENABLE_MULTIPLE_TASK
    viphal_os_unlock_mutex(group->mutex);
#endif
#else
    vip_error("not support group mode, please set vpmdENABLE_GROUP_MODE 1\n");
    status = VIP_ERROR_NOT_SUPPORTED;
#endif
    return status;
}
#endif
