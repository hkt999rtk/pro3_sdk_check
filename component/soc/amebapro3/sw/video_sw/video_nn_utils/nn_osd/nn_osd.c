/******************************************************************************
*
* Copyright(c) 2007 - 2025 Realtek Corporation. All rights reserved.
*
******************************************************************************/
/*
 * Recommended OSD group flow for examples that need colored NN overlays:
 *
 *  [static nn_osd_group_t group]
 *  [static nn_osd_layer_config_t layer[]]  <-- one entry per OSD color layer
 *            |
 *            v
 *   nn_osd_group_init(..., watchdog)       <-- alloc canvases, optional watchdog
 *            |
 *            v  (inside each NN draw callback)
 *   nn_osd_group_lock()                    <-- protect canvases during update
 *            |
 *            v
 *   nn_osd_group_clear()                   <-- redraw a complete OSD frame
 *            |
 *            v
 *   nn_coord_map_init()                    <-- compute NN-space to video map
 *            |
 *            v
 *   for each detected object:
 *     nn_osd_group_canvas()                <-- choose color/layer canvas
 *     nn_coord_map_bbox()                  <-- map normalised bbox to pixels
 *     nn_osd_draw_rect()
 *     nn_osd_draw_point()       (opt.)
 *     nn_osd_draw_stringf()     (opt.)
 *            |
 *            v
 *   nn_osd_group_flush()                   <-- submit all registered layers
 *            |
 *            v
 *   nn_osd_group_kick(object_count > 0)    <-- feed watchdog if enabled
 *            |
 *            v
 *   nn_osd_group_unlock()
 *
 * Lower-level single-canvas flow:
 *
 *  [static nn_osd_canvas_t canvas]   <-- zero-init guaranteed by static storage
 *            |
 *            v
 *   nn_osd_canvas_init()             <-- lazy malloc; no-op on subsequent frames
 *            |
 *            v
 *   nn_osd_canvas_clear()            <-- memset to 0 every frame
 *            |
 *            v
 *   nn_coord_map_init()              <-- compute NN-space → frame-pixel mapping
 *            |
 *            v
 *   for each detected object:
 *     nn_coord_map_bbox()            <-- map normalised coords to pixel coords
 *     nn_osd_draw_rect()             <-- draw bounding box on 1bpp bitmap
 *     nn_osd_draw_point()  (opt.)    <-- draw landmark point
 *     nn_osd_draw_stringf() (opt.)   <-- draw label + score above the box
 *            |
 *            v
 *   nn_osd_flush()                   <-- DCache_Clean + video_set_osd_bitmap
 *            |
 *            v  (on pipeline teardown)
 *   nn_osd_canvas_free()             <-- free bitmap buffer
 */
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "ameba_soc.h"
#include "mmf2_mediatime_pro3.h"
#include "video_api.h"
#include "video_osd_font.h"
#include "video_osd_internal.h"
#include "nn_osd.h"

#define NN_OSD_STALE_TIMEOUT_MS      500
#define NN_OSD_STALE_WATCH_MS        100


/* ========================================================================== */
/* Canvas lifecycle                                                             */
/* ========================================================================== */

int nn_osd_canvas_init(nn_osd_canvas_t *canvas, int width, int height)
{
	if (!canvas || width <= 0 || height <= 0 || (width & 7)) {
		return -1;
	}
	if (canvas->buf != NULL) {
		return 0;   /* already allocated */
	}
	canvas->width  = width;
	canvas->height = height;
	canvas->size   = (width * height + 7) / 8;
	canvas->buf    = (uint8_t *)malloc(canvas->size);
	if (canvas->buf == NULL) {
		return -1;
	}
	return 0;
}

void nn_osd_canvas_clear(nn_osd_canvas_t *canvas)
{
	if (canvas->buf) {
		memset(canvas->buf, 0, canvas->size);
	}
}

