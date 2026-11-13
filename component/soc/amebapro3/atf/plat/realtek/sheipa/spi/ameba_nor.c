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
#include <stdio.h>
#define HAL_OK 0
#define HAL_ERR_PARA -1
//extern FLASH_InitTypeDef flash_init_para;
/**
  * @brief  This function is used to configure SPIC according to parameters in FLASH_InitStruct.
  * @param FLASH_InitStruct: pointer to a FLASH_InitTypeDef
  *         structure that contains the configuration information for SPIC.
  * @retval none
  */
 
void FLASH_Config(SPIC_TypeDef *spi_flash, FLASH_InitTypeDef *FLASH_InitStruct)
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
	/* set dummy byte */
	spi_flash->DUM_BYTE = FLASH_InitStruct->FLASH_prm_byte;
}
/**
* @brief  Controls SPIC DPHY function - Enable/Disable SPIC DPHY
* @param FLASH_InitStruct: pointer to a FLASH_InitTypeDef
*		structure which will be initialized.
* @param ctrl: Enable/Disable
* @retval HAL_ERR_PARA:  Input arguments are invalid.
* @retval HAL_OK: SPIC DPHY control setting OK.
*/
 
u8 FLASH_SPIC_DPHY_CTRL(FLASH_InitTypeDef *FLASH_InitStruct, u8 ctrl)
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
 
void FLASH_SetSpiMode(SPIC_TypeDef *spi_flash, FLASH_InitTypeDef *FLASH_InitStruct, u8 SpicBitMode)
{
	u32 Value32;
	u32 ctrl0 = 0;
	u16 dclen = FLASH_InitStruct->FLASH_rd_dummy_cycle[SpicBitMode] * 2 * FLASH_InitStruct->FLASH_baud_rate;
	/* 4IO or 2IO should enable dummy byte function */
	if ((SpicBitMode == Spic4IOBitMode) || (SpicBitMode == Spic2IOBitMode)) {
		FLASH_InitStruct->FLASH_prm_avoid = 1;
	} else {
		FLASH_InitStruct->FLASH_prm_avoid = 0;
	}
	/* Disable SPI_FLASH User Mode */
	spi_flash->SSIENR = 0;
	/* set auto read dummy cycle and line delay */
	Value32 = spi_flash->AUTO_LENGTH & ~(MASK_RD_DUMMY_LENGTH);
	spi_flash->AUTO_LENGTH = Value32 | RD_DUMMY_LENGTH(dclen);
	/* set user read dummy cycle.
		If DUM_EN, push one dummy byte in addr phase manually and reduce one byte in dummy phase */
	if (FLASH_InitStruct->FLASH_prm_avoid) {
		if (SpicBitMode == Spic4IOBitMode) {
			dclen -= QUAD_DUM_CYCLE_NUM * 2 * FLASH_InitStruct->FLASH_baud_rate;
		} else if (SpicBitMode == Spic2IOBitMode) {
			dclen -= DUAL_DUM_CYCLE_NUM * 2 * FLASH_InitStruct->FLASH_baud_rate;
		}
	}
	Value32 = spi_flash->USER_LENGTH & ~MASK_USER_RD_DUMMY_LENGTH;
	spi_flash->USER_LENGTH = Value32 | USER_RD_DUMMY_LENGTH(dclen);
	/* clear multi channel first */
	ctrl0 = spi_flash->CTRLR0 & ~(MASK_CMD_CH | MASK_ADDR_CH | MASK_DATA_CH);
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
		FLASH_InitStruct->FLASH_cur_cmd = FLASH_CMD_READ;
	}
	if (FLASH_InitStruct->FLASH_prm_avoid) {
		Value32 |= BIT_DUM_EN;
	}
	spi_flash->VALID_CMD = Value32;
	spi_flash->CTRLR0 = ctrl0;
}
 
