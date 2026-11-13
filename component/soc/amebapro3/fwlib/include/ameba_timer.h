/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef AMEBA_TIMER_H
#define AMEBA_TIMER_H

/* AUTO_GEN_START */

/* AUTO_GEN_END */

/* MANUAL_GEN_START */

//Please add your defination here
#include "ameba_tg_type.h"
#include "ameba_tm_type.h"

/* Exported types ------------------------------------------------------------*/
/** @addtogroup PWMTIMER_Exported_Types PWMTIMER Exported Types
  * @{
  */

/**
  * @brief  TIM Basic Init Structure Definition
  */
typedef struct {

	u8 TIM_TimerIdx;
	u8 TIM_TimerGrp;

	u16 TIM_Prescaler;		/*!< Specifies the prescaler value used to divide the TIM clock.
							This parameter can be a number between 0x0 and 0xFFFF, basic timer dont care */
	u32 TIM_LoadCnt;
	u32 TIM_PeriodUs;

	u8  TIM_CntMode;
	u8  TIM_OpMode;
	u8 	TIM_IntMask;

} RTIM_TimeBaseInitTypeDef;


/**
  * @}
  */

/* Exported constants --------------------------------------------------------*/
/** @defgroup PWMTIMER_Exported_Constants PWMTIMER Exported Constants
  * @{
  */

/** @defgroup TIM_Type
  * @{
  */
#define IS_TIM_ALL_TIM(PERIPH)		(((PERIPH) == TIM0) || \
									((PERIPH) == TIM1) || \
									((PERIPH) == TIM2) || \
									((PERIPH) == TIM3) || \
									((PERIPH) == TIM4) || \
									((PERIPH) == TIM5) || \
									((PERIPH) == TIM6) || \
									((PERIPH) == TIM7) || \
									((PERIPH) == TIM8) || \
									((PERIPH) == TIM9) || \
									((PERIPH) == TIM10) || \
									((PERIPH) == TIM11) || \
									((PERIPH) == TIM12) || \
									((PERIPH) == TIM13) || \
									((PERIPH) == TIM14) || \
									((PERIPH) == TIM15) || \
									((PERIPH) == TIM16) || \
									((PERIPH) == TIM17) || \
									((PERIPH) == TIM0_S) || \
									((PERIPH) == TIM1_S) || \
									((PERIPH) == TIM2_S) || \
									((PERIPH) == TIM3_S) || \
									((PERIPH) == TIM4_S) || \
									((PERIPH) == TIM5_S) || \
									((PERIPH) == TIM6_S) || \
									((PERIPH) == TIM7_S) || \
									((PERIPH) == TIM8_S) || \
									((PERIPH) == TIM9_S) || \
									((PERIPH) == TIM10_S) || \
									((PERIPH) == TIM11_S) || \
									((PERIPH) == TIM12_S) || \
									((PERIPH) == TIM13_S) || \
									((PERIPH) == TIM14_S) || \
									((PERIPH) == TIM15_S) || \
									((PERIPH) == TIM16_S) || \
									((PERIPH) == TIM17_S))

#define IS_TIM_40M_TIM(PERIPH)	(((PERIPH) == TIM8) || \
								((PERIPH) == TIM9) || \
								((PERIPH) == TIM10) || \
								((PERIPH) == TIM11) || \
								((PERIPH) == TIM8_S) || \
								((PERIPH) == TIM9_S) || \
								((PERIPH) == TIM10_S)|| \
								((PERIPH) == TIM11_S))
/**
  * @}
  */

/** @defgroup TIM_Prescaler
  * @{
  */
#define IS_TIM_PSC(VAL) (VAL <= 0x3FF)
/**
  * @}
  */



/**
  * @}
  */

/* Exported functions --------------------------------------------------------*/
/** @defgroup PWMTIMER_Exported_Functions PWMTIMER Exported Functions
  * @{
  */

