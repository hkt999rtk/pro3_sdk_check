/*
 * Realtek Semiconductor Corp.
 *
 * OSDI overlay test example for FreeRTOS.
 *
 * Usage:
 *   VIDEO osdi_test <ch> [--blk <n>] [--pic <file> <WxH> <mode>] [--start <sx> <sy>]
 *   VIDEO osdi_stop <ch>
 *
 * ch   : ISP channel (0-2); must match the channel started by VIDEO run
 * --blk: block index (0-5), default 0
 * --pic: raw pixel file on SD card, block WxH, pixel format mode (0-5)
 *        mode: 0=1BPP 1=RGBA1111 2=RGBA2222 3=RGBA4444 4=RGBA5551 5=RGBA8888
 *        if omitted or file missing, a solid-color fallback pattern is used
 * --start: block top-left position, default 0 0
 */

#include "FreeRTOS.h"
#include "task.h"
#include "vfs.h"
#include "diag.h"
#include <string.h>
#include <stdlib.h>
#include "hal_osdi.h"
#include "rts_camera.h"

#define OSDI_LOG(fmt, ...)  DiagPrintf("[OSDI Test] " fmt "\r\n", ##__VA_ARGS__)

/* default block dimensions when --pic is not provided */
#define OSDI_DFT_BLK_W   320
#define OSDI_DFT_BLK_H   240

/* fallback solid color: opaque red in RGBA8888 */
#define OSDI_FALLBACK_COLOR  0xFF0000FFU

/* per-channel persistent OSDI handles */
static RtsOsdiInst g_osdi[3];

struct osdi_test_params {
	int channel;
	int blk_idx;
	char pic_file[128];
	int blk_w;
	int blk_h;
	int blk_x;
	int blk_y;
	int mode;   /* enum rts_pix_fmt_e value */
};

static void *__load_pic_from_sd(const char *filename, uint32_t *out_len)
{
	char path[160];
	void *buf = NULL;
	long sz;
	FILE *fp;

	vfs_target_register("fat", VFS_TARGET_SD, VFS_RW);

	snprintf(path, sizeof(path), "fat:%s", filename);
	fp = fopen(path, "rb");
	if (!fp) {
		goto unreg;
	}

	fseek(fp, 0, SEEK_END);
	sz = ftell(fp);
	rewind(fp);

	if (sz <= 0) {
		goto close;
	}

	buf = pvPortMalloc((size_t)sz);
	if (!buf) {
		goto close;
	}

	if ((long)fread(buf, 1, (size_t)sz, fp) != sz) {
		vPortFree(buf);
		buf = NULL;
		goto close;
	}
	*out_len = (uint32_t)sz;

close:
	fclose(fp);
unreg:
	vfs_target_unregister("fat");
	return buf;
}

static void *__make_fallback_pattern(int w, int h, uint32_t *out_len)
{
	uint32_t len = (uint32_t)(w * h) * 4; /* RGBA8888 */
	uint32_t *buf = pvPortMalloc(len);
	uint32_t i;

	if (!buf) {
		return NULL;
	}

	for (i = 0; i < (uint32_t)(w * h); i++) {
		buf[i] = OSDI_FALLBACK_COLOR;
	}

	*out_len = len;
	return buf;
}

