/*
 * Copyright (c) 2026 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/*
 * eMMC card protocol layer: CMD1/2/3/9 identification, EXT_CSD, bus width,
 * speed negotiation, erase.
 *
 * HS200 @ 100 MHz (emmc_try_hs200, gated on !sd_adapter.force_33v):
 *   VCCQ → 1.8 V (host-side rail only; eMMC has no CMD11) → CMD6 HS_TIMING=2
 *   → SDHost_ExecTuningEx (sd30 div0 100 MHz VP0/VP1 sweep) → single data probe.
 *   Any failure runs SDHost_ForceIdle + CMD12 + CMD6 HS_TIMING=BC + VCCQ → 3.3 V
 *   so EMMCCard_SpeedNegotiate falls back through DDR52 → HS52 → DS26.
 *
 * EMMC_HS200_PROBE_BLK (default LBA 100) validates the CMD25/CMD18 data
 * path on every HS200 init; its content is backed up and restored around
 * the probe so it's safe to reuse a live sector.
 */

#include "ameba_soc.h"
#include "ameba_emmc_card.h"
#include "os_wrapper.h"

static const char *TAG = "EMMC";

#define card_info    sd_adapter.card_info
#define host_ops     sd_adapter.host_ops

static u8 emmc_ext_csd_device_type;
static u8 emmc_ext_csd_rev;
static u8 emmc_ext_csd_csd_structure;

static u32 emmc_cmd6_switch_byte(u8 ext_csd_index, u8 value)
{
	SD_HOST_CmdTypeDef cmd_attr;
	u32 ret;

	cmd_attr.arg = EMMC_CMD6_BUILD(ext_csd_index, value);
	cmd_attr.idx = SD_CMD_SwitchFunc;
	cmd_attr.rsp_type = SD_HOST_RSP_6B;
	cmd_attr.rsp_crc_chk = ENABLE;
	cmd_attr.data_present = SD_HOST_NO_DATA;
	ret = host_ops->SendCommand(&cmd_attr, SD_HOST_CMD_CPLT_TIMEOUT);
	if (ret != HAL_OK) {
		return ret;
	}
	return SDCard_CmdRespError(SDIOH_RESP_R1, SD_CMD_SwitchFunc);
}

/**
  * @brief  CMD1 SEND_OP_COND. Loops until OCR busy bit clears (max ≈1s).
  */
u32 EMMCCard_GetOCR_CMD1(void)
{
	u32 ret;
	SD_HOST_CmdTypeDef cmd_attr;
	u32 retry = 100;
	u8 ocr_resp1 = 0;

	while (retry--) {
		cmd_attr.arg = EMMC_CMD1_ARG_SECTOR_3V3;
		cmd_attr.idx = EMMC_CMD_SendOpCond;
		cmd_attr.rsp_type = SD_HOST_RSP_6B;
		cmd_attr.rsp_crc_chk = DISABLE;
		cmd_attr.data_present = SD_HOST_NO_DATA;
		ret = host_ops->SendCommand(&cmd_attr, SD_HOST_CMD_CPLT_TIMEOUT);
		if (ret != HAL_OK) {
			return ret;
		}

		ret = SDCard_CmdRespError(SDIOH_RESP_R3, EMMC_CMD_SendOpCond);
		if (ret != HAL_OK) {
			return ret;
		}

		ocr_resp1 = host_ops->GetResponse(SDIO_RESP1);
		if (ocr_resp1 & BIT7) {
			break;
		}
		DelayUs(10000);
	}

	if (!(ocr_resp1 & BIT7)) {
		RTK_LOGE(TAG, "CMD1 power-up timeout (OCR=0x%02X)\n", ocr_resp1);
		return HAL_TIMEOUT;
	}

	RTK_LOGS(TAG, RTK_LOG_DEBUG, "Detected eMMC card\n");

	if (ocr_resp1 & BIT6) {
		card_info.is_sdhc_sdxc = 1;
		RTK_LOGS(TAG, RTK_LOG_DEBUG, "eMMC sector addressing (>2GB)\n");
	} else {
		card_info.is_sdhc_sdxc = 0;
		RTK_LOGS(TAG, RTK_LOG_DEBUG, "eMMC byte addressing (<2GB)\n");
	}

	card_info.sig_level = SD_HOST_BUS_SIGNALING_LEVEL_3p3V;
	card_info.bus_spd = SD_SPEED_DS;
	card_info.card_type = CARD_TYPE_EMMC;

	return HAL_OK;
}

