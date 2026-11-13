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

#include <nbg_fmt_descriptor.h>
#include <nbg_fmt_v1.h>
#include <nbg_fmt_util.h>
#include <vip_lite.h>

/***** Read different parts of the binary file. *****/
static vip_status_e read_bin_header(
    nbg_fmt_reader_t *reader,
    nbg_header_t *header
    )
{
    vip_status_e status = VIP_SUCCESS;
    vip_uint32_t i = 0;

    static const char magic[4] = {'V', 'P', 'M', 'N'};
    nbg_fmt_reader_rewind(reader);   /* Go to the beginning. */
    /* Read magic. */
    for (i = 0; i < 4; i++) {
        header->magic[i] = nbg_fmt_read_byte(reader, 1);
        if (header->magic[i] != magic[i]) {
            vip_error("fail nbg magic not match, nbg is bad\n");
            vipGoOnError(VIP_ERROR_INVALID_ARGUMENTS);
        }
    }
    /* Read "Version". */
    header->version = nbg_fmt_read_uInt(reader, 1);
    /* Read "hardware target". */
    header->hw_target = nbg_fmt_read_uInt(reader, 1);
    /* Read "network_name". */
    nbg_fmt_read_data_copy(reader, header->network_name, sizeof(header->network_name), "net_name");
    nbg_fmt_reader_forward(reader, sizeof(header->network_name));
    /* Read "Layer_count". */
    header->layer_count = nbg_fmt_read_uInt(reader, 1);
    /* Read "operation_count". */
    header->operation_count = nbg_fmt_read_uInt(reader, 1);
    /* Read "input_count". */
    header->input_count = nbg_fmt_read_uInt(reader, 1);
    /* Read "output_count". */
    header->output_count = nbg_fmt_read_uInt(reader, 1);
    if (header->version >= 0x00010003) {
        nbg_fmt_read_data_copy(reader, &header->feature_db, sizeof(nbg_feature_database_t), "feature_db");
        nbg_fmt_reader_forward(reader, sizeof(nbg_feature_database_t));
    }
    /* no core count info in old nbg */
    if (0 == header->feature_db.core_count) {
        header->feature_db.core_count = 1;
    }

onError:
    return status;
}

static vip_status_e read_bin_pool(
    nbg_fmt_reader_t *reader,
    nbg_memory_pool_t *pool
    )
{
    vip_status_e status = VIP_SUCCESS;

    /* Read "aligned_size". */
    pool->size = nbg_fmt_read_uInt(reader, 1);
    /* Read "alignment". */
    pool->alignment = nbg_fmt_read_uInt(reader, 1);
    /* Read "pool_base". */
    pool->base = nbg_fmt_read_uInt(reader, 1);

    return status;
}

static vip_status_e read_bin_entry(
    nbg_fmt_reader_t *reader,
    nbg_section_entry_t *entry,
    const vip_char_t *name
    )
{
    vip_status_e status = VIP_SUCCESS;
    vip_bool_e nbg_fail = vip_false_e;

    if (reader->create_from != NBG_CREATE_NETWORK_FROM_FILE) {
        if ((reader->offset + sizeof(vip_uint32_t)) > reader->total_size) {
            vip_error("fail to read offset=%d, entry=%s\n", reader->offset, name);
            vipGoOnError(VIP_ERROR_OUT_OF_MEMORY);
        }
    }
    entry->offset = nbg_fmt_read_uInt(reader, 1);

    if (reader->create_from != NBG_CREATE_NETWORK_FROM_FILE) {
        if ((reader->offset + sizeof(vip_uint32_t)) > reader->total_size) {
            vip_error("fail to read offset=%d, entry=%s\n", reader->offset, name);
            vipGoOnError(VIP_ERROR_OUT_OF_MEMORY);
        }
    }
    entry->size = nbg_fmt_read_uInt(reader, 1);

    /* Sanity check nbg entry */
    if (reader->create_from != NBG_CREATE_NETWORK_FROM_FILE) {
        if ((entry->offset + entry->size) > reader->total_size) {
            nbg_fail = vip_true_e;
        }
    }
    else {
        /* testing */
        status = viphal_os_seek_file((entry->offset + entry->size), VIPHAL_FILE_SET, reader->file);
        if (status != VIP_SUCCESS) {
            nbg_fail = vip_true_e;
        }
        /* back */
        viphal_os_seek_file(reader->offset, VIPHAL_FILE_SET, reader->file);
    }
    if (nbg_fail) {
        vip_error("fail to read %s entry offset=%d, size=%d, offset+size large than NBG file total size=%d\n",
               name, entry->offset, entry->size, reader->total_size);
        vipGoOnError(VIP_ERROR_OUT_OF_MEMORY);
    }

onError:
    return status;
}

