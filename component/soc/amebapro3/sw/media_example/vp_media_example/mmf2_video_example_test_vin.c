/******************************************************************************
*
* Copyright(c) 2007 - 2025 Realtek Corporation. All rights reserved.
*
* Ported from Zephyr test_vin.c to FreeRTOS:
*   - struct device / DEVICE_DT_GET replaced by plain channel index
*   - k_msleep / K_FOREVER replaced by vTaskDelay / polling loop
*   - rts_video_* replaced by rtscam_video_* (same semantics)
*   - RTS_MEMORY_KERNEL used to alloc from ISP page pool (not FreeRTOS heap)
*
******************************************************************************/

#include "diag.h"
#include "hal_platform.h"
#include <stdio.h>
#include <FreeRTOS.h>
#include <task.h>
#include <semphr.h>
#include "ameba_soc.h"
#include "hal_isp.h"
#include "isp_common.h"
#include "rts_camera.h"
#include "rts_video.h"
#include "rts_isp_tuning.h"
#include "rts_isp_sensor.h"
#include "video_app_rcc.h"
#include "rtsavisp.h"

#include "ainr_driver.h"

#include "vfs.h"

/* AINR model blobs — array name matches ISP full-frame size, not channel output.
 * 1920_1080 is not defined anywhere else, so include it here. The gc4023 and
 * os04a10 blobs are already defined by module_isp.c (always linked into the VP
 * image); re-including their .c would be a duplicate-symbol error, so declare
 * them extern instead. */
#include "ainr_model_1920_1080.c"
/* Sizes are hardcoded because sizeof() is unavailable on an extern incomplete
 * array; keep in sync with the model .c files if they are regenerated. */
extern unsigned char ainr_model_2560_1440_gc4023[];
#define AINR_MODEL_2560_1440_GC4023_SIZE 39528u
extern unsigned char ainr_model_2688_1520_os04a10[];
#define AINR_MODEL_2688_1520_OS04A10_SIZE 39560u

#define NUM_VIN_CH 1
#define TEST_VIN_BUFS 2

/* AINR (AI denoise) — off by default. Enable per-run with "video vin ainr 1"
 * BEFORE "video vin" (consumed once by test_vin_setup_task, like sensor mode). */
static int test_vin_ainr_en;

static ainr_ctx *test_vin_ainr;

/*
 * Enable AINR on the ISP full-frame (w x h = sensor size after ISP crop).
 * Sequence mirrors module_isp.c:isp_ai_denoise_start():
 *   open -> LOAD_MODEL -> SET_PARAM(pixfmt=0xE4, mode=0) -> ENABLE.
 * Model blob must match the frame resolution.
 */
static int test_vin_ainr_start(uint32_t w, uint32_t h)
{
	ainr_model_t model = {0};
	ainr_param_t param;
	int status;

	if (test_vin_ainr) {
		return 0; /* already started */
	}

	model.use_dma = 0;
	model.update_mixup_ratio = 0;
	if (w > 2560) {
		model.model_data = (const void *)ainr_model_2688_1520_os04a10;
		model.model_size = AINR_MODEL_2688_1520_OS04A10_SIZE;
	} else if (w > 1920) {
		model.model_data = (const void *)ainr_model_2560_1440_gc4023;
		model.model_size = AINR_MODEL_2560_1440_GC4023_SIZE;
	} else {
		model.model_data = (const void *)ainr_model_1920_1080;
		model.model_size = sizeof(ainr_model_1920_1080);
	}

	test_vin_ainr = ainr_driver_open();
	if (!test_vin_ainr) {
		DiagPrintf("[test_vin] ainr_driver_open fail\r\n");
		return -1;
	}

	status = ainr_driver_ctrl(test_vin_ainr, AINR_CTRL_CMD_LOAD_MODEL, &model);
	if (status != AINR_SUCCESS) {
		DiagPrintf("[test_vin] AINR LOAD_MODEL fail %d\r\n", status);
		goto fail;
	}

	param.width = w;
	param.height = h;
	param.pixfmt = 0xE4;
	param.mode = 0;
	status = ainr_driver_ctrl(test_vin_ainr, AINR_CTRL_CMD_SET_PARAM, &param);
	if (status != AINR_SUCCESS) {
		DiagPrintf("[test_vin] AINR SET_PARAM fail %d\r\n", status);
		goto fail;
	}

	status = ainr_driver_ctrl(test_vin_ainr, AINR_CTRL_CMD_ENABLE, NULL);
	if (status != AINR_SUCCESS) {
		DiagPrintf("[test_vin] AINR ENABLE fail %d\r\n", status);
		goto fail;
	}

	DiagPrintf("[test_vin] AINR enabled %ux%u\r\n", w, h);
	return 0;

fail:
	ainr_driver_close(test_vin_ainr);
	test_vin_ainr = NULL;
	return -1;
}

/*
 * Zoom-up (upscale) test parameters for ch0.
 *
 * Ch0 is the only zoom-up capable channel (zoom_in_enable = true).
 * The zoom hardware upscales the crop region to the output format.
 *
 * ISP full-sensor frame: 2688x1520
 * Crop a 512x288 center region → upscale to 1280x720 = 2.5x (hardware maximum)
 *
 * Scale register (Q10): scale = (src_w << 10) / dest_w
 *   = (512 << 10) / 1280 = 409 < 1024 → upscale confirmed
 *
 * Channels 1-4 are zoom-down only (scale < 0x400 rejected by HW).
 */
#define ZOOM_UP_ISP_W 2688 /* full ISP frame width */
#define ZOOM_UP_ISP_H 1944 /* full ISP frame height */
#define ZOOM_UP_CROP_W 2688 /* source crop width  (center of ISP frame) */
#define ZOOM_UP_CROP_H 1944 /* source crop height (center of ISP frame) */

