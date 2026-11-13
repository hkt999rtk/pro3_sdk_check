/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef AMEBA_PWM_H
#define AMEBA_PWM_H

/* AUTO_GEN_START */

/* PWM is a part of PWM0 */


/** @addtogroup Ameba_Periph_Driver
* @{
*/
/** @defgroup PWM
* @brief PWM driver modules
* @{
*/


/** @defgroup ctrl
 * @brief
 * @{
 */
#define PWM_BIT_CTRL_SET                            ((u32)0x00000001 << 31)                  /*!< R/W   0.0 SW can change setting only at ctrl_set = 0, and set ctrl_set = 1 after changing PWM Ctrl. HW will clear ctrl_set bit after changing PWM Ctrl.PS. If this PWM is disabled (PWMx_EN=0), the HW will not clear this bit.PS. if this bit is asserted and the duty auto adjustment is enabled, when the PWM is enabled the HW will updates its duty at the 1st PWM period and then start the duty adjustment at 2nd PWM period. */
#define PWM_BIT_PAUSE                               ((u32)0x00000001 << 30)                  /*!< R/W   0.0 The paus control:0: Normal running1: To pause the PWM out at current PWM period end. Since the PWM output is not stopped immediately, the software can poll the BIT_PERI_PWMx_RUNSTS to know the PWM out is still running or is paused. */
#define PWM_BIT_RUN_STS                             ((u32)0x00000001 << 29)                  /*!< R     0.0 The PWM output running status:0: PWM output is paused1: the PWM output is running. */
#define PWM_BIT_PERIOD_IE                           ((u32)0x00000001 << 28)                  /*!< R/W   0.0 This bit control the PWM period end interrupt enable (1) or disable (0). When the PWM period end interrupt is enabled, the PWM HW will assert the interrupt on every PWM period end time. This interrupt status can be read from pwm_int_status. */
#define PWM_MASK_CUR_DUTY                           ((u32)0x00000FFF << 16)                  /*!< R     0.0 The current duty size which is adjusted by the duty auto-adjustment HW. The SW can read this field to know the current duty size. However, the latest duty size may not be sync to this field on time or the value may transition while the SW is reading this field. So use in-direct read method to read the duty size is more save and is suggested. */
#define PWM_CUR_DUTY(x)                             ((u32)(((x) & 0x00000FFF) << 16))
#define PWM_GET_CUR_DUTY(x)                         ((u32)(((x >> 16) & 0x00000FFF)))
#define PWM_MASK_CLK_SEL                            ((u32)0x0000000F << 12)                  /*!< R/W   0.0 The tick source selection, G-timer 0 ~ 7 or SClk:0: GTIMER 01: GTIMER 1...7: GTIMER 78: sclk */
#define PWM_CLK_SEL(x)                              ((u32)(((x) & 0x0000000F) << 12))
#define PWM_GET_CLK_SEL(x)                          ((u32)(((x >> 12) & 0x0000000F)))
#define PWM_MASK_DUTY                               ((u32)0x00000FFF << 0)                   /*!< R/W   0.0 The on-duty duration of PWM pulse. The time unit is configured by the GTIMER which is specified by clk_sel field. It can be written at any time, but HW will apply the changing at PWM enable time or at the end of PWM period. */
#define PWM_DUTY(x)                                 ((u32)(((x) & 0x00000FFF) << 0))
#define PWM_GET_DUTY(x)                             ((u32)(((x >> 0) & 0x00000FFF)))
/** @} */


/** @defgroup timing_ctrl
 * @brief
 * @{
 */
