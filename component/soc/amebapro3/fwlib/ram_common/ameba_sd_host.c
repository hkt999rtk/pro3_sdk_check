/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/*
 * Known issue — write_8_bit RMW hazard:
 *
 *   The LX bus does not support byte-strobe writes (STRB).  All 8-bit SD host
 *   register writes must use write_8_bit(), which does a 32-bit read-modify-
 *   write.  Direct volatile byte writes (SD_HOST_DEV->REG = val) were tested and
 *   confirmed broken on ASIC — commands are silently corrupted.
 *
 *   Consequence: registers that share a 32-bit word can be clobbered by the
 *   RMW of a neighboring byte.  Known affected groups:
 *
 *     0x520: CARD_INT_EN (byte0) + CARD_INT_PEND (byte1)
 *       - Workaround applied: use HAL_READ32/HAL_WRITE32 to write both bytes
 *         in a single 32-bit access (see SDHost_Init, SDHost_DeInit, SD_IRQHandler).
 *
 *     0x590: SD_BYTE_CNT_H (byte0) + SD_BLOCK_CNT_L (byte1) +
 *            SD_BLOCK_CNT_H (byte2) + SD_TRANSFER (byte3)
 *       - Current code writes these sequentially via write_8_bit.  Each write
 *         clobbers the other bytes, but since they are all written in order
 *         before SD_TRANSFER START is set, the final state is correct.
 *       - A 32-bit grouped write (like the CARD_INT fix) would be cleaner
 *         and was tested working, but kept as write_8_bit for consistency.
 *
 * CKGEN_CTL _CHANGE-bit gotcha (sd30 / VP-routed tuning):
 *   CRC_CLK_CHANGE (bit16) and SD30_SAMPLE_CHANGE (bit18) are clk4M override
 *   selectors, NOT "apply changes" toggles: 0 = use {crc,sd30_sample}_clk_src
 *   (the VP0/VP1 phase-shifted clock we want), 1 = override with a fixed 4 MHz.
 *   Set them while routing VP clocks and the IP gets two contradictory clock
 *   sources and wedges the LX bus — they MUST stay cleared in ExecTuningEx.
 *
 * Tuning (SDHost_ExecTuningEx): 2-D VP0xVP1 sweep at the FINAL sd30 div0
 *   100 MHz; CMD13 per phase (response-only discriminator) + CMD19/21 pattern
 *   compare + optional data-probe callback; SDHost_ForceIdle drains the FSM
 *   between/after wedged phases.
 */

#include "ameba_soc.h"
static const char *TAG = "SDH";
SD_HOST_InitTypeDef sdhost_init_para;

/**
  * @brief  Write one byte via 32-bit read-modify-write (LX bus has no byte strobes).
  * @param  addr: byte address of the SD host register.
  * @param  data: byte value to write.
  */
void write_8_bit(u32 addr, u8 data)
{
	volatile u32 *reg = (volatile u32 *)(addr & ~3);
	u32 shift = (addr & 3) << 3;

	*reg = (*reg & ~(0xFF << shift)) | ((u32)data << shift);
}

/**
  * @brief  Enable SD host clocks (LX bus, PERI_PLL source, 100M generator).
  */
void SDHost_EnableClk(void)
{
	/* PERI_PLL (378 MHz) is initialized in bootloader_clk.c */
	RCC_PeriphClockDividerSet(SYS_SD_CLK, 0, 0);
	RCC_PeriphClockSourceSet(SYS_SD_CLK, PERI_PLL);

	RCC_PeriphClockCmd(APBPeriph_NULL, APBPeriph_LX_CLOCK, ENABLE);
	RCC_PeriphClockCmd(APBPeriph_LX_BUS, APBPeriph_LXBUS_CLOCK, ENABLE);
	RCC_PeriphClockCmd(APBPeriph_NULL, APBPeriph_PBUS_SLOW_PCLOCK, ENABLE);

	RCC_PeriphClockCmd(NULL, APBPeriph_CBUS_SYS_MEM_CLOCK, ENABLE);
	RCC_PeriphClockCmd(APBPeriph_SDMMC, APBPeriph_SD_CLOCK, ENABLE);
	CORE_S_BASE->SYS_SDIO_SD_CTRL |= (CORE_S_BIT_SYS_SD_SYST_SCLK_EN | CORE_S_BIT_SYS_SD_SYST_100M_EN);
}

/**
  * @brief  Set driving strength on all SD bus pads (except CD).
  * @param  strength: PAD_DRV_ABILITITY_LOW or PAD_DRV_ABILITITY_HIGH.
  */
void SDHost_SetPadDriving(u32 strength)
{
	PAD_DrvStrength(sd_pins.clk, strength);
	PAD_DrvStrength(sd_pins.d0, strength);
	PAD_DrvStrength(sd_pins.d1, strength);
	PAD_DrvStrength(sd_pins.d2, strength);
	PAD_DrvStrength(sd_pins.d3, strength);
	PAD_DrvStrength(sd_pins.cmd, strength);
}

/**
  * @brief  Power down SD host: restore pads to 3.3V, turn off LDO.
  */
void SDHost_PowerDown(void)
{
	/* Restore host pads to 3.3V mode, high driving */
	SD_HOST_DEV->PAD_CTL |= SD_HOST_BIT_PAD_TUNE3318;
	SDHost_SetPadDriving(PAD_DRV_ABILITITY_HIGH);

	/* Turn off LDO and restore to 3.3V */
	LDO_SDIOPowerOff();
	LDO_SDIOPower_Select(SDIO_HOST_3V3);
}

/**
  * @brief  Power up SD host: select 3.3V LDO, turn on, wait 20ms for stabilization.
  */
void SDHost_PowerUp(void)
{
	LDO_SDIOPower_Select(SDIO_HOST_3V3);
	LDO_SDIOPowerOn();
	SD_DelayMs(20);
}

/**
  * @brief  Configure SD bus pinmux and pull-up on CMD/data lines.
  */
void SDHost_PinmuxInit(void)
{
	Pinmux_Config(sd_pins.clk, PINMUX_FUNCTION_SDIO_MST);
	Pinmux_Config(sd_pins.d0,  PINMUX_FUNCTION_SDIO_MST);
	Pinmux_Config(sd_pins.d1,  PINMUX_FUNCTION_SDIO_MST);
	Pinmux_Config(sd_pins.d2,  PINMUX_FUNCTION_SDIO_MST);
	Pinmux_Config(sd_pins.d3,  PINMUX_FUNCTION_SDIO_MST);
	Pinmux_Config(sd_pins.cmd, PINMUX_FUNCTION_SDIO_MST);
	Pinmux_Config(sd_pins.cd,  PINMUX_FUNCTION_SDIO_MST);

	PAD_PullCtrl(sd_pins.d0,  GPIO_PuPd_UP);
	PAD_PullCtrl(sd_pins.d1,  GPIO_PuPd_UP);
	PAD_PullCtrl(sd_pins.d2,  GPIO_PuPd_UP);
	PAD_PullCtrl(sd_pins.d3,  GPIO_PuPd_UP);
	PAD_PullCtrl(sd_pins.cmd, GPIO_PuPd_UP);
}