/** @defgroup TimeBase_Management_Functions TimeBase Management Functions
* @{
*/
_LONG_CALL_ void RTIM_Cmd(u32 TimerIdx, u32 NewState);
_LONG_CALL_ void RTIM_SetLoadCnt(u32 TimerIdx, u32 reload_val);
_LONG_CALL_ void RTIM_GetLoadCnt(u32 TimerIdx, u32 reload_val);
_LONG_CALL_ void RTIM_SetPrescaler(u32 TimerIdx, u32 Prescaler);
_LONG_CALL_ u32 RTIM_GetPrescaler(u32 TimerIdx);
_LONG_CALL_ void RTIM_UpdatePrescalerCnt(u32 TimerIdx, u32 PrescalerCnt);
_LONG_CALL_ u16 RTIM_GetPrescalerCnt(u32 TimerIdx);
_LONG_CALL_ void RTIM_UpdateCurrentCount(u32 TimerIdx, u32 curr_cnt);
_LONG_CALL_ u32 RTIM_GetCurrentCount(u32 TimerIdx);
_LONG_CALL_ void RTIM_SetMatchEvent(u32 TimerIdx, u32 me_num, u32 me_val);
_LONG_CALL_ void RTIM_MatchEventCmd(u32 TimerIdx, u32 me_num, u32 NewState);
_LONG_CALL_ void RTIM_SetOpMode(u32 TimerIdx, u32 op_mode);
_LONG_CALL_ void RTIM_SetCntMode(u32 TimerIdx, u32 cnt_mode);
_LONG_CALL_ void RTIM_Reset(u32 TimerIdx);
_LONG_CALL_ void RTIM_TimeBaseStructInit(RTIM_TimeBaseInitTypeDef *TIM_InitStruct);
_LONG_CALL_ void RTIM_TimeBaseInit(u32 TimerGrp, RTIM_TimeBaseInitTypeDef *TIM_InitStruct);
_LONG_CALL_ void RTIM_DeInit(u32 TimerIdx);
/**
  * @}
  */

/** @defgroup Interrupt_Management_Functions Interrupt Management Functions
* @{
*/
_LONG_CALL_ void RTIM_INTConfig(u32 TimerIdx, u32 TIM_IT, u32 NewState);
_LONG_CALL_ u32 RTIM_INTClear(u32 TimerIdx);
_LONG_CALL_ void RTIM_INTClearPendingBit(u32 TimerIdx, u32 TIM_IT);
_LONG_CALL_ u32 RTIM_GetFlagStatus(u32 TimerIdx, u32 TIM_FLAG);
_LONG_CALL_ u32 RTIM_GetINTStatus(u32 TimerIdx, u32 TIM_IT);
_LONG_CALL_ void RTIM_RegTimeoutIRQ(u32 TimerIdx, IRQ_FUN irq_func, void *irq_data);
_LONG_CALL_ void RTIM_UnRegTimeoutIRQ(u32 TimerIdx);
_LONG_CALL_ void RTIM_RegMEIRQ(u32 TimerIdx, u32 me_num, IRQ_FUN irq_func, void *irq_data);
_LONG_CALL_ void RTIM_UnRegMEIRQ(u32 TimerIdx, u32 me_num);
/**
 * @}
 */


/** @defgroup Pro2 Verification Functions
 * @{
 */
_LONG_CALL_ void RTIM_StartOneShot(u32 TimerGrp, RTIM_TimeBaseInitTypeDef *TIM_InitStruct, IRQ_FUN irq_func, void *irq_data);
_LONG_CALL_ void RTIM_StartPeriodic(u32 TimerGrp, RTIM_TimeBaseInitTypeDef *TIM_InitStruct, IRQ_FUN irq_func, void *irq_data);
_LONG_CALL_ void RTIM_EnableAlarm(u32 TimerIdx, u32 me_num, u32 time_ticks, IRQ_FUN irq_func, void *irq_data);
/**
 * @}
 */

/** @defgroup Timer Group related functions
 * @{
 */
_LONG_CALL_ u8 RTIM_GTIMER_GetISTS(TG_TypeDef *TGx);
_LONG_CALL_ u8 RTIM_GTIMER_GetRawISTS(TG_TypeDef *TGx);
_LONG_CALL_ u32 RTIM_GTIMER_IndirectRead(TG_TypeDef *TGx, u32 tm_num);
_LONG_CALL_ void RTIM_GTIMER_Init(u32 TimerGrp);
_LONG_CALL_ void RTIM_GTIMER_DeInit(u32 TimerGrp);
/**
 * @}
 */

