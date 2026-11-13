/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "ameba_soc.h"
static const char *const TAG = "CLK";

static u32 _ClkGet_IntDiv(u32 freq, u32 divn)
{
	/* Integer divider fields encode divide-by-(DIVN + 1). */
	return freq / (divn + 1);
}

static u32 _SlowClk_SrcFreqGet(u32 clksrc)
{
	switch (clksrc) {
	case PON_SLOW_SCLK_SYS1_PLL:
		return SYS1_PLL_ClkGet();
	case PON_SLOW_SCLK_SYS2_PLL:
		return SYS2_PLL_ClkGet();
	case PON_SLOW_SCLK_PERI_PLL:
		return PERI_PLL_ClkGet();
	case PON_SLOW_SCLK_DDR_PLL:
		return PLL_960M;
	case PON_SLOW_SCLK_USB2_PLL:
		return 480 * MHZ_TICK_CNT;
	case PON_SLOW_SCLK_XTAL40M:
		return XTAL_ClkGet();
	case PON_SLOW_SCLK_OSC4M:
		return 4 * MHZ_TICK_CNT;
	case PON_SLOW_SCLK_SDM32K:
		return 32 * 1024;
	default:
		return XTAL_ClkGet();
	}
}

static u32 KM4_NP_ClkGet(void)
{
	u32 clksrc = RCC_PeriphClockSourceGet(PON_KM4_NP_CLK);
	u32 divn = RCC_PeriphClockDividerGetDivN(PON_KM4_NP_CLK);
	u32 divs = RCC_PeriphClockDividerGetDivS(PON_KM4_NP_CLK);
	u32 freq = 40 * 1000 * 1000;

	switch (clksrc) {
	case PON_KM4_NP_CLK_SYS1_PLL:
		freq = SYS1_PLL_ClkGet();
		break;
	case PON_KM4_NP_CLK_SYS2_PLL:
		freq = SYS2_PLL_ClkGet();
		break;
	case PON_KM4_NP_CLK_PERI_PLL:
		freq = PERI_PLL_ClkGet();
		break;
	case PON_KM4_NP_CLK_XTAL40M :
		freq = XTAL_ClkGet();
		break;
	}
	RTK_LOGI(TAG, "NP Clock Src %dMhz, DIVN %d, DIVS %d\n", freq / 1000 / 1000, divn, divs);

	return freq * divs / divn ;
}

static u32 KM4_MP_ClkGet(void)
{

	u32 clksrc = RCC_PeriphClockSourceGet(SYS_KM4_MP_CLK);
	u32 divn = RCC_PeriphClockDividerGetDivN(SYS_KM4_MP_CLK);
	u32 divs = RCC_PeriphClockDividerGetDivS(SYS_KM4_MP_CLK);
	u32 freq = 40 * 1000 * 1000;

	switch (clksrc) {
	case SYS_KM4_MP_CLK_SYS1_PLL:
		freq = SYS1_PLL_ClkGet();
		break;
	case SYS_KM4_MP_CLK_SYS2_PLL:
		freq = SYS2_PLL_ClkGet();
		break;
	case SYS_KM4_MP_CLK_PERI_PLL:
		freq = PERI_PLL_ClkGet();
		break;
	case SYS_KM4_MP_CLK_XTAL40M :
		freq = XTAL_ClkGet();
		break;
	}
	RTK_LOGI(TAG, "MP Clock Src %dMhz, DIVN %d, DIVS %d\n", freq / 1000 / 1000, divn, divs);

	return freq * divs / divn ;
}

static u32 KM0_FP_ClkGet(void)
{

	u32 clksrc = RCC_PeriphClockSourceGet(PON_KM0_FW_CLK);
	u32 divn = RCC_PeriphClockDividerGetDivN(PON_KM0_FW_CLK);
	u32 divs = RCC_PeriphClockDividerGetDivS(PON_KM0_FW_CLK);
	u32 freq = 40 * 1000 * 1000;

	switch (clksrc) {
	case PON_KM0_FW_CLK_XTAL40M :
		freq = XTAL_ClkGet();
		break;
	case PON_KM0_FW_CLK_OSC4M:
		freq = 4 * 1000 * 1000;
		break;
	case PON_KM0_FW_CLK_OSC131K:
		freq = 128 * 1000;
		break;
	}
	RTK_LOGI(TAG, "FP Clock Src %dMhz, DIVN %d, DIVS %d\n", freq / 1000 / 1000, divn, divs);

	return freq * divs / divn ;
}


