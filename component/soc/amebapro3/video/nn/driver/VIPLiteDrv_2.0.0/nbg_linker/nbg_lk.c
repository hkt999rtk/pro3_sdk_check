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

#include <nbg_lk.h>
#include <nbg_lk_network.h>
#include <nbg_lk_context.h>
#include <nbg_lk_debug.h>
#include <vip_hal.h>
#include <vip_lite.h>


/***** API implementations. *****/
/*
@ brief Get viplite driver version.
*/
VIP_API
vip_uint32_t vip_get_version(void)
{
    vip_uint32_t version = (VERSION_MAJOR << 16) | (VERSION_MINOR << 8) | (VERSION_SUB_MINOR);
    return version;
}

/*
@ brief Init vip lite engine.
*/
VIP_API
vip_status_e vip_init(void)
{
    vip_status_e status = VIP_SUCCESS;
    /* You can config both sys_mem_size and video_mem_size there by your requirement.
       No need to care about there two paramters on Linux system.
       Because configure in gc_vip_platform_config.h file */
    vip_uint32_t sys_mem_size = 0;
    vip_uint32_t video_mem_size = 0;
    VIP_FUNC_ENTER
    status = nbglk_init(sys_mem_size, video_mem_size);
    VIP_FUNC_EXIT
    return status;
}

/*
@ brief Destroy vip lite engine.
*/
VIP_API
vip_status_e vip_destroy(void)
{
    vip_status_e status = VIP_SUCCESS;
    VIP_FUNC_ENTER
    status = nbglk_destroy();
    VIP_FUNC_EXIT
    return status;
}

/*
@ brief Queries hardware caps information. This function shold be called after calling vip_init.
@ param property, the query property enum.
@ param size, the size of value buffer.
@ param value, the value buffer of returns.
*/
VIP_API
vip_status_e vip_query_hardware(
    IN vip_query_hardware_property_e property,
    IN vip_uint32_t size,
    OUT void *value
    )
{
    #define ELSE_SIZE_NOT_MATCH()       \
        else {                          \
            vip_error("fail to query hw property=%d, size=%d\n", property, size); \
        }
    vip_status_e status = VIP_SUCCESS;
    nbglk_context_t *context = nbglk_get_context();
    vip_int32_t *ptr_32 = (vip_int32_t *)value;
    VIP_FUNC_ENTER
    vipIsNULL(value);

    if (context->initialize <= 0) {
        vip_error("not support please call vip_init first\n");
        return VIP_ERROR_NOT_SUPPORTED;
    }

    switch (property) {
    case VIP_QUERY_HW_PROP_CID:
        if (size >= sizeof(vip_uint32_t)) {
            *ptr_32 = context->hw_target;
        }
        ELSE_SIZE_NOT_MATCH();
        break;
    case VIP_QUERY_HW_PROP_DEVICE_COUNT:
        if (size >= sizeof(vip_uint32_t)) {
            *ptr_32 = context->device_count;
        }
        ELSE_SIZE_NOT_MATCH();
        break;
    case VIP_QUERY_HW_PROP_CORE_COUNT_EACH_DEVICE:
        if (size >= (sizeof(vip_uint32_t) * context->device_count)) {
            vip_uint32_t i = 0;
            for (i = 0; i < context->device_count; i++) {
                ptr_32[i] = context->devices[i].core_count;
            }
        }
        ELSE_SIZE_NOT_MATCH();
        break;
    default:
        vip_error("doesn't support this property=%d\n", property);
        status = VIP_ERROR_INVALID_ARGUMENTS;
        break;
    }

onError:
    VIP_FUNC_EXIT
    return status;
}

