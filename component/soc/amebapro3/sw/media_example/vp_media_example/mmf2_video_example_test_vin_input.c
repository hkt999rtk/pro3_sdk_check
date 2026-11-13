/******************************************************************************
*
* Copyright(c) 2007 - 2025 Realtek Corporation. All rights reserved.
*
* ISP verify/input stream test: feed a file into the ISP input stream.
*
******************************************************************************/

#include <stdio.h>
#include <string.h>
#include <FreeRTOS.h>
#include <task.h>
#include <semphr.h>
#include "ameba_soc.h"
#include "rts_camera.h"
#include "rts_video.h"
#include "rts_camera_soc.h"
#include "rtscam_isp_input.h"
#include "rtsavisp.h"
#include "vfs.h"

#define TEST_VIN_INPUT_BUFS 2

struct test_vin_input_fmt {
	uint32_t isp_fmt;
	uint32_t bytes_per_pixel;
	uint8_t packed;
};

/* Mirrors the Linux test_set_vsnr.c -f table and its input size formulas. */
static const struct test_vin_input_fmt test_vin_input_fmts[9] = {
	{ 2, 2, 0 }, /* 0 ISP_INPUT_SGRBG16 */
	{ 0, 2, 0 }, /* 1 ISP_INPUT_SBGGR16 */
	{ 1, 2, 0 }, /* 2 ISP_INPUT_SGBRG16 */
	{ 3, 2, 0 }, /* 3 ISP_INPUT_SRGGB16 */
	{ 6, 4, 0 }, /* 4 ISP_INPUT_SGRBG32 */
	{ 8, 0, 1 }, /* 5 ISP_INPUT_RGB */
	{ 12, 0, 1 }, /* 6 ISP_INPUT_YUV */
	{ 16, 2, 0 }, /* 7 ISP_INPUT_YUYV */
	{ 17, 2, 0 }, /* 8 ISP_INPUT_NV16 */
};

static volatile int test_vin_input_running;
static volatile int test_vin_input_stop_flag;
static volatile int test_vin_input_task_done;
static struct rts_video_buffer *test_vin_input_bufs[TEST_VIN_INPUT_BUFS];
static rtos_task_t test_vin_input_task_handle;
static rtos_task_t test_vin_input_setup_task_handle;
static unsigned long test_vin_input_frames;

struct test_vin_input_args {
	char file[64];
	int width;
	int height;
	int fmt;
};

static struct test_vin_input_args test_vin_input_args;

extern int mmf2_video_example_test_vin_get_log_en(void);
extern void mmf2_video_example_test_vin_ensure_hw(void);

static void test_vin_input_task(void *arg)
{
	struct rts_video_buffer *vbuf;
	unsigned long frame = 0;

	(void)arg;
	while (!test_vin_input_stop_flag) {
		if (rtscam_isp_input_dequeue(&vbuf) != 0) {
			vTaskDelay(pdMS_TO_TICKS(1));
			continue;
		}

		if (mmf2_video_example_test_vin_get_log_en()) {
			DiagPrintf("[test_vin_input] frame %lu size=%u phy=0x%08x\r\n",
					   frame, vbuf->bytesused, vbuf->phy_addr);
		}
		frame++;
		test_vin_input_frames = frame;
		rtscam_isp_input_enqueue(vbuf);
	}

	test_vin_input_task_done = 1;
	test_vin_input_task_handle = NULL;
	vTaskDelete(NULL);
}

static void test_vin_input_release_buffers(void)
{
	int i;

	for (i = 0; i < TEST_VIN_INPUT_BUFS; i++) {
		if (test_vin_input_bufs[i]) {
			rtscam_video_buffer_release(RTSCAM_INPUT_STRM_IDX, test_vin_input_bufs[i]);
			test_vin_input_bufs[i] = NULL;
		}
	}
}

