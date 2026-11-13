/******************************************************************************
*
* Copyright(c) 2007 - 2025 Realtek Corporation. All rights reserved.
*
******************************************************************************/
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <math.h>

#include <FreeRTOS.h>
#include <task.h>
#include <queue.h>
#include <semphr.h>

#include "ameba_soc.h"
#include "mmf2.h"
#include "mmf2_dbg.h"
#include "module_video.h"
#include "video_api.h"
#include "video_common_type.h"
#include "mmf2_mediatime_pro3.h"
#include "avcodec.h"

typedef struct v_global_s {
	int module_cnt;
} v_global_t;

static v_global_t g_video;

static void video_frame_complete_cb(void *v, void *arg)
{
	video_ctx_t *ctx = (video_ctx_t *)arg;
	mm_context_t *mctx = (mm_context_t *)ctx->parent;
	v_out_t *v_frm = (v_out_t *)v;
	video_output_gate_t gate;
	rtos_critical_enter(RTOS_CRITICAL_DEFAULT);
	gate = ctx->output_gate;
	rtos_critical_exit(RTOS_CRITICAL_DEFAULT);
	if (gate.blocked || (gate.filter_timestamp && (int32_t)(v_frm->timestamp - gate.minimum_timestamp) < 0)) {
		video_release_buf(v_frm->ch);
		return;
	}
	if (gate.filter_timestamp) {
		/* Per-stream frame notifications are ordered. Retire the startup filter
		 * after the first eligible frame so long uptime cannot wrap the comparison. */
		rtos_critical_enter(RTOS_CRITICAL_DEFAULT);
		if (!ctx->output_gate.blocked && ctx->output_gate.minimum_timestamp == gate.minimum_timestamp) {
			ctx->output_gate.filter_timestamp = 0;
		}
		rtos_critical_exit(RTOS_CRITICAL_DEFAULT);
	}

	if (v_frm->type == AV_CODEC_ID_H265 || v_frm->type == AV_CODEC_ID_H264 || v_frm->type == AV_CODEC_ID_JPEG) {
		DCache_Invalidate((u32)v_frm->addr, v_frm->size);
	}
	VIDEO_DBG_DEBUG("video_frame_complete_cb [ch%ld][size:%ld]\r\n", v_frm->ch, v_frm->size);
	// video_release_buf(v_frm->ch);

	/* Latch the first frame this channel delivers, readable through
	 * CMD_VIDEO_FCS_GET_INFO. Recorded before the queue check below so
	 * the value reflects when the channel actually started producing, even if
	 * this very first frame has to be dropped for lack of a free queue item. */
	if (!ctx->first_frame.valid) {
		ctx->first_frame.timestamp = v_frm->timestamp;
		ctx->first_frame.arrive_ms = mm_read_mediatime_ms();
		ctx->first_frame.valid = 1;
	}

	int is_output_ready = 0;
	mm_queue_item_t *output_item;
	is_output_ready = xQueueReceive(mctx->output_recycle, (void *)&output_item, 0);
	if (!is_output_ready) {
		video_release_buf(v_frm->ch);
		return;
	}
	output_item->data_addr = (uint32_t)v_frm->addr;
	output_item->size = v_frm->size;
	output_item->timestamp = v_frm->timestamp;
	output_item->type = v_frm->type;

	if (xQueueSend(mctx->output_ready, (void *)&output_item, 0) != pdTRUE) {
		video_release_buf(v_frm->ch);
		output_item->data_addr = (uint32_t)NULL;
		output_item->size = 0;
		output_item->flag = MMQI_FLAG_READY | MMQI_FLAG_DYNAMIC;
		xQueueSend(mctx->output_recycle, (void *)&output_item, 0);
	}
}

