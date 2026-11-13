#include "ameba_soc.h"
#include "FreeRTOS.h"
#include "task.h"

/* heap5 init */
extern u32 total_heap_size;

__attribute__((constructor))
void heap_init(void)
{

#if !defined(CONFIG_DISABLE_PSRAM)
	extern u8 __heap_start__[];
	uint8_t *heap0_start = __heap_start__;
	extern u8 __heap_end__[];
	uint32_t heap0_size = (u32)__heap_end__ - (u32)__heap_start__;
#else
	extern u8 __sram_bss_end__[];
	uint8_t *heap0_start = __sram_bss_end__;
	extern u8 __np_ssram_end__[];
	uint32_t heap0_size = (u32)__np_ssram_end__ - (u32)__sram_bss_end__;
#endif

#if defined(CONFIG_FT2_VERIFY) || defined(CONFIG_OLT_VERIFY)
	total_heap_size += heap0_size;
	uint8_t *heap1_start = (u8 *)0x84000000;
	uint32_t heap1_size = 0x4000000;

	RTK_LOGI("INIT", "Heap Init heap1_start = 0x%X, heap1_size = 0x%X\n", heap1_start, heap1_size);

	total_heap_size += heap1_size;
	const HeapRegion_t xHeapRegions[] = {
		{heap0_start, heap0_size},
		{heap1_start, heap1_size},
		{NULL, 0}
	};

#else
	total_heap_size += heap0_size;

	const HeapRegion_t xHeapRegions[] = {
		{heap0_start, heap0_size},
		{NULL, 0}
	};

#endif



	vPortDefineHeapRegions(xHeapRegions);
	RTK_LOGI("INIT", "Heap Init\n");
}
