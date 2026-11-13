/******************************************************************************
*
* Copyright(c) 2007 - 2025 Realtek Corporation. All rights reserved.
*
******************************************************************************/
#ifndef __MMF2_VIDEO_EXAMPLE_FAST_CAMERA_START_INIT_H__
#define __MMF2_VIDEO_EXAMPLE_FAST_CAMERA_START_INIT_H__

#include "module_video.h"

void mmf2_video_example_fast_camera_start_rtsp_init(void);
int mmf2_fcs_video_channel_reopen(int ch, const video_params_t *new_params);
int mmf2_fcs_params_save_current(void);

#endif /* __MMF2_VIDEO_EXAMPLE_FAST_CAMERA_START_INIT_H__ */
