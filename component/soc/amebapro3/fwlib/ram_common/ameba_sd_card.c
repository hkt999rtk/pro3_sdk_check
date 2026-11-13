/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/*
 * SD card protocol layer — generic card commands and data transfer.
 *
 * This file accesses the SD host controller exclusively through the
 * SD_HOST_OPS_TypeDef vtable (sd_adapter.host_ops), enabling different
 * host IPs to be swapped without modifying card protocol code.
 *
 * All functions here are internal — called only by ameba_sd.c.
 *
 * Performance notes (DDR50 @ 1.8V, 4-bit, double-buffer, 128 blocks/xfer):
 *   AutoRd1/AutoWr1 (hardware CMD12):
 *     Write: 16.5s / 15.9 MB/s   Read: 13.3s / 19.7 MB/s
 *   AutoRd2/AutoWr2 (CMD23, no CMD12):
 *     Write: 21.0s / 12.2 MB/s   Read: 13.6s / 18.8 MB/s
 *   Card tested: SanDisk EXTREM PRO (256GB SDXC V30 U3)
 */

#include "ameba_soc.h"
#include "os_wrapper.h"

static const char *TAG = "SDC";

#define SDCard_DelayUs  DelayUs
#define SDCard_DelayMs  SD_DelayMs   /* weak wrapper in ameba_sd.c — override to yield */

/* sd_adapter is owned by ameba_sd.c, sdioh_config by ameba_intfcfg.c */
#define card_info    sd_adapter.card_info
#define host_ops     sd_adapter.host_ops

static const char *const sd_speed_names[] = {"DS", "HS", "SDR12", "SDR25", "SDR50", "SDR104", "DDR50"};

const char *SDCard_SpeedStr(u8 speed)
{
	return (speed <= SD_SPEED_DDR50) ? sd_speed_names[speed] : "?";
}

const char *SDCard_VolStr(u8 sig_level)
{
	return (sig_level == SD_HOST_BUS_SIGNALING_LEVEL_1p8V) ? "1.8V" : "3.3V";
}

/** @brief Debounce card-detect: SD_CD_STABLE_SAMPLES consecutive matches. */
static bool SDCard_DetectStable(bool expect_present)
{
	u32 stable = 0;

	for (u32 i = 0; i < SD_CD_STABLE_SAMPLES * 2; i++) {
		bool present = host_ops->CardExist();
		if (present == expect_present) {
			if (++stable >= SD_CD_STABLE_SAMPLES) {
				return true;    /* debounced */
			}
		} else {
			stable = 0;         /* restart window on any bounce */
		}
		SDCard_DelayMs(SD_CD_SAMPLE_MS);
	}
	return false;               /* never stable long enough */
}
bool SDCard_CheckCardPresent(void)
{
	SDCard_DelayMs(5);
	return SDCard_DetectStable(true) && host_ops->CardExist();
}

/** @brief Board SD power load-switch (weak; enable=1 ON, 0 OFF). */
__attribute__((weak)) void SDCard_PowerEn(u8 enable)
{
	GPIO_InitTypeDef gpio_init;
	gpio_init.GPIO_Pin = _PF_14;
	gpio_init.GPIO_Mode = GPIO_Mode_OUT;
	gpio_init.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(&gpio_init);
	GPIO_WriteBit(_PF_14, enable ? 0 : 1);  /* active low */
}

/** @brief Allocate a 32-byte aligned DMA buffer; *raw_buf gets the raw block to free. */
u8 *SDCard_AllocAlignedBuf(u8 **raw_buf)
{
	u8 *raw = (u8 *)rtos_mem_malloc(SD_HOST_BUFFER_LENGTH + 0x1F);
	if (raw == NULL) {
		return NULL;
	}

	if (raw_buf != NULL) {
		*raw_buf = raw;
	}

	return (u8 *)(((((u32)raw - 1) >> 5) + 1) << 5);	/* align to next 32-byte boundary */
}
/** @brief Wait for DMA completion — ISR-driven via the yield hook, else polled. */
u32 SDCard_chk_xfer_int(u32 timeout_us)
{

	host_ops->INTConfig(SD_HOST_DMA_TRANSFER_DONE | SD_HOST_CARD_END, ENABLE);
	do {
		if (card_info.transfer_task_yield_cb != NULL) {
			card_info.transfer_task_yield_cb();
			if (card_info.wait_interrupts == 0) {
				host_ops->DMAReset();
				return HAL_OK;
			} else {
				break;
			}
		} else {
			/* Check ISR-cleared flag first */
			if (card_info.wait_interrupts == 0) {
				host_ops->DMAReset();
				return HAL_OK;
			}
			/* Fallback: poll HW registers directly (same as SDHost_WaitDMADone)
			 * in case the SDMMC interrupt is not routed to this core */
			if (host_ops->CheckTransferDone()) {
				card_info.wait_interrupts = 0;
				host_ops->DMAReset();
				return HAL_OK;
			}
			SDCard_DelayUs(1);
		}
	} while (timeout_us-- != 0);
	host_ops->INTConfig(SD_HOST_DMA_TRANSFER_DONE | SD_HOST_CARD_END, DISABLE);
	return HAL_TIMEOUT;
}

/** @brief Validate R2 (136-bit) response — accepts non-zero data even if the
  *        controller flags an error; all-zeros → HAL_TIMEOUT. */
u32 SDCard_CheckR2Response(u8 *pbuf)
{

	if (!host_ops->HasError()) {
		return HAL_OK;
	}

	if (pbuf[0] == 0 && pbuf[1] == 0 && pbuf[2] == 0 && pbuf[3] == 0) {
		return HAL_TIMEOUT;
	}

	host_ops->ClearErrorBit();
	RTK_LOGW(TAG, "R2 response: controller error flag set but data received, continuing\n");
	return HAL_OK;
}

/** @brief Validate the command response (R1/R6/R7 index, error flags, CMD8 pattern). */
u32 SDCard_CmdRespError(u8 resp_type, u8 cmd)
{
	u32 ret = HAL_OK;
	u16 err_status;
	u8 resp_byte0, resp_byte1, resp_byte3, resp_byte4, t_cmd = cmd;

	ret = host_ops->CheckTxError(&err_status);
	if (ret != HAL_OK) {
		if (err_status & ((u16)SD_HOST_BIT_SD_CMD_RESPONSE_TIMEOUT_ERROR << 8)) {
			return HAL_TIMEOUT;
		} else if (err_status & SD_HOST_BIT_SD_TUNING_PATTERN_COMPARE_ERROR) {
			return HAL_ERR_HW;
		} else {
			return ret;
		}
	}

	/* For R1, R6, R7 */
	if ((resp_type == SDIOH_RESP_R1) || (resp_type == SDIOH_RESP_R6) || (resp_type == SDIOH_RESP_R7)) {
		if ((cmd == SD_CMD_WrMulBlk) || (cmd == SD_CMD_RdMulBlk)) {
			t_cmd = SD_CMD_StopXsmission;
		}

		resp_byte0 = host_ops->GetResponse(SDIO_RESP0);
		resp_byte1 = host_ops->GetResponse(SDIO_RESP1);
		resp_byte3 = host_ops->GetResponse(SDIO_RESP3);
		resp_byte4 = host_ops->GetResponse(SDIO_RESP4);

		if ((resp_byte0 & SD_HOST_CMD_IDX_MASK) != t_cmd) {
			RTK_LOGE(TAG, "Command index error !!\r\n");
			return HAL_ERR_UNKNOWN;
		}

		if (cmd == SD_CMD_AppCmd) {
			if (!(resp_byte4 & SD_APP_CMD)) {
				RTK_LOGE(TAG, "ACMD isn't expected !!\r\n");
				return HAL_ERR_UNKNOWN;
			}
		} else if ((cmd == SD_CMD_RdSingleBlk) || (cmd == SD_CMD_RdMulBlk)) {
			if ((resp_byte1 & SD_ADDRESS_ERROR) || (resp_byte1 & SD_BLOCK_LEN_ERROR)) {
				return HAL_ERR_UNKNOWN;
			}
		} else if ((cmd == SD_CMD_WrBlk) || (cmd == SD_CMD_WrMulBlk)) {
			if ((resp_byte1 & SD_ADDRESS_ERROR) || (resp_byte1 & SD_BLOCK_LEN_ERROR) || (resp_byte1 & SD_WP_VIOLATION)) {
				return HAL_ERR_UNKNOWN;
			}
		}
	}

	/* For R7 */
	if (resp_type == SDIOH_RESP_R7) {
		if (resp_byte4 != SD_HOST_CMD8_CHK_PATN) {
			RTK_LOGE(TAG, "Check pattern error !!\r\n");
			return HAL_ERR_UNKNOWN;
		}
		if ((resp_byte3 & 0xF) != SD_HOST_CMD8_VHS) {
			RTK_LOGE(TAG, "Voltage accepted error !!\r\n");
			return HAL_ERR_UNKNOWN;
		}
	}

	return HAL_OK;
}