/**
  * @brief  Wait some time for SDIOH DMA done.
  * @param  timeout_us: timeout value in microseconds.
  * @retval SDIOH dma status:
  *           - HAL_TIMEOUT: SDIOH DMA timeout.
  *           - HAL_OK: SDIOH DMA done within a specified time.
  */
u32 SDHost_WaitDMADone(u32 timeout_us)
{
	do {
		if ((SD_HOST_DEV->SD_TRANSFER & SD_HOST_BIT_END) && (!(SD_HOST_DEV->DMA_CTL3 & SD_HOST_BIT_DMA_XFER))) {
			SDHost_DMAReset();
			return HAL_OK;
		}

		DelayUs(1);
	} while (timeout_us-- != 0);

	return HAL_TIMEOUT;
}


/**
  * @brief  Get SDIOH interrupt status.
  * @param  timeout_us: None.
  * @retval SDIOH interrupt status.
  */
u32 SDHost_GetISR(void)
{
	return SD_HOST_DEV->ISR;
}

/**
  * @brief  SDIOH interrupt configure.
  * 		BIT[SDIO_IT] == 1 && BIT[0] == 1 -> SDIO_IT en will be set 1
  * 		BIT[SDIO_IT] == 1 && BIT[0] == 0 -> SDIO_IT en will be set 0
  * 		Bit0 can only be written and cannot be read, with a default read value of 0
  * @param  SDIO_IT: SDIOH interrupt source, which can be one or combination of the following values:
  * 		@arg SDIOH_DMA_CTL_INT_EN
  * 		@arg SDIOH_CARD_ERR_INT_EN
  * 		@arg SDIOH_CARD_END_INT_EN
  * @param newState: can be ENABLE or Disable
  * @retval None.
  */
void SDHost_INTConfig(u8 SDIO_IT, u32 newState)
{
	/* Card-detect uses CARD_INT_EN/PEND (0x520/0x521) which share a
	 * 32-bit word — must be written together to avoid RMW hazard. */
	if (SDIO_IT & SD_HOST_CARD_DETECT) {
		u32 addr = (u32)&SD_HOST_DEV->CARD_INT_EN;
		u32 reg_val = HAL_READ32(addr, 0);
		if (newState == ENABLE) {
			reg_val |= (u32)SD_HOST_BIT_SD_MMC_INT_EN;
		} else {
			reg_val &= ~(u32)SD_HOST_BIT_SD_MMC_INT_EN;
		}
		reg_val |= ((u32)SD_HOST_BIT_SD_MMC_INT_PENDING << 8);  /* clear pending */
		HAL_WRITE32(addr, 0, reg_val);
	}

	/* Transfer interrupts use ISREN (0x428) */
	u8 isren_bits = SDIO_IT & ~SD_HOST_CARD_DETECT;
	if (isren_bits) {
		if (newState == ENABLE) {
			SD_HOST_DEV->ISREN |= (isren_bits | SD_HOST_BIT_WRITE_DATA_EN);
		} else {
			SD_HOST_DEV->ISREN |= isren_bits;  /* bit0 reads as 0 → disables */
		}
	}
}

/**
  * @brief  Clear SDIOH pending interrupt status.
  * @param  SDIO_IT: SDIOH interrupt pending bit, which can be one or combination of the following values:
  * 		@arg SDIOH_DMA_TRANSFER_DONE
  * 		@arg SDIOH_CARD_ERROR
  * 		@arg SDIOH_CARD_END
  * @retval None.
  */
void SDHost_INTClearPendingBit(u8 SDIO_IT)
{
	/* Card-detect pending — write via 32-bit to avoid RMW hazard */
	if (SDIO_IT & SD_HOST_CARD_DETECT) {
		u32 addr = (u32)&SD_HOST_DEV->CARD_INT_EN;
		u32 reg_val = HAL_READ32(addr, 0);
		reg_val |= ((u32)SD_HOST_BIT_SD_MMC_INT_PENDING << 8);
		HAL_WRITE32(addr, 0, reg_val);
	}

	/* Transfer interrupt pending */
	u8 isr_bits = SDIO_IT & ~SD_HOST_CARD_DETECT;
	if (isr_bits) {
		SD_HOST_DEV->ISR = isr_bits;
	}
}

/**
  * @brief  Get SDIOH bus width.
  * @param  None.
  * @retval Bus width: 0 or 1:
  *		0: 1-bit bus
  *		1: 4-bit bus
  */
u8 SDHost_GetBusWidth(void)
{
	return SD_HOST_DEV->SD_CONFIGURE1 & SD_HOST_MASK_BUS_WIDTH;
}

/**
  * @brief  Set SDIOH bus width.
  * @param  width: can be SDIOH_BUS_WIDTH_1BIT or SDIOH_BUS_WIDTH_4BIT.
  * @retval None
  */
void SDHost_SetBusWidth(u8 width)
{
	write_8_bit((u32)(&SD_HOST_DEV->SD_CONFIGURE1),
				(SD_HOST_DEV->SD_CONFIGURE1 & ~SD_HOST_MASK_BUS_WIDTH) | (width & SD_HOST_MASK_BUS_WIDTH));
}

/**
  * @brief  Check SDIOH is busy or not.
  * @param  None
  * @retval SDIOH busy status value:
  *           - HAL_BUSY: Busy
  *           - HAL_OK: Not Busy
  */
u32 SDHost_Busy(void)
{

	u8 idle_level = sdhost_init_para.SDIOH_idle_level;

	/* check the SD bus status */
	if ((SD_HOST_DEV->SD_BUS_STATUS & idle_level) == idle_level) {
		/* check the CMD & DATA state machine */
		if ((SD_HOST_DEV->SD_CMD_STATE & SD_HOST_MASK_CMD_STATE_MACHINE_IS_IDLE) && (SD_HOST_DEV->SD_DATA_STATE & SD_HOST_MASK_DATA_STATE_MACHINE_IS_IDLE)) {
			/* check the SD card module state machine */
			if ((SD_HOST_DEV->SD_TRANSFER & SD_HOST_BIT_IDLE)) {
				return HAL_OK;
			} else {
				RTK_LOGW(TAG, "SD card module state machine isn't in the idle state !\r\n");
				return HAL_BUSY;
			}
		} else {
			RTK_LOGW(TAG, "CMD or DATA state machine isn't in the idle state !!\r\n");
			return HAL_BUSY;
		}
	} else {
		RTK_LOGW(TAG, "CMD or DAT[3:0] pin isn't in the idle state !!\r\n");
		return HAL_BUSY;
	}
}


