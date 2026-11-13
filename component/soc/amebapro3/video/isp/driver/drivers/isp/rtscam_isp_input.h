/*
 * Copyright 2024 Realtek
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * ISP verify/input (TX) stream driver — feeds GE-reshaped RAW frames back
 * into the ISP through the verify hardware path. See
 * openspec/changes/add-isp-input-verify-stream/design.md.
 */

#ifndef _RTSCAM_ISP_INPUT_H_
#define _RTSCAM_ISP_INPUT_H_

#include <stdint.h>

struct rtscam_video_stream;
struct rts_video_buffer;

/* Bounded ready-list depth; submit beyond this drops the newest buffer. */
#define RTSCAM_INPUT_MAX_READY 2

/* One-time init of the input stream object. Idempotent. */
int rtscam_isp_input_init(void);

/* Resolves RTSCAM_INPUT_STRM_IDX for rtscam_soc_get_video_stream(). */
struct rtscam_video_stream *rtscam_isp_input_get_stream(void);

/* Configure the verify/input behavior programmed by s_stream(enable).
 * Must be called while the stream is not streaming. */
int rtscam_isp_input_set_behavior(uint32_t ddr_mode, uint32_t fmt,
								  uint32_t width, uint32_t height,
								  uint32_t fps_numerator,
								  uint32_t fps_denominator);

/* Push a buffer onto the bounded ready-list (drop-newest on overrun) and
 * wake the stream_thread. Distinct from rtscam_video_enqueue(), which would
 * submit synchronously and break the one-in-flight serialization. */
int rtscam_isp_input_enqueue(struct rts_video_buffer *buf);

/* Thin call-through to rtscam_video_dequeue(RTSCAM_INPUT_STRM_IDX, buf),
 * provided for call-site symmetry with rtscam_isp_input_enqueue(). Unlike
 * enqueue, the generic dequeue path has no HW side effect and is already
 * safe for this stream. */
int rtscam_isp_input_dequeue(struct rts_video_buffer **buf);

unsigned long rtscam_isp_input_get_drop_count(void);

/* Exported verify-done wakeup — called directly from rtscam_isp_mcu_irq()
 * (ISR context) when the VERIFY-INT branch fires. */
void rtscam_isp_input_frame_done(void);

#endif /* _RTSCAM_ISP_INPUT_H_ */
