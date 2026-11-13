/*
* Copyright (c) 2024 Realtek Semiconductor Corp.
*
* SPDX-License-Identifier: Apache-2.0
*/

#include "ameba_soc.h"

#ifdef CONFIG_TRUSTZONE

extern u8 __np_ssram_start__[];
extern u8 __shm_ssram_end__[];

extern u8 __np_ssram_stk_ns_top__[];
extern u8 __np_ssram_stk_ns_lim__[];

extern u8 __np_psram_nsc_start__[];
extern u8 __np_psram_nsc_end__[];

extern u8 __np_psram_start__[];
extern u8 __np_psram_end__[];

extern u8 __psram_start__[];
extern u8 __psram_end__[];


const SAU_CFG_TypeDef sau_config[SAU_ENTRY_NUM] = {
//  Start							End								NSC
	{(u32)__np_psram_nsc_start__,	(u32)__np_psram_nsc_end__ - 1,	1},	/* entry0: PSRAM NSC */
	{(u32)__np_psram_start__,	(u32)__np_psram_end__ - 1,		0},	/* entry1: PSRAM NS */
	{(u32)__np_ssram_start__,	(u32)__shm_ssram_end__,			0},	/* entry2: NP SRAM NS include NC/RET/IPC/SHM */
	{0x28000000,					0x28002000 - 1,					0},	/* entry3: NP SRAM MSP */
	{0x29000000,					0x291FFFFF,						0},	/* entry4: SPIC Device */
	{0x40000000,					0x4FFFFFFF,						0},	/* entry5: NS Wifi Domain Device */
	{0x60000000,					0x6FFFFFFF,						0},	/* entry6: NS Core Domain Device */
	{0xFFFFFFFF,					0xFFFFFFFF,						0},	/* entry7: TODO */
};

/* TODO: AP BL1 should be Secure, it is in Non-Secure at this implement*/
/* PSRAM all NS except NP PSRAM S region, 0x20000000 ~ 0x207F_FFFF */
const TZ_CFG_TypeDef mpc_psram_config[MPC_ENTRY_NUM] =		/* Security configuration for PSRAM */
{
//  Start							End								CTRL
//#if !defined(CONFIG_TRUSTZONE_EN) || (CONFIG_TRUSTZONE_EN==0)
	{(u32)__psram_start__,	(u32)__psram_end__,				MPC_NS},	/* entry0: Non-Secure All PSRAM */
	{0xFFFFFFFF,					0xFFFFFFFF, 					MPC_NS},	/* entry1: TODO */
//#else
//	{(u32)__np_psram_nsc_start__,	(u32)__np_psram_nsc_end__ -1, 	MPC_NS},	/* entry0: Non-Secure Callable for NP */
//	{(u32)__np_psram_start__,		(u32)__psram_end__,		        MPC_NS},	/* entry1: Non-Secure after PSRAM_S */
//#endif
	{0xFFFFFFFF,					0xFFFFFFFF,						MPC_NS},	/* entry2: TODO */
	{0xFFFFFFFF,					0xFFFFFFFF,						MPC_NS},	/* entry3: TODO */
	{0xFFFFFFFF,					0xFFFFFFFF,						MPC_NS},	/* entry4: TODO */
	{0xFFFFFFFF,					0xFFFFFFFF,						MPC_NS},	/* entry5: TODO */
	{0xFFFFFFFF,					0xFFFFFFFF,						MPC_NS},	/* entry6: TODO */
	{0xFFFFFFFF,					0xFFFFFFFF,						MPC_NS},	/* entry7: TODO */
};

extern u8 __ssram_start__[];
extern u8 __bt_shram_end__[];
extern u8 __np_ssram_mbx_end__[];
extern u8 __np_ssram_start__[];

