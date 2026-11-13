/******************************************************************************
*
* Copyright(c) 2007 - 2025 Realtek Corporation. All rights reserved.
*
******************************************************************************/
#ifndef _NN_COORD_MAP_H_
#define _NN_COORD_MAP_H_

#include "mmf2_module.h"
#include "module_vipnn.h"

/*
 * Coordinate mapping: NN normalized output space → video frame pixels.
 * Used by both OSD overlay drawing and mosaic region placement.
 */

typedef enum {
	/* Normalized NN output spans the full displayed frame. */
	NN_COORD_MAP_SCALE_STRETCH   = 0,
	/* Uniform scale with centering. Use when the NN input kept the source
	 * aspect ratio with padding before inference. */
	NN_COORD_MAP_SCALE_LETTERBOX = 1,
} nn_coord_map_scale_mode_t;

typedef struct {
	int im_w, im_h;        /* video frame resolution */
	int base_x, base_y;    /* top-left for normalized NN output in frame pixels */
	int scale_w, scale_h;  /* pixel span for normalized NN output */
} nn_coord_map_t;

/* Build a coordinate map from normalized NN output to a video frame. */
void nn_coord_map_init(nn_coord_map_t *map, int im_w, int im_h,
					   const nn_data_param_t *img_param, nn_coord_map_scale_mode_t mode);

/* Map a normalised bounding box [0..1] to frame pixel coordinates (clamped). */
void nn_coord_map_bbox(const nn_coord_map_t *map,
					   float rx0, float ry0, float rx1, float ry1,
					   int *xmin, int *ymin, int *xmax, int *ymax);

/* Map a normalised point [0..1] to frame pixel coordinates (clamped). */
void nn_coord_map_point(const nn_coord_map_t *map,
						float rx, float ry, int *x, int *y);

#endif /* _NN_COORD_MAP_H_ */
