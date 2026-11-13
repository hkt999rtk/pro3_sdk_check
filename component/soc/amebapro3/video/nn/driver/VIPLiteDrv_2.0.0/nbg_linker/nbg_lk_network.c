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
#include <vip_hal_hardware.h>
#include <nbg_lk_network.h>
#include <nbg_lk_debug.h>
#include <nbg_lk_cpu.h>
#include <nbg_fmt_util.h>
#include <nbg_lk_relocate.h>
#include <nbg_lk_coeff.h>
#include <nbg_lk_context.h>
#include <nbg_lk_capture.h>
#include <nbg_lk_npd.h>
#include <nbg_fmt_util.h>
#include <nbg_lk_nld.h>

#if vpmdENABLE_VIDEO_MEMORY_CACHE
static nbglk_videomemory_t* search_inout_video_memory(
    vip_network_t *network,
    vip_uint32_t physical
    )
{
    nbglk_videomemory_t *vm = VIP_NULL;
    vip_uint32_t i = 0;

    if ((physical > network->nnCmds.physical) &&
        (physical < (network->nnCmds.physical + network->nnCmds.size))) {
        vm = &network->nnCmds;
    }
    else if ((physical > network->tpCmds.physical) &&
             (physical < (network->tpCmds.physical + network->tpCmds.size))) {
        vm = &network->tpCmds;
    }
    else {
        for (i = 0; i < network->segment_num; i++) {
            nbglk_segment_t *segment = &network->segments[i];
            vip_uint32_t tmp_phy = SIZE_CAST(vip_uint32_t, segment->states_buffer.physical +
                segment->states_buffer.size);
            if (vip_false_e == segment->is_sw_segm) {
                if ((physical > segment->states_buffer.physical) && (physical < tmp_phy)) {
                    vm = &segment->states_buffer;
                    break;
                }
            }
        }
    }

    return vm;
}

static vip_status_e flush_inout_command_buffer(
    vip_network network,
    vip_uint32_t physical,
    vip_address_t logical
    )
{
    vip_status_e status = VIP_SUCCESS;
    nbglk_videomemory_t *memory = search_inout_video_memory(network, physical);

    if ((memory != VIP_NULL) && (memory->mem_id != 0)) {
        vipOnError(viphal_flush_cache(memory->mem_id, VIPDRV_CACHE_FLUSH));
    }

onError:
    return status;
}
#endif

#if vpmdENABLE_CHANGE_PPU_PARAM
static vip_status_e nbglk_change_ppu_shape(
    vip_network network,
    nbg_desc_input_output_t *io_entry,
    nbglk_io_patch_info_t *io_patch,
    vip_uint32_t index
    )
{
    vip_status_e status = VIP_SUCCESS;
    nbg_desc_input_output_t *ptr_io = VIP_NULL;
    vip_uint32_t *address = VIP_NULL;
    vip_uint32_t op_code = 0;
    vip_buffer buffer = io_patch->buffer;
    vip_bool_e skip_reshape = vip_true_e;
    vip_uint32_t i = 0;

    if (io_patch->slice_num > 1) {
        vip_error("not support change ppu shape, io slice_num=%d\n", io_patch->slice_num);
        vipGoOnError(VIP_ERROR_INVALID_ARGUMENTS);
    }

    if ((VIP_NULL == io_patch->logical_in_cmd[0]) && (VIP_NULL == io_patch->sw_op_buffer)) {
        vip_error("failed logical_in_cmd[0] is NULL and sw_op_buffer is NULL\n");
        vipGoOnError(VIP_ERROR_INVALID_ARGUMENTS);
    }

    ptr_io = &io_entry[index];
    if (ptr_io->dim_count != buffer->param.num_of_dims) {
        vip_error("fail to shape, index[%d], nbg dim_count=%d, bufer dim_count=%d, not matched\n",
                    index, ptr_io->dim_count, buffer->param.num_of_dims);
        vipGoOnError(VIP_ERROR_INVALID_ARGUMENTS);
    }

    for (i = 0; i < ptr_io->dim_count; i++) {
        if (ptr_io->dim_size[i] != buffer->param.sizes[i]) {
            skip_reshape = vip_false_e;
            break;
        }
    }
    if (skip_reshape) {
        vip_debug("skip change ppu shape.\n");
        return status;
    }

    address = (vip_uint32_t *)(VIPUINT64_TO_PTR(io_patch->logical_in_cmd[0] + io_patch->patch[0]->dest_offset));

    if ((VIP_BUFFER_TYPE_TENSOR == ptr_io->data_type) ||
        (VIP_BUFFER_TYPE_IMAGE == ptr_io->data_type)) {
        op_code = *(address - 1);
        if ((op_code & 0x0804d800) != 0x0804d800) {
            vip_error("not change ppu shape, may not ppu node, data_type=%d\n", ptr_io->data_type);
            vipGoOnError(VIP_ERROR_NOT_SUPPORTED);
        }
    }

    if (buffer->param.data_format != (vip_enum)ptr_io->data_format) {
        vip_error("buffer format=%d not equal to format=%d in nbg\n",
                   buffer->param.data_format, ptr_io->data_format);
        vipGoOnError(VIP_ERROR_NOT_SUPPORTED);
    }

    switch (ptr_io->data_type) {
    case VIP_BUFFER_TYPE_TENSOR:
    {
        vip_uint32_t stride[3] = {0, 0, 0};
        vip_uint32_t size[3] = {0, 0, 0};
        vip_uint32_t depth = 0;

        if (1 == buffer->param.num_of_dims) {
            size[0] = buffer->param.sizes[0];
        }
        else if (2 == buffer->param.num_of_dims) {
            size[0] = buffer->param.sizes[0];
            size[1] = buffer->param.sizes[1];
        }
        else if (3 == buffer->param.num_of_dims) {
            size[0] = buffer->param.sizes[0];
            size[1] = buffer->param.sizes[1];
            size[2] = buffer->param.sizes[2];
        }
        else if (buffer->param.num_of_dims > 3) {
            size[0] = buffer->param.sizes[0];
            size[1] = buffer->param.sizes[1];
            size[2] = buffer->param.sizes[2];
            if (buffer->param.sizes[3] > 1) {
                vip_error("buffer dim_num=%d, shape[3]=%d\n", buffer->param.num_of_dims,
                            buffer->param.sizes[3]);
                vipGoOnError(VIP_ERROR_NOT_SUPPORTED);
            }
        }

        if (buffer->param.num_of_dims >= 3) {
            depth = size[2];
        }
        else {
            depth = 1;
        }

        stride[0] = nbg_get_buf_format_byte(buffer->param.data_format);
        stride[1] = stride[0] * size[0];
        stride[2] = stride[1] * size[1];

        *(address + 1) = stride[1];
        *(address + 2) = size[0] | (size[1] << 16);

        /* height <= 1 sh image is 1D, other than sh imag is 2D */
        if (buffer->param.sizes[1] <= 1) {
            vip_uint32_t data = *(address + 3);
            *(address + 3) =  data & (~(1 << 12));
        }
        else {
            vip_uint32_t data = *(address + 3);
            *(address + 3) =  data | (1 << 12);
        }

        *(address + 6) = stride[2];
        *(address + 7) = depth;
    }
    break;

    case VIP_BUFFER_TYPE_IMAGE:
    {
        vip_uint32_t stride[3] = {0, 0, 0};
        vip_uint32_t size[3] = {0, 0, 0};

        if (1 == buffer->param.num_of_dims) {
            size[0] = buffer->param.sizes[0];
        }
        else if (2 == buffer->param.num_of_dims) {
            size[0] = buffer->param.sizes[0];
            size[1] = buffer->param.sizes[1];
        }
        else if (3 == buffer->param.num_of_dims) {
            size[0] = buffer->param.sizes[0];
            size[1] = buffer->param.sizes[1];
            size[2] = buffer->param.sizes[2];
        }
        else if (buffer->param.num_of_dims > 3) {
            size[0] = buffer->param.sizes[0];
            size[1] = buffer->param.sizes[1];
            size[2] = buffer->param.sizes[2];
            if (buffer->param.sizes[3] > 1) {
                vip_error("buffer dim_num=%d, shape[3]=%d\n", buffer->param.num_of_dims,
                            buffer->param.sizes[3]);
                vipGoOnError(VIP_ERROR_NOT_SUPPORTED);
            }
        }

        stride[0] = nbg_get_buf_format_byte(buffer->param.data_format);
        stride[1] = stride[0] * size[0];
        stride[2] = stride[1] * size[1];

        *(address + 1) = stride[1];
        *(address + 2) = size[0] | (size[1] << 16);

        /* height <= 1 sh image is 1D, other than sh imag is 2D */
        if (buffer->param.sizes[1] <= 1) {
            vip_uint32_t data = *(address + 3);
            *(address + 3) =  data & (~(1 << 12));
        }
        else {
            vip_uint32_t data = *(address + 3);
            *(address + 3) =  data | (1 << 12);
        }

        *(address + 6) = stride[2];
        *(address + 7) = size[2];
    }
    break;

    case VIP_BUFFER_TYPE_SCALAR:
    break;

    case VIP_BUFFER_TYPE_ARRAY:
    {
        if (ptr_io->dim_size[0] == *(address - 2)) {
            *(address - 2) = buffer->param.sizes[0];
        }
        else {
            *(address - 1) = buffer->param.sizes[0];
        }

        if (buffer->param.sizes[1] > 1) {
            vip_error("change ppu shape, array shape[%d, %d]\n",
                        buffer->param.sizes[0], buffer->param.sizes[1]);
        }
    }
    break;

    default:
        vip_error("not support this data type=%d\n", ptr_io->data_type);
        break;
    }

#if vpmdENABLE_CHECKSUM
    /* change the load states command, need calculate the checksum again */
    network->checksum[NBGLK_CHECKSUM_STATE_LOAD] = 0;
    nbglk_network_checksum(network, NBGLK_CHECKSUM_STATE_LOAD);
#endif

onError:
    return status;
}
#endif

vip_status_e nbglk_patch_network_inputs(
    vip_network_t *network
    )
{
    vip_status_e status = VIP_SUCCESS;
    vip_uint32_t i = 0, j = 0;

    if (network->input_dirty == 0) {
        vip_error("input dirty is %d\n", network->input_dirty);
        vipGoOnError(VIP_ERROR_NOT_SUPPORTED);
    }

    for (i = 0; i < network->nbg_desc.n_inputs; i++) {
        nbglk_io_patch_info_t *input = &network->network_inputs[i];

        if (0 == input->physical) {
            vip_error("error, didn't set input %d\n", i);
            vipGoOnError(VIP_ERROR_NOT_SUPPORTED);
        }

        /* change ppu kernel input shape */
        #if vpmdENABLE_CHANGE_PPU_PARAM
        if (network->change_ppu_param) {
            status = nbglk_change_ppu_shape(network, network->nbg_desc.inputs, input, i);
            if (status != VIP_SUCCESS) {
                vip_error("fail to change input[%d] shape\n", i);
            }
        }
        #endif

        for (j = 0; j < input->slice_num; j++) {
            if (input->logical_in_cmd[j] != VIP_NULL) {
                vip_address_t new_addr = 0;
                nbg_physical_value_t* new_physical = (nbg_physical_value_t*)&new_addr;
                vip_uint32_t *address = (vip_uint32_t *)(VIPUINT64_TO_PTR(input->logical_in_cmd[j] +
                    input->patch[j]->dest_offset));
                vip_uint32_t *address_h = (vip_uint32_t *)(VIPUINT64_TO_PTR(input->logical_in_cmd[j] +
                    input->patch[j]->dest_offset_h));
                new_addr = input->physical + input->patch[j]->src_offset;
                if (NBG_PATCH_TRANSFORMATION_RIGHT_SHIFT_6 == input->patch[j]->transformed) {
                    *address = (*address & ~WORD_BIT0_TO_BIT25_MASK) | (new_physical->low >> 6);
                }
                else {
                    *address = new_physical->low;
                }
                if (new_physical->high) {
                    *address_h &= ~input->patch[j]->mask_h;
                    *address_h |= (new_physical->high << get_start_bit(input->patch[j]->mask_h)) &
                        input->patch[j]->mask_h;
                }
            }
            else if(VIP_NULL == input->sw_op_buffer) {
                vip_error("input logical in cmd is NULL, input %d, slice=%d\n", i, j);
                vipGoOnError(VIP_ERROR_IO);
            }
            else {
                continue;
            }

            #if vpmdENABLE_VIDEO_MEMORY_CACHE
            status = flush_inout_command_buffer(network, input->physical_in_cmd[j], input->logical_in_cmd[j]);
            if (status != VIP_SUCCESS) {
                vip_error("fail to flush input%d_%d cmds cache\n", i, j);
            }
            #endif
        }

        if (input->sw_op_buffer != VIP_NULL) {
            vip_info("is CPU layer input %d\n", i);
            if (VIP_NULL == input->logical) {
                input->logical = vip_map_buffer(input->buffer);
                if (VIP_NULL == input->logical) {
                    vip_error("fail to map user logical for input of sw layer\n");
                    vipOnError(VIP_ERROR_FAILURE);
                }
            }
            *input->sw_op_buffer = input->logical;
        }
    }

#if !vpmdENABLE_CAPTURE
    network->input_dirty = 0;
#endif

#if vpmdDUMP_NBG_RESOURCE
    nbglk_dump_network_inputs(network);
#endif

onError:
    return status;
}

vip_status_e nbglk_patch_network_outputs(
    vip_network_t *network
    )
{
    vip_status_e status = VIP_SUCCESS;
    vip_uint32_t  i = 0, j = 0;

    if (network->output_dirty == 0) {
        vip_error("output dirty is %d\n", network->output_dirty);
        vipGoOnError(VIP_ERROR_NOT_SUPPORTED);
    }

    for (i = 0; i < network->nbg_desc.n_outputs; i++) {
        nbglk_io_patch_info_t *output = &network->network_outputs[i];

        if (0 == output->physical) {
            vip_error("error, didn't set output %d\n", i);
            vipGoOnError(VIP_ERROR_NOT_SUPPORTED);
        }

        /* change ppu kernel output shape */
        #if vpmdENABLE_CHANGE_PPU_PARAM
        if (network->change_ppu_param) {
            status = nbglk_change_ppu_shape(network, network->nbg_desc.outputs, output, i);
            if (status != VIP_SUCCESS) {
                vip_error("fail to change output[%d] shape\n", i);
            }
        }
        #endif

        for (j = 0; j < output->slice_num; j++) {
            if (output->logical_in_cmd[j] != VIP_NULL) {
                vip_address_t new_addr = 0;
                nbg_physical_value_t* new_physical = (nbg_physical_value_t*)&new_addr;
                vip_uint32_t* address = (vip_uint32_t*)(VIPUINT64_TO_PTR(output->logical_in_cmd[j] +
                    output->patch[j]->dest_offset));
                vip_uint32_t* address_h = (vip_uint32_t*)(VIPUINT64_TO_PTR(output->logical_in_cmd[j] +
                    output->patch[j]->dest_offset_h));
                new_addr = output->physical + output->patch[j]->src_offset;
                if (NBG_PATCH_TRANSFORMATION_RIGHT_SHIFT_6 == output->patch[j]->transformed) {
                    *address = (*address & ~WORD_BIT0_TO_BIT25_MASK) | (new_physical->low >> 6);
                }
                else {
                    *address = new_physical->low;
                }
                if (new_physical->high) {
                    *address_h &= ~output->patch[j]->mask_h;
                    *address_h |= (new_physical->high << get_start_bit(output->patch[j]->mask_h)) &
                        output->patch[j]->mask_h;
                }
            }
            else if (VIP_NULL == output->sw_op_buffer) {
                vip_error("output logical in cmd is NULL, output %d, slice=%d\n", i, j);
                vipGoOnError(VIP_ERROR_IO);
            }
            else {
                continue;
            }

            #if vpmdENABLE_VIDEO_MEMORY_CACHE
            status = flush_inout_command_buffer(network, output->physical_in_cmd[j], output->logical_in_cmd[j]);
            if (status != VIP_SUCCESS) {
                vip_error("fail to flush output%d_%d cmds cache\n", i, j);
            }
            #endif
        }

        if (output->sw_op_buffer != VIP_NULL) {
            vip_info("is CPU layer output %d\n", i);
            if (VIP_NULL == output->logical) {
                output->logical = vip_map_buffer(output->buffer);
                if (VIP_NULL == output->logical) {
                    vip_error("fail to map user logical for output of sw layer\n");
                    vipOnError(VIP_ERROR_FAILURE);
                }
            }
            *output->sw_op_buffer = output->logical;
        }
    }
    network->output_dirty = 0;

onError:
    return status;
}

static vip_status_e nbglk_update_network_inout(
    vip_network_t *network
    )
{
    vip_status_e status = VIP_SUCCESS;
    vip_uint32_t i = 0;

    if (network->input_dirty != 0) {
       vipOnError(nbglk_patch_network_inputs(network));
    }
    else {
        /* check inputs is present */
        for (i = 0; i < network->nbg_desc.n_inputs; i++) {
            nbglk_io_patch_info_t *input = &network->network_inputs[i];
            if (input->buffer != VIP_NULL) {
                if (0 == input->buffer->memory.size) {
                    vip_error("network input%d size is 0\n", i);
                    vipGoOnError(VIP_ERROR_NETWORK_INCOMPATIBLE);
                }
            }
            else {
                vip_error("network input%d is NULL\n", i);
                vipGoOnError(VIP_ERROR_NETWORK_INCOMPATIBLE);
            }
        }
    }

    if (network->output_dirty != 0) {
        vipOnError(nbglk_patch_network_outputs(network));
    }
    else {
        /* check inputs is present */
        for (i = 0; i < network->nbg_desc.n_outputs; i++) {
            nbglk_io_patch_info_t *output = &network->network_outputs[i];
            if (output->buffer != VIP_NULL) {
                if (0 == output->buffer->memory.size) {
                    vip_error("network output%d size is 0\n", i);
                    vipGoOnError(VIP_ERROR_NETWORK_INCOMPATIBLE);
                }
            }
            else {
                vip_error("network output%d is NULL\n", i);
                vipGoOnError(VIP_ERROR_NETWORK_INCOMPATIBLE);
            }
        }
    }

    return status;
onError:
    vip_error("fail to update and check network input output status=%d\n", status);
    return status;
}

/*
@brief Get network input/output parameters.
       This function is used for NBG format version greater than 0x0001000A.
@param io_table The input/output entry table.
@param property The property of query.
@param [OUT] value The returns value.
*/
static vip_status_e nbglk_get_io_param(
    vip_network_t *network,
    void *io_table,
    vip_enum property,
    void *value
    )
{
    vip_status_e status = VIP_SUCCESS;
    nbg_desc_input_output_t *table = (nbg_desc_input_output_t*)io_table;
    vip_uint32_t *ptr_u32 = (vip_uint32_t *)value;
    vip_int32_t *ptr_32 = (vip_int32_t *)value;
    vip_float_t *ptr_fl = (vip_float_t *)value;
    vip_char_t *ptr_char = (vip_char_t *)value;

    switch (property) {
    case VIP_BUFFER_PROP_QUANT_FORMAT:
        if (table->quant_format >= VIP_BUFFER_QUANTIZE_MAX) {
            vip_error("fail to query quantize format=%d\n", table->quant_format);
            status = VIP_ERROR_FAILURE;
        }
        else {
            *ptr_u32 = table->quant_format;
        }
        break;

    case VIP_BUFFER_PROP_NUM_OF_DIMENSION:
        if (table->dim_count > MAX_NUM_DIMS) {
            vip_error("fail to query dim count=%d\n", table->dim_count);
            status = VIP_ERROR_FAILURE;
        }
        else {
            *ptr_u32 = table->dim_count;
        }
        break;

    case VIP_BUFFER_PROP_SIZES_OF_DIMENSION:
        {
        vip_uint32_t i = 0;
        vip_uint32_t count = (table->dim_count > MAX_NUM_DIMS) ? MAX_NUM_DIMS : table->dim_count;
        for (i = 0; i < count; i++) {
            ptr_u32[i] = SIZE_CAST(vip_uint32_t, table->dim_size[i]);
        }
        }
        break;

    case VIP_BUFFER_PROP_DATA_FORMAT:
        *ptr_u32 = table->data_format;
        break;

    case VIP_BUFFER_PROP_FIXED_POINT_POS:
        *ptr_32 = table->quant_data.dfp.fixed_point_pos;
        break;

    case VIP_BUFFER_PROP_TF_SCALE:
        *ptr_fl = table->quant_data.affine.tf_scale;
        break;

    case VIP_BUFFER_PROP_TF_ZERO_POINT:
        *ptr_32 = table->quant_data.affine.tf_zero_point;
        break;

    case VIP_BUFFER_PROP_NAME:
        if (network->nbg_desc.fixed.header.version >= 0x00010004) {
            viphal_os_strcpy(ptr_char, table->name);
        }
        else {
            viphal_os_strcpy(ptr_char, "dumy_input_name");
        }
        break;

    default:
        vip_error("viplite doesn't support this property=%d\n", property);
        status = VIP_ERROR_INVALID_ARGUMENTS;
        break;
    }

    return status;
}

