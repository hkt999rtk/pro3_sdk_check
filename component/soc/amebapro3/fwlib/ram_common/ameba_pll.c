/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "ameba_soc.h"

/* ======================== Frequency Calculation ======================== */

#define PLL_XTAL_MHZ()	(XTAL_ClkGet() / 1000000)

/**
  * @brief  Calculate SDM parameters from target frequency.
  * @param  freq_in: input reference frequency in MHz (e.g. 40).
  * @param  freq_out: desired VCO output frequency in MHz.
  * @param  N: pointer to store DIVN value.
  * @param  FN: pointer to store F0N value.
  * @param  FF: pointer to store F0F value.
  * @retval None
  */
void PLL_CalFreqParam(u32 freq_in, u32 freq_out, u32 *N, u32 *FN, u32 *FF)
{
	*N = freq_out / freq_in - 2;
	*FN = freq_out * 8 / freq_in - *N * 8 - 16;
	*FF = (freq_out << 16) / freq_in - (*N << 16) - (2 << 16) - (*FN << 13);
}

/**
  * @brief  Calculate output frequency from SDM parameters.
  * @param  freq_in: input reference frequency in MHz.
  * @param  N: DIVN value.
  * @param  FN: F0N value.
  * @param  FF: F0F value.
  * @retval Output frequency in MHz.
  */
u32 PLL_CalFreqOut(u32 freq_in, u32 N, u32 FN, u32 FF)
{
	return (freq_in * (((N + 2) << 16) + (FN << 13) + FF)) >> 16;
}

/* ======================== Bandgap ======================== */

/**
  * @brief  Enable PLL bandgap. Must be called before any PLL init.
  * @retval None
  */
void PLL_Bandgap_Enable(void)
{
	BANDGAP_BASE->AON_PLL_CTRL0 |= BANDGAP_BIT_PLL_POW_BG;
	DelayUs(100);
	BANDGAP_BASE->AON_PLL_CTRL0 |= (BANDGAP_BIT_PLL_POW_MBIAS | BANDGAP_BIT_PLL_POW_I);
}

/**
  * @brief  Disable PLL bandgap. Only call when all PLLs are disabled.
  * @retval None
  */
void PLL_Bandgap_Disable(void)
{
	BANDGAP_BASE->AON_PLL_CTRL0 &= ~(BANDGAP_BIT_PLL_POW_MBIAS | BANDGAP_BIT_PLL_POW_I);
	BANDGAP_BASE->AON_PLL_CTRL0 &= ~BANDGAP_BIT_PLL_POW_BG;
}

/* ======================== SDP PLL Ops (SYS1/SYS2/PERI) ======================== */

static void SYS_PLL_Init(PLL_Device *pll)
{
	*pll->CTRL0 &= ~(PLL_CTRL0_CK_EN | PLL_CTRL0_CK_EN_D4);
	*pll->CTRL0 |= PLL_CTRL0_POW_ERC;

	*pll->CTRL0 |= (PLL_CTRL0_POW_DIV_IN | PLL_CTRL0_POW_PLL);
	*pll->CTRL0 |= PLL_CTRL0_CK_EN;
	*pll->CTRL0 &= ~(PLL_CTRL0_CK_EN_D4 | PLL_CTRL0_DOG_SEL_MASK);
	*pll->CTRL0 |= PLL_CTRL0_DOG_SEL(0x4);
	DelayUs(50);
	SYSON_BASE->PON_SYSON_CTRL &= ~pll->ISO_BIT;
}

static void SYS_PLL_Deinit(PLL_Device *pll)
{
	SYSON_BASE->PON_SYSON_CTRL |= pll->ISO_BIT;
	*pll->CTRL0 &= ~(PLL_CTRL0_CK_EN | PLL_CTRL0_CK_EN_D4);
	*pll->CTRL0 &= ~PLL_CTRL0_POW_ERC;
	*pll->CTRL0 &= ~(PLL_CTRL0_POW_DIV_IN | PLL_CTRL0_POW_PLL);
}