/*
@brief  Create a network object from the given binary data.
@param  data, NBG file path or the buffer of NBG. different data according to type(vip_create_network_type_e).
@param  size_of_data, the bytes size of data. the byte size of NBG buffer.
@param  type, vip_create_network_type_e.
@param  network, return network object which created by NBG.
*/
VIP_API
vip_status_e vip_create_network(
    IN const void *data,
    IN vip_uint32_t size_of_data,
    IN vip_create_network_type_e type,
    OUT vip_network *network
    )
{
    vip_network nw = VIP_NULL;
    vip_status_e status = VIP_SUCCESS;
    NBGLK_CHECK_NETWORK();
    VIP_FUNC_ENTER

    if (VIP_NULL == data) {
        vip_error("network data is NULL\n");
        VIP_FUNC_EXIT
        return VIP_ERROR_INVALID_ARGUMENTS;
    }

    status = viphal_allocate_memory(sizeof(vip_network_t), (void **)&nw);
    if (status != VIP_SUCCESS) {
        vip_error("fail to allocate memory for network object\n");
        VIP_FUNC_EXIT
        return status;
    }
    viphal_zero_memory(nw, sizeof(vip_network_t));
    nw->object.type = NBGLK_OBJ_NETWORK;
    nw->context = nbglk_get_context();
    nw->dup_type = NBGLK_DUP_NONE;
    nw->create_type = type;
    nw->need_schedule = vip_true_e;
    /* network default works on device0 */
    nw->device = &nw->context->devices[0];

    if (VIP_CREATE_NETWORK_FROM_FILE & type) {
        viphal_file_t fp = viphal_os_open_file((vip_char_t*)data, FILE_READ);
        vip_info("create network from nbg file path=%s\n", data);
        if (fp != VIP_NULL) {
            status = nbg_create_network_from_file(fp, nw);
            viphal_os_close_file(fp);
        }
        else {
            vip_error("fail to open NBG file=%s\n", (vip_char_t*)data);
            vipGoOnError(VIP_ERROR_INVALID_NETWORK);
        }
    }
    else if (VIP_CREATE_NETWORK_FROM_MEMORY & type) {
        status = nbg_create_network_from_memory(nw, data, size_of_data);
    }
    else if (VIP_CREATE_NETWORK_FROM_FLASH & type) {
        status = nbg_create_network_from_memory(nw, data, size_of_data);
    }
    else {
        vip_error("failed, not support this create network type=0x%x\n", type);
        vipGoOnError(VIP_ERROR_INVALID_NETWORK);
    }

    vip_info("create network=%p\n", nw);

    if (VIP_SUCCESS == status) {
        *network = nw;
    }
    else {
        vip_error("fail to create network status=%d\n", status);
        vipGoOnError(VIP_ERROR_INVALID_NETWORK);
    }
    VIP_FUNC_EXIT
    return status;

onError:
    *network = VIP_NULL;
    nbglk_destroy_network(nw);
    viphal_free_memory(nw);
    VIP_FUNC_EXIT
    return status;
}

