/* Opt-in test, not included in production CMake. Return the failure count.
 * Compile/run only in an explicitly authorized test build. */
#include <string.h>
#include <limits.h>
#include "video_osd_font.h"
#include "video_osd_font_data.h"

#define CHECK(c) do { if (!(c)) { failures++; } } while (0)

int video_osd_font_test(void)
{
	int failures = 0;
	uint8_t storage[2 + 7 * 48];
	video_osd_font_canvas_t canvas = {storage + 1, 56, 48};
	/* Native scale 5 plus legacy NN resampling at scales 4 and 6. */
	for (int scale = 1; scale <= 6; scale++) {
		int source_scale = scale == 5 ? 5 : (scale < 3 ? scale : 3);
		int table = source_scale == 5 ? 3 : source_scale - 1;
		int digit_width = video_osd_font_string_width("0", scale);
		for (char digit = '0'; digit <= '9'; digit++) {
			char text[] = {digit, '\0'};
			CHECK(video_osd_font_string_width(text, scale) == digit_width);
		}
		CHECK(video_osd_font_string_width(":", scale) < digit_width);
		CHECK(video_osd_font_string_width(" ", scale) < digit_width);
		for (int c = 32; c <= 126; c++) {
			char text[] = {(char)c, '\0'};
			int advance = (video_osd_font_advance[table][c - 32] * scale + source_scale - 1) / source_scale;
			CHECK(video_osd_font_string_width(text, scale) == advance);
			CHECK(advance > 0 && advance <= 8 * scale);
			const uint8_t *glyph = source_scale == 1 ? video_osd_font_8[c - 32] :
								   source_scale == 2 ? video_osd_font_16[c - 32] :
								   source_scale == 5 ? video_osd_font_40[c - 32] : video_osd_font_24[c - 32];
			for (int clipped = 0; clipped < 2; clipped++) {
				int x = clipped ? -5 : 3, y = clipped ? -7 : 2;
				memset(storage, 0, sizeof(storage));
				storage[0] = storage[sizeof(storage) - 1] = 0xa5;
				video_osd_font_draw_char(&canvas, x, y, (char)c, scale);
				CHECK(storage[0] == 0xa5 && storage[sizeof(storage) - 1] == 0xa5);
				for (int row = 0; row < canvas.height; row++) {
					for (int column = 0; column < canvas.width; column++) {
						int gx = column - x, gy = row - y, expected = 0;
						if (gx >= 0 && gy >= 0 && gx < advance && gy < 8 * scale) {
							gx = gx * source_scale / scale;
							gy = gy * source_scale / scale;
							expected = !!(glyph[gy * source_scale + gx / 8] & (0x80U >> (gx & 7)));
						}
						CHECK(!!(canvas.buffer[row * 7 + column / 8] & (0x80U >> (column & 7))) == expected);
					}
				}
			}
		}
	}
	memset(storage, 0, sizeof(storage));
	video_osd_font_draw_char(&canvas, 0, 0, (char)0xff, 3);
	video_osd_font_draw_char(&canvas, 0, 0, 'A', 0);
	video_osd_font_draw_char(&canvas, 0, 0, 'A', -1);
	video_osd_font_draw_char(&canvas, 100, 100, 'A', 3);
	for (unsigned i = 0; i < sizeof(storage); i++) {
		CHECK(storage[i] == 0);
	}
	CHECK(video_osd_font_string_width("2026-09-22 18:35:07", 3) == 259);
	CHECK(video_osd_font_string_width("1111-11-11 11:11:11", 3) == 259);
	CHECK(video_osd_font_string_width("8888-88-88 88:88:88", 3) == 259);
	CHECK(video_osd_font_string_width("2026-09-22 18:35:07", 5) == 428);
	CHECK(video_osd_font_string_width("1111-11-11 11:11:11", 5) == 428);
	CHECK(video_osd_font_string_width("8888-88-88 88:88:88", 5) == 428);
	CHECK(video_osd_font_string_width("", 3) == 0);
	CHECK(video_osd_font_string_width(NULL, 3) == 0);
	CHECK(video_osd_font_string_width("A", INT_MAX) == 0);
	CHECK(video_osd_font_string_width("\xff", 3) == video_osd_font_string_width(" ", 3));
	/* String layout equals individual glyph placement using measured advances,
	 * both left- and right-aligned inside the unchanged timestamp reservation. */
	static uint8_t actual[760 * 40 / 8], expected[760 * 40 / 8];
	const char *samples[] = {"2026-09-22 18:35:07", "Person 98% Wi gjpqy", "09/22/2026 06:35 PM"};
	for (int scale = 3; scale <= 5; scale += 2) {
		int table = scale == 5 ? 3 : 2;
		int reserved_width = 19 * 8 * scale, height = 8 * scale;
		video_osd_font_canvas_t actual_canvas = {actual, reserved_width, height};
		video_osd_font_canvas_t expected_canvas = {expected, reserved_width, height};
		for (unsigned i = 0; i < sizeof(samples) / sizeof(samples[0]); i++) {
			int width = video_osd_font_string_width(samples[i], scale);
			for (int right = 0; right < 2; right++) {
				memset(actual, 0, sizeof(actual));
				memset(expected, 0, sizeof(expected));
				int start = right ? reserved_width - width : 0, cursor = start;
				video_osd_font_draw_string(&actual_canvas, start, 0, samples[i], scale);
				for (const char *p = samples[i]; *p; p++) {
					video_osd_font_draw_char(&expected_canvas, cursor, 0, *p, scale);
					cursor += video_osd_font_advance[table][(unsigned char) * p - 32];
				}
				CHECK(cursor == start + width);
				CHECK(memcmp(actual, expected, sizeof(actual)) == 0);
				for (int y = 0; y < height; y++) {
					for (int x = 0; x < reserved_width; x++) {
						if (x < start || x >= cursor) {
							CHECK(!(actual[y * (reserved_width / 8) + x / 8] & (0x80U >> (x & 7))));
						}
					}
				}
			}
		}
	}
	return failures;
}
