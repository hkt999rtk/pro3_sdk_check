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

#include <nbg_lk_coeff.h>
#include <vip_hal.h>
#include <vip_hal_os_port.h>
#include <vip_hal_hardware.h>
#include <nbg_lk_network.h>
#include <nbg_lk_context.h>
#include <nbg_fmt_util.h>
#include <nbg_lk_relocate.h>

#ifndef vpmdENABLE_CHANGE_MULTI_AXI_ID
#define vpmdENABLE_CHANGE_MULTI_AXI_ID     0
#endif

vip_status_e nbglk_coeff_get_property(
    vip_network network,
    vip_uint32_t patch_index,
    vip_uint8_t **base_logical,
    vip_address_t *base_physical,
    vip_size_t *offset,
    vip_size_t *size
    )
{
    vip_network weight_network = network;
    vip_uint8_t *logical = VIP_NULL;
    vip_address_t physical = 0;
    vip_size_t t_offset = 0;
    vip_size_t t_size = 0;

    if (NBGLK_FROM_NBG_DUP_COMMAND == network->dup_type) {
        weight_network = network->friend_network;
    }

#if vpmdPRELOAD_COEFF
    if (0x0001001A <= network->nbg_desc.fixed.header.version) {
        if (NBGLK_COEFF_LOC_DDR > network->coeff_property[patch_index].location) {
            physical = weight_network->app_preload_coeff->physical;
            logical = weight_network->app_preload_coeff->logical;
        }
        else {
            physical = weight_network->coeff_data.physical;
            logical = weight_network->coeff_data.logical;
        }
        t_offset = network->coeff_property[patch_index].src_offset;
        t_size = network->coeff_property[patch_index].size;
    }
    else
#endif
    {
        physical = weight_network->coeff_data.physical;
        logical = weight_network->coeff_data.logical;
        t_offset = network->nbg_desc.coeff_data_table[patch_index].offset;
        t_size = network->nbg_desc.coeff_data_table[patch_index].size;
    }

    if (offset != VIP_NULL) {
        *offset = t_offset;
    }
    if (base_logical != VIP_NULL) {
        *base_logical = logical;
    }
    if (base_physical != VIP_NULL) {
        *base_physical = physical;
    }
    if (size != VIP_NULL) {
        *size = t_size;
    }

    return VIP_SUCCESS;
}

#if vpmdPRELOAD_COEFF
#if (vpmdENABLE_DEBUG_LOG > 2)
static char* coeff_location_str(
    vip_uint32_t location
    )
{
    switch (location) {
    case NBGLK_COEFF_LOC_USER_SPECIFIED_0: return "USER SPEC_0";
    case NBGLK_COEFF_LOC_DDR: return "DDR";
    case NBGLK_COEFF_LOC_VIP_SRAM: return "VIP-SRAM";
    default:
        return "UNKNOWN";
    }
}
#endif

vip_bool_e nbglk_op_support_vipsram_preload(
    vip_uint32_t op_type
    )
{
    vip_bool_e support = vip_false_e;
    switch (op_type) {
    case NBG_OPERATION_TYPE_NN:
        support = vip_true_e;
        break;
    default:
        break;
    }
    return support;
}

