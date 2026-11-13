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

#include <nbg_lk_npd.h>
#include <nbg_lk_network.h>
#include <vip_hal_os_port.h>
#include <nbg_lk_context.h>
#include <nbg_lk_relocate.h>
#include <nbg_fmt_util.h>

vip_status_e nbglk_npd_generate_header_info(
    vip_network_t *network
    )
{
    vip_status_e status = VIP_SUCCESS;

    vip_char_t magic[4] = "VNPD";
    vip_uint32_t nn_operation_count = 0;
    nbglk_npd_header_t *header = VIP_NULL;
    nbglk_npd_network_info_t *network_info = VIP_NULL;
    vip_uint8_t *cpu_logical = network->npd.npd_buffer.logical;
    vip_uint32_t header_size = sizeof(nbglk_npd_network_info_t) + sizeof(nbglk_npd_header_t);
    vipOnError(viphal_allocate_memory(sizeof(nbglk_npd_header_t), (void **)&header));
    vipOnError(viphal_allocate_memory(sizeof(nbglk_npd_network_info_t), (void **)&network_info));

    /* fill profile header info */
    viphal_copy_memory(&header->magic, &magic, sizeof(magic));
    header->version = NPD_FORMAT_VERSION;
    header->hw_cid = network->context->hw_target;
    header->network_count = 1;
    header->vip_core_count = network->device->core_count;
    viphal_copy_memory(cpu_logical, header, sizeof(nbglk_npd_header_t));

    /* fill profile network info */
    viphal_copy_memory(&network_info->network_name, &network->nbg_desc.net_info.name, NETWORK_NAME_SIZE);
    network_info->data_offset = header_size;

    CACULATE_NN_NUM_IN_NETWORK(network, nn_operation_count)
    network_info->data_size = NPD_ITEM_SIZE * nn_operation_count;
    network_info->profile_item_count = nn_operation_count;
    network_info->profile_item_size = NPD_ITEM_SIZE;
    network_info->core_count = network->device->core_count;
    viphal_copy_memory(cpu_logical + sizeof(nbglk_npd_header_t),
            network_info, sizeof(nbglk_npd_network_info_t));

    viphal_free_memory(header);
    viphal_free_memory(network_info);

onError:
    return status;
}

