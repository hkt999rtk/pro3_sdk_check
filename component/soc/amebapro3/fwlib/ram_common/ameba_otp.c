/**
  ******************************************************************************
  * @file    ameba_otp_rom.c
  * @author
  * @version V1.0.0
  * @date    2016-05-17
  * @brief   This file contains all the functions prototypes for the OTP firmware
  *          library.
  ******************************************************************************
  * @attention
  *
  * This module is a confidential and proprietary property of RealTek and
  * possession or use of this module requires written permission of RealTek.
  *
  * Copyright(c) 2015, Realtek Semiconductor Corporation. All rights reserved.
  ******************************************************************************
  */
#include <string.h>
#include "ameba_soc.h"

//static const char *TAG = "OTP";
otp_adapter_t otp_adpt;

/**
* @brief The global common data structure to store common resource
*        for all OTP adapters.
*/
otp_adapter_t *_potp_adpt;


/**
  * @brief  Get the CRC status of security area 0x280~0x450.
  * @param None
  * @retval CRC status:
  *          - 1: crc check failed
  *          - 0: crc check passed
  */

u32 OTP_GetCRC(void)
{
	u32 crc_status = 0;
	//crc_status = HAL_READ32(AON_REG_BASE, AON_REG_OTP_PRCT_CTRL);
	crc_status = AON_BASE->OTP_PRCT_CTRL;
	return crc_status;
}

/**
  * @brief  Set the CRC of security area.
  * @param None
  * @retval status value:
  *          - RTK_SUCCESS: write ok
  *          - RTK_FAIL: write fail
  */
int OTP_SetCRC(void)
{
	RTK_LOGE("OTP", "_OTP_WriteCRC Fail, Not implement");
	return FAIL;
}

/**
  * @brief  physical map read one byte
  * @param  Addr: otp physical address
  * @param  Data: one byte data buffer for otp data
  * @retval status value:
  *          - SUCCESS: read ok
  *          - FAIL: read fail
  */

