#include "ameba_soc.h"
#include "ameba_pwm.h"

static const char *TAG = "PWM";

// from rtl8735_pwm.h
static PWM_TypeDef *PWM_BASE_TABLE[PWM_MAX_NUM] = {
	PWM0_BASE, PWM1_BASE, PWM2_BASE, PWM3_BASE,
	PWM4_BASE, PWM5_BASE, PWM6_BASE, PWM7_BASE
	// PWM8_BASE, PWM9_BASE, PWM10_BASE, PWM11_BASE
};

static PWM_TypeDef *PWM_S_BASE_TABLE[PWM_MAX_NUM] = {
	PWM0_S_BASE, PWM1_S_BASE, PWM2_S_BASE, PWM3_S_BASE,
	PWM4_S_BASE, PWM5_S_BASE, PWM6_S_BASE, PWM7_S_BASE
	// PWM8_S_BASE, PWM9_S_BASE, PWM10_S_BASE, PWM11_S_BASE
};

static u32 PWM_RegInitFlag = 0;


PWM_IRQ_FUN PWM_PeriodEndIrqHandler[PWM_MAX_NUM];
void *PWM_PeriodEndIrqData[PWM_MAX_NUM];

PWM_IRQ_FUN PWM_DutyAdjUpIrqHandler[PWM_MAX_NUM];
void *PWM_DutyAdjUpIrqData[PWM_MAX_NUM];

PWM_IRQ_FUN PWM_DutyAdjDnIrqHandler[PWM_MAX_NUM];
void *PWM_DutyAdjDnIrqData[PWM_MAX_NUM];


/**
 * @brief common PWM interrupt handler
 * @param
 */
void PWM_COMM_IRQHandler(void)
{
	PWM_COMM_TypeDef *PWM_COMM = PWM_DEV;

	if (TrustZone_IsSecure()) {
		PWM_COMM = PWM_DEV_S;
	}

	u32 clear_int_sts;

	u32 int_sts;
	u32 period_int = 0;
	u32 up_lim_int = 0;
	u32 down_lim_int = 0;

	period_int |= PWM_COMM_GET_PERIOD_END(PWM_COMM->INT_STATUS);
	up_lim_int |= PWM_COMM_GET_DUTY_ADJ_UP_LIM(PWM_COMM->INT_STATUS);
	down_lim_int |= PWM_COMM_DUTY_ADJ_DN_LIM(PWM_COMM->INT_STATUS);

	for (u8 i = 0; i < PWM_MAX_NUM; i++) {

		if (period_int & (1 << i)) {

			period_int &= ~(BIT(i));

			if (PWM_PeriodEndIrqHandler[i] != NULL) {
				PWM_PeriodEndIrqHandler[i](PWM_PeriodEndIrqData[i]);
			}

		}

		if (up_lim_int & (1 << i)) {

			up_lim_int &= ~(BIT(i));

			if (PWM_DutyAdjUpIrqHandler[i] != NULL) {
				PWM_DutyAdjUpIrqHandler[i](PWM_DutyAdjUpIrqData[i]);
			}

		}

		if (down_lim_int & (1 << i)) {

			down_lim_int &= ~(BIT(i));

			if (PWM_DutyAdjDnIrqHandler[i] != NULL) {
				PWM_DutyAdjDnIrqHandler[i](PWM_DutyAdjDnIrqData[i]);
			}

		}

		int_sts = (period_int | up_lim_int | down_lim_int);

		if (int_sts == 0) {
			break;
		}

	}

	clear_int_sts = PWM_COMM->INT_STATUS;		// for pwm 0~7
	PWM_COMM->INT_STATUS |= clear_int_sts;

}

/**
 * @brief   check the enabling status of corresponding PWM
 * @param   void
 * @return  enable status of pwm
 */
u32 PWM_COMM_EnableStatus(void)
{
	PWM_COMM_TypeDef *PWM_COMM = PWM_DEV;

	if (TrustZone_IsSecure()) {
		PWM_COMM = PWM_DEV_S;
	}

	return PWM_COMM->ENABLE_STATUS;
}

/**
 * @brief enable multiple pwm channels at the same time
 * @param pwm_multi_en
 */
void PWM_COMM_EnableCtrl(u16 pwm_multi_en)
{
	PWM_COMM_TypeDef *PWM_COMM = PWM_DEV;

	if (TrustZone_IsSecure()) {
		PWM_COMM = PWM_DEV_S;
	}

	PWM_COMM->ENABLE_CTRL &= ~(PWM_COMM_MASK_PWM_EN);
	PWM_COMM->ENABLE_CTRL |= PWM_COMM_PWM_EN(pwm_multi_en);
}

/**
 * @brief disable multiple pwm channels at the same time
 * @param pwm_multi_dis
 */
void PWM_COMM_DisableCtrl(u16 pwm_multi_dis)
{
	PWM_COMM_TypeDef *PWM_COMM = PWM_DEV;

	if (TrustZone_IsSecure()) {
		PWM_COMM = PWM_DEV_S;
	}

	PWM_COMM->DISABLE_CTRL &= ~(PWM_COMM_MASK_PWM_DIS);
	PWM_COMM->DISABLE_CTRL |= PWM_COMM_PWM_DIS(pwm_multi_dis);
}

/**
 * @brief get interrupt pending status for pwm channels 0-7 of the specific interrupt type
 * @param type can be one of the following parameters:
 * 		@arg PWM_INT_TYPE_PERIOD_END
 * 		@arg PWM_INT_TYPE_DUTY_ADJ_UP_LIMIT
 * 		@arg PWM_INT_TYPE_DUTY_ADJ_DOWN_LIMIT
 * @return pending interrupt status for pwm 0-7
 */
u16 PWM_COMM_IntPendingStatus(u32 type)
{
	assert_param(IS_PWM_INT_TYPE(type));

	PWM_COMM_TypeDef *PWM_COMM = PWM_DEV;

	if (TrustZone_IsSecure()) {
		PWM_COMM = PWM_DEV_S;
	}

	u16 pending_sts = 0;

	if (type == PWM_INT_TYPE_PERIOD_END) {
		pending_sts = PWM_COMM_GET_PERIOD_END(PWM_COMM->INT_STATUS);
	} else if (type == PWM_INT_TYPE_DUTY_ADJ_UP_LIMIT) {
		pending_sts = PWM_COMM_GET_DUTY_ADJ_UP_LIM(PWM_COMM->INT_STATUS);
	} else {
		pending_sts = PWM_COMM_GET_DUTY_ADJ_DN_LIM(PWM_COMM->INT_STATUS);
	}

	return pending_sts;
}