void FLASH_StructInit_Micron(FLASH_InitTypeDef *FLASH_InitStruct)
{
	FLASH_InitStruct->FLASH_Id = FLASH_ID_MICRON;
	FLASH_InitStruct->FLASH_baud_rate = 1;
	FLASH_InitStruct->FLASH_IN_PHYSICAL_CYC = 0;
	/* status bit define */
	FLASH_InitStruct->FLASH_QuadEn_bit = 0;
	FLASH_InitStruct->FLASH_Busy_bit = BIT(0);
	FLASH_InitStruct->FLASH_WLE_bit = BIT(1);
	FLASH_InitStruct->FLASH_Status2_exist = 0;
	/* calibration data */
	FLASH_InitStruct->FLASH_rd_dummy_cycle[0] = 0;
	/* set Micron rd_dummy_cyle based on baudrate, default 100MHz */
	/* FLASH_BAUD_100M */
	FLASH_InitStruct->FLASH_rd_dummy_cycle[1] = 0x05;
	FLASH_InitStruct->FLASH_rd_dummy_cycle[2] = 0x05;
	FLASH_InitStruct->FLASH_rd_dummy_cycle[3] = 0x09;
	FLASH_InitStruct->FLASH_rd_dummy_cycle[4] = 0x09;
	/* FLASH_BAUD_50M */
	//FLASH_InitStruct->FLASH_rd_dummy_cycle[1] = 0x01;
	//FLASH_InitStruct->FLASH_rd_dummy_cycle[2] = 0x03;
	/* FLASH_BAUD_25M */
	//FLASH_InitStruct->FLASH_rd_dummy_cycle[1] = 0x01;
	//FLASH_InitStruct->FLASH_rd_dummy_cycle[2] = 0x01;
	/* < 25MHz */
	//FLASH_InitStruct->FLASH_rd_dummy_cycle[1] = 0x01;
	//FLASH_InitStruct->FLASH_rd_dummy_cycle[2] = 0x01;
	/* set 2bit mode cmd */
	FLASH_InitStruct->FLASH_rd_dual_io = 0xBB;
	FLASH_InitStruct->FLASH_rd_dual_o = 0x3B;
	/* set 4bit mode cmd */
	FLASH_InitStruct->FLASH_rd_quad_io = 0xEB;
	FLASH_InitStruct->FLASH_rd_quad_o = 0x6B;
	/* other flash commnad set */
	FLASH_InitStruct->FLASH_cmd_wr_en = 0x06;
	FLASH_InitStruct->FLASH_cmd_rd_id = 0x9F;
	FLASH_InitStruct->FLASH_cmd_rd_status = 0x05;
	FLASH_InitStruct->FLASH_cmd_rd_status2 = 0;
	FLASH_InitStruct->FLASH_cmd_wr_status = 0x01;
	FLASH_InitStruct->FLASH_cmd_wr_status2 = 0;
	FLASH_InitStruct->FLASH_cmd_chip_e = 0xC7;
	FLASH_InitStruct->FLASH_cmd_block_e = 0xD8;
	FLASH_InitStruct->FLASH_cmd_sector_e = 0x20;
	FLASH_InitStruct->FLASH_cmd_pwdn_release = 0xAB;
	FLASH_InitStruct->FLASH_cmd_pwdn = 0xB9;
	FLASH_InitStruct->FLASH_rd_fast_single = FLASH_CMD_FREAD;
	FLASH_InitStruct->FLASH_addr_phase_len = ADDR_3_BYTE;
	FLASH_InitStruct->FLASH_prm_avoid = DISABLE;
	FLASH_InitStruct->FLASH_prm_byte = 0x0;
}
 
void FLASH_StructInit_MXIC(FLASH_InitTypeDef *FLASH_InitStruct)
{
	FLASH_InitStruct->FLASH_Id = FLASH_ID_MXIC;
	FLASH_InitStruct->FLASH_baud_rate = 1;
	FLASH_InitStruct->FLASH_IN_PHYSICAL_CYC = 0;
	/* status bit define */
	/* mxic  QSPI En: is bit6 */
	FLASH_InitStruct->FLASH_QuadEn_bit = BIT(6);
	FLASH_InitStruct->FLASH_Busy_bit = BIT(0);	//WIP
	FLASH_InitStruct->FLASH_WLE_bit = BIT(1);	//WLE
	FLASH_InitStruct->FLASH_Status2_exist = 0;
	/* calibration data */
	FLASH_InitStruct->FLASH_rd_dummy_cycle[0] = 0;
	FLASH_InitStruct->FLASH_rd_dummy_cycle[1] = 0x08;
	FLASH_InitStruct->FLASH_rd_dummy_cycle[2] = 0x04;
	FLASH_InitStruct->FLASH_rd_dummy_cycle[3] = 0x08;
	FLASH_InitStruct->FLASH_rd_dummy_cycle[4] = 0x06;
	/* set 2bit mode cmd */
	FLASH_InitStruct->FLASH_rd_dual_io = 0xBB;
	FLASH_InitStruct->FLASH_rd_dual_o = 0x3B;
	/* set 4bit mode cmd */
	FLASH_InitStruct->FLASH_rd_quad_io = 0xEB;
	FLASH_InitStruct->FLASH_rd_quad_o = 0x6B;
	/* other flash commnad set */
	FLASH_InitStruct->FLASH_cmd_wr_en = 0x06;
	FLASH_InitStruct->FLASH_cmd_rd_id = 0x9F;
	FLASH_InitStruct->FLASH_cmd_rd_status = 0x05;
	FLASH_InitStruct->FLASH_cmd_rd_status2 = 0;
	FLASH_InitStruct->FLASH_cmd_wr_status = 0x01;
	FLASH_InitStruct->FLASH_cmd_wr_status2 = 0;
	FLASH_InitStruct->FLASH_cmd_chip_e = 0x60;
	FLASH_InitStruct->FLASH_cmd_block_e = 0xD8;
	FLASH_InitStruct->FLASH_cmd_sector_e = 0x20;
	FLASH_InitStruct->FLASH_cmd_pwdn_release = 0xAB;
	FLASH_InitStruct->FLASH_cmd_pwdn = 0xB9;
	FLASH_InitStruct->FLASH_rd_fast_single = FLASH_CMD_FREAD;
	FLASH_InitStruct->FLASH_addr_phase_len = ADDR_3_BYTE;
	FLASH_InitStruct->FLASH_prm_avoid = DISABLE;
	FLASH_InitStruct->FLASH_prm_byte = 0x0;
}
 