/**
  * @brief  Check if some error occurs when SDIOH transfer.
  * @param  status: pointer to word which is used to save error status.
  * @retval SDIOH status:
  *           - HAL_ERR_UNKNOWN: Some error occurs when transfer, detailed error information
  *				can be found in status parameter.
  *           - HAL_OK: No error occurs when transfer.
  */
u32 SDHost_CheckTxError(u16 *status)
{
	if (SD_HOST_DEV->SD_TRANSFER & SD_HOST_BIT_ERROR_BIT) {
		if (status != NULL) {
			*status = SD_HOST_DEV->SD_STATUS1 | ((u16)SD_HOST_DEV->SD_STATUS2 << 8);
		}
		return HAL_ERR_UNKNOWN;
	} else {
		return HAL_OK;
	}
}


/**
  * @brief  Wait some time for SDIOH tx done.
  * @param  timeout_us: timeout value in microseconds.
  * @retval SDIOH tx status value:
  *           - HAL_TIMEOUT: SDIOH tx timeout.
  *           - HAL_OK: SDIOH tx done within a specified time.
  */
u32 SDHost_WaitTxDone(u32 timeout_us)
{

	do {
		if ((SD_HOST_DEV->SD_TRANSFER & (SD_HOST_BIT_END | SD_HOST_BIT_IDLE))
			== (SD_HOST_BIT_END | SD_HOST_BIT_IDLE)) {
			return HAL_OK;
		}
		DelayUs(1);
	} while (timeout_us-- != 0);

	return HAL_TIMEOUT;
}


/**
  * @brief  Set SDIOH DMA configuration.
  * @param  dma_ctl: pointer to a SDIOH_DmaCtl structure which keep the dma parameters.
  * @retval None
  */
void SDHost_DMAConfig(SD_HOST_DMA_CTL *dma_ctl)
{
	u32 dma_ctl3 = 0;

	/* Set block length and count.
	 * SD_BYTE_CNT_H (0x590), SD_BLOCK_CNT_L (0x591), SD_BLOCK_CNT_H (0x592),
	 * SD_TRANSFER (0x593) share a 32-bit word.  Write them together to avoid
	 * write_8_bit RMW corrupting adjacent registers. */
	{
		u8 byte_cnt_l, byte_cnt_h;
		u8 blk_cnt_l = (dma_ctl->blk_cnt) & 0xFF;
		u8 blk_cnt_h = ((dma_ctl->blk_cnt) >> 8) & 0x7F;

		if ((dma_ctl->type) == DMA_Normal) {
			byte_cnt_l = 0;
			byte_cnt_h = SD_HOST_BYTE_COUNT_H(2);  /* 512 bytes */
		} else {
			byte_cnt_l = SD_HOST_BUFFER_LENGTH;     /* 64 bytes */
			byte_cnt_h = 0;
		}

		/* SD_BYTE_CNT_L is at 0x58F (byte 3 of word @ 0x58C) */
		write_8_bit((u32)(&SD_HOST_DEV->SD_BYTE_CNT_L), byte_cnt_l);

		/* Write 0x590 word: [BYTE_CNT_H, BLOCK_CNT_L, BLOCK_CNT_H, TRANSFER] */
		{
			u32 addr = (u32)&SD_HOST_DEV->SD_BYTE_CNT_H;
			u32 dma_word = HAL_READ32(addr, 0) & 0xFF000000;  /* preserve SD_TRANSFER */
			dma_word |= (u32)byte_cnt_h;
			dma_word |= (u32)blk_cnt_l << 8;
			dma_word |= (u32)blk_cnt_h << 16;
			HAL_WRITE32(addr, 0, dma_word);
		}
	}

	/* set the DMA control register */
	SD_HOST_DEV->DMA_CTL1 = (dma_ctl->start_addr) ;	/* DMA start address (unit: 8 Bytes) */
	SD_HOST_DEV->DMA_CTL2 = (dma_ctl->blk_cnt);		/* DMA transfer length (unit: 512 Bytes) */

	if ((dma_ctl->type) == SD_HOST_DMA_64B) {
		dma_ctl3 = SD_HOST_BIT_DATA64_SEL;
	} else if ((dma_ctl->type) == SD_HOST_DMA_R2) {
		dma_ctl3 = SD_HOST_BIT_RSP17_SEL;
	}

	dma_ctl3 |= SD_HOST_DDR_WR(dma_ctl->op);
	SD_HOST_DEV->DMA_CTL3 = dma_ctl3;

	/* clear pending interrupt */
	SDHost_INTClearPendingBit(SD_HOST_ISR_ALL);

	/* enable DMA transfer */
	SD_HOST_DEV->DMA_CTL3 |= SD_HOST_BIT_DMA_XFER;
}

/**
  * @brief  Reset SDIOH DMA configuration.
  * @param  None.
  * @retval None
  */
void SDHost_DMAReset(void)
{
	write_8_bit((u32)(&SD_HOST_DEV->SD_BYTE_CNT_L), 0);

	/* Clear 0x590 word: [BYTE_CNT_H, BLOCK_CNT_L, BLOCK_CNT_H, ...] */
	{
		u32 addr = (u32)&SD_HOST_DEV->SD_BYTE_CNT_H;
		HAL_WRITE32(addr, 0, HAL_READ32(addr, 0) & 0xFF000000);
	}

	SD_HOST_DEV->DMA_CTL1 = 0;
	SD_HOST_DEV->DMA_CTL2 = 0;
	SD_HOST_DEV->DMA_CTL3 = 0;
}

/**
  * @brief  Set SD_CONFIGURE2 register (CRC, timeout, response type) based on command.
  * @param  cmd: command attributes.
  */
