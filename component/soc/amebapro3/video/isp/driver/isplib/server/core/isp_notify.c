/*
 * Realtek Semiconductor Corp.
 * Copyright (C) 2017 Grant Shen <grant_shen@realsil.com.cn>
 */

#include <isp_utils.h>
#include "isp_tracker.h"
#include "isp_notify.h"

#define ISP_DEBUG_TYPE ISP_DEBUG_NOTIFY

struct isp_notify_key {
	uint32_t id;
	int32_t priority;
};

struct isp_notify_entry {
	struct isp_mod *mod;
	isp_mod_action action;
	struct avl_node avl;
	struct isp_notify_key key;
};

struct isp_notify_message {
	int32_t id; /* enum isp_notify_ids */
	void *data;
	size_t len;
	struct isp_mod *mod;
	int has_priority : 1;
	int32_t priority;
};

struct pending_notify {
	struct isp_list list;
	struct isp_notify_message message;
	void (*callback)(int ret, void *priv);
	void *priv;
};

static int notify_entry_cmp(const void *k1, const void *k2, void *ptr)
{
	const struct isp_notify_key *key1 = k1;
	const struct isp_notify_key *key2 = k2;

	if (key1->priority == key2->priority) {
		return key1->id - key2->id;
	}
	return key1->priority - key2->priority;
}

static int notify_add_entry(struct isp_notify *notify, struct isp_mod *mod,
							struct isp_mod_action_info *action_info)
{
	int num;
	struct isp_notify_entry *notify_entry;
	struct isp_notify_key key;

	num = ISP_AID_NR(action_info->id);
	key.id = mod->id;
	key.priority = action_info->priority;
	if (avl_find(&notify->mods[num], &key)) {
		return RTS_ISP_OK;
	}

	if (!isp_calloc_item(notify_entry)) {
		return -RTS_ISP_ENOMEM;
	}

	notify_entry->key = key;
	notify_entry->mod = mod;
	notify_entry->action = action_info->action;
	notify_entry->avl.key = &notify_entry->key;
	avl_insert(&notify->mods[num], &notify_entry->avl);
	debug("mod %s add notfiy %d\n", mod->name, num);

	return RTS_ISP_OK;
}

static int notify_del_entry(struct isp_notify *notify, struct isp_mod *mod,
							struct isp_mod_action_info *action_info)
{
	int num;
	struct isp_notify_entry *notify_entry;
	struct isp_notify_key key;

	num = ISP_AID_NR(action_info->id);
	key.id = mod->id;
	key.priority = action_info->priority;
	notify_entry =
		avl_find_element(&notify->mods[num], &key, notify_entry, avl);
	if (!notify_entry) {
		return RTS_ISP_OK;
	}
	avl_delete(&notify->mods[num], &notify_entry->avl);
	isp_free(notify_entry);
	debug("mod %s del notfiy %d\n", mod->name, num);

	return RTS_ISP_OK;
}

static int __isp_notify(struct isp_notify *notify,
						struct isp_notify_message *message)
{
	int ret = RTS_ISP_OK;
	int32_t num = ISP_AID_NR(message->id);
	struct isp_notify_entry *notify_entry;
	struct isp_notify_entry *first_notify_entry;

	if (ISP_AID_TYPE(message->id) != NOTIFY_TYPE ||
		ISP_AID_SIZE(message->id) != message->len) {
		return -RTS_ISP_EINVAL;
	}
	if (message->len && !message->data) {
		return -RTS_ISP_EINVAL;
	}

	notify->notifying = RTS_ISP_TRUE;
	if (message->has_priority) {
		struct isp_notify_key key;

		key.id = 0;
		key.priority = message->priority;
		first_notify_entry = avl_find_ge_element(
								 &notify->mods[num], &key, notify_entry, avl);
	} else {
		first_notify_entry = avl_first_element(&notify->mods[num],
											   notify_entry, avl);
	}

	if (!first_notify_entry) {
		return RTS_ISP_OK;
	}
	avl_for_element_to_last(&notify->mods[num], first_notify_entry,
							notify_entry, avl) {
		if (message->mod && notify_entry->mod != message->mod) {
			continue;
		}
		if (message->has_priority &&
			notify_entry->key.priority != message->priority) {
			break;
		}
		if (message->id == ISP_NOTIFY_STATIS_DONE) {
			time_tracker_begin(notify_entry->mod->name);
		}
		debug("notify %d to mod %s\n", num, notify_entry->mod->name);
		ret = notify_entry->action(notify_entry->mod, message->data);
		if (message->id == ISP_NOTIFY_STATIS_DONE) {
			time_tracker_end(notify_entry->mod->name);
		}
		if (ret) {
			isp_perror(ret, "mod %s notify %d pro fail",
					   notify_entry->mod->name, num);
			break;
		}
	}
	notify->notifying = RTS_ISP_FALSE;
	return ret;
}

