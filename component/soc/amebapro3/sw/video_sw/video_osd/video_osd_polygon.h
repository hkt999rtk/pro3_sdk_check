#ifndef _VIDEO_OSD_POLYGON_H_
#define _VIDEO_OSD_POLYGON_H_
#include "video_osd_privacy.h"
#include "video_osd_internal.h"

int video_osd_polygon_map(const video_osd_privacy_scene_t *scene, uint32_t width, uint32_t height,
						  video_osd_privacy_polygon_t *mapped, video_osd_region_t *bounds);
void video_osd_polygon_fill(uint32_t *pixels, uint32_t width, uint32_t height,
							const video_osd_privacy_polygon_t *polygon, uint32_t rgb);
void video_osd_polygon_render(void *pixels, video_osd_format_t format, const video_osd_region_t *region,
							  const video_osd_privacy_polygon_t *polygon, uint32_t rgb);
#endif
