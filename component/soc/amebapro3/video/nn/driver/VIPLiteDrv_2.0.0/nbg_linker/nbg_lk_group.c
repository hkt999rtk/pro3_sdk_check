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
#include <nbg_lk_capture.h>
#include <nbg_lk_context.h>
#include <nbg_lk_group.h>
#include <nbg_lk_debug.h>
#include <vip_hal.h>
#include <nbg_lk_relocate.h>
#include <vip_lite.h>
#include <nbg_fmt_util.h>
#if vpmdENABLE_NODE
#include <nbg_lk_node.h>
#endif

#if vpmdENABLE_GROUP_MODE
vip_status_e nbglk_group_item_get_command(
    const nbglk_group_item_t *item,
    nbglk_videomemory_t **cmd_ptr
    )
{
    vip_status_e status = VIP_SUCCESS;

    if (!item) {
        vip_error("unexpect null task.");
        return VIP_ERROR_FAILURE;
    }

    switch (item->object.type) {
    case NBGLK_OBJ_NETWORK:
    {
        vip_network network = (vip_network)item->task;
        if (network->segment_num > 1) {
            vip_error("fail to get command, network segment num is more than 1, is %d\n",
                         network->segment_num);
            vipGoOnError(VIP_ERROR_NOT_SUPPORTED);
        }
        /* check original network weight data free or not */
        if ((network->dup_type) && (0 == network->coeff_data.mem_id)) {
            vip_error("failed the weak network coefficient data is destroy\n");
            vipGoOnError(VIP_ERROR_NOT_SUPPORTED);
        }
        *cmd_ptr = &network->segments[0].states_buffer;
    }
    break;
#if vpmdENABLE_NODE
    case NBGLK_OBJ_NODE:
    {
        vip_node node = (vip_node)item->task;
        *cmd_ptr = &node->states_buffer;
    }
    break;
#endif
    default:
        vip_error("Unknown object type %#x\n", item->object.type);
        status = VIP_ERROR_FAILURE;
        break;
    }

onError:
    return status;
}

/*
@brief update input and output of tasks in group
@param group vip_group object
@param the number of task will be run.
*/
static vip_status_e nbglk_update_group_io(
    vip_group group,
    vip_uint32_t num
    )
{
    vip_status_e status = VIP_SUCCESS;
    vip_uint32_t i = 0;

    for (i = 0; i < num; i++) {
        if (NBGLK_OBJ_NETWORK == group->tasks[i].object.type) {
            vip_network network = (vip_network)group->tasks[i].task;
            if (network->input_dirty != 0) {
                vipOnError(nbglk_patch_network_inputs(network));
            }
            if (network->output_dirty != 0) {
                vipOnError(nbglk_patch_network_outputs(network));
            }
        }
    }

onError:
    return status;
}

static vip_status_e nbglk_set_group(
    IN vip_group_t *group,
    IN vip_enum property,
    IN void *value
    )
{
    vip_status_e status = VIP_SUCCESS;
    vipIsNULL(value);

    if (0 != group->add_task_num) {
        vip_uint8_t flag = 0;
        if (VIP_GROUP_PROP_SET_DEVICE_INDEX == property ||
            VIP_GROUP_PROP_SET_CORE_INDEX == property) {
            if (NBGLK_OBJ_NETWORK == group->tasks[0].object.type) {
                vip_network network = (vip_network)group->tasks[0].task;
                if (1 == network->nbg_desc.net_info.core_count) {
                    flag = 1;
                }
            }
        }
        if (0 == flag) {
            vip_error("failed set group that should be called before add network property=%d\n",
                property);
            vipOnError(VIP_ERROR_FAILURE);
        }
    }

    switch (property) {
    case VIP_GROUP_PROP_SET_DEVICE_INDEX:
    {
        vip_uint32_t *id = (vip_uint32_t*)value;
        nbglk_context_t *context = group->context;
        if (*id >= context->device_count) {
            vip_error("fail to set group device index, value is more than device count=%d.\n",
                        context->device_count);
            vipOnError(VIP_ERROR_FAILURE);
        }

        group->device = &context->devices[*id];
        if (*id != group->device->dev_index) {
            vip_error("fail to set device index, %d not match %d\n", *id, group->device->dev_index);
            vipOnError(VIP_ERROR_FAILURE);
        }
    }
    break;

    case VIP_GROUP_PROP_SET_CORE_INDEX:
    {
        vip_uint32_t *data = (vip_uint32_t*)value;
        vip_uint32_t core_cnt = group->device->core_count;
        if (*data > core_cnt) {
            vip_error("fail to set group core id=%d, cur device core cnt=%d.\n", *data, core_cnt);
            vipOnError(VIP_ERROR_FAILURE);
        }

        group->core_index = *data;
        group->need_schedule = vip_false_e;
    }
    break;

    case VIP_GROUP_PROP_SET_TIME_OUT:
    {
        vip_uint32_t *data = (vip_uint32_t*)value;
        group->wait_timeout = *data;
    }
    break;

    default:
        vip_error("not implement this case property=%d\n", property);
        break;
    }

    return status;

onError:
    vip_error("fail to set group=%p property=%d, value=%p\n",
                 group, property, value);
    return status;
}