/** @brief Reset the card to idle (CMD0). */
u32 SDCard_ResetCard(void)
{
	u32 ret;
	SD_HOST_CmdTypeDef cmd_attr;

	cmd_attr.arg = 0;
	cmd_attr.idx = SD_CMD_GoIdleSte;
	cmd_attr.rsp_type = SD_HOST_NO_RESP;
	cmd_attr.rsp_crc_chk = DISABLE;
	cmd_attr.data_present = SD_HOST_NO_DATA;
	ret = host_ops->SendCommand(&cmd_attr, SD_HOST_CMD_CPLT_TIMEOUT);
	if (ret != HAL_OK) {
		return ret;
	}

	ret = SDCard_CmdRespError(SD_HOST_NO_RESP, SD_CMD_GoIdleSte);
	if (ret != HAL_OK) {
		return ret;
	}

	/* SD spec: card needs at least 8 clock cycles after CMD0.
	 * Some UHS-I cards need more settling time. */
	SDCard_DelayMs(2);

	return HAL_OK;
}


/** @brief CMD8 SEND_IF_COND. Sets *voltage_mismatch=1 if the card never
  *        answers (SD v1.x or CRC issue). Retries 3x for slow-settling cards. */
u32 SDCard_VoltageCheck(u8 *voltage_mismatch)
{
	u32 ret;
	SD_HOST_CmdTypeDef cmd_attr;
	u8 retry;

	for (retry = 0; retry < 3; retry++) {
		if (retry > 0) {
			SDCard_DelayMs(10);
			/* Re-send CMD0 to reset card state before retrying CMD8 */
			cmd_attr.arg = 0;
			cmd_attr.idx = SD_CMD_GoIdleSte;
			cmd_attr.rsp_type = SD_HOST_NO_RESP;
			cmd_attr.rsp_crc_chk = DISABLE;
			cmd_attr.data_present = SD_HOST_NO_DATA;
			host_ops->SendCommand(&cmd_attr, SD_HOST_CMD_CPLT_TIMEOUT);
			host_ops->ClearErrorBit();
			SDCard_DelayMs(10);
		}

		cmd_attr.arg = (SD_HOST_CMD8_VHS << 8) | SD_HOST_CMD8_CHK_PATN;
		cmd_attr.idx = SD_CMD_SendIfCond;
		cmd_attr.rsp_type = SD_HOST_RSP_6B;
		cmd_attr.rsp_crc_chk = ENABLE;
		cmd_attr.data_present = SD_HOST_NO_DATA;
		ret = host_ops->SendCommand(&cmd_attr, SD_HOST_CMD_CPLT_TIMEOUT);
		if (ret != HAL_OK) {
			return ret;
		}

		ret = SDCard_CmdRespError(SDIOH_RESP_R7, SD_CMD_SendIfCond);
		if (ret == HAL_OK) {
			*voltage_mismatch = 0;
			RTK_LOGS(TAG, RTK_LOG_DEBUG, "Voltage match\n");
			return HAL_OK;
		}

		/* CMD8 failed — clear error and retry */
		host_ops->ClearErrorBit();
	}

	/* All retries exhausted: no CMD8 response — SD v1.x or eMMC, fall back. */
	*voltage_mismatch = 1;
	return HAL_OK;
}

/** @brief Stop transmission (CMD12) + host CardStop. */
u32 SDCard_StopTransfer(void)
{
	u32 ret;
	SD_HOST_CmdTypeDef cmd_attr;

	cmd_attr.arg = 0;
	cmd_attr.idx = SD_CMD_StopXsmission;
	cmd_attr.rsp_type = SD_HOST_RSP_6B;
	cmd_attr.rsp_crc_chk = ENABLE;
	cmd_attr.data_present = SD_HOST_NO_DATA;
	ret = host_ops->SendCommand(&cmd_attr, SD_HOST_CMD_CPLT_TIMEOUT);
	if (ret != HAL_OK) {
		return ret;
	}

	ret = SDCard_CmdRespError(SDIOH_RESP_R1, SD_CMD_StopXsmission);
	if (ret != HAL_OK) {
		return ret;
	}

	host_ops->CardStop();

	return HAL_OK;
}


/** @brief Build the ACMD41 argument (voltage_mismatch=1 → SDSC-only; else
  *        SDHC/SDXC, requesting S18R unless force_33v). */
static u32 SDCard_BuildACMD41Arg(u8 voltage_mismatch)
{
	if (voltage_mismatch) {
		return (SD_SUPPORT_SDSC_ONLY << 30) | (SD_MAX_PERFORM << 28) |
			   (SD_USE_CUR_VOL << 24) | SD_HOST_OCR_VDD_WIN;
	} else if (sd_adapter.force_33v) {
		/* SDHC/SDXC but skip S18R — host forced 3.3V */
		return (SD_SUPPORT_SDHC_SDXC << 30) | (SD_MAX_PERFORM << 28) |
			   (SD_USE_CUR_VOL << 24) | SD_HOST_OCR_VDD_WIN;
	} else {
		return (SD_SUPPORT_SDHC_SDXC << 30) | (SD_MAX_PERFORM << 28) |
			   (SD_SWITCH_18V << 24) | SD_HOST_OCR_VDD_WIN;
	}
}

/** @brief ACMD41 SEND_OP_COND loop: power-up, capacity (SDHC/SDXC), and
  *        1.8 V (S18A) decision. Latches the OCR byte before the loop exit. */