u32 CA32_AP_ClkGet(void)
{
	u32 ctrl0 = SYSON_BASE->CPUPLL_CTRL0;
	u32 ctrl1 = SYSON_BASE->CPUPLL_CTRL1;
	u32 ctrl2 = SYSON_BASE->CPUPLL_CTRL2;
	u32 xtal  = XTAL_ClkGet();

	/* If XTAL_SEL, output is xtal directly */
	if (ctrl1 & SYSON_BIT_XTAL_SEL) {
		return (ctrl1 & SYSON_BIT_XTAL_FREQ_SEL) ? xtal / 2 : xtal;
	}

	/* PLL freq = xtal * ((DIVN+2)*2^16 + F0N*2^13 + F0F) / 2^16 */
	u32 divn = SYSON_GET_SDM_DIVN(ctrl2);
	u32 f0n  = SYSON_GET_SDM_F0N(ctrl2);
	u32 f0f  = SYSON_GET_SDM_F0F(ctrl2);
	u64 pll  = (u64)xtal * (((divn + 2) << 16) + (f0n << 13) + (f0f << 5)) >> 16;

	/* CK_OUT_SEL divider */
	u8 ck_sel = SYSON_GET_CK_OUT_SEL(ctrl0);
	u32 ck_div;
	switch (ck_sel) {
	case 4:
		ck_div = 4;
		break;
	case 6:
		ck_div = 8;
		break;
	case 7:
		ck_div = 16;
		break;
	default:
		ck_div = 1;
		break;
	}

	pll /= ck_div;

	/* CK_EN_D2: divide by 2 */
	if (!(ctrl0 & SYSON_BIT_CK_EN_D2)) {
		pll /= 2;
	}

	/* VSFC CPUPLL_CLK_SEL divider */
	u8 vsfc_sel = VSFC_GET_CPUPLL_CLK_SEL(VSFC_BASE->VSFC_MISC);
	switch (vsfc_sel) {
	case 1:
		pll = pll * 8 / 9;
		break;
	case 2:
		pll = pll * 4 / 5;
		break;
	case 3:
		pll = pll * 2 / 3;
		break;
	default:
		break;
	}

	RTK_LOGI(TAG, "AP Clock %dMhz\n", (u32)(pll / 1000 / 1000));

	return (u32)pll;
}

u32 CPU_ClkGet(void)
{
	if (SYSCFG_CHIPType_Get() == CHIP_TYPE_FPGA) {
		return 40 * MHZ_TICK_CNT;
	}

	switch (SYS_CPUID()) {
	case 0:
		return KM4_NP_ClkGet();
	case 1:
		return CA32_AP_ClkGet();
	case 2:
		return KM4_MP_ClkGet();
	case 3:
		return KM0_FP_ClkGet();
	}
	return 0;
}

int CPU_Clk_Check(void)
{
	return 0;
}

/* Peripheral CLock */
static u32 _TRNG_ClkGet(void)
{
	u32 clksrc = RCC_PeriphClockSourceGet(PON_TRNG_CLK);
	u32 divn = RCC_PeriphClockDividerGetDivN(PON_TRNG_CLK);
	u32 divs = RCC_PeriphClockDividerGetDivS(PON_TRNG_CLK);
	u32 freq = 40 * 1000 * 1000;

	switch (clksrc) {
	case PON_TRNG_CLK_XTAL40M :
		freq = XTAL_ClkGet();
		break;
	case PON_TRNG_CLK_OSC4M:
		freq = 4 * 1000 * 1000;
		break;
	case PON_TRNG_CLK_OSC131K:
		freq = 128 * 1000;
		break;
	}
	RTK_LOGI(TAG, "TRNG Clock Src %dMhz, DIVN %d, DIVS %d\n", freq / 1000 / 1000, divn, divs);

	return freq * divs / divn ;
}

static u32 _SEC_ClkGet(void)
{
	u32 clksrc = RCC_PeriphClockSourceGet(PON_SEC_SCLK);
	u32 divn = RCC_PeriphClockDividerGetDivN(PON_SEC_SCLK);
	u32 divs = RCC_PeriphClockDividerGetDivS(PON_SEC_SCLK);
	u32 freq = 40 * 1000 * 1000;

	switch (clksrc) {
	case PON_SEC_SCLK_SYS1_PLL:
		freq = SYS1_PLL_ClkGet();
		break;
	case PON_SEC_SCLK_SYS2_PLL:
		freq = SYS2_PLL_ClkGet();
		break;
	case PON_SEC_SCLK_PERI_PLL:
		freq = PERI_PLL_ClkGet();
		break;
	case PON_SEC_SCLK_XTAL40M :
		freq = XTAL_ClkGet();
		break;
	}
	RTK_LOGI(TAG, "SECIP Clock Src %dMhz, DIVN %d, DIVS %d\n", freq / 1000 / 1000, divn, divs);

	return freq * divs / divn ;
}

