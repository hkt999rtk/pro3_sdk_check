/*
 * Realtek Semiconductor Corp.
 * Copyright (C) 2017 Grant Shen <grant_shen@realsil.com.cn>
 */

#include <isp_utils.h>
#include "isp_top.h"
#include "isp_core.h"
#include "isp_mod.h"

#define ISP_DEBUG_TYPE ISP_DEBUG_MOD

static int check_actions(const struct isp_mod_action_info *actions,
						 uint32_t actions_num, int is_notify)
{
	int i;

	if (!!actions ^ !!actions_num) {
		return -RTS_ISP_EINVAL;
	}
	if (is_notify) {
		for (i = 0; i < actions_num; i++)
			if (!actions[i].id || !actions[i].action) {
				return -RTS_ISP_EINVAL;
			}
	} else {
		for (i = 0; i < actions_num; i++)
			if (ISP_AID_NR(actions[i].id) != i ||
				!actions[i].action) {
				return -RTS_ISP_EINVAL;
			}
	}
	return RTS_ISP_OK;
}

static int check_mod(const struct isp_mod *mod)
{
	if (!mod || !mod->id || !mod->name) {
		return -RTS_ISP_EINVAL;
	}
	if (!mod->init || !mod->cleanup) {
		return -RTS_ISP_EINVAL;
	}
	if (mod->virtual && (mod->hardware_init || mod->hardware_cleanup)) {
		return -RTS_ISP_EINVAL;
	}
	if (check_actions(mod->info_actions, mod->info_actions_num, 0) ||
		check_actions(mod->exec_actions, mod->exec_actions_num, 0) ||
		check_actions(mod->notify_actions, mod->notify_actions_num, 1)) {
		return -RTS_ISP_EINVAL;
	}
	if (mod->next || mod->owner) {
		return -RTS_ISP_EINVAL;
	}
	if (mod->initialized || mod->hardware_initialized || mod->ctrl_added) {
		return -RTS_ISP_EINVAL;
	}

	return RTS_ISP_OK;
}

static int check_id_data(uint32_t id, uint8_t type, void *data, size_t len)
{
	if (ISP_AID_TYPE(id) != type) {
		return -RTS_ISP_EINVAL;
	}
	if (ISP_AID_SIZE(id) != len) {
		return -RTS_ISP_EINVAL;
	}
	if (!len ^ !data) {
		return -RTS_ISP_EINVAL;
	}
	return RTS_ISP_OK;
}

static int mod_do_action(struct isp_mod *mod, uint32_t id, void *data,
						 const struct isp_mod_action_info *actions,
						 size_t actions_num)
{
	if (ISP_AID_NR(id) >= actions_num) {
		return -RTS_ISP_EINVAL;
	}
	if (!actions || !actions_num) {
		return -RTS_ISP_EPERM;
	}
	return actions[ISP_AID_NR(id)].action(mod, data);
}

int isp_mod_register(struct isp_mod *mod)
{
	int ret;
	isp_core_t core;

	if (check_mod(mod)) {
		isp_error("%s module format error\n", mod ? mod->name : "nil");
		return -RTS_ISP_EINVAL;
	}

	core = isp_top_get_core(mod->owner_id);
	if (!core) {
		ret = -RTS_ISP_EINVAL;
		goto err;
	}
	ret = isp_core_register_mod(core, mod);
	if (ret) {
		goto err;
	}
	mod->owner = core;
	debug("module %s registered\n", mod->name);
	return RTS_ISP_OK;

err:
	isp_perror(ret, "register module %s fail", mod->name);
	return ret;
}

int isp_mod_unregister(struct isp_mod *mod)
{
	if (!mod) {
		return -RTS_ISP_EINVAL;
	}

	isp_core_unregister_mod(mod->owner, mod);

	mod->next = NULL;
	mod->owner = NULL;
	mod->initialized = RTS_ISP_FALSE;
	mod->hardware_initialized = RTS_ISP_FALSE;
	mod->ctrl_added = RTS_ISP_FALSE;

	debug("module %s unregistered\n", mod->name);
	return RTS_ISP_OK;
}

int isp_mod_init(struct isp_mod *mod)
{
	int ret;

	if (!mod) {
		return -RTS_ISP_EINVAL;
	}
	if (mod->initialized) {
		return RTS_ISP_OK;
	}

	if (isp_call_op_ret(ret, mod, init)) {
		isp_perror(ret, "module %s init fail", mod->name);
	} else {
		mod->initialized = RTS_ISP_TRUE;
		debug("module %s init ok\n", mod->name);
	}

	return ret;
}

