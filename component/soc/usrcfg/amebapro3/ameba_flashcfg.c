/*
* Copyright (c) 2024 Realtek Semiconductor Corp.
*
* SPDX-License-Identifier: Apache-2.0
*/

#include "ameba_soc.h"

void flash_init_userdef(void);
void nand_init_userdef(void);

/**
* @brif Indicate the flash baudrate. It can be one of the following value:
*	CLKDIV(5): => SPIC clock = 1/5 pll
*	CLKDIV(4): => SPIC clock = 1/4 pll
*	other value is not support.
*/
const u16 Flash_Speed = CLKDIV(5);
const u16 Data_Flash_Speed = CLKDIV(5);

/**
* @brif Indicate the flash read I/O mode. It can be one of the following value:
*	0xFFFF: Read quad IO, Address & Data 4 bits mode
*	0x7FFF: Read quad O, Just data 4 bits mode
*	0x3FFF: Read dual IO, Address & Data 2 bits mode
*	0x1FFF: Read dual O, Just data 2 bits mode
*	0x0FFF: 1 bit mode
* @note If the configured read mode is not supported, other modes would be searched until find the appropriate mode.
*/
const u16 Flash_ReadMode = 0xFFFF;
const u16 Data_Flash_ReadMode = 0xFFFF;


/**
* @brif Flash_AVL maintains the flash IC supported by SDK.
*	If users want to adpot new flash, add item in the following AVL.
*	Note that, if new flash can be classfied as one of the Realtek-defined categories according to Classification SPEC,
*	filling in the defined class index is necessary. Otherwise, FlashClassUser can be used to indicate new class.
*/
const FlashInfo_TypeDef Flash_AVL[] = {
	/*flash_id,		flash_id_mask,	flash_class,		status_mask,	FlashInitHandler */

	/* case1: Realtek defined class, any modification is not allowed */
	{0xEF,			0x000000FF,		FlashClass1,		0x000043FC,		NULL},	/* Winbond: MANUFACTURER_ID_WINBOND */
	{0xA1,			0x000000FF,		FlashClass1,		0x0000FFFC,		NULL}, 	/* Fudan Micro: MANUFACTURER_ID_FM */
	{0x0B,			0x000000FF,		FlashClass1,		0x000043FC,		NULL},	/* XTX */
	{0x0E,			0x000000FF,		FlashClass1,		0x000043FC,		NULL},	/* XTX(FT) */
	{0xC8,			0x000000FF,		FlashClass2,		0x000043FC,		NULL},	/* GD normal: MANUFACTURER_ID_GD */
	{0x28C2,		0x0000FFFF,		FlashClass6,		0x000200FC,		NULL},	/* MXIC wide-range VCC: MANUFACTURER_ID_MXIC */
	{0xC2,			0x000000FF,		FlashClass3,		0x000000FC,		NULL},	/* MXIC normal: MANUFACTURER_ID_BOHONG */
	{0x68,			0x000000FF,		FlashClass3,		0x000000FC,		NULL},	/* Hua Hong */
	{0x51,			0x000000FF,		FlashClass3,		0x000000FC,		NULL},	/* GD MD serial */
	{0x1C,			0x000000FF,		FlashClass4,		0x000000FC,		NULL},	/* ESMT: MANUFACTURER_ID_EON */
	{0x20,			0x000000FF,		FlashClass1,		0x000043FC,		NULL},	/* XMC: MANUFACTURER_ID_WINBOND */
	//{0x20,			0x000000FF,		FlashClass5,		0x000000FC,		NULL},	/* Micron: MANUFACTURER_ID_MICRON */

	/* case2: new flash, ID is not included in case1 list, but specification is compatible with FlashClass1~FlashClass6 */
	//{0xXX,			0x0000XXXX,		FlashClassX,		0x0000XXXX,		NULL},

	/* case3: new flash, ID is not included in case1 list, and specification is not compatible with FlashClass1~FlashClass6 */
	{0x00,			0x000000FF,		FlashClassUser,		0xFFFFFFFF,		&flash_init_userdef},

	/* End */
	{0xFF,			0xFFFFFFFF,		FlashClassNone,		0xFFFFFFFF,		NULL},
};

/*
* @brif	Flash layout is set according to Flash Layout in User Manual
*  In each entry, the first item is flash regoin type, the second item is start address, the second item is end address */
#ifndef CALC_END_ADDR
#define CALC_END_ADDR(offset, size) ((size) == 0 ? (offset) : (offset) + (size) - 1)
#endif

/* Unused regions use size 0 (offset 0xFFFFFFFF) and are skipped by the checks below */
#define FLASH_REGION_UNALIGNED(off, sz)			((sz) != 0 && ((((off) | (sz)) & 0xFFF) != 0))
#define FLASH_REGION_OVERLAP(o1, s1, o2, s2)	((s1) != 0 && (s2) != 0 && (o1) < (o2) + (s2) && (o2) < (o1) + (s1))

#define NOR_UNALIGNED(r)	FLASH_REGION_UNALIGNED(CONFIG_FLASH_##r##_OFFSET, CONFIG_FLASH_##r##_SIZE)
#define NOR_OVERLAP(a, b)	FLASH_REGION_OVERLAP(CONFIG_FLASH_##a##_OFFSET, CONFIG_FLASH_##a##_SIZE, CONFIG_FLASH_##b##_OFFSET, CONFIG_FLASH_##b##_SIZE)

#if NOR_UNALIGNED(BOOT) || NOR_UNALIGNED(VND_RESV) || NOR_UNALIGNED(OTA1) || NOR_UNALIGNED(BOOT_OTA2) || NOR_UNALIGNED(OTA2) || \
	NOR_UNALIGNED(VFS1) || NOR_UNALIGNED(VFS2) || NOR_UNALIGNED(USER) || (CONFIG_FLASH_APIMG_SIZE & 0xFFF)
#error "NOR flash layout: offsets and sizes must be 4KB aligned"
#endif
#if NOR_OVERLAP(BOOT, VND_RESV) || NOR_OVERLAP(BOOT, OTA1) || NOR_OVERLAP(BOOT, BOOT_OTA2) || NOR_OVERLAP(BOOT, OTA2) || NOR_OVERLAP(BOOT, VFS1) || \
	NOR_OVERLAP(BOOT, VFS2) || NOR_OVERLAP(BOOT, USER)
#error "NOR flash layout: BOOT overlaps another region"
#endif
#if NOR_OVERLAP(VND_RESV, OTA1) || NOR_OVERLAP(VND_RESV, BOOT_OTA2) || NOR_OVERLAP(VND_RESV, OTA2) || NOR_OVERLAP(VND_RESV, VFS1) || \
	NOR_OVERLAP(VND_RESV, VFS2) || NOR_OVERLAP(VND_RESV, USER)
#error "NOR flash layout: VND_RESV overlaps another region"
#endif
#if NOR_OVERLAP(OTA1, BOOT_OTA2) || NOR_OVERLAP(OTA1, OTA2) || NOR_OVERLAP(OTA1, VFS1) || NOR_OVERLAP(OTA1, VFS2) || NOR_OVERLAP(OTA1, USER)
#error "NOR flash layout: APP OTA1 overlaps another region"
#endif
#if NOR_OVERLAP(BOOT_OTA2, OTA2) || NOR_OVERLAP(BOOT_OTA2, VFS1) || NOR_OVERLAP(BOOT_OTA2, VFS2) || NOR_OVERLAP(BOOT_OTA2, USER)
#error "NOR flash layout: BOOT OTA2 overlaps another region"
#endif
#if NOR_OVERLAP(OTA2, VFS1) || NOR_OVERLAP(OTA2, VFS2) || NOR_OVERLAP(OTA2, USER)
#error "NOR flash layout: APP OTA2 overlaps another region"
#endif
#if NOR_OVERLAP(VFS1, VFS2) || NOR_OVERLAP(VFS1, USER) || NOR_OVERLAP(VFS2, USER)
#error "NOR flash layout: VFS/USER regions overlap"
#endif

#if CONFIG_FLASH_APIMG_SIZE == 0
#error "FLASH_APIMG_SIZE must be greater than 0"
#endif
#if CONFIG_FLASH_APIMG_SIZE >= CONFIG_FLASH_OTA1_SIZE
#error "FLASH_APIMG_SIZE must be smaller than FLASH_OTA1_SIZE"
#endif
#if CONFIG_FLASH_APIMG_SIZE >= CONFIG_FLASH_OTA2_SIZE
#error "FLASH_APIMG_SIZE must be smaller than FLASH_OTA2_SIZE"
#endif
#if CONFIG_FLASH_BOOT_OTA2_SIZE < CONFIG_FLASH_BOOT_SIZE
#error "FLASH_BOOT_OTA2_SIZE must not be smaller than FLASH_BOOT_SIZE"
#endif

