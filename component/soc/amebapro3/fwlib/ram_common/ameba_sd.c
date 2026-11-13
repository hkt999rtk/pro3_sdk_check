/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/*
 * Top-level SD driver — owns central state and combines:
 *   ameba_sd_host.c  — SD host controller hardware
 *   ameba_sd_card.c  — SD card protocol layer
 */

#include "ameba_soc.h"
#include "ameba_sd.h"
#include "os_wrapper.h"

static const char *TAG = "SD";

SRAM_NOCACHE_DATA_SECTION
SD_Adapter sd_adapter;

const SD_PinConfig sd_pins = {
	.clk = _PS_0,
	.d0  = _PS_2,
	.d1  = _PS_1,
	.d2  = _PS_6,
	.d3  = _PS_5,
	.cmd = _PS_3,
	.cd  = _PS_4,
};

/* SD host controller ops instance — maps vtable to current host IP functions */
const SD_HOST_OPS_TypeDef SDHost_Ops = {
	.SendCommand       = SDHost_SendCommand,
	.GetResponse       = SDHost_GetResponse,
	.CheckTxError      = SDHost_CheckTxError,
	.DMAConfig         = SDHost_DMAConfig,
	.WaitDMADone       = SDHost_WaitDMADone,
	.DMAReset          = SDHost_DMAReset,
	.CheckTransferDone = SDHost_CheckTransferDone,
	.HasError          = SDHost_HasError,
	.ClearErrorBit     = SDHost_ClearErrorBit,
	.SetBusWidth       = SDHost_SetBusWidth,
	.GetBusWidth       = SDHost_GetBusWidth,
	.INTConfig         = SDHost_INTConfig,
	.INTClearPendingBit = SDHost_INTClearPendingBit,
	.CardExist         = SDHost_CardExist,
	.CardStop          = SDHost_CardStop,
};

/**
  * @brief  Force 3.3V signaling — call before SD_Init() to skip 1.8V negotiation.
  * @param  enable: 1 = force 3.3V, 0 = allow 1.8V negotiation.
  */
void SD_Force33V(u8 enable)
{
	sd_adapter.force_33v = enable;
}

/* Forward declarations for internal functions */
static void SD_CardInit(void);

/**
  * @brief  Reset the mutable portion of SD_CardInfo on insert/remove.
  *         Preserves user-registered callback hooks and the wait_interrupts flag
  *         (the latter is managed by SD_CardDetectDeferred for in-flight I/O).
  */
static void SD_ResetCardInfo(void)
{
	_memset(sd_adapter.card_info.csd, 0, sizeof(sd_adapter.card_info.csd));
	sd_adapter.card_info.rca = 0;
	sd_adapter.card_info.is_sdhc_sdxc = 0;
	sd_adapter.card_info.sd_spec_ver = 0;
	sd_adapter.card_info.capacity = 0;
	sd_adapter.card_info.read_bl_len = 0;
	sd_adapter.card_info.write_bl_len = 0;
	sd_adapter.card_info.sig_level = SD_HOST_BUS_SIGNALING_LEVEL_3p3V;
	sd_adapter.card_info.bus_spd = SD_SPEED_DS;
	sd_adapter.card_info.card_type = CARD_TYPE_NONE;
	sd_adapter.card_info.serial = 0;
	sd_adapter.card_info.speed_class = 0;
	sd_adapter.card_info.uhs_grade = 0;
	sd_adapter.card_info.video_class = 0;
	sd_adapter.card_info.app_class = 0;
}

/**
  * @brief  Millisecond delay used by the SD driver (weak).
  * @param  ms: milliseconds to wait.
  */
__attribute__((weak)) void SD_DelayMs(u32 ms)
{
	rtos_time_delay_ms(ms);
}

/**
  * @brief  Default card-insert ISR callback (weak). Runs from the card-detect
  *         timer task context (not ISR), so blocking calls are safe.
  *
  *         A freshly inserted card always powers up at 3.3V / default-speed,
  *         regardless of what the previous card negotiated. We power-cycle the
  *         host and card VDD and wipe the cached card state before re-running
  *         the identification sequence.
  */
__attribute__((weak)) void SD_CardInsertISR(void)
{
	RTK_LOGI(TAG, "Card Insert\n");

	InterruptDis(SDMMC_IRQ);

	SDCard_PowerEn(0);
	SDHost_PowerDown();
	SD_DelayMs(20);

	SD_ResetCardInfo();

	SDHost_PowerUp();
	SDCard_PowerEn(1);
	SD_DelayMs(100);

	SDHost_Init(sdioh_config.sdioh_bus_width);
	SD_CardInit();

	InterruptEn(SDMMC_IRQ, INT_PRI_MIDDLE);
}

/**
  * @brief  Default card-remove ISR callback (weak). Runs from the card-detect
  *         timer task context. Any pending transfer is aborted by
  *         SD_CardDetectDeferred after this returns, so here we just tear the
  *         host down and invalidate the cached card state.
  *
  *         sd_status has already been set to SD_NODISK by SD_CardDetectDeferred.
  */
__attribute__((weak)) void SD_CardRemoveISR(void)
{
	RTK_LOGI(TAG, "Card Remove\n");

	SDHost_INTConfig(SD_HOST_DMA_TRANSFER_DONE | SD_HOST_CARD_END | SD_HOST_CARD_ERROR, DISABLE);
	SDHost_INTClearPendingBit(SD_HOST_ISR_ALL);

	SDCard_PowerEn(0);
	SDHost_PowerDown();

	SD_ResetCardInfo();
}

/**
  * @brief  Default transfer-done ISR callback (weak).
  */
__attribute__((weak)) void SD_TransferDoneISR(void)
{
	RTK_LOGD(TAG, "Transfer Done\n");
}