static u32 _PSRAM_ClkGet(void)
{
	u32 clksrc = RCC_PeriphClockSourceGet(PON_PSRAMC_CLK);
	u32 divn = RCC_PeriphClockDividerGetDivN(PON_PSRAMC_CLK);
	u32 divs = RCC_PeriphClockDividerGetDivS(PON_PSRAMC_CLK);
	u32 freq = 40 * 1000 * 1000;

	switch (clksrc) {
	case PON_PSRAMC_CLK_SYS1_PLL:
		freq = SYS1_PLL_ClkGet();
		break;
	case PON_PSRAMC_CLK_SYS2_PLL:
		freq = SYS2_PLL_ClkGet();
		break;
	case PON_PSRAMC_CLK_PERI_PLL:
		freq = PERI_PLL_ClkGet();
		break;
	case PON_PSRAMC_CLK_XTAL40M :
		freq = XTAL_ClkGet();
		break;
	}

	RTK_LOGI(TAG, "PSRAMC Clock Src %dMhz, DIVN %d, DIVS %d\n", freq / 1000 / 1000, divn, divs);

	return _ClkGet_IntDiv(freq, divn);
}

static u32 _SLOW_SCLK_ClkGet(void)
{
	u32 clksrc = RCC_PeriphClockSourceGet(PON_SLOW_SCLK);
	u32 divn = RCC_PeriphClockDividerGetDivN(PON_SLOW_SCLK);
	u32 freq = _SlowClk_SrcFreqGet(clksrc);
	RTK_LOGI(TAG, "SLOW_SCLK Clock Src %uMHz, DIVN %d\n", freq / 1000 / 1000, divn);

	return _ClkGet_IntDiv(freq, divn);
}

static u32 _SLOW_PCLK_ClkGet(void)
{
	u32 clksrc = RCC_PeriphClockSourceGet(PON_SLOW_PCLK);
	u32 divn = RCC_PeriphClockDividerGetDivN(PON_SLOW_PCLK);
	u32 divs = RCC_PeriphClockDividerGetDivS(PON_SLOW_PCLK);
	u32 freq = _SlowClk_SrcFreqGet(clksrc);

	RTK_LOGI(TAG, "SLOW_PCLK Clock Src %uMHz, DIVN %d, DIVS %d\n", freq / 1000 / 1000, divn, divs);

	return freq * divs / divn ;
}

static u32 _AUDIO_SPORT_ClkGet(void)
{
	u32 clksrc = RCC_PeriphClockSourceGet(SYS_SPORT_CLK);
	u32 divn = RCC_PeriphClockDividerGetDivN(SYS_SPORT_CLK);
	u32 divs = RCC_PeriphClockDividerGetDivS(SYS_SPORT_CLK);
	u32 freq = 40 * 1000 * 1000;

	switch (clksrc) {
	case SYS_SPORT_CLK_SYS1_PLL:
		freq = SYS1_PLL_ClkGet();
		break;
	case SYS_SPORT_CLK_SYS2_PLL:
		freq = SYS2_PLL_ClkGet();
		break;
	case SYS_SPORT_CLK_PERI_PLL:
		freq = PERI_PLL_ClkGet();
		break;
	case SYS_SPORT_CLK_XTAL40M :
		freq = XTAL_ClkGet();
		break;
	}
	RTK_LOGI(TAG, "SPORT Clock Src %dMhz, DIVN %d, DIVS %d\n", freq / 1000 / 1000, divn, divs);

	return freq * divs / divn ;
}

