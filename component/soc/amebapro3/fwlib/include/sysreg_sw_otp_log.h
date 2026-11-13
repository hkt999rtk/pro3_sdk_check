/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef SYSREG_SW_OTP_LOG_H
#define SYSREG_SW_OTP_LOG_H

/* Generated from: AmebaPro3_system_sw_register_file_20260909.xlsx (date: 2026-09-09) */

/* AUTO_GEN_START */

/* OTP_SW_LOG is a part of OTP_SW_LOG */


/** @addtogroup Ameba_Periph_Driver
* @{
*/
/** @defgroup OTP_SW_LOG
* @brief OTP_SW_LOG driver modules
* @{
*/


/** @defgroup REG_ROM_BOOT_CONFIG0
 * @brief
 * @{
 */
#define OTP_SW_LOG_BIT_ROM_SPIC_CLKSRC_SEL          ((u32)0x00000001 << 31)                  /*!< RW    0x0
                                                                                              * 0: low speed, xtal
                                                                                              * 1: high speed , sys1pll
                                                                                              */
#define OTP_SW_LOG_BIT_ROM_SEC_CLKSRC_SEL           ((u32)0x00000001 << 30)                  /*!< RW    0x0
                                                                                              * 0: low speed, xtal
                                                                                              * 1: high speed , sys1pll
                                                                                              */
#define OTP_SW_LOG_BIT_ROM_NP_CLKSRC_SEL            ((u32)0x00000001 << 29)                  /*!< RW    0x0
                                                                                              * 0: low speed, xtal
                                                                                              * 1: high speed , sys1pll
                                                                                              */
#define OTP_SW_LOG_MASK_RSIP_MODE                   ((u32)0x00000007 << 26)                  /*!< RW    0x0
                                                                                              * RSIP mode
                                                                                              * 3b0xx: use physical rsip mode
                                                                                              * 3b100: XTS (mix mode)
                                                                                              * 3b101: CTR
                                                                                              * 3b110: GCM
                                                                                              * 3b111: XTS (mix mode)
                                                                                              */
#define OTP_SW_LOG_RSIP_MODE(x)                     ((u32)(((x) & 0x00000007) << 26))
#define OTP_SW_LOG_GET_RSIP_MODE(x)                 ((u32)(((x >> 26) & 0x00000007)))
#define OTP_SW_LOG_BIT_OTP_CRC_EN                   ((u32)0x00000001 << 25)                  /*!< RW    0x0
                                                                                              * OTP CRC check enable
                                                                                              * 
                                                                                              * 0: disable
                                                                                              * 1: enable
                                                                                              */
#define OTP_SW_LOG_BIT_USB_DOWNLOAD_EN              ((u32)0x00000001 << 24)                  /*!< RW    0x0
                                                                                              * USB download enable
                                                                                              *  
                                                                                              * 0: Disable 
                                                                                              * 1: Enable
                                                                                              */
#define OTP_SW_LOG_BIT_BOOT_CNT_LMT_EN              ((u32)0x00000001 << 23)                  /*!< RW    0x0
                                                                                              * enable boot cnt limit
                                                                                              *  
                                                                                              * 0: disable 
                                                                                              * 1: enable 
                                                                                              */
#define OTP_SW_LOG_BIT_LOGIC_RDP_EN                 ((u32)0x00000001 << 22)                  /*!< RW    0x0
                                                                                              * RDP enable
                                                                                              *  
                                                                                              * 0: disable 
                                                                                              * 1: enable 
                                                                                              */
#define OTP_SW_LOG_BIT_LOGIC_RSIP_EN                ((u32)0x00000001 << 21)                  /*!< RW    0x0
                                                                                              * RSIP enable 
                                                                                              *  
                                                                                              * 0: disable  
                                                                                              * 1: enable 
                                                                                              */
#define OTP_SW_LOG_BIT_LOGIC_SECURE_BOOT_EN         ((u32)0x00000001 << 20)                  /*!< RW    0x0
                                                                                              * Secure boot enable 
                                                                                              *  
                                                                                              * 0: disable 
                                                                                              * 1: enable 
                                                                                              */
#define OTP_SW_LOG_BIT_LOW_BAUD_LOG_EN              ((u32)0x00000001 << 19)                  /*!< RW    0x0
                                                                                              * Loguart baud rate selection
                                                                                              *  
                                                                                              * 0: 1.5Mbps 
                                                                                              * 1: 115200bps 
                                                                                              */
