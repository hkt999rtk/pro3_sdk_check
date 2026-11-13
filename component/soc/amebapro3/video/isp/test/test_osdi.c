/*
 * Copyright (C) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * MCU FreeRTOS OSDI HAL unit test.
 * Run osdi_test_task as a FreeRTOS task after ISP is started.
 *
 * Mapping to test_osdi.sh test items:
 *
 *   Phase 1  — init / release
 *              (no shell equivalent; HAL-level sanity)
 *
 *   Phase 2  — config_frame validation
 *              (no shell equivalent; HAL-level sanity)
 *
 *   Phase 3  — all pixel formats × all ISP instances (config_block + capture)
 *              → test1: -b color/640x480_m<fmt> <fmt> -f 1920 1080 -n <ISP> -c 20
 *                 (all 6 formats, 3 ISP instances, load from SD card or fill 0xFF)
 *              → test2: -b alpha/1280x64x{ff,0,77}_m5.data 5 -f 1920 1080 -n <ISP>
 *                 (RGBA_8888 with varying alpha; covered by fmt=RGBA8888 iteration)
 *              → test6: -p 1920 1080 5 0xff... -f 1920 1080 -n <ISP>
 *                 (full-frame RGBA_8888 fill; covered by RGBA8888 iteration)
 *
 *   Phase 4  — get_blk / set_blk (low-level register round-trip)
 *              (no shell equivalent; exercises raw HW register read/write path)
 *
 *   Phase 5  — six simultaneous blocks, all ISP instances
 *              → test3: -1 1 -f 1920 1080 -n <ISP>
 *                 (sixblkDebug=1: configure all 6 blocks with different formats)
 *
 *   Phase 6  — enable_block / disable_block / query_block
 *              → test3 interactive cases 1-6 / 11-16 (close/open individual blocks)
 *
 *   Phase 7  — dynamic parameter change while streaming
 *              → test4: -p <w> <h> 4 ... -2 1 (dynamicDebug=1: change width/height/
 *                 fmt/start_x/start_y while OSDI is running)
 *              Note: dynamicDebug=2 (DDR offset) and dynamicDebug=4 (SRAM switch) are
 *              driver debug modes not replicated here; they are exercised indirectly
 *              by Phase 4's get_blk/set_blk register writes.
 *
 *   Phase 8  — color table set / get
 *              → test3: -a <w> <h> <fmt> ... -4 1 (onlyblkDebug=1: set color table
 *                 entries for each format and observe change)
 *
 *   Phase 9  — 5 frame_stop + immediate_stop cycles, all ISP instances
 *              → test5: -t -3 1 (ispDebug=1: repeated OSDI stop/start, 20 cycles in
 *                 shell; reduced to 5 cycles here to fit MCU completion timeout)
 *              Note: ispDebug=2 (ISP stream open/close cycles) is not applicable on
 *              MCU where the ISP stream is managed externally to the OSDI test.
 */

#include <string.h>
#include <stdint.h>
#include <stdio.h>
#include <FreeRTOS.h>
#include <task.h>
#include "ameba_soc.h"
#include "diag.h"
#include "hal_isp.h"
#include "isp_common.h"
#include "video_app_rcc.h"
#include "hal_osdi.h"
#include "rts_camera.h"
#include "rts_video.h"
#include "vfs.h"

/* Forward declaration from rtsavisp.h (not in test include path) */
#define RTS_ISP_RUNNING 2
extern int32_t rts_av_isp_get_status(void);

/* ------------------------------------------------------------------ */
/* Constants                                                           */
/* ------------------------------------------------------------------ */

#define OSDI_NUM_ISP       3

/* Block dimensions for each test phase (must match the SD card source file) */
#define TEST_BLK_W         640   /* Phase 3: color/640x480_m<fmt> */
#define TEST_BLK_H         480
#define OSDI_BLOCK_FILE_W  320   /* Phase 5: block/<n>.data  (320x240) */
#define OSDI_BLOCK_FILE_H  240

#define TEST_FPS           10
#define TEST_VIN_BUFS      2
#define DEQUEUE_TIMEOUT_MS 500

/* ------------------------------------------------------------------ */
/* Assertion framework                                                 */
/* ------------------------------------------------------------------ */

static int g_pass;
static int g_fail;

#define OSDI_ASSERT(cond, name) do {                                    \
	if (cond) {                                                         \
		g_pass++;                                                       \
	} else {                                                            \
		g_fail++;                                                       \
		DiagPrintf("[OSDI FAIL] %s  @ %s:%d\r\n",                      \
			   (name), __func__, __LINE__);                             \
	}                                                                   \
} while (0)

#define OSDI_ASSERT_EQ(a, b, n)  OSDI_ASSERT((a) == (b), n)
#define OSDI_ASSERT_NE(a, b, n)  OSDI_ASSERT((a) != (b), n)
#define OSDI_ASSERT_OK(r, n)     OSDI_ASSERT_EQ((r), RTS_OK, n)

/* ------------------------------------------------------------------ */
/* Global state                                                        */
/* ------------------------------------------------------------------ */

struct vin_ctx {
	int ch;
	uint16_t frame_w;
	uint16_t frame_h;
	struct rts_video_buffer *bufs[TEST_VIN_BUFS];
};