static vip_status_e nbglk_query_group(
    IN vip_group_t *group,
    IN vip_enum property,
    OUT void *value
    )
{
    vip_status_e status = VIP_SUCCESS;
    NBGLK_CHECK_GROUP();
    vipIsNULL(value);

    switch (property) {
#if vpmdENABLE_APP_PROFILING
    case VIP_GROUP_PROP_PROFILING:
    {
        vipdrv_query_profiling_t profile;
        vip_inference_profile_t *tmp = (vip_inference_profile_t*)value;
        viphal_os_zero_memory(&profile, sizeof(vipdrv_query_profiling_t));

        profile.task_id = group->task_id;
        profile.subtask_index = 0;
        vipOnError(viphal_query_profiling(&profile));

        tmp->inference_time = profile.inference_time;
        tmp->total_cycle = profile.total_cycle[0] - profile.total_idle_cycle[0];
        vip_debug("profile inference_time=%dus, total_cycle=%d, idle_cycle=%d\n",
                profile.inference_time, profile.total_cycle[0], profile.total_idle_cycle[0]);
    }
    break;
#endif

    default:
        vip_error("query group not support this property=%d\n", property);
        status = VIP_ERROR_INVALID_ARGUMENTS;
        break;
    }

    return status;

onError:
    vip_error("fail to query group, value=%p, property=%d\n", value, property);
    return status;
}