vip_status_e nbglk_coeff_preload_patch_nn(
    vip_network network,
    vip_int32_t coeff_index,
    vip_uint8_t *patch_addr,
    nbglk_coeff_preload_patch_nn_property_e prop
    )
{
    vip_status_e status = VIP_SUCCESS;

#if vpmdPRELOAD_COEFF > 1
    vip_network weight_network = network;
    if (NBGLK_FROM_NBG_DUP_COMMAND == network->dup_type) {
        weight_network = network->friend_network;
    }
#endif

    if (0x0001001A > network->nbg_desc.fixed.header.version) {
        return VIP_SUCCESS;
    }
    if ((VIP_NULL == network->coeff_property) || (coeff_index >= (vip_int32_t)network->coeff_count)) {
        vip_error("fail coeff preload patch nn coeff property is NULL or overflow\n");
        vip_error("fail coeff_index=%d, coeff count=%d\n", coeff_index, network->coeff_count);
        vipGoOnError(VIP_ERROR_NOT_SUPPORTED);
    }

#if vpmdPRELOAD_COEFF > 1
    if (NBGLK_COEFF_LOC_VIP_SRAM == network->coeff_property[coeff_index].location) {
        nbglk_coeff_property_t *property = &network->coeff_property[coeff_index];
        if (NBGLK_COEFF_PATH_NN_CMD == prop) {
            vip_uint32_t *nn = (vip_uint32_t*)patch_addr;
            vip_uint32_t margin = 0;
            if (VIP_NULL == nn) {
                vip_error("fail coeff preload patch nn command is NULL\n");
                vipGoOnError(VIP_ERROR_OUT_OF_MEMORY);
            }
            if (!viphal_query_hw_feature(VIP_HW_FEATURE_KENERL_DESC)) {
                margin = (weight_network->coeff_data.physical + property->src_offset +
                    network->nn_header_size) % viphal_query_hw_feature(VIP_HW_FEATURE_VIPSRAM_WIDTH);
            }
            nn[8]  = ((((vip_uint32_t) (nn[8])) & ~(((vip_uint32_t) (((vip_uint32_t) ((((1 ?
 3:2) - (0 ?
 3:2) + 1) == 32) ?
 ~0 : (~0 & ((1 << (((1 ?
 3:2) - (0 ?
 3:2) + 1))) - 1)))))) << (0 ?
 3:2))) | (((vip_uint32_t) ((vip_uint32_t) (0) & ((vip_uint32_t) ((((1 ?
 3:2) - (0 ?
 3:2) + 1) == 32) ?
 ~0 : (~0 & ((1 << (((1 ? 3:2) - (0 ? 3:2) + 1))) - 1)))))) << (0 ? 3:2)));
            /* cache start address */
            nn[11] = SIZE_CAST(vip_uint32_t, property->dst_offset);
            /* cache end address */
            nn[12] = VIP_ALIGN(property->size - network->nn_header_size + margin,
                viphal_query_hw_feature(VIP_HW_FEATURE_VIPSRAM_WIDTH)) + SIZE_CAST(vip_uint32_t, property->dst_offset);
            nn[22] = ((((vip_uint32_t) (nn[22])) & ~(((vip_uint32_t) (((vip_uint32_t) ((((1 ?
 16:16) - (0 ?
 16:16) + 1) == 32) ?
 ~0 : (~0 & ((1 << (((1 ?
 16:16) - (0 ?
 16:16) + 1))) - 1)))))) << (0 ?
 16:16))) | (((vip_uint32_t) ((vip_uint32_t) (1) & ((vip_uint32_t) ((((1 ?
 16:16) - (0 ?
 16:16) + 1) == 32) ?
 ~0 : (~0 & ((1 << (((1 ?
 16:16) - (0 ? 16:16) + 1))) - 1)))))) << (0 ? 16:16)));
        }
        else if (NBGLK_COEFF_PATH_NN_KERNEL_HEADER == prop) {
            vip_uint32_t *nn_header = (vip_uint32_t*)patch_addr;
            if (VIP_NULL == nn_header) {
                vip_error("fail coeff preload patch nn header is NULL\n");
                vipGoOnError(VIP_ERROR_OUT_OF_MEMORY);
            }
            nn_header[0] = SIZE_CAST(vip_uint32_t, property->dst_offset);
            /* bit 0: direct mode; bit 1~2: cache mode */
            nn_header[1] = (nn_header[1] | 0x1) & (~0x6);
            nn_header[2] = SIZE_CAST(vip_uint32_t, property->dst_offset);
            nn_header[3] = SIZE_CAST(vip_uint32_t, property->dst_offset) + VIP_ALIGN(property->size,
                viphal_query_hw_feature(VIP_HW_FEATURE_VIPSRAM_WIDTH));
        }
        else {
            vip_error("fail coeff preload patch nn\n");
            vipGoOnError(VIP_ERROR_NOT_SUPPORTED);
        }
    }
#endif

#if vpmdENABLE_CHANGE_MULTI_AXI_ID
    if (NBGLK_COEFF_LOC_USER_SPECIFIED_0  == network->coeff_property[coeff_index].location) {
    #define VIP_MULTI_AXI_ID_DDR_OFFSET        1
    #define VIP_MULTI_AXI_ID_SRAM_OFFSET       2
        if (1 == network->app_preload_in_axisram && NBGLK_COEFF_PATH_NN_CMD == prop) {
            vip_uint32_t *nn_cmd = (vip_uint32_t*)patch_addr;
            if (VIP_MULTI_AXI_ID_DDR_OFFSET ==  \
                (((((vip_uint32_t) (nn_cmd[27])) >> (0 ? 13:12)) & ((vip_uint32_t) ((((1 ? 13:12) - (0 ? 13:12) + 1) == 32) ? ~0 : (~0 & ((1 << (((1 ? 13:12) - (0 ? 13:12) + 1))) - 1))))) )) {
                /* change the kernel AXI-id in nn command */
                nn_cmd[27]  = ((((vip_uint32_t) (nn_cmd[27])) & ~(((vip_uint32_t) (((vip_uint32_t) ((((1 ?
 13:12) - (0 ?
 13:12) + 1) == 32) ?
 ~0 : (~0 & ((1 << (((1 ?
 13:12) - (0 ?
 13:12) + 1))) - 1)))))) << (0 ?
 13:12))) | (((vip_uint32_t) ((vip_uint32_t) (VIP_MULTI_AXI_ID_SRAM_OFFSET) & ((vip_uint32_t) ((((1 ?
 13:12) - (0 ?
 13:12) + 1) == 32) ?
 ~0 : (~0 & ((1 << (((1 ?
 13:12) - (0 ? 13:12) + 1))) - 1)))))) << (0 ? 13:12)));
            }
        }
    }
#endif

onError:
    return status;
}

/* preloaded coeff to nbglk_coeff_location_e(location) */
static vip_status_e nbglk_coeff_preload(
    vip_network_t *network,
    vip_size_t offset,
    vip_size_t total_size,
    vip_size_t *used_size,
    vip_uint32_t align_size,
    nbglk_coeff_location_e location
    )
{
    vip_status_e status = VIP_SUCCESS;
    vip_uint32_t i = 0;
    nbglk_coeff_property_t *property = VIP_NULL;
    vip_uint32_t size = 0;
    *used_size = 0;

    for (i = 0; i < network->coeff_count; i++) {
        property = &network->coeff_property[i];

        if (NBGLK_COEFF_LOC_DDR != property->location) {
            continue;
        }

        if (property->shared) {
            property->location = network->coeff_property[property->shared_index].location;
            property->src_offset = network->coeff_property[property->shared_index].src_offset;
            property->dst_offset = network->coeff_property[property->shared_index].dst_offset;
        }
        else {
            if (NBGLK_COEFF_LOC_VIP_SRAM == location &&
                (!nbglk_op_support_vipsram_preload(property->op_type))) {
                /* not support preload tp/sp/sw into vip sram yet */
                continue;
            }

            if (viphal_query_hw_feature(VIP_HW_FEATURE_KENERL_DESC) &&
                NBGLK_COEFF_LOC_VIP_SRAM == location &&
                nbglk_op_support_vipsram_preload(property->op_type) &&
                NBG_SOURCE_MISC_DYNA_GENERIC == property->src_type) {
                /* nn kernel descriptor cannot preload to vip sram */
                continue;
            }

            if (NBGLK_COEFF_LOC_VIP_SRAM == location &&
                nbglk_op_support_vipsram_preload(property->op_type) &&
                NBG_SOURCE_MISC_DYNA_GENERIC == property->src_type) {
                vip_uint32_t margin = (network->coeff_data.physical + property->src_offset +
                    network->nn_header_size) % align_size;
                size = VIP_ALIGN(property->size - network->nn_header_size + margin, align_size);
            }
            else {
                size = VIP_ALIGN(property->size, align_size);
            }
            if (NBGLK_COEFF_LOC_VIP_SRAM == location) {
                if (*used_size + size > total_size - align_size) {
                    continue;
                }
            }
            else {
                if (*used_size + size > total_size) {
                    continue;
                }
            }
            property->location = location;
            property->dst_offset = offset + *used_size;
            *used_size += size;
            vip_info("coeff_%d, preload location=%s, size=%d/%d\n",
                       i, coeff_location_str(location), size, *used_size);
        }
    }

    vip_info("preload coeff location=%s, dst addr offset=0x%lld, size %lld/%lld\n",
          coeff_location_str(location), offset, *used_size, total_size);
    return status;
}

