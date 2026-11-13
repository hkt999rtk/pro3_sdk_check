/******************************************************************************
*
* Copyright(c) 2007 - 2025 Realtek Corporation. All rights reserved.
*
******************************************************************************/
#ifndef __NN_MOSAIC_H__
#define __NN_MOSAIC_H__

#include "../nn_coord_map/nn_coord_map.h"
#include "video_api.h"

/* Mosaic block size codes, matching encoder hardware. */
#define NN_MOSAIC_SIZE_8x8      0
#define NN_MOSAIC_SIZE_16x16    1
#define NN_MOSAIC_SIZE_32x32    2
#define NN_MOSAIC_SIZE_64x64    3
#define NN_MOSAIC_SIZE_128x128  4

/* Total hw overlay/mosaic indexes shared per encoder channel (OSD + mosaic combined). */
#define NN_MOSAIC_MAX_REGIONS  VIDEO_OVERLAY_INDEX_COUNT

/*
 * Configuration passed to nn_mosaic_group_init.
 * The effective alignment applied to each bbox is max(block_size_px, codec_align).
 * For H.264/HEVC set codec_align = 16 to stay on macroblock boundaries.
 */
typedef struct {
	int stream_id;       /* encoder channel (e.g. V1_STREAM_ID) */
	int stream_width;    /* frame width in pixels */
	int stream_height;   /* frame height in pixels */
	int max_regions;     /* hw slots to reserve (1..NN_MOSAIC_MAX_REGIONS) */
	int mosaic_size;     /* NN_MOSAIC_SIZE_* — shared by ALL regions; hardware enforces one block size per channel */
	int codec_align;     /* extra alignment required by codec (0 = none) */
} nn_mosaic_config_t;

typedef struct {
	int hw[NN_MOSAIC_MAX_REGIONS];  /* pre-allocated hw overlay indexes, -1 = unavailable */
	int max_regions;
	int prev_count;      /* region count committed in the last call to nn_mosaic_group_commit */
	int stream_id;
	int stream_width;
	int stream_height;
	int mosaic_size;
	int align;           /* effective alignment = max(block_size_px, codec_align) */
	int ready;
} nn_mosaic_group_t;

/*
 * Allocate hw overlay indexes and prepare the group for use.
 * OSD layers sharing the same stream should be initialized before calling this
 * so that their indexes are reserved first.
 * Returns 0 on success, -1 on invalid arguments or if not enough overlay slots
 * are available (all partially-acquired slots are rolled back on failure).
 */
int  nn_mosaic_group_init(nn_mosaic_group_t *group, const nn_mosaic_config_t *cfg);

/*
 * Disable all active mosaic regions and release the reserved hw overlay indexes.
 */
void nn_mosaic_group_deinit(nn_mosaic_group_t *group);

/*
 * Set mosaic for one slot from a pixel-space bounding box.
 * Coordinates are clipped to stream bounds and aligned to the codec/block grid.
 * Returns 0 on success, -1 if the slot is out of range, unallocated, or the bbox
 * has zero area after clipping.
 */
int  nn_mosaic_group_set_bbox(nn_mosaic_group_t *group, int slot,
							  int xmin, int ymin, int xmax, int ymax);

/*
 * Commit the current frame's mosaic state: disable slots [count..prev_count-1]
 * that are no longer active, then record count as the new prev_count.
 * Call once per NN callback after all nn_mosaic_group_set_bbox calls.
 */
void nn_mosaic_group_commit(nn_mosaic_group_t *group, int count);

/*
 * Immediately disable all mosaic regions and reset prev_count to 0.
 */
void nn_mosaic_group_disable_all(nn_mosaic_group_t *group);

#endif /* __NN_MOSAIC_H__ */