/**
  * @brief  Default transfer-yield-done ISR callback (weak).
  */
__attribute__((weak)) void SD_TransferYieldDoneISR(void)
{
	RTK_LOGD(TAG, "Task Yield Done\n");
}

/**
  * @brief  Register a custom card-insert callback (NULL restores default).
  * @param  cb: callback function, or NULL for default SD_CardInsertISR.
  */
void SD_card_insert_hook(void (*cb)(void))
{
	sd_adapter.card_info.card_insert_cb = cb ? cb : SD_CardInsertISR;
}

/**
  * @brief  Register a custom card-remove callback (NULL restores default).
  * @param  cb: callback function, or NULL for default SD_CardRemoveISR.
  */
void SD_card_remove_hook(void (*cb)(void))
{
	sd_adapter.card_info.card_remove_cb = cb ? cb : SD_CardRemoveISR;
}

/**
  * @brief  Register a custom transfer-done callback (NULL restores default).
  * @param  cb: callback function, or NULL for default SD_TransferDoneISR.
  */
void SD_transfer_done_hook(void (*cb)(void))
{
	sd_adapter.card_info.transfer_done_cb = cb ? cb : SD_TransferDoneISR;
}

/**
  * @brief  Register a custom transfer-yield callback (NULL restores default).
  * @param  cb: callback function, or NULL for default SD_TransferYieldDoneISR.
  */
void SD_transfer_task_yield_hook(void (*cb)(void))
{
	sd_adapter.card_info.transfer_task_yield_cb = cb ? cb : SD_TransferYieldDoneISR;
}

/**
  * @brief  Set semaphore take/give functions for ISR-driven transfers.
  * @param  sema_take_fn: blocking semaphore take (called from task context).
  * @param  sema_give_isr_fn: semaphore give (called from ISR context).
  */
void SD_SetSema(int (*sema_take_fn)(u32), int (*sema_give_isr_fn)(u32))
{
	sd_adapter.sema_take = sema_take_fn;
	sd_adapter.sema_give_isr = sema_give_isr_fn;
}

/* User card-detect callback, fired from the debounced card-detect handler
 * with the new SD_RESULT status on both insert and remove. */
static void (*cd_cb)(SD_RESULT) = NULL;

/**
  * @brief  Register a user card-detect callback for insert/remove events.
  * @param  cd_callback: callback receiving the new SD_RESULT status, or NULL to clear.
  * @note   Invoked from the card-detect timer task context (not ISR), so blocking
  *         calls are safe. Used e.g. by the fatfs SD hotplug support.
  */
void SD_SetCdCallback(void (*cd_callback)(SD_RESULT))
{
	cd_cb = cd_callback;
}

static rtos_timer_t sd_cd_timer;

/**
  * @brief  Debounce timer callback — checks card presence and fires insert/remove callback.
  * @param  param: unused.
  */
static void SD_CardDetectDeferred(void *param)
{
	u8 card_exist;
	UNUSED(param);

	card_exist = SD_HOST_DEV->CARD_EXIST;
	if (card_exist & SD_HOST_BIT_SD_EXISTENCE) {
		sd_adapter.card_info.sd_status = (card_exist & SD_HOST_BIT_SD_WRITE_PROTECT) ? SD_PROTECTED : SD_INSERT;
		if (sd_adapter.card_info.card_insert_cb != NULL) {
			sd_adapter.card_info.card_insert_cb();
		}
	} else {
		sd_adapter.card_info.sd_status = SD_NODISK;
		if (sd_adapter.card_info.card_remove_cb != NULL) {
			sd_adapter.card_info.card_remove_cb();
		}
		if (sd_adapter.card_info.wait_interrupts != 0) {
			if (sd_adapter.card_info.transfer_done_cb != NULL) {
				sd_adapter.card_info.transfer_done_cb();
			}
			sd_adapter.card_info.wait_interrupts = 0;
		}
	}

	if (cd_cb != NULL) {
		cd_cb(sd_adapter.card_info.sd_status);
	}
}

/**
  * @brief  SDMMC IRQ handler — clears transfer interrupts and starts card-detect debounce.
  * @param  param: unused.
  * @retval  0 always.
  */
static u32 SD_IRQHandler(void *param)
{
	volatile u32 isr_status;
	UNUSED(param);

	isr_status = SDHost_GetISR();
	if (isr_status) {
		SDHost_INTClearPendingBit(isr_status);
	}

	if (sd_adapter.card_info.wait_interrupts != 0) {
		sd_adapter.card_info.wait_interrupts &= ~isr_status;
		if ((sd_adapter.card_info.wait_interrupts == 0) || (isr_status & SD_HOST_CARD_ERROR)) {
			if (sd_adapter.card_info.transfer_done_cb != NULL) {
				sd_adapter.card_info.transfer_done_cb();
			}
			sd_adapter.card_info.wait_interrupts = 0;
		}
	}

	if (SD_HOST_DEV->CARD_INT_PEND & SD_HOST_BIT_SD_MMC_INT_PENDING) {
		SDHost_INTClearPendingBit(SD_HOST_CARD_DETECT);
		if (sd_cd_timer != NULL) {
			rtos_timer_change_period(sd_cd_timer, 50, 0);
		}
	}

	return 0;
}

/**
  * @brief  Initialize the SD host controller and SD card.
  * @retval  SD card status (SD_OK, SD_NODISK, SD_INITERR, etc.)
  */
