/** mbed Microcontroller Library
 ******************************************************************************
 * @file	nand_api.c
 * @author
 * @version V1.0.0
 * @date	2026-03-01
 * @brief	This file provides mbed API for flash.
 ******************************************************************************
 * @attention
 *
 * This module is a confidential and proprietary property of RealTek and
 * possession or use of this module requires written permission of RealTek.
 *
 * Copyright(c) 2026, Realtek Semiconductor Corporation. All rights reserved.
 ******************************************************************************
 */

#include "objects.h"
#include "PinNames.h"
#include "ameba_soc.h"
#include "nand_api.h"


/** @addtogroup Ameba_Mbed_API
  * @{
  */

/** @defgroup MBED_NANDFLASH
 *  @brief    MBED_NANDFLASH driver modules.
 *  @{
 */

/** @defgroup MBED_NANDFLASH_Exported_Functions MBED_NANDFLASH Exported Functions
  * @{
  */

/**
  * @brief  Get flash ID (command: 0x9F), works in SPI mode only.
  * @param  obj: Flash object define in application software.
  * @param  buf: Pointer to a byte array to save the readback ID.
  * @param  len: Specifies the length of the buf. It should be 3.
  * @retval -1: Fail, len: Succeed.
  */
int snand_read_id(flash_t *obj, uint8_t *buf, uint8_t len)
{
	/* To avoid gcc warnings */
	(void) obj;

	NAND_RxCmd(flash_init_para.FLASH_cmd_rd_id, 0, NULL, len, buf);

	if ((buf[0] == 0x0) || (buf[0] == 0xFF)) {
		RTK_LOGE("NAND", "Invalid ID\r\n");
		return -1;
	}

	return len;
}

/**
* @brief  Read Status register to check flash status
* @param  obj: Specifies the parameter of flash object.
* @param  offset: Specifies which register offset users like to access
* @retval status: the value of status register.
*/
int snand_get_feature(flash_t *obj, uint32_t offset)
{
	/* To avoid gcc warnings */
	(void) obj;

	return NAND_GetStatus((u8) offset);
}

/**
* @brief  Set Status register to enable desired operation
* @param  obj: Specifies the parameter of flash object.
* @param  offset: Specifies which register offset users like to set
* @param  data: Specifies which bit users like to set
   ex: if users want to set the third bit, data = 0x8.
* @retval SUCCESS, FAIL
*/
int snand_set_feature(flash_t *obj, uint32_t offset, uint32_t data)
{
	/* To avoid gcc warnings */
	(void) obj;

	NAND_SetStatus((u8) offset, data);
	return 1;
}

/**
  * @brief  Erase flash block, usually 1 block = 64K bytes
    Please refer to flash data sheet to confirm the actual block size.
    The actual address which being erased always aligned with block size.
  * @param  address: Specifies the starting address to be erased.
  * @retval SUCCESS, FAIL
  */
int snand_erase_block(flash_t *obj, uint32_t address)
{
	/* To avoid gcc warnings */
	(void) obj;

	return NAND_Erase(address);

}

/**
  * @brief  Read a stream of data from specified address vai user mode
  * @param  obj: Specifies the parameter of flash object.
  * @param  address: Specifies the address to be read.
  * @param  len: Specifies the length of the data to read.
  * @param  data: Specified the address to save the readback data.
  * @retval   SUCCESS, FAIL
  */
int snand_page_read(flash_t *obj, uint32_t address, uint32_t Length, uint8_t *data)
{
	/* To avoid gcc warnings */
	(void) obj;

	return NAND_Page_Read(address, 0, Length, data);

}

/*
Function Description:
This function performans the same functionality as the function snand_stream_write.
It enhances write performance by reducing overheads.
Users can use either of functions depending on their needs.
* @brief  Write a stream of data to specified address
* @param  obj: Specifies the parameter of flash object.
* @param  address: Specifies the address to be programmed.
* @param  Length: Specifies the length of the data to write.
* @param  data: Specified the pointer of the data to be written.
          If the address is in the flash, full address is required, i.e. SPI_FLASH_BASE + Offset
* @retval SUCCESS, FAIL
*/
int snand_page_write(flash_t *obj, uint32_t address, uint32_t Length, uint8_t *data)
{
	/* To avoid gcc warnings */
	(void) obj;

	return NAND_Page_Write(address, 0, Length, data);

}

/**
  * @brief  This function is only for Winbond flash to lock whole flash chip.
  * @param  none
  * @retval none
  * @note But not all Winbond flash supports the function, plase refer to data sheets of the target flashes.
  */
void snand_global_lock(void)
{
	NAND_SetStatus(0xA0, 0x38);
}

/**
  * @brief  This function is only for Winbond flash to unlock whole flash chip.
  * @param  none
  * @retval none
  * @note But not all Winbond flash supports the function, plase refer to data sheets of the target flashes.
  */
void snand_global_unlock(void)
{
	NAND_SetStatus(0xA0, 0x0);
}

/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */
