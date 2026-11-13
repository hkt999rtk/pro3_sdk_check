/******************************************************************************
*
* Copyright(c) 2007 - 2025 Realtek Corporation. All rights reserved.
*
******************************************************************************/
#include <stdlib.h>
#include "ameba_soc.h"
#include "ameba_ipc.h"
#include "ameba_ipc_api.h"
#include "mmf2_module.h"
#include "mmf2_link.h"
#include "mmf2_siso.h"
#include "mmf2_dbg.h"
#include "module_encoder.h"
#include "module_isp.h"
#if defined(CONFIG_4K_DEV) && (CONFIG_4K_DEV == 1)
#include "module_isp_4k.h"
#endif
#include "module_vp2ap.h"
#include "module_array.h"
#include "module_vipnn.h"
#include "avcodec.h"
#include "img_process.h"
#include "ipc_video.h"

#include "video_app_main.h"
#include "video_app_param_config.h"
#include "video_app_rcc.h"
#include "model_yolo.h"

static const char *TAG = "VP_APP";

/* NN Option — defined early so guards on variable declarations below work */
#define NN_ENA       0  /* AI NN model detection on V5 RGB channel */
#define NN_OSD_ENA   1  /* AI NN detection result display on streaming channel */

static mm_context_t *isp_v1_ctx         = NULL;
static mm_context_t *enc_v1_ctx         = NULL;
static mm_context_t *vp2ap_v1_ctx       = NULL;
static mm_siso_t *siso_isp_enc_v1       = NULL;
static mm_siso_t *siso_vp2ap_v1         = NULL;

static mm_context_t *isp_v2_ctx         = NULL;
static mm_context_t *enc_v2_ctx         = NULL;
static mm_context_t *vp2ap_v2_ctx       = NULL;
static mm_siso_t *siso_isp_enc_v2       = NULL;
static mm_siso_t *siso_vp2ap_v2         = NULL;

static mm_context_t *isp_v3_ctx         = NULL;
static mm_context_t *enc_v3_ctx         = NULL;
static mm_context_t *vp2ap_v3_ctx       = NULL;
static mm_siso_t *siso_isp_enc_v3       = NULL;
static mm_siso_t *siso_vp2ap_v3         = NULL;

static mm_context_t *isp_v4_ctx         = NULL;
static mm_context_t *vp2ap_v4_ctx       = NULL;
static mm_siso_t *siso_isp_vp2ap_v4     = NULL;

static mm_context_t *isp_v5_ctx         = NULL;
#if !NN_ENA
static mm_context_t *vp2ap_v5_ctx       = NULL;
#endif
static mm_siso_t *siso_isp_vipnn        = NULL;

static isp_params_t isp_params[VIDEO_ISP_VALID_CH] = {0};
static enc_parameter_t enc_params[VIDEO_ENC_VALID_CH] = {0};
static isp_enc_ch_en_t v_ch_en = {{0}, {0}};

static video_ch_state_t video_ch_state[VIDEO_ISP_VALID_CH] = {VIDEO_CH_CLOSED};

#if defined(CONFIG_FAST_CAMERA_START) && (CONFIG_FAST_CAMERA_START != 0)
/* Latch an attach received while the asynchronous FCS channel startup is still
 * in progress; video_vp2ap_link_ready() consumes it once the SISO is ready. */
static uint8_t fcs_attach_pending[VIDEO_ISP_VALID_CH] = {0};
static int video_fcs_boot_state = VIDEO_FCS_BOOT_NORMAL;
#endif

rtos_mutex_t video_op_lock = NULL;

static int enc_v1_prio = 5;
static int enc_v2_prio = 4;
static int enc_v3_prio = 3;
static int video_rcc_inited = 0;

static int video_vp2ap_link_ready(mm_siso_t *siso, int ch)
{
#if defined(CONFIG_FAST_CAMERA_START) && (CONFIG_FAST_CAMERA_START != 0)
	/* FCS starts the upstream pipeline early, but defers the final VP-to-AP
	 * SISO until AP has installed its receive path and attaches the channel. */
	if (video_ch_state[ch] == VIDEO_CH_FCS_STARTING) {
		if (!fcs_attach_pending[ch]) {
			video_ch_state[ch] = VIDEO_CH_FCS_READY;
			return 0;
		}
		fcs_attach_pending[ch] = 0;
	}
#endif
	if (siso_start(siso) < 0) {
		video_ch_state[ch] = VIDEO_CH_ERROR;
		return -1;
	}
	video_ch_state[ch] = VIDEO_CH_RUNNING;
	return 0;
}

/* model selection: yolov4_tiny, yolov7_tiny, nanodet_plus_m
 * please make sure the choosed model is also selected in amebapro2_fwfs_nn_models.json */
#define NN_MODEL_OBJ    yolov4_tiny
/* RGB video resolution
 * please make sure the resolution is matched to model input size and thereby to avoid SW image resizing */
#define NN_WIDTH	416
#define NN_HEIGHT	416

/* Crop ScaleUp Option (V1) */
#define V1_CROP_SCALEUP_ENA     0

static vp2ap_params_t vp2ap_v1_params = {
	.stream_id = V1_STREAM_ID
};
static vp2ap_params_t vp2ap_v2_params = {
	.stream_id = V2_STREAM_ID
};
#if !defined(CONFIG_4K_DEV) || (CONFIG_4K_DEV != 1)
static vp2ap_params_t vp2ap_v3_params = {
	.stream_id = V3_STREAM_ID
};
static vp2ap_params_t vp2ap_v4_params = {
	.stream_id = V4_STREAM_ID
};

#endif
#if !NN_ENA
static vp2ap_params_t vp2ap_v5_params = {
	.stream_id = V5_STREAM_ID
};
#endif

#if NN_ENA
static mm_context_t *vipnn_ctx  = NULL;
static nn_data_param_t nn_input_params = {
	.img = {
		.width = NN_WIDTH,
		.height = NN_HEIGHT,
	},
	.codec_type = AV_CODEC_ID_RGB888
};
#endif

#if !defined(CONFIG_4K_DEV) || (CONFIG_4K_DEV != 1)
#define SLICE_WRAP_NUM  8

static isp_remap_param_t isp_remap_v1_param = {
	.feature_bit = REMAP_COMPRESS,
	.enc_type = ISP_REMAP_ENC_HEVC,  //0: H264, 1: H265
	.wrap_num = SLICE_WRAP_NUM,
	.burst_id = 0,  //should be 0 (64-aligned) if RTT enabled, 2 (256-aligned) if compression enabled
	.fixp_hts_ratio_2 = 4,
	.encDec400LumTableBase = {0},
	.encDec400ChrTableBase = {0},
};

static enc_remap_param_t enc_remap_v1_param = {
	.slice_wrap_num = SLICE_WRAP_NUM,
};

#endif

/* The JPEG encoder reuses its output only on another snapshot. */
static struct {
	uint32_t id;
	enum { SNAPSHOT_OFF, SNAPSHOT_IDLE, SNAPSHOT_ENCODING, SNAPSHOT_HELD } state;
	vp_snapshot_result_t *reply;
} snapshot_slot[VIDEO_ENC_VALID_CH];

/* Called with the VP critical section held to serialize stop and completion. */
static void snapshot_publish(vp_snapshot_result_t *reply, void *jpeg, int32_t size)
{
	reply->jpeg = jpeg;
	reply->size = size;
	DCache_Clean((uint32_t)reply, CACHE_LINE_SIZE);
}

/* Called with video_op_lock held, keeping reply and id stable.
 * AP owns this separate cache line and clears it even on an IPC timeout. */
static int snapshot_in_use(int ch)
{
	vp_snapshot_result_t *reply = snapshot_slot[ch].reply;
	DCache_Invalidate((uint32_t)&reply->active_id, CACHE_LINE_SIZE);
	taskENTER_CRITICAL();
	int in_use = reply->active_id == snapshot_slot[ch].id;
	taskEXIT_CRITICAL();
	return in_use;
}

static void snapshot_done(int ch, void *jpeg, uint32_t len)
{
	taskENTER_CRITICAL();
	if (snapshot_slot[ch].state == SNAPSHOT_ENCODING) {
		snapshot_slot[ch].state = len ? SNAPSHOT_HELD : SNAPSHOT_IDLE;
		snapshot_publish(snapshot_slot[ch].reply, jpeg, len ? (int32_t)len : VIDEO_SNAPSHOT_ERROR);
	}
	taskEXIT_CRITICAL();
}

static void v1_snapshot_cb(void *jpeg, uint32_t len)
{
	snapshot_done(V1_STREAM_ID, jpeg, len);
}

static void v2_snapshot_cb(void *jpeg, uint32_t len)
{
	snapshot_done(V2_STREAM_ID, jpeg, len);
}

static void v3_snapshot_cb(void *jpeg, uint32_t len)
{
	snapshot_done(V3_STREAM_ID, jpeg, len);
}

static void snapshot_configure(int ch, mm_context_t *encoder)
{
	static const enc_snapshot_t callbacks[VIDEO_ENC_VALID_CH] = {
		[V1_STREAM_ID] = v1_snapshot_cb,
		[V2_STREAM_ID] = v2_snapshot_cb,
		[V3_STREAM_ID] = v3_snapshot_cb,
	};
	uint32_t format = enc_params[ch].codec_fmt;
	if (format == VIDEO_H264_JPEG || format == VIDEO_HEVC_JPEG) {
		snapshot_slot[ch].state = SNAPSHOT_IDLE;
		mm_module_ctrl(encoder, CMD_ENC_JPEG_SNAPSHOT_CB, (int)callbacks[ch]);
	} else {
		snapshot_slot[ch].state = SNAPSHOT_OFF;
	}
}