SD_RESULT SD_Init(void)
{
	if (CORE_S_BASE->SYS_SDIO_SD_CTRL & CORE_S_BIT_SYS_SD_SYST_SCLK_EN) {
		SD_DeInit();
		/* Re-init: the card was powered, so let its VDD discharge before
		 * re-powering for a clean reset. Cold init skips this — nothing was
		 * ever powered, so there is no charge to drain. */
		SD_DelayMs(50);
	} else {
		SDCard_ModuleInit();
	}

	sd_adapter.host_cfg = &sdioh_config;
	sd_adapter.host_ops = &SDHost_Ops;
	sd_adapter.card_info.card_insert_cb = SD_CardInsertISR;
	sd_adapter.card_info.card_remove_cb = SD_CardRemoveISR;
	sd_adapter.card_info.transfer_done_cb = SD_TransferDoneISR;

	rtos_timer_create(&sd_cd_timer, "sd_cd", 0, 50, false, SD_CardDetectDeferred);

	SDHost_EnableClk();
	SDHost_PowerUp();
	SDCard_PowerEn(1);
	/* Card VDD ramp settle. The card-detect debounce in SD_CardInit adds
	 * ~25 ms more before the first command, so 20 ms here is enough. */
	SD_DelayMs(20);
	SDHost_Init(sdioh_config.sdioh_bus_width);
	PAD_PullCtrl(sd_pins.cd, GPIO_PuPd_UP);

	if (SD_HOST_DEV->CARD_EXIST & SD_HOST_BIT_SD_EXISTENCE) {
		SD_CardInit();
	} else {
		/* No card: set NODISK explicitly. On a re-init SD_DeInit() leaves
		 * sd_status stale (possibly SD_OK from a prior card), so without this
		 * SD_Init() could wrongly report success when the card was removed. */
		sd_adapter.card_info.sd_status = SD_NODISK;
	}

	InterruptRegister((IRQ_FUN)SD_IRQHandler, SDMMC_IRQ, (u32)NULL, INT_PRI_MIDDLE);
	InterruptEn(SDMMC_IRQ, INT_PRI_MIDDLE);

	return SD_Status();
}

/**
  * @brief  De-initialize the SD host controller and release resources.
  * @retval  SD_OK always.
  */
SD_RESULT SD_DeInit(void)
{
	InterruptDis(SDMMC_IRQ);
	InterruptUnRegister(SDMMC_IRQ);

	if (sd_cd_timer != NULL) {
		rtos_timer_stop(sd_cd_timer, 0);
		rtos_timer_delete(sd_cd_timer, 0);
		sd_cd_timer = NULL;
	}

	SDCard_PowerEn(0);
	SDHost_PowerDown();
	SDHost_DeInit();

	/* The card is now powered down. Report not-ready so SD_Status() callers
	 * (notably the fatfs adopt path) don't see a stale SD_OK from the prior
	 * session and skip a needed re-init. */
	sd_adapter.card_info.sd_status = SD_NODISK;
	return SD_OK;
}

/**
  * @brief  Get current SD card status.
  * @retval  SD_OK, SD_NODISK, SD_INITERR, SD_PROTECTED, or SD_ERROR.
  */
SD_RESULT SD_Status(void)
{
	/* host_ops is assigned only by SD_Init(). Until then sd_adapter is zeroed,
	 * where sd_status would read SD_OK (0) and falsely report a ready card to
	 * callers (e.g. fatfs adopting an un-powered card). Report SD_NODISK until
	 * a real init has run. */
	if (sd_adapter.host_ops == NULL) {
		return SD_NODISK;
	}
	return sd_adapter.card_info.sd_status;
}

/**
  * @brief  Write one or more 512-byte blocks (handles unaligned buffers).
  * @param  sector: starting sector number.
  * @param  data: source buffer (any alignment).
  * @param  count: number of blocks.
  * @retval  SD_OK on success, SD_ERROR or current sd_status on failure.
  */
SD_RESULT SD_WriteBlocks(u32 sector, const u8 *data, u32 count)
{
	u8 res, i = 0;
	u8 *ptr;

	if (sd_adapter.card_info.sd_status == SD_INITERR || sd_adapter.card_info.sd_status == SD_ERROR || sd_adapter.card_info.sd_status == SD_NODISK) {
		RTK_LOGE(TAG, "SD card not ready\n");
		return sd_adapter.card_info.sd_status;
	}

	if ((u32)data & 0x1F) {
		u8 *raw_ptr = rtos_mem_malloc(SD_BLOCK_SIZE + 0x1F);
		if (raw_ptr == NULL) {
			return SD_ERROR;
		}
		ptr = (u8 *)(((((u32)raw_ptr - 1) >> 5) + 1) << 5);

		do {
			_memcpy(ptr, data + i * SD_BLOCK_SIZE, SD_BLOCK_SIZE);
			res = SDCard_WriteBlock(ptr, sector + i);
			if (res != HAL_OK) {
				break;
			}
		} while (++i < count);

		rtos_mem_free(raw_ptr);
	} else {
		res = (count == 1) ? SDCard_WriteBlock((u8 *)data, sector)
			  : SDCard_WriteMultiBlocks((u8 *)data, sector, count);
	}

	return (res == HAL_OK) ? SD_OK : SD_ERROR;
}

/**
  * @brief  Read one or more 512-byte blocks (handles unaligned buffers).
  * @param  sector: starting sector number.
  * @param  data: destination buffer (any alignment).
  * @param  count: number of blocks.
  * @retval  SD_OK on success, SD_ERROR or current sd_status on failure.
  */
