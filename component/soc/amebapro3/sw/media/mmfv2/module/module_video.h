/******************************************************************************
*
* Copyright(c) 2007 - 2025 Realtek Corporation. All rights reserved.
*
******************************************************************************/
#ifndef _MODULE_VIDEO_H
#define _MODULE_VIDEO_H

#include <stdint.h>
#include "mmf2_module.h"
#include "video_common_type.h"

#define CMD_VIDEO_SET_PARAMS		MM_MODULE_CMD(0x00)  // set parameter
#define CMD_VIDEO_GET_PARAMS		MM_MODULE_CMD(0x01)  // get parameter
#define CMD_VIDEO_SET_HEIGHT		MM_MODULE_CMD(0x02)
#define CMD_VIDEO_SET_WIDTH			MM_MODULE_CMD(0x03)
#define CMD_VIDEO_BITRATE			MM_MODULE_CMD(0x04)
#define CMD_VIDEO_FPS				MM_MODULE_CMD(0x05)
#define CMD_VIDEO_GOP				MM_MODULE_CMD(0x06)
#define CMD_VIDEO_FORCE_IFRAME		MM_MODULE_CMD(0x07)
#define CMD_VIDEO_AINR				MM_MODULE_CMD(0x08)
#define CMD_VIDEO_RCMODE			MM_MODULE_CMD(0x0a)
#define CMD_VIDEO_SET_RCPARAM		MM_MODULE_CMD(0x0b)
#define CMD_VIDEO_GET_RCPARAM		MM_MODULE_CMD(0x0c)
#define CMD_VIDEO_GET_RC_EXT		MM_MODULE_CMD(0x0d)
#define CMD_VIDEO_SET_RC_EXT		MM_MODULE_CMD(0x0e)
#define CMD_VIDEO_GET_CODING		MM_MODULE_CMD(0x0f)
#define CMD_VIDEO_SET_CODING		MM_MODULE_CMD(0x10)
#define CMD_VIDEO_QP_HDR			MM_MODULE_CMD(0x11)
#define CMD_VIDEO_INTRA_QP_DELTA	MM_MODULE_CMD(0x12)
#define CMD_VIDEO_FIXED_INTRA_QP	MM_MODULE_CMD(0x13)
#define CMD_VIDEO_SET_I_QP			MM_MODULE_CMD(0x14)
#define CMD_VIDEO_SET_QP			MM_MODULE_CMD(0x15)
#define CMD_VIDEO_MINQP				MM_MODULE_CMD(0x16)
#define CMD_VIDEO_MAXQP				MM_MODULE_CMD(0x17)
#define CMD_VIDEO_PIC_RC			MM_MODULE_CMD(0x18)
#define CMD_VIDEO_CTB_RC			MM_MODULE_CMD(0x19)
#define CMD_VIDEO_BLOCK_RC_SIZE		MM_MODULE_CMD(0x1a)
#define CMD_VIDEO_PICTURE_SKIP		MM_MODULE_CMD(0x1b)
#define CMD_VIDEO_BITRATE_WINDOW	MM_MODULE_CMD(0x1c)
#define CMD_VIDEO_MOVING_BITRATE	MM_MODULE_CMD(0x1d)
#define CMD_VIDEO_CRF				MM_MODULE_CMD(0x1e)
#define CMD_VIDEO_VBR				MM_MODULE_CMD(0x1f)

#define CMD_VIDEO_APPLY				MM_MODULE_CMD(0x20)  // apply setting
#define CMD_VIDEO_FCS_ATTACH		MM_MODULE_CMD(0x21)  // attach AP to an FCS-prestarted channel
#define CMD_VIDEO_FCS_GET_INFO		MM_MODULE_CMD(0x22)  // arg: video_first_frame_info_t *
#define CMD_VIDEO_STREAM_STOP		MM_MODULE_CMD(0x23)  // stop stream
#define CMD_VIDEO_SLICE_SIZE		MM_MODULE_CMD(0x24)
#define CMD_VIDEO_SEI_USER_DATA		MM_MODULE_CMD(0x25)
#define CMD_VIDEO_DEBLOCK			MM_MODULE_CMD(0x26)
#define CMD_VIDEO_CIR				MM_MODULE_CMD(0x27)
#define CMD_VIDEO_ROI				MM_MODULE_CMD(0x28)
#define CMD_VIDEO_SET_OSD_BITMAP	MM_MODULE_CMD(0x29)
#define CMD_VIDEO_DISABLE_OSD		MM_MODULE_CMD(0x2a)
#define CMD_VIDEO_SET_MOSAIC		MM_MODULE_CMD(0x2b)
#define CMD_VIDEO_DISABLE_MOSAIC	MM_MODULE_CMD(0x2c)
#define CMD_VIDEO_SET_OUTPUT_GATE	MM_MODULE_CMD(0x2d) /* video_output_gate_t * */
#define CMD_VIDEO_GET_SNAPSHOT		MM_MODULE_CMD(0x2e) /* V1/V2/V3 VIDEO_H264_JPEG or VIDEO_HEVC_JPEG; video_snapshot_t * */

