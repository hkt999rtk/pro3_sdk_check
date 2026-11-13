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

#include <nbg_lk_context.h>
#include <nbg_lk_network.h>
#include <nbg_lk_group.h>
#include <nbg_lk_debug.h>
#include <nbg_lk_coeff.h>
#include <nbg_lk.h>
#include <vip_hal.h>
#include <vip_hal_os_port.h>
#include <nbg_fmt_util.h>
#include <nbg_lk_relocate.h>
#if vpmdENABLE_NODE
#include <nbg_lk_node.h>
#endif

#if vpmdTASK_CMD_DUMP
vip_status_e nbglk_network_dump_init(
    vip_network network
    )
{
    vip_status_e status = VIP_SUCCESS;
    vip_char_t name[256];
    viphal_file_t file= VIP_NULL;

    viphal_os_snprint(name, 255, "%s/viplite_network_dump_0x%x.bin",
                vpmdSAVE_FILE_PATH, network->nbg_desc.fixed.header.hw_target);
    vip_print("network dump file name %s\n", name);
    file = viphal_os_open_file(name, FILE_CREATE);
    if (file == VIP_NULL) {
        vip_print("fail to open %s file for cmd dump\n", name);
        return VIP_ERROR_IO;
    }
    network->dump_file = file;

    return status;
}

vip_status_e nbglk_network_dump_uninit(
    vip_network network
    )
{
    if (network->dump_file != VIP_NULL) {
        viphal_os_close_file(network->dump_file);
        network->dump_file  = VIP_NULL;
    }
    return VIP_SUCCESS;
}

vip_status_e nbglk_network_dump_data(
    vip_network network,
    vip_uint8_t* data,
    vip_uint32_t size
    )
{
    vip_status_e status = VIP_SUCCESS;
    if ((network->dump_file != VIP_NULL) && (size > 0) && (data != VIP_NULL)) {
        status = viphal_os_write_file((void*)data, size, network->dump_file);
    }
    else {
        vip_error("fail network cmd dump size=%d\n", size);
        status = VIP_ERROR_FAILURE;
    }

    return status;
}

static vip_bool_e network_dump_is_address(
    vip_uint32_t cmd
    )
{
    vip_bool_e is_address = vip_false_e;

    switch (cmd) {
    case 0x08010E4E:
    case 0x08010E4F:
    case 0x08010E50:
    case 0x08010428:
    case 0x0801042E:
    case 0x0801040A:
    case 0x0804D800:
    case 0x0804D804:
       is_address = vip_true_e;
       break;
    default:
        break;
    }
    return is_address;
}

vip_status_e nbglk_network_dump_cmd(
    vip_network network,
    vip_uint8_t* command,
    vip_uint32_t size
    )
{
    vip_status_e status = VIP_SUCCESS;
    vip_uint32_t i = 0;
    vip_uint32_t* tmp = VIP_NULL;

    vip_info("cmd dump load states\n");

    if ((0 == size) || (VIP_NULL == command)) {
        vip_error("fail dump cmd param is null\n");
        return VIP_ERROR_FAILURE;
    }

    vipOnError(viphal_allocate_memory(size, (void **)&tmp));
    viphal_copy_memory((void*)tmp, (void*)command, size);

    for (i = 0; i < size / sizeof(vip_uint32_t); i++) {
        if (network_dump_is_address(tmp[i])) {
            tmp[i+1] = 0x00;
        }
    }
    vipOnError(nbglk_network_dump_data(network, (void*)tmp, size));
    viphal_free_memory(tmp);

onError:
    return status;
}
#endif

#if vpmdDUMP_NBG_RESOURCE
static vip_status_e nbglk_save_raw_file(
    vip_char_t *raw_name,
    void *data,
    vip_size_t size
    )
{
    void *fp = VIP_NULL;
    vip_char_t name[256];

    if ((VIP_NULL == data) || (0 == size)) {
        vip_error("fail to dump file, param is NULL\n");
        return VIP_ERROR_FAILURE;
    }

    viphal_os_snprint(name, 255, "%s/%s", vpmdSAVE_FILE_PATH, raw_name);
    fp = viphal_os_open_file(name, FILE_CREATE);
    if (fp != VIP_NULL) {
        viphal_os_write_file(data, size, fp);
        viphal_os_close_file(fp);
    }
    else {
        vip_error("fail to dump data, can't open file %s\n", name);
        return VIP_ERROR_FAILURE;
    }

    return VIP_SUCCESS;
}
#endif

