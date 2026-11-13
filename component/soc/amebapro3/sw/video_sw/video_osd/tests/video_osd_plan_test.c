/* Opt-in static-fixture test, not reentrant and not part of production CMake.
 * Return failure count. Run only in an explicitly authorized test build. */
#include <string.h>
#include "video_osd_plan.h"
#include "video_osd_polygon.h"

#define CHECK(c) do { if (!(c)) { failures++; } } while (0)

static uint32_t plan_members(const video_osd_plan_t *plan)
{
	uint32_t members = 0;
	for (int i = 0; i < plan->count; i++) {
		members |= plan->regions[i].members;
	}
	return members;
}

int video_osd_plan_test(void)
{
	int failures = 0;
	video_osd_target_t target = {0, VIDEO_H264, 1280, 720};
	static video_osd_render_item_t items[VIDEO_OSD_MAX_ITEMS];
	static video_osd_plan_t plan;
	static uint32_t pixels[32 * 32];
	static uint32_t reference[32 * 32];
	/* All 16 L/T/N/P presence combinations, with disjoint block ranges. */
	const video_osd_region_t bounds[] = {{0, 0, 16, 16}, {256, 0, 80, 8}, {512, 128, 64, 16}, {768, 256, 64, 64}};
	for (unsigned mask = 0; mask < 16; mask++) {
		memset(items, 0, sizeof(items));
		int count = 0;
		size_t expected_bytes = 0;
		for (int type = 0; type < 4; type++) {
			if (!(mask & (1U << type))) {
				continue;
			}
			items[count].layer = (video_osd_layer_update_t) {
				.enable = 1, .region = bounds[type], .alpha = 255, .y = type == 3 ? 0 : 255, .u = 128, .v = 128,
				.format = type == 0 ? VIDEO_OSD_FORMAT_ARGB8888 : VIDEO_OSD_FORMAT_BITMAP,
			};
			expected_bytes += bounds[type].width * bounds[type].height * (type == 0 ? 4U : 1U) / (type == 0 ? 1U : 8U);
			items[count++].version = type + 1;
		}
		CHECK(video_osd_plan_build(&target, items, count, 12, &plan) == 0);
		CHECK(plan.count == count);
		size_t bytes = 0;
		for (int i = 0; i < plan.count; i++) {
			bytes += video_osd_plan_region_bytes(&plan.regions[i]);
		}
		CHECK(bytes == expected_bytes);
		CHECK(video_osd_plan_build(&target, items, count, 0, &plan) == (count ? VIDEO_OSD_ENOSPC : 0));
		if (count) {
			CHECK(video_osd_plan_build(&target, items, count, 1, &plan) == 0);
			CHECK(plan.count == 1);
		}
	}

	/* Conflict caused only by H.264 block sharing: one same-color bitmap. */
	memset(items, 0, sizeof(items));
	items[0].layer = (video_osd_layer_update_t) {
		.enable = 1, .format = VIDEO_OSD_FORMAT_BITMAP, .region = {0, 0, 8, 8}, .alpha = 255, .y = 255, .u = 128, .v = 128,
	};
	items[1] = items[0];
	items[1].layer.region.x = 16;
	CHECK(video_osd_plan_build(&target, items, 2, 12, &plan) == 0 && plan.count == 1);
	CHECK(plan.regions[0].format == VIDEO_OSD_FORMAT_BITMAP);
	items[1].layer.y = 0;
	CHECK(video_osd_plan_build(&target, items, 2, 12, &plan) == 0 && plan.count == 1);
	CHECK(plan.regions[0].format == VIDEO_OSD_FORMAT_ARGB8888);
	/* Matching translucent layers cannot be ORed as an opaque bitmap union. */
	items[1].layer.y = 255;
	items[1].layer.alpha = 128;
	CHECK(video_osd_plan_build(&target, items, 2, 12, &plan) == 0);
	CHECK(plan.regions[0].format == VIDEO_OSD_FORMAT_ARGB8888);

	/* Merging two rectangles can bridge a third: conflict closure is required. */
	items[1] = items[2] = items[0];
	items[0].layer.region = (video_osd_region_t) {
		0, 0, 80, 8
	};
	items[1].layer.region = (video_osd_region_t) {
		64, 8, 8, 56
	};
	items[2].layer.region = (video_osd_region_t) {
		0, 48, 8, 8
	};
	CHECK(video_osd_plan_build(&target, items, 3, 12, &plan) == 0 && plan.count == 1);
	target.codec = VIDEO_HEVC;
	CHECK(video_osd_plan_build(&target, items, 3, 12, &plan) == 0 && plan.count == 1);
	target.codec = VIDEO_H264;

	/* Polygon crop is identical to the full-canvas rasterizer at each pixel. */
	video_osd_privacy_polygon_t polygon = {
		.enabled = 1, .vertex_count = 3, .vertices = {{8, 8}, {16, 10}, {10, 16}},
	};
	memset(items, 0, sizeof(items));
	items[0].layer = (video_osd_layer_update_t) {
		.enable = 1, .format = VIDEO_OSD_FORMAT_BITMAP, .region = {7, 7, 10, 10}, .alpha = 255, .u = 128, .v = 128,
	};
	items[0].polygon = &polygon;
	CHECK(video_osd_plan_build(&target, items, 1, 12, &plan) == 0);
	CHECK(plan.regions[0].format == VIDEO_OSD_FORMAT_BITMAP);
	video_osd_plan_render((uint8_t *)pixels, &plan.regions[0], items, 1);
	memset(reference, 0, sizeof(reference));
	video_osd_polygon_fill(reference, 32, 32, &polygon, 0);
	video_osd_region_t r = plan.regions[0].region;
	for (uint32_t y = 0; y < r.height; y++) {
		for (uint32_t x = 0; x < r.width; x++) {
			int set = (((uint8_t *)pixels)[y * (r.width / 8) + x / 8] & (0x80U >> (x & 7))) != 0;
			CHECK(set == (reference[(y + r.y) * 32 + x + r.x] != 0));
		}
	}
	/* White timestamp bit above an opaque black mask. */
	uint8_t bit = 0x80;
	items[1].layer = (video_osd_layer_update_t) {
		.enable = 1, .format = VIDEO_OSD_FORMAT_BITMAP, .region = {8, 8, 8, 1},
		.alpha = 255, .y = 255, .u = 128, .v = 128, .buffer = &bit,
	};
	CHECK(video_osd_plan_build(&target, items, 2, 12, &plan) == 0 && plan.count == 1);
	CHECK(plan.regions[0].format == VIDEO_OSD_FORMAT_ARGB8888);
	video_osd_plan_render((uint8_t *)pixels, &plan.regions[0], items, 2);
	r = plan.regions[0].region;
	CHECK(pixels[(8 - r.y) * r.width + 8 - r.x] == 0xffffffffU);
	CHECK(pixels[0] == 0);

	/* Only final bitmap padding crosses x=64. Suppress both layers of the
	 * conflicting group, but keep another group. Replanning must restore NN
	 * once it moves away, without modifying or deleting the input items. */
	static const int groups[2] = {0, 1};
	for (int codec = 0; codec < 2; codec++) {
		target.codec = codec ? VIDEO_HEVC : VIDEO_H264;
		memset(items, 0, sizeof(items));
		items[0].layer = (video_osd_layer_update_t) {
			.enable = 1, .format = VIDEO_OSD_FORMAT_BITMAP, .region = {2, 2, 60, 9},
			.alpha = 255, .u = 128, .v = 128,
		};
		items[1] = items[2] = items[3] = items[0];
		items[1].layer.region = (video_osd_region_t) {
			64, 0, 8, 8
		};
		items[2].layer.region = (video_osd_region_t) {
			512, 128, 8, 8
		};
		items[3].layer.region = (video_osd_region_t) {
			896, 256, 8, 8
		};
		items[1].nn_group = items[2].nn_group = &groups[0];
		items[3].nn_group = &groups[1];
		CHECK(!video_osd_region_conflicts(&items[0].layer.region, &items[1].layer.region, target.codec));
		CHECK(video_osd_plan_build(&target, items, 4, 12, &plan) == 0);
		CHECK(plan_members(&plan) == 0x9U && plan.count == 2);
		items[1].layer.region.x = 256;
		CHECK(video_osd_plan_build(&target, items, 4, 12, &plan) == 0);
		CHECK(plan_members(&plan) == 0xfU && plan.count == 4);
	}
	target.codec = VIDEO_H264;

	/* Two protected regions form an L. Their merged rectangle bridges NN,
	 * although neither original rectangle shares an H.264 block with it. */
	memset(items, 0, sizeof(items));
	items[0].layer = (video_osd_layer_update_t) {
		.enable = 1, .format = VIDEO_OSD_FORMAT_BITMAP, .region = {0, 0, 80, 8}, .alpha = 255,
	};
	items[1] = items[2] = items[0];
	items[1].layer.region = (video_osd_region_t) {
		64, 8, 8, 56
	};
	items[2].layer.region = (video_osd_region_t) {
		0, 48, 8, 8
	};
	items[2].nn_group = &groups[0];
	CHECK(!video_osd_region_conflicts(&items[0].layer.region, &items[2].layer.region, target.codec));
	CHECK(!video_osd_region_conflicts(&items[1].layer.region, &items[2].layer.region, target.codec));
	CHECK(video_osd_plan_build(&target, items, 3, 12, &plan) == 0);
	CHECK(plan_members(&plan) == 0x3U && plan.count == 1);

	/* Slot-pressure merges must not mix NN into protected output either. */
	items[0].layer.region = (video_osd_region_t) {
		0, 0, 8, 8
	};
	items[1].layer.region = (video_osd_region_t) {
		128, 64, 8, 8
	};
	items[1].nn_group = &groups[0];
	CHECK(video_osd_plan_build(&target, items, 2, 12, &plan) == 0);
	CHECK(plan_members(&plan) == 0x3U && plan.count == 2);
	CHECK(video_osd_plan_build(&target, items, 2, 1, &plan) == 0);
	CHECK(plan_members(&plan) == 0x1U && plan.count == 1);
	CHECK(video_osd_plan_build(&target, items, 2, 12, &plan) == 0);
	CHECK(plan_members(&plan) == 0x3U);
	/* NN-only regions may still merge, even across different groups. */
	items[0].nn_group = &groups[1];
	CHECK(video_osd_plan_build(&target, items, 2, 1, &plan) == 0);
	CHECK(plan_members(&plan) == 0x3U && plan.count == 1);

	/* Right-edge bitmap padding shifts the protected origin into NN's block. */
	items[0].nn_group = NULL;
	target.frame_width = 130;
	items[0].layer.region = (video_osd_region_t) {
		128, 0, 2, 8
	};
	items[1].layer.region = (video_osd_region_t) {
		120, 0, 8, 8
	};
	CHECK(!video_osd_region_conflicts(&items[0].layer.region, &items[1].layer.region, target.codec));
	CHECK(video_osd_plan_build(&target, items, 2, 12, &plan) == 0);
	CHECK(plan_members(&plan) == 0x1U && plan.regions[0].region.x == 122);
	return failures;
}