typedef struct {
	int ch;
	int width;
	int height;
	int fps;
	uint32_t pixelformat; /* V4L2_PIX_FMT_* */
	struct rts_video_buffer *bufs[TEST_VIN_BUFS];
	int active; /* 1 after successful setup + stream_start */
	uint32_t frame_count; /* frames dequeued within the current fps window */
	uint32_t fps_measured; /* frames dequeued during the last 1s window */
	TickType_t fps_win_start; /* tick at which the current fps window began */
	volatile uint8_t capture_request; /* set by CLI "vin capture", cleared once written to SD */
	volatile uint8_t ycol_request; /* set by CLI "vin ycol", cleared once stats printed */
} vin_ch_ctx_t;

static SemaphoreHandle_t sd_mutex;

/* Frame-by-frame dequeue log — off by default, toggle via CLI "vin log_en" */
static volatile uint8_t test_vin_log_en;

/* HW init state — written by test_vin_ensure_hw_init() */
static uint8_t test_vin_hw_inited; /* 1 = init attempted */
static uint8_t test_vin_hw_ok; /* 1 = init succeeded */

/* Sensor mode requested via CLI "vin mode <n>"; -1 = none requested, run
 * with whatever mode the sensor driver defaults to. Consumed once, by
 * test_vin_setup_task, before any channel's fps is set — rts_av_isp_set_sensor_mode()
 * is rejected once a channel is streaming. */
static int test_vin_requested_mode = -1;

/* Optional filename postfix set by CLI "vin capture <post_fix>"; empty means
 * no postfix (default filenames unchanged). */
static char test_vin_capture_postfix[32];

/*
 * Dynamic IQ slot reserved by task_mcu.c's register_sensor_iq() at boot
 * (RTS_ISP_PLUG_IQ_ID1, unbound). CLI "vin load_iq" writes a packed IQ file
 * straight into it, then binds it — RTS_ISP_PLUG_IQ_ID0 (the compiled-in
 * default) is never touched. Size must match IQ_TABLE_DYNAMIC_SIZE in
 * component/soc/amebapro3/video/isp/driver/apps/task_mcu.c.
 */
#define TEST_VIN_IQ_DYNAMIC_SIZE (128 * 1024)
extern uint8_t __isp_iq_table_start__[TEST_VIN_IQ_DYNAMIC_SIZE];

/*
 * bind_plugin()'s ISP_NOTIFY_PLUGIN_UPDATE path runs isp_iq_reload()
 * synchronously in the calling task, which recurses deeply through nanopb
 * pb_decode/pb_release on isp_iq_table_t. task_mcu.c's own isp_task reserves
 * configMINIMAL_STACK_SIZE*16 (8192 words = 32KB) for exactly this; the
 * console command task's stack is nowhere near that, so run it on a
 * dedicated task with a matching stack instead of the caller's.
 */
#define TEST_VIN_IQ_SWAP_STACK_BYTES (32 * 1024)

typedef struct {
	int ret_bind;
	int ret_reload;
	SemaphoreHandle_t done;
} iq_swap_result_t;

static void test_vin_iq_swap_task(void *arg)
{
	iq_swap_result_t *result = (iq_swap_result_t *)arg;

	result->ret_bind = rts_av_isp_bind_plugin(ISP0, RTS_ISP_PLUG_IQ_ID1);
	result->ret_reload = result->ret_bind ? 0 : rts_isp_tuning_reload_packed_iq(ISP0);
	xSemaphoreGive(result->done);
	vTaskDelete(NULL);
}

/*
 * Ch0 output is 1280x720: the zoom-up HW upscales the 512x288 crop to fill it.
 * Ch1-3 are YUV420 (NV12) downscale streams from the full ISP frame.
 * Ch4 is the AI channel — supports both Y8I (Y-only) and RGB24.
 *   Y8I  → ZOOM_SYS_IMAGE_AI_CHAN_SEL = 0  (CONFIG_RTS_ZOOM_S4_FMT bit 0x08)
 *   RGB24 → ZOOM_SYS_IMAGE_AI_CHAN_SEL = 1  (CONFIG_RTS_ZOOM_S4_FMT bit 0x04)
 *   NV12 (0x01) is NOT valid for this channel and triggers HW DDR overflow.
 */
static vin_ch_ctx_t vin_ch_ctxs[NUM_VIN_CH] = {
	{ .ch = 0, .width = 1280, .height = 720, .fps = 5, .pixelformat = V4L2_PIX_FMT_NV12 }, /* zoom-up */
	// { .ch = 1, .width = 1280, .height = 720,  .fps = 5, .pixelformat = V4L2_PIX_FMT_NV12 },
	// { .ch = 2, .width = 640,  .height = 480,  .fps = 5, .pixelformat = V4L2_PIX_FMT_NV12 },
	// { .ch = 3, .width = 80,  .height = 60,  .fps = 5, .pixelformat = V4L2_PIX_FMT_NV12 },
	// { .ch = 4, .width = 160,  .height = 120,  .fps = 5, .pixelformat = V4L2_PIX_FMT_RGB24}, /* AI/Y-only */
};

static void write_to_sd(void *data, uint32_t size, char *filename)
{
	int ret = 0;
	FILE *finfo;
	char filepath[64];

	xSemaphoreTake(sd_mutex, portMAX_DELAY);

	vfs_init();
	ret = vfs_target_register("fat", VFS_TARGET_SD, VFS_RW);
	if (ret != 0) {
		DiagPrintf("[%s] VFS-FAT Init Fail\r\n", __FUNCTION__);
		goto exit;
	}

	snprintf(filepath, sizeof(filepath), "fat:%s", filename);
	finfo = fopen(filepath, "w");
	if (finfo == NULL) {
		DiagPrintf("[%s] fopen(%s) failed\r\n", __FUNCTION__, filepath);
		goto exit;
	}

	ret = fwrite(data, 1, size, finfo);
	if (ret != (int)size) {
		DiagPrintf("[%s] fwrite() failed, wrote %d of %u bytes\r\n", __FUNCTION__, ret, size);
	} else {
		DiagPrintf("[%s] fwrite() succeeded, wrote %u bytes to %s\r\n", __FUNCTION__, size, filepath);
	}

	fclose(finfo);

exit:
	vfs_target_unregister("fat");
	xSemaphoreGive(sd_mutex);
}