/**
 * @brief get interrupt pending status for pwm channels 8-11 of the specific type
 * @param type can be one of the following parameters:
 * 		@arg PWM_INT_TYPE_PERIOD_END
 * 		@arg PWM_INT_TYPE_DUTY_ADJ_UP_LIMIT
 * 		@arg PWM_INT_TYPE_DUTY_ADJ_DOWN_LIMIT
 * @return the pending interrupt status for pwm 8-11
 */
u16 PWM_COMM_IntPendingStatus1(u32 type)
{
	assert_param(IS_PWM_INT_TYPE(type));

	PWM_COMM_TypeDef *PWM_COMM = PWM_DEV;

	if (TrustZone_IsSecure()) {
		PWM_COMM = PWM_DEV_S;
	}

	u16 pending_sts = 0;

	if (type == PWM_INT_TYPE_PERIOD_END) {
		pending_sts = PWM_COMM_GET_PERIOD_END_010(PWM_COMM->INT_STATUS1);
	} else if (type == PWM_INT_TYPE_DUTY_ADJ_UP_LIMIT) {
		pending_sts = PWM_COMM_GET_DUTY_ADJ_UP_LIM_010(PWM_COMM->INT_STATUS1);
	} else {
		pending_sts = PWM_COMM_GET_DUTY_ADJ_DN_LIM_010(PWM_COMM->INT_STATUS1);
	}

	return pending_sts;
}

/**
 * @brief To read the time period of current duty of the PWM
 * @param pwm_sel
 * @return The PWM on duty duration
 */
u16 PWM_COMM_IndirectReadAdjDutySize(u8 pwm_sel)
{
	PWM_COMM_TypeDef *PWM_COMM = PWM_DEV;

	if (TrustZone_IsSecure()) {
		PWM_COMM = PWM_DEV_S;
	}

	PWM_COMM->INDREAD_IDX &= ~(PWM_COMM_MASK_PWM_SEL);
	PWM_COMM->INDREAD_IDX |= PWM_COMM_PWM_SEL(pwm_sel);

	PWM_COMM->INDREAD_IDX |= PWM_COMM_BIT_POOL;

	u32 i = 0;
	u16 pwm_duty = 0;

	while (((PWM_COMM->INDREAD_IDX & PWM_COMM_BIT_POOL) >> 7) == 1) {
		i++;

		if (i > 1000000) {
			RTK_LOGE("FPGA", "Wait indirect-read ready timeout\r\n");
			break;
		}
	}

	pwm_duty = PWM_COMM_GET_PWM_DUTY(PWM_COMM->INDREAD_DUTY);

	return pwm_duty;
}

/**
 * @brief enable complementary function
 * @param pwm_pair the selected pwm channel pair can be one of the following parameters:
 * 		@arg PWM_PAIR_0_1
 * 		@arg PWM_PAIR_2_3
 * 		@arg PWM_PAIR_4_5
 * 		@arg PWM_PAIR_6_7
 * 		@arg PWM_PAIR_8_9
 * 		@arg PWM_PAIR_A_B
 * @param db_val dead band value to be set, delay = (1 / sclk) * db_val
 * @param comp_en ENABLE / DISABLE compensation function
 */
void PWM_COMM_SetComplementary(u8 pwm_pair, u8 db_val, u8 comp_en)
{
	assert_param(IS_PWM_PAIR(pwm_pair));

	PWM_COMM_TypeDef *PWM_COMM = PWM_DEV;

	if (TrustZone_IsSecure()) {
		PWM_COMM = PWM_DEV_S;
	}

	if (comp_en == ENABLE) {
		PWM_COMM->COMPLEMENTARY |= BIT(pwm_pair);
		PWM_COMM->COMPLEMENTARY &= ~(PWM_COMM_PWM_DB((0xF << (pwm_pair * 4))));
		PWM_COMM->COMPLEMENTARY |= PWM_COMM_PWM_DB(((db_val & 0xF) << (pwm_pair * 4)));
	} else {
		PWM_COMM->COMPLEMENTARY &= ~(BIT(pwm_pair));
		PWM_COMM->COMPLEMENTARY &= ~(PWM_COMM_PWM_DB((0xF << (pwm_pair * 4))));
	}
}

/**
 * @brief Register/Hook up PWM common interrupt handler
 * @param
 */
void PWM_COMM_IRQInit(void)
{
	InterruptDis(PWM_IRQ);
	InterruptRegister((IRQ_FUN)PWM_COMM_IRQHandler, PWM_IRQ, (u32)NULL, INT_PRI_MIDDLE);
	InterruptEn(PWM_IRQ, INT_PRI_HIGH);
}

/**
 * @brief Unregister/Remove PWM common interrupt handler
 * @param
 */
void PWM_COMM_IRQDeInit(void)
{
	InterruptDis(PWM_IRQ);
	InterruptUnRegister(PWM_IRQ);
}


/**
 * @brief   update pwm setting
 * @param   pwm_num the selected pwm
 * @note 	if pwm is disabled, HW will not clear this bit
 * @note 	if auto duty adj is enabled and this bit is asserted,
 * 			HW updates its duty at 1st pwm period and auto duty adj will
 * 			start at the 2nd pwm period
 */
void PWM_UpdateCtrl(u8 pwm_num)
{
	PWM_TypeDef *PWMx = (PWM_TypeDef *)PWM_BASE_TABLE[pwm_num];

	if (TrustZone_IsSecure()) {
		PWMx = PWM_S_BASE_TABLE[pwm_num];
	}

	PWMx->CTRL |= PWM_BIT_CTRL_SET;
}

/**
 * @brief set tick source (gtimer 0 timers only), can be 0-7 or sclk
 * @param pwm_num the select pwm channel
 * @param tick_src can be one of the following parameters
 * 		@arg PWM_TICK_SOURCE_TIMER0
 * 		@arg PWM_TICK_SOURCE_TIMER1
 *		@arg PWM_TICK_SOURCE_TIMER2
 * 		@arg PWM_TICK_SOURCE_TIMER3
 * 		@arg PWM_TICK_SOURCE_TIMER4
 * 		@arg PWM_TICK_SOURCE_TIMER5
 * 		@arg PWM_TICK_SOURCE_TIMER6
 * 		@arg PWM_TICK_SOURCE_TIMER7
 * 		@arg PWM_TICK_SOURCE_SCLK
 */