/*
@brief run tasks in group.
@param group vip_group object
@param the number of task will be run. eg: num = 4, the 0, 1, 2, 3 taks index in group will be run(inference).
*/
static vip_status_e nbglk_run_group(
    vip_group group,
    vip_uint32_t num
    )
{
    vip_status_e status = VIP_SUCCESS;
    vip_uint8_t priority = 0;
    vip_uint32_t i = 0;
    viphal_task_param_t param = { 0 };
    vip_uint32_t core_cnt = 0;
    vip_bool_e with_event = !vpmdENABLE_POLLING;
    nbglk_videomemory_t* cur_cmd = VIP_NULL;

    /* check weight/bais are not be free */
    for (i = 0; i < num; i++) {
        if (NBGLK_OBJ_NETWORK == group->tasks[i].object.type) {
            vip_network network = (vip_network)group->tasks[i].task;
            if (network->priority > priority) priority = network->priority;
            if ((network->dup_type) && (0 == network->coeff_data.mem_id)) {
                vip_error("failed the weak network coefficient data is destroy, index=%d\n", i);
                vipGoOnError(VIP_ERROR_NOT_SUPPORTED);
            }
        }
    }

    if (NBGLK_OBJ_NETWORK == group->tasks[0].object.type) {
        vip_network network = (vip_network)group->tasks[0].task;
        core_cnt = network->nbg_desc.net_info.core_count;
    }
    else {
        core_cnt = 1;
    }

    if (num != group->run_task_num) {
        nbglk_videomemory_t *pre_cmd = VIP_NULL;
        nbglk_group_item_t *item = VIP_NULL;
        nbglk_videomemory_t *cmd_buf = VIP_NULL;
        vip_uint32_t cmd_size = 0;
        for (i = 0; i < num; i++) {
            status = nbglk_group_item_get_command(&group->tasks[i], &cur_cmd);
            if (status != VIP_SUCCESS) {
                vip_error("fail to get task[%d] cmd buffer, status=%d\n", i, status);
                vipGoOnError(VIP_ERROR_IO);
            }
            if (pre_cmd) {
                status = nbglk_link_command(pre_cmd, cur_cmd);
                if (status != VIP_SUCCESS) {
                    vip_error("fail to link commands, num=%d, add_task_num=%d\n",
                                 num, group->add_task_num);
                    vipGoOnError(status);
                }
            }
            pre_cmd = cur_cmd;
        }
        /* append cmds at the end of last task */
        item = &group->tasks[num-1];
        if (NBGLK_OBJ_NETWORK == item->object.type) {
            vip_network network = (vip_network)item->task;
            cmd_size = SIZE_CAST(vip_uint32_t, network->segments[0].states_buffer.size);
            cmd_buf = &network->segments[0].states_buffer;
        }
        #if vpmdENABLE_NODE
        else if (NBGLK_OBJ_NODE == item->object.type) {
            vip_node node = (vip_node)item->task;
            cmd_size = node->states_buffer.size;
            cmd_buf = &node->states_buffer;
        }
        #endif
        if (VIP_NULL == cmd_buf) {
            vip_error("fail command buffer memory not found\n");
            vipGoOnError(VIP_ERROR_FAILURE);
        }
        status = nbglk_append_cmds(cmd_buf, &cmd_size, with_event,
            core_cnt > 1 ? vip_true_e : vip_false_e, group->core_index);
        if (status != VIP_SUCCESS) {
            vip_error("fail to append commands in run group\n");
            vipGoOnError(status);
        }
    }

    /* update input and output of tasks in group */
    status = nbglk_update_group_io(group, num);
    if (status != VIP_SUCCESS) {
        vip_error("fail to update group input and output, run task count=%d\n", num);
        vipGoOnError(status);
    }

    for (i = 0; i < num; i++) {
        if (NBGLK_OBJ_NETWORK == group->tasks[i].object.type) {
            vip_network network = (vip_network)group->tasks[i].task;
            if (0 != network->init_commands.mem_id) {
                vipOnError(viphal_task_set_property(group->task_id, VIPDRV_TASK_CMD_INIT,
                    0, network->init_commands.mem_id, SIZE_CAST(vip_uint32_t, network->init_commands.size), 0));
                break;
            }
        }
    }

    param.task_id = group->task_id;
    param.subtask_count = num;
    param.subtask_index = 0;
    param.priority = priority;
    param.device_index = group->device->dev_index;
    param.core_index = group->core_index;
    param.core_cnt = core_cnt;
    param.time_out = group->wait_timeout;
    param.need_schedule = vip_true_e == group->need_schedule ? 1 : 0;
    status = viphal_submit_task(&param);
    if (status != VIP_SUCCESS) {
        vip_error("fail to commit command in group\n");
        vipGoOnError(status);
    }
    group->run_task_num = num;

onError:
    return status;
}

/*
@brief Wait for HW to finish executing the submitted group tasks.
@param group vip_group object
*/
static vip_status_e nbglk_wait_group(
    vip_group group
    )
{
    vip_status_e status = VIP_SUCCESS;
    nbglk_videomemory_t *first_cmd = VIP_NULL;

    /* run tasks */
    status = nbglk_group_item_get_command(&group->tasks[0], &first_cmd);
    if (status == VIP_SUCCESS) {
        /* Wait for commands to be done. */
        status = viphal_wait_task(group->task_id, 0);
        if ((status != VIP_SUCCESS) && (status != VIP_ERROR_POWER_OFF) &&(status != VIP_ERROR_CANCELED)) {
            vip_uint32_t i = 0;
            for (i = 0; i < group->run_task_num; i++) {
                if (NBGLK_OBJ_NETWORK == group->tasks[i].object.type) {
                    #if vpmdENABLE_CHECKSUM
                    nbglk_network_checksum((vip_network)group->tasks[i].task, NBGLK_CHECKSUM_MAX);
                    #endif
                    nbglk_section_checksum((vip_network)group->tasks[i].task);
                }
            }

            #if vpmdENABLE_HANG_DUMP > 1
            vip_error("group hang capture....\n");
            nbglk_capture_group(group, vip_true_e);
            #endif
            vip_error("wait group timeout, task count=%d.\n", group->run_task_num);
        }
    }

    return status;
}
#endif

