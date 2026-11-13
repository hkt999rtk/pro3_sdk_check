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

#include <vip_hal.h>
#include <nbg_lk_capture.h>
#include <nbg_fmt_util.h>
#include <nbg_lk_relocate.h>
#include <nbg_lk_debug.h>
#include <nbg_lk_group.h>
#include <nbg_lk_coeff.h>
#if vpmdENABLE_NODE
#include <nbg_lk_node.h>
#endif

#if (vpmdENABLE_HANG_DUMP > 1) || vpmdENABLE_CAPTURE
static void nbglk_capture_trigger(
    vip_network network,
    viphal_file_t capture_file,
    vip_uint32_t physical,
    vip_uint32_t size
    )
{
    vip_uint32_t i = 0;
    vip_char_t buffer[256];
    for (i = 0; i < network->nbg_desc.net_info.core_count; i++) {
        vip_uint32_t core_id = network->device->core_id[i + network->core_index];
        NBGLK_GET_STRING(buffer, 255, "@[register.write %d 0x%05X 0x%08X]\n",
            core_id, 0x00654, physical);
        NBGLK_DUMP_STRING(capture_file, buffer);
        NBGLK_GET_STRING(buffer, 255, "@[register.write %d 0x%05X 0x%08X]\n",
            core_id, 0x003A4, 0x00010000 | ((size + 7) >> 3));
        NBGLK_DUMP_STRING(capture_file, buffer);
    }

    NBGLK_DUMP_STRING(capture_file, "@[stall]\n\n");
}

void nbglk_capture_buffer(
    viphal_file_t fp,
    void *buffers,
    vip_uint64_t physical,
    vip_size_t size,
    nbglk_capture_buf_type_e type
    )
{
    vip_char_t info[256];
    vip_uint32_t *buffer = (vip_uint32_t *)buffers;
    static const vip_char_t *tags[] = {
        "command",
        "states",
        "pool",
        "input",
        "verify",
        "memory"
    };
    vip_uint32_t data[4];

    if ((VIP_NULL == buffers) || (VIP_NULL == fp)) {
        vip_error("dump buffer or file handle is null\n");
        return;
    }

    if (physical & 0xFF00000000) {
        NBGLK_GET_STRING(info, 255, "@[%s 0x%010llX ", tags[type], physical);
    }
    else {
        NBGLK_GET_STRING(info, 255, "@[%s 0x%llx ", tags[type], physical);
    }
    NBGLK_DUMP_STRING(fp, info);

    if (size & 0xFF00000000) {
        NBGLK_GET_STRING(info, 255, "0x%010llX\n", size);
    }
    else {
        NBGLK_GET_STRING(info, 255, "0x%08X\n", size);
    }
    NBGLK_DUMP_STRING(fp, info);

    while (size >= 16) {
        NBGLK_GET_STRING(info, 255, "  0x%08X 0x%08X 0x%08X 0x%08X\n",
                         buffer[0], buffer[1], buffer[2], buffer[3]);
        NBGLK_DUMP_STRING(fp, info);
        size -= 16;
        buffer += 4;
    }

    data[0] = 0; data[1] = 0; data[2] = 0; data[3] = 0;
    /*vip_info("capture %s physical=0x%llx reidue size=%d\n", tags[type], physical, size);*/
    viphal_copy_memory((void*)data, (void*)buffer, size);

    /* The remaining data. */
    switch (size) {
    case 15:
    case 14:
    case 13:
        NBGLK_GET_STRING(info, 255, "  0x%08X 0x%08X 0x%08X 0x%08X\n",
                         data[0], data[1], data[2], data[3]);
        NBGLK_DUMP_STRING(fp, info);
        break;
    case 12:
    case 11:
    case 10:
    case 9:
        NBGLK_GET_STRING(info, 255, "  0x%08X 0x%08X 0x%08X\n", data[0], data[1], data[2]);
        NBGLK_DUMP_STRING(fp, info);
        break;
    case 8:
    case 7:
    case 6:
    case 5:
        NBGLK_GET_STRING(info, 255, "  0x%08X 0x%08X\n", data[0], data[1]);
        NBGLK_DUMP_STRING(fp, info);
        break;
    case 4:
    case 3:
    case 2:
    case 1:
        NBGLK_GET_STRING(info, 255, "  0x%08X\n", data[0]);
        NBGLK_DUMP_STRING(fp, info);
        break;

    default:
        break;
    }

    NBGLK_GET_STRING(info, 255, "] -- %s\n", tags[type]);
    NBGLK_DUMP_STRING(fp, info);
}
#endif


#if (vpmdENABLE_HANG_DUMP > 1) || (vpmdENABLE_CAPTURE > 1)
static void nbglk_capture_memory_layout(
    viphal_file_t capture_file,
    vip_network network
    )
{
#define DUMP_LAYOUT_ONE(name, start, size)   \
{ \
    end = start + size; \
    NBGLK_GET_STRING(buffer, 255, "%s 0x%llx~0x%llx 0x%X]\n", name, start, end, size); \
    NBGLK_DUMP_STRING(capture_file, buffer); \
} \

    vip_char_t buffer[256];
    vip_uint32_t i = 0;
    vip_address_t end = 0;
    vip_address_t size = 0;

    NBGLK_GET_STRING(buffer, 255, "\n#[Video Memory Layout Dump]\n");
    NBGLK_DUMP_STRING(capture_file, buffer);

    for (i = 0; i < network->nbg_desc.n_inputs; i++) {
        nbglk_io_patch_info_t *input = &network->network_inputs[i];
        size = nbg_get_input_size(&network->nbg_desc, i);
        end = input->physical + size;
        NBGLK_GET_STRING(buffer, 255, "#[input_%d 0x%010llX~0x%010llX 0x%llX]\n", i, input->physical, end, size);
        NBGLK_DUMP_STRING(capture_file, buffer);
    }
    for (i = 0; i < network->nbg_desc.n_outputs; i++) {
        nbglk_io_patch_info_t *output = &network->network_outputs[i];
        size = nbg_get_output_size(&network->nbg_desc, i);
        end = output->physical + size;
        NBGLK_GET_STRING(buffer, 255, "#[output_%d 0x%010llX~0x%llX 0x%llX]\n", i, output->physical, end, size);
        NBGLK_DUMP_STRING(capture_file, buffer);
    }
    if (network->pool.size > 0) {
        DUMP_LAYOUT_ONE("#[memory pool", network->pool.physical, network->pool.size);
    }

    {
        vipdrv_query_driver_status_t drv_status = {0};
        drv_status.device_index = network->device->dev_index;
        viphal_query_driver_status(&drv_status);
        if (drv_status.init_cmd_size > 0) {
            DUMP_LAYOUT_ONE("#[initial states kernel", drv_status.init_cmd_physical, drv_status.init_cmd_size);
        }
    }
    if (network->init_commands.size > 0) {
        DUMP_LAYOUT_ONE("#[init_cmd", network->init_commands.physical, network->init_commands.size);
    }
    if (network->nbg_desc.fixed.ICD.size > 0) {
        DUMP_LAYOUT_ONE("#[ICD", network->ICD.physical, network->ICD.size);
    }
    if (0 != network->coeff_header.mem_id) {
        DUMP_LAYOUT_ONE("#[coeff header", network->coeff_header.physical,
            network->coeff_header.size);
    }
    if (network->coeff_data.size > 0) {
        DUMP_LAYOUT_ONE("#[coeff_data", network->coeff_data.physical, network->coeff_data.size);
    }
#if vpmdPRELOAD_COEFF
    if (network->app_preload_coeff != VIP_NULL) {
        DUMP_LAYOUT_ONE("#[App preload coeff", network->app_preload_coeff->physical, network->app_preload_coeff->size);
    }
#endif
    if (network->nbg_desc.fixed.header.version >= 0x00010018) {
        if (network->ppu_instr.size > 0) {
            DUMP_LAYOUT_ONE("#[PPU Instr", network->ppu_instr.physical, network->ppu_instr.size);
        }
        if (network->dyna_io.size > 0) {
            DUMP_LAYOUT_ONE("#[Dyna IO", network->dyna_io.physical, network->dyna_io.size);
        }
    }
    if (network->nnCmds.size > 0) {
        DUMP_LAYOUT_ONE("#[NNCmd", network->nnCmds.physical, network->nnCmds.size);
    }
    if (network->tpCmds.size > 0) {
        DUMP_LAYOUT_ONE("#[TPCmd", network->tpCmds.physical, network->tpCmds.size);
    }
    for (i = 0; i < network->segment_num; i++) {
        nbglk_segment_t *segment = &network->segments[i];
        if (vip_true_e == network->realloc_sh_buff && segment->sh_cmd_buffer.size > 0) {
            DUMP_LAYOUT_ONE("#[SHCmd", segment->sh_cmd_buffer.physical, segment->sh_cmd_buffer.size);
        }
        if (segment->states_buffer.size > 0) {
            DUMP_LAYOUT_ONE("#[states", segment->states_buffer.physical, segment->states_buffer.size);
        }
    }
}