static vip_status_e nbglk_network_segment_submit(
    vip_network network,
    nbglk_segment_t *segment
    )
{
    vip_status_e status = VIP_SUCCESS;
    viphal_task_param_t param = { 0 };

    param.task_id = network->task_id;
    param.subtask_count = 1;
    param.subtask_index = segment->index;
    param.device_index = network->device->dev_index;
    param.core_index = network->core_index;
    param.core_cnt = network->nbg_desc.net_info.core_count;
    param.time_out = network->wait_timeout;
    param.priority = network->priority;
    param.need_schedule = vip_true_e == network->need_schedule ? 1 : 0;

    status = viphal_submit_task(&param);

    return status;
}

static vip_status_e nbglk_network_segment_wait(
    vip_network network,
    nbglk_segment_t *segment
    )
{
    vip_status_e status = VIP_SUCCESS;

    if (VIP_NULL == segment) {
        vip_error("fail to wait network, parameter is NULL\n");
        return VIP_ERROR_INVALID_ARGUMENTS;
    }

    /* Wait for commands to be done. */
    status = viphal_wait_task(network->task_id, segment->index);
    if ((VIP_ERROR_TIMEOUT == status) || (VIP_ERROR_RECOVERY == status))  {
        #if vpmdENABLE_CNN_PROFILING
        nbglk_layer_info(network, segment->name, segment->segm_start_id, segment->segm_end_id,
                         segment->uid, segment->core_id_mask);
        #endif
        #if vpmdENABLE_CHECKSUM
        nbglk_network_checksum(network, NBGLK_CHECKSUM_MAX);
        #endif
        nbglk_section_checksum(network);
        #if vpmdENABLE_HANG_DUMP > 1
        if (network->prepared_network) {
            network->occur_hang = vip_true_e;
            vip_error("network=%p hang capture....\n", network);
            nbglk_capture_hang_dump(network);
            if (VIP_ERROR_RECOVERY == status) {
                network->capture_done = 0;
            }
        }
        #endif
        vip_error("wait network=%p, name=%s timeout, cmd size=0x%x, phy=0x%llx, status=%d\n",
                  network, network->nbg_desc.net_info.name, segment->states_buffer.size,
                  segment->states_buffer.physical, status);
    }
    return status;
}

static vip_uint32_t get_sw_operation_count(
    vip_network_t *network
)
{
    vip_uint32_t count = 0, i = 0;
    nbg_desc_operation_t *operation = VIP_NULL;

    for (i = 0; i < network->nbg_desc.n_operations; i++) {
        operation = &network->nbg_desc.operations[i];
        if (operation->type == NBG_OPERATION_TYPE_SW) {
            count++;
        }
    }

    return count;
}

static vip_uint32_t get_insertlink_states_size(
    vip_network_t *network,
    nbglk_segment_t *segment,
    vip_uint32_t size
    )
{
    vip_status_e status = VIP_SUCCESS;
    vip_uint32_t states_size = CACHE_CMD_SIZE;
    vip_uint32_t tmp_size = 0;
    nbg_desc_operation_t *op =VIP_NULL;
    vip_uint32_t i = 0, npd_states_offset = 0;
    vip_uint32_t split_count = (size + vipdMAX_CMD_SIZE - 1) / vipdMAX_CMD_SIZE;
    vip_uint32_t total_size = 0;
    vip_uint32_t *split_cmd_size = VIP_NULL;
    vip_uint32_t mem_size = split_count * sizeof(vip_uint32_t);

    if (0 == split_count) {
        vip_error("insert link max is 0\n");
        return size;
    }

    vipOnError(viphal_allocate_memory(mem_size, (void **)&segment->operation_pos));
    viphal_zero_memory((void *)segment->operation_pos, mem_size);
    vipOnError(viphal_allocate_memory(mem_size, (void **)&segment->split_pos_table));
    viphal_zero_memory((void *)segment->split_pos_table, mem_size);
    vipOnError(viphal_allocate_memory(mem_size, (void **)&segment->split_size_table));
    viphal_zero_memory((void *)segment->split_size_table, mem_size);

    if ((VIP_NULL == segment->operation_pos) || (VIP_NULL == segment->split_pos_table) ||
        (VIP_NULL == segment->split_size_table)) {
        vip_error("fail to malloc operation_pos or patch pos, split size\n");
        vipGoOnError(VIP_ERROR_FAILURE);
    }
    if (network->npd.enable_npd) {
        ONE_OP_PROFILE_CMD_SIZE(network->device->core_count, npd_states_offset)
    }

    split_cmd_size = segment->split_size_table;
    segment->split_num = 0;

    /* Calculate NB state size. */
    for (i = 0; i < network->nbg_desc.n_operations; i++) {
        /* the size of states added for one operation */
        vip_uint32_t add_state_size = 0;
        vip_uint32_t start_size = states_size;

        tmp_size = 0;

        op = &network->nbg_desc.operations[i];

        #if vpmdENABLE_CNN_PROFILING
        if (network->context->func_config.enable_cnn_profile == vip_true_e) {
            if ((i == segment->segm_end_id) &&
                (op->type != NBG_OPERATION_TYPE_INIT) &&  (op->type != NBG_OPERATION_TYPE_END)) {
              states_size += network->nbg_desc.load_states_table[op->state_id].size;
            }
        }
        else {
            if ((op->type != NBG_OPERATION_TYPE_INIT) &&
                (i < segment->segm_end_id) && (i >= segment->segm_start_id)) {
                tmp_size = network->nbg_desc.load_states_table[op->state_id].size;
                #if vpmdENABLE_CANCELATION > 1
                if (i < (network->nbg_desc.n_operations - 1)) {
                    if (nbg_operation_is_nn_engine(op->type) || (NBG_OPERATION_TYPE_TP == op->type) ||
                        (NBG_OPERATION_TYPE_SH == op->type)) {
                        nbg_desc_operation_t *next_op = VIP_NULL;
                        next_op = &network->nbg_desc.operations[i + 1];
                        if (op->layer_id != next_op->layer_id) {
                            tmp_size += BREAKPOINT_SIZE;
                        }
                    }
                }
                #endif
                #if vpmdENABLE_STALL_FE
                if (nbg_operation_is_nn_engine(op->type) || (op->type == NBG_OPERATION_TYPE_SH)) {
                    tmp_size += STALL_FE_SIZE;
                }
                #endif

                states_size += tmp_size;
            }
        }
        #else
        if ((op->type != NBG_OPERATION_TYPE_INIT) &&
            (i < segment->segm_end_id) && (i >= segment->segm_start_id)) {
            tmp_size = SIZE_CAST(vip_uint32_t, network->nbg_desc.load_states_table[op->state_id].size);
            #if vpmdENABLE_CANCELATION > 1
            if (i < (network->nbg_desc.n_operations - 1)) {
                if (nbg_operation_is_nn_engine(op->type) || (NBG_OPERATION_TYPE_TP == op->type) ||
                    (NBG_OPERATION_TYPE_SH == op->type)) {
                    nbg_desc_operation_t *next_op = VIP_NULL;
                    next_op = &network->nbg_desc.operations[i + 1];
                    if (op->layer_id != next_op->layer_id) {
                        tmp_size += BREAKPOINT_SIZE;
                    }
                }
            }
            #endif
            #if vpmdENABLE_STALL_FE
            if (nbg_operation_is_nn_engine(op->type) || (op->type == NBG_OPERATION_TYPE_SH)) {
                tmp_size += STALL_FE_SIZE;
            }
            #endif

            states_size += tmp_size;
        }
        #endif

        #if vpmdENABLE_LAYER_DUMP
        if (network->nld.status) {
            states_size += LINK_SIZE;
        }
        #endif

        if ((op->type != NBG_OPERATION_TYPE_INIT) &&
                (i < segment->segm_end_id) && (i >= segment->segm_start_id)) {
            if (nbg_operation_is_nn_engine(op->type)) {
                if (network->npd.enable_npd) {
                    states_size += npd_states_offset;
                }
            }
        }

        add_state_size = states_size - start_size;
        /* this is for more than command buffer size, we should split it and link them together */
        if (states_size + APPEND_COMMAND_SIZE > vipdMAX_CMD_SIZE) {
            segment->operation_pos[segment->split_num] = i - 1;
            /* insert LINK_SIZE bytes in command buffer for LINK command */
            states_size = states_size - add_state_size + LINK_SIZE;
            if (states_size >= vipdMAX_CMD_SIZE) {
                vip_error("fail to insert LINK command for big command buffer,"
                            "split command size=0x%x, max support size=0x%x\n",
                             states_size, vipdMAX_CMD_SIZE);
                vipGoOnError(VIP_ERROR_FAILURE);
            }
            split_cmd_size[segment->split_num] = states_size;
            segment->split_num++;
            i = i - 1;
            total_size += states_size;
            states_size = 0;
        }
    }

    split_cmd_size[segment->split_num] = states_size;
    total_size += states_size;
    vip_info("large command buffer split num=%d\n", segment->split_num);

    return total_size;

onError:
    vip_error("fail to insert link states size..\n");
    if (segment->operation_pos != VIP_NULL) {
        viphal_free_memory((void*)segment->operation_pos);
        segment->operation_pos = VIP_NULL;
    }
    if (segment->split_pos_table != VIP_NULL) {
        viphal_free_memory(segment->split_pos_table);
        segment->split_pos_table = VIP_NULL;
    }
    if (segment->split_size_table != VIP_NULL) {
        viphal_free_memory(segment->split_size_table);
       segment->split_size_table = VIP_NULL;
    }
    return 0;
}

static vip_uint32_t get_sh_command_size(
    vip_network_t *network,
    nbglk_segment_t *segment
    )
{
    vip_uint32_t cmd_size = 0;
    nbg_desc_operation_t *op = VIP_NULL;
    nbg_desc_sh_operation_t *shop = VIP_NULL;
    vip_uint32_t i = 0;

    for (i = 0; i < network->nbg_desc.n_operations; i++) {
        op = &network->nbg_desc.operations[i];
        #if vpmdENABLE_CNN_PROFILING
        if (network->context->func_config.enable_cnn_profile == vip_true_e) {
            if (i != segment->segm_end_id) {
                continue;
            }
        }
        else
        #endif
        {
            if ((i >= segment->segm_end_id) || (i < segment->segm_start_id)) {
                continue;
            }
        }

        if (op->type == NBG_OPERATION_TYPE_SH) {
            shop = &network->nbg_desc.sh_ops[op->index];
            cmd_size += VIP_ALIGN(network->nbg_desc.ppu_instr_table[shop->instr_index].size,
                                  vipdSH_ALIGN_SIZE);
        }
    }

    return cmd_size;
}

static vip_uint32_t get_sh_cmds_status(
    vip_network_t *network
    )
{
    vip_status_e status = VIP_SUCCESS;

    if (network->nbg_desc.fixed.header.version < 0x00010005) {
        network->realloc_sh_buff = vip_true_e;
    }
    else if (network->nbg_desc.fixed.header.version >= 0x00010018) {
        /* ppu instr is stored in NBG file separately after 0x00010018 */
        network->realloc_sh_buff = vip_false_e;
    }
    else {
        vip_uint32_t i = 0;
        vip_address_t physical = 0;
        nbg_desc_operation_t* operation = VIP_NULL;
        nbg_desc_sh_operation_t* op_sh = VIP_NULL;
        network->realloc_sh_buff = vip_false_e;
        for (i = 0; i < network->nbg_desc.n_operations; i++) {
            operation = &network->nbg_desc.operations[i];
            if (NBG_OPERATION_TYPE_SH == operation->type) {
                op_sh = &network->nbg_desc.sh_ops[operation->index];
                physical = network->ppu_instr.physical +
                    SIZE_CAST(vip_uint32_t, network->nbg_desc.ppu_instr_table[op_sh->instr_index].offset);
                if ((physical % vipdSH_ALIGN_SIZE) != 0) {
                    network->realloc_sh_buff = vip_true_e;
                    return status;
                }
            }
        }
    }

    return status;
}

/* Get the command states size. */
static vip_uint32_t nbglk_network_states_size(
    vip_network_t *network,
    nbglk_segment_t *segment
    )
{
    vip_uint32_t states_size = CACHE_CMD_SIZE;
    nbg_desc_operation_t *op = VIP_NULL;
    vip_uint32_t i = 0;

    for (i = 0; i < network->nbg_desc.n_operations; i++) {
        op = &network->nbg_desc.operations[i];
        #if vpmdENABLE_CNN_PROFILING
        if (network->context->func_config.enable_cnn_profile == vip_true_e) {
            if ((i == segment->segm_end_id) &&  (op->type != NBG_OPERATION_TYPE_INIT) &&
                (op->type != NBG_OPERATION_TYPE_END)) {
                states_size += network->nbg_desc.load_states_table[op->state_id].size;
            }
        }
        else
        #endif
        {
            /* init command doesn't in command buffer */
            if ((op->type != NBG_OPERATION_TYPE_INIT) &&
                (i < segment->segm_end_id) && (i >= segment->segm_start_id)) {
                states_size += SIZE_CAST(vip_uint32_t, network->nbg_desc.load_states_table[op->state_id].size);
            }
        }

        #if vpmdENABLE_QUICK_RESET
        if (nbg_operation_is_nn_engine(op->type)) {
             states_size += QUICK_RESET_SIZE;
        }
        #endif

        #if vpmdENABLE_STALL_FE
        if (nbg_operation_is_nn_engine(op->type) || (NBG_OPERATION_TYPE_SH == op->type)) {
             states_size += STALL_FE_SIZE;
        }
        #endif
    }

    vip_debug("get original states size=0x%x\n", states_size);
    return states_size;
}

static vip_uint32_t nbglk_network_cmdbuf_size(
    vip_network_t *network,
    nbglk_segment_t *segment
    )
{
    vip_uint32_t cmdbuf_size = 0;
    cmdbuf_size = nbglk_network_states_size(network, segment);
#if vpmdENABLE_CANCELATION > 1
    cmdbuf_size += nbglk_breakpoint_get_count(network, segment) * BREAKPOINT_SIZE;
#endif
    if (vip_true_e == network->npd.enable_npd) {
        cmdbuf_size += nbglk_npd_states_size(network, segment);
    }
#if vpmdENABLE_LAYER_DUMP
    if (network->nld.status) {
        cmdbuf_size += nbglk_nld_states_size(network, segment);
    }
#endif
    if (cmdbuf_size + APPEND_COMMAND_SIZE > vipdMAX_CMD_SIZE) {
        cmdbuf_size = get_insertlink_states_size(network, segment, cmdbuf_size);
    }
    vip_info("segment_%d calculate cmdbuf_size=0x%x\n", segment->index, cmdbuf_size);

    return cmdbuf_size;
}

#if vpmdENABLE_FLUSH_CPU_CACHE
static vip_status_e flush_network_cache(
    vip_network_t *network
    )
{
    vip_status_e status = VIP_SUCCESS;
#if vpmdENABLE_VIDEO_MEMORY_CACHE
    vip_uint32_t i = 0;
#endif
    if (VIP_NULL == network) {
        vipGoOnError(VIP_ERROR_INVALID_ARGUMENTS);
    }

#if vpmdENABLE_FLUSH_CPU_CACHE
    if (network->create_type & VIP_CREATE_NETWORK_FROM_FLASH) {
        if ((network->flash.mem_id != 0) && (NBGLK_DUP_NONE == network->dup_type)) {
            vipOnError(viphal_flush_cache(network->flash.mem_id, VIPDRV_CACHE_FLUSH));
        }
    }
#endif

#if vpmdENABLE_VIDEO_MEMORY_CACHE
    if (!(network->create_type & VIP_CREATE_NETWORK_FROM_FLASH)) {
        if ((network->coeff_data.mem_id != 0) && (NBGLK_DUP_NONE == network->dup_type)) {
            vipOnError(viphal_flush_cache(network->coeff_data.mem_id, VIPDRV_CACHE_FLUSH));
        }
    }
    if (network->nbg_desc.fixed.header.version >= 0x0001001C) {
        if (0 != network->coeff_header.mem_id) {
            vipOnError(viphal_flush_cache(network->coeff_header.mem_id, VIPDRV_CACHE_FLUSH));
        }
    }
    if (network->nbg_desc.fixed.header.version >= 0x00010018) {
        if (network->ppu_instr.size > 0) {
            vipOnError(viphal_flush_cache(network->ppu_instr.mem_id, VIPDRV_CACHE_FLUSH));
        }
        if (network->dyna_io.size > 0) {
            vipOnError(viphal_flush_cache(network->dyna_io.mem_id, VIPDRV_CACHE_FLUSH));
        }
    }

    if (network->nnCmds.size > 0) {
        vipOnError(viphal_flush_cache(network->nnCmds.mem_id, VIPDRV_CACHE_FLUSH));
    }
    if (network->tpCmds.size > 0) {
        vipOnError(viphal_flush_cache(network->tpCmds.mem_id, VIPDRV_CACHE_FLUSH));
    }
    for (i = 0; i < network->segment_num; i++) {
        nbglk_segment_t *segment = &network->segments[i];
        if (vip_false_e == segment->is_sw_segm) {
            if (segment->states_buffer.size > 0) {
                vipOnError(viphal_flush_cache(segment->states_buffer.mem_id, VIPDRV_CACHE_FLUSH));
            }
            if (vip_true_e == network->realloc_sh_buff && segment->sh_cmd_buffer.size > 0) {
                vipOnError(viphal_flush_cache(segment->sh_cmd_buffer.mem_id, VIPDRV_CACHE_FLUSH));
            }
        }
    }
    vip_info("flush network video memory cache\n");
#endif

onError:
    return status;
}
#endif

#if vpmdENABLE_APP_PROFILING
static vip_status_e nbglk_get_profiling(
    vip_network_t *network,
    void *data
    )
{
    vip_status_e status = VIP_SUCCESS;
    vip_uint32_t i = 0;
    vip_uint32_t total_time = 0;
    vip_uint32_t total_cycle = 0;
    vip_uint32_t total_idle_cycle = 0;
    vipdrv_query_profiling_t profile;
    nbglk_segment_t *segment = VIP_NULL;
    vip_inference_profile_t *tmp = (vip_inference_profile_t*)data;
    viphal_os_zero_memory(&profile, sizeof(vipdrv_query_profiling_t));

    for (i = 0; i < network->segment_num; i++) {
        segment = &network->segments[i];
        if (vip_false_e == segment->is_sw_segm) {
            profile.task_id = network->task_id;
            profile.subtask_index = segment->index;
            status = viphal_query_profiling(&profile);
            if (VIP_SUCCESS == status) {
                total_time += profile.inference_time;
                total_cycle += profile.total_cycle[0];
                total_idle_cycle += profile.total_idle_cycle[0];
            }
        }
    }
    tmp->inference_time = total_time;
    tmp->total_cycle = total_cycle - total_idle_cycle;
    vip_debug("profile inference_time=%dus, total_cycle=%d, idle_cycle=%d\n",
               total_time, total_cycle, total_idle_cycle);

    return VIP_SUCCESS;
}
#endif

