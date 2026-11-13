/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#define COMPIL_LOG_LEVEL        RTK_LOG_DEBUG

#include "ameba_soc.h"

static const char *TAG = "DDRC";

/* DPIN pin encodings, driven on both clock edges (CMD_DPIN and CMD_DPIN_NDGE
 * share the same field layout). Command pins are active low. */
#define DPIN_PINS_DESELECT       (DDRC_CS_N_1(0x1) | DDRC_ACT_N(0x1) | DDRC_CS_N(0x1) | DDRC_RAS_N(0x0) | DDRC_CAS_N(0x1) | DDRC_WE_N(0x0))
#define DPIN_PINS_PRECHARGE_ALL  (DDRC_CS_N_1(0x1) | DDRC_ACT_N(0x1) | DDRC_CS_N(0x0) | DDRC_RAS_N(0x0) | DDRC_CAS_N(0x1) | DDRC_WE_N(0x0) | DDRC_ADDR(0x400))
/* REFRESH: CS/RAS/CAS low, WE high - same encoding as the vendor SRE
 * template preloaded into CMD_DPIN by InitConfig (0x02200000) */
#define DPIN_PINS_REFRESH        (DDRC_ACT_N(0x1) | DDRC_CS_N(0x0) | DDRC_RAS_N(0x0) | DDRC_CAS_N(0x0) | DDRC_WE_N(0x1))
/* MRS: CS/RAS/CAS/WE all low, mode register in BA, value on ADDR */
#define DPIN_PINS_MRS(ba, val)   (DDRC_CS_N_1(0x1) | DDRC_ACT_N(0x1) | DDRC_CS_N(0x0) | DDRC_RAS_N(0x0) | DDRC_CAS_N(0x0) | DDRC_WE_N(0x0) | DDRC_BA(ba) | DDRC_ADDR(val))

/* =========================================================================
 * Common DDR controller utilities
 * ========================================================================= */

/** @brief  Enable/gate the DDRC clocks and function-enable line.
  * @note   DISABLE also holds the DDRC in reset (REG_FEN_ID*), losing all
  *         register state - not a plain clock gate. Use DDRC_Reset() for a
  *         reset pulse with the clocks left running.
  * @param  NewState: ENABLE or DISABLE. */
void DDRC_Clock(u32 NewState)
{
	if (NewState == ENABLE) {
		RCC_PeriphClockCmd(APBPeriph_NULL, APBPeriph_CBUS_DDRC_CLOCK, ENABLE);
		RCC_PeriphClockCmd(APBPeriph_PLFM_CORE_DDRC_CLK, APBPeriph_DDRC_CLOCK, ENABLE);
	} else {
		RCC_PeriphClockCmd(APBPeriph_PLFM_CORE_DDRC_CLK, APBPeriph_DDRC_CLOCK, DISABLE);
		RCC_PeriphClockCmd(APBPeriph_NULL, APBPeriph_CBUS_DDRC_CLOCK, DISABLE);
	}
}

/** @brief  Pulse the ddrc_clk-domain reset back to power-on state.
  * @note   The DRAM-init FSM only runs out of reset: kicking CCR.INIT on a
  *         live controller never completes. Clocks must already be running.
  */
void DDRC_Reset(void)
{
	RCC_PeriphClockCmd(APBPeriph_PLFM_CORE_DDRC_CLK, APBPeriph_CLOCK_NULL, DISABLE);
	DelayUs(10);
	RCC_PeriphClockCmd(APBPeriph_PLFM_CORE_DDRC_CLK, APBPeriph_CLOCK_NULL, ENABLE);

	RTK_LOGD(TAG, "DDRC reset done\n");
}

/** @brief Configure one of eight DDR secure zones */
void DDRC_SetZone(int zone_num, int sec_mode, int start_addr, int end_addr)
{
	assert_param((zone_num >= 0) && (zone_num <= 7));
	assert_param(end_addr > start_addr);
	assert_param((start_addr >> 31) && (end_addr >> 31));

	int cgate_en_sec = 0;
	int sec_func_en = 1;

	DDRC_CFG_DEV->SEC[zone_num].BASE = (((start_addr >> 12) & 0xffff) << 12) | (cgate_en_sec << 0);
	DDRC_CFG_DEV->SEC[zone_num].END = (((end_addr >> 12) & 0xffff) << 12)  | (zone_num << 4) | (sec_mode << 1) | (sec_func_en << 0);
	RTK_LOGD("DDR", "Done Setting Zone %d [0x%x to 0x%x] to %s\n", zone_num, start_addr, end_addr, sec_mode ? "SECURE" : "NON SECURE");
}

/** @brief Disable one DDR secure zone */
void DDRC_DisableZone(int zone_num)
{
	assert_param((zone_num >= 0) && (zone_num <= 7));

	DDRC_CFG_DEV->SEC[zone_num].END = DDRC_CFG_DEV->SEC[zone_num].END & ~0x1;
	RTK_LOGD("DDR", "Done Disabling Zone %d\n", zone_num);
}

/** @brief Seed one SEC_NONCE register with a random value */
void DDRC_SetNonce(int zone_num)
{
	assert_param((zone_num >= 0) && (zone_num <= 7));

	DDRC_CFG_DEV->SEC_NONCE[zone_num] = zone_num * rand();
}

/** @brief Unblock all FE ports */
static void DDRC_UnblockPorts(void)
{
	for (int port_index = 0; port_index < DDRC_FE_PORT_NUM; port_index++) {
		DDRC_FE_DEV->PORT_CONF[port_index].CFG_PN_PORTCTRL &= ~DDRC_BIT_BLOCK_COMMAND_REQUEST;
	}
}

/** @brief Block all FE ports (stops new traffic; memory mode stays up) */
static void DDRC_BlockPorts(void)
{
	for (int port_index = 0; port_index < DDRC_FE_PORT_NUM; port_index++) {
		DDRC_FE_DEV->PORT_CONF[port_index].CFG_PN_PORTCTRL |= DDRC_BIT_BLOCK_COMMAND_REQUEST;
	}
}

/** @brief Park the DDRC: block the FE ports and take memory mode down */
static void DDRC_ParkMemory(void)
{
	DDRC_BlockPorts();

	DDRC_DEV->CSR = DDRC_CR_GP(0x0) | DDRC_CUR_GP(0x0) | DDRC_DPIN_IDX(0x0) | DDRC_DPIN_MOD(0x0) | \
					DDRC_BSTC_IDLE(0x1) | DDRC_TM_IDLE(0x1) | DDRC_MEM_IDLE(0x1) | DDRC_SRE_ST(0x0) | \
					DDRC_DPIN_CMD_INFO(0x0);
}

/** @brief Issue one DPIN command (same pins on both clock edges) and wait for
  *        DPIN_DONE.  */
static void DDRC_DpinCommand(u32 pins, u32 cmd_info)
{
	u32 timeout = 1000000;

	DDRC_DEV->CSR = DDRC_CR_GP(0x0) | DDRC_CUR_GP(0x0) | DDRC_DPIN_IDX(0x0) | DDRC_DPIN_MOD(DDR_DPIN_CMD) | \
					DDRC_BSTC_IDLE(0x1) | DDRC_TM_IDLE(0x1) | DDRC_MEM_IDLE(0x1) | DDRC_SRE_ST(0x0) | \
					DDRC_DPIN_CMD_INFO(cmd_info);
	DDRC_DEV->CMD_DPIN_NDGE = pins;
	DDRC_DEV->CMD_DPIN = pins;
	DDRC_DEV->CCR = DDRC_CR_UPDATE(0x0) | DDRC_FLUSH_FIFO(0x0) | DDRC_FRQC(0x0) | DDRC_SREX(0x0) | \
					DDRC_DPIN(0x1) | DDRC_BSTC(0x0) | DDRC_TM(0x0) | DDRC_INIT(0x0);
	while (((DDRC_DEV->CCR & DDRC_BIT_DPIN) != DDRC_BIT_DPIN) && (--timeout != 0));
	if (timeout == 0) {
		RTK_LOGE(TAG, "DPIN timeout, CCR=%08x CSR=%08x\n", DDRC_DEV->CCR, DDRC_DEV->CSR);
	}
}

/** @brief Park the controller and precharge-all, leaving the DRAM idle with
  *        every bank closed and CKE high. */
void DDRC_PrechargeAll(void)
{
	u32 timeout = 1000000;

	DDRC_ParkMemory();
	while (((DDRC_DEV->CSR & (DDRC_BIT_BSTC_IDLE | DDRC_BIT_TM_IDLE | DDRC_BIT_MEM_IDLE))
			!= (DDRC_BIT_BSTC_IDLE | DDRC_BIT_TM_IDLE | DDRC_BIT_MEM_IDLE)) && (--timeout != 0));
	if (timeout == 0) {
		RTK_LOGE(TAG, "park timeout, CSR=%08x\n", DDRC_DEV->CSR);
	}

	/* No auto-refresh may slip in between the DPIN commands below */
	DDRC_DEV->DRR |= DDRC_BIT_REF_DIS;

	DDRC_DpinCommand(DPIN_PINS_DESELECT, DDR_DPIN_CMD_RSVD);

	DDRC_DpinCommand(DPIN_PINS_PRECHARGE_ALL, DDR_DPIN_CMD_PRECHARGE_ALL);
	DelayUs(1); /* > tRP */
}

/** @brief Enter DRAM self-refresh
  *
  * SRE = REFRESH with CKE registered low on the same clock edge, built as a
  * DPIN REFRESH kick with the CKE tie cleared under IOCR.STC_CKE=1. The
  * engine's AUTO_SREX is not used: it was board-measured to drop CKE detached
  * from its REFRESH, giving precharge power-down, never self-refresh.
  *
  * The PHY must launch CKE in the same cycle as the command group
  * (CMD_ADR_PH.CKE_PH_SEL == CMD_PH_SEL) or the SRE degrades the same way.
  * CSR.SRE_ST does not assert - the engine is not involved.
  */
void DDRC_EnterSelfRefresh(void)
{
	u32 timeout = 1000000;

	/* Park, stop auto-refresh, close all banks */
	DDRC_PrechargeAll();

	/* PASR: narrow the refreshed banks. MRS is legal here (banks precharged,
	 * CKE still high); the exit re-init restores full-array MR2. */
	if (ddr_adapter.pasr != JEDEC_MR2_PASR_FULL) {
		u32 mr2 = ddr_adapter.die->mr[2] | ddr_adapter.pasr;

		DDRC_DpinCommand(DPIN_PINS_MRS(2, mr2), DDR_DPIN_CMD_RSVD);
		DelayUs(1); /* > tMOD */
		RTK_LOGI(TAG, "PASR applied, MR2=%08x\n", mr2);
	}

	/* CKE follows TIE_DPIN.CKE sampled at the DPIN kick once STC_CKE is
	 * set; IOCR is CR_UPDATE-latched */
	DDRC_DEV->IOCR |= DDRC_BIT_STC_CKE;
	DDRC_DEV->CCR = DDRC_CR_UPDATE(0x1) | DDRC_FLUSH_FIFO(0x1) | DDRC_FRQC(0x0) | DDRC_SREX(0x0) | \
					DDRC_DPIN(0x0) | DDRC_BSTC(0x0) | DDRC_TM(0x0) | DDRC_INIT(0x0);
	while (((DDRC_DEV->CCR & DDRC_BIT_CR_UPDATE) != DDRC_BIT_CR_UPDATE) && (--timeout != 0));
	if (timeout == 0) {
		RTK_LOGE(TAG, "STC_CKE CR_UPDATE timeout, CCR=%08x CSR=%08x\n", DDRC_DEV->CCR, DDRC_DEV->CSR);
	}

	/* CKE low rides the REFRESH kick = SRE */
	DDRC_DEV->TIE_DPIN = DDRC_ODT_1(0x0) | DDRC_CKE_1(0x0) | DDRC_RST_N_1(0x1) | DDRC_ALERT_N(0x1) | \
						 DDRC_TEN(0x0) | DDRC_ODT(0x0) | DDRC_CKE(0x0) | DDRC_RST_N(0x1);
	DDRC_DpinCommand(DPIN_PINS_REFRESH, DDR_DPIN_CMD_RSVD);

	RTK_LOGI(TAG, "SRE via DPIN+TIE CKE issued\n");
	RTK_LOGI(TAG, "Self Refresh Entered\n");
}

/** @brief Exit DRAM self-refresh
  *
  * Re-runs the cold bring-up order: DDRC reset, PHY from reset, full PHY init
  * (SetupPad powers the pads back up, waking the die), then DDRC init. DRAM
  * contents survive - the re-init never touches the array.
  *
  * The vendor "retention" order (DDRC init with the pads still down, PHY
  * after, no MRS replay) is deliberately not used: it left the DFI data path
  * desynced and reads returned garbage after exit.
  */
void DDRC_ExitSelfRefresh(void)
{
	DDRC_Clock(ENABLE);
	DDR_PHY_Clock(ENABLE);
	DDRC_Reset();
	DDR_PHY_Reset();
	DDR_PHY_SetPLLFreq(DDR_PHY_GetPLLFreq());
	DDR_InitPHY();
	DDR_InitDDRC();

	RTK_LOGI(TAG, "Self Refresh Exited\n");
}

/** @brief Program per-port read QoS: ENC > ISP > others */
void DDRC_SetReadPriority(void)
{
	// Expected Priority: ENC > ISP > TS/GE/LX > NN & CA32/FP/WIFI_MEM/PERI & VP/GDMA1

	// RQOS
	// LPR -> 0 : 8
	// VPR -> 9 : B
	// HPR -> C : F
	DDRC_FE_DEV->PORT_CONF[0].CFG_PN_RQOS = DDRC_rqos_map_region2(2) | DDRC_rqos_map_region1(1) | DDRC_rqos_map_region0(0) |
											DDRC_rqos_map_level2(0xB) | DDRC_rqos_map_level1(0x8);
	RTK_LOGD(TAG, "[RQOS] Addr: 0x%x, Data: 0x%x\n", &(DDRC_FE_DEV->PORT_CONF[0].CFG_PN_RQOS), DDRC_FE_DEV->PORT_CONF[0].CFG_PN_RQOS);

	RXI_CORE_BASE->GPR_2 = (DDRC_P0_ARQOS(0x4) | DDRC_P1_ARQOS(0xD) | DDRC_P2_ARQOS(0xC) |
							DDRC_P3_ARQOS(0x7) | DDRC_P4_ARQOS(0x4) | DDRC_P5_ARQOS(0x4));
	RTK_LOGD(TAG, "[ARQOS] Addr: 0x%x, Data: 0x%x\n", &(RXI_CORE_BASE->GPR_2), RXI_CORE_BASE->GPR_2);

	for (int port_index = 0; port_index < 6; port_index++) {
		DDRC_FE_DEV->PORT_CONF[port_index].CFG_PN_RDEXPIRY = DDRC_rd_expiry_time(0x240000);
		RTK_LOGD(TAG, "[QOS] Addr: 0x%x, Data: 0x%x\n", &(DDRC_FE_DEV->PORT_CONF[port_index].CFG_PN_RDEXPIRY), DDRC_FE_DEV->PORT_CONF[port_index].CFG_PN_RDEXPIRY);
	}
}

/** @brief Program per-port write QoS: ENC > ISP > others */
void DDRC_SetWritePriority(void)
{
	// WQOS
	// LPW -> 0 : 8
	// VPW -> 9 : F
	DDRC_FE_DEV->PORT_CONF[0].CFG_PN_WQOS = DDRC_wqos_map_region2(1) | DDRC_wqos_map_region1(1) | DDRC_wqos_map_region0(0) |
											DDRC_wqos_map_level2(0xF) | DDRC_wqos_map_level1(0x8);
	RTK_LOGD(TAG, "[WQOS] Addr: 0x%x, Data: 0x%x\n", &(DDRC_FE_DEV->PORT_CONF[0].CFG_PN_WQOS), DDRC_FE_DEV->PORT_CONF[0].CFG_PN_WQOS);

	RXI_CORE_BASE->GPR_3 = (DDRC_P0_AWQOS(0x4) | DDRC_P1_AWQOS(0xD) | DDRC_P2_AWQOS(0xC) |
							DDRC_P3_AWQOS(0x7) | DDRC_P4_AWQOS(0x4) | DDRC_P5_AWQOS(0x4));
	RTK_LOGD(TAG, "[AWQOS] Addr: 0x%x, Data: 0x%x\n", &(RXI_CORE_BASE->GPR_3), RXI_CORE_BASE->GPR_3);

	for (int port_index = 0; port_index < 6; port_index++) {
		DDRC_FE_DEV->PORT_CONF[port_index].CFG_PN_WREXPIRY = DDRC_wr_expiry_time(0x240000);
		RTK_LOGD(TAG, "[QOS] Addr: 0x%x, Data: 0x%x\n", &(DDRC_FE_DEV->PORT_CONF[port_index].CFG_PN_WREXPIRY), DDRC_FE_DEV->PORT_CONF[port_index].CFG_PN_WREXPIRY);
	}
}

/* =========================================================================
 * ASIC DDR controller boot up flow
 * ========================================================================= */

/** @brief Queue 10-step JEDEC LMR init sequence into IFCT/IFGP0 */
static void DDRC_QueueLMRSequence(const u32 *mr, u32 reset_wait_sel)
{
	/* Post-reset wait (variant-specific at index 0) */
	DDRC_DEV->IFCT  = DDRC_CMD_WTIME_SEL(reset_wait_sel) | DDRC_MACRO_CMD(MACRO_CMD_NOP) | \
					  DDRC_IFCT_WRRD(0x1) | DDRC_IFCT_INDEX(0x0);
	DDRC_DEV->IFGP0 = DDRC_GP0_MR_NUM(0x0) | DDRC_GP0_ADDR(0x0);

	DDRC_DEV->IFCT  = DDRC_CMD_WTIME_SEL(MACRO_CMD_WAIT_tUS) | DDRC_MACRO_CMD(MACRO_CMD_NOP) | \
					  DDRC_IFCT_WRRD(0x1) | DDRC_IFCT_INDEX(0x1);
	DDRC_DEV->IFGP0 = DDRC_GP0_MR_NUM(0x0) | DDRC_GP0_ADDR(0x0);

	DDRC_DEV->IFCT  = DDRC_CHK_INIT_CYC(0x1) | DDRC_INIT_CYC(0x1) | DDRC_MACRO_CMD(MACRO_CMD_NOP) | \
					  DDRC_IFCT_WRRD(0x1) | DDRC_IFCT_INDEX(0x2);
	DDRC_DEV->IFGP0 = DDRC_GP0_MR_NUM(0x0) | DDRC_GP0_ADDR(0x0);

	DDRC_DEV->IFCT  = DDRC_CMD_WTIME_SEL(MACRO_CMD_WAIT_tNS) | DDRC_MACRO_CMD(MACRO_CMD_NOP) | \
					  DDRC_IFCT_WRRD(0x1) | DDRC_IFCT_INDEX(0x3);
	DDRC_DEV->IFGP0 = DDRC_GP0_MR_NUM(0x0) | DDRC_GP0_ADDR(0x0);

	/* MR2 */
	DDRC_DEV->IFCT  = DDRC_CMD_WTIME_SEL(MACRO_CMD_WAIT_tMRD) | DDRC_MACRO_CMD(MACRO_CMD_LMR) | DDRC_FRQC_EN(1) | \
					  DDRC_IFCT_WRRD(0x1) | DDRC_IFCT_INDEX(0x4);
	DDRC_DEV->IFGP0 = DDRC_GP0_MR_NUM(0x2) | DDRC_GP0_ADDR(mr[2]);

	/* MR3 */
	DDRC_DEV->IFCT  = DDRC_CMD_WTIME_SEL(MACRO_CMD_WAIT_tMRD) | DDRC_MACRO_CMD(MACRO_CMD_LMR) | DDRC_FRQC_EN(1) | \
					  DDRC_IFCT_WRRD(0x1) | DDRC_IFCT_INDEX(0x5);
	DDRC_DEV->IFGP0 = DDRC_GP0_MR_NUM(0x3) | DDRC_GP0_ADDR(mr[3]);

	/* MR1 */
	DDRC_DEV->IFCT  = DDRC_CMD_WTIME_SEL(MACRO_CMD_WAIT_tMRD) | DDRC_MACRO_CMD(MACRO_CMD_LMR) | DDRC_FRQC_EN(1) | \
					  DDRC_IFCT_WRRD(0x1) | DDRC_IFCT_INDEX(0x6);
	DDRC_DEV->IFGP0 = DDRC_GP0_MR_NUM(0x1) | DDRC_GP0_ADDR(mr[1]);

	/* MR0 + MR0_MOD */
	DDRC_DEV->IFCT  = DDRC_CMD_WTIME_SEL(MACRO_CMD_WAIT_tMOD) | DDRC_MACRO_CMD(MACRO_CMD_LMR_MOD) | DDRC_FRQC_EN(1) | \
					  DDRC_IFCT_WRRD(0x1) | DDRC_IFCT_INDEX(0x7);
	DDRC_DEV->IFGP0 = DDRC_GP0_MR_NUM(0x0) | DDRC_GP0_ADDR(mr[0]);

	/* ZQC */
	DDRC_DEV->IFCT  = DDRC_CMD_WTIME_SEL(MACRO_CMD_WAIT_tDLL) | DDRC_MACRO_CMD(MACRO_CMD_ZQC) | DDRC_FRQC_EN(1) | \
					  DDRC_IFCT_WRRD(0x1) | DDRC_IFCT_INDEX(0x8);
	DDRC_DEV->IFGP0 = DDRC_GP0_MR_NUM(0x0) | DDRC_GP0_ADDR(0x0);

	/* Completion */
	DDRC_DEV->IFCT  = DDRC_MACRO_CMD(0x0) | DDRC_FRQC_EN(1) | DDRC_IFCT_WRRD(0x1) | DDRC_IFCT_INDEX(0x9);
	DDRC_DEV->IFGP0 = DDRC_GP0_MR_NUM(0x0) | DDRC_GP0_ADDR(0x0);
}

#if !defined(CONFIG_FPGA) || (CONFIG_FPGA == 0)
void DDRC_ASIC_InitDRAM(const u32 *mr)
{
	DDRC_QueueLMRSequence(mr, MACRO_CMD_WAIT_RST_tUS);
}

/** @brief Program DDRC back-end registers from timing struct (pre-LMR) */
void DDRC_ASIC_InitConfig(const DDRC_Timing_TypeDef *ddr_timing)
{
	// #DDRC remap & bank control
	DDRC_DEV->ROW_REMAP0    =   DDRC_ROW7_REMAP(0x8) | DDRC_ROW6_REMAP(0x8) | DDRC_ROW5_REMAP(0x8) | DDRC_ROW4_REMAP(0x8) | \
								DDRC_ROW3_REMAP(0x8) | DDRC_ROW2_REMAP(0x8) | DDRC_ROW1_REMAP(0x8) | DDRC_ROW0_REMAP(0x8);
	DDRC_DEV->ROW_REMAP1    =   DDRC_ROW15_REMAP(0x8) | DDRC_ROW14_REMAP(0x8) | DDRC_ROW13_REMAP(0x8) | DDRC_ROW12_REMAP(0x8) | \
								DDRC_ROW11_REMAP(0x8) | DDRC_ROW10_REMAP(0x8) | DDRC_ROW9_REMAP(0x8) | DDRC_ROW8_REMAP(0x8);
	DDRC_DEV->ROW_REMAP2    =   DDRC_ROW16_REMAP(0xf);
	DDRC_DEV->BANK_REMAP0   =   DDRC_BA2_REMAP(0xa) | DDRC_BA1_REMAP(0x9) | DDRC_BA0_REMAP(0x8);
	DDRC_DEV->COLU_REMAP0   =   DDRC_COL9_REMAP(0x0) | DDRC_COL8_REMAP(0x0) | DDRC_COL7_REMAP(0x0) | DDRC_COL6_REMAP(0x0) | \
								DDRC_COL5_REMAP(0x0) | DDRC_COL4_REMAP(0x0) | DDRC_COL3_REMAP(0x0) | DDRC_COL2_REMAP(0x0);
	DDRC_DEV->COLU_REMAP1   =   DDRC_COL11_REMAP(0xf) | DDRC_COL10_REMAP(0xf);
	DDRC_DEV->RANK_REMAP0   =   DDRC_RA_REMAP(0x1f);
	DDRC_DEV->CH_REMAP0     =   DDRC_CH_REMAP(0x1f);
	DDRC_DEV->BG_REMAP0     =   DDRC_BG1_REMAP(0x1f) | DDRC_BG0_REMAP(0x1f);
	DDRC_DEV->SLOT_PARK     =   DDRC_IDLE_WINOW(0x10) | DDRC_PERF_PARK(0x0);
	DDRC_DEV->SLOT_STARVE   =   DDRC_BASE_STARVE_CYC(0x7f) | DDRC_LPR_STARVE_CYC(0x4) | DDRC_HPR_STARVE_CYC(0x1) | DDRC_LPW_STARVE_CYC(0x4);
	DDRC_DEV->SLOT_CNTR     =   DDRC_LPR_ISU_LMT(0x8) | DDRC_HPR_ISU_LMT(0x8) | DDRC_LPW_ISU_LMT(0x8);
	DDRC_DEV->EXP_CTRL      =   DDRC_EXP_BASE(0x1);
	DDRC_DEV->HZD_CTRL0     =   DDRC_DFWD_LIMIT_NUM(0x4) | DDRC_RHZD_THD(0x0) | DDRC_WHZD_THD(0x0);
	DDRC_DEV->HZD_CTRL1     =   DDRC_RHZD_CRDT_NUM(0x1) | DDRC_RHZD_CRDT_CYC(0x400) | DDRC_WHZD_CRDT_NUM(0x1) | DDRC_WHZD_CRDT_CYC(0x400);
	DDRC_DEV->SLOT_CNTR1    =   DDRC_RCB_HARD_TH(0x0) | DDRC_WCB_HARD_TH(0x0) | DDRC_SOFT_TH(0x0) | DDRC_WCB_TH_HPR(0x0);
	DDRC_DEV->CBCTL0        =   DDRC_LPR_RMW_RUN_LEN(0xf) | DDRC_LPR_FREESLOT_NUM(0x2f);
	DDRC_DEV->CBCTL1        =   DDRC_PAGE_HIT_THR_WR(0x4) | DDRC_PAGE_HIT_THR_RD(0x4) | DDRC_LPW_RMW_RUN_LEN(0xf) | DDRC_W_HAZ_EXP_CTRL(0xf) | \
								DDRC_PAGE_HIT_THR_WR_EN(0x0) | DDRC_PAGE_HIT_THR_RD_EN(0x0) | DDRC_PAGEOPEN(0x1);
	DDRC_DEV->CBSTARVE0     =   DDRC_LPR_STARVE_DIS(0xffff) | DDRC_LPR_MAX_STARVE(0xffff);
	DDRC_DEV->CBSTARVE1     = 	DDRC_HPR_STARVE_DIS(0x0) | DDRC_HPR_MAX_STARVE(0x80);
	DDRC_DEV->CBSTARVE2     =   DDRC_LPW_STARVE_DIS(0xffff) | DDRC_LPW_MAX_STARVE(0xffff);
	DDRC_DEV->CBSTARVE3     =   DDRC_LPR_RMW_STARVE_DIS(0xffff) | DDRC_LPR_RMW_MAX_STARVE(0xffff);
	DDRC_DEV->CBSTARVE4     =   DDRC_LPW_RMW_STARVE_DIS(0xffff) | DDRC_LPW_RMW_MAX_STARVE(0xffff);
	DDRC_DEV->BSCH          =   DDRC_BA_SEL_LMT(0x2);
	DDRC_DEV->DRAM_SIZE     =   DDRC_DRAM_SIZE(0x7ffffff);
	DDRC_DEV->DSIZE_UP_4GB  =   DDRC_DSIZE_8GB(0x0);
	DDRC_DEV->MISC          =   DDRC_ONE_BANK_EN(0x0) | DDRC_HYBR_SIZE(0x0) | DDRC_BST_SIZE(0x1) | DDRC_BANK_SIZE(0x2) | \
								DDRC_PAGE_SIZE(0x3);
	DDRC_DEV->CHOP_SIZE     =   DDRC_CR_CHOP_SIZE(0x4);
	DDRC_DEV->DCR           =   DDRC_ZQC(0x0) | DDRC_FULLWR(0x0) | DDRC_RMW_GRA(0x0) | DDRC_BG2(0x0) |      \
								DDRC_GEAR_DOWN(0x0) | DDRC_PAR(0x0) | DDRC_RD_DBI(0x0) | DDRC_WR_DBI(0x0) | \
								DDRC_RANK2(0x0) | DDRC_DFI_RATE(0x2) | DDRC_DRAM_TYPE(0x3) | DDRC_BIT_HALF_DQ;
	DDRC_DEV->IOCR          =   DDRC_FPGA_DFI(0x0) | DDRC_FPGA_LP(0x0) | DDRC_AC_SHARE(0x0) | DDRC_ADDR_SHARE(0x0) |                                      \
								DDRC_DC_CH1(0x0) | DDRC_TPHY_RDATA_EN(ddr_timing->TPHY_RD) | DDRC_TPHY_WLAT(ddr_timing->TPHY_WL) | DDRC_TPHY_WDATA(ddr_timing->TPHY_WD) | \
								DDRC_DYN_SRE(0x0) | DDRC_HALF_CS_N(0x0) | DDRC_TRTW_2T_DIS(0x0) | DDRC_2N_PRE_CMD(0x1) |                                  \
								DDRC_STC_ODT(0x0) | DDRC_STC_CKE(0x0) | DDRC_ODT_2PIN(0x0);
	DDRC_DEV->CMD_DPIN      =   DDRC_CS_N_1(0x0) | DDRC_PARITY(0x0) | DDRC_ACT_N(0x1) | DDRC_CS_N(0x0) | \
								DDRC_RAS_N(0x0) | DDRC_CAS_N(0x0) | DDRC_WE_N(0x1) | DDRC_BA(0x0) |      \
								DDRC_ADDR(0x0);
	DDRC_DEV->CMD_DPIN_NDGE =   DDRC_CS_N_1_NDGE(0x0) | DDRC_PARITY_NDGE(0x0) | DDRC_ACT_N_NDGE(0x1) | DDRC_CS_N_NDGE(0x0) | \
								DDRC_RAS_N_NDGE(0x0) | DDRC_CAS_N_NDGE(0x0) | DDRC_WE_N_NDGE(0x1) | DDRC_BA_NDGE(0x0) |      \
								DDRC_ADDR_NDGE(0x0);
	DDRC_DEV->CSR           =   DDRC_CR_GP(0x0) | DDRC_CUR_GP(0x0) | DDRC_DPIN_IDX(0x0) | DDRC_DPIN_MOD(0x0) |    \
								DDRC_BSTC_IDLE(0x1) | DDRC_TM_IDLE(0x1) | DDRC_MEM_IDLE(0x1) | DDRC_SRE_ST(0x0) | \
								DDRC_DPIN_CMD_INFO(0x0);
	DDRC_DEV->TIE_DPIN      =   DDRC_ODT_1(0x0) | DDRC_CKE_1(0x1) | DDRC_RST_N_1(0x1) | DDRC_ALERT_N(0x1) | \
								DDRC_TEN(0x0) | DDRC_ODT(0x0) | DDRC_CKE(0x1) | DDRC_RST_N(0x1);
	DDRC_DEV->MR_INFO       =   DDRC_RTP(ddr_timing->RTP) | DDRC_NWR(ddr_timing->nWR) | DDRC_T_DQSCK_MAX(ddr_timing->tDQSCK) | DDRC_ADD_LAT(0x0) | \
								DDRC_RD_LAT(ddr_timing->RD_LAT) | DDRC_WR_LAT(ddr_timing->WR_LAT);
	DDRC_DEV->DRR           =   DDRC_ZQCL_INV(0x1) | DDRC_REF_DIS(0x0) | DDRC_REF_NUM(0x1) | DDRC_TREF(ddr_timing->tREFI_tck) | \
								DDRC_TRFC(ddr_timing->tRFC_tck);
	DDRC_DEV->TPR0          =   DDRC_TZQCS(ddr_timing->t_ZQCS_tck) | DDRC_TPD_EN(ddr_timing->tPD_EN) | DDRC_TCKE(ddr_timing->tCKE_tck) | DDRC_TRTP(
									ddr_timing->tRTP_tck) | \
								DDRC_TWR(ddr_timing->tWR_tck) | DDRC_TRAS(ddr_timing->tRAS_tck) | DDRC_TRP(ddr_timing->tRP_tck);
	DDRC_DEV->TPR1          =   DDRC_TFAW(ddr_timing->tFAW_tck) | DDRC_TRTW(ddr_timing->tRTW_tck) | DDRC_TWTR(ddr_timing->tWTR_tck) | DDRC_TCCD(
									ddr_timing->tCCD_tck) | \
								DDRC_TRCD(ddr_timing->tRCD_tck) | DDRC_TRC(ddr_timing->tRC_tck) | DDRC_TRRD(ddr_timing->tRRD_tck);
	DDRC_DEV->TPR2          =   DDRC_RST_TUS(ddr_timing->rst_tus_tck) | DDRC_TUS(ddr_timing->tus_tck) | DDRC_TNS(ddr_timing->tns_tck) | DDRC_TMRD_INIT_MRR(
									ddr_timing->tMRD_tck) | \
								DDRC_INIT_REF_NUM_ZQC_INV(ddr_timing->zqc_inv);
	DDRC_DEV->TPR3          =   DDRC_TCCD_K(0x0) | DDRC_TCCD_KW(0x0) | DDRC_TRRD_S(0x0) | DDRC_TWTR_S(0x0) |                                 \
								DDRC_TCCD_S(0x0) | DDRC_TPPD(0x0) | DDRC_TWR_MSB(ddr_timing->tWR_msb) | DDRC_TRFC_MSB(ddr_timing->tRFC_msb) | \
								DDRC_TRTW_MSB(ddr_timing->tTRW_msb) | DDRC_TWTR_MSB(ddr_timing->tWTR_msb) | DDRC_TRCD_MSB(ddr_timing->tRCD_msb) | DDRC_DPIN_CMD_LAT(0x0) | \
								DDRC_TFAW_MSB(ddr_timing->tFAW_msb) | DDRC_TRAS_MSB(ddr_timing->tRAS_msb) | DDRC_TRPAB_PB(ddr_timing->tRPAB_PB_tck);
	DDRC_DEV->TPR4          =   DDRC_TCKECK(ddr_timing->tCKECK);
	DDRC_DEV->TPR5          =   DDRC_ODT_DLY(ddr_timing->odt_dly) | DDRC_ODT_HOLD(ddr_timing->odt_hold);
	DDRC_DEV->TPR6          =   0x0;
	DDRC_DEV->IMR           =   DDRC_INTR_MASK(0xffff);
	DDRC_DEV->CSR           =   DDRC_CR_GP(0x0) | DDRC_CUR_GP(0x0) | DDRC_DPIN_IDX(0x0) | DDRC_DPIN_MOD(0x0) |    \
								DDRC_BSTC_IDLE(0x1) | DDRC_TM_IDLE(0x1) | DDRC_MEM_IDLE(0x1) | DDRC_SRE_ST(0x0) | \
								DDRC_DPIN_CMD_INFO(0x0);
	RTK_LOGI(TAG, "DDRC Config Done\n");
}

/** @brief Kick INIT, CR_UPDATE, open FE ports.
  * @note  The ports reopen even on timeout - this is the only place they do,
  *        and blocked ports stall masters forever rather than erroring. */
void DDRC_ASIC_CR_UPDATE(void)
{
	u32 cr_update_timeout;
	bool failed = false;

	DDRC_DEV->CSR           =   DDRC_CR_GP(0x0) | DDRC_CUR_GP(0x0) | DDRC_DPIN_IDX(0x0) | DDRC_DPIN_MOD(0x0) |    \
								DDRC_BSTC_IDLE(0x1) | DDRC_TM_IDLE(0x1) | DDRC_MEM_IDLE(0x1) | DDRC_SRE_ST(0x0) | \
								DDRC_DPIN_CMD_INFO(0x0);
	DDRC_DEV->CCR           =   DDRC_CR_UPDATE(0x0) | DDRC_FLUSH_FIFO(0x0) | DDRC_FRQC(0x0) | DDRC_SREX(0x0) | \
								DDRC_DPIN(0x0) | DDRC_BSTC(0x0) | DDRC_TM(0x0) | DDRC_INIT(0x1);
	cr_update_timeout = 1000000;
	while (((DDRC_DEV->CCR & DDRC_BIT_INIT) != DDRC_BIT_INIT) && (--cr_update_timeout != 0));
	if (cr_update_timeout == 0) {
		RTK_LOGE(TAG, "CR_UPDATE INIT timeout, CCR=%08x CSR=%08x\n", DDRC_DEV->CCR, DDRC_DEV->CSR);
		failed = true;
	}

	DDRC_DEV->CSR           =   DDRC_CR_GP(0x0) | DDRC_CUR_GP(0x0) | DDRC_DPIN_IDX(0x0) | DDRC_DPIN_MOD(0x0) |    \
								DDRC_BSTC_IDLE(0x1) | DDRC_TM_IDLE(0x1) | DDRC_MEM_IDLE(0x0) | DDRC_SRE_ST(0x0) | \
								DDRC_DPIN_CMD_INFO(0x0);
	DDRC_DEV->CCR           =   DDRC_CR_UPDATE(0x1) | DDRC_FLUSH_FIFO(0x1) | DDRC_FRQC(0x0) | DDRC_SREX(0x0) | \
								DDRC_DPIN(0x0) | DDRC_BSTC(0x0) | DDRC_TM(0x0) | DDRC_INIT(0x0);
	cr_update_timeout = 1000000;
	while (((DDRC_DEV->CSR & (DDRC_BIT_BSTC_IDLE | DDRC_BIT_TM_IDLE | DDRC_BIT_MEM_IDLE))
			!= (DDRC_BIT_BSTC_IDLE | DDRC_BIT_TM_IDLE)) && (--cr_update_timeout != 0));
	if (cr_update_timeout == 0) {
		RTK_LOGE(TAG, "CR_UPDATE flush timeout, CCR=%08x CSR=%08x\n", DDRC_DEV->CCR, DDRC_DEV->CSR);
		failed = true;
	}

	DDRC_FE_DEV->CFG_CM_FE_RRB_0 = DDRC_MAX_NUM_LPR_ENTRIES_CH0(0x2f);
	DDRC_FE_DEV->CFG_CM_FE_RRB_1 = DDRC_MAX_NUM_LPR_ENTRIES_CH0(0x2f);
	DDRC_UnblockPorts();

	if (failed) {
		RTK_LOGE(TAG, "DDRC FE Port Opened after timeout - DDR is not usable\n");
	} else {
		RTK_LOGI(TAG, "DDRC FE Port Opened\n");
	}
}

/* ASIC DDRC ops vtable */
const DDRC_OPS_TypeDef DDRC_ASIC_Ops = {
	.InitConfig       = DDRC_ASIC_InitConfig,
	.InitDRAM         = DDRC_ASIC_InitDRAM,
	.CR_UPDATE        = DDRC_ASIC_CR_UPDATE,
	.EnterSelfRefresh = DDRC_EnterSelfRefresh,
	.ExitSelfRefresh  = DDRC_ExitSelfRefresh,
	.SetZone          = DDRC_SetZone,
	.DisableZone      = DDRC_DisableZone,
};

#else /* CONFIG_FPGA */
void DDRC_FPGA_InitDRAM(const u32 *mr)
{
	DDRC_QueueLMRSequence(mr, MACRO_CMD_WAIT_1_CYCLE);
}

void DDRC_FPGA_InitConfig(const DDRC_Timing_TypeDef *ddr_timing)
{
	DDRC_DEV->ROW_REMAP0    = DDRC_ROW7_REMAP(0x8) | DDRC_ROW6_REMAP(0x8) | DDRC_ROW5_REMAP(0x8) | DDRC_ROW4_REMAP(0x8) | \
							  DDRC_ROW3_REMAP(0x8) | DDRC_ROW2_REMAP(0x8) | DDRC_ROW1_REMAP(0x8) | DDRC_ROW0_REMAP(0x8);
	DDRC_DEV->ROW_REMAP1    = DDRC_ROW15_REMAP(0x8) | DDRC_ROW14_REMAP(0x8) | DDRC_ROW13_REMAP(0x8) | DDRC_ROW12_REMAP(0x8) | \
							  DDRC_ROW11_REMAP(0x8) | DDRC_ROW10_REMAP(0x8) | DDRC_ROW9_REMAP(0x8) | DDRC_ROW8_REMAP(0x8);
	DDRC_DEV->ROW_REMAP2    = DDRC_ROW16_REMAP(0x8);
	DDRC_DEV->BANK_REMAP0   = DDRC_BA2_REMAP(0xa) | DDRC_BA1_REMAP(0x9) | DDRC_BA0_REMAP(0x8);
	DDRC_DEV->COLU_REMAP0   = DDRC_COL9_REMAP(0x0) | DDRC_COL8_REMAP(0x0) | DDRC_COL7_REMAP(0x0) | DDRC_COL6_REMAP(0x0) | \
							  DDRC_COL5_REMAP(0x0) | DDRC_COL4_REMAP(0x0) | DDRC_COL3_REMAP(0x0) | DDRC_COL2_REMAP(0x0);
	DDRC_DEV->COLU_REMAP1   = DDRC_COL11_REMAP(0xf) | DDRC_COL10_REMAP(0xf);
	DDRC_DEV->RANK_REMAP0   = DDRC_RA_REMAP(0x1f);
	DDRC_DEV->CH_REMAP0     = DDRC_CH_REMAP(0x1f);
	DDRC_DEV->BG_REMAP0     = DDRC_BG1_REMAP(0x1f) | DDRC_BG0_REMAP(0x1f);
	DDRC_DEV->SLOT_PARK     = DDRC_IDLE_WINOW(0x10);
	DDRC_DEV->SLOT_STARVE   = DDRC_BASE_STARVE_CYC(0xbf) | DDRC_LPR_STARVE_CYC(0x8) | DDRC_HPR_STARVE_CYC(0x1) | DDRC_LPW_STARVE_CYC(0x8);
	DDRC_DEV->SLOT_CNTR     = DDRC_LPR_ISU_LMT(0xf) | DDRC_HPR_ISU_LMT(0x4) | DDRC_LPW_ISU_LMT(0xf);
	DDRC_DEV->EXP_CTRL      = DDRC_EXP_BASE(0x1);
	DDRC_DEV->HZD_CTRL0     = DDRC_DFWD_LIMIT_NUM(0x4) | DDRC_BIT_DIS_DFWD | DDRC_BIT_DIS_WMG | DDRC_RHZD_THD(0x4) | DDRC_WHZD_THD(0x4);
	DDRC_DEV->HZD_CTRL1     = DDRC_RHZD_CRDT_NUM(0x2) | DDRC_RHZD_CRDT_CYC(0x100) | DDRC_WHZD_CRDT_NUM(0x2) | DDRC_WHZD_CRDT_CYC(0x100);
	DDRC_DEV->SLOT_CNTR1    = DDRC_RCB_HARD_TH(0) | DDRC_WCB_HARD_TH(0) | DDRC_SOFT_TH(0) | DDRC_WCB_TH_HPR(1);
	DDRC_DEV->CBCTL0        = DDRC_LPR_RMW_RUN_LEN(0xf) | DDRC_LPR_FREESLOT_NUM(0x2f);
	DDRC_DEV->CBCTL1        = DDRC_PAGE_HIT_THR_WR(0x0) | DDRC_PAGE_HIT_THR_RD(0x0) | DDRC_LPW_RMW_RUN_LEN(0xf) | \
							  DDRC_W_HAZ_EXP_CTRL(0xf) | DDRC_BIT_PAGEOPEN;
	DDRC_DEV->CBSTARVE0     = DDRC_LPR_STARVE_DIS(0xffff) | DDRC_LPR_MAX_STARVE(0xffff);
	DDRC_DEV->CBSTARVE1     = DDRC_HPR_STARVE_DIS(0xffff) | DDRC_HPR_MAX_STARVE(0xffff);
	DDRC_DEV->CBSTARVE2     = DDRC_LPW_STARVE_DIS(0xffff) | DDRC_LPW_MAX_STARVE(0xffff);
	DDRC_DEV->CBSTARVE3     = DDRC_LPR_RMW_STARVE_DIS(0xffff) | DDRC_LPR_RMW_MAX_STARVE(0xffff);
	DDRC_DEV->CBSTARVE4     = DDRC_LPW_RMW_STARVE_DIS(0xffff) | DDRC_LPW_RMW_MAX_STARVE(0xffff);
	DDRC_DEV->BSCH          = DDRC_BA_SEL_LMT(0x2);

	DDRC_DEV->DRAM_SIZE    = DDRC_DRAM_SIZE(0x7FFFFFF);
	DDRC_DEV->DSIZE_UP_4GB = DDRC_DSIZE_8GB(0);
	DDRC_DEV->MISC         = DDRC_BST_SIZE(0x1) | DDRC_BANK_SIZE(0x2) | DDRC_PAGE_SIZE(0x3);
	DDRC_DEV->CHOP_SIZE    = DDRC_CR_CHOP_SIZE(0x3);
	DDRC_DEV->DCR          = DDRC_RMW_GRA(0x2) | DDRC_DFI_RATE(0x2) | DDRC_BIT_HALF_DQ | DDRC_DRAM_TYPE(0x3);
	DDRC_DEV->IOCR         = DDRC_TPHY_WDATA(ddr_timing->TPHY_WD) | DDRC_TPHY_WLAT(ddr_timing->TPHY_WL) | DDRC_TPHY_RDATA_EN(
								 ddr_timing->TPHY_RD) | DDRC_BIT_2N_PRE_CMD;

	DDRC_DEV->CSR              = DDRC_DPIN_IDX(0x1) | DDRC_BIT_FRQC_FREE | DDRC_BIT_BSTC_IDLE | DDRC_BIT_TM_IDLE | DDRC_BIT_MEM_IDLE | DDRC_BIT_WD_EMPTY |
								 DDRC_BIT_RC_EMPTY | DDRC_BIT_WC_EMPTY | DDRC_DPIN_CMD_INFO(0);
	DDRC_DEV->CMD_DPIN         = DDRC_BIT_ACT_N | DDRC_BIT_WE_N;
	DDRC_DEV->CMD_DPIN_NDGE    = DDRC_BIT_ACT_N_NDGE | DDRC_BIT_WE_N_NDGE;
	DDRC_DEV->CSR              = DDRC_BIT_BSTC_IDLE | DDRC_BIT_TM_IDLE | DDRC_BIT_MEM_IDLE;

	DDRC_DEV->TIE_DPIN = DDRC_BIT_CKE_1 | DDRC_BIT_RST_N_1 | DDRC_BIT_ALERT_N | DDRC_BIT_CKE | DDRC_BIT_RST_N;
	DDRC_DEV->MR_INFO  = (DDRC_WR_LAT(ddr_timing->WR_LAT) | DDRC_RD_LAT(ddr_timing->RD_LAT) | DDRC_ADD_LAT(0) | DDRC_T_DQSCK_MAX(ddr_timing->tDQSCK) | DDRC_NWR(
							  ddr_timing->nWR) | DDRC_RTP(ddr_timing->RTP));
	DDRC_DEV->DRR      = (DDRC_TRFC(ddr_timing->tRFC_tck) | DDRC_TREF(ddr_timing->tREFI_tck) | DDRC_REF_NUM(9) | DDRC_REF_DIS(0) | DDRC_ZQCL_INV(0));
	DDRC_DEV->TPR0     = (DDRC_TRP(ddr_timing->tRP_tck) | DDRC_TRAS(ddr_timing->tRAS_tck) | DDRC_TWR(ddr_timing->tWR_tck) | DDRC_TRTP(
							  ddr_timing->tRTP_tck) | DDRC_TCKE(ddr_timing->tCKE_tck) | \
						  DDRC_TPD_EN(ddr_timing->tPD_EN) | DDRC_TZQCS(ddr_timing->t_ZQCS_tck));
	DDRC_DEV->TPR1     = (DDRC_TRRD(ddr_timing->tRRD_tck) | DDRC_TRC(ddr_timing->tRC_tck) | DDRC_TRCD(ddr_timing->tRCD_tck) | DDRC_TCCD(
							  ddr_timing->tCCD_tck) | DDRC_TWTR(ddr_timing->tWTR_tck) | \
						  DDRC_TRTW(ddr_timing->tRTW_tck) | DDRC_TFAW(ddr_timing->tFAW_tck));
	DDRC_DEV->TPR2     = (DDRC_INIT_REF_NUM_ZQC_INV(ddr_timing->zqc_inv) | DDRC_TMRD_INIT_MRR(ddr_timing->tMRD_tck) | DDRC_TNS(ddr_timing->tns_tck) | DDRC_TUS(
							  ddr_timing->tus_tck) | DDRC_RST_TUS(ddr_timing->rst_tus_tck));
	DDRC_DEV->TPR3     = (DDRC_TRPAB_PB(ddr_timing->tRPAB_PB_tck) | DDRC_TRAS_MSB(ddr_timing->tRAS_msb) | DDRC_TFAW_MSB(ddr_timing->tFAW_msb) | DDRC_DPIN_CMD_LAT(
							  0) | \
						  DDRC_TRCD_MSB(ddr_timing->tRCD_msb) | DDRC_TWTR_MSB(ddr_timing->tWTR_msb) | DDRC_TRTW_MSB(ddr_timing->tTRW_msb) | DDRC_TRFC_MSB(
							  ddr_timing->tRFC_msb) | DDRC_TWR_MSB(ddr_timing->tWR_msb));
	DDRC_DEV->TPR4     = DDRC_TCKECK(ddr_timing->tCKECK);
	DDRC_DEV->TPR5     = DDRC_ODT_DLY(ddr_timing->odt_dly) | DDRC_ODT_HOLD(ddr_timing->odt_hold);
	DDRC_DEV->IMR      = DDRC_MASK_INTR_MASK;
	DDRC_DEV->CSR      = DDRC_BIT_BSTC_IDLE | DDRC_BIT_TM_IDLE | DDRC_BIT_MEM_IDLE;
}

void DDRC_FPGA_CR_UPDATE(void)
{
	DDRC_DEV->CSR = (DDRC_BIT_BSTC_IDLE | DDRC_BIT_TM_IDLE | DDRC_BIT_MEM_IDLE);
	DDRC_DEV->CCR = DDRC_BIT_INIT;
	while (((DDRC_DEV->CCR) & DDRC_BIT_INIT) != DDRC_BIT_INIT) {};

	DDRC_DEV->CSR = (DDRC_BIT_BSTC_IDLE | DDRC_BIT_TM_IDLE);
	DDRC_DEV->CCR = DDRC_BIT_CR_UPDATE;
	while ((DDRC_DEV->CSR & (DDRC_BIT_BSTC_IDLE | DDRC_BIT_TM_IDLE | DDRC_BIT_MEM_IDLE))
		   != (DDRC_BIT_BSTC_IDLE | DDRC_BIT_TM_IDLE)) {};

	DDRC_FE_DEV->CFG_CM_FE_RRB_0 = DDRC_MAX_NUM_LPR_ENTRIES_CH0(0x2f);
	DDRC_FE_DEV->CFG_CM_FE_RRB_1 = DDRC_MAX_NUM_LPR_ENTRIES_CH0(0x2f);
	DDRC_UnblockPorts();

	RTK_LOGI(TAG, "DDR FE Init Done\n");
	DelayUs(1);
}

const DDRC_OPS_TypeDef DDRC_FPGA_Ops = {
	.InitConfig       = DDRC_FPGA_InitConfig,
	.InitDRAM         = DDRC_FPGA_InitDRAM,
	.CR_UPDATE        = DDRC_FPGA_CR_UPDATE,
	.EnterSelfRefresh = NULL,
	.ExitSelfRefresh  = NULL,
	.SetZone          = DDRC_SetZone,
	.DisableZone      = DDRC_DisableZone,
};
#endif /* CONFIG_FPGA */