int video_app_snapshot_start(int ch, vp_snapshot_result_t *reply, uint32_t id)
{
	if ((unsigned)ch >= VIDEO_ENC_VALID_CH || !video_op_lock || !reply || !id) {
		return VIDEO_SNAPSHOT_INACTIVE;
	}
	if (rtos_mutex_take(video_op_lock, 0) != RTK_SUCCESS) {
		return VIDEO_SNAPSHOT_BUSY;
	}
	mm_context_t *encoder = video_app_enc_get_ctx(ch);
	int ret = VIDEO_SNAPSHOT_INACTIVE;
	int in_use = snapshot_slot[ch].reply ? snapshot_in_use(ch) : 0;
	taskENTER_CRITICAL();
	if (snapshot_slot[ch].state == SNAPSHOT_HELD && !in_use) {
		snapshot_slot[ch].state = SNAPSHOT_IDLE;
	}
	if (snapshot_slot[ch].state == SNAPSHOT_ENCODING || snapshot_slot[ch].state == SNAPSHOT_HELD) {
		ret = VIDEO_SNAPSHOT_BUSY;
	} else if (snapshot_slot[ch].state == SNAPSHOT_IDLE && encoder && video_ch_state[ch] == VIDEO_CH_RUNNING) {
		snapshot_slot[ch].state = SNAPSHOT_ENCODING;
		snapshot_slot[ch].id = id;
		snapshot_slot[ch].reply = reply;
		reply->id = id;
		snapshot_publish(reply, NULL, 0);
		ret = 0;
	}
	taskEXIT_CRITICAL();
	if (ret == 0) {
		ret = mm_module_ctrl(encoder, CMD_ENC_JPEG_DO_SNAPSHOT, 1);
		if (ret != 0) {
			ret = VIDEO_SNAPSHOT_ERROR;
			taskENTER_CRITICAL();
			snapshot_slot[ch].state = SNAPSHOT_IDLE;
			taskEXIT_CRITICAL();
		}
	}
	rtos_mutex_give(video_op_lock);
	return ret;
}

#if NN_ENA
static int desired_class_list[] = {0}; //represent the objects to be detected, please refer to yolov9/data/coco.yaml for the category of class id
static int check_in_list(int class_indx)
{
	for (int i = 0; i < (int)(sizeof(desired_class_list) / sizeof(int)); i++) {
		if (class_indx == desired_class_list[i]) {
			return class_indx;
		}
	}
	return -1;
}

static inline void set_bit_msb_first(uint8_t *ptr, int bitpos_in_byte)
{
	*ptr |= (uint8_t)(1u << (7 - bitpos_in_byte));
}

// Draw a horizontal line (inclusive). Fixed y, x from x0 to x1
// Assumes 0<=x0<=x1<width and 0<=y<height
static void draw_hline_1bpp(uint8_t *bitmap, int width, int height, int x0, int x1, int y)
{
	(void)height; // caller ensures validity
	int start_byte = (y * width + x0) >> 3;
	int end_byte   = (y * width + x1) >> 3;

	int start_bit = x0 & 0x7; // 0..7, maps to bit7..bit0 in MSB-first
	int end_bit   = x1 & 0x7;

	if (start_byte == end_byte) {
		// same byte
		uint8_t mask = 0;
		for (int b = start_bit; b <= end_bit; ++b) {
			mask |= (uint8_t)(1u << (7 - b));
		}
		bitmap[start_byte] |= mask;
		return;
	}

	// first byte: start_bit..7
	uint8_t first_mask = 0;
	for (int b = start_bit; b <= 7; ++b) {
		first_mask |= (uint8_t)(1u << (7 - b));
	}
	bitmap[start_byte] |= first_mask;

	// middle full bytes: set to 1 (0xFF)
	for (int idx = start_byte + 1; idx < end_byte; ++idx) {
		bitmap[idx] = 0xFF;
	}

	// last byte: 0..end_bit
	uint8_t last_mask = 0;
	for (int b = 0; b <= end_bit; ++b) {
		last_mask |= (uint8_t)(1u << (7 - b));
	}
	bitmap[end_byte] |= last_mask;
}

// Draw a vertical line (inclusive). Fixed x, y from y0 to y1
// Assumes 0<=y0<=y1<height and 0<=x<width
static void draw_vline_1bpp(uint8_t *bitmap, int width, int height, int x, int y0, int y1)
{
	(void)height;
	int bit_in_byte = x & 0x7;
	uint8_t mask = (uint8_t)(1u << (7 - bit_in_byte));
	int row_stride_bytes = (width >> 3); // bytes per row
	int col_byte_offset = (x >> 3);

	uint8_t *p = bitmap + (y0 * row_stride_bytes + col_byte_offset);
	int count = y1 - y0 + 1;

	for (int i = 0; i < count; ++i) {
		*p |= mask;
		p += row_stride_bytes;
	}
}

// Draw a rectangle outline with specified thickness (thickness >= 1)
void bitmap_update_bounding_box(uint8_t *bitmap, int width, int height,
								int xmin, int ymin, int xmax, int ymax,
								int thickness)
{
	if (!bitmap || thickness <= 0) {
		return;
	}

	// Clamp coordinates
	if (xmin < 0) {
		xmin = 0;
	}
	if (ymin < 0) {
		ymin = 0;
	}
	if (xmax >= width) {
		xmax = width - 1;
	}
	if (ymax >= height) {
		ymax = height - 1;
	}
	if (xmin > xmax || ymin > ymax) {
		return;
	}

	// Top and bottom edges: expand by thickness
	for (int t = 0; t < thickness; ++t) {
		int y_top = ymin + t;
		int y_bottom = ymax - t;
		if (y_top <= ymax) {
			draw_hline_1bpp(bitmap, width, height, xmin, xmax, y_top);
		}
		if (y_bottom >= ymin && y_bottom != y_top) {
			draw_hline_1bpp(bitmap, width, height, xmin, xmax, y_bottom);
		}
	}

	// Left and right edges: shrink y-range to avoid double-drawing corners
	int y0 = ymin + thickness;
	int y1 = ymax - thickness;
	if (y1 >= y0) {
		for (int t = 0; t < thickness; ++t) {
			int x_left = xmin + t;
			int x_right = xmax - t;
			draw_vline_1bpp(bitmap, width, height, x_left, y0, y1);
			if (x_right != x_left) {
				draw_vline_1bpp(bitmap, width, height, x_right, y0, y1);
			}
		}
	}
}

void video_set_osd_bitmap(int ch, int idx, uint32_t fmt, uint32_t x_off, uint32_t y_off, uint32_t w, uint32_t h, uint32_t alpha, uint32_t y, uint32_t u,
						  uint32_t v, uint8_t *bitmap)
{
	(void)ch;
	osd_overlay_t osd = {0};
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

	if (enc_v2_ctx) {
		mm_module_ctrl(enc_v2_ctx, CMD_ENC_SET_OSD, (int)&osd);
	}
}

#define LIMIT(x, lower, upper) if(x<lower) x=lower; else if(x>upper) x=upper;
static uint32_t nn_fps = 0;
static uint32_t nn_ts = 0;
static uint8_t *bitmap = NULL;
static void nn_display_cb(void *p, void *img_param)
{
	int i = 0;
	vipnn_out_buf_t *out = (vipnn_out_buf_t *)p;
	objdetect_res_t *res = (objdetect_res_t *)&out->res[0];

	int obj_num = out->res_cnt;

	nn_data_param_t *im = (nn_data_param_t *)img_param;

	if (!p || !img_param)	{
		return;
	}

	int im_w = enc_params[V2_STREAM_ID].width;
	int im_h = enc_params[V2_STREAM_ID].height;
#if NN_OSD_ENA
	int bitmap_size = im_w * im_h / 8;
	if (bitmap == NULL) {
		bitmap = (uint8_t *)malloc(bitmap_size);
	}
	memset(bitmap, 0, bitmap_size);
#endif
	float ratio_w = (float)im_w / (float)im->img.width;
	float ratio_h = (float)im_h / (float)im->img.height;
	int roi_h, roi_w, roi_x, roi_y;
	roi_w = (int)(im->img.width * ratio_w);
	roi_h = (int)(im->img.height * ratio_h);
	roi_x = 0;
	roi_y = 0;

	RTK_LOGI(TAG, "object num = %d\r\n", obj_num);
	if (obj_num > 0) {
		for (i = 0; i < obj_num; i++) {
			int obj_class = (int)res[i].result[0];
			int class_id = check_in_list(obj_class); //show class in desired_class_list
			if (class_id != -1) {
				int xmin = (int)(res[i].result[2] * roi_w) + roi_x;
				int ymin = (int)(res[i].result[3] * roi_h) + roi_y;
				int xmax = (int)(res[i].result[4] * roi_w) + roi_x;
				int ymax = (int)(res[i].result[5] * roi_h) + roi_y;
				LIMIT(xmin, 0, im_w)
				LIMIT(xmax, 0, im_w)
				LIMIT(ymin, 0, im_h)
				LIMIT(ymax, 0, im_h)
				RTK_LOGI(TAG, "%d,c%d:%d %d %d %d\n\r", i, class_id, xmin, ymin, xmax, ymax);
#if NN_OSD_ENA
				bitmap_update_bounding_box(bitmap, im_w, im_h, xmin, ymin, xmax, ymax, 10);
#endif
			}
		}
	}
#if NN_OSD_ENA
	DCache_Clean((uint32_t)bitmap, bitmap_size);
	video_set_osd_bitmap(V2_STREAM_ID, 0, 2, 0, 0, im_w, im_h, 128, 0, 0, 0, bitmap);
#endif
	nn_fps++;
	if ((rtos_time_get_current_system_time_ms() - nn_ts) > 1000) {
		RTK_LOGI(TAG, "NN fps %u\n\r", (unsigned int)nn_fps);
		nn_fps = 0;
		nn_ts = rtos_time_get_current_system_time_ms();
	}
}
#endif

#if V1_CROP_SCALEUP_ENA && (!defined(CONFIG_4K_DEV) || (CONFIG_4K_DEV != 1))
/* crop and scale by TS */
static void crop_scaleup_isp_to_enc(void *y_buf, void *uv_buf, uint32_t w, uint32_t h,  void *out_yuv_buf)
{
	(void)uv_buf;
	img_t im_in = {
		.width = w,
		.height = h,
		.data = y_buf
	};
	rect_t crop_roi = {
		.xmin = 0,
		.ymin = 0,
		.xmax = 1280,
		.ymax = 720
	};
	img_t im_out = {
		.width = w,
		.height = h,
		.data = out_yuv_buf
	};
	img_crop_then_scale(&im_in, &crop_roi, &im_out);
}
#endif

mm_context_t *video_app_isp_get_ctx(int ch)
{
	switch (ch) {
	case 0:
		return isp_v1_ctx;
	case 1:
		return isp_v2_ctx;
	case 2:
		return isp_v3_ctx;
	case 3:
		return isp_v4_ctx;
	case 4:
		return isp_v5_ctx;
	default:
		RTK_LOGE(TAG, "error: ch%d ISP module not opened yet\n\r", ch);
		return NULL;
	}
}