/*
@brief Create a vip_group object to run multiple tasks(network or node) and
       without interrupt between each task.
@param count The maximum number of tasks supports by this group.
@param group Return vip_group object be created.
*/
VIP_API
vip_status_e vip_create_group(
    IN vip_uint32_t count,
    OUT vip_group *group
    )
{
    vip_status_e status = VIP_SUCCESS;
#if vpmdENABLE_GROUP_MODE
    vip_char_t name[256];
    vip_group_t *new_group = VIP_NULL;
    VIP_FUNC_ENTER

    if (VIP_NULL == group) {
        vip_error("fail to create group, parameter is NULL\n");
        VIP_FUNC_EXIT
        return VIP_ERROR_FAILURE;
    }
    if (0 == count) {
        vip_error("fail to create group, count is 0\n");
        VIP_FUNC_EXIT
        return VIP_ERROR_FAILURE;
    }
    vip_info("create group the max support task count=%d\n", count);
    status = viphal_allocate_memory(sizeof(vip_group_t), (void **)&new_group);
    if (status != VIP_SUCCESS) {
        vip_error("fail to allocate memory for group\n");
        vipGoOnError(status);
    }
    viphal_zero_memory(new_group, sizeof(vip_group_t));
    status = viphal_allocate_memory(sizeof(nbglk_group_item_t) * count,
                                       (void **)&new_group->tasks);
    if (status != VIP_SUCCESS) {
        vip_error("fail to allocate memory for group\n");
        vipGoOnError(status);
    }

    new_group->context = nbglk_get_context();
    new_group->max_task_num = count;
    new_group->wait_timeout = vpmdHARDWARE_TIMEOUT;
    new_group->device = &new_group->context->devices[0];
    new_group->need_schedule = vip_true_e;

    viphal_os_snprint(name, 255, "group_0x%x", new_group);
    vipOnError(viphal_task_create(name, count, &new_group->task_id));

#if vpmdENABLE_MULTIPLE_TASK
    status = viphal_os_create_mutex(&new_group->mutex);
    if (status != VIP_SUCCESS) {
        vip_error("fail to create mutex for group\n");
        vipGoOnError(VIP_ERROR_FAILURE);
    }
#endif

    *group = new_group;
    VIP_FUNC_EXIT
    return status;

onError:
    if (new_group != VIP_NULL) {
        if (new_group->tasks != VIP_NULL) {
            viphal_free_memory(new_group->tasks);
            new_group->tasks = VIP_NULL;
        }
    #if vpmdENABLE_MULTIPLE_TASK
        if (new_group->mutex != VIP_NULL) {
            viphal_os_destroy_mutex(new_group->mutex);
            new_group->mutex = VIP_NULL;
        }
    #endif
    }
    viphal_free_memory(new_group);
#else
    vip_error("not support group mode, please set vpmdENABLE_GROUP_MODE 1\n");
    *group = VIP_NULL;
    status = VIP_ERROR_NOT_SUPPORTED;
#endif
    VIP_FUNC_EXIT
    return status;
}