static struct vin_ctx g_vin[OSDI_NUM_ISP];
static RtsOsdiInst    g_osdi;

/* ------------------------------------------------------------------ */
/* Pixel-size helper (mirrors hal_osdi.c __get_pic_size)              */
/* ------------------------------------------------------------------ */

static uint32_t pix_size(uint16_t w, uint16_t h, int fmt)
{
	switch (fmt) {
	case RTS_PIX_FMT_1BPP:
		return ((uint32_t)w * h + 7) >> 3;
	case RTS_PIX_FMT_RGBA1111:
		return ((uint32_t)w * h) >> 1;
	case RTS_PIX_FMT_RGBA2222:
		return (uint32_t)w * h;
	case RTS_PIX_FMT_RGBA5551:
	case RTS_PIX_FMT_RGBA4444:
		return (uint32_t)w * 2 * h;
	case RTS_PIX_FMT_RGBA8888:
		return (uint32_t)w * 4 * h;
	default:
		return (uint32_t)w * h;
	}
}

/* ------------------------------------------------------------------ */
/* Block position helper (3-col × 2-row grid, even-aligned)           */
/* ------------------------------------------------------------------ */

/*
 * Distribute col ∈ {0,1,2} × row ∈ {0,1} non-overlapping blocks of size
 * blk_w × blk_h inside a frame of fw × fh.  Positions are always
 * even-aligned (OSDI hardware requirement).
 *
 * Column placement guarantees no horizontal overlap:
 *   col 0 → left edge (sx = 0)
 *   col 1 → right of col 0 (sx = blk_w), clamped to center when room allows
 *   col 2 → right edge  (sx = fw - blk_w)
 * Caller must ensure fw >= 3 * blk_w (use scale_blk_for_frame() first).
 */
static void calc_blk_pos(uint16_t fw, uint16_t fh,
						 int col, int row,
						 uint16_t blk_w, uint16_t blk_h,
						 uint16_t *sx, uint16_t *sy)
{
	switch (col) {
	case 0:
		*sx = 0;
		break;
	case 1:
		/* place at centre but never closer than blk_w from either edge */
		*sx = (uint16_t)(((fw - blk_w) / 2) & ~1U);
		if (*sx < blk_w) {
			*sx = (uint16_t)(blk_w & ~1U);
		}
		break;
	case 2:
	default:
		*sx = (uint16_t)((fw - blk_w) & ~1U);
		break;
	}

	if (row == 0) {
		*sy = 0;
	} else {
		*sy = (uint16_t)((fh - blk_h) & ~1U);
	}
}

/*
 * Note: calc_blk_pos guarantees non-overlapping positions only when
 * fw >= 3 * blk_w.  If the frame is narrower than 3 * blk_w the HAL's
 * overlap checker will reject some blocks and log [OSDI FAIL] — this is
 * expected behaviour and documents the hardware limit for that ISP stream.
 */

/* ------------------------------------------------------------------ */
/* load_or_fill: malloc a pixel buffer from file or fill with 0xFF    */
/* ------------------------------------------------------------------ */

static void *load_or_fill(const char *path, uint32_t len)
{
	void *buf = pvPortMalloc(len);
	if (!buf) {
		return NULL;
	}

	{
		FILE *f = fopen(path, "rb");

		if (f) {
			fread(buf, 1, len, f);
			fclose(f);
		} else {
			memset(buf, 0xFF, len);
		}
	}

	return buf;
}

/* ------------------------------------------------------------------ */
/* capture_frame: dequeue one frame and write it to SD                */
/* ------------------------------------------------------------------ */

static void capture_frame(int isp_idx, const char *tag, const char *when)
{
	struct rts_video_buffer *vbuf = NULL;
	int ch = g_vin[isp_idx].ch;
	int elapsed = 0;

	int fcnt = 0;

	while (1) {
		while (rtscam_video_dequeue(ch, &vbuf) != 0) {
			vTaskDelay(pdMS_TO_TICKS(1));
			elapsed++;
			if (elapsed >= DEQUEUE_TIMEOUT_MS) {
				DiagPrintf("[OSDI TEST] capture_frame: timeout isp%d tag=%s when=%s\r\n",
						   isp_idx, tag, when);
				return;
			}
		}
		elapsed = 0;
		if (fcnt++ > 20) {
			break;
		}
		rtscam_video_enqueue(ch, vbuf);
	}

	{
		FILE *f;
		char path[64];

		snprintf(path, sizeof(path),
				 "fat:osdi_%s_isp%d_%s.bin", tag, isp_idx, when);
		f = fopen(path, "wb");
		if (f) {
			fwrite(vbuf->vm_addr, 1, vbuf->bytesused, f);
			fclose(f);
		} else {
			DiagPrintf("[OSDI TEST] capture_frame: fopen(%s) failed\r\n",
					   path);
		}
	}

	rtscam_video_enqueue(ch, vbuf);
}

/* ------------------------------------------------------------------ */
/* stream_open_all: open ch 0/1/2 and start streaming                 */
/* ------------------------------------------------------------------ */