int isp_mod_cleanup(struct isp_mod *mod)
{
	int ret;

	if (!mod) {
		return -RTS_ISP_EINVAL;
	}
	if (!mod->initialized) {
		return RTS_ISP_OK;
	}
	if (mod->hardware_initialized) {
		return -RTS_ISP_EBUSY;
	}

	if (isp_call_op_ret(ret, mod, cleanup)) {
		isp_perror(ret, "module %s cleanup fail", mod->name);
	} else {
		mod->initialized = RTS_ISP_FALSE;
		mod->ctrl_added = RTS_ISP_FALSE;
		isp_memset((void *)mod + sizeof(*mod), 0,
				   mod->size - sizeof(*mod));
		debug("module %s cleanup ok\n", mod->name);
	}

	return ret;
}

int isp_mod_hardware_init(struct isp_mod *mod)
{
	int ret;

	if (!mod) {
		return -RTS_ISP_EINVAL;
	}
	if (mod->hardware_initialized) {
		return RTS_ISP_OK;
	}
	if (!mod->initialized) {
		return -RTS_ISP_ENOTREADY;
	}
	if (!mod->hardware_init) {
		mod->hardware_initialized = RTS_ISP_TRUE;
		return RTS_ISP_OK;
	}

	if (isp_call_op_ret(ret, mod, hardware_init)) {
		isp_perror(ret, "module %s hardware init fail", mod->name);
	} else {
		mod->hardware_initialized = RTS_ISP_TRUE;
		debug("module %s hardware init ok\n", mod->name);
	}

	return ret;
}

int isp_mod_hardware_cleanup(struct isp_mod *mod)
{
	int ret;

	if (!mod) {
		return -RTS_ISP_EINVAL;
	}
	if (!mod->hardware_initialized) {
		return RTS_ISP_OK;
	}
	if (!mod->hardware_cleanup) {
		mod->hardware_initialized = RTS_ISP_FALSE;
		return RTS_ISP_OK;
	}

	if (isp_call_op_ret(ret, mod, hardware_cleanup)) {
		isp_perror(ret, "module %s hardware cleanup fail", mod->name);
	} else {
		mod->hardware_initialized = RTS_ISP_FALSE;
		debug("module %s hardware cleanup ok\n", mod->name);
	}

	return ret;
}

int isp_mod_add_ctrl(struct isp_mod *mod, void *phandler)
{
	int ret;

	if (!mod || !phandler) {
		return -RTS_ISP_EINVAL;
	}
	if (!mod->initialized) {
		return -RTS_ISP_ENOTREADY;
	}
	if (!mod->virtual && !mod->hardware_initialized) {
		return -RTS_ISP_ENOTREADY;
	}

	if (mod->ctrl_added) {
		return RTS_ISP_OK;
	}

	if (!mod->add_ctrl) {
		return RTS_ISP_OK;
	}

	if (isp_call_op_ret(ret, mod, add_ctrl, phandler)) {
		isp_perror(ret, "module %s add ctrl fail", mod->name);
	} else {
		mod->ctrl_added = RTS_ISP_TRUE;
		debug("module %s add ctrl ok\n", mod->name);
	}

	return ret;
}

int isp_mod_get_info(struct isp_mod *mod, uint32_t id, void *info, size_t len)
{
	int ret;

	if (!mod || check_id_data(id, INFO_TYPE, info, len)) {
		return -RTS_ISP_EINVAL;
	}
	if (!mod->initialized && isp_mod_init(mod)) {
		return -RTS_ISP_ENOTREADY;
	}

	ret = mod_do_action(mod, id, info, mod->info_actions,
						mod->info_actions_num);
	if (ret) {
		isp_perror(ret, "module %s get info 0x%x fail", mod->name, id);
	} else {
		debug("module %s get info 0x%x ok\n", mod->name, id);
	}

	return ret;
}

int isp_mod_execute(struct isp_mod *mod, uint32_t id, void *data, size_t len)
{
	int ret;

	if (!mod || check_id_data(id, EXEC_TYPE, data, len)) {
		return -RTS_ISP_EINVAL;
	}
	if (!mod->initialized) {
		return -RTS_ISP_ENOTREADY;
	}
	if (!mod->virtual && !mod->hardware_initialized) {
		return -RTS_ISP_ENOTREADY;
	}

	ret = mod_do_action(mod, id, data, mod->exec_actions,
						mod->exec_actions_num);
	if (ret) {
		isp_perror(ret, "module %s execute 0x%x fail", mod->name, id);
	} else {
		debug("module %s execute 0x%x ok\n", mod->name, id);
	}

	return ret;
}