static u32 _AUDIO_CODEC_ClkGet(void)
{
	u32 clksrc = RCC_PeriphClockSourceGet(AUDIO_CODEC_CLK);
	u32 divn = RCC_PeriphClockDividerGetDivN(AUDIO_CODEC_CLK);
	u32 divs = RCC_PeriphClockDividerGetDivS(AUDIO_CODEC_CLK);
	u32 freq = 40 * 1000 * 1000;

	switch (clksrc) {
	case AUDIO_CODEC_CLK_SYS1_PLL:
		freq = SYS1_PLL_ClkGet();
		break;
	case AUDIO_CODEC_CLK_SYS2_PLL:
		freq = SYS2_PLL_ClkGet();
		break;
	case AUDIO_CODEC_CLK_PERI_PLL:
		freq = PERI_PLL_ClkGet();
		break;
	case AUDIO_CODEC_CLK_XTAL40M :
		freq = XTAL_ClkGet();
		break;
	}
	RTK_LOGI(TAG, "AUDIO CODEC Clock Src %dMhz, DIVN %d, DIVS %d\n", freq / 1000 / 1000, divn, divs);

	return freq * divs / divn ;
}

static u32 _PON_TIMER0_ClkGet(void)
{
	u32 clksrc = RCC_PeriphClockSourceGet(PON_TIMER0_CLK);
	u32 divn = RCC_PeriphClockDividerGetDivN(PON_TIMER0_CLK);
	u32 divs = RCC_PeriphClockDividerGetDivS(PON_TIMER0_CLK);
	u32 freq = 40 * 1000 * 1000;

	switch (clksrc) {
	case PON_TIMER0_CLK_XTAL40M :
		freq = XTAL_ClkGet();
		break;
	case PON_TIMER0_CLK_OSC4M:
		freq = 4 * 1000 * 1000;
		break;
	case PON_TIMER0_CLK_SDM32K:
		freq = 32 * 1024;
		break;
	case PON_TIMER0_CLK_OSC131K:
		freq = 128 * 1000;
		break;
	}
	RTK_LOGD(TAG, "PON TIMER0 Clock Src %uMHz, DIVN %d, DIVS %d\n", freq / 1000 / 1000, divn, divs);

	return _ClkGet_IntDiv(freq, divn);
}

u32 PERI_ClkGet(u32 peri_id)
{
	switch (peri_id) {
	case CLKID_PSRAM:
		return _PSRAM_ClkGet();
	case CLKID_TRNG:
		return _TRNG_ClkGet();
	case CLKID_SECIP:
		return _SEC_ClkGet();
	case CLKID_SLOW_SCLK:
		return _SLOW_SCLK_ClkGet();
	case CLKID_SLOW_PCLK:
		return _SLOW_PCLK_ClkGet();
	case CLKID_AUDIO_SPORT:
		return _AUDIO_SPORT_ClkGet();
	case CLKID_AUDIO_CODEC:
		return _AUDIO_CODEC_ClkGet();
	case CLKID_PON_TIMER0:
		return _PON_TIMER0_ClkGet();
	}
	return 0;
}

static u32 _LSFIF_WaitReady(void)
{
	u32 i;

	for (i = 0; i < 1000000; i++) {
		if ((AON_BASE->AON_LSFIF_CMD & AON_BIT_AON_LSFIF_POLL) == 0) {
			return TRUE;
		} else {
			DelayMs(1);
		}
	}

	RTK_LOGW(TAG, "AON indirect RW not done\r\n");

	return FALSE;
}

static void _LSFIF_Write(u32 lsf_sel, u16 addr, u32 data)
{

	AON_BASE->AON_LSFIF_CMD = AON_LSF_SEL(lsf_sel);

	AON_BASE->AON_LSFIF_RWD = data;
	AON_BASE->AON_LSFIF_CMD |= AON_MASK_AON_LSFIF_WE;
	AON_BASE->AON_LSFIF_CMD |= AON_BIT_AON_LSFIF_WR;
	AON_BASE->AON_LSFIF_CMD |= AON_AON_LSFIF_AD(addr);
	AON_BASE->AON_LSFIF_CMD |= AON_BIT_AON_LSFIF_POLL;

	_LSFIF_WaitReady();
}

static u32 _LSFIF_Read(u32 lsf_sel, u16 addr)
{
	u32 data;

	AON_BASE->AON_LSFIF_CMD = AON_LSF_SEL(lsf_sel);

	AON_BASE->AON_LSFIF_CMD &= ~(AON_MASK_AON_LSFIF_WE);
	AON_BASE->AON_LSFIF_CMD &= ~(AON_BIT_AON_LSFIF_WR);
	AON_BASE->AON_LSFIF_CMD |= AON_AON_LSFIF_AD(addr);
	AON_BASE->AON_LSFIF_CMD |= AON_BIT_AON_LSFIF_POLL;

	_LSFIF_WaitReady();

	data = AON_BASE->AON_LSFIF_RWD;

	return data;
}