void PWM_SetTickSource(u8 pwm_num, u32 tick_src)
{
	assert_param(IS_PWM_TICK_SOURCE(tick_src));

	PWM_TypeDef *PWMx = (PWM_TypeDef *)PWM_BASE_TABLE[pwm_num];

	if (TrustZone_IsSecure()) {
		PWMx = PWM_S_BASE_TABLE[pwm_num];
	}

	PWMx->CTRL &= ~(PWM_MASK_CLK_SEL);
	PWMx->CTRL |= PWM_CLK_SEL(tick_src);
}


/**
 * @brief   set duty period size
 * @param   pwm_num     the selected pwm
 * @param   duty_size   duty time period in terms of Gtimer timeout duration
 */
void PWM_SetDutySize(u8 pwm_num, u16 duty_size)
{
	PWM_TypeDef *PWMx = (PWM_TypeDef *)PWM_BASE_TABLE[pwm_num];

	if (TrustZone_IsSecure()) {
		PWMx = PWM_S_BASE_TABLE[pwm_num];
	}

	PWMx->CTRL &= ~(PWM_MASK_DUTY);
	PWMx->CTRL |= PWM_DUTY(duty_size);
}

/**
 * @brief   set the start of ON duration of PWM pulse in the period
 * @param   pwm_num     the selected pwm
 * @param   offset      duty start offset in terms of Gtimer timeout duration
 */
void PWM_SetDutyStartOffset(u8 pwm_num, u16 offset)
{
	PWM_TypeDef *PWMx = (PWM_TypeDef *)PWM_BASE_TABLE[pwm_num];

	if (TrustZone_IsSecure()) {
		PWMx = PWM_S_BASE_TABLE[pwm_num];
	}

	PWMx->TIMING_CTRL &= ~(PWM_MASK_DUTY_START);
	PWMx->TIMING_CTRL |= PWM_DUTY_START(offset);

}

/**
 * @brief set pwm duty period
 * @param pwm_num   the selected pwm
 * @param unit_time period duration in terms of Gtimer timeout duration
 */
void PWM_SetPeriod(u8 pwm_num, u16 unit_time)
{
	PWM_TypeDef *PWMx = (PWM_TypeDef *)PWM_BASE_TABLE[pwm_num];

	if (TrustZone_IsSecure()) {
		PWMx = PWM_S_BASE_TABLE[pwm_num];
	}

	PWMx->TIMING_CTRL &= ~(PWM_MASK_PERIOD_VAL);
	PWMx->TIMING_CTRL |= PWM_PERIOD_VAL(unit_time);
}

/**
 * @brief pause pwm, pwm will stop at the end of the current period
 * @param pwm_num the selected pwm
 * @param state 0: un-pause, 1: pause
 * @note pwm output might not stop immediately, can call PWM_Status() to
 * 		 check if pwm is paused
 */
void PWM_PauseCtrl(u8 pwm_num, u32 state)
{
	PWM_TypeDef *PWMx = (PWM_TypeDef *)PWM_BASE_TABLE[pwm_num];

	if (TrustZone_IsSecure()) {
		PWMx = PWM_S_BASE_TABLE[pwm_num];
	}

	if (state == ENABLE) {
		PWMx->CTRL |= PWM_BIT_PAUSE;
	} else {
		PWMx->CTRL &= ~(PWM_BIT_PAUSE);
	}
}

/**
 * @brief get the status of pwm
 * @param pwm_num the selected pwm
 * @return current status of pwm
 *      @arg 0: PWM_IS_PAUSED
 *      @arg 1: PWM_IS_RUNNING
 */
u32 PWM_Status(u8 pwm_num)
{
	PWM_TypeDef *PWMx = (PWM_TypeDef *)PWM_BASE_TABLE[pwm_num];

	if (TrustZone_IsSecure()) {
		PWMx = PWM_S_BASE_TABLE[pwm_num];
	}

	u32 sts;

	if ((PWMx->CTRL & PWM_BIT_RUN_STS) != 0) {
		sts = PWM_IS_RUNNING;
	} else {
		sts = PWM_IS_PAUSED;
	}

	return sts;
}

/**
 * @brief set pwm period end interrupt
 * @param pwm_num the selected pwm
 * @param state 0: disable, 1: enable
 */
void PWM_SetPeriodEndInt(u8 pwm_num, u32 state)
{
	PWM_TypeDef *PWMx = (PWM_TypeDef *)PWM_BASE_TABLE[pwm_num];

	if (TrustZone_IsSecure()) {
		PWMx = PWM_S_BASE_TABLE[pwm_num];
	}

	if (state == ENABLE) {
		PWMx->CTRL |= PWM_BIT_PERIOD_IE;
	} else {
		PWMx->CTRL &= ~(PWM_BIT_PERIOD_IE);
	}
}

/**
 * @brief set pwm auto adjustment up limit
 * @param pwm_num the selected pwm
 * @param up_limit up limit value in terms of Gtimer timeout duration
 */
void PWM_SetAdjUpLimit(u8 pwm_num, u16 up_limit)
{
	PWM_TypeDef *PWMx = (PWM_TypeDef *)PWM_BASE_TABLE[pwm_num];

	if (TrustZone_IsSecure()) {
		PWMx = PWM_S_BASE_TABLE[pwm_num];
	}

	PWMx->AUTO_ADJ_LIMIT &= ~(PWM_MASK_DUTY_ADJ_UP_LIM);
	PWMx->AUTO_ADJ_LIMIT |= PWM_DUTY_ADJ_UP_LIM(up_limit);
}

/**
 * @brief set pwm audo adjustment down limit
 * @param pwm_num the selected pwm
 * @param down_limit down limit value in terms of Gtimer timeout duration
 */
void PWM_SetAdjDownLimit(u8 pwm_num, u16 down_limit)
{
	PWM_TypeDef *PWMx = (PWM_TypeDef *)PWM_BASE_TABLE[pwm_num];

	if (TrustZone_IsSecure()) {
		PWMx = PWM_S_BASE_TABLE[pwm_num];
	}

	PWMx->AUTO_ADJ_LIMIT &= ~(PWM_MASK_DUTY_ADJ_DN_LIM);
	PWMx->AUTO_ADJ_LIMIT |= PWM_DUTY_ADJ_DN_LIM(down_limit);
}