static int stream_open_all(void)
{
	int ch;

	for (ch = 0; ch < OSDI_NUM_ISP; ch++) {
		struct rts_video_caps caps = {0};
		struct rts_video_format fmt = {0};
		struct rts_video_fps fps = {0};
		uint32_t max_w = 0, max_h = 0;
		size_t bsize;
		int i;

		g_vin[ch].ch = ch;

		if (rtscam_video_open(ch)) {
			DiagPrintf("[OSDI TEST] open ch%d fail\r\n", ch);
			return -1;
		}

		if (rtscam_video_get_caps(ch, &caps) == 0 &&
			caps.format_caps) {
			int idx = 0;

			while (caps.format_caps[idx].pixelformat) {
				const struct rts_video_format_cap *fc =
						&caps.format_caps[idx];

				if (fc->width_max > max_w) {
					max_w = fc->width_max;
				}
				if (fc->height_max > max_h) {
					max_h = fc->height_max;
				}
				idx++;
			}
		}

		if (max_w == 0) {
			max_w = 2688;
		}
		if (max_h == 0) {
			max_h = 1520;
		}

		fmt.pixelformat = V4L2_PIX_FMT_NV12;
		fmt.width  = max_w;
		fmt.height = max_h;
		if (rtscam_video_set_format(ch, &fmt)) {
			DiagPrintf("[OSDI TEST] set_format ch%d fail\r\n", ch);
			rtscam_video_close(ch);
			return -1;
		}

		fps.numerator   = 1;
		fps.denominator = TEST_FPS;
		if (rtscam_video_set_fps(ch, &fps))
			DiagPrintf("[OSDI TEST] set_fps ch%d fail (non-fatal)\r\n",
					   ch);

		bsize = (size_t)max_w * max_h * 3 / 2;
		bsize = (bsize + 15) & ~15U;

		for (i = 0; i < TEST_VIN_BUFS; i++) {
			g_vin[ch].bufs[i] = rtscam_video_buffer_alloc(
									ch, RTS_MEMORY_KERNEL, bsize);
			if (!g_vin[ch].bufs[i]) {
				DiagPrintf("[OSDI TEST] buf_alloc ch%d[%d] fail\r\n",
						   ch, i);
				rtscam_video_stream_stop(ch);
				rtscam_video_close(ch);
				return -1;
			}
			rtscam_video_enqueue(ch, g_vin[ch].bufs[i]);
		}

		if (rtscam_video_stream_start(ch)) {
			DiagPrintf("[OSDI TEST] stream_start ch%d fail\r\n", ch);
			for (i = 0; i < TEST_VIN_BUFS; i++) {
				if (g_vin[ch].bufs[i])
					rtscam_video_buffer_release(
						ch, g_vin[ch].bufs[i]);
			}
			rtscam_video_close(ch);
			return -1;
		}

		g_vin[ch].frame_w = (uint16_t)max_w;
		g_vin[ch].frame_h = (uint16_t)max_h;

		DiagPrintf("[OSDI TEST] ch%d started %ux%u NV12 @%dfps\r\n",
				   ch, max_w, max_h, TEST_FPS);
	}

	return 0;
}

/* ------------------------------------------------------------------ */
/* stream_close_all                                                    */
/* ------------------------------------------------------------------ */

static void stream_close_all(void)
{
	int ch;

	for (ch = 0; ch < OSDI_NUM_ISP; ch++) {
		int i;

		rtscam_video_stream_stop(ch);
		for (i = 0; i < TEST_VIN_BUFS; i++) {
			if (g_vin[ch].bufs[i]) {
				rtscam_video_buffer_release(ch,
											g_vin[ch].bufs[i]);
				g_vin[ch].bufs[i] = NULL;
			}
		}
		rtscam_video_close(ch);
	}
}

/* ------------------------------------------------------------------ */
/* suite_init / suite_clean                                            */
/* ------------------------------------------------------------------ */

static int suite_init(int isp_idx)
{
	int ret;
	struct rts_osdi_frame_info fi;

	ret = rts_osdi_init(&g_osdi, isp_idx);
	if (ret) {
		DiagPrintf("[OSDI TEST] suite_init: rts_osdi_init(%d) failed %d\r\n",
				   isp_idx, ret);
		g_osdi = NULL;
		return ret;
	}
	if (!g_osdi) {
		DiagPrintf("[OSDI TEST] suite_init: NULL handle for isp%d\r\n",
				   isp_idx);
		return -1;
	}

	fi.fmt    = 0;
	fi.width  = g_vin[isp_idx].frame_w;
	fi.height = g_vin[isp_idx].frame_h;
	ret = rts_osdi_config_frame(g_osdi, &fi);
	if (ret) {
		DiagPrintf("[OSDI TEST] suite_init: config_frame failed %d\r\n",
				   ret);
		rts_osdi_release(g_osdi);
		g_osdi = NULL;
		return ret;
	}

	return 0;
}

static void suite_clean(void)
{
	if (g_osdi) {
		rts_osdi_frame_stop(g_osdi);
		rts_osdi_release(g_osdi);
		g_osdi = NULL;
	}
}

/* ================================================================== */
/* Phase 1: init / release                                            */
/* ================================================================== */