vip_status_e nbglk_npd_generate_command(
    vip_uint8_t *buffer,
    vip_network_t *network,
    nbglk_segment_t *segment,
    nbg_desc_operation_t *operation,
    nbglk_npd_proflie_cmd_e cmd
    )
{
    vip_status_e status = VIP_SUCCESS;
    vip_uint8_t nn_trigger_id = 0x0;
    nbg_desc_data_table_t *data_table = VIP_NULL;
    vip_uint32_t *trigger_word = VIP_NULL;
    nbg_desc_patch_t *pd_entry = VIP_NULL;
    vip_uint32_t *memory = (vip_uint32_t*)buffer;
    vip_uint64_t offset_bits = 0, stride = 0;
    vip_uint32_t enable_cmd = 0, i = 0, core_id = 0;
    vip_uint32_t core_num = network->device->core_count;
    vip_uint32_t COUNTER_SEL[NPD_TOTAL_SEL_COUNT] = {0x0, 0x01, 0x02, 0x03, 0x04, 0x05, 0x0f};
    vip_uint32_t header_bytes = sizeof(nbglk_npd_network_info_t) + sizeof(nbglk_npd_header_t);
    vip_address_t physical = network->npd.npd_buffer.physical + header_bytes;
    vip_uint64_t *cpu_logical = (vip_uint64_t*)(network->npd.npd_buffer.logical + header_bytes);

    enable_cmd = ((((vip_uint32_t) (0)) & ~(((vip_uint32_t) (((vip_uint32_t) ((((1 ?
 27:27) - (0 ?
 27:27) + 1) == 32) ?
 ~0 : (~0 & ((1 << (((1 ?
 27:27) - (0 ?
 27:27) + 1))) - 1)))))) << (0 ?
 27:27))) | (((vip_uint32_t) ((vip_uint32_t) (1) & ((vip_uint32_t) ((((1 ?
 27:27) - (0 ?
 27:27) + 1) == 32) ?
 ~0 : (~0 & ((1 << (((1 ?
 27:27) - (0 ? 27:27) + 1))) - 1)))))) << (0 ? 27:27)));
    do {    do {    *memory++        = ((((vip_uint32_t) (0)) & ~(((vip_uint32_t) (((vip_uint32_t) ((((1 ?
 31:27) - (0 ?
 31:27) + 1) == 32) ?
 ~0 : (~0 & ((1 << (((1 ?
 31:27) - (0 ?
 31:27) + 1))) - 1)))))) << (0 ?
 31:27))) | (((vip_uint32_t) (0x01 & ((vip_uint32_t) ((((1 ?
 31:27) - (0 ?
 31:27) + 1) == 32) ?
 ~0 : (~0 & ((1 << (((1 ?
 31:27) - (0 ?
 31:27) + 1))) - 1)))))) << (0 ?
 31:27)))        | ((((vip_uint32_t) (0)) & ~(((vip_uint32_t) (((vip_uint32_t) ((((1 ?
 26:26) - (0 ?
 26:26) + 1) == 32) ?
 ~0 : (~0 & ((1 << (((1 ?
 26:26) - (0 ?
 26:26) + 1))) - 1)))))) << (0 ?
 26:26))) | (((vip_uint32_t) ((vip_uint32_t) (vip_false_e) & ((vip_uint32_t) ((((1 ?
 26:26) - (0 ?
 26:26) + 1) == 32) ?
 ~0 : (~0 & ((1 << (((1 ?
 26:26) - (0 ?
 26:26) + 1))) - 1)))))) << (0 ?
 26:26)))        | ((((vip_uint32_t) (0)) & ~(((vip_uint32_t) (((vip_uint32_t) ((((1 ?
 25:16) - (0 ?
 25:16) + 1) == 32) ?
 ~0 : (~0 & ((1 << (((1 ?
 25:16) - (0 ?
 25:16) + 1))) - 1)))))) << (0 ?
 25:16))) | (((vip_uint32_t) ((vip_uint32_t) (1) & ((vip_uint32_t) ((((1 ?
 25:16) - (0 ?
 25:16) + 1) == 32) ?
 ~0 : (~0 & ((1 << (((1 ?
 25:16) - (0 ?
 25:16) + 1))) - 1)))))) << (0 ?
 25:16)))        | ((((vip_uint32_t) (0)) & ~(((vip_uint32_t) (((vip_uint32_t) ((((1 ?
 15:0) - (0 ?
 15:0) + 1) == 32) ?
 ~0 : (~0 & ((1 << (((1 ?
 15:0) - (0 ?
 15:0) + 1))) - 1)))))) << (0 ?
 15:0))) | (((vip_uint32_t) ((vip_uint32_t) (0x0E12) & ((vip_uint32_t) ((((1 ?
 15:0) - (0 ?
 15:0) + 1) == 32) ?
 ~0 : (~0 & ((1 << (((1 ? 15:0) - (0 ? 15:0) + 1))) - 1)))))) << (0 ? 15:0)));}while(0);
    do {vip_uint32_t __temp_data32__;
__temp_data32__ = enable_cmd;
*memory++ = __temp_data32__;
} while(0);
}while(0);


    data_table = &network->nbg_desc.load_states_table[operation->state_id];
    for (i = 0; i < operation->patch_count; i++) {
        pd_entry = &network->nbg_desc.pd_sections[operation->patch_index + i];
        if (pd_entry->type == NBG_PATCH_TYPE_STATE && pd_entry->source_type == NBG_SOURCE_COMMAND) {
            trigger_word = (vip_uint32_t*)(network->load_states + data_table->offset +
                    pd_entry->dest_offset);
            nn_trigger_id = (*trigger_word & 0x1f);
        }
    }

    if (core_num > 1) {
        for (i = 0; i < operation->patch_count; i++) {
            pd_entry = &network->nbg_desc.pd_sections[operation->patch_index + i];
            if (pd_entry->type == NBG_PATCH_TYPE_STATE && pd_entry->source_type == NBG_SOURCE_MULTIVIP_CHIPID) {
                core_id = network->device->core_id[0] + pd_entry->index;
                #if vpmdENABLE_MULTIPLE_VIP
                nbglk_append_chip_enable(memory, core_id, vip_false_e);
                memory += 2;
                #endif
            }
        }
    }
    else {
        core_id = network->device->core_id[0];
    }

    if (cmd == NBGLK_NPDCMD_BEGIN) {
        offset_bits = network->npd.index * NPD_ITEM_SIZE * 8;
        stride = offset_bits / 64;
        cpu_logical += stride;
        cpu_logical[0] = network->nbg_desc.orig_layers[operation->layer_id].node_id;
        cpu_logical[1] = core_id;
        vip_info("profile database: nn_engine_index=%d, core=%d, offset_bits=%lld, stride=%lld, user_logical=%p,"
                "layer_id=%d, node_id=%d\n", network->npd.index, core_id, offset_bits, stride, cpu_logical,
                operation->layer_id, cpu_logical[0]);
    }

    for (i = 0; i < NPD_TOTAL_SEL_COUNT; i++) {
        switch (cmd) {
            case NBGLK_NPDCMD_BEGIN:
                /* reset and resume counter */
                do {    do {    *memory++        = ((((vip_uint32_t) (0)) & ~(((vip_uint32_t) (((vip_uint32_t) ((((1 ?
 31:27) - (0 ?
 31:27) + 1) == 32) ?
 ~0 : (~0 & ((1 << (((1 ?
 31:27) - (0 ?
 31:27) + 1))) - 1)))))) << (0 ?
 31:27))) | (((vip_uint32_t) (0x01 & ((vip_uint32_t) ((((1 ?
 31:27) - (0 ?
 31:27) + 1) == 32) ?
 ~0 : (~0 & ((1 << (((1 ?
 31:27) - (0 ?
 31:27) + 1))) - 1)))))) << (0 ?
 31:27)))        | ((((vip_uint32_t) (0)) & ~(((vip_uint32_t) (((vip_uint32_t) ((((1 ?
 26:26) - (0 ?
 26:26) + 1) == 32) ?
 ~0 : (~0 & ((1 << (((1 ?
 26:26) - (0 ?
 26:26) + 1))) - 1)))))) << (0 ?
 26:26))) | (((vip_uint32_t) ((vip_uint32_t) (vip_false_e) & ((vip_uint32_t) ((((1 ?
 26:26) - (0 ?
 26:26) + 1) == 32) ?
 ~0 : (~0 & ((1 << (((1 ?
 26:26) - (0 ?
 26:26) + 1))) - 1)))))) << (0 ?
 26:26)))        | ((((vip_uint32_t) (0)) & ~(((vip_uint32_t) (((vip_uint32_t) ((((1 ?
 25:16) - (0 ?
 25:16) + 1) == 32) ?
 ~0 : (~0 & ((1 << (((1 ?
 25:16) - (0 ?
 25:16) + 1))) - 1)))))) << (0 ?
 25:16))) | (((vip_uint32_t) ((vip_uint32_t) (1) & ((vip_uint32_t) ((((1 ?
 25:16) - (0 ?
 25:16) + 1) == 32) ?
 ~0 : (~0 & ((1 << (((1 ?
 25:16) - (0 ?
 25:16) + 1))) - 1)))))) << (0 ?
 25:16)))        | ((((vip_uint32_t) (0)) & ~(((vip_uint32_t) (((vip_uint32_t) ((((1 ?
 15:0) - (0 ?
 15:0) + 1) == 32) ?
 ~0 : (~0 & ((1 << (((1 ?
 15:0) - (0 ?
 15:0) + 1))) - 1)))))) << (0 ?
 15:0))) | (((vip_uint32_t) ((vip_uint32_t) (0x0E55) & ((vip_uint32_t) ((((1 ?
 15:0) - (0 ?
 15:0) + 1) == 32) ?
 ~0 : (~0 & ((1 << (((1 ? 15:0) - (0 ? 15:0) + 1))) - 1)))))) << (0 ? 15:0)));}while(0);
    do {vip_uint32_t __temp_data32__;
__temp_data32__ = ((((vip_uint32_t) (0)) & ~(((vip_uint32_t) (((vip_uint32_t) ((((1 ?
 25:24) - (0 ?
 25:24) + 1) == 32) ?
 ~0 : (~0 & ((1 << (((1 ?
 25:24) - (0 ?
 25:24) + 1))) - 1)))))) << (0 ?
 25:24))) | (((vip_uint32_t) (0x1 & ((vip_uint32_t) ((((1 ?
 25:24) - (0 ?
 25:24) + 1) == 32) ?
 ~0 : (~0 & ((1 << (((1 ?
 25:24) - (0 ?
 25:24) + 1))) - 1)))))) << (0 ?
 25:24))) | ((((vip_uint32_t) (0)) & ~(((vip_uint32_t) (((vip_uint32_t) ((((1 ?
 19:16) - (0 ?
 19:16) + 1) == 32) ?
 ~0 : (~0 & ((1 << (((1 ?
 19:16) - (0 ?
 19:16) + 1))) - 1)))))) << (0 ?
 19:16))) | (((vip_uint32_t) ((vip_uint32_t) (0x0) & ((vip_uint32_t) ((((1 ?
 19:16) - (0 ?
 19:16) + 1) == 32) ?
 ~0 : (~0 & ((1 << (((1 ?
 19:16) - (0 ?
 19:16) + 1))) - 1)))))) << (0 ?
 19:16))) | ((((vip_uint32_t) (0)) & ~(((vip_uint32_t) (((vip_uint32_t) ((((1 ?
 15:0) - (0 ?
 15:0) + 1) == 32) ?
 ~0 : (~0 & ((1 << (((1 ?
 15:0) - (0 ?
 15:0) + 1))) - 1)))))) << (0 ?
 15:0))) | (((vip_uint32_t) ((vip_uint32_t) (COUNTER_SEL[i]) & ((vip_uint32_t) ((((1 ?
 15:0) - (0 ?
 15:0) + 1) == 32) ?
 ~0 : (~0 & ((1 << (((1 ? 15:0) - (0 ? 15:0) + 1))) - 1)))))) << (0 ? 15:0))); ;
*memory++ = __temp_data32__;
} while(0);
}while(0);

                do {    do {    *memory++        = ((((vip_uint32_t) (0)) & ~(((vip_uint32_t) (((vip_uint32_t) ((((1 ?
 31:27) - (0 ?
 31:27) + 1) == 32) ?
 ~0 : (~0 & ((1 << (((1 ?
 31:27) - (0 ?
 31:27) + 1))) - 1)))))) << (0 ?
 31:27))) | (((vip_uint32_t) (0x01 & ((vip_uint32_t) ((((1 ?
 31:27) - (0 ?
 31:27) + 1) == 32) ?
 ~0 : (~0 & ((1 << (((1 ?
 31:27) - (0 ?
 31:27) + 1))) - 1)))))) << (0 ?
 31:27)))        | ((((vip_uint32_t) (0)) & ~(((vip_uint32_t) (((vip_uint32_t) ((((1 ?
 26:26) - (0 ?
 26:26) + 1) == 32) ?
 ~0 : (~0 & ((1 << (((1 ?
 26:26) - (0 ?
 26:26) + 1))) - 1)))))) << (0 ?
 26:26))) | (((vip_uint32_t) ((vip_uint32_t) (vip_false_e) & ((vip_uint32_t) ((((1 ?
 26:26) - (0 ?
 26:26) + 1) == 32) ?
 ~0 : (~0 & ((1 << (((1 ?
 26:26) - (0 ?
 26:26) + 1))) - 1)))))) << (0 ?
 26:26)))        | ((((vip_uint32_t) (0)) & ~(((vip_uint32_t) (((vip_uint32_t) ((((1 ?
 25:16) - (0 ?
 25:16) + 1) == 32) ?
 ~0 : (~0 & ((1 << (((1 ?
 25:16) - (0 ?
 25:16) + 1))) - 1)))))) << (0 ?
 25:16))) | (((vip_uint32_t) ((vip_uint32_t) (1) & ((vip_uint32_t) ((((1 ?
 25:16) - (0 ?
 25:16) + 1) == 32) ?
 ~0 : (~0 & ((1 << (((1 ?
 25:16) - (0 ?
 25:16) + 1))) - 1)))))) << (0 ?
 25:16)))        | ((((vip_uint32_t) (0)) & ~(((vip_uint32_t) (((vip_uint32_t) ((((1 ?
 15:0) - (0 ?
 15:0) + 1) == 32) ?
 ~0 : (~0 & ((1 << (((1 ?
 15:0) - (0 ?
 15:0) + 1))) - 1)))))) << (0 ?
 15:0))) | (((vip_uint32_t) ((vip_uint32_t) (0x0E55) & ((vip_uint32_t) ((((1 ?
 15:0) - (0 ?
 15:0) + 1) == 32) ?
 ~0 : (~0 & ((1 << (((1 ? 15:0) - (0 ? 15:0) + 1))) - 1)))))) << (0 ? 15:0)));}while(0);
    do {vip_uint32_t __temp_data32__;
__temp_data32__ = ((((vip_uint32_t) (0)) & ~(((vip_uint32_t) (((vip_uint32_t) ((((1 ?
 25:24) - (0 ?
 25:24) + 1) == 32) ?
 ~0 : (~0 & ((1 << (((1 ?
 25:24) - (0 ?
 25:24) + 1))) - 1)))))) << (0 ?
 25:24))) | (((vip_uint32_t) (0x3 & ((vip_uint32_t) ((((1 ?
 25:24) - (0 ?
 25:24) + 1) == 32) ?
 ~0 : (~0 & ((1 << (((1 ?
 25:24) - (0 ?
 25:24) + 1))) - 1)))))) << (0 ?
 25:24))) | ((((vip_uint32_t) (0)) & ~(((vip_uint32_t) (((vip_uint32_t) ((((1 ?
 19:16) - (0 ?
 19:16) + 1) == 32) ?
 ~0 : (~0 & ((1 << (((1 ?
 19:16) - (0 ?
 19:16) + 1))) - 1)))))) << (0 ?
 19:16))) | (((vip_uint32_t) ((vip_uint32_t) (0x0) & ((vip_uint32_t) ((((1 ?
 19:16) - (0 ?
 19:16) + 1) == 32) ?
 ~0 : (~0 & ((1 << (((1 ?
 19:16) - (0 ?
 19:16) + 1))) - 1)))))) << (0 ?
 19:16))) | ((((vip_uint32_t) (0)) & ~(((vip_uint32_t) (((vip_uint32_t) ((((1 ?
 15:0) - (0 ?
 15:0) + 1) == 32) ?
 ~0 : (~0 & ((1 << (((1 ?
 15:0) - (0 ?
 15:0) + 1))) - 1)))))) << (0 ?
 15:0))) | (((vip_uint32_t) ((vip_uint32_t) (COUNTER_SEL[i]) & ((vip_uint32_t) ((((1 ?
 15:0) - (0 ?
 15:0) + 1) == 32) ?
 ~0 : (~0 & ((1 << (((1 ? 15:0) - (0 ? 15:0) + 1))) - 1)))))) << (0 ? 15:0))); ;
*memory++ = __temp_data32__;
} while(0);
}while(0);

                break;
            case NBGLK_NPDCMD_PAUSE:
                /* pause counter */
                do {    do {    *memory++        = ((((vip_uint32_t) (0)) & ~(((vip_uint32_t) (((vip_uint32_t) ((((1 ?
 31:27) - (0 ?
 31:27) + 1) == 32) ?
 ~0 : (~0 & ((1 << (((1 ?
 31:27) - (0 ?
 31:27) + 1))) - 1)))))) << (0 ?
 31:27))) | (((vip_uint32_t) (0x01 & ((vip_uint32_t) ((((1 ?
 31:27) - (0 ?
 31:27) + 1) == 32) ?
 ~0 : (~0 & ((1 << (((1 ?
 31:27) - (0 ?
 31:27) + 1))) - 1)))))) << (0 ?
 31:27)))        | ((((vip_uint32_t) (0)) & ~(((vip_uint32_t) (((vip_uint32_t) ((((1 ?
 26:26) - (0 ?
 26:26) + 1) == 32) ?
 ~0 : (~0 & ((1 << (((1 ?
 26:26) - (0 ?
 26:26) + 1))) - 1)))))) << (0 ?
 26:26))) | (((vip_uint32_t) ((vip_uint32_t) (vip_false_e) & ((vip_uint32_t) ((((1 ?
 26:26) - (0 ?
 26:26) + 1) == 32) ?
 ~0 : (~0 & ((1 << (((1 ?
 26:26) - (0 ?
 26:26) + 1))) - 1)))))) << (0 ?
 26:26)))        | ((((vip_uint32_t) (0)) & ~(((vip_uint32_t) (((vip_uint32_t) ((((1 ?
 25:16) - (0 ?
 25:16) + 1) == 32) ?
 ~0 : (~0 & ((1 << (((1 ?
 25:16) - (0 ?
 25:16) + 1))) - 1)))))) << (0 ?
 25:16))) | (((vip_uint32_t) ((vip_uint32_t) (1) & ((vip_uint32_t) ((((1 ?
 25:16) - (0 ?
 25:16) + 1) == 32) ?
 ~0 : (~0 & ((1 << (((1 ?
 25:16) - (0 ?
 25:16) + 1))) - 1)))))) << (0 ?
 25:16)))        | ((((vip_uint32_t) (0)) & ~(((vip_uint32_t) (((vip_uint32_t) ((((1 ?
 15:0) - (0 ?
 15:0) + 1) == 32) ?
 ~0 : (~0 & ((1 << (((1 ?
 15:0) - (0 ?
 15:0) + 1))) - 1)))))) << (0 ?
 15:0))) | (((vip_uint32_t) ((vip_uint32_t) (0x0E55) & ((vip_uint32_t) ((((1 ?
 15:0) - (0 ?
 15:0) + 1) == 32) ?
 ~0 : (~0 & ((1 << (((1 ? 15:0) - (0 ? 15:0) + 1))) - 1)))))) << (0 ? 15:0)));}while(0);
    do {vip_uint32_t __temp_data32__;
__temp_data32__ = ((((vip_uint32_t) (0)) & ~(((vip_uint32_t) (((vip_uint32_t) ((((1 ?
 25:24) - (0 ?
 25:24) + 1) == 32) ?
 ~0 : (~0 & ((1 << (((1 ?
 25:24) - (0 ?
 25:24) + 1))) - 1)))))) << (0 ?
 25:24))) | (((vip_uint32_t) (0x2 & ((vip_uint32_t) ((((1 ?
 25:24) - (0 ?
 25:24) + 1) == 32) ?
 ~0 : (~0 & ((1 << (((1 ?
 25:24) - (0 ?
 25:24) + 1))) - 1)))))) << (0 ?
 25:24))) | ((((vip_uint32_t) (0)) & ~(((vip_uint32_t) (((vip_uint32_t) ((((1 ?
 19:16) - (0 ?
 19:16) + 1) == 32) ?
 ~0 : (~0 & ((1 << (((1 ?
 19:16) - (0 ?
 19:16) + 1))) - 1)))))) << (0 ?
 19:16))) | (((vip_uint32_t) ((vip_uint32_t) (0x0) & ((vip_uint32_t) ((((1 ?
 19:16) - (0 ?
 19:16) + 1) == 32) ?
 ~0 : (~0 & ((1 << (((1 ?
 19:16) - (0 ?
 19:16) + 1))) - 1)))))) << (0 ?
 19:16))) | ((((vip_uint32_t) (0)) & ~(((vip_uint32_t) (((vip_uint32_t) ((((1 ?
 15:0) - (0 ?
 15:0) + 1) == 32) ?
 ~0 : (~0 & ((1 << (((1 ?
 15:0) - (0 ?
 15:0) + 1))) - 1)))))) << (0 ?
 15:0))) | (((vip_uint32_t) ((vip_uint32_t) (COUNTER_SEL[i]) & ((vip_uint32_t) ((((1 ?
 15:0) - (0 ?
 15:0) + 1) == 32) ?
 ~0 : (~0 & ((1 << (((1 ? 15:0) - (0 ? 15:0) + 1))) - 1)))))) << (0 ? 15:0))); ;
*memory++ = __temp_data32__;
} while(0);
}while(0);

                break;
            case NBGLK_NPDCMD_RESUME:
                /* resume counter */
                do {    do {    *memory++        = ((((vip_uint32_t) (0)) & ~(((vip_uint32_t) (((vip_uint32_t) ((((1 ?
 31:27) - (0 ?
 31:27) + 1) == 32) ?
 ~0 : (~0 & ((1 << (((1 ?
 31:27) - (0 ?
 31:27) + 1))) - 1)))))) << (0 ?
 31:27))) | (((vip_uint32_t) (0x01 & ((vip_uint32_t) ((((1 ?
 31:27) - (0 ?
 31:27) + 1) == 32) ?
 ~0 : (~0 & ((1 << (((1 ?
 31:27) - (0 ?
 31:27) + 1))) - 1)))))) << (0 ?
 31:27)))        | ((((vip_uint32_t) (0)) & ~(((vip_uint32_t) (((vip_uint32_t) ((((1 ?
 26:26) - (0 ?
 26:26) + 1) == 32) ?
 ~0 : (~0 & ((1 << (((1 ?
 26:26) - (0 ?
 26:26) + 1))) - 1)))))) << (0 ?
 26:26))) | (((vip_uint32_t) ((vip_uint32_t) (vip_false_e) & ((vip_uint32_t) ((((1 ?
 26:26) - (0 ?
 26:26) + 1) == 32) ?
 ~0 : (~0 & ((1 << (((1 ?
 26:26) - (0 ?
 26:26) + 1))) - 1)))))) << (0 ?
 26:26)))        | ((((vip_uint32_t) (0)) & ~(((vip_uint32_t) (((vip_uint32_t) ((((1 ?
 25:16) - (0 ?
 25:16) + 1) == 32) ?
 ~0 : (~0 & ((1 << (((1 ?
 25:16) - (0 ?
 25:16) + 1))) - 1)))))) << (0 ?
 25:16))) | (((vip_uint32_t) ((vip_uint32_t) (1) & ((vip_uint32_t) ((((1 ?
 25:16) - (0 ?
 25:16) + 1) == 32) ?
 ~0 : (~0 & ((1 << (((1 ?
 25:16) - (0 ?
 25:16) + 1))) - 1)))))) << (0 ?
 25:16)))        | ((((vip_uint32_t) (0)) & ~(((vip_uint32_t) (((vip_uint32_t) ((((1 ?
 15:0) - (0 ?
 15:0) + 1) == 32) ?
 ~0 : (~0 & ((1 << (((1 ?
 15:0) - (0 ?
 15:0) + 1))) - 1)))))) << (0 ?
 15:0))) | (((vip_uint32_t) ((vip_uint32_t) (0x0E55) & ((vip_uint32_t) ((((1 ?
 15:0) - (0 ?
 15:0) + 1) == 32) ?
 ~0 : (~0 & ((1 << (((1 ? 15:0) - (0 ? 15:0) + 1))) - 1)))))) << (0 ? 15:0)));}while(0);
    do {vip_uint32_t __temp_data32__;
__temp_data32__ = ((((vip_uint32_t) (0)) & ~(((vip_uint32_t) (((vip_uint32_t) ((((1 ?
 25:24) - (0 ?
 25:24) + 1) == 32) ?
 ~0 : (~0 & ((1 << (((1 ?
 25:24) - (0 ?
 25:24) + 1))) - 1)))))) << (0 ?
 25:24))) | (((vip_uint32_t) (0x3 & ((vip_uint32_t) ((((1 ?
 25:24) - (0 ?
 25:24) + 1) == 32) ?
 ~0 : (~0 & ((1 << (((1 ?
 25:24) - (0 ?
 25:24) + 1))) - 1)))))) << (0 ?
 25:24))) | ((((vip_uint32_t) (0)) & ~(((vip_uint32_t) (((vip_uint32_t) ((((1 ?
 19:16) - (0 ?
 19:16) + 1) == 32) ?
 ~0 : (~0 & ((1 << (((1 ?
 19:16) - (0 ?
 19:16) + 1))) - 1)))))) << (0 ?
 19:16))) | (((vip_uint32_t) ((vip_uint32_t) (0x0) & ((vip_uint32_t) ((((1 ?
 19:16) - (0 ?
 19:16) + 1) == 32) ?
 ~0 : (~0 & ((1 << (((1 ?
 19:16) - (0 ?
 19:16) + 1))) - 1)))))) << (0 ?
 19:16))) | ((((vip_uint32_t) (0)) & ~(((vip_uint32_t) (((vip_uint32_t) ((((1 ?
 15:0) - (0 ?
 15:0) + 1) == 32) ?
 ~0 : (~0 & ((1 << (((1 ?
 15:0) - (0 ?
 15:0) + 1))) - 1)))))) << (0 ?
 15:0))) | (((vip_uint32_t) ((vip_uint32_t) (COUNTER_SEL[i]) & ((vip_uint32_t) ((((1 ?
 15:0) - (0 ?
 15:0) + 1) == 32) ?
 ~0 : (~0 & ((1 << (((1 ? 15:0) - (0 ? 15:0) + 1))) - 1)))))) << (0 ? 15:0))); ;
*memory++ = __temp_data32__;
} while(0);
}while(0);

                break;
            case NBGLK_NPDCMD_END:
                /* wait nn exec done, and then start write probe info to ddr */
                if (i == 0) {
                    *memory++ = 0x08010429;
                    *memory++ = nn_trigger_id;
                }

                /* pass address to store counter */
                offset_bits = network->npd.index * NPD_ITEM_SIZE * 8 + NPD_ONE_SEL_SIZE_BITS * i;
                stride = (offset_bits + NPD_ITEM_HEADER_SIZE_BITS) / 32 * 4;
                if (offset_bits / 8 > network->npd.npd_buffer.size) {
                    vip_error("out of memory for store nn profile data.\n");
                    vipGoOnError(VIP_ERROR_OUT_OF_RESOURCE);
                }
                do {    do {    *memory++        = ((((vip_uint32_t) (0)) & ~(((vip_uint32_t) (((vip_uint32_t) ((((1 ?
 31:27) - (0 ?
 31:27) + 1) == 32) ?
 ~0 : (~0 & ((1 << (((1 ?
 31:27) - (0 ?
 31:27) + 1))) - 1)))))) << (0 ?
 31:27))) | (((vip_uint32_t) (0x01 & ((vip_uint32_t) ((((1 ?
 31:27) - (0 ?
 31:27) + 1) == 32) ?
 ~0 : (~0 & ((1 << (((1 ?
 31:27) - (0 ?
 31:27) + 1))) - 1)))))) << (0 ?
 31:27)))        | ((((vip_uint32_t) (0)) & ~(((vip_uint32_t) (((vip_uint32_t) ((((1 ?
 26:26) - (0 ?
 26:26) + 1) == 32) ?
 ~0 : (~0 & ((1 << (((1 ?
 26:26) - (0 ?
 26:26) + 1))) - 1)))))) << (0 ?
 26:26))) | (((vip_uint32_t) ((vip_uint32_t) (vip_false_e) & ((vip_uint32_t) ((((1 ?
 26:26) - (0 ?
 26:26) + 1) == 32) ?
 ~0 : (~0 & ((1 << (((1 ?
 26:26) - (0 ?
 26:26) + 1))) - 1)))))) << (0 ?
 26:26)))        | ((((vip_uint32_t) (0)) & ~(((vip_uint32_t) (((vip_uint32_t) ((((1 ?
 25:16) - (0 ?
 25:16) + 1) == 32) ?
 ~0 : (~0 & ((1 << (((1 ?
 25:16) - (0 ?
 25:16) + 1))) - 1)))))) << (0 ?
 25:16))) | (((vip_uint32_t) ((vip_uint32_t) (1) & ((vip_uint32_t) ((((1 ?
 25:16) - (0 ?
 25:16) + 1) == 32) ?
 ~0 : (~0 & ((1 << (((1 ?
 25:16) - (0 ?
 25:16) + 1))) - 1)))))) << (0 ?
 25:16)))        | ((((vip_uint32_t) (0)) & ~(((vip_uint32_t) (((vip_uint32_t) ((((1 ?
 15:0) - (0 ?
 15:0) + 1) == 32) ?
 ~0 : (~0 & ((1 << (((1 ?
 15:0) - (0 ?
 15:0) + 1))) - 1)))))) << (0 ?
 15:0))) | (((vip_uint32_t) ((vip_uint32_t) (0x0E1C) & ((vip_uint32_t) ((((1 ?
 15:0) - (0 ?
 15:0) + 1) == 32) ?
 ~0 : (~0 & ((1 << (((1 ? 15:0) - (0 ? 15:0) + 1))) - 1)))))) << (0 ? 15:0)));}while(0);
    do {vip_uint32_t __temp_data32__;
__temp_data32__ = (vip_uint32_t)(physical + stride);
*memory++ = __temp_data32__;
} while(0);
}while(0);

                vip_info("profile database: nn_engine_index=%d, core=%d, counter=%d, offset_bits=%lld, stride=%lld,"
                        "npu_physical=0x%"PRIx64".\n", network->npd.index, core_id, i, offset_bits, offset_bits/32,
                        physical + stride);

                /* probe and pass the counter */
                do {    do {    *memory++        = ((((vip_uint32_t) (0)) & ~(((vip_uint32_t) (((vip_uint32_t) ((((1 ?
 31:27) - (0 ?
 31:27) + 1) == 32) ?
 ~0 : (~0 & ((1 << (((1 ?
 31:27) - (0 ?
 31:27) + 1))) - 1)))))) << (0 ?
 31:27))) | (((vip_uint32_t) (0x01 & ((vip_uint32_t) ((((1 ?
 31:27) - (0 ?
 31:27) + 1) == 32) ?
 ~0 : (~0 & ((1 << (((1 ?
 31:27) - (0 ?
 31:27) + 1))) - 1)))))) << (0 ?
 31:27)))        | ((((vip_uint32_t) (0)) & ~(((vip_uint32_t) (((vip_uint32_t) ((((1 ?
 26:26) - (0 ?
 26:26) + 1) == 32) ?
 ~0 : (~0 & ((1 << (((1 ?
 26:26) - (0 ?
 26:26) + 1))) - 1)))))) << (0 ?
 26:26))) | (((vip_uint32_t) ((vip_uint32_t) (vip_false_e) & ((vip_uint32_t) ((((1 ?
 26:26) - (0 ?
 26:26) + 1) == 32) ?
 ~0 : (~0 & ((1 << (((1 ?
 26:26) - (0 ?
 26:26) + 1))) - 1)))))) << (0 ?
 26:26)))        | ((((vip_uint32_t) (0)) & ~(((vip_uint32_t) (((vip_uint32_t) ((((1 ?
 25:16) - (0 ?
 25:16) + 1) == 32) ?
 ~0 : (~0 & ((1 << (((1 ?
 25:16) - (0 ?
 25:16) + 1))) - 1)))))) << (0 ?
 25:16))) | (((vip_uint32_t) ((vip_uint32_t) (1) & ((vip_uint32_t) ((((1 ?
 25:16) - (0 ?
 25:16) + 1) == 32) ?
 ~0 : (~0 & ((1 << (((1 ?
 25:16) - (0 ?
 25:16) + 1))) - 1)))))) << (0 ?
 25:16)))        | ((((vip_uint32_t) (0)) & ~(((vip_uint32_t) (((vip_uint32_t) ((((1 ?
 15:0) - (0 ?
 15:0) + 1) == 32) ?
 ~0 : (~0 & ((1 << (((1 ?
 15:0) - (0 ?
 15:0) + 1))) - 1)))))) << (0 ?
 15:0))) | (((vip_uint32_t) ((vip_uint32_t) (0x0E55) & ((vip_uint32_t) ((((1 ?
 15:0) - (0 ?
 15:0) + 1) == 32) ?
 ~0 : (~0 & ((1 << (((1 ? 15:0) - (0 ? 15:0) + 1))) - 1)))))) << (0 ? 15:0)));}while(0);
    do {vip_uint32_t __temp_data32__;
__temp_data32__ = ((((vip_uint32_t) (0)) & ~(((vip_uint32_t) (((vip_uint32_t) ((((1 ?
 25:24) - (0 ?
 25:24) + 1) == 32) ?
 ~0 : (~0 & ((1 << (((1 ?
 25:24) - (0 ?
 25:24) + 1))) - 1)))))) << (0 ?
 25:24))) | (((vip_uint32_t) (0x0 & ((vip_uint32_t) ((((1 ?
 25:24) - (0 ?
 25:24) + 1) == 32) ?
 ~0 : (~0 & ((1 << (((1 ?
 25:24) - (0 ?
 25:24) + 1))) - 1)))))) << (0 ?
 25:24))) | ((((vip_uint32_t) (0)) & ~(((vip_uint32_t) (((vip_uint32_t) ((((1 ?
 19:16) - (0 ?
 19:16) + 1) == 32) ?
 ~0 : (~0 & ((1 << (((1 ?
 19:16) - (0 ?
 19:16) + 1))) - 1)))))) << (0 ?
 19:16))) | (((vip_uint32_t) ((vip_uint32_t) (0x0) & ((vip_uint32_t) ((((1 ?
 19:16) - (0 ?
 19:16) + 1) == 32) ?
 ~0 : (~0 & ((1 << (((1 ?
 19:16) - (0 ?
 19:16) + 1))) - 1)))))) << (0 ?
 19:16))) | ((((vip_uint32_t) (0)) & ~(((vip_uint32_t) (((vip_uint32_t) ((((1 ?
 15:0) - (0 ?
 15:0) + 1) == 32) ?
 ~0 : (~0 & ((1 << (((1 ?
 15:0) - (0 ?
 15:0) + 1))) - 1)))))) << (0 ?
 15:0))) | (((vip_uint32_t) ((vip_uint32_t) (COUNTER_SEL[i]) & ((vip_uint32_t) ((((1 ?
 15:0) - (0 ?
 15:0) + 1) == 32) ?
 ~0 : (~0 & ((1 << (((1 ? 15:0) - (0 ? 15:0) + 1))) - 1)))))) << (0 ? 15:0))); ;
*memory++ = __temp_data32__;
} while(0);
}while(0);


                /* wait hw init cycle */
                if (i ==0) {
                    do {    do {    *memory++        = ((((vip_uint32_t) (0)) & ~(((vip_uint32_t) (((vip_uint32_t) ((((1 ?
 31:27) - (0 ?
 31:27) + 1) == 32) ?
 ~0 : (~0 & ((1 << (((1 ?
 31:27) - (0 ?
 31:27) + 1))) - 1)))))) << (0 ?
 31:27))) | (((vip_uint32_t) (0x01 & ((vip_uint32_t) ((((1 ?
 31:27) - (0 ?
 31:27) + 1) == 32) ?
 ~0 : (~0 & ((1 << (((1 ?
 31:27) - (0 ?
 31:27) + 1))) - 1)))))) << (0 ?
 31:27)))        | ((((vip_uint32_t) (0)) & ~(((vip_uint32_t) (((vip_uint32_t) ((((1 ?
 26:26) - (0 ?
 26:26) + 1) == 32) ?
 ~0 : (~0 & ((1 << (((1 ?
 26:26) - (0 ?
 26:26) + 1))) - 1)))))) << (0 ?
 26:26))) | (((vip_uint32_t) ((vip_uint32_t) (vip_false_e) & ((vip_uint32_t) ((((1 ?
 26:26) - (0 ?
 26:26) + 1) == 32) ?
 ~0 : (~0 & ((1 << (((1 ?
 26:26) - (0 ?
 26:26) + 1))) - 1)))))) << (0 ?
 26:26)))        | ((((vip_uint32_t) (0)) & ~(((vip_uint32_t) (((vip_uint32_t) ((((1 ?
 25:16) - (0 ?
 25:16) + 1) == 32) ?
 ~0 : (~0 & ((1 << (((1 ?
 25:16) - (0 ?
 25:16) + 1))) - 1)))))) << (0 ?
 25:16))) | (((vip_uint32_t) ((vip_uint32_t) (1) & ((vip_uint32_t) ((((1 ?
 25:16) - (0 ?
 25:16) + 1) == 32) ?
 ~0 : (~0 & ((1 << (((1 ?
 25:16) - (0 ?
 25:16) + 1))) - 1)))))) << (0 ?
 25:16)))        | ((((vip_uint32_t) (0)) & ~(((vip_uint32_t) (((vip_uint32_t) ((((1 ?
 15:0) - (0 ?
 15:0) + 1) == 32) ?
 ~0 : (~0 & ((1 << (((1 ?
 15:0) - (0 ?
 15:0) + 1))) - 1)))))) << (0 ?
 15:0))) | (((vip_uint32_t) ((vip_uint32_t) (0x0E1C) & ((vip_uint32_t) ((((1 ?
 15:0) - (0 ?
 15:0) + 1) == 32) ?
 ~0 : (~0 & ((1 << (((1 ? 15:0) - (0 ? 15:0) + 1))) - 1)))))) << (0 ? 15:0)));}while(0);
    do {vip_uint32_t __temp_data32__;
__temp_data32__ = (vip_uint32_t)(physical + stride);
*memory++ = __temp_data32__;
} while(0);
}while(0);

                    do {    do {    *memory++        = ((((vip_uint32_t) (0)) & ~(((vip_uint32_t) (((vip_uint32_t) ((((1 ?
 31:27) - (0 ?
 31:27) + 1) == 32) ?
 ~0 : (~0 & ((1 << (((1 ?
 31:27) - (0 ?
 31:27) + 1))) - 1)))))) << (0 ?
 31:27))) | (((vip_uint32_t) (0x01 & ((vip_uint32_t) ((((1 ?
 31:27) - (0 ?
 31:27) + 1) == 32) ?
 ~0 : (~0 & ((1 << (((1 ?
 31:27) - (0 ?
 31:27) + 1))) - 1)))))) << (0 ?
 31:27)))        | ((((vip_uint32_t) (0)) & ~(((vip_uint32_t) (((vip_uint32_t) ((((1 ?
 26:26) - (0 ?
 26:26) + 1) == 32) ?
 ~0 : (~0 & ((1 << (((1 ?
 26:26) - (0 ?
 26:26) + 1))) - 1)))))) << (0 ?
 26:26))) | (((vip_uint32_t) ((vip_uint32_t) (vip_false_e) & ((vip_uint32_t) ((((1 ?
 26:26) - (0 ?
 26:26) + 1) == 32) ?
 ~0 : (~0 & ((1 << (((1 ?
 26:26) - (0 ?
 26:26) + 1))) - 1)))))) << (0 ?
 26:26)))        | ((((vip_uint32_t) (0)) & ~(((vip_uint32_t) (((vip_uint32_t) ((((1 ?
 25:16) - (0 ?
 25:16) + 1) == 32) ?
 ~0 : (~0 & ((1 << (((1 ?
 25:16) - (0 ?
 25:16) + 1))) - 1)))))) << (0 ?
 25:16))) | (((vip_uint32_t) ((vip_uint32_t) (1) & ((vip_uint32_t) ((((1 ?
 25:16) - (0 ?
 25:16) + 1) == 32) ?
 ~0 : (~0 & ((1 << (((1 ?
 25:16) - (0 ?
 25:16) + 1))) - 1)))))) << (0 ?
 25:16)))        | ((((vip_uint32_t) (0)) & ~(((vip_uint32_t) (((vip_uint32_t) ((((1 ?
 15:0) - (0 ?
 15:0) + 1) == 32) ?
 ~0 : (~0 & ((1 << (((1 ?
 15:0) - (0 ?
 15:0) + 1))) - 1)))))) << (0 ?
 15:0))) | (((vip_uint32_t) ((vip_uint32_t) (0x0E55) & ((vip_uint32_t) ((((1 ?
 15:0) - (0 ?
 15:0) + 1) == 32) ?
 ~0 : (~0 & ((1 << (((1 ? 15:0) - (0 ? 15:0) + 1))) - 1)))))) << (0 ? 15:0)));}while(0);
    do {vip_uint32_t __temp_data32__;
__temp_data32__ = ((((vip_uint32_t) (0)) & ~(((vip_uint32_t) (((vip_uint32_t) ((((1 ?
 25:24) - (0 ?
 25:24) + 1) == 32) ?
 ~0 : (~0 & ((1 << (((1 ?
 25:24) - (0 ?
 25:24) + 1))) - 1)))))) << (0 ?
 25:24))) | (((vip_uint32_t) (0x0 & ((vip_uint32_t) ((((1 ?
 25:24) - (0 ?
 25:24) + 1) == 32) ?
 ~0 : (~0 & ((1 << (((1 ?
 25:24) - (0 ?
 25:24) + 1))) - 1)))))) << (0 ?
 25:24))) | ((((vip_uint32_t) (0)) & ~(((vip_uint32_t) (((vip_uint32_t) ((((1 ?
 19:16) - (0 ?
 19:16) + 1) == 32) ?
 ~0 : (~0 & ((1 << (((1 ?
 19:16) - (0 ?
 19:16) + 1))) - 1)))))) << (0 ?
 19:16))) | (((vip_uint32_t) ((vip_uint32_t) (0x0) & ((vip_uint32_t) ((((1 ?
 19:16) - (0 ?
 19:16) + 1) == 32) ?
 ~0 : (~0 & ((1 << (((1 ?
 19:16) - (0 ?
 19:16) + 1))) - 1)))))) << (0 ?
 19:16))) | ((((vip_uint32_t) (0)) & ~(((vip_uint32_t) (((vip_uint32_t) ((((1 ?
 15:0) - (0 ?
 15:0) + 1) == 32) ?
 ~0 : (~0 & ((1 << (((1 ?
 15:0) - (0 ?
 15:0) + 1))) - 1)))))) << (0 ?
 15:0))) | (((vip_uint32_t) ((vip_uint32_t) (COUNTER_SEL[i]) & ((vip_uint32_t) ((((1 ?
 15:0) - (0 ?
 15:0) + 1) == 32) ?
 ~0 : (~0 & ((1 << (((1 ? 15:0) - (0 ? 15:0) + 1))) - 1)))))) << (0 ? 15:0))); ;
*memory++ = __temp_data32__;
} while(0);
}while(0);

                }
                break;

            default:
                vip_error("invalid probe command\n");
                return VIP_ERROR_INVALID_ARGUMENTS;
                break;
        }
    }

    if (cmd == NBGLK_NPDCMD_END) {
        network->npd.index++;
    }

    if (core_num > 1) {
        #if vpmdENABLE_MULTIPLE_VIP
        nbglk_append_chip_enable(memory, core_id, vip_true_e);
        memory += 2;
        #endif
    }

onError:
    return status;
}

