/******************************************************************************
*
* Copyright(c) 2007 - 2025 Realtek Corporation. All rights reserved.
*
******************************************************************************/
#ifndef _VIDEO_OSD_INTERNAL_H_
#define _VIDEO_OSD_INTERNAL_H_

#include <stdint.h>
#include "video_osd.h"
#include "video_osd_privacy.h"

#define VIDEO_OSD_NN_MAX_LAYERS 4
#define VIDEO_OSD_NN_SUPPRESSED 1
#define VIDEO_OSD_BUFFER_RELEASE_DELAY_MS 100

/* Operations must release the manager lock before returning EBUSY. */
int video_osd_sync_retry(int (*operation)(void *), void *context, uint32_t timeout_ms);
int video_osd_sync_check_applied(const video_osd_stream_status_t *status, uint32_t revision);

typedef struct {
	uint32_t x;
	uint32_t y;
	uint32_t width;
	uint32_t height;
} video_osd_region_t;

typedef struct {
	int enable;
	int osd_idx;
	video_osd_format_t format;
	video_osd_region_t region;
	uint8_t alpha;
	uint8_t y;
	uint8_t u;
	uint8_t v;
	uint8_t *buffer;
	uint32_t stride_bytes; /* 0: tightly packed; used by managed NN input views. */
} video_osd_layer_update_t;

typedef struct video_osd_nn_group video_osd_nn_group_t;

video_osd_nn_group_t *video_osd_nn_group_register(int stream_id, uint32_t codec, const int *osd_idx, int layer_count);
int video_osd_nn_group_submit(video_osd_nn_group_t *group, const video_osd_layer_update_t *update, int layer_count);
void video_osd_nn_group_hide(video_osd_nn_group_t *group);
int video_osd_nn_group_unregister(video_osd_nn_group_t *group);

/* Compositor callbacks run under the shared manager lock. */
typedef int (*video_osd_commit_fn)(void *context);
typedef int (*video_osd_conflict_fn)(void *context, const video_osd_region_t *region);
int video_osd_manager_lock(void);
void video_osd_manager_unlock(void);
int video_osd_region_conflicts(const video_osd_region_t *first, const video_osd_region_t *second, uint32_t codec);
int video_osd_compositor_attach(const video_osd_target_t *target, void *context,
								video_osd_commit_fn commit, video_osd_conflict_fn conflicts);
int video_osd_compositor_has_clients(int stream_id);
void video_osd_compositor_detach(int stream_id);
void video_osd_composite_layer(uint32_t *pixels, uint32_t width, uint32_t height, const video_osd_layer_update_t *layer);

#define VIDEO_OSD_MAX_ITEMS 24
typedef struct {
	video_osd_layer_update_t layer;
	const video_osd_privacy_polygon_t *polygon; /* Geometry only; no source canvas. */
	uint32_t rgb;
	uint64_t version;
	const void *nn_group; /* Identity only; NULL means protected logo/text/privacy. */
} video_osd_render_item_t;

/* All *_locked functions require the manager mutex. */
uint64_t video_osd_next_version_locked(uint32_t count);
int video_osd_collect_layers_locked(int stream_id, video_osd_render_item_t *items, int capacity, size_t *source_bytes);
void *video_osd_stream_lookup_locked(const video_osd_target_t *target);
int video_osd_stream_commit_locked(void *context);
int video_osd_stream_status_locked(void *context, video_osd_stream_status_t *status);
int video_osd_privacy_collect_locked(int stream_id, video_osd_render_item_t *items, int capacity, size_t *bytes);
int video_osd_privacy_conflicts_locked(int stream_id, const video_osd_region_t *region);
int video_osd_privacy_present_locked(int stream_id);
int video_osd_client_alloc_index(int stream_id);
void video_osd_client_free_index(int stream_id, int index);
int video_osd_client_is_managed(int stream_id);
void video_osd_nn_group_source_bytes(video_osd_nn_group_t *group, size_t bytes);

#endif /* _VIDEO_OSD_INTERNAL_H_ */
