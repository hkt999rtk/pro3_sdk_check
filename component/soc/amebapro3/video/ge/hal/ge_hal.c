#include "ameba_soc.h"


int hal_ge_init(void)
{

	uint32_t reg_value;

	//reg_value = HAL_READ32(0x60250000, 0x174) ;    //REG_SYS_FAST_PCLK_CTRL
	//HAL_WRITE32(0x60250000, 0x174, reg_value | BIT(19)); //GE_SCLK_EN : default 0x1

	//reg_value = HAL_READ32(0x60250000, 0x1A8) ;    //REG_SYS_0_CLK_CTRL
	//HAL_WRITE32(0x60250000, 0x1A8, reg_value | BIT(22)); //SYS_CBUS_GE_CLKEN
	// move to video sub top control
	//CC_PeriphClockCmd(APBPeriph_NULL, APBPeriph_CBUS_GE_CLOCK, ENABLE);  // REG_SYS_0_CLK_CTRL(0x1A8)[22] - SYS_CBUS_GE_CLKEN


	//reg_value = HAL_READ32(0x60250000, 0x1B0) ;    //REG_SYS_2_CLK_CTRL
	//HAL_WRITE32(0x60250000, 0x1B0, reg_value | BIT(27)); //SYS_GE_SCLKEN
	// RCC_PeriphClockCmd(APBPeriph_NULL, APBPeriph_GE_CLOCK, ENABLE); 	//REG_SYS_2_CLK_CTRL(0x1B0)[27]: 'SYS_GE_SCLKEN



	//reg_value = HAL_READ32(0x60250000, 0x1B8) ;   //REG_SYS_1_FUNC_CTRL
	//HAL_WRITE32(0x60250000, 0x1B8, reg_value | BIT(17)); //SYS_FEN_S6_6: ge_clk domain reset
	// move to video sub top control
	//RCC_PeriphClockCmd(APBPeriph_PLFM_CORE_GE_CLK, APBPeriph_CLOCK_NULL, ENABLE);  // REG_SYS_1_FUNC_CTRL[0x1B8:17] - Core Platform function enable (ge_clk domain reset)


	// RCC_PeriphClockCmd(APBPeriph_GE, APBPeriph_CLOCK_NULL, ENABLE);  //REG_SYS_2_FUNC_CTRL(0x1BC)[31] - Graphic Engine function enable


	return 0;



}