SD_RESULT SD_ReadBlocks(u32 sector, u8 *data, u32 count)
{
	u8 res, i = 0;
	u8 *ptr;

	if (sd_adapter.card_info.sd_status == SD_INITERR || sd_adapter.card_info.sd_status == SD_ERROR || sd_adapter.card_info.sd_status == SD_NODISK) {
		RTK_LOGE(TAG, "SD card not ready\n");
		return sd_adapter.card_info.sd_status;
	}

	if ((u32)data & 0x1F) {
		u8 *raw_ptr = rtos_mem_malloc(SD_BLOCK_SIZE + 0x1F);
		if (raw_ptr == NULL) {
			return SD_ERROR;
		}
		ptr = (u8 *)(((((u32)raw_ptr - 1) >> 5) + 1) << 5);

		do {
			if (sd_adapter.card_info.sd_status == SD_INITERR || sd_adapter.card_info.sd_status == SD_ERROR || sd_adapter.card_info.sd_status == SD_NODISK) {
				rtos_mem_free(raw_ptr);
				return sd_adapter.card_info.sd_status;
			}
			res = SDCard_ReadBlock(ptr, sector + i);
			if (res != HAL_OK) {
				break;
			}
			_memcpy(data + i * SD_BLOCK_SIZE, ptr, SD_BLOCK_SIZE);
		} while (++i < count);

		rtos_mem_free(raw_ptr);
	} else {
		res = (count == 1) ? SDCard_ReadBlock(data, sector)
			  : SDCard_ReadMultiBlocks(data, sector, count);
	}

	return (res == HAL_OK) ? SD_OK : SD_ERROR;
}

/**
  * @brief  Get card capacity in 512-byte sectors.
  * @param  sector_count: output pointer to receive total sector count.
  * @retval  SD_OK on success, current sd_status on failure.
  */
SD_RESULT SD_GetCapacity(u32 *sector_count)
{
	if (sd_adapter.card_info.sd_status == SD_INITERR || sd_adapter.card_info.sd_status == SD_ERROR || sd_adapter.card_info.sd_status == SD_NODISK) {
		return sd_adapter.card_info.sd_status;
	}

	*sector_count = sd_adapter.card_info.capacity * (1024 / SD_BLOCK_SIZE);
	return (*sector_count) ? SD_OK : SD_ERROR;
}

/**
  * @brief  Erase a range of blocks on the inserted card.
  *         Dispatches CMD35/36 (eMMC) or CMD32/33 (SD).
  */
SD_RESULT SD_Erase(u32 StartBlock, u32 EndBlock)
{
	u32 ret;

	if (sd_adapter.card_info.sd_status == SD_INITERR ||
		sd_adapter.card_info.sd_status == SD_ERROR ||
		sd_adapter.card_info.sd_status == SD_NODISK) {
		RTK_LOGE(TAG, "card not ready\n");
		return sd_adapter.card_info.sd_status;
	}

	if (sd_adapter.card_info.card_type == CARD_TYPE_EMMC) {
		ret = EMMCCard_Erase(StartBlock, EndBlock);
	} else {
		ret = SDCard_Erase(StartBlock, EndBlock);
	}
	return (ret == HAL_OK) ? SD_OK : SD_ERROR;
}

/**
  * @brief  Read multiple blocks (buffer must be 32-byte aligned).
  * @param  readbuff: 32-byte aligned destination buffer.
  * @param  BlockIdx: starting block index.
  * @param  NumberOfBlocks: number of blocks to read.
  * @retval  SD_OK on success, SD_ERROR on failure.
  */
SD_RESULT SD_ReadMultiBlocks(u8 *readbuff, u32 BlockIdx, u32 NumberOfBlocks)
{
	if (sd_adapter.card_info.sd_status == SD_INITERR || sd_adapter.card_info.sd_status == SD_ERROR || sd_adapter.card_info.sd_status == SD_NODISK) {
		RTK_LOGE(TAG, "SD card not ready\n");
		return sd_adapter.card_info.sd_status;
	}

	return (SDCard_ReadMultiBlocks(readbuff, BlockIdx, NumberOfBlocks) == HAL_OK) ? SD_OK : SD_ERROR;
}

/**
  * @brief  Write multiple blocks (buffer must be 32-byte aligned).
  * @param  writebuff: 32-byte aligned source buffer.
  * @param  BlockIdx: starting block index.
  * @param  NumberOfBlocks: number of blocks to write.
  * @retval  SD_OK on success, SD_ERROR on failure.
  */
SD_RESULT SD_WriteMultiBlocks(const u8 *writebuff, u32 BlockIdx, u32 NumberOfBlocks)
{
	if (sd_adapter.card_info.sd_status == SD_INITERR || sd_adapter.card_info.sd_status == SD_ERROR || sd_adapter.card_info.sd_status == SD_NODISK) {
		RTK_LOGE(TAG, "SD card not ready\n");
		return sd_adapter.card_info.sd_status;
	}

	return (SDCard_WriteMultiBlocks((u8 *)writebuff, BlockIdx, NumberOfBlocks) == HAL_OK) ? SD_OK : SD_ERROR;
}

/* ========================================================================== */
/*        Functions moved from ameba_sd_card.c — host-aware orchestration     */
/* ========================================================================== */

#define card_info  sd_adapter.card_info

/**
  * @brief  Set pull-up/pull-down on SD data and CMD pads.
  * @param  pull: GPIO_PuPd_UP, GPIO_PuPd_DOWN, or GPIO_PuPd_NOPULL.
  */
static void SD_SetBusPull(u32 pull)
{
	PAD_PullCtrl(sd_pins.d1, pull);
	PAD_PullCtrl(sd_pins.d0, pull);
	PAD_PullCtrl(sd_pins.cmd, pull);
	PAD_PullCtrl(sd_pins.d3, pull);
	PAD_PullCtrl(sd_pins.d2, pull);
}

/**
  * @brief  Perform voltage switch sequence (CMD11) to switch from 3.3V to 1.8V signaling.
  *         Follows the SD Spec voltage switch sequence.
  * @retval  HAL_OK: Voltage switch succeeded
  *          Others: Voltage switch failed, card needs power cycle
  */