vip_status_e nbglk_coeff_property_init(
    vip_network network
    )
{
    vip_uint32_t i = 0, j = 0;
    vip_status_e status = VIP_SUCCESS;
    vip_uint32_t coeff_count = 0;
    nbg_desc_operation_t* operation = VIP_NULL;
    nbg_desc_patch_t* pd_entry = VIP_NULL;
    nbg_desc_layer_parameter_t* layer_param = VIP_NULL;
    vip_uint32_t* flag = VIP_NULL;
    vip_uint64_t max_offset = 0;

    if (0x0001001A > network->nbg_desc.fixed.header.version) {
        return VIP_SUCCESS;
    }

    if (0x0001001C > network->nbg_desc.fixed.header.version) {
        vipOnError(viphal_allocate_memory(network->nbg_desc.n_coeff_data * sizeof(vip_uint32_t), (void**)&flag));
        viphal_os_zero_memory(flag, network->nbg_desc.n_coeff_data * sizeof(vip_uint32_t));

        for (i = 0; i < network->nbg_desc.n_operations; i++) {
            vip_uint8_t is_const = vip_false_e;
            operation = &network->nbg_desc.operations[i];

            if ((!nbg_operation_is_nn_engine(operation->type)) &&
                NBG_OPERATION_TYPE_TP != operation->type &&
                NBG_OPERATION_TYPE_SH != operation->type &&
                NBG_OPERATION_TYPE_SW != operation->type) {
                continue;
            }

            for (j = 0; j < operation->patch_count; j++) {
                vip_uint32_t pd_index = 0;
                vip_uint32_t pd_source_type = 0;
                if (NBG_OPERATION_TYPE_SW == operation->type) {
                    layer_param = &network->nbg_desc.lp_entries[operation->patch_index + j];
                    pd_index = layer_param->index;
                    pd_source_type = layer_param->source_type;
                }
                else {
                    pd_entry = &network->nbg_desc.pd_sections[operation->patch_index + j];
                    pd_index = pd_entry->index;
                    pd_source_type = pd_entry->source_type;
                }

                if (network->nbg_desc.n_coeff_data <= pd_index || flag[pd_index]) {
                    /* has record */
                    continue;
                }

                if (NBG_SOURCE_MISC_DYNA_GENERIC == pd_source_type) {
                    is_const = vip_true_e; /* the weight is in coeff data section */
                    coeff_count++;
                    flag[pd_index] = coeff_count;
                }
                else if ((NBG_SOURCE_KERNEL_BITSTREAM == pd_source_type) && (vip_true_e == is_const)) {
                    coeff_count++;
                    flag[pd_index] = coeff_count;
                }
            }
        }

        network->coeff_count = coeff_count;
        if (0 == coeff_count) {
            vip_info("coeff count is 0\n");
            vipGoOnError(VIP_SUCCESS);
        }

        vipOnError(viphal_allocate_memory(sizeof(nbglk_coeff_property_t) * coeff_count,
                                            (void**)&network->coeff_property));
        viphal_zero_memory((void*)network->coeff_property, sizeof(nbglk_coeff_property_t) * coeff_count);

        for (i = 0; i < network->nbg_desc.n_operations; i++) {
            vip_uint8_t is_const = vip_false_e;
            operation = &network->nbg_desc.operations[i];

            if ((!nbg_operation_is_nn_engine(operation->type)) &&
                NBG_OPERATION_TYPE_TP != operation->type &&
                NBG_OPERATION_TYPE_SH != operation->type &&
                NBG_OPERATION_TYPE_SW != operation->type) {
                continue;
            }

            for (j = 0; j < operation->patch_count; j++) {
                vip_uint32_t pd_index = 0;
                vip_uint32_t pd_source_type = 0;
                if (NBG_OPERATION_TYPE_SW == operation->type) {
                    layer_param = &network->nbg_desc.lp_entries[operation->patch_index + j];
                    pd_index = layer_param->index;
                    pd_source_type = layer_param->source_type;
                }
                else {
                    pd_entry = &network->nbg_desc.pd_sections[operation->patch_index + j];
                    pd_index = pd_entry->index;
                    pd_source_type = pd_entry->source_type;
                }

                if (network->nbg_desc.n_coeff_data <= pd_index || !flag[pd_index]) {
                    /* have not record */
                    continue;
                }

                if (NBG_SOURCE_MISC_DYNA_GENERIC == pd_source_type) {
                    is_const = vip_true_e; /* the weight is in coeff data section */
                }
                else if (NBG_SOURCE_KERNEL_BITSTREAM == pd_source_type) {
                    if (vip_false_e == is_const) {
                        continue;
                    }
                }
                else {
                    continue;
                }
                coeff_count = flag[pd_index] - 1;
                network->coeff_property[coeff_count].op_type = operation->type;
                network->coeff_property[coeff_count].src_type = pd_source_type;
                network->coeff_property[coeff_count].location = NBGLK_COEFF_LOC_DDR;
                network->coeff_property[coeff_count].size = network->nbg_desc.coeff_data_table[pd_index].size;
                network->coeff_property[coeff_count].src_offset =
                    network->nbg_desc.coeff_data_table[pd_index].offset;
                if (NBG_OPERATION_TYPE_SW == operation->type) {
                    layer_param->index = coeff_count;
                }
                else {
                    pd_entry->index = coeff_count;
                }
            }
        }
    }
    else {
        coeff_count = network->nbg_desc.n_coeff_data;
        if (0 == coeff_count) {
            vip_info("coeff count is 0\n");
            vipGoOnError(VIP_SUCCESS);
        }
        vipOnError(viphal_allocate_memory(sizeof(nbglk_coeff_property_t) * coeff_count,
            (void**)&network->coeff_property));
        viphal_zero_memory((void*)network->coeff_property, sizeof(nbglk_coeff_property_t) * coeff_count);
        network->coeff_count = coeff_count;

        for (i = 0; i < network->nbg_desc.n_operations; i++) {
            operation = &network->nbg_desc.operations[i];

            if ((!nbg_operation_is_nn_engine(operation->type)) &&
                NBG_OPERATION_TYPE_TP != operation->type &&
                NBG_OPERATION_TYPE_SH != operation->type &&
                NBG_OPERATION_TYPE_SW != operation->type) {
                continue;
            }

            for (j = 0; j < operation->patch_count; j++) {
                vip_uint32_t pd_index = 0;
                vip_uint32_t pd_source_type = 0;
                if (NBG_OPERATION_TYPE_SW == operation->type) {
                    layer_param = &network->nbg_desc.lp_entries[operation->patch_index + j];
                    pd_index = layer_param->index;
                    pd_source_type = layer_param->source_type;
                }
                else {
                    pd_entry = &network->nbg_desc.pd_sections[operation->patch_index + j];
                    pd_index = pd_entry->index;
                    pd_source_type = pd_entry->source_type;
                }

                if (!(NBG_SOURCE_MISC_DYNA_GENERIC == pd_source_type ||
                    NBG_SOURCE_KERNEL_BITSTREAM == pd_source_type)) {
                    continue;
                }
                if (nbglk_op_support_vipsram_preload(operation->type) &&
                    NBG_SOURCE_MISC_DYNA_GENERIC == pd_source_type &&
                    viphal_query_hw_feature(VIP_HW_FEATURE_KENERL_DESC)) {
                    continue;
                }
                if (network->coeff_property[pd_index].size) {
                    /* has recorded */
                    continue;
                }
                network->coeff_property[pd_index].op_type = operation->type;
                network->coeff_property[pd_index].src_type = pd_source_type;
                network->coeff_property[pd_index].location = NBGLK_COEFF_LOC_DDR;
                network->coeff_property[pd_index].size = network->nbg_desc.coeff_data_table[pd_index].size;
                network->coeff_property[pd_index].src_offset = network->nbg_desc.coeff_data_table[pd_index].offset;
            }
        }
    }

    /* mark shared coeff */
    max_offset = network->coeff_property[0].src_offset;
    for (i = 1; i < network->coeff_count; i++) {
        if (network->coeff_property[i].src_offset <= max_offset) {
            for (j = 0; j < i; j++) {
                if (network->coeff_property[i].src_offset == network->coeff_property[j].src_offset) {
                    network->coeff_property[i].shared = 1;
                    network->coeff_property[i].shared_index = j;
                }
            }
        }
        else {
            max_offset = network->coeff_property[i].src_offset;
        }
    }

    if (!viphal_query_hw_feature(VIP_HW_FEATURE_KENERL_DESC)) {
        network->nn_header_size = VIP_ALIGN(viphal_query_hw_feature(VIP_HW_FEATURE_NN_CORE_CNT) *
            4 + 28, HEADER_ALIGN_SIZE);
    }
onError:
    if (VIP_NULL != flag) {
        viphal_free_memory(flag);
    }
    return status;
}

