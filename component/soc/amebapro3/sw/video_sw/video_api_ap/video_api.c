/******************************************************************************
*
* Copyright(c) 2007 - 2025 Realtek Corporation. All rights reserved.
*
******************************************************************************/
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <FreeRTOS.h>
#include <task.h>
#include <queue.h>
#include <semphr.h>

#include "video_api.h"
#include "os_wrapper.h"
#include "os_wrapper_critical.h"
#include "ameba_soc.h"
#include "ameba_ipc.h"
#include "ameba_ipc_api.h"
#include "ipc_video.h"
#include "video_common_type.h"
#include "avcodec.h"
#include "hal_video.h"

static const char *TAG = "AP_VIDEO";

typedef struct video_ch_info_s {
	void (*cb_func)(void *v, void *arg);
	void *cb_arg;
} video_ch_info_t;

typedef struct v_global_s {
	video_ch_info_t v_info[VIDEO_ISP_VALID_CH];
	rtos_task_t vout_task_handle;
	rtos_queue_t out_queue;
} v_global_t;

static v_global_t g_v;
static rtos_mutex_t video_callback_lock;

/* Each result and its read lease outlive the calling task's request. */
static vp_snapshot_result_t snapshot_reply[VIDEO_ENC_VALID_CH];
static uint8_t snapshot_busy[VIDEO_ENC_VALID_CH];
static uint32_t snapshot_id[VIDEO_ENC_VALID_CH];

int video_get_snapshot(int ch, video_snapshot_t *snapshot)
{
	if (!snapshot) {
		return VIDEO_SNAPSHOT_ERROR;
	}
	snapshot->jpeg_len = 0;
	if ((unsigned)ch >= VIDEO_ENC_VALID_CH) {
		return VIDEO_SNAPSHOT_INACTIVE;
	}
	if (!snapshot->dst || !snapshot->capacity) {
		return VIDEO_SNAPSHOT_ERROR;
	}
	taskENTER_CRITICAL();
	if (snapshot_busy[ch]) {
		taskEXIT_CRITICAL();
		return VIDEO_SNAPSHOT_BUSY;
	}
	snapshot_busy[ch] = 1;
	uint32_t id = ++snapshot_id[ch];
	if (!id) {
		id = ++snapshot_id[ch];
	}
	taskEXIT_CRITICAL();

	vp_snapshot_result_t *reply = &snapshot_reply[ch];
	DCache_Invalidate((uint32_t)reply, CACHE_LINE_SIZE);
	reply->active_id = id;
	DCache_Clean((uint32_t)&reply->active_id, CACHE_LINE_SIZE);
	__DSB();
	int ret = VIDEO_SnapshotStart(ch, reply, id);
	if (!ret) {
		TickType_t start = xTaskGetTickCount();
		do {
			DCache_Invalidate((uint32_t)reply, CACHE_LINE_SIZE);
			ret = reply->id == id ? reply->size : 0;
			if (ret != 0) {
				break;
			}
			vTaskDelay(1);
		} while (xTaskGetTickCount() - start < pdMS_TO_TICKS(2000));

		if (ret > 0) {
			uint32_t len = (uint32_t)ret;
			if (len > snapshot->capacity) {
				snapshot->jpeg_len = len;
				ret = VIDEO_SNAPSHOT_NOSPACE;
			} else {
				DCache_Invalidate((uint32_t)reply->jpeg, len);
				memcpy(snapshot->dst, reply->jpeg, len);
				snapshot->jpeg_len = len;
				ret = 0;
			}
		} else if (!ret) {
			ret = VIDEO_SNAPSHOT_TIMEOUT;
		}
	}
	/* Finish all JPEG reads before allowing VP to reuse or free the buffer. */
	__DMB();
	reply->active_id = 0;
	DCache_Clean((uint32_t)&reply->active_id, CACHE_LINE_SIZE);
	__DSB();
	taskENTER_CRITICAL();
	snapshot_busy[ch] = 0;
	taskEXIT_CRITICAL();
	return ret;
}

void video_switch_raw(int format)
{
	VIDEO_ISPSwitchRAW(format);
}

void video_set_sensor_mode(int mode, int fps)
{
	VIDEO_SetSensorMode(mode, fps);
}