/*
 * Sample the pixel at x=1 (second column) of every row and report
 * max/min/avg. Ch0-3 (NV16) read the Y byte directly from the first
 * width*height bytes of the buffer. Ch4 (RGB24) is planar — R plane at
 * offset 0, G plane at offset w*h, B plane at offset 2*w*h (confirmed by
 * rtscam_soc_set_frame_addr()'s separate per-plane HW address registers) —
 * so each row's R/G/B samples are converted to Y via integer fixed-point:
 * y = (299*R + 587*G + 114*B) / 1000.
 */
static void test_vin_ycol_report(vin_ch_ctx_t *ctx, struct rts_video_buffer *vbuf)
{
	uint8_t *base = (uint8_t *)vbuf->vm_addr;
	uint32_t w = (uint32_t)ctx->width;
	uint32_t h = (uint32_t)ctx->height;
	uint32_t plane_sz = w * h;
	int max = -1, min = 256, sum = 0;
	uint32_t r, y;

	for (r = 0; r < h; r++) {
		if (ctx->pixelformat == V4L2_PIX_FMT_RGB24) {
			uint8_t rr = base[r * w + 1];
			uint8_t gg = base[plane_sz + r * w + 1];
			uint8_t bb = base[2 * plane_sz + r * w + 1];
			y = (299u * rr + 587u * gg + 114u * bb) / 1000u;
		} else {
			y = base[r * w + 1];
		}
		if ((int)y > max) {
			max = (int)y;
		}
		if ((int)y < min) {
			min = (int)y;
		}
		sum += (int)y;
	}

	DiagPrintf("[test_vin] ch%d ycol: max=%d min=%d avg=%d\r\n", ctx->ch, max, min, h ? (sum / (int)h) : 0);
}

static void test_vin_dequeue_task(void *arg)
{
	vin_ch_ctx_t *ctx = (vin_ch_ctx_t *)arg;
	struct rts_video_buffer *vbuf;
	unsigned int frame = 0;
	char filename[32];
	TickType_t now;

	ctx->fps_win_start = xTaskGetTickCount();

	while (1) {
		while (rtscam_video_dequeue(ctx->ch, &vbuf) != 0) {
			vTaskDelay(pdMS_TO_TICKS(1));
		}

		if (test_vin_log_en) {
			DiagPrintf("[test_vin] ch%d frame %u  size=%u  phy=0x%08x  ts=%llu\r\n", ctx->ch, frame, vbuf->bytesused, vbuf->phy_addr,
					   (unsigned long long)vbuf->timestamp);
		}
		frame++;

		ctx->frame_count++;
		now = xTaskGetTickCount();
		if ((now - ctx->fps_win_start) >= pdMS_TO_TICKS(1000)) {
			ctx->fps_measured = ctx->frame_count;
			ctx->frame_count = 0;
			ctx->fps_win_start = now;
		}

		if (ctx->ycol_request) {
			test_vin_ycol_report(ctx, vbuf);
			ctx->ycol_request = 0;
		}

		if (ctx->capture_request) {
			const char *ext = (ctx->pixelformat == V4L2_PIX_FMT_Y8I) ? "y" : (ctx->pixelformat == V4L2_PIX_FMT_RGB24) ? "rgb" : "nv12";
			if (test_vin_capture_postfix[0]) {
				snprintf(filename, sizeof(filename), "ch%d_%s.%s", ctx->ch, test_vin_capture_postfix, ext);
			} else {
				snprintf(filename, sizeof(filename), "ch%d.%s", ctx->ch, ext);
			}
			write_to_sd(vbuf->vm_addr, vbuf->bytesused, filename);
			ctx->capture_request = 0;
		}

		rtscam_video_enqueue(ctx->ch, vbuf);
	}
}

/*
 * Single setup task: open + configure + buffer-enqueue all channels
 * sequentially so the ISP pipeline sees all output streams before
 * stream_start fires.  Dequeue tasks are spawned afterward.
 */
