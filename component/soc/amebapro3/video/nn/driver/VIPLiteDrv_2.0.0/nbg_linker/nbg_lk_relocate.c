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

#include <nbg_lk_relocate.h>
#include <nbg_lk_debug.h>
#include <nbg_lk_coeff.h>
#include <vip_hal_os_port.h>
#include <nbg_lk_context.h>
#include <nbg_fmt_util.h>
#include <nbg_lk_npd.h>
#include <nbg_lk_nld.h>

#if vpmdENABLE_RECORD_INFO
#define RECORD_INFO_ADD_LIST(offset, size, record_type)         \
{                                                               \
    vip_uint32_t layer_id = operation->layer_id;                \
    vip_uint32_t abs_id = operation->abs_op_id;                 \
    nbglk_record_info_add_list(segment, offset, size, layer_id, \
    abs_id, op_index, record_type);                             \
}
#else
#define RECORD_INFO_ADD_LIST(offset, size, record_type)
#endif

#define APPEND_FLUSH_CMDS(logical)  \
{   \
    vip_uint32_t count = 0; \
    vip_uint32_t *tmp_data = (vip_uint32_t*)logical; \
    tmp_data[count++] = 0x08010e02;  \
    tmp_data[count++] = 0x00000701;  \
    tmp_data[count++] = 0x48000000;  \
    tmp_data[count++] = 0x00000701;  \
    tmp_data[count++] = 0x0801022c;  \
    tmp_data[count++] = 0x0000001f;  \
    tmp_data[count++] = 0x08010e03;  \
    tmp_data[count++] = 0x00000c23;  \
    tmp_data[count++] = 0x08010e02;  \
    tmp_data[count++] = 0x00000701;  \
    tmp_data[count++] = 0x48000000;  \
    tmp_data[count++] = 0x00000701;  \
}

vip_uint32_t get_start_bit(
    vip_uint32_t mask
    )
{
    vip_uint32_t start_bit = 0;
    if (0 == mask) {
        return 0;
    }

    switch (mask)
    {
    case 0x000000FF: return 0;
    case 0x0000FF00: return 8;
    case 0x00FF0000: return 16;
    case 0xFF000000: return 24;
    case 0xFFFFFFFF: return 0;
    default:
        break;
    }

    while (!(mask & 1)) {
        mask = mask >> 1;
        start_bit += 1;
    }

    return start_bit;
}

static vip_status_e patch_source_input(
    vip_network_t *network,
    vip_uint8_t *cmd_logical,
    vip_uint32_t cmd_physical,
    nbg_desc_patch_t *patch
    )
{
    vip_status_e status = VIP_SUCCESS;
    vip_uint8_t *patch_addr = VIP_NULL;
    vip_address_t *ref_addr = VIP_NULL;
    vip_int32_t io_index = patch->index;
    nbglk_io_patch_info_t *input = VIP_NULL;
    vip_uint32_t counter = 0;

    patch_addr = (vip_uint8_t *)cmd_logical + patch->dest_offset;

    if ((io_index >= 0) && (io_index < (vip_int32_t)network->nbg_desc.n_inputs)) {
        input = &network->network_inputs[io_index];
        counter = network->network_inputs[io_index].counter;
        ref_addr = input->logical_in_cmd + counter;
        *ref_addr = (vip_address_t)(viphal_uintptr_t)cmd_logical;
        input->physical_in_cmd[counter] = cmd_physical;
        if ((network->nbg_desc.fixed.header.version < 0x0001001E) && (0 == network->prepare_count)) {
            if (NBG_PATCH_TRANSFORMATION_RIGHT_SHIFT_6 == patch->transformed) {
                patch->src_offset = (*((vip_uint32_t*)patch_addr) & WORD_BIT0_TO_BIT25_MASK) - patch->src_offset;
            }
            else {
                patch->src_offset = *((vip_uint32_t*)patch_addr) - patch->src_offset;
            }
        }
        input->patch[counter] = patch;
        input->counter++;
    }
    else {
        vip_error("fail to patch network input. ioindex is %d\n", io_index);
        status = VIP_ERROR_IO;
    }

    return status;
}

static vip_status_e patch_source_output(
    vip_network_t *network,
    vip_uint8_t *cmd_logical,
    vip_uint32_t cmd_physical,
    nbg_desc_patch_t *patch
    )
{
    vip_status_e status = VIP_SUCCESS;
    vip_int32_t io_index = patch->index;
    vip_uint8_t *patch_addr = VIP_NULL;
    vip_address_t *ref_addr = VIP_NULL;
    nbglk_io_patch_info_t *output = VIP_NULL;
    vip_uint32_t  counter = 0;

    patch_addr = (vip_uint8_t*)cmd_logical + patch->dest_offset;

    if ((io_index >= 0) && (io_index < (vip_int32_t)network->nbg_desc.n_outputs)) {
        output = &network->network_outputs[io_index];
        counter = network->network_outputs[io_index].counter;
        ref_addr = &output->logical_in_cmd[counter];
        *ref_addr = (vip_address_t)(viphal_uintptr_t)cmd_logical;
        output->physical_in_cmd[counter] = cmd_physical;
        if ((network->nbg_desc.fixed.header.version < 0x0001001E) && (0 == network->prepare_count)) {
            if (NBG_PATCH_TRANSFORMATION_RIGHT_SHIFT_6 == patch->transformed) {
                patch->src_offset = (*((vip_uint32_t*)patch_addr) & WORD_BIT0_TO_BIT25_MASK) - patch->src_offset;
            }
            else {
                patch->src_offset = *((vip_uint32_t*)patch_addr) - patch->src_offset;
            }
        }
        output->patch[counter] = patch;
        output->counter++;
    }
    else {
        vip_error("fail to patch network output. ioindex is %d\n", io_index);
        status = VIP_ERROR_IO;
    }

    return status;
}

static vip_status_e do_patch_source_mempool(
    vip_address_t physical,
    vip_uint8_t *command,
    nbg_desc_patch_t *patch
    )
{
    vip_address_t base_addr = 0;
    nbg_physical_value_t* new_physical = (nbg_physical_value_t*)&base_addr;
    vip_uint32_t *patch_addr = (vip_uint32_t*)(command + patch->dest_offset);
    vip_uint32_t *patch_addr_h = (vip_uint32_t*)(command + patch->dest_offset_h);

    base_addr = physical + patch->src_offset;
    if (NBG_PATCH_TRANSFORMATION_RIGHT_SHIFT_6 == patch->transformed) {
        *patch_addr = (*patch_addr & ~WORD_BIT0_TO_BIT25_MASK) | (new_physical->low >> 6);
    } else {
        *patch_addr = new_physical->low;
    }
    if (new_physical->high) {
        *patch_addr_h &= ~patch->mask_h;
        *patch_addr_h |= (new_physical->high << get_start_bit(patch->mask_h)) & patch->mask_h;
    }

    return VIP_SUCCESS;
}

vip_status_e patch_source_mempool(
    vip_network_t *network,
    vip_uint8_t *command,
    nbg_desc_patch_t *patch
    )
{
    vip_uint32_t old_addr = ~0;
    vip_uint32_t *patch_addr = (vip_uint32_t*)(command + patch->dest_offset);
    vip_status_e status = VIP_SUCCESS;

    if ((network->nbg_desc.fixed.header.version < 0x0001001E) && (0 == network->prepare_count)) {
        if (NBG_PATCH_TRANSFORMATION_RIGHT_SHIFT_6 == patch->transformed) {
            old_addr = *patch_addr & WORD_BIT0_TO_BIT25_MASK;
        }
        else {
            old_addr = *patch_addr;
        }
        patch->src_offset = old_addr - patch->src_offset;
    }

    do_patch_source_mempool(network->pool.physical, command, patch);

#if vpmdDYNAMIC_CHANGE_MEM_POOL
    network->pool_patch_info[network->pool_patch_count].cmd_logical = command;
    network->pool_patch_info[network->pool_patch_count].info.patch = patch;
    network->pool_patch_info[network->pool_patch_count].sw_layer = 0;
    network->pool_patch_count++;
#endif

    return status;
}

vip_status_e patch_source_sram(
    vip_network_t *network,
    vip_uint8_t *command,
    nbg_desc_patch_t *patch,
    vip_uint32_t type
    )
{
    vip_uint32_t *mem_addr = VIP_NULL;
    vip_uint32_t src_addr = 0;
    vip_address_t base_physical = 0;
    nbg_physical_value_t* new_physical = (nbg_physical_value_t*)&base_physical;
    vip_uint32_t device_index = network->device->dev_index;
    vip_status_e status = VIP_SUCCESS;
    nbglk_context_t *context = network->context;

    if (NBG_SOURCE_AXI_SRAM == type) {
        base_physical = context->axi_sram_base[device_index];
    } else if (NBG_SOURCE_VIP_SRAM == type) {
        base_physical = context->vip_sram_base;
    } else {
        vip_error("fail to patch sram, type error\n");
        vipGoOnError(VIP_ERROR_INVALID_ARGUMENTS);
    }

    mem_addr = (vip_uint32_t *)(command + patch->dest_offset);
    src_addr = *mem_addr;
    if (patch->transformed == NBG_PATCH_TRANSFORMATION_RIGHT_SHIFT_6) {
        src_addr &= ~0u >> 6;
        if ((network->nbg_desc.fixed.header.version < 0x00010007)) {
            /* after NBG format 0x00010007 version, store offset in command buffer,
               don't need shift 6bits if store offset in NBG */
            src_addr <<= 6;
        }
    }

    if ((network->nbg_desc.fixed.header.version < 0x0001001E) && (0 == network->prepare_count)) {
        patch->src_offset = src_addr - patch->src_offset;
    }
    base_physical += patch->src_offset;

    if ((NBG_SOURCE_VIP_SRAM == type) && (patch->src_offset > network->max_vipsram_offset)) {
        network->max_vipsram_offset = SIZE_CAST(vip_uint32_t, patch->src_offset);
    }

    if (patch->transformed == NBG_PATCH_TRANSFORMATION_RIGHT_SHIFT_6) {
        new_physical->low = ((*mem_addr) & 0xfc000000) | (new_physical->low >> 6);
    }
    *mem_addr = new_physical->low;

    if (new_physical->high) {
        mem_addr = (vip_uint32_t*)(command + patch->dest_offset_h);
        *mem_addr &= ~patch->mask_h;
        *mem_addr |= (new_physical->high << get_start_bit(patch->mask_h)) & patch->mask_h;
    }
onError:
    return status;
}