void video_set_isp_ctrl(uint32_t id, int *value)
{
	DCache_CleanInvalidate((u32)value, sizeof(int));
	VIDEO_SetISPCtrl(id, value);
}

void video_get_isp_ctrl(uint32_t id, int *value)
{
	memset(value, 0, sizeof(int));
	DCache_CleanInvalidate((u32)value, sizeof(int));
	VIDEO_GetISPCtrl(id, value);
}

void video_set_i2c(int addr, int value)
{
	VIDEO_SetI2C(addr, value);
}

void video_get_i2c(int addr, int *value)
{
	memset(value, 0, sizeof(int));
	DCache_CleanInvalidate((u32)value, sizeof(int));
	VIDEO_GetI2C(addr, value);
}

void video_get_sensor_info(uint32_t *snr_info)
{
	VIDEO_GetISPSensorInfo(snr_info);
}

void video_set_sensor_iq(uint32_t sensor_idx, uint32_t iq_idx)
{
	VIDEO_SetSensorIQ(sensor_idx, iq_idx);
}

void video_open_all(char *cmd)
{
	VIDEO_OpenAll(cmd);
}

void video_open(char *cmd)
{
	VIDEO_Open(cmd);
}

void video_close(int ch)
{
	rtos_mutex_take(video_callback_lock, portMAX_DELAY);
	g_v.v_info[ch].cb_func = NULL;
	g_v.v_info[ch].cb_arg = NULL;
	rtos_mutex_give(video_callback_lock);
	VIDEO_Close(ch);
}

void video_set_fps(int ch, int fps)
{
	VIDEO_SetFPS(ch, fps);
}

void video_release_buf(int ch)
{
	VIDEO_ReleaseBuf(ch);
}

void video_get_rc_param(int ch, void *p)
{
	rc_param_t *rc = (rc_param_t *)p;
	VIDEO_GetRCParam(ch, rc);
}

void video_set_rc_param(int ch, void *p)
{
	rc_param_t *rc = (rc_param_t *)p;
	VIDEO_SetRCParam(ch, rc);
}

void video_get_rc_ext(int ch, vpap_shared_enc_rc_ext_t *rc)
{
	VIDEO_GetRCExt(ch, rc);
}

void video_set_rc_ext(int ch, vpap_shared_enc_rc_ext_t *rc)
{
	VIDEO_SetRCExt(ch, rc);
}

void video_set_bitrate(int ch, uint32_t bps)
{
	ALIGNMTO(CACHE_LINE_SIZE) rc_param_t rc = {0};
	VIDEO_GetRCParam(ch, &rc);
	RTK_LOGI(TAG, "original bps:%u, new bps:%u\r\n", rc.bps, bps);
	rc.bps = bps;
	VIDEO_SetRCParam(ch, &rc);
}

void video_set_rcmode(int ch, uint32_t rc_mode)
{
	ALIGNMTO(CACHE_LINE_SIZE) enc_rc_ext_t rc = {0};
	rc.flags = VPAP_ENC_RC_EXT_RCMODE;
	rc.rcMode = rc_mode;
	VIDEO_SetRCExt(ch, &rc);
}

void video_set_qp_hdr(int ch, int32_t qp_hdr)
{
	ALIGNMTO(CACHE_LINE_SIZE) enc_rc_ext_t rc = {0};
	rc.flags = VPAP_ENC_RC_EXT_QP_HDR;
	rc.qpHdr = qp_hdr;
	VIDEO_SetRCExt(ch, &rc);
}

void video_set_intra_qp_delta(int ch, int32_t intra_qp_delta)
{
	ALIGNMTO(CACHE_LINE_SIZE) enc_rc_ext_t rc = {0};
	rc.flags = VPAP_ENC_RC_EXT_INTRA_QP_DELTA;
	rc.intraQpDelta = intra_qp_delta;
	VIDEO_SetRCExt(ch, &rc);
}

void video_set_fixed_intra_qp(int ch, uint32_t fixed_intra_qp)
{
	ALIGNMTO(CACHE_LINE_SIZE) enc_rc_ext_t rc = {0};
	rc.flags = VPAP_ENC_RC_EXT_FIXED_INTRA_QP;
	rc.fixedIntraQp = fixed_intra_qp;
	VIDEO_SetRCExt(ch, &rc);
}

