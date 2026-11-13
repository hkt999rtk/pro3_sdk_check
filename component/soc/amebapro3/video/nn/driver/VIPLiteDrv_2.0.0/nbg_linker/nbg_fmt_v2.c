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
#include <nbg_fmt_v2.h>
#include <nbg_fmt_util.h>
#include <vip_lite.h>

#define CHECK_SECTION_DATA_STRUCT_SIZE(size, type, name)  \
{   \
    vip_uint32_t tmp = sizeof(type);    \
     if (size != tmp) { \
        vip_error("fail %s section data entry size not match, %d, struct size=%d\n", name, size, tmp);  \
        return VIP_ERROR_FAILURE; \
     }  \
}

#if (vpmdENABLE_DEBUG_LOG > 2)
nbg_section_info_s section_info[NBG_SECTION_MAX+1] = {
    /* index,                          index_name,                             section_name,           version */
    {NBG_SECTION_NONE,                 "SECTION_NONE",                     "none",                     0x00010000},

    {NBG_SECTION_NETWORK_INFO,         "SECTION_NETWORK_INFO",             "network_info",             0x00010000},
    {NBG_SECTION_CUSTOMER_DATA,        "SECTION_CUSTOMER_DATA",            "customer",                 0x00010000},
    {NBG_SECTION_MEMORY_POOL,          "SECTION_MEMORY_POOL",              "memory_pool",              0x00010000},
    {NBG_SECTION_VIP_SRAM,             "SECTION_VIP_SRAM",                 "vip_sram",                 0x00010000},
    {NBG_SECTION_AXI_SRAM,             "SECTION_AXI_SRAM",                 "axi_sram",                 0x00010000},

    {NBG_SECTION_INPUT,                "SECTION_INPUT",                    "input_info",               0x00010000},
    {NBG_SECTION_OUTPUT,               "SECTION_OUTPUT",                   "output_info",              0x00010000},
    {NBG_SECTION_LAYER,                "SECTION_LAYER",                    "layer_info",               0x00010000},
    {NBG_SECTION_OPERATIONS,           "SECTION_OPERATIONS",               "operation_info",           0x00010000},
    {NBG_SECTION_NN,                   "SECTION_NN",                       "nn_instr",                 0x00010000},
    {NBG_SECTION_TP,                   "SECTION_TP",                       "tp_instr",                 0x00010000},
    {NBG_SECTION_SH,                   "SECTION_SH",                       "ppu_operation",            0x00010000},
    {NBG_SECTION_PATCH,                "SECTION_PATCH",                    "patch_data",               0x00010000},
    {NBG_SECTION_LAYER_PARAM,          "SECTION_LAYER_PARAM",              "layer_parameter",          0x00010000},
    {NBG_SECTION_SW,                   "SECTION_SW",                       "sw_operation",             0x00010000},
    {NBG_SECTION_HWINITOPERATIONS,     "SECTION_HWINITOPERATIONS",         "hw_init_operation",        0x00010000},
    {NBG_SECTION_ICDT,                 "SECTION_ICDT",                     "init_data_table",          0x00010000},
    {NBG_SECTION_ICD,                  "SECTION_ICD",                      "init_data",                0x00010000},
    {NBG_SECTION_PPU_PARAM,            "SECTION_PPU_PARAM",                "ppu_parameters_data",      0x00010000},
    {NBG_SECTION_OPSYNC,               "SECTION_OPSYNC",                   "op_sync_info",             0x00010000},
    {NBG_SECTION_DMA_SBI_INFO,         "SECTION_DMA_SBI_INFO",             "dma_sbi_info",             0x00010000},
    {NBG_SECTION_DYNAIO,               "SECTION_DYNAIO",                   "dyna_io",                  0x00010000},
    {NBG_SECTION_DMA_SBI_ADDITION,     "SECTION_DMA_SBI_ADDITION",         "dma_sbi_addition",         0x00010000},
    {NBG_SECTION_LAYER_DUMP_OPERATION, "SECTION_LAYER_DUMP_OPERATION",     "layer_dump_operation",     0x00010000},
    {NBG_SECTION_LAYER_DUMP_OUTPUTS,   "SECTION_LAYER_DUMP_OUTPUTS",       "layer_dump_outputs",       0x00010000},
    {NBG_SECTION_LAYER_DUMP_PATCH_DATA, "SECTION_LAYER_DUMP_PATCH_DATA",   "layer_dump_patch_data",    0x00010000},
    {NBG_SECTION_COEF_HEADER_TABLE,    "SECTION_COEF_HEADER_TABLE",        "coef_header_table",        0x00010000},
    {NBG_SECTION_COEF_DATA_TABLE,      "SECTION_COEF_DATA_TABLE",          "coef_data_table",          0x00010000},
    {NBG_SECTION_PPU_INSTR_TABLE,      "SECTION_PPU_INSTR_TABLE",          "ppu_instr_table",          0x00010000},
    {NBG_SECTION_LOADSTATE_TABLE,      "SECTION_LOADSTATE_TABLE",          "load_state_table",         0x00010000},
    {NBG_SECTION_DYNAIO_TABLE,         "SECTION_DYNAIO_TABLE",             "dyna_io_table",            0x00010000},
    {NBG_SECTION_LAYER_DUMP_INSTR_TABLE, "SECTION_LAYER_DUMP_INSTR_TABLE", "layer_dump_instr_table",   0x00010000},
    {NBG_SECTION_LAYER_DUMP_LOADSTATE_TABLE, "SECTION_LAYER_DUMP_LOADSTATE_TABLE", "layer_dump_loadstate_table",
        0x00010000},

    {NBG_SECTION_COEF_HEADER,          "SECTION_COEF_HEADER",              "coef_header",              0x00010000},
    {NBG_SECTION_COEF_DATA,            "SECTION_COEF_DATA",                "coef_data",                0x00010000},
    {NBG_SECTION_PPU_INSTR,            "SECTION_PPU_INSTR",                "ppu_instr",                0x00010000},
    {NBG_SECTION_LOADSTATE,            "SECTION_LOADSTATE",                "load_state",               0x00010000},
    {NBG_SECTION_LAYER_DUMP_INSTR,     "SECTION_LAYER_DUMP_INSTR",         "layer_dump_instr",         0x00010000},
    {NBG_SECTION_LAYER_DUMP_LOADSTATE, "SECTION_LAYER_DUMP_LOADSTATE",     "layer_dump_loadstate",     0x00010000},
    {NBG_SECTION_CHECKSUM,             "SECTION_CHECKSUM",                 "checksum",                 0x00010000},

    {NBG_SECTION_MAX,                  "SECTION_MAX",                      "max",                      0x00000000},
};

static vip_char_t *nbg_fmt_get_section_name(
    vip_uint32_t index
)
{
    vip_uint32_t i = 0;
    if (index <= NBG_SECTION_NONE || index >= NBG_SECTION_MAX) {
        vip_error("not support index[%d]\n", index);
        return VIP_NULL;
    }
    for (i = 0; i < (sizeof(section_info) / sizeof(section_info[0])); i++) {
        nbg_section_info_s *info = &section_info[i];
        if (info->index == index) {
            return info->section_name;
        }
    }

    return VIP_NULL;
}

static vip_char_t *nbg_fmt_get_section_type_name(
    vip_uint32_t index
)
{
    vip_uint32_t i = 0;
    if (index <= NBG_SECTION_NONE || index >= NBG_SECTION_MAX) {
        vip_error("not support index[%d]\n", index);
        return VIP_NULL;
    }
    for (i = 0; i < (sizeof(section_info) / sizeof(section_info[0])); i++) {
        nbg_section_info_s *info = &section_info[i];
        if (info->index == index) {
            return info->type_name;
        }
    }

    return VIP_NULL;
}

static vip_uint64_t nbg_fmt_get_sd_addr(
    nbg_section_header_t *section_header
)
{
    return CONSTRUCT_U64(section_header->sd_addr_high, section_header->sd_addr_low);
}
#endif

static vip_uint64_t nbg_fmt_get_sd_offset(
    nbg_section_header_t *section_header
)
{
    return CONSTRUCT_U64(section_header->sd_offset_high, section_header->sd_offset_low);
}

static vip_uint64_t nbg_fmt_get_sd_size(
    nbg_section_header_t *section_header
)
{
    return CONSTRUCT_U64(section_header->sd_size_high, section_header->sd_size_low);
}

#if (vpmdENABLE_DEBUG_LOG > 2)
static vip_status_e nbg_show_format_header(
    nbg_format_header_t *header
    )
{
    vip_status_e status = VIP_SUCCESS;
    vip_print("header:\n");
    vip_print("magic: %s\n", header->magic);
    vip_print("version: %08x\n", header->version);
    vip_print("target: %08x\n", header->target);
    vip_print("crc_sum: 0x%08x\n", header->crc_sum);
    vip_print("total_size: 0x%llx\n", CONSTRUCT_U64(header->total_size_high, header->total_size_low));
    vip_print("sub_header_offset: 0x%x\n", header->sub_header_offset);
    vip_print("sub_header_size: 0x%x\n", header->sub_header_size);
    vip_print("sub_header_count: %d\n", header->sub_header_count);
    vip_print("class: %sbit\n", header->class ? "64" : "32");

    return status;
}

static vip_status_e nbg_show_sub_header(
    nbg_sub_header_t *sub_header
    )
{
    vip_status_e status = VIP_SUCCESS;
    vip_print("section header:\n");
    vip_print("sh_offset: 0x%x\n", sub_header->sh_offset);
    vip_print("sh_size: 0x%x\n", sub_header->sh_size);
    vip_print("sh_count: %d\n", sub_header->sh_count);

    return status;
}

static vip_status_e nbg_show_section_header(
    nbg_section_header_t *section_header,
    vip_uint32_t sh_count
    )
{
    vip_status_e status = VIP_SUCCESS;
    vip_uint32_t i = 0;

    vip_print("section header:\n");
    vip_print("%-10s%-10s%-30s%-30s%-20s%-20s%-20s%-15s%-15s\n",
        "[nr]", "sd_type", "sd_type_name", "section_name", "sd_offset", "sd_size",
        "sd_addr", "sd_alignsize", "sd_entsize");
    for (i = 0; i < sh_count; i++) {
        vip_print("%-10d%-10d%-30s%-30s0x%-18x0x%-18x0x%-18x0x%-13x0x%-13x\n",
            i, section_header->sd_type, nbg_fmt_get_section_type_name(section_header->sd_type),
            nbg_fmt_get_section_name(section_header->sd_type),
            nbg_fmt_get_sd_offset(section_header), nbg_fmt_get_sd_size(section_header),
            nbg_fmt_get_sd_addr(section_header), section_header->sd_alignsize, section_header->sd_entsize);
        section_header += 1;
    }

    return status;
}

static vip_status_e nbg_show_header(
    nbg_header_t *nbg_header
    )
{
    vip_status_e status = VIP_SUCCESS;

    nbg_show_format_header(&nbg_header->header);
    nbg_show_sub_header(&nbg_header->sub_header);
    nbg_show_section_header(nbg_header->section_header, nbg_header->sub_header.sh_count);

    return status;
}
#endif