/*
 * Encoder extra parameters for AP examples:
 *
 * Picture RC defaults to enabled for non-CQP modes, with automatic initial
 * QP estimation. CQP defaults to min_qp. Explicit QP_HDR/PIC_RC flags override
 * these defaults. Selecting a mode at runtime restores its picture RC default;
 * bitrate/QP-only updates preserve the current picture RC setting.
 * CTB RC defaults to mixed mode (3), except CQP (0) and CRF (1). An explicit
 * CTB_RC flag overrides this default. CTB RC buffers are allocated at open;
 * enabling precise RC later requires reopening if no buffers were allocated.
 * Selecting a mode at runtime restores the CTB mode configured at open,
 * limited to 0 for CQP and subjective RC for CRF.
 *
 * Recommended flow for application code:
 *   1. Fill basic video_params_t fields such as stream_id/format/width/fps/bps.
 *   2. Before CMD_VIDEO_SET_PARAMS, call video_params_* helpers below for
 *      init-time encoder extras such as profile, SEI, SAR, or CABAC.
 *   3. After CMD_VIDEO_APPLY, use the CMD_VIDEO_* runtime controls below via
 *      mm_module_ctrl() for dynamic controls such as bitrate, QP, ROI, CIR,
 *      or SEI user-data enable.
 *
 * rc_ext and coding still expose a flags field for advanced users. A field
 * value of 0 can be meaningful, so VP ignores a field unless its flag is set.
 * The flags distinguish "user did not set this field" from "user explicitly
 * set this field to 0". For example, pictureRc = 0, ctbRc = 0, seiMessages = 0,
 * or enableCabac = 0 are valid requests to disable those features and still
 * need their corresponding flags.
 *
 * Example:
 *   .rc_ext = {
 *       .flags = VPAP_ENC_RC_EXT_QP_HDR | VPAP_ENC_RC_EXT_PIC_RC,
 *       .qpHdr = -1,
 *       .pictureRc = 1,
 *   },
 *   .coding = {
 *       .flags = VPAP_ENC_CODING_SEI | VPAP_ENC_CODING_SEI_USER_DATA,
 *       .seiMessages = 1,
 *       .seiUserData = 1,
 *   },
 *
 * vpap_shared_enc_rc_ext_t fields and ranges:
 *   VPAP_ENC_RC_EXT_RCMODE:
 *     rcMode: 0=CVBR, 1=CBR, 2=VBR, 3=ABR, 4=CRF, 5=CQP.
 *   VPAP_ENC_RC_EXT_BPS:
 *     bps: target bitrate in bps. When RC is enabled, minimum is 10000.
 *     Maximum is limited by codec profile/level/tier and current frame rate.
 *   VPAP_ENC_RC_EXT_QP_HDR:
 *     qpHdr: -1..51. -1 lets RC choose initial/default picture QP.
 *   VPAP_ENC_RC_EXT_QP_RANGE:
 *     minQp/maxQp: 0..51, and minQp must be <= maxQp.
 *   VPAP_ENC_RC_EXT_I_QP_RANGE:
 *     minIQp/maxIQp: 0..51, and minIQp must be <= maxIQp.
 *     These override minQp/maxQp for I frames.
 *   VPAP_ENC_RC_EXT_INTRA_QP_DELTA:
 *     intraQpDelta: -51..51. Delta between target/inter QP and intra QP.
 *   VPAP_ENC_RC_EXT_FIXED_INTRA_QP:
 *     fixedIntraQp: 0..51. 0 means intra QP is not fixed.
 *   VPAP_ENC_RC_EXT_PIC_RC:
 *     pictureRc: 0=disable, 1=enable picture-level rate control.
 *   VPAP_ENC_RC_EXT_CTB_RC:
 *     ctbRc: 0=disable, 1=subjective RC, 2=precise RC, 3=mixed RC.
 *     Requires CTB RC HW support and valid stream status.
 *   VPAP_ENC_RC_EXT_BLOCK_RC_SIZE:
 *     blockRCSize: 0=64x64, 1=32x32, 2=16x16. Used with CTB RC.
 *   VPAP_ENC_RC_EXT_PIC_SKIP:
 *     pictureSkip: 0=disallow, 1=allow RC to skip pictures.
 *   VPAP_ENC_RC_EXT_HRD:
 *     hrd: 0=disable, 1=enable HRD conformance. HRD conflicts with vbr=1.
 *   VPAP_ENC_RC_EXT_CPB:
 *     hrdCpbSize: CPB size in bits. 0 uses level default; 0xffffffff uses bps.
 *     cpbMaxRate: max CPB bitrate in bps. 0 lets driver derive it from bps.
 *   VPAP_ENC_RC_EXT_BITRATE_WINDOW:
 *     bitrateWindow: 1..300 frames. RC averaging/target window.
 *   VPAP_ENC_RC_EXT_MOVING_BITRATE:
 *     tolMovingBitRate: 0..2000 percent tolerance over bps.
 *     monitorFrames: 10..120 frames for moving bitrate calculation.
 *   VPAP_ENC_RC_EXT_FILLER_DATA:
 *     fillerData: 0=disable, 1=enable. Requires hrdCpbSize != 0 and pictureRc=1.
 *   VPAP_ENC_RC_EXT_CRF:
 *     crf: -1..51. -1 disables CRF. CRF requires look-ahead/pass support and
 *     cannot be used with ctbRc >= 2.
 *   VPAP_ENC_RC_EXT_VBR:
 *     vbr: 0=disable, 1=enable VBR-style control. Cannot be enabled with hrd=1.
 *   VPAP_ENC_RC_EXT_FRAME_RATE:
 *     frameRateNum/frameRateDenom: both must be > 0. For init config,
 *     frameRateNum is also limited to <= (1 << 20) - 1 by the encoder driver.
 *
 * vpap_shared_enc_coding_t fields and ranges:
 *   VPAP_ENC_CODING_SLICE_SIZE:
 *     sliceSize: 0..ctbPerCol. 0 disables slice splitting. For HEVC this is
 *     CTB rows; for H.264 this is MB rows. Not supported with multi-tile mode.
 *   VPAP_ENC_CODING_SEI:
 *     seiMessages: 0=disable, 1=enable driver-generated picture timing and
 *     buffering period SEI messages. This does not carry customer metadata.
 *   VPAP_ENC_CODING_SEI_USER_DATA:
 *     seiUserData: 0=disable, 1=insert Pro3 user_data_unregistered SEI before
 *     each encoded H.264/H.265 frame. The current implementation writes only
 *     the 16-byte UUID payload placeholder. Extend the VP encoder payload
 *     builder when the real Pro3 metadata format is defined.
 *   VPAP_ENC_CODING_FULL_RANGE:
 *     vuiVideoFullRange: 0=limited range, 1=full range in VUI.
 *   VPAP_ENC_CODING_DEBLOCK:
 *     disableDeblockingFilter: 0=enable deblocking, 1=disable deblocking.
 *     tcOffset: -6..6. betaOffset: -6..6.
 *   VPAP_ENC_CODING_SAO:
 *     enableSao: 0=disable, 1=enable HEVC SAO. Requires HW SAO support.
 *   VPAP_ENC_CODING_CABAC:
 *     enableCabac: H.264 only, 0=CAVLC, 1=CABAC. H.264 baseline profile
 *     cannot enable CABAC.
 *   VPAP_ENC_CODING_SAR:
 *     sampleAspectRatioWidth/Height: 0..65535. Set both fields together.
 *     Leave unset for the normal unspecified/square-pixel case. Set only for
 *     non-default display aspect-ratio requirements.
 *   VPAP_ENC_CODING_CIR:
 *     cirStart/cirInterval: 0..ctbPerFrame. 0 disables the corresponding CIR
 *     position/interval behavior.
 *   VPAP_ENC_CODING_ROI:
 *     roi[i].enable: 0=disable, 1=enable. Up to VPAP_ENC_MAX_ROI regions.
 *     roi[i].left/right: CTB or MB column, 0..ctbPerRow-1, left <= right.
 *     roi[i].top/bottom: CTB or MB row, 0..ctbPerCol-1, top <= bottom.
 *     roi[i].deltaQp: -51..51 when absolute-QP ROI is supported by HW, otherwise
 *     -30..0 for ROI1/ROI2. roi[i].qp: 0..51 absolute ROI QP, when supported.
 *
 * These fields are sent in the AP->VP open command and applied before encoder
 * start. For runtime updates, prefer video_api.h helpers for the fields that
 * are explicitly exposed as dynamic commands.
 */