#define OTP_SW_LOG_BIT_BOOT_LOG_DIS                 ((u32)0x00000001 << 18)                  /*!< RW    0x0
                                                                                              * Boot log disable
                                                                                              *  
                                                                                              * 0: enable 
                                                                                              * 1: disable 
                                                                                              */
#define OTP_SW_LOG_BIT_UART_DOWNLOAD_DIS            ((u32)0x00000001 << 17)                  /*!< RW    0x0
                                                                                              * Uart download disable
                                                                                              *  
                                                                                              * 0: enable 
                                                                                              * 1: disable 
                                                                                              */
#define OTP_SW_LOG_MASK_BOOT_WDT_SELECT             ((u32)0x00000003 << 15)                  /*!< RW    0x0
                                                                                              * ROM boot WDT enable
                                                                                              *  
                                                                                              * 0'b00: enable, 8s 
                                                                                              * 0'b01: enable, 30s 
                                                                                              * 0'b10: disable 
                                                                                              * 0'b11: enable, 8s 
                                                                                              */
#define OTP_SW_LOG_BOOT_WDT_SELECT(x)               ((u32)(((x) & 0x00000003) << 15))
#define OTP_SW_LOG_GET_BOOT_WDT_SELECT(x)           ((u32)(((x >> 15) & 0x00000003)))
#define OTP_SW_LOG_MASK_BIG_PAGE_NAND               ((u32)0x00000007 << 12)                  /*!< RW    0x0
                                                                                              * Nand Flash page info
                                                                                              *  
                                                                                              * bit 0 :  [0: auto select, 1: manual ]   , auto select from ONFI or  Page0
                                                                                              * bit 1:   [0: page size 2K,  1: page size  4K]  , byte per page
                                                                                              * bit 2:   [0: page num 64, 1: page num 128], page per block
                                                                                              * 
                                                                                              * bit1&2 are valid when bit0 == 1
                                                                                              * 3'b000, auto select mode
                                                                                              * 3'bxx0, not auto mode and not manuel mode, hard code 2K and 64 
                                                                                              */
#define OTP_SW_LOG_BIG_PAGE_NAND(x)                 ((u32)(((x) & 0x00000007) << 12))
#define OTP_SW_LOG_GET_BIG_PAGE_NAND(x)             ((u32)(((x >> 12) & 0x00000007)))
#define OTP_SW_LOG_MASK_BOOT_SELECT                 ((u32)0x00000003 << 10)                  /*!< RW    0x0
                                                                                              * Boot selection. Boot from Nor Flash, or Nand Flash, or try.
                                                                                              *  
                                                                                              * 0'b00: Boot option determined by power-on latch 
                                                                                              * 0'b01: Boot from Nor flash 
                                                                                              * 0'b10: Boot from Nand flash 
                                                                                              * 0'b11: Boot option determined by power-on latch
                                                                                              */
#define OTP_SW_LOG_BOOT_SELECT(x)                   ((u32)(((x) & 0x00000003) << 10))
#define OTP_SW_LOG_GET_BOOT_SELECT(x)               ((u32)(((x >> 10) & 0x00000003)))
#define OTP_SW_LOG_MASK_BOOT_CORE_POWER_DELAY       ((u32)0x00000003 << 8)                   /*!< RW    0x0
                                                                                              * CORE Power Init Delay option
                                                                                              *  
                                                                                              * 0: 100ms 
                                                                                              * 1: 200ms
                                                                                              * 2: 300ms
                                                                                              * 3: 400ms
                                                                                              */
#define OTP_SW_LOG_BOOT_CORE_POWER_DELAY(x)         ((u32)(((x) & 0x00000003) << 8))
#define OTP_SW_LOG_GET_BOOT_CORE_POWER_DELAY(x)     ((u32)(((x >> 8) & 0x00000003)))
#define OTP_SW_LOG_MASK_BOOT_LOADER_LIM_EXT         ((u32)0x00000003 << 6)                   /*!< RW    0x0
                                                                                              * extend boot loader sram space
                                                                                              * 
                                                                                              * 0: 128K + 0 (no extend)
                                                                                              * 1: 128K + 16K
                                                                                              * 2: 128K + 32K
                                                                                              * 3: 128K + 64K
                                                                                              */