mm_context_t *video_app_enc_get_ctx(int ch)
{
	switch (ch) {
	case 0:
		return enc_v1_ctx;
	case 1:
		return enc_v2_ctx;
	case 2:
		return enc_v3_ctx;
	default:
		RTK_LOGE(TAG, "error: ch%d ENC module not opened yet\n\r", ch);
		return NULL;
	}
}

static int video_v1_start(void);
static int video_v2_start(void);
#if !defined(CONFIG_4K_DEV) || (CONFIG_4K_DEV != 1)
static int video_v3_start(void);
static int video_v4_start(void);

#endif
static int video_v5_start(void);

static void video_v1_deinit(void)
{
	if (siso_vp2ap_v1) {
		siso_stop(siso_vp2ap_v1);
		siso_delete(siso_vp2ap_v1);
		siso_vp2ap_v1 = NULL;
	}
	if (siso_isp_enc_v1) {
		siso_stop(siso_isp_enc_v1);
		siso_delete(siso_isp_enc_v1);
		siso_isp_enc_v1 = NULL;
	}
	if (enc_v1_ctx) {
		mm_module_close(enc_v1_ctx);
		enc_v1_ctx = NULL;
	}
	if (vp2ap_v1_ctx) {
		mm_module_close(vp2ap_v1_ctx);
		vp2ap_v1_ctx = NULL;
	}
	if (isp_v1_ctx) {
		mm_module_close(isp_v1_ctx);
		isp_v1_ctx = NULL;
	}
}

static void video_v2_deinit(void)
{
	if (siso_vp2ap_v2) {
		siso_stop(siso_vp2ap_v2);
		siso_delete(siso_vp2ap_v2);
		siso_vp2ap_v2 = NULL;
	}
	if (siso_isp_enc_v2) {
		siso_stop(siso_isp_enc_v2);
		siso_delete(siso_isp_enc_v2);
		siso_isp_enc_v2 = NULL;
	}
	if (enc_v2_ctx) {
		mm_module_close(enc_v2_ctx);
		enc_v2_ctx = NULL;
	}
	if (vp2ap_v2_ctx) {
		mm_module_close(vp2ap_v2_ctx);
		vp2ap_v2_ctx = NULL;
	}
	if (isp_v2_ctx) {
		mm_module_close(isp_v2_ctx);
		isp_v2_ctx = NULL;
	}
}

static void video_v3_deinit(void)
{
	if (siso_vp2ap_v3) {
		siso_stop(siso_vp2ap_v3);
		siso_delete(siso_vp2ap_v3);
		siso_vp2ap_v3 = NULL;
	}
	if (siso_isp_enc_v3) {
		siso_stop(siso_isp_enc_v3);
		siso_delete(siso_isp_enc_v3);
		siso_isp_enc_v3 = NULL;
	}
	if (enc_v3_ctx) {
		mm_module_close(enc_v3_ctx);
		enc_v3_ctx = NULL;
	}
	if (vp2ap_v3_ctx) {
		mm_module_close(vp2ap_v3_ctx);
		vp2ap_v3_ctx = NULL;
	}
	if (isp_v3_ctx) {
		mm_module_close(isp_v3_ctx);
		isp_v3_ctx = NULL;
	}
}

static void video_v4_deinit(void)
{
	if (siso_isp_vp2ap_v4) {
		siso_stop(siso_isp_vp2ap_v4);
		siso_delete(siso_isp_vp2ap_v4);
		siso_isp_vp2ap_v4 = NULL;
	}
	if (vp2ap_v4_ctx) {
		mm_module_close(vp2ap_v4_ctx);
		vp2ap_v4_ctx = NULL;
	}
	if (isp_v4_ctx) {
		mm_module_close(isp_v4_ctx);
		isp_v4_ctx = NULL;
	}
}

static void video_v5_deinit(void)
{
	if (siso_isp_vipnn) {
		siso_stop(siso_isp_vipnn);
		siso_delete(siso_isp_vipnn);
		siso_isp_vipnn = NULL;
	}
#if NN_ENA
	if (vipnn_ctx) {
		mm_module_close(vipnn_ctx);
		vipnn_ctx = NULL;
	}
#else
	if (vp2ap_v5_ctx) {
		mm_module_close(vp2ap_v5_ctx);
		vp2ap_v5_ctx = NULL;
	}
#endif
	if (isp_v5_ctx) {
		mm_module_close(isp_v5_ctx);
		isp_v5_ctx = NULL;
	}
}

#if !defined(CONFIG_4K_DEV) || (CONFIG_4K_DEV != 1)
static int video_siso_pause(mm_siso_t *siso)
{
	if (!siso || !siso->task) {
		return 0;
	}
	siso_pause(siso);
	if (siso->status != MMIC_STAT_PAUSE) {
		RTK_LOGE(TAG, "error: video linker pause failed\r\n");
		return -1;
	}
	return 0;
}

static void video_siso_resume(mm_siso_t *siso)
{
	if (siso && siso->task && siso->status == MMIC_STAT_PAUSE) {
		siso_resume(siso);
	}
}

static void video_channel_resume_links(int ch)
{
	switch (ch) {
	case V1_STREAM_ID:
		video_siso_resume(siso_isp_enc_v1);
		video_siso_resume(siso_vp2ap_v1);
		break;
	case V2_STREAM_ID:
		video_siso_resume(siso_isp_enc_v2);
		video_siso_resume(siso_vp2ap_v2);
		break;
	case V3_STREAM_ID:
		video_siso_resume(siso_isp_enc_v3);
		video_siso_resume(siso_vp2ap_v3);
		break;
	case V4_STREAM_ID:
		video_siso_resume(siso_isp_vp2ap_v4);
		break;
	case V5_STREAM_ID:
		video_siso_resume(siso_isp_vipnn);
		break;
	default:
		break;
	}
}

static int video_channel_pause_links(int ch)
{
	int ret = 0;

	switch (ch) {
	case V1_STREAM_ID:
		ret = video_siso_pause(siso_vp2ap_v1);
		if (ret == 0) {
			ret = video_siso_pause(siso_isp_enc_v1);
		}
		break;
	case V2_STREAM_ID:
		ret = video_siso_pause(siso_vp2ap_v2);
		if (ret == 0) {
			ret = video_siso_pause(siso_isp_enc_v2);
		}
		break;
	case V3_STREAM_ID:
		ret = video_siso_pause(siso_vp2ap_v3);
		if (ret == 0) {
			ret = video_siso_pause(siso_isp_enc_v3);
		}
		break;
	case V4_STREAM_ID:
		ret = video_siso_pause(siso_isp_vp2ap_v4);
		break;
	case V5_STREAM_ID:
		ret = video_siso_pause(siso_isp_vipnn);
		break;
	default:
		return -1;
	}

	if (ret < 0) {
		video_channel_resume_links(ch);
	}
	return ret;
}

#endif

/* Called with video_op_lock held. Block new work and let an AP read finish
 * before any teardown path can free the encoder output. */
static int snapshot_stop(int ch)
{
	taskENTER_CRITICAL();
	if (snapshot_slot[ch].state == SNAPSHOT_ENCODING) {
		snapshot_publish(snapshot_slot[ch].reply, NULL, VIDEO_SNAPSHOT_INACTIVE);
	}
	int held = snapshot_slot[ch].state == SNAPSHOT_HELD;
	if (!held) {
		snapshot_slot[ch].state = SNAPSHOT_OFF;
	}
	taskEXIT_CRITICAL();
	if (held) {
		TickType_t start = xTaskGetTickCount();
		while (snapshot_in_use(ch)) {
			if (xTaskGetTickCount() - start >= pdMS_TO_TICKS(3000)) {
				return -1;
			}
			vTaskDelay(1);
		}
		snapshot_slot[ch].state = SNAPSHOT_OFF;
	}
	return 0;
}

static int video_channel_deinit(int ch)
{
	mm_context_t *isp_ctx;

	if (ch < 0 || ch >= VIDEO_ISP_VALID_CH) {
		return -1;
	}
	/* Start/FCS error paths hold video_op_lock throughout, so no snapshot can be
	 * HELD; video_stream_stop_task() waits out AP's lease before calling here. */
	if (ch < VIDEO_ENC_VALID_CH && snapshot_stop(ch) < 0) {
		return -1;
	}
#if defined(CONFIG_4K_DEV) && (CONFIG_4K_DEV == 1)
	if (ch != V1_STREAM_ID && ch != V2_STREAM_ID && ch != V5_STREAM_ID) {
		return -1;
	}
	isp_ctx = video_app_isp_get_ctx(ch);
	/* Keep linkers running until the consumer returns its ISP 4K lease. */
	if (isp_ctx && mm_module_ctrl(isp_ctx, CMD_ISP_4K_STREAM_STOP, 0) < 0) {
		return -1;
	}
#else
	if (video_channel_pause_links(ch) < 0) {
		return -1;
	}
	isp_ctx = video_app_isp_get_ctx(ch);

	/* Stop data consumers before stream-off frees ISP frame buffers. */
	if (isp_ctx && mm_module_ctrl(isp_ctx, CMD_ISP_STREAM_STOP, 0) < 0) {
		RTK_LOGE(TAG, "error: ch%d ISP stream-off failed\r\n", ch);
		video_channel_resume_links(ch);
		return -1;
	}

#endif

	switch (ch) {
	case V1_STREAM_ID:
		video_v1_deinit();
		return 0;
	case V2_STREAM_ID:
		video_v2_deinit();
		return 0;
	case V3_STREAM_ID:
		video_v3_deinit();
		return 0;
	case V4_STREAM_ID:
		video_v4_deinit();
		return 0;
	case V5_STREAM_ID:
		video_v5_deinit();
		return 0;
	default:
		return -1;
	}
}

#if defined(CONFIG_FAST_CAMERA_START) && (CONFIG_FAST_CAMERA_START != 0)
static mm_siso_t *video_channel_vp2ap_siso(int ch)
{
	switch (ch) {
	case V1_STREAM_ID:
		return siso_vp2ap_v1;
	case V2_STREAM_ID:
		return siso_vp2ap_v2;
	case V3_STREAM_ID:
		return siso_vp2ap_v3;
	case V4_STREAM_ID:
		return siso_isp_vp2ap_v4;
#if !NN_ENA
	case V5_STREAM_ID:
		return siso_isp_vipnn;
#endif
	default:
		return NULL;
	}
}

