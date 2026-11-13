/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "ameba_soc.h"
#include "ameba_timer.h"

static const char *TAG = "GTIMER";

u8 TG_init_status[GTimerNum] = {0, 0, 0, 0};
u8 TM_inUsed[GTimerNum] = {0x00, 0x00, 0x00, 0x00};

IRQ_FUN TM_TimeoutIrqHandler[TimerNum];
void *TM_TimeoutIrqData[TimerNum];

IRQ_FUN TM_MatchEventIrqHandler[TimerNum * 4];
void *TM_MatchEventIrqData[TimerNum * 4];

IRQ_FUN TIMx_IRQHandler[TimerNum];
void *TIMx_IrqData[TimerNum];

TG_TypeDef *GTIMERx[4] = {
	GTIMER0, GTIMER1, GTIMER2, GTIMER3
};

TG_TypeDef *GTIMERx_S[4] = {
	GTIMER0_S, GTIMER1_S, GTIMER2_S, GTIMER3_S
};

int GTIMERx_irq[4] = {
	GTIMER0_IRQ,
	GTIMER1_IRQ,
	GTIMER2_IRQ,
	GTIMER3_IRQ
};

RTIM_TypeDef *RTIMx[TimerNum] = {
	TIM0,       // tg0
	TIM1,
	TIM2,
	TIM3,
	TIM4,
	TIM5,
	TIM6,
	TIM7,
	TIM8,       // tg1
	TIM9,       // tg2
	TIM10,
	TIM11,
	TIM12,
	TIM13,
	TIM14,
	TIM15,
	TIM16,
	TIM17       // tg3
};

u32 APBPeriph_PON_TIMERx[GTimerNum] = {
	APBPeriph_PON_TIMER0,
	APBPeriph_PON_TIMER1,
	APBPeriph_PON_TIMER2,
	APBPeriph_PON_TIMER3
};

u32 APBPeriph_PON_TIMERx_CLOCK[GTimerNum] = {
	APBPeriph_PON_TIMER0_CLOCK,
	APBPeriph_PON_TIMER1_CLOCK,
	APBPeriph_PON_TIMER2_CLOCK,
	APBPeriph_PON_TIMER3_CLOCK
};

RTIM_TypeDef *RTIMx_S[TimerNum] = {
	TIM0_S,
	TIM1_S,
	TIM2_S,
	TIM3_S,
	TIM4_S,
	TIM5_S,
	TIM6_S,
	TIM7_S,
	TIM8_S,
	TIM9_S,
	TIM10_S,
	TIM11_S,
	TIM12_S,
	TIM13_S,
	TIM14_S,
	TIM15_S,
	TIM16_S,
	TIM17_S
};


/**
  * @brief  Enables or disables the specified TIM peripheral.
  * @param  TimerIdx: where x can be 0-17 to select the TIMx peripheral.
  * @param  NewState: new state of the TIMx peripheral.
  *   This parameter can be: ENABLE or DISABLE.
  * @note gtimer 0: TimerIdx 0-7
  * @note gtimer 1: TimerIdx 8
  * @note gtimer 2: TimerIdx 9-16
  * @note gtimer 3: TimerIdx 17
  * @retval None
  */
void RTIM_Cmd(u32 TimerIdx, u32 NewState)
{
	/* Check the parameters */
	// assert_param(IS_TIM_ALL_TIM(TIMx));

	RTIM_TypeDef *TIMx = (RTIM_TypeDef *)RTIMx[TimerIdx];

	if (TrustZone_IsSecure()) {
		TIMx = RTIMx_S[TimerIdx];
	}

	if (NewState == ENABLE) {
		TIMx->CTRL |= TM_BIT_EN;
	} else {
		TIMx->CTRL &= ~(TM_BIT_EN);
	}
}


/**
  * @brief  set load cnt
  * @param  TimerIdx: where x can be 0-17 to select the TIMx peripheral.
  * @note
  *
  * @retval None
  */
void RTIM_SetLoadCnt(u32 TimerIdx, u32 reload_val)
{
	RTIM_TypeDef *TIMx = (RTIM_TypeDef *)RTIMx[TimerIdx];

	if (TrustZone_IsSecure()) {
		TIMx = RTIMx_S[TimerIdx];
	}

	TIMx->LC = TM_LC(reload_val);
}


/**
  * @brief  get load cnt
  * @param  TimerIdx: where x can be 0-17 to select the TIMx peripheral.
  * @note
  *
  * @retval None
  */
void RTIM_GetLoadCnt(u32 TimerIdx, u32 reload_val)
{
	RTIM_TypeDef *TIMx = (RTIM_TypeDef *)RTIMx[TimerIdx];

	if (TrustZone_IsSecure()) {
		TIMx = RTIMx_S[TimerIdx];
	}

	TIMx->LC = TM_LC(reload_val);
}


/**
  * @brief  set prescaler
  * @param  TimerIdx: where x can be 0-17 to select the TIMx peripheral.
  * @note
  *
  * @retval None
  */
void RTIM_SetPrescaler(u32 TimerIdx, u32 Prescaler)
{
	assert_param(IS_TIM_PSC(Prescaler));

	RTIM_TypeDef *TIMx = (RTIM_TypeDef *)RTIMx[TimerIdx];

	if (TrustZone_IsSecure()) {
		TIMx = RTIMx_S[TimerIdx];
	}

	TIMx->PR = TM_PR(Prescaler);
}

/**
  * @brief  get prescaler
  * @param  TimerIdx: where x can be 0-17 to select the TIMx peripheral.
  * @note
  *
  * @retval None
  */