/**
  * @brief  CMD2 ALL_SEND_CID parsed with eMMC field layout.
  *         eMMC differs from SD: 6-char PNM (vs 5), 1-byte OID (vs 2),
  *         1-byte MDT with year base 2013 for v4.5+ (vs SD's 12-bit, 2000 base).
  */
u32 EMMCCard_GetCID(void)
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
		_memset(pbuf, 0, SD_HOST_BUFFER_LENGTH);
		DCache_CleanInvalidate((u32)pbuf, SD_HOST_BUFFER_LENGTH);

		dma_cfg.op = DMA_Read;
		dma_cfg.start_addr = ((u32)pbuf) / 8;
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

		card_info.serial = ((u32)pbuf[11] << 24) | ((u32)pbuf[12] << 16) | ((u32)pbuf[13] << 8) | pbuf[14];

		RTK_LOGS(TAG, RTK_LOG_DEBUG, "Manufacturer ID: 0x%02X\n", pbuf[1]);
		RTK_LOGS(TAG, RTK_LOG_DEBUG, "OEM/Application ID: 0x%02X\n", pbuf[3]);
		RTK_LOGS(TAG, RTK_LOG_DEBUG, "Product name: %c%c%c%c%c%c\n",
				 pbuf[4], pbuf[5], pbuf[6], pbuf[7], pbuf[8], pbuf[9]);
		RTK_LOGS(TAG, RTK_LOG_DEBUG, "Product revision: %u.%u\n",
				 pbuf[10] >> 4, pbuf[10] & 0xF);
		RTK_LOGS(TAG, RTK_LOG_DEBUG, "Product serial number: %02X%02X%02X%02X\n",
				 pbuf[11], pbuf[12], pbuf[13], pbuf[14]);
		RTK_LOGS(TAG, RTK_LOG_DEBUG, "Manufacturing date: %u/%u\n",
				 2013U + (pbuf[15] >> 4), pbuf[15] & 0xF);
	} while (0);

	rtos_mem_free(pbuf_raw);
	return ret;
}

/**
  * @brief  CMD3 with host-assigned RCA. eMMC: host sets RCA in arg[31:16];
  *         contrasts with SD where the card publishes its RCA in R6.
  */
u32 EMMCCard_AssignRCA(void)
{
	u32 ret;
	SD_HOST_CmdTypeDef cmd_attr;

	cmd_attr.arg = ((u32)EMMC_RCA_DEFAULT) << 16;
	cmd_attr.idx = SD_CMD_SendRelAddr;
	cmd_attr.rsp_type = SD_HOST_RSP_6B;
	cmd_attr.rsp_crc_chk = ENABLE;
	cmd_attr.data_present = SD_HOST_NO_DATA;
	ret = host_ops->SendCommand(&cmd_attr, SD_HOST_CMD_CPLT_TIMEOUT);
	if (ret != HAL_OK) {
		return ret;
	}

	ret = SDCard_CmdRespError(SDIOH_RESP_R1, SD_CMD_SendRelAddr);
	if (ret != HAL_OK) {
		return ret;
	}

	card_info.rca = EMMC_RCA_DEFAULT;
	RTK_LOGS(TAG, RTK_LOG_DEBUG, "RCA = %04X\n", card_info.rca);
	return HAL_OK;
}

/**
  * @brief  CMD9 SEND_CSD. Used only for read_bl_len/write_bl_len;
  *         capacity is taken from EXT_CSD instead.
  */