/*
@brief Destroy group object which created by vip_create_group.
@param group vip_group object.
*/
VIP_API
vip_status_e vip_destroy_group(
    IN vip_group group
    )
{
    vip_status_e status = VIP_SUCCESS;
    VIP_FUNC_ENTER

#if vpmdENABLE_GROUP_MODE
    if (group != VIP_NULL) {
        if (group->tasks != VIP_NULL) {
            viphal_free_memory(group->tasks);
            group->tasks = VIP_NULL;
        }
        #if vpmdENABLE_MULTIPLE_TASK
        if (group->mutex != VIP_NULL) {
            viphal_os_destroy_mutex(group->mutex);
            group->mutex = VIP_NULL;
        }
        #endif
        viphal_task_destroy(group->task_id);
        status = viphal_free_memory(group);
    }
#else
    vip_error("not support group mode, please set vpmdENABLE_GROUP_MODE 1\n");
    status = VIP_ERROR_NOT_SUPPORTED;
#endif

    VIP_FUNC_EXIT
    return status;
}

/*
@brief set group property. configure group. this API should be called before calling vip_run_group.
@param group The group object which created by vip_create_group().
@param property The property be set. see vip_group_property_e.
@param value The set data.
*/
VIP_API
vip_status_e vip_set_group(
    IN vip_group group,
    IN vip_enum property,
    IN void *value
    )
{
    vip_status_e status = VIP_SUCCESS;
#if vpmdENABLE_GROUP_MODE
    NBGLK_CHECK_GROUP();
    VIP_FUNC_ENTER
    status = nbglk_set_group(group, property, value);
    VIP_FUNC_EXIT
#else
    vip_error("not support group mode, please set vpmdENABLE_GROUP_MODE 1\n");
    status = VIP_ERROR_NOT_SUPPORTED;
#endif

    return status;
}

VIP_API
vip_status_e vip_query_group(
    IN vip_group group,
    IN vip_enum property,
    IN void *value
    )
{
    vip_status_e status = VIP_SUCCESS;
#if vpmdENABLE_GROUP_MODE
    NBGLK_CHECK_GROUP();
    VIP_FUNC_ENTER
    status = nbglk_query_group(group, property, value);
    VIP_FUNC_EXIT
#else
    vip_error("not support group mode, please set vpmdENABLE_GROUP_MODE 1\n");
    status = VIP_ERROR_NOT_SUPPORTED;
#endif

    return status;
}