#if vpmdENABLE_CHANGE_PPU_PARAM
static vip_status_e patch_ppu_params(
    vip_network network,
    vip_uint8_t *state_buf,
    vip_uint32_t state_size,
    nbg_desc_sh_operation_t*shop
    )
{
    vip_status_e status = VIP_SUCCESS;
    nbg_desc_ppu_param_t *ppu_param_data = &network->nbg_desc.ppu_param[shop->ppu_param_index];
    vip_ppu_param_patch_info_t *ppu_param_i = VIP_NULL;
    vip_uint32_t *cmd = (vip_uint32_t*)state_buf;
    vip_uint32_t i = 0;

    if (VIP_NULL == network->ppu_param_info) {
        vip_error("fail to patch ppu param, ppu_param_info is NULL\n");
        vipGoOnError(VIP_ERROR_INVALID_ARGUMENTS);
    }

    ppu_param_i = &network->ppu_param_info[shop->ppu_param_index];

    ppu_param_i->glooffset_x_logic = (vip_address_t*)(state_buf + ppu_param_data->global_offset_x);
    ppu_param_i->glooffset_y_logic = (vip_address_t*)(state_buf + ppu_param_data->global_offset_y);
    ppu_param_i->glooffset_z_logic = (vip_address_t*)(state_buf + ppu_param_data->global_offset_z);

    ppu_param_i->gloscale_x_logic = (vip_address_t*)(state_buf + ppu_param_data->global_scale_x);
    ppu_param_i->gloscale_y_logic = (vip_address_t*)(state_buf + ppu_param_data->global_scale_y);
    ppu_param_i->gloscale_z_logic = (vip_address_t*)(state_buf + ppu_param_data->global_scale_z);

    ppu_param_i->grosize_x_logic = (vip_address_t*)(state_buf + ppu_param_data->group_size_x);
    ppu_param_i->grosize_y_logic = (vip_address_t*)(state_buf + ppu_param_data->group_size_y);
    ppu_param_i->grosize_z_logic = (vip_address_t*)(state_buf + ppu_param_data->group_size_z);

    ppu_param_i->grocount_x_logic = (vip_address_t*)(state_buf + ppu_param_data->group_count_x);
    ppu_param_i->grocount_y_logic = (vip_address_t*)(state_buf + ppu_param_data->group_count_y);
    ppu_param_i->grocount_z_logic = (vip_address_t*)(state_buf + ppu_param_data->group_count_z);

    for (i = 0; i < (state_size / sizeof(vip_uint32_t)); i++) {
        if (0x08010240 == cmd[i]) {
            network->ppu_work_dim = cmd[i+1] & 0x3;
            break;
        }
    }

onError:
    return status;
}
#endif

#if vpmdENABLE_MULTIPLE_VIP
vip_status_e patch_multivip_chipid(
    vip_network network,
    nbglk_segment_t *segment,
    vip_uint8_t *states_buffer,
    nbg_desc_patch_t *pd_entry
    )
{
    vip_status_e status = VIP_SUCCESS;
    nbglk_device_t *device = network->device;
    /*Patch the chip id for multi core*/
    vip_uint32_t core_id = device->core_id[network->core_index];
    vip_uint32_t *statestmp = (vip_uint32_t*)(states_buffer + pd_entry->dest_offset);
#if vpmdTASK_SCHEDULE
    vip_uint32_t offset = SIZE_CAST(vip_uint32_t, (vip_uint8_t*)statestmp - segment->states_buffer.logical);
#endif
    if ((device->core_count > 1)) {
        *statestmp = SIZE_CAST(vip_uint32_t, pd_entry->src_offset) | (1 << (core_id + pd_entry->index));
        #if vpmdENABLE_CNN_PROFILING
        segment->core_id_mask |= 1 << (core_id + pd_entry->index);
        #endif
    }
    else {
        vip_error("device index error, patch %d fail.\n", pd_entry->source_type);
    }

#if vpmdTASK_SCHEDULE
    status = viphal_task_set_patch_info(network->task_id, VIPDRV_TASK_CMD_SUBTASK, segment->index,
        segment->patch_count, VIPDRV_TASK_PATCH_CHIP, offset, sizeof(vip_uint32_t), pd_entry->index, 1);
    segment->patch_count += 1;
#endif

    return status;
}

vip_status_e patch_multivip_sync_cmd(
    vip_network network,
    nbglk_segment_t* segment,
    vip_uint8_t *states_buffer,
    nbg_desc_patch_t *pd_entry
    )
{
    vip_status_e status = VIP_SUCCESS;
    vip_uint32_t core_num = network->nbg_desc.net_info.core_count;
    vip_uint32_t *statestmp = (vip_uint32_t*)(states_buffer + pd_entry->dest_offset);
    vip_uint32_t *nbg_sync_cmds = statestmp;
    vip_uint32_t *driver_sync_cmds = network->multivip_sync_cmd;
    vip_uint32_t i = 0;
#if vpmdTASK_SCHEDULE
    vip_uint32_t offset = SIZE_CAST(vip_uint32_t, (vip_uint8_t*)statestmp - segment->states_buffer.logical);
#endif

    if (0xD9 == network->context->hw_target) {
        if (network->sync_cmd_size > pd_entry->src_offset) {
            vip_error("fail sync cmd size runtime=%d, nbg=%d\n", network->sync_cmd_size, pd_entry->src_offset);
            vipGoOnError(VIP_ERROR_FAILURE);
        }
    }
    else {
        if (network->sync_cmd_size != pd_entry->src_offset) {
            vip_error("fail sync cmd size runtime=%d, nbg=%d\n", network->sync_cmd_size, pd_entry->src_offset);
            vipGoOnError(VIP_ERROR_FAILURE);
        }
    }

    /* check sync cmds match with NBG */
    for (i = 0; i < 4; i++) {
        if (nbg_sync_cmds[i] != driver_sync_cmds[i]) {
            vip_error("fail syn cmd not match i=%d nbg=0x%08x driver=0x%08x\n",
                        i, nbg_sync_cmds[i], driver_sync_cmds[i]);
            vipGoOnError(VIP_ERROR_INVALID_ARGUMENTS);
        }
    }

    if (network->nbg_desc.fixed.header.version > 0x0001000E) {
        for (i = 4; i < network->sync_cmd_size/sizeof(vip_uint32_t); i+=4) {
            if ((nbg_sync_cmds[i] != driver_sync_cmds[i]) || (nbg_sync_cmds[i+2] != driver_sync_cmds[i+2])) {
                vip_error("fail syn cmd not match i=%d nbg=0x%08x 0x%08x\n",
                             i, nbg_sync_cmds[i], nbg_sync_cmds[i+2]);
                vip_error("dump NBG sync command:\n");
                viphal_dump_data(nbg_sync_cmds, SIZE_CAST(vip_uint32_t, pd_entry->src_offset));
                vip_error("dump driver sync command:\n");
                viphal_dump_data(network->multivip_sync_cmd, network->sync_cmd_size);
                vipGoOnError(VIP_ERROR_INVALID_ARGUMENTS);
            }
        }
    }

    if (core_num > 1) {
        if (pd_entry->src_offset > 0) {
            vipOnError(viphal_copy_memory(statestmp, network->multivip_sync_cmd,
                network->sync_cmd_size));
        }
    }
    else {
        vip_error("device core count not more than 1, device%d core num %d fail.\n",
                     network->device->dev_index, core_num);
        return VIP_ERROR_INVALID_ARGUMENTS;
    }

#if vpmdTASK_SCHEDULE
    status = viphal_task_set_patch_info(network->task_id, VIPDRV_TASK_CMD_SUBTASK, segment->index,
        segment->patch_count, VIPDRV_TASK_PATCH_SYNC, offset,
        network->sync_cmd_size, 0, network->nbg_desc.net_info.core_count);
    segment->patch_count += 1;
#endif
onError:
    return status;

}

/*
@brief append CHIP_ENABLE command at the command address.
*/
vip_uint32_t nbglk_append_chip_enable(
    vip_uint32_t *command,
    vip_uint32_t core_id,
    vip_bool_e   is_enable_all
    )
{
    if (is_enable_all == vip_true_e) {
        command[0] = 0x6800FFFF;
        command[1] = 0;
    }
    else {
        command[0] = 0x68000000 | (1 << core_id);
        command[1] = 0;
    }
    return 8;
}

#endif

/* get all weight patch information */
static vip_status_e get_weight_patch_info(
    vip_network network,
    nbglk_segment_t *segment,
    nbg_desc_operation_t *operation,
    vip_uint32_t op_index,
    nbg_desc_patch_t *patch_data,
    nbglk_record_info_type_e record_type,
    vip_address_t *base_addr,
    vip_size_t *offset
    )
{
    vip_status_e status = VIP_SUCCESS;
    vip_uint32_t i = 0, j = 0, find_op = 0, found = 0;
    nbg_desc_operation_t *friend_operation = VIP_NULL;
    nbg_desc_patch_t *pd_entry = patch_data;
    vip_network friend_network = network->friend_network;
    vip_network weight_network = network;
    vip_size_t size = 0;

    /* weight sharing, get the new weight_network object */
    if (NBGLK_FROM_NBG_DUP_COMMAND == network->dup_type) {
        vip_uint64_t weight_size = 0, friend_weight_size = 0;
        vip_char_t *op_name = nbg_get_operation_name(operation->type);
        if (VIP_NULL == friend_network) {
            vip_error("fail friend network is NULL in dup command\n");
            vipGoOnError(VIP_ERROR_INVALID_NETWORK);
        }
        if (0 == operation->abs_share_id) {
            /* when abs_op_id is zero, indicate that the operation can't support sharing weight */
            vip_error("error op %s support sharing coeff when abs op id is 0\n", op_name);
            vipGoOnError(VIP_ERROR_FAILURE);
        }
        for (i = 0; i < friend_network->nbg_desc.n_operations; i++) {
            friend_operation = &friend_network->nbg_desc.operations[i];
            if (friend_operation->abs_share_id == operation->abs_share_id) {
                vip_uint32_t coeff_index = 0;
                find_op = 1;
                for (j = 0; j < operation->patch_count; j++) {
                    pd_entry = &network->nbg_desc.pd_sections[operation->patch_index + j];
                    if (patch_data == pd_entry) {
                        break;
                    }
                    if (NBG_SOURCE_MISC_DYNA_GENERIC == pd_entry->source_type) {
                        coeff_index++;
                    }
                }

                for (j = 0; j < friend_operation->patch_count; j++) {
                    pd_entry = &friend_network->nbg_desc.pd_sections[friend_operation->patch_index + j];
                    if (NBG_SOURCE_MISC_DYNA_GENERIC == pd_entry->source_type) {
                        if (0 == coeff_index) {
                            found = 1;
                            break;
                        }
                        coeff_index--;
                    }
                }
                break;
            }
        }
        if (0 == found) {
            vip_error("fail match share_wb_id=%d, abs_op_id=%d, find_op=%s, op_engine=%s, uid=%d, name=%s\n",
                         operation->abs_share_id, operation->abs_op_id, find_op ? "YES" : "NO", op_name,
                         network->nbg_desc.orig_layers[operation->layer_id].uid,
                         network->nbg_desc.orig_layers[operation->layer_id].name);
            vipGoOnError(VIP_ERROR_FAILURE);
        }
        #if vpmdPRELOAD_COEFF
        if (0x0001001A <= network->nbg_desc.fixed.header.version) {
            weight_size = network->coeff_property[patch_data->index].size;
            friend_weight_size = friend_network->coeff_property[pd_entry->index].size;
        }
        else
        #endif
        {
            weight_size = network->nbg_desc.coeff_data_table[patch_data->index].size;
            friend_weight_size = friend_network->nbg_desc.coeff_data_table[pd_entry->index].size;
        }
        if (weight_size != friend_weight_size) {
            vip_error("fail weight size not match ask size is 0x%"PRIx64" vs 0x%"PRIx64"\n",
                       weight_size, friend_weight_size);
            vip_error("fail cur network share_wb_id=%d, abs_op_id=%d, op_engine=%s, uid=%d, name=%s\n",
                         operation->abs_share_id, operation->abs_op_id, op_name,
                         network->nbg_desc.orig_layers[operation->layer_id].uid,
                         network->nbg_desc.orig_layers[operation->layer_id].name);
            vip_error("fail friend network share_wb_id=%d, abs_op_id=%d, op_engine=%s, uid=%d, name=%s\n",
                         friend_operation->abs_share_id, operation->abs_op_id, op_name,
                         friend_network->nbg_desc.orig_layers[operation->layer_id].uid,
                         friend_network->nbg_desc.orig_layers[operation->layer_id].name);
        }
        weight_network = friend_network;
        vip_info("op=%s dup cmd for sharing weight\n", op_name);
    }

    vipOnError(nbglk_coeff_get_property(weight_network, pd_entry->index, VIP_NULL, base_addr, offset, &size));
    RECORD_INFO_ADD_LIST(pd_entry->index, size, record_type);

onError:
    return status;
}