#define APIMG_OTA1_OFFSET	(CONFIG_FLASH_OTA1_OFFSET + CONFIG_FLASH_OTA1_SIZE - CONFIG_FLASH_APIMG_SIZE)
#define APIMG_OTA2_OFFSET	(CONFIG_FLASH_OTA2_OFFSET + CONFIG_FLASH_OTA2_SIZE - CONFIG_FLASH_APIMG_SIZE)

FlashLayoutInfo_TypeDef Flash_Layout[] = {
	/*Region_Type,	[StartAddr,	EndAddr]		*/
	{IMG_BOOT, 		CONFIG_FLASH_BOOT_OFFSET, CALC_END_ADDR(CONFIG_FLASH_BOOT_OFFSET, CONFIG_FLASH_BOOT_SIZE)}, //Boot Manifest(4K) + AP Bootloader(248K)
	{VND_RESV,		CONFIG_FLASH_VND_RESV_OFFSET, CALC_END_ADDR(CONFIG_FLASH_VND_RESV_OFFSET, CONFIG_FLASH_VND_RESV_SIZE)}, //Vendor reserved (4K), right after IMG_BOOT
	//Users should modify below according to their own memory
	{IMG_APP_OTA1,  CONFIG_FLASH_OTA1_OFFSET, CALC_END_ADDR(CONFIG_FLASH_OTA1_OFFSET, CONFIG_FLASH_OTA1_SIZE)}, //Certificate(4K) + Manifest(4K) + AP Application OTA1 + Manifest(4K) + RDP IMG OTA1
	{IMG_APIMG_OTA1, APIMG_OTA1_OFFSET, CALC_END_ADDR(CONFIG_FLASH_OTA1_OFFSET, CONFIG_FLASH_OTA1_SIZE)}, //Manifest(4K)+ AP IMG OTA1 ( optional, overlap APP_OTA1 tail )

	{IMG_BOOT_OTA2, CONFIG_FLASH_BOOT_OTA2_OFFSET, CALC_END_ADDR(CONFIG_FLASH_BOOT_OTA2_OFFSET, CONFIG_FLASH_BOOT_OTA2_SIZE)}, //Boot Manifest(4K) + AP Bootloader(256K) OTA
	{IMG_APP_OTA2,  CONFIG_FLASH_OTA2_OFFSET, CALC_END_ADDR(CONFIG_FLASH_OTA2_OFFSET, CONFIG_FLASH_OTA2_SIZE)}, //Certificate(4K) + Manifest(4K) + AP Application OTA2 + Manifest(4K) + RDP IMG OTA2
	{IMG_APIMG_OTA2, APIMG_OTA2_OFFSET, CALC_END_ADDR(CONFIG_FLASH_OTA2_OFFSET, CONFIG_FLASH_OTA2_SIZE)}, //Manifest(4K)+ AP IMG OTA2 ( optional, overlap APP_OTA2 tail )

	{VFS1,			CONFIG_FLASH_VFS1_OFFSET, CALC_END_ADDR(CONFIG_FLASH_VFS1_OFFSET, CONFIG_FLASH_VFS1_SIZE)}, //VFS region 1: wifi/BT/NN data
	{VFS2,			CONFIG_FLASH_VFS2_OFFSET, CALC_END_ADDR(CONFIG_FLASH_VFS2_OFFSET, CONFIG_FLASH_VFS2_SIZE)},
	{USER, 			CONFIG_FLASH_USER_OFFSET, CALC_END_ADDR(CONFIG_FLASH_USER_OFFSET, CONFIG_FLASH_USER_SIZE)}, //reserve for user

	/* End */
	{0xFF, 			0xFFFFFFFF, 0xFFFFFFFF},
};

/*
* @brif	Nand Flash layout is set according to Nand Flash Layout in User Manual
*  In each entry, the first item is flash regoin type, the second item is start address, the second item is end address */
#define NAND_UNALIGNED(r)	FLASH_REGION_UNALIGNED(CONFIG_FLASH_NAND_##r##_OFFSET, CONFIG_FLASH_NAND_##r##_SIZE)
#define NAND_OVERLAP(a, b)	FLASH_REGION_OVERLAP(CONFIG_FLASH_NAND_##a##_OFFSET, CONFIG_FLASH_NAND_##a##_SIZE, CONFIG_FLASH_NAND_##b##_OFFSET, CONFIG_FLASH_NAND_##b##_SIZE)

