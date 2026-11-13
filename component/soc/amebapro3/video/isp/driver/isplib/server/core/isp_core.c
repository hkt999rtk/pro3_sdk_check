/*
 * Realtek Semiconductor Corp.
 * Copyright (C) 2017 Grant Shen <grant_shen@realsil.com.cn>
 */

#include <isp_os_log.h>
#include <isp_os_mem.h>
#include "isp_notify.h"
#include "isp_statis.h"
#include "isp_v4l2_ctrl.h"
#include "isp_iq.h"
#include "isp_core.h"

#define ISP_DEBUG_TYPE ISP_DEBUG_CORE

#define DEFAULT_ISP_MOD_BUCKETS_NUM 37

struct isp_mod_hash_table {
	struct isp_mod **mod_buckets;
	uint32_t nmod_buckets;
};

struct isp_core {
	struct isp_mod_hash_table hash;
	struct isp_notify notify;
	struct v4l2_ctrl_handler ctrl_handler;
	struct isp_statis statis;
	struct isp_iq iq;

	uint32_t id;

	int initialized : 1;
	int running : 1;
};

static int core_mod_hash_table_init(struct isp_mod_hash_table *hash)
{
	if (!hash) {
		return -RTS_ISP_EINVAL;
	}

	hash->nmod_buckets = DEFAULT_ISP_MOD_BUCKETS_NUM;
	if (!isp_calloc_array(hash->mod_buckets, hash->nmod_buckets)) {
		isp_error("alloc mod hash table memory fail\n");
		return -RTS_ISP_ENOMEM;
	}
	return RTS_ISP_OK;
}

static int core_mod_hash_table_cleanup(struct isp_mod_hash_table *hash)
{
	if (!hash) {
		return -RTS_ISP_EINVAL;
	}

	isp_free(hash->mod_buckets);
	hash->nmod_buckets = 0;

	return RTS_ISP_OK;
}

static int core_add_mod_to_hash(struct isp_mod_hash_table *hash,
								struct isp_mod *mod)
{
	struct isp_mod *node;
	int bucket;

	if (!hash || !mod) {
		return -RTS_ISP_EINVAL;
	}

	bucket = mod->id % hash->nmod_buckets;
	node = hash->mod_buckets[bucket];

	while (node) {
		if (node->id == mod->id) {
			return -RTS_ISP_EEXIST;
		}
		if (!node->next) {
			break;
		}
		node = node->next;
	}

	if (!node) {
		hash->mod_buckets[bucket] = mod;
	} else {
		node->next = mod;
	}

	return RTS_ISP_OK;
}

static int core_del_mod_from_hash(struct isp_mod_hash_table *hash,
								  const struct isp_mod *mod)
{
	struct isp_mod *pre = NULL;
	struct isp_mod *node;
	int bucket;

	if (!hash || !mod) {
		return -RTS_ISP_EINVAL;
	}

	bucket = mod->id % hash->nmod_buckets;
	node = hash->mod_buckets[bucket];

	while (node) {
		if (node->id == mod->id) {
			break;
		}
		pre = node;
		node = node->next;
	}

	if (node) {
		if (pre) {
			pre->next = node->next;
		} else {
			hash->mod_buckets[bucket] = node->next;
		}
	}

	return RTS_ISP_OK;
}

static int core_parse_all_mod(isp_core_t core, int (*func)(struct isp_mod *))
{
	int ret;
	int i;
	struct isp_mod_hash_table *hash;
	struct isp_mod *mod;

	if (!core || !func) {
		return -RTS_ISP_EINVAL;
	}

	hash = &core->hash;
	for (i = 0; i < hash->nmod_buckets; i++) {
		mod = hash->mod_buckets[i];
		while (mod) {
			ret = func(mod);
			if (ret) {
				isp_perror(ret, "parse module: %s error",
						   mod->name);
				return ret;
			}
			mod = mod->next;
		}
	}
	return RTS_ISP_OK;
}

static int core_parse_all_mod_data(isp_core_t core,
								   int (*func)(struct isp_mod *, void *data),
								   void *data)
{
	int ret;
	int i;
	struct isp_mod_hash_table *hash;
	struct isp_mod *mod;

	if (!core || !func) {
		return -RTS_ISP_EINVAL;
	}

	hash = &core->hash;
	for (i = 0; i < hash->nmod_buckets; i++) {
		mod = hash->mod_buckets[i];
		while (mod) {
			ret = func(mod, &core->ctrl_handler);
			if (ret) {
				isp_perror(ret, "parse module: %s error",
						   mod->name);
				return ret;
			}
			mod = mod->next;
		}
	}
	return RTS_ISP_OK;
}

static int core_notify_plugin_update(isp_core_t core)
{
	int ret;
	int plug_type;

	plug_type = RTS_ISP_PLUG_TYPE_SENSOR;
	ret = isp_core_notify(core, ISP_NOTIFY_PLUGIN_UPDATE, &plug_type,
						  sizeof(plug_type));
	if (ret) {
		return ret;
	}
	plug_type = RTS_ISP_PLUG_TYPE_IQ;
	return isp_core_notify(core, ISP_NOTIFY_PLUGIN_UPDATE, &plug_type,
						   sizeof(plug_type));

	/* as algo init in iq change, so do not need algo plugin update */
}