/**
  * @brief  32K clock Enable,
  * @param  NA
  * @note Timer calibrate duration can be
  * 	SDM_TIMERCAL_INTERVAL_16MIN
  * 	SDM_TIMERCAL_INTERVAL_1MIN
  * 	SDM_TIMERCAL_INTERVAL_10SEC
  * 	SDM_TIMERCAL_INTERVAL_1SEC
  */
void SDM32K_Enable(void)
{
	u32 data = 0;

	data = SDM_BIT_RST;
	_LSFIF_Write(SDM_LSF_SEL, Sdm_Ctrl0, data);

	data = SDM_TIMERCAL_INTERVAL_1MIN;
	_LSFIF_Write(SDM_LSF_SEL, Sdm_Timeout, data);

	data = (SDM_BIT_EN | SDM_BIT_RST | SDM_BIT_ALWAYS_CAL_EN | SDM_BIT_TIMER_CAL_EN);
	_LSFIF_Write(SDM_LSF_SEL, Sdm_Ctrl0, data);

}

/**
  * @brief  32K clock Timer auto calibration Enable,
  * @param  newstatus: can be one of the following values:
  *		 @arg ENABLE: open timer-calibration
  *		 @arg FALSE not open Timer-calibration
  * @note NA
  */
void SDM32K_TimerCalEnable(u32 newstatus)
{
	u32 data = _LSFIF_Read(SDM_LSF_SEL, Sdm_Ctrl0);

	if (newstatus == ENABLE) {
		data |= SDM_BIT_TIMER_CAL_EN;
	} else {
		data &= ~SDM_BIT_TIMER_CAL_EN;
	}

	_LSFIF_Write(SDM_LSF_SEL, Sdm_Ctrl0, data);
}

void HSSDM_Enable(void)
{
	u32 data = 0;

	data = (XTAL_DIVIDER_BIT_SDM_EN | XTAL_DIVIDER_BIT_SDM_RST);

	_LSFIF_Write(HSSDM_LSF_SEL, Xtal_Div_Sdm_Fen, data);
}

/**
  * @brief  Get osc calibration result.
  * @param  Option: can be one of the following values:
  *		 @arg OSC4M_CAL_CLK
  *		 @arg AON128K_CAL_CLK
  *		 @arg AON32K_CAL_CLK
  * @note NA
  */
u32 OSC_CalResult_Get(u8 cal_clk)
{
	u32 temp = 0;
	u32 val = 0;

	// switch AON CLK to xtal 40MHz
	val = AON_BASE->AON_AONCLK_CTRL;
	val &= ~(AON_MASK_AON_CLK_SEL);
	val |= AON_AON_CLK_SEL(0x3);
	AON_BASE->AON_AONCLK_CTRL = val;

	// RTK_LOGI(TAG, "[CAL]:clk src = %x\r\n", AON_GET_AON_CLK_SEL(AON_BASE->AON_AONCLK_CTRL));
	// RTK_LOGI(TAG, "[CAL]:cal clk = %x\r\n", cal_clk);

	/* start calibration */
	temp = AON_NS_BASE->ANACK_CAL_CTRL;
	temp &= ~(AON_NS_MASK_ANACK_CAL_SEL);
	temp |= (AON_NS_ANACK_CAL_SEL(cal_clk) | AON_NS_BIT_ANACK_CAL_CMD);
	AON_NS_BASE->ANACK_CAL_CTRL = temp;

	DelayUs(1);

	/* poll calibration complete */
	while (1) {
		temp = AON_NS_BASE->ANACK_CAL_CTRL;
		if ((temp & AON_NS_BIT_ANACK_CAL_CMD) == 0) {
			break;
		}
		DelayUs(1);
	}

	/* read calibration result */
	temp = AON_NS_BASE->ANACK_CAL_CTRL;

	// switch back to 4MHz
	val = AON_BASE->AON_AONCLK_CTRL;
	val &= ~(AON_MASK_AON_CLK_SEL);
	AON_BASE->AON_AONCLK_CTRL = val;

	return AON_NS_GET_ANACK_CAL_RPT(temp);
}

/* setbit/clearbit is: BIT0~BIT5 defined in BIT_AON_MASK_OSC32K_RCAL_5_0 */
void OSC131_R_Set(u32 setbit, u32 clearbit)
{
	REGU_TypeDef *regu = REGU_BASE;
	u32 reg_val = regu->REGU_OSC32K_CTRL0;

	if (setbit) {
		reg_val |= (REGU_AON_RCAL_5_0(setbit));
	}

	if (clearbit) {
		reg_val &= ~(REGU_AON_RCAL_5_0(clearbit));
	}

	regu->REGU_OSC32K_CTRL0 = reg_val;
	/* It takes 1.5 ms to stable */
	DelayUs(1500);
}

