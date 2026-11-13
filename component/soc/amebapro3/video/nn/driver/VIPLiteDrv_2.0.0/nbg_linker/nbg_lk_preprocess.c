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
#include <vip_lite_common.h>
#include <vip_lite.h>
#include <nbg_lk.h>
#include <nbg_lk_network.h>
#include <nbg_lk_context.h>
#include <nbg_lk_debug.h>
#include <vip_hal.h>
#include <nbg_lk_relocate.h>
#include <vip_hal_os_port.h>
#include "preprocess/vip_scaler.h"

#define INIT_CMDS_SIZE      16

static vip_status_e _preprocess_dump_info(
    const void *preprocess_param,
    vip_scaler_input_t  input,
    vip_scaler_output_t output,
    const vip_image input_buffer,
    const vip_buffer output_buffer
    )
{
#if vpmdENABLE_DEBUG_LOG > 3
    const vip_preprocess_yuv2rgb_t *param = (const vip_preprocess_yuv2rgb_t*)preprocess_param;
    static const vip_char_t string[12][48] = {
        "fp32",
        "fp16",
        "uint8",
        "int8",
        "uint16",
        "int16",
    };
    vip_debug("*******dump preprocess info start*********\n");
    vip_debug("addr input y=0x%08x, v=0x%08x, u=0x%08x, rgb_output=0x%08x\n",
                input.address_y, input.address_u, input.address_v, output.address);
    vip_debug("input_w=%d, input_h=%d\n", input_buffer->param.width, input_buffer->param.height);
    vip_debug("output_w=%d, output_h=%d\n", output_buffer->param.sizes[0], output_buffer->param.sizes[1]);
    vip_debug("output format=%s\n", string[output_buffer->param.data_format]);
    vip_debug("input roi[%d, %d, %d, %d]\n", param->input_roi[0], param->input_roi[1],
                 param->input_roi[2], param->input_roi[3]);
    vip_debug("output roi[%d, %d, %d, %d]\n", param->output_roi[0], param->output_roi[1],
                 param->output_roi[2], param->output_roi[3]);
    vip_debug("mean[%d, %d, %d]\n", param->mean[0], param->mean[1], param->mean[2]);
    vip_debug("fill rgb[%d, %d, %d]\n", param->fill_rgb[0], param->fill_rgb[1], param->fill_rgb[2]);
    vip_debug("scale=%f, y_only=%d, reverse_channel=%d, output_to_rect=%d\n",
                param->scale, param->y_only, param->reverse_channel, param->output_to_rect);
    vip_debug("*******dump preprocess info end*********\n");
#endif

    return VIP_SUCCESS;
}