static void nbglk_capture_section_check(
    viphal_file_t capture_file,
    vip_network network
    )
{
    vip_char_t buffer[256];
    vip_uint32_t i = 0;
    const vip_char_t *command_tags[] = {
        "section load_states",
        "section ppu instr",
        "section coeff data",
    };
    if ((network->nbg_desc.fixed.header.version & 0x00020000) != 0x00020000) {
        return;
    }

    NBGLK_GET_STRING(buffer, 255, "\n#[nbg section checksum status:]\n");
    NBGLK_DUMP_STRING(capture_file, buffer);
    for (i = 0; i < NBGLK_SECTION_CHECKSUM_MAX; i++) {
        vip_error("#[%s check %s]\n", command_tags[i], network->check_sect_status[i] ? "Fail" : "Pass");
        NBGLK_GET_STRING(buffer, 255, "#[%s check %s]\n", command_tags[i],
                        network->check_sect_status[i] ? "Fail" : "Pass");
        NBGLK_DUMP_STRING(capture_file, buffer);
    }
    NBGLK_GET_STRING(buffer, 255, "\n");
    NBGLK_DUMP_STRING(capture_file, buffer);
}

#if vpmdENABLE_CHECKSUM
static void nbglk_capture_network_check(
    viphal_file_t capture_file,
    vip_network network
    )
{
    vip_char_t buffer[256];
    vip_uint32_t i = 0;
    const vip_char_t *command_tags[] = {
        "nn command",
        "tp command",
        "sh command",
        "nn weight",
        "tp weight",
        "sh weight",
        "nn weight bit stream",
        "load state",
        "sp weight",
        "sp weight bit stream",
    };
    NBGLK_GET_STRING(buffer, 255, "\n#[network checksum status:]\n");
    NBGLK_DUMP_STRING(capture_file, buffer);
    for (i = 0; i < NBGLK_CHECKSUM_MAX; i++) {
        if (network->checksum[i] != 0) {
            vip_error("#[%s check %s]\n", command_tags[i], network->check_status[i] ? "Fail" : "Pass");
            NBGLK_GET_STRING(buffer, 255, "#[%s 0x%08x check %s]\n", command_tags[i], network->checksum[i],
                    network->check_status[i] ? "Fail" : "Pass");
            NBGLK_DUMP_STRING(capture_file, buffer);
        }
    }
    NBGLK_GET_STRING(buffer, 255, "\n");
    NBGLK_DUMP_STRING(capture_file, buffer);
}
#endif

#if (vpmdENABLE_HANG_DUMP > 1)
static void nbglk_capture_do_hang_dump(
    vip_network network
    )
{
    nbglk_segment_t *segment = VIP_NULL;
    nbglk_capture_network_input(network->capture_file, network);

    if (network->prepared_network) {
        vip_uint32_t i = 0;
        nbglk_capture_record_list(network->capture_file, network);
        for (i = 0; i < network->segment_num; i++) {
            segment = &network->segments[i];
            if (vip_false_e == segment->is_sw_segm) {
                nbglk_capture_dump_states(network->capture_file, network, segment);
            }
        }
    }
    nbglk_capture_network_output(network->capture_file, network);
}

/* pre-network a hang capture file */
vip_status_e nbglk_capture_hang_dump(
    vip_network_t *network
    )
{
    viphal_file_t capture_file = VIP_NULL;

    if (VIP_SUCCESS != nbglk_capture_init(network, VIP_NULL, NBGLK_CAPTURE_TYPE_HANG)) {
        vip_error("catpure init error, not capture hang dump.\n");
        return VIP_ERROR_FAILURE;
    }

    capture_file = network->capture_file;

    if (VIP_NULL == capture_file) {
        return VIP_SUCCESS;
    }

    nbglk_capture_header_info(capture_file, network);
    nbglk_capture_register_info(capture_file, network);
#if vpmdENABLE_CAPTURE_MMU
    nbglk_capture_mmu_info(capture_file, network);
#endif
    nbglk_capture_init_command(capture_file, network);
#if vpmdPRELOAD_COEFF > 1
    nbglk_capture_npu_preload_coeff(capture_file, network);
#endif
    nbglk_capture_do_hang_dump(network);

    /* save video memory info used by this network */
    nbglk_capture_memory_layout(capture_file, network);

#if vpmdENABLE_CHECKSUM
    nbglk_capture_network_check(capture_file, network);
#endif
    nbglk_capture_section_check(capture_file, network);

    network->capture_done = 1;
    nbglk_capture_uninit(network, VIP_NULL);
    return VIP_SUCCESS;
}
#endif
#endif