static u32 SD_VoltageSwitch(void)
{
	u32 ret;
	SD_HOST_CmdTypeDef cmd_attr;

	SDHost_SetClkToggle(1);

	cmd_attr.arg = 0;
	cmd_attr.idx = SD_CMD_VolSwitch;
	cmd_attr.rsp_type = SD_HOST_RSP_6B;
	cmd_attr.rsp_crc_chk = ENABLE;
	cmd_attr.data_present = SD_HOST_NO_DATA;
	ret = SDHost_SendCommand(&cmd_attr, SD_HOST_CMD_CPLT_TIMEOUT);
	if (ret != HAL_OK) {
		RTK_LOGE(TAG, "CMD11 send failed\n");
		return ret;
	}

	ret = SDCard_CmdRespError(SDIOH_RESP_R1, SD_CMD_VolSwitch);
	if (ret != HAL_OK) {
		RTK_LOGE(TAG, "CMD11 response error\n");
		return ret;
	}

	/* Wait for CMD & DAT[3:0] driven low by card */
	ret = SDHost_CheckBusState(0, 100000);
	if (ret != HAL_OK) {
		RTK_LOGE(TAG, "Bus did not go low after CMD11\n");
		return ret;
	}

	/* Disable SDCLK, pull bus lines low */
	SD_SetBusPull(GPIO_PuPd_DOWN);
	SDHost_SetClkToggle(0);

	/* Switch LDO to 1.8V and switch host pad voltage level */
	LDO_SDIOPower_Select(SDIO_HOST_1V8);
	SDHost_SetPadVoltage(SD_HOST_BUS_SIGNALING_LEVEL_1p8V);

	/* Wait for LDO to stabilize at 1.8V */
	SD_DelayMs(100);

	/* Pull bus lines high, re-enable SDCLK at 1.8V */
	SD_SetBusPull(GPIO_PuPd_UP);
	SDHost_SetClkToggle(1);

	/* SD spec: provide at least 1ms of SDCLK before checking bus state */
	SD_DelayMs(1);

	/* Verify CMD & DAT[3:0] are driven high by card at 1.8V */
	ret = SDHost_CheckBusState(1, 100000);
	if (ret != HAL_OK) {
		write_8_bit((u32)&SD_HOST_DEV->SD_BUS_STATUS,
					SD_HOST_DEV->SD_BUS_STATUS & ~SD_HOST_BIT_SD_CLK_TOGGLE_EN);

		/* Restore LDO and pads to 3.3V so the retry path works */
		LDO_SDIOPower_Select(SDIO_HOST_3V3);
		SDHost_SetPadVoltage(SD_HOST_BUS_SIGNALING_LEVEL_3p3V);
		SD_SetBusPull(GPIO_PuPd_UP);

		RTK_LOGE(TAG, "Bus did not go high after 1.8V switch\n");
		return ret;
	}

	write_8_bit((u32)&SD_HOST_DEV->SD_BUS_STATUS,
				SD_HOST_DEV->SD_BUS_STATUS & ~SD_HOST_BIT_SD_CLK_TOGGLE_EN);

	RTK_LOGS(TAG, RTK_LOG_DEBUG, "Voltage switch to 1.8V succeeded\n");
	return HAL_OK;
}

/**
  * @brief  Configure host clock divider and mode for the given speed.
  * @param  speed: target speed (SD_SPEED_DS .. SD_SPEED_DDR50).
  * @retval  HAL_OK on success.
  */
static u32 SD_ApplyHostClock(u8 speed)
{
	if (card_info.sig_level == SD_HOST_BUS_SIGNALING_LEVEL_1p8V) {
		if (speed == SD_SPEED_SDR12) {
			SDHost_SwitchSpeed(2, SD_HOST_modesd30);   /* div4: 25 MHz */
		} else if (speed == SD_SPEED_SDR25) {
			SDHost_SwitchSpeed(1, SD_HOST_modesd30);   /* div2: 50 MHz */
		} else if (speed == SD_SPEED_SDR50) {
			SDHost_SwitchSpeed(0, SD_HOST_modesd30);   /* div1: 100 MHz */
		} else if (speed == SD_SPEED_SDR104) {
			/* Same div as SDR50; card is in SDR104 function mode.
			 * Host clock max is 100 MHz — full SDR104 (208 MHz) needs tuning. */
			SDHost_SwitchSpeed(0, SD_HOST_modesd30);   /* div1: 100 MHz */
		} else if (speed == SD_SPEED_DDR50) {
			SDHost_SetSamplePoint(SD_HOST_DDR50_SAMPLE_POINT);
			SDHost_SwitchSpeed(0, SD_HOST_modeddr);    /* div1: 50 MHz DDR */
		}
	} else {
		if (speed == SD_SPEED_DS) {
			SDHost_SwitchSpeed(2, SD_HOST_modesd20);   /* div4: 25 MHz */
		} else if (speed == SD_SPEED_HS) {
			SDHost_SwitchSpeed(0, SD_HOST_modesd20);   /* div1: 50 MHz */
		}
	}

	return HAL_OK;
}

/**
  * @brief  Switch SD card bus speed (validates signal level, sends CMD6, applies host clock).
  * @param  speed: target speed (SD_SPEED_DS .. SD_SPEED_DDR50).
  * @retval  HAL_OK on success, HAL_ERR_PARA/HAL_ERR_HW on failure.
  */
