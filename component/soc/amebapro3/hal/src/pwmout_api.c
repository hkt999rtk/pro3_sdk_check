/** mbed Microcontroller Library
 ******************************************************************************
 * @file     pwm_api.c
 * @author
 * @version V1.0.0
 * @date     2016-08-01
 * @brief    This file provides mbed API for PWM.
 ******************************************************************************
 * @attention
 *
 * This module is a confidential and proprietary property of RealTek and
 * possession or use of this module requires written permission of RealTek.
 *
 * Copyright(c) 2016, Realtek Semiconductor Corporation. All rights reserved.
 ******************************************************************************
 */

#include "device.h"
#include "objects.h"
#include "pwmout_api.h"
#include "ameba_timer.h"
#include "timer_api.h"
#include "sysreg_rcc.h"

#define PWM_DEBUG_INFO 0

#define PWM_CHANNEL_MAX     8
#define PWM_TIMER_NUM       8

#define PWM_SOURCE_UNMAP	0xFF

/** @addtogroup Ameba_Mbed_API
  * @{
  */

/** @addtogroup MBED_PWM
 *  @brief      MBED_PWM driver modules.
 *  @{
 */

/** @defgroup MBED_PWM_Exported_Constants MBED_PWM Exported Constants
 * @{
 */

static uint8_t timer_start[PWM_TIMER_NUM] = {0};
static uint8_t ch_start[PWM_TIMER_NUM][PWM_CHANNEL_MAX] = {0};
static float tick_src_unit[PWM_TIMER_NUM] = {1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f}; // in us

/**
  * @}
  */

/** @defgroup MBED_PWM_Exported_Functions MBED_PWM Exported Functions
 * @{
 */


static uint64_t pwm_timer_us_to_loadcnt(pwmout_t *obj)
{
	uint64_t res;
	uint32_t clk_freq;

	clk_freq = PERI_ClkGet(CLKID_PON_TIMER0);

	// note that, the CLK_SRC will never be too fast, because LOADCNT is 32bit wide
	// but the CLK_SRC can be too slow, when the resolution can't be less thant 1%, it is too slow
	if ((1000000.0f / clk_freq) > (obj->period / 100.0f)) {
		// resolution > 1%, make LOADCNT 0 to be closest to 1% of period
		res = 0;
		tick_src_unit[obj->pwmtimer_idx] = 1000000.0f / clk_freq; // in us
		DiagPrintf("clk_freq is too slow for this target value, set LOADCNT to 0\n");
	} else if ((1000000.f / clk_freq) <= (obj->period / 1000.0f)) {
		// resolution <= 0.1%, set LOADCNT to make resolution 0.1%
		res = (obj->period / 1000.0f) * clk_freq / 1000000.0f;
		tick_src_unit[obj->pwmtimer_idx] = (res + 1) * 1000000.0f / clk_freq; // in us
		DiagPrintf("pwm precision 0.1%%, set LOADCNT to %ld\n", (uint32_t)res);
	} else {
		// 0.1% < resolution < 1%, set LOADCNT 0 to have best resolution
		res = 0;
		tick_src_unit[obj->pwmtimer_idx] = 1000000.0f / clk_freq; // in us
		DiagPrintf("pwm precision %d%%, set LOADCNT to 0\n", (int)((tick_src_unit[obj->pwmtimer_idx] / obj->period) * 100));
	}

	return res;
}

/**
  * @brief  Initialize the PWMTIM device.
  * @param  obj: PWM object defined in application software.
  * @retval none
  */
static void pwmout_timer_init(pwmout_t *obj)
{
	RTIM_TimeBaseInitTypeDef TIM_InitStruct;

	// Pro3 PWM only support Group0 Timer
	assert_param(obj->pwmtimer_idx < PWM_TIMER_NUM);
	assert_param(obj->pwm_idx < PWM_CHANNEL_MAX);

	// enable Timer Group0 Clock
	RCC_PeriphClockCmd(APBPeriph_PON_TIMER0, APBPeriph_PON_TIMER0_CLOCK, ENABLE);

	TIM_InitStruct.TIM_TimerIdx = obj->pwmtimer_idx;
	TIM_InitStruct.TIM_TimerGrp = 0;
	TIM_InitStruct.TIM_Prescaler = 0;
	TIM_InitStruct.TIM_IntMask = DISABLE;
	TIM_InitStruct.TIM_CntMode = TIM_CNT_MODE_DOWN;
	TIM_InitStruct.TIM_OpMode = TIM_OP_MODE_PERIODICAL;
	TIM_InitStruct.TIM_LoadCnt = pwm_timer_us_to_loadcnt(obj);

	RTIM_StartPeriodic(0, &TIM_InitStruct, 0, 0);

	timer_start[obj->pwmtimer_idx] = 1;
}