static int video_channel_start(int ch)
{
	switch (ch) {
	case V1_STREAM_ID:
		return video_v1_start();
	case V2_STREAM_ID:
		return video_v2_start();
#if !defined(CONFIG_4K_DEV) || (CONFIG_4K_DEV != 1)
	case V3_STREAM_ID:
		return video_v3_start();
	case V4_STREAM_ID:
		return video_v4_start();
#endif
	case V5_STREAM_ID:
		return video_v5_start();
	default:
		return -1;
	}
}
#endif

static int video_v1_start(void)
{
	/* ISP V1 module init */
#if defined(CONFIG_4K_DEV) && (CONFIG_4K_DEV == 1)
	/* The build selects the module; validate the fixed output dimensions. */
	if (isp_params[V1_STREAM_ID].width != ISP_4K_OUTPUT_WIDTH ||
		isp_params[V1_STREAM_ID].height != ISP_4K_OUTPUT_HEIGHT ||
		!v_ch_en.enc[V1_STREAM_ID] ||
		enc_params[V1_STREAM_ID].width != ISP_4K_OUTPUT_WIDTH ||
		enc_params[V1_STREAM_ID].height != ISP_4K_OUTPUT_HEIGHT) {
		RTK_LOGE(TAG, "ISP 4K V1 config invalid: isp=%ux%u enc=%ux%u enc_enabled=%u; "
				 "required isp/enc=%ux%u enc_enabled=1\n\r",
				 (unsigned)isp_params[V1_STREAM_ID].width,
				 (unsigned)isp_params[V1_STREAM_ID].height,
				 (unsigned)enc_params[V1_STREAM_ID].width,
				 (unsigned)enc_params[V1_STREAM_ID].height,
				 (unsigned)v_ch_en.enc[V1_STREAM_ID],
				 (unsigned)ISP_4K_OUTPUT_WIDTH, (unsigned)ISP_4K_OUTPUT_HEIGHT);
		return -1;
	}
	/* Full NV12 frames do not use remap, decompression or slices. */
	isp_params[V1_STREAM_ID].compress_en = 0;
	isp_params[V1_STREAM_ID].slice_mode_en = 0;
	enc_params[V1_STREAM_ID].decompEnable = 0;
	enc_params[V1_STREAM_ID].lowLatencyEnable = 0;
	enc_params[V1_STREAM_ID].cs.cropScaleEnable = 0;

	/* Prepare the hardware-owned NV12 queue; defer APPLY until downstream is ready. */
	isp_v1_ctx = mm_module_open(&isp_4k_module);
	if (!isp_v1_ctx ||
		mm_module_ctrl(isp_v1_ctx, MM_CMD_SET_QUEUE_LEN, 2) < 0 ||
		mm_module_ctrl(isp_v1_ctx, MM_CMD_INIT_QUEUE_ITEMS, MMQI_FLAG_DYNAMIC) < 0) {
		RTK_LOGE(TAG, "ISP 4K V1 open fail\n\r");
		goto error;
	}
#else
	isp_v1_ctx = mm_module_open(&isp_module);
	if (isp_v1_ctx) {
		mm_module_ctrl(isp_v1_ctx, CMD_ISP_SET_PARAMS, (int)&isp_params[V1_STREAM_ID]);
		mm_module_ctrl(isp_v1_ctx, MM_CMD_SET_QUEUE_LEN, 2);
		mm_module_ctrl(isp_v1_ctx, MM_CMD_INIT_QUEUE_ITEMS, MMQI_FLAG_DYNAMIC);
		/* isp remap setting */
		if (isp_params[V1_STREAM_ID].slice_mode_en || isp_params[V1_STREAM_ID].compress_en) {
			isp_remap_v1_param.enc_type = (enc_params[V1_STREAM_ID].codec_fmt & VIDEO_HEVC) ? ISP_REMAP_ENC_HEVC : ISP_REMAP_ENC_H264;
			if (isp_params[V1_STREAM_ID].slice_mode_en && isp_params[V1_STREAM_ID].compress_en) {
				isp_remap_v1_param.feature_bit = (REMAP_SLICE | REMAP_COMPRESS);
				isp_remap_v1_param.burst_id = 2;  //compression must need 256-aligned
				RTK_LOGI(TAG, "video app set slice mode with compression\n\r");
			} else if (isp_params[V1_STREAM_ID].slice_mode_en) {
				isp_remap_v1_param.feature_bit = (REMAP_SLICE | REMAP_RTT);
				isp_remap_v1_param.burst_id = 0;  //RTT must need 64-aligned
				RTK_LOGI(TAG, "video app set slice mode with RTT\n\r");
			} else if (isp_params[V1_STREAM_ID].compress_en) {
				isp_remap_v1_param.feature_bit = REMAP_COMPRESS;
				isp_remap_v1_param.burst_id = 2;  //compression must need 256-aligned
				RTK_LOGI(TAG, "video app set frame mode compression\n\r");
			}
			mm_module_ctrl(isp_v1_ctx, CMD_ISP_SET_REMAP, (int)&isp_remap_v1_param);
		}
		if (mm_module_ctrl(isp_v1_ctx, CMD_ISP_APPLY, 0) < 0) {
			RTK_LOGE(TAG, "isp_v1_ctx apply fail\n\r");
			goto error;
		}
	} else {
		RTK_LOGE(TAG, "isp_v1_ctx open fail\n\r");
		goto error;
	}
#endif
	RTK_LOGI(TAG, "isp_v1_ctx open success\n\r");

	/* VP to AP */
	vp2ap_v1_ctx = mm_module_open(&vp2ap_module);
	if (vp2ap_v1_ctx) {
		mm_module_ctrl(vp2ap_v1_ctx, CMD_VP2AP_SET_PARAMS, (int)&vp2ap_v1_params);
		mm_module_ctrl(vp2ap_v1_ctx, CMD_VP2AP_APPLY, 0);
	} else {
		RTK_LOGE(TAG, "vp2ap open fail\n\r");
		goto error;
	}

	if (v_ch_en.enc[V1_STREAM_ID]) {
		/* ENC V1 module init */
		enc_v1_ctx = mm_module_open(&enc_module);
		if (enc_v1_ctx) {
			mm_module_ctrl(enc_v1_ctx, CMD_ENC_SET_PARAMS, (int)&enc_params[V1_STREAM_ID]);
#if V1_CROP_SCALEUP_ENA && (!defined(CONFIG_4K_DEV) || (CONFIG_4K_DEV != 1))
			mm_module_ctrl(enc_v1_ctx, CMD_ENC_SET_PREPROCESS_CB, (int)crop_scaleup_isp_to_enc);
#endif
			snapshot_configure(V1_STREAM_ID, enc_v1_ctx);
			mm_module_ctrl(enc_v1_ctx, MM_CMD_SET_QUEUE_LEN, 6);//V2_ENC_QUEUE_LEN);
			mm_module_ctrl(enc_v1_ctx, MM_CMD_INIT_QUEUE_ITEMS, MMQI_FLAG_DYNAMIC);
			if (mm_module_ctrl(enc_v1_ctx, CMD_ENC_INIT_MEM_POOL, 0) < 0) {
				RTK_LOGE(TAG, "enc_v1_ctx memory pool init fail\n\r");
				goto error;
			}
#if !defined(CONFIG_4K_DEV) || (CONFIG_4K_DEV != 1)
			mm_module_ctrl(enc_v1_ctx, CMD_ENC_SET_ISP_REMAP, (int)&isp_remap_v1_param);
			mm_module_ctrl(enc_v1_ctx, CMD_ENC_SET_ENC_REMAP, (int)&enc_remap_v1_param);
#endif
			if (mm_module_ctrl(enc_v1_ctx, CMD_ENC_APPLY, 0) < 0) {
				RTK_LOGE(TAG, "enc_v1_ctx apply fail\n\r");
				goto error;
			}
		} else {
			RTK_LOGE(TAG, "ENC open fail\n\r");
			goto error;
		}
		RTK_LOGI(TAG, "enc_v1_ctx open success\n\r");

		/* SISO linker create */
		siso_isp_enc_v1 = siso_create();
		if (siso_isp_enc_v1) {
			siso_ctrl(siso_isp_enc_v1, MMIC_CMD_ADD_INPUT, (uint32_t)isp_v1_ctx, 0);
			siso_ctrl(siso_isp_enc_v1, MMIC_CMD_ADD_OUTPUT, (uint32_t)enc_v1_ctx, 0);
			siso_ctrl(siso_isp_enc_v1, MMIC_CMD_SET_TASKPRIORITY, enc_v1_prio, 0);
			if (siso_start(siso_isp_enc_v1) < 0) {
				RTK_LOGE(TAG, "siso_isp_enc_v1 start fail\n\r");
				goto error;
			}
		} else {
			RTK_LOGE(TAG, "siso_isp_enc_v1 open fail\n\r");
			goto error;
		}
		RTK_LOGI(TAG, "siso_isp_enc_v1 start success\n\r");

		/* SISO linker create */
		siso_vp2ap_v1 = siso_create();
		if (siso_vp2ap_v1) {
			siso_ctrl(siso_vp2ap_v1, MMIC_CMD_ADD_INPUT, (uint32_t)enc_v1_ctx, 0);
			siso_ctrl(siso_vp2ap_v1, MMIC_CMD_ADD_OUTPUT, (uint32_t)vp2ap_v1_ctx, 0);
			if (video_vp2ap_link_ready(siso_vp2ap_v1, V1_STREAM_ID) < 0) {
				goto error;
			}
		} else {
			RTK_LOGE(TAG, "siso_vp2ap_v1 open fail\n\r");
			goto error;
		}

	} else {
		/* SISO linker create */
		while (isp_v1_ctx == NULL) {;}
		siso_vp2ap_v1 = siso_create();
		if (siso_vp2ap_v1) {
			siso_ctrl(siso_vp2ap_v1, MMIC_CMD_ADD_INPUT, (uint32_t)isp_v1_ctx, 0);
			siso_ctrl(siso_vp2ap_v1, MMIC_CMD_ADD_OUTPUT, (uint32_t)vp2ap_v1_ctx, 0);
			if (video_vp2ap_link_ready(siso_vp2ap_v1, V1_STREAM_ID) < 0) {
				goto error;
			}
		} else {
			RTK_LOGE(TAG, "siso_vp2ap_v1 open fail\n\r");
			goto error;
		}
	}

#if defined(CONFIG_4K_DEV) && (CONFIG_4K_DEV == 1)
	/* Publish NV12 leases only after encoder and both linkers are ready. */
	if (mm_module_ctrl(isp_v1_ctx, CMD_ISP_4K_APPLY, 0) != 0) {
		RTK_LOGE(TAG, "ISP 4K apply fail\n\r");
		goto error;
	}
#endif

	return 0;
error:
	video_channel_deinit(V1_STREAM_ID);
	return -1;
}