/*
@brief  Duplicate a network for sharing weight.
        The original network can't be destroy if the dup network is running or will be run later.
@param  data, NBG file path or the buffer of NBG.
              different data according to type(vip_dup_network_type_e) is from file/memory/flash.
              The data object can be set to VIP_NULL when type is VIP_DUP_NETWORK_FROM_NETWORK.
@param  size_of_data, the bytes size of data. the byte size of NBG buffer.
        the size can be set to 0 when data param is NBG file path.
@param  type, vip_dup_network_type_e.
*\param  network, original network to be dup.
*\param  dup_network, network object which created by duplicated.
*/
VIP_API
vip_status_e vip_dup_network(
    IN const void *data,
    IN vip_uint32_t size_of_data,
    IN vip_dup_network_type_e type,
    IN vip_network network,
    OUT vip_network *dup_network
    )
{
    vip_status_e status = VIP_SUCCESS;
    vip_network nw = VIP_NULL;
    NBGLK_CHECK_NETWORK();
    NBGLK_INITIALIZE_CHECK(network);
    VIP_FUNC_ENTER

    if (!network || !dup_network) {
        vip_error("unexpected null inputs.\n");
        return VIP_ERROR_INVALID_ARGUMENTS;
    }

    if (NBGLK_DUP_NONE != network->dup_type) {
        vip_error("fail original network can't be dup network.it must be created by vip_create_network\n");
        vipGoOnError(VIP_ERROR_NOT_SUPPORTED);
    }

    if ((type & VIP_DUP_FOR_CMD_BY_NBG) && network->nbg_desc.fixed.header.version < 0x00010018) {
        vip_error("fail nbg version is 0x%08x, not support dup cmd\n", network->nbg_desc.fixed.header.version);
        vipGoOnError(VIP_ERROR_NOT_SUPPORTED);
    }

    if ((type & VIP_DUP_FOR_CMD_BY_NETWORK) && vip_false_e == network->prepared_network) {
        vip_error("fail to dup network, please prepare original network first\n");
        vipGoOnError(VIP_ERROR_NOT_SUPPORTED);

    }

    status = viphal_allocate_memory(sizeof(vip_network_t), (void **)&nw);
    if (status != VIP_SUCCESS) {
        vip_error("fail to allocate memory for network object\n");
        VIP_FUNC_EXIT
        return status;
    }
    viphal_zero_memory(nw, sizeof(vip_network_t));
    nw->object.type = NBGLK_OBJ_NETWORK;
    nw->context = nbglk_get_context();
    nw->friend_network = network;
    /* network default works on device0 */
    nw->device = &nw->context->devices[0];

    if (type & VIP_DUP_FOR_CMD_BY_NBG) {
        nw->dup_type = NBGLK_FROM_NBG_DUP_COMMAND;
        if (type & VIP_DUP_FROM_NBG_FILE) {
            viphal_file_t fp = viphal_os_open_file((vip_char_t*)data, FILE_READ);
            if (fp != VIP_NULL) {
                status = nbg_create_network_from_file(fp, nw);
                viphal_os_close_file(fp);
            }
            else {
                vip_error("fail to open NBG file=%s\n", (vip_char_t*)data);
                vipGoOnError(VIP_ERROR_INVALID_NETWORK);
            }
            nw->create_type = VIP_CREATE_NETWORK_FROM_FILE;
        }
        else if (type & VIP_DUP_FROM_NBG_MEMORY) {
            status = nbg_create_network_from_memory(nw, data, size_of_data);
            nw->create_type = VIP_CREATE_NETWORK_FROM_MEMORY;
        }
        else if (type & VIP_DUP_FROM_NBG_FLASH) {
            status = nbg_create_network_from_memory(nw, data, size_of_data);
            nw->create_type = VIP_CREATE_NETWORK_FROM_FLASH;
        }
        else {
            vip_error("fail not support this type=0x%x duplicate network\n", type);
            vipGoOnError(VIP_ERROR_NOT_SUPPORTED);
        }
    }
    else if (type & VIP_DUP_FOR_CMD_BY_NETWORK) {
        nw->dup_type = NBGLK_FROM_NETWORK_DUP_COMMAND;
        if (type & VIP_DUP_FROM_NETWORK) {
            status = nbg_create_network_from_network(network, nw);
        }
        else {
            vip_error("fail not support this type=0x%x duplicate network\n", type);
            vipGoOnError(VIP_ERROR_NOT_SUPPORTED);
        }
    }
    else {
        vip_error("fail this feature TBD type=0x%x\n", type);
        vipGoOnError(VIP_ERROR_NOT_SUPPORTED);
    }

    vip_info("dup create network=%p\n", nw);
    if (VIP_SUCCESS == status) {
        *dup_network = nw;
    }
    else {
        vip_error("fail to dup network type=0x%x status=%d\n", type, status);
        vipGoOnError(status);
    }

    VIP_FUNC_EXIT
    return status;

onError:
    *dup_network = VIP_NULL;
    if (nw != VIP_NULL) {
        nbglk_destroy_network(nw);
        viphal_free_memory(nw);
    }
    VIP_FUNC_EXIT
    vip_error("fail to dup network type=0x%x, status=%d\n", type, status);
    return status;
}