void FLASH_StructInit_GD(FLASH_InitTypeDef *FLASH_InitStruct)
{
	FLASH_InitStruct->FLASH_Id = FLASH_ID_GD;
	FLASH_InitStruct->FLASH_baud_rate = 1;
	FLASH_InitStruct->FLASH_IN_PHYSICAL_CYC = 0;
	/* status bit define */
	/* winbond QSPI En: is status2 bit1, so is status bit9*/
	FLASH_InitStruct->FLASH_QuadEn_bit = BIT(9);
	FLASH_InitStruct->FLASH_Busy_bit = BIT(0);//WIP
	FLASH_InitStruct->FLASH_WLE_bit = BIT(1);//WLE
	FLASH_InitStruct->FLASH_Status2_exist = 1;
	/* calibration data */
	FLASH_InitStruct->FLASH_rd_dummy_cycle[0] = 0;
	FLASH_InitStruct->FLASH_rd_dummy_cycle[1] = 0x08;
	FLASH_InitStruct->FLASH_rd_dummy_cycle[2] = 0x04;
	FLASH_InitStruct->FLASH_rd_dummy_cycle[3] = 0x08;
	FLASH_InitStruct->FLASH_rd_dummy_cycle[4] = 0x06;
	/* set 2bit mode cmd */
	FLASH_InitStruct->FLASH_rd_dual_io = 0xBB;
	FLASH_InitStruct->FLASH_rd_dual_o = 0x3B;
	/* set 4bit mode cmd */
	FLASH_InitStruct->FLASH_rd_quad_io = 0xEB;
	FLASH_InitStruct->FLASH_rd_quad_o = 0x6B;
	/* other flash commnad set */
	FLASH_InitStruct->FLASH_cmd_wr_en = 0x06;
	FLASH_InitStruct->FLASH_cmd_rd_id = 0x9F;
	FLASH_InitStruct->FLASH_cmd_rd_status = 0x05;
	FLASH_InitStruct->FLASH_cmd_rd_status2 = 0x35;
	FLASH_InitStruct->FLASH_cmd_wr_status = 0x01;
	FLASH_InitStruct->FLASH_cmd_wr_status2 = 0;
	FLASH_InitStruct->FLASH_cmd_chip_e = 0x60;
	FLASH_InitStruct->FLASH_cmd_block_e = 0xD8;
	FLASH_InitStruct->FLASH_cmd_sector_e = 0x20;
	FLASH_InitStruct->FLASH_cmd_pwdn_release = 0xAB;
	FLASH_InitStruct->FLASH_cmd_pwdn = 0xB9;
	FLASH_InitStruct->FLASH_rd_fast_single = FLASH_CMD_FREAD;
	FLASH_InitStruct->FLASH_addr_phase_len = ADDR_3_BYTE;
	FLASH_InitStruct->FLASH_prm_avoid = DISABLE;
	FLASH_InitStruct->FLASH_prm_byte = 0x0;
}
 