u32 EMMCCard_GetCSD(void)
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
		_memset(pbuf, 0, SD_HOST_BUFFER_LENGTH);
		DCache_CleanInvalidate((u32)pbuf, SD_HOST_BUFFER_LENGTH);

		dma_cfg.op = DMA_Read;
		dma_cfg.start_addr = ((u32)pbuf) / 8;
		dma_cfg.blk_cnt = 1;
		dma_cfg.type = DMA_R2;
		host_ops->DMAConfig(&dma_cfg);

		cmd_attr.arg = ((u32)card_info.rca) << 16;
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

		card_info.read_bl_len  = 1U << (card_info.csd[5] & 0xF);
		card_info.write_bl_len = 1U << (((card_info.csd[12] & 0x3) << 2) | (card_info.csd[13] >> 6));

		RTK_LOGS(TAG, RTK_LOG_DEBUG, "Max. read data block length: %u Bytes\n", card_info.read_bl_len);
		RTK_LOGS(TAG, RTK_LOG_DEBUG, "Max. write data block length: %u Bytes\n", card_info.write_bl_len);
	} while (0);

	rtos_mem_free(pbuf_raw);
	return ret;
}

static u32 emmc_cmd8_read_into(u8 *buf)
{
	SD_HOST_DMA_CTL dma_cfg;
	SD_HOST_CmdTypeDef cmd_attr;
	u32 ret;

	_memset(buf, 0, EMMC_EXT_CSD_LENGTH);
	DCache_CleanInvalidate((u32)buf, EMMC_EXT_CSD_LENGTH);

	dma_cfg.op = DMA_Read;
	dma_cfg.start_addr = ((u32)buf) / 8;
	dma_cfg.blk_cnt = 1;
	dma_cfg.type = DMA_Normal;
	host_ops->DMAConfig(&dma_cfg);

	cmd_attr.arg = 0;
	cmd_attr.idx = EMMC_CMD_SendExtCsd;
	cmd_attr.rsp_type = SD_HOST_RSP_6B;
	cmd_attr.rsp_crc_chk = ENABLE;
	cmd_attr.data_present = SD_HOST_DATA_EXIST;
	ret = host_ops->SendCommand(&cmd_attr, 0);
	if (ret != HAL_OK) {
		return ret;
	}

	ret = host_ops->WaitDMADone(SD_HOST_TRANSFER_CPLT_TIMEOUT);
	if (ret != HAL_OK) {
		return ret;
	}

	DCache_Invalidate((u32)buf, EMMC_EXT_CSD_LENGTH);
	return SDCard_CmdRespError(SDIOH_RESP_R1, EMMC_CMD_SendExtCsd);
}

/* EXT_CSD is 512 B; SDCard_AllocAlignedBuf's 64 B pool is too small. */
static u8 *emmc_alloc_ext_csd_buf(u8 **raw)
{
	u8 *p = (u8 *)rtos_mem_malloc(EMMC_EXT_CSD_LENGTH + 0x1F);
	if (p == NULL) {
		return NULL;
	}
	if (raw) {
		*raw = p;
	}
	return (u8 *)(((((u32)p - 1) >> 5) + 1) << 5);
}

/* Highest speed advertised by EXT_CSD DEVICE_TYPE (bitmap, see JEDEC). */
static const char *emmc_devtype_max_str(u8 dev_type)
{
	if (dev_type & 0xC0) {
		return "HS400";    /* bit7/6: HS400 @ 200 MHz */
	}
	if (dev_type & 0x30) {
		return "HS200";    /* bit5/4: HS200 @ 200 MHz */
	}
	if (dev_type & 0x0C) {
		return "DDR52";    /* bit3/2: HS DDR @ 52 MHz */
	}
	if (dev_type & 0x02) {
		return "HS52";     /* bit1: HS @ 52 MHz */
	}
	if (dev_type & 0x01) {
		return "DS26";     /* bit0: HS @ 26 MHz */
	}
	return "?";
}

/**
  * @brief  CMD8 SEND_EXT_CSD: 512 B DMA read of extended CSD.
  *         Caches DEVICE_TYPE for later speed negotiation; computes capacity
  *         from SEC_COUNT and sets card_info.is_sdhc_sdxc for sector mode.
  */
