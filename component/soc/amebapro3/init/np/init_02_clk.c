#include "ameba_soc.h"

/*
AON_32KCLK_SDM32K  (32768)
AON_32KCLK_HSSDM  (32000)
AON_32KCLK_OSC131K_DIV4
*/
#define AON_32K_CLK_SRC     AON_32KCLK_SDM32K


static void aon_32k_clk_init(void)
{
#if (AON_32K_CLK_SRC == AON_32KCLK_SDM32K)
	/* SDM_32K (LS_SDM ) */
	RCC_PeriphClockCmd(APBPeriph_AON_LSSDM, APBPeriph_AONLSSDM_CLOCK, ENABLE);
	SDM32K_Enable();

#if defined(CONFIG_BT) && CONFIG_BT
	/* HS_SDM */
	RCC_PeriphClockCmd(APBPeriph_AON_HSSDM, APBPeriph_AONHSSDM_CLOCK, ENABLE);
	HSSDM_Enable();
#endif

	RCC_PeriphClockSourceSet(AON_32KCLK, SDM32K);
	RTK_LOGI("INIT", "32K Init [SDM32K]\n");

#elif (AON_32K_CLK_SRC == AON_32KCLK_HSSDM)
	/* HS_SDM */
	RCC_PeriphClockCmd(APBPeriph_AON_HSSDM, APBPeriph_AONHSSDM_CLOCK, ENABLE);
	HSSDM_Enable();

	RCC_PeriphClockSourceSet(AON_32KCLK, HSSDM);
	RTK_LOGI("INIT", "32K Init [HSSDM]\n");

#elif (AON_32K_CLK_SRC == AON_32KCLK_OSC131K_DIV4)
	/* OSC131K_DIV4. HW default value, do nothing here */
	RTK_LOGI("INIT", "32K Init [OSC131K_DIV4]\n");

#endif
}


__attribute__((constructor))
void clk_init(void)
{
	aon_32k_clk_init();
}