static vip_status_e nbg_fmt_read_format_header(
    nbg_fmt_reader_t *reader,
    nbg_header_t *nbg_header,
    nbg_fmt_descriptor_t *nbg_desc
    )
{
    vip_status_e status = VIP_SUCCESS;
    vip_uint32_t i = 0;
    nbg_format_header_t *header = VIP_NULL;
    vip_uint32_t tmp = 0;
    static const char magic[4] = {'V', 'P', 'M', 'N'};

    header = &nbg_header->header;
    nbg_fmt_reader_rewind(reader);   /* Go to the beginning. */
    /* Read magic. */
    for (i = 0; i < 4; i++) {
        header->magic[i] = nbg_fmt_read_byte(reader, 1);
        if (header->magic[i] != magic[i]) {
            vip_error("fail nbg magic not match, nbg is bad\n");
            vipGoOnError(VIP_ERROR_INVALID_ARGUMENTS);
        }
    }

    header->version = nbg_fmt_read_uInt(reader, 1);
    header->target = nbg_fmt_read_uInt(reader, 1);
    header->crc_sum = nbg_fmt_read_uInt(reader, 1);
    header->total_size_low = nbg_fmt_read_uInt(reader, 1);
    header->total_size_high = nbg_fmt_read_uInt(reader, 1);
    header->sub_header_offset = nbg_fmt_read_uInt(reader, 1);
    header->sub_header_size = nbg_fmt_read_uInt(reader, 1);
    header->sub_header_count = nbg_fmt_read_uInt(reader, 1);

    if (header->sub_header_count > 1) {
        vip_error("fail not support sub header count is %d\n", header->sub_header_count);
        vipGoOnError(VIP_ERROR_FAILURE);
    }

    tmp = nbg_fmt_read_uInt(reader, 1);
    header->class = tmp & 0x1;

    /* read reserve */
    nbg_fmt_read_uInt(reader, 1);
    nbg_fmt_read_uInt(reader, 1);

onError:
    return status;
}

static vip_status_e nbg_fmt_read_sub_header(
    nbg_fmt_reader_t *reader,
    nbg_header_t *nbg_header,
    nbg_fmt_descriptor_t *nbg_desc
    )
{
    vip_status_e status = VIP_SUCCESS;
    nbg_format_header_t *header = &nbg_header->header;
    nbg_sub_header_t *sub_header = &nbg_header->sub_header;
    vip_uint32_t offset = 0;

    /* sanity check for sub header */
    if ((sizeof(nbg_sub_header_t) * header->sub_header_count) != header->sub_header_size) {
        vip_error("fail to check sub-header count=%d, size=%d\n",
                header->sub_header_count, header->sub_header_size);
        vipGoOnError(VIP_ERROR_FAILURE);
    }

    offset = header->sub_header_offset;
    nbg_fmt_reader_locate(reader, offset);
    sub_header->sh_offset = nbg_fmt_read_uInt(reader, 1);
    sub_header->sh_size = nbg_fmt_read_uInt(reader, 1);
    sub_header->sh_count = nbg_fmt_read_uInt(reader, 1);

onError:
    return status;
}

static vip_status_e nbg_fmt_read_section_header(
    nbg_fmt_reader_t *reader,
    nbg_header_t *nbg_header,
    nbg_fmt_descriptor_t *nbg_desc
    )
{
    vip_status_e status = VIP_SUCCESS;

    nbg_sub_header_t *sub_header = &nbg_header->sub_header;
    vip_uint32_t sh_count = 0;
    vip_uint32_t sh_size = 0;
    vip_uint32_t sh_offset = 0;
    vip_uint32_t sh_total_size = 0;

    sh_count = sub_header->sh_count;
    sh_size = sub_header->sh_size;
    sh_offset = sub_header->sh_offset;
    sh_total_size = sh_size * sh_count;

    /* sanity check for section header */
    if (sh_size != sizeof(nbg_section_header_t)) {
        vip_error("fail sh_size=%d not match with header struct\n", sh_size);
        vipGoOnError(VIP_ERROR_NOT_SUPPORTED);
    }

    if (0 == sh_count) {
        vip_error("fail section header count could not be zero\n");
        vipGoOnError(VIP_ERROR_INVALID_ARGUMENTS);
    }

    vipOnError(viphal_allocate_memory(sh_total_size, (void **)&nbg_header->section_header));

    vipOnError(nbg_fmt_reader_locate(reader, sh_offset));
    vipOnError(nbg_fmt_read_data_copy(reader, nbg_header->section_header, sh_total_size, "section_header"));
    vipOnError(nbg_fmt_reader_forward(reader, sh_total_size));

onError:
    return status;
}

vip_status_e nbg_fmt_read_header(
    nbg_fmt_reader_t *reader,
    nbg_header_t *nbg_header,
    nbg_fmt_descriptor_t *nbg_desc
    )
{
    vip_status_e status = VIP_SUCCESS;

    vipOnError(nbg_fmt_read_format_header(reader, nbg_header, nbg_desc));
    vipOnError(nbg_fmt_read_sub_header(reader, nbg_header, nbg_desc));
    vipOnError(nbg_fmt_read_section_header(reader, nbg_header, nbg_desc));

#if (vpmdENABLE_DEBUG_LOG > 2)
    nbg_show_header(nbg_header);
#endif

onError:
    return status;
}

static vip_uint64_t nbg_fmt_get_non_video_sd_max_size(
    nbg_header_t *nbg_header,
    nbg_fmt_descriptor_t *nbg_desc
    )
{
    vip_uint32_t video_sd_count = 0;
    vip_uint32_t sh_count = 0;
    vip_uint32_t i = 0;
    vip_uint32_t j = 0;
    nbg_sub_header_t *sub_header = VIP_NULL;
    vip_uint64_t max_size = 0;

    vip_uint32_t video_sd_index[] = {
        NBG_SECTION_DYNAIO,
        NBG_SECTION_COEF_HEADER,
        NBG_SECTION_COEF_DATA,
        NBG_SECTION_LOADSTATE,
        NBG_SECTION_NN,
        NBG_SECTION_TP,
        NBG_SECTION_PPU_INSTR,
        NBG_SECTION_LAYER_DUMP_INSTR,
        NBG_SECTION_LAYER_DUMP_LOADSTATE
    };

    video_sd_count = sizeof(video_sd_index) / sizeof(video_sd_index[0]);
    sub_header = &nbg_header->sub_header;

    #define nbg_fma_max(x, y) (((x) >= (y)) ? (x) : (y))
    sh_count = sub_header->sh_count;
    for (i = 0; i < sh_count; i++) {
        vip_uint32_t sd_type = 0;
        vip_uint64_t sd_size = 0;
        nbg_section_header_t *section_header = VIP_NULL;
        vip_bool_e find = vip_false_e;

        section_header = nbg_header->section_header + i;
        sd_type = section_header->sd_type;
        sd_size = nbg_fmt_get_sd_size(section_header);
        for (j = 0; j < video_sd_count; j++) {
            if (video_sd_index[j] == sd_type) {
                find = vip_true_e;
            }
        }
        if (find) {
            continue;
        }

        max_size = nbg_fma_max(max_size, sd_size);
    }
    #undef nbg_fma_max

    return max_size;
}

static vip_uint32_t nbg_fmt_sd_count(
    nbg_section_header_t *section_header
    )
{
    return SIZE_CAST(vip_uint32_t, nbg_fmt_get_sd_size(section_header) / section_header->sd_entsize);
}

static vip_status_e nbg_fmt_read_section_network_info(
    nbg_fmt_reader_t *reader,
    nbg_header_t *nbg_header,
    nbg_fmt_descriptor_t *nbg_desc,
    nbg_section_header_t *section_header,
    vip_uint8_t *dyna_ptr
    )
{
    vip_status_e status = VIP_SUCCESS;
    vip_size_t sd_size = 0;
    vip_size_t sd_offset = 0;
    nbg_section_network_info_t *network_info = VIP_NULL;

    sd_size = SIZE_CAST(vip_size_t, nbg_fmt_get_sd_size(section_header));
    sd_offset = SIZE_CAST(vip_size_t, nbg_fmt_get_sd_offset(section_header));

    nbg_fmt_reader_locate(reader, sd_offset);
    nbg_fmt_read_data_copy(reader, dyna_ptr, sd_size, "network_info");

    network_info = (nbg_section_network_info_t *)dyna_ptr;

    viphal_copy_memory(nbg_desc->net_info.name, network_info->name, sizeof(vip_char_t)*NETWORK_NAME_SIZE);
    nbg_desc->net_info.core_count = network_info->core_count;
    nbg_desc->net_info.core_count = network_info->core_count;
    nbg_desc->net_info.attribute.change_ppu_param = network_info->attribute.change_ppu_param;
    nbg_desc->net_info.attribute.nn_command_size = network_info->attribute.nn_command_size;
    nbg_desc->net_info.attribute.tp_command_size = network_info->attribute.tp_command_size;
    nbg_desc->net_info.address_width = network_info->address_width;

    nbg_desc->fixed.header.version = nbg_header->header.version;
    nbg_desc->fixed.header.class = nbg_header->header.class;
    nbg_desc->fixed.header.nbg_crc_sum = nbg_header->header.crc_sum;
    nbg_desc->fixed.header.hw_target = nbg_header->header.target;

    return status;
}

static vip_status_e nbg_fmt_read_section_memory_pool(
    nbg_fmt_reader_t *reader,
    nbg_header_t *nbg_header,
    nbg_fmt_descriptor_t *nbg_desc,
    nbg_section_header_t *section_header,
    vip_uint8_t *dyna_ptr
    )
{
    vip_status_e status = VIP_SUCCESS;
    vip_size_t sd_size = 0;
    vip_size_t sd_offset = 0;
    nbg_section_memory_pool_t *memory_pool = VIP_NULL;

    sd_size = SIZE_CAST(vip_size_t, nbg_fmt_get_sd_size(section_header));
    sd_offset = SIZE_CAST(vip_size_t, nbg_fmt_get_sd_offset(section_header));

    nbg_fmt_reader_locate(reader, sd_offset);
    nbg_fmt_read_data_copy(reader, dyna_ptr, sd_size, "memory_pool");

    memory_pool = (nbg_section_memory_pool_t *)(dyna_ptr);
    nbg_desc->mem_info.pool_size =
        SIZE_CAST(vip_size_t, CONSTRUCT_U64(memory_pool->used_size_high, memory_pool->used_size_low));

    return status;
}

static vip_status_e nbg_fmt_read_section_vip_sram(
    nbg_fmt_reader_t *reader,
    nbg_header_t *nbg_header,
    nbg_fmt_descriptor_t *nbg_desc,
    nbg_section_header_t *section_header,
    vip_uint8_t *dyna_ptr
    )
{
    vip_status_e status = VIP_SUCCESS;
    vip_size_t sd_size = 0;
    vip_size_t sd_offset = 0;
    nbg_section_vip_sram_t *vip_sram = VIP_NULL;

    sd_size = SIZE_CAST(vip_size_t, nbg_fmt_get_sd_size(section_header));
    sd_offset = SIZE_CAST(vip_size_t, nbg_fmt_get_sd_offset(section_header));

    nbg_fmt_reader_locate(reader, sd_offset);
    nbg_fmt_read_data_copy(reader, dyna_ptr, sd_size, "vip_sram");

    vip_sram = (nbg_section_vip_sram_t *)(dyna_ptr);
    nbg_desc->mem_info.vip_sram_size = (vip_size_t)vip_sram->used_size;

    return status;
}