#if vpmdENABLE_CAPTURE || (vpmdENABLE_HANG_DUMP > 1)
static void capture_dump_buffer(
    viphal_file_t fp,
    vip_network_t *network,
    nbglk_segment_t *segment,
    nbglk_record_info_t *recd_info
    )
{
    vip_char_t buffer[256];
    const vip_char_t *command_tags[] = {
    "none",
    "nn command",
    "tp command",
    "sh command",
    "nn weight",
    "tp weight",
    "sh weight",
    "dyna data",
    "nn weight bit stream",
    "sp weight",
    "sp weight bit stream",
    "sp command",
    "nn_lut command",
    "nt command", /* 13 */
    "none"
    };
    /*
        op_index: The index of operation order submited to VIP hardware.
        node: The id of node in OVX graph->nodeTable[x].nodeID, the nodeID is after graph opt.
        uid: unique id in application.
        abs_op: the absolute index in OVX operation->absoluteOperationID.
        name: layer name in application.
    */
    NBGLK_GET_STRING(buffer, 255, "#[%s, op_index:%d, nodeId:%d, abs_op:%d, uid:%d, name:%s]\n",
                     command_tags[recd_info->record_type], recd_info->op_index,
                     network->nbg_desc.orig_layers[recd_info->layer_id].node_id,
                     recd_info->abs_op_id,
                     network->nbg_desc.orig_layers[recd_info->layer_id].uid,
                     network->nbg_desc.orig_layers[recd_info->layer_id].name);
    NBGLK_DUMP_STRING(fp, buffer);
    switch (recd_info->record_type) {
    case RECORD_TYPE_NN_COMMAND:
    case RECORD_TYPE_SP_COMMAND:
    case RECORD_TYPE_NN_LUT_COMMAND:
    case RECORD_TYPE_NT_COMMAND:
    {
        nbglk_capture_buffer(fp, network->nnCmds.logical + recd_info->offset,
                          network->nnCmds.physical + recd_info->offset,
                          recd_info->size, NBGLK_CAPTURE_BUF_MEMORY);
    }
    break;

    case RECORD_TYPE_TP_COMMAND:
    {
        nbglk_capture_buffer(fp, network->tpCmds.logical + recd_info->offset,
                          network->tpCmds.physical + recd_info->offset,
                          recd_info->size, NBGLK_CAPTURE_BUF_MEMORY);
    }
    break;

    case RECORD_TYPE_SH_COMMAND:
    {
        if (vip_true_e == network->realloc_sh_buff) {
            nbglk_capture_buffer(fp, segment->sh_cmd_buffer.logical + recd_info->offset,
                              segment->sh_cmd_buffer.physical + recd_info->offset,
                              recd_info->size, NBGLK_CAPTURE_BUF_MEMORY);
        }
        else {
            nbglk_capture_buffer(fp, network->ppu_instr.logical + recd_info->offset,
                              network->ppu_instr.physical + recd_info->offset,
                              recd_info->size, NBGLK_CAPTURE_BUF_MEMORY);
        }
    }
    break;
#if (vpmdENABLE_CAPTURE > 1) || (vpmdENABLE_HANG_DUMP > 1)
    case RECORD_TYPE_NN_WEIGHT:
    case RECORD_TYPE_SP_WEIGHT:
    {
        if (0 != network->coeff_header.mem_id) {
            /* kernel descriptor */
            nbglk_capture_buffer(fp, network->coeff_header.logical + recd_info->offset,
                network->coeff_header.physical + recd_info->offset, recd_info->size, NBGLK_CAPTURE_BUF_MEMORY);
            break;
        }
    }
    case RECORD_TYPE_NN_WEIGHT_BITSTREAM:
    case RECORD_TYPE_TP_WEIGHT:
    case RECORD_TYPE_SH_WEIGHT:
    case RECORD_TYPE_SP_WEIGHT_BITSTREAM:
    {
        vip_size_t offset = 0;
        vip_address_t base_physical = 0;
        vip_uint8_t *base_logical = VIP_NULL;
        nbglk_coeff_get_property(network, (vip_uint32_t)recd_info->offset, &base_logical,
            &base_physical, &offset, VIP_NULL);
        nbglk_capture_buffer(fp, base_logical + offset, base_physical + offset,
                             recd_info->size, NBGLK_CAPTURE_BUF_MEMORY);
    }
    break;
    case RECORD_TYPE_DYNA_DATA:
    {
        nbglk_capture_buffer(fp, network->dyna_io.logical + recd_info->offset,
                      network->dyna_io.physical + recd_info->offset, recd_info->size, NBGLK_CAPTURE_BUF_MEMORY);
    }
    break;
#endif
    default:
        break;
    }
}

void nbglk_capture_record_list(
    viphal_file_t capture_file,
    vip_network_t *network
    )
{
    vip_uint32_t i = 0, j = 0;

    if ((VIP_NULL == network) || (VIP_NULL == capture_file)) {
        return;
    }

    if (1 == network->capture_done) {
        return;
    }

    for (i = 0; i < network->segment_num; i++) {
        nbglk_segment_t *segment = &network->segments[i];
        for (j = 1; j < nbglk_hashmap_free_pos(&segment->record_list); j++) {
            nbglk_record_info_t *record_info = VIP_NULL;
            nbglk_hashmap_get_by_index(&segment->record_list, j, (void**)&record_info);
            capture_dump_buffer(capture_file, network, segment, record_info);
        }
    }
}

vip_status_e nbglk_capture_dump_states(
    viphal_file_t capture_file,
    vip_network_t *network,
    nbglk_segment_t *segment
    )
{
    vip_uint32_t offset = CACHE_CMD_SIZE;

    if ((VIP_NULL == network) || (VIP_NULL == capture_file)) {
        return VIP_SUCCESS;
    }

    CHECK_CAPTURE_FUNC_CONFIG(network);

    if (1 == network->capture_done) {
        return VIP_SUCCESS;
    }
#if vpmdENABLE_MULTIPLE_TASK && vpmdENABLE_CAPTURE
    nbglk_lock_recursive_mutex(&network->context->capture_mutex);
#endif
#if vpmdENABLE_CNN_PROFILING
    if (network->context->func_config.enable_cnn_profile == vip_true_e) {
        if (segment->name != VIP_NULL) {
            vip_char_t info[256];
            NBGLK_GET_STRING(info, 255, "#[layer:%d, uid:%d, name:%s]\n",
                segment->segm_start_id, segment->uid, segment->name);
            NBGLK_DUMP_STRING(capture_file, info);
        }
    }
#endif
    if (segment->states_buffer.size > 0) {
        vip_uint32_t trig_size = 0;
        vip_uint32_t cmd_size = (SIZE_CAST(vip_uint32_t, segment->states_buffer.size) > segment->command_size) ? \
                                 SIZE_CAST(vip_uint32_t, segment->states_buffer.size) : segment->command_size;
        NBGLK_DUMP_STRING(capture_file, "#[states]\n");
        nbglk_capture_buffer(capture_file, segment->states_buffer.logical + offset,
                          segment->states_buffer.physical + offset,
                          cmd_size - offset, NBGLK_CAPTURE_BUF_COMMAND);
        if ((cmd_size - offset) > (vipdMAX_CMD_SIZE - 7)) {
            trig_size =vipdMAX_CMD_SIZE - 7;
        }
        else {
            trig_size = cmd_size - offset;
        }
        nbglk_capture_trigger(network, capture_file,
            SIZE_CAST(vip_uint32_t, segment->states_buffer.physical + offset), trig_size);
    }

#if (vpmdENABLE_CAPTURE == 1)
    if (segment == &network->segments[network->segment_num - 1]) {
        /* check dumping the last segment now, don't catpure output data when CAPTURE is 1
           so, set capture_done=1 */
        network->capture_done = 1;
    }
#endif
#if vpmdENABLE_MULTIPLE_TASK && vpmdENABLE_CAPTURE
    nbglk_unlock_recursive_mutex(&network->context->capture_mutex);
#endif

    return VIP_SUCCESS;
}

#if vpmdENABLE_CAPTURE_MMU
vip_status_e nbglk_capture_mmu_info(
    viphal_file_t capture_file,
    vip_network network
    )
{
    vip_status_e status = VIP_SUCCESS;
    vipdrv_query_mmu_info_t info = {0};
    vip_int32_t i = 0, stlb_index = 0;
    vip_uint32_t core_id = 0;
    vip_uint8_t *logical = VIP_NULL;
    vip_uint32_t mem_id = 0;
    vip_uint32_t *data = VIP_NULL;
    vip_char_t info_buf[256];
    vip_uint32_t pdmode = viphal_query_hw_feature(VIP_HW_FEATURE_MMU_PDMODE);

    if ((VIP_NULL == network) || (VIP_NULL == capture_file)) {
        vip_error("fail captur mmu info network=%p\n", network);
        return VIP_ERROR_FAILURE;
    }

    status = viphal_query_mmu_info(&info);
    if (status != VIP_SUCCESS) {
        return status;
    }
    /* check mmu info */
    if (0 == info.mtlb_mem_id || 0 == info.mtlb_size) {
        return status;
    }

    if (vip_true_e == pdmode) {
        NBGLK_DUMP_STRING(capture_file, "#[mmu pd mode]\n");
    }
    else {
        vip_debug("entry mem_id=0x%08x, physical=0x%"PRIx64"\n", info.entry_mem_id, info.entry_physical);
        mem_id = info.entry_mem_id;
        vipOnError(viphal_map_userlogical(mem_id, &logical));
        if (logical != VIP_NULL) {
            NBGLK_DUMP_STRING(capture_file, "#[mmu entry]\n");
            nbglk_capture_buffer(capture_file, logical, info.entry_physical, 64, NBGLK_CAPTURE_BUF_MEMORY);
        }
        viphal_unmap_userlogical(mem_id);
    }

    vip_debug("mtlb mem_id=0x%08x, physical=0x%"PRIx64"\n", info.mtlb_mem_id, info.mtlb_physical);
    mem_id = info.mtlb_mem_id;
    vipOnError(viphal_map_userlogical(mem_id, &logical));
    if (logical != VIP_NULL) {
        data = (vip_uint32_t*)logical;
        for (i = (vip_int32_t)(info.mtlb_size / sizeof(vip_uint32_t)) - 1; i >= 0; i--) {
            if (data[i] > 0) {
                break;
            }
        }
        if (i > 0) {
            NBGLK_DUMP_STRING(capture_file, "#[mmu mtlb]\n");
            nbglk_capture_buffer(capture_file, logical, info.mtlb_physical,
                              (i+1) * sizeof(vip_uint32_t), NBGLK_CAPTURE_BUF_MEMORY);
        }
    }
    viphal_unmap_userlogical(mem_id);

    for (stlb_index = 0; stlb_index < MAX_STLB_MEM_CNT; stlb_index++) {
        mem_id = info.stlb_mem_id[stlb_index];
        if (0 == mem_id) {
            break;
        }
        vip_debug("stlb mem_id=0x%08x, physical=0x%"PRIx64"\n", mem_id, info.stlb_physical[stlb_index]);
        vipOnError(viphal_map_userlogical(mem_id, &logical));
        if (logical != VIP_NULL) {
            data = (vip_uint32_t*)logical;
            for (i = (vip_int32_t)(info.stlb_size[stlb_index] / sizeof(vip_uint32_t)) - 1; i >= 0; i--) {
                /* 0 bit indicate whether present or not. */
                if (data[i] & (1 << 0)) {
                    break;
                }
            }
            if (i > 0) {
                NBGLK_DUMP_STRING(capture_file, "#[mmu stlb]\n");
                nbglk_capture_buffer(capture_file, logical, info.stlb_physical[stlb_index],
                    (i + 1) * sizeof(vip_uint32_t), NBGLK_CAPTURE_BUF_MEMORY);
            }
        }
        viphal_unmap_userlogical(mem_id);
    }

    if (vip_true_e == pdmode) {
        for (i = 0; i < network->nbg_desc.net_info.core_count; i++) {
            vip_physical_t physical_tmp = 0;
            vip_uint32_t pd_entry_value = 0;
            physical_tmp = info.mtlb_physical >> 12;
            #if vpmdENABLE_40BITVA
            pd_entry_value = ((vip_uint32_t)(physical_tmp << 4) |
                0x0);
            #else
            pd_entry_value = ((vip_uint32_t)(physical_tmp << 4) |
                0x1);
            #endif
            core_id = network->device->core_id[i + network->core_index];
            NBGLK_GET_STRING(info_buf, 255, "@[register.write %u 0x003B4 0x%08X]\n", core_id, pd_entry_value);
            NBGLK_DUMP_STRING(capture_file, info_buf);
            NBGLK_GET_STRING(info_buf, 255, "@[register.write %u 0x00388 0x00000021]\n\n", core_id);
            NBGLK_DUMP_STRING(capture_file, info_buf);
        }
    }
    else {
        {
        vip_physical_t tmp = 0;
        vip_uint32_t page_table_l = 0;
        vip_uint32_t page_table_h = 0;
        tmp = info.entry_physical >> 16;
        page_table_h = (vip_uint32_t)((tmp >> 16) & 0xFFFFFFFF);
        page_table_l = (vip_uint32_t)(info.entry_physical & 0xFFFFFFFF);
        NBGLK_DUMP_STRING(capture_file, "#[setup mmu]\n");
        for (i = 0; i < network->nbg_desc.net_info.core_count; i++) {
            core_id = network->device->core_id[i + network->core_index];
            NBGLK_GET_STRING(info_buf, 255, "@[register.write %u 0x0038c 0x%08x]\n", core_id, page_table_l);
            NBGLK_DUMP_STRING(capture_file, info_buf);
            NBGLK_GET_STRING(info_buf, 255, "@[register.write %u 0x00390 0x%08x]\n", core_id, page_table_h);
            NBGLK_DUMP_STRING(capture_file, info_buf);
            NBGLK_GET_STRING(info_buf, 255, "@[register.write %u 0x00394 0x00000001]\n", core_id);
            NBGLK_DUMP_STRING(capture_file, info_buf);
        }
        }

        {
        #define STATES_SIZE   10
        vip_uint32_t *logical_t = VIP_NULL;
        vip_uint32_t handle_t = 0;
        vip_address_t physical = 0;
        vip_char_t buffer[256];
        vip_uint32_t size = STATES_SIZE * sizeof(vip_uint32_t);
        vip_uint32_t alloc_flag = VIPDRV_VIDEO_MEM_ALLOC_CONTIGUOUS |
                                  VIPDRV_VIDEO_MEM_ALLOC_4GB_ADDR |
                                  VIPDRV_VIDEO_MEM_ALLOC_NO_MMU_PAGE;
        vipOnError(viphal_allocate_videomemory((vip_uint8_t **)&logical_t, &handle_t, &physical,
                                                  size, vipdMEMORY_ALIGN_SIZE,
                                                  alloc_flag));
        logical_t[0] = 0x0801006b;
        logical_t[1] = 0xfffe0000;
        logical_t[2] = 0x08010e12;
        logical_t[3] = 0x00490000;
        logical_t[4] = 0x08010e02;
        logical_t[5] = 0x00000701;
        logical_t[6] = 0x48000000;
        logical_t[7] = 0x00000701;
        logical_t[8] = 0x10000000;
        logical_t[9] = 0x00000000;
        /* only for save cmds into capture file, don't need flush cache */
        nbglk_capture_buffer(capture_file, logical_t, physical, size, NBGLK_CAPTURE_BUF_COMMAND);
        for (i = 0; i < network->nbg_desc.net_info.core_count; i++) {
            core_id = network->device->core_id[i + network->core_index];
            NBGLK_GET_STRING(buffer, 255, "@[register.write %d 0x%05X 0x%08X]\n",
                core_id, 0x00654, physical);
            NBGLK_DUMP_STRING(capture_file, buffer);
            NBGLK_GET_STRING(buffer, 255, "@[register.write %d 0x%05X 0x%08X]\n",
                core_id, 0x003A4, 0x00010000 | ((size + 7) >> 3));
            NBGLK_DUMP_STRING(capture_file, buffer);
        }
        NBGLK_DUMP_STRING(capture_file, "@[stall]\n");
        for (i = 0; i < network->nbg_desc.net_info.core_count; i++) {
            core_id = network->device->core_id[i + network->core_index];
            NBGLK_GET_STRING(info_buf, 255, "@[register.write %u 0x00388 0x00000001]\n\n", core_id);
            NBGLK_DUMP_STRING(capture_file, info_buf);
        }
        viphal_free_videomemory(handle_t);
        }
    }

    return status;
onError:
    vip_error("fail to capture mmu info\n");
    return status;
}
#endif