#define PWM_MASK_DUTY_START                         ((u32)0x00000FFF << 16)                  /*!< R/W   0.0 The start of ON duration of PWM pulse in the period. The time unit is the same as PWM period.It can be written at any time, but HW change setting only at PWM enable and the end of period. */
#define PWM_DUTY_START(x)                           ((u32)(((x) & 0x00000FFF) << 16))
#define PWM_GET_DUTY_START(x)                       ((u32)(((x >> 16) & 0x00000FFF)))
#define PWM_MASK_PERIOD_VAL                         ((u32)0x00000FFF << 0)                   /*!< R/W   0.0 The period of PWM pulse. The time unit is configured by the GTIMER which is specified by the clk_sel field. It can be written at any time, but the HW will apply the changing at PWM enable time or at the end of PWM period. */
#define PWM_PERIOD_VAL(x)                           ((u32)(((x) & 0x00000FFF) << 0))
#define PWM_GET_PERIOD_VAL(x)                       ((u32)(((x >> 0) & 0x00000FFF)))
/** @} */


/** @defgroup auto_adj_ctrl
 * @brief
 * @{
 */
#define PWM_BIT_ADJ_EN                              ((u32)0x00000001 << 31)                  /*!< R/W   0.0 This bit is used to enable the Duty Ato Adjustment.0: Disable1: EnablePS. If this bit is 1, the duty of the auto adjustment will override the duty setting in the pwm_ctrl register. */
#define PWM_BIT_ADJ_DIR                             ((u32)0x00000001 << 30)                  /*!< R/W   0.0 This bit is used to set the Duty Ato Adjustment direction.0: Decrease Duty1: Increase Duty */
#define PWM_BIT_ADJ_LOOP_EN                         ((u32)0x00000001 << 29)                  /*!< R/W   0.0 This bit is used to enable the Duty Auto Adjustment Loop mode.0: Disable1: EnableIf the Loop Mode is enabled, reverse the Duty Auto Adjustment direction when the adjusted Duty reach the Up Limit or the Down Limit. */
#define PWM_BIT_DUTY_UP_LIM_IE                      ((u32)0x00000001 << 28)                  /*!< R/W   0.0 This bit is used to enable the Interrupt of the Duty Auto Adjustment.0: Disable1: EnableIf the Interrupt is enabled, issue an interrupt when the Adjusted Duty reach the Up Limit. */
#define PWM_BIT_DUTY_DN_LIM_IE                      ((u32)0x00000001 << 27)                  /*!< R/W   0.0 This bit is used to enable the Interrupt of the Duty Auto Adjustment.0: Disable1: EnableIf the Interrupt is enabled, issue an interrupt when the Adjusted Duty reach the Down Limit. */
#define PWM_MASK_DUTY_INC_STEP                      ((u32)0x000003FF << 12)                  /*!< R/W   0.0 The Duty Increasing Step size of the Duty Auto Adjustment */
#define PWM_DUTY_INC_STEP(x)                        ((u32)(((x) & 0x000003FF) << 12))
#define PWM_GET_DUTY_INC_STEP(x)                    ((u32)(((x >> 12) & 0x000003FF)))
#define PWM_MASK_DUTY_DEC_STEP                      ((u32)0x000003FF << 0)                   /*!< R/W   0.0 The Duty Decreasing Step size of the Duty Auto Adjustment. */
#define PWM_DUTY_DEC_STEP(x)                        ((u32)(((x) & 0x000003FF) << 0))
#define PWM_GET_DUTY_DEC_STEP(x)                    ((u32)(((x >> 0) & 0x000003FF)))
/** @} */


/** @defgroup auto_adj_limit
 * @brief
 * @{
 */
#define PWM_MASK_DUTY_ADJ_UP_LIM                    ((u32)0x00000FFF << 16)                  /*!< R/W   0.0 The up limit of the duty period for the duty ratio auto adjustment */
#define PWM_DUTY_ADJ_UP_LIM(x)                      ((u32)(((x) & 0x00000FFF) << 16))
#define PWM_GET_DUTY_ADJ_UP_LIM(x)                  ((u32)(((x >> 16) & 0x00000FFF)))
#define PWM_MASK_DUTY_ADJ_DN_LIM                    ((u32)0x00000FFF << 0)                   /*!< R/W   0.0 The down limit (in ticks) of the duty period for the duty ratio auto adjustment */
#define PWM_DUTY_ADJ_DN_LIM(x)                      ((u32)(((x) & 0x00000FFF) << 0))
#define PWM_GET_DUTY_ADJ_DN_LIM(x)                  ((u32)(((x >> 0) & 0x00000FFF)))
/** @} */


