/******************************************************************************
*
* Copyright(c) 2007 - 2025 Realtek Corporation. All rights reserved.
*
******************************************************************************/
#include "FreeRTOS.h"
#include "ameba_soc.h"
#include "rts_blit.h"
#include "ge_sys_api.h"
#include "ge_reshape_api.h"
#include <string.h>

// #define GE_RESHAPE_LOG_DISABLE
/* Define GE_RESHAPE_LOG_DISABLE to suppress ge_raw_reshape / ge_yuv_reshape logs */
#ifndef GE_RESHAPE_LOG_DISABLE
#define GE_RESHAPE_LOG(fmt, ...) DiagPrintf(fmt, ##__VA_ARGS__)
#else
#define GE_RESHAPE_LOG(fmt, ...) do {} while (0)
#endif

/**
 * GE-based RAW reshape function for 16-bit per pixel data with overlap support.
 * Reshapes source RAW image to destination dimensions using GE hardware.
 * Mode: Horizontal split to vertical stacking with configurable overlap
 * Splits wide image (left/right) with overlap for ISP window processing.
 * Based on CmdGETest5 implementation with 32-bit optimization.
 * y_overlap is distributed symmetrically: y_overlap/2 added to top and bottom of each region.
 *
 * Crop windows (with symmetric y_overlap):
 * - Left:  (start_x, start_y - y_overlap/2) size (dst_w + x_overlap, dst_h + y_overlap)
 * - Right: (start_x + dst_w - x_overlap, start_y - y_overlap/2) size (dst_w + x_overlap, dst_h + y_overlap)
 *
 * @param pge_drv_ctx GE driver context (must be opened before calling)
 * @param src_buf Source buffer pointer (16-bit per pixel RAW data)
 * @param dst_buf Destination buffer pointer (16-bit per pixel RAW data)
 * @param src_w Source width in pixels (full sensor width, e.g., 3848)
 * @param src_h Source height in pixels (full sensor height, e.g., 2168)
 * @param start_x Starting X position for crop window center
 * @param start_y Starting Y position for crop window center
 * @param dst_w Destination width per half (e.g., 1920)
 * @param dst_h Destination height per half (e.g., 2040)
 * @param x_overlap Horizontal overlap in pixels (must be even)
 * @param y_overlap Vertical overlap in pixels (must be even, distributed as y_overlap/2 on each end)
 * @return 0 on success, negative error code on failure
 * @note Caller must handle DCache operations and GE device open/close
 * @example ge_raw_reshape(pge_drv_ctx, src, dst, 3848, 2168, 4, 64, 1920, 2040, 8, 6);
 *          // Crops Y-range [61, 2107) for both regions (symmetric 3px padding top/bottom)
 */