u32 SDCard_GetOCR(u8 voltage_mismatch)
{
	u32 ret;
	SD_HOST_CmdTypeDef cmd_attr;
	u32 retry = 100;
	u8 ocr_resp1 = 0;

	while (retry--) {
		/* ACMD41 = CMD55 then CMD41 */
		cmd_attr.arg = 0;
		cmd_attr.idx = SD_CMD_AppCmd;
		cmd_attr.rsp_type = SD_HOST_RSP_6B;
		cmd_attr.rsp_crc_chk = ENABLE;
		cmd_attr.data_present = SD_HOST_NO_DATA;
		ret = host_ops->SendCommand(&cmd_attr, SD_HOST_CMD_CPLT_TIMEOUT);
		if (ret != HAL_OK) {
			return ret;
		}

		ret = SDCard_CmdRespError(SDIOH_RESP_R1, SD_CMD_AppCmd);
		if (ret != HAL_OK) {
			break;
		}

		cmd_attr.arg = SDCard_BuildACMD41Arg(voltage_mismatch);

		cmd_attr.idx = SD_CMD_SdSendOpCond;
		cmd_attr.rsp_type = SD_HOST_RSP_6B;
		cmd_attr.rsp_crc_chk = DISABLE;
		cmd_attr.data_present = SD_HOST_NO_DATA;
		ret = host_ops->SendCommand(&cmd_attr, SD_HOST_CMD_CPLT_TIMEOUT);
		if (ret != HAL_OK) {
			return ret;
		}

		ret = SDCard_CmdRespError(SDIOH_RESP_R3, SD_CMD_SdSendOpCond);
		if (ret != HAL_OK) {
			return ret;
		}

		/* Latch the OCR response byte immediately — the volatile SD_CMD
		 * register can be overwritten by the hardware (e.g. card-detect
		 * re-sampling) before we use it after the loop. */
		ocr_resp1 = host_ops->GetResponse(SDIO_RESP1);

		if (ocr_resp1 & BIT7) {
			break;
		}

		SDCard_DelayUs(10000);
	}

	if (ret != HAL_OK) {
		return ret;
	}

	if (!(ocr_resp1 & BIT7)) {
		RTK_LOGE(TAG, "ACMD41 power-up timeout (OCR=0x%02X)\n", ocr_resp1);
		return HAL_TIMEOUT;
	}

	if (ocr_resp1 & BIT6) {
		card_info.is_sdhc_sdxc = 1;
		RTK_LOGS(TAG, RTK_LOG_DEBUG, "This is a SDHC/SDXC card...\r\n");

		if ((ocr_resp1 & BIT0) && !sd_adapter.force_33v) {
			RTK_LOGS(TAG, RTK_LOG_DEBUG, "Card accepted 1.8V switch (S18A=1)\n");
			/* Actual voltage switch is done by ameba_sd.c after GetOCR returns */
			card_info.sig_level = SD_HOST_BUS_SIGNALING_LEVEL_1p8V;
			card_info.bus_spd = SD_SPEED_SDR12;
		} else {
			card_info.sig_level = SD_HOST_BUS_SIGNALING_LEVEL_3p3V;
			card_info.bus_spd = SD_SPEED_DS;
			/* Only notable when the card declined; staying 3.3V for a DS/HS
			 * (force_33v) request is expected, so keep it quiet. */
			if (!sd_adapter.force_33v) {
				RTK_LOGS(TAG, RTK_LOG_DEBUG, "Card rejected 1.8V, staying at 3.3V\n");
			}
		}
	} else {
		card_info.is_sdhc_sdxc = 0;
		card_info.sig_level = SD_HOST_BUS_SIGNALING_LEVEL_3p3V;
		card_info.bus_spd = SD_SPEED_DS;
		RTK_LOGS(TAG, RTK_LOG_DEBUG, "This is a SDSC card...\r\n");
	}

	return HAL_OK;
}

/** @brief CMD2 ALL_SEND_CID — read + log the card identification. */
u32 SDCard_GetCID(void)
{
	u32 ret;
	u8 *pbuf_raw = NULL;
	u8 *pbuf = SDCard_AllocAlignedBuf(&pbuf_raw);
	SD_HOST_DMA_CTL dma_cfg;
	SD_HOST_CmdTypeDef cmd_attr;

	if (pbuf == NULL) {
		RTK_LOGE(TAG, "Allocate buffer error !!\r\n");
		return HAL_ERR_MEM;
	}

	do {
		_memset((void *)(pbuf), 0, SD_HOST_BUFFER_LENGTH);
		DCache_CleanInvalidate((u32)pbuf, SD_HOST_BUFFER_LENGTH);

		dma_cfg.op = DMA_Read;
		dma_cfg.start_addr = ((u32)(pbuf)) / 8;
		dma_cfg.blk_cnt = 1;
		dma_cfg.type = DMA_R2;
		host_ops->DMAConfig(&dma_cfg);

		cmd_attr.arg = 0;
		cmd_attr.idx = SD_CMD_AllSendCid;
		cmd_attr.rsp_type = SD_HOST_RSP_17B;
		cmd_attr.rsp_crc_chk = DISABLE;
		cmd_attr.data_present = SD_HOST_NO_DATA;
		ret = host_ops->SendCommand(&cmd_attr, 0);
		if (ret != HAL_OK) {
			break;
		}

		ret = host_ops->WaitDMADone(SD_HOST_TRANSFER_CPLT_TIMEOUT);
		if (ret != HAL_OK) {
			break;
		}

		DCache_Invalidate((u32)pbuf, SD_HOST_BUFFER_LENGTH);

		ret = SDCard_CheckR2Response(pbuf);
		if (ret != HAL_OK) {
			break;
		}

		card_info.serial = ((u32)pbuf[10] << 24) | ((u32)pbuf[11] << 16) | ((u32)pbuf[12] << 8) | pbuf[13];

		RTK_LOGS(TAG, RTK_LOG_DEBUG, "Manufacturer ID: %u\r\n", pbuf[1]);
		RTK_LOGS(TAG, RTK_LOG_DEBUG, "OEM/Application ID: %c%c\r\n", pbuf[2], pbuf[3]);
		RTK_LOGS(TAG, RTK_LOG_DEBUG, "Product name: %c%c%c%c%c\r\n", pbuf[4], pbuf[5], pbuf[6], pbuf[7], pbuf[8]);
		RTK_LOGS(TAG, RTK_LOG_DEBUG, "Product serial number: %02X%02X%02X%02X\r\n", pbuf[10], pbuf[11], pbuf[12], pbuf[13]);
		RTK_LOGS(TAG, RTK_LOG_DEBUG, "Manufacturing date: %u/%u\r\n", 2000 + (((pbuf[14] & 0xF) << 4) | (pbuf[15] >> 4)), pbuf[15] & 0xF);
	} while (0);

	rtos_mem_free(pbuf_raw);
	return ret;
}

/** @brief CMD3 — card publishes its relative address (RCA). */
u32 SDCard_GetRCA(void)
{
	u32 ret;
	SD_HOST_CmdTypeDef cmd_attr;

	cmd_attr.arg = 0;
	cmd_attr.idx = SD_CMD_SendRelAddr;
	cmd_attr.rsp_type = SD_HOST_RSP_6B;
	cmd_attr.rsp_crc_chk = ENABLE;
	cmd_attr.data_present = SD_HOST_NO_DATA;
	ret = host_ops->SendCommand(&cmd_attr, SD_HOST_CMD_CPLT_TIMEOUT);
	if (ret != HAL_OK) {
		return ret;
	}

	ret = SDCard_CmdRespError(SDIOH_RESP_R6, SD_CMD_SendRelAddr);
	if (ret != HAL_OK) {
		return ret;
	}

	card_info.rca = (host_ops->GetResponse(SDIO_RESP1) << 8) | (host_ops->GetResponse(SDIO_RESP2));
	RTK_LOGS(TAG, RTK_LOG_DEBUG, "RCA = %04X\r\n", card_info.rca);

	return HAL_OK;
}

