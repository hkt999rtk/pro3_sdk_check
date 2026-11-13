/*
 * Realtek Semiconductor Corp.
 * Copyright (C) 2017 Grant Shen <grant_shen@realsil.com.cn>
 */

#include <rts_isp_define.h>
#include <isp_os_driver.h>
#include <isp_os_mem.h>
#include <isp_utils.h>
#include "isp_plugin.h"

#define ISP_DEBUG_TYPE ISP_DEBUG_CORE

struct isp_cfg_plugin_item {
	uint8_t bind_ids;
	uint8_t linked;
	uint32_t link_info;
};

struct isp_cfg_plugin {
	struct isp_cfg_plugin_item items[32];
};

struct isp_cfg_plugin g_plugin;

static int isp_cfg_plugin_unbind(struct isp_cfg_plugin *plugin, int plug_id,
								 uint32_t isp_id)
{
	if (!plugin || plug_id < RTS_ISP_PLUG_ID_MIN ||
		plug_id > RTS_ISP_PLUG_ID_MAX || isp_id > _ISP_ID_NUM) {
		return -RTS_ISP_EINVAL;
	}

	if (plugin->items[plug_id].bind_ids & 1 << isp_id) {
		plugin->items[plug_id].bind_ids &= ~(1 << isp_id);
	}

	return RTS_ISP_OK;
}

static int isp_cfg_plugin_get_bind(struct isp_cfg_plugin *plugin, int plug_type,
								   uint32_t isp_id)
{
	int id;
	int id_min;
	int id_max;

	if (!plugin || plug_type < RTS_ISP_PLUG_TYPE_MIN ||
		plug_type > RTS_ISP_PLUG_TYPE_MAX || isp_id > _ISP_ID_NUM) {
		return -RTS_ISP_EINVAL;
	}

	rts_isp_plugin_id_range(plug_type, &id_min, &id_max);

	for (id = id_min; id <= id_max; id++)
		if (plugin->items[id].bind_ids & (1 << isp_id)) {
			return id;
		}
	return -RTS_ISP_EINVAL;
}

static int isp_cfg_plugin_bind(struct isp_cfg_plugin *plugin, int plug_id,
							   uint32_t isp_id)
{
	int bind_id;
	int plug_type;

	if (!plugin || plug_id < RTS_ISP_PLUG_ID_MIN ||
		plug_id > RTS_ISP_PLUG_ID_MAX || isp_id > _ISP_ID_NUM) {
		return -RTS_ISP_EINVAL;
	}

	plug_type = rts_isp_plugin_id_to_type(plug_id);
	bind_id = isp_cfg_plugin_get_bind(plugin, plug_type, isp_id);
	if (bind_id >= 0) {
		isp_cfg_plugin_unbind(plugin, bind_id, isp_id);
	}

	plugin->items[plug_id].bind_ids |= 1 << isp_id;

	return RTS_ISP_OK;
}

static int isp_plugin_register_link_info(const struct rts_isp_plugin *plug,
		uint32_t *link_info)
{
	void *(*ops)(void);
	void *handle;

	if (plug->id >= RTS_ISP_PLUG_IQ_ID_MIN &&
		plug->id <= RTS_ISP_PLUG_IQ_ID_MAX) {
		*link_info = (uint32_t)plug->entry;
	} else {
		ops = plug->entry;
		handle = ops();
		*link_info = (uint32_t)handle;
	}
	return 0;
}

const void *isp_plugin_handle(uint32_t isp_id, int plug_type)
{
	int id;
	int id_min;
	int id_max;

	if (isp_id >= _ISP_ID_NUM || plug_type < RTS_ISP_PLUG_TYPE_MIN ||
		plug_type > RTS_ISP_PLUG_TYPE_MAX) {
		return NULL;
	}

	rts_isp_plugin_id_range(plug_type, &id_min, &id_max);

	for (id = id_min; id <= id_max; id++) {
		if ((g_plugin.items[id].bind_ids & (1 << isp_id)) == 0) {
			continue;
		}
		return (void *)g_plugin.items[id].link_info;
	}
	return NULL;
}

int isp_plugin_init(struct isp_plugin_manager *pluginm)
{
	if (!pluginm) {
		return -RTS_ISP_EINVAL;
	}
	isp_memset(pluginm, 0, sizeof(*pluginm));
	return RTS_ISP_OK;
}

void isp_plugin_cleanup(struct isp_plugin_manager *pluginm)
{
	int i;

	if (!pluginm) {
		return;
	}

	for (i = RTS_ISP_PLUG_ID_MIN; i <= RTS_ISP_PLUG_ID_MAX; i++)
		if (pluginm->plugs[i].path) {
			isp_plugin_unregister(pluginm, i);
		}
	isp_memset(pluginm, 0, sizeof(*pluginm));
}

