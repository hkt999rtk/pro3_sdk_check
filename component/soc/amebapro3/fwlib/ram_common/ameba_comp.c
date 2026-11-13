#include "ameba_soc.h"
#include "ameba_comp.h"

static const char *TAG = "AON_CMP";

u32 CMP_WaitReady(void)
{
	AON_TypeDef *AON = (AON_TypeDef *)AON_BASE;

	u32 i;

	for (i = 0; i < 1000000; i++) {
		if ((AON->AON_LSFIF_CMD & AON_BIT_AON_LSFIF_POLL) == 0) {
			return TRUE;
		} else {
			DelayMs(1);
		}
	}

	RTK_LOGW(TAG, "AON CMP indirect RW not done\r\n");

	return FALSE;
}

void CMP_Write(u16 addr, u32 data)
{
	AON_TypeDef *AON = (AON_TypeDef *)AON_BASE;

	AON->AON_LSFIF_CMD = AON_LSF_SEL(0x3);

	AON->AON_LSFIF_RWD = data;
	AON->AON_LSFIF_CMD |= AON_MASK_AON_LSFIF_WE;
	AON->AON_LSFIF_CMD |= AON_BIT_AON_LSFIF_WR;
	AON->AON_LSFIF_CMD |= AON_AON_LSFIF_AD(addr);
	AON->AON_LSFIF_CMD |= AON_BIT_AON_LSFIF_POLL;

	CMP_WaitReady();
}

u32 CMP_Read(u16 addr)
{
	AON_TypeDef *AON = (AON_TypeDef *)AON_BASE;
	u32 data;

	AON->AON_LSFIF_CMD = AON_LSF_SEL(0x3);

	AON->AON_LSFIF_CMD &= ~(AON_MASK_AON_LSFIF_WE);
	AON->AON_LSFIF_CMD &= ~(AON_BIT_AON_LSFIF_WR);
	AON->AON_LSFIF_CMD |= AON_AON_LSFIF_AD(addr);
	AON->AON_LSFIF_CMD |= AON_BIT_AON_LSFIF_POLL;

	CMP_WaitReady();

	data = AON->AON_LSFIF_RWD;

	return data;
}

/**
  * @brief	 Initializes the parameters in the CMP_InitStruct with default value.
  * @param CMP_InitStruct: pointer to a CMP_InitTypeDef structure that contains
  *         the configuration information for the Comparator peripheral.
  * @retval None
  */
void CMP_StructInit(CMP_InitTypeDef *CMP_InitStruct)
{
	u8 i;

	for (i = 0; i < COMP_CH_NUM; i++) {
		CMP_InitStruct->CMP_ChanCtrl[i].CMP_ChIndex = i;
		CMP_InitStruct->CMP_ChanCtrl[i].CMP_Ref0 = 0;		// Vref0 = 0.1*0=0v
		CMP_InitStruct->CMP_ChanCtrl[i].CMP_Ref1 = 0x1F;		//  Vref1 = 0.1*31=3.1v
		CMP_InitStruct->CMP_ChanCtrl[i].CMP_WakeType = COMP_WK_NONE;
		CMP_InitStruct->CMP_ChanCtrl[i].CMP_WakeSysCtrl = COMP_WITHIN_REF0_AND_REF1;
	}
}

/**
  * @brief	 Initializes the Comparator according to the specified parameters in CMP_InitStruct.
  * @param  CMP_InitStruct: pointer to a CMP_InitTypeDef structure that contains
  *         the configuration for the Comparator peripheral.
  * @retval None
  */
void CMP_Init(CMP_InitTypeDef *CMP_InitStruct)
{
	/*
	    ctrl:   sts
	    0:      00
	    1:      11
	    2:      10
	    3:      00 or 11
	*/

	u8 i, ch_idx;
	u32 tmp = 0;
	u32 chsw_val = 0;
	CMP_CHTypeDef *compChan = CMP_InitStruct->CMP_ChanCtrl;
	u8 otp_data;

	AON_BASE->AON_CLK_CTRL &= ~(AON_BIT_AON_COMP_INT_CLK_SEL);

	OTP_Read8(0x6a8, &otp_data);

	if (otp_data != 0xFF) {
		RTK_LOGI(TAG, "OTP_data = 0x%x\r\n", otp_data);
		BANDGAP_BASE->AON_SD_CTRL0 &= ~(BANDGAP_MASK_SD_VOS_H_2_0 | BANDGAP_MASK_SD_VOS_L_2_0);
		BANDGAP_BASE->AON_SD_CTRL0 |= ((otp_data & 0x7) << 15);
		BANDGAP_BASE->AON_SD_CTRL0 |= ((otp_data & 0x70) << 8);
	}

	BANDGAP_BASE->AON_SD_CTRL0 |= BANDGAP_BIT_POW_SD;
	CMP_Write(Cmp_Wk_Shut_Cnt, 0x4020404);

	CMP_Disable();

	for (i = 0; i < COMP_CH_NUM; i++) {

		u32 val = 0;

		ch_idx = compChan[i].CMP_ChIndex;

		chsw_val |= (u32)(ch_idx << COMP_SHIFT_ITEMSW(i));

		val = (u32)(COMP_REF0(compChan[i].CMP_Ref0) | COMP_REF1(compChan[i].CMP_Ref1));

		CMP_Write(Cmp_Ref_Ch0 + (ch_idx * 4), val);

		if (compChan[i].CMP_WakeType & COMP_WK_SYS) {

			tmp |= ((compChan[i].CMP_WakeSysCtrl << COMP_SHIFT_WK_SYS_CTRL(ch_idx)) | (COMP_WK_SYS_EN(ch_idx)));

			CMP_Write(Cmp_Intr_Ctrl, tmp);
		}
	}

	CMP_Write(Cmp_Itemsw_List, chsw_val);

	// comparator->COMP_LPSD |= COMP_BIT_LPSD_1;
	// comparator->COMP_AUTO_SHUT = COMP_BIT_AUTO_SHUT;
}

