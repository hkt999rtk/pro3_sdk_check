/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef AMEBA_PWM_COMM_H
#define AMEBA_PWM_COMM_H

/* AUTO_GEN_START */

/* PWM_COMM is a part of PWM_COMM */


/** @addtogroup Ameba_Periph_Driver
* @{
*/
/** @defgroup PWM_COMM
* @brief PWM_COMM driver modules
* @{
*/


/** @defgroup enable_status
 * @brief
 * @{
 */
#define PWM_COMM_MASK_PWM_EN_STS                    ((u32)0x00000FFF << 0)                   /*!< R     0.0 Each bit indicates the enabling status of corresponding PWM:0: the specific PWM is Disabled1: the specific PWM is Enabled */
#define PWM_COMM_PWM_EN_STS(x)                      ((u32)(((x) & 0x00000FFF) << 0))
#define PWM_COMM_GET_PWM_EN_STS(x)                  ((u32)(((x >> 0) & 0x00000FFF)))
/** @} */


/** @defgroup enable_ctrl
 * @brief
 * @{
 */
#define PWM_COMM_MASK_PWM_EN                        ((u32)0x00000FFF << 0)                   /*!< R/W   0.0 Each bit is used to enablel the corresponding PWM function:When wrote 0: No operation1: the specific PWM is enabled and the specific bit of REG_PWM_EN_STS is ONE. */
#define PWM_COMM_PWM_EN(x)                          ((u32)(((x) & 0x00000FFF) << 0))
#define PWM_COMM_GET_PWM_EN(x)                      ((u32)(((x >> 0) & 0x00000FFF)))
/** @} */


/** @defgroup disable_ctrl
 * @brief
 * @{
 */
#define PWM_COMM_MASK_PWM_DIS                       ((u32)0x00000FFF << 0)                   /*!< R/W   0.0 Each bit is used to disable the corresponding PW6M function:When wrote 0: No operation1: the conrtolled PWM is disabled and the specific bit of REG_PWM_EN_STS is ZERO. */
#define PWM_COMM_PWM_DIS(x)                         ((u32)(((x) & 0x00000FFF) << 0))
#define PWM_COMM_GET_PWM_DIS(x)                     ((u32)(((x >> 0) & 0x00000FFF)))
/** @} */


/** @defgroup int_status
 * @brief
 * @{
 */
#define PWM_COMM_MASK_PERIOD_END                    ((u32)0x000000FF << 24)                  /*!< R/W1C 0.0 This bits are used to indicate the interrupt pending status for PWM 0 ~7 period end. Write 1 clear. Bit 24 for PWM0, bit 25 for PWM1, ...bit31 for PWM7. */
#define PWM_COMM_PERIOD_END(x)                      ((u32)(((x) & 0x000000FF) << 24))
#define PWM_COMM_GET_PERIOD_END(x)                  ((u32)(((x >> 24) & 0x000000FF)))
#define PWM_COMM_MASK_DUTY_ADJ_UP_LIM               ((u32)0x000000FF << 16)                  /*!< R/W1C 0.0 To indicate the interrupt pending status for PWM 0 ~ 7 duty auto-adjustment reaches the up-limit.Write 1 clear. Bit 16 for PWM0, bit 17 for PWM1, ...bit23 for PWM7. */
#define PWM_COMM_DUTY_ADJ_UP_LIM(x)                 ((u32)(((x) & 0x000000FF) << 16))
#define PWM_COMM_GET_DUTY_ADJ_UP_LIM(x)             ((u32)(((x >> 16) & 0x000000FF)))
#define PWM_COMM_MASK_DUTY_ADJ_DN_LIM               ((u32)0x000000FF << 0)                   /*!< R/W1C 0.0 To indicate the interrupt pending status for PWM 0 ~ 7 duty auto-adjustment reaches the down-limit.Write 1 clear. Bit 0 for PWM0, bit 1 for PWM1,  ...bit7 for PWM7. */
#define PWM_COMM_DUTY_ADJ_DN_LIM(x)                 ((u32)(((x) & 0x000000FF) << 0))
#define PWM_COMM_GET_DUTY_ADJ_DN_LIM(x)             ((u32)(((x >> 0) & 0x000000FF)))
/** @} */


/** @defgroup int_status1
 * @brief
 * @{
 */
#define PWM_COMM_MASK_PERIOD_END_010                ((u32)0x0000000F << 24)                  /*!< R/W1C 0.0 This bits are used to indicate the interrupt pending status for PWM 8 ~B period end. Write 1 clear. Bit 24 for PWM8, bit 25 for PWM1, ...bit27 for PWMB. */
#define PWM_COMM_PERIOD_END_010(x)                  ((u32)(((x) & 0x0000000F) << 24))
#define PWM_COMM_GET_PERIOD_END_010(x)              ((u32)(((x >> 24) & 0x0000000F)))
#define PWM_COMM_MASK_DUTY_ADJ_UP_LIM_010           ((u32)0x0000000F << 16)                  /*!< R/W1C 0.0 To indicate the interrupt pending status for PWM 8 ~ B duty auto-adjustment reaches the up-limit.Write 1 clear. Bit 16 for PWM8, bit 17 for PWM9, ...bit19 for PWMB. */
#define PWM_COMM_DUTY_ADJ_UP_LIM_010(x)             ((u32)(((x) & 0x0000000F) << 16))
#define PWM_COMM_GET_DUTY_ADJ_UP_LIM_010(x)         ((u32)(((x >> 16) & 0x0000000F)))
#define PWM_COMM_MASK_DUTY_ADJ_DN_LIM_010           ((u32)0x0000000F << 0)                   /*!< R/W1C 0.0 To indicate the interrupt pending status for PWM 8 ~ B duty auto-adjustment reaches the down-limit.Write 1 clear. Bit 0 for PWM8, bit 1 for PWM9,  ...bit3 for PWMB. */
#define PWM_COMM_DUTY_ADJ_DN_LIM_010(x)             ((u32)(((x) & 0x0000000F) << 0))
#define PWM_COMM_GET_DUTY_ADJ_DN_LIM_010(x)         ((u32)(((x >> 0) & 0x0000000F)))
/** @} */


/** @defgroup indread_idx
 * @brief
 * @{
 */
#define PWM_COMM_BIT_POOL                           ((u32)0x00000001 << 7)                   /*!< R/W   0.0 Set this bit to enable indirect read current value of timer selected by BIT_PWM_INDREAD_IDX. This bit is cleared by HW when reading is finished and indicates REG_PERI_PWM_INDREAD_DUTY is ready. */
#define PWM_COMM_BIT_SYNC_MODE                      ((u32)0x00000001 << 4)                   /*!< R/W   0.0 Register sync mode selection for PWM clock and APB clock0: Freq. of APB clock >> Freq. of PWM clock (about 10 times)1: Freq. of PWM clock >= Freq. of APB clock */
#define PWM_COMM_MASK_PWM_SEL                       ((u32)0x0000000F << 0)                   /*!< R/W   0.0 This field is used to assign the PWM index (0 ~ 7) for the auto adjusted duty size indirect reading. */
#define PWM_COMM_PWM_SEL(x)                         ((u32)(((x) & 0x0000000F) << 0))
#define PWM_COMM_GET_PWM_SEL(x)                     ((u32)(((x >> 0) & 0x0000000F)))
/** @} */


/** @defgroup indread_duty
 * @brief
 * @{
 */
#define PWM_COMM_MASK_PWM_DUTY                      ((u32)0x00000FFF << 0)                   /*!< R/W   0.0 This field is used to read the auto adjusted duty size of the PWM which is assigned by the pwm_sel. The SW should assign the PWM to read by write the pwm_sel and then read this field to get the current duty size of the specified PWM. */
#define PWM_COMM_PWM_DUTY(x)                        ((u32)(((x) & 0x00000FFF) << 0))
#define PWM_COMM_GET_PWM_DUTY(x)                    ((u32)(((x >> 0) & 0x00000FFF)))
/** @} */


/** @defgroup complementary
 * @brief
 * @{
 */
#define PWM_COMM_MASK_PWM_DB                        ((u32)0x00FFFFFF << 6)                   /*!< R/W   0.0 To indicate the amount of delay. Delay time = (1/Sclk)*PWMX_Y_DB. Bit 6~9 for PWM0_1_DB, bit 10~13 for PWM2_3_DB.. Bit 26~29 for PWMA_B_DB */
#define PWM_COMM_PWM_DB(x)                          ((u32)(((x) & 0x00FFFFFF) << 6))
#define PWM_COMM_GET_PWM_DB(x)                      ((u32)(((x >> 6) & 0x00FFFFFF)))
#define PWM_COMM_MASK_PWM_COMP_EN                   ((u32)0x0000003F << 0)                   /*!< R/W   0.0 Enable/Disable PWM Complementary mode. Bit 0 for PWM0_1, bit 1 for PWM2_3..bit 5 for PWMA_B */
#define PWM_COMM_PWM_COMP_EN(x)                     ((u32)(((x) & 0x0000003F) << 0))
#define PWM_COMM_GET_PWM_COMP_EN(x)                 ((u32)(((x >> 0) & 0x0000003F)))
/** @} */




/** @defgroup PWM_COMM_Exported_Types PWM_COMM Exported Types
* @{
*/
/**
* @brief PWM_COMM Register Declaration
*/

typedef struct {
	__I  uint32_t ENABLE_STATUS;                                /*!< Register,  Address offset:0x0000 */
	__IO uint32_t ENABLE_CTRL;                                  /*!< Register,  Address offset:0x0004 */
	__IO uint32_t DISABLE_CTRL;                                 /*!< Register,  Address offset:0x0008 */
	__IO uint32_t INT_STATUS;                                   /*!< Register,  Address offset:0x000C */
	__IO uint32_t INT_STATUS1;                                  /*!< Register,  Address offset:0x0010 */
	__IO uint32_t INDREAD_IDX;                                  /*!< Register,  Address offset:0x0014 */
	__IO uint32_t INDREAD_DUTY;                                 /*!< Register,  Address offset:0x0018 */
	__IO uint32_t COMPLEMENTARY;                                /*!< Register,  Address offset:0x001C */
} PWM_COMM_TypeDef;

#define PWM_COMM_BASE ((PWM_COMM_TypeDef *) (0x4020F000))
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
#endif  // AMEBA_PWM_COMM_H