static void osdi_test_task(void *param)
{
	struct osdi_test_params *p = (struct osdi_test_params *)param;
	int channel  = p->channel;
	int blk_idx  = p->blk_idx;
	int blk_w    = p->blk_w;
	int blk_h    = p->blk_h;
	int blk_x    = p->blk_x;
	int blk_y    = p->blk_y;
	int mode     = p->mode;
	char pic_file[128];
	void *pic_data = NULL;
	uint32_t pic_len = 0;
	int ret;
	int is_new_init = (g_osdi[channel] == NULL);

	strncpy(pic_file, p->pic_file, sizeof(pic_file) - 1);
	pic_file[sizeof(pic_file) - 1] = '\0';
	vPortFree(p);
	p = NULL;

	/* --- load picture data --- */
	if (pic_file[0] != '\0') {
		pic_data = __load_pic_from_sd(pic_file, &pic_len);
		if (!pic_data) {
			OSDI_LOG("File '%s' not found or read failed, using fallback pattern", pic_file);
		}
	}

	if (!pic_data) {
		OSDI_LOG("Generating solid color fallback pattern (%dx%d RGBA8888)", blk_w, blk_h);
		mode = RTS_PIX_FMT_RGBA8888;
		pic_data = __make_fallback_pattern(blk_w, blk_h, &pic_len);
		if (!pic_data) {
			OSDI_LOG("ERROR: failed to allocate fallback pattern");
			vTaskDelete(NULL);
			return;
		}
	}

	/* --- init path (first call on this channel) --- */
	if (!g_osdi[channel]) {
		struct rts_video_format fmt = {0};
		struct rts_osdi_frame_info frame = {0};

		ret = rts_osdi_init(&g_osdi[channel], channel);
		if (ret) {
			OSDI_LOG("rts_osdi_init ch%d failed (%d)", channel, ret);
			goto done;
		}

		/* disable all hardware blocks to clear stale register state */
		{
			int bi;
			for (bi = 0; bi < RTS_OSDI_MAX_BLK_NUM; bi++) {
				rts_osdi_disable_block(g_osdi[channel], bi);
			}
		}

		if (rtscam_video_get_format((unsigned int)channel, &fmt) == 0 &&
			fmt.width && fmt.height) {
			frame.width  = (uint16_t)fmt.width;
			frame.height = (uint16_t)fmt.height;
		} else {
			OSDI_LOG("WARNING: could not query frame size for ch%d, defaulting to 1920x1080", channel);
			frame.width  = 1920;
			frame.height = 1080;
		}

		ret = rts_osdi_config_frame(g_osdi[channel], &frame);
		if (ret) {
			OSDI_LOG("rts_osdi_config_frame failed (%d)", ret);
			rts_osdi_release(g_osdi[channel]);
			g_osdi[channel] = NULL;
			goto done;
		}
	}

	/* --- configure block (init and update paths) --- */
	{
		struct rts_osdi_blk block;

		memset(&block, 0, sizeof(block));
		block.index       = (uint8_t)blk_idx;
		block.enable      = RTS_TRUE;
		block.fmt         = (uint32_t)mode;
		block.pdata       = pic_data;
		block.length      = pic_len;
		block.width       = (uint16_t)blk_w;
		block.height      = (uint16_t)blk_h;
		block.start_x     = (uint16_t)blk_x;
		block.start_y     = (uint16_t)blk_y;
		block.fixed_buf_len = 0;

		ret = rts_osdi_config_block(g_osdi[channel], blk_idx, &block);
		if (ret) {
			OSDI_LOG("rts_osdi_config_block blk%d failed (%d)", blk_idx, ret);
			goto done;
		}
	}

	/* start + enable only on first init; updates just need config_block */
	if (is_new_init) {
		ret = rts_osdi_start(g_osdi[channel]);
		if (ret) {
			OSDI_LOG("rts_osdi_start failed (%d)", ret);
		}

		ret = rts_osdi_enable_block(g_osdi[channel], blk_idx);
		if (ret) {
			OSDI_LOG("rts_osdi_enable_block blk%d failed (%d)", blk_idx, ret);
		}
	}

	OSDI_LOG("ch%d blk%d overlay active (%dx%d at %d,%d fmt=%d)",
			 channel, blk_idx, blk_w, blk_h, blk_x, blk_y, mode);

done:
	/* pic_data can be freed here: config_block already memcpy'd it into DMA memory */
	vPortFree(pic_data);
	vTaskDelete(NULL);
}

void mmf2_video_example_osdi_test_init(int argc, u8 **argv)
{
	struct osdi_test_params *p;
	int i;

	if (argc < 1) {
		OSDI_LOG("Usage: VIDEO osdi_test <ch> [--blk n] [--pic file WxH mode] [--start sx sy]");
		return;
	}

	p = (struct osdi_test_params *)pvPortMalloc(sizeof(*p));
	if (!p) {
		OSDI_LOG("ERROR: out of memory");
		return;
	}

	/* defaults */
	p->channel  = (int)strtol((char *)argv[0], NULL, 0);
	p->blk_idx  = 0;
	p->pic_file[0] = '\0';
	p->blk_w    = OSDI_DFT_BLK_W;
	p->blk_h    = OSDI_DFT_BLK_H;
	p->blk_x    = 0;
	p->blk_y    = 0;
	p->mode     = RTS_PIX_FMT_RGBA8888;

	if (p->channel < 0 || p->channel > 2) {
		OSDI_LOG("ERROR: channel must be 0-2");
		vPortFree(p);
		return;
	}

	for (i = 1; i < argc; i++) {
		const char *arg = (const char *)argv[i];

		if (_strcmp(arg, "--blk") == 0 && i + 1 < argc) {
			p->blk_idx = (int)strtol((char *)argv[++i], NULL, 0);
		} else if (_strcmp(arg, "--pic") == 0 && i + 3 < argc) {
			strncpy(p->pic_file, (char *)argv[++i], sizeof(p->pic_file) - 1);
			p->pic_file[sizeof(p->pic_file) - 1] = '\0';
			if (sscanf((char *)argv[++i], "%dx%d", &p->blk_w, &p->blk_h) != 2) {
				OSDI_LOG("ERROR: bad WxH format, expected e.g. 320x240");
				vPortFree(p);
				return;
			}
			p->mode = (int)strtol((char *)argv[++i], NULL, 0);
		} else if (_strcmp(arg, "--start") == 0 && i + 2 < argc) {
			p->blk_x = (int)strtol((char *)argv[++i], NULL, 0);
			p->blk_y = (int)strtol((char *)argv[++i], NULL, 0);
		}
	}

	if (xTaskCreate(osdi_test_task, "osdi_test", 4096, p,
					tskIDLE_PRIORITY + 1, NULL) != pdPASS) {
		OSDI_LOG("ERROR: failed to create osdi_test task");
		vPortFree(p);
	}
}

void mmf2_video_example_osdi_stop(int channel)
{
	if (channel < 0 || channel > 2) {
		OSDI_LOG("ERROR: channel must be 0-2");
		return;
	}
	if (!g_osdi[channel]) {
		OSDI_LOG("ch%d not active", channel);
		return;
	}
	rts_osdi_frame_stop(g_osdi[channel]);
	rts_osdi_release(g_osdi[channel]);
	g_osdi[channel] = NULL;
	OSDI_LOG("ch%d stopped", channel);
}
