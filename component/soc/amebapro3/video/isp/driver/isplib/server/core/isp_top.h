#ifndef _ISP_TOP_H_INC_
#define _ISP_TOP_H_INC_

#include <rtsavisp.h>

int isp_top_init(void);
int isp_top_cleanup(void);
int isp_top_start(void);
int isp_top_stop(void);
int isp_top_status(void);
int isp_top_plugin_register(const struct rts_isp_plugin *plug);
int isp_top_plugin_unregister(int plug_id);
int isp_top_plugin_get(struct rts_isp_plugin *plug);
int isp_top_plugin_check(uint32_t isp_id, int plug_id);
int isp_top_plugin_bind(uint32_t isp_id, int plug_id);
int isp_top_plugin_unbind(uint32_t isp_id, int plug_id);
int isp_top_get_status(void);
void isp_top_set_status(int32_t status);
void *isp_top_get_poll(void);
void *isp_top_get_core(uint32_t isp_id);
struct isp_mod *isp_top_get_mod(int isp_id, int mod_id);
int isp_top_set_debug_select(uint32_t debug_select);
void isp_top_lock_internal(void);
void isp_top_unlock_internal(void);

#endif /* _ISP_TOP_H_INC_ */
