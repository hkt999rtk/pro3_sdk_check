
#define __FPU_PRESENT			1
#define __CORTEX_A				7
#include "cmsis_ca/core_ca.h"
#include "basic_types.h"
// avoid include GDMA
//#define _AMEBA_GDMA_H_
#include "hal_platform.h"
#include "sysreg_syson.h"
#include "ameba_vector.h"
#include "ameba_spic.h"
#include "ameba_spinand.h"

#include <stdio.h>

#define HAL_OK 0
#define HAL_ERR_PARA -1

//extern FLASH_InitTypeDef flash_init_para;


void FLASH_UserModeEn(u8 enable)
{
	SPIC_TypeDef *spi_flash = SPIC;

	/* Wait spic busy done before switch mode */
	NAND_WaitBusy(WAIT_SPIC_BUSY);

	__DSB();
	__ISB();

	if (enable) {
		spi_flash->CTRLR0 |= BIT_USER_MODE;
	} else {
		spi_flash->CTRLR0 &= ~BIT_USER_MODE;
	}

	__ISB();
}


void NAND_UserModeEn(u8 enable)
{
	return FLASH_UserModeEn(enable);
}

void NAND_RxCmd(u8 cmd, u8 addr_len, u8 *paddr, u32 read_len, u8 *read_data)
{
	SPIC_TypeDef *spi_flash = SPIC;
	u32 idx;
	u32 value;

	/* Enter User Mode */
	FLASH_UserModeEn(ENABLE);

	/* Set OneBit Mode */
	NAND_SetSpiMode(spi_flash, &flash_init_para, Spic1IOBitMode);

	/*Get Feature CMD do not have dummy cycle after address phase*/
	if (cmd == flash_init_para.FLASH_cmd_rd_status) {
		spi_flash->USER_LENGTH &= ~MASK_USER_RD_DUMMY_LENGTH;
	}

	/* Set CMD phase: 1 byte and ADDR phase */
	value = spi_flash->USER_LENGTH & ~MASK_USER_ADDR_LENGTH;
	spi_flash->USER_LENGTH = value | USER_ADDR_LENGTH(addr_len);

	/* Set RX_NDF: frame number of receiving data. TX_NDF should be set in both transmit mode and receive mode.
		TX_NDF should be set to zero in receive mode to skip the TX_DATA phase. */
	spi_flash->RX_NDF = RX_NDF(read_len);
	spi_flash->TX_NDF = 0;

	/* set CTRLR0: RX_mode and one bit mode for data & addr*/
	spi_flash->CTRLR0 = ((spi_flash->CTRLR0 & 0xFFF0FFFF) | TMOD(3));

	/* set flash_cmd: write cmd to fifo */
	spi_flash->DR[0].BYTE = cmd;

	/* fill addr */
	for (idx = 0; idx < addr_len; idx++) {
		spi_flash->DR[0].BYTE = paddr[idx];
	}

	/* Enable SSIENR to start the transfer */
	spi_flash->SSIENR = BIT_SPIC_EN;

	/* read data */
	idx = 0;
	while (idx < read_len) {
		if (spi_flash->SR & BIT_RFNE) {
			read_data[idx] = spi_flash->DR[0].BYTE;
			idx += 1;
		}
	}

	/* Wait transfer complete. When complete, SSIENR.SPIC_EN are cleared by HW automatically. */
	NAND_WaitBusy(WAIT_TRANS_COMPLETE);

	/* Recover */
	NAND_SetSpiMode(spi_flash, &flash_init_para, flash_init_para.FLASH_cur_bitmode);

	/* Exit user mode */
	FLASH_UserModeEn(DISABLE);
}

void NAND_RxData(u8 cmd, u32 StartAddr, u32 read_len, u8 *read_data)
{
	SPIC_TypeDef *spi_flash = SPIC;
	u8 rd_cmd = cmd;
	u32 rx_num = 0;
	u32 temp, idx;
	u32 *aligned_buf;

	if (rd_cmd == 0) {
		rd_cmd = flash_init_para.FLASH_cur_cmd;
	}

	/* Enter User Mode */
	NAND_UserModeEn(ENABLE);

	/* set CTRLR0: RX_mode */
	spi_flash->CTRLR0 |= TMOD(3);

	/* Set RX_NDF: frame number of receiving data. TX_NDF should be set in both transmit mode and receive mode.
		TX_NDF should be set to zero in receive mode to skip the TX_DATA phase. */
	spi_flash->RX_NDF = RX_NDF(read_len);
	spi_flash->TX_NDF = 0;

	/* Set ADDR length, and dummy byte is set by USER_LENGTH.USER_RD_DUMMY_LENGTH*/
	temp = spi_flash->USER_LENGTH & ~MASK_USER_ADDR_LENGTH;
	spi_flash->USER_LENGTH = temp | USER_ADDR_LENGTH(flash_init_para.FLASH_addr_phase_len);

	/* Set target slave */
	spi_flash->SER = 1;

	/* set flash_cmd: write cmd & address to fifo & addr is MSB */
	spi_flash->DR[0].BYTE = rd_cmd;

	/*read from cache of GigaDevice Nand flash need dummy data after cmd phase.*/
	if (flash_init_para.FLASH_addr_phase_len == NAND_COL_ADDR_3_BYTE) {
		spi_flash->DR[0].BYTE = 0x0; 	/*put dummy data after cmd for GigaDevice*/
	}
	spi_flash->DR[0].BYTE = (u8)((StartAddr & 0xFF00) >> 8);
	spi_flash->DR[0].BYTE = (u8)(StartAddr & 0xFF);

	/* Enable SSIENR to start the transfer */
	spi_flash->SSIENR = BIT_SPIC_EN;
	rx_num = 0;

	temp = UNALIGNED32(read_data);
	if (temp) { /* if buf is not aligned */
		while (rx_num < read_len) {
			if (spi_flash->SR & BIT_RFNE) {
				read_data[rx_num] = spi_flash->DR[0].BYTE;
				rx_num += 1;
			}
			if (rx_num == U32BLOCKSIZE - temp) {
				break;
			}
		}
	}

	aligned_buf = (u32 *)&read_data[rx_num];
	while (rx_num < read_len) {
		temp = GET_RXFLR(spi_flash->RXFLR) / U32BLOCKSIZE;
		for (idx = 0; idx < temp; idx++) {
			aligned_buf[idx] = spi_flash->DR[0].WORD;
		}
		aligned_buf += idx;
		rx_num += idx * U32BLOCKSIZE;

		if (read_len - rx_num < U32BLOCKSIZE) {
			break;
		}
	}

	while (rx_num < read_len) {
		if (spi_flash->SR & BIT_RFNE) {
			read_data[rx_num] = spi_flash->DR[0].BYTE;
			rx_num += 1;
		}
	}

	/* Wait transfer complete. When complete, SSIENR.SPIC_EN are cleared by HW automatically. */
	NAND_WaitBusy(WAIT_TRANS_COMPLETE);

	/* Exit user mode */
	NAND_UserModeEn(DISABLE);
}