int OTP_Read8(u32 Addr, u8 *Data)
{
	u32 bResult = FAIL;
	//volatile u32 *AccessReg = NULL;
	uint32_t reg_tmp = 0;
	OTPC_TypeDef *OTPC  = OTPC_DEV;

	if (Addr >= OTP_REAL_CONTENT_LEN) {
		*Data = 0xff;
		return bResult;
	}
	//rtw_enter_critical(NULL, NULL);
	//otp init
	//u8     otp_adj_vol_lvl = 0xFF;
	//RTK_LOGI("OTP","otp init-> adpt addr: %08x, padpt addr: %08x\n", &otp_adpt, &_potp_adpt);
	_potp_adpt = &otp_adpt;
	_potp_adpt->slpc_en_dly = OTP_DEFAULT_SLPCEN_DLY;
	_potp_adpt->pc_en_dly   = OTP_DEFAULT_PCEN_DLY;
	_potp_adpt->wr_dly      = OTP_DEFAULT_WR_DLY;
	_potp_adpt->rd_dly      = OTP_DEFAULT_RD_DLY;
	//otp_adj_vol_lvl = 0x08;

	/* Enable OTP controller */
	//reg_tmp = HAL_READ32(AON_REG_BASE, AON_REG_OTP_PWR_CTRL);
	//reg_tmp |= BIT(0);
	//HAL_WRITE32(AON_REG_BASE, AON_REG_OTP_PWR_CTRL, reg_tmp);
	AON_BASE->OTP_PWR_CTRL |= AON_BIT_OTP_CTRLER_FEN;
	AON_BASE->OTP_PWR_CTRL &= ~AON_BIT_OTP_RDCLK_GT_EN;
	//*((volatile u32 *)(0x402200A4)) &= ~(1 << 6);

	/* set mode/addr first */
	reg_tmp = OTPC->OTP_AS_CTRL_A;
	reg_tmp &= ~OTPC_MASK_OTP_MODE_SEL;
	reg_tmp &= ~OTPC_MASK_OTP_ADDR;
	reg_tmp &= ~OTPC_MASK_OTP_RDATA;
	reg_tmp |= OTP_MODE_SEL(0); //OTPRdMod
	reg_tmp |= OTP_ADDR(Addr);
	/* clear ready bit first */
	reg_tmp |=  OTPC_BIT_OTP_RDY;
	//RTK_LOGI("OTP","to write AS_CTRL_A: %08x\n", reg_tmp);
	OTPC->OTP_AS_CTRL_A = reg_tmp;
	/* enable read operation */
	reg_tmp &=  ~OTPC_BIT_OTP_RDY;
	reg_tmp |= OTPC_BIT_OTP_OP_EN;
	//RTK_LOGI("OTP","to write AS_CTRL_A: %08x\n", reg_tmp);
	OTPC->OTP_AS_CTRL_A = reg_tmp;

	int ret_val = 0;
	reg_tmp = OTPC->OTP_AS_CTRL_A;
	/* wait for operation done */


	while (((reg_tmp & OTPC_BIT_OTP_RDY) == 0) && (ret_val < OTP_RW_TO_CNT)) {
		DelayUs(OTP_RD_CHK_INTV);
		reg_tmp = OTPC->OTP_AS_CTRL_A;
		//RTK_LOGI("OTP","reg_tmp: %08x\r\n", reg_tmp);
		ret_val++;
	}
	reg_tmp = OTPC->OTP_AS_CTRL_A;
	if (ret_val != OTP_RW_TO_CNT) {
		*(Data) = GET_OTP_RDATA(reg_tmp);
	}

	reg_tmp |=  OTPC_BIT_OTP_RDY;
	reg_tmp &=  ~OTPC_BIT_OTP_OP_EN;
	OTPC->OTP_AS_CTRL_A = reg_tmp;
	if (ret_val == OTP_RW_TO_CNT) {
		RTK_LOGE("OTP", "timeout \n");
		bResult = FAIL;
	} else {
		bResult = SUCCESS;
	}

	/* Disable OTP controller */
	//reg_tmp = HAL_READ32(AON_REG_BASE, AON_REG_OTP_PWR_CTRL);
	//reg_tmp &= ~BIT(0);
	//HAL_WRITE32(AON_REG_BASE, AON_REG_OTP_PWR_CTRL, reg_tmp);
	AON_BASE->OTP_PWR_CTRL &= ~AON_BIT_OTP_CTRLER_FEN;
	AON_BASE->OTP_PWR_CTRL |= AON_BIT_OTP_RDCLK_GT_EN;
	//*((volatile u32 *)(0x402200A4)) |= (1 << 6);

	//rtw_exit_critical(NULL, NULL);
	return bResult;
}

/**
  * @brief  physical map write one byte
  * @param  Addr: otp physical address
  * @param  Data: one byte data to write
  * @retval status value:
  *          - SUCCESS: write ok
  *          - FAIL: write fail
  */