#if NAND_UNALIGNED(BOOT) || NAND_UNALIGNED(VND_RESV) || NAND_UNALIGNED(OTA1) || NAND_UNALIGNED(BOOT_OTA2) || NAND_UNALIGNED(OTA2) || NAND_UNALIGNED(FTL) || \
	NAND_UNALIGNED(VFS1) || NAND_UNALIGNED(VFS2) || NAND_UNALIGNED(USER) || (CONFIG_FLASH_NAND_APIMG_SIZE & 0xFFF)
#error "NAND flash layout: offsets and sizes must be 4KB aligned"
#endif
#if NAND_OVERLAP(BOOT, VND_RESV) || NAND_OVERLAP(BOOT, OTA1) || NAND_OVERLAP(BOOT, BOOT_OTA2) || NAND_OVERLAP(BOOT, OTA2) || NAND_OVERLAP(BOOT, FTL) || \
	NAND_OVERLAP(BOOT, VFS1) || NAND_OVERLAP(BOOT, VFS2) || NAND_OVERLAP(BOOT, USER)
#error "NAND flash layout: BOOT overlaps another region"
#endif
#if NAND_OVERLAP(VND_RESV, OTA1) || NAND_OVERLAP(VND_RESV, BOOT_OTA2) || NAND_OVERLAP(VND_RESV, OTA2) || NAND_OVERLAP(VND_RESV, FTL) || \
	NAND_OVERLAP(VND_RESV, VFS1) || NAND_OVERLAP(VND_RESV, VFS2) || NAND_OVERLAP(VND_RESV, USER)
#error "NAND flash layout: VND_RESV overlaps another region"
#endif
#if NAND_OVERLAP(OTA1, BOOT_OTA2) || NAND_OVERLAP(OTA1, OTA2) || NAND_OVERLAP(OTA1, FTL) || NAND_OVERLAP(OTA1, VFS1) || \
	NAND_OVERLAP(OTA1, VFS2) || NAND_OVERLAP(OTA1, USER)
#error "NAND flash layout: APP OTA1 overlaps another region"
#endif
#if NAND_OVERLAP(BOOT_OTA2, OTA2) || NAND_OVERLAP(BOOT_OTA2, FTL) || NAND_OVERLAP(BOOT_OTA2, VFS1) || NAND_OVERLAP(BOOT_OTA2, VFS2) || \
	NAND_OVERLAP(BOOT_OTA2, USER)
#error "NAND flash layout: BOOT OTA2 overlaps another region"
#endif
#if NAND_OVERLAP(OTA2, FTL) || NAND_OVERLAP(OTA2, VFS1) || NAND_OVERLAP(OTA2, VFS2) || NAND_OVERLAP(OTA2, USER)
#error "NAND flash layout: APP OTA2 overlaps another region"
#endif
#if NAND_OVERLAP(FTL, VFS1) || NAND_OVERLAP(FTL, VFS2) || NAND_OVERLAP(FTL, USER) || NAND_OVERLAP(VFS1, VFS2) || \
	NAND_OVERLAP(VFS1, USER) || NAND_OVERLAP(VFS2, USER)
#error "NAND flash layout: FTL/VFS/USER regions overlap"
#endif

#if CONFIG_FLASH_NAND_APIMG_SIZE == 0
#error "FLASH_NAND_APIMG_SIZE must be greater than 0"
#endif
#if CONFIG_FLASH_NAND_APIMG_SIZE >= CONFIG_FLASH_NAND_OTA1_SIZE
#error "FLASH_NAND_APIMG_SIZE must be smaller than FLASH_NAND_OTA1_SIZE"
#endif
#if CONFIG_FLASH_NAND_APIMG_SIZE >= CONFIG_FLASH_NAND_OTA2_SIZE
#error "FLASH_NAND_APIMG_SIZE must be smaller than FLASH_NAND_OTA2_SIZE"
#endif
#if CONFIG_FLASH_NAND_BOOT_OTA2_SIZE < CONFIG_FLASH_NAND_BOOT_SIZE
#error "FLASH_NAND_BOOT_OTA2_SIZE must not be smaller than FLASH_NAND_BOOT_SIZE"
#endif