static int video_v2_start(void)
{
	/* ISP V2 module init */
#if defined(CONFIG_4K_DEV) && (CONFIG_4K_DEV == 1)
	/* V2 is the downscaled ISP 4K output; validate its fixed dimensions. */
	if (isp_params[V2_STREAM_ID].width != ISP_4K_SUB_OUTPUT_WIDTH ||
		isp_params[V2_STREAM_ID].height != ISP_4K_SUB_OUTPUT_HEIGHT ||
		!v_ch_en.enc[V2_STREAM_ID] ||
		enc_params[V2_STREAM_ID].width != ISP_4K_SUB_OUTPUT_WIDTH ||
		enc_params[V2_STREAM_ID].height != ISP_4K_SUB_OUTPUT_HEIGHT) {
		RTK_LOGE(TAG, "ISP 4K V2 config invalid: isp=%ux%u enc=%ux%u enc_enabled=%u; "
				 "required isp/enc=%ux%u enc_enabled=1\n\r",
				 (unsigned)isp_params[V2_STREAM_ID].width,
				 (unsigned)isp_params[V2_STREAM_ID].height,
				 (unsigned)enc_params[V2_STREAM_ID].width,
				 (unsigned)enc_params[V2_STREAM_ID].height,
				 (unsigned)v_ch_en.enc[V2_STREAM_ID],
				 (unsigned)ISP_4K_SUB_OUTPUT_WIDTH, (unsigned)ISP_4K_SUB_OUTPUT_HEIGHT);
		return -1;
	}
	/* Full NV12 frames do not use decompression, low latency or crop-scale. */
	enc_params[V2_STREAM_ID].decompEnable = 0;
	enc_params[V2_STREAM_ID].lowLatencyEnable = 0;
	enc_params[V2_STREAM_ID].cs.cropScaleEnable = 0;

	/* Bind the sub stream and prepare its queue; defer APPLY until downstream is ready. */
	isp_v2_ctx = mm_module_open(&isp_4k_module);
	if (!isp_v2_ctx ||
		mm_module_ctrl(isp_v2_ctx, CMD_ISP_4K_SET_STREAM, ISP_4K_SUB_STREAM_IDX) < 0 ||
		mm_module_ctrl(isp_v2_ctx, MM_CMD_SET_QUEUE_LEN, 2) < 0 ||
		mm_module_ctrl(isp_v2_ctx, MM_CMD_INIT_QUEUE_ITEMS, MMQI_FLAG_DYNAMIC) < 0) {
		RTK_LOGE(TAG, "ISP 4K V2 open fail\n\r");
		goto error;
	}
#else
	isp_v2_ctx = mm_module_open(&isp_module);
	if (isp_v2_ctx) {
		mm_module_ctrl(isp_v2_ctx, CMD_ISP_SET_PARAMS, (int)&isp_params[V2_STREAM_ID]);
		mm_module_ctrl(isp_v2_ctx, MM_CMD_SET_QUEUE_LEN, 2);
		mm_module_ctrl(isp_v2_ctx, MM_CMD_INIT_QUEUE_ITEMS, MMQI_FLAG_DYNAMIC);
		if (mm_module_ctrl(isp_v2_ctx, CMD_ISP_APPLY, 1) < 0) {
			RTK_LOGE(TAG, "isp_v2_ctx apply fail\n\r");
			goto error;
		}
	} else {
		RTK_LOGE(TAG, "ISP open fail\n\r");
		goto error;
	}
#endif
	RTK_LOGI(TAG, "isp_v2_ctx open success\n\r");

	/* VP to AP */
	vp2ap_v2_ctx = mm_module_open(&vp2ap_module);
	if (vp2ap_v2_ctx) {
		mm_module_ctrl(vp2ap_v2_ctx, CMD_VP2AP_SET_PARAMS, (int)&vp2ap_v2_params);
		mm_module_ctrl(vp2ap_v2_ctx, CMD_VP2AP_APPLY, 0);
	} else {
		RTK_LOGE(TAG, "vp2ap_v2_ctx open fail\n\r");
		goto error;
	}

	if (v_ch_en.enc[V2_STREAM_ID]) {
		/* ENC V2 module init */
		enc_v2_ctx = mm_module_open(&enc_module);
		if (enc_v2_ctx) {
			mm_module_ctrl(enc_v2_ctx, CMD_ENC_SET_PARAMS, (int)&enc_params[V2_STREAM_ID]);
			snapshot_configure(V2_STREAM_ID, enc_v2_ctx);
			mm_module_ctrl(enc_v2_ctx, MM_CMD_SET_QUEUE_LEN, 6);//V2_ENC_QUEUE_LEN);
			mm_module_ctrl(enc_v2_ctx, MM_CMD_INIT_QUEUE_ITEMS, MMQI_FLAG_DYNAMIC);
			if (mm_module_ctrl(enc_v2_ctx, CMD_ENC_INIT_MEM_POOL, 0) < 0) {
				RTK_LOGE(TAG, "enc_v2_ctx memory pool init fail\n\r");
				goto error;
			}
			if (mm_module_ctrl(enc_v2_ctx, CMD_ENC_APPLY, 0) < 0) {
				RTK_LOGE(TAG, "enc_v2_ctx apply fail\n\r");
				goto error;
			}
		} else {
			RTK_LOGE(TAG, "ENC open fail\n\r");
			goto error;
		}
		RTK_LOGI(TAG, "enc_v2_ctx open success\n\r");

		/* SISO linker create */
		siso_isp_enc_v2 = siso_create();
		if (siso_isp_enc_v2) {
			siso_ctrl(siso_isp_enc_v2, MMIC_CMD_ADD_INPUT, (uint32_t)isp_v2_ctx, 0);
			siso_ctrl(siso_isp_enc_v2, MMIC_CMD_ADD_OUTPUT, (uint32_t)enc_v2_ctx, 0);
			siso_ctrl(siso_isp_enc_v2, MMIC_CMD_SET_TASKPRIORITY, enc_v2_prio, 0);
			if (siso_start(siso_isp_enc_v2) < 0) {
				RTK_LOGE(TAG, "siso_isp_enc_v2 start fail\n\r");
				goto error;
			}
		} else {
			RTK_LOGE(TAG, "siso_isp_enc_v2 open fail\n\r");
			goto error;
		}
		RTK_LOGI(TAG, "siso_isp_enc_v2 start success\n\r");

		/* SISO linker create */
		siso_vp2ap_v2 = siso_create();
		if (siso_vp2ap_v2) {
			siso_ctrl(siso_vp2ap_v2, MMIC_CMD_ADD_INPUT, (uint32_t)enc_v2_ctx, 0);
			siso_ctrl(siso_vp2ap_v2, MMIC_CMD_ADD_OUTPUT, (uint32_t)vp2ap_v2_ctx, 0);
			if (video_vp2ap_link_ready(siso_vp2ap_v2, V2_STREAM_ID) < 0) {
				goto error;
			}
		} else {
			RTK_LOGE(TAG, "siso_vp2ap_v2 open fail\n\r");
			goto error;
		}

	} else {
		/* SISO linker create */
		while (isp_v2_ctx == NULL) {;}
		siso_vp2ap_v2 = siso_create();
		if (siso_vp2ap_v2) {
			siso_ctrl(siso_vp2ap_v2, MMIC_CMD_ADD_INPUT, (uint32_t)isp_v2_ctx, 0);
			siso_ctrl(siso_vp2ap_v2, MMIC_CMD_ADD_OUTPUT, (uint32_t)vp2ap_v2_ctx, 0);
			if (video_vp2ap_link_ready(siso_vp2ap_v2, V2_STREAM_ID) < 0) {
				goto error;
			}
		} else {
			RTK_LOGE(TAG, "siso_vp2ap_v2 open fail\n\r");
			goto error;
		}
	}

#if defined(CONFIG_4K_DEV) && (CONFIG_4K_DEV == 1)
	/* An attached output that nobody consumes stalls the shared 4K source. */
	if (mm_module_ctrl(isp_v2_ctx, CMD_ISP_4K_APPLY, 0) != 0) {
		RTK_LOGE(TAG, "ISP 4K V2 apply fail\n\r");
		goto error;
	}
#endif

	return 0;
error:
	video_channel_deinit(V2_STREAM_ID);
	return -1;
}

