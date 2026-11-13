/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#define COMPIL_LOG_LEVEL        RTK_LOG_DEBUG

#include "ameba_soc.h"
#include "ameba_ddrphy.h"

static const char *TAG = "DDRPHY";

/* Last PLL frequency programmed, so a re-init reproduces the frequency the
 * DDR was actually brought up with. */
static u32 ddr_phy_pll_freq_mhz = DDR_PHY_FREQ_MHZ;

/** @brief  Last frequency programmed via DDR_PHY_SetPLLFreq() (build default
  *         until then). */
u32 DDR_PHY_GetPLLFreq(void)
{
	return ddr_phy_pll_freq_mhz;
}

/** @brief  Enable/gate the DDRPHY clocks, RBUS access and function-enable
  *         line.
  * @note   DISABLE also holds the PHY in reset (REG_FEN_ID*), losing its
  *         programming - not a plain clock gate. Use DDR_PHY_Reset() for a
  *         reset pulse with the clocks left running.
  * @param  NewState: ENABLE or DISABLE. */
void DDR_PHY_Clock(u32 NewState)
{
	if (NewState == ENABLE) {
		RCC_PeriphClockCmd(APBPeriph_DDRPHY, APBPeriph_DDRPHY_CLOCK, ENABLE);

		/* Enable RBUS (+ its clock gate) and the CRT clock for DDR prog */
		CORE_S_BASE->SYS_DDRPHY_CTRL |= (CORE_S_BIT_HS_DDRPHY_CRT_RST | CORE_S_BIT_DDRPHY_RBUS_EN |
										 CORE_S_BIT_HS_DDRPHY_CRT_CLK_EN | CORE_S_BIT_HS_DDRPHY_RBUS_CLK_EN);

		/* Let the RBUS clock gate take effect before the first PHY register access. */
		DelayUs(10);
	} else {
		/* Disable RBUS (+ its clock gate), mirroring the ENABLE path */
		CORE_S_BASE->SYS_DDRPHY_CTRL &= ~(CORE_S_BIT_DDRPHY_RBUS_EN | CORE_S_BIT_HS_DDRPHY_RBUS_CLK_EN);

		RCC_PeriphClockCmd(APBPeriph_DDRPHY, APBPeriph_DDRPHY_CLOCK, DISABLE);
	}
}

/** @brief  Switch the DDR PLL off and release the PHY clock outputs so the
  *         PHY stops driving the DRAM pins. Used during a self-refresh dwell:
  *         only with the drivers dead does the pad power-down hold CKE/RESET
  *         at the die. DDR_PHY_Reset() + init restores everything. */
void DDR_PHY_PLLOff(void)
{
	DDRPHY_DEV->PLL_CTL0 &= ~DDRPHY_MASK_PLL_CTL0_DPI_MCK_CLK_EN;
	DDRPHY_DEV->PLL_CTL1 &= ~DDRPHY_MASK_PLL_CTL1_DPI_CLK_OE;
	DDRPHY_DEV->CRT_RST_CTL &= ~DDRPHY_BIT_CRT_RST_CTL_PLL_SSC_DIG_RST_N;
	DDRPHY_DEV->SSC0 &= ~DDRPHY_BIT_SSC0_DPI_SSC_FLAG_INIT;
	DDRPHY_DEV->CRT_RST_CTL &= ~DDRPHY_BIT_CRT_RST_CTL_PLL_LDO_RST_N;
}

/** @brief  Put the DDRPHY back into its power-on reset state. Pad power
  *         (PWDPAD15N) is left untouched.
  * @note   Needed on a re-init: without a reset edge the divider, PI and
  *         calibration state machines keep running from the previous session,
  *         and reprogramming PI phases on that live clock tree skews the
  *         whole CA/data path.
  */
void DDR_PHY_Reset(void)
{
	/* Park the mux on XTAL first - the fabric may still be clocked from this
	 * PHY's PLL, and the reset would kill our own register-access clock. */
	CORE_S_BASE->SYS_2_CLK_CTRL &= ~CORE_S_BIT_DDRC_CLK_SEL;
	DelayUs(10);

	CORE_S_BASE->SYS_DDRPHY_CTRL &= ~CORE_S_BIT_HS_DDRPHY_CRT_RST;
	RCC_PeriphClockCmd(APBPeriph_DDRPHY, APBPeriph_CLOCK_NULL, DISABLE);
	DelayUs(10);
	RCC_PeriphClockCmd(APBPeriph_DDRPHY, APBPeriph_CLOCK_NULL, ENABLE);
	CORE_S_BASE->SYS_DDRPHY_CTRL |= CORE_S_BIT_HS_DDRPHY_CRT_RST;
	DelayUs(10);
}

/**
  * @brief  Program DDR PLL frequency via DDRPHY SSC3/SSC2.
  * @param  freq_mhz: target PLL frequency in MHz.
  */
void DDR_PHY_SetPLLFreq(u32 freq_mhz)
{
	assert_param(freq_mhz >= DDR_PHY_MIN_FREQ_MHZ);
	assert_param(freq_mhz <= DDR_PHY_MAX_FREQ_MHZ);

	ddr_phy_pll_freq_mhz = freq_mhz;

	/* multiplier = freq / XTAL, expressed as a Q11 fixed-point value so
	 * the integer part lands in the high bits and F_CODE_T in the low 11. */
	u32 multiplier_q11 = (freq_mhz * DDR_PHY_FRAC_DIVISOR + DDR_PHY_XTAL_MHZ / 2)
						 / DDR_PHY_XTAL_MHZ;
	u32 n_code = (multiplier_q11 / DDR_PHY_FRAC_DIVISOR) - DDR_PHY_N_CODE_OFFSET;
	u32 f_code = multiplier_q11 % DDR_PHY_FRAC_DIVISOR;

	DDRPHY_DEV->SSC3   &=    ~DDRPHY_MASK_SSC3_DPI_N_CODE_T;
	DDRPHY_DEV->SSC3   |=    DDRPHY_SSC3_DPI_N_CODE_T(n_code);
	DDRPHY_DEV->SSC2   &=    ~DDRPHY_MASK_SSC2_DPI_F_CODE_T;
	DDRPHY_DEV->SSC2   |=    DDRPHY_SSC2_DPI_F_CODE_T(f_code);

	RTK_LOGD(TAG, "DDR PLL set to %u MHz (N_CODE_T=0x%02x, F_CODE_T=0x%03x)\n",
			 freq_mhz, n_code, f_code);
}

/**
  * @brief  Clear AFIFO RX/TX bypass before DCC programming.
  */
#if !defined(CONFIG_FPGA) || (CONFIG_FPGA == 0)
static void DDR_PHY_ASIC_ClearAfifoBypass(void)
{
	DDRPHY_DEV->AFIFO_STR_1   &=    ~(DDRPHY_BIT_AFIFO_STR_1_BYPASS_EN_RX |
									  DDRPHY_BIT_AFIFO_STR_1_BYPASS_EN_TX);
}

/**
  * @brief  Program DCC/DLL for CMD, CS and CS1.
  */
static void DDR_PHY_ASIC_SetupDCC_Command(void)
{
	DDRPHY_DEV->DCC_CMD    =    DDRPHY_BIT_DCC_CMD_DPI_DCC_CMD_DSP_STOP |
								DDRPHY_DCC_CMD_DPI_DCC_CMD_DIV(DDR_PHY_DCC_DIV_COMMON) |
								DDRPHY_DCC_CMD_DPI_DCC_CMD_INT_GAIN(DDR_PHY_DCC_INT_GAIN_COMMON) |
								DDRPHY_DCC_CMD_DPI_DCC_CMD_DUTY(DDR_PHY_DCC_DUTY_COMMON) |
								DDRPHY_DCC_CMD_DPI_DCC_CMD_DUTY_PRESETB(DDR_PHY_DCC_PRESET_B_COMMON) |
								DDRPHY_DCC_CMD_DPI_DCC_CMD_DUTY_PRESETA(DDR_PHY_DCC_PRESET_A_COMMON);
	DDRPHY_DEV->DCC_CS     =    DDRPHY_BIT_DCC_CS_DPI_DCC_CS_DSP_STOP |
								DDRPHY_DCC_CS_DPI_DCC_CS_DIV(DDR_PHY_DCC_DIV_COMMON) |
								DDRPHY_DCC_CS_DPI_DCC_CS_INT_GAIN(DDR_PHY_DCC_INT_GAIN_COMMON) |
								DDRPHY_DCC_CS_DPI_DCC_CS_DUTY(DDR_PHY_DCC_DUTY_COMMON) |
								DDRPHY_DCC_CS_DPI_DCC_CS_DUTY_PRESETB(DDR_PHY_DCC_PRESET_B_COMMON) |
								DDRPHY_DCC_CS_DPI_DCC_CS_DUTY_PRESETA(DDR_PHY_DCC_PRESET_A_COMMON);
	DDRPHY_DEV->DCC_CS1    =    DDRPHY_BIT_DCC_CS1_DPI_DCC_CS_1_DSP_STOP |
								DDRPHY_DCC_CS1_DPI_DCC_CS_1_DIV(DDR_PHY_DCC_DIV_COMMON) |
								DDRPHY_DCC_CS1_DPI_DCC_CS_1_INT_GAIN(DDR_PHY_DCC_INT_GAIN_COMMON) |
								DDRPHY_DCC_CS1_DPI_DCC_CS_1_DUTY(DDR_PHY_DCC_DUTY_COMMON) |
								DDRPHY_DCC_CS1_DPI_DCC_CS_1_DUTY_PRESETB(DDR_PHY_DCC_PRESET_B_COMMON) |
								DDRPHY_DCC_CS1_DPI_DCC_CS_1_DUTY_PRESETA(DDR_PHY_DCC_PRESET_A_COMMON);
}

/**
  * @brief  Program DCC/DLL duty-cycle correction for all 4 DQS lanes.
  */
static void DDR_PHY_ASIC_SetupDCC_DataStrobe(void)
{
	for (int lane = 0; lane < 4; lane++) {
		DDRPHY_DEV->DCC_DQS[lane]    =    DDRPHY_BIT_DCC_DQS_DPI_DCC_DQS_DSP_STOP |
										  DDRPHY_DCC_DQS_DPI_DCC_DQS_DIV(DDR_PHY_DCC_DIV_COMMON) |
										  DDRPHY_DCC_DQS_DPI_DCC_DQS_INT_GAIN(DDR_PHY_DCC_INT_GAIN_COMMON) |
										  DDRPHY_DCC_DQS_DPI_DCC_DQS_DUTY(DDR_PHY_DCC_DUTY_COMMON) |
										  DDRPHY_DCC_DQS_DPI_DCC_DQS_DUTY_PRESETB(DDR_PHY_DCC_PRESET_B_COMMON) |
										  DDRPHY_DCC_DQS_DPI_DCC_DQS_DUTY_PRESETA(DDR_PHY_DCC_PRESET_A_COMMON);
	}
}

/**
  * @brief  Program DCC/DLL duty-cycle correction for all 4 DQ byte lanes.
  */
static void DDR_PHY_ASIC_SetupDCC_Data(void)
{
	for (int lane = 0; lane < 4; lane++) {
		DDRPHY_DEV->DCC_DQ[lane]    =    DDRPHY_BIT_DCC_DQ_DPI_DCC_DQ_DSP_STOP |
										 DDRPHY_DCC_DQ_DPI_DCC_DQ_DIV(DDR_PHY_DCC_DIV_COMMON) |
										 DDRPHY_DCC_DQ_DPI_DCC_DQ_INT_GAIN(DDR_PHY_DCC_INT_GAIN_COMMON) |
										 DDRPHY_DCC_DQ_DPI_DCC_DQ_DUTY(DDR_PHY_DCC_DUTY_COMMON) |
										 DDRPHY_DCC_DQ_DPI_DCC_DQ_DUTY_PRESETB(DDR_PHY_DCC_PRESET_B_COMMON) |
										 DDRPHY_DCC_DQ_DPI_DCC_DQ_DUTY_PRESETA(DDR_PHY_DCC_PRESET_A_COMMON);
	}
}

/**
  * @brief  Program DCC/DLL duty-cycle correction for the two CK lanes.
  */
static void DDR_PHY_ASIC_SetupDCC_Clock(void)
{
	for (int lane = 0; lane < 2; lane++) {
		DDRPHY_DEV->DCC_CK[lane]    =    DDRPHY_BIT_DCC_CK_DPI_DCC_CK_DSP_STOP |
										 DDRPHY_DCC_CK_DPI_DCC_CK_DIV(DDR_PHY_DCC_DIV_COMMON) |
										 DDRPHY_DCC_CK_DPI_DCC_CK_INT_GAIN(DDR_PHY_DCC_INT_GAIN_COMMON) |
										 DDRPHY_DCC_CK_DPI_DCC_CK_DUTY(DDR_PHY_DCC_DUTY_COMMON) |
										 DDRPHY_DCC_CK_DPI_DCC_CK_DUTY_PRESETB(DDR_PHY_DCC_PRESET_B_COMMON) |
										 DDRPHY_DCC_CK_DPI_DCC_CK_DUTY_PRESETA(DDR_PHY_DCC_PRESET_A_COMMON);
	}
}

/**
  * @brief  Run DCC/DLL boot up across all signal groups.
  */
static void DDR_PHY_ASIC_SetupDCC(void)
{
	DDR_PHY_ASIC_ClearAfifoBypass();
	DDR_PHY_ASIC_SetupDCC_Command();
	DDR_PHY_ASIC_SetupDCC_DataStrobe();
	DDR_PHY_ASIC_SetupDCC_Data();
	DDR_PHY_ASIC_SetupDCC_Clock();
}

/**
  * @brief  Walk PLL_CTL3 through the analog-lock tuning, then PLL_CTL5/CTL2.
  */
static void DDR_PHY_ASIC_SetupPLL_AnalogLock(void)
{
	//dpi_cco_band = 2'b3, dpi_cco_kvco = 1'b1
	DDRPHY_DEV->PLL_CTL3    =    DDRPHY_PLL_CTL3_DPI_V11_LDO_VSEL(0x0) |
								 DDRPHY_PLL_CTL3_DPI_POST_PI_RL(0x2) |
								 DDRPHY_PLL_CTL3_DPI_POST_DIVN(0x1) |
								 DDRPHY_PLL_CTL3_DPI_CK_LDO_VSEL(0x0) |
								 DDRPHY_PLL_CTL3_DPI_PDIV(0x0) |
								 DDRPHY_PLL_CTL3_DPI_LPF_SR(0x5) |
								 DDRPHY_PLL_CTL3_DPI_LOOP_PI_ISEL(0x7) |
								 DDRPHY_PLL_CTL3_DPI_ICP(0x8) |
								 DDRPHY_PLL_CTL3_DPI_CCO_BAND(0x3) |
								 DDRPHY_BIT_PLL_CTL3_DPI_PLL_SEL_CPMODE |
								 DDRPHY_BIT_PLL_CTL3_DPI_LPF_CP |
								 DDRPHY_BIT_PLL_CTL3_DPI_CCO_KVCO;
	//dpi_loop_pi_isel = 4'h4
	DDRPHY_DEV->PLL_CTL3    = (DDRPHY_DEV->PLL_CTL3 & ~DDRPHY_MASK_PLL_CTL3_DPI_LOOP_PI_ISEL) |
							  DDRPHY_PLL_CTL3_DPI_LOOP_PI_ISEL(0x4);
	//dpi_post_pi_bias = 1'b1
	DDRPHY_DEV->PLL_CTL5    =    DDRPHY_BIT_PLL_CTL5_DPI_POST_PI_BIAS |
								 DDRPHY_PLL_CTL5_DPI_PWROFF_PWR_WAIT_CYCLE(0x7) |
								 DDRPHY_PLL_CTL5_DPI_PWROFF_RD_WAIT_CYCLE(0x7) |
								 DDRPHY_PLL_CTL5_DPI_PWROFF_WR_WAIT_CYCLE(0x7) |
								 DDRPHY_PLL_CTL5_DPI_RW_PWROFF_MODE(0x0) |
								 DDRPHY_PLL_CTL5_DPI_CA_PWROFF_MODE(0x0) |
								 DDRPHY_BIT_PLL_CTL5_DPI_POSTDIV_EN |
								 0x40000000;
	//dpi_post_pi_rl = 2'b01
	DDRPHY_DEV->PLL_CTL3    = (DDRPHY_DEV->PLL_CTL3 & ~DDRPHY_MASK_PLL_CTL3_DPI_POST_PI_RL) | DDRPHY_PLL_CTL3_DPI_POST_PI_RL(0x1);
	//dpi_post_pi_rs = 1'b0
	DDRPHY_DEV->PLL_CTL3    = (DDRPHY_DEV->PLL_CTL3 & ~DDRPHY_BIT_PLL_CTL3_DPI_POST_PI_RS);
	//dpi_pdiv = 2'b00
	DDRPHY_DEV->PLL_CTL3    = (DDRPHY_DEV->PLL_CTL3 & ~DDRPHY_MASK_PLL_CTL3_DPI_POST_DIVN) | DDRPHY_PLL_CTL3_DPI_POST_DIVN(0x0);
	//dpi_pll_sel_cpmode = 1'b1
	DDRPHY_DEV->PLL_CTL3    = (DDRPHY_DEV->PLL_CTL3 & ~DDRPHY_BIT_PLL_CTL3_DPI_PLL_SEL_CPMODE);
	//dpi_icp = 4'h7
	DDRPHY_DEV->PLL_CTL3    = (DDRPHY_DEV->PLL_CTL3 & ~DDRPHY_MASK_PLL_CTL3_DPI_ICP) | DDRPHY_PLL_CTL3_DPI_ICP(0x7);
	//dpi_lpf_sr = 3'h2
	DDRPHY_DEV->PLL_CTL3    = (DDRPHY_DEV->PLL_CTL3 & ~DDRPHY_MASK_PLL_CTL3_DPI_LPF_SR) | DDRPHY_PLL_CTL3_DPI_LPF_SR(0x2);
	//dpi_pll_dum = 8'hff DVDD short
	DDRPHY_DEV->PLL_CTL2    =    DDRPHY_PLL_CTL2_DPI_REG_CTS_FIFO_SYNC_CNT(0x2) | DDRPHY_PLL_CTL2_DPI_PLL_DUM(0xff);
}

/**
  * @brief  Program PLL_PI0..PI3 phase selectors and PLL_CTL1.OESYNC_OP_SEL.
  */
static void DDR_PHY_ASIC_SetupPLL_PhaseSelect(void)
{
	DDRPHY_DEV->PLL_PI0     =    DDRPHY_PLL_PI0_DPI_POST_PI_SEL3(0x2) |
								 DDRPHY_PLL_PI0_DPI_POST_PI_SEL2(0x2) |
								 DDRPHY_PLL_PI0_DPI_POST_PI_SEL1(0xf) |
								 DDRPHY_PLL_PI0_DPI_POST_PI_SEL0(0x1b);
	DDRPHY_DEV->PLL_PI1     =    DDRPHY_PLL_PI1_DPI_POST_PI_SEL(0x0) |
								 DDRPHY_PLL_PI1_DPI_POST_PI_SEL6(0x6) |
								 DDRPHY_PLL_PI1_DPI_POST_PI_SEL5(0x0) |
								 DDRPHY_PLL_PI1_DPI_POST_PI_SEL4(0x0);
	DDRPHY_DEV->PLL_PI2     =    DDRPHY_PLL_PI2_DPI_POST_PI_SEL10(0x2) |
								 DDRPHY_PLL_PI2_DPI_POST_PI_SEL9(0x0) |
								 DDRPHY_PLL_PI2_DPI_POST_PI_SEL8(0x0) |
								 DDRPHY_PLL_PI2_DPI_POST_PI_SEL7(0x5);
	DDRPHY_DEV->PLL_PI3     =    DDRPHY_PLL_PI3_DPI_POST_PI_SEL12(0x0) |
								 DDRPHY_PLL_PI3_DPI_POST_PI_SEL11(0x0);
	DDRPHY_DEV->PLL_CTL1    = (DDRPHY_DEV->PLL_CTL1 & ~DDRPHY_MASK_PLL_CTL1_DPI_OESYNC_OP_SEL) |
							  DDRPHY_PLL_CTL1_DPI_OESYNC_OP_SEL(0x1);
}

/**
  * @brief  Bring up PLL: analog lock, then PI phase selectors.
  */
static void DDR_PHY_ASIC_SetupPLL(void)
{
	DDR_PHY_ASIC_SetupPLL_AnalogLock();
	DDR_PHY_ASIC_SetupPLL_PhaseSelect();
}

/**
  * @brief  Program per-byte AFIFO stream counts and command stream depth.
  */
static void DDR_PHY_ASIC_SetupAFIFO(void)
{
	/* TX DQ/DQS FIFO thresholds (golden 0x00001111): slice0/1 = 1, slice2/3 = 0 */
	DDRPHY_DEV->AFIFO_STR_0      =    DDRPHY_AFIFO_STR_0_DQS_RD_STR_NUM_3(0x1) |
									  DDRPHY_AFIFO_STR_0_DQ_RD_STR_NUM_3(0x1) |
									  DDRPHY_AFIFO_STR_0_DQS_RD_STR_NUM_2(0x1) |
									  DDRPHY_AFIFO_STR_0_DQ_RD_STR_NUM_2(0x1) |
									  DDRPHY_AFIFO_STR_0_DQS_RD_STR_NUM_1(0x2) |
									  DDRPHY_AFIFO_STR_0_DQ_RD_STR_NUM_1(0x2) |
									  DDRPHY_AFIFO_STR_0_DQS_RD_STR_NUM_0(0x2) |
									  DDRPHY_AFIFO_STR_0_DQ_RD_STR_NUM_0(0x2);
	DDRPHY_DEV->AFIFO_STR_2      =    DDRPHY_AFIFO_STR_2_RX_RD_STR_NUM_3(0x1) |
									  DDRPHY_AFIFO_STR_2_RX_RD_STR_NUM_2(0x1) |
									  DDRPHY_AFIFO_STR_2_RX_RD_STR_NUM_1(0x1) |
									  DDRPHY_AFIFO_STR_2_RX_RD_STR_NUM_0(0x1);
	/* FORCE_RST_N_* are active-low AFIFO resets, and the write below sets them to
	 * 1. On cold boot the register starts at 0, so that is a genuine 0->1 release
	 * and the TX/RX FIFO pointers reset. On a re-init they are already 1, no edge
	 * is produced, and the pointers stay where the previous session left them -
	 * the TX FIFO then clocks out empty slots, so DQS toggles, the DRAM captures,
	 * and every write lands as zero while reads stay correct. Drive them low first
	 * to guarantee the edge. */
	DDRPHY_DEV->AFIFO_STR_1     &=   ~(DDRPHY_BIT_AFIFO_STR_1_FORCE_RST_N_RX |
									   DDRPHY_BIT_AFIFO_STR_1_FORCE_RST_N_TX);

	DDRPHY_DEV->AFIFO_STR_1      =    DDRPHY_AFIFO_STR_1_CMD_RD_STR_NUM_SEL_H(0x0) |
									  DDRPHY_AFIFO_STR_1_CMD_EX_RD_STR_NUM(0x2) |
									  DDRPHY_AFIFO_STR_1_CMD_RD_STR_NUM(0x1) |
									  DDRPHY_BIT_AFIFO_STR_1_FORCE_RST_N_RX |
									  DDRPHY_BIT_AFIFO_STR_1_FORCE_RST_N_TX;
	DDRPHY_DEV->AFIFO_STR_SEL    =    DDRPHY_AFIFO_STR_SEL_CMD_RD_STR_NUM_SEL(0x0);
}

/**
  * @brief  Program DDR PHY analog LDO voltages
  */
static void DDR_PHY_ASIC_SetupLDOSupply(void)
{
	DDRPHY_DEV->PLL_CTL3    =    DDRPHY_PLL_CTL3_DPI_V11_LDO_VSEL(0x2) |
								 DDRPHY_PLL_CTL3_DPI_POST_PI_RL(0x1) |
								 DDRPHY_PLL_CTL3_DPI_POST_DIVN(0x1) |
								 DDRPHY_PLL_CTL3_DPI_CK_LDO_VSEL(0x2) |
								 DDRPHY_PLL_CTL3_DPI_PDIV(0x0) |
								 DDRPHY_PLL_CTL3_DPI_LPF_SR(0x2) |
								 DDRPHY_PLL_CTL3_DPI_LOOP_PI_ISEL(0x4) |
								 DDRPHY_PLL_CTL3_DPI_ICP(0x7) |
								 DDRPHY_PLL_CTL3_DPI_CCO_BAND(0x3) |
								 DDRPHY_BIT_PLL_CTL3_DPI_PLL_SEL_CPMODE |
								 DDRPHY_BIT_PLL_CTL3_DPI_LPF_CP |
								 DDRPHY_BIT_PLL_CTL3_DPI_CCO_KVCO;
}

/**
  * @brief  Select DDRC clock source.
  * @param  src: 0 = XTAL 40 MHz, 1 = DDR PLL.
  */
static void DDR_PHY_ASIC_SelectClkSrc(int src)
{
	if (src == 0) {
		CORE_S_BASE->SYS_2_CLK_CTRL   &=    ~CORE_S_BIT_DDRC_CLK_SEL;
	} else {
		DelayUs(10);
		CORE_S_BASE->SYS_2_CLK_CTRL   |=    CORE_S_BIT_DDRC_CLK_SEL;
	}
}

/**
  * @brief  Set CRT_CTL DDR3 FORCE_* hold value, then release one forcing bit per step.
  */
static void DDR_PHY_ASIC_WalkCrtCtlBringUp(void)
{
	//[0]pll_ldo_rst_n=1
	DDRPHY_DEV->CRT_RST_CTL    =    DDRPHY_BIT_CRT_RST_CTL_PTR_RST_N |
									DDRPHY_BIT_CRT_RST_CTL_PLL_LDO_RST_N;
	//mck_cke = mck_cs
	DDRPHY_DEV->CRT_CTL        =    DDRPHY_CRT_CTL_FIFO_RST_INI_TX(0x0) |
									DDRPHY_CRT_CTL_FIFO_RST_INI_RX(0x0) |
									DDRPHY_BIT_CRT_CTL_MCK_CKE_SEL |
									DDRPHY_BIT_CRT_CTL_FORCE_RSTN |
									DDRPHY_BIT_CRT_CTL_RSTN_VALUE |
									DDRPHY_BIT_CRT_CTL_CA_DDR3_MODE |
									DDRPHY_BIT_CRT_CTL_FORCE_CKE_1 |
									DDRPHY_BIT_CRT_CTL_CS_VALUE |
									DDRPHY_BIT_CRT_CTL_CS_1_VALUE |
									DDRPHY_BIT_CRT_CTL_DDR_MODE |
									DDRPHY_BIT_CRT_CTL_FORCE_CKE |
									DDRPHY_BIT_CRT_CTL_PIN_MUX_DDR3 |
									DDRPHY_BIT_CRT_CTL_DPI_DDR3_MODE;
	//force_rstn = 0 , rstn_value = 1, ca_ddr3_mode = 1
	DDRPHY_DEV->CRT_CTL        &=   ~DDRPHY_BIT_CRT_CTL_FORCE_RSTN;
	//force_cke_1 = 0 , cke_1_value = 0, force_cs = 0 , cs_value = 1, force_cs_1 = 0 , cs_1_value = 1
	DDRPHY_DEV->CRT_CTL        &=	~DDRPHY_BIT_CRT_CTL_FORCE_CKE_1;
	//ddr2_mode = 0
	DDRPHY_DEV->CRT_CTL        &=   ~DDRPHY_BIT_CRT_CTL_DDR_MODE;
	//force_cke = 0 , cke_value = 0, pin_mux : ddr3 , pin_mux_mode = 0, dpi_lpddr_en = 0 , dpi_dat_rate_sel = 0 , dpi_ddr2_en = 0 , dpi_ddr3_mode = 1
	DDRPHY_DEV->CRT_CTL        &=	~DDRPHY_BIT_CRT_CTL_FORCE_CKE;
}

/**
  * @brief  Release PHY from reset: LDO wait, SSC up, pulse PTR_RST_N, RST_N up.
  */
static void DDR_PHY_ASIC_ReleasePhyReset(void)
{
	DelayUs(100);
	//[1]pll_ssc_dig_rst_n=1
	DDRPHY_DEV->CRT_RST_CTL    =    DDRPHY_BIT_CRT_RST_CTL_PTR_RST_N |
									DDRPHY_BIT_CRT_RST_CTL_PLL_SSC_DIG_RST_N |
									DDRPHY_BIT_CRT_RST_CTL_PLL_LDO_RST_N;
	DelayUs(50);
	//[3]ptr_rst_n=0
	DDRPHY_DEV->CRT_RST_CTL    =    DDRPHY_BIT_CRT_RST_CTL_PLL_SSC_DIG_RST_N |
									DDRPHY_BIT_CRT_RST_CTL_PLL_LDO_RST_N;
	//[0]pll_ldo_rst_n=1, [1]pll_ssc_dig_rst_n=1
	DDRPHY_DEV->CRT_RST_CTL    =    DDRPHY_BIT_CRT_RST_CTL_PTR_RST_N |
									DDRPHY_BIT_CRT_RST_CTL_RST_N |
									DDRPHY_BIT_CRT_RST_CTL_PLL_SSC_DIG_RST_N |
									DDRPHY_BIT_CRT_RST_CTL_PLL_LDO_RST_N;
	DelayUs(50);
}

/**
  * @brief  Toggle OC_EN, seed SSC, release MCK_EN, reset AFIFO bypass.
  */
static void DDR_PHY_ASIC_ReleaseReset(void)
{
	DDRPHY_DEV->SSC3          &=    ~DDRPHY_BIT_SSC3_DPI_OC_EN;
	DDRPHY_DEV->SSC3          |=    DDRPHY_BIT_SSC3_DPI_OC_EN;
	DDRPHY_DEV->SSC3          &=    ~DDRPHY_BIT_SSC3_DPI_OC_EN;
	DDRPHY_DEV->SSC0          &=    ~DDRPHY_BIT_SSC0_DPI_SSC_FLAG_INIT;
	DDRPHY_DEV->PLL_CTL1      &=    ~DDRPHY_BIT_PLL_CTL1_MCK_MC_EN;
	DDRPHY_DEV->PLL_CTL1      |=    DDRPHY_BIT_PLL_CTL1_MCK_MC_EN;
	DDRPHY_DEV->PLL_CTL0      &=    ~DDRPHY_MASK_PLL_CTL0_DPI_MCK_CLK_EN;
	DDRPHY_DEV->PLL_CTL0      |=    DDRPHY_PLL_CTL0_DPI_MCK_CLK_EN(0x1fff);
#if !defined(CONFIG_DDR_PHY_THREE_POINT_K_ENABLE) || (CONFIG_DDR_PHY_THREE_POINT_K_ENABLE == 0)
	DDRPHY_DEV->PLL_CTL0      |=    DDRPHY_BIT_PLL_CTL0_AFA_DCG_EN | DDRPHY_BIT_PLL_CTL0_TMUX_DCG_EN;
#endif
	DDRPHY_DEV->AFIFO_STR_1   &=    ~(DDRPHY_BIT_AFIFO_STR_1_BYPASS_EN_RX |
									  DDRPHY_BIT_AFIFO_STR_1_BYPASS_EN_TX);
	DDRPHY_DEV->PLL_CTL1      &=    ~DDRPHY_MASK_PLL_CTL1_DPI_CLK_OE;
	DDRPHY_DEV->PLL_CTL1      |=    DDRPHY_PLL_CTL1_DPI_CLK_OE(0x7);
}

/**
  * @brief  Release DDR PHY pads from power-down (PWDPAD15N high).
  */
static void DDR_PHY_ASIC_SetupPwdPad(void)
{
	CORE_S_BASE->SYS_DDRPHY_CTRL    = (CORE_S_BIT_HS_DDRPHY_CRT_RST | CORE_S_BIT_PWDPAD15N | CORE_S_BIT_DDRPHY_RBUS_EN | CORE_S_BIT_HS_DDRPHY_CRT_CLK_EN);
}

/**
  * @brief  Enable PAD output drivers on all DDR3 signals.
  */
static void DDR_PHY_ASIC_EnablePadOE(void)
{
	DDRPHY_DEV->PAD_OE_CTL       =    DDRPHY_BIT_PAD_OE_CTL_ADR54_1TO1 |
									  DDRPHY_BIT_PAD_OE_CTL_DCK_1_OE |
									  DDRPHY_BIT_PAD_OE_CTL_DCK_OE |
									  DDRPHY_BIT_PAD_OE_CTL_CKE_OE |
									  DDRPHY_BIT_PAD_OE_CTL_RAS_N_OE |
									  DDRPHY_BIT_PAD_OE_CTL_CAS_N_OE |
									  DDRPHY_BIT_PAD_OE_CTL_WE_N_OE |
									  DDRPHY_BIT_PAD_OE_CTL_ODT_OE |
									  DDRPHY_BIT_PAD_OE_CTL_RESET_N_OE |
									  DDRPHY_BIT_PAD_OE_CTL_BK2_OE |
									  DDRPHY_BIT_PAD_OE_CTL_BK1_OE |
									  DDRPHY_BIT_PAD_OE_CTL_BK0_OE |
									  DDRPHY_BIT_PAD_OE_CTL_EXT_ADR5_OE |
									  DDRPHY_BIT_PAD_OE_CTL_EXT_ADR4_OE |
									  DDRPHY_BIT_PAD_OE_CTL_ADR15_OE |
									  DDRPHY_BIT_PAD_OE_CTL_ADR14_OE |
									  DDRPHY_BIT_PAD_OE_CTL_ADR13_OE |
									  DDRPHY_BIT_PAD_OE_CTL_ADR12_OE |
									  DDRPHY_BIT_PAD_OE_CTL_ADR11_OE |
									  DDRPHY_BIT_PAD_OE_CTL_ADR10_OE |
									  DDRPHY_BIT_PAD_OE_CTL_ADR09_OE |
									  DDRPHY_BIT_PAD_OE_CTL_ADR08_OE |
									  DDRPHY_BIT_PAD_OE_CTL_ADR07_OE |
									  DDRPHY_BIT_PAD_OE_CTL_ADR06_OE |
									  DDRPHY_BIT_PAD_OE_CTL_ADR05_OE |
									  DDRPHY_BIT_PAD_OE_CTL_ADR04_OE |
									  DDRPHY_BIT_PAD_OE_CTL_ADR03_OE |
									  DDRPHY_BIT_PAD_OE_CTL_ADR02_OE |
									  DDRPHY_BIT_PAD_OE_CTL_ADR01_OE |
									  DDRPHY_BIT_PAD_OE_CTL_ADR00_OE;
	DDRPHY_DEV->PAD_OE_CTL_EX    =    DDRPHY_BIT_PAD_OE_CTL_EX_ACT_N_OE |
									  DDRPHY_BIT_PAD_OE_CTL_EX_DDR4_TEN_OE |
									  DDRPHY_BIT_PAD_OE_CTL_EX_DDR4_ALERT0_OE |
									  DDRPHY_BIT_PAD_OE_CTL_EX_DDR4_ALERT1_OE |
									  DDRPHY_BIT_PAD_OE_CTL_EX_DDR4_CKE_OE |
									  DDRPHY_BIT_PAD_OE_CTL_EX_DDR4_BG0_OE |
									  DDRPHY_BIT_PAD_OE_CTL_EX_PARITY_OE |
									  DDRPHY_BIT_PAD_OE_CTL_EX_PARITY_1_OE |
									  DDRPHY_BIT_PAD_OE_CTL_EX_ADR16_OE |
									  DDRPHY_BIT_PAD_OE_CTL_EX_DDR4_CS_N_OE |
									  DDRPHY_BIT_PAD_OE_CTL_EX_CKE_1_OE |
									  DDRPHY_BIT_PAD_OE_CTL_EX_CS_N_1_OE |
									  DDRPHY_BIT_PAD_OE_CTL_EX_CS_N_OE;
}

/**
  * @brief  Disable ODT: clear ODT_TTC* and write fixed OCDP/OCDN codes.
  */
static void DDR_PHY_ASIC_DisableODT(void)
{
	DDRPHY_DEV->ODT_TTCP0_SET1    =    DDRPHY_ODT_TTCP0_SET1_ODT_TTCP_SET_3(0x0) |
									   DDRPHY_ODT_TTCP0_SET1_ODT_TTCP_SET_2(0x0) |
									   DDRPHY_ODT_TTCP0_SET1_ODT_TTCP_SET_1(0x0) |
									   DDRPHY_ODT_TTCP0_SET1_ODT_TTCP_SET_0(0x0);
	DDRPHY_DEV->ODT_TTCP1_SET1    =    DDRPHY_ODT_TTCP1_SET1_ODT_TTCP_SET_7(0x0) |
									   DDRPHY_ODT_TTCP1_SET1_ODT_TTCP_SET_6(0x0) |
									   DDRPHY_ODT_TTCP1_SET1_ODT_TTCP_SET_5(0x0) |
									   DDRPHY_ODT_TTCP1_SET1_ODT_TTCP_SET_4(0x0);
	DDRPHY_DEV->ODT_TTCP2_SET1    =    DDRPHY_ODT_TTCP2_SET1_ODT_TTCP_SET_11(0x0) |
									   DDRPHY_ODT_TTCP2_SET1_ODT_TTCP_SET_10(0x0) |
									   DDRPHY_ODT_TTCP2_SET1_ODT_TTCP_SET_9(0x0) |
									   DDRPHY_ODT_TTCP2_SET1_ODT_TTCP_SET_8(0x0);
	DDRPHY_DEV->ODT_TTCN0_SET1    =    DDRPHY_ODT_TTCN0_SET1_ODT_TTCN_SET_3(0x0) |
									   DDRPHY_ODT_TTCN0_SET1_ODT_TTCN_SET_2(0x0) |
									   DDRPHY_ODT_TTCN0_SET1_ODT_TTCN_SET_1(0x0) |
									   DDRPHY_ODT_TTCN0_SET1_ODT_TTCN_SET_0(0x0);
	DDRPHY_DEV->ODT_TTCN1_SET1    =    DDRPHY_ODT_TTCN1_SET1_ODT_TTCN_SET_7(0x0) |
									   DDRPHY_ODT_TTCN1_SET1_ODT_TTCN_SET_6(0x0) |
									   DDRPHY_ODT_TTCN1_SET1_ODT_TTCN_SET_5(0x0) |
									   DDRPHY_ODT_TTCN1_SET1_ODT_TTCN_SET_4(0x0);
	DDRPHY_DEV->ODT_TTCN2_SET1    =    DDRPHY_ODT_TTCN2_SET1_ODT_TTCN_SET_11(0x0) |
									   DDRPHY_ODT_TTCN2_SET1_ODT_TTCN_SET_10(0x0) |
									   DDRPHY_ODT_TTCN2_SET1_ODT_TTCN_SET_9(0x0) |
									   DDRPHY_ODT_TTCN2_SET1_ODT_TTCN_SET_8(0x0);

	DDRPHY_DEV->OCDP0_SET1        =    DDRPHY_OCDP0_SET1_OCDP_SET_3(0x23) |
									   DDRPHY_OCDP0_SET1_OCDP_SET_2(0x23) |
									   DDRPHY_OCDP0_SET1_OCDP_SET_1(0x23) |
									   DDRPHY_OCDP0_SET1_OCDP_SET_0(0x23);
	DDRPHY_DEV->OCDP1_SET1        =    DDRPHY_OCDP1_SET1_OCDP_SET_7(0x23) |
									   DDRPHY_OCDP1_SET1_OCDP_SET_6(0x23) |
									   DDRPHY_OCDP1_SET1_OCDP_SET_5(0x23) |
									   DDRPHY_OCDP1_SET1_OCDP_SET_4(0x23);
	DDRPHY_DEV->OCDP2_SET1        =    DDRPHY_OCDP2_SET1_OCDP_SET_11(0x23) |
									   DDRPHY_OCDP2_SET1_OCDP_SET_10(0x23) |
									   DDRPHY_OCDP2_SET1_OCDP_SET_9(0x23) |
									   DDRPHY_OCDP2_SET1_OCDP_SET_8(0x23);
	DDRPHY_DEV->OCDN0_SET1        =    DDRPHY_OCDN0_SET1_OCDN_SET_3(0x21) |
									   DDRPHY_OCDN0_SET1_OCDN_SET_2(0x21) |
									   DDRPHY_OCDN0_SET1_OCDN_SET_1(0x21) |
									   DDRPHY_OCDN0_SET1_OCDN_SET_0(0x21);
	DDRPHY_DEV->OCDN1_SET1        =    DDRPHY_OCDN1_SET1_OCDN_SET_7(0x21) |
									   DDRPHY_OCDN1_SET1_OCDN_SET_6(0x21) |
									   DDRPHY_OCDN1_SET1_OCDN_SET_5(0x21) |
									   DDRPHY_OCDN1_SET1_OCDN_SET_4(0x21);
	DDRPHY_DEV->OCDN2_SET1        =    DDRPHY_OCDN2_SET1_OCDN_SET_11(0x21) |
									   DDRPHY_OCDN2_SET1_OCDN_SET_10(0x21) |
									   DDRPHY_OCDN2_SET1_OCDN_SET_9(0x21) |
									   DDRPHY_OCDN2_SET1_OCDN_SET_8(0x21);
}

/**
  * @brief  Run R480 reference calibration (anchor for ZQ sets).
  */
static void DDR_PHY_ASIC_CalibrateR480(void)
{
	DDRPHY_DEV->PAD_BUS_2        = (DDRPHY_DEV->PAD_BUS_2 & ~(DDRPHY_PAD_BUS_2_VREF_S_0(0x3f) | DDRPHY_BIT_PAD_BUS_2_VREF_RANGE_0 | DDRPHY_BIT_PAD_BUS_2_VREF_PD_0))
								   |
								   DDRPHY_PAD_BUS_2_VREF_S_0(0x6) | DDRPHY_BIT_PAD_BUS_2_VREF_RANGE_0;
	DDRPHY_DEV->PAD_CTRL_PROG   &=    ~DDRPHY_MASK_PAD_CTRL_PROG_ZCTRL_CLK_SEL;
	//zctrl_clk_sel=0x2 (clk/32)
	DDRPHY_DEV->PAD_CTRL_PROG   |=    DDRPHY_PAD_CTRL_PROG_ZCTRL_CLK_SEL(0x2);
	//rzq_ext_R240=0 (internal resistor)
	DDRPHY_DEV->PAD_CTRL_PROG   &=    ~DDRPHY_BIT_PAD_CTRL_PROG_RZQ_EXT_R240;
	//zq_pd = 0x0 (ZQ PAD PD , pad_bus_1[6])
	DDRPHY_DEV->PAD_BUS_1       &=    ~(DDRPHY_PAD_BUS_1_PAD_BUS_1(1 << 6));
	//zq_ena_nocd2=0
	DDRPHY_DEV->ZQ_NOCD2        &=    ~DDRPHY_BIT_ZQ_NOCD2_ZQ_ENA_NOCD2;
	//zq_en_dmyi=0
	DDRPHY_DEV->PAD_BUS_0       &=    ~DDRPHY_BIT_PAD_BUS_0_EN_DMYI;
	//fw_set_mode=immediately
	DDRPHY_DEV->DPI_CTRL_0      &=    ~DDRPHY_MASK_DPI_CTRL_0_FW_SET_MODE;
	DDRPHY_DEV->DPI_CTRL_0      |=    DDRPHY_DPI_CTRL_0_FW_SET_MODE(0x2);
	//fw_set_wr_dly
	DDRPHY_DEV->DPI_CTRL_1       =    DDRPHY_BIT_DPI_CTRL_1_WRITE_EN_0 |
									  DDRPHY_BIT_DPI_CTRL_1_FW_SET_WR_DLY;
	//rzq_cal_en=1
	DDRPHY_DEV->PAD_CTRL_PROG   |=    DDRPHY_BIT_PAD_CTRL_PROG_RZQ_CAL_EN;
	DelayUs(100);
	//rzq_cal_done=1
	u32 rzctrl_sts = DDRPHY_DEV->PAD_RZCTRL_STATUS;
	RTK_LOGD(TAG, "R480 rzq_cal_done=%u rzq_480code=0x%02x (PAD_RZCTRL_STATUS=0x%08x)\n",
			 rzctrl_sts & DDRPHY_BIT_PAD_RZCTRL_STATUS_RZQ_CAL_DONE,
			 (rzctrl_sts & DDRPHY_MASK_PAD_RZCTRL_STATUS_RZQ_480CODE) >> 1,
			 rzctrl_sts);
	//fw_set_wr_dly
	DDRPHY_DEV->DPI_CTRL_1       =    DDRPHY_BIT_DPI_CTRL_1_WRITE_EN_0 |
									  DDRPHY_BIT_DPI_CTRL_1_FW_SET_WR_DLY;
	//r 0x6033014
	RTK_LOGD(TAG, "R480 PAD_ZCTRL_STATUS=0x%08x\n", DDRPHY_DEV->PAD_ZCTRL_STATUS);
	//rzq_cal_en=0
	DDRPHY_DEV->PAD_CTRL_PROG   &=    ~DDRPHY_BIT_PAD_CTRL_PROG_RZQ_CAL_EN;
}

/* OCD drive strength (ohm), selected by ddr_adapter.ca_ocd / dq_ocd (DDR_SetOCD).
 * Codes for 40/60/80/100, indexed by (ohm - 40) / 20. CA drives from OCD set 3. */
static const u8 DDR_PHY_CA_OCDP[4] = {0x26, 0x1a, 0x14, 0x11};
static const u8 DDR_PHY_CA_OCDN[4] = {0x25, 0x19, 0x13, 0x10};

/* DQ 40 selects OCD set 0 (0x26/0x25); DQ 60/80/100 select OCD set 2 with these
 * codes. Set 2 is written either way, the DQ 40 entry is unused by DQ. */
static const u8 DDR_PHY_DQ_OCDP[4] = {0x1f, 0x1a, 0x14, 0x11};
static const u8 DDR_PHY_DQ_OCDN[4] = {0x1e, 0x19, 0x13, 0x10};
#define DDR_PHY_DQ_OCD_SEL	((ddr_adapter.dq_ocd == 40) ? 0x0 : 0x2)

static u32 DDR_PHY_OcdIndex(u8 ohm)
{
	u32 idx = ((u32)ohm - 40U) / 20U;

	return (idx < 4U) ? idx : 3U;
}

/**
  * @brief  Write the calibrated ODT/OCD impedance codes into the SET0 registers.
  * @note   These are the resistance values produced by ZQ calibration; the ZQK
  *         reference is designed for 1.5V.
  */
static void DDR_PHY_ASIC_ProgramCalImpedance(void)
{
	u32 ca = DDR_PHY_OcdIndex(ddr_adapter.ca_ocd);
	u32 dq = DDR_PHY_OcdIndex(ddr_adapter.dq_ocd);

	/* podt3~0  800/300/75/60 */
	DDRPHY_DEV->ODT_TTCP0_SET0        =    DDRPHY_ODT_TTCP0_SET0_ODT_TTCP_SET_3_PRE(0x03) |
										   DDRPHY_ODT_TTCP0_SET0_ODT_TTCP_SET_2_PRE(0x06) |
										   DDRPHY_ODT_TTCP0_SET0_ODT_TTCP_SET_1_PRE(0x15) |
										   DDRPHY_ODT_TTCP0_SET0_ODT_TTCP_SET_0_PRE(0x1a);
	/* podt7~4  open/600/120/150 */
	DDRPHY_DEV->ODT_TTCP1_SET0        =    DDRPHY_ODT_TTCP1_SET0_ODT_TTCP_SET_7_PRE(0x00) |
										   DDRPHY_ODT_TTCP1_SET0_ODT_TTCP_SET_6_PRE(0x04) |
										   DDRPHY_ODT_TTCP1_SET0_ODT_TTCP_SET_5_PRE(0x0e) |
										   DDRPHY_ODT_TTCP1_SET0_ODT_TTCP_SET_4_PRE(0x0c);
	/* nodt3~0 */
	DDRPHY_DEV->ODT_TTCN0_SET0        =    DDRPHY_ODT_TTCN0_SET0_ODT_TTCN_SET_3_PRE(0x02) |
										   DDRPHY_ODT_TTCN0_SET0_ODT_TTCN_SET_2_PRE(0x05) |
										   DDRPHY_ODT_TTCN0_SET0_ODT_TTCN_SET_1_PRE(0x14) |
										   DDRPHY_ODT_TTCN0_SET0_ODT_TTCN_SET_0_PRE(0x19);
	/* nodt7~4 */
	DDRPHY_DEV->ODT_TTCN1_SET0        =    DDRPHY_ODT_TTCN1_SET0_ODT_TTCN_SET_7_PRE(0x00) |
										   DDRPHY_ODT_TTCN1_SET0_ODT_TTCN_SET_6_PRE(0x03) |
										   DDRPHY_ODT_TTCN1_SET0_ODT_TTCN_SET_5_PRE(0x0d) |
										   DDRPHY_ODT_TTCN1_SET0_ODT_TTCN_SET_4_PRE(0x0b);
	/* pocd3~0  CA/DQ/40/40 */
	DDRPHY_DEV->OCDP0_SET0            =    DDRPHY_OCDP0_SET0_OCDP_SET_3_PRE(DDR_PHY_CA_OCDP[ca]) |
										   DDRPHY_OCDP0_SET0_OCDP_SET_2_PRE(DDR_PHY_DQ_OCDP[dq]) |
										   DDRPHY_OCDP0_SET0_OCDP_SET_1_PRE(0x26) |
										   DDRPHY_OCDP0_SET0_OCDP_SET_0_PRE(0x26);
	/* pocd7~4  open/34/34/34/34 */
	DDRPHY_DEV->OCDP1_SET0            =    DDRPHY_OCDP1_SET0_OCDP_SET_7_PRE(0x00) |
										   DDRPHY_OCDP1_SET0_OCDP_SET_6_PRE(0x1f) |
										   DDRPHY_OCDP1_SET0_OCDP_SET_5_PRE(0x3f) |
										   DDRPHY_OCDP1_SET0_OCDP_SET_4_PRE(0x3f);
	/* nocd3~0 */
	DDRPHY_DEV->OCDN0_SET0            =    DDRPHY_OCDN0_SET0_OCDN_SET_3_PRE(DDR_PHY_CA_OCDN[ca]) |
										   DDRPHY_OCDN0_SET0_OCDN_SET_2_PRE(DDR_PHY_DQ_OCDN[dq]) |
										   DDRPHY_OCDN0_SET0_OCDN_SET_1_PRE(0x25) |
										   DDRPHY_OCDN0_SET0_OCDN_SET_0_PRE(0x25);
	/* nocd7~4 */
	DDRPHY_DEV->OCDN1_SET0            =    DDRPHY_OCDN1_SET0_OCDN_SET_7_PRE(0x00) |
										   DDRPHY_OCDN1_SET0_OCDN_SET_6_PRE(0x1e) |
										   DDRPHY_OCDN1_SET0_OCDN_SET_5_PRE(0x3d) |
										   DDRPHY_OCDN1_SET0_OCDN_SET_4_PRE(0x3d);
}

/**
  * @brief  Program ZQ calibration SET0..SET3, SET6 and SET7 (after R480).
  */
static void DDR_PHY_ASIC_CalibrateZq(void)
{
	DDRPHY_DEV->PAD_CTRL_PROG        &=    ~DDRPHY_BIT_PAD_CTRL_PROG_DZQ_AUTO_UP;
	DDRPHY_DEV->PAD_CTRL_PROG        |=    DDRPHY_BIT_PAD_CTRL_PROG_DZQ_AUTO_UP;
	DDRPHY_DEV->DPI_CTRL_1            =    DDRPHY_BIT_DPI_CTRL_1_WRITE_EN_0 |
										   DDRPHY_BIT_DPI_CTRL_1_FW_SET_WR_DLY;
	DelayUs(100);
	// SET0
	DDRPHY_DEV->PAD_CTRL_PROG        &=    ~DDRPHY_MASK_PAD_CTRL_PROG_DZQ_UP_SEL;
	DDRPHY_DEV->PAD_CTRL_PROG        |=    DDRPHY_PAD_CTRL_PROG_DZQ_UP_SEL(0x0);
	DDRPHY_DEV->PAD_CTRL_ZPROG[0]     =    DDRPHY_PAD_CTRL_ZPROG_ZPROG_3(0x0) |
										   DDRPHY_PAD_CTRL_ZPROG_ZPROG_2(0x28) |
										   DDRPHY_PAD_CTRL_ZPROG_ZPROG_1(0x1) |
										   DDRPHY_PAD_CTRL_ZPROG_ZPROG_0(0x14);
	DDRPHY_DEV->PAD_NOCD2_ZPROG[0]    =    DDRPHY_PAD_NOCD2_ZPROG_NOCD2_ZPROG_1(0x1) |
										   DDRPHY_PAD_NOCD2_ZPROG_NOCD2_ZPROG_0(0x14);
	DDRPHY_DEV->PAD_CTRL_PROG        &=    ~DDRPHY_BIT_PAD_CTRL_PROG_ZCTRL_START;
	DDRPHY_DEV->PAD_CTRL_PROG        |=    DDRPHY_BIT_PAD_CTRL_PROG_ZCTRL_START;
	DelayUs(100);
	// SET1
	DDRPHY_DEV->PAD_CTRL_PROG        &=    ~DDRPHY_BIT_PAD_CTRL_PROG_ZCTRL_START;
	DDRPHY_DEV->PAD_CTRL_PROG        &=    ~DDRPHY_MASK_PAD_CTRL_PROG_DZQ_UP_SEL;
	DDRPHY_DEV->PAD_CTRL_PROG        |=    DDRPHY_PAD_CTRL_PROG_DZQ_UP_SEL(0x1);
	DDRPHY_DEV->PAD_CTRL_ZPROG[1]     =    DDRPHY_PAD_CTRL_ZPROG_ZPROG_3(0x0) |
										   DDRPHY_PAD_CTRL_ZPROG_ZPROG_2(0x20) |
										   DDRPHY_PAD_CTRL_ZPROG_ZPROG_1(0x1) |
										   DDRPHY_PAD_CTRL_ZPROG_ZPROG_0(0x14);
	DDRPHY_DEV->PAD_NOCD2_ZPROG[1]    =    DDRPHY_PAD_NOCD2_ZPROG_NOCD2_ZPROG_1(0x1) |
										   DDRPHY_PAD_NOCD2_ZPROG_NOCD2_ZPROG_0(0x14);
	DDRPHY_DEV->PAD_CTRL_PROG        &=    ~DDRPHY_BIT_PAD_CTRL_PROG_ZCTRL_START;
	DDRPHY_DEV->PAD_CTRL_PROG        |=    DDRPHY_BIT_PAD_CTRL_PROG_ZCTRL_START;
	DelayUs(100);
	// SET2
	DDRPHY_DEV->PAD_CTRL_PROG        &=    ~DDRPHY_BIT_PAD_CTRL_PROG_ZCTRL_START;
	DDRPHY_DEV->PAD_CTRL_PROG        &=    ~DDRPHY_MASK_PAD_CTRL_PROG_DZQ_UP_SEL;
	DDRPHY_DEV->PAD_CTRL_PROG        |=    DDRPHY_PAD_CTRL_PROG_DZQ_UP_SEL(0x2);
	DDRPHY_DEV->PAD_CTRL_ZPROG[2]     =    DDRPHY_PAD_CTRL_ZPROG_ZPROG_3(0x0) |
										   DDRPHY_PAD_CTRL_ZPROG_ZPROG_2(0x8) |
										   DDRPHY_PAD_CTRL_ZPROG_ZPROG_1(0x0) |
										   DDRPHY_PAD_CTRL_ZPROG_ZPROG_0(0x30);
	DDRPHY_DEV->PAD_NOCD2_ZPROG[2]    =    DDRPHY_PAD_NOCD2_ZPROG_NOCD2_ZPROG_1(0x0) |
										   DDRPHY_PAD_NOCD2_ZPROG_NOCD2_ZPROG_0(0x30);
	DDRPHY_DEV->PAD_CTRL_PROG        &=    ~DDRPHY_BIT_PAD_CTRL_PROG_ZCTRL_START;
	DDRPHY_DEV->PAD_CTRL_PROG        |=    DDRPHY_BIT_PAD_CTRL_PROG_ZCTRL_START;
	DelayUs(100);
	// SET3
	DDRPHY_DEV->PAD_CTRL_PROG        &=    ~DDRPHY_BIT_PAD_CTRL_PROG_ZCTRL_START;
	DDRPHY_DEV->PAD_CTRL_PROG        &=    ~DDRPHY_MASK_PAD_CTRL_PROG_DZQ_UP_SEL;
	DDRPHY_DEV->PAD_CTRL_PROG        |=    DDRPHY_PAD_CTRL_PROG_DZQ_UP_SEL(0x3);
	DDRPHY_DEV->PAD_CTRL_ZPROG[3]     =    DDRPHY_PAD_CTRL_ZPROG_ZPROG_3(0x0) |
										   DDRPHY_PAD_CTRL_ZPROG_ZPROG_2(0x3) |
										   DDRPHY_PAD_CTRL_ZPROG_ZPROG_1(0x0) |
										   DDRPHY_PAD_CTRL_ZPROG_ZPROG_0(0x28);
	DDRPHY_DEV->PAD_NOCD2_ZPROG[3]    =    DDRPHY_PAD_NOCD2_ZPROG_NOCD2_ZPROG_1(0x0) |
										   DDRPHY_PAD_NOCD2_ZPROG_NOCD2_ZPROG_0(0x28);
	DDRPHY_DEV->PAD_CTRL_PROG        &=    ~DDRPHY_BIT_PAD_CTRL_PROG_ZCTRL_START;
	DDRPHY_DEV->PAD_CTRL_PROG        |=    DDRPHY_BIT_PAD_CTRL_PROG_ZCTRL_START;
	DelayUs(100);
	// SET6
	DDRPHY_DEV->PAD_CTRL_PROG        &=    ~DDRPHY_BIT_PAD_CTRL_PROG_ZCTRL_START;
	DDRPHY_DEV->PAD_CTRL_PROG        &=    ~DDRPHY_MASK_PAD_CTRL_PROG_DZQ_UP_SEL;
	DDRPHY_DEV->PAD_CTRL_PROG        |=    DDRPHY_PAD_CTRL_PROG_DZQ_UP_SEL(0x6);
	DDRPHY_DEV->PAD_CTRL_ZPROG[6]     =    DDRPHY_PAD_CTRL_ZPROG_ZPROG_3(0x0) |
										   DDRPHY_PAD_CTRL_ZPROG_ZPROG_2(0x4) |
										   DDRPHY_PAD_CTRL_ZPROG_ZPROG_1(0x1) |
										   DDRPHY_PAD_CTRL_ZPROG_ZPROG_0(0x8);
	DDRPHY_DEV->PAD_NOCD2_ZPROG[6]    =    DDRPHY_PAD_NOCD2_ZPROG_NOCD2_ZPROG_1(0x1) |
										   DDRPHY_PAD_NOCD2_ZPROG_NOCD2_ZPROG_0(0x8);
	DDRPHY_DEV->PAD_CTRL_PROG        &=    ~DDRPHY_BIT_PAD_CTRL_PROG_ZCTRL_START;
	DDRPHY_DEV->PAD_CTRL_PROG        |=    DDRPHY_BIT_PAD_CTRL_PROG_ZCTRL_START;
	DelayUs(100);
	// SET7 open
	DDRPHY_DEV->PAD_CTRL_PROG        &=    ~DDRPHY_BIT_PAD_CTRL_PROG_ZCTRL_START;
	DDRPHY_DEV->ODT_TTCP1_SET0       &=    ~DDRPHY_MASK_ODT_TTCP1_SET0_ODT_TTCP_SET_7_PRE;
	DDRPHY_DEV->ODT_TTCP1_SET0       |=    DDRPHY_ODT_TTCP1_SET0_ODT_TTCP_SET_7_PRE(0x0);
	DDRPHY_DEV->ODT_TTCN1_SET0       &=    ~DDRPHY_MASK_ODT_TTCN1_SET0_ODT_TTCN_SET_7_PRE;
	DDRPHY_DEV->ODT_TTCN1_SET0       |=    DDRPHY_ODT_TTCN1_SET0_ODT_TTCN_SET_7_PRE(0x0);
	DDRPHY_DEV->OCDP1_SET0           &=    ~DDRPHY_MASK_OCDP1_SET0_OCDP_SET_7_PRE;
	DDRPHY_DEV->OCDP1_SET0           |=    DDRPHY_OCDP1_SET0_OCDP_SET_7_PRE(0x0);
	DDRPHY_DEV->OCDN1_SET0           &=    ~DDRPHY_MASK_OCDN1_SET0_OCDN_SET_7_PRE;
	DDRPHY_DEV->OCDN1_SET0           |=    DDRPHY_OCDN1_SET0_OCDN_SET_7_PRE(0x0);

	/* Write the calibrated ODT/OCD impedance codes into the SET0 registers. */
	DDR_PHY_ASIC_ProgramCalImpedance();

	//bit[18],auto update during refresh
	DDRPHY_DEV->PAD_CTRL_PROG        |=    DDRPHY_BIT_PAD_CTRL_PROG_DZQ_REF_UP;
	//bit[27],auto update calibration done
	DDRPHY_DEV->PAD_CTRL_PROG        |=    DDRPHY_BIT_PAD_CTRL_PROG_DZQ_AUTO_UP;
}

/**
  * @brief  Select per-signal OCD/ODT driver sets using the ZQ codes.
  */
static void DDR_PHY_ASIC_SelectDriveStrength(void)
{
	DDRPHY_DEV->DQ_ODT_SEL[0]       =    DDRPHY_DQ_ODT_SEL_DQ_1_TTCN_SEL(0x6) |
										 DDRPHY_DQ_ODT_SEL_DQ_0_TTCN_SEL(0x6) |
										 DDRPHY_DQ_ODT_SEL_DQ_1_TTCP_SEL(0x6) |
										 DDRPHY_DQ_ODT_SEL_DQ_0_TTCP_SEL(0x6) |
										 0xffff0000;

	DDRPHY_DEV->DQ_OCD_SEL[0]       =    DDRPHY_DQ_OCD_SEL_DQ_1_OCDN_SEL(DDR_PHY_DQ_OCD_SEL) |
										 DDRPHY_DQ_OCD_SEL_DQ_0_OCDN_SEL(DDR_PHY_DQ_OCD_SEL) |
										 DDRPHY_DQ_OCD_SEL_DQ_1_OCDP_SEL(DDR_PHY_DQ_OCD_SEL) |
										 DDRPHY_DQ_OCD_SEL_DQ_0_OCDP_SEL(DDR_PHY_DQ_OCD_SEL) |
										 0xffff0000;

	/* DQS ODT: PUPD3 ~ 250 mV */
	DDRPHY_DEV->DQS_P_ODT_SEL[0]    =    DDRPHY_DQS_P_ODT_SEL_DQS_P_1_TTCN_SEL(0x2) |
										 DDRPHY_DQS_P_ODT_SEL_DQS_P_0_TTCN_SEL(0x2) |
										 DDRPHY_DQS_P_ODT_SEL_DQS_P_1_TTCP_SEL(0x1) |
										 DDRPHY_DQS_P_ODT_SEL_DQS_P_0_TTCP_SEL(0x1) |
										 0xffff0000;
	DDRPHY_DEV->DQS_N_ODT_SEL[0]    =    DDRPHY_DQS_N_ODT_SEL_DQS_N_1_TTCN_SEL(0x7) |
										 DDRPHY_DQS_N_ODT_SEL_DQS_N_0_TTCN_SEL(0x7) |
										 DDRPHY_DQS_N_ODT_SEL_DQS_N_1_TTCP_SEL(0x0) |
										 DDRPHY_DQS_N_ODT_SEL_DQS_N_0_TTCP_SEL(0x0) |
										 0xffff0000;
	DDRPHY_DEV->DQS_OCD_SEL[0]      =    DDRPHY_DQS_OCD_SEL_DQS_N_1_OCDN_SEL(0x0) |
										 DDRPHY_DQS_OCD_SEL_DQS_N_0_OCDN_SEL(0x0) |
										 DDRPHY_DQS_OCD_SEL_DQS_N_1_OCDP_SEL(0x0) |
										 DDRPHY_DQS_OCD_SEL_DQS_N_0_OCDP_SEL(0x0) |
										 DDRPHY_DQS_OCD_SEL_DQS_P_1_OCDN_SEL(0x0) |
										 DDRPHY_DQS_OCD_SEL_DQS_P_0_OCDN_SEL(0x0) |
										 DDRPHY_DQS_OCD_SEL_DQS_P_1_OCDP_SEL(0x0) |
										 DDRPHY_DQS_OCD_SEL_DQS_P_0_OCDP_SEL(0x0);
	DDRPHY_DEV->CKE_OCD_SEL         =    DDRPHY_CKE_OCD_SEL_CKE_1_OCDN_SEL(0x3) |
										 DDRPHY_CKE_OCD_SEL_CKE_1_OCDP_SEL(0x3) |
										 DDRPHY_CKE_OCD_SEL_CKE_OCDN_SEL(0x3) |
										 DDRPHY_CKE_OCD_SEL_CKE_OCDP_SEL(0x3) |
										 0xffff0000;
	DDRPHY_DEV->ADR_OCD_SEL         =    DDRPHY_ADR_OCD_SEL_CS_1_OCDN_SEL(0x3) |
										 DDRPHY_ADR_OCD_SEL_CS_1_OCDP_SEL(0x3) |
										 DDRPHY_ADR_OCD_SEL_CS_OCDN_SEL(0x3) |
										 DDRPHY_ADR_OCD_SEL_CS_OCDP_SEL(0x3) |
										 0xff00ff;
	DDRPHY_DEV->CK_OCD_SEL          =    DDRPHY_CK_OCD_SEL_CK_N_1_OCDN_SEL(0x0) |
										 DDRPHY_CK_OCD_SEL_CK_N_0_OCDN_SEL(0x0) |
										 DDRPHY_CK_OCD_SEL_CK_N_1_OCDP_SEL(0x0) |
										 DDRPHY_CK_OCD_SEL_CK_N_0_OCDP_SEL(0x0) |
										 DDRPHY_CK_OCD_SEL_CK_P_1_OCDN_SEL(0x0) |
										 DDRPHY_CK_OCD_SEL_CK_P_0_OCDN_SEL(0x0) |
										 DDRPHY_CK_OCD_SEL_CK_P_1_OCDP_SEL(0x0) |
										 DDRPHY_CK_OCD_SEL_CK_P_0_OCDP_SEL(0x0);
	DDRPHY_DEV->OCD_SEL_0           =    DDRPHY_OCD_SEL_0_ADR00_OCDN_SEL(0x3) |
										 DDRPHY_OCD_SEL_0_ADR00_OCDP_SEL(0x3) |
										 DDRPHY_OCD_SEL_0_ADR01_OCDN_SEL(0x3) |
										 DDRPHY_OCD_SEL_0_ADR01_OCDP_SEL(0x3) |
										 DDRPHY_OCD_SEL_0_ADR02_OCDN_SEL(0x3) |
										 DDRPHY_OCD_SEL_0_ADR02_OCDP_SEL(0x3) |
										 DDRPHY_OCD_SEL_0_ADR03_OCDN_SEL(0x3) |
										 DDRPHY_OCD_SEL_0_ADR03_OCDP_SEL(0x3);
	DDRPHY_DEV->OCD_SEL_1           =    DDRPHY_OCD_SEL_1_ADR04_OCDN_SEL(0x3) |
										 DDRPHY_OCD_SEL_1_ADR04_OCDP_SEL(0x3) |
										 DDRPHY_OCD_SEL_1_ADR05_OCDN_SEL(0x3) |
										 DDRPHY_OCD_SEL_1_ADR05_OCDP_SEL(0x3) |
										 DDRPHY_OCD_SEL_1_ADR06_OCDN_SEL(0x3) |
										 DDRPHY_OCD_SEL_1_ADR06_OCDP_SEL(0x3) |
										 DDRPHY_OCD_SEL_1_ADR07_OCDN_SEL(0x3) |
										 DDRPHY_OCD_SEL_1_ADR07_OCDP_SEL(0x3);
	DDRPHY_DEV->OCD_SEL_2           =    DDRPHY_OCD_SEL_2_ADR08_OCDN_SEL(0x3) |
										 DDRPHY_OCD_SEL_2_ADR08_OCDP_SEL(0x3) |
										 DDRPHY_OCD_SEL_2_ADR09_OCDN_SEL(0x3) |
										 DDRPHY_OCD_SEL_2_ADR09_OCDP_SEL(0x3) |
										 DDRPHY_OCD_SEL_2_ADR10_OCDN_SEL(0x3) |
										 DDRPHY_OCD_SEL_2_ADR10_OCDP_SEL(0x3) |
										 DDRPHY_OCD_SEL_2_ADR11_OCDN_SEL(0x3) |
										 DDRPHY_OCD_SEL_2_ADR11_OCDP_SEL(0x3);
	DDRPHY_DEV->OCD_SEL_3           =    DDRPHY_OCD_SEL_3_ADR12_OCDN_SEL(0x3) |
										 DDRPHY_OCD_SEL_3_ADR12_OCDP_SEL(0x3) |
										 DDRPHY_OCD_SEL_3_ADR13_OCDN_SEL(0x3) |
										 DDRPHY_OCD_SEL_3_ADR13_OCDP_SEL(0x3) |
										 DDRPHY_OCD_SEL_3_ADR14_OCDN_SEL(0x3) |
										 DDRPHY_OCD_SEL_3_ADR14_OCDP_SEL(0x3) |
										 DDRPHY_OCD_SEL_3_ADR15_OCDN_SEL(0x3) |
										 DDRPHY_OCD_SEL_3_ADR15_OCDP_SEL(0x3);
	DDRPHY_DEV->OCD_SEL_4           =    DDRPHY_OCD_SEL_4_ADR16_OCDN_SEL(0x3) |
										 DDRPHY_OCD_SEL_4_ADR16_OCDP_SEL(0x3) |
										 DDRPHY_OCD_SEL_4_ADR_EXT_4_OCDN_SEL(0x3) |
										 DDRPHY_OCD_SEL_4_ADR_EXT_4_OCDP_SEL(0x3) |
										 DDRPHY_OCD_SEL_4_ADR_EXT_5_OCDN_SEL(0x3) |
										 DDRPHY_OCD_SEL_4_ADR_EXT_5_OCDP_SEL(0x3) |
										 DDRPHY_OCD_SEL_4_BA0_OCDN_SEL(0x3) |
										 DDRPHY_OCD_SEL_4_BA0_OCDP_SEL(0x3);
	DDRPHY_DEV->OCD_SEL_5           =    DDRPHY_OCD_SEL_5_BA1_OCDN_SEL(0x3) |
										 DDRPHY_OCD_SEL_5_BA1_OCDP_SEL(0x3) |
										 DDRPHY_OCD_SEL_5_BA2_OCDN_SEL(0x3) |
										 DDRPHY_OCD_SEL_5_BA2_OCDP_SEL(0x3) |
										 DDRPHY_OCD_SEL_5_RAS_OCDN_SEL(0x3) |
										 DDRPHY_OCD_SEL_5_RAS_OCDP_SEL(0x3) |
										 DDRPHY_OCD_SEL_5_CAS_OCDN_SEL(0x3) |
										 DDRPHY_OCD_SEL_5_CAS_OCDP_SEL(0x3);
	DDRPHY_DEV->OCD_SEL_6           =    DDRPHY_OCD_SEL_6_WE_OCDN_SEL(0x3) |
										 DDRPHY_OCD_SEL_6_WE_OCDP_SEL(0x3) |
										 DDRPHY_OCD_SEL_6_ODT_OCDN_SEL(0x3) |
										 DDRPHY_OCD_SEL_6_ODT_OCDP_SEL(0x3) |
										 DDRPHY_OCD_SEL_6_RST_OCDN_SEL(0x3) |
										 DDRPHY_OCD_SEL_6_RST_OCDP_SEL(0x3) |
										 DDRPHY_OCD_SEL_6_PAR_OCDN_SEL(0x3) |
										 DDRPHY_OCD_SEL_6_PAR_OCDP_SEL(0x3);
	DDRPHY_DEV->OCD_SEL_7           =    DDRPHY_OCD_SEL_7_PAR1_OCDN_SEL(0x3) |
										 DDRPHY_OCD_SEL_7_PAR1_OCDP_SEL(0x3) |
										 DDRPHY_OCD_SEL_7_ACT_OCDN_SEL(0x3) |
										 DDRPHY_OCD_SEL_7_ACT_OCDP_SEL(0x3) |
										 DDRPHY_OCD_SEL_7_DDR4_BG0_OCDN_SEL(0x3) |
										 DDRPHY_OCD_SEL_7_DDR4_BG0_OCDP_SEL(0x3) |
										 DDRPHY_OCD_SEL_7_TEN_OCDN_SEL(0x3) |
										 DDRPHY_OCD_SEL_7_TEN_OCDP_SEL(0x3);
	DDRPHY_DEV->OCD_SEL_8           =    DDRPHY_OCD_SEL_8_ALERT_OCDN_SEL(0x3) |
										 DDRPHY_OCD_SEL_8_ALERT_OCDP_SEL(0x3) |
										 DDRPHY_OCD_SEL_8_ALERT1_OCDN_SEL(0x3) |
										 DDRPHY_OCD_SEL_8_ALERT1_OCDP_SEL(0x3) |
										 0x33ff;

	DDRPHY_DEV->ODT_SEL_0           =    DDRPHY_ODT_SEL_0_CA_TTCP_SEL(0x7) | DDRPHY_ODT_SEL_0_CA_TTCN_SEL(0x7);

	DDRPHY_DEV->CKE_ODT_SEL         =    DDRPHY_CKE_ODT_SEL_CKE_TTCP_SEL(0x7) |
										 DDRPHY_CKE_ODT_SEL_CKE_TTCN_SEL(0x7) |
										 DDRPHY_CKE_ODT_SEL_CKE_1_TTCP_SEL(0x7) |
										 DDRPHY_CKE_ODT_SEL_CKE_1_TTCN_SEL(0x7);

	DDRPHY_DEV->ADR_ODT_SEL         =    DDRPHY_ADR_ODT_SEL_RST_TTCP_SEL(0x7) |
										 DDRPHY_ADR_ODT_SEL_RST_TTCN_SEL(0x7) |
										 DDRPHY_ADR_ODT_SEL_CS_TTCP_SEL(0x7) |
										 DDRPHY_ADR_ODT_SEL_CS_TTCN_SEL(0x7) |
										 DDRPHY_ADR_ODT_SEL_CS_1_TTCP_SEL(0x7) |
										 DDRPHY_ADR_ODT_SEL_CS_1_TTCN_SEL(0x7);

	DDRPHY_DEV->CLK_ODT_SEL         =    DDRPHY_CLK_ODT_SEL_CK_TTCP_SEL(0x7) |
										 DDRPHY_CLK_ODT_SEL_CK_TTCN_SEL(0x7) |
										 DDRPHY_CLK_ODT_SEL_CK_1_TTCP_SEL(0x7) |
										 DDRPHY_CLK_ODT_SEL_CK_1_TTCN_SEL(0x7);
}

/**
  * @brief  Boot up pads, configure ODT or fixed drive codes.
  * @param  odt_calibration: true = calibrated ODT, false = fixed drive codes.
  */
static void DDR_PHY_ASIC_SetupPad(bool odt_calibration)
{
	DDR_PHY_ASIC_SetupPwdPad();
	DDR_PHY_ASIC_EnablePadOE();
	if (!odt_calibration) {
		DDR_PHY_ASIC_DisableODT();
	} else {
		DDR_PHY_ASIC_CalibrateR480();
		DDR_PHY_ASIC_CalibrateZq();
		DDR_PHY_ASIC_SelectDriveStrength();
	}
}

/**
  * @brief  Program DQ/DQS/DM write delay taps (golden: #=====write delay tap=====).
  */
static void DDR_PHY_ASIC_SetupWriteDelayTap(void)
{
	DDRPHY_DEV->DQ_DLY_0[0]    =    DDRPHY_DQ_DLY_0_DQ7_DLY_SEL(0x8) |
									DDRPHY_DQ_DLY_0_DQ6_DLY_SEL(0x8) |
									DDRPHY_DQ_DLY_0_DQ5_DLY_SEL(0x8) |
									DDRPHY_DQ_DLY_0_DQ4_DLY_SEL(0x8) |
									DDRPHY_DQ_DLY_0_DQ3_DLY_SEL(0x9) |
									DDRPHY_DQ_DLY_0_DQ2_DLY_SEL(0x9) |
									DDRPHY_DQ_DLY_0_DQ1_DLY_SEL(0x8) |
									DDRPHY_DQ_DLY_0_DQ0_DLY_SEL(0x4);
	DDRPHY_DEV->DQ_DLY_0[1]    =    DDRPHY_DQ_DLY_0_DQ7_DLY_SEL(0x8) |
									DDRPHY_DQ_DLY_0_DQ6_DLY_SEL(0x9) |
									DDRPHY_DQ_DLY_0_DQ5_DLY_SEL(0x9) |
									DDRPHY_DQ_DLY_0_DQ4_DLY_SEL(0xa) |
									DDRPHY_DQ_DLY_0_DQ3_DLY_SEL(0x9) |
									DDRPHY_DQ_DLY_0_DQ2_DLY_SEL(0x6) |
									DDRPHY_DQ_DLY_0_DQ1_DLY_SEL(0x9) |
									DDRPHY_DQ_DLY_0_DQ0_DLY_SEL(0x5);
	DDRPHY_DEV->DQ_DLY_1[0]    =    DDRPHY_DQ_DLY_1_DQS_OE_DLY_SEL(0x8) |
									DDRPHY_DQ_DLY_1_DM_DLY_SEL(0x8) |
									DDRPHY_DQ_DLY_1_DQS_DLY_SEL(0x8);
	DDRPHY_DEV->DQ_DLY_1[1]    =    DDRPHY_DQ_DLY_1_DQS_OE_DLY_SEL(0x8) |
									DDRPHY_DQ_DLY_1_DM_DLY_SEL(0x8) |
									DDRPHY_DQ_DLY_1_DQS_DLY_SEL(0x8);
	DDRPHY_DEV->DPI_CTRL_1     =    DDRPHY_BIT_DPI_CTRL_1_WRITE_EN_0 |
									DDRPHY_BIT_DPI_CTRL_1_FW_SET_WR_DLY;
}

/**
  * @brief  Enable DFI alert / alert_1 output (golden: # enable DFI alert, alert_1 output).
  */
static void DDR_PHY_ASIC_EnableDFIAlert(void)
{
	DDRPHY_DEV->DPI_CTRL_2     =    DDRPHY_DPI_CTRL_2_RST_3POINT_MODE(0x0) |
									DDRPHY_DPI_CTRL_2_RST_DQSEN_MODE(0x0) |
									DDRPHY_DPI_CTRL_2_RD_UPDATE_DLY(0x1a) |
									DDRPHY_DPI_CTRL_2_RD_UPDATE_DLY_RX(0x30) |
									DDRPHY_DPI_CTRL_2_PWROFF_CMD_GATE(0x0) |
									DDRPHY_BIT_DPI_CTRL_2_RST_DQSEN_CAL |
									DDRPHY_BIT_DPI_CTRL_2_SET_REG_VALID_MODE |
									DDRPHY_BIT_DPI_CTRL_2_ALERT_ENA |
									DDRPHY_BIT_DPI_CTRL_2_ALERT_1_ENA;
	DDRPHY_DEV->CRT_CTL        =    DDRPHY_CRT_CTL_FIFO_RST_INI_TX(0x0) |
									DDRPHY_CRT_CTL_FIFO_RST_INI_RX(0x0) |
									DDRPHY_BIT_CRT_CTL_MCK_CKE_SEL |
									DDRPHY_BIT_CRT_CTL_RSTN_VALUE |
									DDRPHY_BIT_CRT_CTL_CA_DDR3_MODE |
									DDRPHY_BIT_CRT_CTL_CS_VALUE |
									DDRPHY_BIT_CRT_CTL_CS_1_VALUE |
									DDRPHY_BIT_CRT_CTL_PIN_MUX_DDR3 |
									DDRPHY_BIT_CRT_CTL_DPI_DDR3_MODE;
}

/**
  * @brief  Program CMD/ADR phase selectors (golden: w 0x60330154, 0x2c002011).
  */
static void DDR_PHY_ASIC_SetupCmdAdrPhase(void)
{
	DDRPHY_DEV->CMD_ADR_PH     =    DDRPHY_CMD_ADR_PH_CKE_1_PH_SEL(0x0) |
									DDRPHY_CMD_ADR_PH_CKE_PH_SEL(0x1) |
									DDRPHY_CMD_ADR_PH_CS_1_PH_SEL(0x0) |
									DDRPHY_CMD_ADR_PH_CS_PH_SEL(0x1) |
									DDRPHY_CMD_ADR_PH_CMD_PH_SEL(0x1) |
									DDRPHY_BIT_CMD_ADR_PH_CKE_PH_EN |
									DDRPHY_BIT_CMD_ADR_PH_CS_PH_EN |
									DDRPHY_BIT_CMD_ADR_PH_CMD_PH_EN;
}

/**
  * @brief  Program READ_CTRL_0/1 read-latency config.
  */
static void DDR_PHY_ASIC_SetupReadLatency(void)
{
	DDRPHY_DEV->READ_CTRL_0[0]    =    DDRPHY_READ_CTRL_0_DQS_RC1IN_DLY_SEL(0x0) |
									   DDRPHY_READ_CTRL_0_DQS_GLITCH_DLY_PH(0x0) |
									   DDRPHY_READ_CTRL_0_TM_DQ_LAT_DLY(0x0) |
									   DDRPHY_READ_CTRL_0_DQS_FIFO_DLY_SEL(0x0) |
									   DDRPHY_READ_CTRL_0_DQS_EN_DLY_SEL(0x0) |
									   DDRPHY_READ_CTRL_0_TM_DQS_EN(0x0) |
									   DDRPHY_READ_CTRL_0_TM_DQS_EN_FTUN(0x2);
	DDRPHY_DEV->READ_CTRL_0[1]    =    DDRPHY_READ_CTRL_0_DQS_RC1IN_DLY_SEL(0x0) |
									   DDRPHY_READ_CTRL_0_DQS_GLITCH_DLY_PH(0x0) |
									   DDRPHY_READ_CTRL_0_TM_DQ_LAT_DLY(0x0) |
									   DDRPHY_READ_CTRL_0_DQS_FIFO_DLY_SEL(0x0) |
									   DDRPHY_READ_CTRL_0_DQS_EN_DLY_SEL(0x0) |
									   DDRPHY_READ_CTRL_0_TM_DQS_EN(0x0) |
									   DDRPHY_READ_CTRL_0_TM_DQS_EN_FTUN(0x2);
	DDRPHY_DEV->READ_CTRL_1       =    DDRPHY_READ_CTRL_1_RDDATA_EN_DLY_SEL(0x0) |
									   DDRPHY_READ_CTRL_1_TM_RD_FIFO(0x4);
}

/**
  * @brief  Fix DQS TX timing (tDQSS): DQS_DLY_SEL=1, clear DM_IE_FRC_EN.
  */
static void DDR_PHY_ASIC_FixDqssTxTiming(void)
{
	DDRPHY_DEV->DQ_DLY_1[0]      &=    ~DDRPHY_MASK_DQ_DLY_1_DQS_DLY_SEL;
	DDRPHY_DEV->DQ_DLY_1[0]      |=    DDRPHY_DQ_DLY_1_DQS_DLY_SEL(0x1);
	DDRPHY_DEV->DQ_DLY_1[1]      &=    ~DDRPHY_MASK_DQ_DLY_1_DQS_DLY_SEL;
	DDRPHY_DEV->DQ_DLY_1[1]      |=    DDRPHY_DQ_DLY_1_DQS_DLY_SEL(0x1);
	DDRPHY_DEV->DQ_IO_CTL_1[0]   &=    ~DDRPHY_BIT_DQ_IO_CTL_1_DM_IE_FRC_EN;
	DDRPHY_DEV->DQ_IO_CTL_1[1]   &=    ~DDRPHY_BIT_DQ_IO_CTL_1_DM_IE_FRC_EN;
}

/**
  * @brief  Latch RD/WR delay chains via the DPI_CTRL pulse sequence.
  */
static void DDR_PHY_ASIC_UpdateDelayChains(void)
{
	DDRPHY_DEV->DPI_CTRL_1   &=    ~(DDRPHY_BIT_DPI_CTRL_1_WRITE_EN_4 |
									 DDRPHY_BIT_DPI_CTRL_1_SET_REG_VALID);
	DDRPHY_DEV->DPI_CTRL_1   |=    DDRPHY_BIT_DPI_CTRL_1_WRITE_EN_4 |
								   DDRPHY_BIT_DPI_CTRL_1_SET_REG_VALID;
	DDRPHY_DEV->DPI_CTRL_0   &=    ~DDRPHY_MASK_DPI_CTRL_0_FW_SET_MODE;
	DDRPHY_DEV->DPI_CTRL_0   |=    DDRPHY_DPI_CTRL_0_FW_SET_MODE(0x2);
	DDRPHY_DEV->DPI_CTRL_1   &=    ~(DDRPHY_BIT_DPI_CTRL_1_WRITE_EN_1 |
									 DDRPHY_BIT_DPI_CTRL_1_FW_SET_RD_DLY |
									 DDRPHY_BIT_DPI_CTRL_1_WRITE_EN_0 |
									 DDRPHY_BIT_DPI_CTRL_1_FW_SET_WR_DLY);
	DDRPHY_DEV->DPI_CTRL_1   |=    DDRPHY_BIT_DPI_CTRL_1_WRITE_EN_1 |
								   DDRPHY_BIT_DPI_CTRL_1_FW_SET_RD_DLY |
								   DDRPHY_BIT_DPI_CTRL_1_WRITE_EN_0 |
								   DDRPHY_BIT_DPI_CTRL_1_FW_SET_WR_DLY;
	DDRPHY_DEV->DPI_CTRL_0   &=    ~DDRPHY_MASK_DPI_CTRL_0_FW_SET_MODE;
	DDRPHY_DEV->DPI_CTRL_0   |=    DDRPHY_DPI_CTRL_0_FW_SET_MODE(0x0);
}

/**
  * @brief  Apply RXI316 PLL power-off fixup (PLL_CTL5 trim, CTL1 clear).
  */
static void DDR_PHY_ASIC_ApplyPLLRegressionFix(void)
{
	DDRPHY_DEV->PLL_CTL5   &=    ~(DDRPHY_MASK_PLL_CTL5_DPI_PWROFF_PWR_WAIT_CYCLE |
								   DDRPHY_MASK_PLL_CTL5_DPI_RW_PWROFF_MODE |
								   DDRPHY_MASK_PLL_CTL5_DPI_CA_PWROFF_MODE);
	DDRPHY_DEV->PLL_CTL5   |=    DDRPHY_PLL_CTL5_DPI_PWROFF_PWR_WAIT_CYCLE(0x1) |
								 DDRPHY_PLL_CTL5_DPI_RW_PWROFF_MODE(0x2) |
								 DDRPHY_PLL_CTL5_DPI_CA_PWROFF_MODE(0x2);
	DDRPHY_DEV->PLL_CTL1   &=    ~DDRPHY_BIT_PLL_CTL1_DPI_REG_CRT_PWROFF_DIS;
}

/**
  * @brief  Apply 3 Point K deltaforce - JohnnyHsu asked to add on 10/07/2025
  */
static void DDR_PHY_ASIC_Disable3PKDeltaForce(void)
{
	DDRPHY_DEV->RX_CAL_CTRL_0 |= DDRPHY_BIT_RX_CAL_CTRL_0_DELTA_FORCE;
}


/**
  * @brief  Seed DQS_IN_DLY_0..3 and DBI lanes to 0x10 (RX TE start offset).
  */
static void DDR_PHY_ASIC_SetupRxTeDelay(void)
{
	DDRPHY_DEV->DQS_IN_DLY_0[0]        =    DDRPHY_DQS_IN_DLY_0_FW_RD_DLY_POS_SEL_3(0x10) |
											DDRPHY_DQS_IN_DLY_0_FW_RD_DLY_POS_SEL_2(0x10) |
											DDRPHY_DQS_IN_DLY_0_FW_RD_DLY_POS_SEL_1(0x10) |
											DDRPHY_DQS_IN_DLY_0_FW_RD_DLY_POS_SEL_0(0x10);
	DDRPHY_DEV->DQS_IN_DLY_1[0]        =    DDRPHY_DQS_IN_DLY_1_FW_RD_DLY_POS_SEL_7(0x10) |
											DDRPHY_DQS_IN_DLY_1_FW_RD_DLY_POS_SEL_6(0x10) |
											DDRPHY_DQS_IN_DLY_1_FW_RD_DLY_POS_SEL_5(0x10) |
											DDRPHY_DQS_IN_DLY_1_FW_RD_DLY_POS_SEL_4(0x10);
	DDRPHY_DEV->DQS_IN_DLY_1_DBI[0]    =    DDRPHY_DQS_IN_DLY_1_DBI_FW_RD_DLY_POS_SEL_8(0x10);
	DDRPHY_DEV->DQS_IN_DLY_0[1]        =    DDRPHY_DQS_IN_DLY_0_FW_RD_DLY_POS_SEL_3(0x10) |
											DDRPHY_DQS_IN_DLY_0_FW_RD_DLY_POS_SEL_2(0x10) |
											DDRPHY_DQS_IN_DLY_0_FW_RD_DLY_POS_SEL_1(0x10) |
											DDRPHY_DQS_IN_DLY_0_FW_RD_DLY_POS_SEL_0(0x10);
	DDRPHY_DEV->DQS_IN_DLY_1[1]        =    DDRPHY_DQS_IN_DLY_1_FW_RD_DLY_POS_SEL_7(0x10) |
											DDRPHY_DQS_IN_DLY_1_FW_RD_DLY_POS_SEL_6(0x10) |
											DDRPHY_DQS_IN_DLY_1_FW_RD_DLY_POS_SEL_5(0x10) |
											DDRPHY_DQS_IN_DLY_1_FW_RD_DLY_POS_SEL_4(0x10);
	DDRPHY_DEV->DQS_IN_DLY_1_DBI[1]    =    DDRPHY_DQS_IN_DLY_1_DBI_FW_RD_DLY_POS_SEL_8(0x10);
	DDRPHY_DEV->DQS_IN_DLY_2[0]        =    DDRPHY_DQS_IN_DLY_2_FW_RD_DLY_NEG_SEL_3(0x10) |
											DDRPHY_DQS_IN_DLY_2_FW_RD_DLY_NEG_SEL_2(0x10) |
											DDRPHY_DQS_IN_DLY_2_FW_RD_DLY_NEG_SEL_1(0x10) |
											DDRPHY_DQS_IN_DLY_2_FW_RD_DLY_NEG_SEL_0(0x10);
	DDRPHY_DEV->DQS_IN_DLY_3[0]        =    DDRPHY_DQS_IN_DLY_3_FW_RD_DLY_NEG_SEL_7(0x10) |
											DDRPHY_DQS_IN_DLY_3_FW_RD_DLY_NEG_SEL_6(0x10) |
											DDRPHY_DQS_IN_DLY_3_FW_RD_DLY_NEG_SEL_5(0x10) |
											DDRPHY_DQS_IN_DLY_3_FW_RD_DLY_NEG_SEL_4(0x10);
	DDRPHY_DEV->DQS_IN_DLY_3_DBI[0]    =    DDRPHY_DQS_IN_DLY_3_DBI_FW_RD_DLY_NEG_SEL_8(0x10);
	DDRPHY_DEV->DQS_IN_DLY_2[1]        =    DDRPHY_DQS_IN_DLY_2_FW_RD_DLY_NEG_SEL_3(0x10) |
											DDRPHY_DQS_IN_DLY_2_FW_RD_DLY_NEG_SEL_2(0x10) |
											DDRPHY_DQS_IN_DLY_2_FW_RD_DLY_NEG_SEL_1(0x10) |
											DDRPHY_DQS_IN_DLY_2_FW_RD_DLY_NEG_SEL_0(0x10);
	DDRPHY_DEV->DQS_IN_DLY_3[1]        =    DDRPHY_DQS_IN_DLY_3_FW_RD_DLY_NEG_SEL_7(0x10) |
											DDRPHY_DQS_IN_DLY_3_FW_RD_DLY_NEG_SEL_6(0x10) |
											DDRPHY_DQS_IN_DLY_3_FW_RD_DLY_NEG_SEL_5(0x10) |
											DDRPHY_DQS_IN_DLY_3_FW_RD_DLY_NEG_SEL_4(0x10);
	DDRPHY_DEV->DQS_IN_DLY_3_DBI[1]    =    DDRPHY_DQS_IN_DLY_3_DBI_FW_RD_DLY_NEG_SEL_8(0x10);
}

/**
  * @brief  Seed left/right POS/NEG delta selectors to 0x3.
  */
static void DDR_PHY_ASIC_SetupDeltaDelay(void)
{
	DDRPHY_DEV->DEL_POS_SEL[0]             =    DDRPHY_DEL_POS_SEL_DELTA_LEFT_POS_SEL_7(0x3) |
			DDRPHY_DEL_POS_SEL_DELTA_LEFT_POS_SEL_6(0x3) |
			DDRPHY_DEL_POS_SEL_DELTA_LEFT_POS_SEL_5(0x3) |
			DDRPHY_DEL_POS_SEL_DELTA_LEFT_POS_SEL_4(0x3) |
			DDRPHY_DEL_POS_SEL_DELTA_LEFT_POS_SEL_3(0x3) |
			DDRPHY_DEL_POS_SEL_DELTA_LEFT_POS_SEL_2(0x3) |
			DDRPHY_DEL_POS_SEL_DELTA_LEFT_POS_SEL_1(0x3) |
			DDRPHY_DEL_POS_SEL_DELTA_LEFT_POS_SEL_0(0x3);
	DDRPHY_DEV->DEL_POS_SEL_DBI[0]         =    DDRPHY_DEL_POS_SEL_DBI_DELTA_LEFT_POS_SEL_8(0x3);
	DDRPHY_DEV->DEL_POS_SEL[1]             =    DDRPHY_DEL_POS_SEL_DELTA_LEFT_POS_SEL_7(0x3) |
			DDRPHY_DEL_POS_SEL_DELTA_LEFT_POS_SEL_6(0x3) |
			DDRPHY_DEL_POS_SEL_DELTA_LEFT_POS_SEL_5(0x3) |
			DDRPHY_DEL_POS_SEL_DELTA_LEFT_POS_SEL_4(0x3) |
			DDRPHY_DEL_POS_SEL_DELTA_LEFT_POS_SEL_3(0x3) |
			DDRPHY_DEL_POS_SEL_DELTA_LEFT_POS_SEL_2(0x3) |
			DDRPHY_DEL_POS_SEL_DELTA_LEFT_POS_SEL_1(0x3) |
			DDRPHY_DEL_POS_SEL_DELTA_LEFT_POS_SEL_0(0x3);
	DDRPHY_DEV->DEL_POS_SEL_DBI[1]         =    DDRPHY_DEL_POS_SEL_DBI_DELTA_LEFT_POS_SEL_8(0x3);
	DDRPHY_DEV->DEL_POS_SEL_RIGH[0]        =    DDRPHY_DEL_POS_SEL_RIGH_DELTA_RIGH_POS_SEL_7(0x3) |
			DDRPHY_DEL_POS_SEL_RIGH_DELTA_RIGH_POS_SEL_6(0x3) |
			DDRPHY_DEL_POS_SEL_RIGH_DELTA_RIGH_POS_SEL_5(0x3) |
			DDRPHY_DEL_POS_SEL_RIGH_DELTA_RIGH_POS_SEL_4(0x3) |
			DDRPHY_DEL_POS_SEL_RIGH_DELTA_RIGH_POS_SEL_3(0x3) |
			DDRPHY_DEL_POS_SEL_RIGH_DELTA_RIGH_POS_SEL_2(0x3) |
			DDRPHY_DEL_POS_SEL_RIGH_DELTA_RIGH_POS_SEL_1(0x3) |
			DDRPHY_DEL_POS_SEL_RIGH_DELTA_RIGH_POS_SEL_0(0x3);
	DDRPHY_DEV->DEL_POS_SEL_RIGH_DBI[0]    =    DDRPHY_DEL_POS_SEL_RIGH_DBI_DELTA_RIGH_POS_SEL_8(0x3);
	DDRPHY_DEV->DEL_POS_SEL_RIGH[1]        =    DDRPHY_DEL_POS_SEL_RIGH_DELTA_RIGH_POS_SEL_7(0x3) |
			DDRPHY_DEL_POS_SEL_RIGH_DELTA_RIGH_POS_SEL_6(0x3) |
			DDRPHY_DEL_POS_SEL_RIGH_DELTA_RIGH_POS_SEL_5(0x3) |
			DDRPHY_DEL_POS_SEL_RIGH_DELTA_RIGH_POS_SEL_4(0x3) |
			DDRPHY_DEL_POS_SEL_RIGH_DELTA_RIGH_POS_SEL_3(0x3) |
			DDRPHY_DEL_POS_SEL_RIGH_DELTA_RIGH_POS_SEL_2(0x3) |
			DDRPHY_DEL_POS_SEL_RIGH_DELTA_RIGH_POS_SEL_1(0x3) |
			DDRPHY_DEL_POS_SEL_RIGH_DELTA_RIGH_POS_SEL_0(0x3);
	DDRPHY_DEV->DEL_POS_SEL_RIGH_DBI[1]    =    DDRPHY_DEL_POS_SEL_RIGH_DBI_DELTA_RIGH_POS_SEL_8(0x3);
	DDRPHY_DEV->DEL_NEG_SEL[0]             =    DDRPHY_DEL_NEG_SEL_DELTA_LEFT_NEG_SEL_7(0x3) |
			DDRPHY_DEL_NEG_SEL_DELTA_LEFT_NEG_SEL_6(0x3) |
			DDRPHY_DEL_NEG_SEL_DELTA_LEFT_NEG_SEL_5(0x3) |
			DDRPHY_DEL_NEG_SEL_DELTA_LEFT_NEG_SEL_4(0x3) |
			DDRPHY_DEL_NEG_SEL_DELTA_LEFT_NEG_SEL_3(0x3) |
			DDRPHY_DEL_NEG_SEL_DELTA_LEFT_NEG_SEL_2(0x3) |
			DDRPHY_DEL_NEG_SEL_DELTA_LEFT_NEG_SEL_1(0x3) |
			DDRPHY_DEL_NEG_SEL_DELTA_LEFT_NEG_SEL_0(0x3);
	DDRPHY_DEV->DEL_NEG_SEL_DBI[0]         =    DDRPHY_DEL_NEG_SEL_DBI_DELTA_LEFT_NEG_SEL_8(0x3);
	DDRPHY_DEV->DEL_NEG_SEL[1]             =    DDRPHY_DEL_NEG_SEL_DELTA_LEFT_NEG_SEL_7(0x3) |
			DDRPHY_DEL_NEG_SEL_DELTA_LEFT_NEG_SEL_6(0x3) |
			DDRPHY_DEL_NEG_SEL_DELTA_LEFT_NEG_SEL_5(0x3) |
			DDRPHY_DEL_NEG_SEL_DELTA_LEFT_NEG_SEL_4(0x3) |
			DDRPHY_DEL_NEG_SEL_DELTA_LEFT_NEG_SEL_3(0x3) |
			DDRPHY_DEL_NEG_SEL_DELTA_LEFT_NEG_SEL_2(0x3) |
			DDRPHY_DEL_NEG_SEL_DELTA_LEFT_NEG_SEL_1(0x3) |
			DDRPHY_DEL_NEG_SEL_DELTA_LEFT_NEG_SEL_0(0x3);
	DDRPHY_DEV->DEL_NEG_SEL_DBI[1]         =    DDRPHY_DEL_NEG_SEL_DBI_DELTA_LEFT_NEG_SEL_8(0x3);
	DDRPHY_DEV->DEL_NEG_SEL_RIGH[0]        =    DDRPHY_DEL_NEG_SEL_RIGH_DELTA_RIGH_NEG_SEL_7(0x3) |
			DDRPHY_DEL_NEG_SEL_RIGH_DELTA_RIGH_NEG_SEL_6(0x3) |
			DDRPHY_DEL_NEG_SEL_RIGH_DELTA_RIGH_NEG_SEL_5(0x3) |
			DDRPHY_DEL_NEG_SEL_RIGH_DELTA_RIGH_NEG_SEL_4(0x3) |
			DDRPHY_DEL_NEG_SEL_RIGH_DELTA_RIGH_NEG_SEL_3(0x3) |
			DDRPHY_DEL_NEG_SEL_RIGH_DELTA_RIGH_NEG_SEL_2(0x3) |
			DDRPHY_DEL_NEG_SEL_RIGH_DELTA_RIGH_NEG_SEL_1(0x3) |
			DDRPHY_DEL_NEG_SEL_RIGH_DELTA_RIGH_NEG_SEL_0(0x3);
	DDRPHY_DEV->DEL_NEG_SEL_RIGH_DBI[0]    =    DDRPHY_DEL_NEG_SEL_RIGH_DBI_DELTA_RIGH_NEG_SEL_8(0x3);
	DDRPHY_DEV->DEL_NEG_SEL_RIGH[1]        =    DDRPHY_DEL_NEG_SEL_RIGH_DELTA_RIGH_NEG_SEL_7(0x3) |
			DDRPHY_DEL_NEG_SEL_RIGH_DELTA_RIGH_NEG_SEL_6(0x3) |
			DDRPHY_DEL_NEG_SEL_RIGH_DELTA_RIGH_NEG_SEL_5(0x3) |
			DDRPHY_DEL_NEG_SEL_RIGH_DELTA_RIGH_NEG_SEL_4(0x3) |
			DDRPHY_DEL_NEG_SEL_RIGH_DELTA_RIGH_NEG_SEL_3(0x3) |
			DDRPHY_DEL_NEG_SEL_RIGH_DELTA_RIGH_NEG_SEL_2(0x3) |
			DDRPHY_DEL_NEG_SEL_RIGH_DELTA_RIGH_NEG_SEL_1(0x3) |
			DDRPHY_DEL_NEG_SEL_RIGH_DELTA_RIGH_NEG_SEL_0(0x3);
	DDRPHY_DEV->DEL_NEG_SEL_RIGH_DBI[1]    =    DDRPHY_DEL_NEG_SEL_RIGH_DBI_DELTA_RIGH_NEG_SEL_8(0x3);
}

/**
  * @brief  Disable RX calibration when 3-point-K is off (RX_CAL_DIS + latch).
  */
static void DDR_PHY_ASIC_DisableRxCal(void)
{
#if !defined(CONFIG_DDR_PHY_THREE_POINT_K_ENABLE) || (CONFIG_DDR_PHY_THREE_POINT_K_ENABLE == 0)
	//Force reset of the 3-point
	DDRPHY_DEV->DPI_CTRL_2      |=  DDRPHY_DPI_CTRL_2_RST_3POINT_MODE(0x1);
	DDRPHY_DEV->READ_CTRL_1     |=  DDRPHY_BIT_READ_CTRL_1_RX_CAL_DIS;
	DDRPHY_DEV->DPI_CTRL_0      &=  ~(DDRPHY_MASK_DPI_CTRL_0_CAL_SET_MODE |
									  DDRPHY_MASK_DPI_CTRL_0_RST_FIFO_MODE |
									  DDRPHY_MASK_DPI_CTRL_0_FW_SET_MODE |
									  DDRPHY_MASK_DPI_CTRL_0_RSVD_6_7);
	DDRPHY_DEV->DPI_CTRL_0      |=  DDRPHY_DPI_CTRL_0_CAL_SET_MODE(0x3) |
									DDRPHY_DPI_CTRL_0_RST_FIFO_MODE(0x1) |
									DDRPHY_DPI_CTRL_0_FW_SET_MODE(0x2);

	/* Force a read-FIFO pointer reset (mode 0x3), then drop back to the normal
	 * during-refresh mode (0x1). Mode 0x1 alone only resets the pointers on a
	 * refresh boundary, so on a re-init the pointers can stay where the previous
	 * session left them and reads come back byte-shifted. Same 0x3 -> 0x1 pulse
	 * ameba_ddrphy_scan.c uses when it retunes a live PHY. */
	DDRPHY_DEV->DPI_CTRL_0      = (DDRPHY_DEV->DPI_CTRL_0 & ~DDRPHY_MASK_DPI_CTRL_0_RST_FIFO_MODE) |
								  DDRPHY_DPI_CTRL_0_RST_FIFO_MODE(0x3);
	DDRPHY_DEV->DPI_CTRL_0      = (DDRPHY_DEV->DPI_CTRL_0 & ~DDRPHY_MASK_DPI_CTRL_0_RST_FIFO_MODE) |
								  DDRPHY_DPI_CTRL_0_RST_FIFO_MODE(0x1);

	DDRPHY_DEV->DPI_CTRL_1      =   DDRPHY_BIT_DPI_CTRL_1_WRITE_EN_1 |
									DDRPHY_BIT_DPI_CTRL_1_FW_SET_RD_DLY;
#endif
}

/**
  * @brief  Program read-side timing: latency, TX fix, delays, calibration.
  */
static void DDR_PHY_ASIC_SetupRead(void)
{
	DDR_PHY_ASIC_SetupReadLatency();
	DDR_PHY_ASIC_FixDqssTxTiming();
	DDR_PHY_ASIC_UpdateDelayChains();
	DDR_PHY_ASIC_ApplyPLLRegressionFix();
	DDR_PHY_ASIC_Disable3PKDeltaForce();
	DDR_PHY_ASIC_SetupRxTeDelay();
	DDR_PHY_ASIC_SetupDeltaDelay();
	DDR_PHY_ASIC_DisableRxCal();
}

/**
  * @brief
  */
static void DDR_PHY_ASIC_SetupEMI(void)
{
	DDRPHY_DEV->DUMMY_0         =     DDRPHY_DUMMY_0_CK_SR_P(2) | DDRPHY_DUMMY_0_CK_SR_N(2) |
									  DDRPHY_DUMMY_0_CS_SR_P(2) | DDRPHY_DUMMY_0_CS_SR_N(2) |
									  DDRPHY_DUMMY_0_CA_SR_P(2) | DDRPHY_DUMMY_0_CA_SR_N(2);
	DDRPHY_DEV->RW_DMY_A         =    DDRPHY_RW_DMY_A_DQS_1_SR_P(2) | DDRPHY_RW_DMY_A_DQS_1_SR_N(2) |
									  DDRPHY_RW_DMY_A_DQ_1_SR_P(2)  | DDRPHY_RW_DMY_A_DQ_1_SR_N(2)  |
									  DDRPHY_RW_DMY_A_DQS_0_SR_P(2) | DDRPHY_RW_DMY_A_DQS_0_SR_N(2) |
									  DDRPHY_RW_DMY_A_DQ_0_SR_P(2)  | DDRPHY_RW_DMY_A_DQ_0_SR_N(2);
}

/**
  * @brief  Boot up ASIC DDR PHY.
  * @param  odt_calibration: true = calibrated ODT path.
  */
void DDR_PHY_ASIC_Init(bool odt_calibration)
{
	DDR_PHY_ASIC_SetupDCC();
	DDR_PHY_ASIC_SetupPLL();
	DDR_PHY_ASIC_SetupAFIFO();
	DDR_PHY_ASIC_SetupLDOSupply();
	DDR_PHY_ASIC_SelectClkSrc(0);
	DDR_PHY_ASIC_WalkCrtCtlBringUp();
	DDR_PHY_ASIC_ReleasePhyReset();
	DDR_PHY_ASIC_ReleaseReset();
	DDR_PHY_ASIC_SelectClkSrc(1);
	DDR_PHY_ASIC_SetupPad(odt_calibration);
	DDR_PHY_ASIC_SetupWriteDelayTap();
	DDR_PHY_ASIC_EnableDFIAlert();
	DDR_PHY_ASIC_SetupCmdAdrPhase();
	DDR_PHY_ASIC_SetupRead();
	DDR_PHY_ASIC_SetupEMI();
}

/**
  * @brief  ASIC DDR PHY ops instance used by the adapter.
  */
const DDR_PHY_OPS_TypeDef DDR_PHY_ASIC_Ops = {
	.Init            =    DDR_PHY_ASIC_Init,
};
#else /* CONFIG_FPGA */
void DDR_PHY_FPGA_Init(bool odt_calibration)
{
	(void)odt_calibration;
	DDRPHY_FPGA_TypeDef *ddr_phy = DDRPHY_FPGA_DEV;

	CORE_S_BASE->SYS_DDRPHY_CTRL    =    CORE_S_BIT_HS_DDRPHY_CRT_CLK_EN |
										 CORE_S_BIT_HS_DDRPHY_RBUS_CLK_EN |
										 CORE_S_BIT_DDRPHY_VCCON |
										 CORE_S_BIT_DDRPHY_RBUS_EN |
										 CORE_S_BIT_HS_DDRPHY_CRT_RST;
	CORE_S_BASE->SYS_0_CLK_CTRL    |=    CORE_S_BIT_SYS_DDRPHY_CLKEN |
										 CORE_S_BIT_SYS_DDRC_CLKEN;
	*(volatile u32 *)&CORE_S_BASE->SYS_0_FUNC_CTRL |= BIT(11);
	CORE_S_BASE->SYS_1_FUNC_CTRL   |=    CORE_S_BIT_SYS_FEN_S7_14 |
										 CORE_S_BIT_SYS_FEN_S6_7;

	ddr_phy->DDRPHY_DPI_CTL_0       =    DDRPHY_BIT_RST_N |
										 DDRPHY_RST_FIFO_MODE(2) |
										 DDRPHY_CMD_RD_STR_NUM(0x4) |
										 DDRPHY_RX_RD_STR_NUM_1(1) |
										 DDRPHY_RX_RD_STR_NUM_0(1) |
										 DDRPHY_DQS_RD_STR_NUM_1(0x6) |
										 DDRPHY_DQ_RD_STR_NUM_1(0x6) |
										 DDRPHY_DQS_RD_STR_NUM_0(0x6) |
										 DDRPHY_DQ_RD_STR_NUM_0(0x6);
	ddr_phy->DDRPHY_DPI_CTL_1       =    DDRPHY_BIT_DATA_INIT |
										 DDRPHY_TM_RD_FIFO(0x3) |
										 DDRPHY_TM_DQS_EN1(0x2) |
										 DDRPHY_TM_DQS_EN_FTUN1(0x3) |
										 DDRPHY_TM_DQS_EN0(0x2) |
										 DDRPHY_TM_DQS_EN_FTUN0(0x3);
	ddr_phy->DDRPHY_DPI_CTL_2      &=    ~DDRPHY_MASK_FCLK_RX_SEL;
	ddr_phy->DDRPHY_DPI_CTL_2      |=    DDRPHY_FCLK_RX_SEL(0x7);
	ddr_phy->DDRPHY_DPI_CTL_6       =    0;
	RTK_LOGD(TAG, "DDRPHY Init Done\n");
}

const DDR_PHY_OPS_TypeDef DDR_PHY_FPGA_Ops = {
	.Init            =    DDR_PHY_FPGA_Init,
};
#endif