static void test_vin_setup_task(void *arg)
{
	struct rts_video_format fmt;
	struct rts_video_fps fps;
	struct rts_video_caps caps;
	static rtos_task_t dq_tasks[NUM_VIN_CH];
	char task_name[16];
	size_t bsize;
	int i, j;

	(void)arg;

	/* Wait for ISP server to be running */
	while (rts_av_isp_get_status() < RTS_ISP_RUNNING) {
		vTaskDelay(pdMS_TO_TICKS(10));
	}

	/* Apply any requested sensor mode now — before Phase 1 sets fps on
	 * ch0 below, since rts_av_isp_set_sensor_mode() is rejected with
	 * -RTS_ISP_EBUSY once any channel's fps has been set. */
	if (test_vin_requested_mode >= 0) {
		struct rts_isp_sensor_modes modes;
		uint8_t target_hdr;
		uint8_t target_hdr_variant;
		uint32_t m;
		int ret;

		ret = rts_av_isp_enum_sensor_modes(0, &modes);
		if (ret) {
			DiagPrintf("[test_vin] enum_sensor_modes fail %d\r\n", ret);
		} else {
			/* test_vin_requested_mode
			 * 0 -> Linear, 1 -> VC, 2 -> dummy, 3 -> no dummy, 4-> FID
			 */
			switch (test_vin_requested_mode) {
			case 0:
				target_hdr = RTS_ISP_HDR_NONE;
				target_hdr_variant = RTS_ISP_HDR_VAR_INVALID;
				break;
			case 1:
				target_hdr = RTS_ISP_HDR_LINE_2TO1;
				target_hdr_variant = RTS_ISP_HDR_VAR_VC;
				break;
			case 2:
				target_hdr = RTS_ISP_HDR_LINE_2TO1;
				target_hdr_variant = RTS_ISP_HDR_VAR_DUMMY;
				break;
			case 3:
				target_hdr = RTS_ISP_HDR_LINE_2TO1;
				target_hdr_variant = RTS_ISP_HDR_VAR_NODUMMY;
				break;
			case 4:
				target_hdr = RTS_ISP_HDR_LINE_2TO1;
				target_hdr_variant = RTS_ISP_HDR_VAR_FID;
				break;
			default:
				target_hdr = RTS_ISP_HDR_NONE;
				target_hdr_variant = 0;
				break;
			}
			for (m = 0; m < modes.num; m++) {
				if (modes.mode[m].hdr == target_hdr && modes.mode[m].hdr_variant == target_hdr_variant) {
					break;
				}
			}
			if (m == modes.num) {
				DiagPrintf("[test_vin] sensor has no mode matching hdr=0x%x hdr_variant=0x%x\r\n", (unsigned)target_hdr, (unsigned)target_hdr_variant);
			} else {
				ret = rts_av_isp_set_sensor_mode(0, &modes.mode[m]);
				if (ret) {
					DiagPrintf("[test_vin] set_sensor_mode fail %d\r\n", ret);
				} else {
					DiagPrintf("[test_vin] sensor mode set to %d\r\n", test_vin_requested_mode);
				}
			}
		}
		test_vin_requested_mode = -1;
	}

	/* Phase 1: open, configure, and enqueue buffers on every channel */
	for (i = 0; i < NUM_VIN_CH; i++) {
		vin_ch_ctx_t *ctx = &vin_ch_ctxs[i];

		if (rtscam_video_open(ctx->ch)) {
			DiagPrintf("[test_vin] open ch%d fail\r\n", ctx->ch);
			continue;
		}

		if (rtscam_video_get_caps(ctx->ch, &caps) == 0) {
			int idx = 0;
			DiagPrintf("[test_vin] ch%d capabilities:\r\n", ctx->ch);
			while (caps.format_caps[idx].pixelformat) {
				const struct rts_video_format_cap *f = &caps.format_caps[idx];
				DiagPrintf("  %c%c%c%c width [%u; %u; %u] height [%u; %u; %u] fps [%u; %u; %u]\r\n", (char)f->pixelformat, (char)(f->pixelformat >> 8),
						   (char)(f->pixelformat >> 16), (char)(f->pixelformat >> 24), f->width_min, f->width_max, f->width_step, f->height_min, f->height_max,
						   f->height_step, f->fps_min.denominator / f->fps_min.numerator, f->fps_max.denominator / f->fps_max.numerator,
						   f->fps_step.denominator / f->fps_step.numerator);
				idx++;
			}
		}

		memset(&fmt, 0, sizeof(fmt));
		fmt.pixelformat = ctx->pixelformat;
		fmt.width = ctx->width;
		fmt.height = ctx->height;
		if (rtscam_video_set_format(ctx->ch, &fmt)) {
			DiagPrintf("[test_vin] ch%d set_format fail\r\n", ctx->ch);
			rtscam_video_close(ctx->ch);
			continue;
		}

		/* Ch0 zoom-up: crop a small center region, let HW upscale to output */
		if (ctx->ch == 0) {
			struct rts_video_selection sel = {
				.left = (ZOOM_UP_ISP_W - ZOOM_UP_CROP_W) / 2,
				.top = (ZOOM_UP_ISP_H - ZOOM_UP_CROP_H) / 2,
				.width = ZOOM_UP_CROP_W,
				.height = ZOOM_UP_CROP_H,
			};
			if (rtscam_video_set_selection(ctx->ch, &sel) == 0) {
				DiagPrintf("[test_vin] ch0 zoom-up: crop %ux%u@(%u,%u) -> %ux%u (2.5x)\r\n", ZOOM_UP_CROP_W, ZOOM_UP_CROP_H, sel.left, sel.top, ctx->width,
						   ctx->height);
			} else {
				DiagPrintf("[test_vin] ch0 set_selection fail — running at 1:1\r\n");
			}
		}

		memset(&fps, 0, sizeof(fps));
		fps.numerator = 1;
		fps.denominator = ctx->fps;
		if (rtscam_video_set_fps(ctx->ch, &fps)) {
			DiagPrintf("[test_vin] ch%d set_fps fail (non-fatal)\r\n", ctx->ch);
		}

		/* Buffer size depends on format:
		 *   NV12/NV16: stride × height × bpp/8 via standard formula
		 *   Y8I (Y-only): 1 byte/pixel
		 *   RGB24: 3 bytes/pixel (3 planes: R, G, B, each w×h bytes) */
		if (ctx->pixelformat == V4L2_PIX_FMT_Y8I) {
			bsize = (size_t)ctx->width * ctx->height;
		} else if (ctx->pixelformat == V4L2_PIX_FMT_RGB24) {
			bsize = (size_t)ctx->width * ctx->height * 3;
		} else if (ctx->pixelformat == V4L2_PIX_FMT_NV16) {
			bsize = (size_t)ctx->width * ctx->height * 2;
		} else {
			bsize = (size_t)ctx->width * ctx->height * 3 / 2; /* NV12 */
		}
		bsize = (bsize + 15) & ~15U;

		for (j = 0; j < TEST_VIN_BUFS; j++) {
			ctx->bufs[j] = rtscam_video_buffer_alloc(ctx->ch, RTS_MEMORY_KERNEL, bsize);
			if (!ctx->bufs[j]) {
				DiagPrintf("[test_vin] ch%d buf_alloc[%d] fail\r\n", ctx->ch, j);
				goto buf_fail;
			}
			if (rtscam_video_enqueue(ctx->ch, ctx->bufs[j])) {
				DiagPrintf("[test_vin] ch%d enqueue[%d] fail\r\n", ctx->ch, j);
				goto buf_fail;
			}
			DiagPrintf("[test_vin] ch%d buf[%d] phy=0x%08x\r\n", ctx->ch, j, ctx->bufs[j]->phy_addr);
		}
		ctx->active = 1;
		continue;

buf_fail:
		for (j = 0; j < TEST_VIN_BUFS; j++) {
			if (ctx->bufs[j]) {
				rtscam_video_buffer_release(ctx->ch, ctx->bufs[j]);
				ctx->bufs[j] = NULL;
			}
		}
		rtscam_video_close(ctx->ch);
	}

	/* Enable AINR on the ISP full-frame before streaming. Size comes from the
	 * active sensor mode (mode[0] == current after any set_sensor_mode above). */
	if (test_vin_ainr_en) {
		struct rts_isp_sensor_modes ainr_modes;
		if (rts_av_isp_enum_sensor_modes(0, &ainr_modes) == 0 && ainr_modes.num > 0) {
			test_vin_ainr_start((uint32_t)ainr_modes.mode[0].size.w, (uint32_t)ainr_modes.mode[0].size.h);
		} else {
			DiagPrintf("[test_vin] AINR: enum_sensor_modes fail, skip\r\n");
		}
	}

	/* Phase 2: start streaming on all prepared channels */
	for (i = 0; i < NUM_VIN_CH; i++) {
		vin_ch_ctx_t *ctx = &vin_ch_ctxs[i];
		if (!ctx->active) {
			continue;
		}
		if (rtscam_video_stream_start(ctx->ch)) {
			DiagPrintf("[test_vin] ch%d stream_start fail\r\n", ctx->ch);
			ctx->active = 0;
		} else {
			DiagPrintf("[test_vin] ch%d capture started — %ux%u %s @%dfps\r\n", ctx->ch, ctx->width, ctx->height,
					   (ctx->pixelformat == V4L2_PIX_FMT_Y8I)	? "Y8I" :
					   (ctx->pixelformat == V4L2_PIX_FMT_RGB24) ? "RGB24" :
					   "NV12",
					   ctx->fps);
		}
	}

	/* Phase 3: spawn a dequeue task for each active channel */
	for (i = 0; i < NUM_VIN_CH; i++) {
		if (!vin_ch_ctxs[i].active) {
			continue;
		}
		snprintf(task_name, sizeof(task_name), "test_vin_dq%d", i);
		rtos_task_create(&dq_tasks[i], task_name, test_vin_dequeue_task, (void *)&vin_ch_ctxs[i], 4096, 1);
	}

	vTaskDelete(NULL);
}