/**
  * @brief  131.072K calibration
  * @param  max_delta: target_40m_counter-calibration_result, default 10
  * @note Here XTAL clock is used to count N cycles of calibration clock, the total used XTAL clock cycles can be read by this register, N= 8
  */
u32 OSC131K_Calibration(u32 ppm_limit)
{
	u32 temp = 0;
	u32 cal_n = 0;
	u32 rsel_bit_index = 0;
	u32 delta = 0;
	u32 min_delta = 0xFFFFFFFF;
	u32 cur_ppm = 0;
	u32 min_delta_r = 0;
	u32 target_40m_counter = 2441; // cal_rpt = (1000000000 * 8 / 25) / 131072
	u32 clearbit = 0;
	u32 reg_val;
	REGU_TypeDef *regu = REGU_BASE;

	temp = regu->REGU_OSC32K_CTRL0;

	/* Step1: Power on aon 128kHz OSC: 0x4200_8100[0]=1'b1 */
	/* Clear 4m calibration parameter first */
	regu->REGU_OSC32K_CTRL0 &= ~(REGU_MASK_AON_RCAL_5_0);

	/* It takes 1.5 ms to stable */
	DelayUs(1500);

	/* Step2: Adjust R_CAL. Enter the following loop: each loop decides one bit of 4800002c[13:8]-RCAL, i.e. the first loop decides RCAL[5], the second loop decides RCAL[4] …, suppose the current loop is N (N=1..6) */
	/* Loop Step1: Set RCAL[6-N] = 0x1 */
	/* Loop Step2: Wait some time for clock stable (this wait time decide by SW) */
	/* Loop Step3: Enable Calibration: 0x4800_2840 = 0x8200_0000 */
	/* Loop Step2: Poll 0x4800_2840[31] = 0x0, read calibration result: 0x4800_2840[15:0] */
	/* Loop Step3: */
	/* If(frequency is in the allowed deviation range) */
	/* 	Calibration will end successfully */
	/* Else if (frequency counter > target frequency counter) */ /* (frequency > target frequency) */
	/* 	RCAL[6-N] = 0x0 */
	/* Else */
	/* 	RCAL[6-N] keep 1. */
	/* If (N=6) */
	/* 	Calibration will end, and if the last loop frequency is OK , then calibration is OK, otherwise calibration is fail */
	/* Else */
	/* 	Go to Loop step1 */

	for (cal_n = 1; cal_n <= 6; cal_n++) {
		rsel_bit_index = 6 - cal_n;
		OSC131_R_Set(BIT(rsel_bit_index), clearbit);

		/* read calibration result */
		temp = OSC_CalResult_Get(AON128K_CAL_CLK);

		if (temp < target_40m_counter) {
			delta = target_40m_counter - temp;
			clearbit = BIT(rsel_bit_index);
		} else {
			delta = temp - target_40m_counter;
			clearbit = 0;
		}

		if (delta < min_delta) {
			min_delta = delta;
			min_delta_r = regu->REGU_OSC32K_CTRL0;
			min_delta_r &= REGU_MASK_AON_RCAL_5_0;
		}

		// RTK_LOGI(TAG, "[CAL131K]:cal_n %lu delta:%lu \n", cal_n, delta);
	}

	/* the last one is not the best one */
	if (delta > min_delta) {
		reg_val = regu->REGU_OSC32K_CTRL0;
		reg_val &= ~REGU_MASK_AON_RCAL_5_0;
		reg_val |= min_delta_r;
		regu->REGU_OSC32K_CTRL0 = reg_val;

		/* It takes 1.5 ms to stable */
		DelayUs(1500);

		/* read calibration result */
		temp = OSC_CalResult_Get(AON128K_CAL_CLK);

		if (temp < target_40m_counter) {
			delta = target_40m_counter - temp;
		} else {
			delta = temp - target_40m_counter;
		}
	}

	cur_ppm = delta * 1000000 / target_40m_counter;
	RTK_LOGI(TAG, "[CAL131K]: delta:%lu target:%lu PPM: %lu PPM_Limit:%lu \n", delta, target_40m_counter, cur_ppm, ppm_limit);

	if (cur_ppm >= ppm_limit) {
		RTK_LOGE(TAG, "[CAL131K]: !!! cal fail !!! PPM: %lu PPM_Limit:%lu \n", cur_ppm, ppm_limit);
	}

	return TRUE;
}