/** @defgroup auto_adj_cycle
 * @brief
 * @{
 */
#define PWM_MASK_ADJ_CYCLES                         ((u32)0x00000FFF << 0)                   /*!< R/W   0.0 The Cycle Count of the Duty Auto Adjustment.The Duty size will be increased/decreased with a step size every Cycle Count of PWM period */
#define PWM_ADJ_CYCLES(x)                           ((u32)(((x) & 0x00000FFF) << 0))
#define PWM_GET_ADJ_CYCLES(x)                       ((u32)(((x >> 0) & 0x00000FFF)))
/** @} */


/** @defgroup out_dma
 * @brief
 * @{
 */
#define PWM_BIT_DMA_MODE                            ((u32)0x00000001 << 2)                   /*!< R/W   0.0 1 : Enable, 0: Disable DMA Mode. */
#define PWM_BIT_CHG_PERIOD                          ((u32)0x00000001 << 1)                   /*!< R/W   0.0 1: this parameter defines to update PWM period register for each period-end interrupt. 0: no update. */
#define PWM_BIT_CHG_DUTY                            ((u32)0x00000001 << 0)                   /*!< R/W   0.0 1: this parameter defines to update PWM duty register for each period-end interrupt. 0: no update. */
/** @} */


/** @defgroup dma_buff
 * @brief
 * @{
 */
#define PWM_MASK_BUFF                               ((u32)0xFFFFFFFF << 0)                   /*!< R/W   0.0 Data register for DMA mode. */
#define PWM_BUFF(x)                                 ((u32)(((x) & 0xFFFFFFFF) << 0))
#define PWM_GET_BUFF(x)                             ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */




/** @defgroup PWM_Exported_Types PWM Exported Types
* @{
*/
/**
* @brief PWM Register Declaration
*/

typedef struct {
	__IO uint32_t CTRL;                                         /*!< Register,  Address offset:0x0000 */
	__IO uint32_t TIMING_CTRL;                                  /*!< Register,  Address offset:0x0004 */
	__IO uint32_t AUTO_ADJ_CTRL;                                /*!< Register,  Address offset:0x0008 */
	__IO uint32_t AUTO_ADJ_LIMIT;                               /*!< Register,  Address offset:0x000C */
	__IO uint32_t AUTO_ADJ_CYCLE;                               /*!< Register,  Address offset:0x0010 */
	__IO uint32_t OUT_DMA;                                      /*!< Register,  Address offset:0x0014 */
	__IO uint32_t DMA_BUFF;                                     /*!< Register,  Address offset:0x0018 */
} PWM_TypeDef;

// #define PWM_BASE ((PWM_TypeDef *) (0x40045020))
/** @} */


/* AUTO_GEN_END */

/* MANUAL_GEN_START */
#ifdef __cplusplus
extern "C" {
#endif


//Please add your defination here

#define PWM0_BASE ((PWM_TypeDef *) (PWM_REG_BASE + 0x020))
#define PWM1_BASE ((PWM_TypeDef *) (PWM_REG_BASE + 0x040))
#define PWM2_BASE ((PWM_TypeDef *) (PWM_REG_BASE + 0x060))
#define PWM3_BASE ((PWM_TypeDef *) (PWM_REG_BASE + 0x080))
#define PWM4_BASE ((PWM_TypeDef *) (PWM_REG_BASE + 0x0A0))
#define PWM5_BASE ((PWM_TypeDef *) (PWM_REG_BASE + 0x0C0))
#define PWM6_BASE ((PWM_TypeDef *) (PWM_REG_BASE + 0x0E0))
#define PWM7_BASE ((PWM_TypeDef *) (PWM_REG_BASE + 0x100))
// #define PWM8_BASE ((PWM_TypeDef *) (PWM_REG_BASE + 0x120))
// #define PWM9_BASE ((PWM_TypeDef *) (PWM_REG_BASE + 0x140))
// #define PWM10_BASE ((PWM_TypeDef *) (PWM_REG_BASE + 0x160))
// #define PWM11_BASE ((PWM_TypeDef *) (PWM_REG_BASE + 0x180))

#define PWM0_S_BASE ((PWM_TypeDef *) (PWM_REG_BASE_S + 0x020))
#define PWM1_S_BASE ((PWM_TypeDef *) (PWM_REG_BASE_S + 0x040))
#define PWM2_S_BASE ((PWM_TypeDef *) (PWM_REG_BASE_S + 0x060))
#define PWM3_S_BASE ((PWM_TypeDef *) (PWM_REG_BASE_S + 0x080))
#define PWM4_S_BASE ((PWM_TypeDef *) (PWM_REG_BASE_S + 0x0A0))
#define PWM5_S_BASE ((PWM_TypeDef *) (PWM_REG_BASE_S + 0x0C0))
#define PWM6_S_BASE ((PWM_TypeDef *) (PWM_REG_BASE_S + 0x0E0))
#define PWM7_S_BASE ((PWM_TypeDef *) (PWM_REG_BASE_S + 0x100))
// #define PWM8_S_BASE ((PWM_TypeDef *) (PWM_REG_BASE_S + 0x120))
// #define PWM9_S_BASE ((PWM_TypeDef *) (PWM_REG_BASE_S + 0x140))
// #define PWM10_S_BASE ((PWM_TypeDef *) (PWM_REG_BASE_S + 0x160))
// #define PWM11_S_BASE ((PWM_TypeDef *) (PWM_REG_BASE_S + 0x180))

typedef void (*PWM_IRQ_FUN)(void *Data);

/**
  * @brief  PWM Init Structure Definition
  */
typedef struct {

	// u8 PWM_Index;
	u16 PWM_Period;		// in ticks, 1 tick = 1 timer timeout period
	u16 PWM_DutySize;	// in ticks, 1 tick = 1 timer timeout period
	u8 PWM_TickSource;	// tick source, 1 tick = 1 timer timeout period

} PWM_InitTypeDef;

#define PWM_MAX_NUM     ((u32)8)
#define PWM_DMA_NUM		((u32)4)

#define PWM0			((u32)0x00000001)
#define PWM1			((u32)0x00000002)
#define PWM2			((u32)0x00000004)
#define PWM3			((u32)0x00000008)
#define PWM4			((u32)0x00000010)
#define PWM5			((u32)0x00000020)
#define PWM6			((u32)0x00000040)
#define PWM7			((u32)0x00000080)
#define PWM_ALL_CH		((u32)0x000000FF)


#define PWM_IS_PAUSED   ((u32)0x00000000)
#define PWM_IS_RUNNING  ((u32)0x00000001)
#define IS_PWM_STATUS(STS)	(((STS) == PWM_IS_PAUSED) || \
							((STS) == PWM_IS_RUNNING))


#define PWM_DUTY_ADJ_DECREMENT ((u32)0x00000000)
#define PWM_DUTY_ADJ_INCREMENT ((u32)0x00000001)
#define IS_PWM_DUTY_ADJ_DIR(DIR)	(((DIR) == PWM_DUTY_ADJ_DECREMENT) || \
									((DIR) == PWM_DUTY_ADJ_INCREMENT))


