#ifndef _MODULE_ISP_4K_H
#define _MODULE_ISP_4K_H

#include <stdint.h>
#include "mmf2_module.h"
#include "os_wrapper.h"

/* Standalone VP source: packed GRBG RAW16 -> clockwise PPU rotation -> ISP
 * input -> NV12 on ISP stream 1 (full size) and/or stream 2 (downscaled),
 * and/or planar RGB on stream 10 (zoom AI channel).
 * Exclusive use of ISP stream 0/1/2/10, ISP input and PPU; do not run module_isp,
 * the large_image_pipeline example or another NN client concurrently.
 * Common video clocks must be initialized by the VP application, as for
 * module_isp. Sensor must output exactly these RAW dimensions and Bayer order.
 */
#define ISP_4K_RAW_WIDTH     3840U
#define ISP_4K_RAW_HEIGHT    2160U
#define ISP_4K_OUTPUT_WIDTH  ISP_4K_RAW_HEIGHT
#define ISP_4K_OUTPUT_HEIGHT ISP_4K_RAW_WIDTH

#define CMD_ISP_4K_APPLY       MM_MODULE_CMD(0x20)
#define CMD_ISP_4K_STREAM_STOP MM_MODULE_CMD(0x22)
#define CMD_ISP_4K_LOG_EN      MM_MODULE_CMD(0x23)
#define CMD_ISP_4K_SET_STREAM  MM_MODULE_CMD(0x24) /* arg: ISP_4K_MAIN/SUB/RGB_STREAM_IDX */

/* Full-size output (default): ISP_4K_OUTPUT_WIDTH x ISP_4K_OUTPUT_HEIGHT. */
#define ISP_4K_MAIN_STREAM_IDX 1U
#define ISP_4K_OUTPUT_BUFS     1

/* Downscaled output: keep the 9:16 portrait aspect, stay within
 * CONFIG_RTS_ZOOM_S2_WIDTH/HEIGHT, and size CONFIG_RTS_MEM_SIZE for
 * ISP_4K_SUB_OUTPUT_BUFS NV12 frames.
 */
#define ISP_4K_SUB_STREAM_IDX    2U
#define ISP_4K_SUB_OUTPUT_WIDTH  1440U
#define ISP_4K_SUB_OUTPUT_HEIGHT 2560U
#define ISP_4K_SUB_OUTPUT_BUFS   1

/* Planar RGB888 output on the zoom AI channel, for a filesaver / NN sink.
 * Stream id 10 is the public alias for the RGB/YONLY stream (see
 * rtscam_soc_get_video_stream()), independent of zoom streams 0-3, and is
 * bounded by CONFIG_RTS_ZOOM_S4_WIDTH/HEIGHT (2688x1520). The three planes
 * are packed one after another in a single buffer (R at +0, G at +w*h,
 * B at +2*w*h), so the allocation is w*h*3.
 */
#define ISP_4K_RGB_STREAM_IDX    10U
#define ISP_4K_RGB_WIDTH         576U
#define ISP_4K_RGB_HEIGHT        320U
#define ISP_4K_RGB_BUFS          1

#define ISP_4K_OUT_MAX_BUFS (ISP_4K_OUTPUT_BUFS > ISP_4K_SUB_OUTPUT_BUFS ? \
							 (ISP_4K_OUTPUT_BUFS > ISP_4K_RGB_BUFS ? ISP_4K_OUTPUT_BUFS : ISP_4K_RGB_BUFS) : \
							 (ISP_4K_SUB_OUTPUT_BUFS > ISP_4K_RGB_BUFS ? ISP_4K_SUB_OUTPUT_BUFS : ISP_4K_RGB_BUFS))

/* Keep private camera driver headers out of module users. */
struct rts_video_buffer;

/* Shared source (stream 0, PPU, ISP input, scheduler task); private to module_isp_4k.c. */
struct isp_4k_src_s;

/* One output: an MMF context bound to ISP stream 1, 2 or 10. Contexts are static
 * so the worker never sees freed memory. The control task opens and closes
 * the stream; between ATTACH and DETACH only the worker dequeues/requeues it.
 */
typedef struct isp_4k_ctx_s {
	void *parent;
	/* Every context points at the same source: the first APPLY starts it, the last STOP stops it. */
	struct isp_4k_src_s *src;
	uint32_t stream;
	uint32_t width;
	uint32_t height;
	int is_rgb; /* Planar RGB888 on the zoom AI channel, not NV12. */
	int nbufs;
	int in_use;
	int applied;
	int opened;
	int detaching; /* Worker-owned. */
	rtos_sema_t done;
	volatile int request;
	volatile int closing;
	int result;
	volatile int attached;
	uint32_t frames_out;
	uint32_t frames_dropped;
	struct rts_video_buffer *bufs[ISP_4K_OUT_MAX_BUFS];
	mm_queue_item_t *leased_item[ISP_4K_OUT_MAX_BUFS];
	int output_state[ISP_4K_OUT_MAX_BUFS]; /* Protected by RTOS_CRITICAL_SOC. */
} isp_4k_ctx_t;

/* Usage (control calls must be serialized), one context per output:
 *   ctx = mm_module_open(&isp_4k_module);
 *   mm_module_ctrl(ctx, CMD_ISP_4K_SET_STREAM, ISP_4K_SUB_STREAM_IDX); // optional, default main
 *   mm_module_ctrl(ctx, MM_CMD_SET_QUEUE_LEN, 2);
 *   mm_module_ctrl(ctx, MM_CMD_INIT_QUEUE_ITEMS, MMQI_FLAG_DYNAMIC);
 *   mm_module_ctrl(ctx, CMD_ISP_4K_APPLY, 0); // waits for setup, returns status
 * At most three contexts, one per stream. The first APPLY starts the shared
 * source; a later APPLY waits for its first ISP pass (up to 10 s) and joins.
 * Connect a main/sub output to module_encoder with a SISO linker. Encoder
 * dimensions must match the selected stream; disable compression, remap and
 * low-latency/slice modes. NV12 output is contiguous 8-bit NV12; the RGB
 * output is one AV_CODEC_ID_RGB888 block of three planes.
 * MMF calls vrelease_item after downstream finishes; never free pixel data.
 * Every attached output must return a buffer before the next ISP pass, so a
 * slow consumer throttles all outputs.
 *
 * Shutdown: issue CMD_ISP_4K_STREAM_STOP while linker/encoder can still
 * finish and release their current frame, then stop/delete the linker and
 * close the module. STOP waits for the last lease; the last output's STOP
 * also stops the source. Call it BEFORE mm_module_close(), which deletes
 * queues before module destroy.
 * STOP returns -ETIMEDOUT if a lease is not returned in time; then do NOT
 * delete linkers or close modules (buffers are still in use). Retry STOP.
 */
extern mm_module_t isp_4k_module;

#endif
