#include <string.h>
#include "video_osd_polygon.h"

typedef video_osd_privacy_vertex_t point_t;

static int64_t cross(point_t a, point_t b, point_t c)
{
	return ((int64_t)b.x - a.x) * ((int64_t)c.y - a.y) -
		   ((int64_t)b.y - a.y) * ((int64_t)c.x - a.x);
}

static int between(uint32_t a, uint32_t b, uint32_t c)
{
	return c >= (a < b ? a : b) && c <= (a > b ? a : b);
}

static int on_segment(point_t a, point_t b, point_t c)
{
	return cross(a, b, c) == 0 && between(a.x, b.x, c.x) && between(a.y, b.y, c.y);
}

static int intersects(point_t a, point_t b, point_t c, point_t d)
{
	int64_t ab_c = cross(a, b, c), ab_d = cross(a, b, d);
	int64_t cd_a = cross(c, d, a), cd_b = cross(c, d, b);
	return (((ab_c < 0 && ab_d > 0) || (ab_c > 0 && ab_d < 0)) &&
			((cd_a < 0 && cd_b > 0) || (cd_a > 0 && cd_b < 0))) ||
		   on_segment(a, b, c) || on_segment(a, b, d) || on_segment(c, d, a) || on_segment(c, d, b);
}

static int valid(const video_osd_privacy_polygon_t *p)
{
	int64_t area = 0;
	uint32_t n = p->vertex_count;
	for (uint32_t i = 0; i < n; i++) {
		point_t a = p->vertices[i], b = p->vertices[(i + 1) % n];
		if (a.x == b.x && a.y == b.y) {
			return 0;
		}
		area += (int64_t)a.x * b.y - (int64_t)b.x * a.y;
		/* Reject an adjacent edge folding back over its predecessor. */
		point_t c = p->vertices[(i + 2) % n];
		if (cross(a, b, c) == 0 && (on_segment(a, b, c) || on_segment(b, c, a))) {
			return 0;
		}
		for (uint32_t j = i + 1; j < n; j++) {
			if (j == (i + 1) % n || (j + 1) % n == i) {
				continue;
			}
			if (intersects(a, b, p->vertices[j], p->vertices[(j + 1) % n])) {
				return 0;
			}
		}
	}
	return area != 0;
}

int video_osd_polygon_map(const video_osd_privacy_scene_t *scene, uint32_t width, uint32_t height,
						  video_osd_privacy_polygon_t *mapped, video_osd_region_t *bounds)
{
	uint32_t rw = VIDEO_OSD_PRIVACY_NORMALIZED_MAX, rh = rw;
	if (!scene || scene->version != VIDEO_OSD_PRIVACY_VERSION || scene->enabled > 1 ||
		scene->polygon_count > VIDEO_OSD_PRIVACY_MAX_POLYGONS || scene->fill_rgb > 0xffffffU) {
		return -1;
	}
	if (scene->coordinates == VIDEO_OSD_PRIVACY_REFERENCE_PIXELS) {
		rw = scene->reference_width;
		rh = scene->reference_height;
		if (!rw || !rh || rw > 1000000U || rh > 1000000U) {
			return -1;
		}
	} else if (scene->coordinates != VIDEO_OSD_PRIVACY_NORMALIZED) {
		return -1;
	}
	memset(mapped, 0, sizeof(*mapped) * VIDEO_OSD_PRIVACY_MAX_POLYGONS);
	memset(bounds, 0, sizeof(*bounds) * VIDEO_OSD_PRIVACY_MAX_POLYGONS);
	for (uint32_t i = 0; i < scene->polygon_count; i++) {
		video_osd_privacy_polygon_t p = scene->polygons[i];
		if (p.enabled > 1 || p.vertex_count < 3 || p.vertex_count > VIDEO_OSD_PRIVACY_MAX_VERTICES) {
			return -1;
		}
		for (uint32_t j = 0; j < i; j++) {
			if (p.id == scene->polygons[j].id) {
				return -1;
			}
		}
		if (p.vertices[0].x == p.vertices[p.vertex_count - 1].x &&
			p.vertices[0].y == p.vertices[p.vertex_count - 1].y) {
			p.vertex_count--;
		}
		if (p.vertex_count < 3) {
			return -1;
		}
		for (uint32_t j = 0; j < p.vertex_count; j++) {
			if (p.vertices[j].x > rw || p.vertices[j].y > rh) {
				return -1;
			}
		}
		if (!valid(&p)) {
			return -1;
		}
		uint32_t left = width, top = height, right = 0, bottom = 0;
		for (uint32_t j = 0; j < p.vertex_count; j++) {
			p.vertices[j].x = ((uint64_t)p.vertices[j].x * width + rw / 2) / rw;
			p.vertices[j].y = ((uint64_t)p.vertices[j].y * height + rh / 2) / rh;
			uint32_t x = p.vertices[j].x, y = p.vertices[j].y;
			if (x < left) {
				left = x;
			}
			if (x > right) {
				right = x;
			}
			if (y < top) {
				top = y;
			}
			if (y > bottom) {
				bottom = y;
			}
		}
		if (!valid(&p)) {
			return -1;
		}
		mapped[i] = p;
		/* Include the conservative one-pixel outline in conflict bounds. */
		bounds[i].x = left ? left - 1 : 0;
		bounds[i].y = top ? top - 1 : 0;
		bounds[i].width = (right < width ? right + 1 : width) - bounds[i].x;
		bounds[i].height = (bottom < height ? bottom + 1 : height) - bounds[i].y;
	}
	return 0;
}

typedef struct {
	void *pixels;
	video_osd_format_t format;
	video_osd_region_t region;
} polygon_canvas_t;

static void span(polygon_canvas_t *canvas, int y, int left, int right, uint32_t color)
{
	uint32_t width = canvas->region.width, height = canvas->region.height;
	y -= (int)canvas->region.y;
	left -= (int)canvas->region.x;
	right -= (int)canvas->region.x;
	if (y < 0 || y >= (int)height) {
		return;
	}
	if (left < 0) {
		left = 0;
	}
	if (right >= (int)width) {
		right = width - 1;
	}
	for (int x = left; x <= right; x++) {
		if (canvas->format == VIDEO_OSD_FORMAT_BITMAP) {
			((uint8_t *)canvas->pixels)[(size_t)y * (width / 8) + x / 8] |= 0x80U >> (x & 7);
		} else {
			((uint32_t *)canvas->pixels)[(size_t)y * width + x] = color;
		}
	}
}

void video_osd_polygon_render(void *pixels, video_osd_format_t format, const video_osd_region_t *region,
							  const video_osd_privacy_polygon_t *polygon, uint32_t rgb)
{
	polygon_canvas_t canvas = {pixels, format, *region};
	uint32_t color = 0xff000000U | rgb;
	uint32_t n = polygon->vertex_count;
	for (uint32_t y = region->y; y < region->y + region->height; y++) {
		int64_t xs[VIDEO_OSD_PRIVACY_MAX_VERTICES];
		int count = 0;
		for (uint32_t i = 0; i < n; i++) {
			point_t a = polygon->vertices[i], b = polygon->vertices[(i + 1) % n];
			if (a.y > b.y) {
				point_t t = a;
				a = b;
				b = t;
			}
			if (y < a.y || y >= b.y) {
				continue;
			}
			int64_t x = ((int64_t)a.x << 16) + ((int64_t)(2 * y + 1 - 2 * a.y) *
												((int64_t)b.x - a.x) * 32768) / (b.y - a.y);
			int j = count++;
			while (j && xs[j - 1] > x) {
				xs[j] = xs[j - 1];
				j--;
			}
			xs[j] = x;
		}
		for (int j = 0; j + 1 < count; j += 2) {
			span(&canvas, y, (int)(xs[j] >> 16), (int)((xs[j + 1] + 65535) >> 16), color);
		}
	}
	/* Conservative boundary coverage; no antialiasing/translucent edge pixels. */
	for (uint32_t i = 0; i < n; i++) {
		int x = polygon->vertices[i].x, y = polygon->vertices[i].y;
		int ex = polygon->vertices[(i + 1) % n].x, ey = polygon->vertices[(i + 1) % n].y;
		int dx = ex > x ? ex - x : x - ex, dy = ey > y ? y - ey : ey - y;
		int sx = x < ex ? 1 : -1, sy = y < ey ? 1 : -1, err = dx + dy;
		for (;;) {
			span(&canvas, y - 1, x - 1, x, color);
			span(&canvas, y, x - 1, x, color);
			if (x == ex && y == ey) {
				break;
			}
			int twice = 2 * err;
			if (twice >= dy) {
				err += dy;
				x += sx;
			}
			if (twice <= dx) {
				err += dx;
				y += sy;
			}
		}
	}
}

void video_osd_polygon_fill(uint32_t *pixels, uint32_t width, uint32_t height,
							const video_osd_privacy_polygon_t *polygon, uint32_t rgb)
{
	video_osd_region_t region = {0, 0, width, height};
	video_osd_polygon_render(pixels, VIDEO_OSD_FORMAT_ARGB8888, &region, polygon, rgb);
}