/** @brief CMD9 SEND_CSD — read CSD, derive capacity + block lengths. */
u32 SDCard_GetCSD(void)
{
	u32 ret;
	u8 *pbuf_raw = NULL;
	u8 *pbuf = SDCard_AllocAlignedBuf(&pbuf_raw);
	SD_HOST_DMA_CTL dma_cfg;
	SD_HOST_CmdTypeDef cmd_attr;
	u32 c_size, n;

	if (pbuf == NULL) {
		RTK_LOGE(TAG, "Allocate buffer error !!\r\n");
		return HAL_ERR_MEM;
	}

	do {
		_memset((void *)(pbuf), 0, SD_HOST_BUFFER_LENGTH);
		DCache_CleanInvalidate((u32)pbuf, SD_HOST_BUFFER_LENGTH);

		dma_cfg.op = DMA_Read;
		dma_cfg.start_addr = ((u32)(pbuf)) / 8;
		dma_cfg.blk_cnt = 1;
		dma_cfg.type = DMA_R2;
		host_ops->DMAConfig(&dma_cfg);

		cmd_attr.arg = (card_info.rca) << 16;
		cmd_attr.idx = SD_CMD_SendCsd;
		cmd_attr.rsp_type = SD_HOST_RSP_17B;
		cmd_attr.rsp_crc_chk = DISABLE;
		cmd_attr.data_present = SD_HOST_NO_DATA;
		ret = host_ops->SendCommand(&cmd_attr, 0);
		if (ret != HAL_OK) {
			break;
		}

		ret = host_ops->WaitDMADone(SD_HOST_TRANSFER_CPLT_TIMEOUT);
		if (ret != HAL_OK) {
			break;
		}

		DCache_Invalidate((u32)pbuf, SD_HOST_BUFFER_LENGTH);

		ret = SDCard_CheckR2Response(pbuf);
		if (ret != HAL_OK) {
			break;
		}

		_memcpy(card_info.csd, pbuf + 1, SD_HOST_CSD_LENGTH);

		if (card_info.csd[0] >> 6) {
			c_size = (((card_info.csd[7] & 0x3F) << 16) | (card_info.csd[8] << 8) | (card_info.csd[9])) + 1;
			card_info.capacity = c_size << 9;  //KB

			RTK_LOGS(TAG, RTK_LOG_DEBUG, "CSD Version:2.0\r\n");
			RTK_LOGS(TAG, RTK_LOG_DEBUG, "User data area capacity: %u GB\r\n", (u32)((u64)card_info.capacity * 1024 / 1000000000));
		} else {
			c_size = (((card_info.csd[6] & 0x3) << 10) | (card_info.csd[7] << 2) | (card_info.csd[8] >> 6)) + 1;
			n = ((((card_info.csd[9] & 0x3) << 1) | (card_info.csd[10] >> 7)) + 2) + (card_info.csd[5] & 0xF);

			card_info.capacity = (u32)(c_size << (n - 10));  //KB

			RTK_LOGS(TAG, RTK_LOG_DEBUG, "CSD Version:1.0\r\n");
			RTK_LOGS(TAG, RTK_LOG_DEBUG, "User data area capacity: %u MB\r\n", (u32)((u64)card_info.capacity * 1024 / 1000000));
		}

		card_info.read_bl_len = 1 << (card_info.csd[5] & 0xF);
		card_info.write_bl_len = 1 << (((card_info.csd[12] & 0x3) << 2) | (card_info.csd[13] >> 6));

		RTK_LOGS(TAG, RTK_LOG_DEBUG, "Max. read data block length: %u Bytes\r\n", card_info.read_bl_len);
		RTK_LOGS(TAG, RTK_LOG_DEBUG, "Max. write data block length: %u Bytes\r\n", card_info.write_bl_len);
	} while (0);

	rtos_mem_free(pbuf_raw);
	return ret;
}

/** @brief CMD7 select (ENABLE) / deselect the card. */
u32 SDCard_SelectDeselect(u8 select)
{
	u32 ret;
	SD_HOST_CmdTypeDef cmd_attr;

	if (select == ENABLE) {
		cmd_attr.arg = (card_info.rca) << 16;
		cmd_attr.idx = SD_CMD_SelDeselCard;
		cmd_attr.rsp_type = SD_HOST_RSP_6B;
		cmd_attr.rsp_crc_chk = ENABLE;
		cmd_attr.data_present = SD_HOST_NO_DATA;
		ret = host_ops->SendCommand(&cmd_attr, SD_HOST_CMD_CPLT_TIMEOUT);
		if (ret != HAL_OK) {
			return ret;
		}

		ret = SDCard_CmdRespError(SDIOH_RESP_R1, SD_CMD_SelDeselCard);
		if (ret != HAL_OK) {
			return ret;
		}
	} else {
		cmd_attr.arg = 0;
		cmd_attr.idx = SD_CMD_SelDeselCard;
		cmd_attr.rsp_type = SD_HOST_NO_RESP;
		cmd_attr.rsp_crc_chk = DISABLE;
		cmd_attr.data_present = SD_HOST_NO_DATA;
		ret = host_ops->SendCommand(&cmd_attr, SD_HOST_CMD_CPLT_TIMEOUT);
		if (ret != HAL_OK) {
			return ret;
		}

		ret = SDCard_CmdRespError(SD_HOST_NO_RESP, SD_CMD_SelDeselCard);
		if (ret != HAL_OK) {
			return ret;
		}
	}

	return HAL_OK;
}

/** @brief ACMD6 SET_BUS_WIDTH (1- or 4-bit), then mirror it on the host. */
u32 SDCard_SetBusWidth(u8 bus_width)
{
	u32 ret;
	SD_HOST_CmdTypeDef cmd_attr;

	if (bus_width > SDIOH_BUS_WIDTH_4BIT) {
		return HAL_ERR_PARA;
	}

	if (host_ops->GetBusWidth() == bus_width) {
		RTK_LOGS(TAG, RTK_LOG_DEBUG, "Current SD bus width is already the specified setting...\r\n");
		return HAL_OK;
	}

	/* ACMD6 = CMD55 then CMD6 */
	cmd_attr.arg = (card_info.rca) << 16;
	cmd_attr.idx = SD_CMD_AppCmd;
	cmd_attr.rsp_type = SD_HOST_RSP_6B;
	cmd_attr.rsp_crc_chk = ENABLE;
	cmd_attr.data_present = SD_HOST_NO_DATA;
	ret = host_ops->SendCommand(&cmd_attr, SD_HOST_CMD_CPLT_TIMEOUT);
	if (ret != HAL_OK) {
		return ret;
	}

	ret = SDCard_CmdRespError(SDIOH_RESP_R1, SD_CMD_AppCmd);
	if (ret != HAL_OK) {
		return ret;
	}

	if (bus_width == SDIOH_BUS_WIDTH_4BIT) {
		cmd_attr.arg = 0x2;  // 4-bit bus
	} else {
		cmd_attr.arg = 0x0;  // 1-bit bus
	}
	cmd_attr.idx = SD_CMD_SetBusWidth;
	cmd_attr.rsp_type = SD_HOST_RSP_6B;
	cmd_attr.rsp_crc_chk = ENABLE;
	cmd_attr.data_present = SD_HOST_NO_DATA;
	ret = host_ops->SendCommand(&cmd_attr, SD_HOST_CMD_CPLT_TIMEOUT);
	if (ret != HAL_OK) {
		return ret;
	}

	ret = SDCard_CmdRespError(SDIOH_RESP_R1, SD_CMD_SetBusWidth);
	if (ret != HAL_OK) {
		return ret;
	}

	RTK_LOGS(TAG, RTK_LOG_DEBUG, "SD card changes to the specified bus width mode successfully\r\n");

	host_ops->SetBusWidth(bus_width);

	return HAL_OK;
}