#if vpmdDUMP_NBG_RESOURCE
vip_status_e nbglk_dump_network_outputs(
    vip_network_t *network
    )
{
    vip_uint32_t i = 0;

    CHECK_DUMP_NBG(network->context);

    for (i = 0; i < network->nbg_desc.n_outputs; i++) {
        vip_char_t name[256];
        vip_size_t size = 0;
        void *fp = VIP_NULL;

        size = nbg_get_output_size(&network->nbg_desc, i);
        viphal_os_snprint(name, 255, "%s/viplite_output_%d.dat", vpmdSAVE_FILE_PATH, i);
        fp = viphal_os_open_file(name, FILE_CREATE);
        if (fp != VIP_NULL) {
            viphal_os_write_file(network->network_outputs[i].logical, size, fp);
            viphal_os_close_file(fp);
        }
        else {
            vip_error("fail to dump output, can't open file %s\n", name);
        }
    }

    return VIP_SUCCESS;
}

vip_status_e nbglk_dump_network_inputs(
    vip_network_t *network
    )
{
    vip_char_t name[256];
    void *fp = VIP_NULL;
    vip_size_t size = 0;
    vip_uint32_t i = 0;

    CHECK_DUMP_NBG(network->context);

    for (i = 0; i < network->nbg_desc.n_inputs; i++) {
        nbglk_io_patch_info_t *input = &network->network_inputs[i];

        viphal_os_snprint(name, 255, "%s/viplite_input_%d.dat", vpmdSAVE_FILE_PATH, i);
        size = nbg_get_input_size(&network->nbg_desc, i);
        fp = viphal_os_open_file(name, FILE_CREATE);
        if (fp != VIP_NULL) {
            viphal_os_write_file(input->logical, size, fp);
            viphal_os_close_file(fp);
        }
        else {
            vip_error("fail to dump input, can't open file %s\n", name);
        }
    }

    return VIP_SUCCESS;
}

vip_status_e nbglk_dump_nbg(
    viphal_file_t file,
    void *data,
    vip_size_t size
    )
{
    vip_char_t name[256];
    nbglk_context_t *context = nbglk_get_context();

    CHECK_DUMP_NBG(context);

    if ((data != VIP_NULL) && (size != 0)) {
        viphal_os_snprint(name, 255, "viplite_dump_network_binary_%x_%x.nb", viphal_os_get_pid(), viphal_os_get_tid());
        nbglk_save_raw_file(name, data, size);
    }

    if (file != VIP_NULL) {
        /* read file and write file TDO*/
        return VIP_SUCCESS;
    }

    return VIP_SUCCESS;
}
#endif

#if vpmdENABLE_RECORD_INFO
void nbglk_record_info_add_list(
    nbglk_segment_t *segment,
    vip_size_t offset,
    vip_size_t size,
    vip_uint32_t layer_id,
    vip_uint32_t abs_op_id,
    vip_uint32_t op_index,
    vip_uint32_t type
    )
{
    vip_uint32_t index = 0;
    vip_uint64_t handle = type | (offset << 4);

    if (vip_true_e == nbglk_hashmap_full(&segment->record_list)) {
        if (VIP_SUCCESS != nbglk_hashmap_expand(&segment->record_list)) {
            return;
        }
    }

    /* check this data is added in capture list or not */
    if (VIP_SUCCESS == nbglk_hashmap_insert(&segment->record_list, handle, &index)) {
        nbglk_record_info_t *record_info = VIP_NULL;
        nbglk_hashmap_get_by_index(&segment->record_list, index, (void**)&record_info);
        record_info->offset = offset;
        record_info->size = size;
        record_info->record_type = type;
        record_info->layer_id = layer_id;
        record_info->abs_op_id = abs_op_id;
        record_info->op_index = op_index;
    }
}
#endif