int _OTP_Write8(u32 Addr, u8 Data)
{
	u32 bResult = FAIL;
	//volatile u32 *AccessReg = NULL;
	u32 reg_tmp = 0;
	OTPC_TypeDef *OTPC  = OTPC_DEV;
	//u8     otp_adj_vol_lvl = 0xFF;

	if (Addr >= OTP_REAL_CONTENT_LEN) {
		return bResult;
	}
	//rtw_enter_critical(NULL, NULL);
	//otp init

	//RTK_LOGI("OTP","otp init-> adpt addr: %08x, padpt addr: %08x\n", &otp_adpt, &_potp_adpt);
	_potp_adpt = &otp_adpt;
	_potp_adpt->slpc_en_dly = OTP_DEFAULT_SLPCEN_DLY;
	_potp_adpt->pc_en_dly   = OTP_DEFAULT_PCEN_DLY;
	_potp_adpt->wr_dly      = OTP_DEFAULT_WR_DLY;
	_potp_adpt->rd_dly      = OTP_DEFAULT_RD_DLY;
	//otp_adj_vol_lvl = 0x08;

	/* Enable OTP controller */
	//reg_tmp = HAL_READ32(AON_REG_BASE, AON_REG_OTP_PWR_CTRL);
	//reg_tmp |= BIT(0);
	//HAL_WRITE32(AON_REG_BASE, AON_REG_OTP_PWR_CTRL, reg_tmp);
	AON_BASE->OTP_PWR_CTRL |= AON_BIT_OTP_CTRLER_FEN;
	AON_BASE->OTP_PWR_CTRL &= ~(AON_BIT_OTP_RDCLK_GT_EN | AON_BIT_OTP_PGCLK_GT_EN);
	//*((volatile u32 *)(0x402200A4)) &= ~(0x3 << 6);

	/* Set grant code for OTP program */
	OTPC->OTP_PMC_DBG_CTRL &= ~OTPC_MASK_OTP_BURN_GNT;
	OTPC->OTP_PMC_DBG_CTRL |= OTP_BURN_GNT(0x69);

	/* set mode/addr first */
	reg_tmp = OTPC->OTP_AS_CTRL_A;
	reg_tmp &= ~OTPC_MASK_OTP_MODE_SEL;
	reg_tmp &= ~OTPC_MASK_OTP_ADDR;
	reg_tmp &= ~OTPC_MASK_OTP_WDATA;
	reg_tmp |= OTP_MODE_SEL(2); //OTPWRMod
	reg_tmp |= OTP_ADDR(Addr);
	reg_tmp |= OTP_WDATA(Data);
	/* clear ready bit first */
	reg_tmp |=  OTPC_BIT_OTP_RDY;
	//RTK_LOGI("OTP","to write AS_CTRL_A: %08x\n", reg_tmp);
	OTPC->OTP_AS_CTRL_A = reg_tmp;
	/* enable read operation */
	reg_tmp &=  ~OTPC_BIT_OTP_RDY;
	reg_tmp |= OTPC_BIT_OTP_OP_EN;
	//RTK_LOGI("OTP","to write AS_CTRL_A: %08x\n", reg_tmp);
	OTPC->OTP_AS_CTRL_A = reg_tmp;

	int ret_val = 0;
	reg_tmp = OTPC->OTP_AS_CTRL_A;
	/* wait for operation done */


	while (((reg_tmp & OTPC_BIT_OTP_RDY) == 0) && (ret_val < OTP_RW_TO_CNT)) {
		DelayUs(OTP_RD_CHK_INTV);
		reg_tmp = OTPC->OTP_AS_CTRL_A;
		//RTK_LOGI("OTP","reg_tmp: %08x\r\n", reg_tmp);
		ret_val++;
	}

	reg_tmp |=  OTPC_BIT_OTP_RDY;
	reg_tmp &=  ~OTPC_BIT_OTP_OP_EN;
	OTPC->OTP_AS_CTRL_A = reg_tmp;
	if (ret_val == OTP_RW_TO_CNT) {
		RTK_LOGE("OTP", "timeout \n");
		bResult = FAIL;
	} else {
		bResult = SUCCESS;
	}

	/* Clear grant code for OTP program */
	OTPC->OTP_PMC_DBG_CTRL &= ~OTPC_MASK_OTP_BURN_GNT;

	/* Disable OTP controller */
	//reg_tmp = HAL_READ32(AON_REG_BASE, AON_REG_OTP_PWR_CTRL);
	//reg_tmp &= ~BIT(0);
	//HAL_WRITE32(AON_REG_BASE, AON_REG_OTP_PWR_CTRL, reg_tmp);
	AON_BASE->OTP_PWR_CTRL &= ~AON_BIT_OTP_CTRLER_FEN;
	AON_BASE->OTP_PWR_CTRL |= (AON_BIT_OTP_RDCLK_GT_EN | AON_BIT_OTP_PGCLK_GT_EN);
	//*((volatile u32 *)(0x402200A4)) |= (0x3 << 6);
	//rtw_exit_critical(NULL, NULL);
	return bResult;
}