#define PWM_TICK_SOURCE_TIMER0   ((u32)0x00000000)
#define PWM_TICK_SOURCE_TIMER1   ((u32)0x00000001)
#define PWM_TICK_SOURCE_TIMER2   ((u32)0x00000002)
#define PWM_TICK_SOURCE_TIMER3   ((u32)0x00000003)
#define PWM_TICK_SOURCE_TIMER4   ((u32)0x00000004)
#define PWM_TICK_SOURCE_TIMER5   ((u32)0x00000005)
#define PWM_TICK_SOURCE_TIMER6   ((u32)0x00000006)
#define PWM_TICK_SOURCE_TIMER7   ((u32)0x00000007)
#define PWM_TICK_SOURCE_SCLK      ((u32)0x00000008)
#define IS_PWM_TICK_SOURCE(SRC) (((SRC) == PWM_TICK_SOURCE_TIMER0) || \
								((SRC) == PWM_TICK_SOURCE_TIMER1) || \
								((SRC) == PWM_TICK_SOURCE_TIMER2) || \
								((SRC) == PWM_TICK_SOURCE_TIMER3) || \
								((SRC) == PWM_TICK_SOURCE_TIMER4) || \
								((SRC) == PWM_TICK_SOURCE_TIMER5) || \
								((SRC) == PWM_TICK_SOURCE_TIMER6) || \
								((SRC) == PWM_TICK_SOURCE_TIMER7) || \
								((SRC) == PWM_TICK_SOURCE_SCLK))


#define PWM_INT_TYPE_PERIOD_END            ((u32)0x00000000)
#define PWM_INT_TYPE_DUTY_ADJ_UP_LIMIT     ((u32)0x00000001)
#define PWM_INT_TYPE_DUTY_ADJ_DOWN_LIMIT   ((u32)0x00000002)
#define IS_PWM_INT_TYPE(TYPE)	(((TYPE) == PWM_INT_TYPE_PERIOD_END) || \
								((TYPE) == PWM_INT_TYPE_DUTY_ADJ_UP_LIMIT) || \
								((TYPE) == PWM_INT_TYPE_DUTY_ADJ_DOWN_LIMIT))


#define PWM_PAIR_0_1 		((u32)0x00000000)
#define PWM_PAIR_2_3 		((u32)0x00000001)
#define PWM_PAIR_4_5 		((u32)0x00000002)
#define PWM_PAIR_6_7 		((u32)0x00000003)
#define PWM_PAIR_8_9 		((u32)0x00000004)
#define PWM_PAIR_A_B 		((u32)0x00000005)
#define IS_PWM_PAIR(PAIR)	(((PAIR) == PWM_PAIR_0_1) || \
							((PAIR) == PWM_PAIR_2_3) || \
							((PAIR) == PWM_PAIR_4_5) || \
							((PAIR) == PWM_PAIR_6_7) || \
							((PAIR) == PWM_PAIR_8_9) || \
							((PAIR) == PWM_PAIR_A_B))


#define PWM_DMA_CHG_DUTY			((u32)0x00000001)
#define PWM_DMA_CHG_PERIOD			((u32)0x00000002)
#define PWM_DMA_CHG_DUTY_PERIOD		((u32)0x00000003)
#define IS_PWM_DMA_MODE(MODE)		(((MODE) == PWM_DMA_CHG_DUTY) || \
									((MODE) == PWM_DMA_CHG_PERIOD) || \
									((MODE) == PWM_DMA_CHG_DUTY_PERIOD))

#define IS_PWM_DMA_SUPPORT(X)		((X < PWM_DMA_NUM))

