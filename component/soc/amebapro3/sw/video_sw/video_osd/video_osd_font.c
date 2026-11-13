/******************************************************************************
*
* Copyright(c) 2007 - 2025 Realtek Corporation. All rights reserved.
*
******************************************************************************/
#include <limits.h>
#include <stddef.h>
#include "video_osd_font.h"
#include "video_osd_font_data.h"

/* Ameba OSD Sans: independently rasterized 8/16/24/40-pixel font cells derived from
 * Liberation Sans Bold. Font data is SIL OFL 1.1; see fonts/OFL.txt.
 * Proportional advances with tabular digits; source cells still bound the
 * fixed timestamp protection area. No runtime font allocation or kerning. */
static int video_osd_font_source_scale(int scale)
{
	return scale == 5 ? 5 : (scale < 3 ? scale : 3);
}

static int video_osd_font_char_advance(char character, int scale)
{
	if (scale <= 0 || scale > INT_MAX / VIDEO_OSD_FONT_WIDTH) {
		return 0;
	}
	if ((unsigned char)character < 0x20 || (unsigned char)character > 0x7e) {
		character = ' ';
	}
	int source_scale = video_osd_font_source_scale(scale);
	int table = source_scale == 5 ? 3 : source_scale - 1;
	int advance = video_osd_font_advance[table][(unsigned char)character - 0x20];
	if (scale == source_scale) {
		return advance;
	}
	return (int)(((int64_t)advance * scale + source_scale - 1) / source_scale);
}

void video_osd_font_draw_char(video_osd_font_canvas_t *canvas, int x, int y, char character, int scale)
{
	if (!canvas || !canvas->buffer || canvas->width <= 0 || canvas->height <= 0 ||
		scale <= 0 || scale > INT_MAX / VIDEO_OSD_FONT_WIDTH) {
		return;
	}
	if ((unsigned char)character < 0x20 || (unsigned char)character > 0x7e) {
		character = ' ';
	}
	int index = (unsigned char)character - 0x20;
	int source_scale = video_osd_font_source_scale(scale);
	const uint8_t *glyph = source_scale == 1 ? video_osd_font_8[index] :
						   source_scale == 2 ? video_osd_font_16[index] :
						   source_scale == 5 ? video_osd_font_40[index] : video_osd_font_24[index];
	int size = VIDEO_OSD_FONT_WIDTH * scale;
	int advance = video_osd_font_char_advance(character, scale);
	/* Clip before iterating, including negative NN label coordinates. Wider
	 * arithmetic avoids overflow for off-screen coordinates and large scales. */
	int64_t left = x < 0 ? -(int64_t)x : 0;
	int64_t top = y < 0 ? -(int64_t)y : 0;
	int64_t right = (int64_t)canvas->width - x;
	int64_t bottom = (int64_t)canvas->height - y;
	if (right > advance) {
		right = advance;
	}
	if (bottom > size) {
		bottom = size;
	}
	if (left >= right || top >= bottom) {
		return;
	}
	size_t row_bytes = ((size_t)canvas->width + 7U) / 8U;
	for (int row = (int)top; row < (int)bottom; row++) {
		int source_row = scale == source_scale ? row : (int)((int64_t)row * source_scale / scale);
		for (int column = (int)left; column < (int)right; column++) {
			int source_column = scale == source_scale ? column : (int)((int64_t)column * source_scale / scale);
			if (glyph[source_row * source_scale + source_column / 8] & (0x80U >> (source_column & 7))) {
				size_t dx = (size_t)(x + column), dy = (size_t)(y + row);
				canvas->buffer[dy * row_bytes + dx / 8] |= (uint8_t)(0x80U >> (dx & 7));
			}
		}
	}
}

void video_osd_font_draw_string(video_osd_font_canvas_t *canvas, int x, int y, const char *string, int scale)
{
	int64_t cursor_x = x;

	if (!canvas || !canvas->buffer || !string || !video_osd_font_char_advance(' ', scale)) {
		return;
	}
	while (*string && cursor_x < canvas->width) {
		video_osd_font_draw_char(canvas, (int)cursor_x, y, *string, scale);
		cursor_x += video_osd_font_char_advance(*string++, scale);
	}
}

int video_osd_font_string_width(const char *string, int scale)
{
	int width = 0;

	if (!string || !video_osd_font_char_advance(' ', scale)) {
		return 0;
	}
	while (*string) {
		int advance = video_osd_font_char_advance(*string++, scale);
		if (width > INT_MAX - advance) {
			return INT_MAX;
		}
		width += advance;
	}
	return width;
}