/**
  * @brief  physical map write one byte
  * @param  Addr: otp physical address
  * @param  Data: one byte data to write
  * @retval status value:
  *          - SUCCESS: write ok
  *          - FAIL: write fail
  */

int OTP_Write8(u32 Addr, u8 Data)
{
	u8 Temp = 0;
	u8 Target;
	u8 retry = 0;

	Target = Data;
	//printf("OTP_Write8 Addr 0x%x     Data 0x%x \n\r",Addr ,Data);
	if (OTP_Read8(Addr, &Temp) == FAIL) {
		RTK_LOGE("OTP", "OTP Read error!\n");
		goto exit;
	}

retry:

	/*do not need program bits include originally do not need program
		(bits equals 1 in data) and already prgoramed bits(bits euqals 0 in Temp) */
	Data |= ~Temp;

	/*program*/
	if (_OTP_Write8(Addr, Data) == FAIL) {
		RTK_LOGE("OTP", "OTP program error!\n");
		goto exit;
	}

	/*Read after program*/
	if (OTP_Read8(Addr, &Temp) == FAIL)  {
		RTK_LOGE("OTP", "OTP Read error!\n");
		goto exit;
	}

	/*program do not get desired value,the OTP can be programmed at most 3 times
		here only try once.*/
	if (Temp != Target) {
		if ((retry++) < 1) {
			goto retry;
		} else {
			goto exit;
		}
	}

	return SUCCESS;

exit:
	return FAIL;


}


/**
  * @brief  physical map write four byte
  * @param  Addr: otp physical address
  * @param  Data: four byte data to Read
  * @retval status value:
  *          - SUCCESS: write ok
  *          - FAIL: write fail
  */

int OTP_Read32(u32 Addr, u32 *Data)
{
	u8 Temp = 0;
	u8 i;
	*Data = 0;

	for (i = 0; i < 4; i++) {
		if (OTP_Read8(Addr++, &Temp) == FAIL) {
			goto exit;
		}
		*Data |= (((u32)Temp) << (8 * i));
	}

	return SUCCESS;
exit:
	return FAIL;
}


/**
  * @brief  read OTP logical map
  * @param  pbuf: OTP_LMAP_LEN len buffer used for OTP Logical map
  * @retval status value:
  *          - SUCCESS: read ok
  *          - FAIL: read fail
  */

int OTP_LogicalRead(u8 *pbuf, u32 laddr, u32 len)
{
	u16 phy_addr = 0;
	u16 offset;
	u8 wden;
	u8 header, extheader, data;
	u16 i;

	if (!pbuf) {
		return FAIL;
	}
	memset(pbuf, 0xFF, len);


	while (phy_addr < OTP_LOGICAL_MAP_HW_SIZE) {
		/*First 4 bytes are reserved for physical*/
		if (phy_addr == 0 || phy_addr == 1 || phy_addr == 2 || phy_addr == 3) {
			phy_addr++;
			continue;
		}
		OTP_Read8(phy_addr++, &data);
		header = data;

		if (header == 0xFF) {/* not write */
			break;
		}

		/* Check PG header for section num. */
		OTP_Read8(phy_addr++, &data);
		extheader = data;

		offset = (header & 0x0F) << 4;
		offset |= ((extheader & 0xF0) >> 4);
		wden = (extheader & 0x0F);

		/*One section has 8 bytes data, logical map has 512/8 = 64 sections*/
		if (offset < (OTP_LMAP_LEN >> 3)) {
			u32 addr = 0;
			/* Get word enable value from PG header */
			addr = offset * 8;
			/*Each section has 4 words data*/
			for (i = 0; i < 4; i++) {
				/* Check word enable condition in the section */
				if (!(wden & (0x01 << i))) {

					OTP_Read8(phy_addr++, &data);

					if (addr >= laddr && addr < (laddr + len)) {
						pbuf[addr - laddr] = data;
					}

					OTP_Read8(phy_addr++, &data);

					if ((addr + 1) >= laddr && (addr + 1) < (laddr + len)) {
						pbuf[addr + 1 - laddr] = data;
					}
				}
				addr += 2;
			}
		} else {
			u8 word_cnts = 0;
			if (!(wden & BIT(0)))	{
				word_cnts++;    // 0 : write enable
			}
			if (!(wden & BIT(1)))	{
				word_cnts++;
			}
			if (!(wden & BIT(2)))	{
				word_cnts++;
			}
			if (!(wden & BIT(3)))	{
				word_cnts++;
			}
			phy_addr += word_cnts * 2; //word
		}
	}

	return SUCCESS;
}


