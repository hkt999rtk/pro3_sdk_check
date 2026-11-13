/******************************************************************************
*
* Copyright(c) 2007 - 2025 Realtek Corporation. All rights reserved.
*
******************************************************************************/
#ifndef _NN_OSD_H_
#define _NN_OSD_H_

#include <stdint.h>
#include <stdarg.h>
#include "mmf2_module.h"
#include "module_vipnn.h"
#include "video_osd_font.h"

/*
 * 1bpp bitmap canvas for NN detection result OSD overlay.
 *
 * Usage pattern (per frame):
 *   static nn_osd_canvas_t canvas;   // zero-init at startup (static guarantees this)
 *   nn_osd_canvas_init(&canvas, w, h);
 *   nn_osd_canvas_clear(&canvas);
 *   nn_osd_draw_rect(&canvas, ...);
 *   nn_osd_flush(&canvas, stream_id, osd_idx);
 */

/* ---------- Canvas ---------- */

typedef struct {
	uint8_t *buf;    /* 1bpp bitmap buffer */
	uint8_t *crop_buf;
	int      width;
	int      height;
	int      size;   /* bytes = width * height / 8 */
	int      crop_size;
} nn_osd_canvas_t;

#define NN_OSD_WATCHDOG_MAX_ITEMS 4

typedef struct {
	nn_osd_canvas_t *canvas;
	int stream_id;
	int osd_idx;
	uint8_t alpha;
	uint8_t y;
	uint8_t u;
	uint8_t v;
} nn_osd_watchdog_item_t;

typedef struct {
	nn_osd_watchdog_item_t item[NN_OSD_WATCHDOG_MAX_ITEMS];
	int item_count;
	uint32_t timeout_ms;
	uint32_t watch_ms;
	volatile uint32_t last_update_ms;
	volatile int active;
	void *lock;
	void *task;
	void *guard;
} nn_osd_watchdog_t;

typedef struct {
	int stream_id;
	uint32_t codec; /* VIDEO_H264 or VIDEO_HEVC; determines encoder OSD block size */
	uint8_t alpha;
	uint8_t y;
	uint8_t u;
	uint8_t v;
} nn_osd_layer_config_t;

typedef struct {
	nn_osd_canvas_t canvas[NN_OSD_WATCHDOG_MAX_ITEMS];
	nn_osd_watchdog_t watchdog;
	int width;
	int height;
	int layer_count;
	int ready;
	void *guard;
} nn_osd_group_t;

/* Lazy-alloc: safe to call every frame; only allocates on first call.
 * Returns 0 on success, -1 on malloc failure.
 * Caller must ensure canvas is zero-initialised before the first call
 * (static variables satisfy this automatically). */
int  nn_osd_canvas_init(nn_osd_canvas_t *canvas, int width, int height);
void nn_osd_canvas_clear(nn_osd_canvas_t *canvas);
void nn_osd_canvas_free(nn_osd_canvas_t *canvas);

/* ---------- Drawing primitives (1bpp, MSB-first bit order) ---------- */

void nn_osd_draw_hline(nn_osd_canvas_t *canvas, int x0, int x1, int y);
void nn_osd_draw_vline(nn_osd_canvas_t *canvas, int x, int y0, int y1);
void nn_osd_draw_rect(nn_osd_canvas_t *canvas, int xmin, int ymin, int xmax, int ymax, int thickness);

/* Draw a filled square of (2*radius+1) pixels centered at (x, y), for landmarks. */
void nn_osd_draw_point(nn_osd_canvas_t *canvas, int x, int y, int radius);

/* ---------- Coordinate mapping (NN-space → video-frame pixels) ---------- */

#include "../nn_coord_map/nn_coord_map.h"

/* ---------- Text rendering (shared native-size monochrome font) ---------- */

#define NN_OSD_FONT_W  VIDEO_OSD_FONT_WIDTH
#define NN_OSD_FONT_H  VIDEO_OSD_FONT_HEIGHT

/* Draw a single printable ASCII character (0x20–0x7E) at (x, y).
 * scale 1/2/3/5 selects independently rasterized 8/16/24/40-pixel cells; other
 * positive scales resample the 24-pixel glyph. Maximum cell size is 8*scale pixels; advances are
 * proportional except for equal-width digits.
 * Out-of-range characters are rendered as a space. */
void nn_osd_draw_char(nn_osd_canvas_t *canvas, int x, int y, char c, int scale);