u8 NAND_WaitBusy(u32 WaitType)
{
	SPIC_TypeDef *spi_flash = SPIC;
	u32 BusyCheck = 0;
	u8 status = 0;
	u32 timeout_val = 0;

	do {
		if (WaitType == WAIT_SPIC_BUSY) {
			BusyCheck = (spi_flash->SR & BIT_BUSY);

		} else if (WaitType == WAIT_TRANS_COMPLETE) {
			/* When transfer completes, SSIENR.SPIC_EN are cleared by HW automatically. */
			BusyCheck = (spi_flash->SSIENR & BIT_SPIC_EN);

		} else if (WaitType == WAIT_FLASH_BUSY) {
			timeout_val++;
			status = NAND_GetStatus(NAND_REG_STATUS);

			BusyCheck = (status & flash_init_para.FLASH_Busy_bit);
		} else if (WaitType == WAIT_WRITE_EN) {
			timeout_val++;
			status = NAND_GetStatus(NAND_REG_STATUS);

			/* write enable ok & flash not busy */
			BusyCheck = (!(status & flash_init_para.FLASH_WLE_bit)) || (status & flash_init_para.FLASH_Busy_bit);
		}

		if (!BusyCheck) {
			break;
		}

		if (timeout_val > 0x20000) {
			status = 0xFF;
			printf("TIME OUT\n");
			break;
		}
	} while (1);
	return status;
}

void NAND_TxCmd(u8 cmd, u8 AddrLen, u8 *pAddr, u32 DataLen, u8 *pData)
{
	SPIC_TypeDef *spi_flash = SPIC;
	u32 tx_num = 0;
	u32 addr_length = spi_flash->USER_LENGTH;
	u32 ctrl0 = spi_flash->CTRLR0;
	u32 value;

	/* Enter user mode */
	FLASH_UserModeEn(ENABLE);

	/* set CTRLR0: TX mode and one bit mode */
	spi_flash->CTRLR0 &= ~(TMOD(3) | CMD_CH(3) | ADDR_CH(3) | DATA_CH(3));

	/* Set ADDR length */
	value = spi_flash->USER_LENGTH & ~MASK_USER_ADDR_LENGTH;
	spi_flash->USER_LENGTH = value | USER_ADDR_LENGTH(AddrLen);

	/* Set TX_NDF: frame number of Tx data. */
	spi_flash->RX_NDF = 0;
	spi_flash->TX_NDF = TX_NDF(DataLen);

	/* set flash_cmd: write cmd & address to fifo & addr is MSB */
	spi_flash->DR[0].BYTE = cmd;

	for (tx_num = 0; tx_num < AddrLen; tx_num++) {
		spi_flash->DR[0].BYTE = pAddr[tx_num];
	}

	/* we can not set ssienr=1 when FIFO empty */
	/* Pre-load data before enabling, but there are just 16 - 4 = 12 bytes fifo afer cmd+addr */
	tx_num = 0;
	while (tx_num < DataLen) {
		if (spi_flash->SR & BIT_TFNF) {
			spi_flash->DR[0].BYTE = pData[tx_num];
			tx_num += 1;
		} else {
			/* Here mean Transmit FIFO is Full. */
			break;
		}
	}

	/* Enable SSIENR to start the transfer */
	spi_flash->SSIENR = BIT_SPIC_EN;

	/* Put the remain data into transmit FIFO */
	while (tx_num < DataLen) {
		if (spi_flash->SR & BIT_TFNF) {
			spi_flash->DR[0].BYTE = pData[tx_num];
			tx_num += 1;
		}
	}

	/* Wait transfer complete. When complete, SSIENR.SPIC_EN are cleared by HW automatically. */
	NAND_WaitBusy(WAIT_TRANS_COMPLETE);

	/* Restore bitmode & addr_len */
	spi_flash->USER_LENGTH = addr_length;
	spi_flash->CTRLR0 = ctrl0;

	/* Exit user mode */
	FLASH_UserModeEn(DISABLE);
}

u8 NAND_GetStatus(u8 FeatureID)
{
	u8 status;
	NAND_RxCmd(flash_init_para.FLASH_cmd_rd_status, 1, &FeatureID, 1, &status);
	return status;
}

void NAND_SetStatus(u8 FeatureID, u8 Status)
{
	NAND_TxCmd(flash_init_para.FLASH_cmd_wr_status, 1, &FeatureID, 1, &Status);
	NAND_WaitBusy(WAIT_FLASH_BUSY);
}