static int __isp_notify_mod_priority(struct isp_notify *notify,
									 struct isp_notify_message *message)
{
	int ret;

	if (!notify || !message) {
		return -RTS_ISP_EINVAL;
	}
	if (notify->notifying) {
		return -RTS_ISP_EBUSY;
	}
	ret = __isp_notify(notify, message);
	if (ret) {
		return ret;
	}

	while (!list_empty(&notify->pending)) {
		struct isp_list *list;
		struct pending_notify *pending;

		list = list_head(&notify->pending);
		list_remove_init(list);
		pending = list_data(list, struct pending_notify, list);
		if (!ret) {
			ret = __isp_notify(notify, &pending->message);
		}
		if (pending->callback) {
			notify->notifying = RTS_ISP_TRUE;
			pending->callback(ret, pending->priv);
			notify->notifying = RTS_ISP_FALSE;
		}
		isp_free(pending);
	}
	return ret;
}

static int __isp_notify_sched(struct isp_notify *notify,
							  struct isp_notify_message *message,
							  void (*callback)(int ret, void *priv), void *priv)
{
	int ret;
	struct pending_notify *item;

	if (!notify->notifying) {
		ret = __isp_notify_mod_priority(notify, message);
		if (callback) {
			callback(ret, priv);
		}
		return ret;
	}
	if (!isp_calloc_item(item)) {
		ret = -RTS_ISP_ENOMEM;
		if (callback) {
			callback(ret, priv);
		}
		return ret;
	}

	list_init(&item->list);
	item->message = *message;
	item->callback = callback;
	item->priv = priv;
	list_insert_tail(&notify->pending, &item->list);

	return RTS_ISP_OK;
}

int isp_notify_init(struct isp_notify *notify)
{
	int i;

	if (!notify) {
		return -RTS_ISP_EINVAL;
	}

	notify->notify_num = _ISP_NOTIFY_NUM;
	if (!isp_calloc_array(notify->mods, notify->notify_num)) {
		return -RTS_ISP_ENOMEM;
	}
	for (i = 0; i < notify->notify_num; i++)
		avl_init(&notify->mods[i], notify_entry_cmp, RTS_ISP_FALSE,
				 NULL);
	list_init(&notify->pending);
	notify->notifying = RTS_ISP_FALSE;
	return RTS_ISP_OK;
}

int isp_notify_cleanup(struct isp_notify *notify)
{
	int i;

	if (!notify) {
		return -RTS_ISP_EINVAL;
	}

	for (i = 0; i < notify->notify_num; i++) {
		isp_assert(avl_is_empty(&notify->mods[i]));
	}
	isp_assert(list_empty(&notify->pending));
	isp_free(notify->mods);
	notify->notify_num = 0;
	return RTS_ISP_OK;
}

int isp_notify_add_mod(struct isp_notify *notify, struct isp_mod *mod)
{
	int i;
	int ret;

	if (!notify || !mod) {
		return -RTS_ISP_EINVAL;
	}
	for (i = 0; i < mod->notify_actions_num; i++) {
		ret = notify_add_entry(notify, mod, &mod->notify_actions[i]);
		if (ret) {
			goto err;
		}
	}
	return RTS_ISP_OK;
err:
	isp_notify_del_mod(notify, mod);
	return ret;
}

int isp_notify_del_mod(struct isp_notify *notify, struct isp_mod *mod)
{
	int i;

	if (!notify || !mod) {
		return -RTS_ISP_EINVAL;
	}
	for (i = 0; i < mod->notify_actions_num; i++) {
		notify_del_entry(notify, mod, &mod->notify_actions[i]);
	}
	return RTS_ISP_OK;
}

int isp_notify_add_entry(struct isp_notify *notify, struct isp_mod *mod,
						 int32_t id)
{
	int i;
	int ret;
	struct isp_mod_action_info *action_info = NULL;

	if (!notify || !mod) {
		return -RTS_ISP_EINVAL;
	}

	for (i = 0; i < mod->notify_actions_num; i++) {
		if (mod->notify_actions[i].id != id) {
			continue;
		}
		action_info = &mod->notify_actions[i];
		ret = notify_add_entry(notify, mod, action_info);
		if (ret) {
			return ret;
		}
	}
	if (action_info == NULL) {
		return -RTS_ISP_EINVAL;
	}

	return RTS_ISP_OK;
}