/*
 * Setup body: VFS/file I/O, buffer alloc, and rtscam_isp_input_set_behavior()
 * (which descends into isp_mod_input.c's mod_input_exec_set_behavior(), a
 * stack-heavy call chain through isp_notify/mod_do_action). Run on a
 * dedicated task with its own stack rather than inline on the CLI/console
 * task (LOGUART_TASK), which is too small for this call depth — matching
 * why test_vin_setup_task() in mmf2_video_example_test_vin.c does the same.
 */
static void test_vin_input_setup_task(void *arg)
{
	struct test_vin_input_args *args = arg;
	const char *file = args->file;
	int width = args->width;
	int height = args->height;
	int fmt = args->fmt;
	char filepath[64];
	FILE *fp = NULL;
	long actual_size;
	size_t bsize;
	size_t rd;
	int vfs_registered = 0;
	int ret;
	int i;

	if (test_vin_input_fmts[fmt].packed) {
		bsize = (((size_t)width * (size_t)height + 2) / 3) * 16;
	} else {
		bsize = (size_t)width * (size_t)height * test_vin_input_fmts[fmt].bytes_per_pixel;
	}

	mmf2_video_example_test_vin_ensure_hw();
	while (rts_av_isp_get_status() < RTS_ISP_RUNNING) {
		vTaskDelay(pdMS_TO_TICKS(10));
	}

	ret = rtscam_video_open(RTSCAM_INPUT_STRM_IDX);
	if (ret != 0) {
		DiagPrintf("[test_vin_input] video open fail %d\r\n", ret);
		test_vin_input_setup_task_handle = NULL;
		vTaskDelete(NULL);
	}

	vfs_init();
	ret = vfs_target_register("fat", VFS_TARGET_SD, VFS_RW);
	if (ret != 0) {
		DiagPrintf("[test_vin_input] VFS-FAT init fail\r\n");
		rtscam_video_close(RTSCAM_INPUT_STRM_IDX);
		test_vin_input_setup_task_handle = NULL;
		vTaskDelete(NULL);
	}
	vfs_registered = 1;

	snprintf(filepath, sizeof(filepath), "fat:%s", file);
	fp = fopen(filepath, "r");
	if (!fp) {
		DiagPrintf("[test_vin_input] fopen(%s) failed\r\n", filepath);
		goto fail;
	}
	fseek(fp, 0, SEEK_END);
	actual_size = ftell(fp);
	if (actual_size < 0 || (size_t)actual_size != bsize) {
		DiagPrintf("[test_vin_input] file size mismatch: expected %u, actual %ld\r\n",
				   (unsigned)bsize, actual_size);
		goto fail;
	}
	fseek(fp, 0, SEEK_SET);
	for (i = 0; i < TEST_VIN_INPUT_BUFS; i++) {
		test_vin_input_bufs[i] = rtscam_video_buffer_alloc(RTSCAM_INPUT_STRM_IDX,
								 RTS_MEMORY_KERNEL, bsize);
		if (!test_vin_input_bufs[i]) {
			DiagPrintf("[test_vin_input] buffer alloc %d failed\r\n", i);
			goto fail;
		}
		test_vin_input_bufs[i]->bytesused = bsize;
	}

	rd = fread(test_vin_input_bufs[0]->vm_addr, 1, bsize, fp);
	if (rd != bsize) {
		DiagPrintf("[test_vin_input] fread failed, read %u of %u bytes\r\n",
				   (unsigned)rd, (unsigned)bsize);
		goto fail;
	}
	memcpy(test_vin_input_bufs[1]->vm_addr, test_vin_input_bufs[0]->vm_addr, bsize);
	fclose(fp);
	fp = NULL;
	vfs_target_unregister("fat");
	vfs_registered = 0;

	for (i = 0; i < TEST_VIN_INPUT_BUFS; i++) {
		ret = rtscam_isp_input_enqueue(test_vin_input_bufs[i]);
		if (ret != 0) {
			DiagPrintf("[test_vin_input] enqueue %d failed %d\r\n", i, ret);
			goto fail_stream;
		}
	}
	ret = rtscam_isp_input_set_behavior(1, test_vin_input_fmts[fmt].isp_fmt,
										width, height, 1, 5);
	if (ret != 0) {
		DiagPrintf("[test_vin_input] set behavior failed %d\r\n", ret);
		goto fail_stream;
	}
	ret = rtscam_video_stream_start(RTSCAM_INPUT_STRM_IDX);
	if (ret != 0) {
		DiagPrintf("[test_vin_input] stream start failed %d\r\n", ret);
		goto fail_stream;
	}

	test_vin_input_frames = 0;
	test_vin_input_stop_flag = 0;
	test_vin_input_task_done = 0;
	test_vin_input_running = 1;
	ret = rtos_task_create(&test_vin_input_task_handle, "vin_input",
						   test_vin_input_task, NULL, 4096, 1);
	if (ret != 0) {
		DiagPrintf("[test_vin_input] task create failed %d\r\n", ret);
		test_vin_input_running = 0;
		rtscam_video_stream_stop(RTSCAM_INPUT_STRM_IDX);
		test_vin_input_release_buffers();
		rtscam_video_close(RTSCAM_INPUT_STRM_IDX);
		goto done;
	}
	DiagPrintf("[test_vin_input] started %dx%d fmt=%d size=%u\r\n",
			   width, height, fmt, (unsigned)bsize);
	goto done;

fail_stream:
	if (fp) {
		fclose(fp);
	}
	if (vfs_registered) {
		vfs_target_unregister("fat");
	}
	test_vin_input_release_buffers();
	rtscam_video_close(RTSCAM_INPUT_STRM_IDX);
	goto done;

fail:
	if (fp) {
		fclose(fp);
	}
	if (vfs_registered) {
		vfs_target_unregister("fat");
	}
	test_vin_input_release_buffers();
	rtscam_video_close(RTSCAM_INPUT_STRM_IDX);

done:
	test_vin_input_setup_task_handle = NULL;
	vTaskDelete(NULL);
}