void NAND_SetStatusBits(u8 FeatureID, u8 SetBits, u8 NewState)
{
	u8 status = NAND_GetStatus(FeatureID);
	u8 status_mask = (u8)(SetBits & 0xFF);

	if (NewState == ENABLE) {
		status |=  status_mask;
	} else {
		status &=  ~status_mask;
	}

	NAND_SetStatus(FeatureID, status);
}

u8 NAND_Page_Read_ArrayToCache(u32 PageAddr)
{
	u8 Addr[3];
	u8 status;
	Addr[0] = (u8)((PageAddr >> 16) & 0xFF);
	Addr[1] = (u8)((PageAddr >> 8) & 0xFF);
	Addr[2] = (u8)(PageAddr & 0xFF);

	/* Page read Sequence is CMD + PA23-16 + PA15-8 + PA7-0. */
	NAND_TxCmd(flash_init_para.FLASH_cmd_page_read, 3, Addr, 0, NULL);

	status = NAND_WaitBusy(WAIT_FLASH_BUSY);
	/*if status is 0 means NAND_STATUS_ECC_NO_BITFLIPS, ortherwise need confirm spi nand datasheet.*/
	return status;
}

void NAND_Page_Read_FromCache(u8 Cmd, u32 ByteAddr, u32 ByteLen, u8 *pData)
{
	NAND_RxData(Cmd, ByteAddr, ByteLen, pData);
}

u8 NAND_Page_Read(u32 PageAddr, u32 ByteAddr, u32 ByteLen, u8 *pData)
{
	u8 status = NAND_Page_Read_ArrayToCache(PageAddr);

	NAND_Page_Read_FromCache(flash_init_para.FLASH_cur_cmd, ByteAddr, ByteLen, pData);

	/*if status is 0 means NAND_STATUS_ECC_NO_BITFLIPS, ortherwise need confirm spi nand datasheet.*/
	return status;
}

u32 NAND_CheckBadBlock(u32 block_id)
{
	u8 oob[4];

	/*bad block: the first byte of spare area of the fisrt page of block is not 0xFF*/
	NAND_Page_Read(NAND_BLOCK_ID_TO_PAGE_ADDR(block_id), NAND_PAGE_SIZE_MAIN, 4, oob);
	if ((oob[0] != 0xFF) || (oob[1] != 0xFF)) {
		printf("Blk 0x%x is BB!\n", block_id);
		return TRUE;
	}
	return FALSE;
}

void SPIC_Config(SPIC_TypeDef *spi_flash, FLASH_InitTypeDef *FLASH_InitStruct)
{
	u32 Value32;

	/* Disable SPI_FLASH User Mode */
	spi_flash->SSIENR = 0;

	/* Disable SPIC DR Timeout */
	spi_flash->CTRLR0 |= (1 << 11);

	/* set baud rate */
	spi_flash->BAUDR = SCKDV(FLASH_InitStruct->FLASH_baud_rate);

	/* spi slave select */
	spi_flash->SER = BIT_SER;

	/* Reset DR_FIXED bit to default value (in case do narrow SPIC-DMA transfer and it fails) */
	spi_flash->CTRLR2 &= ~(BIT_DR_FIXED);

	/* set 2bit mode cmd */
	spi_flash->READ_FAST_SINGLE = FLASH_InitStruct->FLASH_rd_fast_single;
	spi_flash->READ_DUAL_ADDR_DATA = FLASH_InitStruct->FLASH_rd_dual_io;
	spi_flash->READ_DUAL_DATA = FLASH_InitStruct->FLASH_rd_dual_o;

	/* set 4bit mode cmd */
	spi_flash->READ_QUAD_ADDR_DATA = FLASH_InitStruct->FLASH_rd_quad_io;
	spi_flash->READ_QUAD_DATA = FLASH_InitStruct->FLASH_rd_quad_o;

	/* ctrl2 */
	spi_flash->CTRLR2 |= BIT_SO_DNUM; /*if we are use multi channel we should set it to 1 */
	spi_flash->CTRLR2 &= ~(BIT_WPN_SET | BIT_WPN_DNUM);/* bit1 output control */

	/* set CMD/ADDR phase length */
	Value32 = spi_flash->AUTO_LENGTH & ~MASK_AUTO_ADDR_LENGTH;
	spi_flash->AUTO_LENGTH = Value32 | AUTO_ADDR_LENGTH(FLASH_InitStruct->FLASH_addr_phase_len);

	Value32 = spi_flash->USER_LENGTH & ~(MASK_USER_ADDR_LENGTH | MASK_USER_CMD_LENGHT);
	spi_flash->USER_LENGTH = Value32 | USER_ADDR_LENGTH(FLASH_InitStruct->FLASH_addr_phase_len) | USER_CMD_LENGHT(1);

	/*SPI Nand Flash read random value from floating pin as dummy byte */
	/* set dummy byte but don't ues it*/
	spi_flash->DUM_BYTE = FLASH_InitStruct->FLASH_prm_byte;

	spi_flash->DEVICE_INFO = PAGE_SIZE(FLASH_InitStruct->FLASH_pagesize_main_bit_exp) | BIT_NAND_FLASH;

	Value32 = spi_flash->READ_STATUS & ~(MASK_RD_ST_CMD_1ST_BYTE | MASK_RD_ST_CMD_2ND_BYTE | MASK_ST_CMD_LEN);
	Value32 |= RD_ST_CMD_1ST_BYTE(NAND_CMD_RDSR) | RD_ST_CMD_2ND_BYTE(NAND_REG_STATUS) | ST_CMD_LEN(2);
	spi_flash->READ_STATUS = Value32;

	Value32 = spi_flash->PAGE_READ & ~(MASK_PAGE_RD_CMD | MASK_PAGE_RD_CH | MASK_PAGE_RD_ADDR_LEN);
	Value32 |= PAGE_RD_CMD(NAND_CMD_PAGERD) | PAGE_RD_CH(0) | PAGE_RD_ADDR_LEN(3);
	spi_flash->PAGE_READ = Value32;
}

