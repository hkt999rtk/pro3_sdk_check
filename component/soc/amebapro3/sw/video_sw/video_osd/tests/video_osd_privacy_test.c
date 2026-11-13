/* Opt-in test entry; not linked into production examples. Returns failures. */
#include <string.h>
#include "video_osd_polygon.h"

#define CHECK(condition) do { if (!(condition)) { failures++; } } while (0)
#define TEST_WIDTH 32U
#define TEST_HEIGHT 32U

int video_osd_privacy_geometry_test(void)
{
	int failures = 0;
	/* Keep the optional test usable from a small embedded task stack. */
	static video_osd_privacy_scene_t scene;
	static video_osd_privacy_polygon_t mapped[VIDEO_OSD_PRIVACY_MAX_POLYGONS];
	static video_osd_region_t bounds[VIDEO_OSD_PRIVACY_MAX_POLYGONS];
	static uint32_t pixels[TEST_WIDTH * TEST_HEIGHT];
	video_osd_privacy_scene_default(&scene);
	scene.polygon_count = 1;
	scene.polygons[0] = (video_osd_privacy_polygon_t) {
		.id = 7, .enabled = 1, .vertex_count = 4,
		.vertices = {{0, 0}, {10000, 0}, {10000, 10000}, {0, 10000}},
	};
	CHECK(video_osd_polygon_map(&scene, TEST_WIDTH, TEST_HEIGHT, mapped, bounds) == 0);
	CHECK(mapped[0].vertices[2].x == TEST_WIDTH && mapped[0].vertices[2].y == TEST_HEIGHT);
	CHECK(bounds[0].x == 0 && bounds[0].y == 0 && bounds[0].width == TEST_WIDTH && bounds[0].height == TEST_HEIGHT);
	memset(pixels, 0, sizeof(pixels));
	video_osd_polygon_fill(pixels, TEST_WIDTH, TEST_HEIGHT, &mapped[0], 0);
	for (unsigned i = 0; i < TEST_WIDTH * TEST_HEIGHT; i++) {
		CHECK(pixels[i] == 0xff000000U);
	}

	/* Translucent red remains opaque over privacy; transparent pixels keep black. */
	uint32_t logo[] = {0x80ff0000U, 0x0000ff00U, 0xffff0000U, 0xff0000ffU};
	video_osd_layer_update_t layer = {
		.enable = 1, .format = VIDEO_OSD_FORMAT_ARGB8888,
		.region = {0, 0, 4, 1}, .buffer = (uint8_t *)logo,
	};
	video_osd_composite_layer(pixels, TEST_WIDTH, TEST_HEIGHT, &layer);
	CHECK(pixels[0] == 0xff800000U && pixels[1] == 0xff000000U);
	CHECK(pixels[2] == 0xffff0000U && pixels[3] == 0xff0000ffU);
	uint8_t bits[] = {0x80};
	layer = (video_osd_layer_update_t) {
		.enable = 1, .format = VIDEO_OSD_FORMAT_BITMAP, .region = {0, 0, 8, 1},
		.alpha = 255, .y = 255, .u = 128, .v = 128, .buffer = bits,
	};
	video_osd_composite_layer(pixels, TEST_WIDTH, TEST_HEIGHT, &layer);
	CHECK(pixels[0] == 0xffffffffU && pixels[1] == 0xff000000U);

	/* Reference-pixel concave L; its notch must remain transparent. */
	scene.coordinates = VIDEO_OSD_PRIVACY_REFERENCE_PIXELS;
	scene.reference_width = scene.reference_height = TEST_WIDTH;
	scene.polygons[0] = (video_osd_privacy_polygon_t) {
		.id = 7, .enabled = 1, .vertex_count = 6,
		.vertices = {{4, 4}, {28, 4}, {28, 12}, {12, 12}, {12, 28}, {4, 28}},
	};
	CHECK(video_osd_polygon_map(&scene, TEST_WIDTH, TEST_HEIGHT, mapped, bounds) == 0);
	memset(pixels, 0, sizeof(pixels));
	video_osd_polygon_fill(pixels, TEST_WIDTH, TEST_HEIGHT, &mapped[0], 0x123456);
	CHECK(pixels[8 * TEST_WIDTH + 20] == 0xff123456U);
	CHECK(pixels[20 * TEST_WIDTH + 8] == 0xff123456U);
	CHECK(pixels[20 * TEST_WIDTH + 20] == 0);
	for (unsigned i = 0; i < 3; i++) {
		video_osd_privacy_vertex_t point = scene.polygons[0].vertices[i];
		scene.polygons[0].vertices[i] = scene.polygons[0].vertices[5 - i];
		scene.polygons[0].vertices[5 - i] = point;
	}
	CHECK(video_osd_polygon_map(&scene, TEST_WIDTH, TEST_HEIGHT, mapped, bounds) == 0);
	memset(pixels, 0, sizeof(pixels));
	video_osd_polygon_fill(pixels, TEST_WIDTH, TEST_HEIGHT, &mapped[0], 0x123456);
	CHECK(pixels[8 * TEST_WIDTH + 20] == 0xff123456U && pixels[20 * TEST_WIDTH + 20] == 0);
	/* Repeated closing point is accepted; a duplicate ID is not. */
	scene.polygons[0].vertices[6] = scene.polygons[0].vertices[0];
	scene.polygons[0].vertex_count = 7;
	CHECK(video_osd_polygon_map(&scene, TEST_WIDTH, TEST_HEIGHT, mapped, bounds) == 0 && mapped[0].vertex_count == 6);
	scene.polygons[1] = scene.polygons[0];
	scene.polygon_count = 2;
	CHECK(video_osd_polygon_map(&scene, TEST_WIDTH, TEST_HEIGHT, mapped, bounds) != 0);
	scene.polygon_count = 1;
	scene.polygons[0].vertices[0].x = TEST_WIDTH + 1;
	CHECK(video_osd_polygon_map(&scene, TEST_WIDTH, TEST_HEIGHT, mapped, bounds) != 0);
	scene.polygons[0] = (video_osd_privacy_polygon_t) {
		.enabled = 1, .vertex_count = 4, .vertices = {{2, 2}, {28, 28}, {2, 28}, {28, 2}},
	};
	CHECK(video_osd_polygon_map(&scene, TEST_WIDTH, TEST_HEIGHT, mapped, bounds) != 0);
	scene.polygons[0].vertex_count = 3;
	scene.polygons[0].vertices[0] = (video_osd_privacy_vertex_t) {
		1, 1
	};
	scene.polygons[0].vertices[1] = (video_osd_privacy_vertex_t) {
		2, 2
	};
	scene.polygons[0].vertices[2] = (video_osd_privacy_vertex_t) {
		3, 3
	};
	CHECK(video_osd_polygon_map(&scene, TEST_WIDTH, TEST_HEIGHT, mapped, bounds) != 0);
	scene.polygon_count = 0;
	CHECK(video_osd_polygon_map(&scene, TEST_WIDTH, TEST_HEIGHT, mapped, bounds) == 0);
	scene.version++;
	CHECK(video_osd_polygon_map(&scene, TEST_WIDTH, TEST_HEIGHT, mapped, bounds) != 0);

	/* Valid source geometry that collapses after downscaling is rejected. */
	video_osd_privacy_scene_default(&scene);
	scene.polygon_count = 1;
	scene.polygons[0] = (video_osd_privacy_polygon_t) {
		.enabled = 1, .vertex_count = 3, .vertices = {{1, 1}, {2, 1}, {1, 2}},
	};
	CHECK(video_osd_polygon_map(&scene, TEST_WIDTH, TEST_HEIGHT, mapped, bounds) != 0);
	scene.polygons[0].vertex_count = VIDEO_OSD_PRIVACY_MAX_VERTICES + 1;
	CHECK(video_osd_polygon_map(&scene, TEST_WIDTH, TEST_HEIGHT, mapped, bounds) != 0);
	scene.polygon_count = VIDEO_OSD_PRIVACY_MAX_POLYGONS + 1;
	CHECK(video_osd_polygon_map(&scene, TEST_WIDTH, TEST_HEIGHT, mapped, bounds) != 0);

	/* Pixel-disjoint rectangles still conflict when they share an encoder block. */
	video_osd_region_t first = {0, 0, 8, 8};
	video_osd_region_t second = {8, 0, 8, 8};
	CHECK(video_osd_region_conflicts(&first, &second, VIDEO_H264));
	second.x = 64;
	CHECK(!video_osd_region_conflicts(&first, &second, VIDEO_H264));
	second.x = 0;
	second.y = 16;
	CHECK(!video_osd_region_conflicts(&first, &second, VIDEO_H264));
	CHECK(video_osd_region_conflicts(&first, &second, VIDEO_HEVC));
	second.y = 64;
	CHECK(!video_osd_region_conflicts(&first, &second, VIDEO_HEVC));
	return failures;
}