/**
  * @brief  Get the SD Configuration Register (SCR) via ACMD51.
  * @retval  HAL_OK on success, others on failure.
  */
u32 SDCard_GetSCR(void)
{
	u32 ret;
	u8 *pbuf_raw = NULL;
	u8 *pbuf = SDCard_AllocAlignedBuf(&pbuf_raw);
	SD_HOST_DMA_CTL dma_cfg;
	SD_HOST_CmdTypeDef cmd_attr;

	if (pbuf == NULL) {
		RTK_LOGE(TAG, "Allocate buffer error !!\r\n");
		return HAL_ERR_MEM;
	}

	do {
		/***** ACMD51 (CMD55) *****/
		cmd_attr.arg = (card_info.rca) << 16;
		cmd_attr.idx = SD_CMD_AppCmd;
		cmd_attr.rsp_type = SD_HOST_RSP_6B;
		cmd_attr.rsp_crc_chk = ENABLE;
		cmd_attr.data_present = SD_HOST_NO_DATA;
		ret = host_ops->SendCommand(&cmd_attr, SD_HOST_CMD_CPLT_TIMEOUT);
		if (ret != HAL_OK) {
			break;
		}

		ret = SDCard_CmdRespError(SDIOH_RESP_R1, SD_CMD_AppCmd);
		if (ret != HAL_OK) {
			break;
		}

		/***** ACMD51 (CMD51) *****/
		_memset((void *)(pbuf), 0, SD_HOST_BUFFER_LENGTH);
		DCache_CleanInvalidate((u32)pbuf, SD_HOST_BUFFER_LENGTH);

		dma_cfg.op = DMA_Read;
		dma_cfg.start_addr = ((u32)(pbuf)) / 8;
		dma_cfg.blk_cnt = 1;
		dma_cfg.type = DMA_64B;
		host_ops->DMAConfig(&dma_cfg);

		cmd_attr.arg = 0;
		cmd_attr.idx = SD_CMD_SendScr;
		cmd_attr.rsp_type = SD_HOST_RSP_6B;
		cmd_attr.rsp_crc_chk = ENABLE;
		cmd_attr.data_present = SD_HOST_DATA_EXIST;
		ret = host_ops->SendCommand(&cmd_attr, 0);
		if (ret != HAL_OK) {
			break;
		}

		ret = host_ops->WaitDMADone(SD_HOST_TRANSFER_CPLT_TIMEOUT);
		if (ret != HAL_OK) {
			SDCard_StopTransfer();
			ret = HAL_ERR_UNKNOWN;
			break;
		}

		DCache_Invalidate((u32)pbuf, SD_HOST_BUFFER_LENGTH);

		ret = SDCard_CmdRespError(SDIOH_RESP_R1, SD_CMD_SendScr);
		if (ret != HAL_OK) {
			break;
		}

		switch (pbuf[0] & 0xF) {
		case 2:
			if (pbuf[2] >> 7) {
				RTK_LOGS(TAG, RTK_LOG_DEBUG, "SD specification version: 3.0X\r\n");
				card_info.sd_spec_ver = SD_SPEC_V300;
			} else {
				RTK_LOGS(TAG, RTK_LOG_DEBUG, "SD specification version: 2.00\r\n");
				card_info.sd_spec_ver = SD_SPEC_V200;
			}
			break;
		case 1:
			RTK_LOGS(TAG, RTK_LOG_DEBUG, "SD specification version: 1.10\r\n");
			card_info.sd_spec_ver = SD_SPEC_V110;
			break;
		case 0:
			RTK_LOGS(TAG, RTK_LOG_DEBUG, "SD specification version: 1.01\r\n");
			card_info.sd_spec_ver = SD_SPEC_V101;
			break;
		default:
			RTK_LOGW(TAG, "SD specification version: Unknown\r\n");
			card_info.sd_spec_ver = 0xFF;
		}
	} while (0);

	rtos_mem_free(pbuf_raw);
	return ret;
}

/** @brief ACMD13 SD_STATUS — read the 512-bit SD Status and cache SPEED_CLASS,
  *        UHS_SPEED_GRADE and VIDEO_SPEED_CLASS for the init summary. */
u32 SDCard_GetStatus(void)
{
	u32 ret;
	u8 *pbuf_raw = NULL;
	u8 *pbuf = SDCard_AllocAlignedBuf(&pbuf_raw);
	SD_HOST_DMA_CTL dma_cfg;
	SD_HOST_CmdTypeDef cmd_attr;

	if (pbuf == NULL) {
		return HAL_ERR_MEM;
	}

	do {
		/* ACMD13 = CMD55 then CMD13 with data */
		cmd_attr.arg = ((u32)card_info.rca) << 16;
		cmd_attr.idx = SD_CMD_AppCmd;
		cmd_attr.rsp_type = SD_HOST_RSP_6B;
		cmd_attr.rsp_crc_chk = ENABLE;
		cmd_attr.data_present = SD_HOST_NO_DATA;
		ret = host_ops->SendCommand(&cmd_attr, SD_HOST_CMD_CPLT_TIMEOUT);
		if (ret != HAL_OK) {
			break;
		}

		ret = SDCard_CmdRespError(SDIOH_RESP_R1, SD_CMD_AppCmd);
		if (ret != HAL_OK) {
			break;
		}

		_memset((void *)(pbuf), 0, SD_HOST_BUFFER_LENGTH);
		DCache_CleanInvalidate((u32)pbuf, SD_HOST_BUFFER_LENGTH);

		dma_cfg.op = DMA_Read;
		dma_cfg.start_addr = ((u32)(pbuf)) / 8;
		dma_cfg.blk_cnt = 1;
		dma_cfg.type = DMA_64B;
		host_ops->DMAConfig(&dma_cfg);

		cmd_attr.arg = 0;
		cmd_attr.idx = SD_CMD_SendSts;
		cmd_attr.rsp_type = SD_HOST_RSP_6B;
		cmd_attr.rsp_crc_chk = ENABLE;
		cmd_attr.data_present = SD_HOST_DATA_EXIST;
		ret = host_ops->SendCommand(&cmd_attr, 0);
		if (ret != HAL_OK) {
			break;
		}

		ret = host_ops->WaitDMADone(SD_HOST_TRANSFER_CPLT_TIMEOUT);
		if (ret != HAL_OK) {
			SDCard_StopTransfer();
			ret = HAL_ERR_UNKNOWN;
			break;
		}

		DCache_Invalidate((u32)pbuf, SD_HOST_BUFFER_LENGTH);

		ret = SDCard_CmdRespError(SDIOH_RESP_R1, SD_CMD_SendSts);
		if (ret != HAL_OK) {
			break;
		}

		/* SD Status is sent MSB-first: pbuf[0] = bits[511:504]. */
		card_info.speed_class = pbuf[8];          /* SPEED_CLASS       bits[447:440] */
		card_info.uhs_grade   = pbuf[14] >> 4;    /* UHS_SPEED_GRADE   bits[399:396] */
		card_info.video_class = pbuf[15];         /* VIDEO_SPEED_CLASS bits[391:384] */
		card_info.app_class   = pbuf[21] & 0x0F;  /* APP_PERF_CLASS    bits[339:336] */

		RTK_LOGS(TAG, RTK_LOG_DEBUG, "SD Status: speed_class=0x%02X uhs_grade=%u video_class=%u app_class=%u\n",
				 card_info.speed_class, card_info.uhs_grade, card_info.video_class, card_info.app_class);
	} while (0);

	rtos_mem_free(pbuf_raw);
	return ret;
}