void NAND_SetSpiMode(SPIC_TypeDef *spi_flash, FLASH_InitTypeDef *FLASH_InitStruct, u8 SpicBitMode)
{
	u32 Value32;
	u32 ctrl0 = 0;
	u16 dclen = FLASH_InitStruct->FLASH_rd_dummy_cycle[SpicBitMode] * 2 * FLASH_InitStruct->FLASH_baud_rate;

	/* Disable SPI_FLASH User Mode */
	spi_flash->SSIENR = 0;

	/* set auto read dummy cycle and line delay */
	Value32 = spi_flash->AUTO_LENGTH & ~(MASK_RD_DUMMY_LENGTH);
	spi_flash->AUTO_LENGTH = Value32 | RD_DUMMY_LENGTH(dclen);

	/* set user read dummy cycle, USER_LENGTH will reuse IN_PHYSICAL_CYC in AUTO_LENGTH */
	Value32 = spi_flash->USER_LENGTH & ~MASK_USER_RD_DUMMY_LENGTH;
	spi_flash->USER_LENGTH = Value32 | USER_RD_DUMMY_LENGTH(dclen);

	/* clear multi channel first */
	ctrl0 = spi_flash->CTRLR0 & ~(CMD_CH(3) | ADDR_CH(3) | DATA_CH(3));

	/* clear dual & quad bit mode */
	Value32 = spi_flash->VALID_CMD & ~(SPIC_VALID_CMD_MASK);

	switch (SpicBitMode) {
	case Spic2OBitMode:
		Value32 |= BIT_RD_DUAL_I;
		ctrl0 |= (ADDR_CH(0) | DATA_CH(1));
		FLASH_InitStruct->FLASH_cur_cmd = FLASH_InitStruct->FLASH_rd_dual_o;
		break;
	case Spic2IOBitMode:
		Value32 |= BIT_RD_DUAL_IO;
		ctrl0 |= (ADDR_CH(1) | DATA_CH(1));
		FLASH_InitStruct->FLASH_cur_cmd = FLASH_InitStruct->FLASH_rd_dual_io;
		break;
	case Spic4OBitMode:
		Value32 |= BIT_RD_QUAD_O;
		ctrl0 |= (ADDR_CH(0) | DATA_CH(2));
		FLASH_InitStruct->FLASH_cur_cmd = FLASH_InitStruct->FLASH_rd_quad_o;
		break;
	case Spic4IOBitMode:
		Value32 |= BIT_RD_QUAD_IO;
		ctrl0 |= (ADDR_CH(2) | DATA_CH(2));
		FLASH_InitStruct->FLASH_cur_cmd = FLASH_InitStruct->FLASH_rd_quad_io;
		break;
	default:
		FLASH_InitStruct->FLASH_cur_cmd = NAND_CMD_READ;
	}

	spi_flash->VALID_CMD = Value32;
	spi_flash->CTRLR0 = ctrl0;
}

u8 NAND_SPIC_DPHY_CTRL(FLASH_InitTypeDef *FLASH_InitStruct, u8 ctrl)
{

	SPIC_TypeDef *spi_flash = SPIC;

	if (FLASH_InitStruct->FLASH_baud_rate > 3) {
		return HAL_ERR_PARA; // ERROR
	}

	if (ctrl) {

		// SPIC settings
		spi_flash->CTRLR2 |= BIT_PHY_RD_VALID;

		spi_flash->AUTO_LENGTH &= ~(MASK_IN_PHYSICAL_CYC); // IN_PHYSICAL_CYC must be 0 when DPHY_CTRL enabled!

		// SPIC DPHY system register controls
		// Set RD_DELAY_FACTOR

		if (FLASH_InitStruct->FLASH_baud_rate == 1) {

			SYSON_BASE->SYS_FLASH_PHY_CTRL2 &= ~(SYSON_MASK_DPHY_RD_DELAY_FACTOR);
		} else if (FLASH_InitStruct->FLASH_baud_rate == 2) {

			SYSON_BASE->SYS_FLASH_PHY_CTRL2 &= ~(SYSON_MASK_DPHY_RD_DELAY_FACTOR);
			SYSON_BASE->SYS_FLASH_PHY_CTRL2 |= (SYSON_DPHY_RD_DELAY_FACTOR(0x1));
		} else if (FLASH_InitStruct->FLASH_baud_rate == 3) {

			SYSON_BASE->SYS_FLASH_PHY_CTRL2 &= ~(SYSON_MASK_DPHY_RD_DELAY_FACTOR);
			SYSON_BASE->SYS_FLASH_PHY_CTRL2 |= (SYSON_DPHY_RD_DELAY_FACTOR(0x2));
		}

		// RD_DELAY_FACTOR - This is fixed at 0x5.
		// 0x5 is the optimal value to ensure SPIC DPHY Async FIFO never underrun nor overrun.

		SYSON_BASE->SYS_FLASH_PHY_CTRL2 &= ~(SYSON_MASK_DPHY_RD_DELAY);
		SYSON_BASE->SYS_FLASH_PHY_CTRL2 |= (SYSON_DPHY_RD_DELAY(0x5) | (SYSON_BIT_DPHY_EN));

		FLASH_InitStruct->FLASH_SPIC_DPHY_bit = 1; // indicates SPIC_DPHY is in use

	} else {

		SYSON_BASE->SYS_FLASH_PHY_CTRL2 &= ~(SYSON_BIT_DPHY_EN);
		FLASH_InitStruct->FLASH_SPIC_DPHY_bit = 0; // indicates SPIC_DPHY is NOT in use

		// restore IN_PHYSICAL_CYC value
		spi_flash->AUTO_LENGTH |= ((FLASH_InitStruct->FLASH_IN_PHYSICAL_CYC) << 12);

		// restore PHY_RD_VALID = 0
		spi_flash->CTRLR2 &= ~(BIT_PHY_RD_VALID);
	}

	return HAL_OK;

}