static void isp_set_link_info(int plug_id, uint32_t link_info, int bin_type)
{
	if (plug_id < RTS_ISP_PLUG_ID_MIN || plug_id > RTS_ISP_PLUG_ID_MAX) {
		return;
	}

	g_plugin.items[plug_id].link_info = link_info;
	if (bin_type == RTS_ISP_PLUG_BIN_TYPE_STATIC) {
		g_plugin.items[plug_id].linked = RTS_ISP_TRUE;
	}
}

int isp_plugin_register(struct isp_plugin_manager *pluginm,
						const struct rts_isp_plugin *plug)
{
	int i;
	int ret;
	uint32_t link_info;

	if (!pluginm || !plug || (!plug->path && !plug->entry)) {
		return -RTS_ISP_EINVAL;
	}
	if (plug->id < RTS_ISP_PLUG_ID_MIN || plug->id > RTS_ISP_PLUG_ID_MAX) {
		return -RTS_ISP_EINVAL;
	}
	if (plug->bin_type < RTS_ISP_PLUG_BIN_TYPE_FILE ||
		plug->bin_type > RTS_ISP_PLUG_BIN_TYPE_STATIC) {
		return -RTS_ISP_EINVAL;
	}
	if (pluginm->plugs[plug->id].path || pluginm->plugs[plug->id].entry) {
		return -RTS_ISP_ECONFLICT;
	}
	for (i = RTS_ISP_PLUG_ID_MIN; i <= RTS_ISP_PLUG_ID_MAX; i++) {
		if (!pluginm->plugs[i].path) {
			continue;
		}
		if (plug->bin_type != pluginm->plugs[i].bin_type) {
			continue;
		}
		if (plug->bin_type == RTS_ISP_PLUG_BIN_TYPE_STATIC &&
			pluginm->plugs[i].entry == plug->entry) {
			return -RTS_ISP_EEXIST;
		}
	}
	if (plug->bin_type == RTS_ISP_PLUG_BIN_TYPE_STATIC) {
		ret = isp_plugin_register_link_info(plug, &link_info);
		if (ret) {
			return ret;
		}
		isp_set_link_info(plug->id, link_info, plug->bin_type);
	}
	pluginm->plugs[plug->id] = *plug;
	if (!pluginm->plugs[plug->id].path) {
		isp_memset(&pluginm->plugs[plug->id], 0,
				   sizeof(pluginm->plugs[plug->id]));
		return -RTS_ISP_ENOMEM;
	}

	return RTS_ISP_OK;
}

int isp_plugin_unregister(struct isp_plugin_manager *pluginm, int plug_id)
{
	if (!pluginm || plug_id < RTS_ISP_PLUG_ID_MIN ||
		plug_id > RTS_ISP_PLUG_ID_MAX || !pluginm->plugs[plug_id].path) {
		return -RTS_ISP_EINVAL;
	}

	if (pluginm->plugs[plug_id].bin_type == RTS_ISP_PLUG_BIN_TYPE_FILE) {
		isp_os_free((void *)pluginm->plugs[plug_id].path);
	}
	isp_memset(&pluginm->plugs[plug_id], 0,
			   sizeof(pluginm->plugs[plug_id]));

	return RTS_ISP_OK;
}

int isp_plugin_get(struct isp_plugin_manager *pluginm,
				   struct rts_isp_plugin *plug)
{
	if (!pluginm || !plug || plug->id < RTS_ISP_PLUG_ID_MIN ||
		plug->id > RTS_ISP_PLUG_ID_MAX) {
		return -RTS_ISP_EINVAL;
	}

	if (pluginm->plugs[plug->id].id != plug->id) {
		return -RTS_ISP_EINVAL;
	}

	*plug = pluginm->plugs[plug->id];

	return RTS_ISP_OK;
}

int isp_plugin_bind(struct isp_plugin_manager *pluginm, int plug_id,
					uint32_t isp_id)
{
	return isp_cfg_plugin_bind(&g_plugin, plug_id, isp_id);
}

int isp_plugin_unbind(struct isp_plugin_manager *pluginm, int plug_id,
					  uint32_t isp_id)
{
	return isp_cfg_plugin_unbind(&g_plugin, plug_id, isp_id);
}

int isp_plugin_get_bind(struct isp_plugin_manager *pluginm, int plug_type,
						uint32_t isp_id)
{
	return isp_cfg_plugin_get_bind(&g_plugin, plug_type, isp_id);
}