/**
 * @brief set pwm auto adjustment increasing step size
 * @param pwm_num the selected pwm
 * @param step_size in terms of Gtimer timeout duration
 */
void PWM_SetAdjIncSize(u8 pwm_num, u16 step_size)
{
	PWM_TypeDef *PWMx = (PWM_TypeDef *)PWM_BASE_TABLE[pwm_num];

	if (TrustZone_IsSecure()) {
		PWMx = PWM_S_BASE_TABLE[pwm_num];
	}

	PWMx->AUTO_ADJ_CTRL &= ~(PWM_MASK_DUTY_INC_STEP);
	PWMx->AUTO_ADJ_CTRL |= PWM_DUTY_INC_STEP(step_size);
}

/**
 * @brief set pwm auto adjustment decreasing step size
 * @param pwm_num the selected pwm
 * @param step_size in terms of Gtimer timeout duration
 */
void PWM_SetAdjDecSize(u8 pwm_num, u16 step_size)
{
	PWM_TypeDef *PWMx = (PWM_TypeDef *)PWM_BASE_TABLE[pwm_num];

	if (TrustZone_IsSecure()) {
		PWMx = PWM_S_BASE_TABLE[pwm_num];
	}

	PWMx->AUTO_ADJ_CTRL &= ~(PWM_MASK_DUTY_DEC_STEP);
	PWMx->AUTO_ADJ_CTRL |= PWM_DUTY_DEC_STEP(step_size);
}

/**
 * @brief set pwm auto adjustment cycle count, duty size will be adjusted after every cycle_cnt
 * @param pwm_num the selected pwm
 * @param cycle_cnt number of cycle before duty size will be adjusted
 */
void PWM_SetAdjCycleCnt(u8 pwm_num, u16 cycle_cnt)
{
	PWM_TypeDef *PWMx = (PWM_TypeDef *)PWM_BASE_TABLE[pwm_num];

	if (TrustZone_IsSecure()) {
		PWMx = PWM_S_BASE_TABLE[pwm_num];
	}

	PWMx->AUTO_ADJ_CYCLE &= ~(PWM_MASK_ADJ_CYCLES);
	PWMx->AUTO_ADJ_CYCLE |= PWM_ADJ_CYCLES(cycle_cnt - 1);

}

/**
 * @brief enable pwm duty auto adjustment mode
 * @param pwm_num the selected pwm
 * @param state 0: DISABLE, 1: ENABLE
 */
void PWM_DutyAdjEnable(u8 pwm_num, u32 state)
{
	PWM_TypeDef *PWMx = (PWM_TypeDef *)PWM_BASE_TABLE[pwm_num];

	if (TrustZone_IsSecure()) {
		PWMx = PWM_S_BASE_TABLE[pwm_num];
	}

	if (state == ENABLE) {
		PWMx->AUTO_ADJ_CTRL |= PWM_BIT_ADJ_EN;
	} else {
		PWMx->AUTO_ADJ_CTRL &= ~(PWM_BIT_ADJ_EN);
	}
}

/**
 * @brief set initial pwm auto adjustment start direction
 * @param pwm_num the selected pwm
 * @param init_dir 0: decrement, 1: increment
 */
void PWM_SetDutyAdjDir(u8 pwm_num, u32 init_dir)
{
	PWM_TypeDef *PWMx = (PWM_TypeDef *)PWM_BASE_TABLE[pwm_num];

	if (TrustZone_IsSecure()) {
		PWMx = PWM_S_BASE_TABLE[pwm_num];
	}

	if (init_dir == PWM_DUTY_ADJ_INCREMENT) {
		PWMx->AUTO_ADJ_CTRL |= PWM_BIT_ADJ_DIR;
	} else {
		PWMx->AUTO_ADJ_CTRL &= ~(PWM_BIT_ADJ_DIR);
	}
}

/**
 * @brief set pwm auto adjustment loop mode
 * @param pwm_num the selected pwm
 * @param state 0: DISABLE, 1: ENABLE
 */
void PWM_SetDutyAdjLoopMode(u8 pwm_num, u32 state)
{
	PWM_TypeDef *PWMx = (PWM_TypeDef *)PWM_BASE_TABLE[pwm_num];

	if (TrustZone_IsSecure()) {
		PWMx = PWM_S_BASE_TABLE[pwm_num];
	}

	if (state == ENABLE) {
		PWMx->AUTO_ADJ_CTRL |= PWM_BIT_ADJ_LOOP_EN;
	} else {
		PWMx->AUTO_ADJ_CTRL &= ~(PWM_BIT_ADJ_LOOP_EN);
	}
}

/**
 * @brief set pwm auto adjustment up limit interrupt
 * @param pwm_num the selected pwm
 * @param state 0: DISABLE, 1: ENABLE
 */
void PWM_SetAdjUpLimitInt(u8 pwm_num, u32 state)
{
	PWM_TypeDef *PWMx = (PWM_TypeDef *)PWM_BASE_TABLE[pwm_num];

	if (TrustZone_IsSecure()) {
		PWMx = PWM_S_BASE_TABLE[pwm_num];
	}

	if (state == ENABLE) {
		PWMx->AUTO_ADJ_CTRL |= PWM_BIT_DUTY_UP_LIM_IE;
	} else {
		PWMx->AUTO_ADJ_CTRL &= ~(PWM_BIT_DUTY_UP_LIM_IE);
	}
}

/**
 * @brief set pwm auto adjustment down limit interrupt
 * @param pwm_num the selected pwm
 * @param state 0: DISABLE, 1: ENABLE
 */
void PWM_SetAdjDownLimitInt(u8 pwm_num, u32 state)
{
	PWM_TypeDef *PWMx = (PWM_TypeDef *)PWM_BASE_TABLE[pwm_num];

	if (TrustZone_IsSecure()) {
		PWMx = PWM_S_BASE_TABLE[pwm_num];
	}

	if (state == ENABLE) {
		PWMx->AUTO_ADJ_CTRL |= PWM_BIT_DUTY_DN_LIM_IE;
	} else {
		PWMx->AUTO_ADJ_CTRL &= ~(PWM_BIT_DUTY_DN_LIM_IE);
	}
}