/*! \brief Weak dup a vip_network object.
    instead of vip_dup_network(VIP_DUP_FOR_CMD_BY_NETWORK | VIP_DUP_FROM_NETWORK)
    NOT USE. WILL DELETE. Please don't use it now.
    The weak dup netowrk copy new command buffer. and share coefficient data and ppu instruction with original network.
    Notes:
      1. The original network can't be destroy if the weak dup network is running or will be run later.
      2. The original network has the same input/output shape as the dup network.
      3. Only the input/output addresses of network are difference between the original network with dup network.
    eg: Used to support batch network.
*\param network, original network to be dup.
*\param dup_network, output network.
*\return <tt>\ref vip_status_e </tt>
*/
VIP_API
vip_status_e vip_weak_dup_network(
    IN vip_network network,
    OUT vip_network *dup_network
    )
{
    vip_status_e status = VIP_SUCCESS;
    vip_network copy = VIP_NULL;
    VIP_FUNC_ENTER

    if (!network || !dup_network) {
        vip_error("Unexpected null inputs.");
        return VIP_ERROR_INVALID_ARGUMENTS;
    }
    if (vip_false_e == network->prepared_network) {
        vip_error("fail to dup network, please prepare original network first\n");
        vipGoOnError(VIP_ERROR_NOT_SUPPORTED);

    }
    if (network->nbg_desc.net_info.core_count > 1) {
        vip_error("fail dup network not support core count=%d\n",
            network->nbg_desc.net_info.core_count);
        vipGoOnError(VIP_ERROR_NOT_SUPPORTED);
    }
    status = viphal_allocate_memory(sizeof(vip_network_t), (void **)&copy);
    if (status != VIP_SUCCESS) {
        vip_error("fail to allocate memory for network object\n");
        vipGoOnError(status);
    }

    status = nbg_create_network_from_network(network, copy);
    if (status != VIP_SUCCESS) {
        vip_error("fail to create dup network status=%d\n", status);
        vipGoOnError(VIP_ERROR_FAILURE);
    }

    status = nbglk_prepare_network_from_friend(copy);
    if (status != VIP_SUCCESS) {
        vip_error("fail to prepare dup network status=%d\n", status);
        vipGoOnError(VIP_ERROR_FAILURE);
    }

    *dup_network = copy;
    VIP_FUNC_EXIT
    return status;
onError:
    *dup_network = VIP_NULL;
    if (copy != VIP_NULL) {
        nbglk_destroy_network(copy);
        viphal_free_memory(copy);
    }
    VIP_FUNC_EXIT
    return status;
}

/*
@brief destroy network object.
@param network The network object which created by vip_create_network().
*/
VIP_API
vip_status_e vip_destroy_network(
    IN vip_network network
    )
{
    vip_network_t *nw = (vip_network_t *)network;
    vip_status_e status = VIP_SUCCESS;
    NBGLK_CHECK_NETWORK();
    VIP_FUNC_ENTER
    status = nbglk_destroy_network(network);
    if (status != VIP_SUCCESS) {
        vip_error("fail to destroy network=%p\n", network);
    }
    status = viphal_free_memory(nw);
    VIP_FUNC_EXIT
    return status;
}

/*
@brief query network's information. input/ouput/network name information.
@param network The network object which created by vip_create_network().
@param property The property be query. see vip_network_property_e.
@param value The returns value data.
*/
VIP_API
vip_status_e vip_query_network(
    IN vip_network network,
    IN vip_enum property,
    OUT void *value
    )
{
    vip_status_e status = VIP_SUCCESS;
    NBGLK_CHECK_NETWORK();
    NBGLK_INITIALIZE_CHECK(network);
    VIP_FUNC_ENTER
    status = nbglk_query_network(network, property, value);
    VIP_FUNC_EXIT
    return status;
}

/*
@brief set network property. configure network. this API should be called before calling vip_prepare_network.
@param network The network object which created by vip_create_network().
@param property The property be set. see vip_network_property_e.
@param value The set data.
*/
VIP_API
vip_status_e vip_set_network(
    IN vip_network network,
    IN vip_enum property,
    IN void *value
    )
{
    vip_status_e status = VIP_SUCCESS;
    NBGLK_CHECK_NETWORK();
    NBGLK_INITIALIZE_CHECK(network);
    VIP_FUNC_ENTER
    status = nbglk_set_network(network, property, value);
    VIP_FUNC_EXIT
    return status;
}

/*
@brief interpreter the network for preparing command buffer,
       memory pool for running network on hardware.
@param network The network object which created by vip_create_network().
*/
VIP_API
vip_status_e vip_prepare_network(
    IN vip_network network
    )
{
    vip_status_e status = VIP_SUCCESS;
    NBGLK_CHECK_NETWORK();
    NBGLK_INITIALIZE_CHECK(network);
    VIP_FUNC_ENTER
    vip_info("start prepare network=%p\n", network);
    if (NBGLK_FROM_NETWORK_DUP_COMMAND == network->dup_type) {
        status = nbglk_prepare_network_from_friend(network);
    }
    else {
        status = nbglk_prepare_network(network);
    }
    if (status != VIP_SUCCESS) {
        vip_error("fail to prepare network.\n");
        nbglk_dump_nbg_entry(network);
        nbglk_finish_network(network);
    }
    VIP_FUNC_EXIT
    return status;
}