vip_status_e nbglk_npd_save_file(
    vip_network_t *network
    )
{
    vip_status_e status = VIP_SUCCESS;
    vip_char_t file_name[256];
    viphal_file_t file= VIP_NULL;
    vip_uint32_t pid = viphal_os_get_pid();
    vip_uint32_t tid = viphal_os_get_tid();
    vip_uint32_t time = (vip_uint32_t)(viphal_os_get_time() / 1000);/* 1ms unit */

    viphal_os_snprint(file_name, 255, "%s/%s_%x_%x_%x.npd",
            vpmdSAVE_FILE_PATH, network->nbg_desc.net_info.name, pid, tid, time);
    file = viphal_os_open_file(file_name, FILE_APPENDTEXT);
    if (file == VIP_NULL) {
        vip_print("fail to open %s file for save npd\n", file_name);
        return VIP_ERROR_IO;
    }

    status = viphal_os_write_file((void*)network->npd.npd_buffer.logical,
            network->npd.npd_buffer.size, file);
    if (status != VIP_SUCCESS) {
        vip_print("fail to save npd file.\n");
    }

    return status;
}

vip_uint32_t nbglk_npd_states_size(
    vip_network_t *network,
    nbglk_segment_t *segment
    )
{
    vip_uint32_t nn_operation_count = 0, one_profile_cmd_size = 0;
    vip_uint32_t states_size = 0;

    CACULATE_NN_NUM_IN_SEGMENT(segment, nn_operation_count)
    ONE_OP_PROFILE_CMD_SIZE(network->device->core_count, one_profile_cmd_size)
    states_size += nn_operation_count * one_profile_cmd_size;

    return states_size;
}