static vip_status_e nbg_fmt_read_fixed(
    nbg_fmt_reader_t *reader,
    nbg_fixed_header_t *fixed,
    nbg_fmt_descriptor_t *nbg_desc
    )
{
    vip_status_e status = VIP_SUCCESS;

    /* Read each field. Note that the order is strictly followed. */
    vipOnError(read_bin_header(reader, &fixed->header));
    vipOnError(read_bin_pool(reader, &fixed->pool));
    fixed->axi_sram_base = nbg_fmt_read_uInt(reader, 1);
    fixed->axi_sram_size = nbg_fmt_read_uInt(reader, 1);
    if (fixed->header.version >= 0x00010008) {
        fixed->vip_sram_base = nbg_fmt_read_uInt(reader, 1);
        fixed->vip_sram_size = nbg_fmt_read_uInt(reader, 1);
    }
    vipOnError(read_bin_entry(reader, &fixed->input_table, "input"));
    vipOnError(read_bin_entry(reader, &fixed->output_table, "output"));
    vipOnError(read_bin_entry(reader, &fixed->layer_table, "layer"));
    vipOnError(read_bin_entry(reader, &fixed->operation_table, "operation"));
    if (fixed->header.version >= 0x0001001C) {
        vipOnError(read_bin_entry(reader, &fixed->coeff_header_table, "coeff_header_table"));
        vipOnError(read_bin_entry(reader, &fixed->coeff_header, "coeff_header"));
    }
    else {
        vipOnError(read_bin_entry(reader, &fixed->coeff_data_table, "coeff_table"));
        vipOnError(read_bin_entry(reader, &fixed->coeff_data, "coeff"));
    }
    vipOnError(read_bin_entry(reader, &fixed->nn_op_data_table, "nn_op"));
    vipOnError(read_bin_entry(reader, &fixed->tp_op_data_table, "tp_op"));
    vipOnError(read_bin_entry(reader, &fixed->sh_op_data_table, "sh_op"));
    vipOnError(read_bin_entry(reader, &fixed->patch_data_table, "patch"));
    vipOnError(read_bin_entry(reader, &fixed->layer_param_table, "layer_param"));
    vipOnError(read_bin_entry(reader, &fixed->sw_op_data_table, "sw_op"));
    if (fixed->header.version >= 0x0001000C) {
        vipOnError(read_bin_entry(reader, &fixed->hw_init_op_table, "hw_init"));
        vipOnError(read_bin_entry(reader, &fixed->ICD_table, "ICDT"));
        vipOnError(read_bin_entry(reader, &fixed->ICD, "ICD"));
    }
    else {
        fixed->hw_init_op_table.size = 0;
        fixed->ICD_table.size = 0;
        fixed->ICD.size = 0;
    }
    if (fixed->header.version >= 0x0001000E) {
        vipOnError(read_bin_entry(reader, &fixed->ppu_param_table, "ppu_param"));
        if (0 == fixed->header.feature_db.change_ppu_param) {
            fixed->ppu_param_table.size = 0;
        }
    }
    else {
        fixed->ppu_param_table.size = 0;
    }
    if (fixed->header.version >= 0x0001000F) {
        vipOnError(read_bin_entry(reader, &fixed->op_sync_Table, "op_sync"));
    }
    if (fixed->header.version >= 0x00010013) {
        vipOnError(read_bin_entry(reader, &fixed->dma_sbi_info, "dma_sbi"));
    }
    if (fixed->header.version >= 0x00010018) {
        vipOnError(read_bin_entry(reader, &fixed->ppu_instr, "ppu_inst"));
        vipOnError(read_bin_entry(reader, &fixed->dyna_io, "dyna_io"));
        vipOnError(read_bin_entry(reader, &fixed->load_states, "load_states"));
    }
    if (fixed->header.version >= 0x0001001B) {
        vipOnError(read_bin_entry(reader, &fixed->dma_sbi_addition, "dma_addi"));
    }

    if (fixed->header.version >= 0x0001001C) {
        vipOnError(read_bin_entry(reader, &fixed->coeff_data_table, "coeff_table"));
        vipOnError(read_bin_entry(reader, &fixed->coeff_data, "coeff"));
        vipOnError(read_bin_entry(reader, &fixed->ppu_instr_table, "ppu_instr_table"));
        vipOnError(read_bin_entry(reader, &fixed->dyna_io_table, "dyna_io_table"));
        vipOnError(read_bin_entry(reader, &fixed->load_states_table, "load_states_table"));
    }

    if (fixed->header.version >= 0x00010020) {
        vipOnError(read_bin_entry(reader, &fixed->ldump_operation, "ldump_operation"));
        vipOnError(read_bin_entry(reader, &fixed->ldump_outputs, "ldump_outputs"));
        vipOnError(read_bin_entry(reader, &fixed->ldump_patch_data, "ldump_patchdata"));
        vipOnError(read_bin_entry(reader, &fixed->ldump_instr_table, "ldump_instrtable"));
        vipOnError(read_bin_entry(reader, &fixed->ldump_instr, "ldump_instr"));
        vipOnError(read_bin_entry(reader, &fixed->ldump_load_states_table, "ldump_ls_tble"));
        vipOnError(read_bin_entry(reader, &fixed->ldump_load_states, "ldump_ls"));
    }

    nbg_desc->fixed.header.version = fixed->header.version;
    nbg_desc->fixed.header.hw_target = fixed->header.hw_target;
    nbg_desc->fixed.header.class = 0;
    viphal_copy_memory(nbg_desc->net_info.name, fixed->header.network_name,
        sizeof(vip_char_t) * NETWORK_NAME_SIZE);
    nbg_desc->net_info.attribute.nn_command_size = fixed->header.feature_db.nn_command_size;
    nbg_desc->net_info.attribute.change_ppu_param = fixed->header.feature_db.change_ppu_param;
    nbg_desc->net_info.attribute.tp_command_size = fixed->header.feature_db.tp_command_size;
    nbg_desc->net_info.core_count = fixed->header.feature_db.core_count;
    nbg_desc->fixed.header.nbg_crc_sum = fixed->header.feature_db.nbg_crc_sum;
    if (fixed->header.feature_db.nbg_40bit_va == 0)
    {
        nbg_desc->net_info.address_width = 32;
    }
    else if (fixed->header.feature_db.nbg_40bit_va == 1)
    {
        nbg_desc->net_info.address_width = 40;
    }
    nbg_desc->mem_info.pool_size = fixed->pool.size;
    nbg_desc->mem_info.axi_sram_size = fixed->axi_sram_size;
    nbg_desc->mem_info.vip_sram_size = fixed->vip_sram_size;

    nbg_desc->fixed.coeff_header.offset = fixed->coeff_header.offset;
    nbg_desc->fixed.coeff_header.size = fixed->coeff_header.size;
    nbg_desc->fixed.nn_op_data_table.offset = fixed->nn_op_data_table.offset;
    nbg_desc->fixed.nn_op_data_table.size = fixed->nn_op_data_table.size;
    nbg_desc->fixed.tp_op_data_table.offset = fixed->tp_op_data_table.offset;
    nbg_desc->fixed.tp_op_data_table.size = fixed->tp_op_data_table.size;
    nbg_desc->fixed.ICD.offset = fixed->ICD.offset;
    nbg_desc->fixed.ICD.size = fixed->ICD.size;
    nbg_desc->fixed.ppu_instr.offset = fixed->ppu_instr.offset;
    nbg_desc->fixed.ppu_instr.size = fixed->ppu_instr.size;
    nbg_desc->fixed.load_states.offset = fixed->load_states.offset;
    nbg_desc->fixed.load_states.size = fixed->load_states.size;
    nbg_desc->fixed.dyna_io.offset = fixed->dyna_io.offset;
    nbg_desc->fixed.dyna_io.size = fixed->dyna_io.size;
    nbg_desc->fixed.coeff_data.offset = fixed->coeff_data.offset;
    nbg_desc->fixed.coeff_data.size = fixed->coeff_data.size;

    nbg_desc->fixed.ldump_operation.offset = fixed->ldump_operation.offset;
    nbg_desc->fixed.ldump_operation.size = fixed->ldump_operation.size;
    nbg_desc->fixed.ldump_instr_table.offset = fixed->ldump_instr_table.offset;
    nbg_desc->fixed.ldump_instr_table.size = fixed->ldump_instr_table.size;
    nbg_desc->fixed.ldump_load_states_table.offset = fixed->ldump_load_states_table.offset;
    nbg_desc->fixed.ldump_load_states_table.size = fixed->ldump_load_states_table.size;
    nbg_desc->fixed.ldump_patch_data.offset = fixed->ldump_patch_data.offset;
    nbg_desc->fixed.ldump_patch_data.size = fixed->ldump_patch_data.size;
    nbg_desc->fixed.ldump_outputs.offset = fixed->ldump_outputs.offset;
    nbg_desc->fixed.ldump_outputs.size = fixed->ldump_outputs.size;
    nbg_desc->fixed.ldump_instr.offset = fixed->ldump_instr.offset;
    nbg_desc->fixed.ldump_instr.size = fixed->ldump_instr.size;
    nbg_desc->fixed.ldump_load_states.offset = fixed->ldump_load_states.offset;
    nbg_desc->fixed.ldump_load_states.size = fixed->ldump_load_states.size;

    return status;

onError:
    vip_error("fail to read nbg file fixed section\n");
    return status;
}