/*
This function will be called for patch:
nn: dynamic_input/output
tp: dynamic_input/output and weight
ppu: dynamic_input/output and weight
*/
vip_status_e patch_source_misc_data(
    vip_network_t *network,
    nbglk_segment_t *segment,
    nbg_desc_operation_t *operation,
    vip_uint8_t *command,
    nbg_desc_patch_t *pd_entry,
    vip_uint32_t op_index
    )
{
    vip_status_e status = VIP_SUCCESS;
    vip_uint32_t *addr = (vip_uint32_t*)(command + pd_entry->dest_offset);
    vip_uint32_t *addr_h = (vip_uint32_t*)(command + pd_entry->dest_offset_h);
    vip_size_t offset = 0;
    vip_address_t base_addr = 0;
    nbg_physical_value_t* new_physical = (nbg_physical_value_t*)&base_addr;
    vip_size_t data_offset = 0;
    nbglk_videomemory_t *buffer = VIP_NULL;

    if (pd_entry->source_type == NBG_SOURCE_MISC_DYNA_GENERIC) {
        nbglk_record_info_type_e recod_type = RECORD_TYPE_NONE;
    #if vpmdENABLE_RECORD_INFO
        switch (operation->type) {
        case NBG_OPERATION_TYPE_TP:
        {
            recod_type = RECORD_TYPE_TP_WEIGHT;
        }
        break;
        case NBG_OPERATION_TYPE_SH:
        {
            recod_type = RECORD_TYPE_SH_WEIGHT;
        }
        break;
        default:
        vip_error("not support recod weight for the op type=%s\n", nbg_get_operation_name(operation->type));
        break;
        }
    #endif
        vipOnError(get_weight_patch_info(network, segment, operation, op_index, pd_entry,
                                        recod_type, &base_addr, &data_offset));
    }
    else {
        buffer = &network->dyna_io;
        base_addr = buffer->physical;
        data_offset = network->nbg_desc.dyna_io_table[pd_entry->index].offset;
        RECORD_INFO_ADD_LIST(data_offset, network->nbg_desc.dyna_io_table[pd_entry->index].size,
            RECORD_TYPE_DYNA_DATA);
    }

    if ((network->nbg_desc.fixed.header.version < 0x0001001E) && (0 == network->prepare_count)) {
        if (NBG_PATCH_TRANSFORMATION_RIGHT_SHIFT_6 == pd_entry->transformed) {
            pd_entry->src_offset = ((*addr) << 6) - (pd_entry->src_offset);
        }
        else {
            pd_entry->src_offset = *addr - pd_entry->src_offset;
        }

    }

    offset = pd_entry->src_offset;
    if (pd_entry->index >= 0) {
        #if vpmdPRELOAD_COEFF > 1
        if (0x0001001A <= network->nbg_desc.fixed.header.version &&
            (pd_entry->source_type == NBG_SOURCE_MISC_DYNA_GENERIC) &&
            NBGLK_COEFF_LOC_VIP_SRAM == network->coeff_property[pd_entry->index].location) {
            nbglk_context_t* context = network->context;
            base_addr = context->vip_sram_base + network->coeff_property[pd_entry->index].dst_offset + offset;
        }
        else
        #endif
        {
            base_addr += data_offset + offset;
        }
        if (nbg_operation_is_nn_engine(operation->type) &&
            (NBG_PATCH_TRANSFORMATION_RIGHT_SHIFT_6 == pd_entry->transformed)) {
            *addr = ((*addr) & (~WORD_BIT0_TO_BIT25_MASK)) | (new_physical->low >> 6);
        }
        else {
            *addr = new_physical->low;
        }
        if (new_physical->high) {
            *addr_h &= ~pd_entry->mask_h;
            *addr_h |= (new_physical->high << get_start_bit(pd_entry->mask_h)) & pd_entry->mask_h;
        }
    }
    else {
        vip_error("source type=%d\n", pd_entry->source_type);
        vipGoOnError(VIP_ERROR_INVALID_NETWORK);
    }

    return status;
onError:
    vip_error("fail to patch patch source misc data, source_type=%d\n", pd_entry->source_type);
    return status;
}

/*
 * @brief
 * Patch the NN command data to apply the actual pool base.
 */