/*
@brief add a vip_network object into group.
@param group vip_group object, network be added into group.
@param network vip_network added into group.
*/
VIP_API
vip_status_e vip_add_network(
    IN vip_group group,
    IN vip_network network
    )
{
    vip_status_e status = VIP_SUCCESS;
#if vpmdENABLE_GROUP_MODE
    nbglk_videomemory_t* cmd = VIP_NULL;
    vip_uint32_t count = group->add_task_num;
    vip_uint32_t i = 0;
    vip_bool_e with_event = !vpmdENABLE_POLLING;
    NBGLK_CHECK_NETWORK();
    VIP_FUNC_ENTER

#if vpmdENABLE_MULTIPLE_TASK
    viphal_os_lock_mutex(group->mutex);
#endif
    vip_info("add network=%p into group=%p\n", network, group);

    if (!network->prepared_network) {
        vip_error("failed add network, not prepare network\n");
        vipOnError(VIP_ERROR_FAILURE);
    }

    if (vip_true_e == network->belong_to_group) {
        vip_error("network has belonged to another group\n");
        vipGoOnError(VIP_ERROR_NETWORK_NOT_PREPARED);
    }
#if vpmdPRELOAD_COEFF > 1
    if (0 != network->preload_cmd.mem_id) {
        vip_error("not support add network, which need preload coeff into VIP-SRAM, into group\n");
        vipGoOnError(VIP_ERROR_NETWORK_NOT_PREPARED);
    }
#endif

    if (count > (group->max_task_num - 1)) {
        vip_error("fail to add network, task count is more than max=%d\n", group->max_task_num);
        vipOnError(VIP_ERROR_FAILURE);
    }
    if (network->segment_num > 1) {
        vip_error("fail to add network, cur network segment num=%d\n", network->segment_num);
        vipGoOnError(VIP_ERROR_NOT_SUPPORTED);
    }

    if (1 < network->nbg_desc.net_info.core_count) {
        /* check network device index */
        if ((group->device->dev_index != network->device->dev_index)) {
            vip_error("fail to add network, cur network's device index is not the same as group\n");
            vipGoOnError(VIP_ERROR_FAILURE);
        }

        /* check network core id */
        if ((group->core_index != network->core_index)) {
            vip_error("fail to add network, cur network's core_id is not the same as group\n");
            vipGoOnError(VIP_ERROR_FAILURE);
        }
    }

    if (0 < count) {
        if (NBGLK_OBJ_NETWORK == group->tasks[0].object.type) {
            if (network->nbg_desc.net_info.core_count !=
                ((vip_network)group->tasks[0].task)->nbg_desc.net_info.core_count) {
                vip_error("fail to add network, cur network's core cnt %d != group's %d\n",
                    network->nbg_desc.net_info.core_count,
                    ((vip_network)group->tasks[0].task)->nbg_desc.net_info.core_count);
                vipGoOnError(VIP_ERROR_FAILURE);
            }
        }
        else {
            if (1 != network->nbg_desc.net_info.core_count) {
                vip_error("fail to add network, cur network's core cnt %d != group's 1\n",
                    network->nbg_desc.net_info.core_count);
                vipGoOnError(VIP_ERROR_FAILURE);
            }
        }
    }

    /* check command buffer is not duplicated */
    for (i = 0; i < group->add_task_num; i++) {
        nbglk_videomemory_t *buffer = &network->segments[0].states_buffer;
        nbglk_group_item_get_command(&group->tasks[i], &cmd);
        if (buffer == cmd) {
            vip_error("faied to add network, not support the same network in a group\n");
            vipGoOnError(VIP_ERROR_FAILURE);
        }
    }

    group->tasks[count].object.type = NBGLK_OBJ_NETWORK;
    group->tasks[count].task = (vip_ptr)network;

    if (count > 0) {
        nbglk_segment_t *segment = &network->segments[0];
        status = nbglk_group_item_get_command(&group->tasks[count-1], &cmd);
        if (status != VIP_SUCCESS) {
            vip_error("fail to get group item command\n");
            goto onError;
        }

        status = nbglk_link_command(cmd, &segment->states_buffer);
        if (status != VIP_SUCCESS) {
            vip_error("Fail to link commands\n");
            group->tasks[count].object.type = NBGLK_OBJ_NONE;
            group->tasks[count].task = VIP_NULL;
            goto onError;
        }

        segment->command_size = SIZE_CAST(vip_uint32_t, segment->states_buffer.size);
        status = nbglk_append_cmds(&segment->states_buffer, &segment->command_size, with_event,
            network->nbg_desc.net_info.core_count > 1 ? vip_true_e : vip_false_e,
            network->device->core_id[network->core_index]);
        if (status != VIP_SUCCESS) {
            vip_error("fail to append commands in add network\n");
            vipGoOnError(status);
        }
    }

    group->add_task_num++;
    group->run_task_num = group->add_task_num;
    network->belong_to_group = vip_true_e;

    nbglk_group_item_get_command(&group->tasks[count], &cmd);
    vipOnError(viphal_task_set_property(group->task_id, VIPDRV_TASK_CMD_SUBTASK,
        count, cmd->mem_id, cmd->size, network->segments[0].patch_count));
#if vpmdTASK_SCHEDULE
    vipOnError(viphal_task_dup_info(group->task_id, VIPDRV_TASK_CMD_SUBTASK,
        count, network->task_id, VIPDRV_TASK_CMD_SUBTASK, 0));
#endif
onError:
#if vpmdENABLE_MULTIPLE_TASK
    viphal_os_unlock_mutex(group->mutex);
#endif
    VIP_FUNC_EXIT
#else
    vip_error("not support group mode, please set vpmdENABLE_GROUP_MODE 1\n");
    status = VIP_ERROR_NOT_SUPPORTED;
#endif

    return status;
}

