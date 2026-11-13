/******************************************************************************
*
* Copyright(c) 2007 - 2025 Realtek Corporation. All rights reserved.
*
******************************************************************************/
#ifndef __VIDEO_FCS_CONFIG_H__
#define __VIDEO_FCS_CONFIG_H__

#include "platform_autoconf.h"
#include "video_common_type.h"

/*
 * Fast Camera Start (FCS) shared configuration.
 *
 * This header is the single source of truth for the FCS video pipeline
 * parameters. Both VP and AP derive their defaults from the macros below.
 *
 * Included by:
 *   - sw/video_sw/video_app_vp/video_app_main.c
 *   - media_example/ap_media_example/video_example_src/fast_camera_start/
 *     mmf2_video_example_fast_camera_start_init.c     (AP: supplies channel params)
 */

/* ---- User-configurable settings ----------------------------------------- */

#if defined(CONFIG_4K_DEV) && (CONFIG_4K_DEV == 1)
/* 4K portrait pipeline: V1, V2 and V5 are outputs of one 4K ISP pass, sized by
 * the VP ISP 4K module (ISP_4K_OUTPUT_*, ISP_4K_SUB_OUTPUT_*, ISP_4K_RGB_*).
 * V3 and V4 are not supported. Same streams as mmf2_video_example_v1_4k_mp4_uvc_rgb_init(). */
#define FCS_AINR_ENA    0

#define FCS_V1_ENA      1
#define FCS_V2_ENA      1
#define FCS_V3_ENA      0
#define FCS_V4_ENA      0
#define FCS_V5_ENA      1

#define FCS_V1_W        2160
#define FCS_V1_H        3840
#define FCS_V1_FPS      20
#define FCS_V1_GOP      20
#define FCS_V1_BPS      2000000
#define FCS_V1_QLEVEL   0
#define FCS_V1_CODEC    H264

#define FCS_V2_W        1440
#define FCS_V2_H        2560
#define FCS_V2_FPS      20
#define FCS_V2_GOP      20
#define FCS_V2_BPS      2000000
#define FCS_V2_QLEVEL   0
#define FCS_V2_CODEC    H264
#else
/* AINR (AI noise reduction): set to 1 to enable, 0 to disable */
#define FCS_AINR_ENA    1

/* Video channel enable */
#define FCS_V1_ENA      1  /* V1(CH0) for video recording */
#define FCS_V2_ENA      1  /* V2(CH1) for live streaming */
#define FCS_V3_ENA      1  /* V3(CH2) for snapshot */
#define FCS_V4_ENA      1  /* V4(CH3) for motion detection */
#define FCS_V5_ENA      1  /* V5(CH4) for NN input */

/* Encoder codec per channel: set to HEVC, H264, or JPEG. */

/* V1: video recording (CH0) */
#define FCS_V1_W        2688
#define FCS_V1_H        1520
#define FCS_V1_FPS      30
#define FCS_V1_GOP      30
#define FCS_V1_BPS      2000000
#define FCS_V1_QLEVEL   5
#define FCS_V1_CODEC    HEVC

/* V2: live stream — UVC or RTSP (CH1) */
#define FCS_V2_W        1280
#define FCS_V2_H        720
#define FCS_V2_FPS      15
#define FCS_V2_GOP      15
#define FCS_V2_BPS      500000
#define FCS_V2_QLEVEL   0
#define FCS_V2_CODEC    H264
#endif

/* V3: snapshot (CH2)
 * If switching away from JPEG, adjust bps/rcmode/minqp/maxqp accordingly. */
#define FCS_V3_W        1920
#define FCS_V3_H        1080
#define FCS_V3_FPS      1
#define FCS_V3_GOP      1       /* 1: every frame is a keyframe (JPEG default) */
#define FCS_V3_QLEVEL   5
#define FCS_V3_CODEC    JPEG
#define FCS_V3_BPS      0       /* unused for JPEG; set non-zero when switching to H264/HEVC */
#define FCS_V3_RCMODE   0       /* unused for JPEG; see FCS_ENC_RCMODE for other codecs */
#define FCS_V3_MINQP    0       /* unused for JPEG; see FCS_ENC_MINQP for other codecs */
#define FCS_V3_MAXQP    0       /* unused for JPEG; see FCS_ENC_MAXQP for other codecs */

/* V4: NV12 raw frames, no encoder (CH3) */
#define FCS_V4_W        640
#define FCS_V4_H        480
#define FCS_V4_FPS      10

/* V5: RGB raw frames for NN, no encoder (CH4) */
#if defined(CONFIG_4K_DEV) && (CONFIG_4K_DEV == 1)
#define FCS_V5_W        576
#define FCS_V5_H        320
#define FCS_V5_FPS      5
#else
#define FCS_V5_W        416
#define FCS_V5_H        416
#define FCS_V5_FPS      10
#endif

/* ---- Internal constants (do not modify) ---------------------------------- */

/* Shared encoder rate-control params for V1/V2 (HEVC/H264) */
/* rcmode: 0=ENC_CVBR  1=ENC_CBR  2=ENC_VBR  3=ENC_ABR  4=ENC_CRF  5=ENC_CQP */
#define FCS_ENC_RCMODE      2   /* ENC_VBR */
#define FCS_ENC_MINQP       26
#define FCS_ENC_MAXQP       48

#define FCS_CHANNEL_ENABLE_MASK \
	((FCS_V1_ENA << V1_STREAM_ID) | (FCS_V2_ENA << V2_STREAM_ID) | \
	 (FCS_V3_ENA << V3_STREAM_ID) | (FCS_V4_ENA << V4_STREAM_ID) | \
	 (FCS_V5_ENA << V5_STREAM_ID))

/* Map a codec token (HEVC/H264/JPEG) to its VIDEO_* value. */
#define FCS_CODEC_VAL(c)    _FCS_CODEC_VAL(c)
#define _FCS_CODEC_VAL(c)   VIDEO_##c

#endif /* __VIDEO_FCS_CONFIG_H__ */