#if vpmdENABLE_BW_PROFILING
/*
@param index: the index of core used by the network.
@param profile: the profile data of the network.
*/
static vip_status_e hardware_profiling_AHB(
    vip_network_t *network,
    vipdrv_query_profiling_t *profile,
    vip_uint32_t index
    )
{
    vip_status_e status = VIP_SUCCESS;
    vip_uint64_t total_reads_8B = 0;
    vip_uint64_t total_writes_8B = 0;
    vip_uint64_t total_reads_ocb_16B = 0;
    vip_uint64_t total_writes_ocb_16B = 0;
    vip_uint32_t total_cycle = 0;
    vip_uint32_t total_idle_cycle = 0;
    vip_uint64_t read_bw = 0;
    vip_uint64_t write_bw = 0;
    vip_uint64_t ocb_read_bw = 0;
    vip_uint64_t ocb_write_bw = 0;
    vip_uint64_t ddr_read_bw = 0;
    vip_uint64_t ddr_write_bw = 0;
    vip_uint64_t factor_value = 1; /* 1:64bits, 2:128bits, 4:256bits */
    vip_uint32_t axi_bus_width = viphal_query_hw_feature(VIP_HW_FEATURE_AXI_BUS_WIDTH);

    total_cycle = profile->total_cycle[index];
    total_idle_cycle = profile->total_idle_cycle[index];

    total_reads_8B = profile->total_read[index];
    total_writes_8B = profile->total_write[index];
    if (viphal_query_hw_feature(VIP_HW_FEATURE_OCB_COUNTER)) {
        total_reads_ocb_16B = profile->total_read_ocb[index];
        total_writes_ocb_16B = profile->total_write_ocb[index];
    }

    if (axi_bus_width > 0) {
        factor_value = (axi_bus_width * 8) / 64;
    }
    else {
        vip_uint32_t chip_info = 0;
        viphal_read_reg(0, 0, 0x000A4, &chip_info);
        if ((chip_info & (1 << 21)) == (1 << 21)) {
            factor_value = 2;
        }
    }

    total_reads_8B *= factor_value;
    total_writes_8B *= factor_value;
    read_bw = total_reads_8B * 8;
    write_bw = total_writes_8B * 8;
    ocb_read_bw = total_reads_ocb_16B * 16;
    ocb_write_bw = total_writes_ocb_16B * 16;
    ddr_read_bw = read_bw - ocb_read_bw;
    ddr_write_bw = write_bw - ocb_write_bw;

    vip_print("@dev%d-core%d{\n", network->device->dev_index, network->core_index + index);
    vip_print("Total read bandwidth (Byte): %ld\n", read_bw);
    vip_print("Total write bandwidth (Byte): %ld\n", write_bw);
    vip_print("AXI read bandwidth (Byte): %ld\n", ocb_read_bw);
    vip_print("AXI write bandwidth (Byte): %ld\n", ocb_write_bw);
    vip_print("DDR read bandwidth (Byte): %ld\n", ddr_read_bw);
    vip_print("DDR write bandwidth (Byte): %ld\n", ddr_write_bw);
    vip_print("VIP total cycles: %u\n", total_cycle);
    vip_print("VIP total idle cycles: %u\n", total_idle_cycle);
    vip_print("}@\n");

    return status;
}

vip_status_e nbglk_start_profiling(
    vip_network_t *network,
    nbglk_segment_t *segment
    )
{
    vip_status_e status = VIP_SUCCESS;
    return status;
}

vip_status_e nbglk_end_profiling(
    vip_network_t *network,
    nbglk_segment_t *segment
    )
{
    vip_status_e status = VIP_SUCCESS;
    vip_uint32_t i = 0;
    nbg_desc_network_info_t *net_info = &network->nbg_desc.net_info;
    vipdrv_query_profiling_t profile;

    profile.task_id = network->task_id;
    profile.subtask_index = segment->index;
    status = viphal_query_profiling(&profile);
    if (status != VIP_SUCCESS) {
        vip_error("fail to query profile data\n");
        return status;
    }

    for (i = 0; i < net_info->core_count; i++) {
        #if vpmdENABLE_CNN_PROFILING
        vip_uint32_t id = network->device->core_id[network->core_index + i];
        if ((net_info->core_count > 1) && !(segment->core_id_mask & (1 << id))) {
            continue;
        }
        #endif
        hardware_profiling_AHB(network, &profile, i);
    }

    return status;
}
#endif

#if vpmdENABLE_CNN_PROFILING
vip_uint32_t nbglk_get_num_initend_operation(
    vip_network_t *network
    )
{
    vip_uint32_t num = 0;
    vip_uint32_t i = 0;
    nbg_desc_operation_t *operation = VIP_NULL;

    for (i = 0; i < network->nbg_desc.n_operations; i++) {
        operation = &network->nbg_desc.operations[i];
        if ((operation->type == NBG_OPERATION_TYPE_INIT) ||
            (operation->type == NBG_OPERATION_TYPE_END)) {
            num++;
        }
    }

    return num;
}

