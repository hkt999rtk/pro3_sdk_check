#ifndef IMG_PPU_ROTATE_H
#define IMG_PPU_ROTATE_H
#include <stdint.h>
#include <stddef.h>

#define ROTATE_FRAME_BYTES 16588800u
/* ROTATE_FRAME_BYTES is the default 4K test size, not an API shape limit. */
#define ROTATE_ALIGNMENT 64u
#define ROTATE_SIZE_ALIGNMENT 16u
#define ROTATE_TARGET_CID 0x10000048u
typedef struct {
	uint16_t *data;
	uint32_t width, height, stride_bytes, capacity_bytes;
} img_rotate_image_t;
typedef struct {
	uint64_t api_us, run_us;
	int status;
} img_rotate_timing_t;
typedef struct {
	uint32_t free_bytes, minimum_ever_free_bytes;
} img_rotate_heap_t;
typedef struct {
	uint64_t init_us;
	uint32_t nb_bytes, pool_bytes, customer_id, heap_valid_mask, pool_valid;
	img_rotate_heap_t before_init, after_init, after_create, after_prepare, after_first;
} img_rotate_info_t;
enum { ROTATE_BAD_IMAGE = -10001, ROTATE_BUSY = -10002, ROTATE_NOT_READY = -10003,
	   ROTATE_WRONG_TARGET = -10004, ROTATE_BAD_MODEL = -10005,
	   ROTATE_BAD_ALIGNMENT = -10006
	 };
/* Single-owner API: init/run/deinit from the same FreeRTOS task. No other NN
 * client may run concurrently. This API does not arbitrate other SDK clients.
 * Caller owns both image buffers, each aligned to ROTATE_ALIGNMENT bytes.
 * Width/height must be positive multiples of sixteen, packed U16, with output
 * dimensions swapped. Invalid sizes return ROTATE_BAD_IMAGE before any bind
 * or PPU call. One dynamic-shape NBG serves every supported size. Rebinding
 * after an address/size change incurs setup once; stable pairs reuse it.
 * Unaligned input OR output address returns ROTATE_BAD_ALIGNMENT (-10006).
 * Caller-provided addresses are never rounded, copied or silently adjusted.
 * No image cache maintenance is performed by this API. After CPU writes,
 * caller must clean input and any dirty output lines before device access,
 * with a completion barrier. After successful run, invalidate output before
 * CPU reads. Device-only pipelines must synchronize producers/consumers;
 * no CPU may modify the image buffers while the device owns them. */
int img_ppu_rotate_init(void);
int img_ppu_rotate_u16(const img_rotate_image_t *src, const img_rotate_image_t *dst);
int img_ppu_rotate_deinit(void);
int img_ppu_rotate_get_timing(img_rotate_timing_t *out);
int img_ppu_rotate_get_info(img_rotate_info_t *out);
uint64_t img_ppu_rotate_now_us(void);
/* Pure validation, also exercised in host tests. */
int img_ppu_rotate_validate(const img_rotate_image_t *src, const img_rotate_image_t *dst);
#endif