#define OTP_SW_LOG_BOOT_LOADER_LIM_EXT(x)           ((u32)(((x) & 0x00000003) << 6))
#define OTP_SW_LOG_GET_BOOT_LOADER_LIM_EXT(x)       ((u32)(((x >> 6) & 0x00000003)))
#define OTP_SW_LOG_MASK_BOOT_FLASH_BAUD_SEL         ((u32)0x00000003 << 4)                   /*!< RW    0x0
                                                                                              * Boot Flash Baudrate .
                                                                                              * 
                                                                                              * 0:  1/2
                                                                                              * 1:  1/4 
                                                                                              * 2:  1/6 
                                                                                              * 3:  invalid
                                                                                              */
#define OTP_SW_LOG_BOOT_FLASH_BAUD_SEL(x)           ((u32)(((x) & 0x00000003) << 4))
#define OTP_SW_LOG_GET_BOOT_FLASH_BAUD_SEL(x)       ((u32)(((x >> 4) & 0x00000003)))
#define OTP_SW_LOG_BIT_LOGIC_SECURE_BOOT_PQC_EN     ((u32)0x00000001 << 3)                   /*!< RW    0x0
                                                                                              * Secure boot PQC enable 
                                                                                              *  
                                                                                              * 0: disable 
                                                                                              * 1: enable 
                                                                                              */
#define OTP_SW_LOG_BIT_SPIC_BOOT_SPEEDUP_EN         ((u32)0x00000001 << 2)                   /*!< RW    0x0
                                                                                              * SPIC boot speed up enable
                                                                                              *  
                                                                                              * 0: not speed up 
                                                                                              * 1: speed up
                                                                                              */
#define OTP_SW_LOG_BIT_SPIC_ADDR_4BYTE_EN           ((u32)0x00000001 << 1)                   /*!< RW    0x0
                                                                                              * SPIC address 4-byte enable 
                                                                                              *  
                                                                                              * 0: disable 
                                                                                              * 1: enable 
                                                                                              */
#define OTP_SW_LOG_BIT_FLASH_DEEP_SLEEP_DIS         ((u32)0x00000001 << 0)                   /*!< RW    0x0
                                                                                              * Flash deep-sleep api  disable
                                                                                              * 0: enable function
                                                                                              * 1: disable flash deep sleep control function
                                                                                              */
/** @} */


/** @defgroup REG_ROM_BOOT_CONFIG1
 * @brief
 * @{
 */
#define OTP_SW_LOG_MASK_RESV                        ((u32)0x07FFFFFF << 5)                   /*!< RW    0x0 None */
#define OTP_SW_LOG_RESV(x)                          ((u32)(((x) & 0x07FFFFFF) << 5))
#define OTP_SW_LOG_GET_RESV(x)                      ((u32)(((x >> 5) & 0x07FFFFFF)))
#define OTP_SW_LOG_BIT_FLASH_DEEPPD_FORCE_OFF       ((u32)0x00000001 << 4)                   /*!< RW    0x0
                                                                                              * Flash deep power down force off on ROM init
                                                                                              * 0: default, only off if not cold boot
                                                                                              * 1: force off
                                                                                              */
#define OTP_SW_LOG_MASK_BOOT_SYSPLL_DELAY_RATIO     ((u32)0x00000003 << 2)                   /*!< RW    0x0
                                                                                              * control delay ratio in SYS1/2PLL init. 
                                                                                              * 
                                                                                              * 0: x1
                                                                                              * 1: x2
                                                                                              * 2: x4
                                                                                              */
#define OTP_SW_LOG_BOOT_SYSPLL_DELAY_RATIO(x)       ((u32)(((x) & 0x00000003) << 2))
#define OTP_SW_LOG_GET_BOOT_SYSPLL_DELAY_RATIO(x)   ((u32)(((x >> 2) & 0x00000003)))
#define OTP_SW_LOG_MASK_BOOT_CPUPLL_DELAY_RATIO     ((u32)0x00000003 << 0)                   /*!< RW    0x0
                                                                                              * control delay ratio in CPUPLL init. 
                                                                                              * 
                                                                                              * 0: x1
                                                                                              * 1: x2
                                                                                              * 2: x4
                                                                                              */