void NAND_StructInit_Micron(FLASH_InitTypeDef *FLASH_InitStruct)
{
	FLASH_InitStruct->FLASH_Id = FLASH_ID_MICRON;
	FLASH_InitStruct->FLASH_baud_rate = 1;
	FLASH_InitStruct->FLASH_IN_PHYSICAL_CYC = 0;

	/* status bit define */
	/* micron QSPI is enable by default, and cannot be disabled!! */
	/* WP#, HOLD# are enabled by default, and can be disabled by set bit1 of feature 0xA0*/
	FLASH_InitStruct->FLASH_QuadEn_bit = 0;
	FLASH_InitStruct->FLASH_Busy_bit = NAND_STATUS_BUSY;
	FLASH_InitStruct->FLASH_WLE_bit = NAND_STATUS_WEL;
	FLASH_InitStruct->FLASH_Status2_exist = 0;

	/* calibration data */
	FLASH_InitStruct->FLASH_rd_dummy_cycle[0] = NAND_DM_CYCLE_READ;
	FLASH_InitStruct->FLASH_rd_dummy_cycle[1] = NAND_DM_CYCLE_2O;
	FLASH_InitStruct->FLASH_rd_dummy_cycle[2] = NAND_DM_CYCLE_2IO;
	FLASH_InitStruct->FLASH_rd_dummy_cycle[3] = NAND_DM_CYCLE_4O;
	FLASH_InitStruct->FLASH_rd_dummy_cycle[4] = NAND_DM_CYCLE_4IO;

	/* calibration data in continuous mode */
	FLASH_InitStruct->FLASH_rd_c_dummy_cycle[0] = 0x0;
	FLASH_InitStruct->FLASH_rd_c_dummy_cycle[1] = 0x0;
	FLASH_InitStruct->FLASH_rd_c_dummy_cycle[2] = 0x0;
	FLASH_InitStruct->FLASH_rd_c_dummy_cycle[3] = 0x0;
	FLASH_InitStruct->FLASH_rd_c_dummy_cycle[4] = 0x0;

	/* set 2bit mode cmd */
	FLASH_InitStruct->FLASH_rd_dual_io = NAND_CMD_2READ;
	FLASH_InitStruct->FLASH_rd_dual_o = NAND_CMD_DREAD;

	/* set 4bit mode cmd */
	FLASH_InitStruct->FLASH_rd_quad_io = NAND_CMD_4READ;
	FLASH_InitStruct->FLASH_rd_quad_o = NAND_CMD_QREAD;

	/* other flash commnad set */
	FLASH_InitStruct->FLASH_cmd_wr_en = NAND_CMD_WREN;
	FLASH_InitStruct->FLASH_cmd_rd_id = NAND_CMD_RDID;
	FLASH_InitStruct->FLASH_cmd_rd_status = NAND_CMD_RDSR;
	FLASH_InitStruct->FLASH_cmd_rd_status2 = 0;
	FLASH_InitStruct->FLASH_cmd_wr_status = NAND_CMD_WRSR;
	FLASH_InitStruct->FLASH_cmd_wr_status2 = 0;
	FLASH_InitStruct->FLASH_cmd_chip_e = 0;
	FLASH_InitStruct->FLASH_cmd_block_e = NAND_CMD_BE;
	FLASH_InitStruct->FLASH_cmd_sector_e = 0;
	FLASH_InitStruct->FLASH_cmd_pwdn_release = 0;
	FLASH_InitStruct->FLASH_cmd_pwdn = 0;

	FLASH_InitStruct->FLASH_rd_fast_single = NAND_CMD_FREAD;
	FLASH_InitStruct->FLASH_addr_phase_len = NAND_COL_ADDR_2_BYTE;
	FLASH_InitStruct->FLASH_prm_byte = 0;

	FLASH_InitStruct->FLASH_pagesize_main_bit_exp = NAND_PAGE_SIZE_MAIN_2K_BIT_EXP;
	FLASH_InitStruct->FLASH_page_per_block_bit_exp = NAND_BLOCK_PAGE_CNT_64_BIT_EXP;

	/* basic page rd/wr operation */
	FLASH_InitStruct->FLASH_cmd_page_read = NAND_CMD_PAGERD;
	FLASH_InitStruct->FLASH_cmd_page_write = NAND_CMD_PROMEXEC;
	FLASH_InitStruct->FLASH_EFail_bit = NAND_STATUS_ERASE_FAILED;
	FLASH_InitStruct->FLASH_PFail_bit = NAND_STATUS_PROG_FAILED;
	FLASH_InitStruct->FLASH_ECC_bit = BIT_OFFSET_LEN_MASK_32(4, 3);

	/*The default state of the internal ECC is enabled. */
	FLASH_InitStruct->FLASH_ECC_EN_bit = NAND_CFG_ECC_ENABLE;

	FLASH_InitStruct->FLASH_BUF_CONT_bit = NAND_CFG_BUF_CONT_ENABLE;
}