vip_status_e nbglk_layer_info(
    vip_network_t *network,
    vip_char_t *name,
    vip_uint32_t layer_id,
    vip_uint32_t op_id,
    vip_uint32_t uid,
    vip_uint8_t core_id_mask
    )
{
    vip_status_e status = VIP_SUCCESS;
    vip_uint32_t i = 0;
    nbg_desc_operation_t *operation = VIP_NULL;
    vip_char_t operaton_target[5];
    vip_uint32_t node_id = network->nbg_desc.orig_layers[layer_id].node_id;
    vip_uint32_t core_id = 0;

    for (i = 0; i < network->nbg_desc.net_info.core_count; i++) {
        vip_uint32_t core = network->device->core_id[network->core_index + i];
        if (core_id_mask & (1 << core)) {
            core_id = core;
        }
    }

    for (i = 0; i < network->nbg_desc.n_operations; i++) {
        operation = &network->nbg_desc.operations[i];
        if (i == op_id) {
            switch (operation->type) {
            case NBG_OPERATION_TYPE_NN:
            {
                NBGLK_GET_STRING(operaton_target, 5, "NN");
            }
            break;
            case NBG_OPERATION_TYPE_TP:
            {
                NBGLK_GET_STRING(operaton_target, 5, "TP");
            }
            break;
            case NBG_OPERATION_TYPE_SH:
            {
                NBGLK_GET_STRING(operaton_target, 5, "SH");
            }
            break;
            case NBG_OPERATION_TYPE_SW:
            {
                NBGLK_GET_STRING(operaton_target, 5, "SW");
            }
            break;
            case NBG_OPERATION_TYPE_SP:
            {
                NBGLK_GET_STRING(operaton_target, 5, "SP");
            }
            break;
            case NBG_OPERATION_TYPE_NN_LUT:
            {
                NBGLK_GET_STRING(operaton_target, 5, "NN_LUT");
            }
            break;
            case NBG_OPERATION_TYPE_NT:
            {
                NBGLK_GET_STRING(operaton_target, 5, "NT");
            }
            break;

            default:
                break;
            }
        }
    }

    if (network->nbg_desc.fixed.header.version >= 0x00010017) {
        vip_print("op_index:%d, nodeId:%d, core_id:%d, uid:%d, abs_op_id:%d, layer name:%s, operation target:%s\n",
           op_id, node_id, core_id, uid, operation->abs_op_id, name, operaton_target);
    }
    else {
        vip_print("op_index:%d, nodeId:%d, core_id:%d, uid:%d, layer name:%s, operation target:%s\n",
           op_id, node_id, core_id, uid, name, operaton_target);
    }

    return status;
}

vip_status_e nbglk_start_segment_profile(
    vip_network_t *network,
    nbglk_segment_t *segment
    )
{
    vip_status_e status = VIP_SUCCESS;

    segment->start_time = viphal_os_get_time();

    return status;
}

vip_status_e nbglk_end_segment_profile(
    vip_network_t *network,
    nbglk_segment_t *segment
    )
{
    vip_status_e status = VIP_SUCCESS;
    vip_uint64_t diff_time = 0;

#if vpmdENABLE_CNN_PROFILING > 1
    nbglk_layer_info(network,
                     segment->name,
                     segment->segm_start_id, /* layer id */
                     segment->segm_end_id,   /* op index */
                     segment->uid,
                     segment->core_id_mask
                     );
#endif

    segment->end_time = viphal_os_get_time();

    if (segment->end_time > segment->start_time) {
        diff_time = segment->end_time - segment->start_time;
    }
    else {
        vip_uint64_t tmp = ~0;
        diff_time = tmp - segment->start_time + segment->end_time;
    }

    network->total_time += diff_time;

#if vpmdENABLE_CNN_PROFILING > 1
    vip_print("execution time: %d us\n", diff_time);
#endif

    return status;
}
#endif

