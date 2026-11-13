/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef SYSREG_KM0_H
#define SYSREG_KM0_H

/* AUTO_GEN_START */

/* KM0 is a part of KM0_REG */


/** @addtogroup Ameba_Periph_Driver
* @{
*/
/** @defgroup KM0
* @brief KM0 driver modules
* @{
*/


/** @defgroup REG_KM0_CTRL
 * @brief
 * @{
 */
#define KM0_BIT_CPU_RST_HALT                        ((u32)0x00000001 << 1)                   /*!< R/W   0.0 1: Hold the CPU in reset state after reset */
#define KM0_BIT_SYST_CLK_EN                         ((u32)0x00000001 << 0)                   /*!< R/W   0.0
                                                                                              * Systick clock enable.
                                                                                              * Systick would count if systick is enable and syst_clk_en is 1 at the rising edge of cpu clock (sysclk_f)
                                                                                              */
/** @} */


/** @defgroup REG_KM0_CFG0
 * @brief
 * @{
 */
#define KM0_BIT_INITD2ITCM                          ((u32)0x00000001 << 12)                  /*!< R/W   0.0 Initial value of D2ITCM in ACTLR */
#define KM0_BIT_INITITCMEN0                         ((u32)0x00000001 << 11)                  /*!< R/W   0.0 Initial value of EN in ITCMCR */
#define KM0_BIT_RETENTION_MODE                      ((u32)0x00000001 << 10)                  /*!< R/W   0.0
                                                                                              * Initial value of RETMODE in ACTLR.
                                                                                              * After reset, KM/TM processor hardware automatically clears internal instruction and data caches by writing 0’s to those cache memories. This input pin provides an alternative to keep cache contents when reset. That is, if the retention_mode input pin is tied to 1, the processor hardware will not clear caches after reset, so that cache contents will be retained. On the other hand, when tied to 0, the processor clear cache automatically.
                                                                                              */
#define KM0_MASK_CFG_CPU_NUM                        ((u32)0x000003FF << 0)                   /*!< R/W   0.0 Strap to set CPU number in CPUNUM register */
#define KM0_CFG_CPU_NUM(x)                          ((u32)(((x) & 0x000003FF) << 0))
#define KM0_GET_CFG_CPU_NUM(x)                      ((u32)(((x >> 0) & 0x000003FF)))
/** @} */


/** @defgroup REG_KM0_CFG1
 * @brief
 * @{
 */