int isp_mod_do_action(struct isp_mod *mod, uint32_t id, void *data, size_t len)
{
	int ret;

	if (!mod || ISP_AID_SIZE(id) != len || (!len ^ !data)) {
		return -RTS_ISP_EINVAL;
	}
	if (!mod->initialized) {
		return -RTS_ISP_ENOTREADY;
	}
	if (!mod->virtual && !mod->hardware_initialized) {
		return -RTS_ISP_ENOTREADY;
	}

	switch (ISP_AID_TYPE(id)) {
	case INFO_TYPE:
		ret = mod_do_action(mod, id, data, mod->info_actions,
							mod->info_actions_num);
		break;
	case EXEC_TYPE:
		ret = mod_do_action(mod, id, data, mod->exec_actions,
							mod->exec_actions_num);
		break;
	case NOTIFY_TYPE:
		ret = -RTS_ISP_EACCES;
		break;
	default:
		ret = -RTS_ISP_ENOOPS;
	}
	if (ret) {
		isp_perror(ret, "module %s do action 0x%x fail", mod->name, id);
	} else {
		debug("module %s do action 0x%x ok\n", mod->name, id);
	}

	return ret;
}

int isp_mod_add_notify(struct isp_mod *mod, uint32_t notify_id)
{
	if (!mod) {
		return -RTS_ISP_EINVAL;
	}

	return isp_core_add_notify_entry(mod->owner, mod, notify_id);
}

int isp_mod_del_notify(struct isp_mod *mod, uint32_t notify_id)
{
	if (!mod) {
		return -RTS_ISP_EINVAL;
	}

	return isp_core_del_notify_entry(mod->owner, mod, notify_id);
}

int isp_mod_add_priority_notify(struct isp_mod *mod, uint32_t notify_id,
								int32_t priority)
{
	if (!mod) {
		return -RTS_ISP_EINVAL;
	}

	return isp_core_add_priority_notify_entry(mod->owner, mod, notify_id,
			priority);
}

int isp_mod_del_priority_notify(struct isp_mod *mod, uint32_t notify_id,
								int32_t priority)
{
	if (!mod) {
		return -RTS_ISP_EINVAL;
	}

	return isp_core_del_priority_notify_entry(mod->owner, mod, notify_id,
			priority);
}

int isp_mod_sibling_get_info(struct isp_mod *mod, int mod_id, int id,
							 void *data, size_t len)
{
	return isp_mod_get_info(isp_mod_get_sibling(mod, mod_id), id, data,
							len);
}

int isp_mod_sibling_execute(struct isp_mod *mod, int mod_id, int id, void *data,
							size_t len)
{
	return isp_mod_execute(isp_mod_get_sibling(mod, mod_id), id, data, len);
}

struct isp_mod *isp_mod_get_sibling(struct isp_mod *mod, int mod_id)
{
	struct isp_mod *get;

	if (!mod) {
		return NULL;
	}
	if (isp_core_get_mod(mod->owner, mod_id, &get)) {
		return NULL;
	}
	return get;
}

#include "isp_iq.h"
#include "isp_notify.h"

int isp_mod_set_patch_notify(struct isp_mod *mod, int enable, int statis_flag)
{
	int ret;

	if (enable) {
		if (statis_flag) {
			ret = isp_mod_del_notify(mod, ISP_NOTIFY_STATIS_DONE);
			if (ret) {
				return ret;
			}
		}
		ret = isp_mod_del_notify(mod, ISP_NOTIFY_IQ_CHANGE);
		if (ret) {
			return ret;
		}
		ret = isp_mod_del_priority_notify(mod, ISP_NOTIFY_DYNAMIC,
										  ISP_DYNAMIC_PRIORITY);
	} else {
		if (statis_flag) {
			ret = isp_mod_add_notify(mod, ISP_NOTIFY_STATIS_DONE);
			if (ret) {
				return ret;
			}
		}
		ret = isp_mod_add_notify(mod, ISP_NOTIFY_IQ_CHANGE);
		if (ret) {
			return ret;
		}
		ret = isp_mod_add_priority_notify(mod, ISP_NOTIFY_DYNAMIC,
										  ISP_DYNAMIC_PRIORITY);
	}
	return ret;
}
