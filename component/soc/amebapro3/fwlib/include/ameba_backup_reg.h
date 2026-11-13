/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _AMEBA_BACKUP_REG_H_
#define _AMEBA_BACKUP_REG_H_

/** @addtogroup AmebaD_Platform
  * @{
  */

/** @defgroup BKUP_REG
  * @brief BKUP_REG driver modules
  * @{
  */

/** @addtogroup BKUP_REG
  * @verbatim
  *****************************************************************************************
  * Introduction
  *****************************************************************************************
  * backup register size:
  *		-16bytes (4 dwords)
  *
  * usage:
  *		- user can use this registers to save some data before reset happens
  *
  * backup register can not be reset by following functions:
  *		- cpu reset
  *		- system reset
  *		- soc sleep mode
  *
  * backup register will be reset by following functions:
  *		- soc deep sleep mode
  *		- soc power down reset
  *		- soc power off
  *
  * system defined bits (other bits are reserved for user):
  *		- dword0[9]: this is SW set bit before reboot, for uart download
  *		- dword0[10]: this is SW set bit before reboot, for uart download debug
  *		- dword0[27:24]: this is bootcnt for roll-back
  *		- dword0[30]: boot version, 0 for OTA1, 1 for OTA2
  *		- dword0[31]: set when bootcnt > 3, means default version cannot work
  *
  *****************************************************************************************
  * how to use
  *****************************************************************************************
  *		BKUP_Write: write a dword backup register
  *		BKUP_Read: read a dword backup register
  *		BKUP_Set: set 1 to some bits of backup register
  *		BKUP_Clear: set 0 to some bits of backup register
  *****************************************************************************************
  * @endverbatim
  */

/**************************************************************************//**
 * @defgroup AMEBAD_BACKUP_REG
 * @{
 * @brief AMEBAD_BACKUP_REG Register Declaration
 * @note 16 Bytes total
 *****************************************************************************/
typedef struct {
	__IO uint32_t DWORD[6]; /* 0xE0 */
} BACKUP_REG_TypeDef;

/** @} */

/* Exported constants --------------------------------------------------------*/

/** @defgroup BKUP_REG_Exported_Constants BKUP_REG Exported Constants
  * @{
  */

/** @defgroup BKUP_REG_Idx_definitions
  * @{
  */
#define BKUP_REG0				((u32)0x00000000)	/*!< reserved for system */
#define BKUP_REG1				((u32)0x00000001)	/*!< reserved for system */
#define BKUP_REG2				((u32)0x00000002)	/*!< reserved for system */
#define BKUP_REG3				((u32)0x00000003)	/*!< reserved for system */
#define BKUP_REG4				((u32)0x00000004)	/*!< reserved for usb, rom debug only */
#define BKUP_REG5				((u32)0x00000005)	/*!< reserved for system */
#define IS_BKUP_REG(IDX)		( (((u32)IDX) <= BKUP_REG5) )

#define BKUP_FLASHID			BKUP_REG1			/* flash id */
#define BKUP_BOOTADDR			BKUP_REG2			/* cold boot only */
#define BKUP_BOOTPROG			BKUP_REG3			/* rom debug only */

#define BKUP_MAILBOX			BKUP_REG5			/* share between bootloader(image1) and application(image2) */

#define BKUP2_REG0				((u32)0x00000000)	/*!< User */
#define BKUP2_REG1				((u32)0x00000001)	/*!< User */
#define BKUP2_REG2				((u32)0x00000002)	/*!< reserved for system */
#define BKUP2_REG3				((u32)0x00000003)	/*!< reserved for system */
#define BKUP2_REG4				((u32)0x00000004)	/*!< reserved for system */
#define BKUP2_REG5				((u32)0x00000005)	/*!< reserved for system */
#define IS_BKUP2_REG(IDX)		( (((u32)IDX) <= BKUP2_REG5) )

#define BKUPR_REG0				AON_BASE->AON_WDT_TIMER		/*!< reserved for system */
#define BKUPR_REG1				AON_BASE->AP_WDT_TIMER		/*!< reserved for system */
#define BKUPR_REG2				AON_NS_BASE->FW_WDT_TIMER	/*!< reserved for system */
#define BKUPR_REG3				AON_NS_BASE->MP_WDT_TIMER	/*!< reserved for system */

#define BKUPR_REGx_GET(reg)			((u32)((reg) >> 1) & 0x1F)
#define BKUPR_REGx_SET(reg, val)	do{reg &= ~(0x1F << 1); reg |= ((val & 0x1F) << 1);}while(0)

#define BKUPR_Write(reg, val) 		do{BKUPR_REGx_SET(reg, val);}while(0)
#define BKUPR_Read(reg)				BKUPR_REGx_GET(reg)
#define BKUPR_Set(reg, bitmsk)		do{u32 val = BKUPR_Read(reg); val |= ((bitmsk) & 0x1F); BKUPR_Write(reg, val);}while(0)
#define BKUPR_Clear(reg, bitmsk)	do{u32 val = BKUPR_Read(reg); val &= ~((bitmsk) & 0x1F); BKUPR_Write(reg, val);}while(0)