static vip_status_e patch_nn(
    vip_network_t *network,
    nbglk_segment_t *segment,
    nbg_desc_operation_t *operation,
    vip_uint32_t op_index,
    vip_uint8_t *nn_source,
    vip_uint8_t *states,
    vip_uint32_t *states_size,
    vip_uint32_t *event_id
    )
{
    vip_status_e status = VIP_SUCCESS;
    vip_uint8_t *nn_cmd_logical = VIP_NULL;
    vip_uint8_t *states_buffer = VIP_NULL;
    vip_uint32_t *trigger_word = VIP_NULL;
    vip_uint32_t nn_cmd_physical = ~0;
    nbg_desc_data_table_t *data_table = VIP_NULL;
    nbg_desc_patch_t *pd_entry = VIP_NULL;
    nbg_desc_patch_t* kernel_header_patch = VIP_NULL;
    vip_uint32_t profile_cmd_offset = 0, start_cmd_offset = 0;
    vip_uint32_t i = 0;
#if vpmdENABLE_LAYER_DUMP
    vip_uint32_t core_id = 0;
    vip_uint32_t eventid = 0;
#endif

    if (VIP_NULL == segment) {
        vip_error("segment is NULL\n");
        return VIP_ERROR_INVALID_NETWORK;
    }

    /* Set shortcuts. */
    nn_cmd_logical = nn_source;
    states_buffer = states;

    if (network->npd.enable_npd) {
        nbglk_npd_generate_command(states_buffer, network, segment, operation, NBGLK_NPDCMD_BEGIN);
        ONE_OP_PROFILE_CMD_SIZE(network->device->core_count, profile_cmd_offset)
        ONE_OP_PROFILE_START_CMD_SIZE(network->device->core_count, start_cmd_offset)
        states_buffer = states_buffer + start_cmd_offset;
    }

    nn_cmd_physical = SIZE_CAST(vip_uint32_t, network->nnCmds.physical +
        (vip_uint32_t)(nn_cmd_logical - (vip_uint8_t *)network->nnCmds.logical));

    /* Copy control states. */
    data_table = &network->nbg_desc.load_states_table[operation->state_id];
    viphal_copy_memory(states_buffer, network->load_states + data_table->offset,
        SIZE_CAST(vip_uint32_t, data_table->size));

    /* Patch addresses used by the command. */
    for (i = 0; i < operation->patch_count; i++) {
        pd_entry = &network->nbg_desc.pd_sections[operation->patch_index + i];
        if (pd_entry->type == NBG_PATCH_TYPE_STATE) {
            switch (pd_entry->source_type) {
            case NBG_SOURCE_COMMAND:
            {
                /* Patch the control states for NN. */
                vip_uint32_t *statestmp = (vip_uint32_t*)(states_buffer + pd_entry->dest_offset);
                trigger_word = (vip_uint32_t *)statestmp;
                *trigger_word = ((*trigger_word) & 0x3f) | (nn_cmd_physical & ~0x3f);
                #if vpmdENABLE_LAYER_DUMP
                eventid = *trigger_word & 0x1f;
                #endif
                #if vpmdENABLE_QUICK_RESET || vpmdENABLE_STALL_FE
                *event_id = (*trigger_word & 0x1f);
                #endif
            }
            break;

            #if vpmdENABLE_MULTIPLE_VIP
            case NBG_SOURCE_MULTIVIP_CHIPID:
            {
                vipOnError(patch_multivip_chipid(network, segment, states_buffer, pd_entry));
                #if vpmdENABLE_LAYER_DUMP
                core_id = pd_entry->index;
                #endif
            }
            break;
            case NBG_SOURCE_MULTIVIP_SYNC_CMDS:
            {
                vipOnError(patch_multivip_sync_cmd(network, segment, states_buffer, pd_entry));
            }
            break;
            #endif

            default:
                vip_error("patch nn not support this patch souce type=%d\n", pd_entry->source_type);
                break;
            }
        }
        else if (pd_entry->type == NBG_PATCH_TYPE_COMMAND) {
            switch(pd_entry->source_type) {
            case NBG_SOURCE_INPUT:
            {
                vipOnError(patch_source_input(network, nn_cmd_logical, nn_cmd_physical, pd_entry));
            }
            break;

            case NBG_SOURCE_OUTPUT:
            {
                vipOnError(patch_source_output(network, nn_cmd_logical, nn_cmd_physical, pd_entry));
            }
            break;

            case NBG_SOURCE_MEMORY_POOL:
            {
                vipOnError(patch_source_mempool(network, nn_cmd_logical, pd_entry));
            }
            break;

            case NBG_SOURCE_AXI_SRAM:
            {
                vipOnError(patch_source_sram(network, nn_cmd_logical, pd_entry, NBG_SOURCE_AXI_SRAM));
            }
            break;

            case NBG_SOURCE_VIP_SRAM:
            {
                vipOnError(patch_source_sram(network, nn_cmd_logical, pd_entry, NBG_SOURCE_VIP_SRAM));
            }
            break;

            case NBG_SOURCE_MISC_DYNA_GENERIC:
            {
                vip_address_t base_addr = 0;
                nbg_physical_value_t* new_physical = (nbg_physical_value_t*)&base_addr;
                vip_size_t data_offset = 0;
                vip_uint32_t *addr = (vip_uint32_t *)(nn_cmd_logical + pd_entry->dest_offset);
                vip_uint32_t *addr_h = (vip_uint32_t*)(nn_cmd_logical + pd_entry->dest_offset_h);
                nbglk_record_info_type_e record_type = NBG_OPERATION_TYPE_SP == operation->type ?
                                                       RECORD_TYPE_SP_WEIGHT : RECORD_TYPE_NN_WEIGHT;
                if (0 != network->coeff_header.mem_id) {
                    base_addr = network->coeff_header.physical;
                    data_offset = network->nbg_desc.coeff_header_table[pd_entry->index].offset;
                    RECORD_INFO_ADD_LIST(data_offset, network->nbg_desc.coeff_header_table[pd_entry->index].size,
                        record_type);
                }
                else {
                    vipOnError(get_weight_patch_info(network, segment, operation, op_index, pd_entry,
                        record_type, &base_addr, &data_offset));
                }

                if ((network->nbg_desc.fixed.header.version < 0x0001001E) && (0 == network->prepare_count)) {
                    if (network->nbg_desc.fixed.header.version > 0x00010003) {
                        pd_entry->src_offset = ((*addr) & (WORD_BIT0_TO_BIT25_MASK)) - pd_entry->src_offset;
                    }
                    else {
                        pd_entry->src_offset = 0x0;
                    }
                }
                base_addr += data_offset + pd_entry->src_offset;
                if (new_physical->high) {
                    *addr_h &= ~pd_entry->mask_h;
                    *addr_h |= (new_physical->high << get_start_bit(pd_entry->mask_h)) & pd_entry->mask_h;
                }
                *addr = ((*addr) & (~WORD_BIT0_TO_BIT25_MASK)) | (new_physical->low >> 6);
                kernel_header_patch = pd_entry;/* for patch kernel bit stream */
                #if vpmdPRELOAD_COEFF
                if (0 == network->coeff_header.mem_id) {
                    /* kernel_descriptor not enabled, DYNA_GENERIC patch source_type is for kernel body */
                    status = nbglk_coeff_preload_patch_nn(network, pd_entry->index, nn_cmd_logical,
                                                          NBGLK_COEFF_PATH_NN_CMD);
                    if (status != VIP_SUCCESS) {
                        vip_error("fail to patch nn coeff preload status=%d\n", status);
                        vipGoOnError(status);
                    }
                }
                #endif
            }
            break;

            case NBG_SOURCE_MISC_DYNA_INPUT:
            case NBG_SOURCE_MISC_DYNA_OUTPUT:
            {
                vipOnError(patch_source_misc_data(network, segment, operation, nn_cmd_logical, pd_entry, op_index));
            }
            break;

            case NBG_SOURCE_MULTIVIP_CHIPID:
            case NBG_SOURCE_MULTIVIP_SYNC_CMDS:
            break;

            case NBG_SOURCE_COMMAND:
            default:
                vip_error("not implement this source_type=%d\n", pd_entry->source_type);
                break;
            }
        }
        else if (pd_entry->type == NBG_PATCH_TYPE_KERNEL) {
            switch(pd_entry->source_type) {
                case NBG_SOURCE_KERNEL_BITSTREAM:
                {
                    vip_uint32_t k = 0;
                    vip_uint8_t data_type = NBGLK_RELCAT_TYPE_WEIGHT_BITSTREAM;
                    nbg_desc_patch_t *temp_patch = VIP_NULL;
                    if (NBGLK_FROM_NBG_DUP_COMMAND == network->dup_type) {
                        vip_error("fail dup command not support kernel descriptor\n");
                        vipGoOnError(VIP_ERROR_NOT_SUPPORTED);
                    }
                    if (VIP_NULL == kernel_header_patch) {
                        /* find out header patchInfo. the weight in dynamic io section */
                        for (k = 0; k < operation->patch_count; k++) {
                            temp_patch = &network->nbg_desc.pd_sections[operation->patch_index + k];
                            if ((NBG_PATCH_TYPE_COMMAND == temp_patch->type) &&
                                (NBG_SOURCE_MISC_DYNA_GENERIC == temp_patch->source_type)) {
                                kernel_header_patch = temp_patch;
                                break;
                            }
                            else if ((NBG_PATCH_TYPE_COMMAND == temp_patch->type) &&
                                    (NBG_SOURCE_MISC_DYNA_INPUT == temp_patch->source_type) &&
                                    (NBG_PATCH_TRANSFORMATION_RIGHT_SHIFT_6 == temp_patch->transformed)) {
                                    kernel_header_patch = temp_patch;
                                    data_type = NBGLK_RELCAT_TYPE_DYNA_IO;
                                    break;
                            }
                        }
                    }
                    if (VIP_NULL != kernel_header_patch) {
                        vip_uint32_t *mem_addr = VIP_NULL;
                        vip_uint32_t* mem_addr_h = VIP_NULL;
                        vip_uint8_t *header_logical = VIP_NULL;
                        vip_address_t bitstream_physical = 0;
                        nbg_physical_value_t* new_physical = (nbg_physical_value_t*)&bitstream_physical;
                        vip_size_t offset = 0;
                        vip_size_t coeff_size = 0;
                        if (NBGLK_RELCAT_TYPE_DYNA_IO == data_type) {
                            bitstream_physical = network->dyna_io.physical +
                                network->nbg_desc.dyna_io_table[pd_entry->index].offset;
                            RECORD_INFO_ADD_LIST(network->nbg_desc.dyna_io_table[pd_entry->index].offset,
                                network->nbg_desc.dyna_io_table[pd_entry->index].size,
                                RECORD_TYPE_DYNA_DATA);
                            header_logical = network->dyna_io.logical +
                                network->nbg_desc.dyna_io_table[kernel_header_patch->index].offset;
                        }
                        else {
                            nbglk_coeff_get_property(network, pd_entry->index, VIP_NULL,
                                &bitstream_physical, &offset, &coeff_size);
                            bitstream_physical += offset;
                            RECORD_INFO_ADD_LIST(pd_entry->index, coeff_size,
                                                 NBG_OPERATION_TYPE_SP == operation->type ?
                                                 RECORD_TYPE_SP_WEIGHT_BITSTREAM :
                                                 RECORD_TYPE_NN_WEIGHT_BITSTREAM);
                            if (0 != network->coeff_header.mem_id) {
                                header_logical = network->coeff_header.logical;
                                offset = network->nbg_desc.coeff_header_table[kernel_header_patch->index].offset;
                            }
                            else {
                                nbglk_coeff_get_property(network, kernel_header_patch->index,
                                               &header_logical, VIP_NULL, &offset, VIP_NULL);
                            }
                            header_logical += offset;
                        }
                        mem_addr = (vip_uint32_t *)(header_logical + pd_entry->dest_offset);
                        mem_addr_h = (vip_uint32_t*)(header_logical + pd_entry->dest_offset_h);
                        if (NBGLK_RELCAT_TYPE_DYNA_IO == data_type) {
                            offset = *mem_addr;
                            bitstream_physical += offset;
                        }
                        *mem_addr = new_physical->low;
                        if (new_physical->high) {
                            *mem_addr_h &= ~pd_entry->mask_h;
                            *mem_addr_h |= (new_physical->high << get_start_bit(pd_entry->mask_h)) & pd_entry->mask_h;
                        }

                        #if vpmdPRELOAD_COEFF
                        status = nbglk_coeff_preload_patch_nn(network, pd_entry->index, (vip_uint8_t*)mem_addr,
                                                              NBGLK_COEFF_PATH_NN_KERNEL_HEADER);
                        if (status != VIP_SUCCESS) {
                            vip_error("fail to patch nn header coeff  preload status=%d\n", status);
                            vipGoOnError(status);
                        }
                        #endif
                    }
                    else {
                        vip_error("fail not find header patch info: %d\n", pd_entry->source_type);
                        vipGoOnError(VIP_ERROR_NOT_SUPPORTED);
                    }
                }
                break;

                default:
                    vip_error("fail not support this patch source type[%d]\n", pd_entry->source_type);
                    vipGoOnError(VIP_ERROR_INVALID_ARGUMENTS);
                    break;
            }
        }
    }

    #if vpmdENABLE_LAYER_DUMP
    if (network->nld.enable) {
        network->nld.nn_event_id[core_id] = eventid;
    }
    #endif

    if (network->npd.enable_npd) {
        states_buffer = states_buffer + data_table->size;
        nbglk_npd_generate_command(states_buffer, network, segment, operation, NBGLK_NPDCMD_END);
    }

    /* Query the states size. */
    if (states_size != VIP_NULL) {
        *states_size = SIZE_CAST(vip_uint32_t, data_table->size) + profile_cmd_offset;
    }

#if vpmdENABLE_RECORD_INFO
    {
        vip_uint32_t offset = (vip_uint32_t)(nn_cmd_logical - (vip_uint8_t *)network->nnCmds.logical);
        vip_uint32_t size_tmp = nbg_get_nn_command_size(&network->nbg_desc);
        vip_uint32_t layer_id = operation->layer_id;
        vip_uint32_t type = RECORD_TYPE_NN_COMMAND;
        switch (operation->type) {
        case NBG_OPERATION_TYPE_SP:
            type = RECORD_TYPE_SP_COMMAND;
            break;
        case NBG_OPERATION_TYPE_NN_LUT:
            type = RECORD_TYPE_NN_LUT_COMMAND;
            break;
        case NBG_OPERATION_TYPE_NT:
            type = RECORD_TYPE_NT_COMMAND;
            break;
        default:
            break;
        }
        nbglk_record_info_add_list(segment, offset, size_tmp, layer_id, operation->abs_op_id, op_index, type);
    }
#endif

    return status;
onError:
    vip_error("fail to patch nn status=%d\n", status);
    return status;
}

/*
 * @brief
 * Patch the TP command data to apply the actual pool base.
 */