/*
@brief run tasks in group. ony issue a interrupt after tasks complete.
@param group vip_group object
@param the number of task will be run. eg: num = 4, the 0, 1, 2, 3 taks index in group will be run(inference).
*/
VIP_API
vip_status_e vip_run_group(
    IN vip_group group,
    IN vip_uint32_t num
    )
{
    vip_status_e status = VIP_SUCCESS;
#if vpmdENABLE_GROUP_MODE
   VIP_FUNC_ENTER

#if vpmdENABLE_MULTIPLE_TASK
    viphal_os_lock_mutex(group->mutex);
#endif
    vip_info("group run task num=%d, add num=%d\n", num, group->add_task_num);

    if (num > group->add_task_num) {
        vip_error("failed run group, num=%d, task in group count=%d\n", num, group->add_task_num);
        vipGoOnError(VIP_ERROR_FAILURE);
    }
    else if (0 == num) {
        vip_error("No task to run\n");
        vipGoOnError(VIP_ERROR_FAILURE);
    }

    status = nbglk_run_group(group, num);
    if (status != VIP_SUCCESS) {
        vip_error("fail to run group=%p..\n", group);
        vipGoOnError(status);
    }

    status = nbglk_wait_group(group);
    if (status != VIP_SUCCESS) {
        vip_error("fail to wait group=%p..\n", group);
        vipGoOnError(status);
    }

#if vpmdENABLE_CAPTURE > 1
    nbglk_capture_group(group, vip_false_e);
#endif

onError:
#if vpmdENABLE_MULTIPLE_TASK
    viphal_os_unlock_mutex(group->mutex);
#endif

    VIP_FUNC_EXIT
#else
    vip_error("not support group mode, please set vpmdENABLE_GROUP_MODE 1\n");
    status = VIP_ERROR_NOT_SUPPORTED;
#endif

    return status;
}

/*
@brief run tasks in group.returns immediately without waiting for HW to complete the commands.
@param group vip_group object
@param the number of task will be run. eg: num = 4, the 0, 1, 2, 3 taks index in group will be run(inference).
*/
VIP_API
vip_status_e vip_trigger_group(
    IN vip_group group,
    IN vip_uint32_t num
    )
{
    vip_status_e status = VIP_SUCCESS;
#if vpmdENABLE_GROUP_MODE
   VIP_FUNC_ENTER

#if vpmdENABLE_MULTIPLE_TASK
    viphal_os_lock_mutex(group->mutex);
#endif
    vip_info("group run task num=%d, add num=%d\n", num, group->add_task_num);

    if (num > group->add_task_num) {
        vip_error("failed run group, num=%d, task in group count=%d\n", num, group->add_task_num);
        vipGoOnError(VIP_ERROR_FAILURE);
    }
    else if (0 == num) {
        vip_error("No task to run\n");
        vipGoOnError(VIP_ERROR_FAILURE);
    }

    status = nbglk_run_group(group, num);
    if (status != VIP_SUCCESS) {
        vip_error("fail to run group=%p..\n", group);
        vipGoOnError(status);
    }

#if vpmdENABLE_CAPTURE > 1
    nbglk_capture_group(group, vip_false_e);
#endif

onError:
#if vpmdENABLE_MULTIPLE_TASK
    viphal_os_unlock_mutex(group->mutex);
#endif

    VIP_FUNC_EXIT
#else
    vip_error("not support group mode, please set vpmdENABLE_GROUP_MODE 1\n");
    status = VIP_ERROR_NOT_SUPPORTED;
#endif

    return status;
}

/*
@brief Wait for HW to finish executing the submitted tasks in group.
@param network The network object which created by vip_create_network().
@param group vip_group object
*/
VIP_API
vip_status_e vip_wait_group(
    IN vip_group group
    )
{
    vip_status_e status = VIP_SUCCESS;
#if vpmdENABLE_GROUP_MODE
   VIP_FUNC_ENTER

    status = nbglk_wait_group(group);
    if (status != VIP_SUCCESS) {
        vip_error("fail to wait group=%p..\n", group);
        vipGoOnError(status);
    }

onError:
    VIP_FUNC_EXIT
#else
    vip_error("not support group mode, please set vpmdENABLE_GROUP_MODE 1\n");
    status = VIP_ERROR_NOT_SUPPORTED;
#endif

    return status;
}
