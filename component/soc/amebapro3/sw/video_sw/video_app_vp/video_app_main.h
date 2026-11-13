/******************************************************************************
*
* Copyright(c) 2007 - 2025 Realtek Corporation. All rights reserved.
*
******************************************************************************/
#ifndef __VIDEO_APP_MAIN_H__
#define __VIDEO_APP_MAIN_H__

#include <stdlib.h>
#include "mmf2_module.h"
#include "video_common_type.h"

typedef enum {
	VIDEO_CH_CLOSED = 0,
	VIDEO_CH_FCS_STARTING,
	VIDEO_CH_FCS_READY,
	VIDEO_CH_STARTING,
	VIDEO_CH_RUNNING,
	VIDEO_CH_CLOSING,
	VIDEO_CH_ERROR,
} video_ch_state_t;

int video_app_stream_start(char *user_cmd);
int video_app_snapshot_start(int ch, struct vp_snapshot_result *reply, uint32_t id);
int video_app_stream_stop(int ch);
int video_app_fcs_attach(int ch);
int video_app_fcs_get_boot_state(void);

mm_context_t *video_app_isp_get_ctx(int ch);

mm_context_t *video_app_enc_get_ctx(int ch);

#endif /* __VIDEO_APP_MAIN_H__ */
