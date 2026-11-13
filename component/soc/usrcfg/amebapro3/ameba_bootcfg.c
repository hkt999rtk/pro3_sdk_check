/*
* Copyright (c) 2024 Realtek Semiconductor Corp.
*
* SPDX-License-Identifier: Apache-2.0
*/

#include "ameba_soc.h"

/**
* @brif  TRUSTZONE enable or disable
* 	FALSE: disable
*	TRUE: enable
*/
#ifdef CONFIG_TRUSTZONE
const u8 Boot_Tzcfg_En = TRUE;
#else
const u8 Boot_Tzcfg_En = FALSE;
#endif

/*
* @brif	MMU Configuration.
*	There are 8 MMU entries totally. Entry 0 & Entry 1 are already used by OTA, Entry 2~7 can be used by Users.
*/
MMU_ConfDef Flash_MMU_Config[] = {
	/*VAddrStart,	VAddrEnd,			PAddrStart,		PAddrEnd*/
	{0xFFFFFFFF,	0xFFFFFFFF,			0xFFFFFFFF,		0xFFFFFFFF},	//Entry 2
	{0xFFFFFFFF,	0xFFFFFFFF,			0xFFFFFFFF,		0xFFFFFFFF},	//Entry 3
	{0xFFFFFFFF,	0xFFFFFFFF,			0xFFFFFFFF,		0xFFFFFFFF},	//Entry 4
	{0xFFFFFFFF,	0xFFFFFFFF,			0xFFFFFFFF,		0xFFFFFFFF},	//Entry 5
	{0xFFFFFFFF,	0xFFFFFFFF,			0xFFFFFFFF,		0xFFFFFFFF},	//Entry 6
	{0xFFFFFFFF,	0xFFFFFFFF,			0xFFFFFFFF,		0xFFFFFFFF},	//Entry 7
	{0xFFFFFFFF,	0xFFFFFFFF,			0xFFFFFFFF,		0xFFFFFFFF},
};

/* Certificate is suggested to use SEC_PKKEY_PK1_0 */
u32 Cert_PKHash_OTP_ADDR = SEC_PKKEY_PK1_0;

u8 Boot_From_APM_PSRAM = 0;

// PLL is fixed to 960M, Vol_Type is fixed to 0.9V
// CPU_CKD range is [3, 5], PSRAMC_CKD range is [3, 4]
SocClk_Info_TypeDef SocClk_Info[] = {
	/* PLL_CLK,		Vol_Type,		CPU_CKD,	PSRAMC_CKD*/
#ifdef FEATURE_TODO
	{PLL_960M,		CORE_VOL_0P9,	CLKDIV(5),	CLKDIV(4)},
#endif
};

/**
* @brif  Loguart AGG enable or disable
* 	FALSE: disable
*	TRUE: enable
*/
u8 Boot_Agg_En = FALSE;
