/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "ameba_soc.h"

u8 wakepin_io[] = {
	_PA_0,  _PA_1,  _PA_2,  _PA_3
};

u8 wake_pin_idx_offset_groupA[8] = {0, 0, 0, 11, 21, 5, 0xFF, 21};

/**
  * @brief  set REGU wakeup_pin (just on wakepin mux to 2 pads).
  * @param  PinMask: aon wakepin index
  *		This parameter can be one of the following values:
  *		 @arg WAKEPIN_0 : PA0
  *		 @arg WAKEPIN_1 : PA1
  *		 @arg WAKEPIN_2 : PA2
  *		 @arg WAKEPIN_3 : PA3
  * @param  level: aon wakepin level
  *		This parameter can be one of the following values:
  *		 @arg 1 : high wakeup
  *		 @arg 0 : low wakeup
  * @note wakeup state: sleep PG & deep sleep
  * @retval None
  */
void WAKEPIN_INTMode(u32 GPIO_Pin, u32 NewState,
					 u32 GPIO_ITTrigger,
					 u32 GPIO_ITPolarity,
					 u32 GPIO_ITDebounce)
{
	GPIO_TypeDef *GPIO = AON_GPIO_BASE;
	u8 port_num;
	u8 pin_num;

	u8 gpio_group_sel = 0;
	u8 actual_pin_num;
	volatile u32 *GPIO_INTx_actual_reg;
	u8 pin_idx = 0;

	port_num = PORT_NUM(GPIO_Pin);
	pin_num = PIN_NUM(GPIO_Pin);
	pin_idx = pin_num;
	actual_pin_num = wake_pin_idx_offset_groupA[port_num] + pin_num;
	GPIO_INTx_actual_reg = ((volatile uint32_t *) & (AON_GPIO_BASE->GPIO_INT0_SEL) + pin_idx);
	*GPIO_INTx_actual_reg = ((GPIO_INT0_MER_SEL(actual_pin_num)) | (GPIO_INT0_GP_SEL(gpio_group_sel)));
	GPIO->GPIO_INT_DIS |= (1 << pin_idx);
	if (GPIO_ITTrigger == GPIO_INT_Trigger_EDGE) {
		if (GPIO_ITPolarity == GPIO_INT_POLARITY_ACTIVE_HIGH) {
			GPIO->GPIO_IR_EN = (1 << pin_idx);
			GPIO->GPIO_EI_EN = (1 << pin_idx);
		} else if (GPIO_ITPolarity == GPIO_INT_POLARITY_ACTIVE_LOW) {
			GPIO->GPIO_IF_EN = (1 << pin_idx);
			GPIO->GPIO_EI_EN = (1 << pin_idx);
		}
	} else if (GPIO_ITTrigger == GPIO_INT_Trigger_LEVEL) {
		if (GPIO_ITPolarity == GPIO_INT_POLARITY_ACTIVE_HIGH) {
			GPIO->GPIO_IR_EN = (1 << pin_idx);
			GPIO->GPIO_LI_EN = (1 << pin_idx);
		} else { // GPIO_INT_POLARITY_ACTIVE_LOW
			GPIO->GPIO_IF_EN = (1 << pin_idx);
			GPIO->GPIO_LI_EN = (1 << pin_idx);
		}
	} else if (GPIO_ITTrigger == GPIO_INT_Trigger_BOTHEDGE) {
		GPIO->GPIO_ID_EN = (1 << pin_idx);
		GPIO->GPIO_EI_EN = (1 << pin_idx);
	}

	if (GPIO_ITDebounce == GPIO_INT_DEBOUNCE_ENABLE) {
		//RegValue |= (0x01 << pin_num);
	}
	//GPIO->GPIO_DEBOUNCE = RegValue;

	/* set interrupt mode enable register */
	if (NewState == ENABLE) {
		GPIO->GPIO_INT_EN = (0x01 << pin_idx); //int mode
	} else {
		GPIO->GPIO_INT_DIS = ~(0x01 << pin_num); //normal gpio mode
	}
}

void WAKEPIN_INTConfig(u32 GPIO_Pin, u32 NewState)
{
	(void)GPIO_Pin;
	(void)NewState;

	GPIO_TypeDef *GPIO = AON_GPIO_BASE;
	//u8 port_num;
	u8 pin_num;
	u8 pin_idx = 0;

	//port_num = PORT_NUM(GPIO_Pin);
	pin_num = PIN_NUM(GPIO_Pin);
	pin_idx = pin_num;

	// Clear Pending Interrupt, just in case
	GPIO->GPIO_INT_CLR = (1 << pin_idx);
	__DSB();

	// Set Interrupt Mask Reg, and Enable Interrupt Register
	GPIO->GPIO_IM_EN = (1 << pin_idx);
	GPIO->GPIO_INT_EN = (1 << pin_idx);

	(void)pin_num;
}


void Wakepin_Setting(u32 PinIdx, u32 level)
{
	RCC_PeriphClockCmd(APBPeriph_AON_GPIO, APBPeriph_AONGPIO_CLOCK, ENABLE);
	AON_BASE->AON_CLK_CTRL |= AON_BIT_AON_GPIO_INT_CLK_SEL;

	WAKEPIN_INTMode(wakepin_io[PinIdx], ENABLE, GPIO_INT_Trigger_EDGE, level, GPIO_INT_DEBOUNCE_DISABLE);
	WAKEPIN_INTConfig(wakepin_io[PinIdx], ENABLE);
}


/**
  * @brief  set REGU wakeup_pin debounce cycle.
  * @param  Dbnc_cycle: wakeup_pin debounce cycle, unit is 100k/128k.
  *		This parameter must be set to a value in the 0-0x7FFF range.
  * @param  Status: enable or disable wakepin debounce function
  * @note Dbnc_cycle is valid when Status is set ENABLE.
  * @retval None
  */
void Wakepin_Debounce_Setting(u32 Dbnc_cycle, u32 Status)
{
	(void)Dbnc_cycle;
	(void)Status;
}

/**
  * @brief  get wakepin index.
  * @param  None
  * @retval BIT(0): wakepin0, BIT(1): wakepin1, BIT(2): wakepin2, BIT(3): wakepin3
  */
int WakePin_Get_Idx(void)
{
	return 0;
}


/**
  * @brief  Clear wakepin interrupt.
  * @retval   None
  */
void WakePin_ClearINT(u32 wakepin)
{
	WAKEPIN_INTConfig(wakepin_io[wakepin], ENABLE);
}