vip_status_e nbglk_dump_nbg_entry(
     vip_network network
    )
{
    vip_print("-------Feature DB Start------\n");
    vip_print("nbg version=0x%08x\n", network->nbg_desc.fixed.header.version);
    vip_print("cid=0x%08x\n", network->nbg_desc.fixed.header.hw_target);
    vip_print("network name=%s\n", network->nbg_desc.net_info.name);
    vip_print("core count=%d\n", network->nbg_desc.net_info.core_count);
    vip_print("nn command size=%s\n",
                network->nbg_desc.net_info.attribute.nn_command_size ? "192" : "128");
    if (viphal_query_hw_feature(VIP_HW_FEATURE_TP_CORE_CNT) > 0) {
        vip_print("tp command size=%s\n",
                    network->nbg_desc.net_info.attribute.tp_command_size ? "192" : "128");
    }
    vip_print("change ppu param=%d\n", network->nbg_desc.net_info.attribute.change_ppu_param);
    vip_print("check sum value=0x%08x\n", network->nbg_desc.fixed.header.nbg_crc_sum);
    vip_print("-------Feature DB End--------\n");

    return VIP_SUCCESS;
}

static void crc32_table(
    vip_uint32_t* buffer
    )
{
    vip_uint32_t value;
    vip_uint32_t i, j;
    for (i = 0; i < 256; i++) {
        value = i;
        for (j = 0; j < 8; j++) {
            if (value & 1) {
                value = (value >> 1) ^ 0xedb88320;
            }
            else {
                value = (value >> 1);
            }
        }
        buffer[i] = value;
    }
}

/*
  NOTICE:
  Please don't modify the CRC checksum function.
  Because the same algorithm is used to store NBG files for  NBG files checksum.
*/
static vip_uint32_t nbglk_crc_checksum(
    vip_uint32_t org_crc,
    vip_uint8_t *data,
    vip_size_t size
    )
{
    static vip_uint32_t table[256] = { 0 };
    vip_uint32_t cur_crc = 0;

    if (0 == table[1]) {
        /* after init, only table[0] is 0 */
        crc32_table(table);
    }

    cur_crc = org_crc ^ 0xffffffffL;
    while (size) {
        cur_crc = table[(cur_crc ^ (*data++)) & 0xff] ^ (cur_crc >> 8);
        size--;
    }
    cur_crc ^= 0xffffffffL;

    return cur_crc;
}

vip_status_e nbglk_section_checksum(
    vip_network_t *network
    )
{
    vip_status_e status = VIP_SUCCESS;

    if ((network->nbg_desc.fixed.header.version & 0x00020000) != 0x00020000) {
        return VIP_SUCCESS;
    }

    /* load states section checksum */
    if (network->load_states != VIP_NULL) {
        vip_uint32_t load_state_cs = nbglk_crc_checksum(0, \
            (vip_uint8_t*)network->load_states, network->nbg_desc.fixed.load_states.size);
        if(load_state_cs != network->nbg_desc.check_sum.load_states) {
            vip_error("checksum of load state is not same, origin=0x%x, cur=0x%x\n",
                      network->nbg_desc.check_sum.load_states,load_state_cs);
            network->check_sect_status[NBGLK_SECTION_CHECKSUM_LOAD_STATES] = 1;
            vipGoOnError(VIP_ERROR_FAILURE);
        }
        else {
            vip_info("checksum of load states is correct, origin=0x%x, cur=0x%x\n",
                      network->nbg_desc.check_sum.load_states,load_state_cs);
            network->check_sect_status[NBGLK_SECTION_CHECKSUM_LOAD_STATES] = 0;
        }
    }
    else {
        status = VIP_ERROR_FAILURE;
        vip_warning("load states is NULL!\n");
    }

    /* ppu instr section checksum */
    if (network->ppu_instr.logical != VIP_NULL) {
        vip_uint32_t ppu_instr_cs = nbglk_crc_checksum(0, \
            (vip_uint8_t*)network->ppu_instr.logical, network->ppu_instr.size);
        if(ppu_instr_cs != network->nbg_desc.check_sum.ppu_instr) {
            vip_error("checksum of ppu instr is not same, origin=0x%x, cur=0x%x\n",
                     network->nbg_desc.check_sum.ppu_instr,ppu_instr_cs);
            network->check_sect_status[NBGLK_SECTION_CHECKSUM_PPU_INSTR] = 1;
            vipGoOnError(VIP_ERROR_FAILURE);
        }
        else {
            vip_info("checksum of ppu instr is correct, origin=0x%x, cur=0x%x\n",
                     network->nbg_desc.check_sum.ppu_instr,ppu_instr_cs);
            network->check_sect_status[NBGLK_SECTION_CHECKSUM_PPU_INSTR] = 0;
        }
    }
    else {
        if (network->nbg_desc.check_sum.ppu_instr != 0) {
            network->check_sect_status[NBGLK_SECTION_CHECKSUM_PPU_INSTR] = 0;
            vip_error("ppu instr checksum in nbg is not zero, then ppu_instr buffer is NULL!\n");
            vipGoOnError(VIP_ERROR_FAILURE);
        }
    }

    /* coeff data section checksum */
    if (network->coeff_data.logical != VIP_NULL) {
        vip_uint32_t coeff_data_cs = nbglk_crc_checksum(0, \
            (vip_uint8_t*)network->coeff_data.logical, network->coeff_data.size);
        if(coeff_data_cs != network->nbg_desc.check_sum.coeff_data) {
            vip_error("checksum of coeff data is not same, origin=0x%x, cur=0x%x\n",
                network->nbg_desc.check_sum.coeff_data, coeff_data_cs);
            network->check_sect_status[NBGLK_SECTION_CHECKSUM_COEFF_DATA] = 1;
            vipGoOnError(VIP_ERROR_FAILURE);
        }
        else {
            vip_print("checksum of coeff data is correct, origin=0x%x, cur=0x%x\n",
                network->nbg_desc.check_sum.coeff_data,coeff_data_cs);
            network->check_sect_status[NBGLK_SECTION_CHECKSUM_COEFF_DATA] = 0;
        }
    }
    else {
        if (network->nbg_desc.check_sum.coeff_data != 0) {
            network->check_sect_status[NBGLK_SECTION_CHECKSUM_COEFF_DATA] = 0;
            vip_error("coeff data checksum in nbg is not zero, then coeff data buffer is NULL!\n");
            vipGoOnError(VIP_ERROR_FAILURE);
        }
    }

    vip_info("section checksum successfully\n");
onError:
    return status;
}