static void test_init_release(void)
{
	RtsOsdiInst h = NULL;

	/* Normal path */
	OSDI_ASSERT_EQ(rts_osdi_init(&h, 0), RTS_OK,
				   "init_normal: returns RTS_OK");
	OSDI_ASSERT(h != NULL,
				"init_normal: handle non-NULL");
	OSDI_ASSERT_EQ(rts_osdi_release(h), RTS_OK,
				   "release_normal: returns RTS_OK");

	/* Invalid num — must NOT use assert()-crashing paths */
	h = NULL;
	OSDI_ASSERT_NE(rts_osdi_init(&h, 99), RTS_OK,
				   "init_invalid_num: num=99 fails");
	OSDI_ASSERT_NE(rts_osdi_init(&h, -1), RTS_OK,
				   "init_invalid_num: num=-1 fails");
}

/* ================================================================== */
/* Phase 2: config_frame validation                                   */
/* ================================================================== */

static void test_config_frame(void)
{
	struct rts_osdi_frame_info fi;

	/* g_osdi already init'd by suite_init(0) in caller */
	fi.fmt    = 0;
	fi.width  = 1920;
	fi.height = 1080;
	OSDI_ASSERT_OK(rts_osdi_config_frame(g_osdi, &fi),
				   "config_frame_normal: 1920x1080");

	/* Invalid: zero dimensions */
	fi.width  = 0;
	fi.height = 0;
	OSDI_ASSERT_NE(rts_osdi_config_frame(g_osdi, &fi), RTS_OK,
				   "config_frame_invalid: 0x0");

	/* Restore correct config for subsequent tests */
	fi.width  = g_vin[0].frame_w;
	fi.height = g_vin[0].frame_h;
	rts_osdi_config_frame(g_osdi, &fi);
}

/* ================================================================== */
/* Phase 3: all pixel formats × all ISP instances                     */
/* test_osdi.sh: test1, test2, test6                                  */
/* ================================================================== */

static void test_block_config_all_fmts_all_isp(void)
{
	static const int fmts[] = {
		RTS_PIX_FMT_1BPP,
		RTS_PIX_FMT_RGBA1111,
		RTS_PIX_FMT_RGBA2222,
		RTS_PIX_FMT_RGBA5551,
		RTS_PIX_FMT_RGBA4444,
		RTS_PIX_FMT_RGBA8888,
	};
	char tag[32];
	int isp, fi;

	for (isp = 0; isp < OSDI_NUM_ISP; isp++) {
		for (fi = 0; fi < 6; fi++) {
			int fmt = fmts[fi];
			uint32_t len;
			void *pdata;
			struct rts_osdi_blk blk;

			if (suite_init(isp) != 0) {
				continue;
			}

			snprintf(tag, sizeof(tag), "p3_fmt%d", fi);
			capture_frame(isp, tag, "before");

			len = pix_size(TEST_BLK_W, TEST_BLK_H, fmt);
			{
				char imgpath[64];

				/* Match test1 filename: color/640x480_m<fmt> */
				snprintf(imgpath, sizeof(imgpath),
						 "fat:color/640x480_m%d", fi);
				pdata = load_or_fill(imgpath, len);
			}

			memset(&blk, 0, sizeof(blk));
			blk.index   = 0;
			blk.enable  = 1;
			blk.fmt     = (uint32_t)fmt;
			blk.pdata   = pdata;
			blk.length  = len;
			blk.width   = TEST_BLK_W;
			blk.height  = TEST_BLK_H;
			blk.start_x = 0;
			blk.start_y = 0;

			OSDI_ASSERT_OK(rts_osdi_config_block(g_osdi, 0, &blk),
						   tag);
			OSDI_ASSERT_OK(rts_osdi_start(g_osdi), tag);

			capture_frame(isp, tag, "after");

			rts_osdi_frame_stop(g_osdi);

			/* Release block pixel data */
			blk.pdata = NULL;
			rts_osdi_config_block(g_osdi, 0, &blk);
			vPortFree(pdata);

			suite_clean();
		}
	}
}

/* ================================================================== */
/* Phase 4: get_blk / set_blk low-level register round-trip          */
/* ================================================================== */