static vip_status_e nbg_fmt_read_section_axi_sram(
    nbg_fmt_reader_t *reader,
    nbg_header_t *nbg_header,
    nbg_fmt_descriptor_t *nbg_desc,
    nbg_section_header_t *section_header,
    vip_uint8_t *dyna_ptr
    )
{
    vip_status_e status = VIP_SUCCESS;
    vip_size_t sd_size = 0;
    vip_size_t sd_offset = 0;
    nbg_section_axi_sram_t *axi_sram = VIP_NULL;

    sd_size = SIZE_CAST(vip_size_t, nbg_fmt_get_sd_size(section_header));
    sd_offset = SIZE_CAST(vip_size_t, nbg_fmt_get_sd_offset(section_header));

    nbg_fmt_reader_locate(reader, sd_offset);
    nbg_fmt_read_data_copy(reader, dyna_ptr, sd_size, "axi_sram");

    axi_sram = (nbg_section_axi_sram_t *)(dyna_ptr);
    nbg_desc->mem_info.axi_sram_size = (vip_size_t)axi_sram->used_size;

    return status;
}

static vip_status_e nbg_fmt_read_basic_section(
    nbg_fmt_reader_t *reader,
    nbg_header_t *nbg_header,
    nbg_fmt_descriptor_t *nbg_desc
    )
{
    vip_status_e status = VIP_SUCCESS;

    nbg_sub_header_t *sub_header = VIP_NULL;
    vip_uint32_t sh_count = 0;
    vip_uint32_t i = 0;
    vip_uint64_t dyna_size = 0;
    vip_uint8_t *dyna_ptr = VIP_NULL;

    sub_header = &nbg_header->sub_header;
    sh_count = sub_header->sh_count;

    dyna_size = nbg_fmt_get_non_video_sd_max_size(nbg_header, nbg_desc);
    vipOnError(viphal_allocate_memory(SIZE_CAST(vip_uint32_t, dyna_size), (void **)&dyna_ptr));

    for (i = 0; i < sh_count; i++) {
        vip_uint32_t sd_type = NBG_SECTION_NONE;
        nbg_section_header_t *section_header = VIP_NULL;

        section_header = nbg_header->section_header + i;
        if (!section_header) {
            vip_error("section_header could not be null\n");
            vipGoOnError(VIP_ERROR_INVALID_ARGUMENTS);
        }
        sd_type = section_header->sd_type;
        switch (sd_type) {
            case NBG_SECTION_NETWORK_INFO:
                vipOnError(nbg_fmt_read_section_network_info(reader, nbg_header, nbg_desc, section_header, dyna_ptr));
            break;
            case NBG_SECTION_MEMORY_POOL:
                vipOnError(nbg_fmt_read_section_memory_pool(reader, nbg_header, nbg_desc, section_header, dyna_ptr));
            break;
            case NBG_SECTION_VIP_SRAM:
                vipOnError(nbg_fmt_read_section_vip_sram(reader, nbg_header, nbg_desc, section_header, dyna_ptr));
            break;
            case NBG_SECTION_AXI_SRAM:
                vipOnError(nbg_fmt_read_section_axi_sram(reader, nbg_header, nbg_desc, section_header, dyna_ptr));
            break;
            default:
            break;
        }
    }

onError:
    if (VIP_NULL != dyna_ptr) {
        viphal_free_memory(dyna_ptr);
        dyna_ptr = VIP_NULL;
    }
    return status;
}

static vip_status_e nbg_fmt_read_section_inputs(
    nbg_fmt_reader_t *reader,
    nbg_header_t *nbg_header,
    nbg_fmt_descriptor_t *nbg_desc,
    nbg_section_header_t *section_header,
    vip_uint8_t *dyna_ptr
    )
{
    vip_status_e status = VIP_SUCCESS;

    vip_size_t sd_size = 0;
    vip_size_t sd_offset = 0;
    vip_uint32_t sd_entsize = 0;
    vip_uint32_t index = 0;
    nbg_section_input_output_t *input = VIP_NULL;
    vip_uint32_t sub_index = 0;

    sd_size = SIZE_CAST(vip_size_t, nbg_fmt_get_sd_size(section_header));
    sd_offset = SIZE_CAST(vip_size_t, nbg_fmt_get_sd_offset(section_header));
    sd_entsize = section_header->sd_entsize;

    CHECK_SECTION_DATA_STRUCT_SIZE(sd_entsize, nbg_section_input_output_t, "input");

    nbg_fmt_reader_locate(reader, sd_offset);
    nbg_fmt_read_data_copy(reader, dyna_ptr, sd_size, "input");

    nbg_desc->n_inputs = nbg_fmt_sd_count(section_header);
    vipOnError(viphal_allocate_memory(sizeof(nbg_desc_input_output_t) * nbg_desc->n_inputs,
                        (void **)&nbg_desc->inputs));
    for (index = 0; index < nbg_desc->n_inputs; index++) {
        input = (nbg_section_input_output_t *)((vip_int8_t *)dyna_ptr + index * sd_entsize);
        nbg_desc->inputs[index].dim_count = input->dim_count;
        nbg_desc->inputs[index].data_format = input->data_format;
        nbg_desc->inputs[index].data_type = input->data_type;
        nbg_desc->inputs[index].quant_format = input->quant_format;
        for (sub_index = 0; sub_index < input->dim_count; sub_index++) {
            nbg_desc->inputs[index].dim_size[sub_index] = input->dim_size[sub_index];
        }
        if (VIP_BUFFER_QUANTIZE_DYNAMIC_FIXED_POINT == input->quant_format) {
            nbg_desc->inputs[index].quant_data.dfp.fixed_point_pos = input->quant_data.dfp.fixed_point_pos;
        }
        else {
            nbg_desc->inputs[index].quant_data.affine.tf_scale = input->quant_data.affine.tf_scale;
            nbg_desc->inputs[index].quant_data.affine.tf_zero_point = input->quant_data.affine.tf_zero_point;
        }
        viphal_copy_memory(nbg_desc->inputs[index].name, input->name, sizeof(vip_char_t) * MAX_IO_NAME_LEGTH);
    }

onError:
    return status;
}

static vip_status_e nbg_fmt_read_section_outputs(
    nbg_fmt_reader_t *reader,
    nbg_header_t *nbg_header,
    nbg_fmt_descriptor_t *nbg_desc,
    nbg_section_header_t *section_header,
    vip_uint8_t *dyna_ptr
    )
{
    vip_status_e status = VIP_SUCCESS;

    vip_size_t sd_size = 0;
    vip_size_t sd_offset = 0;
    vip_uint32_t sd_entsize = 0;
    vip_uint32_t index = 0;
    vip_uint32_t sub_index = 0;
    nbg_section_input_output_t *output = VIP_NULL;

    sd_size = SIZE_CAST(vip_size_t, nbg_fmt_get_sd_size(section_header));
    sd_offset = SIZE_CAST(vip_size_t, nbg_fmt_get_sd_offset(section_header));
    sd_entsize = section_header->sd_entsize;

    CHECK_SECTION_DATA_STRUCT_SIZE(sd_entsize, nbg_section_input_output_t, "output");

    nbg_fmt_reader_locate(reader, sd_offset);
    nbg_fmt_read_data_copy(reader, dyna_ptr, sd_size, "output");

    nbg_desc->n_outputs = nbg_fmt_sd_count(section_header);
    vipOnError(viphal_allocate_memory(sizeof(nbg_desc_input_output_t) * nbg_desc->n_outputs,
                    (void **)&nbg_desc->outputs));
    for (index = 0; index < nbg_desc->n_outputs; index++) {
        output = (nbg_section_input_output_t *)((vip_int8_t *)dyna_ptr + index * sd_entsize);
        nbg_desc->outputs[index].dim_count = output->dim_count;
        nbg_desc->outputs[index].data_format = output->data_format;
        nbg_desc->outputs[index].data_type = output->data_type;
        nbg_desc->outputs[index].quant_format = output->quant_format;
        for (sub_index = 0; sub_index < output->dim_count; sub_index++) {
            nbg_desc->outputs[index].dim_size[sub_index] = output->dim_size[sub_index];
        }
        if (VIP_BUFFER_QUANTIZE_DYNAMIC_FIXED_POINT == output->quant_format) {
            nbg_desc->outputs[index].quant_data.dfp.fixed_point_pos = output->quant_data.dfp.fixed_point_pos;
        }
        else {
            nbg_desc->outputs[index].quant_data.affine.tf_scale = output->quant_data.affine.tf_scale;
            nbg_desc->outputs[index].quant_data.affine.tf_zero_point = output->quant_data.affine.tf_zero_point;
        }
        viphal_copy_memory(nbg_desc->outputs[index].name, output->name, sizeof(vip_char_t) * MAX_IO_NAME_LEGTH);
    }

onError:
    return status;
}

static vip_status_e nbg_fmt_read_section_layers(
    nbg_fmt_reader_t *reader,
    nbg_header_t *nbg_header,
    nbg_fmt_descriptor_t *nbg_desc,
    nbg_section_header_t *section_header,
    vip_uint8_t *dyna_ptr
    )
{
    vip_status_e status = VIP_SUCCESS;
    vip_size_t sd_size = 0;
    vip_size_t sd_offset = 0;
    vip_uint32_t sd_entsize = 0;
    vip_uint32_t index = 0;
    nbg_section_layer_t *layer = VIP_NULL;

    sd_size = SIZE_CAST(vip_size_t, nbg_fmt_get_sd_size(section_header));
    sd_offset = SIZE_CAST(vip_size_t, nbg_fmt_get_sd_offset(section_header));
    sd_entsize = section_header->sd_entsize;

    nbg_fmt_reader_locate(reader, sd_offset);
    nbg_fmt_read_data_copy(reader, dyna_ptr, sd_size, "layers");

    nbg_desc->n_orig_layers = nbg_fmt_sd_count(section_header);
    vipOnError(viphal_allocate_memory(sizeof(nbg_desc_layer_t) * nbg_desc->n_orig_layers,
                        (void **)&nbg_desc->orig_layers));
    for (index = 0; index < nbg_desc->n_orig_layers; index++) {
        layer = (nbg_section_layer_t *)((vip_int8_t*)dyna_ptr + index * sd_entsize);
        viphal_copy_memory(nbg_desc->orig_layers[index].name, layer->name, sizeof(vip_char_t) * LAYER_NAME_SIZE);
        nbg_desc->orig_layers[index].node_id = layer->node_id;
        nbg_desc->orig_layers[index].operation_count = layer->operation_count;
        nbg_desc->orig_layers[index].uid = layer->uid;
    }

onError:
    return status;
}

static vip_status_e nbg_fmt_read_section_operations(
    nbg_fmt_reader_t *reader,
    nbg_header_t *nbg_header,
    nbg_fmt_descriptor_t *nbg_desc,
    nbg_section_header_t *section_header,
    vip_uint8_t *dyna_ptr
    )
{
    vip_status_e status = VIP_SUCCESS;
    vip_size_t sd_size = 0;
    vip_size_t sd_offset = 0;
    vip_uint32_t sd_entsize = 0;
    vip_uint32_t index = 0;
    nbg_section_operation_t* operation = VIP_NULL;

    sd_size = SIZE_CAST(vip_size_t, nbg_fmt_get_sd_size(section_header));
    sd_offset = SIZE_CAST(vip_size_t, nbg_fmt_get_sd_offset(section_header));
    sd_entsize = section_header->sd_entsize;

    nbg_fmt_reader_locate(reader, sd_offset);
    nbg_fmt_read_data_copy(reader, dyna_ptr, sd_size, "operat_tb");

    nbg_desc->n_operations = nbg_fmt_sd_count(section_header);
    vipOnError(viphal_allocate_memory(sizeof(nbg_desc_operation_t) * nbg_desc->n_operations,
                 (void **)&nbg_desc->operations));
    for (index = 0; index < nbg_desc->n_operations; index++) {
        operation = (nbg_section_operation_t*)((vip_int8_t*)dyna_ptr + index * sd_entsize);
        nbg_desc->operations[index].type = operation->type;
        nbg_desc->operations[index].index = operation->index;
        nbg_desc->operations[index].layer_id = operation->layer_id;
        nbg_desc->operations[index].patch_index = operation->patch_index;
        nbg_desc->operations[index].patch_count = operation->patch_count;
        nbg_desc->operations[index].state_id = operation->state_id;
        nbg_desc->operations[index].preempt = operation->preempt;
        nbg_desc->operations[index].abs_share_id = operation->abs_share_id;
        nbg_desc->operations[index].abs_op_id = operation->abs_op_id;
    }

onError:
    return status;
}