#if vpmdENABLE_CHECKSUM
static nbglk_checksum_type_e map_checksum_type(
    nbglk_record_info_type_e type
    )
{
    switch (type) {
    case RECORD_TYPE_NN_COMMAND:
    case RECORD_TYPE_SP_COMMAND:
    case RECORD_TYPE_NN_LUT_COMMAND:
    case RECORD_TYPE_NT_COMMAND:
        return NBGLK_CHECKSUM_NN_COMMAND;
    case RECORD_TYPE_TP_COMMAND:
        return NBGLK_CHECKSUM_TP_COMMAND;
    case RECORD_TYPE_SH_COMMAND:
        return NBGLK_CHECKSUM_SH_COMMAND;
    case RECORD_TYPE_NN_WEIGHT:
        return NBGLK_CHECKSUM_NN_WEIGHT;
    case RECORD_TYPE_TP_WEIGHT:
        return NBGLK_CHECKSUM_TP_WEIGHT;
    case RECORD_TYPE_SH_WEIGHT:
        return NBGLK_CHECKSUM_SH_WEIGHT;
    case RECORD_TYPE_NN_WEIGHT_BITSTREAM:
        return NBGLK_CHECKSUM_NN_WEIGHT_BITSTREAM;
    case RECORD_TYPE_SP_WEIGHT:
        return NBGLK_CHECKSUM_SP_WEIGHT;
    case RECORD_TYPE_SP_WEIGHT_BITSTREAM:
        return NBGLK_CHECKSUM_SP_WEIGHT_BITSTREAM;
    default:
        break;
    }

    return NBGLK_CHECKSUM_MAX;
}