u32 SD_SwitchBusSpeed(u8 speed)
{
	u32 ret;
	u8 *pbuf_raw = NULL;
	u8 *pbuf = NULL;
	u8 support_speed;
	u8 cmd6_func;

	if (card_info.bus_spd == speed) {
		RTK_LOGS(TAG, RTK_LOG_DEBUG, "Already at requested speed\r\n");
		return HAL_OK;
	}

	/* Validate speed vs current signal level */
	if (card_info.sig_level == SD_HOST_BUS_SIGNALING_LEVEL_1p8V) {
		if (speed < SD_SPEED_SDR12 || speed > SD_SPEED_DDR50) {
			RTK_LOGW(TAG, "1.8V doesn't support speed %d\n", speed);
			return HAL_ERR_PARA;
		}
	} else {
		if (speed > SD_SPEED_HS) {
			return HAL_ERR_PARA;
		}
	}

	if (card_info.sd_spec_ver < SD_SPEC_V110) {
		RTK_LOGW(TAG, "Card doesn't support CMD6\r\n");
		return HAL_ERR_HW;
	}

	/* Allocate 32-byte aligned DMA buffer */
	pbuf_raw = (u8 *)rtos_mem_malloc(SD_HOST_BUFFER_LENGTH + 0x1F);
	if (pbuf_raw == NULL) {
		RTK_LOGE(TAG, "Allocate buffer error !!\r\n");
		return HAL_ERR_MEM;
	}
	pbuf = (u8 *)(((((u32)pbuf_raw - 1) >> 5) + 1) << 5);

	cmd6_func = (speed >= SD_SPEED_SDR12) ? (speed - SD_SPEED_SDR12) : speed;

	do {
		ret = SDCard_QuerySupportedSpeeds(pbuf);
		if (ret != HAL_OK) {
			break;
		}

		support_speed = pbuf[13];
		if (!(support_speed & (1 << cmd6_func))) {
			RTK_LOGW(TAG, "Card doesn't support speed %d (supported: 0x%02X)\n", speed, support_speed);
			ret = HAL_ERR_HW;
			break;
		}

		ret = SDCard_SwitchFunction(SD_CMD6_CHECK_MODE, cmd6_func, pbuf);
		if (ret != HAL_OK) {
			break;
		}
		if ((pbuf[16] & 0xf) != cmd6_func) {
			RTK_LOGW(TAG, "Card rejected speed switch check\n");
			ret = HAL_ERR_HW;
			break;
		}

		ret = SDCard_SwitchFunction(SD_CMD6_SWITCH_MODE, cmd6_func, pbuf);
		if (ret != HAL_OK) {
			break;
		}
		if ((pbuf[16] & 0xf) != cmd6_func) {
			RTK_LOGW(TAG, "Speed switch was canceled by card\n");
			ret = HAL_ERR_HW;
			break;
		}

		card_info.bus_spd = speed;
		ret = SD_ApplyHostClock(speed);
		if (ret != HAL_OK) {
			break;
		}

		RTK_LOGS(TAG, RTK_LOG_DEBUG, "Bus speed switched to %s\n", SDCard_SpeedStr(speed));
	} while (0);

	rtos_mem_free(pbuf_raw);
	return ret;
}

/**
  * @brief  Negotiate the best bus speed based on config and card capability.
  */
static void SD_SpeedNegotiation(void)
{
	u8 req_speed = sdioh_config.sdioh_bus_speed;

	/* HS200 is eMMC-only; on an SD card promote to auto-best UHS rather than
	 * let SD_SwitchBusSpeed(HS200) fail and strand the (already-switched) bus. */
	if (req_speed == SD_SPEED_HS200) {
		RTK_LOGW(TAG, "HS200 not supported on SD cards, using auto-best UHS\n");
		req_speed = SD_KEEP_CUR_SPEED;
	}

	if (req_speed == SD_KEEP_CUR_SPEED || req_speed == SD_SPEED_HS) {
		if (card_info.sig_level == SD_HOST_BUS_SIGNALING_LEVEL_1p8V) {
			static const u8 uhs_speeds[] = { SD_SPEED_DDR50, SD_SPEED_SDR50, SD_SPEED_SDR25 };
			u8 switched = 0;
			for (u32 s = 0; s < sizeof(uhs_speeds); s++) {
				if (SD_SwitchBusSpeed(uhs_speeds[s]) == HAL_OK) {
					switched = 1;
					break;
				}
			}
			if (!switched) {
				RTK_LOGW(TAG, "All UHS speeds failed, keeping SDR12\n");
			}
		} else {
			SD_SwitchBusSpeed(SD_SPEED_HS);
		}
	} else if (req_speed != SD_SPEED_DS) {
		SD_SwitchBusSpeed(req_speed);
	}
}

/**
  * @brief  Identify card with SD/eMMC dual-path probe.
  *         CMD8 ack → definitely SD ≥2.0. CMD8 timeout → try ACMD41 (SD v1.x);
  *         if that fails, reset and try CMD1 (eMMC).
  */