void video_set_i_qp(int ch, uint32_t min_i_qp, uint32_t max_i_qp)
{
	ALIGNMTO(CACHE_LINE_SIZE) enc_rc_ext_t rc = {0};
	rc.flags = VPAP_ENC_RC_EXT_I_QP_RANGE;
	rc.minIQp = min_i_qp;
	rc.maxIQp = max_i_qp;
	VIDEO_SetRCExt(ch, &rc);
}

void video_set_minqp(int ch, uint32_t min_qp)
{
	ALIGNMTO(CACHE_LINE_SIZE) rc_param_t rc = {0};
	VIDEO_GetRCParam(ch, &rc);
	rc.minQp = min_qp;
	VIDEO_SetRCParam(ch, &rc);
}

void video_set_maxqp(int ch, uint32_t max_qp)
{
	ALIGNMTO(CACHE_LINE_SIZE) rc_param_t rc = {0};
	VIDEO_GetRCParam(ch, &rc);
	rc.maxQp = max_qp;
	VIDEO_SetRCParam(ch, &rc);
}

void video_set_qp(int ch, uint32_t min_qp, uint32_t max_qp)
{
	ALIGNMTO(CACHE_LINE_SIZE) rc_param_t rc = {0};
	VIDEO_GetRCParam(ch, &rc);
	rc.minQp = min_qp;
	rc.maxQp = max_qp;
	VIDEO_SetRCParam(ch, &rc);
}

void video_set_pic_rc(int ch, uint32_t enable)
{
	ALIGNMTO(CACHE_LINE_SIZE) enc_rc_ext_t rc = {0};
	rc.flags = VPAP_ENC_RC_EXT_PIC_RC;
	rc.pictureRc = enable;
	VIDEO_SetRCExt(ch, &rc);
}

void video_set_ctb_rc(int ch, uint32_t mode)
{
	ALIGNMTO(CACHE_LINE_SIZE) enc_rc_ext_t rc = {0};
	rc.flags = VPAP_ENC_RC_EXT_CTB_RC;
	rc.ctbRc = mode;
	VIDEO_SetRCExt(ch, &rc);
}

void video_set_block_rc_size(int ch, uint32_t size)
{
	ALIGNMTO(CACHE_LINE_SIZE) enc_rc_ext_t rc = {0};
	rc.flags = VPAP_ENC_RC_EXT_BLOCK_RC_SIZE;
	rc.blockRCSize = size;
	VIDEO_SetRCExt(ch, &rc);
}

void video_set_picture_skip(int ch, uint32_t enable)
{
	ALIGNMTO(CACHE_LINE_SIZE) enc_rc_ext_t rc = {0};
	rc.flags = VPAP_ENC_RC_EXT_PIC_SKIP;
	rc.pictureSkip = enable;
	VIDEO_SetRCExt(ch, &rc);
}

void video_set_bitrate_window(int ch, uint32_t frames)
{
	ALIGNMTO(CACHE_LINE_SIZE) enc_rc_ext_t rc = {0};
	rc.flags = VPAP_ENC_RC_EXT_BITRATE_WINDOW;
	rc.bitrateWindow = frames;
	VIDEO_SetRCExt(ch, &rc);
}

void video_set_moving_bitrate(int ch, int32_t tolerance, int32_t frames)
{
	ALIGNMTO(CACHE_LINE_SIZE) enc_rc_ext_t rc = {0};
	rc.flags = VPAP_ENC_RC_EXT_MOVING_BITRATE;
	rc.tolMovingBitRate = tolerance;
	rc.monitorFrames = frames;
	VIDEO_SetRCExt(ch, &rc);
}

void video_set_crf(int ch, int32_t crf)
{
	ALIGNMTO(CACHE_LINE_SIZE) enc_rc_ext_t rc = {0};
	rc.flags = VPAP_ENC_RC_EXT_CRF;
	rc.crf = crf;
	VIDEO_SetRCExt(ch, &rc);
}