void nn_osd_canvas_free(nn_osd_canvas_t *canvas)
{
	if (canvas->buf) {
		free(canvas->buf);
		canvas->buf = NULL;
	}
	if (canvas->crop_buf) {
		free(canvas->crop_buf);
		canvas->crop_buf = NULL;
	}
	canvas->crop_size = 0;
}

/* ========================================================================== */
/* Drawing primitives (1bpp, MSB-first bit order)                              */
/* ========================================================================== */

/* Internal helpers — no bounds checking; callers must clamp before calling. */

static void draw_hline(nn_osd_canvas_t *canvas, int x0, int x1, int y)
{
	uint8_t *bm   = canvas->buf;
	int      w    = canvas->width;
	int start_b   = (y * w + x0) >> 3;
	int end_b     = (y * w + x1) >> 3;
	int start_bit = x0 & 0x7;
	int end_bit   = x1 & 0x7;

	if (start_b == end_b) {
		uint8_t mask = 0;
		for (int b = start_bit; b <= end_bit; b++) {
			mask |= (uint8_t)(1u << (7 - b));
		}
		bm[start_b] |= mask;
		return;
	}

	uint8_t first = 0;
	for (int b = start_bit; b <= 7; b++) {
		first |= (uint8_t)(1u << (7 - b));
	}
	bm[start_b] |= first;

	for (int idx = start_b + 1; idx < end_b; idx++) {
		bm[idx] = 0xFF;
	}

	uint8_t last = 0;
	for (int b = 0; b <= end_bit; b++) {
		last |= (uint8_t)(1u << (7 - b));
	}
	bm[end_b] |= last;
}

static void draw_vline(nn_osd_canvas_t *canvas, int x, int y0, int y1)
{
	uint8_t *bm    = canvas->buf;
	int      w     = canvas->width;
	uint8_t  mask  = (uint8_t)(1u << (7 - (x & 0x7)));
	int row_bytes  = w >> 3;
	uint8_t *p     = bm + (y0 * row_bytes + (x >> 3));
	for (int i = 0; i <= y1 - y0; i++, p += row_bytes) {
		*p |= mask;
	}
}

/* Public drawing functions — include bounds clamping. */

void nn_osd_draw_hline(nn_osd_canvas_t *canvas, int x0, int x1, int y)
{
	if (!canvas->buf || x0 > x1 || y < 0 || y >= canvas->height) {
		return;
	}
	if (x0 < 0) {
		x0 = 0;
	}
	if (x1 >= canvas->width) {
		x1 = canvas->width - 1;
	}
	draw_hline(canvas, x0, x1, y);
}

void nn_osd_draw_vline(nn_osd_canvas_t *canvas, int x, int y0, int y1)
{
	if (!canvas->buf || x < 0 || x >= canvas->width || y0 > y1) {
		return;
	}
	if (y0 < 0) {
		y0 = 0;
	}
	if (y1 >= canvas->height) {
		y1 = canvas->height - 1;
	}
	draw_vline(canvas, x, y0, y1);
}

void nn_osd_draw_rect(nn_osd_canvas_t *canvas, int xmin, int ymin, int xmax, int ymax, int thickness)
{
	if (!canvas->buf || thickness <= 0) {
		return;
	}
	if (xmin < 0) {
		xmin = 0;
	}
	if (ymin < 0) {
		ymin = 0;
	}
	if (xmax >= canvas->width) {
		xmax = canvas->width - 1;
	}
	if (ymax >= canvas->height) {
		ymax = canvas->height - 1;
	}
	if (xmin > xmax || ymin > ymax) {
		return;
	}

	/* Top and bottom edges */
	for (int t = 0; t < thickness; t++) {
		int y_top    = ymin + t;
		int y_bottom = ymax - t;
		if (y_top <= ymax) {
			draw_hline(canvas, xmin, xmax, y_top);
		}
		if (y_bottom >= ymin && y_bottom != y_top) {
			draw_hline(canvas, xmin, xmax, y_bottom);
		}
	}

	/* Left and right edges — skip corners already drawn above */
	int y0 = ymin + thickness;
	int y1 = ymax - thickness;
	if (y1 >= y0) {
		for (int t = 0; t < thickness; t++) {
			int x_left  = xmin + t;
			int x_right = xmax - t;
			draw_vline(canvas, x_left, y0, y1);
			if (x_right != x_left) {
				draw_vline(canvas, x_right, y0, y1);
			}
		}
	}
}

