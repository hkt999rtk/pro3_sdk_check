#ifndef SLICE_SRAM_H
#define SLICE_SRAM_H

#include <stdint.h>

/* NV12 ring capacity, including UV; use 64-bit arithmetic before narrowing. */
static inline int slice_sram_y_bytes(uint32_t width, uint32_t slice_height,
                                    uint32_t wrap, uint32_t capacity, uint32_t *y_bytes)
{
	if (!width || (slice_height != 16 && slice_height != 64) || wrap < 2 || wrap > 8) {
		return -1;
	}
	uint64_t stride = ((uint64_t)width + 63) & ~UINT64_C(63);
	uint64_t y = stride * slice_height * wrap;
	if (y + y / 2 > capacity) {
		return -1;
	}
	*y_bytes = (uint32_t)y;
	return 0;
}

#endif
