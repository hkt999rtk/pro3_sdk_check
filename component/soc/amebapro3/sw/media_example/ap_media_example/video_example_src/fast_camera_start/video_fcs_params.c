/******************************************************************************
*
* Copyright(c) 2007 - 2025 Realtek Corporation. All rights reserved.
*
******************************************************************************/
#include "video_fcs_params.h"
#include "video_fcs_config.h"   /* FCS_V*_* compile-time defaults */
#include "ameba_soc.h"          /* DCache_Clean */
#include <string.h>

/*
 * Populate PSRAM struct from compile-time FCS_V*_* defaults.
 * Called when no valid params exist yet (first FCS boot or cold reset).
 * The crc field is intentionally NOT written here; caller must update it.
 */
void fcs_params_init_defaults(fcs_persistent_params_t *p)
{
	memset(p, 0, sizeof(*p));
	p->magic    = FCS_PARAMS_MAGIC;
	p->version  = FCS_PARAMS_VERSION;
	p->struct_size = sizeof(fcs_persistent_params_t);
	p->channel_enable_mask = FCS_CHANNEL_ENABLE_MASK;
	p->ainr_ena = FCS_AINR_ENA;

	/* V1 */
	p->ch[0].format  = FCS_CODEC_VAL(FCS_V1_CODEC);
	p->ch[0].width   = FCS_V1_W;
	p->ch[0].height  = FCS_V1_H;
	p->ch[0].fps     = FCS_V1_FPS;
	p->ch[0].gop     = FCS_V1_GOP;
	p->ch[0].bps     = FCS_V1_BPS;
	p->ch[0].rc_mode = FCS_ENC_RCMODE;
	p->ch[0].min_qp  = FCS_ENC_MINQP;
	p->ch[0].max_qp  = FCS_ENC_MAXQP;
	p->ch[0].qlevel  = FCS_V1_QLEVEL;

	/* V2 */
	p->ch[1].format  = FCS_CODEC_VAL(FCS_V2_CODEC);
	p->ch[1].width   = FCS_V2_W;
	p->ch[1].height  = FCS_V2_H;
	p->ch[1].fps     = FCS_V2_FPS;
	p->ch[1].gop     = FCS_V2_GOP;
	p->ch[1].bps     = FCS_V2_BPS;
	p->ch[1].rc_mode = FCS_ENC_RCMODE;
	p->ch[1].min_qp  = FCS_ENC_MINQP;
	p->ch[1].max_qp  = FCS_ENC_MAXQP;
	p->ch[1].qlevel  = FCS_V2_QLEVEL;

	/* V3 */
	p->ch[2].format  = FCS_CODEC_VAL(FCS_V3_CODEC);
	p->ch[2].width   = FCS_V3_W;
	p->ch[2].height  = FCS_V3_H;
	p->ch[2].fps     = FCS_V3_FPS;
	p->ch[2].gop     = FCS_V3_GOP;
	p->ch[2].bps     = FCS_V3_BPS;
	p->ch[2].rc_mode = FCS_V3_RCMODE;
	p->ch[2].min_qp  = FCS_V3_MINQP;
	p->ch[2].max_qp  = FCS_V3_MAXQP;
	p->ch[2].qlevel  = FCS_V3_QLEVEL;

	/* V4 — raw NV12, no encoder */
	p->ch[3].format  = VIDEO_NV12;
	p->ch[3].width   = FCS_V4_W;
	p->ch[3].height  = FCS_V4_H;
	p->ch[3].fps     = FCS_V4_FPS;
	p->ch[3].gop     = 0;
	p->ch[3].bps     = 0;
	p->ch[3].rc_mode = 0;
	p->ch[3].min_qp  = 0;
	p->ch[3].max_qp  = 0;
	p->ch[3].qlevel  = 0;

	/* V5 — raw RGB, no encoder */
	p->ch[4].format  = VIDEO_RGB;
	p->ch[4].width   = FCS_V5_W;
	p->ch[4].height  = FCS_V5_H;
	p->ch[4].fps     = FCS_V5_FPS;
	p->ch[4].gop     = 0;
	p->ch[4].bps     = 0;
	p->ch[4].rc_mode = 0;
	p->ch[4].min_qp  = 0;
	p->ch[4].max_qp  = 0;
	p->ch[4].qlevel  = 0;
}

/*
 * The shared PSRAM struct is always seeded with compile-time defaults on first
 * power-on so VP can read valid params on a subsequent boot. The VP owns the
 * authoritative decision whether FCS runs for the current boot.
 */
void fcs_params_prepare(void)
{
	fcs_persistent_params_t *p = FCS_PARAMS_ADDR;

	DCache_Invalidate((u32)p, sizeof(fcs_persistent_params_t));
	/* Seed defaults on cold boot unconditionally (the cross-core contract is
	 * that the struct is always valid once any core has booted). */
	if (!fcs_params_valid(p)) {
		fcs_params_init_defaults(p);
		p->crc = fcs_crc_compute(p);
		/* PSRAM RET is write-back cacheable on AP; flush so VP sees valid params */
		DCache_Clean((u32)p, sizeof(fcs_persistent_params_t));
		RTK_LOGI("FCS", "[AP FCS] no valid params in psram (cold boot) -> init defaults\r\n");
	}
}