#if !defined(CONFIG_4K_DEV) || (CONFIG_4K_DEV != 1)
static int video_v3_start(void)
{
	/* ISP V3 module init */
	isp_v3_ctx = mm_module_open(&isp_module);
	if (isp_v3_ctx) {
		mm_module_ctrl(isp_v3_ctx, CMD_ISP_SET_PARAMS, (int)&isp_params[V3_STREAM_ID]);
		mm_module_ctrl(isp_v3_ctx, MM_CMD_SET_QUEUE_LEN, 2);
		mm_module_ctrl(isp_v3_ctx, MM_CMD_INIT_QUEUE_ITEMS, MMQI_FLAG_DYNAMIC);
		if (mm_module_ctrl(isp_v3_ctx, CMD_ISP_APPLY, 2) < 0) {
			RTK_LOGE(TAG, "isp_v3_ctx apply fail\n\r");
			goto error;
		}
	} else {
		RTK_LOGE(TAG, "ISP open fail\n\r");
		goto error;
	}
	RTK_LOGI(TAG, "isp_v3_ctx open success\n\r");

	/* VP to AP */
	vp2ap_v3_ctx = mm_module_open(&vp2ap_module);
	if (vp2ap_v3_ctx) {
		mm_module_ctrl(vp2ap_v3_ctx, CMD_VP2AP_SET_PARAMS, (int)&vp2ap_v3_params);
		mm_module_ctrl(vp2ap_v3_ctx, CMD_VP2AP_APPLY, 0);
	} else {
		RTK_LOGE(TAG, "vp2ap_v3_ctx open fail\n\r");
		goto error;
	}

	if (v_ch_en.enc[V3_STREAM_ID]) {
		/* ENC V3 module init */
		enc_v3_ctx = mm_module_open(&enc_module);
		if (enc_v3_ctx) {
			mm_module_ctrl(enc_v3_ctx, CMD_ENC_SET_PARAMS, (int)&enc_params[V3_STREAM_ID]);
			snapshot_configure(V3_STREAM_ID, enc_v3_ctx);
			mm_module_ctrl(enc_v3_ctx, MM_CMD_SET_QUEUE_LEN, 6);//V2_ENC_QUEUE_LEN);
			mm_module_ctrl(enc_v3_ctx, MM_CMD_INIT_QUEUE_ITEMS, MMQI_FLAG_DYNAMIC);
			if (mm_module_ctrl(enc_v3_ctx, CMD_ENC_INIT_MEM_POOL, 0) < 0) {
				RTK_LOGE(TAG, "enc_v3_ctx memory pool init fail\n\r");
				goto error;
			}
			if (mm_module_ctrl(enc_v3_ctx, CMD_ENC_APPLY, 0) < 0) {
				RTK_LOGE(TAG, "enc_v3_ctx apply fail\n\r");
				goto error;
			}
		} else {
			RTK_LOGE(TAG, "ENC open fail\n\r");
			goto error;
		}
		RTK_LOGI(TAG, "enc_v3_ctx open success\n\r");

		/* SISO linker create */
		while (isp_v3_ctx == NULL || enc_v3_ctx == NULL) {;}
		siso_isp_enc_v3 = siso_create();
		if (siso_isp_enc_v3) {
			siso_ctrl(siso_isp_enc_v3, MMIC_CMD_ADD_INPUT, (uint32_t)isp_v3_ctx, 0);
			siso_ctrl(siso_isp_enc_v3, MMIC_CMD_ADD_OUTPUT, (uint32_t)enc_v3_ctx, 0);
			siso_ctrl(siso_isp_enc_v3, MMIC_CMD_SET_TASKPRIORITY, enc_v3_prio, 0);
			if (siso_start(siso_isp_enc_v3) < 0) {
				RTK_LOGE(TAG, "siso_isp_enc_v3 start fail\n\r");
				goto error;
			}
		} else {
			RTK_LOGE(TAG, "siso_isp_enc_v3 open fail\n\r");
			goto error;
		}
		RTK_LOGI(TAG, "siso_isp_enc_v3 start success\n\r");

		/* SISO linker create */
		siso_vp2ap_v3 = siso_create();
		if (siso_vp2ap_v3) {
			siso_ctrl(siso_vp2ap_v3, MMIC_CMD_ADD_INPUT, (uint32_t)enc_v3_ctx, 0);
			siso_ctrl(siso_vp2ap_v3, MMIC_CMD_ADD_OUTPUT, (uint32_t)vp2ap_v3_ctx, 0);
			if (video_vp2ap_link_ready(siso_vp2ap_v3, V3_STREAM_ID) < 0) {
				goto error;
			}
		} else {
			RTK_LOGE(TAG, "siso_vp2ap_v3 open fail\n\r");
			goto error;
		}
	} else {
		/* SISO linker create */
		while (isp_v3_ctx == NULL) {;}
		siso_vp2ap_v3 = siso_create();
		if (siso_vp2ap_v3) {
			siso_ctrl(siso_vp2ap_v3, MMIC_CMD_ADD_INPUT, (uint32_t)isp_v3_ctx, 0);
			siso_ctrl(siso_vp2ap_v3, MMIC_CMD_ADD_OUTPUT, (uint32_t)vp2ap_v3_ctx, 0);
			if (video_vp2ap_link_ready(siso_vp2ap_v3, V3_STREAM_ID) < 0) {
				goto error;
			}
		} else {
			RTK_LOGE(TAG, "siso_vp2ap_v3 open fail\n\r");
			goto error;
		}
	}
	return 0;
error:
	video_channel_deinit(V3_STREAM_ID);
	return -1;
}

static int video_v4_start(void)
{
	/* ISP V4 module init */
	isp_v4_ctx = mm_module_open(&isp_module);
	if (isp_v4_ctx) {
		mm_module_ctrl(isp_v4_ctx, CMD_ISP_SET_PARAMS, (int)&isp_params[V4_STREAM_ID]);
		mm_module_ctrl(isp_v4_ctx, MM_CMD_SET_QUEUE_LEN, 2);
		mm_module_ctrl(isp_v4_ctx, MM_CMD_INIT_QUEUE_ITEMS, MMQI_FLAG_DYNAMIC);
		if (mm_module_ctrl(isp_v4_ctx, CMD_ISP_APPLY, 3) < 0) {
			RTK_LOGE(TAG, "isp_v4_ctx apply fail\n\r");
			goto error;
		}
	} else {
		RTK_LOGE(TAG, "ISP open fail\n\r");
		goto error;
	}
	RTK_LOGI(TAG, "isp_v4_ctx open success\n\r");

	/* VP to AP */
	vp2ap_v4_ctx = mm_module_open(&vp2ap_module);
	if (vp2ap_v4_ctx) {
		mm_module_ctrl(vp2ap_v4_ctx, CMD_VP2AP_SET_PARAMS, (int)&vp2ap_v4_params);
		mm_module_ctrl(vp2ap_v4_ctx, CMD_VP2AP_APPLY, 0);
	} else {
		RTK_LOGE(TAG, "vp2ap_v4_ctx open fail\n\r");
		goto error;
	}

	/* SISO linker create */
	while (isp_v4_ctx == NULL || vp2ap_v4_ctx == NULL) {;}
	siso_isp_vp2ap_v4 = siso_create();
	if (siso_isp_vp2ap_v4) {
		siso_ctrl(siso_isp_vp2ap_v4, MMIC_CMD_ADD_INPUT, (uint32_t)isp_v4_ctx, 0);
		siso_ctrl(siso_isp_vp2ap_v4, MMIC_CMD_ADD_OUTPUT, (uint32_t)vp2ap_v4_ctx, 0);
		if (video_vp2ap_link_ready(siso_isp_vp2ap_v4, V4_STREAM_ID) < 0) {
			goto error;
		}
	} else {
		RTK_LOGE(TAG, "siso_isp_vp2ap_v4 open fail\n\r");
		goto error;
	}
	return 0;
error:
	video_channel_deinit(V4_STREAM_ID);
	return -1;
}

#endif

static int video_v5_start(void)
{
	/* ISP V5 module init */
#if defined(CONFIG_4K_DEV) && (CONFIG_4K_DEV == 1)
	/* V5 is the planar RGB output on the ISP 4K zoom AI channel. */
	if (isp_params[V5_STREAM_ID].width != ISP_4K_RGB_WIDTH ||
		isp_params[V5_STREAM_ID].height != ISP_4K_RGB_HEIGHT ||
		isp_params[V5_STREAM_ID].format != ISP_FORMAT_RGB888_PLANAR) {
		RTK_LOGE(TAG, "ISP 4K V5 config invalid: isp=%ux%u fmt=%u; required %ux%u fmt=%u\n\r",
				 (unsigned)isp_params[V5_STREAM_ID].width,
				 (unsigned)isp_params[V5_STREAM_ID].height,
				 (unsigned)isp_params[V5_STREAM_ID].format,
				 (unsigned)ISP_4K_RGB_WIDTH, (unsigned)ISP_4K_RGB_HEIGHT,
				 (unsigned)ISP_FORMAT_RGB888_PLANAR);
		return -1;
	}

	/* Bind the RGB stream and prepare its queue; defer APPLY until downstream is ready. */
	isp_v5_ctx = mm_module_open(&isp_4k_module);
	if (!isp_v5_ctx ||
		mm_module_ctrl(isp_v5_ctx, CMD_ISP_4K_SET_STREAM, ISP_4K_RGB_STREAM_IDX) < 0 ||
		mm_module_ctrl(isp_v5_ctx, MM_CMD_SET_QUEUE_LEN, 2) < 0 ||
		mm_module_ctrl(isp_v5_ctx, MM_CMD_INIT_QUEUE_ITEMS, MMQI_FLAG_DYNAMIC) < 0) {
		RTK_LOGE(TAG, "ISP 4K V5 open fail\n\r");
		goto error;
	}
#else
	isp_v5_ctx = mm_module_open(&isp_module);
	if (isp_v5_ctx) {
		mm_module_ctrl(isp_v5_ctx, CMD_ISP_SET_PARAMS, (int)&isp_params[V5_STREAM_ID]);
		mm_module_ctrl(isp_v5_ctx, MM_CMD_SET_QUEUE_LEN, 2);
		mm_module_ctrl(isp_v5_ctx, MM_CMD_INIT_QUEUE_ITEMS, MMQI_FLAG_DYNAMIC);
		if (mm_module_ctrl(isp_v5_ctx, CMD_ISP_APPLY, 4) < 0) {
			RTK_LOGE(TAG, "isp_v5_ctx apply fail\n\r");
			goto error;
		}
	} else {
		RTK_LOGE(TAG, "ISP open fail\n\r");
		goto error;
	}
#endif
	RTK_LOGI(TAG, "isp_v5_ctx open success\n\r");
#if NN_ENA
	// VIPNN
	vipnn_ctx = mm_module_open(&vipnn_module);
	if (vipnn_ctx) {
		mm_module_ctrl(vipnn_ctx, CMD_VIPNN_SET_MODEL, (int)&NN_MODEL_OBJ);
		mm_module_ctrl(vipnn_ctx, CMD_VIPNN_SET_IN_PARAMS, (int)&nn_input_params);
		mm_module_ctrl(vipnn_ctx, CMD_VIPNN_SET_DISPPOST, (int)nn_display_cb);
		mm_module_ctrl(vipnn_ctx, CMD_VIPNN_SET_RES_SIZE, sizeof(objdetect_res_t));		// result size
		mm_module_ctrl(vipnn_ctx, CMD_VIPNN_SET_RES_MAX_CNT, 32);		// result max count
		mm_module_ctrl(vipnn_ctx, CMD_VIPNN_APPLY, 0);
	} else {
		RTK_LOGE(TAG, "VIPNN open fail\n\r");
		goto error;
	}
	RTK_LOGI(TAG, "VIPNN opened\n\r");
#else
	/* VP to AP */
	vp2ap_v5_ctx = mm_module_open(&vp2ap_module);
	if (vp2ap_v5_ctx) {
		mm_module_ctrl(vp2ap_v5_ctx, CMD_VP2AP_SET_PARAMS, (int)&vp2ap_v5_params);
		mm_module_ctrl(vp2ap_v5_ctx, CMD_VP2AP_APPLY, 0);
	} else {
		RTK_LOGE(TAG, "vp2ap_v5_ctx open fail\n\r");
		goto error;
	}
#endif
	siso_isp_vipnn = siso_create();
	if (siso_isp_vipnn) {
		siso_ctrl(siso_isp_vipnn, MMIC_CMD_ADD_INPUT, (uint32_t)isp_v5_ctx, 0);
#if NN_ENA
		siso_ctrl(siso_isp_vipnn, MMIC_CMD_ADD_OUTPUT, (uint32_t)vipnn_ctx, 0);
#else
		siso_ctrl(siso_isp_vipnn, MMIC_CMD_ADD_OUTPUT, (uint32_t)vp2ap_v5_ctx, 0);
#endif
#if NN_ENA
		if (siso_start(siso_isp_vipnn) < 0) {
			RTK_LOGE(TAG, "siso_isp_vipnn start fail\n\r");
			goto error;
		}
		RTK_LOGI(TAG, "siso_isp_vipnn started\n\r");
#else
		if (video_vp2ap_link_ready(siso_isp_vipnn, V5_STREAM_ID) < 0) {
			goto error;
		}
#endif
	} else {
		RTK_LOGE(TAG, "siso_isp_vipnn open fail\n\r");
		goto error;
	}

#if defined(CONFIG_4K_DEV) && (CONFIG_4K_DEV == 1)
	/* An attached output that nobody consumes stalls the shared 4K source.
	 * V5 feeds vp2ap directly, so with a deferred FCS link it is applied at
	 * attach (video_app_fcs_attach()). */
	if (video_ch_state[V5_STREAM_ID] != VIDEO_CH_FCS_READY &&
		mm_module_ctrl(isp_v5_ctx, CMD_ISP_4K_APPLY, 0) != 0) {
		RTK_LOGE(TAG, "ISP 4K V5 apply fail\n\r");
		goto error;
	}
#endif

	return 0;
error:
	video_channel_deinit(V5_STREAM_ID);
	return -1;
}