void CMP_Deinit(void)
{
	BANDGAP_BASE->AON_SD_CTRL0 &= ~(BANDGAP_BIT_POW_SD);

	CMP_Write(Cmp_Enable, 0x0);
}

/**
  * @brief  Enable or Disable the Comparator peripheral.
  * @param  NewState: new state of the Comparator peripheral.
  *   			This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
void CMP_Disable(void)
{
	CMP_Write(Cmp_Enable, 0);
}

/**
  * @brief  Indicate the Comparator is busy or not .
  * @param  None
  * @retval   busy status value:
  *           - 1: Busy
  *           - 0: Not Busy.
  */
u32 CMP_Busy(void)
{
	u32 val = CMP_Read(Cmp_Busy);

	if (val & COMP_BIT_BUSY) {
		return 1;
	} else {
		return 0;
	}
}

/**
  * @brief  Get Comparator Intr Status Register .
  * @param  None
  * @retval Current Comparator Intr Status Register .
  */
u32 CMP_GetISR(void)
{
	return CMP_Read(Cmp_Intr_Sts);
}

/**
  * @brief  Get Comparator Intr Raw Status Register .
  * @param  None
  * @retval Current Comparator Intr Raw Status Register .
  */
u32 CMP_GetRawISR(void)
{
	return CMP_Read(Cmp_Intr_Raw_Sts);
}

/**
  * @brief  Clear pending bit in Comparator Intr Status Register .
  * @param  Cmp_IT Pending Interrupt bits, can be one of the following value
  *     @arg TODO
  * @retval   None
  */
void CMP_INTClearPendingBit(u32 Cmp_IT)
{
	CMP_Write(Cmp_Intr_Sts, Cmp_IT);
}

/**
  * @brief  Get the comparison result .
  * @param  channel : the Comparator channel index
  * @retval  The comparison result of specified channel index. The return value can be:
  *		00: when Vin < Vref0
  *     11: when Vin >= Vref1
  *		10: when Vref0 < Vin < Vref1
  *		11 or 00: when Vin < Vref0 & Vref1 < Vin
  */
u32 CMP_GetCompStatus(u8 channel)
{
	u32 val = CMP_Read(Cmp_Ch_Sts);

	val = (val & COMP_MASK_CH_STS(channel)) >> COMP_SHIFT_CH_STS(channel);

	return val;
}

/**
  * @brief  These reflects the last used item.
  * @param  None.
  * @retval  The last used Item.
  * @note The last_item will be round robin from itemsw_0 to itemsw_3
  */
u32 CMP_GetLastChan(void)
{
	return CMP_Read(Cmp_Last_Item);
}

/**
  * @brief  Reset the channel switch to default state.
  * @param  None
  * @retval  None
  * @note   Once this bit is set, the auto/manual channel switch goes to the first item as a default state.
  *         This control bit will trigger from 0 to 1.
  *         0: Do not reset conversion list to default state.
  *         1: Force last_item to itemsw_0.
  */
void CMP_ResetCSwList(void)
{
	u32 val = 0;

	val |= COMP_BIT_AUTO_SW_EN;
	CMP_Write(Cmp_Rst_List, val);

	val &= ~(COMP_BIT_AUTO_SW_EN);
	CMP_Write(Cmp_Rst_List, val);
}

/**
  * @brief  Enable or disable analog module and mux automatically after the analog conversion is done.
  * @param  NewState: Which can be one of the following values:
  *			@arg ENABLE: The analog module and mux will be disabled automatically after the analog conversion is done.
  *			@arg DISABLE: The analog module and mux will NOT be disabled.The comparator module is always enabled.
  * @retval  None.
  */
void CMP_AutoShutCmd(u32 NewState)
{
	u32 val = CMP_Read(Cmp_Auto_Shut);

	if (NewState != DISABLE) {
		val |= COMP_BIT_AUTO_SHUT;
	} else {
		val &= ~(COMP_BIT_AUTO_SHUT);
	}

	CMP_Write(Cmp_Auto_Shut, val);
}

/**
  * @brief  Controls the automatic channel swtich enabled or disabled in Comparator Automatic Mode.
  * @param  NewState: can be one of the following value:
  *			@arg ENABLE: Enable the automatic channel switch.
  *				When setting this bit, an automatic channel switch starts from the first channel in the channel switch list.
  *			@arg DISABLE:  Disable the automatic channel switch.
  *				If an automatic channel switch is in progess, writing 0 will terminate the automatic channel switch.
  * @retval  None.
  */
void CMP_AutoSwCmd(u32 NewState)
{
	if (NewState != DISABLE) {
		CMP_Write(Cmp_Enable, (COMP_BIT_TRIG_EN | COMP_BIT_ENABLE));
		CMP_Write(Cmp_Auto_Sw_En, ENABLE);
	} else {
		CMP_Write(Cmp_Enable, 0);
		CMP_Write(Cmp_Auto_Sw_En, DISABLE);
	}

}

/**
  * @brief  Enabled or disabled software-trigger channel switch in Comparator Software-Trigger Mode.
  * @param  NewState: can be one of the following value:
  *			@arg ENABLE: Enable the software-trigger channel switch.
  *				When setting this bit, do one channel switch in the channel switch list.
  *			@arg DISABLE:  Disable the software-trigger channel switch.
  * @retval  None.
  */
void CMP_SwTrigCmd(u32 NewState)
{
	if (NewState != DISABLE) {
		CMP_Write(Cmp_Auto_Sw_En, DISABLE);
		CMP_Write(Cmp_Enable, COMP_BIT_ENABLE);
		DelayMs(100);
		CMP_Write(Cmp_Enable, (COMP_BIT_TRIG_EN | COMP_BIT_ENABLE));
	} else {
		CMP_Write(Cmp_Enable, 0);
	}
}

/**
  * @brief	 Initialize the trigger timer when in Comparator Timer-Trigger Mode.
  * @param  Tim_Idx: The timer index would be used to make the Comparator module do a comparison.
  * @param  PeriodMs: Indicate the period of trigger timer.
  * @param  NewState: can be one of the following value:
  *			@arg ENABLE: Enable the Comparator timer trigger mode.
  *			@arg DISABLE: Disable the Comparator timer trigger mode.
  * @retval  None.
  * @note  Used in Comparator Timer-Trigger Mode, period range: 1ms ~ 131071ms.
  */
void CMP_TimerTrigCmd(u8 Tim_Idx, u32 PeriodMs, u32 NewState)
{
	assert_param(IS_COMP_VALID_TIM(Tim_Idx));
	assert_param(PeriodMs > 0 && PeriodMs < 131072); // avoid overflow

	RTIM_TimeBaseInitTypeDef TIM_InitStruct;
	u32 clksrc = RCC_PeriphClockSourceGet(PON_TIMER0_CLK);
	u32 tick_period_ms = 40000;

	switch (clksrc) {
	case PON_TIMER0_CLK_XTAL40M:
		tick_period_ms = 40000;
		break;
	case PON_TIMER0_CLK_OSC4M:
		tick_period_ms = 4 * 1000;
		break;
	case PON_TIMER0_CLK_OSC131K:
		tick_period_ms = 131;
		break;
	case PON_TIMER0_CLK_SDM32K:
		tick_period_ms = 33;  // 32.768
		break;
	}

	if (NewState != DISABLE) {
		RTIM_TimeBaseStructInit(&TIM_InitStruct);
		TIM_InitStruct.TIM_TimerIdx = Tim_Idx;
		TIM_InitStruct.TIM_LoadCnt = (PeriodMs * tick_period_ms) - 1;
		RTIM_TimeBaseInit(0, &TIM_InitStruct);
		RTIM_Cmd(Tim_Idx, ENABLE);

		CMP_Write(Cmp_Wk_Timer, COMP_BIT_WK_TIMER);
		CMP_Write(Cmp_Wk_Timer_Sel, COMP_WK_TIMER_SEL(Tim_Idx));
		CMP_Write(Cmp_Enable, COMP_BIT_ENABLE);
	} else {
		CMP_Write(Cmp_Enable, 0);
		RTIM_Cmd(Tim_Idx, DISABLE);
	}
}

void CMP_SetVref(u8 cmp_idx, u8 vref0, u8 vref1)
{
	assert_param(IS_COMP_CHN_SEL(cmp_idx));

	u32 val = (COMP_REF0(vref0) | COMP_REF1(vref1));

	CMP_Write((Cmp_Ref_Ch0 + (cmp_idx * 4)), val);

}