int isp_core_create(isp_core_t *pcore, uint32_t id)
{
	int ret;
	struct isp_core *isp_table;

	if (id >= _ISP_ID_NUM || !pcore) {
		return -RTS_ISP_EINVAL;
	}

	if (!isp_calloc_item(isp_table)) {
		isp_error("alloc isp table memory fail\n");
		ret = -RTS_ISP_ENOMEM;
		goto err;
	}

	isp_table->id = id;
	ret = core_mod_hash_table_init(&isp_table->hash);
	if (ret) {
		goto err;
	}
	ret = isp_notify_init(&isp_table->notify);
	if (ret) {
		goto err;
	}
	*pcore = isp_table;

	debug("%s ok\n", __func__);

	return RTS_ISP_OK;

err:
	isp_core_release(&isp_table);
	*pcore = NULL;
	debug("%s fail\n", __func__);
	return ret;
}

int isp_core_release(isp_core_t *pcore)
{
	isp_core_t core;

	if (!pcore) {
		return -RTS_ISP_EINVAL;
	}
	core = *pcore;
	if (!core) {
		return -RTS_ISP_EINVAL;
	}

	isp_notify_cleanup(&core->notify);
	core_mod_hash_table_cleanup(&core->hash);
	isp_free(core);
	*pcore = NULL;
	debug("%s ok\n", __func__);

	return RTS_ISP_OK;
}

int isp_core_register_mod(isp_core_t core, struct isp_mod *mod)
{
	int ret;

	if (!core || !mod) {
		return -RTS_ISP_EINVAL;
	}

	ret = core_add_mod_to_hash(&core->hash, mod);
	if (ret) {
		isp_perror(ret, "mod: %s exist", mod->name);
		return ret;
	}
	ret = isp_notify_add_mod(&core->notify, mod);
	if (ret) {
		isp_perror(ret, "add %s to notify list fail", mod->name);
		return ret;
	}

	return RTS_ISP_OK;
}

int isp_core_unregister_mod(isp_core_t core, struct isp_mod *mod)
{
	if (!core || !mod) {
		return -RTS_ISP_EINVAL;
	}

	isp_notify_del_mod(&core->notify, mod);
	core_del_mod_from_hash(&core->hash, mod);
	return RTS_ISP_OK;
}

int isp_core_get_mod(isp_core_t core, uint32_t id, struct isp_mod **pmod)
{
	struct isp_mod_hash_table *hash = &core->hash;
	int bucket;
	struct isp_mod *node;

	if (!core || !id || !pmod) {
		return -RTS_ISP_EINVAL;
	}

	bucket = id % hash->nmod_buckets;
	node = hash->mod_buckets[bucket];
	while (node) {
		if (node->id == id) {
			break;
		}
		node = node->next;
	}

	if (node) {
		*pmod = node;
		return RTS_ISP_OK;
	} else {
		*pmod = NULL;
		return -RTS_ISP_ENOITEM;
	}
}

int isp_core_init(isp_core_t core)
{
	int ret;

	if (!core) {
		return -RTS_ISP_EINVAL;
	}
	if (core->running) {
		return -RTS_ISP_EBUSY;
	}
	if (core->initialized) {
		return RTS_ISP_OK;
	}
	ret = isp_iq_init(&core->iq, core->id);
	if (ret) {
		goto err;
	}

	core->initialized = RTS_ISP_TRUE;
	debug("%s ok\n", __func__);

	return RTS_ISP_OK;
err:
	isp_perror(ret, "core init fail");
	return ret;
}

int isp_core_cleanup(isp_core_t core)
{
	if (!core) {
		return -RTS_ISP_EINVAL;
	}
	if (core->running) {
		return -RTS_ISP_EBUSY;
	}
	if (!core->initialized) {
		return RTS_ISP_OK;
	}
	isp_iq_cleanup(&core->iq);
	core->initialized = RTS_ISP_FALSE;
	debug("isp core cleaned up ok\n");

	return RTS_ISP_OK;
}

int isp_core_start(isp_core_t core)
{
	int ret;

	if (!core) {
		return -RTS_ISP_EINVAL;
	}
	if (core->running) {
		return RTS_ISP_OK;
	}

	ret = core_parse_all_mod(core, isp_mod_init);
	if (ret) {
		goto err;
	}
	ret = core_parse_all_mod(core, isp_mod_hardware_init);
	if (ret) {
		goto err;
	}
	ret = isp_v4l2_ctrl_handler_init(&core->ctrl_handler, 32);
	if (ret) {
		goto err;
	}
	ret = core_parse_all_mod_data(core, isp_mod_add_ctrl,
								  &core->ctrl_handler);
	if (ret) {
		goto err;
	}
	ret = core_notify_plugin_update(core);
	if (ret) {
		goto err;
	}
	core->running = RTS_ISP_TRUE;
	debug("%s ok\n", __func__);

	return RTS_ISP_OK;
err:
	isp_v4l2_ctrl_handler_free(&core->ctrl_handler);
	core_parse_all_mod(core, isp_mod_hardware_cleanup);
	core_parse_all_mod(core, isp_mod_cleanup);
	core->running = RTS_ISP_FALSE;
	return ret;
}