/**
  * @}
  */

/** @} */

/** @} */

/* Other Definitions -------------------------------------------------------------------*/

#define GTimerNum 	(4)
#define TimerNum	(18)

extern int GTIMERx_irq[GTimerNum];
extern RTIM_TypeDef *RTIMx[TimerNum];
extern RTIM_TypeDef *RTIMx_S[TimerNum];
extern u32 APBPeriph_PON_TIMERx[GTimerNum];
extern u32 APBPeriph_PON_TIMERx_CLOCK[GTimerNum];
extern TG_TypeDef *GTIMERx[GTimerNum];
extern TG_TypeDef *GTIMERx_S[GTimerNum];


#define TIMER_TICK_US			(31)
#define TIMER_TICK_US_X4		(4*1000000/32000) //32k clock, 31.25us every timer_tick

#define TIMER_POLLING_TIMES		4000		//Polling Timer Status Register Timeout



#define TIM_GROUP_0_2_TIMER_NUM  (8)
#define TIM_GROUP_1_3_TIMER_NUM  (1)


#define TIM_TG0		((u32)0x00000000)
#define TIM_TG1		((u32)0x00000001)
#define TIM_TG2		((u32)0x00000002)
#define TIM_TG3		((u32)0x00000003)
#define TIM_MAX_TG_NUM           (4)
#define IS_TIM_TG_NUM(NUM)      ((NUM) < TIM_MAX_TG_NUM)

#define TIM_MAX_TM_NUM           (18)
#define IS_TIM_TM_NUM(NUM)      ((NUM) < TIM_MAX_TM_NUM)





#define TIM_MAX_MATCH_EVENT_CNT ((u32)0x00000004)
#define TIM_MATCH_EVENT_0		((u32)0x00000000)
#define TIM_MATCH_EVENT_1		((u32)0x00000001)
#define TIM_MATCH_EVENT_2		((u32)0x00000002)
#define TIM_MATCH_EVENT_3		((u32)0x00000003)
#define IS_TIM_MATCH_EVENT_CNT(CNT)		(((CNT) == TIM_MATCH_EVENT_0) || \
										((CNT) == TIM_MATCH_EVENT_1) || \
										((CNT) == TIM_MATCH_EVENT_2) || \
										((CNT) == TIM_MATCH_EVENT_3))


#define TIM_OP_MODE_PERIODICAL  ((u32)0x00000000)
#define TIM_OP_MODE_ONE_SHOT    ((u32)0x00000001)
#define IS_TIM_OP_MODE(MODE)    (((MODE) == TIM_OP_MODE_PERIODICAL) || \
							    ((MODE) == TIM_OP_MODE_PERIODICAL))


#define TIM_CNT_MODE_UP         ((u32)0x00000000)
#define TIM_CNT_MODE_DOWN       ((u32)0x00000001)
#define IS_TIM_CNT_MODE(MODE)   (((MODE) == TIM_CNT_MODE_UP) || \
							    ((MODE) == TIM_CNT_MODE_DOWN))


#define TIM_IT_TIMEOUT			((u32)0x00000000)
#define TIM_IT_ME0  			((u32)0x00000001)
#define TIM_IT_ME1				((u32)0x00000002)
#define TIM_IT_ME2				((u32)0x00000004)
#define TIM_IT_ME3				((u32)0x00000008)
#define IS_TIM_IT(IT)	        (((IT) == TIM_IT_TIMEOUT) || \
                                ((IT) == TIM_IT_ME0) || \
                                ((IT) == TIM_IT_ME1) || \
                                ((IT) == TIM_IT_ME2) || \
                                ((IT) == TIM_IT_ME3))

#define TIM_TG1_TIMER       ((u32)8)
#define TIM_TG3_TIMER       ((u32)17)

#define TIM_SCLK_4M         ((u32)4000000)
#define TIM_SCLK_40M        ((u32)40000000)
#define TIM_SCLK_31P25M     ((u32)31250000)


#endif		// AMEBA_TIMER_H