void NAND_StructInit_MXIC(FLASH_InitTypeDef *FLASH_InitStruct)
{
	FLASH_InitStruct->FLASH_Id = FLASH_ID_MXIC;
	FLASH_InitStruct->FLASH_baud_rate = 1;
	FLASH_InitStruct->FLASH_IN_PHYSICAL_CYC = 0;

	/* status bit define */
	/* mxic  QE bit is bit0 of feature 0xB0, QSPI is disabled by default */
	/* WP#, HOLD# are enabled by default*/
	FLASH_InitStruct->FLASH_QuadEn_bit = NAND_CFG_QUAD_ENABLE;
	FLASH_InitStruct->FLASH_Busy_bit = NAND_STATUS_BUSY;
	FLASH_InitStruct->FLASH_WLE_bit = NAND_STATUS_WEL;
	FLASH_InitStruct->FLASH_Status2_exist = 0;

	/* calibration data */
	FLASH_InitStruct->FLASH_rd_dummy_cycle[0] = NAND_DM_CYCLE_READ;
	FLASH_InitStruct->FLASH_rd_dummy_cycle[1] = NAND_DM_CYCLE_2O;
	FLASH_InitStruct->FLASH_rd_dummy_cycle[2] = NAND_DM_CYCLE_2IO;
	FLASH_InitStruct->FLASH_rd_dummy_cycle[3] = NAND_DM_CYCLE_4O;
	FLASH_InitStruct->FLASH_rd_dummy_cycle[4] = NAND_DM_CYCLE_4IO;

	/* calibration data in continuous mode */
	FLASH_InitStruct->FLASH_rd_c_dummy_cycle[0] = NAND_DM_CYCLE_READ;
	FLASH_InitStruct->FLASH_rd_c_dummy_cycle[1] = NAND_DM_CYCLE_2O;
	FLASH_InitStruct->FLASH_rd_c_dummy_cycle[2] = NAND_DM_CYCLE_2IO;
	FLASH_InitStruct->FLASH_rd_c_dummy_cycle[3] = NAND_DM_CYCLE_4O;
	FLASH_InitStruct->FLASH_rd_c_dummy_cycle[4] = NAND_DM_CYCLE_4IO;

	/* set 2bit mode cmd */
	FLASH_InitStruct->FLASH_rd_dual_io = NAND_CMD_2READ;
	FLASH_InitStruct->FLASH_rd_dual_o = NAND_CMD_DREAD;

	/* set 4bit mode cmd */
	FLASH_InitStruct->FLASH_rd_quad_io = NAND_CMD_4READ;
	FLASH_InitStruct->FLASH_rd_quad_o = NAND_CMD_QREAD;

	/* other flash commnad set */
	FLASH_InitStruct->FLASH_cmd_wr_en = NAND_CMD_WREN;
	FLASH_InitStruct->FLASH_cmd_rd_id = NAND_CMD_RDID;
	FLASH_InitStruct->FLASH_cmd_rd_status = NAND_CMD_RDSR;
	FLASH_InitStruct->FLASH_cmd_rd_status2 = 0;
	FLASH_InitStruct->FLASH_cmd_wr_status = NAND_CMD_WRSR;
	FLASH_InitStruct->FLASH_cmd_wr_status2 = 0;
	FLASH_InitStruct->FLASH_cmd_chip_e = 0;
	FLASH_InitStruct->FLASH_cmd_block_e = NAND_CMD_BE;
	FLASH_InitStruct->FLASH_cmd_sector_e = 0;
	FLASH_InitStruct->FLASH_cmd_pwdn_release = 0;
	FLASH_InitStruct->FLASH_cmd_pwdn = 0;

	FLASH_InitStruct->FLASH_rd_fast_single = NAND_CMD_FREAD;
	FLASH_InitStruct->FLASH_addr_phase_len = NAND_COL_ADDR_2_BYTE;
	FLASH_InitStruct->FLASH_prm_byte = 0;

	FLASH_InitStruct->FLASH_pagesize_main_bit_exp = NAND_PAGE_SIZE_MAIN_2K_BIT_EXP;
	FLASH_InitStruct->FLASH_page_per_block_bit_exp = NAND_BLOCK_PAGE_CNT_64_BIT_EXP;

	/* basic page rd/wr operation */
	FLASH_InitStruct->FLASH_cmd_page_read = NAND_CMD_PAGERD;
	FLASH_InitStruct->FLASH_cmd_page_write = NAND_CMD_PROMEXEC;
	FLASH_InitStruct->FLASH_EFail_bit = NAND_STATUS_ERASE_FAILED;
	FLASH_InitStruct->FLASH_PFail_bit = NAND_STATUS_PROG_FAILED;
	FLASH_InitStruct->FLASH_ECC_bit = BIT_OFFSET_LEN_MASK_32(4, 2);

	/*The default state of the internal ECC is enabled. */
	FLASH_InitStruct->FLASH_ECC_EN_bit = NAND_CFG_ECC_ENABLE;

	FLASH_InitStruct->FLASH_BUF_CONT_bit = BIT(2);
}

