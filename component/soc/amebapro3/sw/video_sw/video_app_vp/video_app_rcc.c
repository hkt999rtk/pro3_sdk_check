#include <stdio.h>
#include "stdint.h"
#include "ameba_soc.h"

#ifdef CONFIG_VMODULE_ADDREMAP
#include "hal_enc.h"
#include "hal_addremap.h"
#endif

void video_app_common_rcc_init(void)
{
	/* platform clock (250M) for video SRAM */
	RCC_PeriphClockDividerSet(SYS_PLF_CLK, 2, 1);
	RCC_PeriphClockSourceSet(SYS_PLF_CLK, SYS2_PLL);
	RCC_PeriphClockCmd(APBPeriph_PLFM_CORE_SYS_CLK, APBPeriph_CLOCK_NULL, ENABLE);

	/* video SRAM clock, reset */
	RCC_PeriphClockCmd(APBPeriph_VIDEO_SRAM, APBPeriph_VID_SRAM_CLOCK, ENABLE);

	/* fast pclk setting (250M) for AINR, Encoder, NN */
	RCC_PeriphClockDividerSet(SYS_FAST_PCLK, 2, 1);
	RCC_PeriphClockSourceSet(SYS_FAST_PCLK, SYS2_PLL);
	RCC_PeriphClockCmd(APBPeriph_PLFM_CORE_FAST_CLK, APBPeriph_CBUS_FAST_CLOCK, ENABLE);

	/* GE core clock (300M) shared with TS */
	RCC_PeriphClockDividerSet(SYS_GE_SCLK, 2, 1);
	RCC_PeriphClockSourceSet(SYS_GE_SCLK, SYS1_PLL);
	RCC_PeriphClockCmd(APBPeriph_PLFM_CORE_GE_CLK, APBPeriph_CBUS_GE_CLOCK, ENABLE);

#ifdef CONFIG_VMODULE_ADDREMAP
	ENC_Init();
	REMAP_HWInit();
#endif
}

void video_app_common_rcc_deinit(void)
{
	/* platform clock (250M) for video SRAM */
	RCC_PeriphClockCmd(APBPeriph_PLFM_CORE_SYS_CLK, APBPeriph_CLOCK_NULL, DISABLE);

	/* video SRAM clock, reset */
	RCC_PeriphClockCmd(APBPeriph_VIDEO_SRAM, APBPeriph_VID_SRAM_CLOCK, DISABLE);

	/* fast pclk setting (250M) for AINR, Encoder, NN */
	RCC_PeriphClockCmd(APBPeriph_PLFM_CORE_FAST_CLK, APBPeriph_CBUS_FAST_CLOCK, DISABLE);

	/* GE core clock (300M) shared with TS */
	RCC_PeriphClockCmd(APBPeriph_PLFM_CORE_GE_CLK, APBPeriph_CBUS_GE_CLOCK, DISABLE);
}