void mmf2_video_example_test_vin_input(const char *file, int width, int height, int fmt)
{
	if (fmt < 0 || fmt > 8) {
		DiagPrintf("[test_vin_input] invalid fmt %d, valid range is 0-8\r\n", fmt);
		return;
	}
	if (!file || !file[0]) {
		DiagPrintf("[test_vin_input] file is required\r\n");
		return;
	}
	if (test_vin_input_running || test_vin_input_setup_task_handle) {
		DiagPrintf("[test_vin_input] already running\r\n");
		return;
	}

	snprintf(test_vin_input_args.file, sizeof(test_vin_input_args.file), "%s", file);
	test_vin_input_args.width = width;
	test_vin_input_args.height = height;
	test_vin_input_args.fmt = fmt;

	if (rtos_task_create(&test_vin_input_setup_task_handle, "vin_input_setup",
						 test_vin_input_setup_task, &test_vin_input_args, 4096, 1) != 0) {
		DiagPrintf("[test_vin_input] setup task create failed\r\n");
		test_vin_input_setup_task_handle = NULL;
	}
}

void mmf2_video_example_test_vin_input_set_large_image_mode(int enable)
{
	int ret = rts_isp_set_large_image_mode(ISP0, enable ? 1 : 0);

	DiagPrintf("[test_vin_input] large_image_mode %s: ret=%d\r\n",
			   enable ? "on" : "off", ret);
}

void mmf2_video_example_test_vin_input_stop(void)
{
	if (!test_vin_input_running) {
		DiagPrintf("[test_vin_input] nothing running\r\n");
		return;
	}

	test_vin_input_stop_flag = 1;
	vTaskDelay(pdMS_TO_TICKS(50));
	rtscam_video_stream_stop(RTSCAM_INPUT_STRM_IDX);
	test_vin_input_release_buffers();
	rtscam_video_close(RTSCAM_INPUT_STRM_IDX);
	DiagPrintf("[test_vin_input] stopped: frames=%lu drops=%lu\r\n",
			   test_vin_input_frames, rtscam_isp_input_get_drop_count());
	test_vin_input_running = 0;
	test_vin_input_task_done = 0;
	test_vin_input_task_handle = NULL;
}