static vip_status_e _preprocess_scaler(
    const vip_preprocess_yuv2rgb_t *preprocess_param,
    const vip_uint32_t  param_size,
    const vip_image     input_buffer,
    const vip_buffer    output_buffer,
    nbglk_videomemory_t *cmd_memory,
    vip_uint32_t       *cmd_size
    )
{
    const vip_preprocess_yuv2rgb_t* param = preprocess_param;
    vip_scaler_input_t  input;
    vip_scaler_output_t output;
    vip_uint32_t op_number = 0;
    vip_uint32_t total_command_size = 0;
    vip_int32_t used_cmd_buffer_size = 0;
    vip_scaler_rect_t input_rect;
    vip_scaler_rect_t output_rect;
    vip_scaler_split_info_t splitInfo[5];
    vip_uint8_t *stream = VIP_NULL;
    vip_status_e status = VIP_SUCCESS;
    nbglk_context_t *context = nbglk_get_context();
    vip_uint32_t tmp_size = INIT_CMDS_SIZE;
    vip_uint32_t axi_sram_total = 0;
    vip_uint32_t i = 0;
    vip_uint32_t *data = VIP_NULL;
    vip_uint32_t cnt = 0;

    if (!cmd_memory || !cmd_size) {
        vip_error("fail to preprocess scaler, param is NULL\n");
        return VIP_ERROR_INVALID_ARGUMENTS;
    }

    viphal_zero_memory(&input, sizeof(input));
    viphal_zero_memory(&output, sizeof(output));
    viphal_zero_memory(&input_rect, sizeof(vip_scaler_rect_t));
    viphal_zero_memory(&output_rect, sizeof(vip_scaler_rect_t));
    viphal_zero_memory(splitInfo, sizeof(splitInfo));

    input.size[0] = input_buffer->param.width;
    input.size[1] = input_buffer->param.height;
    if (param->y_only) {
        input.size[2] = 1;
    }
    else {
        input.size[2] = input_buffer->param.depth;
    }

    input.color = input_buffer->param.color;
    input.address_y = input_buffer->memories[0].physical;
    if (input.size[2] == 3) {
        input.address_u = input_buffer->memories[1].physical;
        input.address_v = input_buffer->memories[2].physical;
    }

    output.size[0] = output_buffer->param.sizes[0];
    output.size[1] = output_buffer->param.sizes[1];
    output.size[2] = output_buffer->param.sizes[2];
    output.address = output_buffer->memory.physical;
    output.format = output_buffer->param.data_format;
    output.quant_format = output_buffer->param.quant_format;
    switch (output.quant_format) {
        case VIP_BUFFER_QUANTIZE_DYNAMIC_FIXED_POINT:
            output.quant_param.dfp.fl = output_buffer->param.quant_data.dfp.fixed_point_pos;
            break;
        case VIP_BUFFER_QUANTIZE_TF_ASYMM:
            output.quant_param.asymm.scale = output_buffer->param.quant_data.affine.scale;
            output.quant_param.asymm.zero_point = output_buffer->param.quant_data.affine.zeroPoint;
            break;
        default:
            vip_error("not support this quant format %d\n", output.quant_format);
            break;
    }

    _preprocess_dump_info(preprocess_param, input, output, input_buffer, output_buffer);

    input_rect.start_x = param->input_roi[0];
    input_rect.start_y = param->input_roi[1];
    input_rect.end_x   = param->input_roi[2];
    input_rect.end_y   = param->input_roi[3];
    output_rect.start_x = param->output_roi[0];
    output_rect.start_y = param->output_roi[1];
    output_rect.end_x   = param->output_roi[2];
    output_rect.end_y   = param->output_roi[3];
    vip_init_scaler_info(1, &input, &output, &input_rect, &output_rect,
        (int)param->output_to_rect, &op_number, splitInfo);

    vip_debug("scaler op number=%d\n", op_number);
    total_command_size = VIP_SCALER_COMMAND_SIZE * op_number;

    if (context->vip_sram_size > 0) {
        tmp_size += 8;
    }
    if (axi_sram_total > 0) {
        tmp_size += 16;
    }
    total_command_size += tmp_size;

    if (total_command_size > cmd_memory->size) {
        if (cmd_memory->mem_id != 0) {
            /* Free old video memory */
            viphal_free_videomemory(cmd_memory->mem_id);
        }
        cmd_memory->size = total_command_size + APPEND_COMMAND_SIZE;
        /* Init video memory */
        vipOnError(viphal_allocate_videomemory((vip_uint8_t **)(&cmd_memory->logical),
                                         &cmd_memory->mem_id,
                                         &cmd_memory->physical,
                                         cmd_memory->size,
                                         vipdMEMORY_ALIGN_SIZE,
                                         VIPDRV_VIDEO_MEM_ALLOC_MAP_KERNEL |
                                         VIPDRV_VIDEO_MEM_ALLOC_4GB_ADDR));
    }

    for (i = 0; i < context->device_count; i++) {
        axi_sram_total += context->axi_sram_size[i];
    }
    data = (vip_uint32_t*)cmd_memory->logical;
    cnt = 0;
    data[cnt++] = 0x0801028A;
    data[cnt++] = 0x00000011;
    data[cnt++] = 0x08010E13;
    data[cnt++] = 0x00000002;
    if (context->vip_sram_size > 0) {
        data[cnt++] = 0x08010E4E;
        data[cnt++] = context->vip_sram_base;
    }
    if (axi_sram_total > 0) {
        data[cnt++] = 0x08010e4f;
        data[cnt++] = context->axi_sram_base[0];
        data[cnt++] = 0x08010e50;
        data[cnt++] = context->axi_sram_base[0] + axi_sram_total;
    }

    used_cmd_buffer_size = cnt * sizeof(vip_uint32_t);
    stream = cmd_memory->logical + used_cmd_buffer_size;

    used_cmd_buffer_size += vip_generate_scaler_command(&input, &output,
                                                       param->mean[0],
                                                       param->mean[1],
                                                       param->mean[2],
                                                       param->scale,
                                                       (int)param->reverse_channel,
                                                       param->fill_rgb[0],
                                                       param->fill_rgb[1],
                                                       param->fill_rgb[2],
                                                       (int)param->y_only,
                                                       op_number, splitInfo, (void*)stream);
    data = (vip_uint32_t*)(stream + used_cmd_buffer_size);
    cnt = 0;
    data[cnt] = 0x08010E03; cnt++;
    data[cnt] = 0x00000C23; cnt++;
    data[cnt] = 0x08010E03; cnt++;
    data[cnt] = 0x00000C23; cnt++;
    used_cmd_buffer_size += cnt * sizeof(vip_uint32_t);

    *cmd_size = (vip_uint32_t)used_cmd_buffer_size;
    vip_info("total_command_size=0x%x, real_cmd_size=0x%x\n", total_command_size, used_cmd_buffer_size);

onError:
    return status;
}