/*
@brief Check network input or output parameters
@param network The network object create by vip_create_network().
@param index The index of this output.
@param buffer The viplite buffer object
@param flag 0 is for input buffer. 1 is output buffer.
*/
static vip_int8_t nbglk_check_io_params(
    vip_network_t *network,
    vip_uint32_t index,
    vip_buffer buffer,
    vip_int8_t flag
    )
{
    vip_status_e status = VIP_SUCCESS;
    vip_int8_t matched = vip_true_e;
    vip_int32_t quant_format = 0;
    vip_uint32_t dims[MAX_NUM_DIMS];
    vip_uint32_t num_dim;
    vip_int32_t zero_point = 0;
    vip_float_t scale = 0.0;
    vip_int32_t data_format = 0;
    vip_int32_t fixed_point_pos = 0;
    vip_uint32_t i = 0;
    vip_uint32_t total_size = 0;
    nbglk_io_patch_info_t *entry = VIP_NULL;

    if (0 == flag) {
        nbglk_query_input(network, index, VIP_BUFFER_PROP_QUANT_FORMAT, &quant_format);
        nbglk_query_input(network, index, VIP_BUFFER_PROP_SIZES_OF_DIMENSION, dims);
        nbglk_query_input(network, index, VIP_BUFFER_PROP_NUM_OF_DIMENSION, &num_dim);
        nbglk_query_input(network, index, VIP_BUFFER_PROP_TF_ZERO_POINT, &zero_point);
        nbglk_query_input(network, index, VIP_BUFFER_PROP_TF_SCALE, &scale);
        nbglk_query_input(network, index, VIP_BUFFER_PROP_DATA_FORMAT, &data_format);
        nbglk_query_input(network, index, VIP_BUFFER_PROP_FIXED_POINT_POS, &fixed_point_pos);
        entry = &network->network_inputs[index];
    }
    else {
        nbglk_query_output(network, index, VIP_BUFFER_PROP_QUANT_FORMAT, &quant_format);
        nbglk_query_output(network, index, VIP_BUFFER_PROP_SIZES_OF_DIMENSION, dims);
        nbglk_query_output(network, index, VIP_BUFFER_PROP_NUM_OF_DIMENSION, &num_dim);
        nbglk_query_output(network, index, VIP_BUFFER_PROP_TF_ZERO_POINT, &zero_point);
        nbglk_query_output(network, index, VIP_BUFFER_PROP_TF_SCALE, &scale);
        nbglk_query_output(network, index, VIP_BUFFER_PROP_DATA_FORMAT, &data_format);
        nbglk_query_output(network, index, VIP_BUFFER_PROP_FIXED_POINT_POS, &fixed_point_pos);
        entry = &network->network_outputs[index];
    }

    total_size = nbg_get_buf_format_bit(data_format);
    for (i = 0; i < num_dim; i++) {
        total_size *= dims[i];
        if (0 == i) {
            /* round up */
            total_size = (total_size + BITS_PER_BYTE - 1) / BITS_PER_BYTE;
        }
    }

    /* check shape if the network doesn't supports change ppu param */
#if vpmdENABLE_CHANGE_PPU_PARAM
    if (vip_false_e == network->change_ppu_param) {
#endif
        if (total_size > buffer->memory.size) {
            vip_error("fail to set %s[%d], nbg request buffer size=%d, set buffer size=%d",
                     (flag == 0) ? "input" : "output", index, total_size, buffer->memory.size);
            matched = vip_false_e;
        }
        else if (total_size + 64 > buffer->memory.size) {
            if ((entry->patch_belong & NBGLK_PATCH_BELONG_SH) && \
                (NBGLK_BUFFER_ALLOCATE_FROM_USER_MEMORY == buffer->allocated || \
                NBGLK_BUFFER_ALLOCATE_FROM_USER_DMA_BUF == buffer->allocated || \
                NBGLK_BUFFER_ALLOCATE_FROM_USER_PHYSICAL == buffer->allocated)) {
                    vip_info("the buffer for SH access size=%d\n", buffer->memory.size);
            }
        }
#if vpmdENABLE_CHANGE_PPU_PARAM
    }
#endif

    if (vip_false_e == matched) {
        vip_print("nbg request buffer dims: ");
        for (i = 0; i < num_dim; i++) {
            vip_print("%d ", dims[i]);
        }
        vip_print(", vip buffer dims: ");
        for (i = 0; i < num_dim; i++) {
            vip_print("%d ", buffer->param.sizes[i]);
        }
        vip_print("\n");
        status = VIP_ERROR_INVALID_ARGUMENTS;
        vipGoOnError(status);
    }

    /* support recurrent network, only check buffer size, buffer param is a warning log */
    if (quant_format != (vip_int32_t)buffer->param.quant_format) {
        vip_warning("check %s[%d] nbg quant format=%d, runtime quant format=%d\n",
             (flag == 0) ? "input" : "output", index, quant_format, buffer->param.quant_format);
    }

    if (data_format != (vip_int32_t)buffer->param.data_format) {
        vip_warning("check %s[%d] nbg data format=%d, runtime data format=%d\n",
             (flag == 0) ? "input" : "output", index, data_format, buffer->param.data_format);
    }

    if (VIP_BUFFER_QUANTIZE_DYNAMIC_FIXED_POINT == quant_format) {
        if (fixed_point_pos != buffer->param.quant_data.dfp.fixed_point_pos)
        {
            vip_warning("check %s[%d] nbg zfp=%d, runtime zfp=%d\n",
                       (flag == 0) ? "input" : "output", index, fixed_point_pos,
                       buffer->param.quant_data.dfp.fixed_point_pos);
        }
    }
    else if (VIP_BUFFER_QUANTIZE_TF_ASYMM == quant_format) {
        vip_float_t diff = scale / buffer->param.quant_data.affine.scale;
        if ((diff >= 1.1) || (diff <= 0.9)) {
            vip_warning("check %s[%d] nbg scale=%f, runtime scale=%f\n",
                           (flag == 0) ? "input" : "output", index, scale,
                            buffer->param.quant_data.affine.scale);
        }

        if (zero_point != buffer->param.quant_data.affine.zeroPoint) {
            vip_warning("check %s[%d] nbg zero point=%d, runtime zero point=%d\n",
                            (flag == 0) ? "input" : "output", index, zero_point,
                            buffer->param.quant_data.affine.zeroPoint);
        }
    }

    return matched;

onError:
    vip_error("fail to check %s[%d] parameter, status=%d\n",
                (0 == flag) ? "input" : "output", index, status);
    return matched;
}

#if vpmdENABLE_USE_AGENT_TRIGGER
static vip_status_e nbglk_get_network_address_info(
    vip_network_t *network,
    void *value
    )
{
    vip_status_e status = VIP_SUCCESS;
    vipdrv_query_address_info_t address;
    vip_address_info_t *info = (vip_address_info_t*)value;
    nbglk_segment_t *segment = &network->segments[0];
    vip_uint32_t i = 0;
    vip_uint32_t j = 0;
    NBGLK_CHECK_NETWORK();
    vipIsNULL(value);
    vipIsNULL(network);

    if ((value == VIP_NULL) || (segment == VIP_NULL)) {
        vip_error("value or segment is null");
        vipGoOnError(VIP_ERROR_INVALID_ARGUMENTS);
    }

    if (network->segment_num != 1) {
        vip_error("not support segment number is %d for this feature",
                     network->segment_num);
        vipGoOnError(VIP_ERROR_NOT_SUPPORTED);
    }

    viphal_zero_memory((void *)info, sizeof(vip_address_info_t));
    /* allocate memory */
    vipOnError(viphal_allocate_memory(sizeof(vip_input_output_info_t) * network->nbg_desc.n_inputs,
                          (void **) &info->input_info));
    viphal_zero_memory((void *)info->input_info, sizeof(vip_input_output_info_t) * network->nbg_desc.n_inputs);
    vipOnError(viphal_allocate_memory(sizeof(vip_input_output_info_t) * network->nbg_desc.n_outputs,
                          (void **) &info->output_info));
    viphal_zero_memory((void *)info->output_info, sizeof(vip_input_output_info_t) * network->nbg_desc.n_outputs);

    /* query wait-link address */
    viphal_query_address_info(&address);
    info->waitlink_logical = VIPUINT64_TO_PTR(address.waitlink_logical);
    info->waitlink_physical = address.waitlink_physical;
    info->waitlink_size = address.waitlink_size;

    /* get command buffer base address and size */
    info->commandbuffer_logical = (void*)segment->states_buffer.logical;
    info->commandbuffer_physical = SIZE_CAST(vip_uint32_t, segment->states_buffer.physical);
    info->commandbuffer_size = segment->command_size;

    /* get input address in command buffer */
    info->input_num = network->nbg_desc.n_inputs;
    for (i = 0; i < network->nbg_desc.n_inputs; i++) {
        nbglk_io_patch_info_t *input = &network->network_inputs[i];
        info->input_info[i].logical_in_cmd = (void*)input->logical_in_cmd;
        info->input_info[i].physical_in_cmd = (vip_uint32_t*)input->physical_in_cmd;
        info->input_info[i].patch = input->patch;
        info->input_info[i].slice_num = input->slice_num;
        info->input_info[i].physical = input->physical;
        vip_debug("input logical ref=%p, physical ref=%p, patch=%p, num=%d,"
                "input buffer physical=%llX\n", input->logical_in_cmd, input->physical_in_cmd,
                input->patch, input->slice_num, info->input_info[i].physical);
        for (j = 0; j < input->slice_num; j++) {
            vip_debug("input in cmd logical=%08X, physical=%08X, offset=%08X\n",
                        input->logical_in_cmd[j], input->physical_in_cmd[j],
                        (vip_uint32_t)(input->patch[j]->src_offset));
        }
        if (input->physical == 0) {
            vip_error("input buffer is NULL, please call vip_set_input() before this\n");
        }
    }

    /* get output address in command buffer */
    info->output_num = network->nbg_desc.n_outputs;
    for (i = 0; i < network->nbg_desc.n_outputs; i++) {
        nbglk_io_patch_info_t *output = &network->network_outputs[i];
        info->output_info[i].logical_in_cmd = (void*)output->logical_in_cmd;
        info->output_info[i].physical_in_cmd = (vip_uint32_t*)output->physical_in_cmd;
        info->output_info[i].patch = output->patch;
        info->output_info[i].slice_num = output->slice_num;
        info->output_info[i].physical = output->physical;
        vip_debug("output logical ref=%p, physical ref=%p, patch=%p, num=%d,"
                "output buffer physical=%llX\n", output->logical_in_cmd, output->physical_in_cmd,
                output->patch, output->slice_num, info->output_info[i].physical);
        for (j = 0; j < output->slice_num; j++) {
            vip_debug("output in cmd logical=%08X, physical=%08X, offset=%08X\n",
                    output->logical_in_cmd[j], output->physical_in_cmd[j],
                    (vip_uint32_t)(output->patch[j]->src_offset));
        }
        if ((output->physical == 0) || (output->logical == VIP_NULL)) {
            vip_error("output buffer is NULL, please call vip_set_output() before this\n");
            vipGoOnError(VIP_ERROR_NETWORK_INCOMPATIBLE);
        }
    }
    vip_debug("wait-link physical=%08X, logical=%p, command physical=%08X, logical=%p,"
            "size=0x%08X bytes\n", info->waitlink_physical, info->waitlink_logical,
            info->commandbuffer_physical, info->commandbuffer_logical, info->commandbuffer_size);
onError:
    return status;
}
#endif

static vip_status_e nbglk_map_flash_address(
    vip_network_t *network,
    void *data,
    vip_size_t size
    )
{
    vip_status_e status = VIP_SUCCESS;
#if vpmdENABLE_MMU
    vip_address_t virtual = 0;
    vip_info("nbg flash address=%p, size=0x%x\n", data, size);
    if (VIPPTR_TO_UINT64(data) % 4096) {
        vip_error("fail flash base address should align 4096, address=%p\n", data);
        return VIP_ERROR_NOT_SUPPORTED;
    }
    /* wrap the flash address to VIP virtual */
    status = viphal_wrap_usermemory(data, size, VIP_BUFFER_MEMORY_TYPE_HOST,
                                &virtual, &network->flash.mem_id);
    if (status != VIP_SUCCESS) {
        vip_error("fail to create buffer from handle=%p, size=0x%xbytes\n", data, size);
        return status;
    }
    network->flash.physical = virtual;
    network->flash.logical = (vip_uint8_t*)data;
    network->flash.size = SIZE_CAST(vip_uint32_t, size);
#else
    /* the *data means that contiguous physical, the NBG file is copied on the physical memory */
    {
        vip_address_t physical_table[1] = {0};
        vip_uint32_t size_table[1] = {0};
        vip_uint8_t *logical = VIP_NULL;
        vip_address_t vip_virtual = 0;
        vip_uint32_t mem_id = 0;
        vip_uint32_t vip_physical = *(vip_uint32_t*)data;
        vip_info("nbg flash phy address=0x%x, size=0x%x\n", vip_physical, size);
        if (vip_physical % 4096) {
            vip_error("fail flash base address should align 4096, address=%p\n", data);
            return VIP_ERROR_NOT_SUPPORTED;
        }
        physical_table[0] = vip_physical;
        size_table[0] = SIZE_CAST(vip_uint32_t, size);
        status = viphal_wrap_userphysical(physical_table, size_table, 1, VIPDRV_VIDEO_MEM_ALLOC_MAP_USER,
                                              VIP_BUFFER_MEMORY_TYPE_HOST, &vip_virtual, &logical, &mem_id);
        if (status != VIP_SUCCESS) {
            vip_error("fail to create buffer from phy=0x%x, size=0x%xbytes\n", vip_physical, size);
            return status;
        }
        network->flash.physical = vip_virtual;
        network->flash.logical = logical;
        network->flash.size = size;
        network->flash.mem_id = mem_id;
        if (vip_physical != vip_virtual) {
            vip_error("fail nbg flash PA=0x%x, VA=0x%x\n", vip_physical, vip_virtual);
        }
    }
#endif
    return status;
}

static vip_status_e nbglk_unmap_flash_address(
    vip_network_t *network
    )
{
    vip_status_e status = VIP_SUCCESS;
#if vpmdENABLE_MMU
    if (network->flash.mem_id != 0) {
        status = viphal_unwrap_usermemory(network->flash.mem_id);
    }
#else
    if (network->flash.mem_id != 0) {
        status = viphal_unwrap_userphysical(network->flash.mem_id);
    }
#endif
    network->flash.logical = VIP_NULL;
    network->flash.physical = 0;
    network->flash.mem_id = 0;
    return status;
}

static vip_status_e nbglk_prepare_init_command(
    vip_network network
    )
{
    vip_status_e status = VIP_SUCCESS;

    /* init_command doesn't used in latest project. for backward compatibiltiy old project */
    if ((network->nbg_desc.n_hw_init_ops > 0) && (network->ICD.mem_id != 0)) {
        if (0 == network->init_commands.size) {
            status = nbglk_patch_init_command(network);
            if (status != VIP_SUCCESS) {
                vip_error("fail to patch init command\n");
                vipGoOnError(status);
            }
        }
    }
onError:
    return status;
}

/* initialize nbglk_io_patch_info_t for network input/output */
static vip_status_e nbglk_create_io_patch_info(
    vip_network_t *network,
    vip_uint32_t source_type
    )
{
    vip_status_e status = VIP_SUCCESS;
    vip_uint32_t i = 0;
    vip_uint32_t io_count = 0;
    nbglk_io_patch_info_t *io_info = VIP_NULL;

    if (NBG_SOURCE_INPUT == source_type) {
        io_count = network->nbg_desc.n_inputs;
    }
    else if (NBG_SOURCE_OUTPUT == source_type) {
        io_count = network->nbg_desc.n_outputs;
    }
    else {
        vip_error("fail not support source type=%d\n", source_type);
        vipGoOnError(VIP_ERROR_NOT_SUPPORTED);
    }

    for (i = 0; i < io_count; i++) {
        if (NBG_SOURCE_INPUT == source_type) {
            io_info = &network->network_inputs[i];
        }
        else if (NBG_SOURCE_OUTPUT == source_type) {
            io_info = &network->network_outputs[i];
        }

        if (io_info->slice_num > 0) {
            vip_uint32_t tmp_size = sizeof(vip_address_t) * io_info->slice_num;
            vipOnError(viphal_allocate_memory(tmp_size, (void **)&io_info->logical_in_cmd));
            viphal_zero_memory(io_info->logical_in_cmd, tmp_size);

            tmp_size = sizeof(vip_uint32_t) * io_info->slice_num;
            vipOnError(viphal_allocate_memory(tmp_size, (void **)&io_info->physical_in_cmd));
            viphal_zero_memory(io_info->physical_in_cmd, tmp_size);

            tmp_size = sizeof(nbg_desc_patch_t*) * io_info->slice_num;
            vipOnError(viphal_allocate_memory(tmp_size, (void**)&io_info->patch));
            viphal_zero_memory(io_info->patch, tmp_size);
        }
        else {
            vip_error("fail io patch info source type=%d, io index=%d\n", source_type, i);
            vipGoOnError(VIP_ERROR_FAILURE);
        }
        io_info->counter = 0;
    }

onError:
    return status;
}

static vip_status_e nbglk_destroy_io_patch_info(
    vip_network_t *network,
    vip_uint32_t source_type
    )
{
    vip_status_e status = VIP_SUCCESS;
    vip_uint32_t io_count = 0;
    vip_uint32_t i = 0;
    nbglk_io_patch_info_t *io_info = VIP_NULL;

    if (NBG_SOURCE_INPUT == source_type) {
        io_count = network->nbg_desc.n_inputs;
    }
    else if (NBG_SOURCE_OUTPUT == source_type) {
        io_count = network->nbg_desc.n_outputs;
    }
    else {
        vip_error("fail not support source type=%d\n", source_type);
        vipGoOnError(VIP_ERROR_NOT_SUPPORTED);
    }

    for (i = 0; i < io_count; i++) {
        if (NBG_SOURCE_INPUT == source_type) {
            io_info = &network->network_inputs[i];
        }
        else if (NBG_SOURCE_OUTPUT == source_type) {
            io_info = &network->network_outputs[i];
        }
        if (io_info->logical_in_cmd != VIP_NULL) {
            viphal_free_memory(io_info->logical_in_cmd);
            io_info->logical_in_cmd = VIP_NULL;
        }
        if (io_info->physical_in_cmd != VIP_NULL) {
            viphal_free_memory(io_info->physical_in_cmd);
            io_info->physical_in_cmd = VIP_NULL;
        }
        if (io_info->patch != VIP_NULL) {
            viphal_free_memory(io_info->patch);
            io_info->patch = VIP_NULL;
        }
    }

onError:
    return status;
}

/******************************************************************
| vip op || vip op ||   sw op   || vip op (preempt) ||  vip op   |
|    segment 0     || segment 1 ||    segment 2     || segment 3 |
******************************************************************/
static vip_status_e create_network_segment(
    vip_network_t *network,
    nbglk_segment_t *segment
    )
{
    vip_status_e status = VIP_SUCCESS;
    vip_uint32_t prev_segm_id = 0;
    vip_uint32_t count = 0, i = 0;
    nbg_desc_operation_t *operation = VIP_NULL;
    nbg_desc_operation_t *next_operation = VIP_NULL;
    for (i = 0; i < network->nbg_desc.n_operations; i++) {
        operation = &network->nbg_desc.operations[i];
        next_operation = &network->nbg_desc.operations[i + 1];;
        if (operation->type == NBG_OPERATION_TYPE_SW || i >= network->nbg_desc.n_operations - 1 ||
            NBG_OPERATION_TYPE_SW == next_operation->type
            #if vpmdENABLE_PREEMPTION
            || (network->nbg_desc.fixed.header.version >= 0x00010011 && operation->preempt)) {
            /* sw segment or last segment or next segment is sw or preempt segment */
            #else
            ) {
            /* sw segment or last segment or next segment is sw */
            #endif
            segment = &network->segments[count];
            segment->is_sw_segm = operation->type == NBG_OPERATION_TYPE_SW ? vip_true_e : vip_false_e;
            segment->segm_start_id = prev_segm_id;
            segment->segm_end_id = i + 1;
            segment->name = network->nbg_desc.orig_layers[operation->layer_id].name;
            prev_segm_id = i + 1;
            count++;
        }
    }

    if (count != network->segment_num) {
        vip_error("please check segment number count=%d, segment_num=%d..\n",
                count, network->segment_num);
        vipGoOnError(VIP_ERROR_NETWORK_INCOMPATIBLE);
    }
onError:
    return status;
}

static vip_status_e nbglk_valid_nbg_check(
    vip_network_t *network
    )
{
    vip_status_e status = VIP_SUCCESS;
    nbglk_context_t* context = network->context;

    /* check vip sram info */
    if (network->nbg_desc.mem_info.vip_sram_size > context->vip_sram_size) {
        vip_error("binary vip sram size=0x%x, actually size=0x%x\n",
            network->nbg_desc.mem_info.vip_sram_size, context->vip_sram_size);
        vipGoOnError(VIP_ERROR_NOT_SUPPORTED);
    }
    /* check dup command */
    if ((NBGLK_FROM_NBG_DUP_COMMAND == network->dup_type) &&
        (network->nbg_desc.fixed.header.version < 0x00010018)) {
        vip_error("fail nbg version is 0x%08x, not support dup cmd\n",
                network->nbg_desc.fixed.header.version);
        vipGoOnError(VIP_ERROR_NOT_SUPPORTED);
    }
    /* sanity check lcd size. the load states in this section before NBG version 0x00010018 */
    if ((0 == network->nbg_desc.fixed.coeff_data.size) &&
        (network->nbg_desc.fixed.header.version < 0x00010018)) {
        vip_error("fail to read nbg, lcd size is 0\n");
        vipGoOnError(VIP_ERROR_INVALID_NETWORK);
    }
    /* sanity check check sum value */
    if ((0 == network->nbg_desc.fixed.header.nbg_crc_sum) &&
        (network->nbg_desc.fixed.header.version >= 0x00010018)) {
        vip_error("fail to read nbg, crc check sum value is 0\n");
        /*vipGoOnError(VIP_ERROR_INVALID_NETWORK);*/
    }

    /* hardware target check */
    if (network->nbg_desc.fixed.header.hw_target != context->hw_target) {
        if ((0xAE != network->nbg_desc.fixed.header.hw_target) || (0XD1 != context->hw_target)) {
            vip_error("binary target=0x%X, actually target=0x%X\n",
                network->nbg_desc.fixed.header.hw_target, context->hw_target);
            vipGoOnError(VIP_ERROR_NOT_SUPPORTED);
        }
    }

    /* nbg version check */
    if (network->nbg_desc.fixed.header.version > NBG_VERSION) {
        if (network->nbg_desc.fixed.header.version < NBG_VERSION_MAJOR(2)) {
            vip_warning("NBG format version=0x%X, support NBG version=0x%X\n",
                network->nbg_desc.fixed.header.version, NBG_VERSION);
            vipGoOnError(VIP_ERROR_NOT_SUPPORTED);
        }
        else {
            if (network->nbg_desc.fixed.header.version > NBG_VERSION2) {
                vip_warning("NBG format version=0x%X, support NBG version=0x%X\n",
                    network->nbg_desc.fixed.header.version, NBG_VERSION2);
                vipGoOnError(VIP_ERROR_NOT_SUPPORTED);
            }
        }
    }

    if (0 == network->nbg_desc.n_operations) {
        vip_error("fail check nbg n_operations=%d\n", network->nbg_desc.n_operations);
        vipGoOnError(VIP_ERROR_NOT_SUPPORTED);
    }

#if vpmdENABLE_DEBUG_LOG > 2
    nbglk_dump_nbg_entry(network);
#endif

    vip_info("cid=0x%X, NBG version=0x%08X\n", context->hw_target, network->nbg_desc.fixed.header.version);
    vip_info("network name=%s, thread_id=0x%x\n",
        network->nbg_desc.net_info.name, viphal_os_get_tid());

onError:
    return status;
}