/**
  * @brief  Initialize PWM channel output according to the specified pin.
  * @param  obj: PWM object defined in application software.
  * @param  pin: Pinname of corresponding PWM channel to be set.
  * @retval none
  * @note
  */
void pwmout_init(pwmout_t *obj, PinName pin)
{
	// Pro3 PWM only support Group0 Timer
	assert_param(obj->pwmtimer_idx < PWM_TIMER_NUM);
	assert_param(obj->pwm_idx < PWM_CHANNEL_MAX);

	if ((obj->period == 0)) {
		obj->period = 1000;
		obj->pulse = 300;
	}

	if (!timer_start[obj->pwmtimer_idx]) {
		pwmout_timer_init(obj);
	}

#if PWM_DEBUG_INFO
	DiagPrintf("pin: 0x%x\n", pin);
	DiagPrintf("pwm idx %d\n", obj->pwm_idx);
	DiagPrintf("timer idx %d\n", obj->pwmtimer_idx);
	DiagPrintf("period %d\n", obj->period);
	DiagPrintf("pulse %d\n", (uint32_t)(obj->pulse));
#endif

	RCC_PeriphClockCmd(APBPeriph_PWM, APBPeriph_PWM_CLOCK, ENABLE);
	RCC_PeriphClockCmd(APBPeriph_PINMUX, APBPeriph_PINMUX_CLOCK, ENABLE);

	Pinmux_Config(pin, PINMUX_FUNCTION_PWM);

	PWM_SetTickSource(obj->pwm_idx, obj->pwmtimer_idx);

	PWM_SetPeriod(obj->pwm_idx, obj->period / tick_src_unit[obj->pwmtimer_idx]);
	PWM_SetDutySize(obj->pwm_idx, (u16)(obj->pulse / tick_src_unit[obj->pwmtimer_idx]));

	//PWM_Cmd(obj->pwm_idx, ENABLE);

	ch_start[obj->pwmtimer_idx][obj->pwm_idx] = 1;
}

/**
  * @brief  Deinitialize the PWM device of the specified channel.
  * @param  obj: PWM object defined in application software.
  * @retval none
  * @note  If all channels are released, PWMTIMER will also be disabled.
  */
void pwmout_free(pwmout_t *obj)
{
	/* disable pwm channel */
	uint8_t pwm_idx = obj->pwm_idx;

	// Pro3 PWM only support Group0 Timer
	assert_param(obj->pwmtimer_idx < PWM_TIMER_NUM);
	assert_param(obj->pwm_idx < PWM_CHANNEL_MAX);

	if (ch_start[obj->pwmtimer_idx][pwm_idx]) {
		ch_start[obj->pwmtimer_idx][pwm_idx] = 0;
		PWM_Cmd(obj->pwm_idx, DISABLE);

		/* stop pwmtimer if no pwm channels starts */
		for (pwm_idx = 0; pwm_idx < PWM_CHANNEL_MAX; pwm_idx++) {
			if (ch_start[obj->pwmtimer_idx][pwm_idx]) {
				return;
			}
		}

#if PWM_DEBUG_INFO
		DiagPrintf("free timer%d\n", obj->pwmtimer_idx);
#endif
		RTIM_Cmd(obj->pwmtimer_idx, DISABLE);
		timer_start[obj->pwmtimer_idx] = 0;
	}

	return;
}

/**
  * @brief  Set the duty cycle of the specified channel.
  * @param  obj: PWM object defined in application software.
  * @param  percent: The duty cycle value to be set.
  * @retval none
  */
void pwmout_write(pwmout_t *obj, float percent) //write duty-cycle
{
	assert_param(obj->pwm_idx < PWM_CHANNEL_MAX);

	if (percent < (float)0.0) {
		percent = 0.0;
	} else if (percent > (float)1.0) {
		percent = 1.0;
	}

	obj->pulse = percent * obj->period;

#if PWM_DEBUG_INFO
	DiagPrintf("pwm_write: new pluse: %d\n", (u16)(obj->pulse));
#endif

	PWM_SetDutySize(obj->pwm_idx, (u16)(obj->pulse / tick_src_unit[obj->pwmtimer_idx]));

	PWM_UpdateCtrl(obj->pwm_idx);
}

/**
  * @brief  Get the duty cycle of the specified channel.
  * @param  obj: PWM object defined in application software.
  * @return Duty cycle of the specified channel.
  * @retval 0~1.0
  */
float pwmout_read(pwmout_t *obj) //read duty-cycle
{
	float value = 0;

	assert_param(obj->pwm_idx < PWM_CHANNEL_MAX);

	if (obj->period > 0) {
		value = (float)obj->pulse / (float)obj->period;
	}

	return ((value > 1.0) ? (1.0) : (value));
}

/**
  * @brief  Set the period of the specified channel in seconds.
  * @param  obj: PWM object defined in application software.
  * @param  seconds: The period value to be set in seconds(s).
  * @retval none
  */
void pwmout_period(pwmout_t *obj, float seconds)
{
	pwmout_period_us(obj, (int)(seconds * 1000000.0f));
}

/**
  * @brief  Set the period of the specified channel in millseconds.
  * @param  obj: PWM object defined in application software.
  * @param  ms: The period value to be set in millseconds(ms).
  * @retval none
  */
void pwmout_period_ms(pwmout_t *obj, int ms)
{
	pwmout_period_us(obj, (int)(ms * 1000UL));
}

/**
  * @brief  Set the period of the specified channel in microseconds.
  * @param  obj: PWM object defined in application software.
  * @param  us: The period value to be set in microseconds(us).
  * @retval none
  */
void pwmout_period_us(pwmout_t *obj, int us)
{
	float duty_rate;

	assert_param(obj->pwm_idx < PWM_CHANNEL_MAX);

	duty_rate = pwmout_read(obj);

	PWM_SetPeriod(obj->pwm_idx, us / tick_src_unit[obj->pwmtimer_idx]);
	obj->period = us;

	pwmout_write(obj, duty_rate);

	PWM_UpdateCtrl(obj->pwm_idx);
}

/**
  * @brief  Set the pulse width of the specified channel in seconds.
  * @param  obj: PWM object defined in application software.
  * @param  seconds: The pulse width value to be set in seconds(s).
  * @retval none
  */
void pwmout_pulsewidth(pwmout_t *obj, float seconds)
{
	pwmout_pulsewidth_us(obj, (int)(seconds * 1000000.0f));
}

/**
  * @brief  Set the pulse width of the specified channel in milliseconds.
  * @param  obj: PWM object defined in application software.
  * @param  ms: The pulse width value to be set in milliseconds(ms).
  * @retval none
  */
void pwmout_pulsewidth_ms(pwmout_t *obj, int ms)
{
	pwmout_pulsewidth_us(obj, (int)(ms * 1000UL));
}

/**
  * @brief  Set the pulse width of the specified channel in microseconds.
  * @param  obj: PWM object defined in application software.
  * @param  us: The pulse width value to be set in microseconds(us).
  * @retval none
  */
void pwmout_pulsewidth_us(pwmout_t *obj, int us)
{
	assert_param(obj->pwm_idx < PWM_CHANNEL_MAX);

	obj->pulse = (float)us;

	PWM_SetDutySize(obj->pwm_idx, us / tick_src_unit[obj->pwmtimer_idx]);

	PWM_UpdateCtrl(obj->pwm_idx);
}

/**
  * @brief  Enable the specified channel to output PWM.
  * @param  obj: PWM object defined in application software.
  * @retval none
  */
void pwmout_start(pwmout_t *obj)
{
	PWM_Cmd(obj->pwm_idx, ENABLE);
}

/**
  * @brief  Disable the specified channel to output PWM.
  * @param  obj: PWM object defined in application software.
  * @retval none
  */
void pwmout_stop(pwmout_t *obj)
{
	PWM_Cmd(obj->pwm_idx, DISABLE);
}

/**
  * @brief  Pro3 doesn't support this feature
  */
void pwmout_set_polarity(pwmout_t *obj, int polarity)
{
	(void)obj;
	(void)polarity;
}

/** @} */
/** @} */
/** @} */


