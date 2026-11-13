/******************************************************************************
*
* Copyright(c) 2007 - 2025 Realtek Corporation. All rights reserved.
*
******************************************************************************/
#include "ameba_soc.h"
#include "nn_mosaic.h"
#include "video_api.h"

static int prv_clip(int v, int lo, int hi)
{
	return v < lo ? lo : (v > hi ? hi : v);
}

/* Both helpers use bitwise masking and require 'a' to be a power of two.
 * Valid inputs: block_px (always 8 << mosaic_size, i.e. 8/16/32/64/128)
 * and codec_align (must be 0 or a power of two; documented values are 0 and 16).
 * A non-power-of-two codec_align would silently produce wrong aligned coordinates. */
static int prv_align_down(int v, int a)
{
	return v & ~(a - 1);
}

static int prv_align_up(int v, int a)
{
	return (v + a - 1) & ~(a - 1);
}

int nn_mosaic_group_init(nn_mosaic_group_t *group, const nn_mosaic_config_t *cfg)
{
	int block_px;
	int i;

	if (!group || !cfg) {
		return -1;
	}
	if (cfg->max_regions <= 0 || cfg->max_regions > NN_MOSAIC_MAX_REGIONS) {
		return -1;
	}
	if (group->ready) {
		return 0;
	}

	if (cfg->mosaic_size < 0 || cfg->mosaic_size > 4) {
		return -1;
	}
	block_px = 8 << cfg->mosaic_size;
	/* codec_align must be 0 (no extra alignment) or a power of two (e.g. 16 for H.264).
	 * prv_align_up/prv_align_down rely on bitwise masking and produce wrong results
	 * for non-power-of-two values. */
	if (cfg->codec_align != 0 && (cfg->codec_align & (cfg->codec_align - 1)) != 0) {
		return -1;
	}
	group->align = (block_px > cfg->codec_align) ? block_px : cfg->codec_align;
	if (group->align <= 0) {
		group->align = block_px;
	}

	group->stream_id     = cfg->stream_id;
	group->stream_width  = cfg->stream_width;
	group->stream_height = cfg->stream_height;
	group->mosaic_size   = cfg->mosaic_size;
	group->max_regions   = cfg->max_regions;
	group->prev_count    = 0;

	for (i = 0; i < cfg->max_regions; i++) {
		group->hw[i] = video_alloc_overlay_index(cfg->stream_id);
		if (group->hw[i] < 0) {
			printf("error: nn_mosaic_group_init: only %d/%d overlay slots available on ch%d\r\n",
				   i, cfg->max_regions, cfg->stream_id);
			for (int j = 0; j < i; j++) {
				video_free_overlay_index(cfg->stream_id, group->hw[j]);
				group->hw[j] = -1;
			}
			return -1;
		}
	}

	group->ready = 1;
	return 0;
}

void nn_mosaic_group_deinit(nn_mosaic_group_t *group)
{
	int i;

	if (!group || !group->ready) {
		return;
	}

	for (i = 0; i < group->max_regions; i++) {
		if (group->hw[i] >= 0) {
			video_disable_mosaic(group->stream_id, group->hw[i]);
			video_free_overlay_index(group->stream_id, group->hw[i]);
			group->hw[i] = -1;
		}
	}
	group->prev_count = 0;
	group->ready = 0;
}

int nn_mosaic_group_set_bbox(nn_mosaic_group_t *group, int slot,
							 int xmin, int ymin, int xmax, int ymax)
{
	int align, aligned_w, aligned_h;
	int x0, y0, x1, y1;
	int hw_idx;

	if (!group || !group->ready) {
		return -1;
	}
	if (slot < 0 || slot >= group->max_regions) {
		return -1;
	}

	hw_idx = group->hw[slot];
	if (hw_idx < 0) {
		return -1;
	}

	align    = group->align;
	aligned_w = prv_align_up(group->stream_width, align);
	aligned_h = prv_align_up(group->stream_height, align);

	xmin = prv_clip(xmin, 0, group->stream_width - 1);
	xmax = prv_clip(xmax, 0, group->stream_width - 1);
	ymin = prv_clip(ymin, 0, group->stream_height - 1);
	ymax = prv_clip(ymax, 0, group->stream_height - 1);
	if (xmax <= xmin || ymax <= ymin) {
		return -1;
	}

	x0 = prv_align_down(xmin, align);
	y0 = prv_align_down(ymin, align);
	x1 = prv_align_up(xmax + 1, align);
	y1 = prv_align_up(ymax + 1, align);
	x0 = prv_clip(x0, 0, aligned_w - align);
	y0 = prv_clip(y0, 0, aligned_h - align);
	x1 = prv_clip(x1, x0 + align, group->stream_width);
	y1 = prv_clip(y1, y0 + align, group->stream_height);
	/* stream_width/height are always multiples of align: the H.264/HEVC encoder
	 * requires macroblock-aligned (16 px) dimensions and rejects anything else at
	 * configuration time.  The two caps below therefore only guard against the
	 * prv_clip(lo>hi) corner case and can never produce a non-aligned residual. */
	if (x1 > group->stream_width) {
		x1 = group->stream_width;
	}
	if (y1 > group->stream_height) {
		y1 = group->stream_height;
	}
	if (x1 <= x0 || y1 <= y0) {
		return -1;
	}

	video_set_mosaic(group->stream_id, hw_idx, x0, y0, x1 - x0, y1 - y0, (uint32_t)group->mosaic_size);
	return 0;
}

void nn_mosaic_group_commit(nn_mosaic_group_t *group, int count)
{
	int slot;

	if (!group || !group->ready) {
		return;
	}

	if (count > group->max_regions) {
		count = group->max_regions;
	}

	for (slot = count; slot < group->prev_count; slot++) {
		if (group->hw[slot] >= 0) {
			video_disable_mosaic(group->stream_id, group->hw[slot]);
		}
	}
	group->prev_count = count;
}

void nn_mosaic_group_disable_all(nn_mosaic_group_t *group)
{
	int slot;

	if (!group || !group->ready) {
		return;
	}

	for (slot = 0; slot < group->max_regions; slot++) {
		if (group->hw[slot] >= 0) {
			video_disable_mosaic(group->stream_id, group->hw[slot]);
		}
	}
	group->prev_count = 0;
}
