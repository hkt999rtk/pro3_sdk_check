#include "ameba_soc.h"
#include "FreeRTOS.h"
#include "task.h"

/* heap5 init */
extern u8 __heap_start__[];
extern u8 __heap_end__[];
extern u32 total_heap_size;

__attribute__((constructor))
void heap_init(void)
{
	uint8_t *heap0_start = __heap_start__;
	uint32_t heap0_size = (u32)__heap_end__ - (u32)__heap_start__;

	total_heap_size += heap0_size;

	const HeapRegion_t xHeapRegions[] = {
		{heap0_start, heap0_size},
		{NULL, 0}
	};
	vPortDefineHeapRegions(xHeapRegions);
	RTK_LOGI("FP_INIT", "Heap Init\n");
}