u32 EMMCCard_GetExtCSD(void)
{
	u32 ret;
	u8 *pbuf_raw = NULL;
	u8 *pbuf = emmc_alloc_ext_csd_buf(&pbuf_raw);
	u32 sec_count;

	if (pbuf == NULL) {
		RTK_LOGE(TAG, "EXT_CSD buffer alloc fail\n");
		return HAL_ERR_MEM;
	}

	do {
		ret = emmc_cmd8_read_into(pbuf);
		if (ret != HAL_OK) {
			break;
		}

		emmc_ext_csd_device_type   = pbuf[EMMC_EXT_CSD_CARD_TYPE];
		emmc_ext_csd_rev           = pbuf[EMMC_EXT_CSD_REV];
		emmc_ext_csd_csd_structure = pbuf[EMMC_EXT_CSD_CSD_STRUCTURE];

		sec_count = ((u32)pbuf[EMMC_EXT_CSD_SEC_COUNT])
					| ((u32)pbuf[EMMC_EXT_CSD_SEC_COUNT + 1] << 8)
					| ((u32)pbuf[EMMC_EXT_CSD_SEC_COUNT + 2] << 16)
					| ((u32)pbuf[EMMC_EXT_CSD_SEC_COUNT + 3] << 24);

		if (sec_count != 0) {
			card_info.capacity = sec_count / 2;     /* sectors × 512 / 1024 = KiB */
			card_info.is_sdhc_sdxc = 1;
			RTK_LOGS(TAG, RTK_LOG_DEBUG, "User data area capacity: %u MB\n",
					 (u32)((u64)card_info.capacity / 1024));
		} else {
			RTK_LOGW(TAG, "SEC_COUNT=0; <2GB eMMC capacity unsupported\n");
		}

		RTK_LOGS(TAG, RTK_LOG_DEBUG, "eMMC specification: EXT_CSD rev %u, CSD struct %u\n",
				 emmc_ext_csd_rev, emmc_ext_csd_csd_structure);
		RTK_LOGS(TAG, RTK_LOG_DEBUG, "Card supported speeds: 0x%02X (max: %s)\n",
				 emmc_ext_csd_device_type, emmc_devtype_max_str(emmc_ext_csd_device_type));
	} while (0);

	rtos_mem_free(pbuf_raw);
	return ret;
}

u32 EMMCCard_ReadExtCSD(u8 *buf)
{
	if (buf == NULL || (((u32)buf) & 0x1F)) {
		return HAL_ERR_PARA;
	}
	return emmc_cmd8_read_into(buf);
}

u32 EMMCCard_SetBusWidth(u8 bus_width)
{
	u32 ret;
	u8 ext_csd_value;

	if (bus_width == SDIOH_BUS_WIDTH_1BIT) {
		ext_csd_value = EMMC_BW_1BIT_SDR;
	} else if (bus_width == SDIOH_BUS_WIDTH_4BIT) {
		ext_csd_value = EMMC_BW_4BIT_SDR;
	} else {
		return HAL_ERR_PARA;
	}

	if (host_ops->GetBusWidth() == bus_width) {
		return HAL_OK;
	}

	ret = emmc_cmd6_switch_byte(EMMC_EXT_CSD_BUS_WIDTH, ext_csd_value);
	if (ret != HAL_OK) {
		RTK_LOGE(TAG, "CMD6 SWITCH(BUS_WIDTH=%u) fail %lu\n", ext_csd_value, ret);
		return ret;
	}

	host_ops->SetBusWidth(bus_width);
	RTK_LOGS(TAG, RTK_LOG_DEBUG, "eMMC changes to %s bus width mode successfully\n",
			 (bus_width == SDIOH_BUS_WIDTH_4BIT) ? "4-bit" : "1-bit");
	return HAL_OK;
}

/**
  * @brief  Bring VCCQ from 3.3 V to 1.8 V for HS200/HS400 operation.
  *
  *         Unlike SD's CMD11 sequence, eMMC has no card-side voltage-switch
  *         command — VCCQ is purely a host-side power rail. The card keeps its
  *         state across the rail change as long as VCC stays powered.
  *
  *         Sequence:
  *           1. Gate the SD clock.
  *           2. Repoint the SDIOH LDO to 1.8 V and switch the host pad voltage.
  *           3. Wait for the rail to settle (LDO + bypass cap charge time).
  *           4. Re-enable the clock at low speed so the next CMD reaches the card.
  */