/**
  * @}
  */
/**
  * @}
  */


/** @defgroup BKUP_REG_Exported_Functions BKUP_REG Exported Functions
  * @{
  */
_LONG_CALL_ void BKUP_Write(u32 DwordIdx, u32 WriteVal);
_LONG_CALL_ u32 BKUP_Read(u32 DwordIdx);
_LONG_CALL_ void BKUP_Set(u32 DwordIdx, u32 BitMask);
_LONG_CALL_ void BKUP_Clear(u32 DwordIdx, u32 BitMask);
_LONG_CALL_ void BKUP_ClearAll(void);

_LONG_CALL_ void BKUP2_Write(u32 DwordIdx, u32 WriteVal);
_LONG_CALL_ u32 BKUP2_Read(u32 DwordIdx);
/**
  * @}
  */

/* Registers Definitions --------------------------------------------------------*/
/**************************************************************************//**
 * @defgroup BKUP_REG_Register_Definitions BKUP_REG Register Definitions
 * @{
 *****************************************************************************/

/**************************************************************************//**
* @defgroup BKUP_REG_WDORD0 REG_LP_BOOT_REASON0
* @{
*****************************************************************************/
#define BKUP_BIT_INF_SPIC_CLKSRC		BIT(31)   // SPIC clock source select, 0: xtal 1: sys1pll
#define BKUP_BIT_INF_SEC_CLKSRC			BIT(30)   // SECURE IP clock source select, 0: xtal 1: sys1pll
#define BKUP_BIT_INF_NP_CLKSRC			BIT(29)   // KM4 NP clock source select, 0: xtal 1: sys1pll
#define BKUP_BIT_INF_SYS1PLL_EN			BIT(28)   // SYS1PLL enable, 0: off 1: enable
#define BKUP_BIT_INF_SYS1PLL_PMC_SW		BIT(27)   // SYS1PLL enable by, 0: pmc 1: sw
#define BKUP_BIT_INF_SYS2PLL_EN			BIT(26)   // SYS2PLL enable, 0: off 1: enable

#define BKUP_BIT_IBOOT_NOR				BIT(21)   // IBOOT NOR, 0: NAND, 1:NOR
#define BKUP_BIT_IBOOT_SEL				BIT(20)   // IBOOT set 0: not iboot cmd, 1: iboot cmd

#define BKUP_BIT_BOOTSRC_AUTO			BIT(19)   // use NOR/NAND auto-selection code
#define BKUP_BIT_BOOTSRC_NOR			BIT(18)   // use NOR or NAND, 0: NAND, 1: NOR


#define BKUP_BIT_FW_OTA_INDEX			BIT(17)   // APP OTA slot: 0 = OTA1, 1 = OTA2

#define BKUP_BIT_PG_MODE				BIT(16)   // TRP PG mode
#define BKUP_BIT_TEST_BOOTSRC			BIT(15)   // TRAP Test mode : boot source select
#define BKUP_BIT_TEST_ICFG0				BIT(14)   // TRAP Test mode : ICFG bit0
#define BKUP_BIT_TEST_ICFG1				BIT(13)   // TRAP Test mode : ICFG bit1
#define BKUP_BIT_TEST_ICFG2				BIT(12)   // TRAP Test mode : ICFG bit2

#define BKUP_BIT_RAM_PATCH				BIT(11)   /*!<  this is SW set bit after fw authed via xmodem, for uart download */
#define BKUP_BIT_UARTBURN_DEBUG			BIT(10)   /*!<  this is SW set bit before reboot, for uart download debug */

/* REDIRECT to BKUPR */
#define BKUP_MASK_UARTBURN_BOOT				((u32)0x0000007F)
#define BKUP_UARTBURN_TO_PATTERN			((u32)0x35)
#define BKUP_BIT_UARTBURN_BOOT				((u32)0x5A)

/* BKUPR_REG3 */
#define BKUPR_MASK_UARTBURN_BOOT			((u32)0x000000F)
#define BKUPR_UARTBURN_TO_PATTERN			((u32)0x05)		// = BKUP_UARTBURN_TO_PATTERN & 0xF (BKUPR_MASK_UARTBURN_BOOT)
#define BKUPR_BIT_UARTBURN_BOOT				((u32)0x0A)		// = BKUP_BIT_UARTBURN_BOOT & 0xF (BKUPR_MASK_UARTBURN_BOOT)
#define BKUPR_BIT_UARTBURN_DEBUG			BIT(4)

/* BKUPR_REG2 */
#define BKUPR_BIT_BOOTSRC_NOR				BIT(0)

/** @} */
/** @} */



/**
  * @}
  */

/**
  * @}
  */

#endif //_RTL8710B_BACKUP_REG_H_
