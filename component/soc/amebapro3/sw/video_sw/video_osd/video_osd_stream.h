#ifndef _VIDEO_OSD_STREAM_H_
#define _VIDEO_OSD_STREAM_H_

#include "video_osd.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Optional adaptive manager for ANY combination of logo, timestamp, NN and
 * privacy. Initialize before clients; remove clients before deinit. Legacy
 * callers without a stream context retain their direct-overlay path. */
typedef struct {
	void *priv;
} video_osd_stream_t;
typedef struct {
	video_osd_target_t target;
	/* Manager allocations + registered source storage + in-flight old/new
	 * output buffers. 0 means unlimited. Checked at init/commit, not a hard cap
	 * on client allocations made before registration. RTOS/allocator overhead
	 * is excluded. */
	size_t memory_budget;
} video_osd_stream_config_t;

typedef struct {
	uint32_t requested_revision;
	uint32_t applied_revision;
	uint32_t pending;
	uint32_t applied_timestamp;
	int last_error;
	size_t allocated_bytes; /* Manager allocations, including retiring buffers. */
	size_t source_bytes;    /* Registered logo/text/NN/privacy source storage. */
	size_t peak_bytes;      /* Peak observed total during accepted/attempted updates. */
	size_t memory_budget;
	uint32_t index_count;   /* Physical indices, not logical client handles. */
	uint32_t bitmap_regions;
	uint32_t argb_regions;
} video_osd_stream_status_t;

/* Caller serializes all client lifecycles per stream. Task context only.
 * Return 0 or VIDEO_OSD_E*.
 * registered source storage excludes RTOS task/semaphore/allocator overhead;
 * caller-created NN working canvases are accounted after init/flush. */
int video_osd_stream_init(video_osd_stream_t *ctx, const video_osd_stream_config_t *config);
int video_osd_stream_get_status(video_osd_stream_t *ctx, video_osd_stream_status_t *status);
/* Wait for applied_revision >= revision, not for the whole stream to go idle.
 * revision=0 snapshots requested_revision on the first successful status read;
 * later timestamp/NN updates do not move that target. A future revision is invalid.
 * status may be NULL; otherwise receives the last successfully read status.
 * 0 means encoder-applied, NOT network/player delivery. An error in status is
 * returned unless a recovery transaction is pending. Timeout cancels nothing.
 * timeout_ms bounds polling/retries; IPC/mutex calls can extend wall-clock time.
 * 0 performs one attempt. Task context only; serialize application updates and
 * lifecycles per stream. Background timestamp/NN refreshes may continue. */
int video_osd_stream_wait_applied(video_osd_stream_t *ctx, uint32_t revision,
								  uint32_t timeout_ms, video_osd_stream_status_t *status);
/* Retry EBUSY with encoder running. Stop downstream delivery before removal. */
int video_osd_stream_deinit(video_osd_stream_t *ctx);
/* Retry deinit until complete or timeout, with the same timeout contract above.
 * Remove ALL clients first. No automatic client destruction. Already closed = 0.
 * On failure retain ctx, keep output blocked and encoder running, then retry. */
int video_osd_stream_deinit_sync(video_osd_stream_t *ctx, uint32_t timeout_ms);

#ifdef __cplusplus
}
#endif
#endif
