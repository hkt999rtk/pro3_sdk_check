/******************************************************************************
*
* Copyright(c) 2007 - 2025 Realtek Corporation. All rights reserved.
*
******************************************************************************/
#ifndef __VIDEO_FCS_PARAMS_H__
#define __VIDEO_FCS_PARAMS_H__

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "ameba_layout.h"
#include "ameba_soc.h"
#include "video_common_type.h"
#include "video_fcs_config.h"

/* AP/VP shared parameters stored at the end of the AP PSRAM retention region. */
#define FCS_PARAMS_MAGIC        0x46435350UL    /* 'F','C','S','P' */
#define FCS_PARAMS_VERSION      1UL

#define FCS_PARAMS_ADDR \
	((fcs_persistent_params_t *)(AP_PSRAM_RET_ORIGIN + AP_PSRAM_RET_LENGTH - AP_PSRAM_RET_FCS_LENGTH))

typedef struct {
	uint32_t format;
	uint32_t width;
	uint32_t height;
	uint32_t fps;
	uint32_t gop;
	uint32_t bps;
	uint32_t rc_mode;
	uint32_t min_qp;
	uint32_t max_qp;
	uint32_t qlevel;
} fcs_ch_params_t;

typedef struct {
	uint32_t magic;
	uint32_t version;
	uint32_t struct_size;
	uint32_t channel_enable_mask;
	uint32_t ainr_ena;
	fcs_ch_params_t ch[VIDEO_ISP_VALID_CH];
	uint32_t crc;
} fcs_persistent_params_t;

_Static_assert(sizeof(fcs_persistent_params_t) <= AP_PSRAM_RET_FCS_LENGTH,
			   "FCS parameters exceed the reserved retention region");

static inline uint32_t fcs_crc_compute(const fcs_persistent_params_t *p)
{
	const uint8_t *d = (const uint8_t *)p;
	size_t len = offsetof(fcs_persistent_params_t, crc);
	uint32_t h = 2166136261u;
	size_t i;

	for (i = 0; i < len; i++) {
		h ^= d[i];
		h *= 16777619u;
	}
	return h;
}

static inline int fcs_params_valid(const fcs_persistent_params_t *p)
{
	int i;

	if (p->magic != FCS_PARAMS_MAGIC ||
		p->version != FCS_PARAMS_VERSION ||
		p->struct_size != sizeof(fcs_persistent_params_t) ||
		p->ainr_ena > 1 ||
		p->channel_enable_mask != FCS_CHANNEL_ENABLE_MASK) {
		return 0;
	}
	if (p->crc != fcs_crc_compute(p)) {
		return 0;
	}

	for (i = 0; i < VIDEO_ISP_VALID_CH; i++) {
		const fcs_ch_params_t *c = &p->ch[i];

		if ((p->channel_enable_mask & (1UL << i)) == 0) {
			continue;
		}
		if (c->width == 0 || c->height == 0 || c->fps == 0) {
			return 0;
		}
		if (i == V4_STREAM_ID && c->format != VIDEO_NV12) {
			return 0;
		}
		if (i == V5_STREAM_ID && c->format != VIDEO_RGB) {
			return 0;
		}
		if (i < VIDEO_ENC_VALID_CH && c->format != VIDEO_HEVC &&
			c->format != VIDEO_H264 && c->format != VIDEO_JPEG &&
			c->format != VIDEO_NV12 && c->format != VIDEO_RGB) {
			return 0;
		}
		if (c->format != VIDEO_NV12 && c->format != VIDEO_RGB &&
			(c->gop == 0 || c->rc_mode > ENC_CQP || c->min_qp > c->max_qp)) {
			return 0;
		}
	}
	return 1;
}

void fcs_params_init_defaults(fcs_persistent_params_t *p);
void fcs_params_prepare(void);

#endif /* __VIDEO_FCS_PARAMS_H__ */