/*
@brief query input shape, quant, format and so on.
@param index The index of network input.
@param property The property be query. see vip_buffer_property_e.
@param value, The returns value data.
*/
VIP_API
vip_status_e vip_query_input(
    IN vip_network network,
    IN vip_uint32_t index,
    IN vip_enum property,
    OUT void *value
    )
{
    vip_status_e status = VIP_SUCCESS;
    NBGLK_CHECK_NETWORK();
    NBGLK_INITIALIZE_CHECK(network);
    VIP_FUNC_ENTER
    status = nbglk_query_input(network, index, property, value);
    VIP_FUNC_EXIT
    return status;
}

/*
@brief query output shape, quan, format and so on.
@param index The index of network output.
@param property The property be query. see vip_buffer_property_e.
@param value, The returns value data.
*/
VIP_API
vip_status_e vip_query_output(
    IN vip_network network,
    IN vip_uint32_t index,
    IN vip_enum property,
    OUT void *value
    )
{
    vip_status_e status = VIP_SUCCESS;
    NBGLK_CHECK_NETWORK();
    NBGLK_INITIALIZE_CHECK(network);
    VIP_FUNC_ENTER
    status = nbglk_query_output(network, index, property, value);
    VIP_FUNC_EXIT
    return status;
}

/*
@brief set input buffer for network or change network input buffer.
@param index The index of network inputs.
@param input The input buffer of network.
*/
VIP_API
vip_status_e vip_set_input(
    IN vip_network network,
    IN vip_uint32_t index,
    IN vip_buffer input
    )
{
    vip_status_e status = VIP_SUCCESS;
    NBGLK_CHECK_NETWORK();
    NBGLK_INITIALIZE_CHECK(network);
    VIP_FUNC_ENTER
    status = nbglk_set_input(network, index, input);
    VIP_FUNC_EXIT
    return status;
}

/*
@brief set output buffer for network.
@param index The index of network outputs.
@param input The output buffer of network.
*/
VIP_API
vip_status_e vip_set_output(
    IN vip_network network,
    IN vip_uint32_t index,
    IN vip_buffer output
    )
{
    vip_status_e status = VIP_SUCCESS;
    NBGLK_CHECK_NETWORK();
    NBGLK_INITIALIZE_CHECK(network);
    VIP_FUNC_ENTER
    status = nbglk_set_output(network, index, output);
    VIP_FUNC_EXIT
    return status;
}

/*
@brief Run the network object. Assume all related resource are ready.
@param network The network object which created by vip_create_network().
*/
VIP_API
vip_status_e vip_run_network(
    IN vip_network network
    )
{
    vip_status_e status = VIP_SUCCESS;
    NBGLK_CHECK_NETWORK();
    VIP_FUNC_ENTER
    status = nbglk_run_network(network);
    VIP_FUNC_EXIT
    return status;
}

/*
@brief Kick off the network commands to HW. without waiting from HW.
       Assume all related resource are ready.
@param network The network object which created by vip_create_network().
*/
VIP_API
vip_status_e vip_trigger_network(
    IN vip_network network
    )
{
    vip_status_e status = VIP_SUCCESS;
    NBGLK_CHECK_NETWORK();
    VIP_FUNC_ENTER
    status = nbglk_trigger_network(network);
    VIP_FUNC_EXIT
    return status;
}

/*
@brief Wait for HW to finish executing the submitted network.
@param network The network object which created by vip_create_network().
*/
VIP_API
vip_status_e vip_wait_network(
    IN vip_network network
    )
{
    vip_status_e status = VIP_SUCCESS;
    NBGLK_CHECK_NETWORK();
    VIP_FUNC_ENTER

    /* wait for last layer finish */
    status = nbglk_wait_network(network);
    if ((status != VIP_SUCCESS) && (status != VIP_ERROR_CANCELED)) {
        vip_error("fail to wait network in vip wait network\n");
    }

    VIP_FUNC_EXIT
    return status;
}