#define NAND_APIMG_OTA1_OFFSET	(CONFIG_FLASH_NAND_OTA1_OFFSET + CONFIG_FLASH_NAND_OTA1_SIZE - CONFIG_FLASH_NAND_APIMG_SIZE)
#define NAND_APIMG_OTA2_OFFSET	(CONFIG_FLASH_NAND_OTA2_OFFSET + CONFIG_FLASH_NAND_OTA2_SIZE - CONFIG_FLASH_NAND_APIMG_SIZE)

FlashLayoutInfo_TypeDef Flash_Layout_Nand[] = {
	/*Region_Type,		[StartAddr,	EndAddr]		*/
	{IMG_BOOT, 			CONFIG_FLASH_NAND_BOOT_OFFSET, CALC_END_ADDR(CONFIG_FLASH_NAND_BOOT_OFFSET, CONFIG_FLASH_NAND_BOOT_SIZE)}, //Boot Manifest(4K) + Bootloader, 3 x 128K blocks
	{VND_RESV, 			CONFIG_FLASH_NAND_VND_RESV_OFFSET, CALC_END_ADDR(CONFIG_FLASH_NAND_VND_RESV_OFFSET, CONFIG_FLASH_NAND_VND_RESV_SIZE)}, //Vendor reserved, 2 x 128K blocks, right after IMG_BOOT
	//Users should modify below according to their own memory
	{IMG_APP_OTA1, 		CONFIG_FLASH_NAND_OTA1_OFFSET, CALC_END_ADDR(CONFIG_FLASH_NAND_OTA1_OFFSET, CONFIG_FLASH_NAND_OTA1_SIZE)}, //Certificate(4K) + Manifest(4K) + KM0 & KM4 & CA32 Application OTA1 + RDP IMG OTA1
	{IMG_APIMG_OTA1,	NAND_APIMG_OTA1_OFFSET, CALC_END_ADDR(CONFIG_FLASH_NAND_OTA1_OFFSET, CONFIG_FLASH_NAND_OTA1_SIZE)}, //Manifest(4K)+ AP IMG OTA1 ( optional, overlap APP_OTA1 tail )

	{IMG_BOOT_OTA2, 	CONFIG_FLASH_NAND_BOOT_OTA2_OFFSET, CALC_END_ADDR(CONFIG_FLASH_NAND_BOOT_OTA2_OFFSET, CONFIG_FLASH_NAND_BOOT_OTA2_SIZE)}, //Boot Manifest(4K) + KM4 Bootloader(124K) OTA, 3 x 128K blocks
	{IMG_APP_OTA2, 		CONFIG_FLASH_NAND_OTA2_OFFSET, CALC_END_ADDR(CONFIG_FLASH_NAND_OTA2_OFFSET, CONFIG_FLASH_NAND_OTA2_SIZE)}, //Certificate(4K) + Manifest(4K) + KM0 & KM4 & CA32 Application OTA2 + RDP IMG OTA2
	{IMG_APIMG_OTA2,	NAND_APIMG_OTA2_OFFSET, CALC_END_ADDR(CONFIG_FLASH_NAND_OTA2_OFFSET, CONFIG_FLASH_NAND_OTA2_SIZE)}, //Manifest(4K)+ AP IMG OTA2 ( optional, overlap APP_OTA2 tail )

	{FTL,				CONFIG_FLASH_NAND_FTL_OFFSET, CALC_END_ADDR(CONFIG_FLASH_NAND_FTL_OFFSET, CONFIG_FLASH_NAND_FTL_SIZE)}, //FTL for BT, flash pages allocated to the FTL physical map (default 12K)
	{VFS1, 				CONFIG_FLASH_NAND_VFS1_OFFSET, CALC_END_ADDR(CONFIG_FLASH_NAND_VFS1_OFFSET, CONFIG_FLASH_NAND_VFS1_SIZE)}, //VFS region 1
	{VFS2, 			    CONFIG_FLASH_NAND_VFS2_OFFSET, CALC_END_ADDR(CONFIG_FLASH_NAND_VFS2_OFFSET, CONFIG_FLASH_NAND_VFS2_SIZE)}, //VFS region 2: user file system, NN MDL also included
	{USER, 				CONFIG_FLASH_NAND_USER_OFFSET, CALC_END_ADDR(CONFIG_FLASH_NAND_USER_OFFSET, CONFIG_FLASH_NAND_USER_SIZE)}, //reserve for user
	/* End */
	{0xFF, 				0xFFFFFFFF, 0xFFFFFFFF},
};