static int otp_pg_packet(u8 offset, u8 wden, u8 *data)
{
	u16 idx = 4; //the first 4 addresses are reserved
	u8 temp0, temp1, WordEn;
	u8 Len = 0;
	u8 word_idx;

	/* WordEnable bit=0 means write this bit */
	if ((wden & 0xF) == 0xF) {
		return -1;
	}

	//count the physical written num of word
	while (idx < OTP_LOGICAL_MAP_HW_SIZE) {
		OTP_Read8(idx, &temp0);
		if (temp0 != 0xff) {//used
			idx++;
			OTP_Read8(idx, &temp1);

			//Addr = ( ((temp0&0x0f)<<4) | ((temp1&0xf0)>>4) );//logical addr
			WordEn = ((~temp1) & 0x0f); //~ write enbale

			// bit=0: word not write
			// bit=1: word have write
			while (WordEn != 0) {
				if (WordEn & BIT0) {
					idx = idx + 2; //word have write
				}
				WordEn = WordEn >> 1;
			}
		} else {//find address not written
			break;
		}
		idx++;
	}
	//CODE WORD
	wden = wden & 0xf;

	if ((wden & BIT(0)) == 0)	{
		Len = Len + 2;    // 0 : write enable
	}
	if ((wden & BIT(1)) == 0)	{
		Len = Len + 2;
	}
	if ((wden & BIT(2)) == 0)	{
		Len = Len + 2;
	}
	if ((wden & BIT(3)) == 0)	{
		Len = Len + 2;
	}

	if ((idx + Len) < (OTP_LOGICAL_MAP_HW_SIZE)) {
		OTP_Write8(idx, ((offset >> 4) & 0x0f) | 0x30); //addr[7:4]
		idx++;
		OTP_Write8(idx, ((offset & 0x0f) << 4) | wden); //addr[3:0]
		idx++;
		for (word_idx = 0; word_idx < 4; word_idx ++) {
			if ((wden & BIT(word_idx)) == 0) {
				OTP_Write8(idx, *(data + word_idx * 2));
				idx++;
				OTP_Write8(idx, *(data + word_idx * 2 + 1));
				idx++;
			}
		}
	} else {
		RTK_LOGE("OTP", " OTP PG No Enough Space! \n");
		return -1;
	}
	return 0;
}

/**
  * @brief  write OTP logical address
  * @param  addr: logical addr
  * @param  cnts: byte number, should be even
  * @param  data: data buffer to be write
  * @retval status value:
  *          - SUCCESS: write ok
  *          - FAIL: write fail
  */