/*
 * One-time HW bring-up: RCC clocks, ISP_Init, ISR registration via
 * isp_initial(), and rtscam_video_init().  Guarded by test_vin_hw_inited so
 * it is safe to call from both mmf2_video_example_test_vin_init() and the
 * stream_test task (which may run without the "vin" command having been
 * issued first).
 *
 * On success test_vin_hw_ok is set to 1; on failure it stays 0 and the
 * caller can check it.
 */
static void test_vin_ensure_hw_init(void)
{
	static struct isp_io io = { 0 };

	if (test_vin_hw_inited) {
		return;
	}
	test_vin_hw_inited = 1;

	video_app_common_rcc_init();
	ISP_Init();

	/* Register rtscam_soc_isr — without this the ISP interrupt never fires
	 * and rtscam_video_dequeue() will never return a completed frame. */
	if (isp_initial(&io)) {
		DiagPrintf("[test_vin] isp_initial fail\r\n");
		return;
	}

	rtscam_video_init();
	test_vin_hw_ok = 1;
}

void mmf2_video_example_test_vin_init(char *user_cmd)
{
	static rtos_task_t setup_task;

	(void)user_cmd;

	sd_mutex = xSemaphoreCreateMutex();

	test_vin_ensure_hw_init();
	if (!test_vin_hw_ok) {
		return;
	}

	rtos_task_create(&setup_task, "test_vin_setup", test_vin_setup_task, NULL, 4096, 1);
}

/*
 * Select a sensor mode (0=linear, 1=MIPI_HDR_VC, 2=MIPI_HDR_DUMMY,
 * 3=MIPI_HDR_NODUMMY 4=FID) and start the stream in that mode. The actual
 * rts_av_isp_set_sensor_mode() call is deferred to test_vin_setup_task,
 * right before it sets fps on the first channel — matching the existing
 * ISP_SetSensorMode()/ISP_StreamOnByChannel() pattern in hal_isp.c, since
 * the ISP subsystem finishes its own boot sequence (registering the sensor
 * and calling rts_av_isp_start()) well before this command can run, and
 * set_sensor_mode is only valid before any channel's fps is set.
 */
void mmf2_video_example_test_vin_set_mode(int mode, char *user_cmd)
{
	if (mode < 0 || mode > 4) {
		DiagPrintf("[test_vin] invalid mode %d (expect 0-4)\r\n", mode);
		return;
	}

	test_vin_requested_mode = mode;

	mmf2_video_example_test_vin_init(user_cmd);
}

/*
 * Set the AINR-enable flag for the next "video vin" run. Consumed once by
 * test_vin_setup_task (just before stream start). Call BEFORE "video vin".
 */
