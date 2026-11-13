/******************************************************************************
*
* Copyright(c) 2007 - 2025 Realtek Corporation. All rights reserved.
*
******************************************************************************/
#ifndef _ISP_STREAM_H_
#define _ISP_STREAM_H_

#include <stdint.h>
#include "rts_errno.h"

#define STREAM_CNT_MAX      5
#define ISP_STREAM_MAX_BUFS 8

typedef struct {
	uint32_t ch;
	uint32_t enable;
	uint32_t width;
	uint32_t height;
	uint32_t fps;
	uint32_t slot_num;
	uint32_t format;

	uint32_t ar_feature; //0:disable, 1:frame mode compress, 2: slice mode
	uint32_t ar_align_w;
} stream_params_t;

/* Forward declaration — callers that need the full struct must include
 * rts_camera.h before this header (hal_isp.c, isp_stream.c do so). */
struct isp_stream;
typedef struct isp_stream isp_stream_t;

/* Return the per-channel context owned by this module.  Returns NULL if ch
 * is out of range.  The pointer is stable for the lifetime of the process. */
isp_stream_t *isp_stream_get_ctx(uint32_t ch);

/* Initialise ctx and bind it to ch.  Any pre-registered callback is preserved.
 * Called once per channel at ISP_Init time. */
int isp_stream_init(isp_stream_t *ctx, uint32_t ch);

/* Dump capabilities by stream channel */
void isp_stream_dump_caps_by_channel(int ch);

/* Complete stream-on: open → format → fps → alloc_buf → stream_start → recycler.
 * ctx must already be initialised via isp_stream_init(). */
int isp_stream_open(isp_stream_t *ctx, uint32_t pixfmt,
					uint32_t width, uint32_t height,
					uint32_t fps, uint32_t slot_num,
					uint32_t ar_feature, uint32_t ar_align_w);

/* Complete stream-off: stream_stop → destroy (drain+release buffers) → close. */
int  isp_stream_close(isp_stream_t *ctx);

/* Register a frame-ready callback.  May be called at any time; survives
 * isp_stream_init().  Pass NULL to unregister.
 * Callback returns 0 to re-enqueue the buffer; non-zero to take ownership. */
void isp_stream_register_handler(isp_stream_t *ctx,
								 int (*handler)(int stream_id, int buf_id,
										 uint32_t phy_addr));

/* Return a previously-held ISP buffer back to the driver by index.
 * Used by isp_buf_recycle() in hal_isp.c and by the output task. */
void isp_stream_recycle(isp_stream_t *ctx, int buf_id);

/* Release a buffer back to the driver, supporting two lookup modes:
 *   buf_id >= 0 : direct index into bufs[] (clears held_bufs entry if present)
 *   buf_id == -1: scan bufs[] by physical address match
 * Used by isp_buf_release() in hal_isp.c. */
void isp_stream_release_buf(isp_stream_t *ctx, void *buf_addr, int buf_id);

/* Enable manual-release mode for this stream.  When set, the recycler will
 * not re-enqueue buffers after the callback — the user must call
 * isp_buf_release() or isp_stream_release_buf() to return each buffer. */
void isp_stream_set_user_release(isp_stream_t *ctx, int enable);

/* Set the watchdog timeout (milliseconds).  0 = disabled (default).
 * If user_release is set and a held buffer is not released within this
 * period, the recycler force-enqueues it to prevent ISP starvation. */
void isp_stream_set_release_timeout(isp_stream_t *ctx, uint32_t timeout_ms);

#endif /* _ISP_STREAM_H_ */