static void test_get_set_blk(void)
{
	uint32_t len;
	void *pdata;
	struct rts_osdi_blk blk;

	/* g_osdi already init'd by suite_init(0) in caller */
	len   = pix_size(TEST_BLK_W, TEST_BLK_H, RTS_PIX_FMT_RGBA8888);
	pdata = pvPortMalloc(len);
	if (!pdata) {
		DiagPrintf("[OSDI TEST] test_get_set_blk: alloc failed\r\n");
		return;
	}
	memset(pdata, 0xFF, len);

	memset(&blk, 0, sizeof(blk));
	blk.index   = 0;
	blk.enable  = 1;
	blk.fmt     = (uint32_t)RTS_PIX_FMT_RGBA8888;
	blk.pdata   = pdata;
	blk.length  = len;
	blk.width   = TEST_BLK_W;
	blk.height  = TEST_BLK_H;
	blk.start_x = 0;
	blk.start_y = 0;

	rts_osdi_config_block(g_osdi, 0, &blk);

	/* Normal get/set */
	OSDI_ASSERT_OK(rts_osdi_get_blk(g_osdi, 0),
				   "get_blk_normal: idx=0 ok");
	OSDI_ASSERT_OK(rts_osdi_set_blk(g_osdi, 0),
				   "set_blk_normal: idx=0 ok (idempotent)");

	/* Invalid block index — use out-of-range values */
	OSDI_ASSERT_NE(rts_osdi_get_blk(g_osdi, -1), RTS_OK,
				   "get_blk_invalid: idx=-1");
	OSDI_ASSERT_NE(rts_osdi_get_blk(g_osdi, RTS_OSDI_MAX_BLK_NUM), RTS_OK,
				   "get_blk_invalid: idx=max");
	OSDI_ASSERT_NE(rts_osdi_set_blk(g_osdi, -1), RTS_OK,
				   "set_blk_invalid: idx=-1");

	/* Cleanup */
	blk.pdata = NULL;
	rts_osdi_config_block(g_osdi, 0, &blk);
	vPortFree(pdata);
}

/* ================================================================== */
/* Phase 5: six simultaneous blocks, all ISP instances                */
/* test_osdi.sh: test3 (-1 1, sixblkDebug=1)                         */
/* ================================================================== */

static void test_six_blocks_all_isp(void)
{
	static const int blk_fmts[6] = {
		RTS_PIX_FMT_1BPP,
		RTS_PIX_FMT_RGBA1111,
		RTS_PIX_FMT_RGBA2222,
		RTS_PIX_FMT_RGBA5551,
		RTS_PIX_FMT_RGBA4444,
		RTS_PIX_FMT_RGBA8888,
	};
	static const int cols[6] = { 0, 1, 2, 0, 1, 2 };
	static const int rows[6] = { 0, 0, 0, 1, 1, 1 };
	char tag[16];
	int isp, i;

	for (isp = 0; isp < OSDI_NUM_ISP; isp++) {
		void *bufs[6] = { NULL };
		/* Always use the block file's native dimensions (block/<n>.data) */
		const uint16_t blk_w = OSDI_BLOCK_FILE_W;
		const uint16_t blk_h = OSDI_BLOCK_FILE_H;

		if (suite_init(isp) != 0) {
			continue;
		}

		/*
		 * The 3-col x 2-row layout needs strict gaps between blocks:
		 * the HAL overlap check (CMP, hal_osdi.h) flags edge-touching
		 * blocks as overlapping, so the grid requires
		 * frame_w > 3*blk_w and frame_h > 2*blk_h.  Skip (not fail)
		 * any stream too small to host it.
		 */
		if (g_vin[isp].frame_w <= 3 * blk_w ||
			g_vin[isp].frame_h <= 2 * blk_h) {
			DiagPrintf("[OSDI TEST] p5 isp%d: frame %ux%u too small "
					   "for six-block grid (need >%ux%u) - skip\r\n",
					   isp, g_vin[isp].frame_w, g_vin[isp].frame_h,
					   3 * blk_w, 2 * blk_h);
			suite_clean();
			continue;
		}

		DiagPrintf("[OSDI TEST] p5 isp%d: frame=%ux%u blk=%ux%u\r\n",
				   isp, g_vin[isp].frame_w, g_vin[isp].frame_h,
				   blk_w, blk_h);

		snprintf(tag, sizeof(tag), "p5_isp%d", isp);
		capture_frame(isp, tag, "before");

		for (i = 0; i < 6; i++) {
			int enable = (isp == 2 && i == 5) ? 0 : 1;
			uint32_t len = pix_size(blk_w, blk_h, blk_fmts[i]);
			uint16_t sx, sy;
			struct rts_osdi_blk blk;
			char btag[32];

			{
				char imgpath[64];

				/* Match test3 six-block filename: block/<n>.data */
				snprintf(imgpath, sizeof(imgpath),
						 "fat:block/%d.data", i);
				bufs[i] = load_or_fill(imgpath, len);
			}

			calc_blk_pos(g_vin[isp].frame_w, g_vin[isp].frame_h,
						 cols[i], rows[i], blk_w, blk_h, &sx, &sy);

			memset(&blk, 0, sizeof(blk));
			blk.index   = (uint8_t)i;
			blk.enable  = (uint8_t)enable;
			blk.fmt     = (uint32_t)blk_fmts[i];
			blk.pdata   = enable ? bufs[i] : NULL;
			blk.length  = len;
			blk.width   = blk_w;
			blk.height  = blk_h;
			blk.start_x = sx;
			blk.start_y = sy;

			snprintf(btag, sizeof(btag), "p5_isp%d_blk%d", isp, i);
			OSDI_ASSERT_OK(rts_osdi_config_block(g_osdi, i, &blk),
						   btag);
		}

		OSDI_ASSERT_OK(rts_osdi_start(g_osdi), "p5_start");
		capture_frame(isp, tag, "after");
		rts_osdi_frame_stop(g_osdi);

		/* Release all block pixel buffers */
		for (i = 0; i < 6; i++) {
			struct rts_osdi_blk nb;

			memset(&nb, 0, sizeof(nb));
			nb.index  = (uint8_t)i;
			nb.pdata  = NULL;
			rts_osdi_config_block(g_osdi, i, &nb);
			vPortFree(bufs[i]);
		}

		suite_clean();
	}
}

