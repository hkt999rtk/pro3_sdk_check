#include <assert.h>
#include <stdint.h>
#include "slice_sram.h"

int main(void)
{
    uint32_t bytes = 0;
    assert(slice_sram_y_bytes(1920, 16, 4, 196608, &bytes) == 0);
    assert(bytes == 122880 && bytes + bytes / 2 == 184320);
    assert(slice_sram_y_bytes(2048, 16, 4, 196608, &bytes) == 0);
    assert(bytes + bytes / 2 == 196608);
    assert(slice_sram_y_bytes(2049, 16, 4, 196608, &bytes) < 0);
    assert(slice_sram_y_bytes(1920, 16, 8, 196608, &bytes) < 0);
    assert(slice_sram_y_bytes(2160, 16, 2, 196608, &bytes) == 0);
    assert(bytes == 2176 * 16 * 2);
    assert(slice_sram_y_bytes(0, 16, 4, 196608, &bytes) < 0);
    assert(slice_sram_y_bytes(UINT32_MAX, 16, 4, 196608, &bytes) < 0);
    assert(slice_sram_y_bytes(1920, 16, 0, 196608, &bytes) < 0);
    assert(slice_sram_y_bytes(1920, 16, 9, 196608, &bytes) < 0);
    assert(slice_sram_y_bytes(1920, 32, 4, 196608, &bytes) < 0);
    assert(slice_sram_y_bytes(1920, 16, 4, 184319, &bytes) < 0);
    return 0;
}
