#ifndef _VIDEO_OSD_SCENE_H_
#define _VIDEO_OSD_SCENE_H_

#include "video_common_type.h"

/* Internal AP/VP transaction. Request, pixels and completion must remain alive
 * until completion != PENDING. Completion occupies a dedicated cache line. */
#define VIDEO_OSD_SCENE_PENDING 0U
#define VIDEO_OSD_SCENE_APPLIED 1U
#define VIDEO_OSD_SCENE_REJECTED 2U
#define VIDEO_OSD_SCENE_CLOSED 3U
#define VIDEO_OSD_SCENE_FAULT 4U
#define VIDEO_OSD_SCENE_MAX_REGIONS 12
typedef struct {
	osd_overlay_t overlays[VIDEO_OSD_SCENE_MAX_REGIONS];
	uint32_t count;
	uint32_t codec;
	uint32_t revision;
	uint32_t release;
	volatile uint32_t *completion;
} video_osd_scene_t;

#endif
