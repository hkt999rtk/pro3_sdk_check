/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "ameba_soc.h"
#include "ameba_usrcfg.h"

#define FLASH_CALIBRATION_DEBUG		0

static const char *const TAG = "FLASH";
static FlashInfo_TypeDef *current_IC;

/* Flag to check configuration register or not. Necessary for wide-range VCC MXIC flash */
static u8 check_config_reg = 0;

// FEATURE_TODO : CHECK this
__weak u32 valid_img1_addr = 0 + SPI_FLASH_BASE;

static void nand_get_img1_good_block_addr(void)
{
	u32 NandAddr = valid_img1_addr - SPI_FLASH_BASE;
	u32 PageAddr = NAND_ADDR_TO_PAGE_ADDR(NandAddr);
	u32 BlockId = NAND_PAGE_ADDR_TO_BLOCK_ID(PageAddr);

	while (NAND_CheckBadBlock(BlockId)) {
		/*jump to next block when current block is bad block*/
		BlockId = BlockId + 1;
		valid_img1_addr += NAND_BLOCK_PAGE_CNT * NAND_PAGE_SIZE_MAIN;
	}
}

SRAMDRAM_ONLY_TEXT_SECTION
static void FLASH_PLLInit_ClockDiv(void)
{
#ifdef FEATURE_TODO
	u8 spic_ckd;
	u32 usb_pll_clk = RRAM_DEV->clk_info_bk.USBPLL_CLK;
	u32 sys_pll_clk = RRAM_DEV->clk_info_bk.SYSPLL_CLK;

	/* set spic clock src and div */
	spic_ckd = PLL_ClkSrcGet(sys_pll_clk, usb_pll_clk, CLK_LIMIT_SPIC);
	if (spic_ckd & IS_SYS_PLL) {
		RCC_PeriphClockDividerSet(SYS_PLL_SPIC, GET_CLK_DIV(spic_ckd));
		RCC_PeriphClockSourceSet(SPIC, SYS_PLL);
	} else {
		RCC_PeriphClockDividerSet(USB_PLL_SPIC, GET_CLK_DIV(spic_ckd));
		RCC_PeriphClockSourceSet(SPIC, USB_PLL);
	}

	/* save spic clock para into retention memory */
	IPC_SEMTake(IPC_SEM_RRAM, 0xffffffff);
	RRAM_DEV->clk_info_bk.spic_ckd = spic_ckd;
	IPC_SEMFree(IPC_SEM_RRAM);
#endif
}

SRAMDRAM_ONLY_TEXT_SECTION
u32 flash_handshake_highspeed(void)
{

	u8 Dphy_Dly_Cnt = 3; /* DD recommend this value */
	u32 Ret = TRUE;

	FLASH_PLLInit_ClockDiv();

#ifdef FEATURE_TODO
	/* Set CSsetuptime to 2 SPIC Clk if SPIC > 200MHz in order to great than tSLCH(5ns).
	   Note: Full voltage range Flash e.g.GD25WQ64E need 10ns */
	if (SPIC_ClkGet() > SPIC_CS_SETUP_TIME_LMT) {
		SPIC->TPR1 = (SPIC->TPR1 & ~MASK_CS_ACTIVE_SETUP) | CS_ACTIVE_SETUP(2);
	}

	/* Open handshake function */
	FLASH_Read_HandShake_Cmd(Dphy_Dly_Cnt, ENABLE);

	if (FLASH_Read_DataIsRight(valid_img1_addr)) {
	} else {
		/* should disable it, enbale it outside if needed */
		FLASH_Read_HandShake_Cmd(Dphy_Dly_Cnt, DISABLE);

		if (SYSCFG_CHIPType_Get() != CHIP_TYPE_FPGA) {
			RCC_PeriphClockSourceSet(SPIC, XTAL);
		}
		SPIC->TPR1 = (SPIC->TPR1 & ~MASK_CS_ACTIVE_SETUP) | CS_ACTIVE_SETUP(1);
		Ret = FALSE;
	}

	RTK_LOGI(TAG, "FLASH HandShake[0x%x %s]\n", div, Ret ? "OK" : "FAIL");
	return Ret;
#else
	(void)Dphy_Dly_Cnt;
	return Ret;
#endif
}

void flash_get_vendor(void)
{
	u8 flash_ID[4];
	u32 flash_capacity = 0;
	FLASH_InitTypeDef *FLASH_InitStruct = &flash_init_para;

	/* Read flash ID */
	FLASH_RxCmd(FLASH_InitStruct->FLASH_cmd_rd_id, 3, flash_ID);
	/* Byte -> Mbits: 10 + 10 - 3 = 17 (0x11) */
	flash_capacity = (1 << (flash_ID[2] - 0x11));
	RTK_LOGI(TAG, "Flash ID: %x-%x-%x (Capacity: %dM-bit)\n", flash_ID[0], flash_ID[1], flash_ID[2], flash_capacity);

	/* Get flash chip information */
	current_IC = flash_get_chip_info((flash_ID[2] << 16) | (flash_ID[1] << 8) | flash_ID[0]);
	if (current_IC == NULL) {
		RTK_LOGW(TAG, "This flash type is not supported!\n");
		assert_param(0);
	}

	/* Re-initialize flash init structure according to classification */
	switch (current_IC->flash_class) {
	case FlashClass1:
		FLASH_StructInit(FLASH_InitStruct);
		break;
	case FlashClass2:
		FLASH_StructInit_GD(FLASH_InitStruct);
		if (flash_ID[2] > 0x15) { /* GD capacity more than 2MB */
			FLASH_InitStruct->FLASH_cmd_wr_status2 = 0x31;
		}
		break;
	case FlashClass3:
		FLASH_StructInit_MXIC(FLASH_InitStruct);
		break;
	case FlashClass4:	/* EON without QE bit */
		FLASH_StructInit_MXIC(FLASH_InitStruct);
		FLASH_InitStruct->FLASH_QuadEn_bit = 0;
		break;
	case FlashClass5:
		FLASH_StructInit_Micron(FLASH_InitStruct);
		break;
	case FlashClass6:	/* MXIC wide-range VCC chip */
		FLASH_StructInit_MXIC(FLASH_InitStruct);
		check_config_reg = 1;
		break;
	case FlashClassUser:
		assert_param(current_IC->FlashInitHandler != NULL);
		current_IC->FlashInitHandler();
		break;
	default:
		break;
	}

	if (SYSCFG_OTP_SPICAddr4ByteEn()) {
		FLASH_InitStruct->FLASH_addr_phase_len = ADDR_4_BYTE;
	}

}

static void flash_set_status_register(void)
{
	u8 StatusLen = 1;
	u32 data = 0;
	u32 status = 0;
	u32 mask = current_IC->sta_mask;
	FLASH_InitTypeDef *FLASH_InitStruct = &flash_init_para;

	if (FLASH_InitStruct->FLASH_QuadEn_bit != 0) {
		data |= FLASH_InitStruct->FLASH_QuadEn_bit;
	}

	/* read status1 register */
	FLASH_RxCmd(FLASH_InitStruct->FLASH_cmd_rd_status, 1, (u8 *)&status);

	/* check if status2 exist */
	if (FLASH_InitStruct->FLASH_Status2_exist) {
		StatusLen = 2;
		FLASH_RxCmd(FLASH_InitStruct->FLASH_cmd_rd_status2, 1, ((u8 *)&status) + 1);

	} else if (check_config_reg) {	/* for MXIC wide-range flash, 1 status register + 2 config register */
		/* Read configuration register */
		FLASH_RxCmd(0x15, 2, ((u8 *)&status) + 1);
		StatusLen = 3;

		/* L/H Switch */
		data |= (BIT(9) << 8);
	}

	status &= mask;
	if (_memcmp((void *)&status, (void *)&data, StatusLen)) {
		if (!FLASH_InitStruct->FLASH_cmd_wr_status2) {
			FLASH_SetStatus(FLASH_InitStruct->FLASH_cmd_wr_status, StatusLen, (u8 *)&data);
		} else {
			FLASH_SetStatus(FLASH_InitStruct->FLASH_cmd_wr_status, 1, (u8 *)&data);
			FLASH_SetStatus(FLASH_InitStruct->FLASH_cmd_wr_status2, 1, ((u8 *)&data) + 1);
		}
		RTK_LOGI(TAG, "Flash status register changed:0x%x -> 0x%x\n", status, data);
	}
}

/**
  * @brief  read flash data to compare with golden data
  * @param  None
  * @retval 1: read flash data is right
  *			0: read flash data is wrong
  * @retval None
  */
SRAMDRAM_ONLY_TEXT_SECTION
u32 FLASH_Read_DataIsRight(u32 pattern_addr)
{
	FLASH_InitTypeDef *FLASH_InitStruct = &flash_init_para;
	u32 pgolden_data[2];
	u32 LoaderLogAddr = pattern_addr;
	u32 SPIC_CALIB_PATTERN[2] = {0x96969999, 0xFC66CC3F,};

	DCache_Invalidate(LoaderLogAddr, 8);
	pgolden_data[0] = HAL_READ32(LoaderLogAddr, 0x00);
	pgolden_data[1] = HAL_READ32(LoaderLogAddr, 0x04);

	if (FLASH_InitStruct->debug) {
		RTK_LOGI(TAG, "FLASH_Read 0x%x:0x%x\n", pgolden_data[0], pgolden_data[1]);
	}

	/* compare data */
	if (_memcmp(pgolden_data, SPIC_CALIB_PATTERN, 8) == 0) {
		return TRUE;
	} else {
		return FALSE;
	}
}

SRAMDRAM_ONLY_TEXT_SECTION
int flash_rx_mode_switch(u32 spic_mode)
{
	int Ret = SUCCESS;
	u8 status = 0;
	char *str[] = {"1IO", "2O", "2IO", "4O", "4IO"};
	FLASH_InitTypeDef *FLASH_InitStruct = &flash_init_para;

	/* Try sequentially: 4IO, 4O, 2IO, 2O, 1bit */
	while (1) {
		if (SYSCFG_OTP_BootFromNor()) {
			if (FLASH_InitStruct->FLASH_Id == FLASH_ID_MICRON) {
				FLASH_RxCmd(0x85, 1, &status);
				status = (status & 0x0f) | (FLASH_InitStruct->FLASH_rd_dummy_cycle[spic_mode] << 4);
				FLASH_SetStatus(0x81, 1, &status);
			}

			FLASH_Init(spic_mode);
		} else {
			NAND_Init(spic_mode);
		}

		if (spic_mode == Spic1IOBitMode) {
			RTK_LOGE(TAG, "Flash Switch Read Mode FAIL\n");
			Ret = FAIL;
			break;
		}

		if (FLASH_Read_DataIsRight(valid_img1_addr)) {
			RTK_LOGI(TAG, "Flash Read %s\n", str[spic_mode]);
			break;
		}

		spic_mode--;
	}

	return Ret;
}

void nand_get_vendor(void)
{
	u8 flash_ID[4];

	/* Read flash ID */
	NAND_RxCmd(flash_init_para.FLASH_cmd_rd_id, 0, NULL, 2, flash_ID);
	RTK_LOGI(TAG, "NAND ID: %x-%x\n", flash_ID[0], flash_ID[1]);

	/* Get flash chip information */
	current_IC = flash_get_chip_info((flash_ID[2] << 16) | (flash_ID[1] << 8) | flash_ID[0]);
	if (current_IC == NULL) {
		RTK_LOGW(TAG, "This flash type is not supported!\n");
		assert_param(0);
	}

	/* Re-initialize flash init structure according to classification */
	switch (current_IC->flash_class) {
	case FlashClass1:
		NAND_StructInit(&flash_init_para);
		break;
	case FlashClass2:
		NAND_StructInit_GD(&flash_init_para);
		break;
	case FlashClass3:
		NAND_StructInit_MXIC(&flash_init_para);
		break;
	case FlashClass5:
		NAND_StructInit_Micron(&flash_init_para);
		break;
	case FlashClassUser:
		assert_param(current_IC->FlashInitHandler != NULL);
		current_IC->FlashInitHandler();
		break;
	default:
		break;
	}
}

SRAMDRAM_ONLY_TEXT_SECTION
void flash_highspeed_setup(void)
{
	u32 read_mode;
	u8 nand_reg;
	read_mode = flash_get_readmode(Flash_ReadMode);

	__disable_irq();

	/* SPIC stay in BUSY state when there are more than 0x1_0000 cycles between two input data.
	 * Disable DREIR to avoid that interrupt hanler time is lager than 0x1_0000 SPIC cycles.
	 */
	SPIC->CTRLR0 |= BIT_SPI_DREIR_R_DIS;

	if (SYSCFG_OTP_BootFromNor()) {
		/* Get flash ID to reinitialize FLASH_InitTypeDef structure */
		flash_get_vendor();

		/* Set flash status register: set QE, clear protection bits */
		flash_set_status_register();
	} else {
		/* get good block addr form valid_img1_addr */
		nand_get_img1_good_block_addr();
		/* Get Nand ID and Set QE */
		nand_get_vendor();
		/* QuadEnable */
		if (flash_init_para.FLASH_QuadEn_bit != 0) {
			nand_reg = NAND_GetStatus(NAND_REG_CFG);
			if (nand_reg != (nand_reg | NAND_CFG_QUAD_ENABLE)) {
				NAND_SetStatus(NAND_REG_CFG, nand_reg | NAND_CFG_QUAD_ENABLE);
			}
		}

		/*After power-up, the chip is in protection state, set feature bits BPx to 0 to unlock all block. */
		nand_reg = NAND_GetStatus(NAND_REG_BLOCK_LOCK);
		if (nand_reg != NAND_BL_ALL_UNLOCKED) {
			NAND_SetStatus(NAND_REG_BLOCK_LOCK, NAND_BL_ALL_UNLOCKED);
			RTK_LOGI(TAG, "Flash all block unlocked. Feature reg 0x%02x set to 0x%02x\n", NAND_REG_BLOCK_LOCK, NAND_BL_ALL_UNLOCKED);
		}
	}

	/* Set flash I/O mode and high-speed calibration */
	flash_rx_mode_switch(read_mode);

#ifdef FEATURE_TODO
	/* Two SPIC in Dplus, SPIC0(Boot) Can connect to S0 or S1, when SPIC0 select one, SPIC1(Combo) use another one */
	u32 Temp = HAL_READ32(SYSTEM_CTRL_BASE, REG_LSYS_PSRAMC_FLASH_CTRL);
	if (PINMUX_S0 == flash_init_para.FLASH_pinmux) {
		HAL_WRITE32(SYSTEM_CTRL_BASE, REG_LSYS_PSRAMC_FLASH_CTRL, Temp & ~LSYS_BIT_FLASH_FEEDBACK_LOC);
	} else {
		HAL_WRITE32(SYSTEM_CTRL_BASE, REG_LSYS_PSRAMC_FLASH_CTRL, Temp | LSYS_BIT_FLASH_FEEDBACK_LOC);
	}
#endif

	if (SYSCFG_CHIPType_Get() != CHIP_TYPE_FPGA) {
		flash_handshake_highspeed();
	}

	__enable_irq();
}

#ifdef NOT_IMPLEMENT
/* init psramc for flash r/w if needed */
void Combo_SPIC_Init(void)
{
	SPIC_TypeDef *ComboSpic = SPIC_COMBO;
	FLASH_InitTypeDef *FLASH_InitStruct = &flash_init_para;

#if !defined(CONFIG_SECOND_FLASH_NOR) && !defined(CONFIG_SECOND_FLASH_NAND)
	return;
#endif

	RCC_PeriphClockCmd(APBPeriph_PSRAM, APBPeriph_PSRAM_CLOCK, ENABLE);
	RCC_PeriphClockSourceSet(PSRAM, XTAL);

	/*1. Disable psram phy */
	HAL_WRITE32(SYSTEM_CTRL_BASE, REG_LSYS_PSRAMC_FLASH_CTRL, HAL_READ32(SYSTEM_CTRL_BASE, REG_LSYS_PSRAMC_FLASH_CTRL) | LSYS_BIT_PSRAMC_FLASH_EN);

	/*2. ComboSpic pinmux set which is contrary to bootSpic */
	if (PINMUX_S0 == FLASH_InitStruct->FLASH_pinmux) {
		Pinmux_ComboSpicCtrl(PINMUX_S1, ON);
	} else {
		Pinmux_ComboSpicCtrl(PINMUX_S0, ON);
	}

	/*3. use bootSpic para to config ComboSpic(Default NorFlash Config) where 1IO autoread is common for all flash */
#if defined(CONFIG_SECOND_FLASH_NOR)
	FLASH_Config(ComboSpic, FLASH_InitStruct);
	/* Set ComboSpic 1IO Mode */
	FLASH_SetSpiMode(ComboSpic, FLASH_InitStruct, Spic1IOBitMode);
#elif defined(CONFIG_SECOND_FLASH_NAND)
	FLASH_InitTypeDef nand_temp_flash_init_para;
	NAND_StructInit(&nand_temp_flash_init_para);
	nand_temp_flash_init_para.FLASH_cur_bitmode = Spic1IOBitMode;

	SPIC_Config(ComboSpic, &nand_temp_flash_init_para);
	NAND_SetSpiMode(ComboSpic, &nand_temp_flash_init_para, Spic1IOBitMode);
#endif
	/* SPIC stay in BUSY state when there are more than 0x1_0000 cycles between two input data.
	 * Disable DREIR to avoid that interrupt hanler time is lager than 0x1_0000 SPIC cycles.
	 */
	ComboSpic->CTRLR0 |= BIT_SPI_DREIR_R_DIS;
}
#endif
