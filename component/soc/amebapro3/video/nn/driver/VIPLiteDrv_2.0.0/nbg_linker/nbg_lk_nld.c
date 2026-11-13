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
#include <nbg_lk_nld.h>
#include <nbg_lk_network.h>
#include <vip_hal_os_port.h>
#include <nbg_fmt_util.h>

#if vpmdENABLE_LAYER_DUMP
nbg_desc_operation_t *nbglk_get_operation_ptr_by_index(
    vip_network_t *network,
    vip_uint32_t index
    )
{
    nbg_desc_operation_t *ptr = VIP_NULL;

    ptr = network->nbg_desc.operations + index;

    return ptr;
}

nbg_desc_ldump_operation_t *nbglk_get_ldump_operation_ptr_by_index(
    vip_network_t *network,
    vip_int32_t index
    )
{
    nbg_desc_ldump_operation_t *ptr = VIP_NULL;

    ptr = network->nbg_desc.ldump_operation + index;

    return ptr;
}

nbg_desc_layer_t *nbglk_get_layer_ptr_by_index(
    vip_network_t *network,
    vip_uint32_t layer_index
    )
{
    nbg_desc_layer_t *orig_layer = VIP_NULL;

    orig_layer = &network->nbg_desc.orig_layers[layer_index];

    return orig_layer;
}
nbg_desc_patch_t *nbglk_get_ldump_patch_data_ptr_by_index(
    vip_network_t *network,
    vip_int32_t index
    )
{
    nbg_desc_patch_t *ptr = VIP_NULL;

    ptr = network->nbg_desc.ldump_patch_data + index;

    return ptr;
}

nbg_desc_ldump_output_t *nbglk_get_ldump_output_ptr_by_index(
    vip_network_t *network,
    vip_int32_t index
    )
{
    nbg_desc_ldump_output_t *ptr = VIP_NULL;

    ptr = network->nbg_desc.ldump_outputs + index;

    return ptr;
}

vip_uint32_t get_nld_instr_size(
    vip_network_t *network
    )
{
    vip_uint32_t size = 0;

    size = SIZE_CAST(vip_uint32_t, network->nbg_desc.fixed.ldump_instr.size);

    return size;
}

vip_uint32_t get_nld_loadstate_size(
    vip_network_t *network
    )
{
    vip_uint32_t size = 0;
    nbglk_nld_operation_t *entry = VIP_NULL;

    NBGLK_LIST_FOR_EACH_ENTRY(nbglk_nld_operation_t, entry, &network->nld.operation_head, list) {
        nbg_desc_ldump_operation_t *ldump_operation = VIP_NULL;
        ldump_operation = nbglk_get_ldump_operation_ptr_by_index(network, entry->index);
        size += SIZE_CAST(vip_uint32_t,
            network->nbg_desc.ldump_load_states_table[ldump_operation->loadstate_index].size);
    }

    return size;
}

/* Get insert nop command into states size. */
vip_uint32_t nbglk_nld_states_size(
    vip_network_t *network,
    nbglk_segment_t *segment
    )
{
    vip_uint32_t insert_link_size = 0;
    nbg_desc_operation_t *op = VIP_NULL;
    vip_uint32_t i = 0;

#define NORMAL_RUN                                                 \
if ((op->type != NBG_OPERATION_TYPE_INIT) &&                     \
    (i < segment->segm_end_id) && (i >= segment->segm_start_id)) { \
    insert_link_size += LINK_SIZE;  \
}
   for (i = 0; i < network->nbg_desc.n_operations; i++) {
       op = nbglk_get_operation_ptr_by_index(network, i);
        if (VIP_NULL == op) {
            vip_error("op is NULL\n");
            return 0;
        }
       /* init command doesn't in command buffer */
       NORMAL_RUN
   }

    vip_debug("get layer dump insert link size=0x%x\n", insert_link_size);

    return insert_link_size;

#undef NORMAL_RUN
}

vip_uint32_t get_nld_pool_patch_count(
    vip_network_t *network
    )
{
    vip_uint32_t count = 0, i = 0, j = 0;

    for (i = 0; i < network->nbg_desc.n_ldump_operation; i++) {
        nbg_desc_ldump_operation_t *ldump_operation = VIP_NULL;
        vip_uint32_t patch_count = 0;
        ldump_operation = nbglk_get_ldump_operation_ptr_by_index(network, i);
        patch_count = ldump_operation->patch_count;
        for (j = 0; j < patch_count; j++) {
            nbg_desc_patch_t *patch_data = VIP_NULL;
            vip_uint32_t source_type = 0;
            patch_data = nbglk_get_ldump_patch_data_ptr_by_index(network, ldump_operation->start_patch_index + j);
            source_type = patch_data->source_type;
            if (NBG_SOURCE_MEMORY_POOL == source_type) {
                count++;
            }
        }
    }

    return count;
}

vip_uint32_t nbglk_nld_operation_count(
    vip_network network
    )
{
    vip_uint32_t count = 0;
    nbglk_nld_operation_t *entry = VIP_NULL;
    NBGLK_LIST_FOR_EACH_ENTRY(nbglk_nld_operation_t, entry, &network->nld.operation_head, list) {
        count++;
    }

    return count;
}

