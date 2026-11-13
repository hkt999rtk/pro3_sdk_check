#include "ameba_soc.h"

/*
AON_32KCLK_SDM32K  (32768)
AON_32KCLK_HSSDM  (32000)
AON_32KCLK_OSC131K_DIV4
*/


static void aon_32k_clk_init(void)
{
#if defined(CONFIG_BT) && CONFIG_BT
	/* HS_SDM */
	RCC_PeriphClockCmd(APBPeriph_AON_HSSDM, APBPeriph_AONHSSDM_CLOCK, ENABLE);
	HSSDM_Enable();
#endif
}


__attribute__((constructor))
void clk_init(void)
{
	aon_32k_clk_init();
}