static vip_status_e nbg_fmt_read_section_sh(
    nbg_fmt_reader_t *reader,
    nbg_header_t *nbg_header,
    nbg_fmt_descriptor_t *nbg_desc,
    nbg_section_header_t *section_header,
    vip_uint8_t *dyna_ptr
    )
{
    vip_status_e status = VIP_SUCCESS;
    vip_size_t sd_size = 0;
    vip_size_t sd_offset = 0;
    vip_uint32_t sd_entsize = 0;
    vip_uint32_t index = 0;
    nbg_section_sh_operation_t *sh_op = VIP_NULL;

    sd_size = SIZE_CAST(vip_size_t, nbg_fmt_get_sd_size(section_header));
    sd_offset = SIZE_CAST(vip_size_t, nbg_fmt_get_sd_offset(section_header));
    sd_entsize = section_header->sd_entsize;

    nbg_fmt_reader_locate(reader, sd_offset);
    nbg_fmt_read_data_copy(reader, dyna_ptr, sd_size, "sh_op_data");

    nbg_desc->n_sh_ops = nbg_fmt_sd_count(section_header);
    vipOnError(viphal_allocate_memory(sizeof(nbg_desc_sh_operation_t) * nbg_desc->n_sh_ops,
                    (void **)&nbg_desc->sh_ops));
    for (index = 0; index < nbg_desc->n_sh_ops; index++) {
        sh_op = (nbg_section_sh_operation_t*)((vip_int8_t*)dyna_ptr + index * sd_entsize);
        nbg_desc->sh_ops[index].instr_index = sh_op->instr_index;
        nbg_desc->sh_ops[index].ppu_param_index = sh_op->ppu_param_index;
    }

onError:
    return status;
}

static vip_status_e nbg_fmt_read_section_patch_table(
    nbg_fmt_reader_t *reader,
    nbg_header_t *nbg_header,
    nbg_fmt_descriptor_t *nbg_desc,
    nbg_section_header_t *section_header,
    vip_uint8_t *dyna_ptr
    )
{
    vip_status_e status = VIP_SUCCESS;
    vip_size_t sd_size = 0;
    vip_size_t sd_offset = 0;
    vip_uint32_t sd_entsize = 0;
    vip_uint32_t index = 0;

    sd_size = SIZE_CAST(vip_size_t, nbg_fmt_get_sd_size(section_header));
    sd_offset = SIZE_CAST(vip_size_t, nbg_fmt_get_sd_offset(section_header));
    sd_entsize = section_header->sd_entsize;

    /* sanity check */
    if (nbg_desc->net_info.address_width == 40) {
        CHECK_SECTION_DATA_STRUCT_SIZE(sd_entsize, nbg_section_patch_table_40bit_t, "pt_40va");
    }
    else {
        CHECK_SECTION_DATA_STRUCT_SIZE(sd_entsize, nbg_section_patch_table_t, "pt");
    }

    nbg_fmt_reader_locate(reader, sd_offset);
    nbg_fmt_read_data_copy(reader, dyna_ptr, sd_size, "patch_table");

    nbg_desc->n_pd_sections = nbg_fmt_sd_count(section_header);
    vipOnError(viphal_allocate_memory(sizeof(nbg_desc_patch_t) * nbg_desc->n_pd_sections,
                                      (void **)&nbg_desc->pd_sections));
    for (index = 0; index < nbg_desc->n_pd_sections; index++) {
        if (nbg_desc->net_info.address_width == 40) {
            nbg_section_patch_table_40bit_t *pd_entry = VIP_NULL;
            pd_entry = (nbg_section_patch_table_40bit_t *)((vip_int8_t *)dyna_ptr + index * sd_entsize);
            nbg_desc->pd_sections[index].dest_offset = pd_entry->dest_offset;
            nbg_desc->pd_sections[index].index = pd_entry->index;
            nbg_desc->pd_sections[index].source_type = pd_entry->source_type;
            nbg_desc->pd_sections[index].src_offset = pd_entry->src_offset;
            nbg_desc->pd_sections[index].transformed = pd_entry->transformed;
            nbg_desc->pd_sections[index].type = pd_entry->type;
            nbg_desc->pd_sections[index].name = pd_entry->name;
            nbg_desc->pd_sections[index].dest_offset_h = pd_entry->dest_offset_h;
            nbg_desc->pd_sections[index].mask_h = pd_entry->mask_h;
            #if vpmdNBG_SIZE_OVER_4G
            nbg_desc->pd_sections[index].src_offset |= (vip_size_t)(pd_entry->src_offset_h) << 32;
            #endif

            #if vpmdENABLE_MMU
            if (NBG_SOURCE_MISC_DYNA_OUTPUT == pd_entry->source_type) {
                /* the coeff data buffer should be writable, only for MMU enabled */
                nbg_desc->coeff_data_writable = vip_true_e;
                vip_info("can not set weight memory read only attribute\n");
            }
            #endif
        }
        else {
            nbg_section_patch_table_t *pd_entry = VIP_NULL;
            pd_entry = (nbg_section_patch_table_t *)((vip_int8_t *)dyna_ptr + index * sd_entsize);
            nbg_desc->pd_sections[index].dest_offset = pd_entry->dest_offset;
            nbg_desc->pd_sections[index].index = pd_entry->index;
            nbg_desc->pd_sections[index].source_type = pd_entry->source_type;
            nbg_desc->pd_sections[index].src_offset = pd_entry->src_offset;
            nbg_desc->pd_sections[index].transformed = pd_entry->transformed;
            nbg_desc->pd_sections[index].type = pd_entry->type;
            nbg_desc->pd_sections[index].name = pd_entry->name;
            #if vpmdENABLE_MMU
            if (NBG_SOURCE_MISC_DYNA_OUTPUT == pd_entry->source_type) {
                /* the coeff data buffer should be writable, only for MMU enabled */
                nbg_desc->coeff_data_writable = vip_true_e;
                vip_info("can not set weight memory read only attribute\n");
            }
            #endif
        }
    }

onError:
    return status;
}

