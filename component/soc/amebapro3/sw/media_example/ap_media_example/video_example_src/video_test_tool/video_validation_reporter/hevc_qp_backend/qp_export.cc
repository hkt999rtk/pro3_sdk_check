// Exact luma QP export for libde265 v1.0.16. No image/colour-map inference.
#include "libde265/de265.h"
#include "libde265/image.h"
#include "libde265/slice.h"
#include <algorithm>
#include <stdint.h>

struct VideoQPBlock {
    int32_t x, y, w, h, qp, slice_qp, flags;
};

extern "C" LIBDE265_API int video_qp_abi() { return 1; }

// Returns number of coding blocks; negative means invalid/incomplete metadata.
// Call with out=nullptr to obtain required capacity. Coordinates are coded luma pixels.
extern "C" LIBDE265_API int video_qp_copy(const de265_image* img,
        VideoQPBlock* out, int capacity, int* width, int* height) {
    if (!img || !width || !height || capacity < 0) return -1;
    const auto& sps = img->get_sps();
    *width = sps.pic_width_in_luma_samples;
    *height = sps.pic_height_in_luma_samples;
    int count = 0;
    for (int cy = 0; cy < sps.PicHeightInMinCbsY; cy++) {
        for (int cx = 0; cx < sps.PicWidthInMinCbsY; cx++) {
            const int log2size = img->get_log2CbSize_cbUnits(cx, cy);
            if (!log2size) continue;
            if (log2size < 3 || log2size > 6) return -2;
            const int x = cx * sps.MinCbSizeY, y = cy * sps.MinCbSizeY;
            if (x >= *width || y >= *height) continue;
            const auto* slice = img->get_SliceHeaderCtb(x >> sps.Log2CtbSizeY, y >> sps.Log2CtbSizeY);
            if (!slice) return -3;
            if (out) {
                if (count >= capacity) return -4;
                out[count] = {x, y, std::min(1 << log2size, *width - x),
                              std::min(1 << log2size, *height - y), img->get_QPY(x,y),
                              slice->SliceQPY, (img->get_pcm_flag(x,y) ? 1 : 0) | (img->get_cu_transquant_bypass(x,y) ? 2 : 0)};
            }
            ++count;
        }
    }
    return count;
}