// for verification purpose

void PWM_AutoDutyAdjInc(u8 pwm_num, u32 up_limit, u32 step_size, u32 cycle_cnt)
{
	PWM_TypeDef *PWMx = (PWM_TypeDef *)PWM_BASE_TABLE[pwm_num];

	if (TrustZone_IsSecure()) {
		PWMx = PWM_S_BASE_TABLE[pwm_num];
	}

	PWMx->AUTO_ADJ_CTRL |= PWM_BIT_ADJ_DIR;

	PWMx->AUTO_ADJ_CTRL &= ~(PWM_BIT_ADJ_LOOP_EN);

	PWMx->AUTO_ADJ_LIMIT &= ~(PWM_MASK_DUTY_ADJ_UP_LIM);
	PWMx->AUTO_ADJ_LIMIT |= PWM_DUTY_ADJ_UP_LIM(up_limit);

	PWMx->AUTO_ADJ_CTRL &= ~(PWM_MASK_DUTY_INC_STEP);
	PWMx->AUTO_ADJ_CTRL |= PWM_DUTY_INC_STEP(step_size);

	PWMx->AUTO_ADJ_CYCLE &= ~(PWM_MASK_ADJ_CYCLES);
	PWMx->AUTO_ADJ_CYCLE |= PWM_ADJ_CYCLES(cycle_cnt - 1); 		// HW cycle count = cycle count - 1;

}

void PWM_AutoDutyAdjDec(u8 pwm_num, u32 down_limit, u32 step_size, u32 cycle_cnt)
{
	PWM_TypeDef *PWMx = (PWM_TypeDef *)PWM_BASE_TABLE[pwm_num];

	if (TrustZone_IsSecure()) {
		PWMx = PWM_S_BASE_TABLE[pwm_num];
	}

	PWMx->AUTO_ADJ_CTRL &= ~(PWM_BIT_ADJ_DIR);

	PWMx->AUTO_ADJ_CTRL &= ~(PWM_BIT_ADJ_LOOP_EN);

	PWMx->AUTO_ADJ_LIMIT &= ~(PWM_MASK_DUTY_ADJ_DN_LIM);
	PWMx->AUTO_ADJ_LIMIT |= PWM_DUTY_ADJ_DN_LIM(down_limit);

	PWMx->AUTO_ADJ_CTRL &= ~(PWM_MASK_DUTY_DEC_STEP);
	PWMx->AUTO_ADJ_CTRL |= PWM_DUTY_DEC_STEP(step_size);

	PWMx->AUTO_ADJ_CYCLE &= ~(PWM_MASK_ADJ_CYCLES);
	PWMx->AUTO_ADJ_CYCLE |= PWM_ADJ_CYCLES(cycle_cnt - 1);		// HW cycle count = cycle count - 1;

}

void PWM_AutoDutyAdjLoop(
	u8 pwm_num,
	u32 init_dir,
	u32 up_limit,
	u32 down_limit,
	u32 inc_step_size,
	u32 dec_step_size,
	u32 cycle_cnt)
{
	PWM_TypeDef *PWMx = (PWM_TypeDef *)PWM_BASE_TABLE[pwm_num];

	if (TrustZone_IsSecure()) {
		PWMx = PWM_S_BASE_TABLE[pwm_num];
	}

	if (init_dir == PWM_DUTY_ADJ_INCREMENT) {
		PWMx->AUTO_ADJ_CTRL |= PWM_BIT_ADJ_DIR;
	} else {
		PWMx->AUTO_ADJ_CTRL &= ~(PWM_BIT_ADJ_DIR);
	}

	PWMx->AUTO_ADJ_CTRL |= PWM_BIT_ADJ_LOOP_EN;

	PWMx->AUTO_ADJ_LIMIT &= ~(PWM_MASK_DUTY_ADJ_UP_LIM);
	PWMx->AUTO_ADJ_LIMIT |= PWM_DUTY_ADJ_UP_LIM(up_limit);

	PWMx->AUTO_ADJ_LIMIT &= ~(PWM_MASK_DUTY_ADJ_DN_LIM);
	PWMx->AUTO_ADJ_LIMIT |= PWM_DUTY_ADJ_DN_LIM(down_limit);

	PWMx->AUTO_ADJ_CTRL &= ~(PWM_MASK_DUTY_INC_STEP);
	PWMx->AUTO_ADJ_CTRL |= PWM_DUTY_INC_STEP(inc_step_size);

	PWMx->AUTO_ADJ_CTRL &= ~(PWM_MASK_DUTY_DEC_STEP);
	PWMx->AUTO_ADJ_CTRL |= PWM_DUTY_DEC_STEP(dec_step_size);

	PWMx->AUTO_ADJ_CYCLE &= ~(PWM_MASK_ADJ_CYCLES);
	PWMx->AUTO_ADJ_CYCLE |= PWM_ADJ_CYCLES(cycle_cnt - 1);		// HW cycle count = cycle count - 1;
}

/**
 * @brief Init and Enable DMA TX GDMA.
 * @param pwm_num 0 to 3
 * @param GDMA_InitStruct pointer to a GDMA_InitTypeDef structure that contains
 *         the configuration information for the GDMA peripheral.
 * @param CallbackData GDMA callback data.
 * @param CallbackFunc GDMA callback function
 * @param pTXData Tx buffer
 * @param Length Tx count
 * @param SecureTransfer enable of disable gdma security transfer
 * @return TRUE / FALSE
 */