static vip_status_e patch_tp(
    vip_network_t *network,
    nbglk_segment_t *segment,
    nbg_desc_operation_t *operation,
    vip_uint32_t op_index,
    vip_uint8_t *tp_source,
    vip_uint8_t *states,
    vip_uint32_t *states_size
    )
{
    vip_status_e status = VIP_SUCCESS;
    vip_uint8_t *tp_cmd_logical = VIP_NULL;
    vip_uint32_t *trigger_word = VIP_NULL;
    vip_uint8_t *states_buffer = VIP_NULL;
    vip_uint32_t tp_cmd_physical = 0;
    nbg_desc_data_table_t *data_table = VIP_NULL;
    nbg_desc_patch_t *pd_entry = VIP_NULL;
    vip_uint32_t i = 0;

    if (VIP_NULL == segment) {
        vip_error("segment is NULL\n");
        return VIP_ERROR_INVALID_NETWORK;
    }

    /* Set shortcuts. */
    tp_cmd_logical = tp_source;
    states_buffer = states;

    tp_cmd_physical = SIZE_CAST(vip_uint32_t, network->tpCmds.physical +
        (vip_uint32_t)(tp_cmd_logical - (vip_uint8_t *)network->tpCmds.logical));

    /* Copy TP engine control states. */
    data_table = &network->nbg_desc.load_states_table[operation->state_id];
    viphal_copy_memory(states_buffer, network->load_states + data_table->offset,
        SIZE_CAST(vip_uint32_t, data_table->size));

    if (states_size != VIP_NULL) {
        *states_size = SIZE_CAST(vip_uint32_t, data_table->size);
    }

    for (i = 0; i < operation->patch_count; i++) {
        pd_entry = &network->nbg_desc.pd_sections[operation->patch_index + i];
        if (pd_entry->type == NBG_PATCH_TYPE_STATE) {
            switch (pd_entry->source_type) {
            case NBG_SOURCE_COMMAND:
            {
                vip_uint32_t *statestmp = (vip_uint32_t*)(states_buffer + pd_entry->dest_offset);
                trigger_word = (vip_uint32_t *)statestmp;
                *trigger_word = ((*trigger_word) & 0x3f) | (tp_cmd_physical & ~0x3f);
            }
            break;

            #if vpmdENABLE_MULTIPLE_VIP
            case NBG_SOURCE_MULTIVIP_CHIPID:
            {
                vipOnError(patch_multivip_chipid(network, segment, states_buffer, pd_entry));
            }
            break;
            case NBG_SOURCE_MULTIVIP_SYNC_CMDS:
            {
                vipOnError(patch_multivip_sync_cmd(network, segment, states_buffer, pd_entry));
            }
            break;
            #endif

            default:
                vip_error("patch tp not support this patch souce type=%d\n", pd_entry->source_type);
                break;
            }
        }
        else if (pd_entry->type == NBG_PATCH_TYPE_COMMAND) {
            switch (pd_entry->source_type) {
            case NBG_SOURCE_INPUT:
            {
                vipOnError(patch_source_input(network, tp_cmd_logical, tp_cmd_physical, pd_entry));
            }
            break;

            case NBG_SOURCE_OUTPUT:
            {
                vipOnError(patch_source_output(network, tp_cmd_logical, tp_cmd_physical, pd_entry));
            }
            break;

            case NBG_SOURCE_MEMORY_POOL:
            {
                vipOnError(patch_source_mempool(network, tp_cmd_logical, pd_entry));
            }
            break;

            case NBG_SOURCE_AXI_SRAM:
            {
                vipOnError(patch_source_sram(network, tp_cmd_logical, pd_entry, NBG_SOURCE_AXI_SRAM));
            }
            break;

            case NBG_SOURCE_VIP_SRAM:
            {
                vipOnError(patch_source_sram(network, tp_cmd_logical, pd_entry, NBG_SOURCE_VIP_SRAM));
            }
            break;

            case NBG_SOURCE_MISC_DYNA_GENERIC:
            case NBG_SOURCE_MISC_DYNA_INPUT:
            case NBG_SOURCE_MISC_DYNA_OUTPUT:
            {
                vipOnError(patch_source_misc_data(network, segment, operation, tp_cmd_logical, pd_entry, op_index));
            }
            break;

            case NBG_SOURCE_MULTIVIP_CHIPID:
            case NBG_SOURCE_MULTIVIP_SYNC_CMDS:
                break;

            case NBG_SOURCE_COMMAND:
            default:
                vip_error("not implement this source_type=%d\n", pd_entry->source_type);
                break;
            }
        }
    }

#if vpmdENABLE_RECORD_INFO
    {
        vip_uint32_t offset = (vip_uint32_t)(tp_cmd_logical - (vip_uint8_t *)network->tpCmds.logical);
        vip_uint32_t size_tmp = nbg_get_tp_command_size(&network->nbg_desc);
        vip_uint32_t layer_id = operation->layer_id;
        nbglk_record_info_add_list(segment, offset, size_tmp, layer_id, operation->abs_op_id, op_index,
                                   RECORD_TYPE_TP_COMMAND);
    }
#endif

    return status;
onError:
    vip_error("fail to patch tp status=%d\n", status);
    return status;
}

/*
 * @brief
 * Patch the SH command data to apply the actual pool base.
 */
static vip_status_e patch_sh(
    vip_network_t *network,
    nbglk_segment_t *segment,
    nbg_desc_operation_t *operation,
    vip_uint32_t op_index,
    nbg_desc_sh_operation_t*shop,
    vip_uint8_t *cmd_buf,
    vip_uint8_t *states_buf,
    vip_uint32_t *cmd_buf_size,
    vip_uint32_t *states_size
    )
{
    vip_status_e status = VIP_SUCCESS;
    vip_uint8_t *commands_logical = VIP_NULL;
    vip_uint32_t commands_physical = 0;
    vip_uint8_t *states = VIP_NULL;
    vip_uint32_t states_physical = 0;
    vip_uint32_t i = 0;
    nbg_desc_patch_t *pd_entry = VIP_NULL;
    commands_logical = cmd_buf;
    states = states_buf;

    if (VIP_NULL == segment) {
        vip_error("segment is NULL\n");
        return VIP_ERROR_INVALID_NETWORK;
    }

    states_physical = SIZE_CAST(vip_uint32_t, segment->states_buffer.physical +
        (vip_uint32_t)(states_buf - (vip_uint8_t *)segment->states_buffer.logical));

    /* 1. Copy SH command and states. */
    viphal_copy_memory(states, network->load_states +
        network->nbg_desc.load_states_table[operation->state_id].offset,
        SIZE_CAST(vip_uint32_t, network->nbg_desc.load_states_table[operation->state_id].size));

    if (vip_true_e == network->realloc_sh_buff) {
        viphal_copy_memory(commands_logical,
                           (vip_uint8_t *)network->ppu_instr.logical +
                           network->nbg_desc.ppu_instr_table[shop->instr_index].offset,
                           SIZE_CAST(vip_uint32_t, network->nbg_desc.ppu_instr_table[shop->instr_index].size));
        if (cmd_buf_size != VIP_NULL) {
            *cmd_buf_size = VIP_ALIGN(network->nbg_desc.ppu_instr_table[shop->instr_index].size,
                vipdSH_ALIGN_SIZE);
        }
        commands_physical = SIZE_CAST(vip_uint32_t, segment->sh_cmd_buffer.physical +
            (vip_uint32_t)(commands_logical - segment->sh_cmd_buffer.logical));
    }
    else {
        commands_physical = SIZE_CAST(vip_uint32_t, network->ppu_instr.physical +
            network->nbg_desc.ppu_instr_table[shop->instr_index].offset);
    }

    if (states_size != VIP_NULL) {
        *states_size = SIZE_CAST(vip_uint32_t, network->nbg_desc.load_states_table[operation->state_id].size);
    }

#if vpmdENABLE_CHANGE_PPU_PARAM
    if (network->change_ppu_param && (shop->ppu_param_index < 0xFFFFFFFF)) {
        status = patch_ppu_params(network, states,
            SIZE_CAST(vip_uint32_t, network->nbg_desc.load_states_table[operation->state_id].size), shop);
        if (status != VIP_SUCCESS) {
            vip_error("fail to patch ppu parameters\n");
            vipGoOnError(VIP_ERROR_INVALID_NETWORK);
        }
    }
#endif

    /* 2. Patch the SH command. */
    for (i = 0; i < operation->patch_count; i++) {
        pd_entry = &network->nbg_desc.pd_sections[operation->patch_index + i];
        switch (pd_entry->source_type) {
        case NBG_SOURCE_COMMAND:  /* Patch the command address to this offset. */
        {
            vip_uint32_t *memory = (vip_uint32_t*)(states + pd_entry->dest_offset);
            vip_uint32_t cmd_physical = commands_physical;
            if (pd_entry->transformed == NBG_PATCH_TRANSFORMATION_RIGHT_SHIFT_6) {
                cmd_physical >>= 6;
            }
            *memory = cmd_physical;
        }
        break;

        case NBG_SOURCE_INPUT:
        {
            vipOnError(patch_source_input(network, states, states_physical, pd_entry));
        }
        break;

        case NBG_SOURCE_OUTPUT:
        {
            vipOnError(patch_source_output(network, states, states_physical, pd_entry));
        }
        break;

        case NBG_SOURCE_MEMORY_POOL:
        {
            vipOnError(patch_source_mempool(network, states, pd_entry));
        }
        break;

        case NBG_SOURCE_AXI_SRAM:
        {
            vipOnError(patch_source_sram(network, states, pd_entry, NBG_SOURCE_AXI_SRAM));
        }
        break;

        case NBG_SOURCE_VIP_SRAM:
        {
            vipOnError(patch_source_sram(network, states, pd_entry, NBG_SOURCE_VIP_SRAM));
        }
        break;

        case NBG_SOURCE_MISC_DYNA_GENERIC:
        case NBG_SOURCE_MISC_DYNA_INPUT:
        case NBG_SOURCE_MISC_DYNA_OUTPUT:
        {
            vipOnError(patch_source_misc_data(network, segment, operation, states, pd_entry, op_index));
        }
        break;

        #if vpmdENABLE_MULTIPLE_VIP
        case NBG_SOURCE_MULTIVIP_CHIPID:
        {
            vipOnError(patch_multivip_chipid(network, segment, states, pd_entry));
        }
        break;
        case NBG_SOURCE_MULTIVIP_SYNC_CMDS:
        {
            vipOnError(patch_multivip_sync_cmd(network, segment, states, pd_entry));
        }
        break;
        #endif

        default:
            vip_error("not implement this source_type=%d\n", pd_entry->source_type);
            break;
        }
    }

#if vpmdENABLE_RECORD_INFO
    {
        vip_uint32_t offset = 0;
        vip_uint32_t size_tmp = SIZE_CAST(vip_uint32_t, network->nbg_desc.ppu_instr_table[shop->instr_index].size);
        vip_uint32_t layer_id = operation->layer_id;
        if (vip_true_e == network->realloc_sh_buff) {
            offset = (vip_uint32_t)(commands_logical - (vip_uint8_t *)segment->sh_cmd_buffer.logical);
        }
        else {
            offset = SIZE_CAST(vip_uint32_t, network->nbg_desc.ppu_instr_table[shop->instr_index].offset);
        }
        nbglk_record_info_add_list(segment, offset, size_tmp, layer_id, operation->abs_op_id, op_index,
                                   RECORD_TYPE_SH_COMMAND);
    }
#endif

    return status;
onError:
    vip_error("fail to patch sh status=%d\n", status);
    return status;
}

