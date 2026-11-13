/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef SYSREG_CORE_NS_H
#define SYSREG_CORE_NS_H

/* AUTO_GEN_START */

/* CORE_NS is a part of SYSON */


/** @addtogroup Ameba_Periph_Driver
* @{
*/
/** @defgroup CORE_NS
* @brief CORE_NS driver modules
* @{
*/


/** @defgroup REG_SYS_DBG_CTRL
 * @brief
 * @{
 */
#define CORE_NS_BIT_WLPLL_DBG_EN                    ((u32)0x00000001 << 6)                   /*!< R/W   0.0 Enable to output WLPLL clock to pin */
#define CORE_NS_BIT_USBPLL_DBG_EN                   ((u32)0x00000001 << 5)                   /*!< R/W   0.0 Enable to output USBPLL clock to pin */
#define CORE_NS_BIT_DDRPLL_DBG_EN                   ((u32)0x00000001 << 4)                   /*!< R/W   0.0 Enable to output DDRPLL clock to pin */
#define CORE_NS_BIT_PERIPLL_DBG_EN                  ((u32)0x00000001 << 3)                   /*!< R/W   0.0 Enable to output PERIPLL clock to pin */
#define CORE_NS_BIT_SYS2PLL_DBG_EN                  ((u32)0x00000001 << 2)                   /*!< R/W   0.0 Enable to output SYS2PLL clock  to pin */
#define CORE_NS_BIT_SYS1PLL_DBG_EN                  ((u32)0x00000001 << 1)                   /*!< R/W   0.0 Enable to output SYS1PLL clock to pin */
#define CORE_NS_BIT_CPUPLL_DBG_EN                   ((u32)0x00000001 << 0)                   /*!< R/W   0.0 Enable to output CPUPLL clock to pin */
/** @} */


/** @defgroup REG_CORE_NS_DUMMY1
 * @brief
 * @{
 */
/** @} */


/** @defgroup REG_SYS_JTAG_SWD_CTRL
 * @brief
 * @{
 */
#define CORE_NS_BIT_RM3_DBG_MD_SEL                  ((u32)0x00000001 << 4)                   /*!< R/W   0.0
                                                                                              * Real-M300 SWJ-DP Mode Selection
                                                                                              * 0:SWD-DP Mode
                                                                                              * 1:JTAG-DP Mode
                                                                                              */
#define CORE_NS_BIT_RM3_DBG_PIN_EN                  ((u32)0x00000001 << 3)                   /*!< R/W   0.0
                                                                                              * Real-M300 SWJ-DP Pinmux Function Enable
                                                                                              * 0:SWD/JTAG Pin Disable
                                                                                              * 1:SWD/JTAG Pin Enable
                                                                                              */
#define CORE_NS_BIT_DBG_CHAIN_EN                    ((u32)0x00000001 << 2)                   /*!< R/W   0.0
                                                                                              * Real-M500 and Real-M300 JTAG Chain Mode Enable
                                                                                              * 0:JTAG Chain Mode Disable
                                                                                              * 1:JTAG Chain Mode Enable
                                                                                              */
#define CORE_NS_BIT_RM5_DBG_MD_SEL                  ((u32)0x00000001 << 1)                   /*!< R/W   0.0
                                                                                              * Real-M500 SWJ-DP Mode Selection
                                                                                              * 0:SWD-DP Mode
                                                                                              * 1:JTAG-DP Mode
                                                                                              */
#define CORE_NS_BIT_RM5_DBG_PIN_EN                  ((u32)0x00000001 << 0)                   /*!< R/W   0.0
                                                                                              * Real-M500 SWJ-DP Pinmux Function Enable
                                                                                              * 0:SWD/JTAG Pin Disable
                                                                                              * 1:SWD/JTAG Pin Enable
                                                                                              */
/** @} */




/** @defgroup CORE_NS_Exported_Types CORE_NS Exported Types
* @{
*/
/**
* @brief CORE_NS Register Declaration
*/

typedef struct {
	__IO uint32_t SYS_DBG_CTRL;                                 /*!< Register,  Address offset:0x0000 */
	__I  uint32_t CORE_NS_DUMMY1;                               /*!< Register,  Address offset:0x0004 */
	__IO uint32_t SYS_JTAG_SWD_CTRL;                            /*!< Register,  Address offset:0x0008 */
} CORE_NS_TypeDef;

#define CORE_NS_BASE ((CORE_NS_TypeDef *) (0x6020C000))
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
#endif  // SYSREG_CORE_NS_H