int isp_notify_del_entry(struct isp_notify *notify, struct isp_mod *mod,
						 int32_t id)
{
	int i;
	int ret;
	struct isp_mod_action_info *action_info = NULL;

	if (!notify || !mod) {
		return -RTS_ISP_EINVAL;
	}

	for (i = 0; i < mod->notify_actions_num; i++) {
		if (mod->notify_actions[i].id != id) {
			continue;
		}
		action_info = &mod->notify_actions[i];
		ret = notify_del_entry(notify, mod, action_info);
		if (ret) {
			return ret;
		}
	}
	if (action_info == NULL) {
		return -RTS_ISP_EINVAL;
	}

	return RTS_ISP_OK;
}

int isp_notify_add_entry_priority(struct isp_notify *notify,
								  struct isp_mod *mod, uint32_t id,
								  int32_t priority)
{
	int i;
	int ret;
	struct isp_mod_action_info *action_info = NULL;

	if (!notify || !mod) {
		return -RTS_ISP_EINVAL;
	}

	for (i = 0; i < mod->notify_actions_num; i++) {
		if (mod->notify_actions[i].id != id ||
			mod->notify_actions[i].priority != priority) {
			continue;
		}
		action_info = &mod->notify_actions[i];
		ret = notify_add_entry(notify, mod, action_info);
		if (ret) {
			return ret;
		}
	}
	if (action_info == NULL) {
		return -RTS_ISP_EINVAL;
	}

	return RTS_ISP_OK;
}

int isp_notify_del_entry_priority(struct isp_notify *notify,
								  struct isp_mod *mod, uint32_t id,
								  int32_t priority)
{
	int i;
	int ret;
	struct isp_mod_action_info *action_info = NULL;

	if (!notify || !mod) {
		return -RTS_ISP_EINVAL;
	}

	for (i = 0; i < mod->notify_actions_num; i++) {
		if (mod->notify_actions[i].id != id ||
			mod->notify_actions[i].priority != priority) {
			continue;
		}
		action_info = &mod->notify_actions[i];
		ret = notify_del_entry(notify, mod, action_info);
		if (ret) {
			return ret;
		}
	}
	if (action_info == NULL) {
		return -RTS_ISP_EINVAL;
	}

	return RTS_ISP_OK;
}

int isp_notify(struct isp_notify *notify, int32_t id, void *data, size_t len)
{
	return isp_notify_mod(notify, NULL, id, data, len);
}

int isp_notify_sched(struct isp_notify *notify, int32_t id, void *data,
					 size_t len, void (*callback)(int ret, void *priv),
					 void *priv)
{
	return isp_notify_mod_sched(notify, NULL, id, data, len, callback,
								priv);
}

int isp_notify_mod(struct isp_notify *notify, struct isp_mod *mod, int32_t id,
				   void *data, size_t len)
{
	struct isp_notify_message message;

	message.mod = mod;
	message.id = id;
	message.data = data;
	message.len = len;
	message.has_priority = 0;
	message.priority = 0;

	return __isp_notify_mod_priority(notify, &message);
}

int isp_notify_mod_sched(struct isp_notify *notify, struct isp_mod *mod,
						 int32_t id, void *data, size_t len,
						 void (*callback)(int ret, void *priv), void *priv)
{
	struct isp_notify_message message;

	if (!notify) {
		return -RTS_ISP_EINVAL;
	}

	message.mod = mod;
	message.id = id;
	message.data = data;
	message.len = len;
	message.has_priority = 0;
	message.priority = 0;

	return __isp_notify_sched(notify, &message, callback, priv);
}

int isp_notify_priority(struct isp_notify *notify, int32_t priority, int32_t id,
						void *data, size_t len)
{
	return isp_notify_mod_priority(notify, NULL, priority, id, data, len);
}

int isp_notify_priority_sched(struct isp_notify *notify, int32_t priority,
							  int32_t id, void *data, size_t len,
							  void (*callback)(int ret, void *priv), void *priv)
{
	return isp_notify_mod_priority_sched(notify, NULL, priority, id, data,
										 len, callback, priv);
}

int isp_notify_mod_priority(struct isp_notify *notify, struct isp_mod *mod,
							int32_t priority, int32_t id, void *data,
							size_t len)
{
	struct isp_notify_message message;

	message.mod = mod;
	message.id = id;
	message.data = data;
	message.len = len;
	message.has_priority = 1;
	message.priority = priority;

	return __isp_notify_mod_priority(notify, &message);
}

int isp_notify_mod_priority_sched(struct isp_notify *notify,
								  struct isp_mod *mod, int32_t priority,
								  int32_t id, void *data, size_t len,
								  void (*callback)(int ret, void *priv),
								  void *priv)
{
	struct isp_notify_message message;

	if (!notify) {
		return -RTS_ISP_EINVAL;
	}

	message.mod = mod;
	message.id = id;
	message.data = data;
	message.len = len;
	message.has_priority = 1;
	message.priority = priority;

	return __isp_notify_sched(notify, &message, callback, priv);
}