void NAND_StructInit_GD(FLASH_InitTypeDef *FLASH_InitStruct)
{
	FLASH_InitStruct->FLASH_Id = FLASH_ID_GD;
	FLASH_InitStruct->FLASH_baud_rate = 1;
	FLASH_InitStruct->FLASH_IN_PHYSICAL_CYC = 0;

	/* status bit define */
	/* gigadevice QE bit is bit0 of feature 0xB0, QSPI is disabled by default */
	/* WP#, HOLD# are enabled by default*/
	FLASH_InitStruct->FLASH_QuadEn_bit = NAND_CFG_QUAD_ENABLE;
	FLASH_InitStruct->FLASH_Busy_bit = NAND_STATUS_BUSY;
	FLASH_InitStruct->FLASH_WLE_bit = NAND_STATUS_WEL;
	FLASH_InitStruct->FLASH_Status2_exist = 0;

	/* calibration data */
	FLASH_InitStruct->FLASH_rd_dummy_cycle[0] = NAND_DM_CYCLE_READ;
	FLASH_InitStruct->FLASH_rd_dummy_cycle[1] = NAND_DM_CYCLE_2O;
	FLASH_InitStruct->FLASH_rd_dummy_cycle[2] = NAND_DM_CYCLE_2IO;
	FLASH_InitStruct->FLASH_rd_dummy_cycle[3] = NAND_DM_CYCLE_4O;
	FLASH_InitStruct->FLASH_rd_dummy_cycle[4] = NAND_DM_CYCLE_4IO;

	/* calibration data in continuous mode */
	FLASH_InitStruct->FLASH_rd_c_dummy_cycle[0] = 0x08;
	FLASH_InitStruct->FLASH_rd_c_dummy_cycle[1] = 0x10;
	FLASH_InitStruct->FLASH_rd_c_dummy_cycle[2] = 0x08;
	FLASH_InitStruct->FLASH_rd_c_dummy_cycle[3] = 0x10;
	FLASH_InitStruct->FLASH_rd_c_dummy_cycle[4] = 0x08;

	/* set 2bit mode cmd */
	FLASH_InitStruct->FLASH_rd_dual_io = NAND_CMD_2READ;
	FLASH_InitStruct->FLASH_rd_dual_o = NAND_CMD_DREAD;

	/* set 4bit mode cmd */
	FLASH_InitStruct->FLASH_rd_quad_io = NAND_CMD_4READ;
	FLASH_InitStruct->FLASH_rd_quad_o = NAND_CMD_QREAD;

	/* other flash commnad set */
	FLASH_InitStruct->FLASH_cmd_wr_en = NAND_CMD_WREN;
	FLASH_InitStruct->FLASH_cmd_rd_id = NAND_CMD_RDID;
	FLASH_InitStruct->FLASH_cmd_rd_status = NAND_CMD_RDSR;
	FLASH_InitStruct->FLASH_cmd_rd_status2 = 0;
	FLASH_InitStruct->FLASH_cmd_wr_status = NAND_CMD_WRSR;
	FLASH_InitStruct->FLASH_cmd_wr_status2 = 0;
	FLASH_InitStruct->FLASH_cmd_chip_e = 0;
	FLASH_InitStruct->FLASH_cmd_block_e = NAND_CMD_BE;
	FLASH_InitStruct->FLASH_cmd_sector_e = 0;
	FLASH_InitStruct->FLASH_cmd_pwdn_release = 0;
	FLASH_InitStruct->FLASH_cmd_pwdn = 0;

	FLASH_InitStruct->FLASH_rd_fast_single = NAND_CMD_FREAD;
	FLASH_InitStruct->FLASH_addr_phase_len = NAND_COL_ADDR_2_BYTE;
	FLASH_InitStruct->FLASH_prm_byte = 0;

	FLASH_InitStruct->FLASH_pagesize_main_bit_exp = NAND_PAGE_SIZE_MAIN_2K_BIT_EXP;
	FLASH_InitStruct->FLASH_page_per_block_bit_exp = NAND_BLOCK_PAGE_CNT_64_BIT_EXP;

	/* basic page rd/wr operation */
	FLASH_InitStruct->FLASH_cmd_page_read = NAND_CMD_PAGERD;
	FLASH_InitStruct->FLASH_cmd_page_write = NAND_CMD_PROMEXEC;
	FLASH_InitStruct->FLASH_EFail_bit = NAND_STATUS_ERASE_FAILED;
	FLASH_InitStruct->FLASH_PFail_bit = NAND_STATUS_PROG_FAILED;
	FLASH_InitStruct->FLASH_ECC_bit = BIT_OFFSET_LEN_MASK_32(4, 2);

	/*The default state of the internal ECC is enabled. */
	FLASH_InitStruct->FLASH_ECC_EN_bit = NAND_CFG_ECC_ENABLE;

	FLASH_InitStruct->FLASH_BUF_CONT_bit = NAND_CFG_BUF_CONT_ENABLE;
}

void NAND_StructInit(FLASH_InitTypeDef *FLASH_InitStruct)
{
	FLASH_InitStruct->FLASH_Id = FLASH_ID_WINBOND;
	FLASH_InitStruct->FLASH_baud_rate = 1;
	FLASH_InitStruct->FLASH_IN_PHYSICAL_CYC = 0;

	/* status bit define */
	/* winbond QSPI is enable by default, set Bit1 of feature 0xA0 to disable QSPI*/
	/* WP#, HOLD# are disabled by default*/
	FLASH_InitStruct->FLASH_QuadEn_bit = 0;
	FLASH_InitStruct->FLASH_Busy_bit = NAND_STATUS_BUSY;
	FLASH_InitStruct->FLASH_WLE_bit = NAND_STATUS_WEL;
	FLASH_InitStruct->FLASH_Status2_exist = 0;

	/* calibration data */
	FLASH_InitStruct->FLASH_rd_dummy_cycle[0] = NAND_DM_CYCLE_READ;
	FLASH_InitStruct->FLASH_rd_dummy_cycle[1] = NAND_DM_CYCLE_2O;
	FLASH_InitStruct->FLASH_rd_dummy_cycle[2] = NAND_DM_CYCLE_2IO;
	FLASH_InitStruct->FLASH_rd_dummy_cycle[3] = NAND_DM_CYCLE_4O;
	FLASH_InitStruct->FLASH_rd_dummy_cycle[4] = NAND_DM_CYCLE_4IO;

	/* calibration data in continuous mode */
	FLASH_InitStruct->FLASH_rd_c_dummy_cycle[0] = 0x08;
	FLASH_InitStruct->FLASH_rd_c_dummy_cycle[1] = 0x10;
	FLASH_InitStruct->FLASH_rd_c_dummy_cycle[2] = 0x08;
	FLASH_InitStruct->FLASH_rd_c_dummy_cycle[3] = 0x10;
	FLASH_InitStruct->FLASH_rd_c_dummy_cycle[4] = 0x08;

	/* set 2bit mode cmd */
	FLASH_InitStruct->FLASH_rd_dual_io = NAND_CMD_2READ;
	FLASH_InitStruct->FLASH_rd_dual_o = NAND_CMD_DREAD;

	/* set 4bit mode cmd */
	FLASH_InitStruct->FLASH_rd_quad_io = NAND_CMD_4READ;
	FLASH_InitStruct->FLASH_rd_quad_o = NAND_CMD_QREAD;

	/* other flash commnad set */
	FLASH_InitStruct->FLASH_cmd_wr_en = NAND_CMD_WREN;
	FLASH_InitStruct->FLASH_cmd_rd_id = NAND_CMD_RDID;
	FLASH_InitStruct->FLASH_cmd_rd_status = NAND_CMD_RDSR;
	FLASH_InitStruct->FLASH_cmd_rd_status2 = 0;
	FLASH_InitStruct->FLASH_cmd_wr_status = NAND_CMD_WRSR;
	FLASH_InitStruct->FLASH_cmd_wr_status2 = 0;
	FLASH_InitStruct->FLASH_cmd_chip_e = 0;
	FLASH_InitStruct->FLASH_cmd_block_e = NAND_CMD_BE;
	FLASH_InitStruct->FLASH_cmd_sector_e = 0;
	FLASH_InitStruct->FLASH_cmd_pwdn_release = 0;
	FLASH_InitStruct->FLASH_cmd_pwdn = 0;

	FLASH_InitStruct->FLASH_rd_fast_single = NAND_CMD_FREAD;
	FLASH_InitStruct->FLASH_addr_phase_len = NAND_COL_ADDR_2_BYTE;
	FLASH_InitStruct->FLASH_prm_byte = 0;

	FLASH_InitStruct->FLASH_pagesize_main_bit_exp = NAND_PAGE_SIZE_MAIN_2K_BIT_EXP;
	FLASH_InitStruct->FLASH_page_per_block_bit_exp = NAND_BLOCK_PAGE_CNT_64_BIT_EXP;

	/* basic page rd/wr operation */
	FLASH_InitStruct->FLASH_cmd_page_read = NAND_CMD_PAGERD;
	FLASH_InitStruct->FLASH_cmd_page_write = NAND_CMD_PROMEXEC;
	FLASH_InitStruct->FLASH_EFail_bit = NAND_STATUS_ERASE_FAILED;
	FLASH_InitStruct->FLASH_PFail_bit = NAND_STATUS_PROG_FAILED;
	FLASH_InitStruct->FLASH_ECC_bit = BIT_OFFSET_LEN_MASK_32(4, 2);

	/*The default state of the internal ECC is enabled. */
	FLASH_InitStruct->FLASH_ECC_EN_bit = NAND_CFG_ECC_ENABLE;

	FLASH_InitStruct->FLASH_BUF_CONT_bit = NAND_CFG_BUF_CONT_ENABLE;
}