u32 RTIM_GetPrescaler(u32 TimerIdx)
{
	RTIM_TypeDef *TIMx = (RTIM_TypeDef *)RTIMx[TimerIdx];

	if (TrustZone_IsSecure()) {
		TIMx = RTIMx_S[TimerIdx];
	}

	return TM_GET_PR(TIMx->PR);
}

/**
  * @brief  update current prescaler cnt
  * @param  TimerIdx: where x can be 0-17 to select the TIMx peripheral.
  * @note
  *
  * @retval None
  */
void RTIM_UpdatePrescalerCnt(u32 TimerIdx, u32 PrescalerCnt)
{
	assert_param(IS_TIM_PSC(PrescalerCnt));

	RTIM_TypeDef *TIMx = (RTIM_TypeDef *)RTIMx[TimerIdx];

	if (TrustZone_IsSecure()) {
		TIMx = RTIMx_S[TimerIdx];
	}

	TIMx->PC = TM_PC(PrescalerCnt);
}


/**
  * @brief  get current prescaler cnt
  * @param  TimerIdx: where x can be 0-17 to select the TIMx peripheral.
  * @note
  *
  * @retval None
  */
u16 RTIM_GetPrescalerCnt(u32 TimerIdx)
{
	RTIM_TypeDef *TIMx = (RTIM_TypeDef *)RTIMx[TimerIdx];

	if (TrustZone_IsSecure()) {
		TIMx = RTIMx_S[TimerIdx];
	}

	return TIMx->PC;
}


/**
  * @brief  Updates the TIMx current Counter value.
  * @param  TimerIdx: where x can be 0-17 to select the TIMx peripheral.
  * @retval Timer Current Counter Register value
  */
void RTIM_UpdateCurrentCount(u32 TimerIdx, u32 curr_cnt)
{
	RTIM_TypeDef *TIMx = (RTIM_TypeDef *)RTIMx[TimerIdx];

	if (TrustZone_IsSecure()) {
		TIMx = RTIMx_S[TimerIdx];
	}

	TIMx->TC = TM_TC(curr_cnt);
}


/**
  * @brief  Gets the TIMx current Counter value.
  * @param  TimerIdx: where x can be 0-17 to select the TIMx peripheral.
  * @retval Timer Current Counter Register value
  */
u32 RTIM_GetCurrentCount(u32 TimerIdx)
{
	u32 curr_cnt;

	RTIM_TypeDef *TIMx = (RTIM_TypeDef *)RTIMx[TimerIdx];

	if (TrustZone_IsSecure()) {
		TIMx = RTIMx_S[TimerIdx];
	}

	/* from pro2 implement
	 * A timer can be clocked from a domain much slower than the APB read (e.g. TIM0 as systimer
	 * runs on 32kHz), so a single read can sample TC mid-update and return a value one count
	 * behind. Re-read until two consecutive samples agree. */
	do {
		curr_cnt = TIMx->TC;
	} while (curr_cnt != TIMx->TC);

	return curr_cnt;
}


/**
  * @brief  Configures the TIMx match event to be generate by software.
  * @param  TimerIdx: where x can be 0-17 to select the TIMx peripheral.
  * @param  me_num: specifies the match event source.
  *         This parameter can be one of the following values:
  *         @arg TIM_MATCH_EVENT_0
  *         @arg TIM_MATCH_EVENT_1
  *         @arg TIM_MATCH_EVENT_2
  *         @arg TIM_MATCH_EVENT_3
  * @param  me_val: specifies the match event counter value.
  * @note
  * @retval None
  */
__attribute__((optimize("O0")))
void RTIM_SetMatchEvent(u32 TimerIdx, u32 me_num, u32 me_val)
{
	RTIM_TypeDef *TIMx = (RTIM_TypeDef *)RTIMx[TimerIdx];

	if (TrustZone_IsSecure()) {
		TIMx = RTIMx_S[TimerIdx];
	}

	switch (me_num) {
	case TIM_MATCH_EVENT_0:
		TIMx->ME0 = TM_ME0(me_val);
		break;

	case TIM_MATCH_EVENT_1:
		TIMx->ME1 = TM_ME1(me_val);
		break;

	case TIM_MATCH_EVENT_2:
		TIMx->ME2 = TM_ME2(me_val);
		break;

	case TIM_MATCH_EVENT_3:
		TIMx->ME3 = TM_ME3(me_val);
		break;

	default:
		RTK_LOGE(TAG, "ME number should be less than 4!\r\n");
		break;
	}

}


/**
  * @brief  Enables the TIMx match event to be generate by software.
  * @param  TimerIdx: where x can be 0-17 to select the TIMx peripheral.
  * @param  me_num: specifies the match event source.
  *         This parameter can be one of the following values:
  *         @arg TIM_MATCH_EVENT_0
  *         @arg TIM_MATCH_EVENT_1
  *         @arg TIM_MATCH_EVENT_2
  *         @arg TIM_MATCH_EVENT_3
  * @param  NewState: enable / disable match event
  * @note
  * @retval None
  */
void RTIM_MatchEventCmd(u32 TimerIdx, u32 me_num, u32 NewState)
{
	RTIM_TypeDef *TIMx = (RTIM_TypeDef *)RTIMx[TimerIdx];

	if (TrustZone_IsSecure()) {
		TIMx = RTIMx_S[TimerIdx];
	}

	if (NewState == ENABLE) {
		TIMx->MECTRL |= (BIT(me_num));
	} else {
		TIMx->MECTRL &= ~(BIT(me_num));
	}
}

/**
  * @brief  Sets TIMx operation mode
  * @param  TimerIdx: where x can be 0-17 to select the TIMx peripheral.
  * @param 	op_mode: specifies timer operation mode
  * 		This parameter can be one of the following values:
  * 		@arg TIM_OP_MODE_ONE_SHOT
  * 		@arg TIM_OP_MODE_PERIODICAL
  * @retval
  */