static u32 emmc_switch_vccq_1v8(void)
{
	SDHost_SetClkToggle(0);

	LDO_SDIOPower_Select(SDIO_HOST_1V8);
	SDHost_SetPadVoltage(SD_HOST_BUS_SIGNALING_LEVEL_1p8V);

	DelayMs(10);

	SDHost_SetClkToggle(1);
	DelayMs(1);

	card_info.sig_level = SD_HOST_BUS_SIGNALING_LEVEL_1p8V;
	return HAL_OK;
}

/** @brief Revert VCCQ to 3.3 V (cleanup path when HS200 attempt fails). */
static void emmc_revert_vccq_3v3(void)
{
	SDHost_SetClkToggle(0);

	LDO_SDIOPower_Select(SDIO_HOST_3V3);
	SDHost_SetPadVoltage(SD_HOST_BUS_SIGNALING_LEVEL_3p3V);

	DelayMs(10);

	SDHost_SetClkToggle(1);
	DelayMs(1);

	card_info.sig_level = SD_HOST_BUS_SIGNALING_LEVEL_3p3V;
}

/* Scratch LBA + buffers for the HS200 data-path probe. 1.5 KB of static
 * .ddr.data, allocated once. */
#ifndef EMMC_HS200_PROBE_BLK
#define EMMC_HS200_PROBE_BLK   100U
#endif

static u8 emmc_hs200_tx[SD_BLOCK_SIZE] __attribute__((aligned(32)))
__attribute__((section(".ddr.data")));
static u8 emmc_hs200_rx[SD_BLOCK_SIZE] __attribute__((aligned(32)))
__attribute__((section(".ddr.data")));
static u8 emmc_hs200_backup[SD_BLOCK_SIZE] __attribute__((aligned(32)))
__attribute__((section(".ddr.data")));

static u32 emmc_hs200_data_probe(void *ctx)
{
	UNUSED(ctx);
	u32 ret, restore_ret;

	/* back up before overwriting so a live sector is never left corrupted */
	ret = SDCard_ReadBlock(emmc_hs200_backup, EMMC_HS200_PROBE_BLK);
	if (ret != HAL_OK) {
		return ret;
	}

	ret = SDCard_WriteBlock(emmc_hs200_tx, EMMC_HS200_PROBE_BLK);
	if (ret == HAL_OK) {
		_memset(emmc_hs200_rx, 0, SD_BLOCK_SIZE);
		ret = SDCard_ReadBlock(emmc_hs200_rx, EMMC_HS200_PROBE_BLK);
		if (ret == HAL_OK) {
			for (u32 i = 0; i < SD_BLOCK_SIZE; i++) {
				if (emmc_hs200_tx[i] != emmc_hs200_rx[i]) {
					ret = HAL_ERR_HW;
					break;
				}
			}
		}
	}

	restore_ret = SDCard_WriteBlock(emmc_hs200_backup, EMMC_HS200_PROBE_BLK);
	if (restore_ret != HAL_OK) {
		/* Restore is what saves the caller's data — give it one more shot
		 * before admitting LBA is left with the test pattern. */
		restore_ret = SDCard_WriteBlock(emmc_hs200_backup, EMMC_HS200_PROBE_BLK);
	}
	if (restore_ret != HAL_OK) {
		RTK_LOGE(TAG, "!!! HS200 probe FAILED TO RESTORE LBA %d — test pattern "
				 "left on card, data at this LBA is now corrupted !!!\n",
				 EMMC_HS200_PROBE_BLK);
	}

	return (ret != HAL_OK) ? ret : restore_ret;
}

/**
  * @brief  Attempt HS200 @ 100 MHz: VCCQ→1V8, CMD6 HS_TIMING=2, CMD21 tuning.
  *         On any failure cleans up VCCQ and host clock so the caller can
  *         retry HS52/DS26 cleanly.
  * @retval HAL_OK on success (card is now in HS200, host at sd30 div0 100 MHz),
  *         HAL_ERR_HW on any step failure (state restored to 3.3 V / sd20 div2).
  */
