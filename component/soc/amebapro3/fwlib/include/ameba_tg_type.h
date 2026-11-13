/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef AMEBA_TG_TYPE_H
#define AMEBA_TG_TYPE_H

/* AUTO_GEN_START */

/* TG is a part of TG0 */


/** @addtogroup Ameba_Periph_Driver
* @{
*/
/** @defgroup TG
* @brief TG driver modules
* @{
*/


/** @defgroup ists
 * @brief
 * @{
 */
#define TG_MASK_ISTS                                ((u32)0x000000FF << 0)                   /*!< R     0.0 Each bit indicates the interrupt pending status of corresponding timer:0: the specifiied timer has no interrupt pending1: the specifiied timer has pending interrupt */
#define TG_ISTS(x)                                  ((u32)(((x) & 0x000000FF) << 0))
#define TG_GET_ISTS(x)                              ((u32)(((x >> 0) & 0x000000FF)))
/** @} */


/** @defgroup raw_ists
 * @brief
 * @{
 */
#define TG_MASK_RISTS                               ((u32)0x000000FF << 0)                   /*!< R     0.0 Each bit indicates the interrupt pending status of corresponding timer:0: the specifiied timer has no interrupt pending1: the specifiied timer has pending interrupt (pre-masing) */
#define TG_RISTS(x)                                 ((u32)(((x) & 0x000000FF) << 0))
#define TG_GET_RISTS(x)                             ((u32)(((x >> 0) & 0x000000FF)))
/** @} */


/** @defgroup tsel
 * @brief
 * @{
 */
#define TG_BIT_POLL                                 ((u32)0x00000001 << 7)                   /*!< R/W   0.0 Set this bit to enable indirect read current value of timer selected by tsel. This bit is cleared by HW while finishing read and indicate REG_TIMER_TC is ready */
#define TG_BIT_SYNC_MODE                            ((u32)0x00000001 << 4)                   /*!< R/W   0.0 Sync mode between Timer clock and APB clock0: Freq. of APB clock >> Freq. of Timer clock (about 10 times)1: others */
#define TG_MASK_TSEL                                ((u32)0x00000007 << 0)                   /*!< R/W   0.0 The timer index in a timer group, to select the timer for indirect read. */
#define TG_TSEL(x)                                  ((u32)(((x) & 0x00000007) << 0))
#define TG_GET_TSEL(x)                              ((u32)(((x >> 0) & 0x00000007)))
/** @} */


/** @defgroup tc
 * @brief
 * @{
 */
#define TG_MASK_TIMER_TC                            ((u32)0xFFFFFFFF << 0)                   /*!< R     0.0 Current counter value of the specifiied timer by register tg_tsel. */
#define TG_TIMER_TC(x)                              ((u32)(((x) & 0xFFFFFFFF) << 0))
#define TG_GET_TIMER_TC(x)                          ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */




/** @defgroup TG_Exported_Types TG Exported Types
* @{
*/
/**
* @brief TG Register Declaration
*/

typedef struct {
	__I  uint32_t ISTS;                                         /*!< Register,  Address offset:0x0000 */
	__I  uint32_t RAW_ISTS;                                     /*!< Register,  Address offset:0x0004 */
	__IO uint32_t TSEL;                                         /*!< Register,  Address offset:0x0008 */
	__I  uint32_t TC;                                           /*!< Register,  Address offset:0x000C */
} TG_TypeDef;


/** @} */


/* AUTO_GEN_END */

/* MANUAL_GEN_START */
#ifdef __cplusplus
extern "C" {
#endif


//Please add your defination here

// #define TG0_BASE ((TG_TypeDef *) (TIMER0_REG_BASE))
// #define TG1_BASE ((TG_TypeDef *) (TIMER1_REG_BASE))
// #define TG2_BASE ((TG_TypeDef *) (TIMER2_REG_BASE))
// #define TG3_BASE ((TG_TypeDef *) (TIMER3_REG_BASE))

// #define TG0_BASE_S ((TG_TypeDef *) (TIMER0_REG_BASE_S))
// #define TG1_BASE_S ((TG_TypeDef *) (TIMER1_REG_BASE_S))
// #define TG2_BASE_S ((TG_TypeDef *) (TIMER2_REG_BASE_S))
// #define TG3_BASE_S ((TG_TypeDef *) (TIMER3_REG_BASE_S))



#ifdef __cplusplus
}
#endif

/* MANUAL_GEN_END */

/** @} */

/** @} */
#endif  // AMEBA_TG_TYPE_H
