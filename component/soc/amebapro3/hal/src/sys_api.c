/** mbed Microcontroller Library
  ******************************************************************************
  * @file    sys_api.c
  * @author
  * @version V1.0.0
  * @date    2016-08-01
  * @brief   This file provides following mbed system API:
  *				-JTAG OFF
  *				-LOGUART ON/OFF
  *				-OTA image switch
  *				-System Reset
  ******************************************************************************
  * @attention
  *
  * This module is a confidential and proprietary property of RealTek and
  * possession or use of this module requires written permission of RealTek.
  *
  * Copyright(c) 2016, Realtek Semiconductor Corporation. All rights reserved.
  ******************************************************************************
  */

#include "cmsis.h"
#include "sys_api.h"
#include "flash_api.h"
#include "ameba_ota.h"
#include "ameba_spinand.h"

#include "log.h"
//#define printf					printf

static const char *const TAG = "SYS";

#define RSIP_REMAP_REGION_ADDR_SHIFT	12

/** @addtogroup Ameba_Mbed_API
  * @{
  */

/** @addtogroup MBED_SYSAPI
 *  @brief      MBED_SYSAPI driver modules.
 *  @{
 */

/** @defgroup MBED_SYSAPI_Exported_Functions MBED_SYSAPI Exported Functions
  * @{
  */

/**
  * @brief  Turn off the JTAG function.
  * @retval none
  */
void sys_jtag_off(void)
{
	Pinmux_Swdoff();
}

/**
  * @brief  Write signature for NAND flash (read-modify-write at block granularity).
  * @note   NAND cannot overwrite in-place; the whole block must be backed up,
  *         erased, and rewritten with the patched signature.
  * @param  NandAddr  byte address within NAND (offset from flash base).
  * @param  sig_len   number of signature bytes to patch at the start of the block.
  * @param  sig       pointer to the new signature data.
  * @retval none
  */
static void sys_nand_flash_write_sig(u32 NandAddr, int sig_len, u8 *sig)
{
	u32 PageAddr, ByteAddr, BlockId;
	int DataLen = NAND_PAGE_SIZE_MAIN * NAND_BLOCK_PAGE_CNT;
	u8 *pData = (u8 *)rtos_mem_malloc(DataLen);
	if (pData == NULL) {
		RTK_LOGE(TAG, "[%s] malloc failed\n", __func__);
		return;
	}

	/* backup all pages in this block */
	for (int idx = 0; idx < DataLen; idx += NAND_PAGE_SIZE_MAIN) {
		PageAddr = NAND_ADDR_TO_PAGE_ADDR(NandAddr + idx);
		ByteAddr = NAND_ADDR_TO_BYTE_ADDR(NandAddr + idx);
		BlockId  = NAND_PAGE_ADDR_TO_BLOCK_ID(PageAddr);

		if (NAND_CheckBadBlock(BlockId)) {
			RTK_LOGE(TAG, "[%s] bad block at read, abort\n", __func__);
			goto end;
		}
		u8 status = NAND_Page_Read(PageAddr, ByteAddr, NAND_PAGE_SIZE_MAIN, pData + idx);
		if (0 != (status & NAND_STATUS_ECC_MASK)) {
			RTK_LOGE(TAG, "[%s] page read 0x%x fail, st:0x%x\n", __func__, (unsigned int)PageAddr, status);
			goto end;
		}
	}

	/* erase block */
	PageAddr = NAND_ADDR_TO_PAGE_ADDR(NandAddr);
	if (NAND_Erase(PageAddr)) {
		RTK_LOGE(TAG, "[%s] erase page 0x%x fail\n", __func__, (unsigned int)PageAddr);
		goto end;
	}

	/* patch signature into page-0 head */
	_memcpy((void *)pData, (const void *)sig, sig_len);

	/* write all pages back */
	for (int idx = 0; idx < DataLen; idx += NAND_PAGE_SIZE_MAIN) {
		PageAddr = NAND_ADDR_TO_PAGE_ADDR(NandAddr + idx);
		ByteAddr = NAND_ADDR_TO_BYTE_ADDR(NandAddr + idx);
		BlockId  = NAND_PAGE_ADDR_TO_BLOCK_ID(PageAddr);
		if (NAND_CheckBadBlock(BlockId)) {
			RTK_LOGE(TAG, "[%s] bad block at write, skip\n", __func__);
		} else if (NAND_Page_Write(PageAddr, ByteAddr, NAND_PAGE_SIZE_MAIN, pData + idx)) {
			RTK_LOGE(TAG, "[%s] page write 0x%x fail\n", __func__, (unsigned int)PageAddr);
		}
	}
end:
	rtos_mem_free(pData);
}