static u32 emmc_try_hs200(void)
{
	u32 ret;

	if (!(emmc_ext_csd_device_type & EMMC_DEV_TYPE_HS200_1V8)) {
		RTK_LOGW(TAG, "HS200@1V8 not advertised by card (DEV_TYPE=0x%02X)\n",
				 emmc_ext_csd_device_type);
		return HAL_ERR_HW;
	}

	/* Caller (or board init) asked us to stay at 3.3V signaling. HS200
	 * requires VCCQ at 1.8V — refuse before touching the LDO. This catches
	 * the "board has only 3.3V VCCQ rail" case as well as the
	 * `SD_Force33V(1)` runtime opt-out. */
	if (sd_adapter.force_33v) {
		RTK_LOGW(TAG, "HS200 refused: force_33v set — board/caller forces 3.3V "
				 "but HS200 needs 1.8V VCCQ\n");
		return HAL_ERR_HW;
	}

	ret = emmc_switch_vccq_1v8();
	if (ret != HAL_OK) {
		RTK_LOGW(TAG, "VCCQ→1V8 failed %lu\n", ret);
		emmc_revert_vccq_3v3();
		SDHost_SwitchSpeed(2, SD_HOST_modesd20);
		return ret;
	}

	/* Bring host into sd30 base at low speed (div2 ≈ 25 MHz) so CMD6
	 * goes out cleanly before we ramp to 100 MHz. */
	SDHost_SwitchSpeed(2, SD_HOST_modesd30);

	ret = emmc_cmd6_switch_byte(EMMC_EXT_CSD_HS_TIMING, EMMC_HS_TIMING_HS200);
	if (ret != HAL_OK) {
		RTK_LOGW(TAG, "CMD6 HS_TIMING=HS200 failed %lu\n", ret);
		emmc_revert_vccq_3v3();
		SDHost_SwitchSpeed(2, SD_HOST_modesd20);
		return ret;
	}

	/* Pick best phase via CMD13 + CMD21 (ExecTuningEx), then validate the
	 * data path once below — the tuning-pattern compare alone is too
	 * permissive on this IP (a "passing" phase can still fail CMD25). */
	ret = SDHost_ExecTuningEx(EMMC_CMD_SendTuningBlk,
							  ((u32)card_info.rca) << 16,
							  NULL, NULL);
	if (ret != HAL_OK) {
		RTK_LOGW(TAG, "HS200 tuning failed %lu\n", ret);
		/* ForceIdle before CMD6 BC so the SWITCH reaches the card; otherwise
		 * the card stays in HS200 while VCCQ reverts and rollbacks all fail. */
		SDHost_ForceIdle();
		SDHost_SwitchSpeed(2, SD_HOST_modesd30);
		(void)emmc_cmd6_switch_byte(EMMC_EXT_CSD_HS_TIMING, EMMC_HS_TIMING_BC);
		SDHost_ForceIdle();
		emmc_revert_vccq_3v3();
		SDHost_SwitchSpeed(2, SD_HOST_modesd20);
		return ret;
	}

	for (u32 i = 0; i < SD_BLOCK_SIZE; i++) {
		emmc_hs200_tx[i] = (u8)(i ^ 0xA5);
	}
	if (emmc_hs200_data_probe(NULL) != HAL_OK) {
		RTK_LOGW(TAG, "HS200 post-tuning data probe failed — picked phase "
				 "doesn't carry CMD25 data path on this card. Rolling back.\n");

		/* Recover so the caller's DDR52/HS52 rollbacks aren't wedged: idle the
		 * FSM, drop to sd30 div2 (CMD12 at the failing 100 MHz phase would just
		 * re-fail), CMD12 to exit the card's rcv state, CMD6 HS_TIMING=BC while
		 * still at 1.8 V, then VCCQ → 3.3 V / sd20 div2. */
		SDHost_ForceIdle();
		SDHost_SwitchSpeed(2, SD_HOST_modesd30);

		{
			SD_HOST_CmdTypeDef cmd_attr;
			cmd_attr.arg = 0;
			cmd_attr.idx = SD_CMD_StopXsmission;   /* CMD12 */
			cmd_attr.rsp_type = SD_HOST_RSP_6B;
			cmd_attr.rsp_crc_chk = ENABLE;
			cmd_attr.data_present = SD_HOST_NO_DATA;
			(void)host_ops->SendCommand(&cmd_attr, SD_HOST_CMD_CPLT_TIMEOUT);
			SDHost_ForceIdle();
		}

		(void)emmc_cmd6_switch_byte(EMMC_EXT_CSD_HS_TIMING, EMMC_HS_TIMING_BC);
		SDHost_ForceIdle();

		emmc_revert_vccq_3v3();
		SDHost_SwitchSpeed(2, SD_HOST_modesd20);
		return HAL_ERR_HW;
	}

	card_info.bus_spd = SD_SPEED_HS200;
	RTK_LOGS(TAG, RTK_LOG_DEBUG, "Bus speed switched to HS200 @ 100 MHz\n");
	return HAL_OK;
}