/* ================================================================== */
/* Phase 6: enable_block / disable_block / query_block                */
/* test_osdi.sh: test3 interactive cases 1-6 / 11-16                 */
/* ================================================================== */

static void test_enable_disable_and_query(void)
{
	uint32_t len;
	void *pdata;
	struct rts_osdi_blk blk;
	struct rts_osdi_blk qblk;

	/* g_osdi already init'd by suite_init(0) in caller */
	len   = pix_size(TEST_BLK_W, TEST_BLK_H, RTS_PIX_FMT_RGBA8888);
	pdata = pvPortMalloc(len);
	if (!pdata) {
		DiagPrintf("[OSDI TEST] test_enable_disable_and_query: alloc failed\r\n");
		return;
	}
	memset(pdata, 0xAB, len);

	memset(&blk, 0, sizeof(blk));
	blk.index   = 0;
	blk.enable  = 1;
	blk.fmt     = (uint32_t)RTS_PIX_FMT_RGBA8888;
	blk.pdata   = pdata;
	blk.length  = len;
	blk.width   = TEST_BLK_W;
	blk.height  = TEST_BLK_H;
	blk.start_x = 0;
	blk.start_y = 0;

	rts_osdi_config_block(g_osdi, 0, &blk);

	/* enable / disable */
	OSDI_ASSERT_OK(rts_osdi_disable_block(g_osdi, 0),
				   "disable_block: ok");
	OSDI_ASSERT_OK(rts_osdi_enable_block(g_osdi, 0),
				   "enable_block: ok");

	/* query round-trip */
	memset(&qblk, 0, sizeof(qblk));
	OSDI_ASSERT_OK(rts_osdi_query_block(g_osdi, 0, &qblk),
				   "query_block: ok");
	OSDI_ASSERT_EQ(qblk.width,  TEST_BLK_W, "query_block: width match");
	OSDI_ASSERT_EQ(qblk.height, TEST_BLK_H, "query_block: height match");
	OSDI_ASSERT_EQ(qblk.index,  0,          "query_block: index match");

	/* Cleanup */
	blk.pdata = NULL;
	rts_osdi_config_block(g_osdi, 0, &blk);
	vPortFree(pdata);
}

/* ================================================================== */
/* Phase 7: dynamic parameter change while streaming                  */
/* test_osdi.sh: test4 (-2 1, dynamicDebug=1)                        */
/* ================================================================== */

static void test_dynamic_param_change(void)
{
	uint32_t len, len2, len3;
	void *pdata, *pdata2, *pdata3;
	struct rts_osdi_blk blk;
	uint16_t sx2, sy2;

	/* g_osdi already init'd by suite_init(0) in caller */
	len   = pix_size(TEST_BLK_W, TEST_BLK_H, RTS_PIX_FMT_RGBA4444);
	pdata = pvPortMalloc(len);
	if (!pdata) {
		DiagPrintf("[OSDI TEST] test_dynamic_param_change: alloc1 failed\r\n");
		return;
	}
	memset(pdata, 0xFF, len);

	memset(&blk, 0, sizeof(blk));
	blk.index   = 0;
	blk.enable  = 1;
	blk.fmt     = (uint32_t)RTS_PIX_FMT_RGBA4444;
	blk.pdata   = pdata;
	blk.length  = len;
	blk.width   = TEST_BLK_W;
	blk.height  = TEST_BLK_H;
	blk.start_x = 0;
	blk.start_y = 0;

	rts_osdi_config_block(g_osdi, 0, &blk);
	rts_osdi_start(g_osdi);
	capture_frame(0, "p7", "before");

	/* Change width to half */
	len2   = pix_size(TEST_BLK_W / 2, TEST_BLK_H, RTS_PIX_FMT_RGBA4444);
	pdata2 = pvPortMalloc(len2);
	if (!pdata2) {
		DiagPrintf("[OSDI TEST] test_dynamic_param_change: alloc2 failed\r\n");
		rts_osdi_frame_stop(g_osdi);
		blk.pdata = NULL;
		rts_osdi_config_block(g_osdi, 0, &blk);
		vPortFree(pdata);
		return;
	}
	memset(pdata2, 0xFF, len2);

	blk.width  = TEST_BLK_W / 2;
	blk.pdata  = pdata2;
	blk.length = len2;
	OSDI_ASSERT_OK(rts_osdi_config_block(g_osdi, 0, &blk),
				   "dynamic_width_half: ok");
	capture_frame(0, "p7", "width_half");

	/* Move block to right column */
	calc_blk_pos(g_vin[0].frame_w, g_vin[0].frame_h, 2, 0,
				 TEST_BLK_W, TEST_BLK_H, &sx2, &sy2);
	blk.start_x = sx2;
	blk.width   = TEST_BLK_W;

	len3   = pix_size(TEST_BLK_W, TEST_BLK_H, RTS_PIX_FMT_RGBA4444);
	pdata3 = pvPortMalloc(len3);
	if (!pdata3) {
		DiagPrintf("[OSDI TEST] test_dynamic_param_change: alloc3 failed\r\n");
		rts_osdi_frame_stop(g_osdi);
		blk.pdata = NULL;
		rts_osdi_config_block(g_osdi, 0, &blk);
		vPortFree(pdata);
		vPortFree(pdata2);
		return;
	}
	memset(pdata3, 0xFF, len3);

	blk.pdata  = pdata3;
	blk.length = len3;
	OSDI_ASSERT_OK(rts_osdi_config_block(g_osdi, 0, &blk),
				   "dynamic_move_x: ok");
	capture_frame(0, "p7", "moved_x");

	rts_osdi_frame_stop(g_osdi);

	blk.pdata = NULL;
	rts_osdi_config_block(g_osdi, 0, &blk);

	vPortFree(pdata);
	vPortFree(pdata2);
	vPortFree(pdata3);
}