/*! \brief. Cancle network running on vip hardware after network is commited.
@param network The network object which created by vip_create_network().
*/
VIP_API
vip_status_e vip_cancel_network(
    IN vip_network network
    )
{
    vip_status_e status = VIP_SUCCESS;
#if vpmdENABLE_CANCELATION
    NBGLK_CHECK_NETWORK();
    VIP_FUNC_ENTER

    /* cancel netowrk running on vip hardware */
    status = nbglk_cancel_network(network);
    if (status != VIP_SUCCESS) {
        vip_error("fail to cancel network status=%d\n", status);
    }

    VIP_FUNC_EXIT
#else
    vip_error("not support cancel network, please set vpmdENABLE_CANCELATION to 1\n");
    status = VIP_ERROR_NOT_SUPPORTED;
#endif

    return status;
}

/*
@brief Finish running the network. matched with vip_prepare_network and vip_create_network.
@param network The network object which created by vip_create_network().
*/
VIP_API
vip_status_e vip_finish_network(
    IN vip_network network
    )
{
    vip_status_e status = VIP_SUCCESS;
    NBGLK_CHECK_NETWORK();
    VIP_FUNC_ENTER
    status = nbglk_finish_network(network);
    VIP_FUNC_EXIT
    return status;
}

/*
@brief Give user applications more control over power management for VIP cores.
@param Index of the managed device. device_index is 0 if VIP is single core.
@param perperty Control VIP core frequency and power status by property. see vip_power_property_e.
@param value The value for vip_power_property_e property.
       Please see vip_power_frequency_t if property is setting to VIP_POWER_PROPERTY_SET_FREQUENCY.
*/
VIP_API
vip_status_e vip_power_management(
    IN vip_uint32_t device_index,
    IN vip_power_property_e property,
    IN void *value
    )
{
    vip_status_e status = VIP_SUCCESS;
#if vpmdENABLE_USER_CONTROL_POWER
    nbglk_context_t* context = nbglk_get_context();
    vipdrv_power_property_e prop = VIPDRV_POWER_PROPERTY_NONE;
    VIP_FUNC_ENTER

    if ((property <= VIP_POWER_PROPERTY_NONE) || (property >= VIP_POWER_PROPERTY_MAX)) {
        vip_error("invalid power control property: %d\n", property);
        vipGoOnError(VIP_ERROR_INVALID_ARGUMENTS);
    }

    if (device_index >= context->device_count) {
        vip_error("invalid device index: %d. (device count: %d)\n", device_index, context->device_count);
        vipGoOnError(VIP_ERROR_INVALID_ARGUMENTS);
    }

    switch (property)
    {
    case VIP_POWER_PROPERTY_SET_FREQUENCY:
    {
        vip_power_frequency_t* freq = (vip_power_frequency_t*)value;
        prop = VIPDRV_POWER_PROPERTY_SET_FREQUENCY;
        value = (void*)&freq->fscale_percent;
        break;
    }
    case VIP_POWER_PROPERTY_OFF:
        prop = VIPDRV_POWER_PROPERTY_OFF;
        break;
    case VIP_POWER_PROPERTY_ON:
        prop = VIPDRV_POWER_PROPERTY_ON;
        break;
    case VIP_POWER_PROPERTY_STOP:
        prop = VIPDRV_POWER_PROPERTY_STOP;
        break;
    case VIP_POWER_PROPERTY_START:
        prop = VIPDRV_POWER_PROPERTY_START;
        break;
    default:
        break;
    }

    status = viphal_power_management(prop, value, device_index);

onError:
    VIP_FUNC_EXIT
#else
    vip_error("not support power management, please set vpmdENABLE_USER_CONTROL_POWER 1\n");
    status = VIP_ERROR_NOT_SUPPORTED;
#endif
    return status;
}

