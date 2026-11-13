/******************************************************************************
*
* Copyright(c) 2007 - 2025 Realtek Corporation. All rights reserved.
*
******************************************************************************/
#ifndef _VIDEO_OSD_FONT_H_
#define _VIDEO_OSD_FONT_H_

#include <stdint.h>

/* Maximum cell units, not the actual character advance. Scale 1/2/3/5 selects
 * native glyphs in 8/16/24/40-pixel Ameba OSD Sans Bold cells; other positive
 * scales retain NN compatibility by resampling the 24-pixel table.
 * ASCII 0x20..0x7e, 1-bit, proportional advances with equal-width digits and
 * 1/1/2/3-pixel extra spacing at scale 1/2/3/5, capped at the maximum cell width.
 * Use video_osd_font_string_width() for layout; no kerning or grayscale AA. */
#define VIDEO_OSD_FONT_WIDTH  8
#define VIDEO_OSD_FONT_HEIGHT 8

typedef struct {
	uint8_t *buffer;
	int width;
	int height;
} video_osd_font_canvas_t;

void video_osd_font_draw_char(video_osd_font_canvas_t *canvas, int x, int y, char c, int scale);
void video_osd_font_draw_string(video_osd_font_canvas_t *canvas, int x, int y, const char *string, int scale);
/* Sum of the same advances used by drawing; 0 for invalid input/scale,
 * saturated at INT_MAX if the measured width cannot fit in an int. */
int video_osd_font_string_width(const char *string, int scale);

#endif /* _VIDEO_OSD_FONT_H_ */
