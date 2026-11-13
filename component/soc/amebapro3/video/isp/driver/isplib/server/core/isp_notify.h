#ifndef _ISP_NOTIFY_H_INC_
#define _ISP_NOTIFY_H_INC_

#include <rts_isp_dynamic.h>
#include "isp_mod.h"
#include "isp_avl.h"
#include "isp_sensor.h"
#include "isp_statis.h"

enum isp_notify_num {
	_ISP_START_PRE,
	_ISP_START_POST,
	_ISP_STOP_PRE,
	_ISP_STOP_POST,
	_ISP_MODE_CHANGE,
	_ISP_CROP_CHANGE,
	_ISP_MCROP_CHANGE,
	_ISP_FPS_CHANGE,
	_ISP_PLUGIN_UPDATE,
	_ISP_STATIS_DONE,
	_ISP_IQ_CHANGE,
	_ISP_DYNAMIC,
	_ISP_MIRROR_FLIP_CHANGE,
	_ISP_NOTIFY_NUM,
};

enum isp_notify_ids {
	ISP_NOTIFY_START_PRE = isp_action(NOTIFY_TYPE, _ISP_START_PRE),
	ISP_NOTIFY_START_POST = isp_action(NOTIFY_TYPE, _ISP_START_POST),
	ISP_NOTIFY_STOP_PRE = isp_action(NOTIFY_TYPE, _ISP_STOP_PRE),
	ISP_NOTIFY_STOP_POST = isp_action(NOTIFY_TYPE, _ISP_STOP_POST),
	ISP_NOTIFY_MODE_CHANGE = isp_action_w(NOTIFY_TYPE, _ISP_MODE_CHANGE,
										  struct isp_sensor_mode),
	ISP_NOTIFY_CROP_CHANGE =
		isp_action_w(NOTIFY_TYPE, _ISP_CROP_CHANGE, rts_isp_point_t),
	ISP_NOTIFY_MCROP_CHANGE =
		isp_action_w(NOTIFY_TYPE, _ISP_MCROP_CHANGE, rts_isp_rect_t),
	ISP_NOTIFY_FPS_CHANGE =
		isp_action_w(NOTIFY_TYPE, _ISP_FPS_CHANGE, float),
	ISP_NOTIFY_PLUGIN_UPDATE =
		isp_action_w(NOTIFY_TYPE, _ISP_PLUGIN_UPDATE, uint32_t),
	ISP_NOTIFY_STATIS_DONE = isp_action_w(NOTIFY_TYPE, _ISP_STATIS_DONE,
										  struct rts_isp_statis),
	ISP_NOTIFY_IQ_CHANGE = isp_action(NOTIFY_TYPE, _ISP_IQ_CHANGE),
	ISP_NOTIFY_DYNAMIC = isp_action_w(NOTIFY_TYPE, _ISP_DYNAMIC,
									  struct isp_notify_dynamic),
	ISP_NOTIFY_MIRROR_FLIP_CHANGE =
		isp_action_w(NOTIFY_TYPE, _ISP_MIRROR_FLIP_CHANGE,
					 struct rts_isp_mirror_flip),
};

struct isp_notify {
	int notify_num;
	struct avl_tree *mods;
	struct isp_list pending;
	uint32_t notifying : 1;
};

int isp_notify_init(struct isp_notify *notify);
int isp_notify_cleanup(struct isp_notify *notify);
int isp_notify_add_mod(struct isp_notify *notify, struct isp_mod *mod);
int isp_notify_del_mod(struct isp_notify *notify, struct isp_mod *mod);
int isp_notify_add_entry(struct isp_notify *notify, struct isp_mod *mod,
						 int32_t id);
int isp_notify_del_entry(struct isp_notify *notify, struct isp_mod *mod,
						 int32_t id);
int isp_notify_add_entry_priority(struct isp_notify *notify,
								  struct isp_mod *mod, uint32_t id,
								  int32_t priority);
int isp_notify_del_entry_priority(struct isp_notify *notify,
								  struct isp_mod *mod, uint32_t id,
								  int32_t priority);
int isp_notify(struct isp_notify *notify, int32_t id, void *data, size_t len);
int isp_notify_sched(struct isp_notify *notify, int32_t id, void *data,
					 size_t len, void (*callback)(int ret, void *priv),
					 void *priv);
int isp_notify_mod(struct isp_notify *notify, struct isp_mod *mod, int32_t id,
				   void *data, size_t len);
int isp_notify_mod_sched(struct isp_notify *notify, struct isp_mod *mod,
						 int32_t id, void *data, size_t len,
						 void (*callback)(int ret, void *priv), void *priv);
int isp_notify_priority(struct isp_notify *notify, int32_t priority, int32_t id,
						void *data, size_t len);
int isp_notify_priority_sched(struct isp_notify *notify, int32_t priority,
							  int32_t id, void *data, size_t len,
							  void (*callback)(int ret, void *priv),
							  void *priv);
int isp_notify_mod_priority(struct isp_notify *notify, struct isp_mod *mod,
							int32_t priority, int32_t id, void *data,
							size_t len);
int isp_notify_mod_priority_sched(struct isp_notify *notify,
								  struct isp_mod *mod, int32_t priority,
								  int32_t id, void *data, size_t len,
								  void (*callback)(int ret, void *priv),
								  void *priv);

#endif /* _ISP_NOTIFY_H_INC_ */