void mmf2_video_example_test_vin_set_ainr_en(int enable)
{
	test_vin_ainr_en = enable ? 1 : 0;
	DiagPrintf("[test_vin] AINR %s for next run\r\n", test_vin_ainr_en ? "ENABLED" : "disabled");
}

/*
 * Sensor I2C register access via the ISP server.
 *
 * rts_isp_read/write_sensor_reg() -> sensor_get_i2c_info() -> <sensor>_get_info()
 * puts a large struct rts_isp_sensor_info (~hundreds of bytes: 8-entry modes
 * array + i2c + power + focus) on the stack AND some sensor get_info() handlers
 * (e.g. os04a10) call printf(). The console command task (LOGUART_TASK) runs on
 * only SHELL_STACK_SIZE (1024 words = 4KB on VP), so calling this chain inline
 * overflows its stack (UFSR STKOF -> hard fault). Run it on a dedicated task
 * with a matching stack instead of the caller's — same approach as the IQ-swap
 * path above.
 */
#define TEST_VIN_I2C_STACK_BYTES (8 * 1024)

typedef struct {
	int is_write;
	uint16_t addr;
	uint16_t val;
	int ret;
	uint16_t rdata;
	SemaphoreHandle_t done;
} i2c_req_t;

static void test_vin_i2c_task(void *arg)
{
	i2c_req_t *req = (i2c_req_t *)arg;
	struct rts_isp_i2c_reg reg = {0};

	reg.addr = req->addr;
	if (req->is_write) {
		reg.data = req->val;
		req->ret = rts_isp_write_sensor_reg(ISP0, &reg);
	} else {
		req->ret = rts_isp_read_sensor_reg(ISP0, &reg);
		req->rdata = reg.data;
	}
	xSemaphoreGive(req->done);
	vTaskDelete(NULL);
}

/* Run a sensor-reg access on a dedicated task and wait for the result.
 * Returns the driver return code, or -1 if the task/sema could not be set up. */
static int test_vin_i2c_run(i2c_req_t *req)
{
	rtos_task_t i2c_task;

	req->done = xSemaphoreCreateBinary();
	if (!req->done) {
		DiagPrintf("[test_vin] i2c: sema create fail\r\n");
		return -1;
	}
	if (rtos_task_create(&i2c_task, "test_vin_i2c", test_vin_i2c_task, req, TEST_VIN_I2C_STACK_BYTES, 1)) {
		DiagPrintf("[test_vin] i2c: task create fail\r\n");
		vSemaphoreDelete(req->done);
		req->done = NULL;
		return -1;
	}
	xSemaphoreTake(req->done, portMAX_DELAY);
	vSemaphoreDelete(req->done);
	req->done = NULL;
	return req->ret;
}

/*
 * Read a sensor register over I2C via the ISP server.
 * addr is the 16-bit sensor register address; data is 8-bit on most sensors.
 */
void mmf2_video_example_test_vin_read_i2c(uint16_t addr)
{
	i2c_req_t req = {0};

	req.is_write = 0;
	req.addr = addr;
	if (test_vin_i2c_run(&req) != 0) {
		DiagPrintf("[test_vin] read_i2c 0x%04x fail %d\r\n", addr, req.ret);
		return;
	}
	DiagPrintf("[test_vin] read_i2c [0x%04x] = 0x%04x\r\n", addr, req.rdata);
}

void mmf2_video_example_test_vin_write_i2c(uint16_t addr, uint16_t val)
{
	i2c_req_t req = {0};

	req.is_write = 1;
	req.addr = addr;
	req.val = val;
	if (test_vin_i2c_run(&req) != 0) {
		DiagPrintf("[test_vin] write_i2c [0x%04x] = 0x%04x fail %d\r\n", addr, val, req.ret);
		return;
	}
	DiagPrintf("[test_vin] write_i2c [0x%04x] = 0x%04x\r\n", addr, val);
}

void mmf2_video_example_test_vin_set_log_en(int enable)
{
	test_vin_log_en = enable ? 1 : 0;
	DiagPrintf("[test_vin] frame log %s\r\n", test_vin_log_en ? "enabled" : "disabled");
}

int mmf2_video_example_test_vin_get_log_en(void)
{
	return test_vin_log_en;
}

void mmf2_video_example_test_vin_ensure_hw(void)
{
	test_vin_ensure_hw_init();
}

////// Oscar comment
//extern uint8_t zoom_linespace;
void mmf2_video_example_test_vin_set_zoom_linespace(uint8_t ls)
{
//	zoom_linespace = ls;
//	DiagPrintf("[test_vin] Set zoom line space to %d\r\n", ls);
}
////// Oscar comment

void mmf2_video_example_test_vin_show_strm_info(void)
{
	int i;

	for (i = 0; i < NUM_VIN_CH; i++) {
		vin_ch_ctx_t *ctx = &vin_ch_ctxs[i];

		if (!ctx->active) {
			continue;
		}
		DiagPrintf("[test_vin] ch%d fps=%u\r\n", ctx->ch, ctx->fps_measured);
	}
}

void mmf2_video_example_test_vin_capture(const char *postfix)
{
	int i;

	if (postfix && postfix[0]) {
		snprintf(test_vin_capture_postfix, sizeof(test_vin_capture_postfix), "%s", postfix);
	} else {
		test_vin_capture_postfix[0] = '\0';
	}

	for (i = 0; i < NUM_VIN_CH; i++) {
		vin_ch_ctx_t *ctx = &vin_ch_ctxs[i];

		if (!ctx->active) {
			continue;
		}
		ctx->capture_request = 1;
	}
	if (test_vin_capture_postfix[0]) {
		DiagPrintf("[test_vin] capture requested for all active channels (postfix=%s)\r\n", test_vin_capture_postfix);
	} else {
		DiagPrintf("[test_vin] capture requested for all active channels\r\n");
	}
}