int NAND_Reset(void)
{
	u32 timeout = 1000;
	u8 nand_status = NAND_STATUS_BUSY;

	NAND_TxCmd(NAND_CMD_RESET, 0, NULL, 0, NULL);
	while ((nand_status & flash_init_para.FLASH_Busy_bit) && (timeout > 0)) {
		nand_status = NAND_GetStatus(NAND_REG_STATUS);
		timeout --;
	}

	if (timeout == 0) {
		printf("Flash may still be busy \n");
		return RTK_FAIL;
	} else {
		return RTK_SUCCESS;
	}
}

u8 NAND_Init(u8 SpicBitMode)
{
	/* no need to wait, has been done in rom */

	/* config fixed parameters */
	SPIC_Config(SPIC, &flash_init_para);

	/* Set SPI Mode */
	flash_init_para.FLASH_cur_bitmode = SpicBitMode;
	NAND_SetSpiMode(SPIC, &flash_init_para, SpicBitMode);

	return TRUE;
}

void nand_init(void)
{
	NAND_Init(Spic4IOBitMode);
}

void nand_copy(u8 *pData, u8 *HostAddr, size_t DataLen)
{
	u32 PageAddr, ByteAddr, ByteLen, BlockId;
	u32 NandAddr = (u32)HostAddr - SPI_FLASH_BASE;

	u32 page_begin = NAND_ADDR_TO_PAGE_ADDR(NandAddr);
	u32 page_end = NAND_ADDR_TO_PAGE_ADDR(NandAddr + DataLen - 1);
	u32 page_cnt = page_end - page_begin + 1;
	u32 good_block_id = 0;
	u32 pData_Offset = 0;

	while (page_cnt) {
		/*Maybe lookup the L2P table to find the mapping between logic addr and physical addr. */
		PageAddr = NAND_ADDR_TO_PAGE_ADDR(NandAddr);
		BlockId = NAND_PAGE_ADDR_TO_BLOCK_ID(PageAddr);
		if (BlockId >= good_block_id) {
			while (NAND_CheckBadBlock(BlockId)) {
				/*jump to next block when current block is bad block*/
				BlockId = BlockId + 1;
				PageAddr += NAND_BLOCK_PAGE_CNT;
				NandAddr += NAND_BLOCK_PAGE_CNT * NAND_PAGE_SIZE_MAIN;
			}
			good_block_id = BlockId + 1; /* next block we should check */
		}

		ByteAddr = NAND_ADDR_TO_BYTE_ADDR(NandAddr);
		ByteLen = (page_cnt == 1) ? DataLen : (NAND_PAGE_SIZE_MAIN - ByteAddr);

		/*Read Data from NandAddr to NandAddr+DataLen, and put the return data into pData[pData_Offset].*/
		u8 status = NAND_Page_Read(PageAddr, ByteAddr, ByteLen, &pData[pData_Offset]);
		if (0 != (status & NAND_STATUS_ECC_MASK)) {
			//RTK_LOGW(TAG, "Read Page 0x%lx of Block 0x%x May Fail, status 0x%x!\n",
			//		 PageAddr & NAND_BLOCK_PAGE_MASK, NAND_PAGE_ADDR_TO_BLOCK_ID(PageAddr), status);
		}

		pData_Offset = pData_Offset + ByteLen;
		NandAddr = NandAddr + ByteLen;
		DataLen = DataLen - ByteLen;
		page_cnt--;
	}
}