int OTP_LogicalWrite(u32 addr, u32 cnts, u8 *data)
{

	u8	offset, word_en;
	u8	map[OTP_LMAP_LEN];
	u16	mapLen = OTP_LMAP_LEN;
	u8	newdata[8];
	u32	i, idx;
	int	ret = 0, used_bytes;

	if ((addr + cnts) > mapLen) {
		return FAIL;
	}

	used_bytes = OTP_LogicalRead(map, 0, mapLen);
	if (used_bytes < 0) {
		return FAIL;
	}

	if (addr < 0x200) { // without auto load
		offset = (addr >> 3);
		word_en = 0xF;
		memset(newdata, 0xFF, 8);
		i = addr & 0x7;	// index of one package
		idx = 0;	// data index

		if (i & 0x1) {
			// odd start
			if (data[idx] != map[addr + idx]) {
				word_en &= ~(BIT(i >> 1));
				newdata[i - 1] = map[addr + idx - 1];
				newdata[i] = data[idx];
			}
			i++;
			idx++;
		}
		do {
			for (; i < 8; i += 2) {
				if (cnts == idx) {
					break;
				}
				if ((cnts - idx) == 1) {
					if (data[idx] != map[addr + idx]) {
						word_en &= ~(BIT(i >> 1));
						newdata[i] = data[idx];
						newdata[i + 1] = map[addr + idx + 1];
					}
					idx++;
					break;
				} else {
					if ((data[idx] != map[addr + idx]) ||
						(data[idx + 1] != map[addr + idx + 1])) {
						word_en &= ~(BIT(i >> 1));
						newdata[i] = data[idx];
						newdata[i + 1] = data[idx + 1];
					}
					idx += 2;
				}
				if (idx == cnts) {
					break;
				}
			}

			if (word_en != 0xF) {
				//printf("otp_pg_packet = 0x%x  0x%x \n\r",offset , word_en);
				ret = otp_pg_packet(offset, word_en, newdata);
				if (ret < 0) {
					return ret;
				}
			}

			if (idx == cnts) {
				break;
			}

			offset++;
			i = 0;
			word_en = 0xF;
			memset(newdata, 0xFF, 8);
		} while (1);

	} else { // logical auto load area
		offset = (addr >> 3);
		word_en = 0xF;
		memset(newdata, 0xFF, 8);
		i = addr & 0x7;	// index of one package
		idx = 0;	// data index

		if (i & 0x1) {
			// odd start
			if ((data[idx] != map[addr + idx]) || (data[idx] == 0xFF)) {
				word_en &= ~(BIT(i >> 1));
				newdata[i - 1] = map[addr + idx - 1];
				newdata[i] = data[idx];
			}
			i++;
			idx++;
		}
		do {
			for (; i < 8; i += 2) {
				if (cnts == idx) {
					break;
				}
				if ((cnts - idx) == 1) {
					if ((data[idx] != map[addr + idx]) || (data[idx] == 0xFF)) {
						word_en &= ~(BIT(i >> 1));
						newdata[i] = data[idx];
						newdata[i + 1] = map[addr + idx + 1];
					}
					idx++;
					break;
				} else {
					if ((data[idx] != map[addr + idx]) || (data[idx] == 0xFF) ||
						(data[idx + 1] != map[addr + idx + 1]) || (data[idx + 1] == 0xFF)) {
						word_en &= ~(BIT(i >> 1));
						newdata[i] = data[idx];
						newdata[i + 1] = data[idx + 1];
					}
					idx += 2;
				}
				if (idx == cnts) {
					break;
				}
			}

			if (word_en != 0xF) {
				//printf("otp_pg_packet = 0x%x  0x%x \n\r",offset , word_en);
				ret = otp_pg_packet(offset, word_en, newdata);
				if (ret < 0) {
					return ret;
				}
			}

			if (idx == cnts) {
				break;
			}

			offset++;
			i = 0;
			word_en = 0xF;
			memset(newdata, 0xFF, 8);
		} while (1);

	}


	return SUCCESS;
}

/**
  * @brief  Get OTP logical address remain length.
  * @param  none
  * @retval OTP logical address remain length
  */

u32 OTP_LogicalGetRemain(void)
{
	u16 phy_addr = 0;
	u8 header, extheader, data = 0;
	u8 wden, word_cnts;

	/*First 4 bytes are reserved for physical*/
	phy_addr = 4;
	while (phy_addr < OTP_LOGICAL_MAP_HW_SIZE) {

		OTP_Read8(phy_addr, &data);
		header = data;

		if (header == 0xFF) {/* not write */
			break;
		}
		phy_addr++;
		OTP_Read8(phy_addr, &data);
		extheader = data;

		wden = (extheader & 0x0F);
		word_cnts = 0;
		if (!(wden & BIT(0)))	{
			word_cnts++;    // 0 : write enable
		}
		if (!(wden & BIT(1)))	{
			word_cnts++;
		}
		if (!(wden & BIT(2)))	{
			word_cnts++;
		}
		if (!(wden & BIT(3)))	{
			word_cnts++;
		}
		phy_addr += word_cnts * 2; //word

		phy_addr++;
	}

	return (OTP_LOGICAL_MAP_HW_SIZE - phy_addr);

}

u32 otp_logical_remain(void)
{
	return OTP_LogicalGetRemain();
}

/**
 * @brief Write a byte buffer to physical OTP.
 * @param addr Start physical OTP address.
 * @param data Data buffer to write.
 * @param len Number of bytes to write.
 * @return RTK_SUCCESS on success, RTK_FAIL on any write error.
 */
int OTP_WritePhyBuf(u32 addr, const u8 *data, u32 len)
{
	u32 i;

	for (i = 0; i < len; i++) {
		if (OTP_Write8(addr + i, data[i]) != RTK_SUCCESS) {
			RTK_LOGS(NOTAG, RTK_LOG_ERROR, "W %x fail\n", addr + i);
			return RTK_FAIL;
		}
	}
	return RTK_SUCCESS;
}

/**
 * @brief Clear a single bit in physical OTP (irreversible 1->0).
 * @param addr OTP byte address.
 * @param bit Bit index (0-7) to clear.
 * @return RTK_SUCCESS on success, RTK_FAIL on read/write error.
 */
int OTP_ClearPhyBit(u32 addr, u8 bit)
{
	u8 rd;
	u8 wr_val;

	if (bit > 7) {
		return RTK_FAIL;
	}
	if (OTP_Read8(addr, &rd) != RTK_SUCCESS) {
		return RTK_FAIL;
	}
	wr_val = rd & ~(u8)(1 << bit);
	if (OTP_Write8(addr, wr_val) != RTK_SUCCESS) {
		return RTK_FAIL;
	}
	return RTK_SUCCESS;
}

/**
 * @brief Set or clear a bit in logical OTP.
 * @param addr Logical OTP address.
 * @param bit Bit position (0-7) to modify.
 * @param val 1 to set the bit, 0 to clear it.
 * @return RTK_SUCCESS on success, RTK_FAIL on read/write error.
 */
int OTP_SetLogBit(u32 addr, u8 bit, u8 val)
{
	u8 old, new_val;

	if (bit > 7) {
		return RTK_FAIL;
	}

	if (OTP_LogicalRead(&old, addr, 1) != RTK_SUCCESS) {
		RTK_LOGS(NOTAG, RTK_LOG_ERROR, "log rd fail %x\n", addr);
		return RTK_FAIL;
	}

	/* First-time programming: logical OTP reads 0xFF but the
	 * corresponding system register holds 0x00. Use 0x00 as
	 * the base so the target bit is actually set.
	 */
	if (old == 0xFF) {
		old = 0x00;
	}

	if (val) {
		new_val = old | (1 << bit);
	} else {
		new_val = old & ~(1 << bit);
	}

	if (OTP_LogicalWrite(addr, 1, &new_val) != RTK_SUCCESS) {
		return RTK_FAIL;
	}
	return RTK_SUCCESS;
}
/******************* (C) COPYRIGHT 2016 Realtek Semiconductor *****END OF FILE****/