vip_status_e nbglk_npd_init(
    vip_network_t *network
    )
{
    vip_status_e status = VIP_SUCCESS;
    nbglk_videomemory_t *allocation = VIP_NULL;
    vip_uint32_t nn_operation_count = 0;
    vip_uint32_t header_size = sizeof(nbglk_npd_network_info_t) + sizeof(nbglk_npd_header_t);
    CACULATE_NN_NUM_IN_NETWORK(network, nn_operation_count)
    /* memory layout for nn operation probe info:
     * header + network_info +
     * operation_0[node_id:64 + core_id:64 + sel_0:128 + ... +sel_x:128] +...+
     * operation_x[node_id:64 + core_id:64 + sel0:128 + ...+ sel_x:128]] */
    network->npd.npd_buffer.size = NPD_ITEM_SIZE * nn_operation_count + header_size;
    allocation = &network->npd.npd_buffer;
    vipOnError(viphal_allocate_videomemory((vip_uint8_t **)&allocation->logical,
                                           &allocation->mem_id, &allocation->physical,
                                           allocation->size, vipdMEMORY_ALIGN_SIZE,
                                           VIPDRV_VIDEO_MEM_ALLOC_MAP_KERNEL));
    vipOnError(nbglk_npd_generate_header_info(network));

onError:
    return status;
}

vip_status_e nbglk_npd_uninit(
    vip_network_t *network
    )
{
    vip_status_e status = VIP_SUCCESS;

    if (network->npd.npd_buffer.logical != VIP_NULL) {
        viphal_free_videomemory(network->npd.npd_buffer.mem_id);
        network->npd.npd_buffer.logical = VIP_NULL;
    }

    return status;
}

