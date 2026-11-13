#ifndef _ISP_MOD_H_INC_
#define _ISP_MOD_H_INC_

#include <isp_os_mem.h>
#include <isp_os_log.h>
#include <isp_mod_common.h>

enum isp_mod_status {
	UNINITIALIZED,
	INITIALIZED,
};

struct isp_mod;

typedef int (*isp_mod_action)(struct isp_mod *mod, void *data);

struct isp_mod_action_info {
	uint32_t id;
	isp_mod_action action;
	uint32_t priority;
};

#define ISP_MOD_ACTION(_id, _action) \
	[ISP_AID_NR(_id)] = {        \
		.id = _id,           \
		.action = _action,   \
	}

#define ISP_NOTIFY_ACTION(_id, _action)                      \
	{                                                    \
		.id = _id, .priority = 0, .action = _action, \
	}

#define ISP_NOTIFY_PRIO_ACTION(_id, _priority, _action)              \
	{                                                            \
		.id = _id, .priority = _priority, .action = _action, \
	}

#define get_mod(ptr, name) container_of(ptr, struct isp_mod_##name, mod)

struct isp_mod {
	uint32_t id;
	const char *name;
	uint32_t owner_id;

	/* do not directly use these callbacks, use api in this file */
	int (*init)(struct isp_mod *mod);
	int (*cleanup)(struct isp_mod *mod);
	int (*hardware_init)(struct isp_mod *mod);
	int (*hardware_cleanup)(struct isp_mod *mod);
	int (*add_ctrl)(struct isp_mod *mod, void *phandler);
	struct isp_mod_action_info *exec_actions;
	size_t exec_actions_num;
	struct isp_mod_action_info *info_actions;
	size_t info_actions_num;
	struct isp_mod_action_info *notify_actions;
	size_t notify_actions_num;
	size_t size; /* size of the struct which we embedded in */
	int virtual : 1;

	void *owner; /* auto assigned by framework after registered */

	/* private, only used for internal */
	struct isp_mod *next;
	int initialized : 1;
	int hardware_initialized : 1;
	int ctrl_added : 1;
};

int isp_mod_register(struct isp_mod *mod);
int isp_mod_unregister(struct isp_mod *mod);
int isp_mod_init(struct isp_mod *mod);
int isp_mod_cleanup(struct isp_mod *mod);
int isp_mod_hardware_init(struct isp_mod *mod);
int isp_mod_hardware_cleanup(struct isp_mod *mod);
int isp_mod_add_ctrl(struct isp_mod *mod, void *phandler);
int isp_mod_get_info(struct isp_mod *mod, uint32_t id, void *info, size_t len);
int isp_mod_execute(struct isp_mod *mod, uint32_t id, void *data, size_t len);
int isp_mod_do_action(struct isp_mod *mod, uint32_t id, void *data, size_t len);
int isp_mod_add_notify(struct isp_mod *mod, uint32_t notify_id);
int isp_mod_del_notify(struct isp_mod *mod, uint32_t notify_id);
int isp_mod_add_priority_notify(struct isp_mod *mod, uint32_t notify_id,
								int32_t priority);
int isp_mod_del_priority_notify(struct isp_mod *mod, uint32_t notify_id,
								int32_t priority);
int isp_mod_sibling_get_info(struct isp_mod *mod, int mod_id, int id,
							 void *data, size_t len);
int isp_mod_sibling_execute(struct isp_mod *mod, int mod_id, int id, void *data,
							size_t len);
struct isp_mod *isp_mod_get_sibling(struct isp_mod *mod, int mod_id);
int isp_mod_set_patch_notify(struct isp_mod *mod, int enable, int statis_flag);

int isp_register_all_modules(void);
void isp_unregister_all_modules(void);

#endif /* _ISP_MOD_H_INC_ */
