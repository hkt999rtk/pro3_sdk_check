#include "ameba_soc.h"

__attribute__((constructor))
void clock_init(void)
{
	/* In 4K ISP mode bring the USB2 PLL up early: besides the USB PHY, its 480MHz
	output is used as the clock source for other peripherals. */
#if USB2_PLL_SHARED_BY_4K
	if (USB2PLL_Init() != HAL_OK) {
		RTK_LOGE("VP_INIT", "USB2 PLL init fail \n");
	} else {
		RTK_LOGI("VP_INIT", "USB2 PLL init done \n");
	}
#endif
}