/**
  * @brief  Clear the signature of current firmware so the bootloader
  *         falls back to the other OTA bank on next boot.
  * @param  ImgID  image identifier (OTA_IMGID_APP / OTA_IMGID_BOOT).
  *                Currently only OTA_IMGID_APP is meaningful on Pro3.
  * @retval none
  */
void sys_clear_ota_signature(int ImgID)
{
	u8 otaCurIdx;
	u8 otaDstIdx;
	u32 Address[2];
	u32 ota1_start, ota2_start;

	UNUSED(ImgID);

	/* Certificate header (ImagePattern magic) lives at IMG_APP_OTA1/2.
	 * The NP bootloader checks this magic to validate each bank. */
	flash_get_layout_info(IMG_APP_OTA1, &ota1_start, NULL);
	flash_get_layout_info(IMG_APP_OTA2, &ota2_start, NULL);

	otaCurIdx = ota_get_cur_index(OTA_IMGID_APP);
	otaDstIdx = otaCurIdx ^ 1;

	Address[0] = ota1_start - SPI_FLASH_BASE;
	Address[1] = ota2_start - SPI_FLASH_BASE;

	RTK_LOGA(TAG, "[%s] current OTA%d (0x%08x), target OTA%d (0x%08x)\n",
			 __func__, otaCurIdx + 1, Address[otaCurIdx],
			 otaDstIdx + 1, Address[otaDstIdx]);

	if (SYSCFG_OTP_BootFromNor()) {
		u32 ota_sig[2];
		u8 empty_sig[8] = {0};

		/* Only clear if the target bank has a valid signature;
		 * prevents being left with no bootable image. */
		FLASH_ReadStream(Address[otaDstIdx], 8, (u8 *)ota_sig);
		if (ota_sig[0] == APP_IMAGE_PATTERN_1 && ota_sig[1] == APP_IMAGE_PATTERN_2) {
			FLASH_WriteStream(Address[otaCurIdx], 8, empty_sig);
			RTK_LOGA(TAG, "[%s] OTA%d signature cleared, will boot OTA%d on next reset\n",
					 __func__, otaCurIdx + 1, otaDstIdx + 1);
		} else {
			RTK_LOGE(TAG, "[%s] target OTA%d signature invalid, aborting\n",
					 __func__, otaDstIdx + 1);
		}
	} else {
		/* NAND: not memory-mapped, must use page-read to verify target bank sig. */
		u32 ota_sig[2] = {0};
		u8 empty_sig[8] = {0};
		u32 PageAddr = NAND_ADDR_TO_PAGE_ADDR(Address[otaDstIdx]);
		u32 ByteAddr = NAND_ADDR_TO_BYTE_ADDR(Address[otaDstIdx]);

		NAND_Page_Read(PageAddr, ByteAddr, sizeof(ota_sig), (u8 *)ota_sig);
		if (ota_sig[0] == APP_IMAGE_PATTERN_1 && ota_sig[1] == APP_IMAGE_PATTERN_2) {
			sys_nand_flash_write_sig(Address[otaCurIdx], 8, empty_sig);
			RTK_LOGA(TAG, "[%s] OTA%d signature cleared, will boot OTA%d on next reset\n",
					 __func__, otaCurIdx + 1, otaDstIdx + 1);
		} else {
			RTK_LOGE(TAG, "[%s] target OTA%d signature invalid, aborting\n",
					 __func__, otaDstIdx + 1);
		}
	}
}

/**
  * @brief  Recover the signature of the other (target) firmware so the
  *         bootloader can use it again after a previous sys_clear_ota_signature.
  * @param  ImgID  image identifier (OTA_IMGID_APP / OTA_IMGID_BOOT).
  *                Currently only OTA_IMGID_APP is meaningful on Pro3.
  * @retval none
  */
void sys_recover_ota_signature(int ImgID)
{
	u8 otaCurIdx;
	u8 otaDstIdx;
	u32 Address[2];
	u32 ota1_start, ota2_start;
	const u32 sig[2] = {APP_IMAGE_PATTERN_1, APP_IMAGE_PATTERN_2};

	UNUSED(ImgID);

	flash_get_layout_info(IMG_APP_OTA1, &ota1_start, NULL);
	flash_get_layout_info(IMG_APP_OTA2, &ota2_start, NULL);

	otaCurIdx = ota_get_cur_index(OTA_IMGID_APP);
	otaDstIdx = otaCurIdx ^ 1;

	Address[0] = ota1_start - SPI_FLASH_BASE;
	Address[1] = ota2_start - SPI_FLASH_BASE;

	RTK_LOGA(TAG, "[%s] current OTA%d (0x%08x), recovering OTA%d (0x%08x)\n",
			 __func__, otaCurIdx + 1, Address[otaCurIdx],
			 otaDstIdx + 1, Address[otaDstIdx]);

	if (SYSCFG_OTP_BootFromNor()) {
		u8 *backup;
		int idx;

		/* Read the first 4 KB sector of the target bank, erase it, patch
		 * the magic back into the first 8 bytes, then write the sector back.
		 * NOR flash requires an erase before changing 0 bits back to 1. */
		backup = (u8 *)rtos_mem_malloc(0x1000);
		if (!backup) {
			RTK_LOGE(TAG, "[%s] backup malloc failed\n", __func__);
			return;
		}
		FLASH_ReadStream(Address[otaDstIdx], 0x1000, backup);
		FLASH_EraseXIP(EraseSector, Address[otaDstIdx]);
		_memcpy(backup, sig, 8);
		for (idx = 0; idx < 0x1000; idx += 256) {
			FLASH_WriteStream(Address[otaDstIdx] + idx, 256, backup + idx);
		}
		rtos_mem_free(backup);
	} else {
		sys_nand_flash_write_sig(Address[otaDstIdx], 8, (u8 *)sig);
	}

	RTK_LOGA(TAG, "[%s] OTA%d signature recovered\n", __func__, otaDstIdx + 1);
}

/**
  * @brief  Open log uart.
  * @retval none
  */
void sys_log_uart_on(void)
{
	/* Just Support S0 */
	Pinmux_UartLogCtrl(PINMUX_S0, ON);

	LOGUART_INTConfig(LOGUART_DEV, RUART_BIT_ERBI, ENABLE);
	LOGUART_RxCmd(LOGUART_DEV, ENABLE);
}

/**
  * @brief  Close log uart.
  * @retval none
  */
void sys_log_uart_off(void)
{
	LOGUART_INTConfig(LOGUART_DEV, RUART_BIT_ERBI, DISABLE);
	LOGUART_RxCmd(LOGUART_DEV, DISABLE);

	/* Just Support S0 */
	Pinmux_UartLogCtrl(PINMUX_S0, OFF);
}

/**
  * @brief  Store or load ADC calibration parameter.
  * @param  write: This parameter can be one of the following values:
  *	   @arg 0: Load ADC calibration parameter a & b & c.
  *    @arg 1: Store ADC calibration parameter a & b & c.
  * @param  a: Pointer to ADC parameter a.
  * @param  b: Pointer to ADC parameter b.
  * @param  c: Pointer to ADC parameter c.
  * @retval none
  */
void sys_adc_calibration(u8 write, u16 *a, u16 *b, u16 *c)
{
	/* To avoid gcc warnings */
	(void) write;
	(void) a;
	(void) b;
	(void) c;

	RTK_LOGA(TAG, "ADC calibration is finished in FT test. Calibration parameters can be found in EFUSE." \
			 "Please refer to Battery Measurement chapter in Application Note to get calibration parameters.\n");

	assert_param(0);
}

/**
  * @brief  System software reset.
  * @retval none
  */
void sys_reset(void)
{
	System_Reset();
}

/**
  * @brief Vector reset.
  * @retval none
  */
void sys_cpu_reset(void)
{
	RTK_LOGA(TAG, "AmebaPro3 not support sys_cpu_reset function!\n");
	assert_param(0);
}
/** @} */
/** @} */
/** @} */


/******************* (C) COPYRIGHT 2016 Realtek Semiconductor *****END OF FILE****/