/*1. Allocate video memory.
  2. Read out NPU section data and fill data into video memory. */
static vip_status_e nbglk_alloc_memory(
    nbg_fmt_reader_t *reader,
    vip_network_t *network
    )
{
    vip_status_e status = VIP_SUCCESS;
    nbg_fmt_descriptor_t* nbg_desc = &network->nbg_desc;

    /* NN op data entries. */
    if (nbg_desc->fixed.nn_op_data_table.size > 0) {
        /* request video memory */
        network->nnCmds.size = SIZE_CAST(vip_uint32_t, nbg_desc->fixed.nn_op_data_table.size);
        vipOnError(viphal_allocate_videomemory(&network->nnCmds.logical, &network->nnCmds.mem_id,
            &network->nnCmds.physical, network->nnCmds.size,
            vipdMEMORY_ALIGN_SIZE, VIPDRV_VIDEO_MEM_ALLOC_4GB_ADDR));
        nbg_fmt_reader_locate(reader, SIZE_CAST(vip_uint32_t, nbg_desc->fixed.nn_op_data_table.offset));
        nbg_fmt_read_data_copy(reader, network->nnCmds.logical,
            nbg_desc->fixed.nn_op_data_table.size, "nnoptb");
    }

    /* TP op data entries. */
    if (nbg_desc->fixed.tp_op_data_table.size > 0) {
        /* request video memory */
        network->tpCmds.size = SIZE_CAST(vip_uint32_t, nbg_desc->fixed.tp_op_data_table.size);
        vipOnError(viphal_allocate_videomemory(&network->tpCmds.logical, &network->tpCmds.mem_id,
            &network->tpCmds.physical, network->tpCmds.size,
            vipdMEMORY_ALIGN_SIZE, VIPDRV_VIDEO_MEM_ALLOC_4GB_ADDR));
        nbg_fmt_reader_locate(reader, SIZE_CAST(vip_uint32_t, nbg_desc->fixed.tp_op_data_table.offset));
        nbg_fmt_read_data_copy(reader, network->tpCmds.logical,
            nbg_desc->fixed.tp_op_data_table.size, "tpoptb");
    }

#if vpmdENABLE_CHANGE_PPU_PARAM
    if (nbg_desc->n_ppu_param > 0) {
        vip_uint32_t size = sizeof(vip_ppu_param_patch_info_t) * nbg_desc->n_ppu_param;
        vipOnError(viphal_allocate_memory(size, (void**)&network->ppu_param_info));
        viphal_zero_memory(network->ppu_param_info, size);
    }
#endif

    /* ICD */
    if (nbg_desc->fixed.ICD.size > 0) {
        /* request video memory */
        network->ICD.size = SIZE_CAST(vip_uint32_t, nbg_desc->fixed.ICD.size);
        vipOnError(viphal_allocate_videomemory(&network->ICD.logical, &network->ICD.mem_id,
            &network->ICD.physical, network->ICD.size,
            vipdSH_ALIGN_SIZE, VIPDRV_VIDEO_MEM_ALLOC_MMU_PAGE_WRITE));
        nbg_fmt_reader_locate(reader, SIZE_CAST(vip_uint32_t, nbg_desc->fixed.ICD.offset));
        nbg_fmt_read_data_copy(reader, network->ICD.logical, network->ICD.size, "ICD");
    }

    /* Alloc video memory for coeff data. after NBG 0x00010018 verision, coeff data is weight */
    if ((nbg_desc->fixed.coeff_data.size > 0) && (network->dup_type != NBGLK_FROM_NBG_DUP_COMMAND)) {
        if (network->create_type & VIP_CREATE_NETWORK_FROM_FLASH) {
            network->coeff_data.physical = network->flash.physical + nbg_desc->fixed.coeff_data.offset;
            network->coeff_data.logical = network->flash.logical + nbg_desc->fixed.coeff_data.offset;
            network->coeff_data.mem_id = 0;
            /* santify check */
            if ((network->coeff_data.physical % 64) != 0) {
                vip_error("fail coeff data base physical=0x%x not align to 64\n",
                        network->coeff_data.physical);
                vipGoOnError(VIP_ERROR_INVALID_NETWORK);
            }
        }
        else {
            vip_size_t size = nbg_desc->fixed.coeff_data.size;
            #if vpmdPRELOAD_COEFF > 1
            size += viphal_query_hw_feature(VIP_HW_FEATURE_VIPSRAM_WIDTH);
            #endif
            vipOnError(viphal_allocate_videomemory(&network->coeff_data.logical,
                &network->coeff_data.mem_id,
                &network->coeff_data.physical, size, vipdSH_ALIGN_SIZE,
                vip_true_e == nbg_desc->coeff_data_writable ?
                VIPDRV_VIDEO_MEM_ALLOC_MMU_PAGE_WRITE : VIPDRV_VIDEO_MEM_ALLOC_NONE));
        }
        network->coeff_data.size = nbg_desc->fixed.coeff_data.size;
        /* Load data to coeff data buffer */
        nbg_fmt_reader_locate(reader, nbg_desc->fixed.coeff_data.offset);
        nbg_fmt_read_data(reader, &network->coeff_data.logical, nbg_desc->fixed.coeff_data.size, "coeff");
    }

    if (nbg_desc->fixed.header.version >= 0x00010018) {
        if (nbg_desc->fixed.ppu_instr.size > 0) {
            vipOnError(viphal_allocate_videomemory(&network->ppu_instr.logical, &network->ppu_instr.mem_id,
                &network->ppu_instr.physical, SIZE_CAST(vip_uint32_t, nbg_desc->fixed.ppu_instr.size),
                vipdSH_ALIGN_SIZE, VIPDRV_VIDEO_MEM_ALLOC_4GB_ADDR));
            nbg_fmt_reader_locate(reader, SIZE_CAST(vip_uint32_t, nbg_desc->fixed.ppu_instr.offset));
            nbg_fmt_read_data_copy(reader, network->ppu_instr.logical,
                nbg_desc->fixed.ppu_instr.size, "ppu_instr");
            network->ppu_instr.size = SIZE_CAST(vip_uint32_t, nbg_desc->fixed.ppu_instr.size);
        }
        else {
            if (nbg_desc->n_sh_ops > 0) {
                /* Sanity check */
                vip_error("fail the size of ppu instr is 0, then have %d ppu op\n", nbg_desc->n_sh_ops);
                vipGoOnError(VIP_ERROR_INVALID_NETWORK);
            }
        }
        if (nbg_desc->fixed.dyna_io.size > 0) {
            vipOnError(viphal_allocate_videomemory(&network->dyna_io.logical, &network->dyna_io.mem_id,
                &network->dyna_io.physical, nbg_desc->fixed.dyna_io.size,
                vipdMEMORY_ALIGN_SIZE, VIPDRV_VIDEO_MEM_ALLOC_MMU_PAGE_WRITE));
            nbg_fmt_reader_locate(reader, nbg_desc->fixed.dyna_io.offset);
            nbg_fmt_read_data_copy(reader, network->dyna_io.logical,
                nbg_desc->fixed.dyna_io.size, "dyna_io");
            network->dyna_io.size = nbg_desc->fixed.dyna_io.size;
        }
        if (nbg_desc->fixed.load_states.size > 0) {
            vipOnError(viphal_allocate_memory(SIZE_CAST(vip_uint32_t, nbg_desc->fixed.load_states.size),
                        (void **)&network->load_states));
            nbg_fmt_reader_locate(reader, SIZE_CAST(vip_uint32_t, nbg_desc->fixed.load_states.offset));
            nbg_fmt_read_data_copy(reader, network->load_states,
                nbg_desc->fixed.load_states.size, "load_states");
        }
        else {
            vip_error("fail the size of load states is 0\n");
            vipGoOnError(VIP_ERROR_INVALID_NETWORK);
        }
    }
    else {
        network->ppu_instr.logical = network->coeff_data.logical;
        network->ppu_instr.physical = network->coeff_data.physical;
        network->ppu_instr.size = network->coeff_data.size;
        network->dyna_io.logical = network->coeff_data.logical;
        network->dyna_io.physical = network->coeff_data.physical;
        network->dyna_io.size = network->coeff_data.size;
        network->load_states = network->coeff_data.logical;
    }

    if (nbg_desc->fixed.header.version >= 0x0001001C) {
        if (nbg_desc->fixed.coeff_header.size > 0) {
            vipOnError(viphal_allocate_videomemory(&network->coeff_header.logical,
                        &network->coeff_header.mem_id,
                        &network->coeff_header.physical, SIZE_CAST(vip_uint32_t, nbg_desc->fixed.coeff_header.size),
                        vipdMEMORY_ALIGN_SIZE, VIPDRV_VIDEO_MEM_ALLOC_NONE));
            nbg_fmt_reader_locate(reader, SIZE_CAST(vip_uint32_t, nbg_desc->fixed.coeff_header.offset));
            nbg_fmt_read_data_copy(reader, network->coeff_header.logical,
                nbg_desc->fixed.coeff_header.size, "coeff_header");
            network->coeff_header.size = SIZE_CAST(vip_uint32_t, nbg_desc->fixed.coeff_header.size);
        }
        else {
            network->coeff_header.logical = network->coeff_data.logical;
            network->coeff_header.physical = network->coeff_data.physical;
            network->coeff_header.size = network->coeff_data.size;
            network->coeff_header.mem_id = 0;
        }
    }
    else {
        network->coeff_header.logical = network->coeff_data.logical;
        network->coeff_header.physical = network->coeff_data.physical;
        network->coeff_header.size = network->coeff_data.size;
    }

onError:
    return status;
}

static vip_status_e nbglk_prepare_patch_info(
    vip_network_t *network
    )
{
    vip_status_e status = VIP_SUCCESS;
    vip_uint32_t i = 0, j = 0, k = 0;
    nbg_desc_patch_t *patch = VIP_NULL;
    vip_int32_t io_id = 0;

    for (k = 0; k < network->segment_num; k++) {
        nbglk_segment_t* segment = &network->segments[k];
        segment->patch_count = 0;
        for (i = 0; i < network->nbg_desc.n_operations; i++) {
            nbg_desc_operation_t* op = &network->nbg_desc.operations[i];
            CHECK_OPERATION_IN_SEGMENT(op, segment, i);

            for (j = 0; j < op->patch_count; j++) {
                nbglk_io_patch_info_t* patch_info = VIP_NULL;
                if (op->type != NBG_OPERATION_TYPE_SW) {
                    patch = &network->nbg_desc.pd_sections[op->patch_index + j];
                    io_id = patch->index;
                    if (patch->source_type == NBG_SOURCE_INPUT) {
                        if (io_id >= 0) {
                            patch_info = &network->network_inputs[io_id];
                        }
                    }
                    else if (patch->source_type == NBG_SOURCE_OUTPUT) {
                        if (io_id >= 0) {
                            patch_info = &network->network_outputs[io_id];
                        }
                    }
                    else if (NBG_SOURCE_MULTIVIP_CHIPID == patch->source_type ||
                        NBG_SOURCE_MULTIVIP_SYNC_CMDS == patch->source_type) {
                        segment->patch_count += 1;
                    }
                }
                else {
                    nbg_desc_layer_parameter_t* layer_param = &network->nbg_desc.lp_entries[op->patch_index + j];
                    io_id = layer_param->index;
                    if (layer_param->source_type == NBG_SOURCE_INPUT) {
                        if (io_id >= 0) {
                            patch_info = &network->network_inputs[io_id];
                        }
                    }
                    else if (layer_param->source_type == NBG_SOURCE_OUTPUT) {
                        if (io_id >= 0) {
                            patch_info = &network->network_outputs[io_id];
                        }
                    }
                }
                if (VIP_NULL != patch_info) {
                    patch_info->slice_num++;

                    switch (op->type) {
                    case NBG_OPERATION_TYPE_NN:
                        patch_info->patch_belong |= NBGLK_PATCH_BELONG_NN;
                        break;
                    case NBG_OPERATION_TYPE_SP:
                        patch_info->patch_belong |= NBGLK_PATCH_BELONG_SP;
                        break;
                    case NBG_OPERATION_TYPE_NN_LUT:
                        patch_info->patch_belong |= NBGLK_PATCH_BELONG_NN_LUT;
                        break;
                    case NBG_OPERATION_TYPE_NT:
                        patch_info->patch_belong |= NBGLK_PATCH_BELONG_NT;
                        break;
                    case NBG_OPERATION_TYPE_TP:
                        patch_info->patch_belong |= NBGLK_PATCH_BELONG_TP;
                        break;
                    case NBG_OPERATION_TYPE_SH:
                        patch_info->patch_belong |= NBGLK_PATCH_BELONG_SH;
                        break;
                    default:
                        patch_info->patch_belong |= NBGLK_PATCH_BELONG_OTHER;
                        break;
                    }
                }
            }
        }
    }

    return status;
}

#if vpmdDYNAMIC_CHANGE_MEM_POOL
static vip_status_e nbglk_prepare_pool_patch_info(
    vip_network_t *network
    )
{
    vip_status_e status = VIP_SUCCESS;
    vip_uint32_t i = 0, j = 0;
    nbg_desc_patch_t *patch = VIP_NULL;

    for (i = 0; i < network->nbg_desc.n_operations; i++) {
        nbg_desc_operation_t *op = &network->nbg_desc.operations[i];
        for (j = 0; j < op->patch_count; j++) {
            if (op->type != NBG_OPERATION_TYPE_SW) {
                patch = &network->nbg_desc.pd_sections[op->patch_index + j];
                if (patch->source_type == NBG_SOURCE_MEMORY_POOL) {
                    network->pool_patch_count++;
                }
            }
            else {
                nbg_desc_layer_parameter_t *layer_param = &network->nbg_desc.lp_entries[op->patch_index + j];
                if (layer_param->source_type == NBG_SOURCE_MEMORY_POOL) {
                    network->pool_patch_count++;
                }
            }
        }
    }

    #if vpmdENABLE_LAYER_DUMP
    network->pool_patch_count += get_nld_pool_patch_count(network);
    #endif

    if (0 < network->pool_patch_count) {
        vipOnError(viphal_allocate_memory(sizeof(nbglk_memory_pool_info_t) * network->pool_patch_count,
            (void**)&network->pool_patch_info));
        viphal_zero_memory(network->pool_patch_info, sizeof(nbglk_memory_pool_info_t)* network->pool_patch_count);
    }

onError:
    return status;
}
#endif

static vip_status_e nbglk_create_network(
    vip_network_t *network
    )
{
    vip_status_e status = VIP_SUCCESS;
    nbglk_segment_t *segment = VIP_NULL;
#if vpmdENABLE_CNN_PROFILING || vpmdENABLE_PREEMPTION
    vip_uint32_t i = 0;
#endif
    network->prepared_network = vip_false_e;
    network->input_dirty = 1;
    network->output_dirty = 1;
    network->prepare_count = 0;
    network->wait_timeout = vpmdHARDWARE_TIMEOUT;
    network->op_segm.start_id = 0;
    network->op_segm.end_id = -1;

#if vpmdENABLE_CNN_PROFILING
    if (network->context->func_config.enable_cnn_profile == vip_true_e) {
        /* per-operation run network */
        network->segment_num = network->nbg_desc.n_operations - nbglk_get_num_initend_operation(network);
    }
    else
#endif
    {
        /* Normal run mode. segment by software layer/operation.
           segment_num is the number of segment in viplite. this is for
           supporting CPU operation in viplite.
        */
        network->segment_num = 2 * get_sw_operation_count(network) + 1;
        #if vpmdENABLE_PREEMPTION
        for (i = 0; i < network->nbg_desc.n_operations; i++) {
            nbg_desc_operation_t *operation = &network->nbg_desc.operations[i];
            if (network->nbg_desc.fixed.header.version >= 0x00010011 && operation->preempt &&
                operation->type != NBG_OPERATION_TYPE_SW && i + 1 < network->nbg_desc.n_operations &&
                network->nbg_desc.operations[i + 1].type != NBG_OPERATION_TYPE_SW) {
                network->segment_num++;
            }
        }
        #endif
    }

    vipOnError(viphal_allocate_memory(sizeof(nbglk_segment_t) * network->segment_num,
                                         (void **)&network->segments));
    viphal_zero_memory((void *)network->segments, sizeof(nbglk_segment_t) * network->segment_num);

#if vpmdENABLE_CNN_PROFILING
    if (network->context->func_config.enable_cnn_profile == vip_true_e) {
        vip_uint32_t count = 0;
        nbg_desc_operation_t *operation = VIP_NULL;
        for (i = 0; i < network->nbg_desc.n_operations; i++) {
            operation = &network->nbg_desc.operations[i];
            if ((operation->type == NBG_OPERATION_TYPE_INIT) ||
                (operation->type == NBG_OPERATION_TYPE_END)) {
                continue;
            }
            segment = &network->segments[count];
            segment->segm_start_id = operation->layer_id; /* layer id */
            segment->segm_end_id = i;  /* op index */
            segment->name = network->nbg_desc.orig_layers[operation->layer_id].name;
            segment->uid = network->nbg_desc.orig_layers[operation->layer_id].uid;
            if (operation->type == NBG_OPERATION_TYPE_SW) {
                segment->is_sw_segm = vip_true_e;
            }
            else {
                segment->is_sw_segm = vip_false_e;
            }

            count++;
            if (count >network->segment_num) {
                vip_error("fail to check segment number\n");
                vipGoOnError(VIP_ERROR_NETWORK_INCOMPATIBLE);
            }
        }
    }
    else
#endif
    {
        vipOnError(create_network_segment(network, segment));
    }

#if vpmdPRELOAD_COEFF
    vipOnError(nbglk_coeff_property_init(network));
#endif

    if (1 < network->segment_num) {
        network->need_schedule = vip_false_e;
    }
    else {
        network->need_schedule = vip_true_e;
    }

    if ((0 == network->nbg_desc.n_inputs) || (0 == network->nbg_desc.n_outputs)) {
        vip_error("network input or output is 0, input=%d, output=%d\n",
                     network->nbg_desc.n_inputs, network->nbg_desc.n_outputs);
        vipGoOnError(VIP_ERROR_NETWORK_INCOMPATIBLE);
    }

    /* Create input/output table for command buffer. */
    vipOnError(viphal_allocate_memory(sizeof(nbglk_io_patch_info_t) * network->nbg_desc.n_inputs,
                                      (void **) &network->network_inputs));
    vipOnError(viphal_allocate_memory(sizeof(nbglk_io_patch_info_t) * network->nbg_desc.n_outputs,
                                      (void **) &network->network_outputs));
    viphal_zero_memory((void *)network->network_inputs,
                            sizeof(nbglk_io_patch_info_t) * network->nbg_desc.n_inputs);
    viphal_zero_memory((void *)network->network_outputs,
                            sizeof(nbglk_io_patch_info_t) * network->nbg_desc.n_outputs);

    vipOnError(nbglk_prepare_patch_info(network));

    vipOnError(nbglk_create_io_patch_info(network, NBG_SOURCE_INPUT));
    vipOnError(nbglk_create_io_patch_info(network, NBG_SOURCE_OUTPUT));

    get_sh_cmds_status(network);

#if vpmdDYNAMIC_CHANGE_MEM_POOL
    vipOnError(nbglk_prepare_pool_patch_info(network));
#endif

    if (NBGLK_FROM_NBG_DUP_COMMAND == network->dup_type) {
        vip_char_t name[256];
        viphal_os_snprint(name, 255, "%s_cmd_dup", network->nbg_desc.net_info.name);
        vipOnError(viphal_task_create(name, network->segment_num, &network->task_id));
    }
    else {
        vipOnError(viphal_task_create(network->nbg_desc.net_info.name,
            network->segment_num, &network->task_id));
    }

onError:
    if (status != VIP_SUCCESS) {
        vip_error("fail to create network\n");
        nbglk_finish_network(network);
    }
    return status;
}

vip_status_e nbg_create_network_from_file(
    IN viphal_file_t file,
    OUT vip_network_t *network
    )
{
    vip_status_e status = VIP_SUCCESS;
    nbg_fmt_reader_t reader;

#if vpmdDUMP_NBG_RESOURCE
    nbglk_dump_nbg(file, VIP_NULL, VIP_NULL);
#endif

    nbg_fmt_reader_open(&reader, VIP_NULL, 0, file, network->create_type);

    status = nbg_fmt_read(&reader, &network->nbg_desc);
    if (status != VIP_SUCCESS) {
        vip_error("fail to read nbg status=%d\n", status);
        vipGoOnError(status);
    }

    vipOnError(nbglk_valid_nbg_check(network));
    vipOnError(nbglk_alloc_memory(&reader, network));

    status = nbglk_create_network(network);
    if (status != VIP_SUCCESS) {
        vip_error("fail to create network status=%d\n", status);
        vipGoOnError(status);
    }

onError:
    nbg_fmt_reader_close(&reader);
    return status;
}

vip_status_e nbg_create_network_from_memory(
    vip_network_t *network,
    const void *data,
    vip_size_t size
    )
{
    vip_status_e status = VIP_SUCCESS;
    void *nbg_file = (void*)data;
    nbg_fmt_reader_t reader;

    if ((VIP_NULL == data) || (0 == size)) {
        vip_error("fail create network data is NULL or size is 0\n");
        vipGoOnError(VIP_ERROR_INVALID_ARGUMENTS);
    }

    if (network->create_type & VIP_CREATE_NETWORK_FROM_FLASH) {
        status = nbglk_map_flash_address(network, (void *)data, size);
        if (status != VIP_SUCCESS) {
            vip_error("fail to map flash address\n");
            vipGoOnError(VIP_ERROR_FAILURE);
        }
        vip_debug("create network from flash logical=%p, physical=0x%llx, size=%d\n",
                     network->flash.logical, network->flash.physical, size);
        nbg_file = (void*)network->flash.logical;
    }

#if vpmdDUMP_NBG_RESOURCE
    nbglk_dump_nbg(VIP_NULL, (void*)nbg_file, size);
#endif

#if vpmdENABLE_CHECKSUM > 1
    nbglk_nbg_checksum((void*)nbg_file, size);
#endif

    nbg_fmt_reader_open(&reader, (void*)nbg_file, size, VIP_NULL, network->create_type);

    status = nbg_fmt_read(&reader, &network->nbg_desc);
    if (status != VIP_SUCCESS) {
        vip_error("fail to read nbg status=%d\n", status);
        vipGoOnError(status);
    }

    vipOnError(nbglk_valid_nbg_check(network));
    vipOnError(nbglk_alloc_memory(&reader, network));

    status = nbglk_create_network(network);
    if (status != VIP_SUCCESS) {
        vip_error("fail to create network status=%d\n", status);
        vipGoOnError(status);
    }

onError:
    nbg_fmt_reader_close(&reader);
    return status;
}

vip_status_e nbg_create_network_from_network(
    IN vip_network_t *network_base,
    OUT vip_network_t *network_dup
    )
{
    vip_status_e status = VIP_SUCCESS;
    vip_uint32_t i = 0;
    vip_char_t name[256] = {0};

    /* copy the base network all objects into dup network */
    viphal_copy_memory(network_dup, network_base, sizeof(vip_network_t));
    network_dup->friend_network = network_base;
    network_dup->dup_type = NBGLK_FROM_NETWORK_DUP_COMMAND;
    if (1 < network_dup->segment_num) {
        network_dup->need_schedule = vip_false_e;
    }
    else {
        network_dup->need_schedule = vip_true_e;
    }

    /* Copy segments */
    vipOnError(viphal_allocate_memory(sizeof(nbglk_segment_t) * network_dup->segment_num,
              (void **)&network_dup->segments));
    viphal_copy_memory(network_dup->segments, network_base->segments,
                   sizeof(nbglk_segment_t) * network_dup->segment_num);

    /* Copy nnCmd and tpCmd */
    if (network_base->nbg_desc.fixed.nn_op_data_table.size > 0) {
        vipOnError(viphal_allocate_videomemory(&network_dup->nnCmds.logical,
            &network_dup->nnCmds.mem_id, &network_dup->nnCmds.physical,
            network_base->nnCmds.size, vipdMEMORY_ALIGN_SIZE, VIPDRV_VIDEO_MEM_ALLOC_4GB_ADDR));
        viphal_copy_memory(network_dup->nnCmds.logical,
            network_base->nnCmds.logical, network_base->nnCmds.size);
    }

    if (network_base->nbg_desc.fixed.tp_op_data_table.size > 0) {
        vipOnError(viphal_allocate_videomemory(&network_dup->tpCmds.logical,
            &network_dup->tpCmds.mem_id, &network_dup->tpCmds.physical,
            network_base->tpCmds.size, vipdMEMORY_ALIGN_SIZE, VIPDRV_VIDEO_MEM_ALLOC_4GB_ADDR));
        viphal_copy_memory(network_dup->tpCmds.logical,
            network_base->tpCmds.logical, network_base->tpCmds.size);
    }
    if ((network_base->nbg_desc.fixed.dyna_io.size > 0) &&
        (0x00010018 <= network_base->nbg_desc.fixed.header.version)) {
        vipOnError(viphal_allocate_videomemory(&network_dup->dyna_io.logical,
            &network_dup->dyna_io.mem_id, &network_dup->dyna_io.physical,
            network_base->nbg_desc.fixed.dyna_io.size, vipdMEMORY_ALIGN_SIZE,
            VIPDRV_VIDEO_MEM_ALLOC_MMU_PAGE_WRITE));
        viphal_copy_memory(network_dup->dyna_io.logical,
            network_base->dyna_io.logical, network_base->dyna_io.size);
    }

    viphal_os_snprint(name, 255, "%s_weak_dup", network_dup->nbg_desc.net_info.name);
    vipOnError(viphal_task_create(name, network_dup->segment_num, &network_dup->task_id));

#if vpmdDYNAMIC_CHANGE_MEM_POOL
    if (network_dup->pool_patch_count > 0) {
        vipOnError(viphal_allocate_memory(sizeof(nbglk_memory_pool_info_t) * network_dup->pool_patch_count,
                                          (void**)&network_dup->pool_patch_info));
        viphal_zero_memory(network_dup->pool_patch_info,
                           sizeof(nbglk_memory_pool_info_t) * network_dup->pool_patch_count);
    }
#endif

    /* Copy inputs and outputs */
    vipOnError(viphal_allocate_memory(sizeof(nbglk_io_patch_info_t) *
        network_base->nbg_desc.n_inputs, (void **) &network_dup->network_inputs));
    vipOnError(viphal_allocate_memory(sizeof(nbglk_io_patch_info_t) *
        network_base->nbg_desc.n_outputs, (void **) &network_dup->network_outputs));
    viphal_copy_memory(network_dup->network_inputs, network_base->network_inputs,
        sizeof(nbglk_io_patch_info_t) * network_base->nbg_desc.n_inputs);
    viphal_copy_memory(network_dup->network_outputs, network_base->network_outputs,
        sizeof(nbglk_io_patch_info_t) * network_base->nbg_desc.n_outputs);

    vipOnError(nbglk_create_io_patch_info(network_dup, NBG_SOURCE_INPUT));
    vipOnError(nbglk_create_io_patch_info(network_dup, NBG_SOURCE_OUTPUT));

    for (i = 0; i < network_dup->nbg_desc.n_inputs; i ++) {
        const nbglk_io_patch_info_t *orig_io = &network_base->network_inputs[i];
        nbglk_io_patch_info_t *copy_io = &network_dup->network_inputs[i];

        vip_uint32_t size = sizeof(vip_address_t) * orig_io->slice_num;
        viphal_copy_memory(copy_io->logical_in_cmd, orig_io->logical_in_cmd, size);

        size = sizeof(vip_uint32_t) * orig_io->slice_num;
        viphal_copy_memory(copy_io->physical_in_cmd, orig_io->physical_in_cmd, size);

        size = sizeof(nbg_desc_patch_t*) * orig_io->slice_num;
        viphal_copy_memory(copy_io->patch, orig_io->patch, size);
    }
    for (i = 0; i < network_dup->nbg_desc.n_outputs; i ++) {
        const nbglk_io_patch_info_t *orig_io = &network_base->network_outputs[i];
        nbglk_io_patch_info_t *copy_io = &network_dup->network_outputs[i];

        vip_uint32_t size = sizeof(vip_address_t) * orig_io->slice_num;
        viphal_copy_memory(copy_io->logical_in_cmd, orig_io->logical_in_cmd, size);

        size = sizeof(vip_uint32_t) * orig_io->slice_num;
        viphal_copy_memory(copy_io->physical_in_cmd, orig_io->physical_in_cmd, size);

        size = sizeof(nbg_desc_patch_t*) * orig_io->slice_num;
        viphal_copy_memory(copy_io->patch, orig_io->patch, size);
    }

    vip_info("dup create network=%p\n", network_dup);
    network_dup->belong_to_group = vip_false_e;
    network_dup->prepared_network = vip_false_e;
onError:
    return status;
}

static vip_status_e free_dyna_data(
    vip_network network
    )
{
    if (VIP_NULL != network->nbg_desc.inputs) {
        viphal_free_memory(network->nbg_desc.inputs);
        network->nbg_desc.inputs = VIP_NULL;
    }

    if (VIP_NULL != network->nbg_desc.outputs) {
        viphal_free_memory(network->nbg_desc.outputs);
        network->nbg_desc.outputs = VIP_NULL;
    }

    if (VIP_NULL != network->nbg_desc.orig_layers) {
        viphal_free_memory(network->nbg_desc.orig_layers);
        network->nbg_desc.orig_layers = VIP_NULL;
    }

    if (VIP_NULL != network->nbg_desc.operations) {
        viphal_free_memory(network->nbg_desc.operations);
        network->nbg_desc.operations = VIP_NULL;
    }

    if (VIP_NULL != network->nbg_desc.lp_entries) {
        viphal_free_memory(network->nbg_desc.lp_entries);
        network->nbg_desc.lp_entries = VIP_NULL;
    }

    if (VIP_NULL != network->nbg_desc.sw_ops) {
        viphal_free_memory(network->nbg_desc.sw_ops);
        network->nbg_desc.sw_ops = VIP_NULL;
    }

    if (VIP_NULL != network->nbg_desc.init_ops) {
        viphal_free_memory(network->nbg_desc.init_ops);
        network->nbg_desc.init_ops = VIP_NULL;
    }

    if (VIP_NULL != network->nbg_desc.ICDT) {
        viphal_free_memory(network->nbg_desc.ICDT);
        network->nbg_desc.ICDT = VIP_NULL;
    }

    if (VIP_NULL != network->nbg_desc.ppu_param) {
        viphal_free_memory(network->nbg_desc.ppu_param);
        network->nbg_desc.ppu_param = VIP_NULL;
    }

    return VIP_SUCCESS;
}

static vip_status_e free_dyna_data_two(
    vip_network network
    )
{
    if (VIP_NULL != network->nbg_desc.sh_ops) {
        viphal_free_memory(network->nbg_desc.sh_ops);
        network->nbg_desc.sh_ops = VIP_NULL;
    }

    if (VIP_NULL != network->nbg_desc.pd_sections) {
        viphal_free_memory(network->nbg_desc.pd_sections);
        network->nbg_desc.pd_sections = VIP_NULL;
    }

    if (network->nbg_desc.fixed.header.version >= 0x0001001C) {
        if (network->nbg_desc.coeff_header_table != network->nbg_desc.coeff_data_table) {
            if (VIP_NULL != network->nbg_desc.coeff_header_table) {
                viphal_free_memory(network->nbg_desc.coeff_header_table);
                network->nbg_desc.coeff_header_table = VIP_NULL;
            }
        }

        if (VIP_NULL != network->nbg_desc.ppu_instr_table) {
            viphal_free_memory(network->nbg_desc.ppu_instr_table);
            network->nbg_desc.ppu_instr_table = VIP_NULL;
        }

        if (VIP_NULL != network->nbg_desc.dyna_io_table) {
            viphal_free_memory(network->nbg_desc.dyna_io_table);
            network->nbg_desc.dyna_io_table = VIP_NULL;
        }

        if (VIP_NULL != network->nbg_desc.load_states_table) {
            viphal_free_memory(network->nbg_desc.load_states_table);
            network->nbg_desc.load_states_table = VIP_NULL;
        }
    }

    if (VIP_NULL != network->nbg_desc.coeff_data_table) {
        viphal_free_memory(network->nbg_desc.coeff_data_table);
        network->nbg_desc.coeff_data_table = VIP_NULL;
    }

    if (VIP_NULL != network->nbg_desc.ldump_patch_data) {
        viphal_free_memory(network->nbg_desc.ldump_patch_data);
        network->nbg_desc.ldump_patch_data = VIP_NULL;
    }
    if (VIP_NULL != network->nbg_desc.ldump_outputs) {
        viphal_free_memory(network->nbg_desc.ldump_outputs);
        network->nbg_desc.ldump_outputs = VIP_NULL;
    }
    if (VIP_NULL != network->nbg_desc.ldump_operation) {
        viphal_free_memory(network->nbg_desc.ldump_operation);
        network->nbg_desc.ldump_operation = VIP_NULL;
    }
    if (VIP_NULL != network->nbg_desc.ldump_instr_table) {
        viphal_free_memory(network->nbg_desc.ldump_instr_table);
        network->nbg_desc.ldump_instr_table = VIP_NULL;
    }
    if (VIP_NULL != network->nbg_desc.ldump_load_states_table) {
        viphal_free_memory(network->nbg_desc.ldump_load_states_table);
        network->nbg_desc.ldump_load_states_table = VIP_NULL;
    }
    if (VIP_NULL != network->nbg_desc.ldump_instr_logical) {
        viphal_free_memory(network->nbg_desc.ldump_instr_logical);
        network->nbg_desc.ldump_instr_logical = VIP_NULL;
    }
    if (VIP_NULL != network->nbg_desc.ldump_load_states_logical) {
        viphal_free_memory(network->nbg_desc.ldump_load_states_logical);
        network->nbg_desc.ldump_load_states_logical = VIP_NULL;
    }

    return VIP_SUCCESS;
}

vip_status_e nbglk_destroy_network(
    IN vip_network network
    )
{
    vip_status_e status = VIP_SUCCESS;
    NBGLK_CHECK_NETWORK();

    if (network->create_type & VIP_CREATE_NETWORK_FROM_FLASH) {
        status = nbglk_unmap_flash_address(network);
        if (status != VIP_SUCCESS) {
            vip_error("fail to unmap flash address\n");
        }
    }

#if vpmdDYNAMIC_CHANGE_MEM_POOL
    if (VIP_NULL != network->pool_patch_info) {
        viphal_free_memory(network->pool_patch_info);
        network->pool_patch_info = VIP_NULL;
    }
#endif

    /* Free the resources allocated during creating network. */
    nbglk_destroy_io_patch_info(network, NBG_SOURCE_INPUT);
    nbglk_destroy_io_patch_info(network, NBG_SOURCE_OUTPUT);

    if (network->network_inputs != VIP_NULL) {
        viphal_free_memory((void *)network->network_inputs);
        network->network_inputs = VIP_NULL;
    }

    if (network->network_outputs != VIP_NULL) {
        viphal_free_memory((void *)network->network_outputs);
        network->network_outputs = VIP_NULL;
    }

    if (network->tpCmds.mem_id != 0) {
        status = viphal_free_videomemory(network->tpCmds.mem_id);
        network->tpCmds.mem_id = 0;
    }

    if (network->nnCmds.mem_id != 0) {
        status = viphal_free_videomemory(network->nnCmds.mem_id);
        network->nnCmds.mem_id = 0;
    }

    if (network->nbg_desc.fixed.header.version >= 0x00010018) {
        if (network->dyna_io.mem_id != 0) {
            status = viphal_free_videomemory(network->dyna_io.mem_id);
            network->dyna_io.mem_id = 0;
        }
    }

    #if vpmdENABLE_LAYER_DUMP
    if (network->nld.enable) {
        nbglk_nld_destroy(network);
        network->nld.enable = vip_false_e;
    }
    #endif

    if (network->dup_type != NBGLK_FROM_NETWORK_DUP_COMMAND) {
        if (network->nbg_desc.fixed.header.version >= 0x00010018) {
            if (network->ppu_instr.mem_id != 0) {
                status = viphal_free_videomemory(network->ppu_instr.mem_id);
                network->ppu_instr.mem_id = 0;
            }
            if (network->load_states != VIP_NULL) {
                viphal_free_memory(network->load_states);
                network->load_states = VIP_NULL;
            }
        }
        if (network->coeff_header.mem_id != 0) {
            status = viphal_free_videomemory(network->coeff_header.mem_id);
            network->coeff_header.mem_id = 0;
        }
        if (network->ICD.mem_id != 0) {
            viphal_free_videomemory(network->ICD.mem_id);
            network->ICD.mem_id = 0;
        }
        if (network->init_commands.mem_id != 0) {
            viphal_free_videomemory(network->init_commands.mem_id);
            network->init_commands.mem_id = 0;
            network->init_commands.logical = VIP_NULL;
            network->init_commands.size = 0;
        }
        free_dyna_data(network);
        #if vpmdPRELOAD_COEFF
        #if vpmdPRELOAD_COEFF > 1
        nbglk_coeff_npu_preload_uninit(network);
        #endif
        if (VIP_NULL != network->coeff_property) {
            viphal_free_memory(network->coeff_property);
            network->coeff_property = VIP_NULL;
        }
        #endif
        free_dyna_data_two(network);
        if (network->coeff_data.mem_id != 0) {
            viphal_free_videomemory(network->coeff_data.mem_id);
            network->coeff_data.mem_id = 0;
            network->coeff_data.logical = VIP_NULL;
            network->coeff_data.physical = 0;
            network->coeff_data.size = 0;
        }
        #if vpmdENABLE_CHANGE_PPU_PARAM
        if (network->ppu_param_info != VIP_NULL) {
            viphal_free_memory(network->ppu_param_info);
            network->ppu_param_info = VIP_NULL;
        }
        #endif
    }

    if (network->segments != VIP_NULL) {
        viphal_free_memory(network->segments);
        network->segments = VIP_NULL;
    }

    viphal_task_destroy(network->task_id);

    vip_info("destroy network=%p\n", network);
    /* Reset the network members. */
    viphal_zero_memory((void *)network, sizeof(*network));

    return status;
}

vip_status_e nbglk_finish_network(
    IN vip_network_t *network
    )
{
    vip_status_e status = VIP_SUCCESS;
    nbglk_segment_t *segment = VIP_NULL;
    vip_uint32_t i = 0;
    NBGLK_CHECK_NETWORK();

    network->prepared_network = vip_false_e;

#if vpmdENABLE_CANCELATION > 1
    if (network->dup_type != NBGLK_FROM_NETWORK_DUP_COMMAND) {
        nbglk_breakpoint_uninit(network);
    }
#endif
#if vpmdENABLE_MULTIPLE_VIP
    if (network->multivip_sync_cmd != VIP_NULL) {
        viphal_free_memory(network->multivip_sync_cmd);
        network->multivip_sync_cmd = VIP_NULL;
        network->sync_cmd_size = 0;
    }
#endif

    for (i = 0; i < network->nbg_desc.n_inputs; i++) {
        if (network->network_inputs[i].slice_num > 0) {
            vip_uint32_t tmp_size = sizeof(vip_address_t) * network->network_inputs[i].slice_num;
            viphal_zero_memory(network->network_inputs[i].logical_in_cmd, tmp_size);

            tmp_size = sizeof(vip_uint32_t) * network->network_inputs[i].slice_num;
            viphal_zero_memory(network->network_inputs[i].physical_in_cmd, tmp_size);

            tmp_size = sizeof(nbg_desc_patch_t*) * network->network_inputs[i].slice_num;
            viphal_zero_memory(network->network_inputs[i].patch, tmp_size);
        }
        network->network_inputs[i].counter = 0;
    }

    for (i = 0; i < network->nbg_desc.n_outputs; i++) {
        vip_uint32_t tmp_size = sizeof(vip_address_t) * network->network_outputs[i].slice_num;
        viphal_zero_memory(network->network_outputs[i].logical_in_cmd, tmp_size);

        tmp_size = sizeof(vip_uint32_t) * network->network_outputs[i].slice_num;
        viphal_zero_memory(network->network_outputs[i].physical_in_cmd, tmp_size);

        tmp_size = sizeof(nbg_desc_patch_t*) * network->network_outputs[i].slice_num;
        viphal_zero_memory(network->network_outputs[i].patch, tmp_size);

        network->network_outputs[i].counter = 0;
    }
    #if !vpmdENABLE_CNN_PROFILING
    for (i = 0; i < network->segment_num; i++) {
        segment = &network->segments[i];
        if ((segment != VIP_NULL) && (segment->is_sw_segm != vip_true_e)) {
            status = nbglk_network_segment_wait(network, segment);
            if ((status != VIP_SUCCESS) && (status != VIP_ERROR_CANCELED)) {
                vip_error("fail to wait network segment %d, status=%d\n", i, status);
                break;
            }
            /*return success status when segment is canceled or executed done */
            #if vpmdENABLE_CANCELATION
            if (status == VIP_ERROR_CANCELED) {
                status = VIP_SUCCESS;
            }
            #endif
        }
    }
    #endif
    if (network->dup_type != NBGLK_FROM_NETWORK_DUP_COMMAND) {
        #if vpmdENABLE_CAPTURE || (vpmdENABLE_HANG_DUMP > 1)
        network->capture_done = 0;
        #endif
        if ((network->pool.logical != VIP_NULL) && (VIP_NULL == network->memory_pool)) {
            viphal_free_videomemory(network->pool.mem_id);
            network->pool.logical = VIP_NULL;
        }

        if (vip_true_e == network->npd.enable_npd) {
            nbglk_npd_uninit(network);
        }

        for (i = 0; i < network->segment_num; i++) {
            segment = &network->segments[i];
            if (segment != VIP_NULL) {
            #if vpmdENABLE_CANCELATION > 1
                if ((segment->is_sw_segm != vip_true_e)) {
                    if (segment->bp_pos_table != VIP_NULL) {
                      viphal_free_memory(segment->bp_pos_table);
                      segment->bp_pos_table = VIP_NULL;
                    }
                }
            #endif
                if (segment->split_pos_table != VIP_NULL) {
                    viphal_free_memory(segment->split_pos_table);
                    segment->split_pos_table = VIP_NULL;
                }
                if (segment->operation_pos != VIP_NULL) {
                    viphal_free_memory((void*)segment->operation_pos);
                    segment->operation_pos = VIP_NULL;
                }
                if (segment->split_size_table != VIP_NULL) {
                    viphal_free_memory(segment->split_size_table);
                    segment->split_size_table = VIP_NULL;
                }
            }
        }
    }

    if (network->segments != VIP_NULL) {
        for (i = 0; i < network->segment_num; i++) {
            segment = &network->segments[i];
            #if vpmdENABLE_RECORD_INFO
            if (network->dup_type != NBGLK_FROM_NETWORK_DUP_COMMAND) {
                nbglk_hashmap_destroy(&segment->record_list);
            }
            #endif
            if (vip_true_e == segment->is_sw_segm) {
                #if vpmdENABLE_SUPPORT_CPU_LAYER
                nbglk_sw_release_resource(segment);
                #else
                vip_error("NOT support CPU layers, please set vpmdENABLE_SUPPORT_CPU_LAYER to 1\n");
                #endif
            }
            if (vip_true_e == network->realloc_sh_buff && segment->sh_cmd_buffer.logical != VIP_NULL) {
                viphal_free_videomemory(segment->sh_cmd_buffer.mem_id);
                segment->sh_cmd_buffer.logical = VIP_NULL;
            }
            if (segment->states_buffer.logical != VIP_NULL) {
                viphal_free_videomemory(segment->states_buffer.mem_id);
                segment->states_buffer.logical = VIP_NULL;
            }
        }
    }

    return status;
}

vip_status_e nbglk_set_network(
    IN vip_network_t *network,
    IN vip_enum property,
    IN void *value
    )
{
    vip_status_e status = VIP_SUCCESS;
    vipIsNULL(value);

    if (network->prepared_network) {
        vip_uint8_t flag = 0;
        /* Allows set_network() property after calling vip_preapre_network() */
        if (VIP_NETWORK_PROP_SET_DEVICE_INDEX == property ||
            VIP_NETWORK_PROP_SET_CORE_INDEX == property) {
            if (1 == network->nbg_desc.net_info.core_count) {
                flag = 1;
            }
        }
        #if vpmdDYNAMIC_CHANGE_MEM_POOL
        else if (VIP_NETWORK_PROP_SET_MEMORY_POOL == property) {
            flag = 1;
        }
        #endif
        if (0 == flag) {
            vip_error("failed set network should be called before prepare network property=%d\n",
                         property);
            vipOnError(VIP_ERROR_FAILURE);
        }
    }

    switch (property) {
    case VIP_NETWORK_PROP_SET_MEMORY_POOL:
    {
        vip_buffer buffer = (vip_buffer)value;
        vip_uint32_t pool_size = VIP_ALIGN(network->nbg_desc.mem_info.pool_size, vipdMEMORY_ALIGN_SIZE);

        vip_debug("set network memory pool buffer=%p\n", buffer);
        if ((buffer->allocated > NBGLK_BUFFER_ALLOCATE_MAX) ||
            (NBGLK_BUFFER_ALLOCATE_MAX == buffer->allocated)) {
            vip_error("fail to set memory pool, memory not allocated=%d\n", buffer->allocated);
            vipOnError(VIP_ERROR_FAILURE);
        }
        if (0 == buffer->memory.physical) {
            vip_error("fail to set memory pool, mem_id=0x%x, logical=%d, physical=0x%llx\n",
                         buffer->memory.mem_id, buffer->memory.logical,
                         buffer->memory.physical);
            vipOnError(VIP_ERROR_FAILURE);
        }
        if (pool_size > buffer->memory.size) {
            vip_error("fail to set memory pool, buffer size is small than"
                        "network memory pool size, buffer_size=%d < pool_size=%d\n",
                        buffer->memory.size, pool_size);
            vipOnError(VIP_ERROR_FAILURE);
        }
        #if vpmdDYNAMIC_CHANGE_MEM_POOL
        vipOnError(nbglk_patch_memory_pool(network, buffer));
        #endif
        network->memory_pool = buffer;
    }
    break;

    case VIP_NETWORK_PROP_SET_DEVICE_INDEX:
    {
        vip_uint32_t *id = (vip_uint32_t*)value;
        vip_uint32_t core_cnt = 0;
        if (*id >= network->context->device_count) {
            vip_error("fail to set device index=%d, value is more than device count=%d.\n",
                        *id, network->context->device_count);
            vipOnError(VIP_ERROR_FAILURE);
        }

        core_cnt = network->context->devices[*id].core_count;
        if (network->nbg_desc.net_info.core_count > core_cnt) {
            vip_error("fail to set device index=%d, need core cnt=%d, cur device core cnt=%d.\n",
                *id, network->nbg_desc.net_info.core_count, core_cnt);
            vipOnError(VIP_ERROR_FAILURE);
        }

        network->device = &network->context->devices[*id];
        if (*id != network->device->dev_index) {
            vip_error("fail to set device index, %d not match %d\n", *id, network->device->dev_index);
            vipOnError(VIP_ERROR_FAILURE);
        }
    }
    break;

    case VIP_NETWORK_PROP_SET_CORE_INDEX:
    {
        vip_int32_t *data = (vip_int32_t*)value;
        vip_int32_t core_cnt = (vip_int32_t)network->device->core_count;
        if ((*data < 0) ||
            (*data + (vip_int32_t)network->nbg_desc.net_info.core_count > core_cnt)) {
            vip_error("fail to set core id=%d, network need core cnt=%d, cur device core cnt=%d.\n",
                *data, network->nbg_desc.net_info.core_count, core_cnt);
            vipOnError(VIP_ERROR_FAILURE);
        }

        network->core_index = *data;
        network->need_schedule = vip_false_e;
    }
    break;

    case VIP_NETWORK_PROP_SET_PRIORITY:
    {
        #if vpmdENABLE_PREEMPTION
        vip_uint8_t *data = (vip_uint8_t*)value;
        network->priority = *data;
        #else
        vip_error("driver doesn't support this feature, please enable in driver\n");
        #endif
    }
    break;

    case VIP_NETWORK_PROP_CHANGE_PPU_PARAM:
    {
        #if vpmdENABLE_CHANGE_PPU_PARAM
        vip_uint32_t* data = (vip_uint32_t*)value;
        network->change_ppu_param = vip_false_e;
        if (1 == *data) {
            if (1 == network->nbg_desc.net_info.attribute.change_ppu_param) {
                network->change_ppu_param = vip_true_e;
            }
            else {
                vip_error("the network not support change ppu param change ppu=%d\n",
                          network->nbg_desc.net_info.attribute.change_ppu_param);
                vipOnError(VIP_ERROR_FAILURE);
            }
        }
        #else
        vip_error("driver doesn't support this feature, please enable in driver\n");
        #endif
    }
    break;

    case VIP_NETWORK_PROP_SET_TIME_OUT:
    {
        vip_uint32_t *data = (vip_uint32_t*)value;
        network->wait_timeout = *data;
    }
    break;

    case VIP_NETWORK_PROP_SET_COEFF_MEMORY:
    {
        #if vpmdPRELOAD_COEFF
        vip_buffer buffer = (vip_buffer)value;
        if ((0 == buffer->memory.size)) {
            vip_error("fail to set network coeff buff size=%d\n", buffer->memory.size);
            vipOnError(VIP_ERROR_FAILURE);
        }
        network->app_preload_coeff = &buffer->memory;
        if (VIP_NULL == buffer->memory.logical) {
            vip_map_buffer(buffer);
            if (VIP_NULL == buffer->memory.logical) {
                vip_error("fail to set network coeff buff, cpu logical is NULL\n");
                vipOnError(VIP_ERROR_FAILURE);
            }
        }
        vipOnError(nbglk_coeff_cpu_preload(network));
        #else
        vip_warning("pls enable vpmdPRELOAD_COEFF firstly\n");
        #endif
    }
    break;

    case VIP_NETWORK_PROP_SET_ENABLE_NPD:
    {
        vip_bool_e *enable_npd = (vip_bool_e*)value;
        vip_uint32_t nn_probe = viphal_query_hw_feature(VIP_HW_FEATURE_NN_PROBE);
        if (nn_probe) {
            if (*enable_npd == vip_true_e) {
                network->npd.enable_npd = vip_true_e;
                vip_info("Enable NPD function.\n");
            }
        }
        else {
            vip_error("error, not support probe mode performance counter\n");
            network->npd.enable_npd = vip_false_e;
        }
    }
    break;

    case VIP_NETWORK_PROP_SET_VIPSRAM_PRELOAD:
    {
        #if vpmdPRELOAD_COEFF > 1
        vip_bool_e* enable_preload = (vip_bool_e*)value;
        if (*enable_preload) {
            vipOnError(nbglk_coeff_npu_preload_init(network));
            vip_info("Enable perload vip-sram function.\n");
        }
        else {
            nbglk_coeff_npu_preload_uninit(network);
        }
        #else
        vip_warning("pls enable vpmdPRELOAD_COEFF=2 firstly\n");
        #endif
    }
    break;

    /* for debugging */
    case 1024:
    {
        nbglk_op_id_segment_t* op_segm = (nbglk_op_id_segment_t*)value;
        network->op_segm.start_id = op_segm->start_id;
        network->op_segm.end_id = op_segm->end_id;
        vip_debug("set run abs op id [%d ~ %d]\n", op_segm->start_id, op_segm->end_id);
        #if !(vpmdHARDWARE_BYPASS_MODE & 0x1000)
        vip_error("fail set operation segment, not enable\n");
        status = VIP_ERROR_FAILURE;
        #endif
    }
    break;

    case VIP_NETWORK_PROP_SET_LAYER_DUMP_ID:
    {
        vip_nld_layer_id_t *dump_id = (vip_nld_layer_id_t*)value;
        vipOnError(nbglk_nld_init(network, dump_id->layer_id, dump_id->layer_count));
    }
    break;

    default:
        vip_error("not implement this case property=%d\n", property);
        break;
    }

    return status;

onError:
    vip_error("fail to set network=%p property=%d, value=%p\n",
                 network, property, value);
    return status;
}

vip_status_e nbglk_query_network(
    IN vip_network_t *network,
    IN vip_enum property,
    OUT void *value
    )
{
    vip_status_e status = VIP_SUCCESS;
    vipIsNULL(value);

    switch (property) {
    case VIP_NETWORK_PROP_LAYER_COUNT:
        *((vip_uint32_t*)value) = network->nbg_desc.n_orig_layers;
        break;

    case VIP_NETWORK_PROP_INPUT_COUNT:
        *((vip_uint32_t *)value) = network->nbg_desc.n_inputs;
        break;

    case VIP_NETWORK_PROP_OUTPUT_COUNT:
        *((vip_uint32_t *)value) = network->nbg_desc.n_outputs;
        break;

    case VIP_NETWORK_PROP_NETWORK_NAME:
        viphal_copy_memory(value, network->nbg_desc.net_info.name,
                           sizeof(network->nbg_desc.net_info.name));
        break;
    case VIP_NETWORK_PROP_MEMORY_POOL_SIZE:
        *((vip_uint32_t *)value) = VIP_ALIGN(network->nbg_desc.mem_info.pool_size, vipdMEMORY_ALIGN_SIZE);
        break;
    case VIP_NETWORK_PROP_CORE_COUNT:
        *((vip_uint8_t *)value) = network->nbg_desc.net_info.core_count;
        break;
#if vpmdENABLE_APP_PROFILING
    case VIP_NETWORK_PROP_PROFILING:
        status = nbglk_get_profiling(network, value);
        if (status != VIP_SUCCESS) {
            vip_error("fail to get profiling data\n");
        }
    break;
#endif

#if vpmdENABLE_USE_AGENT_TRIGGER
    case VIP_NETWORK_PROP_ADDRESS_INFO:
        if (network->prepared_network) {
            status = nbglk_get_network_address_info(network, value);
        }
        else {
            vip_error("fail to query address, network not prepare\n");
            vipOnError(VIP_ERROR_FAILURE);
        }
        break;
#endif

/* TBD */
#if 0
    case VIP_NETWORK_PROP_LAYER_DUMP_OUTPUT_COUNT:
    {
        *((vip_uint32_t*)value) = nbglk_nld_output_count(network, value);
    }
    break;

    case VIP_NETWORK_PROP_LAYER_DUMP_LAYER_COUNT:
    {
        *((vip_uint32_t*)value) = nbglk_nld_layer_count(network);
    }
    break;

    case VIP_NETWORK_PROP_LAYER_DUMP_LAYER_ID:
    {
        status = nbglk_nld_layer_id(network, value);
        if (status != VIP_SUCCESS) {
            vip_error("query network fail for property=%d\n", property);
        }
    }
    break;

    case VIP_NETWORK_PROP_LAYER_DUMP_OUTPUT_INFO:
    {
        status = nbglk_nld_output_info(network, value);
        if (status != VIP_SUCCESS) {
            vip_error("query network fail for property=%d\n", property);
        }
    }
    break;

    case VIP_NETWORK_PROP_LAYER_DUMP_OUTPUT_MEMORY:
    {
        status = nbglk_nld_output_memory(network, value);
        if (status != VIP_SUCCESS) {
            vip_error("query network fail for property=%d\n", property);
        }
    }
    break;
#endif

    case VIP_NETWORK_PROP_GET_LAYER_DUMP_OUTPUT:
    {
        vip_nld_output_t *nld_output = (vip_nld_output_t*)value;
        vipOnError(nbglk_nld_get_output(network, &nld_output->count, &nld_output->info));
    }
    break;

    default:
        vip_error("query network not support this property=%d\n", property);
        status = VIP_ERROR_INVALID_ARGUMENTS;
        break;
    }

    return status;

onError:
    vip_error("fail to query network, value=%p, property=%d\n", value, property);
    return status;
}

/*
 * @brief
 * Generate the executable command buffer from the given network object (loaded from NBG file).
 * It should create the pool, load the data, and patch the necessary addresses used by commands.
 */
vip_status_e nbglk_prepare_network(
    vip_network_t *network
    )
{
    vip_status_e status = VIP_SUCCESS;
    vip_uint32_t i = 0;
    vip_uint32_t device_index = 0;
    nbglk_videomemory_t *allocation = VIP_NULL;
    nbglk_context_t *context = network->context;
    NBGLK_CHECK_NETWORK_DYNA_IS_NULL(network);

    device_index = network->device->dev_index;

    if (vip_true_e == network->prepared_network) {
        vip_error("network has been prepared....\n");
        vipGoOnError(VIP_ERROR_NOT_SUPPORTED);
    }
    if (network->segment_num < 1) {
        vip_error("fail the number of segment is %d\n",network->segment_num);
        vipGoOnError(VIP_ERROR_NOT_SUPPORTED);
    }
    if (device_index >= context->device_count) {
        vip_error("fail prepare network device%d, device cnt=%d\n",
            device_index, context->device_count);
        vipGoOnError(VIP_ERROR_NOT_SUPPORTED);
    }
    if (network == network->friend_network) {
        vip_error("fail network=%p equal to friend network=%p\n", network, network->friend_network);
        vipGoOnError(VIP_ERROR_NOT_SUPPORTED);
    }
    /* check axi sram size */
    if (network->nbg_desc.mem_info.axi_sram_size != context->axi_sram_size[device_index]) {
        vip_debug("binary axi-sram size=0x%08X, context=0x%08X, device%d\n",
         network->nbg_desc.mem_info.axi_sram_size, context->axi_sram_size[device_index], device_index);
        if (network->nbg_desc.mem_info.axi_sram_size > context->axi_sram_size[device_index]) {
            vip_error("binary axi-sram size=0x%08X, context=0x%08X\n",
                      network->nbg_desc.mem_info.axi_sram_size, context->axi_sram_size[device_index]);
            vipGoOnError(VIP_ERROR_INVALID_ARGUMENTS);
        }
    }
    if ((network->nbg_desc.net_info.core_count > 0) &&
        (network->nbg_desc.net_info.core_count > network->device->core_count)) {
        vip_error("network need core number %d, more than selected device owned core number %d.\n",
                  network->nbg_desc.net_info.core_count, network->device->core_count);
        vipGoOnError(VIP_ERROR_NOT_SUPPORTED);
    }

    /* allocate memory for save nn profile data */
    if (vip_true_e == network->npd.enable_npd) {
        vipOnError(nbglk_npd_init(network));
    }

#if vpmdENABLE_MULTIPLE_VIP
    /* Allocate memory for multiple sync cmds */
    if (network->nbg_desc.net_info.core_count > 1) {
        vip_uint32_t core_id[vipdMAX_CORE] = { 0 };
        vip_uint32_t core_count = 0;
        vip_uint32_t size = 0;
        core_count = network->nbg_desc.net_info.core_count;
        for (i = 0; i < core_count; i++) {
            core_id[i] = network->device->core_id[i + network->core_index];
        }
        size = (network->nbg_desc.net_info.core_count - 1) * 8 + 10;
        size *= sizeof(vip_uint32_t);
        vipOnError(viphal_allocate_memory(size, (void**)&network->multivip_sync_cmd));
        viphal_zero_memory(network->multivip_sync_cmd, size);
        vipOnError(viphal_hardware_multivip_sync(network->multivip_sync_cmd, &network->sync_cmd_size,
                                               core_count, core_id));
        if (network->sync_cmd_size > size) {
            vip_error("fail to get mp sync cmds, command memory overflow\n");
            vipGoOnError(VIP_ERROR_OUT_OF_MEMORY);
        }
    }
#endif

#if vpmdDYNAMIC_CHANGE_MEM_POOL
    network->pool_patch_count = 0;
#endif

    /* 1. Create the pool if necessary. */
    if (network->nbg_desc.mem_info.pool_size > 0) {
        vip_size_t tmp_size = VIP_ALIGN(network->nbg_desc.mem_info.pool_size, vipdMEMORY_ALIGN_SIZE);
        network->pool.size = network->nbg_desc.mem_info.pool_size;
        if ((network->memory_pool != VIP_NULL) && (tmp_size <= network->memory_pool->memory.size)) {
            network->pool.logical = network->memory_pool->memory.logical;
            network->pool.mem_id = network->memory_pool->memory.mem_id;
            network->pool.physical = network->memory_pool->memory.physical;
            vip_debug("memory pool specify by app, physical=0x%llX, logical=0x%p, pool size=0x%08X\n",
                     network->pool.physical, network->pool.logical, network->memory_pool->memory.size);
        }
        else {
            vipOnError(viphal_allocate_videomemory(&network->pool.logical, &network->pool.mem_id,
                                                  &network->pool.physical, network->pool.size,
                                                  vipdMEMORY_ALIGN_SIZE,
                                                  VIPDRV_VIDEO_MEM_ALLOC_MMU_PAGE_WRITE));
            #if vpmdENABLE_VIDEO_MEMORY_CACHE
            vipOnError(viphal_flush_cache(network->pool.mem_id, VIPDRV_CACHE_CLEAN));
            #endif
            vip_debug("memory pool physical=0x%llx, logical=0x%p, pool size=0x%08X\n",
                       network->pool.physical, network->pool.logical, network->pool.size);
        }
    }

    for (i = 0; i < network->segment_num; i++) {
        vip_uint32_t states_size = 0;
        nbglk_segment_t *segment = &network->segments[i];
        segment->index = i;
        #if vpmdENABLE_RECORD_INFO
        vipOnError(nbglk_hashmap_init(&segment->record_list, HASH_MAP_CAPABILITY,
                                    sizeof(nbglk_record_info_t), "record-list"));
        #endif
        if (vip_false_e == segment->is_sw_segm) {
            if (vip_true_e == network->realloc_sh_buff) {
                /* 2. alloc memory */
                vip_uint32_t sh_cmd_size = get_sh_command_size(network, segment);
                if (sh_cmd_size > 0) {
                    allocation = &segment->sh_cmd_buffer;
                    vipOnError(viphal_allocate_videomemory((vip_uint8_t **)&allocation->logical,
                                                   &allocation->mem_id, &allocation->physical,
                                                   sh_cmd_size, vipdSH_ALIGN_SIZE,
                                                   VIPDRV_VIDEO_MEM_ALLOC_4GB_ADDR));
                    allocation->size = sh_cmd_size;
                }
            }

            states_size = nbglk_network_cmdbuf_size(network, segment);
            if (states_size > 0) {
                vip_uint32_t alloc_size = states_size + APPEND_COMMAND_SIZE;
                allocation = &segment->states_buffer;
                vipOnError(viphal_allocate_videomemory((vip_uint8_t **)&allocation->logical,
                                                    &allocation->mem_id, &allocation->physical,
                                                    alloc_size, vipdMEMORY_ALIGN_SIZE,
                                                    VIPDRV_VIDEO_MEM_ALLOC_MAP_KERNEL |
                                                    VIPDRV_VIDEO_MEM_ALLOC_4GB_ADDR));
                allocation->size = states_size;
                viphal_zero_memory(allocation->logical, alloc_size);
            }
            else {
                vip_error("fail states size is 0\n");
                vipGoOnError(VIP_ERROR_NOT_SUPPORTED);
            }

            vipOnError(viphal_task_set_property(network->task_id, VIPDRV_TASK_CMD_SUBTASK, i,
                                            nbglk_get_segment_handle(&network->segments[i]),
                                            segment->states_buffer.size, segment->patch_count));
            /* 3. Patch and generate command buffer. */
            status = nbglk_generate_command(network, segment);
            if (status != VIP_SUCCESS) {
                vip_error("fail to generate hardware commands, status=%d.\n", status);
                vipGoOnError(status);
            }
            vip_info("segment%d actual states size=0x%x\n", i, segment->command_size);
        }
        else {
            #if vpmdENABLE_SUPPORT_CPU_LAYER
            /* handle cpu operation */
            vip_debug("prepare cpu layer.. segment index=%d, num=%d\n", i, network->segment_num);
            status = nbglk_sw_prepare_layer(network, segment);
            if (status != VIP_SUCCESS) {
                vip_error("fail to prepare software operation, status=%d.\n", status);
                vipGoOnError(status);
            }
            #else
            vip_error("NOT support CPU layers, please set vpmdENABLE_SUPPORT_CPU_LAYER to 1\n");
            #endif
        }
    }
#if vpmdENABLE_FLUSH_CPU_CACHE
    status = flush_network_cache(network);
    if (status != VIP_SUCCESS) {
        vip_error("fail to flush network cache status=%d\n", status);
        vipGoOnError(status);
    }
#endif

    status = nbglk_prepare_init_command(network);
    if (status != VIP_SUCCESS) {
        vip_error("fail to prepare initialize command\n");
        vipGoOnError(status);
    }

#if vpmdENABLE_CANCELATION > 1
    vipOnError(nbglk_breakpoint_init(network));
#endif

#if vpmdENABLE_REDUCE_MEMORY
    free_dyna_data_two(network);
    network->nbg_desc.corrupted = vip_true_e;
    vip_info("free header reduce system mem size=%dbytes\n",
        sizeof(nbg_desc_sh_operation_t) * network->nbg_desc.n_sh_ops +
        sizeof(nbg_desc_patch_t) * network->nbg_desc.n_pd_sections +
        sizeof(nbg_desc_data_table_t) * network->nbg_desc.n_coeff_header +
        sizeof(nbg_desc_data_table_t) * network->nbg_desc.n_coeff_data +
        sizeof(nbg_desc_data_table_t) * network->nbg_desc.n_load_state +
        sizeof(nbg_desc_data_table_t) * network->nbg_desc.n_ppu_instr +
        sizeof(nbg_desc_data_table_t) * network->nbg_desc.n_dyna_io);
    if (network->nbg_desc.fixed.header.version >= 0x00010018) {
        if (network->load_states != VIP_NULL) {
            viphal_free_memory(network->load_states);
            network->load_states = VIP_NULL;
            vip_info("free LS reduce system mem size=%dbytes\n",
                     network->nbg_desc.fixed.load_states.size);
        }
    }
#endif

#if vpmdENABLE_MULTIPLE_VIP
    if (network->multivip_sync_cmd != VIP_NULL) {
        viphal_free_memory(network->multivip_sync_cmd);
        network->multivip_sync_cmd = VIP_NULL;
        network->sync_cmd_size = 0;
    }
#endif

    network->prepared_network = vip_true_e;
#if vpmdENABLE_CHECKSUM
    for (i = 0; i < NBGLK_CHECKSUM_MAX; i++) {
        network->checksum[i] = 0;
    }
    nbglk_network_checksum(network, NBGLK_CHECKSUM_MAX);
    vipOnError(nbglk_section_checksum(network));
#endif

    if (network->prepare_count >= 255) {
        network->prepare_count = 0;
    }
    network->prepare_count++;
    network->belong_to_group = vip_false_e;

    vip_info("prepare network=%p done, alloc_sh_buff=%d, prepare_count=%d\n",
                network, network->realloc_sh_buff, network->prepare_count);

    return status;

onError:
    vip_error("fail to prepare network status=%d\n", status);
    nbglk_finish_network(network);

    return status;
}

vip_status_e nbglk_prepare_network_from_friend(
    vip_network_t *network
    )
{
    vip_status_e status = VIP_SUCCESS;
    nbg_desc_patch_t *pd_entry = VIP_NULL;
    void *pd_ptr = VIP_NULL;
    nbg_desc_operation_t *operation = VIP_NULL;
    vip_uint8_t *op_tp = VIP_NULL;
    vip_uint8_t *op_nn = VIP_NULL;
    nbg_desc_data_table_t *data_table = VIP_NULL;
    vip_uint8_t *states_buffer = VIP_NULL;
    vip_uint32_t *trigger_word = VIP_NULL;
    vip_uint32_t i = 0;
    vip_uint32_t j = 0;
    vip_uint32_t k = 0;
    vip_address_t cmd_physical = 0;
    vip_uint32_t states_index = 0;
    vip_network friend_network = network->friend_network;
    vip_uint32_t states_size = 0;
    vip_bool_e with_event = !vpmdENABLE_POLLING;
#if vpmdENABLE_CANCELATION > 1
    vip_uint8_t *states_base = VIP_NULL;
    vip_uint32_t bp_index = 0;
#endif

#if vpmdENABLE_MULTIPLE_VIP
    /* Allocate memory for multiple sync cmds */
    if (network->nbg_desc.net_info.core_count > 1) {
        vip_uint32_t core_id[vipdMAX_CORE] = { 0 };
        vip_uint32_t core_count = network->nbg_desc.net_info.core_count;
        vip_uint32_t size = 0;
        for (i = 0; i < core_count; i++) {
            core_id[i] = network->device->core_id[i + network->core_index];
        }
        size = (network->nbg_desc.net_info.core_count - 1) * 8 + 10;
        size *= sizeof(vip_uint32_t);
        vipOnError(viphal_allocate_memory(size, (void**)&network->multivip_sync_cmd));
        viphal_zero_memory(network->multivip_sync_cmd, size);
        vipOnError(viphal_hardware_multivip_sync(network->multivip_sync_cmd,
                                               &network->sync_cmd_size,
                                               core_count, core_id));
        if (network->sync_cmd_size > size) {
            vip_error("fail to get mp sync cmds, command memory overflow\n");
            vipGoOnError(VIP_ERROR_OUT_OF_MEMORY);
        }
    }
#endif

#if vpmdDYNAMIC_CHANGE_MEM_POOL
    network->pool_patch_count = 0;
#endif

    if (0 < network->nbg_desc.mem_info.pool_size) {
        vip_uint32_t tmp_size = 0;
        tmp_size = VIP_ALIGN(network->nbg_desc.mem_info.pool_size, vipdMEMORY_ALIGN_SIZE);
        if (0 == network->pool.mem_id && VIP_NULL == network->memory_pool) {
            vip_error("pls indicate memory pool for dup network\n");
            vipGoOnError(VIP_ERROR_FAILURE);
        }
        if (VIP_NULL != network->memory_pool) {
            if (tmp_size <= network->memory_pool->memory.size) {
                network->pool.logical = network->memory_pool->memory.logical;
                network->pool.mem_id = network->memory_pool->memory.mem_id;
                network->pool.physical = network->memory_pool->memory.physical;
                network->pool.size = network->nbg_desc.mem_info.pool_size;
            }
            else {
                vip_error("memory pool is not enough for dup network\n");
                vipGoOnError(VIP_ERROR_FAILURE);
            }
        }
        if (network->pool.mem_id == friend_network->pool.mem_id) {
            vip_warning("origin network share memory pool with dup network\n");
        }
    }

    for (i = 0; i < network->segment_num; i ++) {
        const nbglk_videomemory_t *orig_alloc = &friend_network->segments[i].states_buffer;
        nbglk_videomemory_t *copy_alloc = &network->segments[i].states_buffer;
        nbglk_segment_t *segment = &network->segments[i];
        if (VIP_NULL == segment) {
            vip_error("fail segment %s is NULL\n", i);
            continue;
        }
        segment->index = i;
        vipOnError(viphal_allocate_videomemory((vip_uint8_t **)&copy_alloc->logical,
                                    &copy_alloc->mem_id,
                                    &copy_alloc->physical, network->segments[i].command_size,
                                    vipdMEMORY_ALIGN_SIZE,
                                    VIPDRV_VIDEO_MEM_ALLOC_MAP_KERNEL |
                                    VIPDRV_VIDEO_MEM_ALLOC_4GB_ADDR));
        viphal_copy_memory(copy_alloc->logical, orig_alloc->logical, network->segments[i].command_size);

        /* insert link command */
        if (network->segments[i].split_num > 0) {
            vip_info("dup network insert link split num=%d\n", network->segments[i].split_num);
            status = nbglk_insert_link(&network->segments[i]);
            if (status != VIP_SUCCESS) {
                vip_error("fail to patch link cmd in dup network command buffer\n");
                vipGoOnError(status);
            }
        }

        if (VIP_NULL == segment->states_buffer.logical) {
            vip_error("states buffer logical is NULL");
            vipGoOnError(VIP_ERROR_INVALID_NETWORK);
        }
        #if vpmdENABLE_CANCELATION > 1
        states_base = (vip_uint8_t *)segment->states_buffer.logical;
        #endif
        states_buffer = (vip_uint8_t *)segment->states_buffer.logical + CACHE_CMD_SIZE;

        if (VIP_NULL == states_buffer) {
            vip_error("states buffer is NULL");
            vipGoOnError(VIP_ERROR_INVALID_NETWORK);
        }
        vipOnError(viphal_task_set_property(network->task_id, VIPDRV_TASK_CMD_SUBTASK, i,
            nbglk_get_segment_handle(&network->segments[i]), segment->states_buffer.size, segment->patch_count));
        segment->patch_count = 0;

        if (vip_false_e == segment->is_sw_segm) {
            for (j = 0; j < network->nbg_desc.n_operations; j++) {
                operation = &network->nbg_desc.operations[j];

                CHECK_OPERATION_IN_SEGMENT(operation, segment, j);
                data_table = &network->nbg_desc.load_states_table[operation->state_id];
                states_size = SIZE_CAST(vip_uint32_t, data_table->size);

                switch (operation->type) {
                case NBG_OPERATION_TYPE_NN:
                case NBG_OPERATION_TYPE_SP:
                case NBG_OPERATION_TYPE_NN_LUT:
                case NBG_OPERATION_TYPE_NT:
                {
                    vip_uint32_t event_id = 0;
                    op_nn = network->nnCmds.logical +
                        nbg_get_nn_command_size(&network->nbg_desc) * operation->index;
                    cmd_physical = network->nnCmds.physical +
                                   (vip_uint32_t)(op_nn - network->nnCmds.logical);
                    for (k = 0; k < operation->patch_count; k++) {
                        pd_ptr = &network->nbg_desc.pd_sections[operation->patch_index + k];
                        pd_entry = (nbg_desc_patch_t*)pd_ptr;
                        if (NBG_PATCH_TYPE_STATE == pd_entry->type) {
                            if (NBG_SOURCE_COMMAND == pd_entry->source_type) {
                                vip_uint8_t *tmp = states_buffer + pd_entry->dest_offset;
                                trigger_word = (vip_uint32_t *)tmp;
                                event_id = *trigger_word & 0x3f;
                                *trigger_word = event_id | (cmd_physical & ~0x3f);
                            }
                            #if vpmdENABLE_MULTIPLE_VIP
                            else if (NBG_SOURCE_MULTIVIP_CHIPID == pd_entry->source_type) {
                                vipOnError(patch_multivip_chipid(network, segment,
                                                states_buffer, pd_entry));
                            }
                            else if (NBG_SOURCE_MULTIVIP_SYNC_CMDS == pd_entry->source_type) {
                                vipOnError(patch_multivip_sync_cmd(network, segment, states_buffer, pd_entry));
                            }
                            #endif
                        }
                        else if (NBG_PATCH_TYPE_COMMAND == pd_entry->type) {
                            if (NBG_SOURCE_MEMORY_POOL == pd_entry->source_type) {
                                vipOnError(patch_source_mempool(network, op_nn, pd_entry));
                            }
                            else if (NBG_SOURCE_AXI_SRAM == pd_entry->source_type) {
                                vipOnError(patch_source_sram(network, op_nn, pd_entry,
                                                NBG_SOURCE_AXI_SRAM));
                            }
                            else if (NBG_SOURCE_MISC_DYNA_INPUT == pd_entry->source_type ||
                                     NBG_SOURCE_MISC_DYNA_OUTPUT == pd_entry->source_type) {
                                vipOnError(patch_source_misc_data(network, segment,
                                                operation, op_nn, pd_entry, j));
                            }
                        }
                    }
                    #if (vpmdHARDWARE_BYPASS_MODE & 0x1)
                    APPEND_NOP_CMDS(states_buffer, states_size);
                    #elif (vpmdHARDWARE_BYPASS_MODE & 0x1000)
                    CHECK_OPERATION_BYPASS(states_buffer, states_size);
                    #endif
                    states_buffer += states_size;
                    APPEND_STALL_FE_CMDS(states_buffer, event_id);
                    APPEND_QUICKRST_CMDS(states_buffer, event_id);
                    APPEND_BREAKPOINT_CMDS(states_buffer);
                }
                break;

                case NBG_OPERATION_TYPE_TP:
                {
                    op_tp = network->tpCmds.logical +
                        nbg_get_tp_command_size(&network->nbg_desc) * operation->index;
                    cmd_physical = network->tpCmds.physical +
                                   (vip_uint32_t)(op_tp - network->tpCmds.logical);
                    for (k = 0; k < operation->patch_count; k++) {
                        pd_ptr = &network->nbg_desc.pd_sections[operation->patch_index + k];
                        pd_entry = (nbg_desc_patch_t*)pd_ptr;
                        if (NBG_PATCH_TYPE_STATE == pd_entry->type) {
                            if (NBG_SOURCE_COMMAND == pd_entry->source_type) {
                                vip_uint8_t *tmp = states_buffer + pd_entry->dest_offset;
                                trigger_word = (vip_uint32_t *)tmp;
                                *trigger_word = ((*trigger_word) & 0x3f) | (cmd_physical & ~0x3f);
                            }
                            #if vpmdENABLE_MULTIPLE_VIP
                            else if (NBG_SOURCE_MULTIVIP_CHIPID == pd_entry->source_type) {
                                vipOnError(patch_multivip_chipid(network, segment, states_buffer, pd_entry));
                            }
                            else if (NBG_SOURCE_MULTIVIP_SYNC_CMDS == pd_entry->source_type) {
                                vipOnError(patch_multivip_sync_cmd(network, segment, states_buffer, pd_entry));
                            }
                            #endif
                        }
                        else if (NBG_PATCH_TYPE_COMMAND == pd_entry->type) {
                            if (NBG_SOURCE_MEMORY_POOL == pd_entry->source_type) {
                                vipOnError(patch_source_mempool(network, op_tp, pd_entry));
                            }
                            else if (NBG_SOURCE_AXI_SRAM == pd_entry->source_type) {
                                vipOnError(patch_source_sram(network, op_tp, pd_entry,
                                            NBG_SOURCE_AXI_SRAM));
                            }
                            else if (NBG_SOURCE_MISC_DYNA_INPUT == pd_entry->source_type ||
                                     NBG_SOURCE_MISC_DYNA_OUTPUT == pd_entry->source_type) {
                                vipOnError(patch_source_misc_data(network, segment,
                                            operation, op_tp, pd_entry, j));
                            }
                        }
                    }
                    #if (vpmdHARDWARE_BYPASS_MODE & 0x2)
                    APPEND_NOP_CMDS(states_buffer, states_size);
                    #elif (vpmdHARDWARE_BYPASS_MODE & 0x1000)
                    CHECK_OPERATION_BYPASS(states_buffer, states_size);
                    #endif
                    states_buffer += states_size;
                    APPEND_BREAKPOINT_CMDS(states_buffer);
                }
                break;

                case NBG_OPERATION_TYPE_SH:
                {
                    for (k = 0; k < operation->patch_count; k++) {
                        pd_ptr = &network->nbg_desc.pd_sections[operation->patch_index + k];
                        pd_entry = (nbg_desc_patch_t*)pd_ptr;
                        if (NBG_SOURCE_MEMORY_POOL == pd_entry->source_type) {
                            vipOnError(patch_source_mempool(network, states_buffer, pd_entry));
                        }
                        #if vpmdENABLE_MULTIPLE_VIP
                        else if (NBG_SOURCE_MULTIVIP_CHIPID == pd_entry->source_type) {
                            vipOnError(patch_multivip_chipid(network, segment, states_buffer, pd_entry));
                        }
                        else if (NBG_SOURCE_MULTIVIP_SYNC_CMDS == pd_entry->source_type) {
                            vipOnError(patch_multivip_sync_cmd(network, segment, states_buffer, pd_entry));
                        }
                        #endif
                        else if (NBG_SOURCE_AXI_SRAM == pd_entry->source_type) {
                            vipOnError(patch_source_sram(network, states_buffer, pd_entry,
                                            NBG_SOURCE_AXI_SRAM));
                        }
                        else if (NBG_SOURCE_MISC_DYNA_INPUT == pd_entry->source_type ||
                                 NBG_SOURCE_MISC_DYNA_OUTPUT == pd_entry->source_type) {
                            vipOnError(patch_source_misc_data(network, segment, operation,
                                                states_buffer, pd_entry, j));
                        }
                    }
                    #if (vpmdHARDWARE_BYPASS_MODE & 0x4)
                    APPEND_NOP_CMDS(states_buffer, states_size);
                    #elif (vpmdHARDWARE_BYPASS_MODE & 0x1000)
                    CHECK_OPERATION_BYPASS(states_buffer, states_size);
                    #endif
                    states_buffer += states_size;
                    APPEND_STALL_FE_CMDS(states_buffer, 0);
                    APPEND_BREAKPOINT_CMDS(states_buffer);
                }
                break;

                default:
                    break;
                }

                /* reserved LINK bits in states buffer for patch it in submit function */
                if ((segment->operation_pos != VIP_NULL) &&
                    (j == segment->operation_pos[states_index]) && (j != 0)) {
                    states_buffer += LINK_SIZE;
                    states_index++;
                }
            }
        }
        else {
            vip_error("fail to dup network, have SW operation in network, not support\n");
            vipGoOnError(VIP_ERROR_FAILURE);
        }

        /* append commands at the end of command buffer */
        segment->command_size = SIZE_CAST(vip_uint32_t, segment->states_buffer.size);
        status = nbglk_append_cmds(&segment->states_buffer, &segment->command_size, with_event,
                  network->nbg_desc.net_info.core_count > 1 ? vip_true_e : vip_false_e,
                  network->device->core_id[network->core_index]);
        if (status != VIP_SUCCESS) {
            vip_error("fail to append commands\n");
            vipGoOnError(status);
        }
    }

    /* patch network input */
    for (i = 0; i < network->nbg_desc.n_inputs; i ++) {
        const nbglk_io_patch_info_t *orig_io = &friend_network->network_inputs[i];
        nbglk_io_patch_info_t *copy_io = &network->network_inputs[i];
        for (j = 0; j < orig_io->slice_num; j ++) {
            const vip_address_t physical = orig_io->physical_in_cmd[j];
            vip_address_t orig_base = 0;
            vip_address_t offset = 0;
            const nbglk_videomemory_t *copy_mem = VIP_NULL;
            if ((physical >= friend_network->nnCmds.physical) &&
               (physical < (friend_network->nnCmds.physical + friend_network->nnCmds.size))) {
                orig_base = friend_network->nnCmds.physical;
                copy_mem = &network->nnCmds;
            }
            else if ((physical >= friend_network->tpCmds.physical) &&
                (physical < (friend_network->tpCmds.physical + friend_network->tpCmds.size))) {
                orig_base = friend_network->tpCmds.physical;
                copy_mem = &network->tpCmds;
            }
            else {
                for (k = 0; k < friend_network->segment_num; k ++) {
                    nbglk_videomemory_t *buffer = &friend_network->segments[k].states_buffer;
                    if ((physical >= buffer->physical) &&
                        (physical < (buffer->physical + buffer->size))) {
                        orig_base = buffer->physical;
                        copy_mem = &network->segments[k].states_buffer;
                    }
                }
            }
            if(physical < orig_base) {
                vip_error("Invalid physical=%#x, base=%#x.\n", physical, orig_base);
                vipGoOnError(VIP_ERROR_FAILURE);
            }
            if (VIP_NULL == copy_mem) {
                vip_error("fail not get valid command memory for input%d\n", i);
                vipGoOnError(VIP_ERROR_FAILURE);
            }
            offset = physical - orig_base;
            copy_io->physical_in_cmd[j] = SIZE_CAST(vip_uint32_t, copy_mem->physical + offset);
            copy_io->logical_in_cmd[j] = VIPPTR_TO_UINT64(copy_mem->logical + offset);
        }
    }

    /* patch network output */
    for (i = 0; i < network->nbg_desc.n_outputs; i ++) {
        const nbglk_io_patch_info_t *orig_io = &friend_network->network_outputs[i];
        nbglk_io_patch_info_t *copy_io = &network->network_outputs[i];

        for (j = 0; j < orig_io->slice_num; j ++) {
            const vip_address_t physical = orig_io->physical_in_cmd[j];
            vip_address_t orig_base = 0;
            vip_address_t offset = 0;
            const nbglk_videomemory_t *copy_mem = VIP_NULL;
            if ((physical >= friend_network->nnCmds.physical) &&
              (physical < (friend_network->nnCmds.physical + friend_network->nnCmds.size))) {
                orig_base = friend_network->nnCmds.physical;
                copy_mem = &network->nnCmds;
            }
            else if ((physical >= friend_network->tpCmds.physical) &&
              (physical < (friend_network->tpCmds.physical + friend_network->tpCmds.size))) {
                orig_base = friend_network->tpCmds.physical;
                copy_mem = &network->tpCmds;
            }
            else {
                for (k = 0; k < friend_network->segment_num; k ++) {
                    nbglk_videomemory_t *buffer = &friend_network->segments[k].states_buffer;
                    if ((physical >= buffer->physical) &&
                        (physical < (buffer->physical + buffer->size))) {
                        orig_base = buffer->physical;
                        copy_mem = &network->segments[k].states_buffer;
                    }
                }
            }
            if(physical < orig_base) {
                vip_error("Invalid physical=%#x, base=%#x.\n", physical, orig_base);
                vipGoOnError(VIP_ERROR_FAILURE);
            }
            if (VIP_NULL == copy_mem) {
                vip_error("fail not get valid command memory for output%d\n", i);
                vipGoOnError(VIP_ERROR_FAILURE);
            }
            offset = physical - orig_base;
            copy_io->physical_in_cmd[j] = SIZE_CAST(vip_uint32_t, copy_mem->physical + offset);
            copy_io->logical_in_cmd[j] = VIPPTR_TO_UINT64(copy_mem->logical + offset);
        }
    }

#if vpmdENABLE_FLUSH_CPU_CACHE
    status = flush_network_cache(network);
    if (status != VIP_SUCCESS) {
        vip_error("fail to flush weak dup network cache status=%d\n", status);
        vipGoOnError(status);
    }
#endif

#if vpmdENABLE_MULTIPLE_VIP
    if (network->multivip_sync_cmd != VIP_NULL) {
        viphal_free_memory(network->multivip_sync_cmd);
        network->multivip_sync_cmd = VIP_NULL;
        network->sync_cmd_size = 0;
    }
#endif

    network->prepared_network = vip_true_e;
#if vpmdENABLE_CHECKSUM
    for (i = 0; i < NBGLK_CHECKSUM_MAX; i++) {
        network->checksum[i] = 0;
    }
    nbglk_network_checksum(network, NBGLK_CHECKSUM_MAX);
#endif
    if (network->prepare_count >= 255) {
        network->prepare_count = 0;
    }
    network->prepare_count++;
    return status;

onError:
    vip_error("fail to prepare dup network status=%d\n", status);
    nbglk_finish_network(network);

    return status;
}

static vip_status_e nbglk_commit_network(
    IN vip_network_t *network
    )
{
    vip_status_e status = VIP_SUCCESS;
    vip_uint32_t i = 0;
    vip_uint32_t j = 0;
    nbglk_segment_t *segment = VIP_NULL;

    /* Error if network is not well prepared. */
    if (network->prepared_network == vip_false_e) {
        vip_error("network=%p doesn't initialize\n", network);
        vipGoOnError(VIP_ERROR_NETWORK_NOT_PREPARED);
    }
    if (vip_true_e == network->belong_to_group) {
        vip_error("network=%p is belong to one group\n", network);
        vipGoOnError(VIP_ERROR_NETWORK_NOT_PREPARED);
    }

    /* update and check network's input and output */
    vipOnError(nbglk_update_network_inout(network));

#if vpmdENABLE_CAPTURE
    nbglk_capture_network(network);
#endif
    /* submit commands */
    while (i < network->segment_num) {
        segment = &network->segments[i];
        /* do sw segment */
        if (vip_true_e == segment->is_sw_segm) {
            #if vpmdENABLE_SUPPORT_CPU_LAYER
            vip_debug("do sw operation...\n");
            vipOnError(nbglk_sw_do_layer(network, segment));
            #else
            vip_error("NOT support CPU layers, please set vpmdENABLE_SUPPORT_CPU_LAYER to 1\n");
            #endif

            i++;
            continue;
        }

        /* 1. start profiling*/
        #if vpmdENABLE_BW_PROFILING
        nbglk_start_profiling(network, segment);
        #endif

        /* 2. commit vip segment(s) */
        while (i < network->segment_num) {
            segment = &network->segments[i];
            if (vip_true_e == segment->is_sw_segm) {
                break;
            }

            #if vpmdENABLE_CNN_PROFILING
            if (network->context->func_config.enable_cnn_profile == vip_true_e) {
                nbglk_start_segment_profile(network, segment);
            }
            #endif

            status = nbglk_network_segment_submit(network, segment);
            if (status != VIP_SUCCESS) {
                vip_error("fail to submit network segment%d\n", i);
                vipGoOnError(status);
            }

            #if vpmdENABLE_CAPTURE
            nbglk_capture_dump_states(network->context->capture_file, network, segment);
            #endif

            i++;
            #if vpmdENABLE_CNN_PROFILING
            break;
            #else
            if (i >= network->segment_num) {
                /* wait segments finish in nbglk_wait_network */
                goto onError;
            }
            #endif
        }

        /* 3. wait previous vip segment(s) finish */
        while (j < i) {
            if (vip_false_e == network->segments[j].is_sw_segm) {
                status = nbglk_network_segment_wait(network, &network->segments[j]);
                if ((status != VIP_SUCCESS) && (status != VIP_ERROR_CANCELED)) {
                    vip_error("fail to wait network finish segment%d\n", j);
                    vipGoOnError(status);
                }
                #if vpmdENABLE_CNN_PROFILING
                if (network->context->func_config.enable_cnn_profile == vip_true_e) {
                    nbglk_end_segment_profile(network, &network->segments[j]);
                }
                #endif
            }
            j++;
        }

        /* 4. end profiling */
        #if vpmdENABLE_BW_PROFILING
        nbglk_end_profiling(network, segment);
        #endif
    }

#if vpmdENABLE_CNN_PROFILING
    if (network->context->func_config.enable_cnn_profile == vip_true_e) {
        vip_print("=== network process total time=%lld us or %lld ms===\n",
            network->total_time, network->total_time / 1000);
    }
#endif

onError:
#if vpmdENABLE_CHECKSUM && (vpmdENABLE_DEBUG_LOG == 4)
    if (VIP_SUCCESS == status) {
        status = nbglk_network_checksum(network, NBGLK_CHECKSUM_MAX);
        vip_info("commit network checksum status=%d\n", status);
    }
#endif

    return status;
}

vip_status_e nbglk_trigger_network(
    IN vip_network_t *network
    )
{
    vip_status_e status = VIP_SUCCESS;

    status = nbglk_commit_network(network);
    if (status != VIP_SUCCESS) {
        vip_error("fail to commit network status=%d\n", status);
        vipGoOnError(status);
    }

onError:
    return status;
}

/* Waiting for commands to be done. */
vip_status_e nbglk_wait_network(
    IN vip_network network
    )
{
    vip_status_e status = VIP_SUCCESS;
    vip_int32_t i = 0;
    nbglk_segment_t *segment = &network->segments[0];

    for (i = (vip_int32_t)(network->segment_num - 1); i >= 0; i--) {
        segment = &network->segments[i];
        vipIsNULL(segment);
        if ((segment != VIP_NULL) && (segment->is_sw_segm == vip_true_e)) {
            break;
        }
    }

    {
    vip_int32_t final_sw_segment = i;
    /* wait network done */
    #if vpmdENABLE_CNN_PROFILING
    if (network->context->func_config.enable_cnn_profile == vip_false_e) {
    #endif
        for (i = final_sw_segment + 1; i < (vip_int32_t)(network->segment_num); i++) {
            segment = &network->segments[i];
            status = nbglk_network_segment_wait(network, segment);
            if ((status != VIP_SUCCESS) && (status != VIP_ERROR_CANCELED)) {
                vip_error("fail to wait network %s finish\n", network->nbg_desc.net_info.name);
                vipGoOnError(status);
            }
        }
        #if vpmdENABLE_BW_PROFILING
        nbglk_end_profiling(network, segment);
        #endif
    #if vpmdENABLE_CNN_PROFILING
    }
    #endif
    }

#if (vpmdENABLE_CAPTURE > 1)
    nbglk_capture_network_output(network->context->capture_file, network);
#endif

#if vpmdDUMP_NBG_RESOURCE
    nbglk_dump_network_outputs(network);
#endif

    if (network->npd.enable_npd) {
        nbglk_npd_save_file(network);
    }

    return status;
onError:
    nbglk_dump_nbg_entry(network);
    return status;
}

vip_status_e nbglk_run_network(
    IN vip_network_t *network
    )
{
    vip_status_e status = VIP_SUCCESS;

    status = nbglk_commit_network(network);
    if (status != VIP_SUCCESS) {
        vip_error("fail to commit network status=%d\n", status);
        vipGoOnError(status);
    }

    status = nbglk_wait_network(network);
    if ((status != VIP_SUCCESS) && (status != VIP_ERROR_CANCELED)) {
        vip_error("fail to wait network finish in run network status=%d\n", status);
        vipGoOnError(status);
    }

onError:
    return status;
}

#if vpmdENABLE_CANCELATION
static vip_status_e nbglk_hw_cancel_network(
    IN vip_network network
    )
{
    vip_status_e status = VIP_SUCCESS;
    nbglk_segment_t *segment = VIP_NULL;
    vip_uint32_t i = 0;

    for (i = 0; i < network->segment_num; i++) {
        segment = &network->segments[i];
        if ((segment != VIP_NULL) && (segment->is_sw_segm == vip_false_e)) {
            status = viphal_cancel_task(network->task_id, segment->index);
            if (status != VIP_SUCCESS) {
                vip_error("fail to hw cancel network segment[%d] status=%d.\n", i, status);
                vipGoOnError(status);
            }
        }
        else {
            vip_error("fail to hw cancel, segment[%d] is NULL or unsupport cancel cpu opr.\n", i);
            vipGoOnError(VIP_ERROR_FAILURE);
        }
    }

onError:
    return status;
}

#if vpmdENABLE_CANCELATION > 1
static vip_status_e nbglk_sw_cancel_network(
    IN vip_network network
    )
{
    vip_status_e status = VIP_SUCCESS;
    vip_bool_e support_cancel = vip_false_e;

#if vpmdENABLE_MULTIPLE_TASK
    vip_uint32_t address = 0;
    vip_uint32_t i = 0;
    viphal_read_reg(network->device->dev_index, network->core_index, 0x00664, &address);
    for (i = 0; i < network->segment_num; i++) {
        nbglk_segment_t *segment = &network->segments[i];
        vip_int32_t size = segment->states_buffer.size - CACHE_CMD_SIZE - APPEND_COMMAND_SIZE;
        vip_error("fe fetch adderss=0x%08x, cmd phy=0x%llx, size=0x%x\n", address,
                    segment->states_buffer.physical, segment->states_buffer.size);
        if ((address >= segment->states_buffer.physical) &&
            (address < segment->states_buffer.physical + size)) {
                support_cancel = vip_true_e;
                break;
        }
    }
#else
    support_cancel = vip_true_e;
#endif
    if (vip_true_e == support_cancel) {
        status = nbglk_breakpoint_change_p(network);
        if (status != VIP_SUCCESS) {
            vip_error("fail to change breakpoint to end engine\n");
            vipGoOnError(status);
        }
    }

    status = viphal_cancel_task(network->task_id, 0);

    if (VIP_SUCCESS != status) {
        vip_error("failed do sw cancel network on device%d.\n", network->device->dev_index);
    }

    if (vip_true_e == support_cancel) {
        status = nbglk_breakpoint_change_n(network);
        if (status != VIP_SUCCESS) {
            vip_error("fail to change breakpoint to end engine\n");
            vipGoOnError(status);
        }
    }

    return status;

onError:
    return status;
}
#endif

/* Cancle network to be run. */
vip_status_e nbglk_cancel_network(
    IN vip_network network
    )
{
    vip_status_e status = VIP_SUCCESS;

    if (VIP_NULL == network) {
        vip_error("fail cancel network object is NULL\n");
        vipGoOnError(VIP_ERROR_INVALID_ARGUMENTS);
    }

    if (viphal_query_hw_feature(VIP_HW_FEATURE_CANCEL)) {
        vipGoOnError(nbglk_hw_cancel_network(network));
        vip_info("job cancel successfully...\n");
    }
#if vpmdENABLE_CANCELATION > 1
    else {
        vipOnError(nbglk_sw_cancel_network(network));
    }
#else
    else {
        vip_error("fail not support hardware job cancel\n");
        vipGoOnError(VIP_ERROR_NOT_SUPPORTED);
    }
#endif

onError:
    return status;
}
#endif

vip_status_e nbglk_set_input(
    IN vip_network_t *network,
    IN vip_uint32_t index,
    IN vip_buffer input
    )
{
    vip_status_e status = VIP_SUCCESS;
    nbglk_io_patch_info_t *entry= VIP_NULL;
    nbglk_videomemory_t *memory = VIP_NULL;
    vip_int8_t matched = vip_true_e;

    if (VIP_NULL == input) {
        vip_error("input is NULL\n");
        return VIP_ERROR_INVALID_ARGUMENTS;
    }

    memory = &input->memory;
    if (VIP_NULL == memory) {
        vip_error("fail to set input, parameter is NULL\n");
        return VIP_ERROR_INVALID_ARGUMENTS;
    }

    if (index >= network->nbg_desc.n_inputs) {
        /* Out of index. */
        vip_error("set index=%d >= network input count=%d error\n",
            index, network->nbg_desc.n_inputs);
        vipGoOnError(VIP_ERROR_INVALID_ARGUMENTS);
    }

    matched = nbglk_check_io_params(network, index, input, 0);
    if(vip_true_e != matched) {
        vip_error("fail to set input %d, parameter not matched\n", index);
        vipGoOnError(VIP_ERROR_INVALID_ARGUMENTS);
    }

    if (vip_false_e == network->prepared_network) {
        vip_error("pls prepare network firstly\n");
        vipGoOnError(VIP_ERROR_NETWORK_NOT_PREPARED);
    }

    /* Set input. */
    entry = &network->network_inputs[index];
    entry->physical = memory->physical;
    entry->logical = memory->logical;
    entry->buffer = input;

    vip_debug("set input index=%d, physical=0x%llx, logical=%p, mem_id=0x%x, size=0x%x\n",
               index, memory->physical, memory->logical, memory->mem_id,  memory->size);

    /* Mark cmd input dirty. */
    network->input_dirty = 1;

    return status;

onError:
    vip_error("fail to set network input %d, mem_id=0x%x, status=%d\n",
                index, input->memory.mem_id, status);
    return status;
}

vip_status_e nbglk_set_output(
    IN vip_network_t *network,
    IN vip_uint32_t index,
    IN vip_buffer output
    )
{
    vip_status_e status = VIP_SUCCESS;
    nbglk_io_patch_info_t *entry = VIP_NULL;
    nbglk_videomemory_t *memory = VIP_NULL;
    vip_int8_t matched = vip_true_e;

    if (VIP_NULL == output) {
        vip_error("output is NULL\n");
        return VIP_ERROR_INVALID_ARGUMENTS;
    }

    memory = &output->memory;
    if (VIP_NULL == memory) {
        vip_error("fail to set output, parameter is NULL\n");
        return VIP_ERROR_INVALID_ARGUMENTS;
    }

    if (index >= network->nbg_desc.n_outputs) {
        /* Out of index. */
        vip_error("set index=%d >= network out count=%d error\n",
            index, network->nbg_desc.n_outputs);
        vipGoOnError(VIP_ERROR_INVALID_ARGUMENTS);
    }

    matched = nbglk_check_io_params(network, index, output, 1);
    if(vip_true_e != matched) {
        vip_error("fail to set output %d, parameter not matched\n", index);
        vipGoOnError(VIP_ERROR_INVALID_ARGUMENTS);
    }

    if (vip_false_e == network->prepared_network) {
        vip_error("pls prepare network firstly\n");
        vipGoOnError(VIP_ERROR_NETWORK_NOT_PREPARED);
    }

    /* Set output. */
    entry = &network->network_outputs[index];
    entry->physical = memory->physical;
    entry->logical = memory->logical;
    entry->buffer = output;

    vip_debug("set output index=%d, physical=0x%llx, logical=%p, mem_id=0x%x, size=0x%x\n",
               index, memory->physical, memory->logical, memory->mem_id, memory->size);

    /* Mark cmd input dirty. */
    network->output_dirty = 1;

    return status;

onError:
    vip_error("fail to set network output %d, mem_id=0x%x, status=%d\n",
                index, output->memory.mem_id, status);
    return status;
}

/*
@brief Query network input information. format, shape, quante format and so on.
@param network The network object create by vip_create_network().
@param index The index of this input.
@param property The query property, see vip_buffer_property_e.
@param [OUT] value The return value.
*/
vip_status_e nbglk_query_input(
    IN vip_network_t *network,
    IN vip_uint32_t index,
    IN vip_enum property,
    OUT void *value
    )
{
    vip_status_e status = VIP_SUCCESS;
    void *input;
    vipIsNULL(value);

    if (index >= network->nbg_desc.n_inputs) {
        vip_error("query index=%d >= network input count=%d error\n",
                     index, network->nbg_desc.n_inputs);
        vipGoOnError(VIP_ERROR_INVALID_ARGUMENTS);
    }

    input = (void*)&network->nbg_desc.inputs[index];

    vipOnError(nbglk_get_io_param(network, input, property, value));

    return status;

onError:
    vip_error("fail to query input[%d], property=%d\n", index, property);
    return status;
}

/*
@brief Query network output information. format, shape, quante format and so on.
@param network The network object create by vip_create_network().
@param index The index of this output.
@param property The query property, see vip_buffer_property_e.
@param [OUT] value The return value.
*/
vip_status_e nbglk_query_output(
    IN vip_network_t *network,
    IN vip_uint32_t index,
    IN vip_enum property,
    OUT void *value
    )
{
    vip_status_e status = VIP_SUCCESS;
    void *output = VIP_NULL;
    vipIsNULL(value);

    if (index >= network->nbg_desc.n_outputs) {
        vip_error("query index=%d >= network out count=%d error\n",
            index, network->nbg_desc.n_outputs);
        vipGoOnError(VIP_ERROR_INVALID_ARGUMENTS);
    }

    output = (void*)&network->nbg_desc.outputs[index];

    vipOnError(nbglk_get_io_param(network, output, property, value));

    return status;

onError:
    vip_error("fail to query output[%d], property=%d\n", index, property);
    return status;
}

/*
@brief Link two command buffer together
*/
vip_status_e nbglk_link_command(
    nbglk_videomemory_t *pre_cmd,
    nbglk_videomemory_t *cur_cmd
    )
{
    nbglk_append_link((vip_uint32_t*)(pre_cmd->logical + pre_cmd->size),
        SIZE_CAST(vip_uint32_t, cur_cmd->physical));

#if vpmdENABLE_VIDEO_MEMORY_CACHE
    viphal_flush_cache(pre_cmd->mem_id, VIPDRV_CACHE_FLUSH);
#endif
    return VIP_SUCCESS;
}

vip_uint32_t nbglk_get_segment_handle(
    nbglk_segment_t *segment
    )
{
    return segment->states_buffer.mem_id;
}