vip_status_e nbglk_capture_header_info(
    viphal_file_t capture_file,
    vip_network network
    )
{
    vip_uint32_t chip_rev1 = 0;
    vip_uint32_t chip_rev2 = 0;
    vip_uint32_t chip_date = 0;
    vip_uint32_t chip_cid = 0;
    vip_char_t buffer[256];
    vip_uint32_t core_index = 0;
    vip_uint32_t device_index = 0;

    CHECK_CAPTURE_FUNC_CONFIG(network);

    if (VIP_NULL == capture_file) {
        return VIP_SUCCESS;
    }

    if (VIP_NULL == network) {
        device_index = core_index = 0;
    }
    else {
        core_index = network->core_index;
        device_index = network->device->dev_index;
        NBGLK_GET_STRING(buffer, 256, "#[network task_id=0x%x network name=%s]\n",
                network->task_id, network->nbg_desc.net_info.name);
        NBGLK_DUMP_STRING(capture_file, buffer);
    }

    viphal_read_reg(device_index, core_index, 0x00020, &chip_rev1);
    viphal_read_reg(device_index, core_index, 0x00024, &chip_rev2);
    viphal_read_reg(device_index, core_index, 0x00028, &chip_date);
    viphal_read_reg(device_index, core_index, 0x00030, &chip_cid);

    NBGLK_GET_STRING(buffer, 256, "#[VIPLite version %d.%d.%d.%s]\n",
                    VERSION_MAJOR, VERSION_MINOR, VERSION_SUB_MINOR, VERSION_PATCH);
    NBGLK_DUMP_STRING(capture_file, buffer);
    if (network != VIP_NULL) {
        NBGLK_GET_STRING(buffer, 256, "#[NBG version 0x%08X]\n", network->nbg_desc.fixed.header.version);
        NBGLK_DUMP_STRING(capture_file, buffer);
        NBGLK_GET_STRING(buffer, 256, "#[device index %d]\n", device_index);
        NBGLK_DUMP_STRING(capture_file, buffer);
        NBGLK_GET_STRING(buffer, 256, "#[start core index %d]\n", network->core_index);
        NBGLK_DUMP_STRING(capture_file, buffer);
        NBGLK_GET_STRING(buffer, 256, "#[core count %d]\n", network->nbg_desc.net_info.core_count);
        NBGLK_DUMP_STRING(capture_file, buffer);
    }

    NBGLK_GET_STRING(buffer, 256, "#[chip rev1 0x%08X]\n", chip_rev1);
    NBGLK_DUMP_STRING(capture_file, buffer);
    NBGLK_GET_STRING(buffer, 256, "#[chip rev2 0x%08X]\n", chip_rev2);
    NBGLK_DUMP_STRING(capture_file, buffer);
    NBGLK_GET_STRING(buffer, 256, "#[chip date 0x%08X]\n", chip_date);
    NBGLK_DUMP_STRING(capture_file, buffer);
    NBGLK_GET_STRING(buffer, 256, "#[chip cid 0x%08X]\n\n", chip_cid);
    NBGLK_DUMP_STRING(capture_file, buffer);

    return VIP_SUCCESS;
}