/* Draw a null-terminated ASCII string starting at (x, y).
 * Use video_osd_font_string_width(str, scale) to measure the actual advances.
 * NN_OSD_FONT_W*scale is only an upper bound per character; no line wrapping. */
void nn_osd_draw_string(nn_osd_canvas_t *canvas, int x, int y, const char *str, int scale);

/* printf-style text drawing. Internally formats into a 128-byte stack buffer.
 * Note: scale comes before fmt so variadic args remain last.
 * Example: nn_osd_draw_stringf(&canvas, x, y, 2, "%s %.2f", class_name, score); */
void nn_osd_draw_stringf(nn_osd_canvas_t *canvas, int x, int y, int scale, const char *fmt, ...);

/* ---------- Submit to hardware ---------- */

/* DCache_Clean + video_set_osd_bitmap with bitmap format (fmt=2), alpha=128,
 * zero offset. Covers the full canvas area. */
void nn_osd_flush(nn_osd_canvas_t *canvas, int stream_id, int osd_idx);

/* Same as nn_osd_flush(), but lets the caller choose the bitmap OSD colour.
 * Useful when different OSD indexes are used for different 1bpp colours. */
void nn_osd_flush_color(nn_osd_canvas_t *canvas, int stream_id, int osd_idx,
						uint8_t alpha, uint8_t y, uint8_t u, uint8_t v);

/* Flush only the smallest byte-aligned region containing non-zero pixels.
 * This keeps multiple bitmap OSD indexes from occupying overlapping full-frame
 * overlay areas in the encoder driver. Empty canvases disable the OSD index. */
void nn_osd_flush_region_color(nn_osd_canvas_t *canvas, int stream_id, int osd_idx,
							   uint8_t alpha, uint8_t y, uint8_t u, uint8_t v);

/* ---------- Stale OSD watchdog ----------
 *
 * Use this when an NN callback may stop arriving after objects disappear. The
 * watchdog clears registered OSD indexes if no caller kicks it before timeout.
 *
 * Typical callback flow:
 *   nn_osd_watchdog_lock(&wd);
 *   nn_osd_canvas_clear(&canvas);
 *   draw objects...
 *   nn_osd_flush_region_color(...);
 *   nn_osd_watchdog_kick(&wd, object_count > 0);
 *   nn_osd_watchdog_unlock(&wd);
 */
int  nn_osd_watchdog_init(nn_osd_watchdog_t *wd, uint32_t timeout_ms, uint32_t watch_ms);
int  nn_osd_watchdog_add(nn_osd_watchdog_t *wd, nn_osd_canvas_t *canvas, int stream_id, int osd_idx,
						 uint8_t alpha, uint8_t y, uint8_t u, uint8_t v);
int  nn_osd_watchdog_lock(nn_osd_watchdog_t *wd);
void nn_osd_watchdog_unlock(nn_osd_watchdog_t *wd);
void nn_osd_watchdog_clear_locked(nn_osd_watchdog_t *wd);
void nn_osd_watchdog_kick(nn_osd_watchdog_t *wd, int active);

/* ---------- OSD layer group ----------
 *
 * Convenience wrapper for examples that need several OSD colors. The group owns
 * one canvas per layer and assigns hardware OSD indexes internally.
 * Set enable_watchdog to 1 if the OSD should be cleared automatically when NN
 * draw callbacks stop arriving after objects disappear.
 */
int  nn_osd_group_init(nn_osd_group_t *group, int width, int height,
					   const nn_osd_layer_config_t *layer, int layer_count,
					   int enable_watchdog);
int  nn_osd_group_init_ex(nn_osd_group_t *group, int width, int height,
						  const nn_osd_layer_config_t *layer, int layer_count,
						  int enable_watchdog, uint32_t timeout_ms, uint32_t watch_ms);
void nn_osd_group_deinit(nn_osd_group_t *group);
int  nn_osd_group_lock(nn_osd_group_t *group);
void nn_osd_group_unlock(nn_osd_group_t *group);
void nn_osd_group_clear(nn_osd_group_t *group);
void nn_osd_group_clear_locked(nn_osd_group_t *group);
nn_osd_canvas_t *nn_osd_group_canvas(nn_osd_group_t *group, int layer_idx);
void nn_osd_group_flush(nn_osd_group_t *group);
void nn_osd_group_kick(nn_osd_group_t *group, int active);

#endif /* _NN_OSD_H_ */