/* bitnum is: 0~7 defined in REGU_MASK_FREQ_R_SEL */
void OSC4M_R_Set(u32 setbit, u32 clearbit)
{
	REGU_TypeDef *regu = REGU_BASE;

	if (setbit) {
		regu->REGU_OSC4M_CTRL |= REGU_FREQ_R_SEL_7_0(setbit);
	}

	if (clearbit) {
		regu->REGU_OSC4M_CTRL &= ~REGU_FREQ_R_SEL_7_0(clearbit);
	}

	/* It takes 2us to stable */
	DelayUs(2);
}

/* bitnum is: 0~3 defined in REGU_MASK_VCM_SEL_H/L */
void OSC4M_VCM_Set(u32 value)
{
	u32 temp;

	REGU_TypeDef *regu = REGU_BASE;
	temp = regu->REGU_OSC4M_CTRL;

	temp &= ~(REGU_MASK_VCM_SEL_L_3_0 | REGU_MASK_VCM_SEL_H_3_0);
	temp |= (REGU_VCM_SEL_L_3_0(value) | REGU_VCM_SEL_H_3_0(value));
	regu->REGU_OSC4M_CTRL = temp;


	/* It takes 2us to stable */
	DelayUs(2);
}

/**
  * @brief  OSC4M calibration
  * @param  ppm_limit: target_40m_counter-calibration_result, default 10
  * @note NA
  */