/**
  * @brief  Checks switchable function (mode 0) and switch card function (mode 1).
  * @param  mode: can be a value of @ref SD_CMD6_operation_mode
  * @param  speed: can be a value of @SD_CMD6_Function_Group1
  * @param  buf_32align: pointer to a buffer to save the switch function status
  * @retval  HAL_OK: Check/Set function successfully
  *		   Others: Fail to get check/Set function
  */
u32 SDCard_SwitchFunction(u8 mode, u8 speed, u8 *buf_32align)
{
	u32 ret;
	SD_HOST_DMA_CTL dma_cfg;
	SD_HOST_CmdTypeDef cmd_attr;

	if ((buf_32align == NULL) || (((u32)buf_32align) & 0x1F)) {
		return HAL_ERR_PARA;
	}

	/***** CMD6 *****/
	_memset((void *)buf_32align, 0, SD_HOST_BUFFER_LENGTH);
	DCache_CleanInvalidate((u32)buf_32align, SD_HOST_BUFFER_LENGTH);

	dma_cfg.op = DMA_Read;
	dma_cfg.start_addr = ((u32)buf_32align) / 8;
	dma_cfg.blk_cnt = 1;
	dma_cfg.type = DMA_64B;
	host_ops->DMAConfig(&dma_cfg);

	cmd_attr.arg = (mode << 31) | (0xF << 20) | (0xF << 16) | (0xF << 12) | (0xF << 8) | (0xF << 4) | speed;
	cmd_attr.idx = SD_CMD_SwitchFunc;
	cmd_attr.rsp_type = SD_HOST_RSP_6B;
	cmd_attr.rsp_crc_chk = ENABLE;
	cmd_attr.data_present = SD_HOST_DATA_EXIST;
	ret = host_ops->SendCommand(&cmd_attr, 0);
	if (ret != HAL_OK) {
		return ret;
	}

	ret = host_ops->WaitDMADone(SD_HOST_TRANSFER_CPLT_TIMEOUT);
	if (ret != HAL_OK) {
		ret = SDCard_StopTransfer();
		if (ret != HAL_OK) {
			RTK_LOGE(TAG,  "Stop transmission error !!\r\n");
		}

		return HAL_ERR_UNKNOWN;
	}

	DCache_Invalidate((u32)buf_32align, SD_HOST_BUFFER_LENGTH);

	ret = SDCard_CmdRespError(SDIOH_RESP_R1, SD_CMD_SwitchFunc);
	if (ret != HAL_OK) {
		return ret;
	}

	return HAL_OK;
}

/**
  * @brief  Query card supported speed modes via CMD6 check mode.
  * @param  pbuf: 32-byte aligned DMA buffer for CMD6 response.
  * @retval  HAL_OK on success, others on failure.
  */
u32 SDCard_QuerySupportedSpeeds(u8 *pbuf)
{
	u32 ret;
	u8 support_speed;

	/* Query supported functions */
	ret = SDCard_SwitchFunction(SD_CMD6_CHECK_MODE, SD_KEEP_CUR_SPEED, pbuf);
	if (ret != HAL_OK) {
		return ret;
	}

	support_speed = pbuf[13];

	/* Log card's max supported speed from CMD6 function group 1 bitmask */
	{
		static const char *const func_names[] = {
			"DS/SDR12", "HS/SDR25", "SDR50", "SDR104", "DDR50"
		};
		const char *max_name = "DS/SDR12";
		/* Scan from highest to lowest to find max supported */
		for (int f = 4; f >= 0; f--) {
			if (support_speed & (1 << f)) {
				max_name = func_names[f];
				break;
			}
		}
		RTK_LOGS(TAG, RTK_LOG_DEBUG, "Card supported speeds: 0x%02X (max: %s)\n", support_speed, max_name);
	}

	return HAL_OK;
}

/**
 *  @brief To read one block from the SD card.
 *  @param  BlockIdx: The start block to begin to read from the card.
 *  @param  readbuff: The buffer to read data blocks (must be 32-Byte alignment).
 *  @retval  HAL_OK: Read data successfully
 *  		   Others: Fail to read data
 */
u32 SDCard_ReadBlock(uint8_t *readbuff, uint32_t BlockIdx)
{
	u32 ret, start;
	SD_HOST_DMA_CTL dma_cfg;
	SD_HOST_CmdTypeDef cmd_attr;

	assert_param((readbuff != NULL) && ((((u32)readbuff) & 0x1F) == 0));

	if (card_info.is_sdhc_sdxc) {
		start = (u32)BlockIdx;
	} else {
		start = (u32)(BlockIdx * SD_BLOCK_SIZE);
	}

	/***** CMD17 *****/
	DCache_CleanInvalidate((u32)readbuff, SD_BLOCK_SIZE);

	card_info.wait_interrupts = SD_HOST_CARD_END | SD_HOST_DMA_TRANSFER_DONE;

	dma_cfg.op = DMA_Read;
	dma_cfg.start_addr = ((u32)readbuff) / 8;
	dma_cfg.blk_cnt = 1;
	dma_cfg.type = DMA_Normal;
	host_ops->DMAConfig(&dma_cfg);

	cmd_attr.arg = start;
	cmd_attr.idx = SD_CMD_RdSingleBlk;
	cmd_attr.rsp_type = SD_HOST_RSP_6B;
	cmd_attr.rsp_crc_chk = ENABLE;
	cmd_attr.data_present = SD_HOST_DATA_EXIST;
	ret = host_ops->SendCommand(&cmd_attr, 0);
	if (ret != HAL_OK) {
		RTK_LOGE(TAG, "Send CMD17 error !!\r\n");
		return ret;
	}

	ret = SDCard_chk_xfer_int(SD_HOST_READ_TIMEOUT);

	if (ret != HAL_OK) {
		ret = SDCard_StopTransfer();
		if (ret != HAL_OK) {
			RTK_LOGE(TAG, "Stop transmission error !!\r\n");
		}

		return HAL_ERR_UNKNOWN;
	}

	DCache_Invalidate((u32)readbuff, SD_BLOCK_SIZE);

	ret = SDCard_CmdRespError(SDIOH_RESP_R1, SD_CMD_RdSingleBlk);

	return ret;
}

/**
 *  @brief To read multi-block from the SD card.
 *  @param  BlockIdx: The start block to begin to read from the card.
 *  @param  readbuff: The buffer to read data blocks (must be 32-Byte alignment).
 *  @param  NumberOfBlocks: the number of blocks to be read
 *  @retval  HAL_OK: Read data successfully
 *  		   Others: Fail to read data
 */