void RTIM_SetOpMode(u32 TimerIdx, u32 op_mode)
{
	RTIM_TypeDef *TIMx = (RTIM_TypeDef *)RTIMx[TimerIdx];

	if (TrustZone_IsSecure()) {
		TIMx = RTIMx_S[TimerIdx];
	}

	if (op_mode == TIM_OP_MODE_ONE_SHOT) {
		TIMx->CTRL |= TM_BIT_MOD;
	} else if (op_mode == TIM_OP_MODE_PERIODICAL) {
		TIMx->CTRL &= ~(TM_BIT_MOD);
	} else {
		return;
	}
}

/**
  * @brief  Sets TIMx counting mode
  * @param  TimerIdx: where x can be 0-17 to select the TIMx peripheral.
  * @param 	cnt_mode: specifies timer counting mode
  * 		This parameter can be one of the following values:
  * 		@arg TIM_CNT_MODE_DOWN
  * 		@arg TIM_CNT_MODE_UP
  * @retval
  */
void RTIM_SetCntMode(u32 TimerIdx, u32 cnt_mode)
{
	RTIM_TypeDef *TIMx = (RTIM_TypeDef *)RTIMx[TimerIdx];

	if (TrustZone_IsSecure()) {
		TIMx = RTIMx_S[TimerIdx];
	}

	if (cnt_mode == TIM_CNT_MODE_DOWN) {
		TIMx->CTRL |= TM_BIT_CNT_MOD;
	} else if (cnt_mode == TIM_CNT_MODE_UP) {
		TIMx->CTRL &= ~(TM_BIT_CNT_MOD);
	} else {
		return;
	}
}

/**
  * @brief  Reset current timer counter value
  * @param  TimerIdx: where x can be 0-17 to select the TIMx peripheral.
  * @retval None
  */
void RTIM_Reset(u32 TimerIdx)
{
	RTIM_TypeDef *TIMx = (RTIM_TypeDef *)RTIMx[TimerIdx];

	if (TrustZone_IsSecure()) {
		TIMx = RTIMx_S[TimerIdx];
	}

	u16 curr_cnt = 0;

	// TIMx->PC = TM_PC(0);

	if ((TIMx->CTRL & TM_BIT_CNT_MOD) == 0) {
		// reset to 0;
		TIMx->TC = TM_TC(curr_cnt);
	} else {
		// reset to load cnt val if counting down
		curr_cnt = TM_GET_LC(TIMx->LC);
		TIMx->TC = TM_TC(curr_cnt);
	}

}

/**
  * @brief  ENABLE/DISABLE  the TIMx's interrupt bits.
  * @param  TimerIdx: where x can be 0-17 to select the TIMx peripheral.
  * @param  TIM_IT: NOT USED
  * @param NewState: ENABLE or DISABLE
  * @note
  *
  * @retval None
  */
void RTIM_INTConfig(u32 TimerIdx, u32 TIM_IT, u32 NewState)
{
	RTIM_TypeDef *TIMx = (RTIM_TypeDef *)RTIMx[TimerIdx];

	if (TrustZone_IsSecure()) {
		TIMx = RTIMx_S[TimerIdx];
	}

	/* Check the parameters */
	// assert_param(IS_TIM_ALL_TIM(TIMx));
	// assert_param(IS_TIM_IT(TIM_IT));

	if (NewState == ENABLE) {
		/* Enable the Interrupt sources */
		TIMx->CTRL |= TM_BIT_IMR;
	} else {
		/* Disable the Interrupt sources */
		TIMx->CTRL &= ~(TM_BIT_IMR);
	}

	(void)TIM_IT;
}

/**
  * @brief  Clears the TIMx's all interrupt pending bits.
  * @param  TimerIdx: where x can be 0-17 to select the TIMx peripheral.
  * @retval the pending interrupt bits that are cleared
  */
u32 RTIM_INTClear(u32 TimerIdx)
{
	// assert_param(IS_TIM_ALL_TIM(TIMx));
	RTIM_TypeDef *TIMx = (RTIM_TypeDef *)RTIMx[TimerIdx];

	if (TrustZone_IsSecure()) {
		TIMx = RTIMx_S[TimerIdx];
	}

	u32 Status = TIMx->ISR;

	/* Clear the all IT pending Bits */
	TIMx->ISR |= Status;

	return Status;
}


/**
  * @brief  Clears the TIMx's specific interrupt pending bit.
  * @param  TimerIdx: where x can be 0-17 to select the TIMx peripheral.
  * @param  TIM_IT: specifies the pending bit to clear.
  *   This parameter can be any combination of the following values:
  *     @arg TIM_IT_TIMEOUT: TIM update Interrupt source
  *     @arg TIM_IT_ME0: Counter value match event0 interrupt.
  *     @arg TIM_IT_ME1: Counter value match event1 interrupt.
  *     @arg TIM_IT_ME2: Counter value match event2 interrupt.
  *     @arg TIM_IT_ME3: Counter value match event3 interrupt.
  * @note
  *
  * @retval None
  */
void RTIM_INTClearPendingBit(u32 TimerIdx, u32 TIM_IT)
{
	RTIM_TypeDef *TIMx = (RTIM_TypeDef *)RTIMx[TimerIdx];

	if (TrustZone_IsSecure()) {
		TIMx = RTIMx_S[TimerIdx];
	}

	/* Check the parameters */
	// assert_param(IS_TIM_ALL_TIM(TIMx));
	assert_param(IS_TIM_IT(TIM_IT));

	/* Clear the IT pending Bit */
	TIMx->ISR = TIM_IT;

}