/**
  * @brief  Negotiate eMMC bus speed from sdioh_config.sdioh_bus_speed:
  *           DS      → DS26          HS    → HS52 only
  *           DDR50   → DDR52→HS52→DS26
  *           HS200   → HS200→DDR52→HS52→DS26 (opt-in; not in auto)
  *           KEEP_CUR→ auto DDR52→HS52→DS26
  *         Each step verifies the EXT_CSD DEVICE_TYPE bit before its CMD6.
  */
u32 EMMCCard_SpeedNegotiate(void)
{
	u8 requested = sdioh_config.sdioh_bus_speed;
	u8 dev_type  = emmc_ext_csd_device_type;
	u8 try_ddr52;
	u32 ret;

	if (requested == SD_SPEED_HS200) {
		ret = emmc_try_hs200();
		if (ret == HAL_OK) {
			return HAL_OK;
		}
		RTK_LOGW(TAG, "HS200 failed, rolling back through DDR52/HS52/DS26\n");
		/* Fall through to DDR52 → HS52 / DS26 below. */
	}

	if (requested == SD_SPEED_DS) {
		SDHost_SwitchSpeed(2, SD_HOST_modesd20);
		card_info.bus_spd = SD_SPEED_DS;
		RTK_LOGS(TAG, RTK_LOG_DEBUG, "Bus speed switched to DS26\n");
		return HAL_OK;
	}

	/* HS52 explicit request: skip the DDR52 attempt so the user gets
	 * pure SDR 52 MHz behavior for verification. Any other value
	 * (DDR50, HS200 fall-through, KEEP_CUR_SPEED, ...) keeps the
	 * historical DDR-first walk. */
	try_ddr52 = (requested != SD_SPEED_HS);

	if (try_ddr52 && (dev_type & EMMC_DEV_TYPE_HS_DDR_52_3V)) {
		ret = emmc_cmd6_switch_byte(EMMC_EXT_CSD_HS_TIMING, EMMC_HS_TIMING_HS);
		if (ret == HAL_OK) {
			SDHost_SwitchSpeed(0, SD_HOST_modesd20);
			ret = emmc_cmd6_switch_byte(EMMC_EXT_CSD_BUS_WIDTH, EMMC_BW_4BIT_DDR);
			if (ret == HAL_OK) {
				SDHost_SetSamplePoint(SD_HOST_DDR50_SAMPLE_POINT);
				SDHost_SwitchSpeed(0, SD_HOST_modeddr);
				card_info.bus_spd = SD_SPEED_DDR50;
				RTK_LOGS(TAG, RTK_LOG_DEBUG, "Bus speed switched to DDR52\n");
				return HAL_OK;
			}
			RTK_LOGW(TAG, "DDR52 BUS_WIDTH SWITCH failed %lu, rolling back to HS52\n", ret);
		} else {
			RTK_LOGW(TAG, "DDR52 HS_TIMING SWITCH failed %lu, rolling back to HS52\n", ret);
		}
		/* Reduce host clock right after HS_TIMING=BC so the BUS_WIDTH reset
		 * SWITCH is sent at DS speed (JEDEC eMMC 5.x §6.6.3). */
		(void)emmc_cmd6_switch_byte(EMMC_EXT_CSD_HS_TIMING, EMMC_HS_TIMING_BC);
		SDHost_SwitchSpeed(2, SD_HOST_modesd20);
		(void)emmc_cmd6_switch_byte(EMMC_EXT_CSD_BUS_WIDTH, EMMC_BW_4BIT_SDR);
	}

	if (dev_type & EMMC_DEV_TYPE_HS_52) {
		ret = emmc_cmd6_switch_byte(EMMC_EXT_CSD_HS_TIMING, EMMC_HS_TIMING_HS);
		if (ret == HAL_OK) {
			SDHost_SwitchSpeed(0, SD_HOST_modesd20);
			card_info.bus_spd = SD_SPEED_HS;
			RTK_LOGS(TAG, RTK_LOG_DEBUG, "Bus speed switched to HS52\n");
			return HAL_OK;
		}
		RTK_LOGW(TAG, "HS52 SWITCH failed %lu, rolling back to DS26\n", ret);
		(void)emmc_cmd6_switch_byte(EMMC_EXT_CSD_HS_TIMING, EMMC_HS_TIMING_BC);
	}

	SDHost_SwitchSpeed(2, SD_HOST_modesd20);
	card_info.bus_spd = SD_SPEED_DS;
	RTK_LOGS(TAG, RTK_LOG_DEBUG, "Bus speed switched to DS26 (rollback)\n");
	return HAL_OK;
}