vip_status_e nbglk_coeff_cpu_preload(
    vip_network network
    )
{
    vip_status_e status = VIP_SUCCESS;
    vip_size_t total_size = network->app_preload_coeff->size;
    vip_size_t used_size = 0;
    vip_uint8_t *preload_logical = network->app_preload_coeff->logical;
    vip_uint32_t i = 0;
    nbglk_videomemory_t tmp_coeff = { 0 };
    nbglk_coeff_property_t *property = VIP_NULL;

    if (network->nbg_desc.fixed.header.version < 0x0001001A) {
        vip_error("fail not support this feature nbg version=0x%08x\n", network->nbg_desc.fixed.header.version);
        vipGoOnError(VIP_ERROR_NOT_SUPPORTED);
    }
    if (network->dup_type != NBGLK_DUP_NONE) {
        vip_error("fail dup network can't support preload coeff dup_type=0x%x\n", network->dup_type);
        vipGoOnError(VIP_ERROR_NOT_SUPPORTED);
    }
    if ((network->app_preload_coeff->physical % 64) != 0) {
        vip_error("fail cpu preload coeff physical=0x%x not align\n", network->app_preload_coeff->physical);
        vipGoOnError(VIP_ERROR_FAILURE);
    }

    vipOnError(nbglk_coeff_preload(network, 0, total_size, &used_size,
        vipdMEMORY_ALIGN_SIZE, NBGLK_COEFF_LOC_USER_SPECIFIED_0));

    if (0 == used_size) {
        vip_error("not preload coeff, the memory size=%d is small\n", total_size);
        vipGoOnError(status);
    }

    if (network->coeff_data.size - used_size) {
        tmp_coeff.size = network->coeff_data.size - used_size;
        vipOnError(viphal_allocate_videomemory(&tmp_coeff.logical, &tmp_coeff.mem_id, &tmp_coeff.physical,
            tmp_coeff.size, vipdSH_ALIGN_SIZE, VIPDRV_VIDEO_MEM_ALLOC_NONE));
    }
    vip_info("cpu preload coeff memory size=%d, preload size=%d, network total weight size=%d\n",
                total_size, used_size, network->nbg_desc.fixed.coeff_data.size);

    used_size = 0;
    for (i = 0; i < network->coeff_count; i++) {
        property = &network->coeff_property[i];
        if (property->shared) {
            property->location = network->coeff_property[property->shared_index].location;
            property->src_offset = network->coeff_property[property->shared_index].src_offset;
            property->dst_offset = network->coeff_property[property->shared_index].dst_offset;
            continue;
        }
        if (NBGLK_COEFF_LOC_USER_SPECIFIED_0 == property->location) {
            viphal_copy_memory(preload_logical + property->dst_offset,
                               network->coeff_data.logical + property->src_offset,
                               property->size);
            property->src_offset = property->dst_offset;
            property->dst_offset = 0;
        }
        else {
            viphal_copy_memory(tmp_coeff.logical + used_size,
                               network->coeff_data.logical + property->src_offset,
                               property->size);
            /* update src_offset for new coeff buffer on DDR */
            property->src_offset = used_size;
            used_size += VIP_ALIGN(property->size, vipdMEMORY_ALIGN_SIZE);
        }
    }

    if (network->coeff_data.mem_id != 0) {
        viphal_free_videomemory(network->coeff_data.mem_id);
        network->coeff_data.mem_id = 0;
        vip_info("free orignal coeff data memory\n");
    }
    if (0 != tmp_coeff.mem_id) {
        /* update coeff data to new coeff buffer after preload user specify memory done */
        network->coeff_data.mem_id = tmp_coeff.mem_id;
        network->coeff_data.physical = tmp_coeff.physical;
        network->coeff_data.logical = tmp_coeff.logical;
        network->coeff_data.size = tmp_coeff.size;

        if (0 == network->coeff_header.mem_id) {
            network->coeff_header.logical = network->coeff_data.logical;
            network->coeff_header.physical = network->coeff_data.physical;
            network->coeff_header.size = network->coeff_data.size;
        }
    }

#if vpmdENABLE_CHANGE_MULTI_AXI_ID
    {
        /* check the application specify coeff memory in AXI-SRAM or not */
        nbglk_context_t *context = network->context;
        vip_uint32_t axi_sram_base = context->axi_sram_base[0];
        vip_uint32_t axi_sram_size = context->axi_sram_size[0];
        vip_uint32_t physical = network->app_preload_coeff->physical;
        for (i = 1; i < context->device_count; i++) {
            if (context->axi_sram_base[i] < axi_sram_base) {
                axi_sram_base = context->axi_sram_base[i];
            }
            axi_sram_size += context->axi_sram_size[i];
        }
        if ((physical < (axi_sram_base + axi_sram_size)) && (physical >= axi_sram_base)) {
            network->app_preload_in_axisram = 1;
        }
    }
#endif

#if vpmdPRELOAD_COEFF > 1
    if (network->preload_cmd.mem_id != 0) {
        /* Should relocation coeff if these coeff has been preload to VIP-SRAM */
        vipOnError(nbglk_coeff_npu_preload_init(network));
    }
#endif

    return status;
onError:
    vip_error("fail cpu preload coeff status=%d\n", status);
    return status;
#undef CHECK_AXI_WRITE_COUNT
}