vip_status_e nbglk_create_preprocess(
    const vip_preprocess_type_e type,
    const vip_preprocess_yuv2rgb_t *param,
    const vip_uint32_t param_size,
    const vip_image input_buffer,
    const vip_buffer output_buffer,
    vip_node *out_node
    )
{
    vip_status_e status = VIP_SUCCESS;
    vip_uint32_t cmd_size = 0;
    vip_node node = VIP_NULL;
    nbglk_videomemory_t* memory = VIP_NULL;
#if vpmdENABLE_POLLING
    vip_bool_e with_event = vip_false_e;
#else
    vip_bool_e with_event = vip_true_e;
#endif

    if (!out_node) {
        vip_error("output node is NULL\n");
        return VIP_ERROR_INVALID_ARGUMENTS;
    }
    if (type != VIP_PREPROCESS_YUV2RGB) {
        vip_error("Only support VIP_PREPROCESS_YUV2RGB type, "
                    "Got %#x\n", type);
        return VIP_ERROR_INVALID_ARGUMENTS;
    }
    if (input_buffer->object.type != NBGLK_OBJ_IMAGE) {
        vip_error("Only support input image type, "
                    "Got %#x\n", input_buffer->object.type);
        return VIP_ERROR_INVALID_ARGUMENTS;
    }

    nbglk_create_node(&node);
    if (!node) {
        vip_error("faied to create node object\n");
        return VIP_ERROR_OUT_OF_MEMORY;
    }

    /* Create node */
    if (status == VIP_SUCCESS) {
        memory = &node->states_buffer;
        /* Generate preprocess command */
        switch (type) {
            case VIP_PREPROCESS_YUV2RGB:
                status = _preprocess_scaler(param, param_size, input_buffer,
                                            output_buffer, memory, &cmd_size);
                break;
            default:
                status = VIP_ERROR_FAILURE;
                vip_error("not support this type create preprocess %d\n", type);
                break;
        }

        node->object.type = NBGLK_OBJ_NODE;
        node->type = NBGLK_NODE_TYPE_SCALER;

        if (VIP_SUCCESS == status) {
            *out_node = node;
            /* append command */
            node->command_size = cmd_size;
            nbglk_append_cmds(memory, &node->command_size, with_event, vip_false_e, 0);
        }
    }

    if (status != VIP_SUCCESS) {
        if (node) {
            vip_error("fail to create prrprocess status=%d\n", status);
            viphal_free_memory(node);
            *out_node = node = VIP_NULL;
        }
    }
    else {
        node->input_num = 1;
        node->output_num = 1;
        vipOnError(viphal_allocate_memory(sizeof(vip_ptr*) * node->input_num, (vip_ptr*)&node->inputs));
        vipOnError(viphal_allocate_memory(sizeof(vip_ptr*) * node->output_num, (vip_ptr*)&node->outputs));
        node->inputs[0] = (vip_ptr)input_buffer;
        node->outputs[0] = (vip_ptr)output_buffer;
    }

#if vpmdENABLE_VIDEO_MEMORY_CACHE
    vipOnError(viphal_flush_cache(node->states_buffer.mem_id, VIPDRV_CACHE_FLUSH));
#endif
onError:
    return status;
}

vip_status_e nbglk_update_preprocess(
    vip_node node,
    const vip_preprocess_type_e type,
    const vip_preprocess_yuv2rgb_t *param,
    const vip_uint32_t param_size,
    const vip_image input_buffer,
    const vip_buffer output_buffer
    )
{
    vip_status_e status = VIP_SUCCESS;
    vip_uint32_t cmd_size = 0;
    nbglk_videomemory_t* memory = VIP_NULL;
#if vpmdENABLE_POLLING
    vip_bool_e with_event = vip_false_e;
#else
    vip_bool_e with_event = vip_true_e;
#endif

    if (type != VIP_PREPROCESS_YUV2RGB) {
        vip_error("Only support VIP_PREPROCESS_YUV2RGB type, "
                    "Got %#x\n",
                    type);
        return VIP_ERROR_INVALID_ARGUMENTS;
    }
    if (!node || !param || !input_buffer || !output_buffer) {
        vip_error("fail to update preprocess, param is NULL\n");
        return VIP_ERROR_INVALID_ARGUMENTS;
    }

    status = VIP_ERROR_FAILURE;
    memory = &node->states_buffer;
    /* Generate preprocess command */
    switch (type) {
        case VIP_PREPROCESS_YUV2RGB:
            status = _preprocess_scaler(param, param_size,
                input_buffer, output_buffer, memory, &cmd_size);
            break;
        default:
            vip_error("not support this type update preprocess %d\n", type);
            break;
    }

    if (status == VIP_SUCCESS) {
        /* append command */
        node->command_size = cmd_size;
        nbglk_append_cmds(memory, &node->command_size, with_event, vip_false_e, 0);
    }
    else {
        vip_error("fail to update preprocess status=%d\n", status);
        vipGoOnError(status);
    }

    node->object.type = NBGLK_OBJ_NODE;
    node->type = NBGLK_NODE_TYPE_SCALER;
    node->inputs[0] = (vip_ptr)input_buffer;
    node->outputs[0] = (vip_ptr)output_buffer;

#if vpmdENABLE_VIDEO_MEMORY_CACHE
    vipOnError(viphal_flush_cache(node->states_buffer.mem_id, VIPDRV_CACHE_FLUSH));
#endif

onError:
    return status;
}
#endif