vip_status_e patch_nld(
    vip_network_t *network,
    nbglk_nld_operation_t *nld_operation,
    vip_uint8_t *state_buf,
    vip_uint32_t *state_bufSize,
    nbglk_segment_t *segment
    )
{
    vip_status_e status = VIP_SUCCESS;
    vip_uint32_t i = 0;
    vip_uint8_t *states = VIP_NULL;
    nbg_desc_ldump_operation_t *ldump_operation = VIP_NULL;
    nbg_desc_patch_t *pd_entry = VIP_NULL;

    states = state_buf;

    if (VIP_NULL == segment) {
        vip_error("segment is NULL\n");
        return VIP_ERROR_INVALID_NETWORK;
    }

    ldump_operation = nbglk_get_ldump_operation_ptr_by_index(network, nld_operation->index);

    viphal_copy_memory(states, (vip_uint8_t *)network->nbg_desc.ldump_load_states_logical +
                    network->nbg_desc.ldump_load_states_table[ldump_operation->loadstate_index].offset,
                    network->nbg_desc.ldump_load_states_table[ldump_operation->loadstate_index].size);

    if (state_bufSize != VIP_NULL) {
        *state_bufSize = SIZE_CAST(vip_uint32_t,
            network->nbg_desc.ldump_load_states_table[ldump_operation->loadstate_index].size);
    }

    for (i = 0; i < ldump_operation->patch_count; i++) {
        pd_entry = (nbg_desc_patch_t *)nbglk_get_ldump_patch_data_ptr_by_index(network,
                                                                    ldump_operation->start_patch_index + i);
        switch (pd_entry->source_type) {
        case NBG_SOURCE_COMMAND:
        {
            vip_uint32_t *memory = (vip_uint32_t*)(states + pd_entry->dest_offset);
            vip_uint32_t commands_physical = 0;
            commands_physical = SIZE_CAST(vip_uint32_t, network->nld.instr_memory.physical +
                network->nbg_desc.ldump_instr_table[pd_entry->index].offset);
            nld_operation->instr.physical = commands_physical;
            nld_operation->instr.logical = network->nld.instr_memory.logical +
                                (vip_uint32_t)(network->nbg_desc.ldump_instr_table[pd_entry->index].offset);
            nld_operation->instr.size = SIZE_CAST(vip_uint32_t,
                network->nbg_desc.ldump_instr_table[pd_entry->index].size);

            *memory = commands_physical;
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

        case NBG_SOURCE_VIP_SRAM:
        {
            vipOnError(patch_source_sram(network, states, pd_entry, NBG_SOURCE_VIP_SRAM));
        }
        break;

        case NBG_SOURCE_LAYER_DUMP_OUTPUT:
        {
            vipOnError(patch_nld_output(network, segment, nld_operation, states, pd_entry));
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

        case NBG_SOURCE_INPUT:
        case NBG_SOURCE_OUTPUT:
        case NBG_SOURCE_MISC_DYNA_GENERIC:
        case NBG_SOURCE_MISC_DYNA_INPUT:
        case NBG_SOURCE_MISC_DYNA_OUTPUT:
        {
            vip_error("not support this source_type=%d\n", pd_entry->source_type);
            vipOnError(VIP_ERROR_NOT_SUPPORTED);
        }
        break;

        default:
            vip_error("not implement this source_type=%d\n", pd_entry->source_type);
            break;
        }
    }

    return status;

onError:
    vip_error("fail to patch layer dump\n");
    return status;
}

vip_status_e nbglk_nld_patch(
    vip_network_t *network,
    nbglk_segment_t *segment,
    nbg_desc_operation_t *operation,
    vip_uint8_t **states_buffer_ptr,
    vip_uint32_t *states_size_ptr,
    vip_uint32_t op_index,
    vip_uint32_t *insert_index_ptr,
    nbglk_nld_operation_t **nld_operation_ptr,
    vip_uint8_t **nld_states_buffer_ptr,
    vip_uint8_t *nld_states_base,
    vip_uint32_t states_physical_base,
    vip_uint8_t *states_base
    )
{
    vip_status_e status = VIP_SUCCESS;

    vip_uint8_t *states_buffer = VIP_NULL;
    vip_uint32_t states_size = 0;
    vip_uint32_t insert_index = 0;
    nbglk_nld_operation_t *nld_operation = VIP_NULL;
    vip_uint8_t *nld_states_buffer = VIP_NULL;

    if (network->nld.status) {
        vip_uint32_t append_nop_size = 0;
        vip_uint32_t states_physical_location = 0;

        states_buffer = *states_buffer_ptr;
        states_size = *states_size_ptr;
        insert_index = *insert_index_ptr;
        nld_operation = *nld_operation_ptr;
        nld_states_buffer = *nld_states_buffer_ptr;

        if (operation->type != NBG_OPERATION_TYPE_INIT) {
            append_nop_size = nbglk_append_nop((vip_uint32_t *)states_buffer);
            if (NOP_CMD_SIZE != append_nop_size) {
                vip_error("append_nop_size=%d\n", append_nop_size);
                vipOnError(VIP_ERROR_FAILURE);
            }
            states_buffer += NOP_CMD_SIZE;
            states_size += NOP_CMD_SIZE;

            states_physical_location = states_physical_base + SIZE_CAST(vip_uint32_t, states_buffer - states_base);
        }

        if (network->nld.insert_flag) {
            vip_uint8_t *tmp = states_buffer - LINK_SIZE;
            vip_uint32_t nld_states_npu_physical = 0;
            nbglk_nld_operation_t *last_nld_operation = VIP_NULL;

            nld_states_npu_physical = SIZE_CAST(vip_uint32_t, network->nld.state_memory.physical +
                (nld_states_buffer - nld_states_base));
            nbglk_append_link((vip_uint32_t *)tmp, nld_states_npu_physical);

            while (op_index == insert_index && nld_operation) {
                vip_uint32_t core_id = nld_operation->core_id;
                vip_uint32_t nld_states_size = 0;

                nld_operation->state.physical = nld_states_npu_physical;
                nld_operation->state.logical = nld_states_buffer;
                nld_operation->state.size = 0;

                if (nld_operation->wait_count) {
                    vip_uint32_t wait_index = 0;
                    for (wait_index = 0; wait_index < nld_operation->wait_count; wait_index++) {
                        ((vip_uint32_t *)(nld_states_buffer))[0] = 0x08010429;
                        /* all event id should be 1 for nn operation */
                        ((vip_uint32_t *)(nld_states_buffer))[1] = network->nld.nn_event_id[core_id];
                        nld_states_buffer += WAIT_SIZE;
                        nld_states_npu_physical += WAIT_SIZE;
                        nld_operation->state.size += WAIT_SIZE;
                        nld_operation->event_id = network->nld.nn_event_id[core_id];
                    }
                }

                vipOnError(patch_nld(network, nld_operation, nld_states_buffer, &nld_states_size, segment));
                nld_states_buffer += nld_states_size;
                nld_states_npu_physical += nld_states_size;
                nld_operation->state.size += nld_states_size;

                last_nld_operation = nld_operation;
                if (!nbglk_list_is_last(&nld_operation->list, &network->nld.operation_head)) {
                    nld_operation = NBGLK_LIST_NEXT_ENTRY(nbglk_nld_operation_t,
                                                        nld_operation, list);
                    insert_index = nld_operation->insert_index;
                }
                else {
                    nld_operation = VIP_NULL;
                    insert_index = 0xFFFFFFFF;
                }
            }
            nbglk_append_link((vip_uint32_t *)nld_states_buffer, states_physical_location);
            nld_states_buffer += LINK_SIZE;
            if (last_nld_operation) {
                last_nld_operation->state.size += LINK_SIZE;
            }
        }

        *states_buffer_ptr = states_buffer;
        *states_size_ptr = states_size;
        *insert_index_ptr = insert_index;
        *nld_operation_ptr = nld_operation;
        *nld_states_buffer_ptr = nld_states_buffer;
    }

onError:
    return status;
}

vip_uint32_t get_nld_link_loadstate_size(
    vip_network_t *network
    )
{
    vip_uint32_t size = 0;
    vip_uint32_t nld_operation_count = 0;
    vip_uint32_t i = 0;

    size = get_nld_loadstate_size(network);

    nld_operation_count = nbglk_nld_operation_count(network);
    /* link to next nbg operation */
    size += LINK_SIZE * nld_operation_count;
    /* append a wait event id before nld operation, need wait before nn finish before sh copy */

    for (i = 0; i < nld_operation_count; i++) {
        vip_uint32_t wait_count = 0;
        nbg_desc_ldump_operation_t *ldump_operation = VIP_NULL;

        ldump_operation = nbglk_get_ldump_operation_ptr_by_index(network, i);
        wait_count = ldump_operation->wait_count;
        size += EVENT_SIZE * wait_count;
    }
    return size;
}

#if (vpmdENABLE_DEBUG_LOG > 2)
vip_status_e nbglk_nld_show_output(
    vip_network_t *network
    )
{
    vip_status_e status = VIP_SUCCESS;
    nbglk_nld_output_t *entry = VIP_NULL;

    vip_print("network->nld.output_head:\n");
    vip_print("%-10s%-10s%-10s%-10s", "index", "node_id", "abs_op_id", "uid");
    vip_print("%-20s%-20s", "physical", "logical");
    vip_print("%-15s%-15s", "size", "used_size");
    vip_print("%-15s%-15s%-15s\n", "index_in_table", "index_in_op", "index_in_node");
    NBGLK_LIST_FOR_EACH_ENTRY(nbglk_nld_output_t, entry, &network->nld.output_head, list) {
        vip_print("%-10d%-10d%-10d%-10d", entry->index, entry->node_id, entry->abs_op_id, entry->uid);
        vip_print("0x%-18x%-20p", entry->memory.physical, entry->memory.logical);
        vip_print("0x%-13x0x%-13x", entry->size, entry->used_size);
        vip_print("%-15d%-15d%-15d\n", entry->index_in_table, entry->index_in_op, entry->index_in_node);
    }

    return status;
}

vip_status_e nbglk_nld_show_operation(
    vip_network_t *network
    )
{
    vip_status_e status = VIP_SUCCESS;
    nbglk_nld_operation_t *entry = VIP_NULL;
    vip_print("network->nld.operation_head:\n");
    vip_print("%-10s%-10s%-10s", "index", "node_id", "abs_op_id");
    vip_print("%-15s%-15s", "out_tab_index", "insert_index");
    vip_print("%-10s%-10s%-10s", "core_id", "event_id", "wait_num");
    vip_print("%-20s%-20s", "state_physical", "state_logical");
    vip_print("%-20s%-20s\n", "instr_physical", "instr_logical");

    NBGLK_LIST_FOR_EACH_ENTRY(nbglk_nld_operation_t, entry, &network->nld.operation_head, list) {
        vip_print("%-10d%-10d%-10d", entry->index, entry->node_id, entry->abs_op_id);
        vip_print("%-15d%-15d", entry->output_table_index, entry->insert_index);
        vip_print("%-10d%-10d%-10d", entry->core_id, entry->event_id, entry->wait_count);
        vip_print("0x%-18x%-20p", entry->state.physical, entry->state.logical);
        vip_print("0x%-18x%-20p\n", entry->instr.physical, entry->instr.logical);
    }

    return status;
}

vip_status_e nbglk_nld_show_instr(
    vip_network_t *network
    )
{
    vip_status_e status = VIP_SUCCESS;
    nbglk_nld_instr_t *entry = VIP_NULL;
    vip_print("network->nld.instr_head:\n");
    vip_print("%-10s%-15s%-10s\n", "index", "offset", "size");

    NBGLK_LIST_FOR_EACH_ENTRY(nbglk_nld_instr_t, entry, &network->nld.instr_head, list) {
        vip_print("%-10d%-15d%-10d\n", entry->index, entry->offset, entry->memory.size);
    }

    return status;
}

vip_status_e nbglk_nld_show_layer(
    vip_network_t *network
    )
{
    vip_status_e status = VIP_SUCCESS;
    nbglk_nld_layer_t *entry = VIP_NULL;
    vip_print("network->nld.layer_head:\n");
    vip_print("%-10s%-10s%-20s\n", "index", "node_id", "index_in_table");

    NBGLK_LIST_FOR_EACH_ENTRY(nbglk_nld_layer_t, entry, &network->nld.layer_head, list) {
        vip_print("%-10d%-10d%-20d\n", entry->index, entry->node_id, entry->index_in_table);
    }

    return status;
}
#else
vip_status_e nbglk_nld_show_output(
    vip_network_t *network
    )
{
    return VIP_SUCCESS;
}

vip_status_e nbglk_nld_show_operation(
    vip_network_t *network
    )
{
    return VIP_SUCCESS;
}

vip_status_e nbglk_nld_show_instr(
    vip_network_t *network
    )
{
    return VIP_SUCCESS;
}

vip_status_e nbglk_nld_show_layer(
    vip_network_t *network
    )
{
    return VIP_SUCCESS;
}
#endif

vip_status_e patch_nld_output(
    vip_network network,
    nbglk_segment_t *segment,
    nbglk_nld_operation_t *nld_operation,
    vip_uint8_t *states,
    nbg_desc_patch_t *pd_entry
    )
{
    vip_status_e status = VIP_SUCCESS;

    nbglk_nld_output_t *entry = VIP_NULL;
    vip_uint32_t index_in_table = 0;
    vip_bool_e find = vip_false_e;
    vip_uint32_t *memory = VIP_NULL;
    vip_uint32_t cmd_physical = 0;

    index_in_table = pd_entry->index;

    NBGLK_LIST_FOR_EACH_ENTRY(nbglk_nld_output_t, entry, &network->nld.output_head, list) {
        if (index_in_table == entry->index_in_table) {
            find = vip_true_e;
            break;
        }
    }
    if (!find) {
        vip_error("find output info fail\n");
        vipOnError(VIP_ERROR_FAILURE);
    }

    if (entry->memory.physical == 0) {
        vip_error("npu physical should not be zero\n");
        vipOnError(VIP_ERROR_FAILURE);
    }

    memory = (vip_uint32_t*)(states + pd_entry->dest_offset);
    cmd_physical = SIZE_CAST(vip_uint32_t, entry->memory.physical + pd_entry->src_offset);
    *memory = cmd_physical;

    return status;

onError:
    vip_error("fail to patch layer dump\n");
    return status;
}

vip_status_e nbglk_nld_first_dump_op(
    vip_network_t *network,
    nbglk_segment_t *segment,
    vip_uint8_t **nld_states_buffer_ptr,
    nbglk_nld_operation_t **nld_operation_ptr,
    vip_uint32_t *insert_index_ptr
    )
{
    vip_status_e status = VIP_SUCCESS;
    nbglk_nld_operation_t *nld_operation = VIP_NULL;
    vip_uint32_t insert_index = 0xFFFFFFFF;
    vip_uint8_t *nld_states_buffer = VIP_NULL;

    if (network->nld.enable) {
        vip_uint32_t nld_instr_size = 0;
        nld_instr_size = get_nld_instr_size(network);
        viphal_copy_memory(network->nld.instr_memory.logical, network->nbg_desc.ldump_instr_logical, nld_instr_size);
        nld_states_buffer = network->nld.state_memory.logical;
        if (VIP_NULL == nld_states_buffer) {
            vip_error("nld_states_buffer is NULL");
            vipGoOnError(VIP_ERROR_INVALID_NETWORK);
        }

        if (!nbglk_list_empty(&network->nld.operation_head)) {
            nld_operation = NBGLK_LIST_FIRST_ENTRY(nbglk_nld_operation_t, &network->nld.operation_head, list);
            insert_index = nld_operation->insert_index;
        }

        *nld_states_buffer_ptr = nld_states_buffer;
        *nld_operation_ptr = nld_operation;
        *insert_index_ptr = insert_index;
    }

onError:
    return status;
}

static vip_status_e nbglk_nld_memory_destroy(
    vip_network_t *network
    )
{
    vip_status_e status = VIP_SUCCESS;
    if (network->nld.instr_memory.mem_id != 0) {
        status = viphal_free_videomemory(network->nld.instr_memory.mem_id);
        network->nld.instr_memory.mem_id = 0;
    }

    if (network->nld.state_memory.mem_id != 0) {
        status = viphal_free_videomemory(network->nld.state_memory.mem_id);
        network->nld.state_memory.mem_id = 0;
    }

    return status;
}

static vip_status_e nbglk_nld_memory_init(
    vip_network_t *network
    )
{
    vip_status_e status = VIP_SUCCESS;
    nbglk_nld_instr_t *nld_instr = VIP_NULL;
    vip_uint32_t size = 0;

    size = get_nld_instr_size(network);
    network->nld.instr_memory.size = size;
    vipOnError(viphal_allocate_videomemory(&network->nld.instr_memory.logical, &network->nld.instr_memory.mem_id,
        &network->nld.instr_memory.physical, network->nld.instr_memory.size,
        vipdMEMORY_ALIGN_SIZE, VIPDRV_VIDEO_MEM_ALLOC_4GB_ADDR));

    vip_info("network->nld.instr_memory.physical=0x%x network->nld.instr_memory.size=0x%x\n",
        network->nld.instr_memory.physical, network->nld.instr_memory.size);

    NBGLK_LIST_FOR_EACH_ENTRY(nbglk_nld_instr_t, nld_instr, &network->nld.instr_head, list) {
        nld_instr->memory.logical = network->nld.instr_memory.logical + nld_instr->offset;
        nld_instr->memory.physical = network->nld.instr_memory.physical + nld_instr->offset;
    }

    size = get_nld_link_loadstate_size(network);
    network->nld.state_memory.size = size;
    vipOnError(viphal_allocate_videomemory(&network->nld.state_memory.logical, &network->nld.state_memory.mem_id,
        &network->nld.state_memory.physical, network->nld.state_memory.size,
        vipdMEMORY_ALIGN_SIZE, VIPDRV_VIDEO_MEM_ALLOC_4GB_ADDR));

    vip_info("network->nld.state_memory.physical=0x%x network->nld.state_memory.size=0x%x\n",
        network->nld.state_memory.physical, network->nld.state_memory.size);

onError:
    return status;
}

/* sanity check layer id is in nbg */
static vip_status_e nbglk_nld_check_layer_index(
    vip_network network,
    vip_int32_t *layer_id,
    vip_int32_t count
    )
{
    vip_status_e status = VIP_SUCCESS;
    vip_uint32_t i = 0;
    vip_uint32_t j = 0;
    vip_uint32_t node_id = 0;
    vip_bool_e find = vip_false_e;

    if (!layer_id) {
        vip_error("index_array is null\n");
        vipOnError(VIP_ERROR_FAILURE);
    }

    if (count < 1) {
        vip_error("count is %d\n", count);
        vipOnError(VIP_ERROR_FAILURE);
    }

    for (i = 0; i < (vip_uint32_t)count; i++) {
        node_id = (vip_uint32_t)layer_id[i];
        find = vip_false_e;
        for (j = 0; j < network->nbg_desc.n_orig_layers; j++) {
            nbg_desc_layer_t *layer = VIP_NULL;
            layer = nbglk_get_layer_ptr_by_index(network, j);
            if (layer->node_id == node_id) {
                find = vip_true_e;
                break;
            }
        }
        if (!find) {
            vip_error("fail can't find layer-id=%d in nbg\n", node_id);
            vipOnError(VIP_ERROR_FAILURE);
        }
    }

onError:
    return status;
}

static vip_status_e nbglk_nld_layerid_destroy(
    vip_network network
    )
{
    vip_status_e status = VIP_SUCCESS;

    nbglk_nld_layer_t *entry = VIP_NULL;
    nbglk_nld_layer_t *tmp = VIP_NULL;

    if (!nbglk_list_is_init(&network->nld.layer_head)) {
        return status;
    }

    NBGLK_LIST_FOR_EACH_ENTRY_SAFE(nbglk_nld_layer_t, entry, tmp, &network->nld.layer_head, list) {
        nbglk_list_del(&entry->list);
        viphal_free_memory(entry);
    }

    return status;
}

vip_bool_e nbglk_nld_find_node_id_in_list(
    vip_network network,
    vip_uint32_t node_id
    )
{
    nbglk_nld_layer_t *entry = VIP_NULL;

    NBGLK_LIST_FOR_EACH_ENTRY(nbglk_nld_layer_t, entry, &network->nld.layer_head, list) {
        if (node_id == entry->node_id) {
            return vip_true_e;
        }
    }

    return vip_false_e;
}

vip_bool_e nbglk_nld_find_node_id_in_table(
    vip_network network,
    vip_uint32_t node_id,
    vip_uint32_t *index_in_table
    )
{
    vip_uint32_t i = 0;

    for (i = 0; i < network->nbg_desc.n_ldump_operation; i++) {
        nbg_desc_operation_t *operation = VIP_NULL;
        nbg_desc_layer_t *layer = VIP_NULL;
        nbg_desc_ldump_operation_t *ldump_operation = VIP_NULL;

        vip_uint32_t layer_index = 0;
        vip_uint32_t insert_index = 0;

        ldump_operation = nbglk_get_ldump_operation_ptr_by_index(network, i);
        insert_index = ldump_operation->insert_index;
        operation = (nbg_desc_operation_t *)nbglk_get_operation_ptr_by_index(network, insert_index);
        layer_index = operation->layer_id;

        layer = nbglk_get_layer_ptr_by_index(network, layer_index);
        node_id = layer->node_id;

        if (node_id == layer->node_id) {
            *index_in_table = layer_index;
            return vip_true_e;
        }
    }

    return vip_false_e;
}

static vip_status_e nbglk_nld_layerid_init(
    vip_network network,
    vip_int32_t *layer_id,
    vip_int32_t layer_count
    )
{
    vip_status_e status = VIP_SUCCESS;
    vip_uint32_t i = 0;
    vip_uint32_t count = 0;

    NBGLK_INIT_LIST_HEAD(&network->nld.layer_head);

    if ((vip_int32_t)layer_count == -1) {
        for (i = 0; i < network->nbg_desc.n_ldump_operation; i++) {
            nbg_desc_operation_t *operation = VIP_NULL;
            nbg_desc_layer_t *layer = VIP_NULL;
            nbg_desc_ldump_operation_t *ldump_operation = VIP_NULL;

            nbglk_nld_layer_t *nld_layer = VIP_NULL;

            vip_uint32_t node_id = 0;
            vip_uint32_t index_in_table = 0;
            vip_uint32_t insert_index = 0;

            ldump_operation = nbglk_get_ldump_operation_ptr_by_index(network, i);
            insert_index = ldump_operation->insert_index;
            operation = (nbg_desc_operation_t *)nbglk_get_operation_ptr_by_index(network, insert_index);
            index_in_table = operation->layer_id;

            layer = nbglk_get_layer_ptr_by_index(network, index_in_table);
            node_id = layer->node_id;

            if (nbglk_nld_find_node_id_in_list(network, node_id)) {
                continue;
            }

            vipOnError(viphal_allocate_memory(sizeof(nbglk_nld_layer_t), (void **)&nld_layer));
            nld_layer->index = count;
            nld_layer->node_id = node_id;
            nld_layer->index_in_table = index_in_table;
            nbglk_list_add_tail(&nld_layer->list, &network->nld.layer_head);
            count++;
        }
    }
    else {
        for (i = 0; i < (vip_uint32_t)layer_count; i++) {
            nbglk_nld_layer_t *nld_layer = VIP_NULL;
            vip_uint32_t node_id = 0;
            vip_uint32_t index_in_table = 0;

            node_id = (vip_uint32_t)layer_id[i];
            if (!nbglk_nld_find_node_id_in_table(network, node_id, &index_in_table)) {
                vip_error("not found node id:%d in table\n", node_id);
                vipOnError(VIP_ERROR_FAILURE);
            }

            if (nbglk_nld_find_node_id_in_list(network, node_id)) {
                continue;
            }

            vipOnError(viphal_allocate_memory(sizeof(nbglk_nld_layer_t), (void **)&nld_layer));
            nld_layer->index = count;
            nld_layer->node_id = node_id;
            nld_layer->index_in_table = index_in_table;
            nbglk_list_add_tail(&nld_layer->list, &network->nld.layer_head);
            count++;
        }
    }

    nbglk_nld_show_layer(network);

onError:
    return status;
}

static vip_status_e nbglk_nld_operation_destroy(
    vip_network network
    )
{
    vip_status_e status = VIP_SUCCESS;

    nbglk_nld_operation_t *entry = VIP_NULL;
    nbglk_nld_operation_t *tmp = VIP_NULL;

    if (!nbglk_list_is_init(&network->nld.operation_head)) {
        return status;
    }

    NBGLK_LIST_FOR_EACH_ENTRY_SAFE(nbglk_nld_operation_t, entry, tmp, &network->nld.operation_head, list) {
        nbglk_list_del(&entry->list);
        viphal_free_memory(entry);
    }

    return status;
}

static vip_status_e nbglk_nld_operation_init(
    vip_network network
    )
{
    vip_status_e status = VIP_SUCCESS;
    vip_uint32_t i = 0;
    vip_uint32_t j = 0;

    NBGLK_INIT_LIST_HEAD(&network->nld.operation_head);

    for (i = 0; i < network->nbg_desc.n_ldump_operation; i++) {
        nbg_desc_operation_t *operation = VIP_NULL;
        nbg_desc_layer_t *layer = VIP_NULL;

        nbg_desc_ldump_operation_t *ldump_operation = VIP_NULL;

        nbglk_nld_operation_t *nld_operation = VIP_NULL;
        nbglk_nld_layer_t *nld_layer = VIP_NULL;

        vip_uint32_t insert_index = 0;
        vip_uint32_t abs_op_id = 0;
        vip_uint32_t node_id = 0;
        vip_uint32_t output_table_index = 0xFFFFFFFF;
        vip_uint32_t uid = 0;
        vip_uint32_t core_id = 0;
        vip_uint32_t wait_count = 0;

        vip_bool_e find = vip_false_e;
        vip_uint32_t layer_index = 0;

        ldump_operation = nbglk_get_ldump_operation_ptr_by_index(network, i);
        insert_index = ldump_operation->insert_index;
        core_id = ldump_operation->core_id;
        wait_count = ldump_operation->wait_count;

        operation = (nbg_desc_operation_t *)nbglk_get_operation_ptr_by_index(network, insert_index);
        abs_op_id = operation->abs_op_id;
        layer_index = operation->layer_id;

        layer = nbglk_get_layer_ptr_by_index(network, layer_index);
        node_id = layer->node_id;
        uid = layer->uid;

        NBGLK_LIST_FOR_EACH_ENTRY(nbglk_nld_layer_t, nld_layer, &network->nld.layer_head, list) {
            if (node_id == nld_layer->node_id) {
                find = vip_true_e;
                break;
            }
        }
        if (!find) {
            continue;
        }

        find = vip_false_e;
        for (j = 0; j < ldump_operation->patch_count; j++) {
            nbg_desc_patch_t *patch_data = VIP_NULL;
            patch_data = nbglk_get_ldump_patch_data_ptr_by_index(network, ldump_operation->start_patch_index + j);
            if (NBG_SOURCE_LAYER_DUMP_OUTPUT == patch_data->source_type) {
                output_table_index = patch_data->index;
                find = vip_true_e;
                break;
            }
        }
        if (!find) {
            vip_error("find dumped layer's output fail\n");
            vip_error("layer dump operation index:%d node_id:%d uid:%d insert_index:%d start_patch_index:%d\n",
                i, node_id, uid, insert_index, ldump_operation->start_patch_index);
            vipOnError(VIP_ERROR_FAILURE);
        }

        if (output_table_index == 0xFFFFFFFF) {
            vip_error("output_table_index is not right\n");
            vip_error("layer dump operation index:%d node_id:%d uid:%d insert_index:%d start_patch_index:%d\n",
                i, node_id, uid, insert_index, ldump_operation->start_patch_index);
            vipOnError(VIP_ERROR_FAILURE);
        }

        vipOnError(viphal_allocate_memory(sizeof(nbglk_nld_operation_t), (void **)&nld_operation));

        nld_operation->insert_index = insert_index;
        nld_operation->abs_op_id = abs_op_id;
        nld_operation->node_id = node_id;
        nld_operation->uid = uid;
        nld_operation->output_table_index = output_table_index;
        nld_operation->core_id = core_id;
        nld_operation->event_id = 0;
        nld_operation->wait_count = wait_count;
        nld_operation->index = i;

        nbglk_list_add_tail(&nld_operation->list, &network->nld.operation_head);
    }

    nbglk_nld_show_operation(network);

onError:
    return status;
}

static vip_status_e nbglk_nld_output_destroy(
    vip_network network
    )
{
    vip_status_e status = VIP_SUCCESS;

    nbglk_nld_output_t *entry = VIP_NULL;
    nbglk_nld_output_t *tmp = VIP_NULL;

    if (!nbglk_list_is_init(&network->nld.output_head)) {
        return status;
    }

    NBGLK_LIST_FOR_EACH_ENTRY_SAFE(nbglk_nld_output_t, entry, tmp, &network->nld.output_head, list) {
        nbglk_list_del(&entry->list);
        if (entry->memory.mem_id != 0) {
            status = viphal_free_videomemory(entry->memory.mem_id);
            entry->memory.mem_id = 0;
        }
        viphal_free_memory(entry);
    }

    return status;
}

static vip_uint32_t nbglk_nld_get_output_count_in_op(
    vip_network network,
    vip_uint32_t abs_op_id
    )
{
    vip_uint32_t count = 0;
    nbglk_nld_output_t *entry = VIP_NULL;

    NBGLK_LIST_FOR_EACH_ENTRY(nbglk_nld_output_t, entry, &network->nld.output_head, list) {
        if (entry->abs_op_id == abs_op_id) {
            count++;
        }
    }

    return count;
}

static vip_uint32_t nbglk_nld_get_output_count_in_node(
    vip_network network,
    vip_uint32_t node_id
    )
{
    vip_uint32_t count = 0;
    nbglk_nld_output_t *entry = VIP_NULL;

    NBGLK_LIST_FOR_EACH_ENTRY(nbglk_nld_output_t, entry, &network->nld.output_head, list) {
        if (entry->node_id == node_id) {
            count++;
        }
    }

    return count;
}

static vip_uint32_t nbglk_nld_find_output_in_list(
    vip_network network,
    vip_uint32_t index_in_table,
    vip_uint32_t abs_op_id,
    vip_uint32_t node_id
    )
{
    nbglk_nld_output_t *entry = VIP_NULL;

    NBGLK_LIST_FOR_EACH_ENTRY(nbglk_nld_output_t, entry, &network->nld.output_head, list) {
        if (index_in_table == entry->index_in_table &&
            abs_op_id == entry->abs_op_id) {
            return vip_true_e;
        }
    }

    return vip_false_e;
}

static vip_status_e nbglk_nld_output_init(
    vip_network network
    )
{
    vip_status_e status = VIP_SUCCESS;
    vip_uint32_t output_count = 0;
    nbglk_nld_operation_t *nld_operation = VIP_NULL;

    NBGLK_INIT_LIST_HEAD(&network->nld.output_head);

    NBGLK_LIST_FOR_EACH_ENTRY(nbglk_nld_operation_t, nld_operation, &network->nld.operation_head, list) {
        nbg_desc_operation_t *operation = VIP_NULL;
        nbg_desc_ldump_output_t *ldump_output = VIP_NULL;

        nbglk_nld_output_t *nld_output = VIP_NULL;

        vip_uint32_t index_in_table = 0;
        vip_uint32_t index_in_op = 0;
        vip_uint32_t index_in_node = 0;
        vip_uint32_t abs_op_id = 0;
        vip_uint32_t node_id = 0;
        vip_uint32_t uid = 0;
        vip_uint32_t insert_index = 0xFFFFFFFF;
        vip_uint32_t buf_size = 0;
        vip_uint32_t i = 0;

        nbg_desc_layer_t *layer = VIP_NULL;

        index_in_table = nld_operation->output_table_index;
        abs_op_id = nld_operation->abs_op_id;
        node_id = nld_operation->node_id;
        uid  = nld_operation->uid;
        insert_index = nld_operation->insert_index;

        if (nbglk_nld_find_output_in_list(network, index_in_table, abs_op_id, node_id)) {
            continue;
        }

        vipOnError(viphal_allocate_memory(sizeof(nbglk_nld_output_t), (void **)&nld_output));

        ldump_output = nbglk_get_ldump_output_ptr_by_index(network, index_in_table);
        vipOnError(viphal_copy_memory(&nld_output->info, &ldump_output->info, sizeof(ldump_output->info)));

        buf_size = nbg_get_buf_format_bit(nld_output->info.data_format);
        for (i = 0; i < nld_output->info.dim_count; i++) {
            buf_size *= SIZE_CAST(vip_uint32_t, nld_output->info.dim_size[i]);
            if (0 == i) {
                /* round up */
                buf_size = (buf_size + BITS_PER_BYTE - 1) / BITS_PER_BYTE;
            }
        }
        nld_output->used_size = buf_size;
        buf_size = VIP_ALIGN(buf_size, vipdMEMORY_ALIGN_SIZE);
        nld_output->size = buf_size;

        nld_output->memory.size = nld_output->size;
        vipOnError(viphal_allocate_videomemory(&nld_output->memory.logical, &nld_output->memory.mem_id,
            &nld_output->memory.physical, nld_output->memory.size,
            vipdMEMORY_ALIGN_SIZE, VIPDRV_VIDEO_MEM_ALLOC_NONE));

        operation = (nbg_desc_operation_t *)nbglk_get_operation_ptr_by_index(network, insert_index);
        layer = nbglk_get_layer_ptr_by_index(network, operation->layer_id);

        nld_output->abs_op_id = abs_op_id;
        nld_output->node_id = node_id;
        nld_output->index = output_count;
        nld_output->uid = uid;
        vipOnError(viphal_copy_memory(nld_output->layer_name, &layer->name, LAYER_NAME_SIZE));

        index_in_op = nbglk_nld_get_output_count_in_op(network, abs_op_id);
        index_in_node = nbglk_nld_get_output_count_in_node(network, node_id);
        nld_output->index_in_op = index_in_op;
        nld_output->index_in_node = index_in_node;
        nld_output->index_in_table = index_in_table;

        nbglk_list_add_tail(&nld_output->list, &network->nld.output_head);
        output_count++;
    }

    nbglk_nld_show_output(network);

onError:
    return status;
}

static vip_status_e nbglk_nld_instr_destroy(
    vip_network network
    )
{
    vip_status_e status = VIP_SUCCESS;

    nbglk_nld_instr_t *entry = VIP_NULL;
    nbglk_nld_instr_t *tmp = VIP_NULL;

    if (!nbglk_list_is_init(&network->nld.instr_head)) {
        return status;
    }

    NBGLK_LIST_FOR_EACH_ENTRY_SAFE(nbglk_nld_instr_t, entry, tmp, &network->nld.instr_head, list) {
        nbglk_list_del(&entry->list);
        if (entry->memory.mem_id != 0) {
            status = viphal_free_videomemory(entry->memory.mem_id);
            entry->memory.mem_id = 0;
        }
        viphal_free_memory(entry);
    }

    return status;
}

static vip_status_e nbglk_nld_instr_init(
    vip_network network
    )
{
    vip_status_e status = VIP_SUCCESS;
    vip_uint32_t i = 0;
    vip_uint32_t j = 0;
    vip_uint32_t instr_num = 0;

    NBGLK_INIT_LIST_HEAD(&network->nld.instr_head);

    instr_num = network->nbg_desc.n_ldump_instr_table;
    for (i = 0; i < instr_num; i++) {
        nbglk_nld_instr_t *nld_instr = VIP_NULL;
        vip_bool_e find = vip_false_e;
        nbglk_nld_operation_t *nld_operation = VIP_NULL;
        nbg_desc_ldump_operation_t *ldump_operation = VIP_NULL;

        NBGLK_LIST_FOR_EACH_ENTRY(nbglk_nld_operation_t, nld_operation, &network->nld.operation_head, list) {
            ldump_operation = nbglk_get_ldump_operation_ptr_by_index(network, nld_operation->index);
            find = vip_false_e;
            for (j = 0; j < ldump_operation->patch_count; j++) {
                nbg_desc_patch_t *patch_data = VIP_NULL;
                patch_data = nbglk_get_ldump_patch_data_ptr_by_index(network, ldump_operation->start_patch_index + j);
                if (NBG_SOURCE_COMMAND == patch_data->source_type) {
                    find = vip_true_e;
                    break;
                }
            }
        }

        if (!find) {
            continue;
        }

        vipOnError(viphal_allocate_memory(sizeof(nbglk_nld_instr_t), (void **)&nld_instr));

        nld_instr->index = i;
        nld_instr->op_index = nld_operation->index;
        nld_instr->offset = SIZE_CAST(vip_uint32_t, network->nbg_desc.ldump_instr_table[i].offset);
        nld_instr->memory.size = SIZE_CAST(vip_uint32_t, network->nbg_desc.ldump_instr_table[i].size);

        vipOnError(viphal_allocate_videomemory(&nld_instr->memory.logical, &nld_instr->memory.mem_id,
            &nld_instr->memory.physical, nld_instr->memory.size,
            vipdMEMORY_ALIGN_SIZE, VIPDRV_VIDEO_MEM_ALLOC_4GB_ADDR));

        nbglk_list_add_tail(&nld_instr->list, &network->nld.instr_head);
    }

    nbglk_nld_show_instr(network);

onError:
    return status;
}

vip_status_e nbglk_nld_output_info_destory(
    vip_network network
)
{
    vip_status_e status = VIP_SUCCESS;

    if (network->nld.nld_output.info) {
        viphal_free_memory(network->nld.nld_output.info);
        network->nld.nld_output.info = VIP_NULL;
        network->nld.nld_output.count = 0;
    }

    return status;
}

vip_status_e nbglk_nld_destroy(
    vip_network network
    )
{
    vip_status_e status = VIP_SUCCESS;

    nbglk_nld_layerid_destroy(network);
    nbglk_nld_operation_destroy(network);
    nbglk_nld_output_destroy(network);
    nbglk_nld_instr_destroy(network);
    nbglk_nld_memory_destroy(network);
    nbglk_nld_output_info_destory(network);

    network->nld.enable = vip_false_e;

    return status;
}

/* layer_count is -1 dump all layer outputs */
vip_status_e nbglk_nld_init(
    vip_network network,
    vip_int32_t *layer_id,
    vip_int32_t layer_count
    )
{
    vip_status_e status = VIP_SUCCESS;

    if (!layer_id) {
        vip_error("fail layer id is null\n");
        vipOnError(VIP_ERROR_FAILURE);
    }

    if (0 == layer_count) {
        vip_error("fail layer count is 0\n");
        vipOnError(VIP_ERROR_FAILURE);
    }

    if ((vip_int32_t)layer_count == -1) {
        vip_info("dump all layer's outputs for this network\n");
    }
    else {
        status = nbglk_nld_check_layer_index(network, layer_id, layer_count);
        if (status != VIP_SUCCESS) {
            vip_error("check layer index fail\n");
            vipOnError(VIP_ERROR_FAILURE);
        }
    }

    vipOnError(nbglk_nld_layerid_init(network, layer_id, layer_count));
    vipOnError(nbglk_nld_operation_init(network));
    vipOnError(nbglk_nld_output_init(network));
    vipOnError(nbglk_nld_instr_init(network));
    vipOnError(nbglk_nld_memory_init(network));

    network->nld.status = 1;
    network->nld.enable = vip_true_e;

    return status;

onError:
    nbglk_nld_destroy(network);
    network->nld.status = 0;
    network->nld.enable = vip_false_e;
    return status;
}

vip_uint32_t nbglk_nld_get_output_count(
    vip_network network
)
{
    vip_uint32_t count = 0;
    nbglk_nld_output_t *entry = VIP_NULL;


    NBGLK_LIST_FOR_EACH_ENTRY(nbglk_nld_output_t, entry, &network->nld.output_head, list) {
        count++;
    }

    return count;
}

vip_nld_output_info_t *nbglk_nld_get_output_info(
    vip_network network
)
{
    vip_status_e status = VIP_SUCCESS;
    vip_nld_output_info_t *info = VIP_NULL;
    vip_nld_output_info_t *output = VIP_NULL;
    nbglk_nld_output_t *entry = VIP_NULL;
    vip_uint32_t count = 0;
    vip_uint32_t index = 0;

    count = network->nld.nld_output.count;
    if (count < 1) {
        vip_error("fail, count of layer dump is 0\n");
        vipOnError(VIP_ERROR_FAILURE);
    }

    vipOnError(viphal_allocate_memory(sizeof(vip_nld_output_info_t) * count, (void **)&info));
    output = info;

    NBGLK_LIST_FOR_EACH_ENTRY(nbglk_nld_output_t, entry, &network->nld.output_head, list) {
        vipOnError(viphal_copy_memory(output->layer_name, entry->layer_name, LAYER_NAME_SIZE));
        output->layer_id = entry->node_id;
        output->layer_output_index = entry->index_in_node;
        output->uid = entry->uid;

        output->param.dim_count = entry->info.dim_count;
        for (index = 0; index < entry->info.dim_count; index++) {
            output->param.dim_size[index] = entry->info.dim_size[index];
        }
        output->param.data_format = entry->info.data_format;
        output->param.quant_format = entry->info.quant_format;
        switch(entry->info.quant_format) {
        case VIP_BUFFER_QUANTIZE_DYNAMIC_FIXED_POINT:
            output->param.quant_data.dfp.fixed_point_pos = entry->info.quant_data.dfp.fixed_point_pos;
            break;
        case VIP_BUFFER_QUANTIZE_TF_ASYMM:
        case VIP_BUFFER_QUANTIZE_NONE:
            output->param.quant_data.affine.tf_zero_point = entry->info.quant_data.affine.tf_zero_point;
            output->param.quant_data.affine.tf_scale = entry->info.quant_data.affine.tf_scale;
            break;
        default:
            vip_error("not support this quan_format=%d\n", entry->info.quant_format);
            vipOnError(VIP_ERROR_INVALID_ARGUMENTS);
        }
        output->size = entry->used_size;
        output->memory = entry->memory.logical;
        output++;
    }

onError:
    return info;
}

vip_status_e nbglk_nld_get_output(
    vip_network network,
    vip_uint32_t *count,
    vip_nld_output_info_t **info
)
{
    vip_status_e status = VIP_SUCCESS;

    if (!count) {
        vip_error("fail, count is null\n");
        vipOnError(VIP_ERROR_FAILURE);
    }

    if (!info) {
        vip_error("fail, info is null\n");
        vipOnError(VIP_ERROR_FAILURE);
    }

    network->nld.nld_output.count = nbglk_nld_get_output_count(network);
    network->nld.nld_output.info = nbglk_nld_get_output_info(network);
    *count = network->nld.nld_output.count;
    *info = network->nld.nld_output.info;

onError:
    return status;
}
#else
vip_status_e nbglk_nld_init(
    vip_network network,
    vip_int32_t *layer_id,
    vip_int32_t layer_count
    )
{
    vip_warning("warning please set vpmdENABLE_LAYER_DUMP 1 to enable this feature\n");
    return VIP_SUCCESS;
}

vip_status_e nbglk_nld_get_output(
    vip_network network,
    vip_uint32_t *count,
    vip_nld_output_info_t **info
)
{
    return VIP_SUCCESS;
}
#endif
