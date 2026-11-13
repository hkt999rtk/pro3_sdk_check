/******************************************************************************
*
* Copyright(c) 2007 - 2025 Realtek Corporation. All rights reserved.
*
******************************************************************************/
#ifndef _VIDEO_OSD_H_
#define _VIDEO_OSD_H_

#include <stddef.h>
#include <stdint.h>
#include "video_common_type.h"

enum {
	VIDEO_OSD_EINVAL = -1,
	VIDEO_OSD_ENOMEM = -2,
	VIDEO_OSD_EBUSY = -3,
	VIDEO_OSD_EENCODER = -4,
	VIDEO_OSD_ENOSPC = -5,
	VIDEO_OSD_ETIMEDOUT = -6,
};

#define VIDEO_OSD_TIMESTAMP_FORMAT_YMD_DASH_24  "%Y-%m-%d %H:%M:%S"
#define VIDEO_OSD_TIMESTAMP_FORMAT_YMD_SLASH_24 "%Y/%m/%d %H:%M:%S"
#define VIDEO_OSD_TIMESTAMP_FORMAT_MDY_SLASH_24 "%m/%d/%Y %H:%M:%S"
#define VIDEO_OSD_TIMESTAMP_FORMAT_DMY_SLASH_24 "%d/%m/%Y %H:%M:%S"
#define VIDEO_OSD_TIMESTAMP_FORMAT_YMD_DASH_12  "%Y-%m-%d %I:%M:%S %p"
#define VIDEO_OSD_TIMESTAMP_FORMAT_YMD_SLASH_12 "%Y/%m/%d %I:%M:%S %p"
#define VIDEO_OSD_TIMESTAMP_FORMAT_MDY_SLASH_12 "%m/%d/%Y %I:%M:%S %p"
#define VIDEO_OSD_TIMESTAMP_FORMAT_DMY_SLASH_12 "%d/%m/%Y %I:%M:%S %p"

#define VIDEO_OSD_TIMESTAMP_DEFAULT_MAX_CHARS 19
#define VIDEO_OSD_TIMESTAMP_MAX_CHARS         64

typedef enum {
	VIDEO_OSD_POSITION_UPPER_LEFT = 0,
	VIDEO_OSD_POSITION_UPPER_RIGHT,
	VIDEO_OSD_POSITION_LOWER_LEFT,
	VIDEO_OSD_POSITION_LOWER_RIGHT,
	VIDEO_OSD_POSITION_CUSTOM,
} video_osd_position_t;

typedef enum {
	VIDEO_OSD_TIME_LOCAL = 0,
	VIDEO_OSD_TIME_UTC,
} video_osd_time_zone_t;

typedef struct {
	int stream_id;
	uint32_t codec;
	uint32_t frame_width;
	uint32_t frame_height;
} video_osd_target_t;

typedef struct {
	video_osd_position_t position;
	uint32_t margin_x;
	uint32_t margin_y;
	uint32_t x;
	uint32_t y;
} video_osd_placement_t;

typedef struct {
	video_osd_target_t target;
	video_osd_placement_t placement;
	const char *format;
	video_osd_time_zone_t time_zone;
	uint32_t valid_after;
	uint16_t max_chars;
	uint8_t font_scale; /* 1/2/3/5: native glyphs in 8/16/24/40-pixel cells; default 3. */
	uint8_t alpha;
	uint8_t y;
	uint8_t u;
	uint8_t v;
} video_osd_timestamp_config_t;

typedef struct {
	void *priv;
} video_osd_timestamp_t;

typedef struct {
	video_osd_target_t target;
	video_osd_placement_t placement;
	/* Input pixels are uint32_t 0xAARRGGBB. show() copies every row, so the
	 * source array may be released or reused after the call returns. */
	const uint32_t *argb8888;
	uint32_t width;
	uint32_t height;
	uint32_t stride_bytes;
} video_osd_logo_config_t;

typedef struct {
	void *priv;
} video_osd_logo_t;

/* Initialise optional timestamp fields to the camera defaults: local time,
 * YYYY-MM-DD HH:mm:ss, 3x white bitmap font, and lower-right 16-pixel margins. */
void video_osd_timestamp_config_default(video_osd_timestamp_config_t *config);
int video_osd_timestamp_start(video_osd_timestamp_t *ctx, const video_osd_timestamp_config_t *config);
/* Managed streams only; ctx must be zero-initialized/fully stopped. Retry EBUSY
 * before acceptance, then wait for the initial revision. 0 = encoder-applied.
 * A matching stream manager must exist first. Keep config/format valid and stable
 * throughout this call. Task context; serialize application updates/lifecycle.
 * timeout_ms is one retry+completion budget, not a hard IPC/mutex deadline;
 * 0 attempts once. Timeout does NOT cancel an accepted update or stop its clock.
 * On any failure preserve ctx: stop_sync safely cleans up either an empty or a
 * live context before a fresh start. Do not blindly repeat start or clear priv. */
int video_osd_timestamp_start_sync(video_osd_timestamp_t *ctx, const video_osd_timestamp_config_t *config, uint32_t timeout_ms);
void video_osd_timestamp_stop(video_osd_timestamp_t *ctx);
/* Managed streams only. Stop the clock task, remove the overlay and wait for
 * encoder completion before releasing the context. 0 also means already stopped.
 * On error/timeout keep ctx intact and retry this same API; do not call the
 * legacy void stop or restart it. The clock stays stopped during removal.
 * Caller serializes application updates/lifecycle per stream; task context,
 * encoder still running. Stop downstream delivery before shutdown removals.
 * timeout_ms bounds polling/retries, not an in-flight IPC/mutex operation;
 * 0 performs one attempt. Timeout does not cancel a submitted update. */
int video_osd_timestamp_stop_sync(video_osd_timestamp_t *ctx, uint32_t timeout_ms);

/* Initialise logo placement to upper-left with 16-pixel margins. */
void video_osd_logo_config_default(video_osd_logo_config_t *config);
int video_osd_logo_show(video_osd_logo_t *ctx, const video_osd_logo_config_t *config);
/* Same managed start/completion/timeout contract as timestamp_start_sync.
 * Keep config and source pixels stable until return. Input rows are copied;
 * they may be released after return, including when completion times out.
 * On failure retain ctx; hide_sync before attempting a fresh show. */
int video_osd_logo_show_sync(video_osd_logo_t *ctx, const video_osd_logo_config_t *config, uint32_t timeout_ms);
void video_osd_logo_hide(video_osd_logo_t *ctx);
/* Same managed completion/timeout contract as timestamp_stop_sync. On error,
 * retain ctx and retry hide_sync, not the legacy void hide. Already hidden = 0. */
int video_osd_logo_hide_sync(video_osd_logo_t *ctx, uint32_t timeout_ms);

#endif /* _VIDEO_OSD_H_ */