u32 OSC4M_Calibration(u32 ppm_limit)
{
	u32 temp = 0;
	u32 cal_n = 0;
	u32 rsel_bit_index = 0;
	u32 delta = 0;
	u32 min_delta = 0xFFFFFFFF;
	u32 cur_ppm = 0;
	u32 min_delta_r = 0;
	u32 target_40m_counter = 320; //cal_rpt=32*40Mhz/fclk_osc4m
	u32 clearbit = 0;
	REGU_TypeDef *regu = REGU_BASE;

	/* Step1: Power on aon 4MHz OSC: 0x4200_8100[2]=1'b1 */
	regu->REGU_OSC4M_CTRL |= REGU_BIT_AON_POW_4MOSC;

	/* Step2: Clear 4m calibration parameter first */
	// regu->REGU_OSC4M_CTRL &= (u16)(~(REGU_MASK_VCM_SEL_L_3_0 | REGU_MASK_VCM_SEL_H_3_0));

	DelayMs(1);

	/* Step3: Adjust V_H_SEL & V_L_SEL. Enter the following loop: suppose the current loop is N (N=1..4) */
	/* Loop Step1: Set REGU_VCM_SEL_L[4-N] = 0x1, REGU_VCM_SEL_H[4-N] = 0x1 */
	/* Loop Step2: Enable Calibration, read calibration result*/
	/* Loop Step3: */
	/* If(frequency is in the allowed deviation range)  */
	/* 	Calibration will end successfully */
	/* Else if (frequency counter > target frequency counter) */ /* (frequency < target frequency) */
	/* 	REGU_VCM_SEL_L/H[4-N] = 0x0 */
	/* Else  */
	/* 	REGU_VCM_SEL_L/H[4-N] keep 1. */
	for (cal_n = 5; cal_n <= 11; cal_n++) {

		OSC4M_VCM_Set(cal_n);

		/* read calibration result */
		temp = OSC_CalResult_Get(OSC4M_CAL_CLK);

		// RTK_LOGI(TAG, "[CAL4M]:cal_n %lu H:0x%lx L:0x%lx result:%lu ", cal_n,REGU_GET_VCM_SEL_H_3_0(regu->REGU_OSC4M_CTRL),REGU_GET_VCM_SEL_L_3_0(regu->REGU_OSC4M_CTRL),temp);

		if (temp < target_40m_counter) {
			delta = target_40m_counter - temp;
			clearbit = 0;
		} else if (temp > target_40m_counter) {
			delta = temp - target_40m_counter;
			clearbit = BIT(rsel_bit_index);
		} else {
			delta = 0;
			goto cal_end;
		}

		if (delta < min_delta) {
			min_delta = delta;
			min_delta_r = regu->REGU_OSC4M_CTRL;
			min_delta_r &= (REGU_MASK_VCM_SEL_L_3_0 | REGU_MASK_VCM_SEL_H_3_0 | REGU_MASK_FREQ_R_SEL_7_0);
		}
		// RTK_LOGI(TAG, "delta:%lu min_delta:%lu min_delta_r:0x%lx \n", delta,min_delta,min_delta_r);
	}

	/* the last one is not the best one */
	if (delta > min_delta) {
		temp = regu->REGU_OSC4M_CTRL;
		temp &= ~(REGU_MASK_VCM_SEL_L_3_0 | REGU_MASK_VCM_SEL_H_3_0);
		temp |= min_delta_r;
		regu->REGU_OSC4M_CTRL = temp;
	}
	//RTK_LOGI(TAG, "[CAL4M]:hL last one: 0x%lx 0x%lx temp:%lu \n", REGU_GET_VCM_SEL_H(regu->REGU_4MOSC0),REGU_GET_VCM_SEL_L(regu->REGU_4MOSC0),temp);
	/* Step4: Adjust R_SEL. Enter the following loop: suppose the current loop is N (N=1..8) */
	/* Clear 4m calibration parameter first */
	/* Loop Step1: Set REGU_FREQ_R_SEL[8-N] = 0x1 */
	/* Loop Step2: Enable Calibration, read calibration result*/
	/* Loop Step3: */
	/* If(frequency is in the allowed deviation range)  */
	/* 	Calibration will end successfully */
	/* Else if (frequency counter > target frequency counter) */ /* (frequency < target frequency) */
	/* 	REGU_FREQ_R_SEL[8-N] = 0x0 */
	/* Else  */
	/* 	REGU_FREQ_R_SEL[8-N] keep 1. */
	clearbit = 0;
	// regu->REGU_OSC4M_CTRL &= (u16)(~REGU_MASK_FREQ_R_SEL_7_0);
	for (cal_n = 1; cal_n <= 8; cal_n++) {
		rsel_bit_index = 8 - cal_n;
		OSC4M_R_Set(BIT(rsel_bit_index), clearbit);

		/* read calibration result */
		temp = OSC_CalResult_Get(OSC4M_CAL_CLK);
		// RTK_LOGI(TAG, "[CAL4M]:cal_n %lu R:0x%lx result:%lu ", cal_n,REGU_GET_FREQ_R_SEL_7_0(regu->REGU_OSC4M_CTRL), temp, target_40m_counter, delta);

		if (temp < target_40m_counter) {
			delta = target_40m_counter - temp;
			clearbit = 0;
		} else if (temp > target_40m_counter) {
			delta = temp - target_40m_counter;
			clearbit = BIT(rsel_bit_index);
		}  else {
			delta = 0;
			goto cal_end;
		}

		if (delta < min_delta) {
			min_delta = delta;
			min_delta_r = regu->REGU_OSC4M_CTRL;
			min_delta_r &= REGU_MASK_FREQ_R_SEL_7_0;
		}
		//RTK_LOGI(TAG, "delta:%lu \n", delta);
	}

	/* the last one is not the best one */
	if (delta > min_delta) {
		temp = regu->REGU_OSC4M_CTRL;
		temp &= ~REGU_MASK_FREQ_R_SEL_7_0;
		temp |= min_delta_r;
		regu->REGU_OSC4M_CTRL = temp;

		/* It takes 2us to stable */
		DelayUs(2) ;

		/* read calibration result */
		temp = OSC_CalResult_Get(OSC4M_CAL_CLK);

		// RTK_LOGI(TAG, "OSC4M_Calibration:cal_n %lu result:%lu target:%lu delta:%lu\n", cal_n, temp, target_40m_counter, delta);

		if (temp < target_40m_counter) {
			delta = target_40m_counter - temp;
		} else {
			delta = temp - target_40m_counter;
		}
	}
	//RTK_LOGI(TAG, "[CAL4M]:R last one: 0x%lx \n",REGU_GET_FREQ_R_SEL(regu->REGU_4MOSC0));

cal_end:
	cur_ppm = delta * 1000000 / target_40m_counter;
	RTK_LOGI(TAG, "[CAL4M]: delta:%lu target:%lu PPM: %lu PPM_Limit:%lu \n", delta, target_40m_counter, cur_ppm, ppm_limit);

	if (cur_ppm >= ppm_limit) {
		RTK_LOGW(TAG, "[CAL4M]: PPM: %lu PPM_Limit:%lu \n", cur_ppm, ppm_limit);
	}

	return TRUE;
}