static void SDHost_SetupConfigure2(SD_HOST_CmdTypeDef *cmd)
{
	u8 cfg2 = SD_HOST_CRC7_CALCULATION_EN(0);

	u8 is_read = (cmd->idx == SD_CMD_RdSingleBlk || cmd->idx == SD_CMD_RdMulBlk
				  || cmd->idx == SD_CMD_SendTuningBlk
				  || cmd->idx == EMMC_CMD_SendTuningBlk);
	u8 is_write = (cmd->idx == SD_CMD_WrBlk || cmd->idx == SD_CMD_WrMulBlk);

	cfg2 |= SD_HOST_CRC16_CHECK_EN(is_read ? 0 : 1);
	cfg2 |= SD_HOST_WAIT_WRITE_CRC_STATUS_TIMEOUT_EN(is_write ? 0 : 1);
	cfg2 |= SD_HOST_IGNORE_WRITE_CRC_ERROR_EN(is_write ? 0 : 1);
	cfg2 |= SD_HOST_WAIT_BUSY_END_EN((cmd->idx == SD_CMD_VolSwitch) ? 0 : 1);
	cfg2 |= SD_HOST_CRC7_CHECK_EN(cmd->rsp_crc_chk ? 0 : 1);
	cfg2 |= SD_HOST_RESPONSE_TYPE_CONFIG(cmd->rsp_type);

	write_8_bit((u32)(&SD_HOST_DEV->SD_CONFIGURE2), cfg2);
}

/**
  * @brief  Set SD_CONFIGURE3 register (busy wait, response timeout) based on command.
  * @param  cmd: command attributes.
  */
static void SDHost_SetupConfigure3(SD_HOST_CmdTypeDef *cmd)
{
	u8 cfg3 = SD_HOST_STOP_CMD_START_NO_WAIT_BUSY(0);

	u8 is_multi_or_stop = (cmd->idx == SD_CMD_RdMulBlk || cmd->idx == SD_CMD_WrMulBlk
						   || cmd->idx == SD_CMD_StopXsmission);
	u8 is_vol_or_stop = (cmd->idx == SD_CMD_VolSwitch || cmd->idx == SD_CMD_StopXsmission);

	if (is_multi_or_stop) {
		cfg3 |= SD_HOST_CMD_START_WAIT_NO_CARD_BUSY(1);
	}
	cfg3 |= SD_HOST_DATA_PHASE_WAIT_CARD_BUSY_EN(is_vol_or_stop ? 0 : 1);
	cfg3 |= SD_HOST_SD_CMD_RESPONSE_TIMEOUT_EN((cmd->rsp_type == SD_HOST_NO_RESP) ? 0 : 1);

	write_8_bit((u32)(&SD_HOST_DEV->SD_CONFIGURE3), cfg3);
}

/**
  * @brief  Write command index and argument bytes into SD_CMD[0..5] registers.
  * @param  cmd: command attributes (idx, arg).
  */
static void SDHost_FillCmdRegisters(SD_HOST_CmdTypeDef *cmd)
{
	write_8_bit((u32)(&SD_HOST_DEV->SD_CMD[0]), SD_HOST_HOST_COMMAND | (cmd->idx & 0x3F));
	write_8_bit((u32)(&SD_HOST_DEV->SD_CMD[1]), (cmd->arg >> 24) & 0xFF);
	write_8_bit((u32)(&SD_HOST_DEV->SD_CMD[2]), (cmd->arg >> 16) & 0xFF);
	write_8_bit((u32)(&SD_HOST_DEV->SD_CMD[3]), (cmd->arg >> 8) & 0xFF);
	write_8_bit((u32)(&SD_HOST_DEV->SD_CMD[4]), cmd->arg & 0xFF);
	write_8_bit((u32)(&SD_HOST_DEV->SD_CMD[5]), 0x0);
}

/**
  * @brief  Set command code in SD_TRANSFER and trigger START bit.
  * @param  cmd_code: hardware command code (e.g. SD_HOST_CMD_AutoRd1).
  */
static void SDHost_StartTransfer(u8 cmd_code)
{
	u8 xfer = (SD_HOST_DEV->SD_TRANSFER & ~SD_HOST_MASK_COMMAND_CODE) | cmd_code;
	write_8_bit((u32)&SD_HOST_DEV->SD_TRANSFER, xfer);
	write_8_bit((u32)&SD_HOST_DEV->SD_TRANSFER, xfer | SD_HOST_BIT_START);
}

/**
  * @brief  Map SD command index to hardware command code (AutoRd1/AutoWr1/etc.).
  * @param  cmd: command attributes (idx, data_present).
  * @retval  Hardware command code for SD_TRANSFER register.
  */
static u8 SDHost_GetCommandCode(SD_HOST_CmdTypeDef *cmd)
{
	switch (cmd->idx) {
	case SD_CMD_SwitchFunc:
	case SD_CMD_SendSts:
		return (cmd->data_present == SD_HOST_NO_DATA)
			   ? SD_HOST_CMD_SendCmdGetRsp : SD_HOST_CMD_NormalRd;
	case SD_CMD_SendTuningBlk:
	case EMMC_CMD_SendTuningBlk:
		return SD_HOST_CMD_Tuning;
	/* CMD8 = SD SEND_IF_COND (no data) or eMMC SEND_EXT_CSD (512 B data). */
	case SD_CMD_SendIfCond:
		return (cmd->data_present == SD_HOST_NO_DATA)
			   ? SD_HOST_CMD_SendCmdGetRsp : SD_HOST_CMD_AutoRd2;
	case SD_CMD_RdSingleBlk:
		return SD_HOST_CMD_AutoRd2;
	case SD_CMD_RdMulBlk:
		return SD_HOST_CMD_AutoRd1;
	case SD_CMD_WrBlk:
		return SD_HOST_CMD_AutoWr2;
	case SD_CMD_WrMulBlk:
		return SD_HOST_CMD_AutoWr1;
	case SD_CMD_SendScr:
		return SD_HOST_CMD_NormalRd;
	default:
		return SD_HOST_CMD_SendCmdGetRsp;
	}
}

/**
  * @brief  Send a command to SD card with auto-selected hardware command code.
  * @param  cmd_attrib: pointer to SD_HOST_CmdTypeDef with command attributes
  *         (arg, idx, rsp_type, rsp_crc_chk, data_present).
  * @param  timeout_us: timeout in microseconds. 0 = non-blocking (returns
  *         immediately after starting the transfer without waiting).
  * @retval  HAL_OK: command completed successfully.
  *          HAL_TIMEOUT: command did not complete within timeout.
  *          HAL_BUSY: SD host is busy.
  */
u32 SDHost_SendCommand(SD_HOST_CmdTypeDef *cmd_attrib, u32 timeout_us)
{
	u32 ret = SDHost_Busy();
	if (ret != HAL_OK) {
		return ret;
	}

	SDHost_SetupConfigure2(cmd_attrib);
	SDHost_SetupConfigure3(cmd_attrib);
	SDHost_FillCmdRegisters(cmd_attrib);
	SDHost_StartTransfer(SDHost_GetCommandCode(cmd_attrib));

	if (timeout_us != 0) {
		ret = SDHost_WaitTxDone(timeout_us);
	}
	return ret;
}