static u32 SD_IdentifyCard(void)
{
	u32 ret;
	u8 voltage_mismatch = 0;

	ret = SDCard_ResetCard();
	if (ret != HAL_OK) {
		RTK_LOGE(TAG, "ResetCard(CMD0) fail %lu\n", ret);
		return ret;
	}

	ret = SDCard_VoltageCheck(&voltage_mismatch);
	if (ret != HAL_OK) {
		RTK_LOGE(TAG, "VoltageCheck(CMD8) fail %lu\n", ret);
		return ret;
	}

	if (voltage_mismatch == 0) {
		ret = SDCard_GetOCR(voltage_mismatch);
		if (ret != HAL_OK) {
			RTK_LOGS(TAG, RTK_LOG_DEBUG, "GetOCR fail %u\n", ret);
			return ret;
		}

		if (card_info.sig_level == SD_HOST_BUS_SIGNALING_LEVEL_1p8V) {
			ret = SD_VoltageSwitch();
			if (ret != HAL_OK) {
				card_info.sig_level = SD_HOST_BUS_SIGNALING_LEVEL_3p3V;
				card_info.bus_spd = SD_SPEED_DS;
				RTK_LOGW(TAG, "Voltage switch failed, card needs power cycle\n");
				return ret;
			}
		}

		ret = SDCard_GetCID();
		if (ret != HAL_OK) {
			RTK_LOGE(TAG, "GetCID fail %lu\n", ret);
			return ret;
		}

		ret = SDCard_GetRCA();
		if (ret != HAL_OK) {
			RTK_LOGE(TAG, "GetRCA fail %lu\n", ret);
			return ret;
		}

		card_info.card_type = CARD_TYPE_SD;
		return HAL_OK;
	}

	ret = SDCard_GetOCR(voltage_mismatch);
	if (ret == HAL_OK) {
		ret = SDCard_GetCID();
		if (ret != HAL_OK) {
			RTK_LOGE(TAG, "SD v1.x GetCID fail %lu\n", ret);
			return ret;
		}
		ret = SDCard_GetRCA();
		if (ret != HAL_OK) {
			RTK_LOGE(TAG, "SD v1.x GetRCA fail %lu\n", ret);
			return ret;
		}
		card_info.card_type = CARD_TYPE_SD;
		return HAL_OK;
	}

	/* No CMD8/ACMD41 response: not an SD card. Reset and probe eMMC via CMD1. */
	ret = SDCard_ResetCard();
	if (ret != HAL_OK) {
		RTK_LOGE(TAG, "ResetCard before eMMC try fail %lu\n", ret);
		return ret;
	}

	ret = EMMCCard_GetOCR_CMD1();
	if (ret != HAL_OK) {
		RTK_LOGS(TAG, RTK_LOG_DEBUG, "CMD1 fail %u — no recognized card\n", ret);
		return ret;
	}

	ret = EMMCCard_GetCID();
	if (ret != HAL_OK) {
		RTK_LOGE(TAG, "GetCID(eMMC) fail %lu\n", ret);
		return ret;
	}

	ret = EMMCCard_AssignRCA();
	if (ret != HAL_OK) {
		RTK_LOGE(TAG, "AssignRCA fail %lu\n", ret);
		return ret;
	}

	return HAL_OK;
}

/**
  * @brief  Configure card post-identification, branched by card_type.
  */
static u32 SD_ConfigureCard(void)
{
	u32 ret;

	ret = SDHost_InitialModeCmd(DISABLE, card_info.sig_level);
	if (ret != HAL_OK) {
		RTK_LOGE(TAG, "InitialModeCmd(OFF) fail %lu\n", ret);
		return ret;
	}

	if (card_info.sig_level == SD_HOST_BUS_SIGNALING_LEVEL_1p8V) {
		card_info.bus_spd = SD_SPEED_SDR12;
	} else {
		card_info.bus_spd = SD_SPEED_DS;
	}

	if (card_info.card_type == CARD_TYPE_EMMC) {
		ret = EMMCCard_GetCSD();
		if (ret != HAL_OK) {
			RTK_LOGE(TAG, "EMMC GetCSD fail %lu\n", ret);
			return ret;
		}

		ret = SDCard_SelectDeselect(ENABLE);
		if (ret != HAL_OK) {
			RTK_LOGE(TAG, "SelectDeselect fail %lu\n", ret);
			return ret;
		}

		/* EXT_CSD is read in 1-bit mode (eMMC post-CMD0 default; host was
		 * left at 1-bit by InitialModeCmd above). EMMCCard_SetBusWidth
		 * switches card and host together for subsequent transfers. */
		ret = EMMCCard_GetExtCSD();
		if (ret != HAL_OK) {
			RTK_LOGE(TAG, "EMMC GetExtCSD fail %lu\n", ret);
			return ret;
		}

		if (sdioh_config.sdioh_bus_width == SDIOH_BUS_WIDTH_4BIT) {
			ret = EMMCCard_SetBusWidth(SDIOH_BUS_WIDTH_4BIT);
			if (ret != HAL_OK) {
				RTK_LOGE(TAG, "EMMC SetBusWidth fail %lu\n", ret);
				return ret;
			}
		}

		ret = EMMCCard_SpeedNegotiate();
		if (ret != HAL_OK) {
			RTK_LOGE(TAG, "EMMC SpeedNegotiate fail %lu\n", ret);
			return ret;
		}
		return HAL_OK;
	}

	/* SD path — original code preserved verbatim */
	ret = SDCard_GetCSD();
	if (ret != HAL_OK) {
		RTK_LOGE(TAG, "GetCSD fail %lu\n", ret);
		return ret;
	}

	ret = SDCard_SelectDeselect(ENABLE);
	if (ret != HAL_OK) {
		RTK_LOGE(TAG, "SelectDeselect fail %lu\n", ret);
		return ret;
	}

	if (sdioh_config.sdioh_bus_width == SDIOH_BUS_WIDTH_4BIT) {
		ret = SDCard_SetBusWidth(SDIOH_BUS_WIDTH_4BIT);
		if (ret != HAL_OK) {
			RTK_LOGE(TAG, "SetBusWidth fail %lu\n", ret);
			return ret;
		}
	}

	ret = SDCard_GetSCR();
	if (ret != HAL_OK) {
		RTK_LOGW(TAG, "GetSCR fail %lu\n", ret);
	}

	/* Best-effort: speed/video class for the init summary. */
	if (SDCard_GetStatus() != HAL_OK) {
		card_info.speed_class = 0;
		card_info.uhs_grade = 0;
		card_info.video_class = 0;
		card_info.app_class = 0;
	}

	SD_SpeedNegotiation();

	return HAL_OK;
}

/* Append a ", "-separated token to buf, returning the new length. */
static u32 SD_ClassAppend(char *buf, u32 n, const char *tok)
{
	if (tok == NULL) {
		return n;
	}
	if (n > 0) {
		buf[n++] = ',';
		buf[n++] = ' ';
	}
	while (*tok != '\0') {
		buf[n++] = *tok++;
	}
	return n;
}

