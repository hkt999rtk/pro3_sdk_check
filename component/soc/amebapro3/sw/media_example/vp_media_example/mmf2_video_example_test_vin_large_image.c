#include <stdio.h>
#include <string.h>
#include <FreeRTOS.h>
#include <task.h>
#include "ameba_soc.h"
#include "rts_camera.h"
#include "rts_video.h"
#include "rts_camera_soc.h"
#include "rtscam_isp_input.h"
#include "rtscam_isp_command.h"
#include "rtsavisp.h"
#include "vfs.h"

#define LARGE_IMAGE_BUFS 2
#define LARGE_IMAGE_WIDTH 1280
#define LARGE_IMAGE_HEIGHT 2880
#define LARGE_IMAGE_BSIZE (LARGE_IMAGE_WIDTH * LARGE_IMAGE_HEIGHT * 2)
/* Stream 0: RAW capture output, frame-size-frozen at 2560x1440. */
#define LARGE_IMAGE_STREAM0_WIDTH 2568
#define LARGE_IMAGE_STREAM0_HEIGHT 1448
/* Stream 1: processed output at the injected large-image geometry itself
 * (LARGE_IMAGE_WIDTH/HEIGHT above) so its ISP output mirrors the raw frame
 * fed through the TNR injection path. */
#define LARGE_IMAGE_STREAM1_WIDTH LARGE_IMAGE_WIDTH
#define LARGE_IMAGE_STREAM1_HEIGHT LARGE_IMAGE_HEIGHT
#define LARGE_IMAGE_STREAM_BUFS 2

static volatile int large_image_running;
static volatile int large_image_stop_flag;
static rtos_task_t large_image_task_handle;
static rtos_task_t large_image_setup_task_handle;
static struct rts_video_buffer *large_image_input_bufs[LARGE_IMAGE_BUFS];
static struct rts_video_buffer *large_image_stream_bufs[LARGE_IMAGE_STREAM_BUFS];
static struct rts_video_buffer *large_image_stream1_bufs[LARGE_IMAGE_STREAM_BUFS];
static unsigned long large_image_frames;
static char large_image_file[64];
/*
 * Capture is requested by the console command but performed by the loop task
 * (large_image_input_task), because the console/LOGUART task stack is too
 * small for the deep FatFs -> SD call chain in large_image_save_to_sd(). The
 * loop task runs with a 4096-word stack, matching test_vin's deferred-capture
 * pattern.
 */
static volatile int large_image_capture_request;
static char large_image_capture_suffix[32];

extern void mmf2_video_example_test_vin_ensure_hw(void);
extern int mmf2_video_example_test_vin_get_log_en(void);

static void large_image_release_input_buffers(void)
{
	int i;

	for (i = 0; i < LARGE_IMAGE_BUFS; i++) {
		if (large_image_input_bufs[i]) {
			rtscam_video_buffer_release(RTSCAM_INPUT_STRM_IDX,
										large_image_input_bufs[i]);
			large_image_input_bufs[i] = NULL;
		}
	}
}

static void large_image_release_stream_buffers(void)
{
	int i;

	for (i = 0; i < LARGE_IMAGE_STREAM_BUFS; i++) {
		if (large_image_stream_bufs[i]) {
			rtscam_video_buffer_release(0, large_image_stream_bufs[i]);
			large_image_stream_bufs[i] = NULL;
		}
	}
	for (i = 0; i < LARGE_IMAGE_STREAM_BUFS; i++) {
		if (large_image_stream1_bufs[i]) {
			rtscam_video_buffer_release(1, large_image_stream1_bufs[i]);
			large_image_stream1_bufs[i] = NULL;
		}
	}
}

/* Forward decl: defined after large_image_save_to_sd(). */
static void large_image_do_capture(const char *suffix);

static void large_image_input_task(void *arg)
{
	struct rts_video_buffer *vbuf;
	unsigned long frame = 0;

	(void)arg;
	while (!large_image_stop_flag) {
		/*
		 * Service a pending capture request here rather than in the
		 * console task: the SD/FatFs write chain needs this task's
		 * larger stack.
		 */
		if (large_image_capture_request) {
			large_image_do_capture(large_image_capture_suffix);
			large_image_capture_request = 0;
		}
		if (rtscam_isp_input_dequeue(&vbuf) != 0) {
			vTaskDelay(pdMS_TO_TICKS(1));
			continue;
		}
		if (mmf2_video_example_test_vin_get_log_en()) {
			DiagPrintf("[large_image_test] frame %lu size=%u phy=0x%08x\r\n",
					   frame, vbuf->bytesused, vbuf->phy_addr);
		}
		frame++;
		large_image_frames = frame;
		rtscam_isp_input_enqueue(vbuf);
	}
	large_image_task_handle = NULL;
	vTaskDelete(NULL);
}

static int large_image_check_file(const char *file)
{
	char filepath[64];
	FILE *fp;
	long actual_size;
	int ret;

	vfs_init();
	ret = vfs_target_register("fat", VFS_TARGET_SD, VFS_RW);
	if (ret != 0) {
		DiagPrintf("[large_image_test] VFS-FAT init fail\r\n");
		return -1;
	}
	snprintf(filepath, sizeof(filepath), "fat:%s", file);
	fp = fopen(filepath, "r");
	if (!fp) {
		DiagPrintf("[large_image_test] fopen(%s) failed\r\n", filepath);
		vfs_target_unregister("fat");
		return -1;
	}
	fseek(fp, 0, SEEK_END);
	actual_size = ftell(fp);
	fclose(fp);
	vfs_target_unregister("fat");
	if (actual_size < 0 || (size_t)actual_size != LARGE_IMAGE_BSIZE) {
		DiagPrintf("[large_image_test] file size mismatch: expected %u, actual %ld\r\n",
				   (unsigned)LARGE_IMAGE_BSIZE, actual_size);
		return -1;
	}
	return 0;
}

static void large_image_save_to_sd(void *data, uint32_t size, const char *filename)
{
	char filepath[64];
	FILE *fp;
	int ret;

	vfs_init();
	ret = vfs_target_register("fat", VFS_TARGET_SD, VFS_RW);
	if (ret != 0) {
		DiagPrintf("[large_image_test] VFS-FAT init fail\r\n");
		return;
	}
	snprintf(filepath, sizeof(filepath), "fat:%s", filename);
	fp = fopen(filepath, "wb");
	if (!fp) {
		DiagPrintf("[large_image_test] fopen(%s) failed\r\n", filepath);
		vfs_target_unregister("fat");
		return;
	}
	ret = fwrite(data, 1, size, fp);
	if (ret != (int)size) {
		DiagPrintf("[large_image_test] fwrite() failed, wrote %d of %u bytes\r\n",
				   ret, size);
	} else {
		DiagPrintf("[large_image_test] fwrite() succeeded, wrote %u bytes to %s\r\n",
				   size, filepath);
	}
	fclose(fp);
	vfs_target_unregister("fat");
}

