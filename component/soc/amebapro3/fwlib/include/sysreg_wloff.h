/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef SYSREG_WLOFF_H
#define SYSREG_WLOFF_H

/* AUTO_GEN_START */

/* WLOFF is a part of PON */


/** @addtogroup Ameba_Periph_Driver
* @{
*/
/** @defgroup WLOFF
* @brief WLOFF driver modules
* @{
*/


/** @defgroup REG_SEC_SCLK_CTRL
 * @brief
 * @{
 */
/** @} */


/** @defgroup REG_TRNG_CLK_CTRL
 * @brief
 * @{
 */
/** @} */


/** @defgroup REG_BT_32K_CLK_CTRL
 * @brief
 * @{
 */
#define WLOFF_BIT_BT_32K_CLK_VLD                    ((u32)0x00000001 << 24)                  /*!< R     0x0 Clock valid */
#define WLOFF_BIT_BT_32K_CLK_DIV_DONE               ((u32)0x00000001 << 23)                  /*!< R     0x0 1: Clock dividing processing is done */
#define WLOFF_BIT_BT_32K_CLK_EN                     ((u32)0x00000001 << 19)                  /*!< R/W   0x1
                                                                                              * Clock enable. Used during clock source switching.
                                                                                              * 0: Clock disable
                                                                                              * 1: Clock enable
                                                                                              * SW flow for switching clock source:
                                                                                              * (1) Disable clock.
                                                                                              * (2) Poll clock valid = 0.
                                                                                              * (3) Program clock select bits.
                                                                                              * (4) Enable clock.
                                                                                              */
#define WLOFF_MASK_BT_32K_CLK_SEL                   ((u32)0x00000007 << 16)                  /*!< R/W   0x4
                                                                                              * Clock select:
                                                                                              * 0x0: SDM_32K
                                                                                              * 0x1: HSSDM
                                                                                              * 0x2: XTAL_40M_div
                                                                                              * 0x3: XTAL_32K
                                                                                              * 0x4: OSC_131K_div4
                                                                                              * 0x5: OSC_131K
                                                                                              * Others: 1'b0
                                                                                              */
#define WLOFF_BT_32K_CLK_SEL(x)                     ((u32)(((x) & 0x00000007) << 16))
#define WLOFF_GET_BT_32K_CLK_SEL(x)                 ((u32)(((x >> 16) & 0x00000007)))
/** @} */


/** @defgroup REG_BT_40M_CLK_CTRL
 * @brief
 * @{
 */
#define WLOFF_BIT_BT_40M_CLK_VLD                    ((u32)0x00000001 << 24)                  /*!< R     0x0 Clock valid */
#define WLOFF_BIT_BT_40M_CLK_DIV_DONE               ((u32)0x00000001 << 23)                  /*!< R     0x0 1: Clock dividing processing is done */
#define WLOFF_BIT_BT_40M_CLK_EN                     ((u32)0x00000001 << 19)                  /*!< R/W   0x1
                                                                                              * Clock enable. Used during clock source switching.
                                                                                              * 0: Clock disable
                                                                                              * 1: Clock enable
                                                                                              * SW flow for switching clock source:
                                                                                              * (1) Disable clock.
                                                                                              * (2) Poll clock valid = 0.
                                                                                              * (3) Program clock select bits.
                                                                                              * (4) Enable clock.
                                                                                              */
#define WLOFF_MASK_BT_40M_CLK_SEL                   ((u32)0x00000007 << 16)                  /*!< R/W   0x2
                                                                                              * Clock select:
                                                                                              * 0x0: XTAL_40M
                                                                                              * 0x1: WLFI_PLL (320Mhz/8)
                                                                                              * 0x2: BT PLL (160Mhz/4)
                                                                                              * Others: 1'b0
                                                                                              */
#define WLOFF_BT_40M_CLK_SEL(x)                     ((u32)(((x) & 0x00000007) << 16))
#define WLOFF_GET_BT_40M_CLK_SEL(x)                 ((u32)(((x >> 16) & 0x00000007)))
#define WLOFF_BIT_BT_40M_CLK_DIVTRG                 ((u32)0x00000001 << 14)                  /*!< R/W1P 0x0
                                                                                              * Trigger to change clock divider ratio. Write 1 to start the clock divider.
                                                                                              * SW flow for changing clock divider ratio:
                                                                                              * (1) Poll clock valid = 1.
                                                                                              * (2) Program clock divider factor(s).
                                                                                              * (3) Write 1 to clock divider trigger bit (DIVTRG).
                                                                                              */
#define WLOFF_MASK_BT_40M_CLK_DIVN                  ((u32)0x0000003F << 0)                   /*!< R/W   0x3 Clock divider factor. */
#define WLOFF_BT_40M_CLK_DIVN(x)                    ((u32)(((x) & 0x0000003F) << 0))
#define WLOFF_GET_BT_40M_CLK_DIVN(x)                ((u32)(((x >> 0) & 0x0000003F)))
/** @} */


/** @defgroup REG_SYS_CLK_CTRL
 * @brief
 * @{
 */
