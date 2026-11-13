/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _AMEBA_CLK_H_
#define _AMEBA_CLK_H_

/** @addtogroup AmebaD_Platform
  * @{
  */

/** @defgroup CLOCK
  * @brief CLOCK driver modules
  * @{
  */

/** @addtogroup CLOCK
  * @verbatim
  *****************************************************************************************
  * SDM32K
  *****************************************************************************************
  *	-RTC clock in
  *	-TIM0-TIM8 clock in
  *	-WIFI 32K clock in

  *****************************************************************************************
  * AP CPU clock
  *****************************************************************************************
  *	-CLK_AP_200M: 200MHz
  *	-CLK_AP_100M: 100MHz
  *	-CLK_AP_50M: 50MHz
  *	-CLK_AP_25M: 25MHz
  *	-CLK_AP_XTAL: XTAL
  *
  *****************************************************************************************
  * XTAL clock
  *****************************************************************************************

  *	-40000000

  *****************************************************************************************
  * EXT32K
  *****************************************************************************************
  *	-External 32K: 32K clock from external 32k source
  *	-Internal 32K: 32K clock from internal 32K source: NCO32K
  *
  *****************************************************************************************
  * @endverbatim
  */

/* Exported constants --------------------------------------------------------*/

/** @defgroup CLK_Exported_Constants CLK Exported Constants
  * @{
  */
#define MHZ_TICK_CNT 		(1000000)

/** @defgroup CPU_CLK_Source definitions
  * @{
  */
#define CLK_PERI_PLL	3
#define CLK_SYS2_PLL	2
#define CLK_SYS1_PLL	1
#define CLK_XTAL		0
/**
  * @}
  */


/** @defgroup XTAL_CLK_definitions
* @{
*/
#define XTAL_30M		((u32)30000000)	/*This is FPGA default value*/
#define XTAL_40M		((u32)40000000)	/*This is ASIC default value*/
/**
  * @}
  */

/** @defgroup PLL_CLK_definitions
  * @{
  */
#define PLL_960M		((u32)960000000)	/*This is minimum value*/
/**
  * @}
  */

/** @defgroup Soc_CLK_LIMIT_definitions
  * @{
  */
#define CLKDIV(x)				(x)

#define SRAM_CLK_LIMIT			(200 * MHZ_TICK_CNT)
#define SPIC_CS_SETUP_TIME_LMT	(200 * MHZ_TICK_CNT) //CS# Active Setup Time relative to CLK(Some Flash tSLCH min is 5ns)

/**
  * @}
  */

/** @defgroup CAL_CLK_SEL_definitions
  * @{
  */
#define OSC4M_CAL_CLK		  0
#define AON128K_CAL_CLK		1
#define AON32K_CAL_CLK		2
/**
  * @}
  */


/** @defgroup CPU_CLK_Exported_Functions CPU_CLK Exported Functions
  * @{
  */
_LONG_CALL_ u32 CPU_ClkGet(void);
_LONG_CALL_ u32 CA32_AP_ClkGet(void);


/**
  * @}
  */

/** @defgroup Peri_CLK_Exported_Functions Peri_CLK Exported Functions
  * @{
  */
_LONG_CALL_ u32 PERI_ClkGet(u32 peri_id);

/**
  * @}
  */

/** @defgroup XTAL_CLK_Exported_Functions XTAL_CLK Exported Functions
  * @{
  */
_LONG_CALL_ u32 XTAL_ClkGet(void);
//_LONG_CALL_ u32 PLL_ClkGet(u8 pll_src);
/**
  * @}
  */


/**
  * @}defgroup PLL_Exported_Functions PLL Exported Functions
  */
//_LONG_CALL_ void PLL_Cmd(u32 status);
//_LONG_CALL_ void PLL_BG_Cmd(u32 status);
//_LONG_CALL_ bool PLL_State(void);
//_LONG_CALL_ void PLL_ClkSet(u32 PllClk);

/**
  * @}
  */


/** @defgroup SDM32K_CLK_Exported_Functions SDM32K_CLK Exported Functions
  * @{
  */
_LONG_CALL_ void SDM32K_Enable(void);
_LONG_CALL_ void SDM32K_TimerCalEnable(u32 newstatus);
/**
  * @}
  */

/** @defgroup HSSDM_Exported_Functions HSSDM_CLK Exported Functions
* @{
*/
_LONG_CALL_ void HSSDM_Enable(void);
/**
  * @}
  */


/* Registers Definitions --------------------------------------------------------*/


/* Other definations --------------------------------------------------------*/
#define CLKID_PSRAM         0
#define CLKID_TRNG          1
#define CLKID_SECIP         2
#define CLKID_SLOW_SCLK     3
#define CLKID_SLOW_PCLK     4
#define CLKID_AUDIO_SPORT   5
#define CLKID_AUDIO_CODEC   6
#define CLKID_PON_TIMER0    7

// void OSC4M_Init(void);
void OSC4M_R_Set(u32 setbit, u32 clearbit);
void OSC4M_VCM_Set(u32 value);
u32 OSC4M_Calibration(u32 ppm_limit);
u32 OSC131K_Calibration(u32 ppm_limit);
void OSC131_R_Set(u32 setbit, u32 clearbit);
u32 OSC_CalResult_Get(u8 cal_clk);

#endif //_AMEBA_CLK_H_
