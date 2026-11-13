#ifndef _ISP_PLUGIN_H_INC_
#define _ISP_PLUGIN_H_INC_

#include <rtsavisp.h>

struct isp_plugin_manager {
	struct rts_isp_plugin plugs[_RTS_ISP_PLUG_ID_NUM];
};

int isp_plugin_init(struct isp_plugin_manager *pluginm);
void isp_plugin_cleanup(struct isp_plugin_manager *pluginm);
int isp_plugin_register(struct isp_plugin_manager *pluginm,
						const struct rts_isp_plugin *plug);
int isp_plugin_unregister(struct isp_plugin_manager *pluginm, int plug_id);
int isp_plugin_get(struct isp_plugin_manager *pluginm,
				   struct rts_isp_plugin *plug);
int isp_plugin_bind(struct isp_plugin_manager *pluginm, int plug_id,
					uint32_t isp_id);
int isp_plugin_unbind(struct isp_plugin_manager *pluginm, int plug_id,
					  uint32_t isp_id);
int isp_plugin_get_bind(struct isp_plugin_manager *pluginm, int plug_type,
						uint32_t isp_id);
const void *isp_plugin_handle(uint32_t isp_id, int plug_type);

#endif /* _ISP_PLUGIN_H_INC_ */