#define WLOFF_BIT_BT_32K_CLKEN                      ((u32)0x00000001 << 8)                   /*!< R/W   0x0 Clock enable BT_32K_CLKEN */
#define WLOFF_BIT_BT_40M_CLKEN                      ((u32)0x00000001 << 7)                   /*!< R/W   0x0 Clock enable BT_40M_CLKEN */
#define WLOFF_BIT_BB_CLKEN                          ((u32)0x00000001 << 4)                   /*!< R/W   0x0 Clock enable BB_CLKEN */
#define WLOFF_BIT_BTUART_PCLKEN                     ((u32)0x00000001 << 3)                   /*!< R/W   0x0 Clock enable BTUART_PCLKEN */
#define WLOFF_BIT_BTUART_SCLKEN                     ((u32)0x00000001 << 2)                   /*!< R/W   0x0 Clock enable BTUART_SCLKEN */
#define WLOFF_BIT_WLOFF_HCLKEN                      ((u32)0x00000001 << 1)                   /*!< R/W   0x0 NOT USED */
#define WLOFF_BIT_WLOFF_REG_CLKEN                   ((u32)0x00000001 << 0)                   /*!< R/W   0x0 NOT USED; Moved to SYOSON register (0x100[15]) */
/** @} */


/** @defgroup REG_SYS_FUNC_CTRL
 * @brief
 * @{
 */
#define WLOFF_BIT_BUS_WMAC_FEN_P2_7                 ((u32)0x00000001 << 4)                   /*!< R/W   0  
                                                                                              * WIFI PLATFORM function enable (WMAC CLK domain)
                                                                                              * 0: Keep IP design in reset
                                                                                              * 1: Release reset to IP design
                                                                                              * 
                                                                                              * [UPDATE]
                                                                                              * This bit is not used anymore.
                                                                                              * WIFI PLATFORM WMAC MST port reset is controlled by 0x4000_0004[1]
                                                                                              */
#define WLOFF_BIT_BUS_SYSMEM_FEN_P2_4               ((u32)0x00000001 << 3)                   /*!< R/W   0  
                                                                                              * WIFI PLATFORM function enable (SYS MEM CLK domain)
                                                                                              * 0: Keep IP design in reset
                                                                                              * 1: Release reset to IP design
                                                                                              */
#define WLOFF_BIT_BUS_SLOW_PCLK_FEN_P2_6            ((u32)0x00000001 << 2)                   /*!< R/W   0  
                                                                                              * WIFI PLATFORM function enable (SLOW PCLK CLK domain)
                                                                                              * 0: Keep IP design in reset
                                                                                              * 1: Release reset to IP design
                                                                                              */
#define WLOFF_BIT_BUS_KM0_FW_FEN_P2_2               ((u32)0x00000001 << 1)                   /*!< R/W   0  
                                                                                              * WIFI PLATFORM function enable (KM0 FW CLK domain)
                                                                                              * 0: Keep IP design in reset
                                                                                              * 1: Release reset to IP design
                                                                                              */
#define WLOFF_BIT_BTUART_FEN_S7_25                  ((u32)0x00000001 << 0)                   /*!< R/W   0  
                                                                                              * BTUART function enable
                                                                                              * 0: Keep IP design in reset
                                                                                              * 1: Release reset to IP design
                                                                                              */
/** @} */


/** @defgroup REG_SYS_UART_CTRL
 * @brief
 * @{
 */
#define WLOFF_BIT_SYS_BTUART2PIN_EN                 ((u32)0x00000001 << 1)                   /*!< R/W   0   1: Enable BTUART <-> PIN access path */
#define WLOFF_BIT_SYS_BTUART_MUX_EN                 ((u32)0x00000001 << 0)                   /*!< R/W   0   1: BT_UART to internal BT, 0: Pinmux to internal BT */
/** @} */


/** @defgroup REG_WLOFF_DUMMY0
 * @brief
 * @{
 */
/** @} */


/** @defgroup REG_WLOFF_DUMMY1
 * @brief
 * @{
 */
/** @} */


/** @defgroup REG_WLOFF_DUMMY2
 * @brief
 * @{
 */
/** @} */


/** @defgroup REG_WLOFF_DUMMY3
 * @brief
 * @{
 */
/** @} */


/** @defgroup REG_SYS_MISC_CTRL
 * @brief
 * @{
 */
#define WLOFF_BIT_BT_SRAM_SEL                       ((u32)0x00000001 << 0)                   /*!< R/W     1 1: Enable BT to use 120KB BT SRAM */
/** @} */




/** @defgroup WLOFF_Exported_Types WLOFF Exported Types
* @{
*/
/**
* @brief WLOFF Register Declaration
*/

typedef struct {
	__I  uint32_t SEC_SCLK_CTRL;                                /*!< Register,  Address offset:0x0000 */
	__I  uint32_t TRNG_CLK_CTRL;                                /*!< Register,  Address offset:0x0004 */
	__IO uint32_t BT_32K_CLK_CTRL;                              /*!< Register,  Address offset:0x0008 */
	__IO uint32_t BT_40M_CLK_CTRL;                              /*!< Register,  Address offset:0x000C */
	__IO uint32_t SYS_CLK_CTRL;                                 /*!< Register,  Address offset:0x0010 */
	__IO uint32_t SYS_FUNC_CTRL;                                /*!< Register,  Address offset:0x0014 */
	__IO uint32_t SYS_UART_CTRL;                                /*!< Register,  Address offset:0x0018 */
	__I  uint32_t WLOFF_DUMMY0;                                 /*!< Register,  Address offset:0x001C */
	__I  uint32_t WLOFF_DUMMY1;                                 /*!< Register,  Address offset:0x0020 */
	__I  uint32_t WLOFF_DUMMY2;                                 /*!< Register,  Address offset:0x0024 */
	__I  uint32_t WLOFF_DUMMY3;                                 /*!< Register,  Address offset:0x0028 */
	__IO uint32_t SYS_MISC_CTRL;                                /*!< Register,  Address offset:0x002C */
} WLOFF_TypeDef;

#define WLOFF_BASE ((WLOFF_TypeDef *) (0x4023B000))
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
#endif  // SYSREG_WLOFF_H