void FLASH_StructInit(FLASH_InitTypeDef *FLASH_InitStruct)
{
	FLASH_InitStruct->FLASH_Id = FLASH_ID_WINBOND;
	FLASH_InitStruct->FLASH_baud_rate = 1;
	FLASH_InitStruct->FLASH_IN_PHYSICAL_CYC = 0;
	/* status bit define */
	/* winbond QSPI En: is status2 bit1, so is status bit9*/
	FLASH_InitStruct->FLASH_QuadEn_bit = BIT(9);
	FLASH_InitStruct->FLASH_Busy_bit = FLASH_STATUS_BUSY;
	FLASH_InitStruct->FLASH_WLE_bit = FLASH_STATUS_WLE;
	FLASH_InitStruct->FLASH_Status2_exist = 1;
	/* calibration data */
	FLASH_InitStruct->FLASH_rd_dummy_cycle[0] = 0;
	FLASH_InitStruct->FLASH_rd_dummy_cycle[1] = FLASH_DM_CYCLE_2O;
	FLASH_InitStruct->FLASH_rd_dummy_cycle[2] = FLASH_DM_CYCLE_2IO;
	FLASH_InitStruct->FLASH_rd_dummy_cycle[3] = FLASH_DM_CYCLE_4O;
	FLASH_InitStruct->FLASH_rd_dummy_cycle[4] = FLASH_DM_CYCLE_4IO;
	/* set 2bit mode cmd */
	FLASH_InitStruct->FLASH_rd_dual_io = FLASH_CMD_2READ;
	FLASH_InitStruct->FLASH_rd_dual_o = FLASH_CMD_DREAD;
	/* set 4bit mode cmd */
	FLASH_InitStruct->FLASH_rd_quad_io = FLASH_CMD_4READ;
	FLASH_InitStruct->FLASH_rd_quad_o = FLASH_CMD_QREAD;
	/* other flash commnad set */
	FLASH_InitStruct->FLASH_cmd_wr_en = FLASH_CMD_WREN;
	FLASH_InitStruct->FLASH_cmd_rd_id = FLASH_CMD_RDID;
	FLASH_InitStruct->FLASH_cmd_rd_status = FLASH_CMD_RDSR;
	FLASH_InitStruct->FLASH_cmd_rd_status2 = 0x35;//FLASH_CMD_RDSR2;
	FLASH_InitStruct->FLASH_cmd_wr_status = FLASH_CMD_WRSR;
	FLASH_InitStruct->FLASH_cmd_wr_status2 = 0x31;
	FLASH_InitStruct->FLASH_cmd_chip_e = FLASH_CMD_CE;
	FLASH_InitStruct->FLASH_cmd_block_e = FLASH_CMD_BE;
	FLASH_InitStruct->FLASH_cmd_sector_e = FLASH_CMD_SE;
	FLASH_InitStruct->FLASH_cmd_pwdn_release = FLASH_CMD_RDP;
	FLASH_InitStruct->FLASH_cmd_pwdn = FLASH_CMD_DP;
	FLASH_InitStruct->FLASH_rd_fast_single = FLASH_CMD_FREAD;
	FLASH_InitStruct->FLASH_addr_phase_len = ADDR_3_BYTE;
	FLASH_InitStruct->FLASH_prm_avoid = DISABLE;
	FLASH_InitStruct->FLASH_prm_byte = 0x0;
}
 
u8 FLASH_Init(u8 SpicBitMode)
{
	SPIC_TypeDef *spi_flash = SPIC;
	SYSON_TypeDef *syson_reg = SYSON_BASE;
	FLASH_InitTypeDef *FLASH_InitStruct = &flash_init_para;
	/* (Follow AmebaPro2) Wait SPIC BOOT_FINISH */
	while ((syson_reg->SYS_FLASH_CTRL & SYSON_BIT_HS_FLASH_INIT_OK) == 0);
	/* (Follow AmebaPro2) Enable SPIC PHY RX */
	/* Note: This is SPIC PHY, NOT SPIC DPHY */
	syson_reg->SYS_FLASH_PHY_CTRL0 |= SYSON_BIT_FLASH_PHY_EN;
	/* config fixed parameters */
	FLASH_Config(spi_flash, FLASH_InitStruct);
	/* Set SPI Mode */
	FLASH_InitStruct->FLASH_cur_bitmode = SpicBitMode;
	FLASH_SetSpiMode(spi_flash, FLASH_InitStruct, SpicBitMode);
	return TRUE;
}
void nor_init(void)
{
	FLASH_Init(flash_init_para.FLASH_cur_bitmode);
}