#define OND_CMD_MAX_COPY_SIZE       (64*1024)
#define PRELOAD_ONE_CMD_SIZE        (10*sizeof(vip_uint32_t))
#define PRELOAD_ONE_INSTR_SIZE      (nbg_get_nn_command_size(&network->nbg_desc))
#define PRELOAD_MEM_ALIGN           64

#if vpmdPRELOAD_COEFF > 1
static vip_status_e npu_preload_in_uninit(
    vip_network network
    )
{
    if (network->preload_cmd.mem_id != 0) {
        viphal_free_videomemory(network->preload_cmd.mem_id);
        network->preload_cmd.mem_id = 0;
    }
    if (network->preload_instr.mem_id != 0) {
        viphal_free_videomemory(network->preload_instr.mem_id);
        network->preload_instr.mem_id = 0;
    }
    network->preload_cmd_size = 0;
    viphal_task_set_property(network->task_id, VIPDRV_TASK_CMD_PRELOAD, 0, 0, 0, 0);

    return VIP_SUCCESS;
}

/* copy data from DDR to VIP-SRAM */
static vip_status_e npu_preload_in_init(
    vip_network network
    )
{
    vip_status_e status = VIP_SUCCESS;
    nbglk_context_t *context = network->context;
    vip_uint32_t split_count = 0;
    vip_uint32_t i = 0, j = 0;
    vip_uint8_t *preload_instr_logical = VIP_NULL;
    vip_uint8_t *preload_cmd_logical = VIP_NULL;
    vip_address_t preload_instr_addr = 0;
    vip_uint32_t total_cmd_size = 0;
    vip_uint32_t size = 0;

    for (i = 0; i < network->coeff_count; i++) {
        nbglk_coeff_property_t* property = &network->coeff_property[i];
        if (NBGLK_COEFF_LOC_VIP_SRAM == property->location) {
            if (nbglk_op_support_vipsram_preload(property->op_type) &&
                NBG_SOURCE_MISC_DYNA_GENERIC == property->src_type) {
                size = VIP_ALIGN(property->size - network->nn_header_size,
                    viphal_query_hw_feature(VIP_HW_FEATURE_VIPSRAM_WIDTH));
            }
            else {
                size = VIP_ALIGN(property->size, viphal_query_hw_feature(VIP_HW_FEATURE_VIPSRAM_WIDTH));
            }
            split_count += (size + OND_CMD_MAX_COPY_SIZE - 1) / OND_CMD_MAX_COPY_SIZE;
        }
    }

    vip_info("split count=%d\n", split_count);
    if (0 == split_count) {
        vip_error("no coeff can be preloaded into VIP SRAM\n");
        return VIP_ERROR_FAILURE;
    }

    network->preload_instr.size = PRELOAD_ONE_INSTR_SIZE * split_count;
    vipOnError(viphal_allocate_videomemory((vip_uint8_t **)&network->preload_instr.logical,
                                                &network->preload_instr.mem_id,
                                                &network->preload_instr.physical,
                                                network->preload_instr.size, vipdMEMORY_ALIGN_SIZE,
                                                VIPDRV_VIDEO_MEM_ALLOC_4GB_ADDR));
    vip_info("preload instr logical=%p, physical=0x%x, size=0x%x\n",
        network->preload_instr.logical, network->preload_instr.physical, network->preload_instr.size);

    network->preload_cmd.size = PRELOAD_ONE_CMD_SIZE * split_count + LINK_SIZE;
    vipOnError(viphal_allocate_videomemory((vip_uint8_t **)&network->preload_cmd.logical,
                                                &network->preload_cmd.mem_id,
                                                &network->preload_cmd.physical,
                                                network->preload_cmd.size, vipdMEMORY_ALIGN_SIZE,
                                                VIPDRV_VIDEO_MEM_ALLOC_MAP_KERNEL |
                                                VIPDRV_VIDEO_MEM_ALLOC_4GB_ADDR));
    viphal_zero_memory(network->preload_cmd.logical, network->preload_cmd.size);
    vip_info("preload cmd logical=%p, physical=0x%x, size=0x%x\n",
        network->preload_cmd.logical, network->preload_cmd.physical, network->preload_cmd.size);

    preload_cmd_logical = network->preload_cmd.logical;
    split_count = 0;
    for (i = 0; i < network->coeff_count; i++) {
        nbglk_coeff_property_t *property = &network->coeff_property[i];
        if (NBGLK_COEFF_LOC_VIP_SRAM == property->location) {
            vip_address_t coeff_phy = 0;
            vip_uint32_t margin = 0;
            if (nbglk_op_support_vipsram_preload(property->op_type) &&
                NBG_SOURCE_MISC_DYNA_GENERIC == property->src_type) {
                coeff_phy = network->coeff_data.physical + property->src_offset +
                    network->nn_header_size;
                margin = coeff_phy % viphal_query_hw_feature(VIP_HW_FEATURE_VIPSRAM_WIDTH);
                size = VIP_ALIGN(property->size - network->nn_header_size,
                    viphal_query_hw_feature(VIP_HW_FEATURE_VIPSRAM_WIDTH));
            }
            else {
                size = VIP_ALIGN(property->size, viphal_query_hw_feature(VIP_HW_FEATURE_VIPSRAM_WIDTH));
                coeff_phy = network->coeff_data.physical + property->src_offset;
            }
            for (j = 0; j < (property->size + OND_CMD_MAX_COPY_SIZE - 1) / OND_CMD_MAX_COPY_SIZE; j++) {
                vip_uint32_t instr_size = 0;
                vip_uint32_t cmd_size = 0;
                vip_uint32_t cmd_preload_size = size > OND_CMD_MAX_COPY_SIZE ? OND_CMD_MAX_COPY_SIZE : size;
                vip_address_t src_address_tmp = coeff_phy + j * OND_CMD_MAX_COPY_SIZE;
                vip_uint32_t dst_address_tmp = context->vip_sram_base + SIZE_CAST(vip_uint32_t, property->dst_offset) +
                    margin + j * OND_CMD_MAX_COPY_SIZE;
                preload_instr_addr = network->preload_instr.physical + split_count * PRELOAD_ONE_INSTR_SIZE;
                preload_instr_logical = network->preload_instr.logical + split_count * PRELOAD_ONE_INSTR_SIZE;

                vipOnError(viphal_hardware_trsp2_instr(preload_instr_logical, &instr_size,
                    SIZE_CAST(vip_uint32_t, src_address_tmp),
                    dst_address_tmp, cmd_preload_size, VIPHAL_HARDWARE_TRSP2_DDR_TO_VIPSRAM));
                if (instr_size > PRELOAD_ONE_INSTR_SIZE) {
                    vip_error("fail preload in instr size more that max value\n");
                    vipGoOnError(VIP_ERROR_FAILURE);
                }

                /* preload command */
                vipOnError(viphal_hardware_trsp2_loadstate(preload_cmd_logical,
                    &cmd_size, SIZE_CAST(vip_uint32_t, preload_instr_addr)));
                if (cmd_size > PRELOAD_ONE_CMD_SIZE) {
                    vip_error("fail preload in cmd size more that max value\n");
                    vipGoOnError(VIP_ERROR_FAILURE);
                }
                preload_cmd_logical += cmd_size;

                split_count++;
                size -= OND_CMD_MAX_COPY_SIZE;
            }
        }
    }

    /* eache vip core should be prload for multi-vip. TBD */
    total_cmd_size = (vip_uint32_t)(preload_cmd_logical - network->preload_cmd.logical);
    network->preload_cmd_size = total_cmd_size;
    vip_info("preload in cmd logical=%p, physical=0x%x, size=0x%x\n",
        network->preload_cmd.logical, network->preload_cmd.physical, network->preload_cmd_size);

#if vpmdENABLE_VIDEO_MEMORY_CACHE
    if (network->preload_cmd.size > 0) {
        vipOnError(viphal_flush_cache(network->preload_cmd.mem_id, VIPDRV_CACHE_FLUSH));
    }
    if (network->preload_instr.size > 0) {
        vipOnError(viphal_flush_cache(network->preload_instr.mem_id, VIPDRV_CACHE_FLUSH));
    }
#endif
    vipOnError(viphal_task_set_property(network->task_id, VIPDRV_TASK_CMD_PRELOAD,
              0, network->preload_cmd.mem_id, network->preload_cmd_size, 0));

    return status;
onError:
    vip_error("fail to preload in init status=%d\n", status);
    return status;
}

