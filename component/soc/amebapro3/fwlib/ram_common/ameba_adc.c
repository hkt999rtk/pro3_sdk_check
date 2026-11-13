/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "ameba_soc.h"
#include "ameba_adc.h"

static const char *TAG = "ADC";

static ADC_CalParaTypeDef CalParaNorm;
static ADC_CalParaTypeDef CalParaBypassMode;

static bool adc_bypass_en = 0;

/** @addtogroup Ameba_Periph_Driver
  * @{
  */

/** @defgroup ADC
* @brief ADC driver modules
* @{
*/

/* Exported functions --------------------------------------------------------*/
/** @defgroup ADC_Exported_Functions ADC Exported Functions
  * @{
  */

/**
  * @brief	 Initializes the parameters in the ADC_InitStruct with default values.
  * @param  ADC_InitStruct: pointer to a ADC_InitTypeDef structure that contains
  *         the configuration information for the ADC peripheral.
  * @retval  None
  */
void ADC_StructInit(ADC_InitTypeDef *ADC_InitStruct)
{
	u8 i;

	ADC_InitStruct->ADC_OpMode = ADC_SW_TRI_MODE;
	ADC_InitStruct->ADC_ClkDiv = ADC_CLK_DIV_640;
	ADC_InitStruct->ADC_RxThresholdLevel = 0;
	ADC_InitStruct->ADC_DMAThresholdLevel = 7;
	ADC_InitStruct->ADC_CvlistLen = ADC_CH_NUM - 1;
	ADC_InitStruct->ADC_ChanInType = 0;
	ADC_InitStruct->ADC_SpecialCh = 0xFF;
	ADC_InitStruct->ADC_ChIDEn = DISABLE;

	for (i = 0; i < ADC_InitStruct->ADC_CvlistLen + 1; i++) {
		ADC_InitStruct->ADC_Cvlist[i] = i;
	}
}

/**
  * @brief Initialize ADC according to the specified parameters in ADC_InitStruct.
  * @param ADC_InitStruct: pointer to a ADC_InitTypeDef structure that contains
  *         the configuration information of the ADC peripheral.
  * @retval None
  */
void ADC_Init(ADC_InitTypeDef *ADC_InitStruct)
{
	ADC_TypeDef	*ADC = ADC_DEV;
	u32 value;
	int len, i;
	//u8 vref_sel = 0x7;

	if (TrustZone_IsSecure()) {
		ADC = ADC_DEV_S;
	}

	assert_param(IS_ADC_MODE(ADC_InitStruct->ADC_OpMode));
	assert_param(ADC_InitStruct->ADC_CvlistLen < 16);

	/* Disable ADC, clear pending interrupt, clear FIFO */
	ADC_Cmd(DISABLE);

	ADC->INTR_CTRL = 0;
	ADC_INTClear();
	ADC_ClearFIFO();

	/* Set clock divider */
	ADC->CLK_DIV = ADC_CLK_DIV(ADC_InitStruct->ADC_ClkDiv);

	/* Set adc configuration*/
	value = ADC->CONF;
	value &= ~(ADC_MASK_CVLIST_LEN | ADC_MASK_OP_MOD);
	value |= ADC_OP_MOD(ADC_InitStruct->ADC_OpMode) | ADC_CVLIST_LEN(ADC_InitStruct->ADC_CvlistLen);
	ADC->CONF = value;

	/* Set channel input type [TODO] */
	ADC->IN_TYPE = ADC_InitStruct->ADC_ChanInType;

	/* Set channel switch list */
	value = 0;
	len = ((ADC_InitStruct->ADC_CvlistLen + 1) > 8) ? 8 : (ADC_InitStruct->ADC_CvlistLen + 1);
	for (i = 0; i < len; i++) {
		value |= (u32)(ADC_InitStruct->ADC_Cvlist[i] << ADC_SHIFT_CHSW0(i));
	}
	ADC->CHSW_LIST_0 = value;

	value = 0;
	if ((ADC_InitStruct->ADC_CvlistLen + 1) > 8) {
		for (i = 8; i < (ADC_InitStruct->ADC_CvlistLen + 1); i++) {
			value |= (u32)(ADC_InitStruct->ADC_Cvlist[i] << ADC_SHIFT_CHSW1(i));
		}
	}
	ADC->CHSW_LIST_1 = value;

	/* Set particular channel */
	if (ADC_InitStruct->ADC_SpecialCh < ADC_CH_NUM) {
		ADC_INTConfig(ADC_BIT_IT_CHCV_END_EN, ENABLE);
		ADC->IT_CHNO_CON = (u32)ADC_InitStruct->ADC_SpecialCh;
	}

	/* Set FIFO full level */
	ADC->FULL_LVL = (u32)ADC_InitStruct->ADC_RxThresholdLevel;

	/* Set DMA threshold level */
	ADC->DMA_CON = ADC_DMA_CON_LVL(ADC_InitStruct->ADC_DMAThresholdLevel);

	// Disable ADC analog to digital domain isolation
	SYSON_BASE->SYS_ADC_CTRL1 &= ~(SYSON_BIT_SYS_ISO_ADC);

	ADC_SetVrefSettleTime(ADC_VREF_SETTLE_TIME_20_US);

	ADC_PowerFromPad(ENABLE);
}

/**
  * @brief  Enable or Disable the ADC peripheral.
  * @param  NewState: new state of the ADC peripheral.
  *   			This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
void ADC_Cmd(u32 NewState)
{
	ADC_TypeDef	*ADC = ADC_DEV;

	if (TrustZone_IsSecure()) {
		ADC = ADC_DEV_S;
	}

	if (NewState != DISABLE) {
		ADC->CONF |= ADC_BIT_EN;
	} else {
		ADC->CONF &= ~ADC_BIT_EN;

		/* Need to clear FIFO, or there will be waste data in FIFO next time ADC is enable. Ameba-D A-cut bug */
		ADC_ClearFIFO();
	}
}

