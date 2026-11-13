#include "video_osd_internal.h"

static uint32_t clamp(int value)
{
	return value < 0 ? 0U : value > 255 ? 255U : (uint32_t)value;
}

static uint32_t bitmap_color(const video_osd_layer_update_t *layer)
{
	int u = (int)layer->u - 128, v = (int)layer->v - 128;
	return ((uint32_t)layer->alpha << 24) |
		   (clamp(layer->y + ((359 * v) >> 8)) << 16) |
		   (clamp(layer->y - ((88 * u + 183 * v) >> 8)) << 8) |
		   clamp(layer->y + ((454 * u) >> 8));
}

/* Straight alpha source-over. In a privacy area destination alpha is 255, so
 * translucent logos blend with the mask color, never the source video. */
static uint32_t over(uint32_t dst, uint32_t src)
{
	uint32_t sa = src >> 24, da = dst >> 24;
	if (!sa) {
		return dst;
	}
	if (sa == 255 || !da) {
		return src;
	}
	uint32_t alpha = sa * 255 + da * (255 - sa);
	uint32_t out = ((alpha + 127) / 255) << 24;
	for (int shift = 0; shift <= 16; shift += 8) {
		uint32_t value = ((src >> shift) & 255) * sa * 255 + ((dst >> shift) & 255) * da * (255 - sa);
		out |= ((value + alpha / 2) / alpha) << shift;
	}
	return out;
}

void video_osd_composite_layer(uint32_t *pixels, uint32_t width, uint32_t height, const video_osd_layer_update_t *layer)
{
	if (!layer->enable || !layer->buffer || layer->region.x >= width || layer->region.y >= height) {
		return;
	}
	uint32_t color = bitmap_color(layer);
	uint32_t w = layer->region.width, h = layer->region.height;
	if (w > width - layer->region.x) {
		w = width - layer->region.x;
	}
	if (h > height - layer->region.y) {
		h = height - layer->region.y;
	}
	for (uint32_t y = 0; y < h; y++) {
		for (uint32_t x = 0; x < w; x++) {
			uint32_t src;
			if (layer->format == VIDEO_OSD_FORMAT_BITMAP) {
				if (!(layer->buffer[(size_t)y * (layer->region.width / 8) + x / 8] & (0x80U >> (x & 7)))) {
					continue;
				}
				src = color;
			} else {
				src = ((const uint32_t *)layer->buffer)[(size_t)y * layer->region.width + x];
			}
			size_t at = (size_t)(y + layer->region.y) * width + x + layer->region.x;
			pixels[at] = over(pixels[at], src);
		}
	}
}