static void SYS_PLL_SetFreq(PLL_Device *pll, u32 Freq_MHz)
{
	u32 DivN_SDM, F0N_SDM, F0F_SDM;
	u32 div_sel = 0x0;

	assert_param(Freq_MHz >= 88 && Freq_MHz <= 700);

	if (Freq_MHz < 175) {
		PLL_CalFreqParam(PLL_XTAL_MHZ(), Freq_MHz * 4, &DivN_SDM, &F0N_SDM, &F0F_SDM);
		div_sel = 0xC;									/* output /4 */
	} else if (Freq_MHz < 350) {
		PLL_CalFreqParam(PLL_XTAL_MHZ(), Freq_MHz * 2, &DivN_SDM, &F0N_SDM, &F0F_SDM);
		div_sel = 0x8;									/* output /2 */
	} else {
		PLL_CalFreqParam(PLL_XTAL_MHZ(), Freq_MHz, &DivN_SDM, &F0N_SDM, &F0F_SDM);
	}

	*pll->CTRL1 &= ~PLL_CTRL1_CLEAR_MASK;
	*pll->CTRL1 |= PLL_CTRL1_SET(DivN_SDM, div_sel);

	*pll->CTRL3 &= ~PLL_CTRL3_CLEAR_MASK;
	*pll->CTRL3 |= PLL_CTRL3_SET(F0F_SDM, F0N_SDM);

	DelayUs(1);
	*pll->CTRL1 |= PLL_CTRL1_TRIG;
	DelayUs(1);
	*pll->CTRL1 &= ~PLL_CTRL1_TRIG;
}

static u32 SYS_PLL_ClkGet(PLL_Device *pll)
{
	u32 ctrl1 = *pll->CTRL1;
	u32 ctrl3 = *pll->CTRL3;
	u32 N = (ctrl1 >> 5) & 0x3F;
	u32 div_sel = (ctrl1 >> 1) & 0xF;
	u32 FN = (ctrl3 >> 16) & 0x7;
	u32 FF = (ctrl3 >> 19) & 0x1FFF;

	u32 vco_freq = PLL_CalFreqOut(PLL_XTAL_MHZ(), N, FN, FF);

	if (div_sel == 0xC) {
		vco_freq /= 4;
	} else if (div_sel == 0x8) {
		vco_freq /= 2;
	}

	return vco_freq * 1000 * 1000;
}

static void SYS_PLL_GetParam(PLL_Device *pll, u32 *DivN, u32 *F0N, u32 *F0F)
{
	*DivN = (*pll->CTRL1 >> 5) & 0x3F;
	*F0N = (*pll->CTRL3 >> 16) & 0x7;
	*F0F = (*pll->CTRL3 >> 19) & 0x1FFF;
}

const PLL_Ops SYS_PLL_Ops = {
	.Init     = SYS_PLL_Init,
	.Deinit   = SYS_PLL_Deinit,
	.SetFreq  = SYS_PLL_SetFreq,
	.ClkGet   = SYS_PLL_ClkGet,
	.GetParam = SYS_PLL_GetParam,
};

/* ======================== CPU PLL Ops ======================== */

static void CPU_PLL_Init(PLL_Device *pll)
{
	*pll->CTRL2 &= ~SYSON_MASK_SDM_DIVN;
	*pll->CTRL2 |= SYSON_SDM_DIVN(0x17);							/* default 1GHz: 40*(0x17+2)=1000 */

	SYSON_BASE->CPUPLL_CTRL4 |= SYSON_BIT_POW_RSTB;
	DelayUs(1);

	SYSON_BASE->CPUPLL_CTRL4 |= SYSON_BIT_POW_POWER_CUT;
	SYSON_BASE->CPUPLL_CTRL4 |= SYSON_BIT_POW_MBIAS;
	SYSON_BASE->CPUPLL_CTRL4 |= SYSON_BIT_POW_LDO;
	SYSON_BASE->CPUPLL_CTRL4 |= SYSON_BIT_VPULSE09_LDO;
	DelayUs(4);
	SYSON_BASE->CPUPLL_CTRL4 &= ~SYSON_BIT_VPULSE09_LDO;
	DelayUs(6);
	SYSON_BASE->CPUPLL_CTRL4 |= SYSON_BIT_POW_PLL;
	SYSON_BASE->CPUPLL_CTRL4 |= SYSON_BIT_POW_PI;
	DelayUs(50);

	SYSON_BASE->PON_SYSON_CTRL &= ~pll->ISO_BIT;
}

static void CPU_PLL_Deinit(PLL_Device *pll)
{
	SYSON_BASE->PON_SYSON_CTRL |= pll->ISO_BIT;

	SYSON_BASE->CPUPLL_CTRL4 &= ~SYSON_BIT_POW_RSTB;

	SYSON_BASE->CPUPLL_CTRL4 &= ~SYSON_BIT_POW_POWER_CUT;
	SYSON_BASE->CPUPLL_CTRL4 &= ~SYSON_BIT_POW_MBIAS;
	SYSON_BASE->CPUPLL_CTRL4 &= ~SYSON_BIT_POW_LDO;
	SYSON_BASE->CPUPLL_CTRL4 &= ~SYSON_BIT_VPULSE09_LDO;
	SYSON_BASE->CPUPLL_CTRL4 &= ~SYSON_BIT_POW_PLL;
	SYSON_BASE->CPUPLL_CTRL4 &= ~SYSON_BIT_POW_PI;
}

static void CPU_PLL_SetFreq(PLL_Device *pll, u32 Freq_MHz)
{
	assert_param((Freq_MHz % PLL_XTAL_MHZ()) == 0);

	u32 divn = Freq_MHz / PLL_XTAL_MHZ() - 2;

	*pll->CTRL2 &= ~SYSON_MASK_SDM_DIVN;
	*pll->CTRL2 |= SYSON_SDM_DIVN(divn);
}

static u32 CPU_PLL_ClkGet(PLL_Device *pll)
{
	u32 divn = SYSON_GET_SDM_DIVN(*pll->CTRL2);

	return PLL_XTAL_MHZ() * (divn + 2) * 1000 * 1000;
}

static void CPU_PLL_GetParam(PLL_Device *pll, u32 *DivN, u32 *F0N, u32 *F0F)
{
	*DivN = SYSON_GET_SDM_DIVN(*pll->CTRL2);
	*F0N = 0;
	*F0F = 0;
}

const PLL_Ops CPU_PLL_Ops = {
	.Init     = CPU_PLL_Init,
	.Deinit   = CPU_PLL_Deinit,
	.SetFreq  = CPU_PLL_SetFreq,
	.ClkGet   = CPU_PLL_ClkGet,
	.GetParam = CPU_PLL_GetParam,
};

/* ======================== PLL Device Instances ======================== */

PLL_Device PLL_SYS1 = {
	.ops     = &SYS_PLL_Ops,
	.CTRL0   = &SYSON_BASE->PON_SYS1PLL_CTRL0,
	.CTRL1   = &SYSON_BASE->PON_SYS1PLL_CTRL1,
	.CTRL2   = NULL,
	.CTRL3   = &SYSON_BASE->PON_SYS1PLL_CTRL3,
	.ISO_BIT = SYSON_BIT_PON_ISO_SYS1PLL,
};

PLL_Device PLL_SYS2 = {
	.ops     = &SYS_PLL_Ops,
	.CTRL0   = &SYSON_BASE->PON_SYS2PLL_CTRL0,
	.CTRL1   = &SYSON_BASE->PON_SYS2PLL_CTRL1,
	.CTRL2   = NULL,
	.CTRL3   = &SYSON_BASE->PON_SYS2PLL_CTRL3,
	.ISO_BIT = SYSON_BIT_PON_ISO_SYS2PLL,
};

PLL_Device PLL_PERI = {
	.ops     = &SYS_PLL_Ops,
	.CTRL0   = &SYSON_BASE->PON_PERIPLL_CTRL0,
	.CTRL1   = &SYSON_BASE->PON_PERIPLL_CTRL1,
	.CTRL2   = NULL,
	.CTRL3   = &SYSON_BASE->PON_PERIPLL_CTRL3,
	.ISO_BIT = SYSON_BIT_PON_ISO_PERIPLL,
};

PLL_Device PLL_CPU = {
	.ops     = &CPU_PLL_Ops,
	.CTRL0   = &SYSON_BASE->CPUPLL_CTRL0,
	.CTRL1   = NULL,
	.CTRL2   = &SYSON_BASE->CPUPLL_CTRL2,
	.CTRL3   = NULL,
	.ISO_BIT = SYSON_BIT_PON_ISO_CPUPLL,
};

/* ======================== Wrappers ======================== */

void CPUPLL_Init(void)
{
	PLL_Init(&PLL_CPU);
}
void CPUPLL_Deinit(void)
{
	PLL_Deinit(&PLL_CPU);
}
void CPUPLL_SetFreq(u32 Freq_MHz)
{
	PLL_SetFreq(&PLL_CPU, Freq_MHz);
}

void SYS1PLL_Init(void)
{
	PLL_Init(&PLL_SYS1);
}
void SYS1PLL_Deinit(void)
{
	PLL_Deinit(&PLL_SYS1);
}
void SYS1PLL_SetFreq(u32 Freq_MHz)
{
	PLL_SetFreq(&PLL_SYS1, Freq_MHz);
}

void SYS2PLL_Init(void)
{
	PLL_Init(&PLL_SYS2);
}
void SYS2PLL_Deinit(void)
{
	PLL_Deinit(&PLL_SYS2);
}
void SYS2PLL_SetFreq(u32 Freq_MHz)
{
	PLL_SetFreq(&PLL_SYS2, Freq_MHz);
}

void PERIPLL_Init(void)
{
	PLL_Init(&PLL_PERI);
}
void PERIPLL_Deinit(void)
{
	PLL_Deinit(&PLL_PERI);
}
void PERIPLL_SetFreq(u32 Freq_MHz)
{
	PLL_SetFreq(&PLL_PERI, Freq_MHz);
}

u32 CPUPLL_ClkGet(void)
{
	return PLL_ClkGet(&PLL_CPU);
}
u32 SYS1_PLL_ClkGet(void)
{
	return PLL_ClkGet(&PLL_SYS1);
}
u32 SYS2_PLL_ClkGet(void)
{
	return PLL_ClkGet(&PLL_SYS2);
}
u32 PERI_PLL_ClkGet(void)
{
	return PLL_ClkGet(&PLL_PERI);
}

/* PHY clock ready wait: 1 ms expected per PHY spec; bound at 10 ms for safety margin */
#define USB_PHY_CLK_RDY_WAIT_US		10U
#define USB_PHY_CLK_RDY_WAIT_CNT	1000U

static const char *const USB_PLL_TAG = "USB";

/* ======================== USB2 PLL (fixed 480MHz UTMI PHY output) ======================== */
/**
  * @brief  Enable USB2 PLL (UTMI PHY fixed 480MHz output) for use as a peripheral clock source.
  *         The hardware requires the full power sequence on every call; it is not refcounted.
  * @note   Also clocks/enables the OTG controller IP (UTMI_CLK_EN/OTG_CLK_EN/OTG_EN/OTG_RST_MUX)
  *         since it shares SYS_OTG_CTRL with the PHY; only otg_co_en is left to the USB OTG driver.
  * @retval HAL_OK or HAL_TIMEOUT (PHY clock not ready)
  */
int USB2PLL_Init(void)
{
	u32 count = 0;

	/* BANDGAP_BASE->USB2_BG_CTRL |= (BANDGAP_BIT_BG_ON_USB2 | BANDGAP_BIT_BG_ON_MIPI | BANDGAP_BIT_MAINPWR | BANDGAP_BG(0x01) | BANDGAP_BIT_BG_LPF); */

	BANDGAP_BASE->USB2_BG_CTRL |= (BANDGAP_BIT_BG_ON_USB2 | BANDGAP_BIT_BG_ON_MIPI | BANDGAP_BIT_MAINPWR | BANDGAP_BIT_BG_LPF);

	RCC_PeriphClockCmd(NULL, APBPeriph_CBUS_SYS_MEM_CLOCK, ENABLE);

	CORE_S_BASE->SYS_USBOTG_CLK_CTRL |= CORE_S_BIT_USBOTG_CLK_DIVTRG;
	CORE_S_BASE->SYS_USBOTG_CLK_CTRL &= ~(CORE_S_MASK_USBOTG_CLK_DIVN);

	RCC_PeriphClockCmd(NULL, APBPeriph_USBOTG_CLOCK, ENABLE);
	RCC_PeriphClockCmd(APBPeriph_USBOTG, NULL, ENABLE);

	CORE_S_BASE->SYS_OTG_CTRL |= CORE_S_BIT_SYS_UABG_EN;
	CORE_S_BASE->SYS_OTG_CTRL |= CORE_S_BIT_SYS_UABG_EN;

	CORE_S_BASE->SYS_OTG_CTRL |= CORE_S_BIT_SYS_UABG_EN;// USB Bandgap enable
	DelayUs(500);
	CORE_S_BASE->SYS_OTG_CTRL |= CORE_S_BIT_USB_PDN33;// power down USB
	DelayUs(1000);
	CORE_S_BASE->SYS_OTG_CTRL |= CORE_S_BIT_SYS_UAHV_EN;// USB analog hv power cut enable
	DelayUs(100);
	CORE_S_BASE->SYS_OTG_CTRL |= CORE_S_BIT_USB_IBX2USB_EN;// enable IBX to USB
	CORE_S_BASE->SYS_OTG_CTRL |= CORE_S_BIT_SYS_USBD_EN;// USB digital power cut enable

	CORE_S_BASE->SYS_OTG_CTRL &= ~(CORE_S_BIT_SYS_ISO_USBD_EN);// enable USB digital iso
	CORE_S_BASE->SYS_OTG_CTRL &= ~(CORE_S_BIT_SYS_ISO_USBA_EN);// enable USB analog iso
	DelayUs(1);

	CORE_S_BASE->SYS_OTG_CTRL |= CORE_S_BIT_SYS_OTG_UTMI_CLK_EN;// USB OTG UTMI CLK enable
	CORE_S_BASE->SYS_OTG_CTRL |= CORE_S_BIT_SYS_OTG_CLK_EN;// enable OTG clock
	CORE_S_BASE->SYS_OTG_CTRL |= CORE_S_BIT_SYS_OTG_EN;// enable OTG IP
	CORE_S_BASE->SYS_OTG_CTRL |= CORE_S_BIT_SYS_OTG_RST_MUX;// mask watch dog reset
	DelayUs(200);

	USB_ADDON_PWR_CTRL->b.phy_en = 1;				// enable PHY
	DelayUs(200);
	/* wait for phy clock ready, bounded so a PLL lock failure cannot hang usb_chip_init() forever */
	while (!USB_ADDON_PWR_CTRL->b.phy_clk_rdy) {	// PHY clock ready (RO)
		DelayUs(USB_PHY_CLK_RDY_WAIT_US);
		if (++count > USB_PHY_CLK_RDY_WAIT_CNT) {
			RTK_LOGS(USB_PLL_TAG, RTK_LOG_ERROR, "USB PHY clk not ready\r\n");
#if !USB2_PLL_SHARED_BY_4K
			USB2PLL_Deinit();
#endif
			return HAL_TIMEOUT;
		}
	}

	return HAL_OK;
}

/**
  * @brief  Disable USB2 PLL analog PHY path enabled by USB2PLL_Init().
  * @retval None
  */
void USB2PLL_Deinit(void)
{
	USB_ADDON_PWR_CTRL->b.otg_co_en = 0;        // 0x6024_0004[8] = 0;  // disable otg core
	USB_ADDON_PWR_CTRL->b.phy_en = 0;           // 0x6024_0004[9] = 0;  // disable PHY
	DelayMs(5);
	CORE_S_BASE->SYS_OTG_CTRL &= ~(CORE_S_BIT_SYS_OTG_EN);         // disable OTG IP
	CORE_S_BASE->SYS_OTG_CTRL &= ~(CORE_S_BIT_SYS_OTG_CLK_EN);     // disable OTG clock
	CORE_S_BASE->SYS_OTG_CTRL |= CORE_S_BIT_SYS_ISO_USBA_EN;       // enable USB analog iso
	CORE_S_BASE->SYS_OTG_CTRL |= CORE_S_BIT_SYS_ISO_USBD_EN;       // enable USB digital iso
	CORE_S_BASE->SYS_OTG_CTRL &= ~(CORE_S_BIT_SYS_USBD_EN);        // USB digital power cut disable
	CORE_S_BASE->SYS_OTG_CTRL &= ~(CORE_S_BIT_USB_IBX2USB_EN);     // disable IBX to USB
	CORE_S_BASE->SYS_OTG_CTRL &= ~(CORE_S_BIT_SYS_UAHV_EN);        // USB analog hv power cut disable
	CORE_S_BASE->SYS_OTG_CTRL |= CORE_S_BIT_USB_PDN33;             // power down USB
	CORE_S_BASE->SYS_OTG_CTRL &= ~(CORE_S_BIT_SYS_UABG_EN);        // USB Bandgap disable


	RCC_PeriphClockCmd(APBPeriph_USBOTG, APBPeriph_USBOTG_CLOCK, DISABLE);
}

/**
  * @brief  Get USB2 PLL clock. Fixed-frequency UTMI PHY output, not register-derived.
  * @retval 480000000
  */
u32 USB2_PLL_ClkGet(void)
{
	return 480 * MHZ_TICK_CNT;
}
