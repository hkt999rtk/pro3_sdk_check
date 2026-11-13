#ifndef _ISP_CORE_H_INC_
#define _ISP_CORE_H_INC_

#include <stddef.h>
#include <rtsavisp.h>

struct isp_mod;
typedef struct isp_core *isp_core_t;

int isp_core_create(isp_core_t *pcore, uint32_t id);
int isp_core_release(isp_core_t *pcore);
int isp_core_register_mod(isp_core_t core, struct isp_mod *mod);
int isp_core_unregister_mod(isp_core_t core, struct isp_mod *mod);
int isp_core_get_mod(isp_core_t core, uint32_t id, struct isp_mod **pmod);
int isp_core_init(isp_core_t core);
int isp_core_cleanup(isp_core_t core);
int isp_core_start(isp_core_t core);
int isp_core_stop(isp_core_t core);
int isp_core_add_notify_entry(isp_core_t core, struct isp_mod *mod,
							  uint32_t notify_id);
int isp_core_del_notify_entry(isp_core_t core, struct isp_mod *mod,
							  uint32_t notify_id);
int isp_core_add_priority_notify_entry(isp_core_t core, struct isp_mod *mod,
									   uint32_t notify_id, int32_t priority);
int isp_core_del_priority_notify_entry(isp_core_t core, struct isp_mod *mod,
									   uint32_t notify_id, int32_t priority);
int isp_core_notify(isp_core_t core, uint32_t id, void *data, size_t len);
int isp_core_notify_sched(isp_core_t core, uint32_t id, void *data, size_t len,
						  void (*callback)(int ret, void *priv), void *priv);
int isp_core_notify_mod(isp_core_t core, uint32_t mod_id, uint32_t notify_id,
						void *data, size_t len);
int isp_core_notify_mod_sched(isp_core_t core, uint32_t mod_id,
							  uint32_t notify_id, void *data, size_t len,
							  void (*callback)(int ret, void *priv),
							  void *priv);
int isp_core_notify_priority(isp_core_t core, int32_t priority,
							 uint32_t notify_id, void *data, size_t len);
int isp_core_notify_priority_sched(isp_core_t core, int32_t priority,
								   uint32_t notify_id, void *data, size_t len,
								   void (*callback)(int ret, void *priv),
								   void *priv);
struct isp_statis *isp_core_get_statis(isp_core_t core);
struct isp_iq *isp_core_get_iq(isp_core_t core);

#endif /* _ISP_CORE_H_INC_ */