#if 0
/* copy data from VIP-SRAM to DDR, for debug */
vip_status_e nbglk_coeff_npu_preload_out(
    vip_network network
    )
{
    vip_status_e status = VIP_SUCCESS;
    nbglk_context_t *context = network->context;
    vip_uint32_t split_count = 0;
    vip_uint32_t i = 0, j = 0;
    vip_uint32_t device_index = 0;
    vip_uint8_t *preload_instr_logical = VIP_NULL;
    vip_uint8_t *preload_cmd_logical = VIP_NULL;
    vip_uint32_t preload_instr_addr = 0;
    vip_uint32_t total_cmd_size = 0;
    vip_uint32_t size = 0;
    nbglk_videomemory_t preload_instr = { 0 };
    nbglk_videomemory_t preload_cmd = { 0 };
    viphal_task_param_t param = { 0 };
    nbglk_subtask_param_t cmd = { 0 };
    vip_bool_e with_event = !vpmdENABLE_POLLING;

    device_index = network->device->dev_index;
    for (i = 0; i < network->coeff_count; i++) {
        nbglk_coeff_property_t* property = &network->coeff_property[i];
        if (NBGLK_COEFF_LOC_VIP_SRAM == property->location) {
            if (nbglk_op_support_vipsram_preload(property->op_type) &&
                NBG_SOURCE_MISC_DYNA_GENERIC == property->src_type) {
                size = VIP_ALIGN(property->size - network->nn_header_size,
                    viphal_query_hw_feature(VIP_HW_FEATURE_VIPSRAM_WIDTH));
            }
            else {
                size = VIP_ALIGN(property->size, viphal_query_hw_feature(VIP_HW_FEATURE_VIPSRAM_WIDTH));
            }
            split_count += (size + OND_CMD_MAX_COPY_SIZE - 1) / OND_CMD_MAX_COPY_SIZE;
        }
    }

    vip_info("split count=%d\n", split_count);
    if (0 == split_count) {
        vip_error("no coeff can be preloaded out to DDR\n");
        return VIP_ERROR_FAILURE;
    }

    preload_instr.size = PRELOAD_ONE_INSTR_SIZE * split_count;
    vipOnError(viphal_allocate_videomemory((vip_uint8_t **)&preload_instr.logical,
                                                &preload_instr.mem_id,
                                                &preload_instr.physical,
                                                preload_instr.size, vipdMEMORY_ALIGN_SIZE,
                                                VIPDRV_VIDEO_MEM_ALLOC_4GB_ADDR));
    viphal_zero_memory(preload_instr.logical, preload_instr.size);
    vip_info("preload instr out logical=%p, physical=0x%x, size=0x%x\n",
        preload_instr.logical, preload_instr.physical, preload_instr.size);

    preload_cmd.size = PRELOAD_ONE_CMD_SIZE * split_count + APPEND_COMMAND_SIZE;
    vipOnError(viphal_allocate_videomemory((vip_uint8_t **)&preload_cmd.logical,
                                                &preload_cmd.mem_id,
                                                &preload_cmd.physical,
                                                preload_cmd.size, vipdMEMORY_ALIGN_SIZE,
                                                VIPDRV_VIDEO_MEM_ALLOC_MAP_KERNEL |
                                                VIPDRV_VIDEO_MEM_ALLOC_4GB_ADDR));
    viphal_zero_memory(preload_cmd.logical, preload_cmd.size);
    vip_info("preload cmd out logical=%p, physical=0x%x, size=0x%x\n",
        preload_cmd.logical, preload_cmd.physical, preload_cmd.size);

    preload_cmd_logical = preload_cmd.logical;
    split_count = 0;
    for (i = 0; i < network->coeff_count; i++) {
        nbglk_coeff_property_t *property = &network->coeff_property[i];
        if (NBGLK_COEFF_LOC_VIP_SRAM == property->location) {
            vip_uint32_t coeff_phy = 0;
            vip_uint32_t margin = 0;
            if (nbglk_op_support_vipsram_preload(property->op_type) &&
                NBG_SOURCE_MISC_DYNA_GENERIC == property->src_type) {
                size = VIP_ALIGN(property->size - network->nn_header_size,
                    viphal_query_hw_feature(VIP_HW_FEATURE_VIPSRAM_WIDTH));
                coeff_phy = network->coeff_data.physical + property->src_offset + network->nn_header_size;
                margin = coeff_phy % viphal_query_hw_feature(VIP_HW_FEATURE_VIPSRAM_WIDTH);
            }
            else {
                size = VIP_ALIGN(property->size, viphal_query_hw_feature(VIP_HW_FEATURE_VIPSRAM_WIDTH));
                coeff_phy = network->coeff_data.physical + property->src_offset;
            }
            for (j = 0; j < (property->size + OND_CMD_MAX_COPY_SIZE - 1) / OND_CMD_MAX_COPY_SIZE; j++) {
                vip_uint32_t instr_size = 0;
                vip_uint32_t cmd_size = 0;
                vip_uint32_t cmd_preload_size = size > OND_CMD_MAX_COPY_SIZE ? OND_CMD_MAX_COPY_SIZE : size;
                vip_uint32_t dst_address_tmp = coeff_phy + j * OND_CMD_MAX_COPY_SIZE;
                vip_uint32_t src_address_tmp = context->vip_sram_base + property->dst_offset + margin +
                    j * OND_CMD_MAX_COPY_SIZE;
                preload_instr_addr = preload_instr.physical + split_count * PRELOAD_ONE_INSTR_SIZE;
                preload_instr_logical = preload_instr.logical + split_count * PRELOAD_ONE_INSTR_SIZE;

                vipOnError(viphal_hardware_trsp2_instr(preload_instr_logical, &instr_size, src_address_tmp,
                    dst_address_tmp, cmd_preload_size,
                    VIPHAL_HARDWARE_TRSP2_VIPSRAM_TO_DDR));
                if (instr_size > PRELOAD_ONE_INSTR_SIZE) {
                    vip_error("fail preload out instr size more that max value\n");
                    vipGoOnError(VIP_ERROR_FAILURE);
                }

                /* preload command */
                vipOnError(viphal_hardware_trsp2_loadstate(preload_cmd_logical, &cmd_size, preload_instr_addr));
                if (cmd_size > PRELOAD_ONE_CMD_SIZE) {
                    vip_error("fail preload out cmd size more that max value\n");
                    vipGoOnError(VIP_ERROR_FAILURE);
                }
                preload_cmd_logical += cmd_size;

                split_count++;
                size -= OND_CMD_MAX_COPY_SIZE;
            }
        }
    }

    /* eache vip core should be prload for multi-vip. TBD */
    total_cmd_size = preload_cmd_logical - preload_cmd.logical;
    status = nbglk_append_cmds(&preload_cmd, &total_cmd_size, with_event,
        network->nbg_desc.net_info.core_count > 1 ? vip_true_e : vip_false_e,
        network->device->core_id[network->core_index]);
    vip_info("preload out cmd logical=%p, physical=0x%x, size=0x%x\n",
        preload_cmd.logical, preload_cmd.physical, total_cmd_size);

#if vpmdENABLE_VIDEO_MEMORY_CACHE
    if (preload_cmd.size > 0) {
        vipOnError(viphal_flush_cache(preload_cmd.mem_id, VIPDRV_CACHE_FLUSH));
    }
    if (preload_instr.size > 0) {
        vipOnError(viphal_flush_cache(preload_instr.mem_id, VIPDRV_CACHE_FLUSH));
    }
#endif

    if (0 == total_cmd_size) {
        return VIP_SUCCESS;
    }

    vip_info("npu preload out coeff, network=%p\n", network);
    cmd.logical = preload_cmd.logical;
    cmd.physical = preload_cmd.physical;
    cmd.subtask_id = preload_cmd.handle_id;
    cmd.size = total_cmd_size;
    param.task_id = preload_cmd.handle_id;
    param.cmds = &cmd;
    param.cmd_count = 1;
    param.priority = network->priority;
    param.device_index = device_index;
    param.core_index = network->core_index;
    param.core_cnt = network->nbg_desc.net_info.core_count;
    param.time_out = network->wait_timeout;
    status = viphal_submit_task(&param);
    if (status != VIP_SUCCESS) {
        vip_error("fail to npu preload out coeff, status=%d.\n", status);
        vipGoOnError(status);
    }

    status = viphal_wait_task(preload_cmd.handle_id);
    if (status != VIP_SUCCESS) {
        vip_error("fail to wait npu preload out coeff, status=%d.\n", status);
        vipGoOnError(status);
    }

onError:
    if (preload_cmd.mem_id != 0) {
        viphal_free_videomemory(preload_cmd.mem_id);
        preload_cmd.mem_id = 0;
    }
    if (preload_instr.mem_id != 0) {
        viphal_free_videomemory(preload_instr.mem_id);
        preload_instr.mem_id = 0;
    }

    return status;
}
#endif