int ge_raw_reshape(ge_drv_ctx_t *pge_drv_ctx, void *src_buf, void *dst_buf,
				   u32 src_w, u32 src_h, u32 start_x, u32 start_y, u32 dst_w,
				   u32 dst_h, u32 x_overlap, u32 y_overlap)
{
	int ret = 0;
	struct ge_blit blit_cfg;

	/* Validate parameters */
	if (!pge_drv_ctx) {
		GE_RESHAPE_LOG("[GE_RESHAPE] Error: NULL GE driver context\n\r");
		return -1;
	}

	if (!src_buf || !dst_buf) {
		GE_RESHAPE_LOG("[GE_RESHAPE] Error: NULL buffer pointer\n\r");
		return -2;
	}

	if (src_w == 0 || src_h == 0 || dst_w == 0 || dst_h == 0) {
		GE_RESHAPE_LOG("[GE_RESHAPE] Error: Invalid dimensions\n\r");
		return -3;
	}

	/* Check overlap alignment (must be even for ARGB8888 optimization) */
	if ((x_overlap % 2) != 0 || (y_overlap % 2) != 0) {
		DiagPrintf(
			"[GE_RESHAPE] Error: Overlap must be even (x_overlap=%d, y_overlap=%d)\n\r",
			x_overlap, y_overlap);
		return -4;
	}

	/* Calculate actual crop dimensions including overlap */
	u32 crop_w = dst_w + x_overlap; /* Width of each cropped region */
	u32 crop_h = dst_h + y_overlap; /* Height of each cropped region */

	/* Validate crop windows fit within source */
	if (start_x + crop_w > src_w) {
		DiagPrintf(
			"[GE_RESHAPE] Error: Left crop window exceeds source width (%d + %d > %d)\n\r",
			start_x, crop_w, src_w);
		return -5;
	}

	if (start_x + dst_w - x_overlap + crop_w > src_w) {
		DiagPrintf(
			"[GE_RESHAPE] Error: Right crop window exceeds source width\n\r");
		return -6;
	}

	/* Check Y bounds with symmetric overlap (start_y - y_overlap/2 must be >= 0) */
	if (start_y < y_overlap / 2) {
		DiagPrintf(
			"[GE_RESHAPE] Error: start_y (%d) must be >= y_overlap/2 (%d)\n\r",
			start_y, y_overlap / 2);
		return -7;
	}

	if (start_y - y_overlap / 2 + crop_h > src_h) {
		DiagPrintf(
			"[GE_RESHAPE] Error: Crop windows exceed source height (start_y=%d, crop_h=%d, src_h=%d)\n\r",
			start_y, crop_h, src_h);
		return -8;
	}

	/* Treat 16-bit RAW as 32-bit ARGB8888 for optimization
	 * Each ARGB8888 pixel = 2 RAW16 pixels
	 */
	u32 src_w_argb = src_w / 2;
	u32 crop_w_argb = crop_w / 2;

	/* Configure blit operation */
	memset(&blit_cfg, 0, sizeof(blit_cfg));
	blit_cfg.cmd = GE_BLT_ROI_BLEND;
	blit_cfg.work_mode = FILL_SRCRECT;

	/* Source and destination image formats (as ARGB8888) */
	blit_cfg.src_img.w = src_w_argb;
	blit_cfg.src_img.h = src_h;
	blit_cfg.src_img.img_fmt = GE_BLT_ARGB8888;

	blit_cfg.dst_img.w = crop_w_argb;
	blit_cfg.dst_img.h = crop_h * 2;
	blit_cfg.dst_img.img_fmt = GE_BLT_ARGB8888;

	/* Left crop window: (start_x, start_y - y_overlap/2) size (crop_w, crop_h)
	 * Adds symmetric y_overlap/2 padding to top and bottom */
	u32 crop_y = start_y - y_overlap / 2;
	blit_cfg.src_rect.x = start_x / 2; /* Convert to ARGB units */
	blit_cfg.src_rect.y = crop_y;
	blit_cfg.src_rect.w = crop_w_argb;
	blit_cfg.src_rect.h = crop_h;

	blit_cfg.dst_rect.x = 0;
	blit_cfg.dst_rect.y = 0;
	blit_cfg.dst_rect.w = crop_w_argb;
	blit_cfg.dst_rect.h = crop_h;

	blit_cfg.usr_src_buf = (u32)src_buf;
	blit_cfg.usr_dst_buf = (u32)dst_buf;

	ret = ge_ctrl(pge_drv_ctx, GEDRV_CTRL_BLIT_START, &blit_cfg);
	if (ret != 0) {
		GE_RESHAPE_LOG("[GE_RESHAPE] Left crop blit failed\n\r");
		return -9;
	}
	ge_ctrl(pge_drv_ctx, GEDRV_CTRL_BLIT_CHK_END, NULL);

	/* Right crop window: (start_x + dst_w - x_overlap, start_y - y_overlap/2) size (crop_w, crop_h)
	 * Uses same Y-coordinate as left region for symmetric y_overlap/2 padding */
	u32 right_x = start_x + dst_w - x_overlap;

	blit_cfg.src_rect.x = right_x / 2; /* Convert to ARGB units */
	blit_cfg.src_rect.y = crop_y; /* Same Y as left region - symmetric! */
	blit_cfg.src_rect.w = crop_w_argb;
	blit_cfg.src_rect.h = crop_h;

	blit_cfg.dst_rect.x = 0;
	blit_cfg.dst_rect.y = crop_h; /* Stack below the left region */
	blit_cfg.dst_rect.w = crop_w_argb;
	blit_cfg.dst_rect.h = crop_h;

	ret = ge_ctrl(pge_drv_ctx, GEDRV_CTRL_BLIT_START, &blit_cfg);
	if (ret != 0) {
		GE_RESHAPE_LOG("[GE_RESHAPE] Right crop blit failed\n\r");
		return -10;
	}
	ge_ctrl(pge_drv_ctx, GEDRV_CTRL_BLIT_CHK_END, NULL);

	return 0;
}