void video_set_vbr(int ch, uint32_t enable)
{
	ALIGNMTO(CACHE_LINE_SIZE) enc_rc_ext_t rc = {0};
	rc.flags = VPAP_ENC_RC_EXT_VBR;
	rc.vbr = enable;
	VIDEO_SetRCExt(ch, &rc);
}

void video_set_gop(int ch, int gop)
{
	VIDEO_SetGOP(ch, gop);
}

void video_force_iframe(int ch)
{
	VIDEO_ForceI(ch);
}

void video_get_coding(int ch, vpap_shared_enc_coding_t *coding)
{
	VIDEO_GetCoding(ch, coding);
}

void video_set_coding(int ch, vpap_shared_enc_coding_t *coding)
{
	VIDEO_SetCoding(ch, coding);
}

void video_set_slice_size(int ch, uint32_t slice_size)
{
	ALIGNMTO(CACHE_LINE_SIZE) enc_coding_t coding = {0};
	coding.flags = VPAP_ENC_CODING_SLICE_SIZE;
	coding.sliceSize = slice_size;
	VIDEO_SetCoding(ch, &coding);
}

void video_set_sei_user_data(int ch, uint32_t enable)
{
	ALIGNMTO(CACHE_LINE_SIZE) enc_coding_t coding = {0};
	coding.flags = VPAP_ENC_CODING_SEI_USER_DATA;
	coding.seiUserData = enable;
	VIDEO_SetCoding(ch, &coding);
}

void video_set_deblock(int ch, uint32_t disable, int32_t tc_offset, int32_t beta_offset)
{
	ALIGNMTO(CACHE_LINE_SIZE) enc_coding_t coding = {0};
	coding.flags = VPAP_ENC_CODING_DEBLOCK;
	coding.disableDeblockingFilter = disable;
	coding.tcOffset = tc_offset;
	coding.betaOffset = beta_offset;
	VIDEO_SetCoding(ch, &coding);
}

void video_set_cir(int ch, uint32_t start, uint32_t interval)
{
	ALIGNMTO(CACHE_LINE_SIZE) enc_coding_t coding = {0};
	coding.flags = VPAP_ENC_CODING_CIR;
	coding.cirStart = start;
	coding.cirInterval = interval;
	VIDEO_SetCoding(ch, &coding);
}

void video_set_roi(int ch, int idx, uint32_t enable, uint32_t top, uint32_t left, uint32_t bottom, uint32_t right, int32_t delta_qp, int32_t qp)
{
	if (idx < 0 || idx >= VPAP_ENC_MAX_ROI) {
		return;
	}

	ALIGNMTO(CACHE_LINE_SIZE) enc_coding_t coding = {0};
	VIDEO_GetCoding(ch, &coding);
	coding.flags = VPAP_ENC_CODING_ROI;
	coding.roi[idx].enable = enable;
	coding.roi[idx].top = top;
	coding.roi[idx].left = left;
	coding.roi[idx].bottom = bottom;
	coding.roi[idx].right = right;
	coding.roi[idx].deltaQp = delta_qp;
	coding.roi[idx].qp = qp;
	VIDEO_SetCoding(ch, &coding);
}

int video_set_osd_bitmap(int ch, int idx, video_osd_format_t fmt, uint32_t x_off, uint32_t y_off, uint32_t w, uint32_t h, uint32_t alpha, uint32_t y,
						 uint32_t u, uint32_t v, uint8_t *bitmap)
{
	ALIGNMTO(CACHE_LINE_SIZE) osd_overlay_t osd = {0};
	osd.osdOverlayIdx = idx;
	osd.osdOverlayEnable = 1;
	osd.osdOverlayFormat = fmt;
	osd.osdOverlayXoffset = x_off;
	osd.osdOverlayYoffset = y_off;
	osd.osdOverlayWidth = w;
	osd.osdOverlayHeight = h;
	osd.osdOverlayAlpha = alpha;
	osd.osdOverlayBitmapY = y;
	osd.osdOverlayBitmapU = u;
	osd.osdOverlayBitmapV = v;
	osd.osdOverlayPicBuf = bitmap;
	return VIDEO_SetOSD(ch, &osd);
}

int video_disable_osd(int ch, int idx)
{
	ALIGNMTO(CACHE_LINE_SIZE) osd_overlay_t osd = {0};
	osd.osdOverlayIdx = idx;
	osd.osdOverlayEnable = 0;
	return VIDEO_SetOSD(ch, &osd);
}