static vip_status_e patch_end(
    nbg_desc_operation_t *operation,
    vip_uint8_t *states,
    vip_network_t *network,
    vip_uint32_t *size
    )
{
    vip_status_e status = VIP_SUCCESS;
    vip_uint32_t state_size = SIZE_CAST(vip_uint32_t, network->nbg_desc.load_states_table[operation->state_id].size);
    vip_uint32_t offset = SIZE_CAST(vip_uint32_t, network->nbg_desc.load_states_table[operation->state_id].offset);

    viphal_copy_memory(states, network->load_states + offset, state_size);
    if (size != VIP_NULL) {
        *size = state_size;
    }

    if (operation->patch_count > 0) {
        vip_error("end operation patch count=%d\n", operation->patch_count);
    }

    return status;
}

vip_status_e nbglk_patch_init_command(
    vip_network_t *network
    )
{
    vip_status_e status = VIP_SUCCESS;
    vip_uint32_t commands_size = 0;
    vip_uint32_t i = 0;
    vip_uint32_t j = 0;
    nbg_desc_data_table_t *icdt_entry = VIP_NULL;
    vip_uint8_t *states_buffer = VIP_NULL;
    nbg_desc_patch_t *pd_entry = VIP_NULL;
    nbg_desc_hw_init_operation_t *op = VIP_NULL;
    vip_uint32_t last_output_phy = 0;
    vip_uint32_t output_phy_orig = 0;
    vip_uint32_t last_ker_phy = 0;
    vip_uint32_t ker_phy_orig = 0;
    vip_uint8_t *instr = VIP_NULL;
    vip_uint32_t *tmp = VIP_NULL;
    vip_uint32_t index[3] = {1, 2, 0};

#if vpmdTASK_CMD_DUMP
    nbglk_network_dump_init(network);
#endif
    vip_debug("patch init command init op num=%d\n", network->nbg_desc.n_hw_init_ops);
    network->init_commands.size = CACHE_CMD_SIZE;
    for (i = 0; i< network->nbg_desc.n_hw_init_ops; i++) {
        op = &network->nbg_desc.init_ops[i];
        icdt_entry = &network->nbg_desc.ICDT[op->state_id];
        network->init_commands.size += SIZE_CAST(vip_uint32_t, icdt_entry->size);
    }

    commands_size += SIZE_CAST(vip_uint32_t, network->init_commands.size) + LINK_SIZE;
    vipOnError(viphal_allocate_videomemory(&network->init_commands.logical,
                                              &network->init_commands.mem_id,
                                              &network->init_commands.physical,
                                              commands_size,
                                              vipdMEMORY_ALIGN_SIZE,
                                              VIPDRV_VIDEO_MEM_ALLOC_MAP_KERNEL |
                                              VIPDRV_VIDEO_MEM_ALLOC_4GB_ADDR));
    viphal_zero_memory(network->init_commands.logical, commands_size);

    APPEND_FLUSH_CMDS(network->init_commands.logical);
    states_buffer = network->init_commands.logical + CACHE_CMD_SIZE;

    for (i = 0; i < network->nbg_desc.n_hw_init_ops; i++) {
        if ((network->nbg_desc.fixed.header.version < 0x0001001B) && (3 == network->nbg_desc.n_hw_init_ops)) {
            op = &network->nbg_desc.init_ops[index[i]];
            vip_debug("patch init command op index=%d\n", index[i]);
        }
        else {
            op = &network->nbg_desc.init_ops[i];
        }
        icdt_entry = &network->nbg_desc.ICDT[op->state_id];
        viphal_copy_memory(states_buffer, (vip_uint8_t *)network->ICD.logical + icdt_entry->offset,
            SIZE_CAST(vip_uint32_t, icdt_entry->size));

        for (j = 0; j < op->patch_count; j++) {
            pd_entry = &network->nbg_desc.pd_sections[op->patch_index + j];
            if (pd_entry->type == NBG_PATCH_TYPE_STATE) {
                vip_uint32_t *memory = (vip_uint32_t*)(states_buffer + pd_entry->dest_offset);
                switch (pd_entry->source_type) {
                case NBG_SOURCE_COMMAND:
                {
                    nbg_desc_data_table_t *icdt = &network->nbg_desc.ICDT[pd_entry->index];
                    vip_uint32_t physical = SIZE_CAST(vip_uint32_t, network->ICD.physical + icdt->offset);
                    #if vpmdTASK_CMD_DUMP
                    vip_info("init%d cmd dump instr\n", i);
                    vipOnError(nbglk_network_dump_data(network, network->ICD.logical + icdt->offset,
                                icdt->size));
                    #endif
                    if (pd_entry->transformed == NBG_PATCH_TRANSFORMATION_RIGHT_SHIFT_6) {
                        vip_uint32_t *trigger_word = VIP_NULL;
                        trigger_word = (vip_uint32_t *)memory;
                        *trigger_word = ((*trigger_word) & 0x3f) | (physical & ~0x3f);
                    }
                    else {
                        *memory = physical;
                    }
                    instr = network->ICD.logical + icdt->offset;
                }
                break;

                case NBG_SOURCE_MISC_DYNA_INPUT:
                case NBG_SOURCE_MISC_DYNA_OUTPUT:
                {
                    nbg_desc_data_table_t *icdt = &network->nbg_desc.ICDT[pd_entry->index];
                    vip_uint32_t physical = SIZE_CAST(vip_uint32_t, network->ICD.physical + icdt->offset);
                    *memory = physical;
                    #if vpmdTASK_CMD_DUMP
                    if (NBG_SOURCE_MISC_DYNA_INPUT == pd_entry->source_type) {
                        vip_info("init%d cmd dump input\n", i);
                        vipOnError(nbglk_network_dump_data(network, network->ICD.logical + icdt->offset,
                                 icdt->size));
                    }
                    #endif
                }
                break;

                default:
                    vip_error("not support this source_type=%d\n", pd_entry->source_type);
                    break;
                }
            }
            else if (pd_entry->type == NBG_PATCH_TYPE_COMMAND) {
                if (instr != VIP_NULL) {
                    vip_uint32_t *memory = (vip_uint32_t*)(instr + pd_entry->dest_offset);
                    switch (pd_entry->source_type) {
                    case NBG_SOURCE_MISC_DYNA_INPUT:
                    {
                        nbg_desc_data_table_t *icdt = &network->nbg_desc.ICDT[pd_entry->index];
                        vip_uint32_t physical = SIZE_CAST(vip_uint32_t, network->ICD.physical + icdt->offset);
                        *memory = physical;
                        #if vpmdTASK_CMD_DUMP
                        vip_info("init%d cmd dump input\n", i);
                        vipOnError(nbglk_network_dump_data(network, network->ICD.logical + icdt->offset,
                                icdt->size));
                        #endif
                    }
                    break;

                    case NBG_SOURCE_MISC_DYNA_OUTPUT:
                    {
                        nbg_desc_data_table_t *icdt = &network->nbg_desc.ICDT[pd_entry->index];
                        vip_uint32_t physical = SIZE_CAST(vip_uint32_t, network->ICD.physical + icdt->offset);
                        if (last_output_phy != physical) {
                            output_phy_orig = *memory;
                            *memory = physical;
                            last_output_phy = physical;
                        }
                        else if (output_phy_orig != 0) {
                            *memory = physical + (*memory - output_phy_orig);
                        }
                        else {
                            *memory = physical;
                        }
                    }
                    break;

                    case NBG_SOURCE_MISC_DYNA_GENERIC:
                    {
                        nbg_desc_data_table_t *icdt = &network->nbg_desc.ICDT[pd_entry->index];
                        vip_uint32_t physical = SIZE_CAST(vip_uint32_t, network->ICD.physical + icdt->offset);
                        #if vpmdTASK_CMD_DUMP
                        vip_info("init%d cmd dump coeff\n", i);
                        vipOnError(nbglk_network_dump_data(network, network->ICD.logical + icdt->offset,
                                icdt->size));
                        #endif
                        if (pd_entry->transformed == NBG_PATCH_TRANSFORMATION_RIGHT_SHIFT_6) {
                            vip_uint32_t *trigger_word = VIP_NULL;
                            trigger_word = (vip_uint32_t *)memory;
                            physical >>= 6;
                            *trigger_word = ((*trigger_word) & 0xfc000000) | physical;
                        }
                        else {
                            if (last_ker_phy != physical) {
                                ker_phy_orig = *memory;
                                *memory = physical;
                                last_ker_phy = physical;
                            }
                            else if (ker_phy_orig != 0) {
                                *memory = physical + (*memory - ker_phy_orig);
                            }
                            else {
                                *memory = physical;
                            }
                        }
                    }
                    break;

                    default:
                        vip_error("not support this source_type=%d\n", pd_entry->source_type);
                        break;
                    }
                }
                else {
                    vip_error("fail to patch command, op index=%d\n", i);
                    vipOnError(VIP_ERROR_FAILURE);
                }
            }
        }

        states_buffer += icdt_entry->size;
        instr = VIP_NULL;
    }

    /* remove remap vip-sram/axi-sram cmds */
    tmp = (vip_uint32_t*)network->init_commands.logical;
    for (i = 0; i < network->init_commands.size / sizeof(vip_uint32_t); i++) {
        if ((0x08010E4E == tmp[i]) || (0x08010E4F == tmp[i]) || (0x08010E50 == tmp[i])) {
            tmp[i] = 0x18000000;
            tmp[i + 1] = 0x00;
        }
    }
#if vpmdENABLE_VIDEO_MEMORY_CACHE
    vipOnError(viphal_flush_cache(network->ICD.mem_id, VIPDRV_CACHE_FLUSH));
    vipOnError(viphal_flush_cache(network->init_commands.mem_id, VIPDRV_CACHE_FLUSH));
#endif
    vipOnError(viphal_task_set_property(network->task_id, VIPDRV_TASK_CMD_INIT,
        0, network->init_commands.mem_id, SIZE_CAST(vip_uint32_t, network->init_commands.size), 0));

#if vpmdTASK_CMD_DUMP
    vipOnError(nbglk_network_dump_cmd(network, network->init_commands.logical, network->init_commands.size));
#endif
onError:
#if vpmdTASK_CMD_DUMP
    nbglk_network_dump_uninit(network);
#endif
    return status;
}

#if vpmdENABLE_CANCELATION > 1
vip_uint32_t nbglk_append_breakpoint(
    vip_uint32_t *command
    )
{
    command[0] = (3 << 27);
    command[1] = 0;
    return BREAKPOINT_SIZE;
}