static void large_image_setup_task(void *arg)
{
	const char *file = arg;
	struct rts_video_format format;
	char filepath[64];
	FILE *fp = NULL;
	size_t rd;
	int ret;
	int i;
	int vfs_registered = 0;

	/* Validate before touching ISP, stream, or driver buffers. */
	if (large_image_check_file(file) != 0) {
		goto done;
	}

	mmf2_video_example_test_vin_ensure_hw();
	while (rts_av_isp_get_status() < RTS_ISP_RUNNING) {
		vTaskDelay(pdMS_TO_TICKS(10));
	}

	ret = rts_isp_set_large_image_mode(ISP0, 1);
	if (ret != 0) {
		DiagPrintf("[large_image_test] large image mode failed %d\r\n", ret);
		goto done;
	}

	/* Bring up and start the input path FIRST, before any capture stream
	 * is opened. Starting the input stream drives the sensor into the
	 * large-image geometry, which makes the ISP server force a SoC video
	 * detach+attach (force_reset). That attach loop re-inits every
	 * capture stream's open state, so any capture stream opened before
	 * this point would have its handle wiped out from under it.
	 */
	ret = rtscam_video_open(RTSCAM_INPUT_STRM_IDX);
	if (ret != 0) {
		DiagPrintf("[large_image_test] input open failed %d\r\n", ret);
		goto disable_mode;
	}

	vfs_init();
	ret = vfs_target_register("fat", VFS_TARGET_SD, VFS_RW);
	if (ret != 0) {
		goto fail_input_configured;
	}
	vfs_registered = 1;
	snprintf(filepath, sizeof(filepath), "fat:%s", file);
	fp = fopen(filepath, "r");
	if (!fp) {
		goto fail_input_configured;
	}
	for (i = 0; i < LARGE_IMAGE_BUFS; i++) {
		large_image_input_bufs[i] = rtscam_video_buffer_alloc(RTSCAM_INPUT_STRM_IDX,
									RTS_MEMORY_KERNEL, LARGE_IMAGE_BSIZE);
		if (!large_image_input_bufs[i]) {
			goto fail_input_configured;
		}
	}
	rd = fread(large_image_input_bufs[0]->vm_addr, 1, LARGE_IMAGE_BSIZE, fp);
	if (rd != LARGE_IMAGE_BSIZE) {
		goto fail_input_configured;
	}
	memcpy(large_image_input_bufs[1]->vm_addr,
		   large_image_input_bufs[0]->vm_addr, LARGE_IMAGE_BSIZE);
	fclose(fp);
	fp = NULL;
	vfs_target_unregister("fat");
	vfs_registered = 0;
	for (i = 0; i < LARGE_IMAGE_BUFS; i++) {
		large_image_input_bufs[i]->bytesused = LARGE_IMAGE_BSIZE;
	}
	for (i = 0; i < LARGE_IMAGE_BUFS; i++) {
		if (rtscam_isp_input_enqueue(large_image_input_bufs[i]) != 0) {
			goto fail_input_configured;
		}
	}

	ret = rtscam_isp_input_set_behavior(1, ISP_INPUT_SGRBG16,
										LARGE_IMAGE_WIDTH, LARGE_IMAGE_HEIGHT, 1, 5);
	if (ret != 0) {
		goto fail_input_configured;
	}

	/* rtscam_isp_input_set_behavior() above only stored the parameters;
	 * the actual INPUT_EXEC_SET_BEHAVIOR module action -- and thus the
	 * sensor geometry change and force_reset described above -- is
	 * dispatched here, by the input stream's s_stream(1) handler.
	 */
	ret = rtscam_video_stream_start(RTSCAM_INPUT_STRM_IDX);
	if (ret != 0) {
		DiagPrintf("[large_image_test] input stream start failed %d\r\n", ret);
		goto fail_input_configured;
	}

	/* Capture streams are opened and configured only AFTER the input
	 * stream is already running, so the force_reset triggered above
	 * cannot wipe out their open state.
	 */
	ret = rtscam_video_open(0);
	if (ret != 0) {
		DiagPrintf("[large_image_test] stream 0 open failed %d\r\n", ret);
		goto fail_input_started;
	}
	memset(&format, 0, sizeof(format));
	format.pixelformat = V4L2_PIX_FMT_YUYV;
	format.width = LARGE_IMAGE_STREAM0_WIDTH;
	format.height = LARGE_IMAGE_STREAM0_HEIGHT;
	ret = rtscam_video_set_format(0, &format);
	if (ret != 0) {
		goto fail_stream0_configured;
	}
	for (i = 0; i < LARGE_IMAGE_STREAM_BUFS; i++) {
		large_image_stream_bufs[i] = rtscam_video_buffer_alloc(0,
									 RTS_MEMORY_KERNEL,
									 LARGE_IMAGE_STREAM0_WIDTH *
									 LARGE_IMAGE_STREAM0_HEIGHT * 2);
		if (!large_image_stream_bufs[i] ||
			rtscam_video_enqueue(0, large_image_stream_bufs[i]) != 0) {
			goto fail_stream0_configured;
		}
	}

	DiagPrintf("Success to open stream 0\n");

	ret = rtscam_video_open(1);
	if (ret != 0) {
		goto fail_stream0_configured;
	}
	memset(&format, 0, sizeof(format));
	format.pixelformat = V4L2_PIX_FMT_NV12;
	format.width = LARGE_IMAGE_STREAM1_WIDTH;
	format.height = LARGE_IMAGE_STREAM1_HEIGHT;
	ret = rtscam_video_set_format(1, &format);
	if (ret != 0) {
		goto fail_stream1_configured;
	}
	for (i = 0; i < LARGE_IMAGE_STREAM_BUFS; i++) {
		large_image_stream1_bufs[i] = rtscam_video_buffer_alloc(1,
									  RTS_MEMORY_KERNEL,
									  LARGE_IMAGE_STREAM1_WIDTH *
									  LARGE_IMAGE_STREAM1_HEIGHT * 2);
		if (!large_image_stream1_bufs[i] ||
			rtscam_video_enqueue(1, large_image_stream1_bufs[i]) != 0) {
			goto fail_stream1_configured;
		}
	}

	DiagPrintf("Success to open stream 1\n");

	ret = rtscam_video_stream_start(0);
	if (ret != 0) {
		DiagPrintf("[large_image_test] stream 0 start failed %d\r\n", ret);
		goto fail_stream0_started;
	}
	ret = rtscam_video_stream_start(1);
	if (ret != 0) {
		DiagPrintf("[large_image_test] stream 1 start failed %d\r\n", ret);
		goto fail_stream1_started;
	}

	/* Hardcoded: set stream-0 output position for large-image capture. */
	HAL_WRITE32(0x60000000, 0x4c, 0x100);

	large_image_frames = 0;
	large_image_stop_flag = 0;
	large_image_running = 1;
	if (rtos_task_create(&large_image_task_handle, "large_image",
						 large_image_input_task, NULL, 4096, 1) != 0) {
		large_image_running = 0;
		goto fail_task_create;
	}
	DiagPrintf("[large_image_test] started stream0 YUYV %ux%u, stream1 NV12 %ux%u, input %ux%u\r\n",
			   LARGE_IMAGE_STREAM0_WIDTH, LARGE_IMAGE_STREAM0_HEIGHT,
			   LARGE_IMAGE_STREAM1_WIDTH, LARGE_IMAGE_STREAM1_HEIGHT,
			   LARGE_IMAGE_WIDTH, LARGE_IMAGE_HEIGHT);
	goto done;

fail_task_create:
	rtscam_video_stream_stop(1);
fail_stream1_started:
	rtscam_video_stream_stop(0);
fail_stream0_started:
	rtscam_video_stream_stop(RTSCAM_INPUT_STRM_IDX);
fail_stream1_configured:
	rtscam_video_close(1);
fail_stream0_configured:
	large_image_release_stream_buffers();
	rtscam_video_close(0);
fail_input_started:
	rtscam_video_stream_stop(RTSCAM_INPUT_STRM_IDX);
fail_input_configured:
	if (fp) {
		fclose(fp);
	}
	if (vfs_registered) {
		vfs_target_unregister("fat");
	}
	large_image_release_input_buffers();
	rtscam_video_close(RTSCAM_INPUT_STRM_IDX);
disable_mode:
	rts_isp_set_large_image_mode(ISP0, 0);
done:
	large_image_setup_task_handle = NULL;
	vTaskDelete(NULL);
}