/**
  * @brief  Checks whether the TIM interrupt has occurred or not.
  * @param  TimerIdx: where x can be 0-17 to select the TIMx peripheral.
  * @param  TIM_IT: specifies the TIM interrupt source to check.
  *      This parameter can be one of the following values:
  *     @arg TIM_IT_TIMEOUT: TIM update Interrupt source (TIM0-8)
  *     @arg TIM_IT_ME0: Counter value match event0 interrupt.
  *     @arg TIM_IT_ME1: Counter value match event1 interrupt.
  *     @arg TIM_IT_ME2: Counter value match event2 interrupt.
  *     @arg TIM_IT_ME3: Counter value match event3 interrupt.
  * @note
  *
  * @retval TRUE: The specified TIM interrupt has occurred
  *         FALSE: The specified TIM interrupt has not occurred.
  */
u32 RTIM_GetINTStatus(u32 TimerIdx, u32 TIM_IT)
{
	u32 bitstatus = FALSE;
	u32 itstatus = 0x0;
	RTIM_TypeDef *TIMx = (RTIM_TypeDef *)RTIMx[TimerIdx];

	if (TrustZone_IsSecure()) {
		TIMx = RTIMx_S[TimerIdx];
	}

	/* Check the parameters */
	// assert_param(IS_TIM_ALL_TIM(TIMx));
	assert_param(IS_TIM_IT(TIM_IT));

	itstatus = TIMx->ISR & TIM_IT;

	if ((itstatus != 0)) {
		bitstatus = TRUE;
	} else {
		bitstatus = FALSE;
	}

	return bitstatus;
}

/**
  * @brief  Registers TIMx Timeout interrupt handler
  * @param  TimerIdx: select timer index 0-17
  * @param  irq_func: ISR function handler. The type IRQ_FUN is defined
  *         as follows: typedef u32 (*IRQ_FUN)(void *Data);
  * @param  irq_data: parameters of ISR function.
  * @retval  none
  */
void RTIM_RegTimeoutIRQ(u32 TimerIdx, IRQ_FUN irq_func, void *irq_data)
{
	TM_TimeoutIrqHandler[TimerIdx] = irq_func;
	TM_TimeoutIrqData[TimerIdx] = irq_data;
}


/**
  * @brief  Unregisters TIMx Timeout interrupt handler
  * @param  TimerIdx: select 0-17
  * @retval  none
  */
void RTIM_UnRegTimeoutIRQ(u32 TimerIdx)
{
	TM_TimeoutIrqHandler[TimerIdx] = NULL;
	TM_TimeoutIrqData[TimerIdx] = NULL;
}


/**
  * @brief  Registers TIMx Matchevent handler
  * @param  TimerIdx: select timer index 0-17
  * @param 	me_num: select matchevent number 0-3
  * @param  irq_func: ISR function handler. The type IRQ_FUN is defined
  *         as follows: typedef u32 (*IRQ_FUN)(void *Data);
  * @param  irq_data: parameters of ISR function.
  * @retval  none
  */
void RTIM_RegMEIRQ(u32 TimerIdx, u32 me_num, IRQ_FUN irq_func, void *irq_data)
{
	TM_MatchEventIrqHandler[(TimerIdx * 4) + me_num] = irq_func;
	TM_MatchEventIrqData[(TimerIdx * 4) + me_num] = irq_data;
}


/**
  * @brief  Registers TIMx Matchevent handler
  * @param  TimerIdx: select timer index 0-17
  * @param 	me_num: select matchevent number 0-3
  * @retval  none
  */
void RTIM_UnRegMEIRQ(u32 TimerIdx, u32 me_num)
{
	TM_MatchEventIrqHandler[(TimerIdx * 4) + me_num] = NULL;
	TM_MatchEventIrqData[(TimerIdx * 4) + me_num] = NULL;
}


void RTIM_IRQHandler(u8 tg_num, u8 max_tm, u8 tm_start_idx)
{
	TG_TypeDef *TGx = (TG_TypeDef *) GTIMERx[tg_num];

	if (TrustZone_IsSecure()) {
		TGx = GTIMERx_S[tg_num];
		// TIMx = RTIMx_S[TimerIdx];
	}

	u32 tg_ists;
	u32 tm_isr;

	u8 tm_num;
	u8 i, j;

	RTIM_TypeDef *TMx;

	tg_ists = TGx->ISTS;

	for (i = 0; i < max_tm; i++) {

		tm_num = tm_start_idx + i;

		if (tg_ists & (1 << i)) {

			if (TrustZone_IsSecure()) {
				TMx = RTIMx_S[tm_num];
			} else {
				TMx = RTIMx[tm_num];
			}

			// TMx = RTIMx[tm_num];

			tm_isr = TMx->ISR;
			TMx->ISR |= tm_isr;      // clr tm isr status

			tg_ists = TGx->ISTS;

			if (tm_isr & TM_BIT_TIMEOUT) {

				if (TMx->CTRL & TM_BIT_MOD) {
					TMx->CTRL &= ~(TM_BIT_EN);
				}

				if (TM_TimeoutIrqHandler[tm_num] != NULL) {
					TM_TimeoutIrqHandler[tm_num](TM_TimeoutIrqData[tm_num]);
				}

				tm_isr &= ~(TM_BIT_TIMEOUT);

				if (tm_isr == 0) {
					continue;
				}
			}

			for (j = 0; j < TIM_MAX_MATCH_EVENT_CNT; j++) {
				if (tm_isr & (1 << (j + 1))) {
					if (TM_MatchEventIrqHandler[j + (tm_num * 4)] != NULL) {
						TM_MatchEventIrqHandler[j + (tm_num * 4)](TM_MatchEventIrqData[j + (tm_num * 4)]);
					}

					tm_isr &= ~(1 << (j + 1));

					if (tm_isr == 0) {
						break;
					}
				}
			}
		}

	}

}

u32 RTIM_GTIMER0_IRQHandler(void *Data)
{
	(void)Data;
	RTIM_IRQHandler(0, 8, 0);
	return 0;
}

u32 RTIM_GTIMER1_IRQHandler(void *Data)
{
	(void)Data;
	RTIM_IRQHandler(1, 1, 8);
	return 0;
}

u32 RTIM_GTIMER2_IRQHandler(void *Data)
{
	(void)Data;
	RTIM_IRQHandler(2, 8, 9);
	return 0;
}

u32 RTIM_GTIMER3_IRQHandler(void *Data)
{
	(void)Data;
	RTIM_IRQHandler(3, 1, 17);
	return 0;
}


/**
  * @brief  Get Timer Group interrupt pending status
  * @param  TGx: Timer Group peripheral selected: 0-3
  * @note 	Each bit indicates the interrupt pending status of corresponding timer
  * @retval  interrupt pending status
  */
u8 RTIM_GTIMER_GetISTS(TG_TypeDef *TGx)
{
	return TG_GET_ISTS(TGx->ISTS);
}


/**
  * @brief  Get Timer Group RAW interrupt pending status
  * @param  TGx: Timer Group peripheral selected: 0-3
  * @note 	Each bit indicates the RAW interrupt pending status of corresponding timer
  * @retval  RAW interrupt pending status
  */
u8 RTIM_GTIMER_GetRawISTS(TG_TypeDef *TGx)
{
	return TG_GET_RISTS(TGx->RAW_ISTS);
}

/**
  * @brief  get current counter value of timer indirectly
  * @param  TGx: Timer Group peripheral selected: GTIMERx
  * @param 	tm_num: The timer index (0-7) in a GTIMERx, to select the timer for indirect read.
  * @note 	indirect read timer counter value
  * @retval  Current counter value of the specified timer by register tg_tsel.
  */
u32 RTIM_GTIMER_IndirectRead(TG_TypeDef *TGx, u32 tm_num)
{
	u32 val = 0;

	val |= TG_TSEL(tm_num);
	val |= TG_BIT_POLL;

	// if using 40MHz
	// val |= TG_BIT_SYNC_MODE;

	TGx->TSEL = val;

	while ((TGx->TSEL & TG_BIT_POLL) != 0) {

	}

	return TGx->TC;
}


/**
  * @brief  Initiallized a Timer Group
  * @param  TimerGrp: Timer Group peripheral selected: 0~3
  * @note 	To initialize a timer group and register hooked interrupt for timer group
  * @note 	Has to be called first before using any timer
  * @retval
  */
__attribute__((optimize("O0")))
void RTIM_GTIMER_Init(u32 TimerGrp)
{
	// DiagPrintf("gtimer init\r\n");

	/* Initialized TimerIRQHandler */

	if (TG_init_status[TimerGrp] != 0) {
		return;
	}

	RCC_PeriphClockCmd(APBPeriph_PON_TIMERx[TimerGrp], APBPeriph_PON_TIMERx_CLOCK[TimerGrp], ENABLE);

	TG_init_status[TimerGrp] = 1;

	TG_TypeDef *TGx = (TG_TypeDef *)GTIMERx[TimerGrp];

	if (TrustZone_IsSecure()) {
		TGx = GTIMERx_S[TimerGrp];
	}

	u8 max_tm;
	u32 tg_ists;
	u32 tm_isr;

	if (TimerGrp == 0 || TimerGrp == 2) {
		max_tm = 8;
	} else {
		max_tm = 1;
	}

	tg_ists = TGx->ISTS;

	if (tg_ists != 0) {

		for (int i = 0; i < max_tm; i++) {
			if ((tg_ists & (BIT(i))) != 0) {

				RTIM_TypeDef *TMx = (RTIM_TypeDef *)((u32)TGx + ((i + 1) * 0x40));

				tm_isr = TMx->ISR;
				TMx->ISR = tm_isr;

			}
		}
	}

	if (TimerGrp == TIM_TG0) {
		InterruptRegister((IRQ_FUN)RTIM_GTIMER0_IRQHandler, GTIMERx_irq[TimerGrp], (u32)TimerGrp, INT_PRI_MIDDLE);
		InterruptEn(GTIMER0_IRQ, INT_PRI_MIDDLE);
	} else if (TimerGrp == TIM_TG1) {
		InterruptRegister((IRQ_FUN)RTIM_GTIMER1_IRQHandler, GTIMERx_irq[TimerGrp], (u32)TimerGrp, INT_PRI_MIDDLE);
		InterruptEn(GTIMER1_IRQ, INT_PRI_MIDDLE);
	} else if (TimerGrp == TIM_TG2) {
		InterruptRegister((IRQ_FUN)RTIM_GTIMER2_IRQHandler, GTIMERx_irq[TimerGrp], (u32)TimerGrp, INT_PRI_MIDDLE);
		InterruptEn(GTIMER2_IRQ, INT_PRI_MIDDLE);
	} else if (TimerGrp == TIM_TG3) {
		InterruptRegister((IRQ_FUN)RTIM_GTIMER3_IRQHandler, GTIMERx_irq[TimerGrp], (u32)TimerGrp, INT_PRI_MIDDLE);
		InterruptEn(GTIMER3_IRQ, INT_PRI_MIDDLE);
	} else {

	}
}


/**
  * @brief  De-initiallized a Timer Group
  * @param  TimerGrp: Timer Group selected: 0-3
  * @note 	To De-initialize a timer group and unregister hooked interrupt for timer group
  * @retval
  */
void RTIM_GTIMER_DeInit(u32 TimerGrp)
{
	switch (TimerGrp) {
	case TIM_TG0:
		InterruptDis(GTIMER0_IRQ);
		InterruptUnRegister(GTIMER0_IRQ);
		break;

	case TIM_TG1:
		InterruptDis(GTIMER1_IRQ);
		InterruptUnRegister(GTIMER1_IRQ);
		break;

	case TIM_TG2:
		InterruptDis(GTIMER2_IRQ);
		InterruptUnRegister(GTIMER2_IRQ);
		break;

	case TIM_TG3:
		InterruptDis(GTIMER3_IRQ);
		InterruptUnRegister(GTIMER3_IRQ);
		break;

	default:
		break;
	}

	TG_init_status[TimerGrp] = 0;
	RCC_PeriphClockCmd(APBPeriph_PON_TIMERx[TimerGrp], APBPeriph_PON_TIMERx_CLOCK[TimerGrp], DISABLE);
}


/**
  * @brief  Initializes the parameters in the TIM_InitStruct with its default value
  * @param  TIM_InitStruct: pointer to a RTIM_TimeBaseInitTypeDef structure which will be
  *         initialized.
  * @retval None
  */
void RTIM_TimeBaseStructInit(RTIM_TimeBaseInitTypeDef *TIM_InitStruct)
{
	TIM_InitStruct->TIM_TimerIdx = 0x0;
	TIM_InitStruct->TIM_TimerGrp = 0x0;
	TIM_InitStruct->TIM_Prescaler = 0x00;
	TIM_InitStruct->TIM_LoadCnt = 0x0;
	TIM_InitStruct->TIM_CntMode = TIM_CNT_MODE_UP;
	TIM_InitStruct->TIM_OpMode = TIM_OP_MODE_PERIODICAL;
	TIM_InitStruct->TIM_IntMask = DISABLE;
}


/**
  * @brief  Initializes the TIMx Time Base Unit peripheral according to
  *         the specified parameters in the TIM_TimeBaseInitStruct.
  * @param  TimerGrp: select gtimer group
  * @param  TIM_InitStruct: pointer to a TIM_TimeBaseInitTypeDef
  *         structure that contains the configuration information for the
  *         specified TIM peripheral.
  * @note 	TimerGrp = 0 (TimerIdx 0 ~ 7) ADC & PWM can only use timers in gtimer0
  * @note 	TimerGrp = 1 (TimerIdx 8)
  * @note 	TimerGrp = 2 (TimerIdx 9 ~ 16)
  * @note 	TimerGrp = 3 (TimerIdx 17)
  * @retval  none
  */
void RTIM_TimeBaseInit(u32 TimerGrp, RTIM_TimeBaseInitTypeDef *TIM_InitStruct)
{

	u8 tm_num;

	if (TM_inUsed[TimerGrp] == 0x0) {
		RTIM_GTIMER_Init(TimerGrp);
	}

	if (TIM_InitStruct->TIM_TimerIdx < 8) {
		tm_num = TIM_InitStruct->TIM_TimerIdx;
	} else if (TIM_InitStruct->TIM_TimerIdx == 8) {
		tm_num = 0;
	} else if (TIM_InitStruct->TIM_TimerIdx < 17) {
		tm_num = TIM_InitStruct->TIM_TimerIdx - 9;
	} else if (TIM_InitStruct->TIM_TimerIdx == 17) {
		tm_num = 0;
	} else {
		return;
	}

	TM_inUsed[TimerGrp] |= BIT(tm_num);

	RTIM_Cmd(TIM_InitStruct->TIM_TimerIdx, DISABLE);
	RTIM_SetPrescaler(TIM_InitStruct->TIM_TimerIdx, TIM_InitStruct->TIM_Prescaler);
	RTIM_SetLoadCnt(TIM_InitStruct->TIM_TimerIdx, TIM_InitStruct->TIM_LoadCnt);
	RTIM_SetCntMode(TIM_InitStruct->TIM_TimerIdx, TIM_InitStruct->TIM_CntMode);
	RTIM_SetOpMode(TIM_InitStruct->TIM_TimerIdx, TIM_InitStruct->TIM_OpMode);
	RTIM_INTConfig(TIM_InitStruct->TIM_TimerIdx, 0, TIM_InitStruct->TIM_IntMask);

	// TIMx_IRQHandler[TIM_InitStruct->TIM_TimerIdx] = UserCB;
	// TIMx_IrqData[TIM_InitStruct->TIM_TimerIdx] = UserCBData;

	// DiagPrintf("timebase init done %x\r\n", TIM_InitStruct->TIM_TimerIdx);

}

/**
  * @brief  DeInit the specified TIM peripheral,
  *	including disable TIM, close and clear interrupt.
  * @param  TimerIdx: where x can be 0-17 to select the TIMx peripheral.
  * @retval None
  */
void RTIM_DeInit(u32 TimerIdx)
{
	/* Check the parameters */
	// assert_param(IS_TIM_ALL_TIM(TIMx));

	u8 tg_num;
	u8 tm_num;

	/* disable */
	RTIM_Cmd(TimerIdx, DISABLE);

	/* close and clear all interrupt */
	RTIM_INTConfig(TimerIdx, 0, DISABLE);
	RTIM_INTClear(TimerIdx);

	if (TimerIdx < 8) {
		tg_num = TIM_TG0;
		tm_num = TimerIdx;
	} else if (TimerIdx == 8) {
		tg_num = TIM_TG1;
		tm_num = 0;
	} else if (TimerIdx < 17) {
		tg_num = TIM_TG2;
		tm_num = TimerIdx - 9;
	} else if (TimerIdx == 17) {
		tg_num = TIM_TG3;
		tm_num = 0;
	} else {
		return;
	}

	TM_inUsed[tg_num] &= ~(BIT(tm_num));

	if (TM_inUsed[tg_num] == 0x0) {
		RTIM_GTIMER_DeInit(tg_num);
	}

}