typedef struct video_params_s {
	uint32_t stream_id;
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
	uint32_t ainr_en;  // global; first opened channel (first selected for open-all) determines AINR
	int32_t profile;
	uint32_t level;
	uint32_t tier;
	uint8_t profile_set;
	uint8_t level_set;
	uint8_t tier_set;
	vpap_shared_enc_rc_ext_t rc_ext;           // init-time encoder rate-control extras
	vpap_shared_enc_coding_t coding;           // init-time bitstream/coding extras
	uint32_t isp_slice_en;                  // ISP-to-encoder NV12 handoff; unrelated to coding.sliceSize
} video_params_t;

typedef struct video_qp_range_s {
	uint32_t min_qp;
	uint32_t max_qp;
} video_qp_range_t;

typedef struct video_moving_bitrate_s {
	int32_t tolerance;
	int32_t frames;
} video_moving_bitrate_t;

typedef struct video_deblock_s {
	uint32_t disable;
	int32_t tc_offset;
	int32_t beta_offset;
} video_deblock_t;

typedef struct video_cir_s {
	uint32_t start;
	uint32_t interval;
} video_cir_t;

typedef struct video_roi_s {
	int idx;
	uint32_t enable;
	uint32_t top;
	uint32_t left;
	uint32_t bottom;
	uint32_t right;
	int32_t delta_qp;
	int32_t qp;
} video_roi_t;