void nn_osd_draw_point(nn_osd_canvas_t *canvas, int x, int y, int radius)
{
	if (!canvas->buf || radius < 0) {
		return;
	}
	int x0 = x - radius < 0 ? 0 : x - radius;
	int y0 = y - radius < 0 ? 0 : y - radius;
	int x1 = x + radius >= canvas->width  ? canvas->width - 1  : x + radius;
	int y1 = y + radius >= canvas->height ? canvas->height - 1 : y + radius;
	for (int row = y0; row <= y1; row++) {
		draw_hline(canvas, x0, x1, row);
	}
}

/* ========================================================================== */
/* Text rendering delegates to the shared encoder OSD font implementation. */
/* ========================================================================== */

static video_osd_font_canvas_t nn_osd_font_canvas(nn_osd_canvas_t *canvas)
{
	video_osd_font_canvas_t font_canvas = {
		.buffer = canvas->buf,
		.width = canvas->width,
		.height = canvas->height,
	};
	return font_canvas;
}

void nn_osd_draw_char(nn_osd_canvas_t *canvas, int x, int y, char c, int scale)
{
	video_osd_font_canvas_t font_canvas;

	if (!canvas) {
		return;
	}
	font_canvas = nn_osd_font_canvas(canvas);
	video_osd_font_draw_char(&font_canvas, x, y, c, scale);
}

void nn_osd_draw_string(nn_osd_canvas_t *canvas, int x, int y, const char *str, int scale)
{
	video_osd_font_canvas_t font_canvas;

	if (!canvas) {
		return;
	}
	font_canvas = nn_osd_font_canvas(canvas);
	video_osd_font_draw_string(&font_canvas, x, y, str, scale);
}

void nn_osd_draw_stringf(nn_osd_canvas_t *canvas, int x, int y, int scale, const char *fmt, ...)
{
	char buf[128];
	va_list args;

	va_start(args, fmt);
	vsnprintf(buf, sizeof(buf), fmt, args);
	va_end(args);
	nn_osd_draw_string(canvas, x, y, buf, scale);
}

/* ========================================================================== */
/* Submit to hardware                                                           */
/* ========================================================================== */

void nn_osd_flush(nn_osd_canvas_t *canvas, int stream_id, int osd_idx)
{
	nn_osd_flush_color(canvas, stream_id, osd_idx, 128, 0, 0, 0);
}

void nn_osd_flush_color(nn_osd_canvas_t *canvas, int stream_id, int osd_idx,
						uint8_t alpha, uint8_t y, uint8_t u, uint8_t v)
{
	if (!canvas->buf) {
		return;
	}
	DCache_Clean((uint32_t)canvas->buf, canvas->size);
	video_set_osd_bitmap(stream_id, osd_idx, VIDEO_OSD_FORMAT_BITMAP, 0, 0,
						 canvas->width, canvas->height,
						 alpha, y, u, v, canvas->buf);
}

static int nn_osd_prepare_region_color(nn_osd_canvas_t *canvas, int osd_idx,
									   uint8_t alpha, uint8_t y, uint8_t u, uint8_t v,
									   video_osd_layer_update_t *update, int copy_pixels)
{
	int row_bytes;
	int min_byte;
	int max_byte = -1;
	int min_y;
	int max_y = -1;
	int crop_width;
	int crop_height;
	int crop_row_bytes;
	int crop_size;
	int x_off;
	int y_off;

	memset(update, 0, sizeof(*update));
	update->osd_idx = osd_idx;
	update->format = VIDEO_OSD_FORMAT_BITMAP;
	update->alpha = alpha;
	update->y = y;
	update->u = u;
	update->v = v;

	if (!canvas->buf || canvas->width <= 0 || canvas->height <= 0) {
		return 0;
	}

	row_bytes = canvas->width >> 3;
	min_byte = row_bytes;
	min_y = canvas->height;

	for (int y_idx = 0; y_idx < canvas->height; y_idx++) {
		uint8_t *row = canvas->buf + y_idx * row_bytes;
		for (int byte_idx = 0; byte_idx < row_bytes; byte_idx++) {
			if (row[byte_idx] == 0) {
				continue;
			}
			if (byte_idx < min_byte) {
				min_byte = byte_idx;
			}
			if (byte_idx > max_byte) {
				max_byte = byte_idx;
			}
			if (y_idx < min_y) {
				min_y = y_idx;
			}
			if (y_idx > max_y) {
				max_y = y_idx;
			}
		}
	}

	if (max_byte < min_byte || max_y < min_y) {
		return 0;
	}

	x_off = min_byte << 3;
	y_off = min_y;
	crop_row_bytes = max_byte - min_byte + 1;
	crop_width = crop_row_bytes << 3;
	crop_height = max_y - min_y + 1;
	crop_size = crop_row_bytes * crop_height;

	/* In managed mode submit a strided view; the manager makes the one tight
	 * snapshot it needs for later timestamp/logo recompositions. No crop copy. */
	if (!copy_pixels) {
		update->enable = 1;
		update->region = (video_osd_region_t) {
			x_off, y_off, crop_width, crop_height
		};
		update->buffer = canvas->buf + min_y * row_bytes + min_byte;
		update->stride_bytes = row_bytes;
		return 0;
	}
	if (crop_size > canvas->crop_size) {
		uint8_t *new_buf = (uint8_t *)malloc(crop_size);
		if (!new_buf) {
			return -1;
		}
		if (canvas->crop_buf) {
			free(canvas->crop_buf);
		}
		canvas->crop_buf = new_buf;
		canvas->crop_size = crop_size;
	}

	for (int row = 0; row < crop_height; row++) {
		memcpy(canvas->crop_buf + row * crop_row_bytes,
			   canvas->buf + (min_y + row) * row_bytes + min_byte,
			   crop_row_bytes);
	}

	DCache_Clean((uint32_t)canvas->crop_buf, crop_size);
	update->enable = 1;
	update->region.x = (uint32_t)x_off;
	update->region.y = (uint32_t)y_off;
	update->region.width = (uint32_t)crop_width;
	update->region.height = (uint32_t)crop_height;
	update->buffer = canvas->crop_buf;
	return 0;
}

void nn_osd_flush_region_color(nn_osd_canvas_t *canvas, int stream_id, int osd_idx,
							   uint8_t alpha, uint8_t y, uint8_t u, uint8_t v)
{
	video_osd_layer_update_t update;

	if (nn_osd_prepare_region_color(canvas, osd_idx, alpha, y, u, v, &update, 1) != 0) {
		video_disable_osd(stream_id, osd_idx);
		return;
	}
	if (!update.enable) {
		video_disable_osd(stream_id, osd_idx);
		return;
	}
	video_set_osd_bitmap(stream_id, update.osd_idx, update.format,
						 update.region.x, update.region.y, update.region.width, update.region.height,
						 update.alpha, update.y, update.u, update.v, update.buffer);
}

/* ========================================================================== */
/* Stale OSD watchdog                                                           */
/* ========================================================================== */

static void nn_osd_watchdog_task(void *param)
{
	nn_osd_watchdog_t *wd = (nn_osd_watchdog_t *)param;

	while (1) {
		vTaskDelay(wd->watch_ms);

		if (wd->active &&
			(uint32_t)(mm_read_mediatime_ms() - wd->last_update_ms) > wd->timeout_ms &&
			nn_osd_watchdog_lock(wd) == 0) {
			if (wd->active &&
				(uint32_t)(mm_read_mediatime_ms() - wd->last_update_ms) > wd->timeout_ms) {
				nn_osd_watchdog_clear_locked(wd);
			}
			nn_osd_watchdog_unlock(wd);
		}
	}
}

static int nn_osd_watchdog_init_lock(nn_osd_watchdog_t *wd)
{
	if (!wd) {
		return -1;
	}

	if (!wd->lock) {
		wd->lock = (void *)xSemaphoreCreateMutex();
		if (!wd->lock) {
			return -1;
		}
	}

	return 0;
}

int nn_osd_watchdog_init(nn_osd_watchdog_t *wd, uint32_t timeout_ms, uint32_t watch_ms)
{
	TaskHandle_t task;

	if (!wd) {
		return -1;
	}

	wd->timeout_ms = timeout_ms;
	wd->watch_ms = watch_ms ? watch_ms : 100;

	if (nn_osd_watchdog_init_lock(wd) != 0) {
		return -1;
	}

	if (!wd->task) {
		if (xTaskCreate(nn_osd_watchdog_task, "nn_osd_watch", 1024, wd, tskIDLE_PRIORITY + 1, &task) != pdPASS) {
			return -1;
		}
		wd->task = (void *)task;
	}

	return 0;
}

int nn_osd_watchdog_add(nn_osd_watchdog_t *wd, nn_osd_canvas_t *canvas, int stream_id, int osd_idx,
						uint8_t alpha, uint8_t y, uint8_t u, uint8_t v)
{
	nn_osd_watchdog_item_t *item;

	if (!wd || !canvas || wd->item_count >= NN_OSD_WATCHDOG_MAX_ITEMS) {
		return -1;
	}

	item = &wd->item[wd->item_count++];
	item->canvas = canvas;
	item->stream_id = stream_id;
	item->osd_idx = osd_idx;
	item->alpha = alpha;
	item->y = y;
	item->u = u;
	item->v = v;

	return 0;
}

int nn_osd_watchdog_lock(nn_osd_watchdog_t *wd)
{
	if (!wd || !wd->lock) {
		return -1;
	}

	return xSemaphoreTake((SemaphoreHandle_t)wd->lock, portMAX_DELAY) == pdTRUE ? 0 : -1;
}

void nn_osd_watchdog_unlock(nn_osd_watchdog_t *wd)
{
	if (wd && wd->lock) {
		xSemaphoreGive((SemaphoreHandle_t)wd->lock);
	}
}

void nn_osd_watchdog_clear_locked(nn_osd_watchdog_t *wd)
{
	if (!wd) {
		return;
	}

	for (int i = 0; i < wd->item_count; i++) {
		nn_osd_watchdog_item_t *item = &wd->item[i];
		nn_osd_canvas_clear(item->canvas);
		if (!wd->guard) {
			nn_osd_flush_region_color(item->canvas, item->stream_id, item->osd_idx,
									  item->alpha, item->y, item->u, item->v);
		}
	}
	if (wd->guard) {
		video_osd_nn_group_hide((video_osd_nn_group_t *)wd->guard);
	}

	wd->active = 0;
}

void nn_osd_watchdog_kick(nn_osd_watchdog_t *wd, int active)
{
	if (!wd) {
		return;
	}

	wd->last_update_ms = mm_read_mediatime_ms();
	wd->active = active ? 1 : 0;
}

/* ========================================================================== */
/* OSD layer group                                                              */
/* ========================================================================== */

static int nn_osd_alloc_hw_index(int stream_id)
{
	return video_osd_client_alloc_index(stream_id);
}

int nn_osd_group_init(nn_osd_group_t *group, int width, int height,
					  const nn_osd_layer_config_t *layer, int layer_count,
					  int enable_watchdog)
{
	return nn_osd_group_init_ex(group, width, height, layer, layer_count,
								enable_watchdog, NN_OSD_STALE_TIMEOUT_MS, NN_OSD_STALE_WATCH_MS);
}

int nn_osd_group_init_ex(nn_osd_group_t *group, int width, int height,
						 const nn_osd_layer_config_t *layer, int layer_count,
						 int enable_watchdog, uint32_t timeout_ms, uint32_t watch_ms)
{
	if (!group || !layer || layer_count <= 0 || layer_count > NN_OSD_WATCHDOG_MAX_ITEMS ||
		(layer[0].codec != VIDEO_H264 && layer[0].codec != VIDEO_HEVC)) {
		return -1;
	}
	for (int i = 1; i < layer_count; i++) {
		if (layer[i].stream_id != layer[0].stream_id || layer[i].codec != layer[0].codec) {
			return -1;
		}
	}

	if (group->ready) {
		return 0;
	}

	group->width = width;
	group->height = height;
	group->layer_count = layer_count;

	for (int i = 0; i < layer_count; i++) {
		if (nn_osd_canvas_init(&group->canvas[i], width, height) != 0) {
			goto cleanup;
		}
	}
	if (nn_osd_watchdog_init_lock(&group->watchdog) != 0) {
		goto cleanup;
	}

	for (int i = 0; i < layer_count; i++) {
		int osd_idx = nn_osd_alloc_hw_index(layer[i].stream_id);
		if (osd_idx < 0) {
			goto cleanup;
		}
		if (nn_osd_watchdog_add(&group->watchdog, &group->canvas[i], layer[i].stream_id, osd_idx,
								layer[i].alpha, layer[i].y, layer[i].u, layer[i].v) != 0) {
			video_osd_client_free_index(layer[i].stream_id, osd_idx);
			goto cleanup;
		}
	}
	{
		int osd_idx[NN_OSD_WATCHDOG_MAX_ITEMS];
		for (int i = 0; i < layer_count; i++) {
			osd_idx[i] = group->watchdog.item[i].osd_idx;
		}
		group->guard = video_osd_nn_group_register(layer[0].stream_id, layer[0].codec, osd_idx, layer_count);
		if (!group->guard) {
			goto cleanup;
		}
		group->watchdog.guard = group->guard;
	}

	if (enable_watchdog && nn_osd_watchdog_init(&group->watchdog, timeout_ms, watch_ms) != 0) {
		goto cleanup;
	}

	size_t source_bytes = 0;
	for (int i = 0; i < group->layer_count; i++) {
		source_bytes += group->canvas[i].size + group->canvas[i].crop_size;
	}
	video_osd_nn_group_source_bytes((video_osd_nn_group_t *)group->guard, source_bytes);
	group->ready = 1;
	return 0;

cleanup:
	if (group->guard) {
		video_osd_nn_group_unregister((video_osd_nn_group_t *)group->guard);
		group->guard = NULL;
		group->watchdog.guard = NULL;
	}
	/* Release hw overlay indices registered so far (item_count tracks how many). */
	for (int j = 0; j < group->watchdog.item_count; j++) {
		video_osd_client_free_index(group->watchdog.item[j].stream_id,
									group->watchdog.item[j].osd_idx);
	}
	group->watchdog.item_count = 0;
	/* nn_osd_canvas_free() checks canvas->buf before freeing, so it is safe to call
	 * on canvases that were not yet initialised (their buf pointer is still NULL). */
	for (int j = 0; j < layer_count; j++) {
		nn_osd_canvas_free(&group->canvas[j]);
	}
	if (group->watchdog.lock) {
		vSemaphoreDelete((SemaphoreHandle_t)group->watchdog.lock);
		group->watchdog.lock = NULL;
	}
	return -1;
}

int nn_osd_group_lock(nn_osd_group_t *group)
{
	if (!group || !group->ready) {
		return -1;
	}

	return nn_osd_watchdog_lock(&group->watchdog);
}

void nn_osd_group_unlock(nn_osd_group_t *group)
{
	if (group && group->ready) {
		nn_osd_watchdog_unlock(&group->watchdog);
	}
}

void nn_osd_group_clear(nn_osd_group_t *group)
{
	if (!group || !group->ready) {
		return;
	}

	for (int i = 0; i < group->layer_count; i++) {
		nn_osd_canvas_clear(&group->canvas[i]);
	}
}

void nn_osd_group_clear_locked(nn_osd_group_t *group)
{
	if (group && group->ready) {
		nn_osd_watchdog_clear_locked(&group->watchdog);
	}
}

nn_osd_canvas_t *nn_osd_group_canvas(nn_osd_group_t *group, int layer_idx)
{
	if (!group || !group->ready || layer_idx < 0 || layer_idx >= group->layer_count) {
		return NULL;
	}

	return &group->canvas[layer_idx];
}

void nn_osd_group_flush(nn_osd_group_t *group)
{
	video_osd_layer_update_t update[NN_OSD_WATCHDOG_MAX_ITEMS];

	if (!group || !group->ready) {
		return;
	}

	for (int i = 0; i < group->watchdog.item_count; i++) {
		nn_osd_watchdog_item_t *item = &group->watchdog.item[i];
		if (nn_osd_prepare_region_color(item->canvas, item->osd_idx,
										item->alpha, item->y, item->u, item->v, &update[i], !video_osd_client_is_managed(item->stream_id)) != 0) {
			video_osd_nn_group_hide((video_osd_nn_group_t *)group->guard);
			return;
		}
	}
	size_t source_bytes = 0;
	for (int i = 0; i < group->layer_count; i++) {
		source_bytes += group->canvas[i].size + group->canvas[i].crop_size;
	}
	video_osd_nn_group_source_bytes((video_osd_nn_group_t *)group->guard, source_bytes);
	video_osd_nn_group_submit((video_osd_nn_group_t *)group->guard, update, group->watchdog.item_count);
}

void nn_osd_group_deinit(nn_osd_group_t *group)
{
	SemaphoreHandle_t lock;

	if (!group || !group->ready) {
		return;
	}
	lock = (SemaphoreHandle_t)group->watchdog.lock;
	if (!lock || xSemaphoreTake(lock, portMAX_DELAY) != pdTRUE) {
		return;
	}
	group->ready = 0;
	if (group->guard) {
		if (video_osd_nn_group_unregister((video_osd_nn_group_t *)group->guard) != 0) {
			group->ready = 1;
			xSemaphoreGive(lock);
			return;
		}
		group->guard = NULL;
		group->watchdog.guard = NULL;
	}
	if (group->watchdog.task) {
		vTaskDelete((TaskHandle_t)group->watchdog.task);
		group->watchdog.task = NULL;
	}
	/* VP has accepted the disable commands but may still be finishing a frame
	 * that references the old crop buffers. */
	rtos_time_delay_ms(VIDEO_OSD_BUFFER_RELEASE_DELAY_MS);
	for (int i = 0; i < group->watchdog.item_count; i++) {
		video_osd_client_free_index(group->watchdog.item[i].stream_id, group->watchdog.item[i].osd_idx);
	}
	for (int i = 0; i < group->layer_count; i++) {
		nn_osd_canvas_free(&group->canvas[i]);
	}
	xSemaphoreGive(lock);
	vSemaphoreDelete(lock);
	memset(group, 0, sizeof(*group));
}

void nn_osd_group_kick(nn_osd_group_t *group, int active)
{
	if (group && group->ready && group->watchdog.task) {
		nn_osd_watchdog_kick(&group->watchdog, active);
	}
}