/* ================================================================== */
/* Phase 8: color table set / get                                     */
/* test_osdi.sh: test3 (-a ... -4 1, onlyblkDebug=1)                 */
/* ================================================================== */

/*
 * Color-table tests use RGBA4444 — RGBA8888 has no LUT in ColorTableDesc.
 *
 * IMPORTANT — the red/green/blue/alpha args to rts_osdi_set_color_table are
 * LUT slot INDICES, not channel values.  At render time the hardware uses each
 * pixel's per-channel nibble as the LUT index.  The block is filled with 0xFF,
 * so every RGBA4444 channel nibble = 0xF = 15; we must therefore write slot 15
 * (CT_PIX_IDX) for the change to affect the rendered pixels.  Writing any other
 * slot leaves the displayed block unchanged (captures would be identical).
 *
 * 'val' carries the actual channel bytes, high→low = R,G,B,A:
 *   0xffffffff → opaque white, 0xffffff00 → transparent, 0xffffff77 → ~47% alpha.
 *
 * The driver requires ALL OSDI instances stopped before SET_COLOR_TABLE
 * (__check_all_osdi_status).  Pattern: set → start → capture → frame_stop.
 */
#define CT_PIX_IDX 15   /* 0xFF fill → every RGBA4444 nibble = 0xF = 15 */

static void test_color_table(void)
{
	uint32_t len;
	void *pdata;
	struct rts_osdi_blk blk;
	uint32_t got_val = 0;
	const uint8_t i = CT_PIX_IDX;

	/* g_osdi already init'd by suite_init(0) in caller */
	len   = pix_size(TEST_BLK_W, TEST_BLK_H, RTS_PIX_FMT_RGBA4444);
	pdata = pvPortMalloc(len);
	if (!pdata) {
		DiagPrintf("[OSDI TEST] test_color_table: alloc failed\r\n");
		return;
	}
	memset(pdata, 0xFF, len);

	memset(&blk, 0, sizeof(blk));
	blk.index   = 0;
	blk.enable  = 1;
	blk.fmt     = (uint32_t)RTS_PIX_FMT_RGBA4444;
	blk.pdata   = pdata;
	blk.length  = len;
	blk.width   = TEST_BLK_W;
	blk.height  = TEST_BLK_H;
	blk.start_x = 0;
	blk.start_y = 0;

	rts_osdi_config_block(g_osdi, 0, &blk);

	/* Full-opaque white: set slot 15 → start → capture → stop */
	OSDI_ASSERT_OK(rts_osdi_set_color_table(RTS_PIX_FMT_RGBA4444,
											0xffffffff, i, i, i, i),
				   "set_color_table_normal: ok");
	rts_osdi_start(g_osdi);
	capture_frame(0, "p8_ctFF", "after");
	rts_osdi_frame_stop(g_osdi);

	/* Fully transparent (alpha byte = 0x00) → block should vanish */
	OSDI_ASSERT_OK(rts_osdi_set_color_table(RTS_PIX_FMT_RGBA4444,
											0xffffff00, i, i, i, i),
				   "set_color_table_00: ok");
	rts_osdi_start(g_osdi);
	capture_frame(0, "p8_ct00", "after");
	rts_osdi_frame_stop(g_osdi);

	/* Semi-transparent (alpha byte = 0x77) → block blends with background */
	OSDI_ASSERT_OK(rts_osdi_set_color_table(RTS_PIX_FMT_RGBA4444,
											0xffffff77, i, i, i, i),
				   "set_color_table_77: ok");
	rts_osdi_start(g_osdi);
	capture_frame(0, "p8_ct77", "after");
	rts_osdi_frame_stop(g_osdi);

	/* Restore and verify get round-trip (OSDI still stopped) */
	rts_osdi_set_color_table(RTS_PIX_FMT_RGBA4444, 0xffffffff, i, i, i, i);
	OSDI_ASSERT_OK(rts_osdi_get_color_table(RTS_PIX_FMT_RGBA4444,
											&got_val, i, i, i, i),
				   "get_color_table: ok");
	OSDI_ASSERT_EQ(got_val, 0xffffffff,
				   "get_color_table: value matches last set");

	blk.pdata = NULL;
	rts_osdi_config_block(g_osdi, 0, &blk);
	vPortFree(pdata);
}