/**
  * @brief	 ENABLE/DISABLE  the ADC interrupt bits.
  * @param  ADC_IT: specifies the ADC interrupts to be configured.
  *         This parameter can be one or combinations of the following parameters:
  *            @arg ADC_BIT_IT_COMP_CHx_EN:		ADC channel x compare interrupt
  *                 where x can be 0-9 corresponding to channelx
  *            @arg ADC_BIT_IT_ERR_EN:			ADC error state interrupt
  *            @arg ADC_BIT_IT_DAT_OVW_EN:		ADC data overwritten interrupt
  *            @arg ADC_BIT_IT_FIFO_EMPTY_EN:	ADC FIFO empty interrupt
  *            @arg ADC_BIT_IT_FIFO_OVER_EN:	ADC FIFO overflow interrupt
  *            @arg ADC_BIT_IT_FIFO_FULL_EN:	ADC FIFO full interrupt
  *            @arg ADC_BIT_IT_CHCV_END_EN:		ADC particular channel conversion done interrupt
  *            @arg ADC_BIT_IT_CV_END_EN:		ADC conversion end interrupt
  *            @arg ADC_BIT_IT_CVLIST_END_EN:	ADC conversion list end interrupt
  * @param  NewState: ENABLE/DISABLE.
  * @retval  None
  */
void ADC_INTConfig(u32 ADC_IT, u32 NewState)
{
	ADC_TypeDef	*ADC = ADC_DEV;

	if (TrustZone_IsSecure()) {
		ADC = ADC_DEV_S;
	}

	if (NewState != DISABLE) {
		ADC->INTR_CTRL |= (ADC_IT);
	} else {
		ADC->INTR_CTRL &= ~(ADC_IT);
	}
}

/**
  * @brief	 Clear all the ADC interrupt pending bits.
  * @param  None
  * @retval  None
  * @note  This function can also used to clear raw status.
  */
void ADC_INTClear(void)
{
	ADC_TypeDef	*ADC = ADC_DEV;

	if (TrustZone_IsSecure()) {
		ADC = ADC_DEV_S;
	}

	/* Clear all the IT pending Bits */
	ADC->INTR_STS = ADC_BIT_IT_ALL_STS;
}

/**
  * @brief  Clear specified ADC interrupt pending bits.
  * @param  ADC_IT: specifies the pending bits to be cleared.
  *   This parameter can be one or combinations of the following values:
  *            @arg ADC_BIT_IT_COMP_CHx_STS
  *                 where x can be 0-9 corresponding to channelx
  *            @arg ADC_BIT_IT_ERR_STS
  *            @arg ADC_BIT_IT_DAT_OVW_STS
  *            @arg ADC_BIT_IT_FIFO_EMPTY_STS
  *            @arg ADC_BIT_IT_FIFO_OVER_STS
  *            @arg ADC_BIT_IT_FIFO_FULL_STS
  *            @arg ADC_BIT_IT_CHCV_END_STS
  *            @arg ADC_BIT_IT_CV_END_STS
  *            @arg ADC_BIT_IT_CVLIST_END_STS
  * @retval  None
  */
void ADC_INTClearPendingBits(u32 ADC_IT)
{
	ADC_TypeDef	*ADC = ADC_DEV;

	if (TrustZone_IsSecure()) {
		ADC = ADC_DEV_S;
	}

	ADC->INTR_STS = ADC_IT;
}

/**
  * @brief	 Get ADC interrupt status.
  * @param  None
  * @retval  Current Interrupt Status, each bit of this value represents one
  *		interrupt status which is as follows:
  *            @arg ADC_BIT_IT_COMP_CHx_STS
  *                 where x can be 0-9 corresponding to channelx
  *            @arg ADC_BIT_IT_ERR_STS
  *            @arg ADC_BIT_IT_DAT_OVW_STS
  *            @arg ADC_BIT_IT_FIFO_EMPTY_STS
  *            @arg ADC_BIT_IT_FIFO_OVER_STS
  *            @arg ADC_BIT_IT_FIFO_FULL_STS
  *            @arg ADC_BIT_IT_CHCV_END_STS
  *            @arg ADC_BIT_IT_CV_END_STS
  *            @arg ADC_BIT_IT_CVLIST_END_STS
  */
u32 ADC_GetISR(void)
{
	ADC_TypeDef	*ADC = ADC_DEV;

	if (TrustZone_IsSecure()) {
		ADC = ADC_DEV_S;
	}

	return ADC->INTR_STS;
}

/**
  * @brief  Get ADC raw interrupt status.
  * @param  None
  * @retval  Current Raw Interrupt Status, each bit of this value represents one
  *		raw interrupt status which is as follows:
  *            @arg ADC_BIT_IT_COMP_CHx_RAW_STS
  *                 where x can be 0-9 corresponding to channelx
  *            @arg ADC_BIT_IT_ERR_RAW_STS
  *            @arg ADC_BIT_IT_DAT_OVW_RAW_STS
  *            @arg ADC_BIT_IT_FIFO_EMPTY_RAW_STS
  *            @arg ADC_BIT_IT_FIFO_OVER_RAW_STS
  *            @arg ADC_BIT_IT_FIFO_FULL_RAW_STS
  *            @arg ADC_BIT_IT_CHCV_END_RAW_STS
  *            @arg ADC_BIT_IT_CV_END_RAW_STS
  *            @arg ADC_BIT_IT_CVLIST_END_RAW_STS
  */