void mmf2_video_example_test_vin_large_image(const char *file)
{
	if (!file || !file[0]) {
		DiagPrintf("[large_image_test] file is required\r\n");
		return;
	}
	if (large_image_running || large_image_setup_task_handle) {
		DiagPrintf("[large_image_test] already running\r\n");
		return;
	}
	snprintf(large_image_file, sizeof(large_image_file), "%s", file);
	if (rtos_task_create(&large_image_setup_task_handle, "large_image_setup",
						 large_image_setup_task, large_image_file, 4096, 1) != 0) {
		DiagPrintf("[large_image_test] setup task create failed\r\n");
		large_image_setup_task_handle = NULL;
	}
}

void mmf2_video_example_test_vin_large_image_stop(void)
{
	if (!large_image_running) {
		DiagPrintf("[large_image_test] nothing running\r\n");
		return;
	}
	large_image_stop_flag = 1;
	vTaskDelay(pdMS_TO_TICKS(50));
	rtscam_video_stream_stop(1);
	rtscam_video_stream_stop(0);
	rtscam_video_stream_stop(RTSCAM_INPUT_STRM_IDX);
	large_image_release_input_buffers();
	large_image_release_stream_buffers();
	rtscam_video_close(1);
	rtscam_video_close(0);
	rtscam_video_close(RTSCAM_INPUT_STRM_IDX);
	rts_isp_set_large_image_mode(ISP0, 0);
	DiagPrintf("[large_image_test] stopped: frames=%lu drops=%lu\r\n",
			   large_image_frames, rtscam_isp_input_get_drop_count());
	large_image_running = 0;
	large_image_task_handle = NULL;
}

/*
 * Worker: dequeue one frame from ch0 and ch1 and save each to SD. Runs in the
 * loop task (large_image_input_task) context for stack headroom, never in the
 * console task.
 */
static void large_image_do_capture(const char *suffix)
{
	struct rts_video_buffer *vbuf;
	char filename[64];
	int ch;
	int i;
	int ret;

	for (ch = 0; ch < 2; ch++) {
		vbuf = NULL;
		ret = -1;
		for (i = 0; i < 200; i++) {
			ret = rtscam_video_dequeue(ch, &vbuf);
			if (ret == 0) {
				break;
			}
			vTaskDelay(pdMS_TO_TICKS(10));
		}
		if (ret != 0) {
			DiagPrintf("[large_image_test] ch%d dequeue timeout\r\n", ch);
			continue;
		}

		if (suffix && suffix[0]) {
			snprintf(filename, sizeof(filename), "large_img_ch%d_%s.bin",
					 ch, suffix);
		} else {
			snprintf(filename, sizeof(filename), "large_img_ch%d.bin", ch);
		}
		large_image_save_to_sd(vbuf->vm_addr, vbuf->bytesused, filename);

		rtscam_video_enqueue(ch, vbuf);
	}
}

void mmf2_video_example_test_vin_large_image_capture(const char *suffix)
{
	if (!large_image_running) {
		DiagPrintf("[large_image_test] nothing running\r\n");
		return;
	}
	if (large_image_capture_request) {
		DiagPrintf("[large_image_test] capture already pending\r\n");
		return;
	}

	if (suffix && suffix[0]) {
		snprintf(large_image_capture_suffix,
				 sizeof(large_image_capture_suffix), "%s", suffix);
	} else {
		large_image_capture_suffix[0] = '\0';
	}
	large_image_capture_request = 1;
	DiagPrintf("[large_image_test] capture requested (suffix=%s)\r\n",
			   large_image_capture_suffix[0] ? large_image_capture_suffix
			   : "<none>");
}