bool PWM_TXGDMA_Init(
	u8 pwm_num,
	GDMA_InitTypeDef *GDMA_InitStruct,
	void *CallbackData,
	IRQ_FUN CallbackFunc,
	u8 *pTXData,
	u32 Length)
{
	PWM_TypeDef *PWMx = (PWM_TypeDef *)PWM_BASE_TABLE[pwm_num];

	if (TrustZone_IsSecure()) {
		PWMx = PWM_S_BASE_TABLE[pwm_num];
	}

	// PWMx->OUT_DMA &= ~(PWM_BIT_CHG_DUTY | PWM_BIT_CHG_PERIOD);
	// PWMx->OUT_DMA |= dma_mode;

	u8 GdmaChnl;
	u8 GdmaIdx = 0;

	DCache_CleanInvalidate((u32)pTXData, Length);

	GdmaChnl = GDMA_ChnlAlloc(GdmaIdx, CallbackFunc, (u32)CallbackData, INT_PRI_MIDDLE);

	if (GdmaChnl == 0xFF) {
		// No Available DMA channel
		return _FALSE;
	}

	// _memset((void *)GDMA_InitStruct, 0, sizeof(GDMA_InitTypeDef));

	// GDMA_StructInit(GDMA_InitStruct);

	// if (SecureTransfer == ENABLE) {
	// 	GDMA_InitStruct->SecureTransfer = 0x1;
	// } else {
	// 	GDMA_InitStruct->SecureTransfer = 0x0;
	// }


	/*set GDMA initial structure member value*/
	GDMA_InitStruct->MuliBlockCunt = 0;
	GDMA_InitStruct->GDMA_ReloadSrc = 0;
	GDMA_InitStruct->MaxMuliBlock = 1;

	GDMA_InitStruct->GDMA_DIR = TTFCMemToPeri;

	if (pwm_num == 0) {
		GDMA_InitStruct->GDMA_DstHandshakeInterface = GDMA0_HANDSHAKE_INTERFACE_PWM0_TX;
	} else if (pwm_num == 1) {
		GDMA_InitStruct->GDMA_DstHandshakeInterface = GDMA0_HANDSHAKE_INTERFACE_PWM1_TX;
	} else if (pwm_num == 2) {
		GDMA_InitStruct->GDMA_DstHandshakeInterface = GDMA0_HANDSHAKE_INTERFACE_PWM2_TX;
	} else if (pwm_num == 3) {
		GDMA_InitStruct->GDMA_DstHandshakeInterface = GDMA0_HANDSHAKE_INTERFACE_PWM3_TX;
	} else {
		RTK_LOGE(TAG, "Does not support DMA transfer\r\n");
	}

	GDMA_InitStruct->GDMA_DstAddr = (u32)&PWMx->DMA_BUFF;

	GDMA_InitStruct->GDMA_Index = GdmaIdx;
	GDMA_InitStruct->GDMA_ChNum = GdmaChnl;
	GDMA_InitStruct->GDMA_IsrType = (BlockType | TransferType | ErrType);
	GDMA_InitStruct->GDMA_DstMsize  = MsizeFour;
	GDMA_InitStruct->GDMA_DstDataWidth = TrWidthOneByte;
	GDMA_InitStruct->GDMA_DstInc = NoChange;
	GDMA_InitStruct->GDMA_SrcInc = IncType;

	/*	Cofigure GDMA transfer */
	/*	24bits or 16bits mode */
	if (((Length & 0x03) == 0) && (((u32)(pTXData) & 0x03) == 0)) {
		/*	4-bytes aligned, move 4 bytes each transfer */
		GDMA_InitStruct->GDMA_SrcMsize	 = MsizeFour;
		GDMA_InitStruct->GDMA_SrcDataWidth = TrWidthFourBytes;
		GDMA_InitStruct->GDMA_BlockSize = Length >> 2;
	} else if (((Length & 0x01) == 0) && (((u32)(pTXData) & 0x01) == 0)) {
		/*	2-bytes aligned, move 2 bytes each transfer */
		GDMA_InitStruct->GDMA_SrcMsize	 = MsizeEight;
		GDMA_InitStruct->GDMA_SrcDataWidth = TrWidthTwoBytes;
		GDMA_InitStruct->GDMA_BlockSize = Length >> 1;
	} else {
		RTK_LOGE(TAG, "PWM DMA: Aligment Err: pTXData=%p,  Length=%lu\n", pTXData, Length);
		GDMA_ChnlFree(GDMA_InitStruct->GDMA_Index, GDMA_InitStruct->GDMA_ChNum);
		return _FALSE;
	}
	GDMA_InitStruct->GDMA_DstMsize = MsizeFour;
	GDMA_InitStruct->GDMA_DstDataWidth = TrWidthFourBytes;

	/*configure GDMA source address */
	GDMA_InitStruct->GDMA_SrcAddr = (u32)pTXData;


	/*	Enable GDMA for TX */
	GDMA_Init(GDMA_InitStruct->GDMA_Index, GDMA_InitStruct->GDMA_ChNum, GDMA_InitStruct);
	GDMA_Cmd(GDMA_InitStruct->GDMA_Index, GDMA_InitStruct->GDMA_ChNum, ENABLE);


	// RTK_LOGD(TAG, "TXGDMA_End\n");
	return _TRUE;
}

/**
 * @brief Enable / Disable DMA
 * @param pwm_num 0 to 3
 * @param dma_mode to update duty or period after each period-end interrupt
 *  	This parameter can be one of the following parameters:
 * 		@arg PWM_DMA_CHG_DUTY
 * 		@arg PWM_DMA_CHG_PERIOD
 * 		@arg PWM_DMA_CHG_DUTY_PERIOD
 * @param NewState ENABLE / DISABLE
 */
void PWM_SetDmaEnable(u8 pwm_num, u8 dma_mode, u32 NewState)
{
	assert_param(IS_PWM_DMA_SUPPORT(pwm_num));
	assert_param(IS_PWM_DMA_MODE(dma_mode));

	PWM_TypeDef *PWMx = (PWM_TypeDef *)PWM_BASE_TABLE[pwm_num];

	PWMx = PWM_BASE_TABLE[pwm_num];

	if (TrustZone_IsSecure()) {
		PWMx = PWM_S_BASE_TABLE[pwm_num];
	}

	PWMx->OUT_DMA &= ~(PWM_BIT_CHG_DUTY | PWM_BIT_CHG_PERIOD);
	PWMx->OUT_DMA |= dma_mode;

	if (NewState == ENABLE) {
		PWMx->OUT_DMA |= PWM_BIT_DMA_MODE;
	} else {
		PWMx->OUT_DMA &= ~(PWM_BIT_DMA_MODE);
	}


}

// register PWM_IRQ
void PWM_RegPeriodEndInt(u8 pwm_num, PWM_IRQ_FUN irq_func, void *irq_data)
{
	PWM_PeriodEndIrqHandler[pwm_num] = irq_func;
	PWM_PeriodEndIrqData[pwm_num] = irq_data;
}