void video_set_mosaic(int ch, int idx, uint32_t x_off, uint32_t y_off, uint32_t w, uint32_t h, uint32_t size)
{
	ALIGNMTO(CACHE_LINE_SIZE) mosaic_overlay_t mosaic = {0};
	mosaic.mosOverlayIdx = idx;
	mosaic.mosOverlayEnable = 1;
	mosaic.mosOverlayXoffset = x_off;
	mosaic.mosOverlayYoffset = y_off;
	mosaic.mosOverlayWidth = w;
	mosaic.mosOverlayHeight = h;
	mosaic.mosOverlaySizeIdx = size;
	VIDEO_SetMosaic(ch, &mosaic);
}

void video_disable_mosaic(int ch, int idx)
{
	ALIGNMTO(CACHE_LINE_SIZE) mosaic_overlay_t mosaic = {0};
	mosaic.mosOverlayIdx = idx;
	mosaic.mosOverlayEnable = 0;
	VIDEO_SetMosaic(ch, &mosaic);
}

static uint16_t video_overlay_used[VIDEO_ISP_VALID_CH];
static rtos_mutex_t video_overlay_lock;

int video_alloc_overlay_index(int ch)
{
	int ret = -1;

	if (ch < 0 || ch >= VIDEO_ISP_VALID_CH) {
		return -1;
	}
	if (video_overlay_lock == NULL) {
		rtos_critical_enter(RTOS_CRITICAL_DEFAULT);
		if (video_overlay_lock == NULL) {
			rtos_mutex_create(&video_overlay_lock);
		}
		rtos_critical_exit(RTOS_CRITICAL_DEFAULT);
		if (video_overlay_lock == NULL) {
			return -1;
		}
	}
	rtos_mutex_take(video_overlay_lock, portMAX_DELAY);
	for (int i = 0; i < VIDEO_OVERLAY_INDEX_COUNT; i++) {
		uint16_t mask = (uint16_t)(1u << i);
		if ((video_overlay_used[ch] & mask) == 0) {
			video_overlay_used[ch] |= mask;
			ret = i;
			break;
		}
	}
	rtos_mutex_give(video_overlay_lock);
	return ret;
}

void video_free_overlay_index(int ch, int idx)
{
	if (ch < 0 || ch >= VIDEO_ISP_VALID_CH || idx < 0 || idx >= VIDEO_OVERLAY_INDEX_COUNT) {
		return;
	}
	if (video_overlay_lock == NULL) {
		return;
	}
	rtos_mutex_take(video_overlay_lock, portMAX_DELAY);
	video_overlay_used[ch] &= (uint16_t)~(1u << idx);
	rtos_mutex_give(video_overlay_lock);
}

void video_ainr_enable(int en)
{
	VIDEO_AinrEnable(en);
}

int video_fcs_attach(int ch)
{
	return VIDEO_FcsAttach(ch);
}

int video_fcs_get_boot_state(void)
{
	return VIDEO_FcsGetBootState();
}

void video_rx_cb_register(int ch, void (*cb_func)(void *v, void *arg), void *cb_arg)
{
	if (video_callback_lock == NULL) {
		g_v.v_info[ch].cb_func = cb_func;
		g_v.v_info[ch].cb_arg = cb_arg;
		return;
	}
	rtos_mutex_take(video_callback_lock, portMAX_DELAY);
	g_v.v_info[ch].cb_func = cb_func;
	g_v.v_info[ch].cb_arg = cb_arg;
	rtos_mutex_give(video_callback_lock);
}

#define VIDEO_FPS_PRINT_INTERVAL_SEC	5