/* Lazily create video_op_lock exactly once. The double-checked create is
 * guarded by a critical section so concurrent first-callers (e.g. the IPC
 * handler task and the FCS / stream-start path) cannot create two mutexes. */
static void video_start_lock_ensure(void)
{
	if (video_op_lock == NULL) {
		rtos_critical_enter(RTOS_CRITICAL_DEFAULT);
		if (video_op_lock == NULL) {
			rtos_mutex_create(&video_op_lock);
		}
		rtos_critical_exit(RTOS_CRITICAL_DEFAULT);
	}
}

#if defined(CONFIG_FAST_CAMERA_START) && (CONFIG_FAST_CAMERA_START != 0)
int video_app_fcs_attach(int ch)
{
	int ret = -1;
	mm_siso_t *siso;

	if (ch < 0 || ch >= VIDEO_ISP_VALID_CH) {
		return -1;
	}
	video_start_lock_ensure();
	rtos_mutex_take(video_op_lock, portMAX_DELAY);

	if (video_ch_state[ch] == VIDEO_CH_FCS_STARTING) {
		fcs_attach_pending[ch] = 1;
		ret = 0;
		RTK_LOGI(TAG, "fcs ch%d attach pending\n\r", ch);
	} else if (video_ch_state[ch] == VIDEO_CH_FCS_READY) {
		siso = video_channel_vp2ap_siso(ch);
		if (siso && siso_start(siso) >= 0) {
			video_ch_state[ch] = VIDEO_CH_RUNNING;
			ret = 0;
#if defined(CONFIG_4K_DEV) && (CONFIG_4K_DEV == 1)
			/* V5's APPLY waits for its deferred vp2ap link, so an unconsumed
			 * output never stalls the shared 4K source (video_v5_start()). */
			if (ch == V5_STREAM_ID && mm_module_ctrl(isp_v5_ctx, CMD_ISP_4K_APPLY, 0) != 0) {
				RTK_LOGE(TAG, "ISP 4K V5 apply fail\n\r");
				siso_stop(siso);
				video_ch_state[ch] = VIDEO_CH_ERROR;
				ret = -1;
			}
#endif
			if (ret == 0) {
				RTK_LOGI(TAG, "fcs ch%d attached\n\r", ch);
			}
		}
	} else if (video_ch_state[ch] == VIDEO_CH_RUNNING) {
		ret = 0;
	}

	rtos_mutex_give(video_op_lock);
	return ret;
}

int video_app_fcs_get_boot_state(void)
{
	return video_fcs_boot_state;
}
#endif

/* Restart a channel whose ISP context is still open (V1, V2, V5). */
static int video_isp_restart(int ch, mm_context_t *isp_ctx)
{
#if defined(CONFIG_4K_DEV) && (CONFIG_4K_DEV == 1)
	/* The 4K module has no resume/reconfigure path: new params would be
	 * silently ignored. Require an explicit stop before restarting.
	 */
	(void)isp_ctx;
	RTK_LOGE(TAG, "error: ch%d ISP 4K already started, stop it first\r\n", ch);
	return -1;
#else
	if (mm_module_ctrl(isp_ctx, CMD_ISP_STREAM_START, 0) < 0) {
		video_ch_state[ch] = VIDEO_CH_ERROR;
		return -1;
	}
	video_ch_state[ch] = VIDEO_CH_RUNNING;
	return 0;
#endif
}

static int video_stream_start(char *user_cmd)
{
	int ret = 0;

	rtos_mutex_take(video_op_lock, portMAX_DELAY);

	/* parsing video app params to isp/enc params */
	memset(&v_ch_en, 0, sizeof(isp_enc_ch_en_t));
	if (video_app_params_parsing(&v_ch_en, user_cmd, isp_params, enc_params) != 0) {
		RTK_LOGE(TAG, "error: fail to parse video params\r\n");
		rtos_mutex_give(video_op_lock);
		return -1;
	}
#if defined(CONFIG_4K_DEV) && (CONFIG_4K_DEV == 1)
	/* The 4K pipeline owns the ISP exclusively: V1 (main), V2 (sub) and V5 (RGB) only. */
	if (v_ch_en.isp[V3_STREAM_ID] || v_ch_en.isp[V4_STREAM_ID]) {
		RTK_LOGE(TAG, "ISP 4K supports V1, V2 and V5 only\r\n");
		rtos_mutex_give(video_op_lock);
		return -1;
	}
#endif
	/* A deferred stop still owns the channel while AP finishes reading. */
	for (int ch = 0; ch < VIDEO_ISP_VALID_CH; ch++) {
		if (v_ch_en.isp[ch] && video_ch_state[ch] == VIDEO_CH_CLOSING) {
			RTK_LOGW(TAG, "ch%d still closing, retry later\r\n", ch);
			rtos_mutex_give(video_op_lock);
			return -1;
		}
	}
	/* set slice mode encoder task priority */
	enc_v1_prio = 5;
	enc_v2_prio = 4;
	enc_v3_prio = 3;
	if (isp_params[V1_STREAM_ID].slice_mode_en) {
		enc_v1_prio = 4;
		enc_v2_prio = 5;
	}

	/* start channel if required */
	if (v_ch_en.isp[V1_STREAM_ID]) {
		if (isp_v1_ctx == NULL) {
			video_ch_state[V1_STREAM_ID] = VIDEO_CH_STARTING;
			if (video_v1_start() < 0) {
				video_ch_state[V1_STREAM_ID] = VIDEO_CH_ERROR;
				ret = -1;
			}
		} else if (video_isp_restart(V1_STREAM_ID, isp_v1_ctx) < 0) {
			ret = -1;
		}
	}
	if (v_ch_en.isp[V2_STREAM_ID]) {
		if (isp_v2_ctx == NULL) {
			video_ch_state[V2_STREAM_ID] = VIDEO_CH_STARTING;
			if (video_v2_start() < 0) {
				video_ch_state[V2_STREAM_ID] = VIDEO_CH_ERROR;
				ret = -1;
			}
		} else if (video_isp_restart(V2_STREAM_ID, isp_v2_ctx) < 0) {
			ret = -1;
		}
	}
#if !defined(CONFIG_4K_DEV) || (CONFIG_4K_DEV != 1)
	if (v_ch_en.isp[V3_STREAM_ID]) {
		if (isp_v3_ctx == NULL) {
			video_ch_state[V3_STREAM_ID] = VIDEO_CH_STARTING;
			if (video_v3_start() < 0) {
				video_ch_state[V3_STREAM_ID] = VIDEO_CH_ERROR;
				ret = -1;
			}
		} else {
			if (mm_module_ctrl(isp_v3_ctx, CMD_ISP_STREAM_START, 0) < 0) {
				video_ch_state[V3_STREAM_ID] = VIDEO_CH_ERROR;
				ret = -1;
			} else {
				video_ch_state[V3_STREAM_ID] = VIDEO_CH_RUNNING;
			}
		}
	}
	if (v_ch_en.isp[V4_STREAM_ID]) {
		if (isp_v4_ctx == NULL) {
			video_ch_state[V4_STREAM_ID] = VIDEO_CH_STARTING;
			if (video_v4_start() < 0) {
				video_ch_state[V4_STREAM_ID] = VIDEO_CH_ERROR;
				ret = -1;
			}
		} else {
			if (mm_module_ctrl(isp_v4_ctx, CMD_ISP_STREAM_START, 0) < 0) {
				video_ch_state[V4_STREAM_ID] = VIDEO_CH_ERROR;
				ret = -1;
			} else {
				video_ch_state[V4_STREAM_ID] = VIDEO_CH_RUNNING;
			}
		}
	}
#endif
	if (v_ch_en.isp[V5_STREAM_ID]) {
		if (isp_v5_ctx == NULL) {
			video_ch_state[V5_STREAM_ID] = VIDEO_CH_STARTING;
			if (video_v5_start() < 0) {
				video_ch_state[V5_STREAM_ID] = VIDEO_CH_ERROR;
				ret = -1;
			}
		} else if (video_isp_restart(V5_STREAM_ID, isp_v5_ctx) < 0) {
			ret = -1;
		}
	}
	rtos_mutex_give(video_op_lock);
	return ret;
}

typedef struct video_start_request_s {
	char *user_cmd;
	int result;
	TaskHandle_t waiter;
} video_start_request_t;

static void video_stream_start_task(void *param)
{
	video_start_request_t *request = (video_start_request_t *)param;

	request->result = video_stream_start(request->user_cmd);
	xTaskNotifyGive(request->waiter);
	vTaskDelete(NULL);
}

static void video_stream_stop_task(void *param)
{
	int ch = (int)(uintptr_t)param;

	rtos_mutex_take(video_op_lock, portMAX_DELAY);

	/* A previous stop task owns any deferred teardown for this channel. */
	if (video_ch_state[ch] == VIDEO_CH_CLOSING) {
		goto error;
	}
	video_ch_state[ch] = VIDEO_CH_CLOSING;
	if (ch < VIDEO_ENC_VALID_CH && snapshot_stop(ch) < 0) {
		RTK_LOGW(TAG, "ch%d teardown deferred until AP releases snapshot\r\n", ch);
		do {
			rtos_mutex_give(video_op_lock);
			vTaskDelay(pdMS_TO_TICKS(10));
			rtos_mutex_take(video_op_lock, portMAX_DELAY);
		} while (snapshot_in_use(ch));
	}
	if (video_channel_deinit(ch) < 0) {
		video_ch_state[ch] = VIDEO_CH_ERROR;
		RTK_LOGE(TAG, "error: video_channel_deinit fail\r\n");
		goto error;
	}
#if defined(CONFIG_FAST_CAMERA_START) && (CONFIG_FAST_CAMERA_START != 0)
	fcs_attach_pending[ch] = 0;
#endif
	video_ch_state[ch] = VIDEO_CH_CLOSED;

error:
	rtos_mutex_give(video_op_lock);
	vTaskDelete(NULL);
}

int video_app_stream_stop(int ch)
{
	if (ch < 0 || ch >= VIDEO_ISP_VALID_CH) {
		RTK_LOGE(TAG, "error: invalid channel %d for stream close\r\n", ch);
		return -1;
	}
	/* Spawn background task so the IPC command handler returns immediately,
	 * freeing ipc_mutex[ch] before video_channel_deinit() blocks in
	 * vp_ipc_wait_frame_recycled() — which needs AP's BUF_RELEASE IPC
	 * command to use that same mutex to complete. */
	if (xTaskCreate(video_stream_stop_task, "vid_stop", 2048, (void *)(uintptr_t)ch, tskIDLE_PRIORITY + 5, NULL) != pdPASS) {
		RTK_LOGE(TAG, "error: failed to create stream stop task for ch%d\r\n", ch);
		return -1;
	}
	return 0;
}

int video_app_stream_start(char *user_cmd)
{
	video_start_request_t request = {
		.user_cmd = user_cmd,
		.result = -1,
		.waiter = xTaskGetCurrentTaskHandle(),
	};

	// DBG_MMF_INFO_MSG_OFF(0xffffffff);
	// DBG_MMF_DEBUG_MSG_ON(_MMF_DBG_ENCODER_ | _MMF_DBG_ISP_);
	// DBG_MMF_DEBUG_MSG_ON(_MMF_DBG_ENCODER_);

	/* video IP function & clock init */
	if (video_rcc_inited == 0) {
		video_app_common_rcc_init();
		video_rcc_inited = 1;
	}

	video_start_lock_ensure();
	if (video_op_lock == NULL) {
		return -1;
	}

	ulTaskNotifyTake(pdTRUE, 0);
	if (xTaskCreate(video_stream_start_task, ((const char *)"video_stream_start_task"), 2048, &request, tskIDLE_PRIORITY + 1, NULL) != pdPASS) {
		RTK_LOGE(TAG, "error: fail to create video_stream_start task\r\n");
		return -1;
	}
	ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
	return request.result;
}

#if defined(CONFIG_FAST_CAMERA_START) && (CONFIG_FAST_CAMERA_START != 0)
#include "video_fcs_config.h"
#include "video_fcs_params.h"
#include "isp_stream.h"
#include "video_app_fcs_stage1.h"

void video_fcs_stream_start_task(void *param)
{
	int ch;
	int failed = 0;
	(void)param;
	rtos_mutex_take(video_op_lock, portMAX_DELAY);

	enc_v1_prio = 5;
	enc_v2_prio = 4;
	enc_v3_prio = 3;
	if (isp_params[V1_STREAM_ID].slice_mode_en) {
		enc_v1_prio = 4;
		enc_v2_prio = 5;
	}

	for (ch = 0; ch < VIDEO_ISP_VALID_CH; ch++) {
		if (v_ch_en.isp[ch] && video_channel_start(ch) < 0) {
			video_ch_state[ch] = VIDEO_CH_ERROR;
			failed = 1;
			RTK_LOGE(TAG, "[VP FCS] ch%d start failed\r\n", ch);
		}
	}
	if (failed) {
		for (ch = 0; ch < VIDEO_ISP_VALID_CH; ch++) {
			if (v_ch_en.isp[ch]) {
				video_channel_deinit(ch);
				fcs_attach_pending[ch] = 0;
				video_ch_state[ch] = VIDEO_CH_ERROR;
			}
		}
		video_fcs_boot_state = VIDEO_FCS_BOOT_ERROR;
	}

	rtos_mutex_give(video_op_lock);
	vTaskDelete(NULL);
}

/*
 * User-configurable product policy hook for deciding whether this boot should
 * run FCS. Customers can provide a strong implementation with product-specific
 * conditions such as retained state, GPIO state, boot mode, or wake source.
 */
__weak bool fcs_user_config_should_run(void)
{
	return true;
}

/* fcs_isp_start_ch: set by video_app_fast_camera_start(), consumed by
 * video_fcs_isp_stage1_task() in video_app_fcs_stage1.c */
int fcs_isp_start_ch = -1;

/* Accessors for stage1.c to reach static state without breaking encapsulation. */
int video_app_fcs_get_start_ch(void)
{
	return fcs_isp_start_ch;
}

void video_app_fcs_set_ch_state(int ch, int state)
{
	if ((uint32_t)ch >= VIDEO_ISP_VALID_CH) {
		return;
	}
	video_start_lock_ensure();
	rtos_mutex_take(video_op_lock, portMAX_DELAY);
	video_ch_state[ch] = (video_ch_state_t)state;
	rtos_mutex_give(video_op_lock);
}

void video_app_fcs_set_boot_state(int state)
{
	video_start_lock_ensure();
	rtos_mutex_take(video_op_lock, portMAX_DELAY);
	video_fcs_boot_state = state;
	rtos_mutex_give(video_op_lock);
}

int video_app_fcs_isp_ch_enabled(int ch)
{
	if ((uint32_t)ch >= VIDEO_ISP_VALID_CH) {
		return 0;
	}
	return v_ch_en.isp[ch];
}

int video_app_fcs_get_isp_params(int ch, stream_params_t *out)
{
	const isp_params_t *p;
	if (!out || (uint32_t)ch >= VIDEO_ISP_VALID_CH) {
		return -1;
	}
	p = &isp_params[ch];
	out->ch       = (uint32_t)p->stream_id;
	out->width    = (uint32_t)p->width;
	out->height   = (uint32_t)p->height;
	out->fps      = (uint32_t)p->fps;
	out->slot_num = (uint32_t)p->slot_num;
	out->format   = (uint32_t)p->format;
	return 0;
}

void video_app_fast_camera_start(void)
{
	fcs_persistent_params_t *p = FCS_PARAMS_ADDR;
	fcs_persistent_params_t defaults;
	const fcs_persistent_params_t *start_params;
	int ch;

	/* Init semaphore here so it is ready before stage1 task is created.
	 * Safe to call before rtos_sched_start() since both tasks only run
	 * after the scheduler is started. */
	if (video_fcs_isp_stage1_sema_init() != 0) {
		RTK_LOGE(TAG, "error: video_fcs_isp_stage1_sema_init failed, abort fast camera start\r\n");
		video_fcs_boot_state = VIDEO_FCS_BOOT_ERROR;
		return;
	}

	DCache_Invalidate((u32)p, sizeof(fcs_persistent_params_t));
	if (!fcs_user_config_should_run()) {
		RTK_LOGI(TAG, "[VP FCS] product policy skipped fast camera start\r\n");
		video_fcs_boot_state = VIDEO_FCS_BOOT_NORMAL;
		return;
	}

	if (fcs_params_valid(p)) {
		start_params = p;
	} else {
		fcs_params_init_defaults(&defaults);
		defaults.crc = fcs_crc_compute(&defaults);
		start_params = &defaults;
		RTK_LOGI(TAG, "[VP FCS] invalid retained params -> use shared defaults\r\n");
	}

	if (video_app_fcs_params_load(start_params, &v_ch_en, isp_params, enc_params) != 0) {
		RTK_LOGE(TAG, "[VP FCS] params conflict, abort fast camera start\r\n");
		video_fcs_boot_state = VIDEO_FCS_BOOT_ERROR;
		return;
	}

	if (video_rcc_inited == 0) {
		video_app_common_rcc_init();
		video_rcc_inited = 1;
	}

	video_start_lock_ensure();
	fcs_isp_start_ch = -1;
	for (ch = 0; ch < VIDEO_ISP_VALID_CH; ch++) {
		if (v_ch_en.isp[ch]) {
			video_ch_state[ch] = VIDEO_CH_FCS_STARTING;
			if (fcs_isp_start_ch < 0) {
				fcs_isp_start_ch = ch;
			}
		}
	}
	if (fcs_isp_start_ch < 0) {
		RTK_LOGE(TAG, "error: no fcs isp channel\r\n");
		video_fcs_boot_state = VIDEO_FCS_BOOT_ERROR;
		return;
	}
	video_fcs_boot_state = VIDEO_FCS_BOOT_ACTIVE;
	if (rtos_task_create(NULL, "video_fcs_stage1", video_fcs_isp_stage1_task, NULL, 2048 * 4, 1) != RTK_SUCCESS) {
		RTK_LOGE(TAG, "error: fail to create video_fcs_stage1 task\r\n");
		for (ch = 0; ch < VIDEO_ISP_VALID_CH; ch++) {
			if (v_ch_en.isp[ch]) {
				video_ch_state[ch] = VIDEO_CH_ERROR;
			}
		}
		video_fcs_boot_state = VIDEO_FCS_BOOT_ERROR;
	}
}
#endif

static u32 cmd_enc_mem_info(u16 argc, u8 *argv[])
{
	(void)argc;
	(void)argv;

#if !defined(CONFIG_PXP) || (CONFIG_PXP != 1)
	extern void EncLogMemInfo(void);
	EncLogMemInfo();
#endif

	return TRUE;
}

CMD_TABLE_DATA_SECTION
COMMAND_TABLE __attribute__((unused)) vp_vapp_cmd_table[] = {
	{ "ENCMEMINFO", cmd_enc_mem_info },
};