vip_uint32_t nbglk_breakpoint_get_count(
     vip_network_t *network,
     nbglk_segment_t *segment
    )
{
    vip_status_e status = VIP_SUCCESS;
    vip_uint32_t i = 0;
    vip_uint32_t bp_count = 0;
    nbg_desc_operation_t *operation = VIP_NULL;
    nbg_desc_operation_t *next_operation = VIP_NULL;

    for (i = 0; i < network->nbg_desc.n_operations - 1; i++) {
        operation = &network->nbg_desc.operations[i];
        next_operation = &network->nbg_desc.operations[i + 1];

        CHECK_OPERATION_IN_SEGMENT(operation, segment, i);

        switch (operation->type) {
        case NBG_OPERATION_TYPE_NN:
        case NBG_OPERATION_TYPE_TP:
        case NBG_OPERATION_TYPE_SH:
        case NBG_OPERATION_TYPE_SP:
        case NBG_OPERATION_TYPE_NN_LUT:
        case NBG_OPERATION_TYPE_NT:
        {
            if (operation->layer_id != next_operation->layer_id) {
                bp_count++;
            }
        }
        break;

        default:
            break;
        }
    }

    segment->bp_count = bp_count;
    vip_info("insert breakpoint count=%d\n", bp_count);

    if (segment->bp_count > 0) {
        vipOnError(viphal_allocate_memory(sizeof(vip_uint32_t) * segment->bp_count,
                                            (void **)&segment->bp_pos_table));
    }

onError:
    return segment->bp_count;
}

vip_status_e nbglk_breakpoint_change_p(
    vip_network_t *network
    )
{
    vip_status_e status = VIP_SUCCESS;
    vip_uint8_t *states_buffer = VIP_NULL;
    vip_uint32_t i = 0;
    vip_uint32_t j = 0;

    for (i = 0; i < network->segment_num; i++) {
        nbglk_segment_t *segment = &network->segments[i];
        #if (vpmdENABLE_DEBUG_LOG == 4)
        vip_uint32_t address = 0, dmaLo = 0, dmaHi = 0;
        vip_uint32_t dev_index = network->device->dev_index;
        vip_uint32_t core_index = network->core_index;
        viphal_read_reg(dev_index, core_index, 0x00668, &dmaLo);
        viphal_read_reg(dev_index, core_index, 0x0066C, &dmaHi);
        viphal_read_reg(dev_index, core_index, 0x00664, &address);
        vip_info("fe address=0x%08x, dmalo=0x%08x, dmahi=0x%08x\n", address, dmaLo, dmaHi);
        #endif
        vip_info("p command size=0x%x, memory mem_id=0x%x, physical=0x%llx\n",
                   segment->states_buffer.size, segment->states_buffer.mem_id, segment->states_buffer.physical);
        if ((segment->bp_count > 0) && (vip_false_e == segment->is_sw_segm)) {
            states_buffer = (vip_uint8_t *)segment->states_buffer.logical;
            for (j = 0; j < segment->bp_count; j++) {
                nbglk_append_link((vip_uint32_t*)(segment->bp_pos_table[j] + states_buffer),
                    network->stop_cmds.physical);
            }
        }
        #if vpmdENABLE_VIDEO_MEMORY_CACHE
        viphal_flush_cache(segment->states_buffer.mem_id, VIPDRV_CACHE_FLUSH);
        #endif
    }

    return status;
}

vip_status_e nbglk_breakpoint_change_n(
    vip_network_t *network
    )
{
    vip_status_e status = VIP_SUCCESS;
    vip_uint8_t *states_buffer = VIP_NULL;
    vip_uint32_t i = 0;
    vip_uint32_t j = 0;

    for (i = 0; i < network->segment_num; i++) {
        nbglk_segment_t *segment = &network->segments[i];
        vip_info("n command size=0x%x, memory mem_id=0x%x, physical=0x%llx\n",
                   segment->states_buffer.size, segment->states_buffer.mem_id, segment->states_buffer.physical);
        if ((segment->bp_count > 0) && (vip_false_e == segment->is_sw_segm)) {
            states_buffer = (vip_uint8_t *)segment->states_buffer.logical;
            for (j = 0; j < segment->bp_count; j++) {
                nbglk_append_breakpoint((vip_uint32_t*)(segment->bp_pos_table[j] + states_buffer));
            }
        }
        #if vpmdENABLE_VIDEO_MEMORY_CACHE
        viphal_flush_cache(segment->states_buffer.mem_id, VIPDRV_CACHE_FLUSH);
        #endif
    }

    return status;
}

vip_status_e nbglk_breakpoint_init(
    vip_network_t *network
    )
{
    #define END_CMDS_SIZE  32
    vip_status_e status = VIP_SUCCESS;
    vip_uint32_t *logical = VIP_NULL;
    vip_uint32_t count = 0;
    vip_uint32_t buffer_size = 0;

    network->stop_cmds.size = END_CMDS_SIZE + EVENT_SIZE + END_SIZE;
    buffer_size = VIP_ALIGN(network->stop_cmds.size, vpmdCPU_CACHE_LINE_SIZE);
    vipOnError(viphal_allocate_videomemory(&network->stop_cmds.logical, &network->stop_cmds.mem_id,
                                              &network->stop_cmds.physical, buffer_size,
                                              vipdMEMORY_ALIGN_SIZE, VIPDRV_VIDEO_MEM_ALLOC_4GB_ADDR));
    logical = (vip_uint32_t*)network->stop_cmds.logical;
    logical[count++] = 0x08010E03;
    logical[count++] = 0x00000C20;
    logical[count++] = 0x08010E02;
    logical[count++] = 0x30000701;
    logical[count++] = 0x48000000;
    logical[count++] = 0x30000701;
    logical[count++] = 0x08010E03;
    logical[count++] = 0x00000C23;
    logical[count++] = 0x08010E01;
    logical[count++] = 0x00000044;
    logical[count++] = 0x10000000;
    logical[count++] = 0x00;

#if vpmdENABLE_VIDEO_MEMORY_CACHE
    viphal_flush_cache(network->stop_cmds.mem_id, VIPDRV_CACHE_FLUSH);
#endif

    return status;
onError:
    vip_error("fail to initialize breakpoint\n");
    return status;
}

vip_status_e nbglk_breakpoint_uninit(
    vip_network_t *network
    )
{
    if (network->stop_cmds.mem_id != 0) {
        viphal_free_videomemory(network->stop_cmds.mem_id);
        network->stop_cmds.mem_id = 0;
    }
    return VIP_SUCCESS;
}
#endif

/*
@ brief
    insert link command in command buffer
*/
vip_status_e nbglk_insert_link(
    nbglk_segment_t *segment
    )
{
    vip_status_e status = VIP_SUCCESS;
    vip_uint8_t i = 0;
    vip_uint8_t *command = (vip_uint8_t *)segment->states_buffer.logical;
    vip_uint32_t physical = SIZE_CAST(vip_uint32_t, segment->states_buffer.physical);
    vip_uint32_t link_physical = 0;
    vip_uint32_t location = 0;
    vip_uint32_t total = 0;
    vip_uint32_t *split_size_table = segment->split_size_table;
    vip_uint32_t *patch_loc = segment->split_pos_table;
    vip_uint32_t *tmp = VIP_NULL;

    if (segment->split_num < 1) {
        return VIP_SUCCESS;
    }

    total = split_size_table[0];
    vip_info("index=0, command buffer phy=0x%08X, link command size=0x%X, split num=%d\n",
                physical, total, segment->split_num);
    for (i = 0; i < segment->split_num; i++) {
        location = patch_loc[i];
        tmp = (vip_uint32_t*)(command + location);
        link_physical = physical + location + LINK_SIZE;
        vip_info("index=%d, insert link location=0x%08X, link phy=0x%08X\n",
                    i, location, link_physical);
        if ((tmp[0] || tmp[1]) && !(tmp[0] & 0x40000000)) {
            vip_error("warning location value=0x%08x, 0x%08x\n", tmp[0], tmp[1]);
        }
        nbglk_append_link(tmp, link_physical);
        total += split_size_table[i + 1];
    }

    if (segment->states_buffer.size != total) {
        vip_error("fail to insert link command total size=0x%X, submit->size=0x%08X\n",
                    total, segment->states_buffer.size);
        return VIP_ERROR_NOT_SUPPORTED;
    }

    return status;
}

vip_uint32_t *nbglk_append_link(
    vip_uint32_t *command,
    vip_uint32_t address
    )
{
    command[1] = address;
    command[0] = ((8 << 27) | (vipdMAX_CMD_SIZE >> 3));
    return &command[0];
}

vip_uint32_t nbglk_append_nop(
    vip_uint32_t *command
    )
{
    command[0] = (0x3 << 27) | 0x1;
    command[1] = 0;

    return NOP_CMD_SIZE;
}

#if !vpmdTASK_PARALLEL_ASYNC
/*
@brief append END command at the command address.
*/
static vip_uint32_t nbglk_append_end(
    vip_uint32_t *command
    )
{
    command[0] = (2 << 27);
    command[1] = 0;
    return END_SIZE;
}

/*
@brief append EVENT command at the command address.
*/
static vip_uint32_t nbglk_append_event(
    vip_uint32_t *command,
    vip_uint32_t block,
    vip_uint32_t id
    )
{
    command[0] = ((1 << 27) | (1 << 16) | (0x0E01));
    command[1] = ((id & 0x0000001F) | (block));
    return 8;
}
#endif

vip_status_e nbglk_append_cmds(
    nbglk_videomemory_t *cmd_memory,
    vip_uint32_t *cmd_size,
    vip_bool_e with_event,
    vip_bool_e is_multi,
    vip_uint32_t core_id
    )
{
    vip_uint32_t size = *cmd_size;
    vip_uint32_t input_size = *cmd_size;
    vip_uint32_t *logical = (vip_uint32_t*)(cmd_memory->logical + input_size);
    vip_uint32_t count = 0;

    if (VIP_NULL == logical) {
        vip_error("logical out of memory\n");
        return VIP_ERROR_OUT_OF_MEMORY;
    }

    /* semphore stall*/
    logical[count++] = 0x08010e02;
    logical[count++] = 0x30000701;
    logical[count++] = 0x48000000;
    logical[count++] = 0x30000701;
    logical[count++] = 0x08010E03;
    logical[count++] = 0x00000C23;
    size += count * sizeof(vip_uint32_t);

#if !vpmdTASK_PARALLEL_ASYNC
    /* enable core id */
    if (is_multi && with_event) {
        size += nbglk_append_chip_enable((vip_uint32_t*)(cmd_memory->logical + size), core_id, vip_false_e);
    }

   /* Append the PE event command. */
    if (with_event) {
        size += nbglk_append_event((vip_uint32_t*)(cmd_memory->logical + size), (1 << 6), IRQ_EVENT_ID);
    }

    /* enable all cores */
    if (is_multi && with_event) {
        size += nbglk_append_chip_enable((vip_uint32_t*)(cmd_memory->logical + size), core_id, vip_true_e);
    }

    size += nbglk_append_end((vip_uint32_t*)(cmd_memory->logical + size));
#endif

#if vpmdENABLE_VIDEO_MEMORY_CACHE
    if ((size - input_size) > 0) {
        vip_status_e status = VIP_SUCCESS;
        status = viphal_flush_cache(cmd_memory->mem_id, VIPDRV_CACHE_FLUSH);
        if (status != VIP_SUCCESS) {
            vip_error("fail to flush cache\n");
            return status;
        }
    }
#endif

    *cmd_size = size;

    return VIP_SUCCESS;
}


