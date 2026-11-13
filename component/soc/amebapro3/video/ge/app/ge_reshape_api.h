/******************************************************************************
*
* Copyright(c) 2007 - 2025 Realtek Corporation. All rights reserved.
*
******************************************************************************/
#ifndef __GE_RESHAPE_API_H__
#define __GE_RESHAPE_API_H__

#include <stdint.h>
#include "ge_sys_api.h"

/**
 * GE-based RAW reshape function for 16-bit per pixel data.
 * Reshapes source RAW image to destination dimensions using GE hardware.
 * Mode: Horizontal split to vertical stacking with overlap support
 * Splits wide image (left/right) with configurable overlap, stacks right half below left half
 * Flexible: automatically crops if source dimensions are slightly larger
 * y_overlap is distributed symmetrically: y_overlap/2 added to top and bottom of each region.
 *
 * Crop windows (with symmetric y_overlap):
 * - Left:  (start_x, start_y - y_overlap/2) size (dst_w + x_overlap, dst_h + y_overlap)
 * - Right: (start_x + dst_w - x_overlap, start_y - y_overlap/2) size (dst_w + x_overlap, dst_h + y_overlap)
 *
 * @param pge_drv_ctx GE driver context (must be opened before calling this function)
 * @param src_buf Source buffer pointer (16-bit per pixel RAW data)
 * @param dst_buf Destination buffer pointer (16-bit per pixel RAW data)
 * @param src_w Source width in pixels (full sensor width, e.g., 3848)
 * @param src_h Source height in pixels (full sensor height, e.g., 2168)
 * @param start_x Starting X position for crop window center (typically 0 or alignment offset)
 * @param start_y Starting Y position for crop window center (must be >= y_overlap/2)
 * @param dst_w Destination width per half (e.g., 1920)
 * @param dst_h Destination height per half (e.g., 2040)
 * @param x_overlap Horizontal overlap in pixels for ISP window (must be even)
 * @param y_overlap Vertical overlap in pixels for ISP window (must be even, distributed as y_overlap/2 on each end)
 * @return 0 on success, negative error code on failure
 * @note GE hardware limit: output dimensions must not exceed 4095×4095
 * @note Caller must handle DCache operations (CleanInvalidate before, Invalidate after)
 * @note Caller must open GE device before calling and close after
 * @example ge_raw_reshape(pge_drv_ctx, src, dst, 3848, 2168, 4, 64, 1920, 2040, 8, 6);
 *          // start_y=64, y_overlap=6 → crops Y-range [61, 2107) for both regions (symmetric 3px padding)
 *          // Output: 1928×4092 (vertically stacked, each region has 3px padding top and bottom)
 */
int ge_raw_reshape(ge_drv_ctx_t *pge_drv_ctx, void *src_buf, void *dst_buf,
				   uint32_t src_w, uint32_t src_h, uint32_t start_x,
				   uint32_t start_y, uint32_t dst_w, uint32_t dst_h,
				   uint32_t x_overlap, uint32_t y_overlap);

/**
 * GE-based YUV420 reshape function with overlap removal.
 * Reshapes source YUV420 image to destination dimensions using GE hardware.
 * Mode: Vertical split to horizontal placement with overlap removal
 * Takes vertically-stacked regions (output of ge_raw_reshape after ISP processing)
 * and converts to horizontal side-by-side layout while removing overlap regions.
 * YUV420 format: Y plane (width × height), UV plane (width × height/2)
 * y_overlap is removed symmetrically: y_overlap/2 skipped from top and bottom of each region.
 *
 * Crop operations (calculated internally, symmetric y_overlap/2 removal):
 * - Top region (→ Left):   Extract from (0, y_overlap/2) size (dst_w/2, dst_h)
 * - Bottom region (→ Right): Extract from (x_overlap, src_h/2 + y_overlap/2) size (dst_w/2, dst_h)
 *
 * @param pge_drv_ctx GE driver context (must be opened before calling this function)
 * @param src_buf Source buffer pointer (YUV420 format: Y plane followed by UV plane)
 * @param dst_buf Destination buffer pointer (YUV420 format: Y plane followed by UV plane)
 * @param src_w Source width in pixels (with overlap: dst_w/2 + x_overlap, must be even)
 * @param src_h Source height in pixels (stacked with overlap: (dst_h + y_overlap) × 2, must be even)
 * @param dst_w Final destination width (horizontal side-by-side, without overlap, must be even, e.g., 3840)
 * @param dst_h Final destination height (without overlap, must be even, e.g., 2040)
 * @param x_overlap Horizontal overlap in pixels to remove (must be even)
 * @param y_overlap Vertical overlap in pixels to remove (must be even, removes y_overlap/2 from each end)
 * @return 0 on success, negative error code on failure
 * @note GE hardware limit: input dimensions must not exceed 4095×4095
 * @note Caller must handle DCache operations (CleanInvalidate before, Invalidate after)
 * @note Caller must open GE device before calling and close after
 * @example ge_yuv_reshape(pge_drv_ctx, src, dst, 1926, 4092, 3840, 2040, 6, 6);
 *          // Input:  1926×4092 (vertically stacked, each half: 1926×2046 with 3px padding on each end)
 *          // Extracts clean middle 1920×2040 from each half (skips 3px top and 3px bottom)
 *          // Output: 3840×2040 (horizontal side-by-side, no overlap)
 */
int ge_yuv_reshape(ge_drv_ctx_t *pge_drv_ctx, void *src_buf, void *dst_buf,
				   uint32_t src_w, uint32_t src_h, uint32_t dst_w,
				   uint32_t dst_h, uint32_t x_overlap, uint32_t y_overlap);

#endif /* __GE_RESHAPE_API_H__ */
