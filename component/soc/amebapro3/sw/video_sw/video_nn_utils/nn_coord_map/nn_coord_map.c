/******************************************************************************
*
* Copyright(c) 2007 - 2025 Realtek Corporation. All rights reserved.
*
******************************************************************************/
#include "ameba_soc.h"
#include "nn_coord_map.h"

#define _CLAMP(v, lo, hi)  ((v) < (lo) ? (lo) : (v) > (hi) ? (hi) : (v))

void nn_coord_map_init(nn_coord_map_t *map, int im_w, int im_h,
					   const nn_data_param_t *img_param, nn_coord_map_scale_mode_t mode)
{
	map->im_w = im_w;
	map->im_h = im_h;
	map->base_x = 0;
	map->base_y = 0;
	map->scale_w = im_w;
	map->scale_h = im_h;

	if (mode == NN_COORD_MAP_SCALE_LETTERBOX && img_param &&
		img_param->img.width > 0 && img_param->img.height > 0) {
		float src_w = (float)img_param->img.width;
		float src_h = (float)img_param->img.height;
		float rw = (float)im_w / src_w;
		float rh = (float)im_h / src_h;
		float r  = rh < rw ? rh : rw;
		map->scale_w = (int)(src_w * r);
		map->scale_h = (int)(src_h * r);
		map->base_x  = (im_w - map->scale_w) / 2;
		map->base_y  = (im_h - map->scale_h) / 2;
	}
}

void nn_coord_map_bbox(const nn_coord_map_t *map,
					   float rx0, float ry0, float rx1, float ry1,
					   int *xmin, int *ymin, int *xmax, int *ymax)
{
	/* Coordinates are inclusive pixel indices in [0, im_w-1] x [0, im_h-1]. */
	*xmin = _CLAMP((int)(rx0 * map->scale_w) + map->base_x, 0, map->im_w - 1);
	*ymin = _CLAMP((int)(ry0 * map->scale_h) + map->base_y, 0, map->im_h - 1);
	*xmax = _CLAMP((int)(rx1 * map->scale_w) + map->base_x, 0, map->im_w - 1);
	*ymax = _CLAMP((int)(ry1 * map->scale_h) + map->base_y, 0, map->im_h - 1);
}

void nn_coord_map_point(const nn_coord_map_t *map,
						float rx, float ry, int *x, int *y)
{
	*x = _CLAMP((int)(rx * map->scale_w) + map->base_x, 0, map->im_w - 1);
	*y = _CLAMP((int)(ry * map->scale_h) + map->base_y, 0, map->im_h - 1);
}
