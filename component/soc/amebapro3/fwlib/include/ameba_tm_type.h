/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef AMEBA_TM_TYPE_H
#define AMEBA_TM_TYPE_H

/* AUTO_GEN_START */

/* TM is a part of TM0 */


/** @addtogroup Ameba_Periph_Driver
* @{
*/
/** @defgroup TM
* @brief TM driver modules
* @{
*/


/** @defgroup lc
 * @brief
 * @{
 */
#define TM_MASK_LC                                  ((u32)0xFFFFFFFF << 0)                   /*!< R/W   0.0 The counter value to be loaded into the timer counter when timer is enabled or reset.Min = 0, when prescaler register > 0; otherwise Min = 1. */
#define TM_LC(x)                                    ((u32)(((x) & 0xFFFFFFFF) << 0))
#define TM_GET_LC(x)                                ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup tc
 * @brief
 * @{
 */
#define TM_MASK_TC                                  ((u32)0xFFFFFFFF << 0)                   /*!< R/W   0.0 Current counter value. */
#define TM_TC(x)                                    ((u32)(((x) & 0xFFFFFFFF) << 0))
#define TM_GET_TC(x)                                ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup pc
 * @brief
 * @{
 */
#define TM_MASK_PC                                  ((u32)0x000003FF << 0)                   /*!< R/W   0.0 Pre-scaler counter will be increased every clock after timer is enabled. When the prescaler counter is equal to PR, the next clock increments (decrements) the TC and clears the PC. */
#define TM_PC(x)                                    ((u32)(((x) & 0x000003FF) << 0))
#define TM_GET_PC(x)                                ((u32)(((x >> 0) & 0x000003FF)))
/** @} */


/** @defgroup pr
 * @brief
 * @{
 */
#define TM_MASK_PR                                  ((u32)0x000003FF << 0)                   /*!< R/W   0.0 When the PC is equal to this value, the next clock increments (decrements) the TC and clears the PC. */
#define TM_PR(x)                                    ((u32)(((x) & 0x000003FF) << 0))
#define TM_GET_PR(x)                                ((u32)(((x >> 0) & 0x000003FF)))
/** @} */


/** @defgroup ctrl
 * @brief
 * @{
 */
#define TM_BIT_CNT_MOD                              ((u32)0x00000001 << 3)                   /*!< R/W   0.0 Timer counting mode:0: Up-counter1: Down-counter */
#define TM_BIT_IMR                                  ((u32)0x00000001 << 2)                   /*!< R/W   0.0 Timer Interrupt mask:0: disable interrupt1: enable interrupt */
#define TM_BIT_MOD                                  ((u32)0x00000001 << 1)                   /*!< R/W   0.0 Timer operation mode:0: timer mode (periodical mode); it reloads LC to TC and repeat the process according to the setting when the timeout event occurred.1: counter mode (one-shot mode); only one time process. */
#define TM_BIT_EN                                   ((u32)0x00000001 << 0)                   /*!< R/W   0.0 Timer enable control:0: Disable1: Enable */
/** @} */


/** @defgroup isr
 * @brief
 * @{
 */
#define TM_BIT_MATCH_EV3                            ((u32)0x00000001 << 4)                   /*!< R/W1C 0.0 Counter value match event3 interrupt. */
#define TM_BIT_MATCH_EV2                            ((u32)0x00000001 << 3)                   /*!< R/W1C 0.0 Counter value match event2 interrupt. */
#define TM_BIT_MATCH_EV1                            ((u32)0x00000001 << 2)                   /*!< R/W1C 0.0 Counter value match event1 interrupt. */
#define TM_BIT_MATCH_EV0                            ((u32)0x00000001 << 1)                   /*!< R/W1C 0.0 Counter value match event0 interrupt. */
#define TM_BIT_TIMEOUT                              ((u32)0x00000001 << 0)                   /*!< R/W1C 0.0 Timeout interrupt */
/** @} */


/** @defgroup mectrl
 * @brief
 * @{
 */
#define TM_BIT_ME3_EN                               ((u32)0x00000001 << 3)                   /*!< R/W   0.0 Counter value match event3 enable control:0: Disable1: Enable */
#define TM_BIT_ME2_EN                               ((u32)0x00000001 << 2)                   /*!< R/W   0.0 Counter value match event2 enable control:0: Disable1: Enable */
#define TM_BIT_ME1_EN                               ((u32)0x00000001 << 1)                   /*!< R/W   0.0 Counter value match event1 enable control:0: Disable1: Enable */
#define TM_BIT_ME0_EN                               ((u32)0x00000001 << 0)                   /*!< R/W   0.0 Counter value match event0 enable control:0: Disable1: Enable */
/** @} */


/** @defgroup me0
 * @brief
 * @{
 */
#define TM_MASK_ME0                                 ((u32)0xFFFFFFFF << 0)                   /*!< R/W   0.0 Timer counter value for metch event0. */
#define TM_ME0(x)                                   ((u32)(((x) & 0xFFFFFFFF) << 0))
#define TM_GET_ME0(x)                               ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup me1
 * @brief
 * @{
 */
#define TM_MASK_ME1                                 ((u32)0xFFFFFFFF << 0)                   /*!< R/W   0.0 Timer counter value for metch event1. */
#define TM_ME1(x)                                   ((u32)(((x) & 0xFFFFFFFF) << 0))
#define TM_GET_ME1(x)                               ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup me2
 * @brief
 * @{
 */
#define TM_MASK_ME2                                 ((u32)0xFFFFFFFF << 0)                   /*!< R/W   0.0 Timer counter value for metch event2. */
#define TM_ME2(x)                                   ((u32)(((x) & 0xFFFFFFFF) << 0))
#define TM_GET_ME2(x)                               ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup me3
 * @brief
 * @{
 */
#define TM_MASK_ME3                                 ((u32)0xFFFFFFFF << 0)                   /*!< R/W   0.0 Timer counter value for metch event3. */
#define TM_ME3(x)                                   ((u32)(((x) & 0xFFFFFFFF) << 0))
#define TM_GET_ME3(x)                               ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */




/** @defgroup TM_Exported_Types TM Exported Types
* @{
*/
/**
* @brief TM Register Declaration
*/

typedef struct {
	__IO uint32_t LC;                                           /*!< Register,  Address offset:0x0000 */
	__IO uint32_t TC;                                           /*!< Register,  Address offset:0x0004 */
	__IO uint32_t PC;                                           /*!< Register,  Address offset:0x0008 */
	__IO uint32_t PR;                                           /*!< Register,  Address offset:0x000C */
	__IO uint32_t CTRL;                                         /*!< Register,  Address offset:0x0010 */
	__IO uint32_t ISR;                                          /*!< Register,  Address offset:0x0014 */
	uint32_t RSVD_NO_FUNCTION0[1];
	__IO uint32_t MECTRL;                                       /*!< Register,  Address offset:0x001C */
	__IO uint32_t ME0;                                          /*!< Register,  Address offset:0x0020 */
	__IO uint32_t ME1;                                          /*!< Register,  Address offset:0x0024 */
	__IO uint32_t ME2;                                          /*!< Register,  Address offset:0x0028 */
	__IO uint32_t ME3;                                          /*!< Register,  Address offset:0x002C */
} RTIM_TypeDef;

// #define TM_BASE ((TM_TypeDef *) (0x40002040))
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

// TMx offset (x + 1) * 0x40


// // TG0
// #define TM0_BASE    ((TM_TypeDef *) (0x40205040))
// #define TM1_BASE    ((TM_TypeDef *) (0x40205080))
// #define TM2_BASE    ((TM_TypeDef *) (0x402050C0))
// #define TM3_BASE    ((TM_TypeDef *) (0x40205100))
// #define TM4_BASE    ((TM_TypeDef *) (0x40205140))
// #define TM5_BASE    ((TM_TypeDef *) (0x40205180))
// #define TM6_BASE    ((TM_TypeDef *) (0x402051C0))
// #define TM7_BASE    ((TM_TypeDef *) (0x40205200))


// // TODO
// // TG1
// #define TM8_BASE    ((TM_TypeDef *) (0x40206040))

// // TG2
// #define TM9_BASE    ((TM_TypeDef *) (0x40207040))
// #define TM10_BASE   ((TM_TypeDef *) (0x40207080))
// #define TM11_BASE   ((TM_TypeDef *) (0x402070C0))
// #define TM12_BASE   ((TM_TypeDef *) (0x40207100))
// #define TM13_BASE   ((TM_TypeDef *) (0x40207140))
// #define TM14_BASE   ((TM_TypeDef *) (0x40207180))
// #define TM15_BASE   ((TM_TypeDef *) (0x402071C0))
// #define TM16_BASE   ((TM_TypeDef *) (0x40207200))

// // TG3
// #define TM17_BASE   ((TM_TypeDef *) (0x40208040))



/** @} */

/** @} */
#endif  // AMEBA_TM_TYPE_H