vip_status_e nbglk_network_checksum(
    vip_network_t *network,
    nbglk_checksum_type_e checksum_type
    )
{
#define HEAP_SORT                                                               \
    while (cur > 0) {                                                           \
        parent = (cur - 1) / 2;                                                 \
        if (ignore_point[parent] >= ignore_point[cur]) {                        \
            break;                                                              \
        }                                                                       \
        temp = ignore_point[parent];                                            \
        ignore_point[parent] = ignore_point[cur];                               \
        ignore_point[cur] = temp;                                               \
        cur = parent;                                                           \
    }

#define CAL_CHECKSUM(type)                                                      \
    for (j = 0; j < index; j++) {                                               \
        vip_size_t cur_size = 0;                                                \
        if (ignore_point[j] < VIPPTR_TO_UINT64(logical)) continue;              \
        if (ignore_point[j] >= VIPPTR_TO_UINT64(logical) + size) break;         \
        cur_size = (vip_size_t)(ignore_point[j] - VIPPTR_TO_UINT64(logical));   \
        value[type] = nbglk_crc_checksum(value[type], logical, cur_size);       \
        logical += (cur_size + sizeof(vip_uint32_t));                           \
        size -= (cur_size + sizeof(vip_uint32_t));                              \
    }                                                                           \
    value[type] = nbglk_crc_checksum(value[type], logical, size);

    vip_status_e status = VIP_SUCCESS;
    vip_uint32_t value[NBGLK_CHECKSUM_MAX] = { 0 };
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
    vip_uint32_t i = 0, j = 0, k = 0;
    vip_uint32_t index = 0;
    vip_uint32_t checksum_mask = (NBGLK_CHECKSUM_MAX == checksum_type) ?
        (1 << checksum_type) - 1 : (1 << checksum_type);
    vip_address_t *ignore_point = VIP_NULL;
    vip_address_t temp = 0;
    nbglk_checksum_type_e cur_type = NBGLK_CHECKSUM_MAX;
    if ((VIP_NULL == network) || (vip_false_e == network->prepared_network)) {
        vip_error("fail to cal kernel crc, please prepare network firstly\n");
        return VIP_ERROR_NOT_SUPPORTED;
    }
    vip_info("start do network checksum mask=0x%x\n", checksum_mask);

    /* do not cal input and output in checksum */
    for (i = 0; i < network->nbg_desc.n_inputs; i++) {
        index += (network->network_inputs[i].slice_num * 2);
    }
    for (i = 0; i < network->nbg_desc.n_outputs; i++) {
        index += (network->network_outputs[i].slice_num * 2);
    }
    if (0 < index) {
        viphal_allocate_memory(sizeof(vip_address_t) * index, (void**)&ignore_point);
        viphal_zero_memory(ignore_point, sizeof(vip_address_t) * index);
        index = 0;
        /* heap sort for all patched address */
        for (i = 0; i < network->nbg_desc.n_inputs; i++) {
            for (j = 0; j < network->network_inputs[i].slice_num; j++) {
                vip_uint32_t cur = index;
                vip_uint32_t parent = 0;
                ignore_point[index++] = network->network_inputs[i].logical_in_cmd[j] +
                    network->network_inputs[i].patch[j]->dest_offset;
                HEAP_SORT
                #if vpmdENABLE_40BITVA
                cur = index;
                ignore_point[index++] = network->network_inputs[i].logical_in_cmd[j] +
                    network->network_inputs[i].patch[j]->dest_offset_h;
                HEAP_SORT
                #endif
            }
        }
        for (i = 0; i < network->nbg_desc.n_outputs; i++) {
            for (j = 0; j < network->network_outputs[i].slice_num; j++) {
                vip_uint32_t cur = index;
                vip_uint32_t parent = 0;
                ignore_point[index++] = network->network_outputs[i].logical_in_cmd[j] +
                    network->network_outputs[i].patch[j]->dest_offset;
                HEAP_SORT
                #if vpmdENABLE_40BITVA
                cur = index;
                ignore_point[index++] = network->network_outputs[i].logical_in_cmd[j] +
                    network->network_outputs[i].patch[j]->dest_offset_h;
                HEAP_SORT
                #endif
            }
        }
        for (i = 1; i < index; i++) {
            vip_uint32_t cur = 0, left = 0, right = 0, priority = 0;
            temp = ignore_point[0];
            ignore_point[0] = ignore_point[index - i];
            ignore_point[index - i] = temp;

            while (cur * 2 + 1 < index - i) {
                left = cur * 2 + 1;
                right = cur * 2 + 2;

                if (left < index - i && ignore_point[left] > ignore_point[priority]) priority = left;
                if (right < index - i && ignore_point[right] > ignore_point[priority]) priority = right;

                if (priority == cur) break;

                temp = ignore_point[cur];
                ignore_point[cur] = ignore_point[priority];
                ignore_point[priority] = temp;
                cur = priority;
            }
        }
    }

    for (i = 0; i < network->segment_num; i++) {
        nbglk_segment_t *segment = &network->segments[i];
        nbglk_record_info_t *recd_info = VIP_NULL;
        if (segment->is_sw_segm == vip_false_e) {
            vip_uint8_t *logical = VIP_NULL;
            vip_size_t size = 0;
            for (k = 1; k < nbglk_hashmap_free_pos(&segment->record_list); k++) {
                nbglk_hashmap_get_by_index(&segment->record_list, k, (void**)&recd_info);
                cur_type = map_checksum_type(recd_info->record_type);

                if (!(checksum_mask & (1 << cur_type))) {
                    continue;
                }

                switch (cur_type) {
                case NBGLK_CHECKSUM_NN_COMMAND:
                {
                    logical = network->nnCmds.logical + recd_info->offset;
                }
                break;

                case NBGLK_CHECKSUM_TP_COMMAND:
                {
                    logical = network->tpCmds.logical + recd_info->offset;
                }
                break;

                case NBGLK_CHECKSUM_SH_COMMAND:
                {
                    if (vip_true_e == network->realloc_sh_buff) {
                        logical = segment->sh_cmd_buffer.logical + recd_info->offset;
                    }
                    else {
                        logical = network->ppu_instr.logical + recd_info->offset;
                    }
                }
                break;

                case NBGLK_CHECKSUM_SP_WEIGHT:
                case NBGLK_CHECKSUM_NN_WEIGHT:
                {
                    if (0 != network->coeff_header.mem_id) {
                        /* kernel descriptor */
                        logical = network->coeff_header.logical + recd_info->offset;
                        break;
                    }
                }
                case NBGLK_CHECKSUM_TP_WEIGHT:
                case NBGLK_CHECKSUM_SH_WEIGHT:
                case NBGLK_CHECKSUM_SP_WEIGHT_BITSTREAM:
                case NBGLK_CHECKSUM_NN_WEIGHT_BITSTREAM:
                {
                    vip_size_t offset = 0;
                    vip_uint8_t* base_logical = VIP_NULL;
                    vipOnError(nbglk_coeff_get_property(network, (vip_uint32_t)recd_info->offset,
                        &base_logical, VIP_NULL, &offset, VIP_NULL));
                    logical = base_logical + offset;
                }
                break;

                default:
                    break;
                }
                size = recd_info->size;
                if (logical != VIP_NULL) {
                    CAL_CHECKSUM(cur_type);
                }
            }

            if (checksum_mask & (1 << NBGLK_CHECKSUM_STATE_LOAD)) {
                logical = segment->states_buffer.logical;
                size = segment->states_buffer.size;
                CAL_CHECKSUM(NBGLK_CHECKSUM_STATE_LOAD);
            }
        }
    }

    for (i = 0; i < NBGLK_CHECKSUM_MAX; i++) {
        if (!(checksum_mask & (1 << i))) {
            continue;
        }

        if ((0 == network->checksum[i]) || (network->checksum[i] == value[i])) {
            network->checksum[i] = value[i];
            if (0 != network->checksum[i]) {
                vip_info("checksum %s value=0x%x\n", command_tags[i], value[i]);
            }
        }
        else {
            status = VIP_ERROR_FAILURE;
            vip_error("checksum of %s is not same, origin=0x%x, cur=0x%x\n",
                command_tags[i], network->checksum[i], value[i]);
            network->check_status[i] = vip_true_e;
        }
    }

    if (VIP_NULL != ignore_point) {
        viphal_free_memory(ignore_point);
        ignore_point = VIP_NULL;
    }

onError:
    return status;
#undef CAL_CHECKSUM
#undef HEAP_SORT
}

#if vpmdENABLE_CHECKSUM > 1
vip_status_e nbglk_nbg_checksum(
    void *data,
    vip_size_t size
    )
{
    vip_status_e status = VIP_SUCCESS;
    vip_uint8_t *buffer = (vip_uint8_t*)data;
    vip_uint32_t checksum_offset = 108; 
    vip_uint32_t nbg_checksum = *(vip_uint32_t*)(buffer + checksum_offset);
    vip_uint32_t rt_checksum = 0;

    if (0 == nbg_checksum) {
        return VIP_SUCCESS;
    }
    vip_info("start do nbg file checksum\n");

    /* checksum field is 0 used to cal */
    *(vip_uint32_t*)(buffer + checksum_offset) = 0;

    rt_checksum = nbglk_crc_checksum(0, buffer, size);

    /* restore checksum field value */
    *(vip_uint32_t*)(buffer + checksum_offset) = nbg_checksum;

    if (nbg_checksum != rt_checksum) {
        vip_error("fail to NBG file checksum nbg_checksum=0x%08x, rt_checksum=0x%08x\n",
                     nbg_checksum, rt_checksum);
        status = VIP_ERROR_INVALID_ARGUMENTS;
    }

    return status;
}
#endif

#endif