void PWM_RegDutyUpLimInt(u8 pwm_num, PWM_IRQ_FUN irq_func, void *irq_data)
{
	PWM_DutyAdjUpIrqHandler[pwm_num] = irq_func;
	PWM_DutyAdjUpIrqData[pwm_num] = irq_data;
}

void PWM_RegDutyDnLimInt(u8 pwm_num, PWM_IRQ_FUN irq_func, void *irq_data)
{
	PWM_DutyAdjDnIrqHandler[pwm_num] = irq_func;
	PWM_DutyAdjDnIrqData[pwm_num] = irq_data;
}

// unregister PWM_IRQ
void PWM_UnRegPeriodEndInt(u8 pwm_num)
{
	PWM_PeriodEndIrqHandler[pwm_num] = NULL;
	PWM_PeriodEndIrqData[pwm_num] = NULL;
}

void PWM_UnRegDutyUpLimInt(u8 pwm_num)
{
	PWM_DutyAdjUpIrqHandler[pwm_num] = NULL;
	PWM_DutyAdjUpIrqData[pwm_num] = NULL;
}

void PWM_UnRegDutyDnLimInt(u8 pwm_num)
{
	PWM_DutyAdjDnIrqHandler[pwm_num] = NULL;
	PWM_DutyAdjDnIrqData[pwm_num] = NULL;
}

/**
 * @brief enable/disable specific pwm channel
 * @param pwm_num 	pwm channel to be enable/disable
 * @param NewState 	ENABLE or DISABLE
 * @note call PWM_COMM_EnableCtrl() or PWM_COMM_DisableCtrl() to enable/disable
 * 		 multiple pwm channels at the same time
 */
void PWM_Cmd(u8 pwm_num, u32 NewState)
{
	PWM_COMM_TypeDef *PWM_COMM = PWM_DEV;

	if (TrustZone_IsSecure()) {
		PWM_COMM = PWM_DEV_S;
	}

	if (NewState == ENABLE) {
		PWM_COMM->ENABLE_CTRL &= ~(PWM_COMM_MASK_PWM_EN);
		PWM_COMM->ENABLE_CTRL |= PWM_COMM_PWM_EN(BIT(pwm_num));
	} else {
		PWM_COMM->DISABLE_CTRL &= ~(PWM_COMM_MASK_PWM_DIS);
		PWM_COMM->DISABLE_CTRL |= PWM_COMM_PWM_DIS(BIT(pwm_num));
	}

}

/**
 * @brief enable/disable pwm interrupt type
 * @param pwm_num 	the select pwm channel
 * @param int_type 	interrupt type to enable/disable
 * 		@arg PWM_INT_TYPE_PERIOD_END
 * 		@arg PWM_INT_TYPE_DUTY_ADJ_UP_LIMIT
 * 		@arg PWM_INT_TYPE_DUTY_ADJ_DOWN_LIMIT
 * @param NewState ENABLE or DISABLE
 */
void PWM_INTConfig(u8 pwm_num, u32 int_type, u32 NewState)
{
	PWM_TypeDef *PWMx = (PWM_TypeDef *)PWM_BASE_TABLE[pwm_num];

	if (TrustZone_IsSecure()) {
		PWMx = PWM_S_BASE_TABLE[pwm_num];
	}

	if (NewState == ENABLE) {
		if (int_type == PWM_INT_TYPE_PERIOD_END) {
			PWMx->CTRL |= PWM_BIT_PERIOD_IE;
		} else if (int_type == PWM_INT_TYPE_DUTY_ADJ_UP_LIMIT) {
			PWMx->AUTO_ADJ_CTRL |= PWM_BIT_DUTY_UP_LIM_IE;
		} else {
			PWMx->AUTO_ADJ_CTRL |= PWM_BIT_DUTY_DN_LIM_IE;
		}
	} else {
		if (int_type == PWM_INT_TYPE_PERIOD_END) {
			PWMx->CTRL &= ~(PWM_BIT_PERIOD_IE);
		} else if (int_type == PWM_INT_TYPE_DUTY_ADJ_UP_LIMIT) {
			PWMx->AUTO_ADJ_CTRL &= ~(PWM_BIT_DUTY_UP_LIM_IE);
		} else {
			PWMx->AUTO_ADJ_CTRL &= ~(PWM_BIT_DUTY_DN_LIM_IE);
		}
	}
}

/**
 * @brief Clear all pending interrupt for specific pwm channel
 * @param pwm_num 	selected pwm channel
 */
void PWM_INTClear(u8 pwm_num)
{
	// clear all interrupt
	PWM_COMM_TypeDef *PWM_COMM = PWM_DEV;

	if (TrustZone_IsSecure()) {
		PWM_COMM = PWM_DEV_S;
	}

	if (pwm_num < 8) {
		PWM_COMM->INT_STATUS |= PWM_COMM_PERIOD_END(BIT(pwm_num));
		PWM_COMM->INT_STATUS |= PWM_COMM_DUTY_ADJ_UP_LIM(BIT(pwm_num));
		PWM_COMM->INT_STATUS |= PWM_COMM_DUTY_ADJ_DN_LIM(BIT(pwm_num));
	} else {
		pwm_num -= 8;
		PWM_COMM->INT_STATUS1 |= PWM_COMM_PERIOD_END_010(BIT(pwm_num));
		PWM_COMM->INT_STATUS1 |= PWM_COMM_DUTY_ADJ_UP_LIM_010(BIT(pwm_num));
		PWM_COMM->INT_STATUS1 |= PWM_COMM_DUTY_ADJ_DN_LIM_010(BIT(pwm_num));
	}
}

/**
 * @brief Clear the specific pending interrupt bit of a particular pwm channel
 * @param pwm_num 	select pwm channel
 * @param pending_bit 	interrupt bit to be cleared
 * 		@arg PWM_INT_TYPE_PERIOD_END
 * 		@arg PWM_INT_TYPE_DUTY_ADJ_UP_LIMIT
 * 		@arg PWM_INT_TYPE_DUTY_ADJ_DOWN_LIMIT
 */
