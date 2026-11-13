/*
 * Realtek Semiconductor Corp.
 * Copyright (C) 2017 Grant Shen <grant_shen@realsil.com.cn>
 */

#include "isp_top.h"
#include "isp_plugin.h"
#include "isp_algo.h"

#define ISP_DEBUG_TYPE ISP_DEBUG_ALGO

static int algo_get_algo(uint32_t isp_id, int type, const void **algo)
{
	if (isp_id >= _ISP_ID_NUM || !algo ||
		type < RTS_ISP_PLUG_TYPE_ALGO_AE ||
		type > RTS_ISP_PLUG_TYPE_ALGO_OTHER) {
		return -RTS_ISP_EINVAL;
	}

	*algo = isp_plugin_handle(isp_id, type);
	if (!*algo) {
		return -RTS_ISP_ENOTREADY;
	}

	return RTS_ISP_OK;
}

#define ALGO_GET_FUNC(type, upper)                                           \
	int isp_algo_get_##type(                                             \
		uint32_t isp_id,                                             \
		const struct rts_isp_##type##_algo **type##_algo)            \
	{                                                                    \
		return algo_get_algo(isp_id, RTS_ISP_PLUG_TYPE_ALGO_##upper, \
				     (const void **)type##_algo);            \
	}

#define ALGO_GET_OTHER_FUNC(type)                                         \
	int isp_algo_get_##type(                                          \
		uint32_t isp_id,                                          \
		const struct rts_isp_##type##_algo **type##_algo)         \
	{                                                                 \
		int ret;                                                  \
		const struct rts_isp_other_algo *other;                   \
		if (!type##_algo)                                         \
			return -RTS_ISP_EINVAL;                           \
		ret = algo_get_algo(isp_id, RTS_ISP_PLUG_TYPE_ALGO_OTHER, \
				    (const void **)&other);               \
		if (ret)                                                  \
			return ret;                                       \
		*type##_algo = &other->type;                              \
		return RTS_ISP_OK;                                        \
	}

ALGO_GET_FUNC(ae, AE);
ALGO_GET_FUNC(awb, AWB);
ALGO_GET_FUNC(af, AF);
ALGO_GET_OTHER_FUNC(flick);
ALGO_GET_OTHER_FUNC(wdr);