#define KM0_MASK_INITVTOR_S                         ((u32)0xFFFFFFFF << 0)                   /*!< R/W   0.0 Initial value of secure VTOR (0xe000ed08, Vector Table Offset Register). */
#define KM0_INITVTOR_S(x)                           ((u32)(((x) & 0xFFFFFFFF) << 0))
#define KM0_GET_INITVTOR_S(x)                       ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup REG_KM0_CFG2
 * @brief
 * @{
 */
#define KM0_MASK_INITVTOR_NS                        ((u32)0xFFFFFFFF << 0)                   /*!< R/W   0.0 Initial value of non-secure VTOR (0xe000ed08, Vector Table Offset Register). */
#define KM0_INITVTOR_NS(x)                          ((u32)(((x) & 0xFFFFFFFF) << 0))
#define KM0_GET_INITVTOR_NS(x)                      ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup REG_KM0_CFG3
 * @brief
 * @{
 */
#define KM0_MASK_INITZWFRANGE                       ((u32)0xFFFFFFFF << 0)                   /*!< R/W   0.0 None */
#define KM0_INITZWFRANGE(x)                         ((u32)(((x) & 0xFFFFFFFF) << 0))
#define KM0_GET_INITZWFRANGE(x)                     ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup REG_KM0_DBG0
 * @brief
 * @{
 */
#define KM0_BIT_TRACE_I0_RETIRE                     ((u32)0x00000001 << 15)                  /*!< R     0.0 None */
#define KM0_BIT_TRACE_I1_RETIRE                     ((u32)0x00000001 << 14)                  /*!< R     0.0 None */
#define KM0_BIT_PC_VALID                            ((u32)0x00000001 << 13)                  /*!< R     0.0 None */
#define KM0_BIT_SL_SLEEPSYS_R                       ((u32)0x00000001 << 12)                  /*!< R     0.0 CPU in sleep state */
#define KM0_BIT_SYSRESET_REQ                        ((u32)0x00000001 << 11)                  /*!< R     0.0 System reset request */
#define KM0_BIT_DBGRESTARTED_M                      ((u32)0x00000001 << 10)                  /*!< R     1.0 Indication whether the core is restarted from debug mode */
#define KM0_BIT_HALTED_M                            ((u32)0x00000001 << 9)                   /*!< R     0.0 Indication whether the core is halted */
#define KM0_BIT_LOCKUP_M                            ((u32)0x00000001 << 8)                   /*!< R     0.0 Indication whether the core is locked up */
#define KM0_BIT_EDBGRQ                              ((u32)0x00000001 << 5)                   /*!< R/W   0.0 None */
#define KM0_BIT_DBGRESTART                          ((u32)0x00000001 << 4)                   /*!< R/W   0.0 None */
#define KM0_BIT_SPNIDEN                             ((u32)0x00000001 << 3)                   /*!< R/W   0.0 Debug authentication interface. Secure debug noninvasive enable. */
#define KM0_BIT_SPIDEN                              ((u32)0x00000001 << 2)                   /*!< R/W   0.0 Debug authentication interface. Secure debug enable. */
#define KM0_BIT_NIDEN                               ((u32)0x00000001 << 1)                   /*!< R/W   0.0 Debug authentication interface. Debug noninvasive enable. */
#define KM0_BIT_DBGEN                               ((u32)0x00000001 << 0)                   /*!< R/W   0.0 Debug authentication interface. Debug enable. */
/** @} */


/** @defgroup REG_KM0_DBG1
 * @brief
 * @{
 */
#define KM0_MASK_TRACE_I0_IVADDR                    ((u32)0xFFFFFFFF << 0)                   /*!< R     0.0 None */
#define KM0_TRACE_I0_IVADDR(x)                      ((u32)(((x) & 0xFFFFFFFF) << 0))
#define KM0_GET_TRACE_I0_IVADDR(x)                  ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup REG_KM0_DBG2
 * @brief
 * @{
 */
#define KM0_MASK_TRACE_I1_IVADDR                    ((u32)0xFFFFFFFF << 0)                   /*!< R     0.0 None */
#define KM0_TRACE_I1_IVADDR(x)                      ((u32)(((x) & 0xFFFFFFFF) << 0))
#define KM0_GET_TRACE_I1_IVADDR(x)                  ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup REG_KM0_DBG3
 * @brief
 * @{
 */
#define KM0_MASK_PC_VALUE                           ((u32)0xFFFFFFFF << 0)                   /*!< R     0.0 None */
#define KM0_PC_VALUE(x)                             ((u32)(((x) & 0xFFFFFFFF) << 0))
#define KM0_GET_PC_VALUE(x)                         ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup REG_KM0_INTR_STS
 * @brief
 * @{
 */
/** @} */


/** @defgroup REG_KM0_INTR_MASK
 * @brief
 * @{
 */
/** @} */


/** @defgroup REG_KM0_SPARE0
 * @brief
 * @{
 */
#define KM0_MASK_KM0_SPARE0                         ((u32)0xFFFFFFFF << 0)                   /*!< R/W   0.0 SPARE for SW */
#define KM0_KM0_SPARE0(x)                           ((u32)(((x) & 0xFFFFFFFF) << 0))
#define KM0_GET_KM0_SPARE0(x)                       ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup REG_KM0_SPARE1
 * @brief
 * @{
 */
#define KM0_MASK_KM0_SPARE1                         ((u32)0xFFFFFFFF << 0)                   /*!< R/W   0.0 SPARE for SW */
#define KM0_KM0_SPARE1(x)                           ((u32)(((x) & 0xFFFFFFFF) << 0))
#define KM0_GET_KM0_SPARE1(x)                       ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */




/** @defgroup KM0_Exported_Types KM0 Exported Types
* @{
*/
/**
* @brief KM0 Register Declaration
*/

typedef struct {
	__IO uint32_t KM0_CTRL;                                     /*!< Register,  Address offset:0x0000 */
	__IO uint32_t KM0_CFG0;                                     /*!< Register,  Address offset:0x0004 */
	__IO uint32_t KM0_CFG1;                                     /*!< Register,  Address offset:0x0008 */
	__IO uint32_t KM0_CFG2;                                     /*!< Register,  Address offset:0x000C */
	__IO uint32_t KM0_CFG3;                                     /*!< Register,  Address offset:0x0010 */
	__IO uint32_t KM0_DBG0;                                     /*!< Register,  Address offset:0x0014 */
	__I  uint32_t KM0_DBG1;                                     /*!< Register,  Address offset:0x0018 */
	__I  uint32_t KM0_DBG2;                                     /*!< Register,  Address offset:0x001C */
	__I  uint32_t KM0_DBG3;                                     /*!< Register,  Address offset:0x0020 */
	__I  uint32_t KM0_INTR_STS;                                 /*!< Register,  Address offset:0x0024 */
	__I  uint32_t KM0_INTR_MASK;                                /*!< Register,  Address offset:0x0028 */
	__IO uint32_t KM0_SPARE0;                                   /*!< Register,  Address offset:0x002C */
	__IO uint32_t KM0_SPARE1;                                   /*!< Register,  Address offset:0x0030 */
} KM0_TypeDef;

#define KM0_BASE ((KM0_TypeDef *) (0x40239000))
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
#endif  // SYSREG_KM0_H