void mmf2_video_example_test_vin_ycol(void)
{
	int i;

	for (i = 0; i < NUM_VIN_CH; i++) {
		vin_ch_ctx_t *ctx = &vin_ch_ctxs[i];

		if (!ctx->active) {
			continue;
		}
		ctx->ycol_request = 1;
	}
	DiagPrintf("[test_vin] ycol requested for all active channels\r\n");
}

void mmf2_video_example_test_vin_load_iq(const char *filename)
{
	char filepath[64];
	FILE *fp;
	long fsize;
	size_t rd;
	int ret;

	if (!filename || !filename[0]) {
		DiagPrintf("[test_vin] usage: video vin load_iq <file>\r\n");
		return;
	}

	xSemaphoreTake(sd_mutex, portMAX_DELAY);

	vfs_init();
	ret = vfs_target_register("fat", VFS_TARGET_SD, VFS_RW);
	if (ret != 0) {
		DiagPrintf("[test_vin] VFS-FAT init fail\r\n");
		xSemaphoreGive(sd_mutex);
		return;
	}

	snprintf(filepath, sizeof(filepath), "fat:%s", filename);
	fp = fopen(filepath, "r");
	if (!fp) {
		DiagPrintf("[test_vin] fopen(%s) failed\r\n", filepath);
		vfs_target_unregister("fat");
		xSemaphoreGive(sd_mutex);
		return;
	}

	fseek(fp, 0, SEEK_END);
	fsize = ftell(fp);
	fseek(fp, 0, SEEK_SET);
	if (fsize <= 0 || fsize > TEST_VIN_IQ_DYNAMIC_SIZE) {
		DiagPrintf("[test_vin] %s bad size %ld (max %d)\r\n", filepath, fsize, TEST_VIN_IQ_DYNAMIC_SIZE);
		fclose(fp);
		vfs_target_unregister("fat");
		xSemaphoreGive(sd_mutex);
		return;
	}

	rd = fread(__isp_iq_table_start__, 1, fsize, fp);
	fclose(fp);
	vfs_target_unregister("fat");
	xSemaphoreGive(sd_mutex);

	if (rd != (size_t)fsize) {
		DiagPrintf("[test_vin] %s short read %u/%ld\r\n", filepath, (unsigned)rd, fsize);
		return;
	}

	/* iq_table_dynamic (RTS_ISP_PLUG_IQ_ID1) is already registered by
	 * task_mcu.c's register_sensor_iq() — just bind it (auto-unbinds ID0)
	 * and have the ISP reparse it. No register/unregister at runtime.
	 * Done on a dedicated task: see test_vin_iq_swap_task's stack comment. */
	{
		iq_swap_result_t result = { 0 };
		rtos_task_t swap_task;

		result.done = xSemaphoreCreateBinary();
		if (!result.done) {
			DiagPrintf("[test_vin] semaphore alloc fail\r\n");
			return;
		}
		if (rtos_task_create(&swap_task, "test_vin_iq_swap", test_vin_iq_swap_task, &result, TEST_VIN_IQ_SWAP_STACK_BYTES, 1)) {
			DiagPrintf("[test_vin] swap task create fail\r\n");
			vSemaphoreDelete(result.done);
			return;
		}
		xSemaphoreTake(result.done, portMAX_DELAY);
		vSemaphoreDelete(result.done);

		if (result.ret_bind) {
			DiagPrintf("[test_vin] bind_plugin fail ret=%d\r\n", result.ret_bind);
			return;
		}
		if (result.ret_reload) {
			DiagPrintf("[test_vin] reload_packed_iq fail ret=%d\r\n", result.ret_reload);
			return;
		}
	}

	DiagPrintf("[test_vin] loaded packed IQ from %s (%ld bytes) into IQ slot 1\r\n", filepath, fsize);
}

/* -------------------------------------------------------------------------
 * Stream open/close stress test
 * -------------------------------------------------------------------------
 * Repeatedly opens a channel, streams for hold_ms, then tears it down
 * cleanly.  Tracks frame counts and free-heap per cycle to surface leaks
 * or open/close failures.
 *
 * Usage:  VIDEO stream_test [ch] [cycles] [hold_ms]
 *   ch       : index into vin_ch_ctxs[] (default 0)
 *   cycles   : number of open/close cycles; <= 0 = run forever (default 10)
 *   hold_ms  : milliseconds to stream per cycle (default 1000)
 * -------------------------------------------------------------------------
 */

typedef struct {
	int ch;
	int cycles;
	int hold_ms;
} stream_test_params_t;