vip_status_e nbglk_capture_register_info(
    viphal_file_t capture_file,
    vip_network network
    )
{
#define GET_VALUE(data, name) \
    ((~0U >> (31 - (name##_END))) & data) >> (name##_START)
    vip_uint32_t count = 0, i = 0;
    vip_char_t buffer[256];
    vip_address_t physical;
    vip_uint32_t mem_id = 0;
    vip_uint8_t* temp_1 = VIP_NULL;
    vip_uint32_t* logical = VIP_NULL;
    vip_status_e status = VIP_SUCCESS;
    nbglk_device_t *device = network->device;

    if (VIP_NULL == capture_file) {
        return VIP_SUCCESS;
    }

    NBGLK_GET_STRING(buffer, 255, "#[AHB initialize]\n");
    NBGLK_DUMP_STRING(capture_file, buffer);

#if vpmdENABLE_MULTIPLE_TASK && vpmdENABLE_CAPTURE
    nbglk_lock_recursive_mutex(&network->context->capture_mutex);
#endif
    vip_info("start query ahb reigster\n");
    vipOnError(viphal_query_register_dump(&mem_id, &physical, &count));
    vipOnError(viphal_map_userlogical(mem_id, &temp_1));
    logical = (vip_uint32_t*)temp_1;
    while (i < count) {
        vip_uint32_t address = 0;
        vip_uint32_t core_id = 0;
        vipdrv_register_dump_type_e type = 0;
        vip_uint32_t expect = 0;
        vip_uint32_t data = 0;
        vip_uint32_t j = 0;
        vip_bool_e valid = vip_false_e;
        vip_uint32_t temp = logical[i++];
        address = GET_VALUE(temp, DUMP_REGISTER_ADDRESS);
        core_id = GET_VALUE(temp, DUMP_REGISTER_CORE_ID);
        type = GET_VALUE(temp, DUMP_REGISTER_TYPE);
        data = logical[i++];
        if (VIPDRV_REGISTER_DUMP_WAIT == type) {
            expect = logical[i++];
        }

        for (j = 0; j < device->core_count; j++) {
            if (core_id == device->core_id[j]) {
                valid = vip_true_e;
                break;
            }
        }

        if (vip_false_e == valid) {
            continue;
        }

        switch (type)
        {
        case VIPDRV_REGISTER_DUMP_READ:
            NBGLK_GET_STRING(buffer, 256, "@[register.read %d 0x%05X 0x%08X]\n", core_id, address, data);
            NBGLK_DUMP_STRING(capture_file, buffer);
            break;
        case VIPDRV_REGISTER_DUMP_WRITE:
            NBGLK_GET_STRING(buffer, 256, "@[register.write %d 0x%05X 0x%08X]\n", core_id, address, data);
            NBGLK_DUMP_STRING(capture_file, buffer);
            break;
        case VIPDRV_REGISTER_DUMP_WAIT:
            NBGLK_GET_STRING(buffer, 256, "@[register.wait %d 0x%05X 0x%08X 0x%08X]\n",
                core_id, address, expect, data);
            NBGLK_DUMP_STRING(capture_file, buffer);
            break;
        default:
            vip_error("unknown register dump type\n");
            break;
        }
    }
    viphal_unmap_userlogical(mem_id);
    NBGLK_GET_STRING(buffer, 256, "\n");
    NBGLK_DUMP_STRING(capture_file, buffer);
#if vpmdENABLE_MULTIPLE_TASK && vpmdENABLE_CAPTURE
    nbglk_unlock_recursive_mutex(&network->context->capture_mutex);
#endif

onError:
    return status;
#undef GET_VALUE
}

vip_status_e nbglk_capture_init_command(
    viphal_file_t capture_file,
    vip_network network
    )
{
#define STRING_SIZE  256
    vip_status_e status = VIP_SUCCESS;
    vip_uint32_t i = 0, j = 0;
    vip_char_t info[STRING_SIZE];
    nbg_desc_patch_t *pd_entry = VIP_NULL;
    nbg_desc_data_table_t *icdt = VIP_NULL;
    nbglk_capture_buf_type_e type = NBGLK_CAPTURE_BUF_MEMORY;
    vip_uint8_t cmd_dump = vip_false_e;
    vip_uint8_t input_dump = vip_false_e;
    vip_uint8_t output_dump = vip_false_e;
    vip_uint8_t ks_dump = vip_false_e;

    CHECK_CAPTURE_FUNC_CONFIG(network);

    if ((VIP_NULL == network) || (VIP_NULL == capture_file)) {
        return VIP_SUCCESS;
    }
    NBGLK_CHECK_NETWORK_DYNA_IS_NULL(network);

    {
        vipdrv_query_driver_status_t drv_status = {0};
        drv_status.device_index = network->device->dev_index;
        vipOnError(viphal_query_driver_status(&drv_status));
        if (drv_status.init_cmd_size > 0) {
            vip_uint32_t mem_id = 0;
            vip_uint8_t *logical = VIP_NULL;
            mem_id = drv_status.init_cmd_handle;
            vipOnError(viphal_map_userlogical(mem_id, &logical));
            NBGLK_DUMP_STRING(capture_file, "#[initial states kernel]\n");
            nbglk_capture_buffer(capture_file, logical, drv_status.init_cmd_physical,
                              drv_status.init_cmd_size, NBGLK_CAPTURE_BUF_COMMAND);
            nbglk_capture_trigger(network, capture_file, drv_status.init_cmd_physical, drv_status.init_cmd_size);
            viphal_unmap_userlogical(mem_id);
        }
        else {
            vip_error("not save init command into capture\n");
        }
    }

    if ((0 == network->nbg_desc.n_hw_init_ops) || (VIP_NULL == network->ICD.logical) ||
        (VIP_NULL == network->init_commands.logical)) {
        return VIP_SUCCESS;
    }

    for (j = 0; j < network->nbg_desc.n_hw_init_ops; j++) {
        nbg_desc_hw_init_operation_t *op = &network->nbg_desc.init_ops[j];

        cmd_dump = vip_false_e;
        input_dump = vip_false_e;
        output_dump = vip_false_e;
        ks_dump = vip_false_e;
        for (i = 0; i < op->patch_count; i++) {
            pd_entry = &network->nbg_desc.pd_sections[op->patch_index + i];
            icdt = &network->nbg_desc.ICDT[pd_entry->index];

            switch (pd_entry->source_type) {
            case NBG_SOURCE_COMMAND:
                if (cmd_dump) {
                    continue;
                }
                NBGLK_GET_STRING(info, STRING_SIZE, "#[initial_%d command]\n", j);
                type = NBGLK_CAPTURE_BUF_MEMORY;
                cmd_dump = vip_true_e;
                break;
            case NBG_SOURCE_MISC_DYNA_INPUT:
                if (input_dump) {
                    continue;
                }
                NBGLK_GET_STRING(info, STRING_SIZE, "#[initial_%d input]\n", j);
                type = NBGLK_CAPTURE_BUF_MEMORY;
                input_dump = vip_true_e;
                break;
            case NBG_SOURCE_MISC_DYNA_OUTPUT:
                if (output_dump) {
                    continue;
                }
                NBGLK_GET_STRING(info, STRING_SIZE, "#[initial_%d output]\n", j);
                type = NBGLK_CAPTURE_BUF_MEMORY;
                output_dump = vip_true_e;
                break;
            case NBG_SOURCE_MISC_DYNA_GENERIC:
                if (ks_dump) {
                    continue;
                }
                NBGLK_GET_STRING(info, STRING_SIZE, "#[initial_%d KS data]\n", j);
                type = NBGLK_CAPTURE_BUF_MEMORY;
                ks_dump = vip_true_e;
                break;
            default:
                continue;
            }

            NBGLK_DUMP_STRING(capture_file, info);
            nbglk_capture_buffer(capture_file,
                              network->ICD.logical + icdt->offset,
                              network->ICD.physical + icdt->offset,
                              SIZE_CAST(vip_uint32_t, icdt->size),
                              type);
        }
    }

    NBGLK_GET_STRING(info, STRING_SIZE, "#[initial command]\n");
    NBGLK_DUMP_STRING(capture_file, info);
    nbglk_capture_buffer(capture_file,
                      network->init_commands.logical,
                      network->init_commands.physical,
                      network->init_commands.size,
                      NBGLK_CAPTURE_BUF_COMMAND);
    nbglk_capture_trigger(network, capture_file,
        SIZE_CAST(vip_uint32_t, network->init_commands.physical),
        SIZE_CAST(vip_uint32_t, network->init_commands.size));

    return status;
onError:
    vip_error("fail to capture init command\n");
    return status;
#undef STRING_SIZE
}

#if vpmdPRELOAD_COEFF > 1
vip_status_e nbglk_capture_npu_preload_coeff(
    viphal_file_t capture_file,
    vip_network network
    )
{
#define STRING_SIZE  256
    vip_status_e status = VIP_SUCCESS;
    vip_char_t info[STRING_SIZE];

    if ((0 == network->preload_instr.size) || (0 == network->preload_cmd_size)) {
        return VIP_SUCCESS;
    }

    NBGLK_GET_STRING(info, STRING_SIZE, "#[preload instr]\n");
    NBGLK_DUMP_STRING(capture_file, info);
    nbglk_capture_buffer(capture_file, network->preload_instr.logical, network->preload_instr.physical,
                         network->preload_instr.size, NBGLK_CAPTURE_BUF_MEMORY);

    NBGLK_GET_STRING(info, STRING_SIZE, "#[preload states]\n");
    NBGLK_DUMP_STRING(capture_file, info);
    nbglk_capture_buffer(capture_file, network->preload_cmd.logical, network->preload_cmd.physical,
                         network->preload_cmd_size, NBGLK_CAPTURE_BUF_COMMAND);
    nbglk_capture_trigger(network, capture_file,
        SIZE_CAST(vip_uint32_t, network->preload_cmd.physical), network->preload_cmd_size);

    return status;
#undef STRING_SIZE
}
#endif

vip_status_e nbglk_capture_init(
    vip_network network,
    vip_group group,
    nbglk_capture_type_e type
    )
{
    vip_status_e status = VIP_SUCCESS;
    vip_char_t file_name[256];
    viphal_file_t file= VIP_NULL;
    vip_uint32_t pid = viphal_os_get_pid();
    vip_uint32_t tid = viphal_os_get_tid();
    vip_uint32_t time = (vip_uint32_t)(viphal_os_get_time() / 1000);/* 1ms unit */

    CHECK_CAPTURE_FUNC_CONFIG(network);

#if vpmdENABLE_CAPTURE
    viphal_os_snprint(file_name, 255, "%s/viplite_hal_capture_%x_%x_%x.log", vpmdSAVE_FILE_PATH, pid, tid, time);
#else
    viphal_os_snprint(file_name, 255, "%s/viplite_hang_capture_%x_%x_%x.log", vpmdSAVE_FILE_PATH, pid, tid, time);
#endif
    vip_error("catpure file name %s\n", file_name);
    file = viphal_os_open_file(file_name, FILE_APPENDTEXT);
    if (file == VIP_NULL) {
        vip_print("fail to open %s file for capture\n", file_name);
        return VIP_ERROR_IO;
    }

    if ((group != VIP_NULL) && (NBGLK_CAPTURE_TYPE_GROUP == type)) {
        group->capture_file = file;
        vip_debug("group capture_file=%p\n", file);
    }
#if vpmdENABLE_CAPTURE
    else if (NBGLK_CAPTURE_TYPE_CAP == type) {
        nbglk_context_t* context = nbglk_get_context();
        context->capture_file = file;
        vip_debug("context capture_file=%p\n", file);
    }
#endif
#if vpmdENABLE_HANG_DUMP > 1
    else if ((network != VIP_NULL) && (NBGLK_CAPTURE_TYPE_HANG == type)) {
        network->capture_file = file;
        vip_debug("network capture_file=%p\n", file);
    }
#endif
    else {
        vip_error("fail to capture init\n");
        status = VIP_ERROR_NOT_SUPPORTED;
    }

    return status;
}

vip_status_e nbglk_capture_uninit(
    vip_network network,
    vip_group group
    )
{
    vip_status_e status = VIP_SUCCESS;

    CHECK_CAPTURE_FUNC_CONFIG(network);

    if ((group != VIP_NULL) && (group->capture_file != VIP_NULL)) {
        viphal_os_close_file(group->capture_file);
        group->capture_file = VIP_NULL;
    }
#if vpmdENABLE_CAPTURE
    {
    nbglk_context_t* context = nbglk_get_context();
    if (context->capture_file != VIP_NULL) {
        viphal_os_close_file(context->capture_file);
        context->capture_file = VIP_NULL;
    }
    }
#else
    if ((network != VIP_NULL) && (network->capture_file != VIP_NULL)) {
        viphal_os_close_file(network->capture_file);
        network->capture_file = VIP_NULL;
    }
#endif
    return status;
}
#endif

#if vpmdENABLE_GROUP_MODE && ((vpmdENABLE_HANG_DUMP > 1) || (vpmdENABLE_CAPTURE > 1))
static vip_uint32_t nbglk_get_group_cmds_size(
    vip_group group
    )
{
    vip_uint32_t i = 0, j = 0;
    vip_uint32_t cmds_size = 0;
    nbglk_segment_t *segment = VIP_NULL;

    for (i = 0; i < group->run_task_num; i++) {
        if (NBGLK_OBJ_NETWORK == group->tasks[i].object.type) {
            vip_network network = (vip_network)group->tasks[i].task;
            if (1 == network->capture_done) {
                return 0;
            }
            if (network->prepared_network) {
                for (j = 0; j < network->segment_num; j++) {
                    segment = &network->segments[j];
                    if (vip_false_e == segment->is_sw_segm) {
                        if (i != (group->run_task_num - 1)) {
                            cmds_size += SIZE_CAST(vip_uint32_t, segment->states_buffer.size) + LINK_SIZE;
                        }
                        else {
                            cmds_size += SIZE_CAST(vip_uint32_t, segment->states_buffer.size) + APPEND_COMMAND_SIZE;
                        }
                    }
                }
            }
        }
    #if vpmdENABLE_NODE
        else if (NBGLK_OBJ_NODE == group->tasks[i].object.type) {
            vip_node node = (vip_node)group->tasks[i].task;
            if (1 == node->capture_done) {
                return 0;
            }
            else {
                if (i != (group->run_task_num - 1)) {
                    cmds_size += node->states_buffer.size + LINK_SIZE;
                }
                else {
                    cmds_size += node->states_buffer.size + APPEND_COMMAND_SIZE;
                }
            }
        }
    #endif
    }

    return cmds_size;
}

/* pre-group a hang capture file */
vip_status_e nbglk_capture_group(
    vip_group group,
    vip_bool_e is_hang
    )
{
    vip_uint32_t i = 0, j = 0;
    vip_uint32_t cmds_size = 0;
    vip_status_e status = VIP_SUCCESS;
    nbglk_segment_t *segment = VIP_NULL;
    nbglk_context_t *context = VIP_NULL;
    vip_uint8_t *logical = VIP_NULL;
    vip_uint8_t *base_logical = VIP_NULL;
    vip_uint32_t mem_id = 0;
    vip_address_t physical = 0;
    vip_uint32_t dump_init = 0;
    vip_network g_network = VIP_NULL;
    viphal_file_t capture_file = VIP_NULL;

    if (VIP_NULL == group) {
        vip_error("fail capture group, group is NULL\n");
        return VIP_ERROR_FAILURE;
    }
    context = group->context;

    if ((is_hang == vip_false_e) && (context->func_config.enable_capture == vip_false_e)) {
        vip_info("capture function is disabled by kernel space fs node.\n")
        return VIP_SUCCESS;
    }

    cmds_size = nbglk_get_group_cmds_size(group);
    if (0 == cmds_size) {
        return VIP_ERROR_FAILURE;
    }

    if (VIP_SUCCESS != nbglk_capture_init(VIP_NULL, group, NBGLK_CAPTURE_TYPE_GROUP)) {
        vip_error("catpure init error, not capture group.\n");
        return VIP_ERROR_FAILURE;
    }
    capture_file = group->capture_file;

    status = viphal_allocate_videomemory(&logical, &mem_id, &physical,
                                             cmds_size, vipdMEMORY_ALIGN_SIZE,
                                             VIPDRV_VIDEO_MEM_ALLOC_4GB_ADDR);
    if (status != VIP_SUCCESS) {
        vip_error("fail capture group in allocal memory\n");
        return VIP_ERROR_FAILURE;
    }
    base_logical = logical;
    /* only for save cmds into capture file, don't need flush cache */

    for (i = 0; i < group->run_task_num; i++) {
        if (NBGLK_OBJ_NETWORK == group->tasks[i].object.type) {
            g_network = (vip_network)group->tasks[i].task;
            break;
        }
    }

    if (g_network != VIP_NULL) {
        nbglk_capture_header_info(capture_file, g_network);
        nbglk_capture_register_info(capture_file, g_network);
        #if vpmdENABLE_CAPTURE_MMU
        nbglk_capture_mmu_info(capture_file, g_network);
        #endif
    }

    for (i = 0; i < group->run_task_num; i++) {
        vip_uint32_t tmp_size = 0;
        if (NBGLK_OBJ_NETWORK == group->tasks[i].object.type) {
            vip_network network = (vip_network)group->tasks[i].task;
            vip_char_t buffer[256];
            if (0 == dump_init) {
                nbglk_capture_init_command(capture_file, network);
            }
            dump_init = 1;

            NBGLK_GET_STRING(buffer, 256, "#[group task_%d network name=%s dump:]\n",
                             i, network->nbg_desc.net_info.name);
            NBGLK_DUMP_STRING(capture_file, buffer);
            nbglk_capture_network_input(group->capture_file, network);
            if (network->prepared_network) {
                nbglk_capture_record_list(capture_file, network);
                for (j = 0; j < network->segment_num; j++) {
                    segment = &network->segments[j];
                    if (vip_false_e == segment->is_sw_segm) {
                        if (i != (group->run_task_num - 1)) {
                            tmp_size = SIZE_CAST(vip_uint32_t, segment->states_buffer.size) + LINK_SIZE;
                        }
                        else {
                            tmp_size = SIZE_CAST(vip_uint32_t, segment->states_buffer.size) + APPEND_COMMAND_SIZE;
                        }
                        viphal_copy_memory(logical, segment->states_buffer.logical, tmp_size);
                        logical += tmp_size;
                    }
                }
            }
            else {
                vip_error("task %d not prepare network\n", i);
            }

            nbglk_capture_memory_layout(capture_file, network);
            #if vpmdENABLE_CHECKSUM
            vip_error("index=%d, network=%p crc check:\n", i, network);
            nbglk_capture_network_check(capture_file, network);
            #endif
            nbglk_capture_section_check(capture_file, network);
        }
        #if vpmdENABLE_NODE
        else if (NBGLK_OBJ_NODE == group->tasks[i].object.type) {
            vip_node node = (vip_node)group->tasks[i].task;
            vip_char_t buffer[256];
            NBGLK_GET_STRING(buffer, 256, "#[group task_%d node dump:]\n", i);
            NBGLK_DUMP_STRING(capture_file, buffer);
            nbglk_capture_node_inputs(node, group->capture_file);
            viphal_copy_memory(logical, node->states_buffer.logical, node->states_buffer.size);
            logical += node->states_buffer.size;
        }
        #endif
    }

    /* capture commands in this group */
    NBGLK_DUMP_STRING(capture_file, "#[states]\n");
    nbglk_capture_buffer(capture_file, base_logical, physical, cmds_size, NBGLK_CAPTURE_BUF_COMMAND);
    if (g_network != VIP_NULL) {
        vip_uint32_t trig_size = cmds_size;
        if (cmds_size > (vipdMAX_CMD_SIZE - 7)) {
            trig_size =vipdMAX_CMD_SIZE - 7;
        }
        nbglk_capture_trigger(g_network, capture_file, SIZE_CAST(vip_uint32_t, physical), trig_size);
    }

    /* capture outputs */
    for (i = 0; i < group->run_task_num; i++) {
        if (NBGLK_OBJ_NETWORK == group->tasks[i].object.type) {
            vip_network network = (vip_network)group->tasks[i].task;
            vip_char_t buffer[256];
            NBGLK_GET_STRING(buffer, 256, "#[group task_%d network output:]\n", i);
            NBGLK_DUMP_STRING(capture_file, buffer);
            nbglk_capture_network_output(capture_file, network);
            network->capture_done = 1;
        }
    #if vpmdENABLE_NODE
        else if (NBGLK_OBJ_NODE == group->tasks[i].object.type) {
            vip_node node = (vip_node)group->tasks[i].task;
            vip_char_t buffer[256];
            NBGLK_GET_STRING(buffer, 256, "#[group task_%d node output:]\n", i);
            NBGLK_DUMP_STRING(capture_file, buffer);
            nbglk_capture_node_outputs(node, group->capture_file);
            node->capture_done = 1;
        }
    #endif
    }

    nbglk_capture_uninit(VIP_NULL, group);

    if (mem_id != 0) {
        viphal_free_videomemory(mem_id);
    }

    return VIP_SUCCESS;
}
#endif

#if (vpmdENABLE_HANG_DUMP > 1) || (vpmdENABLE_CAPTURE > 1)
vip_status_e nbglk_capture_network_output(
    viphal_file_t capture_file,
    vip_network_t *network
    )
{
    vip_uint32_t i = 0;
    vip_size_t size = 0;
    vip_char_t buffer[256];

    CHECK_CAPTURE_FUNC_CONFIG(network);

    if ((VIP_NULL == network) || (VIP_NULL == capture_file)) {
        return VIP_SUCCESS;
    }

    if (1 == network->capture_done) {
        return VIP_SUCCESS;
    }
#if vpmdENABLE_MULTIPLE_TASK && vpmdENABLE_CAPTURE
    nbglk_lock_recursive_mutex(&network->context->capture_mutex);
#endif
    for (i = 0; i < network->nbg_desc.n_outputs; i++) {
        vip_info("dump output_%d\n", i);
        NBGLK_GET_STRING(buffer, 255, "#[output %d dump]\n", i);
        NBGLK_DUMP_STRING(capture_file, buffer);
        if (network->network_outputs[i].logical != VIP_NULL) {
            size = nbg_get_output_size(&network->nbg_desc, i);
            nbglk_capture_buffer(capture_file, network->network_outputs[i].logical,
                              network->network_outputs[i].physical, size, NBGLK_CAPTURE_BUF_VERIFY);
        }
        /* only for keep format */
        else {
            vip_uint32_t *data;
            size = nbg_get_output_size(&network->nbg_desc, i);
            viphal_allocate_memory(size, (void**)&data);
            viphal_zero_memory(data, size);
            nbglk_capture_buffer(capture_file, data, network->network_outputs[i].physical,
                                size, NBGLK_CAPTURE_BUF_VERIFY);
            viphal_free_memory(data);
        }
    }
    network->capture_done = 1;
#if vpmdENABLE_MULTIPLE_TASK && vpmdENABLE_CAPTURE
    nbglk_unlock_recursive_mutex(&network->context->capture_mutex);
#endif
    return VIP_SUCCESS;
}

vip_status_e nbglk_capture_network_input(
    viphal_file_t capture_file,
    vip_network network
    )
{
    vip_uint32_t i = 0;

    CHECK_CAPTURE_FUNC_CONFIG(network);

    if (VIP_NULL == capture_file) {
        return VIP_SUCCESS;
    }

    if (1 == network->capture_done) {
        return VIP_SUCCESS;
    }

    for (i = 0; i < network->nbg_desc.n_inputs; i++) {
        vip_size_t size = 0;
        vip_char_t buffer[256];
        nbglk_io_patch_info_t *input = &network->network_inputs[i];
        vip_info("dump input_%d\n", i);
        NBGLK_GET_STRING(buffer, 255, "#[input %d dump]\n", i);
        NBGLK_DUMP_STRING(capture_file, buffer);
        if (input->logical != VIP_NULL) {
            size = nbg_get_input_size(&network->nbg_desc, i);
            nbglk_capture_buffer(capture_file, input->logical, input->physical, size, NBGLK_CAPTURE_BUF_MEMORY);
        }
        /* only for keep format */
        else {
            vip_uint32_t* data = VIP_NULL;
            size = nbg_get_input_size(&network->nbg_desc, i);
            viphal_allocate_memory(size, (void**)&data);
            viphal_zero_memory(data, size);
            nbglk_capture_buffer(capture_file, data, input->physical, size, NBGLK_CAPTURE_BUF_MEMORY);
            viphal_free_memory(data);
        }
    }
    return VIP_SUCCESS;
}
#endif

#if vpmdENABLE_CAPTURE
vip_status_e nbglk_capture_network(
    vip_network network
    )
{
    vip_status_e status = VIP_SUCCESS;
    nbglk_context_t *context = network->context;
    viphal_file_t capture_file = context->capture_file;

    CHECK_CAPTURE_FUNC_CONFIG(network);

    if (VIP_NULL == capture_file) {
        return VIP_SUCCESS;
    }

    if (1 == network->capture_done) {
        return VIP_SUCCESS;
    }
#if vpmdENABLE_MULTIPLE_TASK
    nbglk_lock_recursive_mutex(&context->capture_mutex);
#endif
    nbglk_capture_header_info(capture_file, network);
    nbglk_capture_register_info(capture_file, network);
#if vpmdENABLE_CAPTURE_MMU
    nbglk_capture_mmu_info(capture_file, network);
#endif
    nbglk_capture_init_command(capture_file, network);
#if vpmdPRELOAD_COEFF > 1
    nbglk_capture_npu_preload_coeff(capture_file, network);
#endif
#if (vpmdENABLE_CAPTURE > 1)
    if (network->input_dirty != 0) {
        nbglk_capture_network_input(capture_file, network);
    }
#endif

    nbglk_capture_record_list(capture_file, network);

    network->input_dirty = 0;
#if vpmdENABLE_MULTIPLE_TASK
    nbglk_unlock_recursive_mutex(&context->capture_mutex);
#endif
    return status;
}
#endif