/*
 * @brief
 * Go through all operations, copy and patch them into the command buffer,
   and generate control states.
 */
vip_status_e nbglk_generate_command(
    vip_network_t *network,
    nbglk_segment_t *segment
    )
{
    vip_uint8_t *sh_commands = VIP_NULL;
    vip_uint8_t *states_buffer = VIP_NULL;
    vip_uint8_t *states_base = VIP_NULL;
    nbg_desc_operation_t *operation = VIP_NULL;
    vip_uint8_t *op_tp = VIP_NULL;
    void *op_sh = VIP_NULL;
    vip_uint8_t *op_nn = VIP_NULL;
    vip_uint32_t cmd_size = 0;
    vip_uint32_t states_size = 0;
    vip_uint32_t total_states_size = 0;
    vip_uint32_t i = 0;
    vip_uint32_t states_index = 0;
    vip_status_e status = VIP_SUCCESS;
    vip_bool_e with_event = !vpmdENABLE_POLLING;
#if vpmdENABLE_CANCELATION > 1
    vip_uint32_t bp_index = 0;
#endif

#if vpmdENABLE_LAYER_DUMP
    nbglk_nld_operation_t *nld_operation = VIP_NULL;
    vip_uint32_t insert_index = 0xFFFFFFFF;
    vip_uint32_t states_physical_base = 0;
    vip_uint8_t *nld_states_buffer = VIP_NULL;
    vip_uint8_t *nld_states_base = VIP_NULL;
#endif

    if ((segment != VIP_NULL) && (network != VIP_NULL)) {
        sh_commands = (vip_uint8_t *)segment->sh_cmd_buffer.logical;
        states_buffer = (vip_uint8_t *)segment->states_buffer.logical;
        #if vpmdENABLE_LAYER_DUMP
        states_physical_base = SIZE_CAST(vip_uint32_t, segment->states_buffer.physical);
        #endif
    }
    else {
        vip_error("segment or network is NULL");
        vipGoOnError(VIP_ERROR_INVALID_NETWORK);
    }

    if (VIP_NULL == states_buffer) {
        vip_error("states buffer is NULL");
        vipGoOnError(VIP_ERROR_INVALID_NETWORK);
    }

    segment->patch_count = 0;
    states_base = states_buffer;
    APPEND_FLUSH_CMDS(states_buffer);
    states_buffer += CACHE_CMD_SIZE;
    total_states_size += CACHE_CMD_SIZE;

    #if vpmdENABLE_LAYER_DUMP
    if (network->nld.enable) {
        nbglk_nld_first_dump_op(network, segment, &nld_states_buffer, &nld_operation, &insert_index);
    }
    nld_states_base = nld_states_buffer;
    #endif

    /* Go through all operations to generate commands of TP, NN, SH */
    for (i = 0; i < network->nbg_desc.n_operations; i++) {
        operation = &network->nbg_desc.operations[i];

        CHECK_OPERATION_IN_SEGMENT(operation, segment, i);
        states_size = 0;
        #if vpmdENABLE_LAYER_DUMP
        SET_NLD_INSERT_FLAG();
        #endif
        switch (operation->type) {
        case NBG_OPERATION_TYPE_NN:
        case NBG_OPERATION_TYPE_SP:
        case NBG_OPERATION_TYPE_NN_LUT:
        case NBG_OPERATION_TYPE_NT:
        {
            vip_uint32_t event_id = 0;
            op_nn = network->nnCmds.logical +
                nbg_get_nn_command_size(&network->nbg_desc) * operation->index;
            vipOnError(patch_nn(network, segment, operation, i, op_nn, states_buffer,
                                &states_size, &event_id));
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
            vipOnError(patch_tp(network, segment, operation, i, (vip_uint8_t_ptr)op_tp, states_buffer,
                                &states_size));
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
            op_sh = &network->nbg_desc.sh_ops[operation->index];
            vipOnError(patch_sh(network, segment, operation, i, op_sh, sh_commands, states_buffer,
                               &cmd_size, &states_size));
            if (vip_true_e == network->realloc_sh_buff) {
                sh_commands += cmd_size;
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

        case NBG_OPERATION_TYPE_SC:
        {
            vip_warning("not implement sc operation\n");
        }
        break;

        case NBG_OPERATION_TYPE_INIT:
        break;

        case NBG_OPERATION_TYPE_END:
        {
            vipOnError(patch_end(operation, states_buffer, network, &states_size));
            states_buffer += states_size;
        }
        break;

        case NBG_OPERATION_TYPE_SW:
            break;

        default:
            vip_error("unsupport operation type=%d\n", operation->type);
            break;
        }

        #if vpmdENABLE_LAYER_DUMP
        if (network->nld.enable) {
            vipOnError(nbglk_nld_patch(network, segment, operation, &states_buffer, &states_size,
                i, &insert_index, &nld_operation, &nld_states_buffer, nld_states_base,
                states_physical_base, states_base));
        }
        #endif

        /* reserved LINK bits in states buffer for patch it in submit function */
        if ((segment->operation_pos != VIP_NULL) &&
            (i == segment->operation_pos[states_index]) && (i != 0)) {
            vip_uint32_t *memory = segment->split_pos_table;
            vip_uint32_t *split_cmd_size = segment->split_size_table;
            memory[states_index] = (vip_uint32_t)(states_buffer - (vip_uint8_t *)states_base);
            states_buffer += LINK_SIZE;
            total_states_size += LINK_SIZE;
            if (states_index > 0) {
                if ((memory[states_index] - memory[states_index - 1]) != split_cmd_size[states_index]) {
                    vip_error("fail to split command buffer. index=%d.\n", states_index);
                    vipGoOnError(VIP_ERROR_NOT_SUPPORTED);
                }
            }
            else {
                if ((memory[states_index] + LINK_SIZE) != split_cmd_size[states_index]) {
                    vip_error("fail to split command buffer, size not match. index=%d.\n", states_index);
                    vipGoOnError(VIP_ERROR_NOT_SUPPORTED);
                }
            }
            states_index++;
        }
        total_states_size += states_size;
    }

    #if vpmdENABLE_LAYER_DUMP
    if (network->nld.enable) {
        nbglk_nld_show_operation(network);
    }
    #endif

    if (total_states_size != segment->states_buffer.size) {
        vip_warning("fail to generate hardware commmand, total command size=0x%xbyte "
                      "not equal buffer size 0x%xbyte\n",
                       total_states_size, segment->states_buffer.size);
        if (0 == total_states_size || total_states_size > segment->states_buffer.size) {
            vipGoOnError(VIP_ERROR_NOT_SUPPORTED);
        }
    }

    /* insert link command */
    if (segment->split_num > 0) {
        status = nbglk_insert_link(segment);
        if (status != VIP_SUCCESS) {
            vip_error("fail to patch link cmd in command buffer\n");
            vipGoOnError(status);
        }
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

    /* sanity check vip-sram used out of bound or not */
    if ((network->max_vipsram_offset > network->context->vip_sram_size)
    #if vpmdPRELOAD_COEFF > 1
    || ((network->preload_cmd.mem_id != 0) && (network->max_vipsram_offset > network->nbg_desc.mem_info.vip_sram_size))
    #endif
    )
    {
        vip_error("fail network real used vip-sram=0x%x, vip_sram_size=0x%x/0x%x\n",
             network->max_vipsram_offset, network->context->vip_sram_size, network->nbg_desc.mem_info.vip_sram_size);
        vipGoOnError(VIP_ERROR_NOT_SUPPORTED);
    }

    return status;
onError:
    vip_error("fail to generate cmd patch op=%d\n", i);
    return status;
}


#if vpmdDYNAMIC_CHANGE_MEM_POOL
vip_status_e nbglk_patch_memory_pool(
    vip_network_t *network,
    vip_buffer buffer
    )
{
    vip_status_e status = VIP_SUCCESS;
    vip_uint32_t i = 0;
    nbglk_memory_pool_info_t* info = VIP_NULL;

    if (vip_false_e == network->prepared_network) {
        return status;
    }

    if (network->dup_type != NBGLK_FROM_NETWORK_DUP_COMMAND) {
        /* free pool if allocated by driver */
        if ((network->pool.logical != VIP_NULL) && (VIP_NULL == network->memory_pool)) {
            viphal_free_videomemory(network->pool.mem_id);
            network->pool.logical = VIP_NULL;
        }
    }

    network->memory_pool = buffer;
    viphal_copy_memory(&network->pool, &buffer->memory, sizeof(nbglk_videomemory_t));

    for (i = 0; i < network->pool_patch_count; i++) {
        vip_uint32_t* cmd_logical = VIP_NULL;
        info = &network->pool_patch_info[i];
        cmd_logical = (vip_uint32_t*)info->cmd_logical;

        if (info->sw_layer) {
            if (VIP_NULL == network->pool.logical) {
                network->pool.logical = vip_map_buffer(buffer);
                if (VIP_NULL == network->pool.logical) {
                    vip_error("fail to map user logical for new memory pool of sw layer\n");
                    status = VIP_ERROR_FAILURE;
                }
            }
            *cmd_logical = (vip_uint32_t)(viphal_uintptr_t)(network->pool.logical + info->info.pool_offset);
        }
        else {
            do_patch_source_mempool(network->pool.physical, info->cmd_logical, info->info.patch);
        }
    }

#if vpmdENABLE_VIDEO_MEMORY_CACHE
    if (network->nnCmds.size > 0) {
        viphal_flush_cache(network->nnCmds.mem_id, VIPDRV_CACHE_FLUSH);
    }
    if (network->tpCmds.size > 0) {
        viphal_flush_cache(network->tpCmds.mem_id, VIPDRV_CACHE_FLUSH);
    }
    for (i = 0; i < network->segment_num; i++) {
        nbglk_segment_t *segment = &network->segments[i];
        if (vip_false_e == segment->is_sw_segm) {
            if (segment->states_buffer.size > 0) {
                viphal_flush_cache(segment->states_buffer.mem_id, VIPDRV_CACHE_FLUSH);
            }
        }
    }
#endif

    return status;
}
#endif