/**
 * GE-based YUV420 reshape function with overlap removal.
 * Reshapes source YUV420 image to destination dimensions using GE hardware.
 * Mode: Vertical split to horizontal placement with overlap removal
 * Takes vertically-stacked regions (output of ge_raw_reshape after ISP processing)
 * and converts to horizontal side-by-side layout while removing overlap regions.
 * YUV420 format: Y plane (width × height), UV plane (width × height/2)
 * Based on CmdGETest9 implementation treating YUV data as ARGB8888.
 * y_overlap is removed symmetrically: y_overlap/2 skipped from top and bottom of each region.
 *
 * Crop operations (calculated internally, symmetric y_overlap/2 removal):
 * - Top region (→ Left):   Extract from (0, y_overlap/2) size (dst_w/2, dst_h)
 * - Bottom region (→ Right): Extract from (x_overlap, src_h/2 + y_overlap/2) size (dst_w/2, dst_h)
 *
 * @param pge_drv_ctx GE driver context (must be opened before calling)
 * @param src_buf Source buffer pointer (YUV420 format: Y plane followed by UV plane)
 * @param dst_buf Destination buffer pointer (YUV420 format: Y plane followed by UV plane)
 * @param src_w Source width in pixels (with overlap: dst_w/2 + x_overlap, must be even)
 * @param src_h Source height in pixels (stacked with overlap: (dst_h + y_overlap) × 2, must be even)
 * @param dst_w Final destination width (horizontal side-by-side, without overlap, must be even, e.g., 3840)
 * @param dst_h Final destination height (without overlap, must be even, e.g., 2040)
 * @param x_overlap Horizontal overlap in pixels to remove (must be even)
 * @param y_overlap Vertical overlap in pixels to remove (must be even, removes y_overlap/2 from each end)
 * @return 0 on success, negative error code on failure
 * @note Caller must handle DCache operations and GE device open/close
 * @example ge_yuv_reshape(pge_drv_ctx, src, dst, 1926, 4092, 3840, 2040, 6, 6);
 *          // Input: 1926×4092 stacked (each half: 1926×2046 with 3px padding on each end)
 *          // Extracts clean middle 1920×2040 from each half
 *          // Output: 3840×2040 side-by-side (no overlap)
 */