u32 ADC_GetRawISR(void)
{
	ADC_TypeDef	*ADC = ADC_DEV;

	if (TrustZone_IsSecure()) {
		ADC = ADC_DEV_S;
	}

	return ADC->INTR_RAW_STS;
}

/**
  * @brief  Get the number of valid entries in ADC receive FIFO.
  * @param  None.
  * @retval  The number of valid entries in receive FIFO.
  */
u32 ADC_GetRxCount(void)
{
	ADC_TypeDef	*ADC = ADC_DEV;

	if (TrustZone_IsSecure()) {
		ADC = ADC_DEV_S;
	}

	return ADC_GET_FLR(ADC->FLR);
}

// /**
//   * @brief  Get the last ADC used channel.
//   * @param  None.
//   * @retval  The last ADC used channel index.
//   */
// u32 ADC_GetLastChan(void)
// {
// 	ADC_TypeDef	*ADC = ADC_DEV;

// 	if (TrustZone_IsSecure()) {
// 		ADC = ADC_DEV_S;
// 	}

// 	return ADC_GET_LAST_CH(ADC->LAST_CH);
// }

/**
  * @brief  [ADDED] Get the index which the last conversion in the conversion list is done.
  * @param  None.
  * @retval  The last converted index in the conversion list.
  */
u32 ADC_GetLastItem(void)
{
	ADC_TypeDef	*ADC = ADC_DEV;

	if (TrustZone_IsSecure()) {
		ADC = ADC_DEV_S;
	}

	return ADC_GET_LAST_ITEM(ADC->LAST_ITEM);
}

/**
  * @brief  Get comparison result of ADC channel.
  * @param  ADC_Channel: The channel index
  * @retval  The comparison result of specified ADC channel.
  */
u32 ADC_GetCompStatus(u8 ADC_Channel)
{
	ADC_TypeDef	*ADC = ADC_DEV;

	if (TrustZone_IsSecure()) {
		ADC = ADC_DEV_S;
	}

	u32 value = (ADC->COMP_STS & ADC_MASK_COMP_STS_CH(ADC_Channel)) \
				>> ADC_SHIFT_COMP_STS_CH(ADC_Channel);

	return  value;
}

/**
  * @brief  Set ADC channel threshold and criteria for comparison.
  * @param  ADC_channel: can be a value of @ref ADC_Chn_Selection as following:
  *				@arg ADC_CH0
  *				@arg ADC_CH1
  *				@arg ADC_CH2
  *				@arg ADC_CH3
  *				@arg ADC_CH4
  *				@arg ADC_CH5
  *				@arg ADC_CH6
  *				@arg ADC_CH7
  * @param  CompThresH:  the higher threshold of channel for ADC automatic comparison
  * @param  CompThresH:  the lower threshold of channel for ADC automatic comparison
  * @param  CompCtrl:  This parameter can be a value of @ref ADC_Compare_Control_Definitions as following:
  *		 		@arg ADC_COMP_SMALLER_THAN_THL: less than the lower threshold
  *		 		@arg ADC_COMP_GREATER_THAN_THH: greater than the higher threshod
  *		 		@arg ADC_COMP_WITHIN_THL_AND_THH: between the lower and higher threshod
  *		 		@arg ADC_COMP_OUTSIDE_THL_AND_THH: out the range of the higher and lower threshod
  * @retval None
  */
void ADC_SetComp(u8 ADC_channel, u16 CompThresH, u16 CompThresL, u8 CompCtrl)
{
	ADC_TypeDef	*ADC = ADC_DEV;

	if (TrustZone_IsSecure()) {
		ADC = ADC_DEV_S;
	}

	u32 value;

	assert_param(IS_ADC_CHN_SEL(ADC_channel));
	assert_param(IS_ADC_VALID_COMP_TH(CompThresH));
	assert_param(IS_ADC_VALID_COMP_TH(CompThresL));
	assert_param(IS_ADC_COMP_CRITERIA(CompCtrl));

	/* Set ADC channel threshold for comparison */
	ADC->COMP_TH_CHx[ADC_channel] =
		(u32)(ADC_CHx_TH_H(CompThresH) | ADC_CHx_TH_L(CompThresL));

	/* Set ADC channel comparison criteria */
	value = ADC->COMP_CTRL;
	value &= ~ ADC_MASK_COMP_CTRL_CH(ADC_channel);
	value |= (CompCtrl << ADC_SHIFT_COMP_CTRL_CH(ADC_channel));
	ADC->COMP_CTRL = value;

	/* Enable comparison interrupt */
	ADC_INTConfig(ADC_IT_COMP_CH_EN(ADC_channel), ENABLE);
}

/**
  * @brief  Reset the channel switch to default state.
  * @param  None
  * @retval  None
  */
void ADC_ResetChSwList(void)
{
	ADC_TypeDef	*ADC = ADC_DEV;

	if (TrustZone_IsSecure()) {
		ADC = ADC_DEV_S;
	}

	ADC->RST_LIST = ADC_BIT_RST_LIST;
	ADC->RST_LIST = 0;
}

/**
  * @brief  Determine ADC FIFO is readable or not.
  * @param  None.
  * @retval ADC FIFO is readable or not:
  *        - 0: Not readable
  *        - 1: Readable
  */
u32 ADC_Readable(void)
{
	u32 Status = ADC_GetStatus();
	u32 Readable = (((Status & ADC_BIT_STS_EMPTY) == 0) ? 1 : 0);

	return Readable;
}

/**
  * @brief  Read data from ADC Data Global register.
  * @param  None
  * @retval  The conversion data bit[11:0]
  * @note bit[25:23] indicates lastest converted channel data in fifo
  * @note bit[22] HW set to 1 when a conversion is done, will be cleared to 0 after a read
  * @note bit[11:0] conversion data
  */
u32 ADC_Read(void)
{
	ADC_TypeDef	*ADC = ADC_DEV;

	if (TrustZone_IsSecure()) {
		ADC = ADC_DEV_S;
	}

	u32 value = ADC->DAT_GLOBAL;

	return value;
}

/**
  * @brief  Continuous Read data in auto mode.
  * @param  pBuf: pointer to buffer to keep sample data
  * @param  len: the number of sample data to be read
  * @retval  None.
  */
void ADC_ReceiveBuf(u16 *pBuf, u32 len)
{
	u32 i = 0;

	ADC_ClearFIFO();

	ADC_AutoChSwCmd(ENABLE);

	for (i = 0; i < len; i++) {
		while (ADC_Readable() == 0);
		pBuf[i] = (u16)ADC_Read();
	}
	ADC_AutoChSwCmd(DISABLE);
}

/**
  * @brief  Indicates that a conversion of ADC channel is done or not.
  * @param  ADC_channel: can be a value of @ref ADC_Chn_Selection as following:
  *				@arg ADC_CH0
  *				@arg ADC_CH1
  *				@arg ADC_CH2
  *				@arg ADC_CH3
  *				@arg ADC_CH4
  *				@arg ADC_CH5
  *				@arg ADC_CH6
  *				@arg ADC_CH7
  * @param  ADC_data:  store adc data channel register value if not NULL.
  * @retval  0: A conversion is not done and data is not ready.
  *             1: A conversion is done and data is ready.
  * @note  Every time a conversion is done, the bit ADC_BIT_DAT_RDY_CHx
  *			should be set to 1 and it would be cleared to 0 when a read operation
  *			of ADC_DATA_CHx.
  * @note This function would not pop data from rx FIFO.
  */
u32 ADC_ChDataReady(u8 ADC_channel, u32 *ADC_data)
{
	u32 value;
	ADC_TypeDef	*ADC = ADC_DEV;

	if (TrustZone_IsSecure()) {
		ADC = ADC_DEV_S;
	}

	assert_param(IS_ADC_CHN_SEL(ADC_channel));

	if (ADC_data != NULL) {
		value = ADC->DAT_CHx[ADC_channel];

		if ((value & ADC_BIT_DAT_CHx_RDY) && ((value & ADC_BIT_DAT_CHx_OVW) == 0)) {
			*ADC_data = ADC_ID_AND_DATA(value);
			return 1;
		}

	}

	return 0;
}

/**
  * @brief  [TODO] Clear ADC FIFO.
  * @param  None
  * @retval  None
  */
void ADC_ClearFIFO(void)
{
	ADC_TypeDef	*ADC = ADC_DEV;

	if (TrustZone_IsSecure()) {
		ADC = ADC_DEV_S;
	}

	ADC->CLR_FIFO = ADC_BIT_CLR_FIFO;
	while (ADC->FLR != 0);
	ADC->CLR_FIFO = 0;
}

/**
  * @brief Get ADC status.
  * @param  None
  * @retval  Current status,each bit of this value represents one status which is as follows:
  *
  *		bit 2 : ADC_BIT_STS_EMPTY  ADC FIFO Empty.
  *			- 0: FIFO in ADC is not empty
  *			- 1: FIFO in ADC is empty
  *
  *		bit 1 : ADC_BIT_STS_FULL  ADC FIFO full flag.
  *			- 0: FIFO in ADC is not full real
  *			- 1: FIFO in ADC is full real
  *
  *		bit 0 : ADC_BIT_STS_BUSY  ADC busy Flag.
  *			- 0: The ADC is ready
  *			- 1: The ADC is busy
  */
u32 ADC_GetStatus(void)
{
	ADC_TypeDef	*ADC = ADC_DEV;

	if (TrustZone_IsSecure()) {
		ADC = ADC_DEV_S;
	}

	return ADC->STS;
}

/**
  * @brief  Control the ADC module to do a conversion. Used as a start-convert event which is controlled by software.
  * @param  NewState: can be one of the following value:
  *			@arg ENABLE: Enable the analog module and analog mux. And then start a new channel conversion.
  *			@arg DISABLE:  Disable the analog module and analog mux.
  * @retval  None.
  * @note  1. Every time this bit is set to 1, ADC module would switch to a new channel and do one conversion.
  *			  Every time a conversion is done, HW clears this bit to 0 when the operation is done (busy=0).
  *		  2. Used in Sotfware Trigger Mode
  */
void ADC_SWTrigCmd(u32 NewState)
{
	ADC_TypeDef	*ADC = ADC_DEV;

	if (TrustZone_IsSecure()) {
		ADC = ADC_DEV_S;
	}

	if (NewState != DISABLE) {

		while ((ADC->SW_TRIG == 1)) {

			// wait for HW to clear this bit

			if (ADC->SW_TRIG == 0) {
				break;
			}
		}

		ADC->SW_TRIG = ADC_BIT_SW_TRIG;
	} else {
		ADC->SW_TRIG = 0;
	}
}

/**
  * @brief  Sets clk divider value
  * @param  div: clk divider level of the ADC
  * @retval  None.
  * @note  Used in Automatic Mode
  * @note  This clock is the SCLK input to the ADC.
  */
void ADC_SetClkDiv(u16 div)
{
	ADC_TypeDef	*ADC = ADC_DEV;

	if (TrustZone_IsSecure()) {
		ADC = ADC_DEV_S;
	}

	ADC->CLK_DIV = ADC_CLK_DIV(div);

}

/**
  * @brief  Gets clk divider value
  * @retval  current clk divier value
  * @note  Used in Automatic Mode
  * @note  This clock is the SCLK input to the ADC.
  */
u16 ADC_GetClkDiv(void)
{
	ADC_TypeDef	*ADC = ADC_DEV;

	if (TrustZone_IsSecure()) {
		ADC = ADC_DEV_S;
	}

	return ADC_GET_CLK_DIV(ADC->CLK_DIV);
}

/**
  * @brief  Sets vref settle time
  * @param  time_us: time in us
  * @retval  None.
  * @note  If clk_div = 16'd400  then divider clock = 100KHz.  *
  * @note  To have 40us delay, need to set vref_settle_time = 4'h4.
  */
void ADC_SetVrefSettleTime(u16 vref_settle_time)
{
	ADC_TypeDef	*ADC = ADC_DEV;

	if (TrustZone_IsSecure()) {
		ADC = ADC_DEV_S;
	}

	u32 clk_src = RCC_PeriphClockSourceGet(PON_ADC_CLK);
	u32 clk_div = ADC_GET_CLK_DIV(ADC->CLK_DIV);
	u32 tick = 0;
	u32 period_us;

	if (clk_src == PON_ADC_CLK_XTAL40M) {

		period_us = (clk_div / 40);

	} else if (clk_src == PON_ADC_CLK_OSC4M) {

		period_us = (clk_div / 4);

	} else if (clk_src == PON_ADC_CLK_SDM32K) {

		period_us = (clk_div * 1000 * 1000) / (32768);

	} else if (clk_src == PON_ADC_CLK_OSC131K) {
		period_us = (clk_div * 1000 * 1000) / (131072);
	} else {
		RTK_LOGE(TAG, "ADC invalid clock source \r\n");
		return;
	}

	tick = ((vref_settle_time * 10) / period_us);

	if ((tick % 10) >= 5) {
		tick /= 10;
		tick = tick + 1;
	} else {
		tick /= 10;
	}

	ADC->VREF_SETTLE_TIME = ADC_VREF_SETTLE_TIME(tick);

}

/**
  * @brief  Controls the automatic channel switch enabled or disabled.
  * @param  NewState: can be one of the following value:
  *		@arg ENABLE: Enable the automatic channel switch.
  *			When setting this bit, an automatic channel switch starts from the first channel in the channel switch list.
  *		@arg DISABLE:  Disable the automatic channel switch.
  *			If an automatic channel switch is in progess, writing 0 will terminate the automatic channel switch.
  * @retval  None.
  * @note  Used in Automatic Mode
  */
void ADC_AutoChSwCmd(u32 NewState)
{
	ADC_TypeDef	*ADC = ADC_DEV;

	if (TrustZone_IsSecure()) {
		ADC = ADC_DEV_S;
	}

	if (NewState != DISABLE) {
		ADC->AUTO_CHSW_CTRL |= (ADC_BIT_AUTO_CHSW_EN);
	} else {
		ADC->AUTO_CHSW_CTRL &= ~(ADC_BIT_AUTO_CHSW_EN);
	}
}

/**
  * @brief	Initialize the trigger timer when in ADC Timer-Trigger Mode.
  * @param  Tim_Idx: The timer index would be used to make ADC module do a conversion.
  * @note   This para should be 0-7
  * @param  PeriodMs: Indicate the period of trigger timer.
  * @param  NewState: can be one of the following value:
  *			@arg ENABLE: Enable the ADC timer trigger mode.
  *			@arg DISABLE: Disable the ADC timer trigger mode.
  * @retval  None.
  * @note  Used in Timer-Trigger Mode, period range: 1ms ~ 131071ms.
  */
void ADC_TimerTrigCmd(u8 Tim_Idx, u32 PeriodMs, u32 NewState)
{
	ADC_TypeDef	*ADC = ADC_DEV;

	if (TrustZone_IsSecure()) {
		ADC = ADC_DEV_S;
	}

	RTIM_TimeBaseInitTypeDef TIM_InitStruct;

	u32 TimerGrp = 0;
	u32 clksrc = RCC_PeriphClockSourceGet(PON_TIMER0_CLK);
	u32 tick_period_ms = 40000;

	switch (clksrc) {
	case PON_TIMER0_CLK_XTAL40M:
		tick_period_ms = 40 * 1000;
		break;
	case PON_TIMER0_CLK_OSC4M:
		tick_period_ms = 4 * 1000;
		break;
	case PON_TIMER0_CLK_OSC131K:
		tick_period_ms = 128;
		break;
	case PON_TIMER0_CLK_SDM32K:
		tick_period_ms = 33;  // 32.768
		break;
	}

	assert_param(IS_ADC_VALID_TIM(Tim_Idx));
	ADC->TRIG_TIMER_SEL = Tim_Idx;

	if (NewState != DISABLE) {
		assert_param(PeriodMs > 0 && PeriodMs < 131072); // avoid overflow

		RTIM_TimeBaseStructInit(&TIM_InitStruct);
		TIM_InitStruct.TIM_TimerIdx = Tim_Idx;
		// TIM_InitStruct.TIM_Period = (PeriodMs * 32768) / 1000 / 2; //ms to tick
		TIM_InitStruct.TIM_LoadCnt = (PeriodMs * tick_period_ms); //ms to tick, using 40MHz clock [FPGA only]
		TIM_InitStruct.TIM_IntMask = ENABLE;

		RTIM_TimeBaseInit(TimerGrp, &TIM_InitStruct);
		RTIM_Cmd(Tim_Idx, ENABLE);
	} else {
		RTIM_Cmd(Tim_Idx, DISABLE);
	}
}

/**
  * @brief  Enable or Disable DMA read mode.
  * @param  newState: ENABLE/DISABLE
  * @retval  None.
  */
void ADC_SetDmaEnable(u32 newState)
{
	ADC_TypeDef	*ADC = ADC_DEV;

	if (TrustZone_IsSecure()) {
		ADC = ADC_DEV_S;
	}

	if (newState == DISABLE) {
		ADC->DMA_CON &= ~ADC_BIT_DMA_CON_EN;
	} else {
		ADC->DMA_CON |= ADC_BIT_DMA_CON_EN;
	}
}

/**
  * @brief    Init and Enable ADC RX GDMA.
  * @param  GDMA_InitStruct: pointer to a GDMA_InitTypeDef structure that contains
  *         the configuration information for the GDMA peripheral.
  * @param  CallbackData: GDMA callback data.
  * @param  CallbackFunc: GDMA callback function.
  * @param  pDataBuf: Rx Buffer.
  * @param  DataLen: Rx Count.
  * @retval   TRUE/FLASE
  */
u32 ADC_RXGDMA_Init(
	GDMA_InitTypeDef *GDMA_InitStruct,
	void *CallbackData,
	IRQ_FUN CallbackFunc,
	u8 *pDataBuf,
	u32 DataLen)
{
	ADC_TypeDef	*ADC = ADC_DEV;

	if (TrustZone_IsSecure()) {
		ADC = ADC_DEV_S;
	}

	u8 GdmaChnl;
	u8 GdmaIdx = 0;			// TODO

	RTK_LOGI(TAG, "DataLen = %d\r\n", DataLen);

	DCache_CleanInvalidate((u32)pDataBuf, (DataLen * 2));

	GdmaChnl = GDMA_ChnlAlloc(GdmaIdx, (IRQ_FUN)CallbackFunc, (u32)CallbackData, INT_PRI_MIDDLE);

	if (GdmaChnl == 0xFF) {
		RTK_LOGE(TAG, "ADC_RXGDMA_Init GDMA busy \n");
		return FALSE;
	}

	/* ADC RX DMA */
	_memset((void *)GDMA_InitStruct, 0, sizeof(GDMA_InitTypeDef));

	GDMA_InitStruct->SecureTransfer = 0x1;		// enable secure transfer

	GDMA_InitStruct->MuliBlockCunt      = 0;
	GDMA_InitStruct->MaxMuliBlock       = 1;                //MaxLlp;
	GDMA_InitStruct->GDMA_SrcDataWidth  = TrWidthTwoBytes; // TrWidthFourBytes;
	GDMA_InitStruct->GDMA_DstDataWidth  = TrWidthTwoBytes; // TrWidthFourBytes;
	GDMA_InitStruct->GDMA_SrcMsize      = MsizeEight;
	GDMA_InitStruct->GDMA_DstMsize      = MsizeEight;
	GDMA_InitStruct->GDMA_SrcInc        = NoChange;
	GDMA_InitStruct->GDMA_DstInc        = IncType;
	GDMA_InitStruct->GDMA_DIR           = TTFCPeriToMem;
	GDMA_InitStruct->GDMA_SrcHandshakeInterface = GDMA0_HANDSHAKE_INTERFACE_ADC_RX;
	GDMA_InitStruct->GDMA_ReloadSrc     = 1;
	GDMA_InitStruct->GDMA_IsrType       = (BlockType | TransferType | ErrType);
	GDMA_InitStruct->GDMA_ChNum         = GdmaChnl;
	GDMA_InitStruct->GDMA_Index         = GdmaIdx;

	GDMA_InitStruct->GDMA_BlockSize     = DataLen;
	GDMA_InitStruct->GDMA_SrcAddr       = (u32) & (ADC->DAT_GLOBAL);
	GDMA_InitStruct->GDMA_DstAddr       = (u32)pDataBuf;

	/* GDMA initialization */
	GDMA_Init(GDMA_InitStruct->GDMA_Index, GDMA_InitStruct->GDMA_ChNum, GDMA_InitStruct);
	GDMA_Cmd(GDMA_InitStruct->GDMA_Index, GDMA_InitStruct->GDMA_ChNum, ENABLE);

	return TRUE;
}

/**
  * @brief Initialize ADC calibration parameters according to EFuse in divider mode.
  * @param CalPara: Pointer to ADC calibration parameter structure.
  * @retval None.
  */
void ADC_InitCalPara(ADC_CalParaTypeDef *CalPara)
{
	u8 index;
	u8 EfuseBuf[6];
	u32 AdcCalAddr = DIVIDOR_MODE_ADDR;
	u16 K_A, K_B, K_C;
	s32 ka, kb, kc;

	for (index = 0; index < 6; index++) {
		OTP_Read8(AdcCalAddr + index, EfuseBuf + index);
	}
	K_A = EfuseBuf[1] << 8 | EfuseBuf[0];
	K_B = EfuseBuf[3] << 8 | EfuseBuf[2];
	K_C = EfuseBuf[5] << 8 | EfuseBuf[4];

	if (K_A == 0xFFFF) {
		K_A = 0x0049;
	}

	if (K_B == 0xFFFF) {
		K_B = 0x6EE8;
	}

	if (K_C == 0xFFFF) {
		K_C = 0xE334;
	}

	ka = (K_A & BIT15) ? -(0x10000 - K_A) : (K_A & 0x7FFF);
	kb = (K_B & 0xFFFF);
	kc = (K_C & BIT15) ? -(0x10000 - K_C) : (K_C & 0x7FFF);

	CalPara->cal_a = ka;
	CalPara->cal_b = kb;
	CalPara->cal_c = kc;

	RTK_LOGI(TAG, "a = %d\r\n", (int)ka);
	RTK_LOGI(TAG, "b = %d\r\n", (int)kb);
	RTK_LOGI(TAG, "c = %d\r\n", (int)kc);

	CalPara->init_done = TRUE;
}

/**
  * @brief Initialize ADC calibration parameters according to EFuse in bypass mode.
  * @param CalPara: Pointer to ADC calibration parameter structure.
  * @retval None.
  */
void ADC_InitCalParaBypass(ADC_CalParaTypeDef *CalPara)
{
	u8 index;
	u8 EfuseBuf[6];
	u32 AdcCalAddr = BYPASS_MODE_ADDR;
	u16 K_A, K_B, K_C;
	s32 ka, kb, kc;

	RTK_LOGI(TAG, "bypass otp addr = 0x%x\r\n", AdcCalAddr);

	for (index = 0; index < 6; index++) {
		OTP_Read8(AdcCalAddr + index, EfuseBuf + index);
	}
	K_A = EfuseBuf[1] << 8 | EfuseBuf[0];
	K_B = EfuseBuf[3] << 8 | EfuseBuf[2];
	K_C = EfuseBuf[5] << 8 | EfuseBuf[4];

	if (K_A == 0xFFFF) {
		K_A = 0x0007;
	}

	if (K_B == 0xFFFF) {
		K_B = 0x1af7;
	}

	if (K_C == 0xFFFF) {
		K_C = 0xf8e8;
	}

	ka = (K_A & BIT15) ? -(0x10000 - K_A) : (K_A & 0x7FFF);
	kb = (K_B & 0xFFFF);
	kc = (K_C & BIT15) ? -(0x10000 - K_C) : (K_C & 0x7FFF);

	CalPara->cal_a = ka;
	CalPara->cal_b = kb;
	CalPara->cal_c = kc;

	RTK_LOGI(TAG, "a = %d\r\n", (int)ka);
	RTK_LOGI(TAG, "b = %d\r\n", (int)kb);
	RTK_LOGI(TAG, "c = %d\r\n", (int)kc);

	CalPara->init_done = TRUE;
}

/**
  * @brief  Get voltage value in mV.
  * @param  chan_data: ADC conversion data.
  * @retval ADC voltage value in mV.
  */
s32 ADC_GetVoltage(u32 chan_data)
{
	s64 ka, kb;
	s32 kc;

	s32 ch_vol;

	if (!CalParaNorm.init_done) {
		ADC_InitCalPara(&CalParaNorm);
	}

	if (!CalParaBypassMode.init_done) {
		ADC_InitCalParaBypass(&CalParaBypassMode);
	}

	ka = CalParaNorm.cal_a;
	kb = CalParaNorm.cal_b;
	kc = CalParaNorm.cal_c;

	if (adc_bypass_en) {
		RTK_LOGI(TAG, "bypass mode enable\r\n");
		ka = CalParaBypassMode.cal_a;
		kb = CalParaBypassMode.cal_b;
		kc = CalParaBypassMode.cal_c;
	}

	ch_vol = ((ka * chan_data * chan_data) >> 26)  + ((kb * chan_data) >> 15)  + (kc >> 6);

	return ch_vol;
}

/**
  * @brief Get internal R resistance of V33 channels(CH0~CH7) in divider mode.
  * @param none.
  * @retval Internal R resistance value in Kohm.
  */
u32 ADC_GetInterR(void)
{
	u8 index;
	u8 EfuseBuf[2];

	for (index = 0; index < 2; index++) {
		OTP_Read8(INTER_R_ADDR + index, EfuseBuf + index);
	}

	return (u32)(EfuseBuf[1] << 8 | EfuseBuf[0]);
}

/**
  * @brief	 Power 1p35V from pad
  * @param  NewState: ENABLE or DISABLE
  * @retval  None
  */
void ADC_PowerFromPad(u32 NewState)
{
	if (NewState == ENABLE) {
		// REG0X_BG[0] (POW_BG) = HIGH
		// REG0X_BG[1] (POW_I)  = HIGH	0: on, 1: off
		BANDGAP_BASE->AON_PLL_CTRL0 |= BANDGAP_BIT_PLL_POW_BG;
		BANDGAP_BASE->AON_PLL_CTRL0 &= ~(BANDGAP_BIT_PLL_POW_I);

		DelayUs(100);

		// REG0X_BG[1] (POW_I)  	= LOW	0: on, 1: off
		// REG0X_BG[2] (POW_MBIAS) 	= HIGH
		// REG0X_LPSD[0] (POW_SD)	= HIGH
		BANDGAP_BASE->AON_PLL_CTRL0 |= BANDGAP_BIT_PLL_POW_I;
		BANDGAP_BASE->AON_PLL_CTRL0 |= BANDGAP_BIT_PLL_POW_MBIAS;

		// REG0X_LPSD[0] (pow_sd)
		BANDGAP_BASE->AON_SD_CTRL0 &= ~(BANDGAP_BIT_POW_SD);	// aon cmp and adc cannot be on at same time

		DelayUs(10);

		// REG0X_LDO[3],REG0X_LDO[0] (POW_LDO,POW)
		BANDGAP_BASE->AON_PLL_CTRL0 |= (BANDGAP_BIT_POW_LDO09 | BANDGAP_BIT_POW_LDO18);

		DelayUs(30);

		// REG0X_LPAD[1] (pow_ref)
		SYSON_BASE->SYS_ADC_CTRL0 |= SYSON_BIT_POW_REF;

		DelayUs(530);

		// REG0X_LPAD[0] (pow_ad)
		// SYSON_BASE->SYS_ADC_CTRL0 |= SYSON_BIT_POW;			// this is controlled by digital FSM
	} else {

		SYSON_BASE->SYS_ADC_CTRL0 &= ~(SYSON_BIT_POW_REF);
		BANDGAP_BASE->AON_PLL_CTRL0 &= ~(BANDGAP_BIT_POW_LDO09 | BANDGAP_BIT_POW_LDO18);

		// no need to switch off bandgap
		// BANDGAP_BASE->AON_PLL_CTRL0 &= ~(BANDGAP_BIT_PLL_POW_MBIAS);
		// BANDGAP_BASE->AON_PLL_CTRL0 &= ~(BANDGAP_BIT_PLL_POW_BG);
	}

}

/**
  * @brief	 Power 1p35V from LDO
  * @param  NewState: ENABLE or DISABLE
  * @retval  None
  */
void ADC_PowerFromLDO(u32 NewState)
{
	if (NewState == ENABLE) {
		// REG1X_LDO[1] (135_LDO_SOFT_START)
		// Pulled low before REG1X_LDO[1] turn on
		BANDGAP_BASE->AON_PLL_CTRL1 &= ~(BANDGAP_BIT_POW_SOFT_START);

		// REG0X_BG[0] (POW_BG) = HIGH
		// REG0X_BG[1] (POW_I)  = HIGH	0: on, 1: off
		BANDGAP_BASE->AON_PLL_CTRL0 |= BANDGAP_BIT_PLL_POW_BG;
		BANDGAP_BASE->AON_PLL_CTRL0 &= ~(BANDGAP_BIT_PLL_POW_I);

		DelayUs(100);

		// REG0X_BG[1] (POW_I)  	= LOW	0: on, 1: off
		// REG0X_BG[2] (POW_MBIAS) 	= HIGH
		// REG0X_LPSD[0] (POW_SD)	= HIGH
		BANDGAP_BASE->AON_PLL_CTRL0 |= BANDGAP_BIT_PLL_POW_I;
		BANDGAP_BASE->AON_PLL_CTRL0 |= BANDGAP_BIT_PLL_POW_MBIAS;

		// REG0X_LPSD[0] (pow_sd)
		BANDGAP_BASE->AON_SD_CTRL0 &= ~(BANDGAP_BIT_POW_SD);		// aon cmp and adc cannot be on at same time

		DelayUs(10);

		// REG1X_LDO[0] (POW_135_LDO)
		BANDGAP_BASE->AON_PLL_CTRL1 |= BANDGAP_BIT_POW_LDO_N;

		// ERC_VA1 (135_LDO_output)

		DelayUs(30);

		// REG0X_LDO[3],REG0X_LDO[0] (POW_LDO,POW)
		BANDGAP_BASE->AON_PLL_CTRL0 |= (BANDGAP_BIT_POW_LDO09 | BANDGAP_BIT_POW_LDO18);

		// REG1X_LDO[1] (135_LDO_SOFT_START)
		// Pulled low before REG1X_LDO[1] turn on
		BANDGAP_BASE->AON_PLL_CTRL1 |= BANDGAP_BIT_POW_SOFT_START;

		// ERC_VD09AD,VA18

		DelayUs(30);

		// REG0X_LPAD[1] (pow_ref)
		SYSON_BASE->SYS_ADC_CTRL0 |= SYSON_BIT_POW_REF;

		DelayUs(530);

		// REG0X_LPAD[0] (pow_ad)
		// SYSON_BASE->SYS_ADC_CTRL0 |= SYSON_BIT_POW;			// this is controlled by digital FSM
	} else {

		SYSON_BASE->SYS_ADC_CTRL0 &= ~(SYSON_BIT_POW_REF);
		BANDGAP_BASE->AON_PLL_CTRL1 &= ~(BANDGAP_BIT_POW_SOFT_START);
		BANDGAP_BASE->AON_PLL_CTRL0 &= ~(BANDGAP_BIT_POW_LDO09 | BANDGAP_BIT_POW_LDO18);
		BANDGAP_BASE->AON_PLL_CTRL1 &= ~(BANDGAP_BIT_POW_LDO_N);

		// no need to switch off bandgap
		// BANDGAP_BASE->AON_PLL_CTRL0 &= ~(BANDGAP_BIT_PLL_POW_MBIAS);
		// BANDGAP_BASE->AON_PLL_CTRL0 &= ~(BANDGAP_BIT_PLL_POW_BG);
	}

}

/**
  * @brief	 ENABLE ADC BYPASS MODE
  * @param  NewState: ENABLE or DISABLE
  * @retval  None
  * @note 	bypass enable -> 0.8V
  * 		bypass disable -> 3.15V (default)
  *
  */
void ADC_BypassMode(u32 NewState)
{
	if (NewState == ENABLE) {
		RTK_LOGI(TAG, "adc_bypass_en = 1 [0.8V] \r\n");
		SYSON_BASE->SYS_ADC_CTRL0 |= (SYSON_MASK_BYPASSEN_7_0);
		adc_bypass_en = 1;
	} else {
		RTK_LOGI(TAG, "adc_bypass_en = 0 [3.15V] \r\n");
		SYSON_BASE->SYS_ADC_CTRL0 &= ~(SYSON_MASK_BYPASSEN_7_0);
		adc_bypass_en = 0;
	}

}

/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */
