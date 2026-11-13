/******************************************************************************
*
* Copyright(c) 2007 - 2025 Realtek Corporation. All rights reserved.
*
******************************************************************************/

#include "ameba_soc.h"
#include "hal_nn.h"

/* Software state for this core; callers must serialize access to the NPU. */
static int nn_hw_inited = 0;

static void NN_FastPclkInit(void)
{
	/* check if fast_pclk is already enabled */
	if (CORE_S_BASE->SYS_1_FUNC_CTRL & CORE_S_BIT_SYS_FEN_S6_8) {
		return;
	}

	/* fast pclk setting (250M) for AINR, Encoder, NN */
	RCC_PeriphClockDividerSet(SYS_FAST_PCLK, 2, 1);
	RCC_PeriphClockSourceSet(SYS_FAST_PCLK, SYS2_PLL);
	RCC_PeriphClockCmd(APBPeriph_PLFM_CORE_FAST_CLK, APBPeriph_CBUS_FAST_CLOCK, ENABLE);
}

/**
*  @brief Disable the NPU clocks and functions, then clear the initialization state.
*  @note The caller must stop NPU activity and release driver resources first.
*        The shared fast PCLK remains enabled for AINR and the encoder.
*
*  @returns void
*/
void NN_DeInit(void)
{
	/* Always disable hardware, even if another core initialized it. */
	RCC_PeriphClockCmd(APBPeriph_PLFM_CORE_NN_ACLK, APBPeriph_CBUS_NN_CLOCK, DISABLE);
	RCC_PeriphClockCmd(APBPeriph_NN, APBPeriph_NN_CLOCK, DISABLE);
	nn_hw_inited = 0;
}

/**
*  @brief Initialize the NPU clocks and functions once for this core.
*  @note Call NN_DeInit() before reinitializing after hardware state is lost.
*
*  @returns void
*/
void NN_Init(void)
{
	if (nn_hw_inited) {
		return;
	}

	NN_FastPclkInit();

	/* Reset the NPU to prevent issues caused by residual states after VP usage */
	NN_DeInit();
	DelayUs(500);

	/* NN ACLK 300MHz */
	RCC_PeriphClockDividerSet(SYS_NN_ACLK, 2, 1);
	RCC_PeriphClockSourceSet(SYS_NN_ACLK, SYS1_PLL);
	RCC_PeriphClockCmd(APBPeriph_PLFM_CORE_NN_ACLK, APBPeriph_CBUS_NN_CLOCK, ENABLE);

	/* NN SCLK 600MHz */
	RCC_PeriphClockDividerSet(SYS_NN_SCLK, 1, 1);
	RCC_PeriphClockSourceSet(SYS_NN_SCLK, SYS1_PLL);
	RCC_PeriphClockCmd(APBPeriph_NN, APBPeriph_NN_CLOCK, ENABLE);

	nn_hw_inited = 1;
}

/**
*  @brief Return the NPU initialization state recorded by this core's HAL.
*  @note This does not read hardware or report driver/inference readiness.
*        Changes made by another core or by power management are not tracked.
*
*  @returns 1 after NN_Init() completes, or 0 initially and after NN_DeInit().
*/
int NN_IsInited(void)
{
	return nn_hw_inited;
}
