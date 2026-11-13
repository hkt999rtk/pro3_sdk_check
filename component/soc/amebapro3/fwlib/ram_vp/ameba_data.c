#include <string.h>
#include <stdint.h>
#include <ameba.h>
#include <platform_autoconf.h>

extern uint8_t __bkup_s1_data_start__[];
extern uint8_t __bkup_s1_data_end__[];
extern uint8_t __vp_s1_data_start__[];
extern uint8_t __vp_s1_data_end__[];

extern uint8_t __bkup_data_start__[];
extern uint8_t __bkup_data_end__[];
extern uint8_t __vp_data_start__[];
extern uint8_t __vp_data_end__[];

extern uint8_t __bss_start__[];
extern uint8_t __bss_end__[];

extern uint8_t __bss_bf_start__[];
extern uint8_t __bss_bf_end__[];

void stage1_bss_init(void)
{
	if (__bss_end__ - __bss_start__ > 0) {
		memset(__bss_start__, 0, __bss_end__ - __bss_start__);
	}
}

void stage2_bss_init(void)
{
	if (__bss_bf_end__ - __bss_bf_start__ > 0) {
		memset(__bss_bf_start__, 0, __bss_bf_end__ - __bss_bf_start__);
	}
}

void stage1_data_init(void)
{
	if (__vp_s1_data_end__ - __vp_s1_data_start__ > 0) {
		// restore stage1 data to ddr
		memcpy(__vp_s1_data_start__, __bkup_s1_data_start__, __vp_s1_data_end__ - __vp_s1_data_start__);

		DCache_CleanInvalidate((u32)__vp_s1_data_start__, (u32)__vp_s1_data_end__ - (u32)__vp_s1_data_start__);
	}
}

void stage2_data_init(void)
{
	if (__vp_data_end__ - __vp_data_start__ > 0) {
		// restore stage1 data to ddr
		memcpy(__vp_data_start__, __bkup_data_start__, __vp_data_end__ - __vp_data_start__);

		DCache_CleanInvalidate((u32)__vp_data_start__, (u32)__vp_data_end__ - (u32)__vp_data_start__);
	}
}