u32 SDCard_ReadMultiBlocks(uint8_t *readbuff, uint32_t BlockIdx, uint32_t NumberOfBlocks)
{
	u32 ret, start;
	SD_HOST_DMA_CTL dma_cfg;
	SD_HOST_CmdTypeDef cmd_attr;

	assert_param(NumberOfBlocks > 1);
	assert_param((readbuff != NULL) && ((((u32)readbuff) & 0x1F) == 0));

	if (card_info.is_sdhc_sdxc) {
		start = (u32)BlockIdx;
	} else {
		start = (u32)(BlockIdx * SD_BLOCK_SIZE);
	}

	/***** CMD18 *****/
	DCache_CleanInvalidate((u32)readbuff, NumberOfBlocks * SD_BLOCK_SIZE);

	card_info.wait_interrupts = SD_HOST_CARD_END | SD_HOST_DMA_TRANSFER_DONE;

	dma_cfg.op = DMA_Read;
	dma_cfg.start_addr = ((u32)readbuff) / 8;
	dma_cfg.blk_cnt = NumberOfBlocks;
	dma_cfg.type = DMA_Normal;
	host_ops->DMAConfig(&dma_cfg);

	cmd_attr.arg = start;
	cmd_attr.idx = SD_CMD_RdMulBlk;
	cmd_attr.rsp_type = SD_HOST_RSP_6B;
	cmd_attr.rsp_crc_chk = ENABLE;
	cmd_attr.data_present = SD_HOST_DATA_EXIST;
	ret = host_ops->SendCommand(&cmd_attr, 0);
	if (ret != HAL_OK) {
		return ret;
	}

	ret = SDCard_chk_xfer_int(SD_HOST_READ_TIMEOUT * NumberOfBlocks);
	if (ret != HAL_OK) {
		return ret;
	}

	DCache_Invalidate((u32)readbuff, NumberOfBlocks * SD_BLOCK_SIZE);

	ret = SDCard_CmdRespError(SDIOH_RESP_R1, SD_CMD_RdMulBlk);

	return ret;

}

/**
 *  @brief To write one block to the SD card.
 *  @param  BlockIdx: The start block to begin writing to the card.
 *  @param  writebuff: The buffer to write data blocks (must be 32-Byte alignment).
 *  @retval  HAL_OK: Write data successfully
 *  		   Others: Fail to write data
 */
u32 SDCard_WriteBlock(uint8_t *writebuff, uint32_t BlockIdx)
{
	u32 ret, start;
	SD_HOST_DMA_CTL dma_cfg;
	SD_HOST_CmdTypeDef cmd_attr;
	assert_param((writebuff != NULL) && ((((u32)writebuff) & 0x1F) == 0));

	if (card_info.is_sdhc_sdxc) {
		start = (u32)BlockIdx;
	} else {
		start = (u32)(BlockIdx * SD_BLOCK_SIZE);
	}

	/***** CMD24 *****/
	DCache_CleanInvalidate((u32)writebuff, SD_BLOCK_SIZE);

	card_info.wait_interrupts = SD_HOST_CARD_END | SD_HOST_DMA_TRANSFER_DONE;

	dma_cfg.op = DMA_Write;
	dma_cfg.start_addr = ((u32)writebuff) / 8;
	dma_cfg.blk_cnt = 1;
	dma_cfg.type = DMA_Normal;
	host_ops->DMAConfig(&dma_cfg);

	cmd_attr.arg = start;
	cmd_attr.idx = SD_CMD_WrBlk;
	cmd_attr.rsp_type = SD_HOST_RSP_6B;
	cmd_attr.rsp_crc_chk = ENABLE;
	cmd_attr.data_present = SD_HOST_DATA_EXIST;
	ret = host_ops->SendCommand(&cmd_attr, 0);
	if (ret != HAL_OK) {
		return ret;
	}

	ret = SDCard_chk_xfer_int(SD_HOST_WRITE_TIMEOUT);
	if (ret != HAL_OK) {
		ret = SDCard_StopTransfer();
		if (ret != HAL_OK) {
			RTK_LOGE(TAG, "Stop transmission error !!\r\n");
		}

		return HAL_ERR_UNKNOWN;
	}

	ret = SDCard_CmdRespError(SDIOH_RESP_R1, SD_CMD_WrBlk);

	return ret;

}

/**
 *  @brief To write multi-block to the SD card.
 *  @param  BlockIdx: The start block to begin writing to the card.
 *  @param  NumberOfBlocks: The block count.
 *  @param  writebuff: The buffer to write data blocks (must be 32-Byte alignment).
 *  @retval  HAL_OK: Write data successfully
 *  		   Others: Fail to write data
 */
u32 SDCard_WriteMultiBlocks(uint8_t *writebuff, uint32_t BlockIdx, uint32_t NumberOfBlocks)
{
	u32 start, ret;
	SD_HOST_DMA_CTL dma_cfg;
	SD_HOST_CmdTypeDef cmd_attr;

	assert_param(NumberOfBlocks > 1);
	assert_param((writebuff != NULL) && ((((u32)writebuff) & 0x1F) == 0));

	if (card_info.is_sdhc_sdxc) {
		start = (u32)BlockIdx;
	} else {
		start = (u32)(BlockIdx * SD_BLOCK_SIZE);
	}

#if defined(SDIO) && (SDIO == SD)
	/***** ACMD23 (CMD55) *****/
	cmd_attr.arg = (card_info.rca) << 16;
	cmd_attr.idx = SD_CMD_AppCmd;
	cmd_attr.rsp_type = SD_HOST_RSP_6B;
	cmd_attr.rsp_crc_chk = ENABLE;
	cmd_attr.data_present = SD_HOST_NO_DATA;
	ret = host_ops->SendCommand(&cmd_attr, SD_HOST_CMD_CPLT_TIMEOUT);
	if (ret != HAL_OK) {
		return ret;
	}

	ret = SDCard_CmdRespError(SDIOH_RESP_R1, SD_CMD_AppCmd);
	if (ret != HAL_OK) {
		return ret;
	}

	/***** ACMD23 (CMD23) *****/
	cmd_attr.arg = NumberOfBlocks;
	cmd_attr.idx = SD_CMD_SetWrBlkEraseCnt;
	cmd_attr.rsp_type = SD_HOST_RSP_6B;
	cmd_attr.rsp_crc_chk = ENABLE;
	cmd_attr.data_present = SD_HOST_NO_DATA;
	ret = host_ops->SendCommand(&cmd_attr, SD_HOST_CMD_CPLT_TIMEOUT);
	if (ret != HAL_OK) {
		return ret;
	}

	ret = SDCard_CmdRespError(SDIOH_RESP_R1, SD_CMD_SetWrBlkEraseCnt);
	if (ret != HAL_OK) {
		return ret;
	}
#endif
	DCache_CleanInvalidate((u32)writebuff, NumberOfBlocks * SD_BLOCK_SIZE);

	/***** CMD25 *****/

	card_info.wait_interrupts = SD_HOST_CARD_END | SD_HOST_DMA_TRANSFER_DONE;

	dma_cfg.op = DMA_Write;
	dma_cfg.start_addr = ((u32)writebuff) / 8;
	dma_cfg.blk_cnt = NumberOfBlocks;
	dma_cfg.type = DMA_Normal;
	host_ops->DMAConfig(&dma_cfg);

	cmd_attr.arg = start;
	cmd_attr.idx = SD_CMD_WrMulBlk;
	cmd_attr.rsp_type = SD_HOST_RSP_6B;
	cmd_attr.rsp_crc_chk = ENABLE;
	cmd_attr.data_present = SD_HOST_DATA_EXIST;
	ret = host_ops->SendCommand(&cmd_attr, 0);
	if (ret != HAL_OK) {
		return ret;
	}

	ret = SDCard_chk_xfer_int(SD_HOST_WRITE_TIMEOUT * NumberOfBlocks);
	if (ret != HAL_OK) {
		return ret;
	}

	ret = SDCard_CmdRespError(SDIOH_RESP_R1, SD_CMD_WrMulBlk);

	return ret;

}