/*
Read dynamic:
1. none-video memory section, alloc system memory and read out data in memory.
2. video memory section, read output entry table only.
*/
static vip_status_e nbg_fmt_read_dynamic(
    nbg_fmt_reader_t *reader,
    nbg_fixed_header_t *fixed,
    nbg_fmt_descriptor_t *nbg_desc
    )
{
    vip_status_e status = VIP_SUCCESS;
    vip_uint8_t *dyna_ptr = VIP_NULL;
    vip_uint32_t dyna_size = 0;
#define nbg_fma_max(x, y) (((x) >= (y)) ? (x) : (y))
    /* Calculate the mem size to put dyna data structures . */
    dyna_size = nbg_fma_max(fixed->input_table.size, dyna_size);
    dyna_size = nbg_fma_max(fixed->output_table.size, dyna_size);
    dyna_size = nbg_fma_max(fixed->layer_table.size, dyna_size);
    dyna_size = nbg_fma_max(fixed->operation_table.size, dyna_size);
    dyna_size = nbg_fma_max(fixed->layer_param_table.size, dyna_size);
    dyna_size = nbg_fma_max(fixed->sw_op_data_table.size, dyna_size);
    dyna_size = nbg_fma_max(fixed->hw_init_op_table.size, dyna_size);
    dyna_size = nbg_fma_max(fixed->ICD_table.size, dyna_size);
    dyna_size = nbg_fma_max(fixed->ppu_param_table.size, dyna_size);
    dyna_size = nbg_fma_max(fixed->patch_data_table.size, dyna_size);
    dyna_size = nbg_fma_max(fixed->sh_op_data_table.size, dyna_size);
    dyna_size = nbg_fma_max(fixed->coeff_header_table.size, dyna_size);
    dyna_size = nbg_fma_max(fixed->coeff_data_table.size, dyna_size);
    dyna_size = nbg_fma_max(fixed->ppu_instr_table.size, dyna_size);
    dyna_size = nbg_fma_max(fixed->dyna_io_table.size, dyna_size);
    dyna_size = nbg_fma_max(fixed->load_states_table.size, dyna_size);

    dyna_size = nbg_fma_max(fixed->ldump_operation.size, dyna_size);
    dyna_size = nbg_fma_max(fixed->ldump_instr_table.size, dyna_size);
    dyna_size = nbg_fma_max(fixed->ldump_load_states_table.size, dyna_size);
    dyna_size = nbg_fma_max(fixed->ldump_patch_data.size, dyna_size);
    dyna_size = nbg_fma_max(fixed->ldump_instr.size, dyna_size);
    dyna_size = nbg_fma_max(fixed->ldump_load_states.size, dyna_size);
#undef nbg_fma_max
    if (0 == dyna_size) {
        vip_error("fail sanity check dyna_size=%d\n", dyna_size);
        vipGoOnError(VIP_ERROR_INVALID_NETWORK);
    }
    /* Create the memory to put all dyna data structures in,
       then set the pointers in network to this memory with offsets.
       Since related data will be directly accessed by VIP */
    vipOnError(viphal_allocate_memory(dyna_size, (void **)&dyna_ptr));
    viphal_zero_memory(dyna_ptr, dyna_size);
    /****** Copy data and set each pointer. ******/
    /* Input data entries. */
    if (fixed->input_table.size > 0) {
        vip_uint32_t index = 0;
        vip_uint32_t size = 0;
        nbg_section_input_output_t* input = VIP_NULL;
        nbg_section_input_output_old_t* input_old = VIP_NULL;
        vip_uint32_t sub_index = 0;

        nbg_fmt_reader_locate(reader, fixed->input_table.offset);
        nbg_fmt_read_data_copy(reader, dyna_ptr, fixed->input_table.size, "input");
        if (fixed->header.version >= 0x0001000B) {
            size = sizeof(nbg_section_input_output_t);
        }
        else if (fixed->header.version >= 0x00010004) {
            size = sizeof(nbg_section_input_output_t) - sizeof(vip_uint32_t) * (MAX_NUM_DIMS - OLD_MAX_NUM_DIMS);
        }
        else {
            size = sizeof(nbg_section_input_output_t) - sizeof(vip_char_t) * MAX_IO_NAME_LEGTH -
                sizeof(vip_uint32_t) * (MAX_NUM_DIMS - OLD_MAX_NUM_DIMS);
        }
        nbg_desc->n_inputs = fixed->input_table.size / size;

        vipOnError(viphal_allocate_memory(sizeof(nbg_desc_input_output_t) * nbg_desc->n_inputs,
                                         (void **)&nbg_desc->inputs));
        viphal_zero_memory(nbg_desc->inputs, sizeof(nbg_desc_input_output_t) * nbg_desc->n_inputs);
        for (index = 0; index < nbg_desc->n_inputs; index++) {
            if (fixed->header.version >= 0x0001000B) {
                input = (nbg_section_input_output_t*)((vip_int8_t*)dyna_ptr + index * size);

                nbg_desc->inputs[index].data_format = input->data_format;
                nbg_desc->inputs[index].data_type = input->data_type;
                nbg_desc->inputs[index].dim_count = input->dim_count;
                nbg_desc->inputs[index].quant_format = input->quan_format;
                for (sub_index = 0; sub_index < input->dim_count; sub_index++) {
                    nbg_desc->inputs[index].dim_size[sub_index] = input->dim_size[sub_index];
                }
                if (VIP_BUFFER_QUANTIZE_DYNAMIC_FIXED_POINT == input->quan_format) {
                    nbg_desc->inputs[index].quant_data.dfp.fixed_point_pos = input->fixed_pos;
                }
                else {
                    nbg_desc->inputs[index].quant_data.affine.tf_scale = input->tf_scale;
                    nbg_desc->inputs[index].quant_data.affine.tf_zero_point = input->tf_zerop;
                }
                viphal_copy_memory(nbg_desc->inputs[index].name, input->name,
                                   sizeof(vip_char_t) * MAX_IO_NAME_LEGTH);
            }
            else {
                input_old = (nbg_section_input_output_old_t*)((vip_int8_t*)dyna_ptr + index * size);

                nbg_desc->inputs[index].data_format = input_old->data_format;
                nbg_desc->inputs[index].data_type = input_old->data_type;
                nbg_desc->inputs[index].dim_count = input_old->dim_count;
                nbg_desc->inputs[index].quant_format = input_old->quan_format;
                for (sub_index = 0; sub_index < input_old->dim_count; sub_index++) {
                    nbg_desc->inputs[index].dim_size[sub_index] = input_old->dim_size[sub_index];
                }
                if (VIP_BUFFER_QUANTIZE_DYNAMIC_FIXED_POINT == input_old->quan_format) {
                    nbg_desc->inputs[index].quant_data.dfp.fixed_point_pos = input_old->fixed_pos;
                }
                else {
                    nbg_desc->inputs[index].quant_data.affine.tf_scale = input_old->tf_scale;
                    nbg_desc->inputs[index].quant_data.affine.tf_zero_point = input_old->tf_zerop;
                }
                if (fixed->header.version >= 0x00010004) {
                    viphal_copy_memory(nbg_desc->inputs[index].name, input_old->name,
                        sizeof(vip_char_t) * MAX_IO_NAME_LEGTH);
                }
            }
        }
    }

    /* Output data entries. */
    if (fixed->output_table.size > 0) {
        vip_uint32_t index = 0;
        vip_uint32_t size = 0;
        nbg_section_input_output_t* output = VIP_NULL;
        nbg_section_input_output_old_t* output_old = VIP_NULL;
        vip_uint32_t sub_index = 0;

        nbg_fmt_reader_locate(reader, fixed->output_table.offset);
        nbg_fmt_read_data_copy(reader, dyna_ptr, fixed->output_table.size, "output");
        if (fixed->header.version >= 0x0001000B) {
            size = sizeof(nbg_section_input_output_t);
        }
        else if (fixed->header.version >= 0x00010004) {
            size = sizeof(nbg_section_input_output_t) - sizeof(vip_uint32_t) * (MAX_NUM_DIMS - OLD_MAX_NUM_DIMS);
        }
        else {
            size = sizeof(nbg_section_input_output_t) - sizeof(vip_char_t) * MAX_IO_NAME_LEGTH -
                sizeof(vip_uint32_t) * (MAX_NUM_DIMS - OLD_MAX_NUM_DIMS);
        }
        nbg_desc->n_outputs = fixed->output_table.size / size;

        vipOnError(viphal_allocate_memory(sizeof(nbg_desc_input_output_t) * nbg_desc->n_outputs,
                                         (void **)&nbg_desc->outputs));
        viphal_zero_memory(nbg_desc->outputs, sizeof(nbg_desc_input_output_t) * nbg_desc->n_outputs);
        for (index = 0; index < nbg_desc->n_outputs; index++) {
            if (fixed->header.version >= 0x0001000B) {
                output = (nbg_section_input_output_t*)((vip_int8_t*)dyna_ptr + index * size);
                nbg_desc->outputs[index].data_format = output->data_format;
                nbg_desc->outputs[index].data_type = output->data_type;
                nbg_desc->outputs[index].dim_count = output->dim_count;
                nbg_desc->outputs[index].quant_format = output->quan_format;
                for (sub_index = 0; sub_index < output->dim_count; sub_index++) {
                    nbg_desc->outputs[index].dim_size[sub_index] = output->dim_size[sub_index];
                }
                if (VIP_BUFFER_QUANTIZE_DYNAMIC_FIXED_POINT == output->quan_format) {
                    nbg_desc->outputs[index].quant_data.dfp.fixed_point_pos = output->fixed_pos;
                }
                else {
                    nbg_desc->outputs[index].quant_data.affine.tf_scale = output->tf_scale;
                    nbg_desc->outputs[index].quant_data.affine.tf_zero_point = output->tf_zerop;
                }
                viphal_copy_memory(nbg_desc->outputs[index].name, output->name,
                                   sizeof(vip_char_t) * MAX_IO_NAME_LEGTH);
            }
            else {
                output_old = (nbg_section_input_output_old_t*)((vip_int8_t*)dyna_ptr + index * size);
                nbg_desc->outputs[index].data_format = output_old->data_format;
                nbg_desc->outputs[index].data_type = output_old->data_type;
                nbg_desc->outputs[index].dim_count = output_old->dim_count;
                nbg_desc->outputs[index].quant_format = output_old->quan_format;
                for (sub_index = 0; sub_index < output_old->dim_count; sub_index++) {
                    nbg_desc->outputs[index].dim_size[sub_index] = output_old->dim_size[sub_index];
                }
                if (VIP_BUFFER_QUANTIZE_DYNAMIC_FIXED_POINT == output_old->quan_format) {
                    nbg_desc->outputs[index].quant_data.dfp.fixed_point_pos = output_old->fixed_pos;
                }
                else {
                    nbg_desc->outputs[index].quant_data.affine.tf_scale = output_old->tf_scale;
                    nbg_desc->outputs[index].quant_data.affine.tf_zero_point = output_old->tf_zerop;
                }
                if (fixed->header.version >= 0x00010004) {
                    viphal_copy_memory(nbg_desc->outputs[index].name, output_old->name,
                        sizeof(vip_char_t) * MAX_IO_NAME_LEGTH);
                }
            }
        }
    }

    /* Layer data entries. */
    if (fixed->layer_table.size > 0) {
        vip_uint32_t index = 0;
        vip_uint32_t size = 0;
        nbg_section_layer_t* layer = VIP_NULL;

        nbg_fmt_reader_locate(reader, fixed->layer_table.offset);
        nbg_fmt_read_data_copy(reader, dyna_ptr, fixed->layer_table.size, "layer_table");
        if (fixed->header.version >= 0x00010008) {
            size = sizeof(nbg_section_layer_t);
        }
        else {
            size = sizeof(nbg_section_layer_t) - sizeof(vip_uint32_t);
        }
        nbg_desc->n_orig_layers = fixed->layer_table.size / size;

        vipOnError(viphal_allocate_memory(sizeof(nbg_desc_layer_t) * nbg_desc->n_orig_layers,
            (void **)&nbg_desc->orig_layers));
        viphal_zero_memory(nbg_desc->orig_layers, sizeof(nbg_desc_layer_t) * nbg_desc->n_orig_layers);
        for (index = 0; index < nbg_desc->n_orig_layers; index++) {
            layer = (nbg_section_layer_t*)((vip_int8_t*)dyna_ptr + index * size);

            viphal_copy_memory(nbg_desc->orig_layers[index].name, layer->name,
                sizeof(vip_char_t) * LAYER_NAME_SIZE);
            nbg_desc->orig_layers[index].node_id = layer->node_id;
            nbg_desc->orig_layers[index].operation_count = layer->operation_count;
            if (fixed->header.version >= 0x00010008) {
                nbg_desc->orig_layers[index].uid = layer->uid;
            }
        }
    }

    /* operation data entries. */
    if (fixed->operation_table.size > 0) {
        vip_uint32_t index = 0;
        vip_uint32_t size = 0;
        nbg_section_operation_t* operation = VIP_NULL;

        nbg_fmt_reader_locate(reader, fixed->operation_table.offset);
        nbg_fmt_read_data_copy(reader, dyna_ptr, fixed->operation_table.size, "operat_tb");
        if (fixed->header.version >= 0x00010017) {
            size = sizeof(nbg_section_operation_t);
        }
        else if (fixed->header.version >= 0x00010011) {
            size = sizeof(nbg_section_operation_t) - sizeof(vip_uint32_t);
        }
        else {
            size = sizeof(nbg_section_operation_t) - sizeof(vip_uint32_t) * 2;
        }
        nbg_desc->n_operations = fixed->operation_table.size / size;

        vipOnError(viphal_allocate_memory(sizeof(nbg_desc_operation_t) * nbg_desc->n_operations,
            (void **)&nbg_desc->operations));
        viphal_zero_memory(nbg_desc->operations, sizeof(nbg_desc_operation_t) * nbg_desc->n_operations);
        for (index = 0; index < nbg_desc->n_operations; index++) {
            operation = (nbg_section_operation_t*)((vip_int8_t*)dyna_ptr + index * size);

            nbg_desc->operations[index].index = operation->index;
            nbg_desc->operations[index].layer_id = operation->layer_id;
            nbg_desc->operations[index].patch_count = operation->patch_count;
            nbg_desc->operations[index].patch_index = operation->patch_index;
            nbg_desc->operations[index].state_id = operation->state_id;
            nbg_desc->operations[index].type = operation->type;
            if (fixed->header.version >= 0x00010011) {
                nbg_desc->operations[index].preempt = operation->preempt;
            }
            if (fixed->header.version >= 0x00010017) {
                nbg_desc->operations[index].abs_op_id = operation->abs_op_id;
                if (fixed->header.version < 0x0001001f) {
                    nbg_desc->operations[index].abs_share_id = operation->abs_op_id;
                }
                else {
                    nbg_desc->operations[index].abs_share_id = operation->abs_share_id;
                }
            }
            else {
                nbg_desc->operations[index].abs_op_id = 0;
                nbg_desc->operations[index].abs_share_id = 0;
            }
        }
    }

    /* NN op data entries. */
    if (fixed->nn_op_data_table.size > 0) {
        if (NBG_NN_COMMAND_SIZE_192 == fixed->header.feature_db.nn_command_size) {
            nbg_desc->n_nn_ops = fixed->nn_op_data_table.size / sizeof(nbg_desc_operation_192_t);
        }
        else {
            nbg_desc->n_nn_ops = fixed->nn_op_data_table.size / sizeof(nbg_desc_operation_128_t);
        }
    }

    /* TP op data entries. */
    if (fixed->tp_op_data_table.size > 0) {
        if (NBG_TP_COMMAND_SIZE_192 == fixed->header.feature_db.tp_command_size) {
            nbg_desc->n_tp_ops = fixed->tp_op_data_table.size / sizeof(nbg_desc_operation_192_t);
        }
        else {
            nbg_desc->n_tp_ops = fixed->tp_op_data_table.size / sizeof(nbg_desc_operation_128_t);
        }
    }

    /* SH op data entries. */
    if (fixed->sh_op_data_table.size > 0) {
        vip_uint32_t index = 0;
        vip_uint32_t size = 0;
        nbg_section_sh_operation_t* sh_op = VIP_NULL;

        nbg_fmt_reader_locate(reader, fixed->sh_op_data_table.offset);
        nbg_fmt_read_data_copy(reader, dyna_ptr, fixed->sh_op_data_table.size, "shoptb");
        if (fixed->header.version >= 0x0001000E) {
            size = sizeof(nbg_section_sh_operation_t);
        }
        else {
            size = sizeof(nbg_section_sh_operation_t) - sizeof(vip_uint32_t);
        }
        nbg_desc->n_sh_ops = fixed->sh_op_data_table.size / size;

        vipOnError(viphal_allocate_memory(sizeof(nbg_desc_sh_operation_t) * nbg_desc->n_sh_ops,
            (void **)&nbg_desc->sh_ops));
        viphal_zero_memory(nbg_desc->sh_ops, sizeof(nbg_desc_sh_operation_t) * nbg_desc->n_sh_ops);
        for (index = 0; index < nbg_desc->n_sh_ops; index++) {
            sh_op = (nbg_section_sh_operation_t*)((vip_int8_t*)dyna_ptr + index * size);

            nbg_desc->sh_ops[index].instr_index = sh_op->instr_index;
            if (fixed->header.version >= 0x0001000E) {
                nbg_desc->sh_ops[index].ppu_param_index = sh_op->ppu_param_index;
            }
        }
    }

    /* Patch data entries. */
    if (fixed->patch_data_table.size > 0) {
        vip_uint32_t index = 0;
        vip_uint32_t size = 0;
        nbg_section_patch_table_t* pd_entry = VIP_NULL;

        nbg_fmt_reader_locate(reader, fixed->patch_data_table.offset);
        nbg_fmt_read_data_copy(reader, dyna_ptr, fixed->patch_data_table.size, "patchtb");
        if (fixed->header.version >= 0x00010014) {
            /* added offset_h, Original_base_address_h, mask_h field */
            size = sizeof(nbg_section_patch_table_t);
        }
        else if (fixed->header.version >= 0x00010006) {
            /* added name field */
            size = sizeof(nbg_section_patch_table_t) - sizeof(vip_uint32_t) * 3;
        }
        else {
            size = sizeof(nbg_section_patch_table_t) - sizeof(vip_uint32_t) * 4;
        }
        nbg_desc->n_pd_sections = fixed->patch_data_table.size / size;

        vipOnError(viphal_allocate_memory(sizeof(nbg_desc_patch_t) * nbg_desc->n_pd_sections,
            (void **)&nbg_desc->pd_sections));
        viphal_zero_memory(nbg_desc->pd_sections, sizeof(nbg_desc_patch_t) * nbg_desc->n_pd_sections);
        for (index = 0; index < nbg_desc->n_pd_sections; index++) {
            pd_entry = (nbg_section_patch_table_t*)((vip_int8_t*)dyna_ptr + index * size);

            nbg_desc->pd_sections[index].dest_offset = pd_entry->dest_offset;
            nbg_desc->pd_sections[index].index = pd_entry->index;
            nbg_desc->pd_sections[index].source_type = pd_entry->source_type;
            nbg_desc->pd_sections[index].src_offset = pd_entry->src_offset;
            nbg_desc->pd_sections[index].transformed = pd_entry->transformed;
            nbg_desc->pd_sections[index].type = pd_entry->type;
            if (fixed->header.version >= 0x00010014) {
                nbg_desc->pd_sections[index].dest_offset_h = pd_entry->dest_offset_h;
                nbg_desc->pd_sections[index].mask_h = pd_entry->mask_h;
                #if vpmdNBG_SIZE_OVER_4G
                nbg_desc->pd_sections[index].src_offset |= (vip_size_t)(pd_entry->src_offset_h) << 32;
                #endif
            }
            if (fixed->header.version >= 0x00010006) {
                nbg_desc->pd_sections[index].name = pd_entry->name;
            }

            #if vpmdENABLE_MMU
            if (NBG_SOURCE_MISC_DYNA_OUTPUT == pd_entry->source_type) {
                /* the coeff data buffer should be writable, only for MMU enabled */
                nbg_desc->coeff_data_writable = vip_true_e;
                vip_info("can not set weight memory read only attribute\n");
            }
            #endif
        }
    }

    /*Layer Parameter Entrance*/
    if (fixed->layer_param_table.size > 0) {
        vip_uint32_t index = 0;
        vip_uint32_t size = 0;
        nbg_section_layer_parameter_t* layer_param = VIP_NULL;
        nbg_section_layer_parameter_old_t* layer_param_old = VIP_NULL;
        vip_uint32_t sub_index = 0;

        nbg_fmt_reader_locate(reader, fixed->layer_param_table.offset);
        nbg_fmt_read_data_copy(reader, dyna_ptr, fixed->layer_param_table.size, "layeparatb");
        if (fixed->header.version >= 0x0001000B) {
            size = sizeof(nbg_section_layer_parameter_t);
        }
        else {
            size = sizeof(nbg_section_layer_parameter_t) - sizeof(vip_uint32_t) * (MAX_NUM_DIMS - OLD_MAX_NUM_DIMS);
        }
        nbg_desc->n_layer_param = fixed->layer_param_table.size / size;

        vipOnError(viphal_allocate_memory(sizeof(nbg_desc_layer_parameter_t) * nbg_desc->n_layer_param,
            (void **)&nbg_desc->lp_entries));
        viphal_zero_memory(nbg_desc->lp_entries, sizeof(nbg_desc_layer_parameter_t) * nbg_desc->n_layer_param);
        for (index = 0; index < nbg_desc->n_layer_param; index++) {
            if (fixed->header.version >= 0x0001000B) {
                layer_param = (nbg_section_layer_parameter_t*)((vip_int8_t*)dyna_ptr + index * size);

                nbg_desc->lp_entries[index].address_offset = layer_param->address_offset;
                nbg_desc->lp_entries[index].data_format = layer_param->data_format;
                nbg_desc->lp_entries[index].data_type = layer_param->data_type;
                nbg_desc->lp_entries[index].dim_count = layer_param->dim_count;
                nbg_desc->lp_entries[index].fixpoint_zeropoint = layer_param->fixpoint_zeropoint;
                nbg_desc->lp_entries[index].index = layer_param->index;
                viphal_copy_memory(nbg_desc->lp_entries[index].param_name, layer_param->param_name,
                    sizeof(vip_char_t)* MAX_LAYER_PARAM_NAME_LENGTH);
                nbg_desc->lp_entries[index].quant_format = layer_param->quant_format;
                nbg_desc->lp_entries[index].source_type = layer_param->source_type;
                nbg_desc->lp_entries[index].tfscale = layer_param->tfscale;
                for (sub_index = 0; sub_index < layer_param->dim_count; sub_index++) {
                    nbg_desc->lp_entries[index].dims[sub_index] = layer_param->dims[sub_index];
                }
            }
            else {
                layer_param_old = (nbg_section_layer_parameter_old_t*)((vip_int8_t*)dyna_ptr + index * size);

                nbg_desc->lp_entries[index].address_offset = layer_param_old->address_offset;
                nbg_desc->lp_entries[index].data_format = layer_param_old->data_format;
                nbg_desc->lp_entries[index].data_type = layer_param_old->data_type;
                nbg_desc->lp_entries[index].dim_count = layer_param_old->dim_count;
                nbg_desc->lp_entries[index].fixpoint_zeropoint = layer_param_old->fixpoint_zeropoint;
                nbg_desc->lp_entries[index].index = layer_param_old->index;
                viphal_copy_memory(nbg_desc->lp_entries[index].param_name, layer_param_old->param_name,
                    sizeof(vip_char_t)* MAX_LAYER_PARAM_NAME_LENGTH);
                nbg_desc->lp_entries[index].quant_format = layer_param_old->quant_format;
                nbg_desc->lp_entries[index].source_type = layer_param_old->source_type;
                nbg_desc->lp_entries[index].tfscale = layer_param_old->tfscale;
                for (sub_index = 0; sub_index < layer_param_old->dim_count; sub_index++) {
                    nbg_desc->lp_entries[index].dims[sub_index] = layer_param_old->dims[sub_index];
                }
            }
        }
    }

    /*Software Operation Data Table Entrance*/
    if (fixed->sw_op_data_table.size > 0) {
        vip_uint32_t index = 0;
        vip_uint32_t size = 0;
        nbg_section_sw_operation_t* sw_op = VIP_NULL;

        nbg_fmt_reader_locate(reader, fixed->sw_op_data_table.offset);
        nbg_fmt_read_data_copy(reader, dyna_ptr, fixed->sw_op_data_table.size, "swoptb");
        if (fixed->header.version >= 0x00010009) {
            size = sizeof(nbg_section_sw_operation_t);
        }
        else {
            size = sizeof(nbg_section_sw_operation_t) - sizeof(vip_char_t) * MAX_SW_LAYER_NAME_LENGTH;
        }
        nbg_desc->n_sw_ops = fixed->sw_op_data_table.size / size;

        vipOnError(viphal_allocate_memory(sizeof(nbg_desc_sw_operation_t) * nbg_desc->n_sw_ops,
            (void **)&nbg_desc->sw_ops));
        viphal_zero_memory(nbg_desc->sw_ops, sizeof(nbg_desc_sw_operation_t) * nbg_desc->n_sw_ops);
        for (index = 0; index < nbg_desc->n_sw_ops; index++) {
            sw_op = (nbg_section_sw_operation_t*)((vip_int8_t*)dyna_ptr + index * size);

            nbg_desc->sw_ops[index].sw_peration_type = sw_op->sw_peration_type;
            if (fixed->header.version >= 0x00010009) {
                viphal_copy_memory(nbg_desc->sw_ops[index].name, sw_op->name,
                    sizeof(vip_char_t)* MAX_SW_LAYER_NAME_LENGTH);
            }
        }
    }

    /* coeff data table entries. */
    if (fixed->coeff_data_table.size > 0) {
        vip_uint32_t index = 0;
        vip_uint32_t size = 0;
        nbg_section_entry_t* table = VIP_NULL;

        nbg_fmt_reader_locate(reader, fixed->coeff_data_table.offset);
        nbg_fmt_read_data_copy(reader, dyna_ptr, fixed->coeff_data_table.size, "coeff_tb");
        size = sizeof(nbg_section_entry_t);
        nbg_desc->n_coeff_data = fixed->coeff_data_table.size / size;

        vipOnError(viphal_allocate_memory(sizeof(nbg_desc_data_table_t) * nbg_desc->n_coeff_data,
            (void **)&nbg_desc->coeff_data_table));
        viphal_zero_memory(nbg_desc->coeff_data_table, sizeof(nbg_desc_data_table_t) * nbg_desc->n_coeff_data);
        for (index = 0; index < nbg_desc->n_coeff_data; index++) {
            table = (nbg_section_entry_t*)((vip_int8_t*)dyna_ptr + index * size);

            nbg_desc->coeff_data_table[index].offset = table->offset;
            nbg_desc->coeff_data_table[index].size = table->size;
        }
    }

    /* HW initialize commands Entrance */
    if (fixed->hw_init_op_table.size > 0) {
        vip_uint32_t index = 0;
        vip_uint32_t size = 0;
        nbg_section_hw_init_operation_t* init_op = VIP_NULL;

        nbg_fmt_reader_locate(reader, fixed->hw_init_op_table.offset);
        nbg_fmt_read_data_copy(reader, dyna_ptr, fixed->hw_init_op_table.size, "hw_init_tb");
        size = sizeof(nbg_section_hw_init_operation_t);
        nbg_desc->n_hw_init_ops = fixed->hw_init_op_table.size / size;

        vipOnError(viphal_allocate_memory(sizeof(nbg_desc_hw_init_operation_t) * nbg_desc->n_hw_init_ops,
            (void **)&nbg_desc->init_ops));
        viphal_zero_memory(nbg_desc->init_ops, sizeof(nbg_desc_hw_init_operation_t) * nbg_desc->n_hw_init_ops);
        for (index = 0; index < nbg_desc->n_hw_init_ops; index++) {
            init_op = (nbg_section_hw_init_operation_t*)((vip_int8_t*)dyna_ptr + index * size);

            nbg_desc->init_ops[index].patch_count = init_op->patch_count;
            nbg_desc->init_ops[index].patch_index = init_op->patch_index;
            nbg_desc->init_ops[index].state_id = init_op->state_id;
        }
    }

    /* ICDT */
    if (fixed->ICD_table.size > 0) {
        vip_uint32_t index = 0;
        vip_uint32_t size = 0;
        nbg_section_entry_t* table = VIP_NULL;

        nbg_fmt_reader_locate(reader, fixed->ICD_table.offset);
        nbg_fmt_read_data_copy(reader, dyna_ptr, fixed->ICD_table.size, "ICDT");
        size = sizeof(nbg_section_entry_t);
        nbg_desc->n_ICDT = fixed->ICD_table.size / size;

        vipOnError(viphal_allocate_memory(sizeof(nbg_desc_data_table_t) * nbg_desc->n_ICDT,
            (void **)&nbg_desc->ICDT));
        viphal_zero_memory(nbg_desc->ICDT, sizeof(nbg_desc_data_table_t) * nbg_desc->n_ICDT);
        for (index = 0; index < nbg_desc->n_ICDT; index++) {
            table = (nbg_section_entry_t*)((vip_int8_t*)dyna_ptr + index * size);

            nbg_desc->ICDT[index].offset = table->offset;
            nbg_desc->ICDT[index].size = table->size;
        }
    }

    /* PPU param */
    if (fixed->ppu_param_table.size > 0) {
        vip_uint32_t index = 0;
        vip_uint32_t size = 0;
        nbg_section_ppu_param_t* ppu_param = VIP_NULL;

        nbg_fmt_reader_locate(reader, fixed->ppu_param_table.offset);
        nbg_fmt_read_data_copy(reader, dyna_ptr, fixed->ppu_param_table.size, "ppupara_tb");
        size = sizeof(nbg_section_ppu_param_t);
        nbg_desc->n_ppu_param = fixed->ppu_param_table.size / size;

        vipOnError(viphal_allocate_memory(sizeof(nbg_desc_ppu_param_t) * nbg_desc->n_ppu_param,
            (void **)&nbg_desc->ppu_param));
        viphal_zero_memory(nbg_desc->ppu_param, sizeof(nbg_desc_ppu_param_t) * nbg_desc->n_ppu_param);
        for (index = 0; index < nbg_desc->n_ppu_param; index++) {
            ppu_param = (nbg_section_ppu_param_t*)((vip_int8_t*)dyna_ptr + index * size);

            nbg_desc->ppu_param[index].global_offset_x = ppu_param->global_offset_x;
            nbg_desc->ppu_param[index].global_offset_y = ppu_param->global_offset_y;
            nbg_desc->ppu_param[index].global_offset_z = ppu_param->global_offset_z;
            nbg_desc->ppu_param[index].global_scale_x = ppu_param->global_scale_x;
            nbg_desc->ppu_param[index].global_scale_y = ppu_param->global_scale_y;
            nbg_desc->ppu_param[index].global_scale_z = ppu_param->global_scale_z;
            nbg_desc->ppu_param[index].group_count_x = ppu_param->group_count_x;
            nbg_desc->ppu_param[index].group_count_y = ppu_param->group_count_y;
            nbg_desc->ppu_param[index].group_count_z = ppu_param->group_count_z;
            nbg_desc->ppu_param[index].group_size_x = ppu_param->group_size_x;
            nbg_desc->ppu_param[index].group_size_y = ppu_param->group_size_y;
            nbg_desc->ppu_param[index].group_size_z = ppu_param->group_size_z;
        }
    }

    if (fixed->header.version >= 0x0001001C) {
        if (fixed->coeff_header_table.size > 0) {
            vip_uint32_t index = 0;
            vip_uint32_t size = 0;
            nbg_section_entry_t* table = VIP_NULL;

            nbg_fmt_reader_locate(reader, fixed->coeff_header_table.offset);
            nbg_fmt_read_data_copy(reader, dyna_ptr, fixed->coeff_header_table.size, "coef_heder_tb");
            size = sizeof(nbg_section_entry_t);
            nbg_desc->n_coeff_header = fixed->coeff_header_table.size / size;

            vipOnError(viphal_allocate_memory(sizeof(nbg_desc_data_table_t) * nbg_desc->n_coeff_header,
                (void **)&nbg_desc->coeff_header_table));
            viphal_zero_memory(nbg_desc->coeff_header_table,
                sizeof(nbg_desc_data_table_t) * nbg_desc->n_coeff_header);
            for (index = 0; index < nbg_desc->n_coeff_header; index++) {
                table = (nbg_section_entry_t*)((vip_int8_t*)dyna_ptr + index * size);

                nbg_desc->coeff_header_table[index].offset = table->offset;
                nbg_desc->coeff_header_table[index].size = table->size;
            }
        }
        else {
            nbg_desc->coeff_header_table = nbg_desc->coeff_data_table;
        }
        if (fixed->ppu_instr_table.size > 0) {
            vip_uint32_t index = 0;
            vip_uint32_t size = 0;
            nbg_section_entry_t* table = VIP_NULL;

            nbg_fmt_reader_locate(reader, fixed->ppu_instr_table.offset);
            nbg_fmt_read_data_copy(reader, dyna_ptr, fixed->ppu_instr_table.size, "ppu_istr_tb");
            size = sizeof(nbg_section_entry_t);
            nbg_desc->n_ppu_instr = fixed->ppu_instr_table.size / size;

            vipOnError(viphal_allocate_memory(sizeof(nbg_desc_data_table_t) * nbg_desc->n_ppu_instr,
                (void **)&nbg_desc->ppu_instr_table));
            viphal_zero_memory(nbg_desc->ppu_instr_table, sizeof(nbg_desc_data_table_t) * nbg_desc->n_ppu_instr);
            for (index = 0; index < nbg_desc->n_ppu_instr; index++) {
                table = (nbg_section_entry_t*)((vip_int8_t*)dyna_ptr + index * size);

                nbg_desc->ppu_instr_table[index].offset = table->offset;
                nbg_desc->ppu_instr_table[index].size = table->size;
            }
        }
        if (fixed->load_states_table.size > 0) {
            vip_uint32_t index = 0;
            vip_uint32_t size = 0;
            nbg_section_entry_t* table = VIP_NULL;

            nbg_fmt_reader_locate(reader, fixed->load_states_table.offset);
            nbg_fmt_read_data_copy(reader, dyna_ptr, fixed->load_states_table.size, "loadsate_tb");
            size = sizeof(nbg_section_entry_t);
            nbg_desc->n_load_state = fixed->load_states_table.size / size;

            vipOnError(viphal_allocate_memory(sizeof(nbg_desc_data_table_t) * nbg_desc->n_load_state,
                (void **)&nbg_desc->load_states_table));
            viphal_zero_memory(nbg_desc->load_states_table,
                sizeof(nbg_desc_data_table_t) * nbg_desc->n_load_state);
            for (index = 0; index < nbg_desc->n_load_state; index++) {
                table = (nbg_section_entry_t*)((vip_int8_t*)dyna_ptr + index * size);

                nbg_desc->load_states_table[index].offset = table->offset;
                nbg_desc->load_states_table[index].size = table->size;
            }
        }
        if (fixed->dyna_io_table.size > 0) {
            vip_uint32_t index = 0;
            vip_uint32_t size = 0;
            nbg_section_entry_t* table = VIP_NULL;

            nbg_fmt_reader_locate(reader, fixed->dyna_io_table.offset);
            nbg_fmt_read_data_copy(reader, dyna_ptr, fixed->dyna_io_table.size, "dyna_io_tb");
            size = sizeof(nbg_section_entry_t);
            nbg_desc->n_dyna_io = fixed->dyna_io_table.size / size;

            vipOnError(viphal_allocate_memory(sizeof(nbg_desc_data_table_t) * nbg_desc->n_dyna_io,
                (void **)&nbg_desc->dyna_io_table));
            viphal_zero_memory(nbg_desc->dyna_io_table, sizeof(nbg_desc_data_table_t) * nbg_desc->n_dyna_io);
            for (index = 0; index < nbg_desc->n_dyna_io; index++) {
                table = (nbg_section_entry_t*)((vip_int8_t*)dyna_ptr + index * size);

                nbg_desc->dyna_io_table[index].offset = table->offset;
                nbg_desc->dyna_io_table[index].size = table->size;
            }
        }
    }
    else {
        nbg_desc->coeff_header_table = nbg_desc->coeff_data_table;
        nbg_desc->ppu_instr_table = nbg_desc->coeff_data_table;
        nbg_desc->dyna_io_table = nbg_desc->coeff_data_table;
        nbg_desc->load_states_table = nbg_desc->coeff_data_table;
    }

    /* layer dump */
    if (fixed->header.version >= 0x00010020) {
        vip_uint32_t index = 0;
        vip_uint32_t sub_index = 0;
        vip_uint32_t size = 0;
        if (fixed->ldump_operation.size > 0) {
            nbg_ldump_operation_t* ldump_operation = VIP_NULL;
            nbg_fmt_reader_locate(reader, fixed->ldump_operation.offset);
            nbg_fmt_read_data_copy(reader, dyna_ptr, fixed->ldump_operation.size, "ldump_operation");
            size = sizeof(nbg_ldump_operation_t);
            nbg_desc->n_ldump_operation = fixed->ldump_operation.size / size;
            vipOnError(viphal_allocate_memory(sizeof(nbg_desc_ldump_operation_t) * nbg_desc->n_ldump_operation,
                (void **)&nbg_desc->ldump_operation));
            viphal_zero_memory(nbg_desc->ldump_operation,
                sizeof(nbg_desc_ldump_operation_t) * nbg_desc->n_ldump_operation);

            for (index = 0; index < nbg_desc->n_ldump_operation; index++) {
                ldump_operation = (nbg_ldump_operation_t *)((vip_int8_t *)dyna_ptr + index * size);
                nbg_desc->ldump_operation[index].insert_index = ldump_operation->insert_index;
                nbg_desc->ldump_operation[index].loadstate_index = ldump_operation->loadstate_index;
                nbg_desc->ldump_operation[index].start_patch_index = ldump_operation->start_patch_index;
                nbg_desc->ldump_operation[index].patch_count = ldump_operation->patch_count;
                nbg_desc->ldump_operation[index].core_id = ldump_operation->core_id;
                nbg_desc->ldump_operation[index].wait_count = ldump_operation->wait_count;
            }
        }
        if (fixed->ldump_outputs.size > 0) {
            nbg_ldump_output_t* ldump_outputs = VIP_NULL;
            nbg_fmt_reader_locate(reader, fixed->ldump_outputs.offset);
            nbg_fmt_read_data_copy(reader, dyna_ptr, fixed->ldump_outputs.size, "ldump_outputs");
            size = sizeof(nbg_ldump_output_t);
            nbg_desc->n_ldump_outputs = fixed->ldump_outputs.size / size;
            vipOnError(viphal_allocate_memory(sizeof(nbg_desc_ldump_output_t) * nbg_desc->n_ldump_outputs,
                (void **)&nbg_desc->ldump_outputs));
            viphal_zero_memory(nbg_desc->ldump_outputs, sizeof(nbg_desc_ldump_output_t) * nbg_desc->n_ldump_outputs);

            for (index = 0; index < nbg_desc->n_ldump_outputs; index++) {
                ldump_outputs = (nbg_ldump_output_t *)((vip_int8_t *)dyna_ptr + index * size);
                nbg_desc->ldump_outputs[index].info.dim_count = ldump_outputs->info.dim_count;
                nbg_desc->ldump_outputs[index].info.data_format = ldump_outputs->info.data_format;
                nbg_desc->ldump_outputs[index].info.data_type = ldump_outputs->info.data_type;
                nbg_desc->ldump_outputs[index].info.quant_format = ldump_outputs->info.quan_format;
                if (VIP_BUFFER_QUANTIZE_DYNAMIC_FIXED_POINT == ldump_outputs->info.quan_format) {
                    nbg_desc->ldump_outputs[index].info.quant_data.dfp.fixed_point_pos = ldump_outputs->info.fixed_pos;
                }
                else {
                    nbg_desc->ldump_outputs[index].info.quant_data.affine.tf_scale = ldump_outputs->info.tf_scale;
                    nbg_desc->ldump_outputs[index].info.quant_data.affine.tf_zero_point = ldump_outputs->info.tf_zerop;
                }
                viphal_copy_memory(nbg_desc->ldump_outputs[index].info.name, ldump_outputs->info.name,
                    sizeof(vip_char_t)* MAX_LAYER_PARAM_NAME_LENGTH);
                for (sub_index = 0; sub_index < ldump_outputs->info.dim_count; sub_index++) {
                    nbg_desc->ldump_outputs[index].info.dim_size[sub_index] = ldump_outputs->info.dim_size[sub_index];
                }
            }
        }
        if (fixed->ldump_patch_data.size > 0) {
            nbg_section_patch_table_t* ldump_patch_data = VIP_NULL;
            nbg_fmt_reader_locate(reader, fixed->ldump_patch_data.offset);
            nbg_fmt_read_data_copy(reader, dyna_ptr, fixed->ldump_patch_data.size, "ldump_patch_data");
            size = sizeof(nbg_section_patch_table_t);
            nbg_desc->n_ldump_patch_data = fixed->ldump_patch_data.size / size;
            vipOnError(viphal_allocate_memory(sizeof(nbg_desc_patch_t) * nbg_desc->n_ldump_patch_data,
                (void **)&nbg_desc->ldump_patch_data));
            viphal_zero_memory(nbg_desc->ldump_patch_data, sizeof(nbg_desc_patch_t) * nbg_desc->n_ldump_patch_data);

            for (index = 0; index < nbg_desc->n_ldump_patch_data; index++) {
                ldump_patch_data = (nbg_section_patch_table_t*)((vip_int8_t*)dyna_ptr + index * size);
                nbg_desc->ldump_patch_data[index].dest_offset = ldump_patch_data->dest_offset;
                nbg_desc->ldump_patch_data[index].index = ldump_patch_data->index;
                nbg_desc->ldump_patch_data[index].source_type = ldump_patch_data->source_type;
                nbg_desc->ldump_patch_data[index].src_offset = ldump_patch_data->src_offset;
                nbg_desc->ldump_patch_data[index].transformed = ldump_patch_data->transformed;
                nbg_desc->ldump_patch_data[index].type = ldump_patch_data->type;
                nbg_desc->ldump_patch_data[index].name = ldump_patch_data->name;
            }
        }
        if (fixed->ldump_instr_table.size > 0) {
            nbg_section_entry_t* ldump_instr_table = VIP_NULL;
            nbg_fmt_reader_locate(reader, fixed->ldump_instr_table.offset);
            nbg_fmt_read_data_copy(reader, dyna_ptr, fixed->ldump_instr_table.size, "ldump_instr_table");
            size = sizeof(nbg_section_entry_t);
            nbg_desc->n_ldump_instr_table = fixed->ldump_instr_table.size / size;
            vipOnError(viphal_allocate_memory(sizeof(nbg_desc_data_table_t) * nbg_desc->n_ldump_instr_table,
                (void **)&nbg_desc->ldump_instr_table));
            viphal_zero_memory(nbg_desc->ldump_instr_table,
                sizeof(nbg_desc_data_table_t) * nbg_desc->n_ldump_instr_table);

            for (index = 0; index < nbg_desc->n_ldump_instr_table; index++) {
                ldump_instr_table = (nbg_section_entry_t *)((vip_int8_t *)dyna_ptr + index * size);
                nbg_desc->ldump_instr_table[index].offset = ldump_instr_table->offset;
                nbg_desc->ldump_instr_table[index].size = ldump_instr_table->size;
            }
        }
        if (fixed->ldump_load_states_table.size > 0) {
            nbg_section_entry_t* ldump_load_states_table = VIP_NULL;
            nbg_fmt_reader_locate(reader, fixed->ldump_load_states_table.offset);
            nbg_fmt_read_data_copy(reader, dyna_ptr, fixed->ldump_load_states_table.size, "ldump_load_states_table");
            size = sizeof(nbg_section_entry_t);
            nbg_desc->n_ldump_load_states_table = fixed->ldump_load_states_table.size / size;
            vipOnError(viphal_allocate_memory(sizeof(nbg_desc_data_table_t) * nbg_desc->n_ldump_load_states_table,
                (void **)&nbg_desc->ldump_load_states_table));
            viphal_zero_memory(nbg_desc->ldump_load_states_table,
                sizeof(nbg_desc_data_table_t) * nbg_desc->n_ldump_load_states_table);

            for (index = 0; index < nbg_desc->n_ldump_load_states_table; index++) {
                ldump_load_states_table = (nbg_section_entry_t *)((vip_int8_t *)dyna_ptr + index * size);
                nbg_desc->ldump_load_states_table[index].offset = ldump_load_states_table->offset;
                nbg_desc->ldump_load_states_table[index].size = ldump_load_states_table->size;
            }
        }
        if (fixed->ldump_instr.size > 0) {
            nbg_fmt_reader_locate(reader, fixed->ldump_instr.offset);
            nbg_fmt_read_data_copy(reader, dyna_ptr, fixed->ldump_instr.size, "ldump_instr");
            size = fixed->ldump_instr.size;
            vipOnError(viphal_allocate_memory(size, (void **)&nbg_desc->ldump_instr_logical));
            viphal_zero_memory(nbg_desc->ldump_instr_logical, size);
            viphal_copy_memory(nbg_desc->ldump_instr_logical, dyna_ptr, size);
        }
        if (fixed->ldump_load_states.size > 0) {
            nbg_fmt_reader_locate(reader, fixed->ldump_load_states.offset);
            nbg_fmt_read_data_copy(reader, dyna_ptr, fixed->ldump_load_states.size, "ldump_load_states");
            size = fixed->ldump_load_states.size;
            vipOnError(viphal_allocate_memory(size, (void **)&nbg_desc->ldump_load_states_logical));
            viphal_zero_memory(nbg_desc->ldump_load_states_logical, size);
            viphal_copy_memory(nbg_desc->ldump_load_states_logical, dyna_ptr, size);
        }
    }

onError:
    if (VIP_NULL != dyna_ptr) {
        viphal_free_memory(dyna_ptr);
        dyna_ptr = VIP_NULL;
    }
    if (VIP_SUCCESS != status) {
        nbg_desc->corrupted = vip_true_e;
        vip_error("fail to read nbg file dynamic section\n");
    }
    return status;
}

vip_status_e nbg_fmt_read_v1(
    nbg_fmt_reader_t *reader,
    nbg_fmt_descriptor_t *nbg_desc
    )
{
    vip_status_e status = VIP_SUCCESS;
    nbg_fixed_header_t fixed;
    viphal_zero_memory(&fixed, sizeof(nbg_fixed_header_t));

    vipOnError(nbg_fmt_read_fixed(reader, &fixed, nbg_desc));
    vipOnError(nbg_fmt_read_dynamic(reader, &fixed, nbg_desc));
onError:
    return status;
}