void PWM_INTClearPendingBit(u8 pwm_num, u32 pending_bit)
{
	PWM_COMM_TypeDef *PWM_COMM = PWM_DEV;

	if (TrustZone_IsSecure()) {
		PWM_COMM = PWM_DEV_S;
	}

	if (pwm_num < 8) {
		// clear period end
		if (pending_bit == PWM_INT_TYPE_PERIOD_END) {
			PWM_COMM->INT_STATUS |= PWM_COMM_PERIOD_END(BIT(pwm_num));
		} else if (pending_bit == PWM_INT_TYPE_DUTY_ADJ_UP_LIMIT) {
			PWM_COMM->INT_STATUS |= PWM_COMM_DUTY_ADJ_UP_LIM(BIT(pwm_num));
		} else {
			PWM_COMM->INT_STATUS |= PWM_COMM_DUTY_ADJ_DN_LIM(BIT(pwm_num));
		}
	} else {
		pwm_num -= 8;
		if (pending_bit == PWM_INT_TYPE_PERIOD_END) {
			PWM_COMM->INT_STATUS1 |= PWM_COMM_PERIOD_END(BIT(pwm_num));
		} else if (pending_bit == PWM_INT_TYPE_DUTY_ADJ_UP_LIMIT) {
			PWM_COMM->INT_STATUS1 |= PWM_COMM_DUTY_ADJ_UP_LIM(BIT(pwm_num));
		} else {
			PWM_COMM->INT_STATUS1 |= PWM_COMM_DUTY_ADJ_DN_LIM(BIT(pwm_num));
		}
	}
}

/**
 * @brief get interrupt status of specific interrupt type for specific pwm channel
 * @param pwm_num the selected pwm channel
 * @param int_type interrupt type
 * 		@arg PWM_INT_TYPE_PERIOD_END
 * 		@arg PWM_INT_TYPE_DUTY_ADJ_UP_LIMIT
 * 		@arg PWM_INT_TYPE_DUTY_ADJ_DOWN_LIMIT
 * @return
 */
u32 PWM_GetINTStatus(u8 pwm_num, u32 int_type)
{
	PWM_COMM_TypeDef *PWM_COMM = PWM_DEV;

	if (TrustZone_IsSecure()) {
		PWM_COMM = PWM_DEV_S;
	}

	u32 int_sts;

	if (pwm_num < 8) {
		// clear period end
		if (int_type == PWM_INT_TYPE_PERIOD_END) {
			int_sts = (PWM_COMM->INT_STATUS | PWM_COMM_PERIOD_END(BIT(pwm_num)));
		} else if (int_type == PWM_INT_TYPE_DUTY_ADJ_UP_LIMIT) {
			int_sts = (PWM_COMM->INT_STATUS | PWM_COMM_DUTY_ADJ_UP_LIM(BIT(pwm_num)));
		} else {
			int_sts = (PWM_COMM->INT_STATUS | PWM_COMM_DUTY_ADJ_DN_LIM(BIT(pwm_num)));
		}
	} else {
		pwm_num -= 8;
		if (int_type == PWM_INT_TYPE_PERIOD_END) {
			int_sts = (PWM_COMM->INT_STATUS1 | PWM_COMM_PERIOD_END_010(BIT(pwm_num)));
		} else if (int_type == PWM_INT_TYPE_DUTY_ADJ_UP_LIMIT) {
			int_sts = (PWM_COMM->INT_STATUS1 | PWM_COMM_DUTY_ADJ_UP_LIM_010(BIT(pwm_num)));
		} else {
			int_sts = (PWM_COMM->INT_STATUS1 | PWM_COMM_DUTY_ADJ_DN_LIM_010(BIT(pwm_num)));
		}
	}

	return int_sts;
}

/**
 * @brief Initializes the parameters in the PWM_InitStruct with its default value
 * @param PWM_InitStruct pointer to a PWM_InitTypeDef structure which will be
 *         initialized.
 */
void PWM_StructInit(PWM_InitTypeDef *PWM_InitStruct)
{
	// PWM_InitStruct->PWM_Index = 0;
	PWM_InitStruct->PWM_Period = 0;
	PWM_InitStruct->PWM_TickSource = 0;
	PWM_InitStruct->PWM_DutySize = 0;
}

/**
 * @brief Init pwm  according to the specified
 *        parameters in the PWM_InitStruct
 * @param pwm_num 	the selected pwm channel to init
 * @param PWM_InitStruct 	pointer to a PWM_InitTypeDef
 *         structure that contains the configuration information for the
 *         specified PWM peripheral.
 * @param irq_func only for pwm period end interrupt function handler
 * @param irq_data parameter for the oeriod end interrupt function handler
 * @note call PWM_RegDutyUpLimInt() and PWM_RegDutyDnLimInt() to register duty auto adjustment
 * 		 interrupt function handler
 */
void PWM_Init(u8 pwm_num, PWM_InitTypeDef *PWM_InitStruct, PWM_IRQ_FUN irq_func, void *irq_data)
{
	if (PWM_RegInitFlag == 0) {
		PWM_COMM_IRQInit();
	}

	PWM_RegInitFlag |= BIT(pwm_num);

	PWM_SetPeriod(pwm_num, PWM_InitStruct->PWM_Period);
	PWM_SetDutySize(pwm_num, PWM_InitStruct->PWM_DutySize);
	PWM_SetTickSource(pwm_num, PWM_InitStruct->PWM_TickSource);

	if (irq_func != NULL) {
		PWM_PeriodEndIrqHandler[pwm_num] = irq_func;
		PWM_PeriodEndIrqData[pwm_num] = irq_data;
	}
}

/**
 * @brief De-init the specific pwm channel
 * @param pwm_num the selected pwm channel to be de-init
 * @note disable pwm, clears all pending interrupts and disables all interrupt types
 */
void PWM_DeInit(u8 pwm_num)
{
	PWM_RegInitFlag &= ~(BIT(pwm_num));

	PWM_Cmd(pwm_num, DISABLE);
	PWM_INTClear(pwm_num);
	PWM_INTConfig(pwm_num, PWM_INT_TYPE_PERIOD_END, DISABLE);
	PWM_INTConfig(pwm_num, PWM_INT_TYPE_DUTY_ADJ_UP_LIMIT, DISABLE);
	PWM_INTConfig(pwm_num, PWM_INT_TYPE_DUTY_ADJ_DOWN_LIMIT, DISABLE);

	PWM_UnRegPeriodEndInt(pwm_num);
	PWM_UnRegDutyUpLimInt(pwm_num);
	PWM_UnRegDutyDnLimInt(pwm_num);

	if (PWM_RegInitFlag == 0) {
		PWM_COMM_IRQDeInit();
	}
}