/**
  * @brief  Start a multi-block read DMA (non-blocking).
  *         Call SDCard_WaitDMA() to wait for completion, then DCache_Invalidate the buffer.
  * @param  readbuff: 32-byte aligned destination buffer.
  * @param  BlockIdx: starting block.
  * @param  NumberOfBlocks: number of blocks (>1).
  * @retval  HAL_OK if DMA started successfully.
  */
u32 SDCard_ReadMultiBlocks_Start(uint8_t *readbuff, uint32_t BlockIdx, uint32_t NumberOfBlocks)
{
	u32 ret, start;
	SD_HOST_DMA_CTL dma_cfg;
	SD_HOST_CmdTypeDef cmd_attr;

	if (card_info.is_sdhc_sdxc) {
		start = (u32)BlockIdx;
	} else {
		start = (u32)(BlockIdx * SD_BLOCK_SIZE);
	}

	card_info.wait_interrupts = SD_HOST_CARD_END | SD_HOST_DMA_TRANSFER_DONE;

	dma_cfg.op = DMA_Read;
	dma_cfg.start_addr = ((u32)readbuff) / 8;
	dma_cfg.blk_cnt = NumberOfBlocks;
	dma_cfg.type = DMA_Normal;
	host_ops->DMAConfig(&dma_cfg);

	/* AutoRd1: CMD18 + response + read data + auto CMD12 */
	cmd_attr.arg = start;
	cmd_attr.idx = SD_CMD_RdMulBlk;
	cmd_attr.rsp_type = SD_HOST_RSP_6B;
	cmd_attr.rsp_crc_chk = ENABLE;
	cmd_attr.data_present = SD_HOST_DATA_EXIST;
	ret = host_ops->SendCommand(&cmd_attr, 0);

	return ret;
}

/**
  * @brief  Start a multi-block write DMA (non-blocking).
  * @param  writebuff: 32-byte aligned source buffer (already cache-cleaned).
  * @param  BlockIdx: starting block.
  * @param  NumberOfBlocks: number of blocks (>1).
  * @retval  HAL_OK if DMA started successfully.
  */
u32 SDCard_WriteMultiBlocks_Start(uint8_t *writebuff, uint32_t BlockIdx, uint32_t NumberOfBlocks)
{
	u32 ret, start;
	SD_HOST_DMA_CTL dma_cfg;
	SD_HOST_CmdTypeDef cmd_attr;

	if (card_info.is_sdhc_sdxc) {
		start = (u32)BlockIdx;
	} else {
		start = (u32)(BlockIdx * SD_BLOCK_SIZE);
	}

	card_info.wait_interrupts = SD_HOST_CARD_END | SD_HOST_DMA_TRANSFER_DONE;

	dma_cfg.op = DMA_Write;
	dma_cfg.start_addr = ((u32)writebuff) / 8;
	dma_cfg.blk_cnt = NumberOfBlocks;
	dma_cfg.type = DMA_Normal;
	host_ops->DMAConfig(&dma_cfg);

	/* AutoWr1: CMD25 + response + write data + auto CMD12 + busy wait */
	cmd_attr.arg = start;
	cmd_attr.idx = SD_CMD_WrMulBlk;
	cmd_attr.rsp_type = SD_HOST_RSP_6B;
	cmd_attr.rsp_crc_chk = ENABLE;
	cmd_attr.data_present = SD_HOST_DATA_EXIST;
	ret = host_ops->SendCommand(&cmd_attr, 0);

	return ret;
}

/**
  * @brief  Wait for a previously started async DMA transfer to complete.
  * @param  cmd: the SD command that started the transfer (SD_CMD_RdMulBlk or SD_CMD_WrMulBlk).
  * @param  timeout_us: timeout in microseconds.
  * @retval  HAL_OK on success.
  */
u32 SDCard_WaitDMA(u8 cmd, u32 timeout_us)
{
	u32 ret;

	ret = SDCard_chk_xfer_int(timeout_us);
	if (ret != HAL_OK) {
		return ret;
	}

	ret = SDCard_CmdRespError(SDIOH_RESP_R1, cmd);
	return ret;
}

/**
  * @brief  Erase a range of blocks on the SD card (CMD32 + CMD33 + CMD38).
  * @param  StartBlock: first block to erase.
  * @param  EndBlock: last block to erase (inclusive).
  * @retval  HAL_OK on success, others on failure.
  */
u32 SDCard_Erase(uint32_t StartBlock, uint32_t EndBlock)
{
	u32 ret;
	u32 start, end;
	SD_HOST_CmdTypeDef cmd_attr;

	if (StartBlock > EndBlock) {
		return HAL_ERR_PARA;
	}

	/* SDHC/SDXC: block address.  SDSC: byte address. */
	if (card_info.is_sdhc_sdxc) {
		start = StartBlock;
		end = EndBlock;
	} else {
		start = StartBlock * SD_BLOCK_SIZE;
		end = EndBlock * SD_BLOCK_SIZE;
	}

	/***** CMD32 — Erase Block Start *****/
	cmd_attr.arg = start;
	cmd_attr.idx = SD_CMD_EraseBlkSt;
	cmd_attr.rsp_type = SD_HOST_RSP_6B;
	cmd_attr.rsp_crc_chk = ENABLE;
	cmd_attr.data_present = SD_HOST_NO_DATA;
	ret = host_ops->SendCommand(&cmd_attr, SD_HOST_CMD_CPLT_TIMEOUT);
	if (ret != HAL_OK) {
		return ret;
	}
	ret = SDCard_CmdRespError(SDIOH_RESP_R1, SD_CMD_EraseBlkSt);
	if (ret != HAL_OK) {
		return ret;
	}

	/***** CMD33 — Erase Block End *****/
	cmd_attr.arg = end;
	cmd_attr.idx = SD_CMD_EraseBlkEd;
	cmd_attr.rsp_type = SD_HOST_RSP_6B;
	cmd_attr.rsp_crc_chk = ENABLE;
	cmd_attr.data_present = SD_HOST_NO_DATA;
	ret = host_ops->SendCommand(&cmd_attr, SD_HOST_CMD_CPLT_TIMEOUT);
	if (ret != HAL_OK) {
		return ret;
	}
	ret = SDCard_CmdRespError(SDIOH_RESP_R1, SD_CMD_EraseBlkEd);
	if (ret != HAL_OK) {
		return ret;
	}

	/***** CMD38 — Erase (timeout scales with block count) *****/
	cmd_attr.arg = 0;
	cmd_attr.idx = SD_CMD_Erase;
	cmd_attr.rsp_type = SD_HOST_RSP_6B;
	cmd_attr.rsp_crc_chk = ENABLE;
	cmd_attr.data_present = SD_HOST_NO_DATA;
	ret = host_ops->SendCommand(&cmd_attr, 250000U * (EndBlock - StartBlock + 1));
	if (ret != HAL_OK) {
		return ret;
	}
	ret = SDCard_CmdRespError(SDIOH_RESP_R1, SD_CMD_Erase);

	return ret;
}


/**
  * @brief  Initialize the card protocol layer (card_info, timer, callbacks).
  * @param  irq_handler: output pointer to receive the SD IRQ handler function.
  * @retval  None.
  */
void SDCard_ModuleInit(void)
{
	SDCard_PowerEn(0);
	_memset(&card_info, 0, sizeof(SD_CardInfo));
	card_info.sd_status = SD_NODISK;
}