int ge_yuv_reshape(ge_drv_ctx_t *pge_drv_ctx, void *src_buf, void *dst_buf,
				   u32 src_w, u32 src_h, u32 dst_w, u32 dst_h, u32 x_overlap,
				   u32 y_overlap)
{
	int ret = 0;
	struct ge_blit blit_cfg;

	/* Calculate internal variables from final dimensions */
	u32 start_x = 0; /* Always start from origin horizontally */
	u32 start_y = y_overlap / 2; /* Skip top y_overlap/2 padding */
	u32 dst_w_half = dst_w / 2; /* Width per half (e.g., 3840 / 2 = 1920) */

	/* Validate parameters */
	if (!pge_drv_ctx) {
		DiagPrintf(
			"[GE_YUV_RESHAPE] Error: NULL GE driver context\n\r");
		return -1;
	}

	if (!src_buf || !dst_buf) {
		GE_RESHAPE_LOG("[GE_YUV_RESHAPE] Error: NULL buffer pointer\n\r");
		return -2;
	}

	if (src_w == 0 || src_h == 0 || dst_w == 0 || dst_h == 0) {
		GE_RESHAPE_LOG("[GE_YUV_RESHAPE] Error: Invalid dimensions\n\r");
		return -2;
	}

	/* YUV420 requires even dimensions */
	if ((src_w % 2) || (src_h % 2) || (dst_w % 2) || (dst_h % 2)) {
		DiagPrintf(
			"[GE_YUV_RESHAPE] Error: YUV420 requires even dimensions\n\r");
		return -3;
	}

	/* Check overlap alignment (must be even) */
	if ((x_overlap % 2) != 0 || (y_overlap % 2) != 0) {
		DiagPrintf(
			"[GE_YUV_RESHAPE] Error: Overlap must be even (x_overlap=%d, y_overlap=%d)\n\r",
			x_overlap, y_overlap);
		return -4;
	}

	/* Validate source dimensions match expected pattern
	 * src_w should equal dst_w/2 + x_overlap (e.g., 1920 + 6 = 1926)
	 */
	if (src_w != dst_w_half + x_overlap) {
		DiagPrintf(
			"[GE_YUV_RESHAPE] Error: src_w (%d) must equal dst_w/2 (%d) + x_overlap (%d)\n\r",
			src_w, dst_w_half, x_overlap);
		return -5;
	}

	if (src_h != (dst_h + y_overlap) * 2) {
		DiagPrintf(
			"[GE_YUV_RESHAPE] Error: src_h (%d) must equal (dst_h (%d) + y_overlap (%d)) × 2\n\r",
			src_h, dst_h, y_overlap);
		return -6;
	}

	/* Calculate YUV420 buffer sizes and plane addresses */
	u32 src_y_size = src_w * src_h;
	u32 src_uv_size = src_w * src_h / 2;
	u32 dst_y_size = dst_w * dst_h; /* Output is dst_w wide */
	u32 dst_uv_size = dst_w * dst_h / 2;

	u8 *src_y_addr = (u8 *)src_buf;
	/* GE hardware requires 16-byte aligned addresses.
	 * Align Y plane size up so UV plane starts on a 16-byte boundary.
	 * The ISP output buffer must be allocated with this same aligned offset. */
	u32 src_y_size_aligned = (src_y_size + 15) & ~15;
	u8 *src_uv_addr = (u8 *)src_buf + src_y_size_aligned;
	u8 *dst_y_addr = (u8 *)dst_buf;
	u8 *dst_uv_addr = (u8 *)dst_buf + dst_y_size;

	/* Treat YUV data as ARGB8888 for GE operations
	 * Each ARGB8888 pixel = 4 YUV bytes
	 */
	memset(&blit_cfg, 0, sizeof(blit_cfg));
	blit_cfg.cmd = GE_BLT_ROI_BLEND;
	blit_cfg.work_mode = FILL_SRCRECT;

	/* Convert dimensions to ARGB8888 units */
	u32 src_y_argb_width = src_w / 4;
	u32 src_uv_argb_width = src_w / 4;
	u32 dst_y_argb_width = dst_w / 4; /* Output width in ARGB units */
	u32 dst_uv_argb_width = dst_w / 4;
	u32 dst_w_half_argb =
		dst_w_half / 4; /* Single half width in ARGB units */

	/* ===== Process Y Plane ===== */

	/* Source image dimensions (entire vertically stacked image) */
	blit_cfg.src_img.w = src_y_argb_width;
	blit_cfg.src_img.h = src_h;
	blit_cfg.src_img.img_fmt = GE_BLT_ARGB8888;

	/* Destination image dimensions (horizontal output) */
	blit_cfg.dst_img.w = dst_y_argb_width;
	blit_cfg.dst_img.h = dst_h;
	blit_cfg.dst_img.img_fmt = GE_BLT_ARGB8888;

	/* Y: Top region -> Left side of output
	 * Crop from (0, y_overlap/2) to skip top padding, extract clean middle portion
	 */
	blit_cfg.src_rect.x = start_x / 4;
	blit_cfg.src_rect.y = start_y; /* Skip y_overlap/2 at top */
	blit_cfg.src_rect.w = dst_w_half_argb;
	blit_cfg.src_rect.h = dst_h;

	blit_cfg.dst_rect.x = 0;
	blit_cfg.dst_rect.y = 0;
	blit_cfg.dst_rect.w = dst_w_half_argb;
	blit_cfg.dst_rect.h = dst_h;

	blit_cfg.usr_src_buf = (u32)src_y_addr;
	blit_cfg.usr_dst_buf = (u32)dst_y_addr;

	ret = ge_ctrl(pge_drv_ctx, GEDRV_CTRL_BLIT_START, &blit_cfg);
	if (ret != 0) {
		GE_RESHAPE_LOG("[GE_YUV_RESHAPE] Y top region blit failed\n\r");
		return -7;
	}
	ge_ctrl(pge_drv_ctx, GEDRV_CTRL_BLIT_CHK_END, NULL);

	/* Y: Bottom region -> Right side of output
	 * Crop from (x_overlap, src_h/2 + y_overlap/2) to skip top padding of bottom half
	 */
	u32 bottom_x = start_x + x_overlap;
	u32 bottom_y =
		src_h / 2 +
		y_overlap /
		2; /* Start from middle of stacked image + skip top padding */

	blit_cfg.src_rect.x = bottom_x / 4;
	blit_cfg.src_rect.y = bottom_y;
	blit_cfg.src_rect.w = dst_w_half_argb;
	blit_cfg.src_rect.h = dst_h;

	blit_cfg.dst_rect.x = dst_w_half_argb; /* Right half of output */
	blit_cfg.dst_rect.y = 0;
	blit_cfg.dst_rect.w = dst_w_half_argb;
	blit_cfg.dst_rect.h = dst_h;

	ret = ge_ctrl(pge_drv_ctx, GEDRV_CTRL_BLIT_START, &blit_cfg);
	if (ret != 0) {
		GE_RESHAPE_LOG("[GE_YUV_RESHAPE] Y bottom region blit failed\n\r");
		return -8;
	}
	ge_ctrl(pge_drv_ctx, GEDRV_CTRL_BLIT_CHK_END, NULL);

	/* ===== Process UV Plane ===== */

	/* Source image dimensions (entire vertically stacked UV plane) */
	blit_cfg.src_img.w = src_uv_argb_width;
	blit_cfg.src_img.h = src_h / 2; /* UV height is half of Y */
	blit_cfg.src_img.img_fmt = GE_BLT_ARGB8888;

	/* Destination image dimensions (horizontal UV output) */
	blit_cfg.dst_img.w = dst_uv_argb_width;
	blit_cfg.dst_img.h = dst_h / 2;
	blit_cfg.dst_img.img_fmt = GE_BLT_ARGB8888;

	/* UV: Top region -> Left side of output
	 * Crop from (0, y_overlap/4) - UV is vertically subsampled by 2
	 */
	u32 start_y_uv = start_y / 2; /* y_overlap/2 / 2 = y_overlap/4 */
	blit_cfg.src_rect.x = start_x / 4;
	blit_cfg.src_rect.y = start_y_uv; /* UV vertical subsampling */
	blit_cfg.src_rect.w = dst_w_half_argb;
	blit_cfg.src_rect.h = dst_h / 2;

	blit_cfg.dst_rect.x = 0;
	blit_cfg.dst_rect.y = 0;
	blit_cfg.dst_rect.w = dst_w_half_argb;
	blit_cfg.dst_rect.h = dst_h / 2;

	blit_cfg.usr_src_buf = (u32)src_uv_addr;
	blit_cfg.usr_dst_buf = (u32)dst_uv_addr;

	ret = ge_ctrl(pge_drv_ctx, GEDRV_CTRL_BLIT_START, &blit_cfg);
	if (ret != 0) {
		GE_RESHAPE_LOG("[GE_YUV_RESHAPE] UV top region blit failed\n\r");
		return -9;
	}
	ge_ctrl(pge_drv_ctx, GEDRV_CTRL_BLIT_CHK_END, NULL);

	/* UV: Bottom region -> Right side of output
	 * Crop from (x_overlap, src_h/4 + y_overlap/4) - UV is vertically subsampled
	 */
	u32 bottom_uv_y =
		bottom_y /
		2; /* (src_h/2 + y_overlap/2) / 2 = src_h/4 + y_overlap/4 */

	blit_cfg.src_rect.x = bottom_x / 4;
	blit_cfg.src_rect.y = bottom_uv_y;
	blit_cfg.src_rect.w = dst_w_half_argb;
	blit_cfg.src_rect.h = dst_h / 2;

	blit_cfg.dst_rect.x = dst_w_half_argb; /* Right half of output */
	blit_cfg.dst_rect.y = 0;
	blit_cfg.dst_rect.w = dst_w_half_argb;
	blit_cfg.dst_rect.h = dst_h / 2;

	ret = ge_ctrl(pge_drv_ctx, GEDRV_CTRL_BLIT_START, &blit_cfg);
	if (ret != 0) {
		GE_RESHAPE_LOG("[GE_YUV_RESHAPE] UV bottom region blit failed\n\r");
		return -10;
	}
	ge_ctrl(pge_drv_ctx, GEDRV_CTRL_BLIT_CHK_END, NULL);

	return 0;
}