int isp_core_stop(isp_core_t core)
{
	if (!core) {
		return -RTS_ISP_EINVAL;
	}

	if (!core->running) {
		return RTS_ISP_OK;
	}

	isp_v4l2_ctrl_handler_free(&core->ctrl_handler);
	core_parse_all_mod(core, isp_mod_hardware_cleanup);
	core_parse_all_mod(core, isp_mod_cleanup);
	core->running = RTS_ISP_FALSE;
	debug("isp core stop running\n");

	return RTS_ISP_OK;
}

int isp_core_add_notify_entry(isp_core_t core, struct isp_mod *mod,
							  uint32_t notify_id)
{
	if (!core || !mod) {
		return -RTS_ISP_EINVAL;
	}

	return isp_notify_add_entry(&core->notify, mod, notify_id);
}

int isp_core_del_notify_entry(isp_core_t core, struct isp_mod *mod,
							  uint32_t notify_id)
{
	if (!core || !mod) {
		return -RTS_ISP_EINVAL;
	}

	return isp_notify_del_entry(&core->notify, mod, notify_id);
}

int isp_core_add_priority_notify_entry(isp_core_t core, struct isp_mod *mod,
									   uint32_t notify_id, int32_t priority)
{
	if (!core || !mod) {
		return -RTS_ISP_EINVAL;
	}

	return isp_notify_add_entry_priority(&core->notify, mod, notify_id,
										 priority);
}

int isp_core_del_priority_notify_entry(isp_core_t core, struct isp_mod *mod,
									   uint32_t notify_id, int32_t priority)
{
	if (!core || !mod) {
		return -RTS_ISP_EINVAL;
	}

	return isp_notify_del_entry_priority(&core->notify, mod, notify_id,
										 priority);
}

int isp_core_notify(isp_core_t core, uint32_t id, void *data, size_t len)
{
	if (!core) {
		return -RTS_ISP_EINVAL;
	}
	if (len && !data) {
		return -RTS_ISP_EINVAL;
	}

	return isp_notify(&core->notify, id, data, len);
}

int isp_core_notify_sched(isp_core_t core, uint32_t id, void *data, size_t len,
						  void (*callback)(int ret, void *priv), void *priv)
{
	if (!core) {
		return -RTS_ISP_EINVAL;
	}
	if (len && !data) {
		return -RTS_ISP_EINVAL;
	}

	return isp_notify_sched(&core->notify, id, data, len, callback, priv);
}

int isp_core_notify_mod(isp_core_t core, uint32_t mod_id, uint32_t notify_id,
						void *data, size_t len)
{
	struct isp_mod *mod;

	if (!core) {
		return -RTS_ISP_EINVAL;
	}
	if (len && !data) {
		return -RTS_ISP_EINVAL;
	}
	if (isp_core_get_mod(core, mod_id, &mod)) {
		return -RTS_ISP_EINVAL;
	}
	return isp_notify_mod(&core->notify, mod, notify_id, data, len);
}

int isp_core_notify_mod_sched(isp_core_t core, uint32_t mod_id,
							  uint32_t notify_id, void *data, size_t len,
							  void (*callback)(int ret, void *priv), void *priv)
{
	struct isp_mod *mod;

	if (!core) {
		return -RTS_ISP_EINVAL;
	}
	if (len && !data) {
		return -RTS_ISP_EINVAL;
	}
	if (isp_core_get_mod(core, mod_id, &mod)) {
		return -RTS_ISP_EINVAL;
	}
	return isp_notify_mod_sched(&core->notify, mod, notify_id, data, len,
								callback, priv);
}

int isp_core_notify_priority(isp_core_t core, int32_t priority,
							 uint32_t notify_id, void *data, size_t len)
{
	if (!core) {
		return -RTS_ISP_EINVAL;
	}
	if (len && !data) {
		return -RTS_ISP_EINVAL;
	}

	return isp_notify_priority(&core->notify, priority, notify_id, data,
							   len);
}

int isp_core_notify_priority_sched(isp_core_t core, int32_t priority,
								   uint32_t notify_id, void *data, size_t len,
								   void (*callback)(int ret, void *priv),
								   void *priv)
{
	if (!core) {
		return -RTS_ISP_EINVAL;
	}
	if (len && !data) {
		return -RTS_ISP_EINVAL;
	}

	return isp_notify_priority_sched(&core->notify, priority, notify_id,
									 data, len, callback, priv);
}

struct isp_statis *isp_core_get_statis(isp_core_t core)
{
	if (!core) {
		return NULL;
	}
	return &core->statis;
}

struct isp_iq *isp_core_get_iq(isp_core_t core)
{
	if (!core) {
		return NULL;
	}
	return &core->iq;
}