//0x2800_0000 ~ 0x2807_FFFF 512K sram0
const TZ_CFG_TypeDef mpc_sram_config[MPC_ENTRY_NUM] =						/* Security configuration for sram */
{
//  Start						End							CTRL
#if !defined(CONFIG_TRUSTZONE_EN) || (CONFIG_TRUSTZONE_EN==0)
	{(u32)__ssram_start__,	(u32)__bt_shram_end__,		MPC_NS},	/* entry0: Non-Secure ALL SRAM and BT SHRAM */
	{0xFFFFFFFF,				0xFFFFFFFF,					MPC_NS},	/* entry2: TODO */
#else
	{(u32)__ssram_start__,	(u32)__np_ssram_mbx_end__,	MPC_NS},	/* entry0: Non-Secure NP MSP, ROM BSS, MAILBOX */
	{(u32)__np_ssram_start__,	(u32)__bt_shram_end__,		MPC_NS},	/* entry2: Non-Secure NP SRAM/NC/RET/IPC/SHM, FP SRAM/MSP, BT SHRAM */
#endif
	{0xFFFFFFFF,				0xFFFFFFFF, 				MPC_NS},	/* entry3: TODO */
	{0xFFFFFFFF,				0xFFFFFFFF,					MPC_NS},	/* entry4: TODO */
	{0xFFFFFFFF,				0xFFFFFFFF,					MPC_NS},	/* entry5: TODO */
	{0xFFFFFFFF,				0xFFFFFFFF,					MPC_NS},	/* entry6: TODO */
	{0xFFFFFFFF,				0xFFFFFFFF,					MPC_NS},	/* entry7: TODO */
};

extern u8 __vsram_start__[];
extern u8 __vsram_end__[];

const TZ_CFG_TypeDef mpc_vsram_config[MPC_ENTRY_NUM] =			/* Security configuration for VSRAM */
{
//  Start					End						CTRL
	{(u32)__vsram_start__,	(u32)__vsram_end__,		MPC_NS},	/* entry0: Non-Secure All VSRAM */
	{0xFFFFFFFF,			0xFFFFFFFF, 			MPC_NS},	/* entry1: TODO */
	{0xFFFFFFFF,			0xFFFFFFFF,				MPC_NS},	/* entry2: TODO */
	{0xFFFFFFFF,			0xFFFFFFFF,				MPC_NS},	/* entry3: TODO */
	{0xFFFFFFFF,			0xFFFFFFFF,				MPC_NS},	/* entry4: TODO */
	{0xFFFFFFFF,			0xFFFFFFFF,				MPC_NS},	/* entry5: TODO */
	{0xFFFFFFFF,			0xFFFFFFFF,				MPC_NS},	/* entry6: TODO */
	{0xFFFFFFFF,			0xFFFFFFFF,				MPC_NS},	/* entry7: TODO */
};

extern u8 __ap_ddr_ro_start__[];
extern u8 __ddr_end__[];

const TZ_CFG_TypeDef mpc_ddram_config[MPC_ENTRY_NUM] =			/* Security configuration for VSRAM */
{
//  Start					End						CTRL
	{(u32)__ap_ddr_ro_start__,	(u32)__ddr_end__,	MPC_NS},	/* entry0: Non-Secure All DDR after ap_ddr_ro_start */
	{0xFFFFFFFF,			0xFFFFFFFF, 			MPC_NS},	/* entry1: TODO */
	{0xFFFFFFFF,			0xFFFFFFFF,				MPC_NS},	/* entry2: TODO */
	{0xFFFFFFFF,			0xFFFFFFFF,				MPC_NS},	/* entry3: TODO */
	{0xFFFFFFFF,			0xFFFFFFFF,				MPC_NS},	/* entry4: TODO */
	{0xFFFFFFFF,			0xFFFFFFFF,				MPC_NS},	/* entry5: TODO */
	{0xFFFFFFFF,			0xFFFFFFFF,				MPC_NS},	/* entry6: TODO */
	{0xFFFFFFFF,			0xFFFFFFFF,				MPC_NS},	/* entry7: TODO */
};
#endif

