#ifndef _VIDEO_OSD_PRIVACY_H_
#define _VIDEO_OSD_PRIVACY_H_

#include "video_osd.h"
#include "video_osd_stream.h"

#ifdef __cplusplus
extern "C" {
#endif

#define VIDEO_OSD_PRIVACY_VERSION 1U
#define VIDEO_OSD_PRIVACY_MAX_POLYGONS 8
#define VIDEO_OSD_PRIVACY_MAX_VERTICES 16
#define VIDEO_OSD_PRIVACY_NORMALIZED_MAX 10000U

#define VIDEO_OSD_PRIVACY_EINVAL VIDEO_OSD_EINVAL
#define VIDEO_OSD_PRIVACY_ENOMEM VIDEO_OSD_ENOMEM
#define VIDEO_OSD_PRIVACY_EBUSY VIDEO_OSD_EBUSY
#define VIDEO_OSD_PRIVACY_EENCODER VIDEO_OSD_EENCODER
#define VIDEO_OSD_PRIVACY_ENOSPC VIDEO_OSD_ENOSPC
#define VIDEO_OSD_PRIVACY_ETIMEDOUT VIDEO_OSD_ETIMEDOUT

typedef enum {
	VIDEO_OSD_PRIVACY_NORMALIZED = 0,
	VIDEO_OSD_PRIVACY_REFERENCE_PIXELS = 1,
} video_osd_privacy_coordinates_t;

typedef struct {
	uint32_t x;
	uint32_t y;
} video_osd_privacy_vertex_t;

typedef struct {
	uint32_t id;
	uint32_t enabled;
	uint32_t vertex_count;
	video_osd_privacy_vertex_t vertices[VIDEO_OSD_PRIVACY_MAX_VERTICES];
} video_osd_privacy_polygon_t;

typedef struct {
	uint32_t version;
	uint32_t enabled;
	video_osd_privacy_coordinates_t coordinates;
	uint32_t reference_width;
	uint32_t reference_height;
	uint32_t fill_rgb; /* 0x00RRGGBB; always opaque, no alpha option. */
	uint32_t polygon_count;
	video_osd_privacy_polygon_t polygons[VIDEO_OSD_PRIVACY_MAX_POLYGONS];
} video_osd_privacy_scene_t;

typedef struct {
	video_osd_target_t target;
	size_t memory_budget; /* Used when init creates a stream manager. An existing manager keeps its budget. */
} video_osd_privacy_config_t;

typedef struct {
	uint32_t version;
	uint32_t max_polygons;
	uint32_t max_vertices;
	uint32_t normalized_max;
	uint32_t concave_supported;
	uint32_t reference_pixels_supported;
	size_t canvas_bytes; /* Compatibility worst-case ARGB estimate, NOT allocated by init. Query status for actual usage. */
} video_osd_privacy_caps_t;

typedef video_osd_stream_status_t video_osd_privacy_status_t;

typedef struct {
	void *priv;
} video_osd_privacy_t;

/* One zero-initialized context per running H.264/HEVC stream. Prefer creating a
 * video_osd_stream_t before ANY clients; privacy can then join/remove independently.
 * Without a stream manager, init creates one for compatibility and must precede
 * logo/timestamp/NN; remove those clients before deinitializing this owner.
 * Different crops/rotations require caller-side mapping.
 * Coordinates refer to image EDGES, origin top-left, x right, y down. Vertices
 * follow the boundary in either winding; no holes or self intersections.
 * These APIs run in task context, never ISR/encoder callbacks. The caller must
 * serialize the lifetime of a context (especially init/deinit versus other calls).
 * Low-latency, look-ahead, multi-pass and combined H.264/HEVC + JPEG encoding
 * are not supported by this frame-boundary transaction. */
int video_osd_privacy_get_caps(const video_osd_target_t *target, video_osd_privacy_caps_t *caps);
void video_osd_privacy_scene_default(video_osd_privacy_scene_t *scene);
int video_osd_privacy_init(video_osd_privacy_t *ctx, const video_osd_privacy_config_t *config);
/* Copies the complete scene. 0 means ACCEPTED; poll status until applied_revision
 * reaches revision (and last_error is zero) before reporting success. Logo/clock
 * updates also advance this shared revision. Gate output until then, discarding
 * older queued frames using applied_timestamp as shown in the V1 example. EBUSY means
 * no change was submitted. Other negative values leave the previous scene intact.
 * Runtime encoder rejection is reported through status. */
int video_osd_privacy_apply(video_osd_privacy_t *ctx, const video_osd_privacy_scene_t *scene, uint32_t *revision);
/* Retry EBUSY until accepted, then wait for that revision; never resubmit an
 * accepted scene in this call. 0 = encoder-applied. revision may be NULL;
 * otherwise it is cleared on entry and set when accepted, even on later timeout.
 * On timeout retain ctx: revision != 0 can be passed to stream_wait_applied.
 * Keep scene stable for the duration of this call. Same task/lifecycle and
 * timeout contract as stream_wait_applied; the budget covers retry + wait. */
int video_osd_privacy_apply_sync(video_osd_privacy_t *ctx, const video_osd_privacy_scene_t *scene,
								 uint32_t *revision, uint32_t timeout_ms);
/* Returns the last accepted request; status distinguishes it from the applied scene. */
int video_osd_privacy_get(video_osd_privacy_t *ctx, video_osd_privacy_scene_t *scene);
int video_osd_privacy_get_status(video_osd_privacy_t *ctx, video_osd_privacy_status_t *status);
/* Explicitly removes masking. May return EBUSY while the encoder retires a buffer;
 * retry with the context intact. Stop downstream delivery before calling. */
int video_osd_privacy_deinit(video_osd_privacy_t *ctx);
/* Remove masking and wait for safe retirement. Already closed = 0. On failure
 * retain ctx and retry, with output blocked and encoder still running.
 * Other clients must be removed first if privacy owns the stream manager. */
int video_osd_privacy_deinit_sync(video_osd_privacy_t *ctx, uint32_t timeout_ms);

#ifdef __cplusplus
}
#endif
#endif
