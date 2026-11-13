/*
 * Realtek Semiconductor Corp.
 *
 * Raw-tap debug/test tool for FreeRTOS, exercising rts_raw.c against a
 * dedicated video channel this tool opens and tears down itself.
 *
 * Usage:
 *   VIDEO dbg_raw <raw_fmt> [--save <file>] [--nhead] [--skip <n>] [--no-freeze2a]
 * Example:
 *   @video dbg_raw 4 --save l_blc_b.raw --nhead
 * raw_fmt      : tap point, see enum rts_raw_src in rts_raw/rts_raw.h
 * --save <file>: save the captured frame to this path on the SD card
 *                (mounted as "fat:"); omit to just tap and log, no file write
 * --nhead      : when saving, omit the RTS_RAW_HEADER_LENGTH-byte header
 * --skip <n>   : frames to skip after retapping before capturing (default 10)
 * --no-freeze2a: don't freeze exposure/AWB while capturing (default: frozen)
 *
 * Only channel 0 supports raw tap-off on this SoC, so the channel is fixed.
 * Must not run concurrently with "VIDEO run"/"VIDEO vin" — this tool does
 * its own ISP_Init()/isp_initial()/rtscam_video_init() bring-up, same as
 * mmf2_video_example_test_vin.c, and is meant to run standalone on a fresh
 * boot.
 */

#include <FreeRTOS.h>
#include <task.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ameba_soc.h"
#include "hal_isp.h"
#include "isp_common.h"
#include "rts_camera.h"
#include "rts_video.h"
#include "rts_isp.h"
#include "rts_raw/rts_raw.h"
#include "video_app_rcc.h"
#include "vfs.h"

#define DBG_RAW_LOG(fmt, ...) DiagPrintf("[dbg_raw] " fmt "\r\n", ##__VA_ARGS__)

/* Only channel 0 supports raw tap-off on this SoC. */
#define DBG_RAW_STREAMID 0
#define DBG_RAW_BUFS     1

/*
 * The channel is always configured as V4L2_PIX_FMT_128BIT_3PIXEL for raw
 * capture (see rts_init_raw()), regardless of which tap point is active.
 * rts_camera.c's __rtscam_calc_sizeimage() special-cases this fourcc as
 * "3 pixels packed per 128 bits" — (w*h+2)/3*16 — matching the largest
 * entry in rts_raw.c's per-tap-point length table (the CCM_B/UVT_B/WDR_A/
 * YUV444TO422_B taps). rtscam_video_buffer_alloc() enforces that as a
 * floor, so buffers are sized off this formula rather than praw->length,
 * which is smaller for the RAW16/RAW32 tap points.
 *
 * KNOWN LIMITATION / TODO: since every raw_fmt shares the single R128
 * channel registration, this floor is paid on every capture regardless
 * of tap point — RAW16 taps (praw->length = w*h*2) only need ~37% of
 * this buffer, RAW32 taps (w*h*4) only ~75%; the rest is wasted padding.
 * The 128-bit/3-pixel packing is only the ISP's native layout for the
 * 3-channel 12-bit taps (RGB12/YUV444 12-bit: CCM_B/UVT_B/WDR_A/
 * YUV444TO422_B) — 3 pixels x 3 channels x 12 bits = 108 of the 128 bits,
 * matching praw->eff_bits. RAW16/RAW32 taps are single-channel and don't
 * naturally fit that layout, so a tighter allocation for them would need
 * the channel registered/configured with a pixelformat matching their
 * actual bit depth instead of always V4L2_PIX_FMT_128BIT_3PIXEL. Deferred.
 */
static uint32_t dbg_raw_alloc_size(uint32_t width, uint32_t height)
{
	return (width * height + 2) / 3 * 16;
}

/* Forward declaration from rtsavisp.h (not on this file's include path) */
#define RTS_ISP_RUNNING 2  /* enum rts_isp_status: UNINITIALIZED=0, STOPPED=1, RUNNING=2 */
extern int32_t rts_av_isp_get_status(void);

struct dbg_raw_params {
	int raw_fmt;
	int skip_count;
	int freeze2a;
	int nhead;
	int saveflag;
	char savepath[128];
};

static void dbg_raw_save(struct rts_raw_t *praw, struct rts_video_buffer *vbuf,
						 const char *savepath, int nhead)
{
	char filepath[160];
	FILE *fp;
	int ret;

	vfs_init();
	ret = vfs_target_register("fat", VFS_TARGET_SD, VFS_RW);
	if (ret != 0) {
		DBG_RAW_LOG("VFS-FAT init fail, not saving");
		return;
	}

	snprintf(filepath, sizeof(filepath), "fat:%s", savepath);
	fp = fopen(filepath, "wb");
	if (!fp) {
		DBG_RAW_LOG("fopen(%s) fail, not saving", filepath);
		goto unreg;
	}

	if (!nhead) {
		fwrite(praw->header, 1, RTS_RAW_HEADER_LENGTH, fp);
	}
	/* vbuf->bytesused mirrors the R128 buffer/DMA size, not the tap
	 * point's real data length — use praw->length, same as Linux
	 * dbg_raw.c's save_raw_data(). */
	fwrite(vbuf->vm_addr, 1, praw->length, fp);
	fclose(fp);

	DBG_RAW_LOG("saved %u bytes to %s%s", praw->length, filepath,
				nhead ? " (no header)" : "");

unreg:
	vfs_target_unregister("fat");
}