static inline void video_params_set_profile_level(video_params_t *p, int32_t profile, uint32_t level)
{
	p->profile = profile;
	p->level = level;
	p->profile_set = 1;
	p->level_set = 1;
}

static inline void video_params_set_tier(video_params_t *p, uint32_t tier)
{
	p->tier = tier;
	p->tier_set = 1;
}

static inline void video_params_set_qp_hdr(video_params_t *p, int32_t qp_hdr)
{
	p->rc_ext.flags |= VPAP_ENC_RC_EXT_QP_HDR;
	p->rc_ext.qpHdr = qp_hdr;
}

static inline void video_params_set_driver_sei(video_params_t *p, uint32_t enable)
{
	p->coding.flags |= VPAP_ENC_CODING_SEI;
	p->coding.seiMessages = enable;
}

static inline void video_params_set_sei_user_data(video_params_t *p, uint32_t enable)
{
	p->coding.flags |= VPAP_ENC_CODING_SEI_USER_DATA;
	p->coding.seiUserData = enable;
}

static inline void video_params_set_cabac(video_params_t *p, uint32_t enable)
{
	p->coding.flags |= VPAP_ENC_CODING_CABAC;
	p->coding.enableCabac = enable;
}

static inline void video_params_set_sar(video_params_t *p, uint32_t width, uint32_t height)
{
	p->coding.flags |= VPAP_ENC_CODING_SAR;
	p->coding.sampleAspectRatioWidth = width;
	p->coding.sampleAspectRatioHeight = height;
}

/*
 * First frame a channel delivered to AP, read back with
 * CMD_VIDEO_FCS_GET_INFO. Useful for FCS, where the channel is already
 * streaming on VP before AP attaches: comparing `timestamp` (media time VP
 * stamped at encode) with `arrive_ms` (media time it reached AP) shows how long
 * the frame was in flight, and comparing `timestamp` against the audio module's
 * CMD_AUDIO_GET_FIRST_DATA_TS gives the audio/video start offset.
 */
typedef struct video_first_frame_info_s {
	uint32_t valid;         /* 0 until the first frame has been delivered */
	uint32_t timestamp;     /* media time stamped by VP when the frame was encoded */
	uint32_t arrive_ms;     /* media time the frame was handed to the AP pipeline */
} video_first_frame_info_t;

/* Suppress delivery while configuring privacy OSD. Opening with a minimum
 * timestamp also discards older encoded frames still travelling through IPC.
 * Timestamp comparisons use signed wraparound arithmetic (window < 2^31 ms). */
typedef struct {
	uint32_t blocked;
	uint32_t minimum_timestamp;
	uint32_t filter_timestamp;
} video_output_gate_t;

typedef struct video_ctx_s {
	void *parent;
	video_params_t params;
	char init_cmd[1024];
	video_first_frame_info_t first_frame;
	video_output_gate_t output_gate;
} video_ctx_t;

void video_module_stream_start(mm_context_t *m1, mm_context_t *m2, mm_context_t *m3, mm_context_t *m4, mm_context_t *m5);

extern mm_module_t video_module;

#endif