#define OTP_SW_LOG_BOOT_CPUPLL_DELAY_RATIO(x)       ((u32)(((x) & 0x00000003) << 0))
#define OTP_SW_LOG_GET_BOOT_CPUPLL_DELAY_RATIO(x)   ((u32)(((x >> 0) & 0x00000003)))
/** @} */


/** @defgroup REG_SYS_SW_RESV_2
 * @brief
 * @{
 */
#define OTP_SW_LOG_MASK_RESV_008                    ((u32)0xFFFFFFFF << 0)                   /*!< RW    0x0 None */
#define OTP_SW_LOG_RESV_008(x)                      ((u32)(((x) & 0xFFFFFFFF) << 0))
#define OTP_SW_LOG_GET_RESV_008(x)                  ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup REG_SYS_SW_RESV_3
 * @brief
 * @{
 */
#define OTP_SW_LOG_MASK_RESV_00C                    ((u32)0xFFFFFFFF << 0)                   /*!< RW    0x0 None */
#define OTP_SW_LOG_RESV_00C(x)                      ((u32)(((x) & 0xFFFFFFFF) << 0))
#define OTP_SW_LOG_GET_RESV_00C(x)                  ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup REG_SYS_SW_RESV_4
 * @brief
 * @{
 */
#define OTP_SW_LOG_MASK_RESV_010                    ((u32)0xFFFFFFFF << 0)                   /*!< RW    0x0 None */
#define OTP_SW_LOG_RESV_010(x)                      ((u32)(((x) & 0xFFFFFFFF) << 0))
#define OTP_SW_LOG_GET_RESV_010(x)                  ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup REG_SYS_SW_RESV_5
 * @brief
 * @{
 */
#define OTP_SW_LOG_MASK_RESV_014                    ((u32)0xFFFFFFFF << 0)                   /*!< RW    0x0 None */
#define OTP_SW_LOG_RESV_014(x)                      ((u32)(((x) & 0xFFFFFFFF) << 0))
#define OTP_SW_LOG_GET_RESV_014(x)                  ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup REG_SYS_SW_RESV_6
 * @brief
 * @{
 */
#define OTP_SW_LOG_MASK_RESV_018                    ((u32)0xFFFFFFFF << 0)                   /*!< RW    0x0 None */
#define OTP_SW_LOG_RESV_018(x)                      ((u32)(((x) & 0xFFFFFFFF) << 0))
#define OTP_SW_LOG_GET_RESV_018(x)                  ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup REG_SYS_SW_RESV_7
 * @brief
 * @{
 */
#define OTP_SW_LOG_MASK_RESV_01C                    ((u32)0xFFFFFFFF << 0)                   /*!< RW    0x0 None */
#define OTP_SW_LOG_RESV_01C(x)                      ((u32)(((x) & 0xFFFFFFFF) << 0))
#define OTP_SW_LOG_GET_RESV_01C(x)                  ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */




/** @defgroup OTP_SW_LOG_Exported_Types OTP_SW_LOG Exported Types
* @{
*/
/**
* @brief OTP_SW_LOG Register Declaration
*/

typedef struct {
	__IO uint32_t ROM_BOOT_CONFIG0;                             /*!< Register,  Address offset:0x0000 */
	__IO uint32_t ROM_BOOT_CONFIG1;                             /*!< Register,  Address offset:0x0004 */
	__IO uint32_t SYS_SW_RESV_2;                                /*!< Register,  Address offset:0x0008 */
	__IO uint32_t SYS_SW_RESV_3;                                /*!< Register,  Address offset:0x000C */
	__IO uint32_t SYS_SW_RESV_4;                                /*!< Register,  Address offset:0x0010 */
	__IO uint32_t SYS_SW_RESV_5;                                /*!< Register,  Address offset:0x0014 */
	__IO uint32_t SYS_SW_RESV_6;                                /*!< Register,  Address offset:0x0018 */
	__IO uint32_t SYS_SW_RESV_7;                                /*!< Register,  Address offset:0x001C */
} OTP_SW_LOG_TypeDef;

#define OTP_SW_LOG_BASE ((OTP_SW_LOG_TypeDef *) (0x40230600))
/** @} */


/* AUTO_GEN_END */

/* MANUAL_GEN_START */
#ifdef __cplusplus
extern "C" {
#endif


//Please add your defination here


#ifdef __cplusplus
}
#endif

/* MANUAL_GEN_END */

/** @} */

/** @} */
#endif  // SYSREG_SW_OTP_LOG_H