/**
  * @brief  Start one shot counter for specific TimerIdx
  * @param TimerGrp: can be 0-3 to select gtimer group
  * @param  TIM_InitStruct: pointer to a TIM_TimeBaseInitTypeDef
  *         structure that contains the configuration information for the
  *         specified TIM peripheral.
  * @param irq_func: timeout interrupt function to be called
  * @param irq_data: timeout interrupt data to be used
  * @note 	TimerGrp = 0 (TimerIdx 0 ~ 7) ADC & PWM can only use timers in gtimer0
  * @note 	TimerGrp = 1 (TimerIdx 8)
  * @note 	TimerGrp = 2 (TimerIdx 9 ~ 16)
  * @note 	TimerGrp = 3 (TimerIdx 17)
  * @retval None
  */
__attribute__((optimize("O0")))
void RTIM_StartOneShot(u32 TimerGrp, RTIM_TimeBaseInitTypeDef *TIM_InitStruct, IRQ_FUN irq_func, void *irq_data)
{
	TIM_InitStruct->TIM_OpMode = TIM_OP_MODE_ONE_SHOT;

	RTIM_RegTimeoutIRQ(TIM_InitStruct->TIM_TimerIdx, irq_func, irq_data);

	RTIM_TimeBaseInit(TimerGrp, TIM_InitStruct);
	RTIM_Cmd(TIM_InitStruct->TIM_TimerIdx, ENABLE);

}


/**
  * @brief  Start periodic counter for specific TimerIdx
  * @param TimerGrp: can be 0-3 to select gtimer group
  * @param  TIM_InitStruct: pointer to a TIM_TimeBaseInitTypeDef
  *         structure that contains the configuration information for the
  *         specified TIM peripheral.
  * @param irq_func: timeout interrupt function to be called
  * @param irq_data: timeout interrupt data to be used
  * @note 	TimerGrp = 0 (TimerIdx 0 ~ 7) ADC & PWM can only use timers in gtimer0
  * @note 	TimerGrp = 1 (TimerIdx 8)
  * @note 	TimerGrp = 2 (TimerIdx 9 ~ 16)
  * @note 	TimerGrp = 3 (TimerIdx 17)
  * @retval None
  */
__attribute__((optimize("O0")))
void RTIM_StartPeriodic(u32 TimerGrp, RTIM_TimeBaseInitTypeDef *TIM_InitStruct, IRQ_FUN irq_func, void *irq_data)
{
	TIM_InitStruct->TIM_OpMode = TIM_OP_MODE_PERIODICAL;

	RTIM_RegTimeoutIRQ(TIM_InitStruct->TIM_TimerIdx, irq_func, irq_data);

	RTIM_TimeBaseInit(TimerGrp, TIM_InitStruct);
	RTIM_Cmd(TIM_InitStruct->TIM_TimerIdx, ENABLE);
}

/**
  * @brief  Start matchevent counter for specific TimerIdx
  * @param TimerGrp: can be 0-3 to select gtimer group
  * @param  TIM_InitStruct: pointer to a TIM_TimeBaseInitTypeDef
  *         structure that contains the configuration information for the
  *         specified TIM peripheral.
  * @param me_num: can be 0-3 to select matchevent counter
  * @param time_ticks: matchevent counter timeout duration in ticks
  * @param irq_func: matchevent timeout interrupt function to be called
  * @param irq_data: matchevent timeout interrupt data to be used
  * @note 	TimerGrp = 0 (TimerIdx 0 ~ 7) ADC & PWM can only use timers in gtimer0
  * @note 	TimerGrp = 1 (TimerIdx 8)
  * @note 	TimerGrp = 2 (TimerIdx 9 ~ 16)
  * @note 	TimerGrp = 3 (TimerIdx 17)
  * @retval None
  */
__attribute__((optimize("O0")))
void RTIM_EnableAlarm(u32 TimerIdx, u32 me_num, u32 time_ticks, IRQ_FUN irq_func, void *irq_data)
{
	RTIM_RegMEIRQ(TimerIdx, me_num, irq_func, irq_data);
	RTIM_SetMatchEvent(TimerIdx, me_num, time_ticks);
	RTIM_MatchEventCmd(TimerIdx, me_num, ENABLE);
}



#if 0
/**
  * @brief  Sets the TIMx Autoreload Register(TIMx_ARR) value to change period
  * @param  TimerIdx: where x can be 0-8 to select the TIM peripheral.
  * @param  Autoreload: specifies the Autoreload register new value.
  *			To TIM0-3, this value can be a number in 0~0xFFFFFFFF range.
  *			To TIM4-8, this value can be a number in 0~0xFFFF range.
  * @retval None
  */
void RTIM_ChangePeriodImmediate(u32 TimerIdx, u32 Autoreload)
{
	// not used in pro3
}


void RTIM_ChangePeriod_us(u32 TimerIdx, RTIM_TimeBaseInitTypeDef *TIM_InitStruct, u32 period_us)
{
	u32 one_us = (30 - 1);      // using 30MHz

	RTIM_Cmd(TimerIdx, DISABLE);

	TIM_InitStruct->TIM_LoadCnt = period_us * one_us;

	RTIM_SetLoadCnt(TimerIdx, TIM_InitStruct->TIM_LoadCnt);
	RTIM_Cmd(TimerIdx, ENABLE);

}

/**
  * @brief  Sets the TIMx Autoreload Register(TIMx_ARR) value to change period with protection
  * @param  TimerIdx: where x can be 0-8 to select the TIM peripheral.
  * @param  load_cnt: specifies the load count register new value.
  * @note disable tm, set new load cnt, reset current timer & prescaler counter value, enable tm
  * @retval None
  */
void RTIM_ChangePeriod(u32 TimerIdx, u32 load_cnt)
{
	// assert_param(IS_TIM_ALL_TIM(TIMx));

	// disable tm
	// set new period
	// reset current timer counter and current prescaler counter value
	// enable tm

	RTIM_TypeDef *TIMx = (RTIM_TypeDef *)RTIMx[TimerIdx];

	RTIM_Cmd(TimerIdx, DISABLE);

	TIMx->LC &= ~(TM_MASK_LC);
	TIMx->LC |= TM_LC(load_cnt);

	RTIM_Reset(TimerIdx);
	RTIM_Cmd(TimerIdx, ENABLE);
}


/**
  * @brief  Sets the TIMx Autoreload Register(TIMx_ARR) value to change period(us) with protection.
  * @param  TimerIdx: where x can be 0-8 to select the TIM peripheral.
  * @param  tim_idx: where x can be 0-8.
  * @param  period_us: Period to be set in microseconds.
  * @retval None
  */
void RTIM_ChangePeriod_us(u32 TimerIdx, u8 tim_idx, u32 period_us)
{
	u8 clk_source = 0;
	u32 temp = 0;;
	// if (IS_TIM_BASIC_TIM(TIMx)) {
	// 	switch (tim_idx) {
	// 	case 0:
	// 		clk_source = RCC_PeriphClockSourceGet(LTIM0);
	// 		break;
	// 	case 1:
	// 		clk_source = RCC_PeriphClockSourceGet(LTIM1);
	// 		break;
	// 	case 2:
	// 		clk_source = RCC_PeriphClockSourceGet(LTIM2);
	// 		break;
	// 	case 3:
	// 		clk_source = RCC_PeriphClockSourceGet(LTIM3);
	// 		break;
	// 	}

	// 	if (!clk_source) {
	// 		temp = (u32)((float)period_us / 1000000 * 32768) - 1;
	// 	} else {
	// 		temp = (u32)period_us - 1;
	// 	}
	// } else {
	// 	temp = (u32)((float)(period_us / 1000000 * 40000000 / TIMx->PSC));
	// }

	// RTIM_ChangePeriod(TIMx, temp);
}

// added
/**
  * @brief  Checks whether the TIM interrupt has occurred or not.
  * @param  tm_num: where x can be 0-8 to select the TIM peripheral.
  * @param  TIM_IT: specifies the TIM interrupt source to check.
  *      This parameter can be one of the following values:
  *     @arg TIM_IT_TIMEOUT: TIM update Interrupt source (TIM0-8)
  *     @arg TIM_IT_ME0: Counter value match event0 interrupt.
  *     @arg TIM_IT_ME1: Counter value match event1 interrupt.
  *     @arg TIM_IT_ME2: Counter value match event2 interrupt.
  *     @arg TIM_IT_ME3: Counter value match event3 interrupt.
  * @param  irq_func: ISR function handler. The type IRQ_FUN is defined
  *         as follows: typedef u32 (*IRQ_FUN)(void *Data);
  * @param  irq_data: parameters of ISR function.
  * @note
  *
  * @retval TRUE: The specified TIM interrupt has occurred
  *         FALSE: The specified TIM interrupt has not occurred.
  */
void RTIM_UserINTHandler(u32 tm_num, u32 TIM_IT, IRQ_FUN irq_func, void *irq_data)
{
	u32 me_num;

	if (irq_func == NULL) {
		return;
	}

	if (TIM_IT == TIM_IT_TIMEOUT) {
		DiagPrintf("reg timeout irq\r\n");
		TM_TimeoutIrqHandler[tm_num] = irq_func;
		TM_TimeoutIrqData[tm_num] = irq_data;
		return;
	}


	DiagPrintf("reg me irq\r\n");

	if (TIM_IT == TIM_IT_ME0) {
		me_num = 0;
	} else if (TIM_IT == TIM_IT_ME1) {
		me_num = 1;
	} else if (TIM_IT == TIM_IT_ME2) {
		me_num = 2;
	} else {
		me_num = 3;
	}

	TM_MatchEventIrqHandler[tm_num * 4 + me_num] = irq_func;
	TM_MatchEventIrqData[tm_num * 4 + me_num] = irq_data;
}

/**
  * @brief  Checks whether the specified TIM flag is set or not.
  * @param  TimerIdx: where x can be 0-8 to select the TIM peripheral.
  * @param  TIM_FLAG: specifies the TIM interrupt source to check.
  * @note
  *   - TIM0-3 can generate only an TIM_IT_Update interrupt.
  *   - TIM8 can generate TIM_IT_Update and TIM_IT_CC0 interrupt.
  *   - TIM4-7 can generate all interrupts.
  * @retval  TRUE: the specified TIM flag is set
  *          FALSE: the specified TIM flag is not set.
  */
u32 RTIM_GetFlagStatus(u32 TimerIdx, u32 TIM_FLAG)
{
	u32 bitstatus = FALSE;
	RTIM_TypeDef *TIMx = (RTIM_TypeDef *)RTIMx[TimerIdx];

	// not used in pro2

	return bitstatus;
}
#endif