_LONG_CALL_ void PWM_SetTickSource(u8 pwm_num, u32 tick_src);
_LONG_CALL_ void PWM_COMM_EnableCtrl(u16 pwm_multi_en);
_LONG_CALL_ void PWM_COMM_DisableCtrl(u16 pwm_multi_dis);
_LONG_CALL_ BOOL PWM_TXGDMA_Init(u8 pwm_num, GDMA_InitTypeDef *GDMA_InitStruct, void *CallbackData, IRQ_FUN CallbackFunc, u8 *pTXData, u32 Length);
_LONG_CALL_ void PWM_SetDmaEnable(u8 pwm_num, u8 dma_mode, u32 NewState);
_LONG_CALL_ void PWM_RegPeriodEndInt(u8 pwm_num, PWM_IRQ_FUN irq_func, void *irq_data);
_LONG_CALL_ void PWM_RegDutyUpLimInt(u8 pwm_num, PWM_IRQ_FUN irq_func, void *irq_data);
_LONG_CALL_ void PWM_RegDutyDnLimInt(u8 pwm_num, PWM_IRQ_FUN irq_func, void *irq_data);
_LONG_CALL_ u16 PWM_COMM_IndirectReadAdjDutySize(u8 pwm_sel);
_LONG_CALL_ void PWM_COMM_SetComplementary(u8 pwm_pair, u8 db_val, u8 comp_en);
_LONG_CALL_ void PWM_COMM_IRQInit(void);
_LONG_CALL_ void PWM_StructInit(PWM_InitTypeDef *PWM_InitStruct);
_LONG_CALL_ void PWM_Init(u8 pwm_num, PWM_InitTypeDef *PWM_InitStruct, PWM_IRQ_FUN irq_func, void *irq_data);
_LONG_CALL_ void PWM_DeInit(u8 pwm_num);
_LONG_CALL_ void PWM_Cmd(u8 pwm_num, u32 NewState);
_LONG_CALL_ void PWM_INTConfig(u8 pwm_num, u32 int_type, u32 NewState);
_LONG_CALL_ void PWM_UpdateCtrl(u8 pwm_num);
_LONG_CALL_ void PWM_SetDutySize(u8 pwm_num, u16 duty_size);
_LONG_CALL_ void PWM_SetDutyStartOffset(u8 pwm_num, u16 offset);
_LONG_CALL_ void PWM_SetPeriod(u8 pwm_num, u16 unit_time);
_LONG_CALL_ void PWM_SetPeriodEndInt(u8 pwm_num, u32 state);
_LONG_CALL_ void PWM_SetAdjUpLimit(u8 pwm_num, u16 up_limit);
_LONG_CALL_ void PWM_SetAdjDownLimit(u8 pwm_num, u16 down_limit);
_LONG_CALL_ void PWM_SetAdjIncSize(u8 pwm_num, u16 step_size);
_LONG_CALL_ void PWM_SetAdjDecSize(u8 pwm_num, u16 step_size);
_LONG_CALL_ void PWM_SetAdjCycleCnt(u8 pwm_num, u16 cycle_cnt);
_LONG_CALL_ void PWM_DutyAdjEnable(u8 pwm_num, u32 state);
_LONG_CALL_ void PWM_SetDutyAdjDir(u8 pwm_num, u32 init_dir);
_LONG_CALL_ void PWM_SetDutyAdjLoopMode(u8 pwm_num, u32 state);
_LONG_CALL_ void PWM_SetAdjUpLimitInt(u8 pwm_num, u32 state);
_LONG_CALL_ void PWM_SetAdjDownLimitInt(u8 pwm_num, u32 state);

/* for verification purpose */
_LONG_CALL_ void PWM_AutoDutyAdjInc(u8 pwm_num, u32 up_limit, u32 step_size, u32 cycle_cnt);
_LONG_CALL_ void PWM_AutoDutyAdjDec(u8 pwm_num, u32 down_limit, u32 step_size, u32 cycle_cnt);
_LONG_CALL_ void PWM_AutoDutyAdjLoop(u8 pwm_num, u32 init_dir, u32 up_limit, u32 down_limit, u32 inc_step_size, u32 dec_step_size, u32 cycle_cnt);

#ifdef __cplusplus
}
#endif

/* MANUAL_GEN_END */

/** @} */

/** @} */
#endif  // AMEBA_PWM_H