/*
@brief change parameters of PPU.
@param network the network object.
@param param the params of PPU.
@param index, NOT used now. default is zero.
*/
VIP_API
vip_status_e vip_set_ppu_param(
    IN vip_network network,
    IN vip_ppu_param_t *param,
    IN vip_uint32_t index
    )
{
    vip_status_e status = VIP_SUCCESS;
#if vpmdENABLE_CHANGE_PPU_PARAM
    vip_uint32_t global_offset[3] = {0, 0, 0};
    vip_uint32_t global_scale[3] = {0, 0, 0};
    vip_uint32_t work_size[3] = {0, 0, 0};
    vip_uint32_t group_count[3]= {0, 0, 0};
    vip_ppu_param_patch_info_t *ppu_param_i = VIP_NULL;
    NBGLK_CHECK_NETWORK();
    VIP_FUNC_ENTER

    if (VIP_NULL == param) {
        vip_error("fail to set ppu param, param is NULL\n");
        vipGoOnError(VIP_ERROR_INVALID_ARGUMENTS);
    }

    if (param->work_dim > 3) {
        vip_error("fail to set ppu param, work dim=%d\n", param->work_dim);
        vipGoOnError(VIP_ERROR_INVALID_ARGUMENTS);
    }

    if (VIP_NULL == network->ppu_param_info) {
        vip_error("fail to set ppu param, not patch ppu param\n");
        vipGoOnError(VIP_ERROR_FAILURE);
    }

    if (!network->prepared_network) {
        vip_error("failed please change ppu param after calling prepare network\n");
        vipOnError(VIP_ERROR_FAILURE);
    }

    ppu_param_i = &network->ppu_param_info[index];

    if (VIP_NULL == ppu_param_i) {
        vip_error("fail to change ppu param ppu_param_info is NULL\n");
        vipGoOnError(VIP_ERROR_FAILURE);
    }

    if (param->work_dim != network->ppu_work_dim) {
        vip_warning("warning set work-dim=%d, nbg work-dim=%d\n",
                     param->work_dim, network->ppu_work_dim);
    }

    global_offset[0] = param->global_offset[0];
    global_scale[0] = param->global_scale[0];
    work_size[0] = param->local_size[0] ? param->local_size[0] : 1;
    group_count[0] = param->global_size[0] / param->local_size[0];

    if (param->work_dim > 1) {
        global_offset[1] = param->global_offset[1];
        global_scale[1] = param->global_scale[1];
        work_size[1] = param->local_size[1] ? param->local_size[1] : 1;
        group_count[1] = param->global_size[1] / param->local_size[1];
    }

    if (param->work_dim > 2) {
        global_offset[2] = param->global_offset[2];
        global_scale[2] = param->global_scale[2];
        work_size[2] = param->local_size[2] ? param->local_size[2] : 1;
        group_count[2] = param->global_size[2] / param->local_size[2];
    }

    NBGLK_SET_VALUE((vip_uint32_t*)ppu_param_i->glooffset_x_logic, global_offset[0]);
    NBGLK_SET_VALUE((vip_uint32_t*)ppu_param_i->glooffset_y_logic, global_offset[1]);
    NBGLK_SET_VALUE((vip_uint32_t*)ppu_param_i->glooffset_z_logic, global_offset[2]);

    NBGLK_SET_VALUE((vip_uint32_t*)ppu_param_i->gloscale_x_logic, global_scale[0] & 0xFF);
    NBGLK_SET_VALUE((vip_uint32_t*)ppu_param_i->gloscale_y_logic, global_scale[1] & 0xFF);
    NBGLK_SET_VALUE((vip_uint32_t*)ppu_param_i->gloscale_z_logic, global_scale[2] & 0xFF);

    NBGLK_SET_VALUE(ppu_param_i->grocount_x_logic, group_count[0] - 1);
    NBGLK_SET_VALUE(ppu_param_i->grocount_y_logic, group_count[1] - 1);
    NBGLK_SET_VALUE(ppu_param_i->grocount_z_logic, group_count[2] - 1);

    NBGLK_SET_VALUE(ppu_param_i->grosize_x_logic, (work_size[0] -1) & 0x3FF);
    NBGLK_SET_VALUE(ppu_param_i->grosize_y_logic, (work_size[1] -1) & 0x3FF);
    NBGLK_SET_VALUE(ppu_param_i->grosize_z_logic, (work_size[2] -1) & 0x3FF);

#if vpmdENABLE_VIDEO_MEMORY_CACHE
    {
    vip_uint32_t i = 0;
    for (i = 0; i < network->segment_num; i++) {
        nbglk_segment_t *segment = &network->segments[i];
        if (vip_false_e == segment->is_sw_segm) {
            vipOnError(viphal_flush_cache(segment->states_buffer.mem_id, VIPDRV_CACHE_FLUSH));
        }
    }
    }
#endif
    VIP_FUNC_EXIT
#else
    vip_error("not support set ppu parameters\n");
    vipGoOnError(VIP_ERROR_NOT_SUPPORTED);
#endif

onError:
    return status;
}