static vip_status_e nbg_fmt_read_section_layer_param(
    nbg_fmt_reader_t *reader,
    nbg_header_t *nbg_header,
    nbg_fmt_descriptor_t *nbg_desc,
    nbg_section_header_t *section_header,
    vip_uint8_t *dyna_ptr
    )
{
    vip_status_e status = VIP_SUCCESS;
    vip_size_t sd_size = 0;
    vip_size_t sd_offset = 0;
    vip_uint32_t sd_entsize = 0;
    vip_uint32_t index = 0;
    vip_uint32_t sub_index = 0;
    nbg_section_layer_parameter_t *layer_param = VIP_NULL;

    sd_size = SIZE_CAST(vip_size_t, nbg_fmt_get_sd_size(section_header));
    sd_offset = SIZE_CAST(vip_size_t, nbg_fmt_get_sd_offset(section_header));
    sd_entsize = section_header->sd_entsize;

    nbg_fmt_reader_locate(reader, sd_offset);
    nbg_fmt_read_data_copy(reader, dyna_ptr, sd_size, "layer_param");

    nbg_desc->n_layer_param = nbg_fmt_sd_count(section_header);
    vipOnError(viphal_allocate_memory(sizeof(nbg_desc_layer_parameter_t) * nbg_desc->n_layer_param,
                    (void **)&nbg_desc->lp_entries));
    viphal_zero_memory(nbg_desc->lp_entries, sizeof(nbg_desc_layer_parameter_t) * nbg_desc->n_layer_param);
    for (index = 0; index < nbg_desc->n_layer_param; index++) {
        layer_param = (nbg_section_layer_parameter_t*)((vip_int8_t*)dyna_ptr + index * sd_entsize);
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

onError:
    return status;
}

static vip_status_e nbg_fmt_read_section_sw(
    nbg_fmt_reader_t *reader,
    nbg_header_t *nbg_header,
    nbg_fmt_descriptor_t *nbg_desc,
    nbg_section_header_t *section_header,
    vip_uint8_t *dyna_ptr
    )
{
    vip_status_e status = VIP_SUCCESS;
    vip_size_t sd_size = 0;
    vip_size_t sd_offset = 0;
    vip_uint32_t sd_entsize = 0;
    vip_uint32_t index = 0;
    nbg_section_sw_operation_t *sw_op = VIP_NULL;

    sd_size = SIZE_CAST(vip_size_t, nbg_fmt_get_sd_size(section_header));
    sd_offset = SIZE_CAST(vip_size_t, nbg_fmt_get_sd_offset(section_header));
    sd_entsize = section_header->sd_entsize;

    nbg_fmt_reader_locate(reader, sd_offset);
    nbg_fmt_read_data_copy(reader, dyna_ptr, sd_size, "sw_op");

    nbg_desc->n_sw_ops = nbg_fmt_sd_count(section_header);
    vipOnError(viphal_allocate_memory(sizeof(nbg_desc_sw_operation_t) * nbg_desc->n_sw_ops,
                (void **)&nbg_desc->sw_ops));
    for (index = 0; index < nbg_desc->n_sw_ops; index++) {
        sw_op = (nbg_section_sw_operation_t*)((vip_int8_t*)dyna_ptr + index * sd_entsize);
        nbg_desc->sw_ops[index].sw_peration_type = sw_op->sw_peration_type;
        viphal_copy_memory(nbg_desc->sw_ops[index].name, sw_op->name,
            sizeof(vip_char_t)* MAX_SW_LAYER_NAME_LENGTH);
    }

onError:
    return status;
}

static vip_status_e nbg_fmt_read_section_hw_init_op(
    nbg_fmt_reader_t *reader,
    nbg_header_t *nbg_header,
    nbg_fmt_descriptor_t *nbg_desc,
    nbg_section_header_t *section_header,
    vip_uint8_t *dyna_ptr
    )
{
    vip_status_e status = VIP_SUCCESS;
    vip_size_t sd_size = 0;
    vip_size_t sd_offset = 0;
    vip_uint32_t sd_entsize = 0;
    vip_uint32_t index = 0;
    nbg_section_hw_init_operation_t *init_op = VIP_NULL;

    sd_size = SIZE_CAST(vip_size_t, nbg_fmt_get_sd_size(section_header));
    sd_offset = SIZE_CAST(vip_size_t, nbg_fmt_get_sd_offset(section_header));
    sd_entsize = section_header->sd_entsize;

    nbg_fmt_reader_locate(reader, sd_offset);
    nbg_fmt_read_data_copy(reader, dyna_ptr, sd_size, "init_op");

    nbg_desc->n_hw_init_ops = nbg_fmt_sd_count(section_header);
    vipOnError(viphal_allocate_memory(sizeof(nbg_desc_hw_init_operation_t) * nbg_desc->n_hw_init_ops,
                        (void **)&nbg_desc->init_ops));
    for (index = 0; index < nbg_desc->n_hw_init_ops; index++) {
        init_op = (nbg_section_hw_init_operation_t*)((vip_int8_t*)dyna_ptr + index * sd_entsize);
        nbg_desc->init_ops[index].patch_count = init_op->patch_count;
        nbg_desc->init_ops[index].patch_index = init_op->patch_index;
        nbg_desc->init_ops[index].state_id = init_op->state_id;
    }

onError:
    return status;
}

static vip_status_e nbg_fmt_read_section_ppu_param(
    nbg_fmt_reader_t *reader,
    nbg_header_t *nbg_header,
    nbg_fmt_descriptor_t *nbg_desc,
    nbg_section_header_t *section_header,
    vip_uint8_t *dyna_ptr
    )
{
    vip_status_e status = VIP_SUCCESS;
    vip_size_t sd_size = 0;
    vip_size_t sd_offset = 0;
    vip_uint32_t sd_entsize = 0;
    vip_uint32_t index = 0;
    nbg_section_ppu_param_t *ppu_param = VIP_NULL;

    sd_size = SIZE_CAST(vip_size_t, nbg_fmt_get_sd_size(section_header));
    sd_offset = SIZE_CAST(vip_size_t, nbg_fmt_get_sd_offset(section_header));
    sd_entsize = section_header->sd_entsize;

    nbg_fmt_reader_locate(reader, sd_offset);
    nbg_fmt_read_data_copy(reader, dyna_ptr, sd_size, "ppu_param");

    nbg_desc->n_ppu_param = nbg_fmt_sd_count(section_header);
    vipOnError(viphal_allocate_memory(sizeof(nbg_desc_ppu_param_t) * nbg_desc->n_ppu_param,
                        (void **)&nbg_desc->ppu_param));
    for (index = 0; index < nbg_desc->n_ppu_param; index++) {
        ppu_param = (nbg_section_ppu_param_t*)((vip_int8_t*)dyna_ptr + index * sd_entsize);
        nbg_desc->ppu_param[index].global_offset_x = ppu_param->global_offset_x;
        nbg_desc->ppu_param[index].global_offset_y = ppu_param->global_offset_y;
        nbg_desc->ppu_param[index].global_offset_z = ppu_param->global_offset_z;
        nbg_desc->ppu_param[index].global_scale_x = ppu_param->global_scale_x;
        nbg_desc->ppu_param[index].global_scale_y = ppu_param->global_scale_y;
        nbg_desc->ppu_param[index].global_scale_z = ppu_param->global_scale_z;
        nbg_desc->ppu_param[index].group_size_x = ppu_param->group_size_x;
        nbg_desc->ppu_param[index].group_size_y = ppu_param->group_size_y;
        nbg_desc->ppu_param[index].group_size_z = ppu_param->group_size_z;
        nbg_desc->ppu_param[index].group_count_x = ppu_param->group_count_x;
        nbg_desc->ppu_param[index].group_count_y = ppu_param->group_count_y;
        nbg_desc->ppu_param[index].group_count_z = ppu_param->group_count_z;
    }

onError:
    return status;
}

static vip_status_e nbg_fmt_copy_table_data(
    vip_uint32_t count,
    vip_uint32_t sd_entsize,
    nbg_desc_data_table_t *desc_table,
    vip_uint8_t *dyna_ptr
    )
{
    vip_status_e status = VIP_SUCCESS;
    vip_uint32_t index = 0;

    for (index = 0; index < count; index++) {
        if (sd_entsize == sizeof(nbg_section_entry_32bit_t)) {
            nbg_section_entry_32bit_t *table_32bit = VIP_NULL;
            table_32bit = (nbg_section_entry_32bit_t *)((vip_int8_t *)dyna_ptr + index * sd_entsize);
            desc_table[index].offset = SIZE_CAST(vip_size_t, table_32bit->offset);
            desc_table[index].size = table_32bit->size;
        }
        else if (sd_entsize == sizeof(nbg_section_entry_64bit_t)) {
            nbg_section_entry_64bit_t *table_64bit = VIP_NULL;
            table_64bit = (nbg_section_entry_64bit_t *)((vip_int8_t *)dyna_ptr + index * sd_entsize);
            desc_table[index].offset = SIZE_CAST(vip_size_t, table_64bit->offset);
            desc_table[index].size =  SIZE_CAST(vip_size_t, table_64bit->size);
        }
        else {
            vip_error("not support sd_entsize:%d\n", sd_entsize);
            vipGoOnError(VIP_ERROR_INVALID_ARGUMENTS);
        }
    }

onError:
    return status;
}

static vip_status_e nbg_fmt_read_section_coef_data_table(
    nbg_fmt_reader_t *reader,
    nbg_header_t *nbg_header,
    nbg_fmt_descriptor_t *nbg_desc,
    nbg_section_header_t *section_header,
    vip_uint8_t *dyna_ptr
    )
{
    vip_status_e status = VIP_SUCCESS;
    vip_size_t sd_size = 0;
    vip_size_t sd_offset = 0;
    vip_uint32_t sd_entsize = 0;

    sd_size = SIZE_CAST(vip_size_t, nbg_fmt_get_sd_size(section_header));
    sd_offset = SIZE_CAST(vip_size_t, nbg_fmt_get_sd_offset(section_header));
    sd_entsize = section_header->sd_entsize;

    nbg_fmt_reader_locate(reader, sd_offset);
    nbg_fmt_read_data_copy(reader, dyna_ptr, sd_size, "coeff_tb");

    nbg_desc->n_coeff_data = nbg_fmt_sd_count(section_header);
    vipOnError(viphal_allocate_memory(sizeof(nbg_desc_data_table_t) * nbg_desc->n_coeff_data,
                                      (void **)&nbg_desc->coeff_data_table));
    vipOnError(nbg_fmt_copy_table_data(nbg_desc->n_coeff_data, sd_entsize, nbg_desc->coeff_data_table, dyna_ptr));

onError:
    return status;
}

static vip_status_e nbg_fmt_read_section_ICD_table(
    nbg_fmt_reader_t *reader,
    nbg_header_t *nbg_header,
    nbg_fmt_descriptor_t *nbg_desc,
    nbg_section_header_t *section_header,
    vip_uint8_t *dyna_ptr
    )
{
    vip_status_e status = VIP_SUCCESS;
    vip_size_t sd_size = 0;
    vip_size_t sd_offset = 0;
    vip_uint32_t sd_entsize = 0;

    sd_size = SIZE_CAST(vip_size_t, nbg_fmt_get_sd_size(section_header));
    sd_offset = SIZE_CAST(vip_size_t, nbg_fmt_get_sd_offset(section_header));
    sd_entsize = section_header->sd_entsize;

    nbg_fmt_reader_locate(reader, sd_offset);
    nbg_fmt_read_data_copy(reader, dyna_ptr, sd_size, "ICDT");

    nbg_desc->n_ICDT = nbg_fmt_sd_count(section_header);
    vipOnError(viphal_allocate_memory(sizeof(nbg_desc_data_table_t) * nbg_desc->n_ICDT,
                    (void **)&nbg_desc->ICDT));
    vipOnError(nbg_fmt_copy_table_data(nbg_desc->n_ICDT, sd_entsize, nbg_desc->ICDT, dyna_ptr));

onError:
    return status;
}

static vip_status_e nbg_fmt_read_section_coef_header_table(
    nbg_fmt_reader_t *reader,
    nbg_header_t *nbg_header,
    nbg_fmt_descriptor_t *nbg_desc,
    nbg_section_header_t *section_header,
    vip_uint8_t *dyna_ptr
    )
{
    vip_status_e status = VIP_SUCCESS;
    vip_size_t sd_size = 0;
    vip_size_t sd_offset = 0;
    vip_uint32_t sd_entsize = 0;

    sd_size = SIZE_CAST(vip_size_t, nbg_fmt_get_sd_size(section_header));
    sd_offset = SIZE_CAST(vip_size_t, nbg_fmt_get_sd_offset(section_header));
    sd_entsize = section_header->sd_entsize;

    nbg_fmt_reader_locate(reader, sd_offset);
    nbg_fmt_read_data_copy(reader, dyna_ptr, sd_size, "coef_header_tb");

    nbg_desc->n_coeff_header = nbg_fmt_sd_count(section_header);
    vipOnError(viphal_allocate_memory(sizeof(nbg_desc_data_table_t) * nbg_desc->n_coeff_header,
                                      (void **)&nbg_desc->coeff_header_table));
    vipOnError(nbg_fmt_copy_table_data(nbg_desc->n_coeff_header, sd_entsize, nbg_desc->coeff_header_table, dyna_ptr));

onError:
    return status;
}

static vip_status_e nbg_fmt_read_section_ppu_instr_table(
    nbg_fmt_reader_t *reader,
    nbg_header_t *nbg_header,
    nbg_fmt_descriptor_t *nbg_desc,
    nbg_section_header_t *section_header,
    vip_uint8_t *dyna_ptr
    )
{
    vip_status_e status = VIP_SUCCESS;
    vip_size_t sd_size = 0;
    vip_size_t sd_offset = 0;
    vip_uint32_t sd_entsize = 0;

    sd_size = SIZE_CAST(vip_size_t, nbg_fmt_get_sd_size(section_header));
    sd_offset = SIZE_CAST(vip_size_t, nbg_fmt_get_sd_offset(section_header));
    sd_entsize = section_header->sd_entsize;

    nbg_fmt_reader_locate(reader, sd_offset);
    nbg_fmt_read_data_copy(reader, dyna_ptr, sd_size, "ppu_instr_tb");

    nbg_desc->n_ppu_instr = nbg_fmt_sd_count(section_header);
    vipOnError(viphal_allocate_memory(sizeof(nbg_desc_data_table_t) * nbg_desc->n_ppu_instr,
                                     (void **)&nbg_desc->ppu_instr_table));
    vipOnError(nbg_fmt_copy_table_data(nbg_desc->n_ppu_instr, sd_entsize, nbg_desc->ppu_instr_table, dyna_ptr));

onError:
    return status;
}

static vip_status_e nbg_fmt_read_section_load_states_table(
    nbg_fmt_reader_t *reader,
    nbg_header_t *nbg_header,
    nbg_fmt_descriptor_t *nbg_desc,
    nbg_section_header_t *section_header,
    vip_uint8_t *dyna_ptr
    )
{
    vip_status_e status = VIP_SUCCESS;
    vip_size_t sd_size = 0;
    vip_size_t sd_offset = 0;
    vip_uint32_t sd_entsize = 0;

    sd_size = SIZE_CAST(vip_size_t, nbg_fmt_get_sd_size(section_header));
    sd_offset = SIZE_CAST(vip_size_t, nbg_fmt_get_sd_offset(section_header));
    sd_entsize = section_header->sd_entsize;

    nbg_fmt_reader_locate(reader, sd_offset);
    nbg_fmt_read_data_copy(reader, dyna_ptr, sd_size, "load_states_tb");

    nbg_desc->n_load_state = nbg_fmt_sd_count(section_header);
    vipOnError(viphal_allocate_memory(sizeof(nbg_desc_data_table_t) * nbg_desc->n_load_state,
                                      (void **)&nbg_desc->load_states_table));
    vipOnError(nbg_fmt_copy_table_data(nbg_desc->n_load_state, sd_entsize, nbg_desc->load_states_table, dyna_ptr));

onError:
    return status;
}

static vip_status_e nbg_fmt_read_section_dyna_io_table(
    nbg_fmt_reader_t *reader,
    nbg_header_t *nbg_header,
    nbg_fmt_descriptor_t *nbg_desc,
    nbg_section_header_t *section_header,
    vip_uint8_t *dyna_ptr
    )
{
    vip_status_e status = VIP_SUCCESS;
    vip_size_t sd_size = 0;
    vip_size_t sd_offset = 0;
    vip_uint32_t sd_entsize = 0;

    sd_size = SIZE_CAST(vip_size_t, nbg_fmt_get_sd_size(section_header));
    sd_offset = SIZE_CAST(vip_size_t, nbg_fmt_get_sd_offset(section_header));
    sd_entsize = section_header->sd_entsize;

    nbg_fmt_reader_locate(reader, sd_offset);
    nbg_fmt_read_data_copy(reader, dyna_ptr, sd_size, "dyna_io_tb");

    nbg_desc->n_dyna_io = nbg_fmt_sd_count(section_header);
    vipOnError(viphal_allocate_memory(sizeof(nbg_desc_data_table_t) * nbg_desc->n_dyna_io,
                                     (void **)&nbg_desc->dyna_io_table));
    vipOnError(nbg_fmt_copy_table_data(nbg_desc->n_dyna_io, sd_entsize, nbg_desc->dyna_io_table, dyna_ptr));

onError:
    return status;
}

#if vpmdENABLE_LAYER_DUMP
static vip_status_e nbg_fmt_read_section_layer_dump_operation(
    nbg_fmt_reader_t *reader,
    nbg_header_t *nbg_header,
    nbg_fmt_descriptor_t *nbg_desc,
    nbg_section_header_t *section_header,
    vip_uint8_t *dyna_ptr
    )
{
    vip_status_e status = VIP_SUCCESS;
    vip_size_t sd_size = 0;
    vip_size_t sd_offset = 0;
    vip_uint32_t sd_entsize = 0;
    vip_uint32_t index = 0;
    nbg_ldump_operation_t *ldump_operation = VIP_NULL;

    sd_size = SIZE_CAST(vip_size_t, nbg_fmt_get_sd_size(section_header));
    sd_offset = SIZE_CAST(vip_size_t, nbg_fmt_get_sd_offset(section_header));
    sd_entsize = section_header->sd_entsize;

    nbg_fmt_reader_locate(reader, sd_offset);
    nbg_fmt_read_data_copy(reader, dyna_ptr, sd_size, "nld_op");

    nbg_desc->n_ldump_operation = nbg_fmt_sd_count(section_header);
    vipOnError(viphal_allocate_memory(sizeof(nbg_desc_ldump_operation_t) * nbg_desc->n_ldump_operation,
                                    (void **)&nbg_desc->ldump_operation));
    viphal_zero_memory(nbg_desc->ldump_operation, sizeof(nbg_desc_ldump_operation_t) * nbg_desc->n_ldump_operation);
    for (index = 0; index < nbg_desc->n_ldump_operation; index++) {
        ldump_operation = (nbg_ldump_operation_t*)((vip_int8_t*)dyna_ptr + index * sd_entsize);
        nbg_desc->ldump_operation[index].insert_index = ldump_operation->insert_index;
        nbg_desc->ldump_operation[index].loadstate_index = ldump_operation->loadstate_index;
        nbg_desc->ldump_operation[index].start_patch_index = ldump_operation->start_patch_index;
        nbg_desc->ldump_operation[index].patch_count = ldump_operation->patch_count;
        nbg_desc->ldump_operation[index].core_id = ldump_operation->core_id;
        nbg_desc->ldump_operation[index].wait_count = ldump_operation->wait_count;
    }

onError:
    return status;
}

static vip_status_e nbg_fmt_read_section_layer_dump_outputs(
    nbg_fmt_reader_t *reader,
    nbg_header_t *nbg_header,
    nbg_fmt_descriptor_t *nbg_desc,
    nbg_section_header_t *section_header,
    vip_uint8_t *dyna_ptr
    )
{
    vip_status_e status = VIP_SUCCESS;
    vip_size_t sd_size = 0;
    vip_size_t sd_offset = 0;
    vip_uint32_t sd_entsize = 0;
    vip_uint32_t index = 0;
    vip_uint32_t sub_index = 0;
    nbg_ldump_output_t *ldump_outputs = VIP_NULL;

    sd_size = SIZE_CAST(vip_size_t, nbg_fmt_get_sd_size(section_header));
    sd_offset = SIZE_CAST(vip_size_t, nbg_fmt_get_sd_offset(section_header));
    sd_entsize = section_header->sd_entsize;

    nbg_fmt_reader_locate(reader, sd_offset);
    nbg_fmt_read_data_copy(reader, dyna_ptr, sd_size, "nld_output");

    nbg_desc->n_ldump_outputs = nbg_fmt_sd_count(section_header);
    vipOnError(viphal_allocate_memory(sizeof(nbg_desc_ldump_output_t) * nbg_desc->n_ldump_outputs,
                                    (void **)&nbg_desc->ldump_outputs));
    viphal_zero_memory(nbg_desc->ldump_outputs, sizeof(nbg_desc_ldump_output_t) * nbg_desc->n_ldump_outputs);
    for (index = 0; index < nbg_desc->n_ldump_outputs; index++) {
        ldump_outputs = (nbg_ldump_output_t *)((vip_int8_t *)dyna_ptr + index * sd_entsize);
        nbg_desc->ldump_outputs[index].info.dim_count = ldump_outputs->info.dim_count;
        nbg_desc->ldump_outputs[index].info.data_format = ldump_outputs->info.data_format;
        nbg_desc->ldump_outputs[index].info.data_type = ldump_outputs->info.data_type;
        nbg_desc->ldump_outputs[index].info.quant_format = ldump_outputs->info.quant_format;
        if (VIP_BUFFER_QUANTIZE_DYNAMIC_FIXED_POINT == ldump_outputs->info.quant_format) {
            nbg_desc->ldump_outputs[index].info.quant_data.dfp.fixed_point_pos = \
                    ldump_outputs->info.quant_data.dfp.fixed_point_pos;
        }
        else {
            nbg_desc->ldump_outputs[index].info.quant_data.affine.tf_scale = \
                    ldump_outputs->info.quant_data.affine.tf_scale;
            nbg_desc->ldump_outputs[index].info.quant_data.affine.tf_zero_point = \
                    ldump_outputs->info.quant_data.affine.tf_zero_point;
        }
        viphal_copy_memory(nbg_desc->ldump_outputs[index].info.name, ldump_outputs->info.name,
                           sizeof(vip_char_t)* MAX_LAYER_PARAM_NAME_LENGTH);
        for (sub_index = 0; sub_index < ldump_outputs->info.dim_count; sub_index++) {
            nbg_desc->ldump_outputs[index].info.dim_size[sub_index] = ldump_outputs->info.dim_size[sub_index];
        }
    }

onError:
    return status;
}

static vip_status_e nbg_fmt_read_section_layer_dump_patch_data(
    nbg_fmt_reader_t *reader,
    nbg_header_t *nbg_header,
    nbg_fmt_descriptor_t *nbg_desc,
    nbg_section_header_t *section_header,
    vip_uint8_t *dyna_ptr
    )
{
    vip_status_e status = VIP_SUCCESS;
    vip_size_t sd_size = 0;
    vip_size_t sd_offset = 0;
    vip_uint32_t sd_entsize = 0;
    vip_uint32_t index = 0;

    sd_size = SIZE_CAST(vip_size_t, nbg_fmt_get_sd_size(section_header));
    sd_offset = SIZE_CAST(vip_size_t, nbg_fmt_get_sd_offset(section_header));
    sd_entsize = section_header->sd_entsize;

    /* sanity check */
    if (nbg_desc->net_info.address_width == 40) {
        CHECK_SECTION_DATA_STRUCT_SIZE(sd_entsize, nbg_section_patch_table_40bit_t, "nld pt_40va");
    }
    else {
        CHECK_SECTION_DATA_STRUCT_SIZE(sd_entsize, nbg_section_patch_table_t, "nld pt");
    }

    nbg_fmt_reader_locate(reader, sd_offset);
    nbg_fmt_read_data_copy(reader, dyna_ptr, sd_size, "nld_patch_table");

    nbg_desc->n_ldump_patch_data = nbg_fmt_sd_count(section_header);
    vipOnError(viphal_allocate_memory(sizeof(nbg_desc_patch_t) * nbg_desc->n_ldump_patch_data,
                                     (void **)&nbg_desc->ldump_patch_data));
    viphal_zero_memory(nbg_desc->ldump_patch_data, sizeof(nbg_desc_patch_t) * nbg_desc->n_ldump_patch_data);
    for (index = 0; index < nbg_desc->n_ldump_patch_data; index++) {
        if (nbg_desc->net_info.address_width == 40) {
            nbg_section_patch_table_40bit_t *ldump_patch_data = VIP_NULL;
            ldump_patch_data = (nbg_section_patch_table_40bit_t *)((vip_int8_t *)dyna_ptr + index * sd_entsize);
            nbg_desc->ldump_patch_data[index].dest_offset = ldump_patch_data->dest_offset;
            nbg_desc->ldump_patch_data[index].index = ldump_patch_data->index;
            nbg_desc->ldump_patch_data[index].source_type = ldump_patch_data->source_type;
            nbg_desc->ldump_patch_data[index].src_offset = ldump_patch_data->src_offset;
            nbg_desc->ldump_patch_data[index].transformed = ldump_patch_data->transformed;
            nbg_desc->ldump_patch_data[index].type = ldump_patch_data->type;
            nbg_desc->ldump_patch_data[index].name = ldump_patch_data->name;

            nbg_desc->ldump_patch_data[index].dest_offset_h = ldump_patch_data->dest_offset_h;
            nbg_desc->ldump_patch_data[index].mask_h = ldump_patch_data->mask_h;
            #if vpmdNBG_SIZE_OVER_4G
            nbg_desc->ldump_patch_data[index].src_offset |= (vip_size_t)(ldump_patch_data->src_offset_h) << 32;
            #endif
        }
        else {
            nbg_section_patch_table_t *ldump_patch_data = VIP_NULL;
            ldump_patch_data = (nbg_section_patch_table_t *)((vip_int8_t *)dyna_ptr + index * sd_entsize);
            nbg_desc->ldump_patch_data[index].dest_offset = ldump_patch_data->dest_offset;
            nbg_desc->ldump_patch_data[index].index = ldump_patch_data->index;
            nbg_desc->ldump_patch_data[index].source_type = ldump_patch_data->source_type;
            nbg_desc->ldump_patch_data[index].src_offset = ldump_patch_data->src_offset;
            nbg_desc->ldump_patch_data[index].transformed = ldump_patch_data->transformed;
            nbg_desc->ldump_patch_data[index].type = ldump_patch_data->type;
            nbg_desc->ldump_patch_data[index].name = ldump_patch_data->name;
        }
    }

onError:
    return status;
}

static vip_status_e nbg_fmt_read_section_layer_dump_instr_table(
    nbg_fmt_reader_t *reader,
    nbg_header_t *nbg_header,
    nbg_fmt_descriptor_t *nbg_desc,
    nbg_section_header_t *section_header,
    vip_uint8_t *dyna_ptr
    )
{
    vip_status_e status = VIP_SUCCESS;
    vip_size_t sd_size = 0;
    vip_size_t sd_offset = 0;
    vip_uint32_t sd_entsize = 0;

    sd_size = SIZE_CAST(vip_size_t, nbg_fmt_get_sd_size(section_header));
    sd_offset = SIZE_CAST(vip_size_t, nbg_fmt_get_sd_offset(section_header));
    sd_entsize = section_header->sd_entsize;

    nbg_fmt_reader_locate(reader, sd_offset);
    nbg_fmt_read_data_copy(reader, dyna_ptr, sd_size, "nld_istrtable");

    nbg_desc->n_ldump_instr_table = nbg_fmt_sd_count(section_header);
    vipOnError(viphal_allocate_memory(sizeof(nbg_desc_data_table_t) * nbg_desc->n_ldump_instr_table,
                            (void **)&nbg_desc->ldump_instr_table));
    viphal_zero_memory(nbg_desc->ldump_instr_table, sizeof(nbg_desc_data_table_t) * nbg_desc->n_ldump_instr_table);
    vipOnError(nbg_fmt_copy_table_data(nbg_desc->n_ldump_instr_table,
        sd_entsize, nbg_desc->ldump_instr_table, dyna_ptr));

onError:
    return status;
}

static vip_status_e nbg_fmt_read_section_layer_dump_loadstate_table(
    nbg_fmt_reader_t *reader,
    nbg_header_t *nbg_header,
    nbg_fmt_descriptor_t *nbg_desc,
    nbg_section_header_t *section_header,
    vip_uint8_t *dyna_ptr
    )
{
    vip_status_e status = VIP_SUCCESS;
    vip_size_t sd_size = 0;
    vip_size_t sd_offset = 0;
    vip_uint32_t sd_entsize = 0;

    sd_size = SIZE_CAST(vip_size_t, nbg_fmt_get_sd_size(section_header));
    sd_offset = SIZE_CAST(vip_size_t, nbg_fmt_get_sd_offset(section_header));
    sd_entsize = section_header->sd_entsize;

    nbg_fmt_reader_locate(reader, sd_offset);
    nbg_fmt_read_data_copy(reader, dyna_ptr, sd_size, "nld_LStable");

    nbg_desc->n_ldump_load_states_table = nbg_fmt_sd_count(section_header);
    vipOnError(viphal_allocate_memory(sizeof(nbg_desc_data_table_t) * nbg_desc->n_ldump_load_states_table,
                            (void **)&nbg_desc->ldump_load_states_table));
    viphal_zero_memory(nbg_desc->ldump_load_states_table,
        sizeof(nbg_desc_data_table_t) * nbg_desc->n_ldump_load_states_table);
    vipOnError(nbg_fmt_copy_table_data(nbg_desc->n_ldump_load_states_table, sd_entsize,
        nbg_desc->ldump_load_states_table, dyna_ptr));

onError:
    return status;
}
#endif

static vip_status_e nbg_fmt_read_section_checksum(
    nbg_fmt_reader_t *reader,
    nbg_header_t *nbg_header,
    nbg_fmt_descriptor_t *nbg_desc,
    nbg_section_header_t *section_header,
    vip_uint8_t *dyna_ptr
    )
{
    vip_status_e status = VIP_SUCCESS;
    vip_size_t sd_size = 0;
    vip_size_t sd_offset = 0;
    vip_uint32_t sd_entsize = 0;
    vip_uint32_t index = 0;
    nbg_section_checksum_t *checksum_info = VIP_NULL;
    vip_uint32_t n_checksum = 0;

    sd_size = SIZE_CAST(vip_size_t, nbg_fmt_get_sd_size(section_header));
    sd_offset = SIZE_CAST(vip_size_t, nbg_fmt_get_sd_offset(section_header));
    sd_entsize = section_header->sd_entsize;

    nbg_fmt_reader_locate(reader, sd_offset);
    nbg_fmt_read_data_copy(reader, dyna_ptr, sd_size, "check_sum");

    n_checksum = nbg_fmt_sd_count(section_header);

    for(index = 0; index < n_checksum; index++) {
        checksum_info = (nbg_section_checksum_t*)((vip_int8_t*)dyna_ptr + index * sd_entsize);
        switch (checksum_info->sd_type) {
            case NBG_SECTION_LOADSTATE:
                nbg_desc->check_sum.load_states = checksum_info->checksum_value;
            break;
            case NBG_SECTION_COEF_DATA:
                nbg_desc->check_sum.coeff_data = checksum_info->checksum_value;
            break;
            case NBG_SECTION_PPU_INSTR:
                nbg_desc->check_sum.ppu_instr = checksum_info->checksum_value;
            break;
            case NBG_SECTION_COEF_HEADER:
            case NBG_SECTION_NN:
            case NBG_SECTION_TP:
            break;
            default:
                vip_print("check sum section is over now support section!\n");
                status = VIP_ERROR_FAILURE;
                goto OnError;
        }
    }

OnError:
    return status;
}

static vip_status_e nbg_fmt_read_non_video_section(
    nbg_fmt_reader_t *reader,
    nbg_header_t *nbg_header,
    nbg_fmt_descriptor_t *nbg_desc
    )
{
    vip_status_e status = VIP_SUCCESS;

    nbg_sub_header_t *sub_header = VIP_NULL;
    vip_uint32_t sh_count = 0;
    vip_uint32_t i = 0;

    vip_uint64_t dyna_size = 0;
    vip_uint8_t *dyna_ptr = VIP_NULL;

    sub_header = &nbg_header->sub_header;
    sh_count = sub_header->sh_count;

    dyna_size = nbg_fmt_get_non_video_sd_max_size(nbg_header, nbg_desc);
    vipOnError(viphal_allocate_memory(SIZE_CAST(vip_uint32_t, dyna_size), (void **)&dyna_ptr));

    for (i = 0; i < sh_count; i++) {
        vip_uint32_t sd_type = NBG_SECTION_NONE;
        nbg_section_header_t *section_header = VIP_NULL;

        section_header = nbg_header->section_header + i;
        if (!section_header) {
            vip_error("section_header could not be null\n");
            vipGoOnError(VIP_ERROR_INVALID_ARGUMENTS);
        }
        sd_type = section_header->sd_type;
        switch (sd_type) {
            case NBG_SECTION_INPUT:
                vipOnError(nbg_fmt_read_section_inputs(reader, nbg_header, nbg_desc, section_header, dyna_ptr));
            break;
            case NBG_SECTION_OUTPUT:
                vipOnError(nbg_fmt_read_section_outputs(reader, nbg_header, nbg_desc, section_header, dyna_ptr));
            break;
            case NBG_SECTION_LAYER:
                vipOnError(nbg_fmt_read_section_layers(reader, nbg_header, nbg_desc, section_header, dyna_ptr));
            break;
            case NBG_SECTION_OPERATIONS:
                vipOnError(nbg_fmt_read_section_operations(reader, nbg_header, nbg_desc, section_header, dyna_ptr));
            break;
            case NBG_SECTION_SH:
                vipOnError(nbg_fmt_read_section_sh(reader, nbg_header, nbg_desc, section_header, dyna_ptr));
            break;
            case NBG_SECTION_PATCH:
                vipOnError(nbg_fmt_read_section_patch_table(reader, nbg_header, nbg_desc, section_header, dyna_ptr));
            break;
            case NBG_SECTION_LAYER_PARAM:
                vipOnError(nbg_fmt_read_section_layer_param(reader, nbg_header, nbg_desc, section_header, dyna_ptr));
            break;
            case NBG_SECTION_SW:
                vipOnError(nbg_fmt_read_section_sw(reader, nbg_header, nbg_desc, section_header, dyna_ptr));
            break;
            case NBG_SECTION_HWINITOPERATIONS:
                vipOnError(nbg_fmt_read_section_hw_init_op(reader, nbg_header, nbg_desc, section_header, dyna_ptr));
            break;
            case NBG_SECTION_ICDT:
                vipOnError(nbg_fmt_read_section_ICD_table(reader, nbg_header, nbg_desc, section_header, dyna_ptr));
            break;
            case NBG_SECTION_PPU_PARAM:
                vipOnError(nbg_fmt_read_section_ppu_param(reader, nbg_header, nbg_desc, section_header, dyna_ptr));
            break;
            case NBG_SECTION_COEF_HEADER_TABLE:
                vipOnError(nbg_fmt_read_section_coef_header_table(reader,
                                                            nbg_header, nbg_desc, section_header, dyna_ptr));
            break;
            case NBG_SECTION_PPU_INSTR_TABLE:
                vipOnError(nbg_fmt_read_section_ppu_instr_table(reader,
                                                            nbg_header, nbg_desc, section_header, dyna_ptr));
            break;
            case NBG_SECTION_LOADSTATE_TABLE:
                vipOnError(nbg_fmt_read_section_load_states_table(reader,
                                                            nbg_header, nbg_desc, section_header, dyna_ptr));
            break;
            case NBG_SECTION_DYNAIO_TABLE:
                vipOnError(nbg_fmt_read_section_dyna_io_table(reader, nbg_header, nbg_desc, section_header, dyna_ptr));
            break;
            case NBG_SECTION_COEF_DATA_TABLE:
                vipOnError(nbg_fmt_read_section_coef_data_table(reader,
                                                            nbg_header, nbg_desc, section_header, dyna_ptr));
            break;

            #if vpmdENABLE_LAYER_DUMP
            case NBG_SECTION_LAYER_DUMP_OPERATION:
                vipOnError(nbg_fmt_read_section_layer_dump_operation(reader,
                                                            nbg_header, nbg_desc, section_header, dyna_ptr));
            break;
            case NBG_SECTION_LAYER_DUMP_OUTPUTS:
                vipOnError(nbg_fmt_read_section_layer_dump_outputs(reader,
                                                            nbg_header, nbg_desc, section_header, dyna_ptr));
            break;
            case NBG_SECTION_LAYER_DUMP_PATCH_DATA:
                vipOnError(nbg_fmt_read_section_layer_dump_patch_data(reader,
                                                            nbg_header, nbg_desc, section_header, dyna_ptr));
            break;
            case NBG_SECTION_LAYER_DUMP_INSTR_TABLE:
                vipOnError(nbg_fmt_read_section_layer_dump_instr_table(reader,
                                                            nbg_header, nbg_desc, section_header, dyna_ptr));
            break;
            case NBG_SECTION_LAYER_DUMP_LOADSTATE_TABLE:
                vipOnError(nbg_fmt_read_section_layer_dump_loadstate_table(reader,
                                                            nbg_header, nbg_desc, section_header, dyna_ptr));
            break;
            #endif
            case NBG_SECTION_CHECKSUM:
                vipOnError(nbg_fmt_read_section_checksum(reader,
                                                            nbg_header, nbg_desc, section_header, dyna_ptr));
            break;

            default:
            break;
        }
    }

onError:
    if (VIP_NULL != dyna_ptr) {
        viphal_free_memory(dyna_ptr);
        dyna_ptr = VIP_NULL;
    }
    return status;
}

static vip_status_e nbg_fmt_read_section_nn(
    nbg_fmt_reader_t *reader,
    nbg_header_t *nbg_header,
    nbg_fmt_descriptor_t *nbg_desc,
    nbg_section_header_t *section_header,
    vip_uint8_t *dyna_ptr
    )
{
    vip_status_e status = VIP_SUCCESS;

    nbg_desc->n_nn_ops = nbg_fmt_sd_count(section_header);
    nbg_desc->fixed.nn_op_data_table.offset =
        SIZE_CAST(vip_size_t, nbg_fmt_get_sd_offset(section_header));
    nbg_desc->fixed.nn_op_data_table.size = SIZE_CAST(vip_size_t, nbg_fmt_get_sd_size(section_header));

    return status;
}

static vip_status_e nbg_fmt_read_section_tp(
    nbg_fmt_reader_t *reader,
    nbg_header_t *nbg_header,
    nbg_fmt_descriptor_t *nbg_desc,
    nbg_section_header_t *section_header,
    vip_uint8_t *dyna_ptr
    )
{
    vip_status_e status = VIP_SUCCESS;

    nbg_desc->n_tp_ops = nbg_fmt_sd_count(section_header);
    nbg_desc->fixed.tp_op_data_table.offset =
        SIZE_CAST(vip_size_t, nbg_fmt_get_sd_offset(section_header));
    nbg_desc->fixed.tp_op_data_table.size = SIZE_CAST(vip_size_t, nbg_fmt_get_sd_size(section_header));

    return status;
}

static vip_status_e nbg_fmt_read_section_coef_header(
    nbg_fmt_reader_t *reader,
    nbg_header_t *nbg_header,
    nbg_fmt_descriptor_t *nbg_desc,
    nbg_section_header_t *section_header,
    vip_uint8_t *dyna_ptr
    )
{
    vip_status_e status = VIP_SUCCESS;

    nbg_desc->fixed.coeff_header.offset =
        SIZE_CAST(vip_size_t, nbg_fmt_get_sd_offset(section_header));
    nbg_desc->fixed.coeff_header.size = SIZE_CAST(vip_size_t, nbg_fmt_get_sd_size(section_header));

    return status;
}

static vip_status_e nbg_fmt_read_section_coef_data(
    nbg_fmt_reader_t *reader,
    nbg_header_t *nbg_header,
    nbg_fmt_descriptor_t *nbg_desc,
    nbg_section_header_t *section_header,
    vip_uint8_t *dyna_ptr
    )
{
    vip_status_e status = VIP_SUCCESS;

    nbg_desc->fixed.coeff_data.offset =
        SIZE_CAST(vip_size_t, nbg_fmt_get_sd_offset(section_header));
    nbg_desc->fixed.coeff_data.size = SIZE_CAST(vip_size_t, nbg_fmt_get_sd_size(section_header));

    return status;
}

static vip_status_e nbg_fmt_read_section_ICD(
    nbg_fmt_reader_t *reader,
    nbg_header_t *nbg_header,
    nbg_fmt_descriptor_t *nbg_desc,
    nbg_section_header_t *section_header,
    vip_uint8_t *dyna_ptr
    )
{
    vip_status_e status = VIP_SUCCESS;

    nbg_desc->fixed.ICD.offset =
        SIZE_CAST(vip_size_t, nbg_fmt_get_sd_offset(section_header));
    nbg_desc->fixed.ICD.size = SIZE_CAST(vip_size_t, nbg_fmt_get_sd_size(section_header));

    return status;
}

static vip_status_e nbg_fmt_read_section_ppu_instr(
    nbg_fmt_reader_t *reader,
    nbg_header_t *nbg_header,
    nbg_fmt_descriptor_t *nbg_desc,
    nbg_section_header_t *section_header,
    vip_uint8_t *dyna_ptr
    )
{
    vip_status_e status = VIP_SUCCESS;

    nbg_desc->fixed.ppu_instr.offset =
        SIZE_CAST(vip_size_t, nbg_fmt_get_sd_offset(section_header));
    nbg_desc->fixed.ppu_instr.size = SIZE_CAST(vip_size_t, nbg_fmt_get_sd_size(section_header));

    return status;
}

static vip_status_e nbg_fmt_read_section_loadstate(
    nbg_fmt_reader_t *reader,
    nbg_header_t *nbg_header,
    nbg_fmt_descriptor_t *nbg_desc,
    nbg_section_header_t *section_header,
    vip_uint8_t *dyna_ptr
    )
{
    vip_status_e status = VIP_SUCCESS;

    nbg_desc->fixed.load_states.offset =
        SIZE_CAST(vip_size_t, nbg_fmt_get_sd_offset(section_header));
    nbg_desc->fixed.load_states.size = SIZE_CAST(vip_size_t, nbg_fmt_get_sd_size(section_header));

    return status;
}

static vip_status_e nbg_fmt_read_section_dyna_io(
    nbg_fmt_reader_t *reader,
    nbg_header_t *nbg_header,
    nbg_fmt_descriptor_t *nbg_desc,
    nbg_section_header_t *section_header,
    vip_uint8_t *dyna_ptr
    )
{
    vip_status_e status = VIP_SUCCESS;

    nbg_desc->fixed.dyna_io.offset =
        SIZE_CAST(vip_size_t, nbg_fmt_get_sd_offset(section_header));
    nbg_desc->fixed.dyna_io.size = SIZE_CAST(vip_size_t, nbg_fmt_get_sd_size(section_header));

    return status;
}

#if vpmdENABLE_LAYER_DUMP
static vip_status_e nbg_fmt_read_section_layer_dump_instr(
    nbg_fmt_reader_t *reader,
    nbg_header_t *nbg_header,
    nbg_fmt_descriptor_t *nbg_desc,
    nbg_section_header_t *section_header,
    vip_uint8_t *dyna_ptr
    )
{
    vip_status_e status = VIP_SUCCESS;
    vip_size_t sd_size = 0;
    vip_size_t sd_offset = 0;

    sd_size = SIZE_CAST(vip_size_t, nbg_fmt_get_sd_size(section_header));
    sd_offset = SIZE_CAST(vip_size_t, nbg_fmt_get_sd_offset(section_header));

    nbg_desc->fixed.ldump_instr.size = sd_size;
    nbg_desc->fixed.ldump_instr.offset = sd_offset;

    vipOnError(viphal_allocate_memory(sd_size, (void **)&nbg_desc->ldump_instr_logical));
    nbg_fmt_reader_locate(reader, sd_offset);
    nbg_fmt_read_data_copy(reader, nbg_desc->ldump_instr_logical, sd_size, "nld_istr");

onError:
    return status;
}

static vip_status_e nbg_fmt_read_section_layer_dump_loadstate(
    nbg_fmt_reader_t *reader,
    nbg_header_t *nbg_header,
    nbg_fmt_descriptor_t *nbg_desc,
    nbg_section_header_t *section_header,
    vip_uint8_t *dyna_ptr
    )
{
    vip_status_e status = VIP_SUCCESS;
    vip_size_t sd_size = 0;
    vip_size_t sd_offset = 0;

    sd_size = SIZE_CAST(vip_size_t, nbg_fmt_get_sd_size(section_header));
    sd_offset = SIZE_CAST(vip_size_t, nbg_fmt_get_sd_offset(section_header));

    nbg_desc->fixed.ldump_load_states.size = sd_size;
    nbg_desc->fixed.ldump_load_states.offset = sd_offset;

    vipOnError(viphal_allocate_memory(sd_size, (void **)&nbg_desc->ldump_load_states_logical));
    nbg_fmt_reader_locate(reader, sd_offset);
    nbg_fmt_read_data_copy(reader, nbg_desc->ldump_load_states_logical, sd_size, "nld_loadstate");

onError:
    return status;
}
#endif

static vip_status_e nbg_fmt_read_video_section(
    nbg_fmt_reader_t *reader,
    nbg_header_t *nbg_header,
    nbg_fmt_descriptor_t *nbg_desc
    )
{
    vip_status_e status = VIP_SUCCESS;

    nbg_sub_header_t *sub_header = VIP_NULL;
    vip_uint32_t sh_count = 0;
    vip_uint32_t i = 0;
    vip_uint64_t dyna_size = 0;
    vip_uint8_t *dyna_ptr = VIP_NULL;

    sub_header = &nbg_header->sub_header;
    sh_count = sub_header->sh_count;

    dyna_size = nbg_fmt_get_non_video_sd_max_size(nbg_header, nbg_desc);
    vipOnError(viphal_allocate_memory(SIZE_CAST(vip_uint32_t, dyna_size), (void **)&dyna_ptr));

    for (i = 0; i < sh_count; i++) {
        vip_uint32_t sd_type = NBG_SECTION_NONE;
        nbg_section_header_t *section_header = VIP_NULL;

        section_header = nbg_header->section_header + i;
        if (!section_header) {
            vip_error("section_header could not be null\n");
            vipGoOnError(VIP_ERROR_INVALID_ARGUMENTS);
        }
        sd_type = section_header->sd_type;
        switch (sd_type) {
            case NBG_SECTION_NN:
                vipOnError(nbg_fmt_read_section_nn(reader, nbg_header, nbg_desc, section_header, dyna_ptr));
            break;
            case NBG_SECTION_TP:
                vipOnError(nbg_fmt_read_section_tp(reader, nbg_header, nbg_desc, section_header, dyna_ptr));
            break;
            case NBG_SECTION_COEF_HEADER:
                vipOnError(nbg_fmt_read_section_coef_header(reader, nbg_header, nbg_desc, section_header, dyna_ptr));
            break;
            case NBG_SECTION_COEF_DATA:
                vipOnError(nbg_fmt_read_section_coef_data(reader, nbg_header, nbg_desc, section_header, dyna_ptr));
            break;
            case NBG_SECTION_ICD:
                vipOnError(nbg_fmt_read_section_ICD(reader, nbg_header, nbg_desc, section_header, dyna_ptr));
            break;
            case NBG_SECTION_PPU_INSTR:
                vipOnError(nbg_fmt_read_section_ppu_instr(reader, nbg_header, nbg_desc, section_header, dyna_ptr));
            break;
            case NBG_SECTION_LOADSTATE:
                vipOnError(nbg_fmt_read_section_loadstate(reader, nbg_header, nbg_desc, section_header, dyna_ptr));
            break;
            case NBG_SECTION_DYNAIO:
                vipOnError(nbg_fmt_read_section_dyna_io(reader, nbg_header, nbg_desc, section_header, dyna_ptr));
            break;

            #if vpmdENABLE_LAYER_DUMP
            case NBG_SECTION_LAYER_DUMP_INSTR:
                vipOnError(nbg_fmt_read_section_layer_dump_instr(reader,
                    nbg_header, nbg_desc, section_header, dyna_ptr));
            break;
            case NBG_SECTION_LAYER_DUMP_LOADSTATE:
                vipOnError(nbg_fmt_read_section_layer_dump_loadstate(reader,
                    nbg_header, nbg_desc, section_header, dyna_ptr));
            break;
            #endif

            default:
            break;
        }
    }

onError:
    if (VIP_NULL != dyna_ptr) {
        viphal_free_memory(dyna_ptr);
        dyna_ptr = VIP_NULL;
    }
    return status;
}

/*
Read section:
1. none-video memory section, alloc system memory and read out data in memory.
2. video memory section, read output section data table only.
*/
vip_status_e nbg_fmt_read_section(
    nbg_fmt_reader_t *reader,
    nbg_header_t *nbg_header,
    nbg_fmt_descriptor_t *nbg_desc
    )
{
    vip_status_e status = VIP_SUCCESS;

    vipOnError(nbg_fmt_read_basic_section(reader, nbg_header, nbg_desc));
    vipOnError(nbg_fmt_read_non_video_section(reader, nbg_header, nbg_desc));
    vipOnError(nbg_fmt_read_video_section(reader, nbg_header, nbg_desc));

onError:
    return status;
}

vip_status_e nbg_fmt_read_v2(
    nbg_fmt_reader_t *reader,
    nbg_fmt_descriptor_t *nbg_desc
    )
{
    vip_status_e status = VIP_SUCCESS;
    nbg_header_t nbg_header;

    viphal_zero_memory(&nbg_header, sizeof(nbg_header_t));
    vipOnError(nbg_fmt_read_header(reader, &nbg_header, nbg_desc));
    vipOnError(nbg_fmt_read_section(reader, &nbg_header, nbg_desc));

onError:
    if (nbg_header.section_header != VIP_NULL) {
        viphal_free_memory(nbg_header.section_header);
        nbg_header.section_header = VIP_NULL;
    }
    return status;
}