/* Every speed rating the card reports, shown together (e.g. "V30, U3, C10"):
 * Video Speed Class, UHS Speed Grade, then legacy Speed Class. From ACMD13. */
static const char *SD_SpeedClassStr(void)
{
	static char buf[24];
	const char *tok;
	u32 n = 0;

	switch (card_info.video_class) {
	case 90:
		tok = "V90";
		break;
	case 60:
		tok = "V60";
		break;
	case 30:
		tok = "V30";
		break;
	case 10:
		tok = "V10";
		break;
	case 6:
		tok = "V6";
		break;
	default:
		tok = NULL;
		break;
	}
	n = SD_ClassAppend(buf, n, tok);

	tok = (card_info.uhs_grade == 3) ? "U3" : (card_info.uhs_grade == 1) ? "U1" : NULL;
	n = SD_ClassAppend(buf, n, tok);

	tok = (card_info.app_class == 2) ? "A2" : (card_info.app_class == 1) ? "A1" : NULL;
	n = SD_ClassAppend(buf, n, tok);

	switch (card_info.speed_class) {
	case 4:
		tok = "C10";
		break;
	case 3:
		tok = "C6";
		break;
	case 2:
		tok = "C4";
		break;
	case 1:
		tok = "C2";
		break;
	default:
		tok = NULL;
		break;
	}
	n = SD_ClassAppend(buf, n, tok);

	if (n == 0) {
		return "C0";
	}
	buf[n] = '\0';
	return buf;
}

/**
  * @brief  Initialize card: identify, configure, log result.
  *         Called from SD_Init() and SD_CardInsertISR().
  */
/* A freshly inserted card can answer the broadcast CMD8 while its internal
 * power-up is still settling, so the addressed ACMD41/CMD55 times out and
 * identification fails fast. A clean power re-cycle clears it, so retry the
 * whole identification a few times before giving up. */
#define SD_INIT_MAX_ATTEMPTS 3

static void SD_CardInit(void)
{
	u32 ret = HAL_ERR_UNKNOWN;
	u8 attempt;
	u8 card_present = 1;

	SDHost_INTConfig(SD_HOST_DMA_TRANSFER_DONE | SD_HOST_CARD_END | SD_HOST_CARD_ERROR, DISABLE);
	SDHost_INTClearPendingBit(SD_HOST_ISR_ALL);

	for (attempt = 0; attempt < SD_INIT_MAX_ATTEMPTS; attempt++) {
		if (!SDCard_CheckCardPresent()) {
			RTK_LOGW(TAG, "Card not fully seated, abort init\n");
			card_present = 0;
			ret = HAL_ERR_UNKNOWN;
			break;
		}

		ret = SD_IdentifyCard();
		if (ret == HAL_OK) {
			ret = SD_ConfigureCard();
		}
		if (ret == HAL_OK) {
			break;
		}

		if (attempt + 1 < SD_INIT_MAX_ATTEMPTS) {
			RTK_LOGS(TAG, RTK_LOG_DEBUG, "Identify failed (ret %u), power-cycle retry %u\n", ret, attempt + 1);
			SDCard_PowerEn(0);
			SDHost_PowerDown();
			SD_DelayMs(50);
			SD_ResetCardInfo();
			SDHost_PowerUp();
			SDCard_PowerEn(1);
			SD_DelayMs(100);
			SDHost_Init(sdioh_config.sdioh_bus_width);
		}
	}

	if (ret == HAL_OK) {
		u64 bytes = (u64)card_info.capacity * 1024;
		u32 size = (bytes >= 1000000000ULL) ? (u32)(bytes / 1000000000ULL) : (u32)(bytes / 1000000ULL);
		const char *unit = (bytes >= 1000000000ULL) ? "GB" : "MB";
		u8 width = (sd_adapter.host_ops->GetBusWidth() == SDIOH_BUS_WIDTH_4BIT) ? 4 : 1;

		card_info.sd_status = SD_OK;
		if (card_info.card_type == CARD_TYPE_EMMC) {
			RTK_LOGI(TAG, "eMMC initialized:\n"
					 "  Capacity    : %lu %s\n"
					 "  Addressing  : %s\n"
					 "  Bus mode    : %s, %u-bit, %s\n"
					 "  Serial no.  : %08lX\n",
					 size, unit,
					 card_info.is_sdhc_sdxc ? "sector mode" : "byte mode",
					 EMMCCard_SpeedStr(card_info.bus_spd), width, SDCard_VolStr(card_info.sig_level),
					 card_info.serial);
		} else {
			RTK_LOGI(TAG, "SD card initialized:\n"
					 "  Capacity    : %lu %s\n"
					 "  Card type   : %s\n"
					 "  Bus mode    : %s, %u-bit, %s\n"
					 "  Speed class : %s\n"
					 "  Serial no.  : %08lX\n",
					 size, unit,
					 card_info.is_sdhc_sdxc ? "SDHC/SDXC" : "SDSC",
					 SDCard_SpeedStr(card_info.bus_spd), width, SDCard_VolStr(card_info.sig_level),
					 SD_SpeedClassStr(),
					 card_info.serial);
		}
	} else if (card_present) {
		card_info.sd_status = SD_INITERR;
		RTK_LOGE(TAG, "Init FAIL, ret: %lu\n", ret);
	} else {
		card_info.sd_status = SD_NODISK;
	}

	SDHost_INTClearPendingBit(SD_HOST_ISR_ALL);
	SDHost_INTConfig(SD_HOST_DMA_TRANSFER_DONE | SD_HOST_CARD_END | SD_HOST_CARD_ERROR, ENABLE);
}