vip_status_e nbglk_coeff_npu_preload_init(
    vip_network network
    )
{
    vip_status_e status = VIP_SUCCESS;
    nbglk_context_t *context = network->context;
    vip_uint32_t preload_mem_size = 0;
    vip_uint32_t preload_vipsram_start = 0;
    vip_size_t used_size = 0;

    if (!viphal_query_hw_feature(VIP_HW_FEATURE_VIPSRAM_WIDTH)) {
        vip_info("no vip sram\n");
        return VIP_SUCCESS;
    }

    if (network->nbg_desc.fixed.header.version < 0x0001001A) {
        vip_info("not support npu preload coeff nbg version=0x%08x\n", network->nbg_desc.fixed.header.version);
        return VIP_SUCCESS;
    }

    if (!viphal_query_hw_feature(VIP_HW_FEATURE_NN_TRANS_PHASE2)) {
        vip_info("not support nn transpose phase2 for preloading coeff\n");
        return VIP_SUCCESS;
    }

    if (0 == network->coeff_data.size) {
        network->preload_cmd_size = 0;
        return VIP_SUCCESS;
    }

    /* may has been enabled preload, need preload again */
    nbglk_coeff_npu_preload_uninit(network);

    preload_vipsram_start = VIP_ALIGN(network->nbg_desc.mem_info.vip_sram_size,
        viphal_query_hw_feature(VIP_HW_FEATURE_VIPSRAM_WIDTH));
    preload_vipsram_start = preload_vipsram_start > OND_CMD_MAX_COPY_SIZE * 2 ?
                            preload_vipsram_start : OND_CMD_MAX_COPY_SIZE * 2;
    if (context->vip_sram_size <= preload_vipsram_start) {
        vip_info("not support preload to vipsram, size is not enough\n");
        return VIP_SUCCESS;
    }

    preload_mem_size = context->vip_sram_size - preload_vipsram_start;
    vipOnError(nbglk_coeff_preload(network, preload_vipsram_start, preload_mem_size, &used_size,
        viphal_query_hw_feature(VIP_HW_FEATURE_VIPSRAM_WIDTH), NBGLK_COEFF_LOC_VIP_SRAM));

    vip_info("the rest vipsram size=0x%x, preload use size0x%x\n", preload_mem_size, used_size);

    status = npu_preload_in_init(network);
    if (status != VIP_SUCCESS) {
        vip_error("fail to preload in init status=%d\n", status);
        vipGoOnError(status);
    }
onError:
    return status;
}

vip_status_e nbglk_coeff_npu_preload_uninit(
    vip_network network
    )
{
    vip_uint32_t i = 0;
    nbglk_coeff_property_t* property = VIP_NULL;
    for (i = 0; i < network->coeff_count; i++) {
        property = &network->coeff_property[i];
        if (property->shared) {
            property->location = network->coeff_property[property->shared_index].location;
            property->src_offset = network->coeff_property[property->shared_index].src_offset;
            property->dst_offset = network->coeff_property[property->shared_index].dst_offset;
            continue;
        }
        if (NBGLK_COEFF_LOC_VIP_SRAM == property->location) {
            /* do not map to vip sram */
            property->location = NBGLK_COEFF_LOC_DDR;
            property->dst_offset = 0;
        }
    }

    npu_preload_in_uninit(network);
    return VIP_SUCCESS;
}
#endif
#endif