UNUSED_WARN_DIS
static int video_cmd_gen(char *cmd, int buf_len, video_params_t *v_param)
{
	int len = 0;
	int total_len = buf_len;
	char *p = cmd;

	/* convert to ISP output format */
	uint32_t isp_fmt = 0;
	if (v_param->format & (VIDEO_HEVC | VIDEO_H264 | VIDEO_JPEG | VIDEO_NV12)) {
		isp_fmt = 1;  // NV12
	} else if (v_param->format == VIDEO_NV16) {
		isp_fmt = 2;  // NV16
	} else if (v_param->format == VIDEO_YUY2) {
		isp_fmt = 3;  // YUY2
	} else if (v_param->format == VIDEO_BAYER_RAW) {
		isp_fmt = 4;  // RAW
	} else if (v_param->format == VIDEO_RGB) {
		isp_fmt = 5;  // RGB888
	}

	/* AINR is global, but any channel can be the first channel opened. */
	len = snprintf(p, total_len, "--ainr=%lu ", v_param->ainr_en);
	if (len < 0 || len >= total_len) {
		VIDEO_DBG_ERROR("error: fail to generate ISP command\r\n");
		goto error;
	}
	total_len -= len;
	p += len;

	len = snprintf(p, total_len,
				   "--isp%ld:w=%ld,h=%ld,fps=%ld,fmt=%ld ",
				   v_param->stream_id,
				   v_param->width,
				   v_param->height,
				   v_param->fps,
				   isp_fmt);
	if (len < 0 || len >= total_len) {
		VIDEO_DBG_ERROR("error: fail to generate ISP command\r\n");
		goto error;
	}
	total_len -= len;
	p += len;

	if (v_param->format & (VIDEO_HEVC | VIDEO_H264 | VIDEO_JPEG)) {
		len = snprintf(p, total_len,
					   "--enc%ld:w=%ld,h=%ld,fps=%ld,gop=%ld,bps=%ld,rcmode=%ld,minqp=%ld,maxqp=%ld,qlevel=%ld,fmt=%ld",
					   v_param->stream_id,
					   v_param->width,
					   v_param->height,
					   v_param->fps,
					   v_param->gop,
					   v_param->bps,
					   v_param->rc_mode,
					   v_param->min_qp,
					   v_param->max_qp,
					   v_param->qlevel,
					   v_param->format);
		if (len < 0 || len >= total_len) {
			VIDEO_DBG_ERROR("error: fail to generate ENC command\r\n");
			goto error;
		}
		total_len -= len;
		p += len;

#define APPEND_ENC_PARAM(_fmt, ...) \
		do { \
			len = snprintf(p, total_len, _fmt, ##__VA_ARGS__); \
			if (len < 0 || len >= total_len) { \
				VIDEO_DBG_ERROR("error: fail to append ENC command\r\n"); \
				goto error; \
			} \
			total_len -= len; \
			p += len; \
		} while (0)

		if (v_param->profile_set) {
			APPEND_ENC_PARAM(",profile=%ld", v_param->profile);
		}
		if (v_param->level_set) {
			APPEND_ENC_PARAM(",level=%ld", v_param->level);
		}
		if (v_param->tier_set) {
			APPEND_ENC_PARAM(",tier=%ld", v_param->tier);
		}
		if (v_param->rc_ext.flags & VPAP_ENC_RC_EXT_BPS) {
			APPEND_ENC_PARAM(",bps=%ld", v_param->rc_ext.bps);
		}
		if (v_param->rc_ext.flags & VPAP_ENC_RC_EXT_QP_HDR) {
			APPEND_ENC_PARAM(",qphdr=%ld", v_param->rc_ext.qpHdr);
		}
		if (v_param->rc_ext.flags & VPAP_ENC_RC_EXT_QP_RANGE) {
			APPEND_ENC_PARAM(",minqp=%ld,maxqp=%ld", v_param->rc_ext.minQp, v_param->rc_ext.maxQp);
		}
		if (v_param->rc_ext.flags & VPAP_ENC_RC_EXT_I_QP_RANGE) {
			APPEND_ENC_PARAM(",miniqp=%ld,maxiqp=%ld", v_param->rc_ext.minIQp, v_param->rc_ext.maxIQp);
		}
		if (v_param->rc_ext.flags & VPAP_ENC_RC_EXT_INTRA_QP_DELTA) {
			APPEND_ENC_PARAM(",intraqpdelta=%ld", v_param->rc_ext.intraQpDelta);
		}
		if (v_param->rc_ext.flags & VPAP_ENC_RC_EXT_FIXED_INTRA_QP) {
			APPEND_ENC_PARAM(",fixediqp=%ld", v_param->rc_ext.fixedIntraQp);
		}
		if (v_param->rc_ext.flags & VPAP_ENC_RC_EXT_PIC_RC) {
			APPEND_ENC_PARAM(",picrc=%ld", v_param->rc_ext.pictureRc);
		}
		if (v_param->rc_ext.flags & VPAP_ENC_RC_EXT_CTB_RC) {
			APPEND_ENC_PARAM(",ctbrc=%ld", v_param->rc_ext.ctbRc);
		}
		if (v_param->rc_ext.flags & VPAP_ENC_RC_EXT_BLOCK_RC_SIZE) {
			APPEND_ENC_PARAM(",blockrc=%ld", v_param->rc_ext.blockRCSize);
		}
		if (v_param->rc_ext.flags & VPAP_ENC_RC_EXT_PIC_SKIP) {
			APPEND_ENC_PARAM(",picskip=%ld", v_param->rc_ext.pictureSkip);
		}
		if (v_param->rc_ext.flags & VPAP_ENC_RC_EXT_HRD) {
			APPEND_ENC_PARAM(",hrd=%ld", v_param->rc_ext.hrd);
		}
		if (v_param->rc_ext.flags & VPAP_ENC_RC_EXT_CPB) {
			APPEND_ENC_PARAM(",cpb=%ld,cpbmax=%ld", v_param->rc_ext.hrdCpbSize, v_param->rc_ext.cpbMaxRate);
		}
		if (v_param->rc_ext.flags & VPAP_ENC_RC_EXT_BITRATE_WINDOW) {
			APPEND_ENC_PARAM(",bitratewin=%ld", v_param->rc_ext.bitrateWindow);
		}
		if (v_param->rc_ext.flags & VPAP_ENC_RC_EXT_MOVING_BITRATE) {
			APPEND_ENC_PARAM(",tolmoving=%ld,monitor=%ld", v_param->rc_ext.tolMovingBitRate, v_param->rc_ext.monitorFrames);
		}
		if (v_param->rc_ext.flags & VPAP_ENC_RC_EXT_FILLER_DATA) {
			APPEND_ENC_PARAM(",filler=%ld", v_param->rc_ext.fillerData);
		}
		if (v_param->rc_ext.flags & VPAP_ENC_RC_EXT_CRF) {
			APPEND_ENC_PARAM(",crf=%ld", v_param->rc_ext.crf);
		}
		if (v_param->rc_ext.flags & VPAP_ENC_RC_EXT_VBR) {
			APPEND_ENC_PARAM(",vbr=%ld", v_param->rc_ext.vbr);
		}
		if (v_param->rc_ext.flags & VPAP_ENC_RC_EXT_FRAME_RATE) {
			APPEND_ENC_PARAM(",rcfps=%ld:%ld", v_param->rc_ext.frameRateNum, v_param->rc_ext.frameRateDenom);
		}
		if (v_param->coding.flags & VPAP_ENC_CODING_SLICE_SIZE) {
			APPEND_ENC_PARAM(",slice=%ld", v_param->coding.sliceSize);
		}
		if (v_param->coding.flags & VPAP_ENC_CODING_SEI) {
			APPEND_ENC_PARAM(",sei=%ld", v_param->coding.seiMessages);
		}
		if (v_param->coding.flags & VPAP_ENC_CODING_SEI_USER_DATA) {
			APPEND_ENC_PARAM(",seiuser=%ld", v_param->coding.seiUserData);
		}
		if (v_param->coding.flags & VPAP_ENC_CODING_FULL_RANGE) {
			APPEND_ENC_PARAM(",fullrange=%ld", v_param->coding.vuiVideoFullRange);
		}
		if (v_param->coding.flags & VPAP_ENC_CODING_DEBLOCK) {
			APPEND_ENC_PARAM(",deblock=%ld,tcoffset=%ld,betaoffset=%ld",
							 v_param->coding.disableDeblockingFilter,
							 v_param->coding.tcOffset,
							 v_param->coding.betaOffset);
		}
		if (v_param->coding.flags & VPAP_ENC_CODING_SAO) {
			APPEND_ENC_PARAM(",sao=%ld", v_param->coding.enableSao);
		}
		if (v_param->coding.flags & VPAP_ENC_CODING_CABAC) {
			APPEND_ENC_PARAM(",cabac=%ld", v_param->coding.enableCabac);
		}
		if (v_param->coding.flags & VPAP_ENC_CODING_SAR) {
			APPEND_ENC_PARAM(",sarw=%ld,sarh=%ld", v_param->coding.sampleAspectRatioWidth, v_param->coding.sampleAspectRatioHeight);
		}
		if (v_param->coding.flags & VPAP_ENC_CODING_CIR) {
			APPEND_ENC_PARAM(",cir=%ld:%ld", v_param->coding.cirStart, v_param->coding.cirInterval);
		}
		if (v_param->coding.flags & VPAP_ENC_CODING_ROI) {
			for (int i = 0; i < VPAP_ENC_MAX_ROI; i++) {
				vpap_shared_enc_roi_t *roi = &v_param->coding.roi[i];
				APPEND_ENC_PARAM(",roi%d=%ld:%ld:%ld:%ld:%ld:%ld:%ld",
								 i, roi->enable, roi->top, roi->left,
								 roi->bottom, roi->right, roi->deltaQp, roi->qp);
			}
		}
		APPEND_ENC_PARAM(" ");
#undef APPEND_ENC_PARAM
	}
	VIDEO_DBG_DEBUG("[Video CMD]:%s\r\n", cmd);

	return 0;
error:
	cmd[0] = '\0';
	return -1;
}

static ALIGNMTO(CACHE_LINE_SIZE) char video_cmd[2048] = {0};

void video_module_stream_start(mm_context_t *m1, mm_context_t *m2, mm_context_t *m3, mm_context_t *m4, mm_context_t *m5)
{
	video_ctx_t *ctx_1 = m1 == NULL ? NULL : (video_ctx_t *)m1->priv;
	video_ctx_t *ctx_2 = m2 == NULL ? NULL : (video_ctx_t *)m2->priv;
	video_ctx_t *ctx_3 = m3 == NULL ? NULL : (video_ctx_t *)m3->priv;
	video_ctx_t *ctx_4 = m4 == NULL ? NULL : (video_ctx_t *)m4->priv;
	video_ctx_t *ctx_5 = m5 == NULL ? NULL : (video_ctx_t *)m5->priv;
	video_ctx_t *pp_ctx[5] = {ctx_1, ctx_2, ctx_3, ctx_4, ctx_5};

	video_ctx_t *first_ctx = NULL;
	uint32_t cmd_total_len = 0;
	for (int i = 0; i < 5; i++) {
		if (pp_ctx[i] != NULL) {
			if (!first_ctx) {
				first_ctx = pp_ctx[i];
			}
			cmd_total_len += strlen(pp_ctx[i]->init_cmd);
		}
	}
	if (cmd_total_len >= sizeof(video_cmd)) {
		VIDEO_DBG_ERROR("error: total command length > command buffer size\r\n");
		return;
	}

	memset(video_cmd, 0, sizeof(video_cmd));
	for (int i = 0; i < 5; i++) {
		if (pp_ctx[i] != NULL) {
			VIDEO_DBG_INFO("[CH%d CMD]:%s\r\n", i, pp_ctx[i]->init_cmd);
			strcat(video_cmd, pp_ctx[i]->init_cmd);
		}
	}
	/* Match sequential startup: the first selected channel owns global AINR.
	 * The parser uses the last --ainr value, so override later channel defaults. */
	if (first_ctx) {
		int len = snprintf(video_cmd + cmd_total_len, sizeof(video_cmd) - cmd_total_len,
						   " --ainr=%lu ", first_ctx->params.ainr_en);
		if (len < 0 || (size_t)len >= sizeof(video_cmd) - cmd_total_len) {
			VIDEO_DBG_ERROR("error: global AINR command exceeds command buffer\r\n");
			return;
		}
	}
	VIDEO_DBG_INFO("[Video CMD]:%s\r\n", video_cmd);
	video_open_all(video_cmd);
}

/* Return unconsumed VP2AP buffers before recycling the MMF queue items.
 * MM_CMD_CLEAR_QUEUE_ITEMS cannot be used here because video_del_item()
 * does not release the buffers owned by VP. */
static void video_flush_ready_queue(mm_context_t *mctx, int ch)
{
	mm_queue_item_t *item;
	int i;

	for (i = 0; i < mctx->queue_num; i++) {
		if (!mctx->port[i].output_ready || !mctx->port[i].output_recycle) {
			continue;
		}
		while (xQueueReceive(mctx->port[i].output_ready, &item, 0) == pdTRUE) {
			if (item->flag & MMQI_FLAG_DYNAMIC) {
				if (item->data_addr) {
					video_release_buf(ch);
					item->data_addr = (uint32_t)NULL;
					item->size = 0;
				}
				item->flag = MMQI_FLAG_READY | MMQI_FLAG_DYNAMIC;
			} else {
				item->flag = MMQI_FLAG_READY | MMQI_FLAG_STATIC;
			}
			xQueueSend(mctx->port[i].output_recycle, &item, 0);
		}
	}
}

int video_control(void *p, int cmd, int arg)
{
	video_ctx_t *ctx = (video_ctx_t *)p;
	mm_context_t *mctx = (mm_context_t *)ctx->parent;
	(void)mctx;
	int ch = ctx->params.stream_id;
	int ret = 0;
	switch (cmd) {
	case CMD_VIDEO_SET_PARAMS:
		memcpy(&ctx->params, (void *)arg, sizeof(video_params_t));
		break;
	case CMD_VIDEO_GET_PARAMS:
		memcpy((void *)arg, &ctx->params, sizeof(video_params_t));
		break;
	case CMD_VIDEO_GET_RCPARAM:
		video_get_rc_param(ch, (void *)arg);
		break;
	case CMD_VIDEO_SET_RCPARAM:
		video_set_rc_param(ch, (void *)arg);
		break;
	case CMD_VIDEO_GET_RC_EXT:
		video_get_rc_ext(ch, (vpap_shared_enc_rc_ext_t *)arg);
		break;
	case CMD_VIDEO_SET_RC_EXT:
		video_set_rc_ext(ch, (vpap_shared_enc_rc_ext_t *)arg);
		break;
	case CMD_VIDEO_GET_CODING:
		video_get_coding(ch, (vpap_shared_enc_coding_t *)arg);
		break;
	case CMD_VIDEO_SET_CODING:
		video_set_coding(ch, (vpap_shared_enc_coding_t *)arg);
		break;
	case CMD_VIDEO_BITRATE:
		video_set_bitrate(ch, arg);
		break;
	case CMD_VIDEO_RCMODE:
		video_set_rcmode(ch, arg);
		break;
	case CMD_VIDEO_QP_HDR:
		video_set_qp_hdr(ch, arg);
		break;
	case CMD_VIDEO_INTRA_QP_DELTA:
		video_set_intra_qp_delta(ch, arg);
		break;
	case CMD_VIDEO_FIXED_INTRA_QP:
		video_set_fixed_intra_qp(ch, arg);
		break;
	case CMD_VIDEO_SET_I_QP: {
		video_qp_range_t *qp = (video_qp_range_t *)arg;
		video_set_i_qp(ch, qp->min_qp, qp->max_qp);
		break;
	}
	case CMD_VIDEO_SET_QP: {
		video_qp_range_t *qp = (video_qp_range_t *)arg;
		video_set_qp(ch, qp->min_qp, qp->max_qp);
		break;
	}
	case CMD_VIDEO_MINQP:
		video_set_minqp(ch, arg);
		break;
	case CMD_VIDEO_MAXQP:
		video_set_maxqp(ch, arg);
		break;
	case CMD_VIDEO_PIC_RC:
		video_set_pic_rc(ch, arg);
		break;
	case CMD_VIDEO_CTB_RC:
		video_set_ctb_rc(ch, arg);
		break;
	case CMD_VIDEO_BLOCK_RC_SIZE:
		video_set_block_rc_size(ch, arg);
		break;
	case CMD_VIDEO_PICTURE_SKIP:
		video_set_picture_skip(ch, arg);
		break;
	case CMD_VIDEO_BITRATE_WINDOW:
		video_set_bitrate_window(ch, arg);
		break;
	case CMD_VIDEO_MOVING_BITRATE: {
		video_moving_bitrate_t *moving = (video_moving_bitrate_t *)arg;
		video_set_moving_bitrate(ch, moving->tolerance, moving->frames);
		break;
	}
	case CMD_VIDEO_CRF:
		video_set_crf(ch, arg);
		break;
	case CMD_VIDEO_VBR:
		video_set_vbr(ch, arg);
		break;
	case CMD_VIDEO_GOP:
		video_set_gop(ch, arg);
		break;
	case CMD_VIDEO_FPS:
		video_set_fps(ch, arg);
		break;
	case CMD_VIDEO_GET_SNAPSHOT:
		ret = video_get_snapshot(ch, (video_snapshot_t *)arg);
		if (ret < 0) {
			VIDEO_DBG_ERROR("error: video_get_snapshot fail\r\n");
		}
		break;
	case CMD_VIDEO_FORCE_IFRAME:
		video_force_iframe(ch);
		break;
	case CMD_VIDEO_AINR:
		video_ainr_enable(arg);
		break;
	case CMD_VIDEO_SLICE_SIZE:
		video_set_slice_size(ch, arg);
		break;
	case CMD_VIDEO_SEI_USER_DATA:
		video_set_sei_user_data(ch, arg);
		break;
	case CMD_VIDEO_DEBLOCK: {
		video_deblock_t *deblock = (video_deblock_t *)arg;
		video_set_deblock(ch, deblock->disable, deblock->tc_offset, deblock->beta_offset);
		break;
	}
	case CMD_VIDEO_CIR: {
		video_cir_t *cir = (video_cir_t *)arg;
		video_set_cir(ch, cir->start, cir->interval);
		break;
	}
	case CMD_VIDEO_ROI: {
		video_roi_t *roi = (video_roi_t *)arg;
		video_set_roi(ch, roi->idx, roi->enable, roi->top, roi->left, roi->bottom, roi->right, roi->delta_qp, roi->qp);
		break;
	}
	case CMD_VIDEO_SET_OSD_BITMAP: {
		vpap_shared_osd_t *osd = (vpap_shared_osd_t *)arg;
		return video_set_osd_bitmap(ch, osd->osdOverlayIdx, (video_osd_format_t)osd->osdOverlayFormat,
									osd->osdOverlayXoffset, osd->osdOverlayYoffset,
									osd->osdOverlayWidth, osd->osdOverlayHeight,
									osd->osdOverlayAlpha,
									osd->osdOverlayBitmapY, osd->osdOverlayBitmapU,
									osd->osdOverlayBitmapV, osd->osdOverlayPicBuf);
	}
	case CMD_VIDEO_DISABLE_OSD:
		return video_disable_osd(ch, arg);
	case CMD_VIDEO_SET_OUTPUT_GATE:
		if (!arg) {
			return -1;
		}
		rtos_critical_enter(RTOS_CRITICAL_DEFAULT);
		ctx->output_gate = *(video_output_gate_t *)arg;
		rtos_critical_exit(RTOS_CRITICAL_DEFAULT);
		return 0;
	case CMD_VIDEO_SET_MOSAIC: {
		mosaic_overlay_t *mosaic = (mosaic_overlay_t *)arg;
		video_set_mosaic(ch, mosaic->mosOverlayIdx, mosaic->mosOverlayXoffset, mosaic->mosOverlayYoffset,
						 mosaic->mosOverlayWidth, mosaic->mosOverlayHeight, mosaic->mosOverlaySizeIdx);
		break;
	}
	case CMD_VIDEO_DISABLE_MOSAIC:
		video_disable_mosaic(ch, arg);
		break;
	case CMD_VIDEO_STREAM_STOP:
		video_rx_cb_register(ch, NULL, NULL);
		video_flush_ready_queue(mctx, ch);
		/* Give v_output_task time to drain g_v.out_queue and send
		 * BUF_RELEASE to VP before VIDEO_CMD_CLOSE is issued.
		 * VP's async stop task blocks in vp_ipc_wait_frame_recycled()
		 * until all in-flight frames are released. */
		vTaskDelay(pdMS_TO_TICKS(50));
		video_flush_ready_queue(mctx, ch);
		video_close(ch);
		/* Recycle any frame queued while VP was completing its teardown. */
		video_flush_ready_queue(mctx, ch);
		break;
	case CMD_VIDEO_APPLY:
		ret = video_cmd_gen(ctx->init_cmd, sizeof(ctx->init_cmd), &ctx->params);
		if (ret < 0) {
			VIDEO_DBG_ERROR("error: video_cmd_gen fail\r\n");
			break;
		}
		memset(&ctx->first_frame, 0, sizeof(ctx->first_frame));
		video_rx_cb_register(ch, video_frame_complete_cb, ctx);
		video_open(ctx->init_cmd);
		break;
	case CMD_VIDEO_FCS_GET_INFO:
		if (arg) {
			memcpy((void *)arg, &ctx->first_frame, sizeof(ctx->first_frame));
		} else {
			ret = -1;
		}
		break;
#if defined(CONFIG_FAST_CAMERA_START) && (CONFIG_FAST_CAMERA_START != 0)
	case CMD_VIDEO_FCS_ATTACH:
		memset(&ctx->first_frame, 0, sizeof(ctx->first_frame));
		video_rx_cb_register(ch, video_frame_complete_cb, ctx);
		ret = video_fcs_attach(ch);
		if (ret < 0) {
			video_rx_cb_register(ch, NULL, NULL);
		}
		break;
#endif
	}
	return ret;
}

int video_handle(void *p, void *input, void *output)
{
	(void)p;
	(void)input;
	(void)output;
	return 0;
}

void *video_destroy(void *p)
{
	video_ctx_t *ctx = (video_ctx_t *)p;
	if (ctx) {
		free(ctx);
	}

	g_video.module_cnt--;
	if (g_video.module_cnt == 0) {
		video_rx_sw_deinit();
	}

	return NULL;
}

void *video_create(void *parent)
{
	video_ctx_t *ctx = malloc(sizeof(video_ctx_t));
	if (!ctx) {
		return NULL;
	}
	memset(ctx, 0, sizeof(video_ctx_t));

	ctx->parent = parent;

	g_video.module_cnt++;
	if (g_video.module_cnt == 1) {
		/* init video sw */
		video_rx_sw_init();
	}

	return ctx;
}

void *video_new_item(void *p)
{
	(void)p;
	return NULL;
}

void *video_del_item(void *p, void *d)
{
	(void)p;
	(void)d;
	return NULL;
}

void *video_vp_release_item(void *p, void *d, int length)
{
	(void)length;
	video_ctx_t *ctx = (video_ctx_t *)p;
	mm_queue_item_t *free_item = (mm_queue_item_t *)d;
	(void)free_item;
	int ch = ctx->params.stream_id;

	VIDEO_DBG_DEBUG("%s ch:%d\r\n", __func__, ch);
	video_release_buf(ch);

	return NULL;
}

mm_module_t video_module = {
	.create = video_create,
	.destroy = video_destroy,
	.control = video_control,
	.handle = video_handle,

	.new_item = video_new_item,
	.del_item = video_del_item,
	.rsz_item = NULL,
	.vrelease_item = video_vp_release_item,

	.output_type = MM_TYPE_VDSP,    // output for video algorithm
	.module_type = MM_TYPE_VSRC,    // module type is video source
	.name = "VIDEO"
};