/**
 * @brief  To adopt user defined class, need to initialize the parameters in the function below
 *       with its default values according to flash Spec, and add additional config code if need.
 * @param none.
 * @retval none
 * @note	The following parameter setting is an example of MXIC flash. For users reference.
 */
void flash_init_userdef(void)
{
	FLASH_InitTypeDef *FLASH_InitStruct = &flash_init_para;

	/*1. Initialize CMD parameters */

	FLASH_InitStruct->FLASH_Id = FLASH_ID_MXIC;

	/*1.1 status bit define */
	FLASH_InitStruct->FLASH_QuadEn_bit = BIT(6);
	FLASH_InitStruct->FLASH_Busy_bit = BIT(0);
	FLASH_InitStruct->FLASH_WLE_bit = BIT(1);
	FLASH_InitStruct->FLASH_Status2_exist = 0;

	/*1.2 dummy cycle */
	FLASH_InitStruct->FLASH_rd_dummy_cycle[0] = 0;
	FLASH_InitStruct->FLASH_rd_dummy_cycle[1] = FLASH_DM_CYCLE_2O;
	FLASH_InitStruct->FLASH_rd_dummy_cycle[2] = FLASH_DM_CYCLE_2IO;
	FLASH_InitStruct->FLASH_rd_dummy_cycle[3] = FLASH_DM_CYCLE_4O;
	FLASH_InitStruct->FLASH_rd_dummy_cycle[4] = FLASH_DM_CYCLE_4IO;

	/*1.3 set 2bit mode cmd */
	FLASH_InitStruct->FLASH_rd_dual_io = 0xBB;
	FLASH_InitStruct->FLASH_rd_dual_o = 0x3B;

	/*1.4 set 4bit mode cmd */
	FLASH_InitStruct->FLASH_rd_quad_io = 0xEB;
	FLASH_InitStruct->FLASH_rd_quad_o = 0x6B;

	/*1.5 other flash commnad set */
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

	/*2. Add other configuration code here if needed for flash */
	//TODO

}

u32 flash_get_readmode(u32 ReadMode)
{
	u32 flash_bit_mode;
	switch (ReadMode) {
	case 0xFFFF:
		flash_bit_mode = Spic4IOBitMode;
		break;
	case 0x7FFF:
		flash_bit_mode = Spic4OBitMode;
		break;
	case 0x3FFF:
		flash_bit_mode = Spic2IOBitMode;
		break;
	case 0x1FFF:
		flash_bit_mode = Spic2OBitMode;
		break;
	default:
		flash_bit_mode = Spic1IOBitMode;
		break;
	}

	return flash_bit_mode;
}

FlashInfo_TypeDef *flash_get_chip_info(u32 flash_id)
{
	u32 temp, i = 0;
	FlashInfo_TypeDef *pAVL = (FlashInfo_TypeDef *)Flash_AVL;

	while (pAVL[i].flash_class != FlashClassNone) {
		temp = flash_id & pAVL[i].id_mask;
		if (pAVL[i].flash_id == temp) {
			return &pAVL[i];
		}

		i++;
	}

	return NULL;
}

FlashLayoutInfo_TypeDef *flash_get_layout_struct(void)
{
	FlashLayoutInfo_TypeDef *pLayout;

	if (SYSCFG_OTP_BootFromNor()) {
		pLayout = Flash_Layout;
	} else {
		pLayout = Flash_Layout_Nand;
	}
	return pLayout;
}


void flash_get_layout_info(u32 type, u32 *start, u32 *end)
{
	u32 i = 0;
	u32 tmp_type;
	FlashLayoutInfo_TypeDef *Layout = flash_get_layout_struct();

	while (Layout[i].region_type != 0xFF) {
		tmp_type = Layout[i].region_type;
		if (tmp_type == type) {
			if (start != NULL) {
				*start = Layout[i].start_addr;
			}
			if (end != NULL) {
				*end = Layout[i].end_addr;
			}
			return;
		}
		i++;
	}
}