static void video_show_fps(int ch)
{
	static uint32_t current_fps[VIDEO_ISP_VALID_CH] = {0};
	static uint32_t ts = 0;

	if (ts == 0) {
		ts = xTaskGetTickCount();
	}
	current_fps[ch]++;
	if ((xTaskGetTickCount() - ts) > VIDEO_FPS_PRINT_INTERVAL_SEC * 1000) {
		/* round to nearest instead of truncating */
		uint32_t half = VIDEO_FPS_PRINT_INTERVAL_SEC / 2;

		RTK_LOGI(TAG, "FPS=%u, %u, %u, %u, %u\r\n"
				 , (current_fps[0] + half) / VIDEO_FPS_PRINT_INTERVAL_SEC
				 , (current_fps[1] + half) / VIDEO_FPS_PRINT_INTERVAL_SEC
				 , (current_fps[2] + half) / VIDEO_FPS_PRINT_INTERVAL_SEC
				 , (current_fps[3] + half) / VIDEO_FPS_PRINT_INTERVAL_SEC
				 , (current_fps[4] + half) / VIDEO_FPS_PRINT_INTERVAL_SEC);
		memset(current_fps, 0, sizeof(current_fps));
		ts = xTaskGetTickCount();
	}
}

static void v_output_task(void *p)
{
	(void)p;
	/* calculate each channel's frame interval */
	while (1) {
		if (g_v.out_queue) {
			v_out_t v_frm;
			if (rtos_queue_receive(g_v.out_queue, &v_frm, 1000) != SUCCESS) {
				RTK_LOGW(TAG, "no frame in queue\r\n");
				continue;
			}
			int ch = v_frm.ch;

			video_show_fps(ch);

			rtos_mutex_take(video_callback_lock, portMAX_DELAY);
			if (g_v.v_info[ch].cb_func) {
				g_v.v_info[ch].cb_func(&v_frm, g_v.v_info[ch].cb_arg);
				rtos_mutex_give(video_callback_lock);
			} else {
				rtos_mutex_give(video_callback_lock);
				/* channel closed: no callback, but must still release to unblock VP */
				video_release_buf(ch);
			}
		} else {
			vTaskDelay(50);
		}
	}
}

void video_send_to_ipc_rx_queue(vp_frame_info_t *frm_info)
{
	v_out_t v_frm = {
		.ch = frm_info->ch,
		.type = frm_info->codec,
		.timestamp = frm_info->time_stamp,
		.addr = frm_info->enc_addr,
		.size = frm_info->enc_len
	};

	if (rtos_queue_send(g_v.out_queue, &v_frm, 0) != SUCCESS) {
		RTK_LOGW(TAG, "out_queue full, frame dropped\r\n");
		/* skip and release */
		// isp_buf_release(stream_id, (void *)NULL, buf_id);
	}
}

void video_rx_sw_deinit(void)
{
	if (g_v.vout_task_handle) {
		rtos_task_delete(g_v.vout_task_handle);
		g_v.vout_task_handle = NULL;
	}

	if (g_v.out_queue) {
		rtos_queue_delete(g_v.out_queue);
		g_v.out_queue = NULL;
	}

	if (video_callback_lock) {
		rtos_mutex_delete(video_callback_lock);
		video_callback_lock = NULL;
	}

	if (video_overlay_lock) {
		rtos_mutex_take(video_overlay_lock, portMAX_DELAY);
		memset(video_overlay_used, 0, sizeof(video_overlay_used));
		rtos_mutex_give(video_overlay_lock);
		rtos_mutex_delete(video_overlay_lock);
		video_overlay_lock = NULL;
	}
}

void video_rx_sw_init(void)
{
	if (video_callback_lock == NULL) {
		rtos_mutex_create(&video_callback_lock);
		if (video_callback_lock == NULL) {
			RTK_LOGE(TAG, "fail to create video callback lock\r\n");
			goto error;
		}
	}
	if (g_v.out_queue == NULL) {
		if (rtos_queue_create(&g_v.out_queue, 60, sizeof(v_out_t)) != SUCCESS) {
			RTK_LOGE(TAG, "fail to create isp out queue, memory not enough\r\n");
			goto error;
		}
	}
	RTK_LOGI(TAG, "out_queue create success\r\n");

	if (g_v.vout_task_handle == NULL) {
		if (rtos_task_create(&g_v.vout_task_handle, "v_output_task", v_output_task, (void *)NULL, 1024, 9) != RTK_SUCCESS) {
			RTK_LOGE(TAG, "fail to create video output task, memory not enough\r\n");
			goto error;
		}
	}
	RTK_LOGI(TAG, "v_output_task create success\r\n");

	return;

error:
	video_rx_sw_deinit();
	return;
}