/* ================================================================== */
/* Phase 9: 5 frame_stop cycles + immediate_stop, all ISP instances  */
/* test_osdi.sh: test5 (-t -3 1, ispDebug=1)                         */
/* ================================================================== */

static void test_start_stop_cycles_all_isp(void)
{
	int isp;

	for (isp = 0; isp < OSDI_NUM_ISP; isp++) {
		uint32_t len;
		void *pdata;
		struct rts_osdi_blk blk;
		int c;

		if (suite_init(isp) != 0) {
			continue;
		}

		len   = pix_size(TEST_BLK_W, TEST_BLK_H, RTS_PIX_FMT_RGBA8888);
		pdata = pvPortMalloc(len);
		if (!pdata) {
			DiagPrintf("[OSDI TEST] p9 isp%d: alloc failed\r\n", isp);
			suite_clean();
			continue;
		}
		memset(pdata, 0xFF, len);

		memset(&blk, 0, sizeof(blk));
		blk.index   = 0;
		blk.enable  = 1;
		blk.fmt     = (uint32_t)RTS_PIX_FMT_RGBA8888;
		blk.pdata   = pdata;
		blk.length  = len;
		blk.width   = TEST_BLK_W;
		blk.height  = TEST_BLK_H;
		blk.start_x = 0;
		blk.start_y = 0;

		rts_osdi_config_block(g_osdi, 0, &blk);

		/* 5 start / frame_stop cycles */
		for (c = 0; c < 5; c++) {
			char ctag[24];

			snprintf(ctag, sizeof(ctag), "p9_isp%d_c%d", isp, c);
			OSDI_ASSERT_OK(rts_osdi_start(g_osdi), ctag);
			capture_frame(isp, ctag, "after");
			OSDI_ASSERT_OK(rts_osdi_frame_stop(g_osdi), ctag);
		}

		/* immediate_stop */
		OSDI_ASSERT_OK(rts_osdi_start(g_osdi), "p9_imm_start");
		OSDI_ASSERT_OK(rts_osdi_immediate_stop(g_osdi), "p9_imm_stop");

		blk.pdata = NULL;
		rts_osdi_config_block(g_osdi, 0, &blk);
		vPortFree(pdata);

		suite_clean();
	}
}

/* ================================================================== */
/* Task entry point                                                    */
/* ================================================================== */

void osdi_test_task(void *pvParameters)
{
	(void)pvParameters;

	g_pass = 0;
	g_fail = 0;

	DiagPrintf("[OSDI TEST] starting\r\n");

	/* Initialise ISP hardware and start the ISP server */
	{
		static struct isp_io io = {0};

		video_app_common_rcc_init();
		ISP_Init();
		if (isp_initial(&io)) {
			DiagPrintf("[OSDI TEST] isp_initial fail — aborting\r\n");
			vTaskDelete(NULL);
			return;
		}
	}

	/* Wait for ISP server to be running */
	while (rts_av_isp_get_status() < RTS_ISP_RUNNING) {
		vTaskDelay(pdMS_TO_TICKS(10));
	}

	/* Mount SD card via VFS */
	vfs_init();
	vfs_target_register("fat", VFS_TARGET_SD, VFS_RW);

	/* Initialise the rtscam driver and open all 3 VIN streams */
	rtscam_video_init();
	if (stream_open_all() != 0) {
		DiagPrintf("[OSDI TEST] stream_open_all failed — aborting\r\n");
		goto done;
	}

	/* Phase 1: init / release */
	DiagPrintf("[OSDI TEST] Phase 1: init/release\r\n");
	test_init_release();

	/* Phase 2: config_frame */
	DiagPrintf("[OSDI TEST] Phase 2: config_frame\r\n");
	if (suite_init(0) == 0) {
		test_config_frame();
		suite_clean();
	}

	/* Phase 3: all formats × all ISP instances */
	DiagPrintf("[OSDI TEST] Phase 3: all fmts all isp\r\n");
	test_block_config_all_fmts_all_isp();

	/* Phase 4: get_blk / set_blk */
	DiagPrintf("[OSDI TEST] Phase 4: get_blk/set_blk\r\n");
	if (suite_init(0) == 0) {
		test_get_set_blk();
		suite_clean();
	}

	/* Phase 5: six blocks, all ISP instances */
	DiagPrintf("[OSDI TEST] Phase 5: six blocks all isp\r\n");
	test_six_blocks_all_isp();

	/* Phases 6-8: share a single suite_init(0) */
	DiagPrintf("[OSDI TEST] Phase 6-8: enable/disable, dynamic, color table\r\n");
	if (suite_init(0) == 0) {
		test_enable_disable_and_query();
		test_dynamic_param_change();
		test_color_table();
		suite_clean();
	}

	/* Phase 9: start/stop cycles, all ISP instances */
	DiagPrintf("[OSDI TEST] Phase 9: start/stop cycles all isp\r\n");
	test_start_stop_cycles_all_isp();

done:
	stream_close_all();
	vfs_target_unregister("fat");

	DiagPrintf("[OSDI TEST] done — PASS: %d  FAIL: %d\r\n", g_pass, g_fail);
	vTaskDelete(NULL);
}