/**
  * @brief  Send a command with an explicit hardware command code.
  * @param  cmd_attrib: command attributes
  * @param  cmd_code: hardware command code (e.g. SD_HOST_CMD_AutoRd2)
  * @param  timeout_us: timeout (0 = non-blocking)
  */
u32 SDHost_SendCommandEx(SD_HOST_CmdTypeDef *cmd_attrib, u8 cmd_code, u32 timeout_us)
{
	u32 ret = SDHost_Busy();
	if (ret != HAL_OK) {
		return ret;
	}

	SDHost_SetupConfigure2(cmd_attrib);
	SDHost_SetupConfigure3(cmd_attrib);
	SDHost_FillCmdRegisters(cmd_attrib);
	SDHost_StartTransfer(cmd_code);

	if (timeout_us != 0) {
		ret = SDHost_WaitTxDone(timeout_us);
	}
	return ret;
}


/**
  * @brief  SDIOH get command response.
  * @param  byte_index: which byte of the response to be read
  * @retval the specified byte value of Response
  */
u8 SDHost_GetResponse(u8 byte_index)
{
	return SD_HOST_DEV->SD_CMD[byte_index];
}


/**
  * @brief  Switch speed of SDIOH.
  * @param  clk_div: can be one of the following values:
  *		 @arg 0:  divide by 1
  *		 @arg 1:  divide by 2
  *		 @arg 2:  divide by 4
  *		 @arg 3:  divide by 8
  * @param  mode: mode selection, can be one of the following values:
  *		 @arg 0:  sd20
  *		 @arg 1:  sd30
  * @retval None
  * @note AmebaD only support SD20_MODE
  */
void SDHost_SwitchSpeed(u8 clk_div, u8 mode)
{
	SD_HOST_DEV->CKGEN_CTL = (SD_HOST_DEV->CKGEN_CTL & ~SD_HOST_MASK_CLK_DIV) | SD_HOST_CLK_DIV(clk_div);
	write_8_bit((u32)&SD_HOST_DEV->SD_CONFIGURE1,
				(SD_HOST_DEV->SD_CONFIGURE1 & ~SD_HOST_MASK_MODE_SELECTION) | SD_HOST_MODE_SELECTION(mode));
}


/**
  * @brief  Set phase shift for VP0 or VP1 clock via SYS_SDIO_SD_CTRL register.
  * @param  vp_sel: 0 = VP0 (CRC/push clock), 1 = VP1 (sample clock)
  * @param  phase: 0=0deg, 1=90deg, 2=180deg, 3=270deg
  * @retval  HAL_OK or HAL_TIMEOUT if clock not ready
  */
u32 SDHost_SetPhase(u8 vp_sel, u8 phase)
{

	u32 sd_ctrl = CORE_S_BASE->SYS_SDIO_SD_CTRL;
	u32 ready_bit;
	u32 timeout = 10000;

	if (vp_sel == 0) {
		sd_ctrl &= ~CORE_S_MASK_SYS_SD_CLK1_PSYSEL;
		sd_ctrl |= CORE_S_SYS_SD_CLK1_PSYSEL(phase);
		ready_bit = CORE_S_BIT_SYS_SD_CK100_VP0_RDY;
	} else {
		sd_ctrl &= ~CORE_S_MASK_SYS_SD_CLK2_PSYSEL;
		sd_ctrl |= CORE_S_SYS_SD_CLK2_PSYSEL(phase);
		ready_bit = CORE_S_BIT_SYS_SD_CK100_VP1_RDY;
	}
	CORE_S_BASE->SYS_SDIO_SD_CTRL = sd_ctrl;

	/* Wait for clock ready */
	while (!(CORE_S_BASE->SYS_SDIO_SD_CTRL & ready_bit)) {
		DelayUs(1);
		if (--timeout == 0) {
			RTK_LOGW(TAG, "VP%d phase %d ready timeout\n", vp_sel, phase);
			return HAL_TIMEOUT;
		}
	}

	/* Reset async FIFO after clock phase change */
	{
		u8 cfg1 = SD_HOST_DEV->SD_CONFIGURE1;
		cfg1 |= SD_HOST_BIT_SD30_ASYNC_FIFO_RST_N;
		write_8_bit((u32)&SD_HOST_DEV->SD_CONFIGURE1, cfg1);
	}

	return HAL_OK;
}


/**
  * @brief  Send a tuning command (CMD19 for SD UHS, CMD21 for eMMC HS200) and
  *         check whether the tuning pattern compare passed.
  *
  *         The host's SD_HOST_CMD_Tuning opcode expects a real 64-byte DMA
  *         data path — without one configured the IP wedges waiting for
  *         a buffer. Ported from AmebaPro2 hal_sdhost.c::sdhost_send_tuning_block.
  *
  * @param  cmd_idx: SD_CMD_SendTuningBlk (19) or EMMC_CMD_SendTuningBlk (21).
  * @retval HAL_OK on pattern match, HAL_ERR_HW on mismatch / timeout.
  */
u32 SDHost_SendTuningBlockEx(u8 cmd_idx)
{
	/* 64-byte tuning-pattern landing zone. Discarded after the hardware
	 * tuning-pattern compare; no DCache sync needed (matches Pro2 comment). */
	static u8 tuning_buf[64] __attribute__((aligned(32)))
	__attribute__((section(".ddr.data")));

	u32 ret;
	SD_HOST_CmdTypeDef cmd_attr;
	SD_HOST_DMA_CTL dma_cfg;

	dma_cfg.op = DMA_Read;
	dma_cfg.start_addr = ((u32)tuning_buf) / 8;
	dma_cfg.blk_cnt = 1;
	dma_cfg.type = SD_HOST_DMA_64B;
	SDHost_DMAConfig(&dma_cfg);

	cmd_attr.arg = 0;
	cmd_attr.idx = cmd_idx;
	cmd_attr.rsp_type = SD_HOST_RSP_6B;
	cmd_attr.rsp_crc_chk = ENABLE;
	cmd_attr.data_present = SD_HOST_DATA_EXIST;

	/* Use short timeout — at a bad phase the command may not complete */
	ret = SDHost_SendCommand(&cmd_attr, 10000);

	/* Clean up the DMA registers regardless of outcome (Pro2 pattern). */
	SDHost_DMAReset();

	if (ret != HAL_OK) {
		/* Reset transfer state so the next attempt starts clean */
		u8 tmp = SD_HOST_DEV->SD_TRANSFER;
		tmp &= ~(SD_HOST_BIT_START | SD_HOST_BIT_ERROR_BIT);
		write_8_bit((u32)&SD_HOST_DEV->SD_TRANSFER, tmp);
		return ret;
	}

	/* Check if tuning pattern compare passed */
	if (SD_HOST_DEV->SD_STATUS1 & SD_HOST_BIT_SD_TUNING_PATTERN_COMPARE_ERROR) {
		return HAL_ERR_HW;
	}

	return HAL_OK;
}

/** @brief Back-compat CMD19 tuning sender. NOTE: the Ex path now drives a
  *        64-byte DMA + DMAReset, so any pending DMA config is wiped. */
u32 SDHost_SendTuningBlock(void)
{
	return SDHost_SendTuningBlockEx(SD_CMD_SendTuningBlk);
}


/**
 * @brief Unstick the SD module FSM after a wedged CMD (Pro2 CARD_STOP +
 *        SDCLK-toggle drain): abort transfer, then poll SD_BUS_STATUS for the
 *        all-high (0x1F) idle pattern.
 *
 *        Precondition: the SD host clock must be running — if it's gated the
 *        SDCLK toggle is a no-op and the bus never idles.
 *
 * @retval HAL_OK   bus reached idle within 10 ms.
 * @retval HAL_BUSY timed out (clock likely gated, or host wedged deeper).
 */
u32 SDHost_ForceIdle(void)
{
	/* (1) Stop SD module — abort transfer, force xfer FSM to idle. */
	write_8_bit((u32)&SD_HOST_DEV->CARD_STOP, SD_HOST_BIT_TARGET_MODULE_IS_SDMMC);

	/* Scrub leftover transfer bits + DMA + ISR pending. */
	{
		u8 tmp = SD_HOST_DEV->SD_TRANSFER;
		tmp &= ~(SD_HOST_BIT_START | SD_HOST_BIT_ERROR_BIT);
		write_8_bit((u32)&SD_HOST_DEV->SD_TRANSFER, tmp);
	}
	SDHost_DMAReset();
	SDHost_INTClearPendingBit(SD_HOST_ISR_ALL);

	/* (2)+(3) Toggle SDCLK and wait for bus to read idle (CMD+DAT[3:0] high). */
	SDHost_SetClkToggle(1);
	{
		const u8 bus_mask = (u8)(SD_HOST_MASK_DATA3_TO_DATA0 | SD_HOST_BIT_CMD);
		u32 retry = 1000;  /* 1000 * 10 us = 10 ms */
		u8 last_status;
		while (retry--) {
			last_status = SD_HOST_DEV->SD_BUS_STATUS;
			if ((last_status & bus_mask) == bus_mask) {
				SDHost_SetClkToggle(0);
				return HAL_OK;
			}
			DelayUs(10);
		}
		SDHost_SetClkToggle(0);
		RTK_LOGW(TAG, "SDHost_ForceIdle: bus stuck at 0x%02x (expected 0x%02x) "
				 "after 10ms — host clock may be gated\n",
				 last_status & bus_mask, bus_mask);
		return HAL_BUSY;
	}
}

/** @brief Internal helper — send CMD13 SEND_STATUS for the given RCA. */
static u32 sdhost_send_cmd13(u32 cmd13_arg)
{
	SD_HOST_CmdTypeDef cmd_attr;
	cmd_attr.arg = cmd13_arg;
	cmd_attr.idx = SD_CMD_SendSts;
	cmd_attr.rsp_type = SD_HOST_RSP_6B;
	cmd_attr.rsp_crc_chk = ENABLE;
	cmd_attr.data_present = SD_HOST_NO_DATA;
	return SDHost_SendCommand(&cmd_attr, 10000);
}

/**
  * @brief  2-D VP0xVP1 phase tuning at sd30 div0 100 MHz (see file header).
  * @param  tuning_cmd_idx: SD_CMD_SendTuningBlk (SD UHS) or EMMC_CMD_SendTuningBlk.
  * @param  cmd13_arg:      RCA << 16 for the per-phase CMD13 discriminator, or 0 to skip.
  * @param  data_test:      Optional per-phase data-path probe; pass NULL to skip.
  * @param  data_test_ctx:  Opaque pointer forwarded to data_test.
  * @retval HAL_OK if a valid phase was found, HAL_ERR_HW otherwise.
  */
u32 SDHost_ExecTuningEx(u8 tuning_cmd_idx, u32 cmd13_arg,
						u32(*data_test)(void *ctx), void *data_test_ctx)
{

	u32 pass_map[4] = {0};
	u32 best_count = 0;
	u8 best_vp0 = 0, best_vp1 = 0;
	u32 ret;

	RTK_LOGI(TAG, "Starting 2D phase tuning at 100 MHz (CMD%u%s%s)\n",
			 tuning_cmd_idx,
			 cmd13_arg  ? " +CMD13" : "",
			 data_test  ? " +data"  : "");

	/* Reset both phases to 0 */
	SDHost_SetPhase(0, 0);
	SDHost_SetPhase(1, 0);

	/* div0 100 MHz, VP0/VP1 sourced. _CHANGE bits MUST stay cleared — see
	 * the CKGEN_CTL gotcha in the file header. */
	{
		SD_HOST_DEV->CKGEN_CTL = SD_HOST_CLK_DIV(0)      /* div0: 100 MHz */
								 | SD_HOST_CRC_CLK_SRC(1)                      /* VP0 */
								 | SD_HOST_SD30_SAMPLE_CLK_SRC(2);             /* VP1 */

		u8 cfg1 = SD_HOST_DEV->SD_CONFIGURE1;
		cfg1 &= ~SD_HOST_MASK_MODE_SELECTION;
		cfg1 |= SD_HOST_MODE_SELECTION(SD_HOST_modesd30);
		cfg1 |= SD_HOST_BIT_SD30_ASYNC_FIFO_RST_N;
		write_8_bit((u32)&SD_HOST_DEV->SD_CONFIGURE1, cfg1);
	}

	/* Sweep all 4x4 VP0/VP1 phase combinations */
	for (u8 vp0 = 0; vp0 < 4; vp0++) {
		ret = SDHost_SetPhase(0, vp0);
		if (ret != HAL_OK) {
			continue;
		}

		for (u8 vp1 = 0; vp1 < 4; vp1++) {
			ret = SDHost_SetPhase(1, vp1);
			if (ret != HAL_OK) {
				continue;
			}

			/* CMD13 first so a bad phase trips on response timing rather
			 * than relying solely on the over-permissive CMD19/21 pattern
			 * compare. Skip if caller didn't supply an RCA. */
			if (cmd13_arg != 0) {
				ret = sdhost_send_cmd13(cmd13_arg);
				if (ret != HAL_OK) {
					SDHost_ForceIdle();
					continue;
				}
			}

			ret = SDHost_SendTuningBlockEx(tuning_cmd_idx);
			if (ret != HAL_OK) {
				SDHost_ForceIdle();
				continue;
			}

			/* Pro2-style data-path probe: the strict discriminator. CMD13 +
			 * tuning-pattern compare is too forgiving on this IP — we've seen
			 * phases that pass both yet fail an actual CMD25 multi-block
			 * write. When the caller supplies a data_test callback (typically
			 * write+read+compare on a scratch block), the phase only counts
			 * as passing if the data path also round-trips cleanly. */
			if (data_test != NULL) {
				if (data_test(data_test_ctx) != HAL_OK) {
					SDHost_ForceIdle();
					continue;
				}
			}

			pass_map[vp0] |= (1 << vp1);
		}
	}

	RTK_LOGI(TAG, "Phase map: [%lX %lX %lX %lX]\n",
			 pass_map[0], pass_map[1], pass_map[2], pass_map[3]);

	/* Find phase with the most neighboring passes (center of window) */
	for (u8 vp0 = 0; vp0 < 4; vp0++) {
		for (u8 vp1 = 0; vp1 < 4; vp1++) {
			if (!(pass_map[vp0] & (1 << vp1))) {
				continue;
			}
			u32 count = 0;
			for (int d0 = -1; d0 <= 1; d0++) {
				u8 p0 = (vp0 + d0 + 4) % 4;
				for (int d1 = -1; d1 <= 1; d1++) {
					u8 p1 = (vp1 + d1 + 4) % 4;
					if (pass_map[p0] & (1 << p1)) {
						count++;
					}
				}
			}
			if (count > best_count) {
				best_count = count;
				best_vp0 = vp0;
				best_vp1 = vp1;
			}
		}
	}

	if (best_count == 0) {
		RTK_LOGE(TAG, "Tuning failed: no passing phase found\n");
		SDHost_SetPhase(0, 0);
		SDHost_SetPhase(1, 0);
		return HAL_ERR_HW;
	}

	RTK_LOGI(TAG, "Best phase: VP0=%d VP1=%d (score=%lu)\n",
			 best_vp0, best_vp1, best_count);

	/* Apply best phase. Tuning already ran at div0/100 MHz so CKGEN_CTL is
	 * already where we want it; re-write to be explicit. */
	SDHost_SetPhase(0, best_vp0);
	SDHost_SetPhase(1, best_vp1);
	SD_HOST_DEV->CKGEN_CTL = SD_HOST_CLK_DIV(0)      /* div0: 100 MHz */
							 | SD_HOST_CRC_CLK_SRC(1)                      /* VP0 */
							 | SD_HOST_SD30_SAMPLE_CLK_SRC(2);             /* VP1 */

	/* Tuning's last failing iteration leaves the host CMD/DATA state machines
	 * mid-recovery. The light scrub we used to do isn't enough — Pro2 issues
	 * a CARD_STOP plus an SDCLK-toggled wait-for-bus-idle so the in-flight
	 * partial transactions drain. */
	SDHost_ForceIdle();

	/* Pro2-style final sanity check: prove CMD13 round-trips at the picked
	 * phase before declaring success. Each retry re-runs force_idle so we
	 * don't bail just because the state machines are still draining. */
	if (cmd13_arg != 0) {
		u32 final_ret = HAL_ERR_HW;
		for (u8 retry = 0; retry < 5; retry++) {
			final_ret = sdhost_send_cmd13(cmd13_arg);
			if (final_ret == HAL_OK) {
				break;
			}
			SDHost_ForceIdle();
		}
		if (final_ret != HAL_OK) {
			RTK_LOGE(TAG, "Best phase CMD13 verify FAIL %lu\n", final_ret);
			return HAL_ERR_HW;
		}
	}

	return HAL_OK;
}

/** @brief Back-compat SD-UHS tuning (CMD19, no CMD13/data discriminator —
  *        relies on the over-permissive pattern compare; prefer ExecTuningEx). */
u32 SDHost_ExecTuning(void)
{
	return SDHost_ExecTuningEx(SD_CMD_SendTuningBlk, 0, NULL, NULL);
}


/**
  * @brief  Configure SDIOH to initialization mode or not.
  * @param  NewState: can be ENABLE/DISABLE
  * @param  level: signal level, which can be @arg SDIOH_SIG_VOL_33.
  * @retval  0: SDIOH is configured successfully
  *			others: SDIOH is fail to configure.
  */
u32 SDHost_InitialModeCmd(u8 NewState, u8 Level)
{

	u32 ret = HAL_OK;

	ret = SDHost_Busy();
	if (ret != HAL_OK) {
		return ret;
	}

	if (NewState) {
		SD_HOST_DEV->CKGEN_CTL = SD_HOST_CLK_DIV(0);
		write_8_bit((u32)&SD_HOST_DEV->SD_CONFIGURE1, SD_HOST_BIT_CLK_DIVDER | SD_HOST_MODE_SELECTION(SD_HOST_modesd20) | SD_HOST_BIT_INITIAL_MODE | SD_HOST_BUS_WIDTH(
						SDIOH_BUS_WIDTH_1BIT));
	} else {
		if (Level == SD_HOST_BUS_SIGNALING_LEVEL_1p8V) {
			SD_HOST_DEV->CKGEN_CTL = SD_HOST_CLK_DIV(2);
			write_8_bit((u32)&SD_HOST_DEV->SD_CONFIGURE1, SD_HOST_MODE_SELECTION(SD_HOST_modesd30) | SD_HOST_BIT_SD30_ASYNC_FIFO_RST_N | SD_HOST_BUS_WIDTH(
							SDIOH_BUS_WIDTH_1BIT));
		} else {
			SD_HOST_DEV->CKGEN_CTL = SD_HOST_CLK_DIV(2);
			write_8_bit((u32)&SD_HOST_DEV->SD_CONFIGURE1, SD_HOST_MODE_SELECTION(SD_HOST_modesd20) | SD_HOST_BUS_WIDTH(SDIOH_BUS_WIDTH_1BIT));
		}
	}

	return ret;
}


/**
  * @brief  Initialize SDIOH peripheral.
  * @param  None
  * @retval  0: SDIOH is initialized successfully
  *		   others: SDIOH is fail to initialize
  */
u32 SDHost_Init(u8 BusBitMode)
{
	u32 ret = HAL_OK;

	/* Set pin idle level */
	if (BusBitMode == SDIOH_BUS_WIDTH_4BIT) {
		sdhost_init_para.SDIOH_idle_level = 0x1F;
	} else {
		sdhost_init_para.SDIOH_idle_level = 0x03;
	}

	SDHost_EnableClk();
	SDHost_PinmuxInit();

	/* Enable direct access mode */
	SD_HOST_DEV->SRAM_CTL = SD_HOST_LX_BURST_SIZE(0x0) | SD_HOST_MAP_SEL(0x1) | SD_HOST_ACCESS_EN(0x1);

	SDHost_INTConfig(SD_HOST_CARD_DETECT, ENABLE);
	write_8_bit((u32)&SD_HOST_DEV->CARD_SELECT, SD_HOST_CARD_SELECT(0x2));
	write_8_bit((u32)&SD_HOST_DEV->CARD_STOP, SD_HOST_BIT_TARGET_MODULE_IS_SDMMC);

	/* Initial mode */
	ret = SDHost_InitialModeCmd(ENABLE, 0);
	if (ret != HAL_OK) {
		return ret;
	}

	write_8_bit((u32)&SD_HOST_DEV->CARD_CLOCK_EN_CTL, SD_HOST_BIT_SD_CARD_MODULE_CLK_EN);

	return ret;
}

/**
  * @brief  Check SDIOH bus line signal level (CMD + DAT[3:0]).
  * @param  status: expected level. 0: all lines low, non-zero: all lines high (idle).
  * @param  timeout_us: timeout value in microseconds.
  * @retval  HAL_OK: Bus is at the expected level.
  *          HAL_TIMEOUT: Bus did not reach the expected level in time.
  */
u32 SDHost_CheckBusState(u8 status, u32 timeout_us)
{

	u8 bus_mask = (u8)(SD_HOST_MASK_DATA3_TO_DATA0 | SD_HOST_BIT_CMD); /* 0x1F */
	u8 expect = status ? bus_mask : 0;

	do {
		if ((SD_HOST_DEV->SD_BUS_STATUS & bus_mask) == expect) {
			return HAL_OK;
		}
		DelayUs(1);
	} while (timeout_us-- != 0);

	RTK_LOGW(TAG, "CheckBusState timeout (expect %s, got 0x%02X)\n",
			 status ? "HIGH" : "LOW", SD_HOST_DEV->SD_BUS_STATUS & bus_mask);
	return HAL_TIMEOUT;
}

/**
  * @brief  De-initialize SDIOH peripheral.
  * @param  None
  * @retval  None
  */
void SDHost_DeInit(void)
{
	SDHost_INTConfig(SD_HOST_CARD_DETECT | SD_HOST_ISR_ALL, DISABLE);
	SDHost_INTClearPendingBit(SD_HOST_ISR_ALL);

	/* Disable SD host clocks */
	CORE_S_BASE->SYS_SDIO_SD_CTRL &= ~(CORE_S_BIT_SYS_SD_SYST_SCLK_EN | CORE_S_BIT_SYS_SD_SYST_100M_EN);
	RCC_PeriphClockCmd(APBPeriph_SDMMC, APBPeriph_SD_CLOCK, DISABLE);
}

/**
  * @brief  Clear the SD_TRANSFER error flag.
  */
void SDHost_ClearErrorBit(void)
{
	write_8_bit((u32)&SD_HOST_DEV->SD_TRANSFER, SD_HOST_DEV->SD_TRANSFER & ~SD_HOST_BIT_ERROR_BIT);
}

/**
  * @brief  Check if SD card is physically present.
  * @retval  1 if card present, 0 if absent.
  */
u8 SDHost_CardExist(void)
{
	return (SD_HOST_DEV->CARD_EXIST & SD_HOST_BIT_SD_EXISTENCE) ? 1 : 0;
}

/**
  * @brief  Check if SD transfer and DMA are both complete (polled, non-blocking).
  * @retval  1 if done, 0 if still in progress.
  */
u8 SDHost_CheckTransferDone(void)
{
	return ((SD_HOST_DEV->SD_TRANSFER & SD_HOST_BIT_END) &&
			(!(SD_HOST_DEV->DMA_CTL3 & SD_HOST_BIT_DMA_XFER))) ? 1 : 0;
}

/**
  * @brief  Check if SD_TRANSFER error flag is set.
  * @retval  1 if error, 0 if no error.
  */
u8 SDHost_HasError(void)
{
	return (SD_HOST_DEV->SD_TRANSFER & SD_HOST_BIT_ERROR_BIT) ? 1 : 0;
}

/**
  * @brief  Get SD_STATUS1 and SD_STATUS2 combined as 16-bit value.
  * @retval  status1 | (status2 << 8).
  */
u16 SDHost_GetStatus(void)
{
	return SD_HOST_DEV->SD_STATUS1 | ((u16)SD_HOST_DEV->SD_STATUS2 << 8);
}

/**
  * @brief  Enable or disable continuous SD clock toggling.
  * @param  enable: 1 = enable clock toggle, 0 = disable.
  */
void SDHost_SetClkToggle(u8 enable)
{
	u8 tmp = SD_HOST_DEV->SD_BUS_STATUS;
	if (enable) {
		tmp |= SD_HOST_BIT_SD_CLK_TOGGLE_EN;
		tmp &= ~SD_HOST_BIT_STOP_SD_CLK_TOGGLING_WHEN_NO_CMD_DATA_TRANSFER;
	} else {
		tmp &= ~SD_HOST_BIT_SD_CLK_TOGGLE_EN;
		tmp |= SD_HOST_BIT_STOP_SD_CLK_TOGGLING_WHEN_NO_CMD_DATA_TRANSFER;
	}
	write_8_bit((u32)&SD_HOST_DEV->SD_BUS_STATUS, tmp);
}

/**
  * @brief  Set SD host pad voltage mode.
  * @param  level: SD_HOST_BUS_SIGNALING_LEVEL_3p3V or SD_HOST_BUS_SIGNALING_LEVEL_1p8V.
  */
void SDHost_SetPadVoltage(u8 level)
{
	if (level == SD_HOST_BUS_SIGNALING_LEVEL_1p8V) {
		SD_HOST_DEV->PAD_CTL &= ~SD_HOST_BIT_PAD_TUNE3318;
	} else {
		SD_HOST_DEV->PAD_CTL |= SD_HOST_BIT_PAD_TUNE3318;
	}
}

/**
  * @brief  Issue card stop to the SD card module.
  */
void SDHost_CardStop(void)
{
	write_8_bit((u32)&SD_HOST_DEV->CARD_STOP, SD_HOST_BIT_TARGET_MODULE_IS_SDMMC);
}

/**
  * @brief  Set DDR50 sample point register.
  * @param  val: sample point value.
  */
void SDHost_SetSamplePoint(u8 val)
{
	write_8_bit((u32)&SD_HOST_DEV->SD_SAMPLE_POINT_CTL, val);
}
