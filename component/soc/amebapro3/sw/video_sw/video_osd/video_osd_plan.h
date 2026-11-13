#ifndef _VIDEO_OSD_PLAN_H_
#define _VIDEO_OSD_PLAN_H_
#include "video_osd_internal.h"

typedef struct {
	uint32_t members;
	video_osd_region_t region;
	video_osd_format_t format;
	uint8_t alpha, y, u, v;
} video_osd_plan_region_t;
typedef struct {
	int count;
	video_osd_plan_region_t regions[VIDEO_OSD_MAX_ITEMS];
} video_osd_plan_t;

size_t video_osd_plan_region_bytes(const video_osd_plan_region_t *region);
int video_osd_plan_build(const video_osd_target_t *target, const video_osd_render_item_t *items,
						 int count, int max_regions, video_osd_plan_t *plan);
void video_osd_plan_render(uint8_t *buffer, const video_osd_plan_region_t *region,
						   const video_osd_render_item_t *items, int count);
#endif