static void stream_test_task(void *arg)
{
	stream_test_params_t *p = (stream_test_params_t *)arg;
	int ch = p->ch;
	int cycles = p->cycles;
	int hold_ms = p->hold_ms;
	vin_ch_ctx_t *ctx;
	struct rts_video_format fmt;
	struct rts_video_fps fps;
	struct rts_video_buffer *local_bufs[TEST_VIN_BUFS];
	int iter;
	int j;
	int fail_count = 0;
	unsigned int heap0, heap_prev, heap_cycle;
	int heap_delta;
	size_t bsize;
	int opened, started, cycle_ok, frames;

	vPortFree(p);
	p = NULL;

	ctx = &vin_ch_ctxs[ch];

	/* Bring up HW if not already done (e.g., test runs before "vin" cmd) */
	test_vin_ensure_hw_init();

	/* Wait for ISP server to be ready */
	while (rts_av_isp_get_status() < RTS_ISP_RUNNING) {
		vTaskDelay(pdMS_TO_TICKS(10));
	}

	/* Buffer size — exact same formula as test_vin_setup_task */
	if (ctx->pixelformat == V4L2_PIX_FMT_Y8I) {
		bsize = (size_t)ctx->width * ctx->height;
	} else if (ctx->pixelformat == V4L2_PIX_FMT_RGB24) {
		bsize = (size_t)ctx->width * ctx->height * 3;
	} else if (ctx->pixelformat == V4L2_PIX_FMT_NV16) {
		bsize = (size_t)ctx->width * ctx->height * 2;
	} else {
		bsize = (size_t)ctx->width * ctx->height * 3 / 2; /* NV12 */
	}
	bsize = (bsize + 15) & ~15U;

	heap0 = xPortGetFreeHeapSize();
	heap_prev = heap0;

	DiagPrintf("[stream_test] ch%d start: cycles=%d hold=%dms heap=%u\r\n", ctx->ch, cycles, hold_ms, heap0);

	for (iter = 1; cycles <= 0 || iter <= cycles; iter++) {
		opened = 0;
		started = 0;
		cycle_ok = 0;
		frames = 0;

		for (j = 0; j < TEST_VIN_BUFS; j++) {
			local_bufs[j] = NULL;
		}

		/* --- OPEN sequence (mirrors test_vin_setup_task) --- */

		if (rtscam_video_open(ctx->ch) != 0) {
			DiagPrintf("[stream_test] cycle %d: open failed\r\n", iter);
			goto cycle_end;
		}
		opened = 1;

		memset(&fmt, 0, sizeof(fmt));
		fmt.pixelformat = ctx->pixelformat;
		fmt.width = ctx->width;
		fmt.height = ctx->height;
		if (rtscam_video_set_format(ctx->ch, &fmt) != 0) {
			DiagPrintf("[stream_test] cycle %d: set_format failed\r\n", iter);
			goto cycle_end;
		}

		memset(&fps, 0, sizeof(fps));
		fps.numerator = 1;
		fps.denominator = ctx->fps;
		if (rtscam_video_set_fps(ctx->ch, &fps) != 0) {
			DiagPrintf("[stream_test] cycle %d: set_fps failed\r\n", iter);
			goto cycle_end;
		}

		for (j = 0; j < TEST_VIN_BUFS; j++) {
			local_bufs[j] = rtscam_video_buffer_alloc(ctx->ch, RTS_MEMORY_KERNEL, bsize);
			if (!local_bufs[j]) {
				DiagPrintf("[stream_test] cycle %d: buf_alloc[%d] failed\r\n", iter, j);
				goto cycle_end;
			}
			if (rtscam_video_enqueue(ctx->ch, local_bufs[j]) != 0) {
				DiagPrintf("[stream_test] cycle %d: enqueue[%d] failed\r\n", iter, j);
				goto cycle_end;
			}
		}

		if (rtscam_video_stream_start(ctx->ch) != 0) {
			DiagPrintf("[stream_test] cycle %d: stream_start failed\r\n", iter);
			goto cycle_end;
		}
		started = 1;

		/* --- RUN: drain and re-enqueue frames for hold_ms --- */
		{
			struct rts_video_buffer *vbuf;
			TickType_t t_start = xTaskGetTickCount();

			while ((xTaskGetTickCount() - t_start) < pdMS_TO_TICKS(hold_ms)) {
				if (rtscam_video_dequeue(ctx->ch, &vbuf) == 0) {
					frames++;
					rtscam_video_enqueue(ctx->ch, vbuf);
				} else {
					vTaskDelay(pdMS_TO_TICKS(1));
				}
			}
		}

		cycle_ok = 1;

cycle_end:
		/* --- CLOSE sequence (mirrors dbg_raw teardown order) --- */
		if (started) {
			rtscam_video_stream_stop(ctx->ch);
		}
		for (j = 0; j < TEST_VIN_BUFS; j++) {
			if (local_bufs[j]) {
				rtscam_video_buffer_release(ctx->ch, local_bufs[j]);
				local_bufs[j] = NULL;
			}
		}
		if (opened) {
			rtscam_video_close(ctx->ch);
		}

		heap_cycle = xPortGetFreeHeapSize();
		heap_delta = (int)heap_cycle - (int)heap_prev;
		heap_prev = heap_cycle;

		if (cycle_ok) {
			DiagPrintf("[stream_test] cycle %d ok: frames=%d free_heap=%u (delta=%d)\r\n", iter, frames, heap_cycle, heap_delta);
		} else {
			fail_count++;
			break; /* a failed open/close is the bug we are hunting — stop */
		}

		vTaskDelay(pdMS_TO_TICKS(100));
	}

	/* --- Summary --- */
	{
		int net_delta = (int)heap_prev - (int)heap0;

		DiagPrintf("[stream_test] done: completed=%d failures=%d net_heap_delta=%d%s\r\n", iter - 1, fail_count, net_delta,
				   (net_delta < -1024) ? " (possible leak!)" : "");
	}

	vTaskDelete(NULL);
}

void mmf2_video_example_test_vin_stream_test(int ch, int cycles, int hold_ms)
{
	static rtos_task_t task;
	stream_test_params_t *p;

	if (ch < 0 || ch >= NUM_VIN_CH) {
		DiagPrintf("[stream_test] usage: ch must be in [0, %d)\r\n", NUM_VIN_CH);
		return;
	}

	p = (stream_test_params_t *)pvPortMalloc(sizeof(*p));
	if (!p) {
		DiagPrintf("[stream_test] out of memory\r\n");
		return;
	}
	p->ch = ch;
	p->cycles = cycles;
	p->hold_ms = hold_ms;

	if (rtos_task_create(&task, "stream_test", stream_test_task, p, 4096, 1)) {
		DiagPrintf("[stream_test] task create fail\r\n");
		vPortFree(p);
	}
}