/**
  * @brief  Erase via CMD35/36/38. Address mode follows is_sdhc_sdxc, mirroring SDCard_Erase.
  */
u32 EMMCCard_Erase(u32 StartBlock, u32 EndBlock)
{
	u32 ret;
	u32 start, end;
	SD_HOST_CmdTypeDef cmd_attr;

	if (StartBlock > EndBlock) {
		return HAL_ERR_PARA;
	}

	if (card_info.is_sdhc_sdxc) {
		start = StartBlock;
		end   = EndBlock;
	} else {
		start = StartBlock * SD_BLOCK_SIZE;
		end   = EndBlock * SD_BLOCK_SIZE;
	}

	cmd_attr.arg = start;
	cmd_attr.idx = EMMC_CMD_EraseAddrSt;
	cmd_attr.rsp_type = SD_HOST_RSP_6B;
	cmd_attr.rsp_crc_chk = ENABLE;
	cmd_attr.data_present = SD_HOST_NO_DATA;
	ret = host_ops->SendCommand(&cmd_attr, SD_HOST_CMD_CPLT_TIMEOUT);
	if (ret != HAL_OK) {
		return ret;
	}
	ret = SDCard_CmdRespError(SDIOH_RESP_R1, EMMC_CMD_EraseAddrSt);
	if (ret != HAL_OK) {
		return ret;
	}

	cmd_attr.arg = end;
	cmd_attr.idx = EMMC_CMD_EraseAddrEd;
	cmd_attr.rsp_type = SD_HOST_RSP_6B;
	cmd_attr.rsp_crc_chk = ENABLE;
	cmd_attr.data_present = SD_HOST_NO_DATA;
	ret = host_ops->SendCommand(&cmd_attr, SD_HOST_CMD_CPLT_TIMEOUT);
	if (ret != HAL_OK) {
		return ret;
	}
	ret = SDCard_CmdRespError(SDIOH_RESP_R1, EMMC_CMD_EraseAddrEd);
	if (ret != HAL_OK) {
		return ret;
	}

	cmd_attr.arg = 0;
	cmd_attr.idx = SD_CMD_Erase;
	cmd_attr.rsp_type = SD_HOST_RSP_6B;
	cmd_attr.rsp_crc_chk = ENABLE;
	cmd_attr.data_present = SD_HOST_NO_DATA;
	ret = host_ops->SendCommand(&cmd_attr, 250000U * (EndBlock - StartBlock + 1));
	if (ret != HAL_OK) {
		return ret;
	}
	return SDCard_CmdRespError(SDIOH_RESP_R1, SD_CMD_Erase);
}

const char *EMMCCard_SpeedStr(u8 speed)
{
	switch (speed) {
	case SD_SPEED_DS:
		return "DS26";
	case SD_SPEED_HS:
		return "HS52";
	case SD_SPEED_DDR50:
		return "DDR52";
	case SD_SPEED_HS200:
		return "HS200";
	default:
		return "?";
	}
}