static void dbg_raw_task(void *param)
{
	struct dbg_raw_params *p = (struct dbg_raw_params *)param;
	int raw_fmt = p->raw_fmt;
	int skip_count = p->skip_count;
	int freeze2a = p->freeze2a;
	int nhead = p->nhead;
	int saveflag = p->saveflag;
	char savepath[sizeof(p->savepath)];
	struct rts_raw_t *praw = NULL;
	struct rts_video_format fmt;
	struct rts_video_buffer *bufs[DBG_RAW_BUFS] = {NULL};
	struct rts_video_buffer *vbuf = NULL;
	struct isp_v4l2_control ctrl;
	int exp_mode_saved = 0;
	int awb_saved = 0;
	int max_iters;
	int got = 0;
	int i, j;
	int ret;
	uint32_t alloc_size;

	memcpy(savepath, p->savepath, sizeof(savepath));
	vPortFree(p);
	p = NULL;

	while (rts_av_isp_get_status() < RTS_ISP_RUNNING) {
		vTaskDelay(pdMS_TO_TICKS(10));
	}

	ret = rts_init_raw((unsigned int)raw_fmt, DBG_RAW_STREAMID, &praw);
	if (ret) {
		DBG_RAW_LOG("rts_init_raw fail (%d)", ret);
		goto done;
	}

	ret = rtscam_video_open(DBG_RAW_STREAMID);
	if (ret) {
		DBG_RAW_LOG("rtscam_video_open fail (%d)", ret);
		goto release_raw;
	}

	memset(&fmt, 0, sizeof(fmt));
	fmt.pixelformat = praw->fmt;
	fmt.width = praw->width;
	fmt.height = praw->height;
	ret = rtscam_video_set_format(DBG_RAW_STREAMID, &fmt);
	if (ret) {
		DBG_RAW_LOG("rtscam_video_set_format fail (%d)", ret);
		goto close_ch;
	}

	alloc_size = dbg_raw_alloc_size(praw->width, praw->height);
	for (i = 0; i < DBG_RAW_BUFS; i++) {
		bufs[i] = rtscam_video_buffer_alloc(DBG_RAW_STREAMID,
											RTS_MEMORY_KERNEL, alloc_size);
		if (!bufs[i]) {
			DBG_RAW_LOG("buffer_alloc[%d] fail", i);
			goto free_bufs;
		}
		if (rtscam_video_enqueue(DBG_RAW_STREAMID, bufs[i])) {
			DBG_RAW_LOG("enqueue[%d] fail", i);
			goto free_bufs;
		}
	}

	if (freeze2a) {
		memset(&ctrl, 0, sizeof(ctrl));
		ctrl.id = ISP_V4L2_CID_AUTOGAIN;
		ret = rts_isp_v4l2_g_ctrl(ISP0, &ctrl);
		if (ret) {
			DBG_RAW_LOG("get exposure mode fail (%d)", ret);
			goto free_bufs;
		}
		exp_mode_saved = ctrl.value;
		if (ctrl.value) {
			ctrl.value = 0;
			ret = rts_isp_v4l2_s_ctrl(ISP0, &ctrl);
			if (ret) {
				DBG_RAW_LOG("freeze exposure mode fail (%d)", ret);
				exp_mode_saved = 0;
				goto restore_2a;
			}
		}

		memset(&ctrl, 0, sizeof(ctrl));
		ctrl.id = ISP_V4L2_CID_AUTO_WHITE_BALANCE;
		ret = rts_isp_v4l2_g_ctrl(ISP0, &ctrl);
		if (ret) {
			DBG_RAW_LOG("get AWB ctrl fail (%d)", ret);
			goto restore_2a;
		}
		awb_saved = ctrl.value;
		if (ctrl.value) {
			ctrl.value = 0;
			ret = rts_isp_v4l2_s_ctrl(ISP0, &ctrl);
			if (ret) {
				DBG_RAW_LOG("freeze AWB fail (%d)", ret);
				awb_saved = 0;
				goto restore_2a;
			}
		}
	}

	ret = rts_config_raw(praw);
	if (ret) {
		DBG_RAW_LOG("rts_config_raw fail (%d)", ret);
		goto restore_2a;
	}

	ret = rtscam_video_stream_start(DBG_RAW_STREAMID);
	if (ret) {
		DBG_RAW_LOG("stream_start fail (%d)", ret);
		goto disable_raw;
	}

	/* Budget ~200ms/frame plus a skip_count-sized margin, polled every 10ms. */
	max_iters = (skip_count + 20) * 20;
	j = 0;
	for (i = 0; i < max_iters; i++) {
		vTaskDelay(pdMS_TO_TICKS(10));
		if (rtscam_video_dequeue(DBG_RAW_STREAMID, &vbuf) != 0) {
			continue;
		}
		j++;
		if (j > skip_count) {
			got = 1;
			break;
		}
		rtscam_video_enqueue(DBG_RAW_STREAMID, vbuf);
		vbuf = NULL;
	}

	if (!got) {
		DBG_RAW_LOG("timed out waiting for a raw frame");
		goto stream_stop;
	}

	/* vbuf->bytesused is not meaningful here — it mirrors the R128
	 * buffer/DMA size (alloc_size), not the tap point's real data
	 * length (praw->length is what actually gets saved). */
	DBG_RAW_LOG("captured raw_fmt=%d %ux%u tap_len=%u buf_alloc=%u",
				raw_fmt, praw->width, praw->height, praw->length, alloc_size);

	if (saveflag) {
		dbg_raw_save(praw, vbuf, savepath, nhead);
	}

	rtscam_video_enqueue(DBG_RAW_STREAMID, vbuf);
	vbuf = NULL;

stream_stop:
	rtscam_video_stream_stop(DBG_RAW_STREAMID);
disable_raw:
	rts_disable_raw(praw);
restore_2a:
	if (exp_mode_saved) {
		ctrl.id = ISP_V4L2_CID_AUTOGAIN;
		ctrl.value = exp_mode_saved;
		rts_isp_v4l2_s_ctrl(ISP0, &ctrl);
	}
	if (awb_saved) {
		ctrl.id = ISP_V4L2_CID_AUTO_WHITE_BALANCE;
		ctrl.value = awb_saved;
		rts_isp_v4l2_s_ctrl(ISP0, &ctrl);
	}
free_bufs:
	for (i = 0; i < DBG_RAW_BUFS; i++) {
		if (bufs[i]) {
			rtscam_video_buffer_release(DBG_RAW_STREAMID, bufs[i]);
		}
	}
close_ch:
	rtscam_video_close(DBG_RAW_STREAMID);
release_raw:
	rts_release_raw(praw);
done:
	vTaskDelete(NULL);
}

void mmf2_video_example_dbg_raw_init(int argc, u8 **argv)
{
	static struct isp_io io = {0};
	struct dbg_raw_params *p;
	int i;

	if (argc < 1) {
		DBG_RAW_LOG("Usage: VIDEO dbg_raw <raw_fmt> [--save <file>] [--nhead] [--skip <n>] [--no-freeze2a]");
		return;
	}

	p = (struct dbg_raw_params *)pvPortMalloc(sizeof(*p));
	if (!p) {
		DBG_RAW_LOG("ERROR: out of memory");
		return;
	}

	p->raw_fmt = (int)strtol((char *)argv[0], NULL, 0);
	p->skip_count = 10;
	p->freeze2a = 1;
	p->nhead = 0;
	p->saveflag = 0;
	p->savepath[0] = '\0';

	if (p->raw_fmt < 0 || p->raw_fmt >= RTS_RAW_SRC_RESERVED) {
		DBG_RAW_LOG("ERROR: raw_fmt must be 0-%d", RTS_RAW_SRC_RESERVED - 1);
		vPortFree(p);
		return;
	}

	for (i = 1; i < argc; i++) {
		const char *arg = (const char *)argv[i];

		if (_strcmp(arg, "--save") == 0 && i + 1 < argc) {
			strncpy(p->savepath, (char *)argv[++i], sizeof(p->savepath) - 1);
			p->savepath[sizeof(p->savepath) - 1] = '\0';
			p->saveflag = 1;
		} else if (_strcmp(arg, "--nhead") == 0) {
			p->nhead = 1;
		} else if (_strcmp(arg, "--skip") == 0 && i + 1 < argc) {
			p->skip_count = (int)strtol((char *)argv[++i], NULL, 0);
		} else if (_strcmp(arg, "--no-freeze2a") == 0) {
			p->freeze2a = 0;
		}
	}

	video_app_common_rcc_init();
	ISP_Init();
	if (isp_initial(&io)) {
		DBG_RAW_LOG("ERROR: isp_initial fail");
		vPortFree(p);
		return;
	}
	rtscam_video_init();

	if (xTaskCreate(dbg_raw_task, "dbg_raw", 4096, p,
					tskIDLE_PRIORITY + 1, NULL) != pdPASS) {
		DBG_RAW_LOG("ERROR: failed to create dbg_raw task");
		vPortFree(p);
	}
}
