/*
 * Copyright (c) 2026 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _AMEBA_EMMC_CARD_H
#define _AMEBA_EMMC_CARD_H

#include "ameba_sd_host.h"
#include "ameba_sd_card.h"

/* CMD1 / R3 OCR bits */
#define EMMC_OCR_BUSY            BIT(31)
#define EMMC_OCR_SECTOR_MODE     BIT(30)
#define EMMC_OCR_VDD_2V7_3V6     0x00FF8000U
#define EMMC_CMD1_ARG_SECTOR_3V3 (EMMC_OCR_SECTOR_MODE | EMMC_OCR_VDD_2V7_3V6)

#define EMMC_RCA_DEFAULT         0x0001U

/* EXT_CSD register layout (JEDEC eMMC) */
#define EMMC_EXT_CSD_LENGTH            512
#define EMMC_EXT_CSD_PARTITION_CONFIG  179
#define EMMC_EXT_CSD_BUS_WIDTH         183
#define EMMC_EXT_CSD_HS_TIMING         185
#define EMMC_EXT_CSD_REV               192
#define EMMC_EXT_CSD_CSD_STRUCTURE     194
#define EMMC_EXT_CSD_CARD_TYPE         196
#define EMMC_EXT_CSD_SEC_COUNT         212

/* DEVICE_TYPE bitmask in EXT_CSD[196] */
#define EMMC_DEV_TYPE_HS_26               BIT(0)
#define EMMC_DEV_TYPE_HS_52               BIT(1)
#define EMMC_DEV_TYPE_HS_DDR_52_3V        BIT(2)
#define EMMC_DEV_TYPE_HS_DDR_52_1V2       BIT(3)
#define EMMC_DEV_TYPE_HS200_1V8           BIT(4)
#define EMMC_DEV_TYPE_HS200_1V2           BIT(5)
#define EMMC_DEV_TYPE_HS400_1V8           BIT(6)
#define EMMC_DEV_TYPE_HS400_1V2           BIT(7)

/* Values written to EXT_CSD[183] BUS_WIDTH via CMD6 SWITCH */
#define EMMC_BW_1BIT_SDR         0x00
#define EMMC_BW_4BIT_SDR         0x01
#define EMMC_BW_8BIT_SDR         0x02
#define EMMC_BW_4BIT_DDR         0x05
#define EMMC_BW_8BIT_DDR         0x06

/* Values written to EXT_CSD[185] HS_TIMING via CMD6 SWITCH */
#define EMMC_HS_TIMING_BC        0x00
#define EMMC_HS_TIMING_HS        0x01
#define EMMC_HS_TIMING_HS200     0x02
#define EMMC_HS_TIMING_HS400     0x03

/* CMD6 SWITCH argument layout (JEDEC) */
#define EMMC_CMD6_ACCESS_WRITE_BYTE   (0x3U << 24)
#define EMMC_CMD6_INDEX_SHIFT         16
#define EMMC_CMD6_VALUE_SHIFT         8
#define EMMC_CMD6_BUILD(index, value) \
	(EMMC_CMD6_ACCESS_WRITE_BYTE | \
	 ((u32)(index) << EMMC_CMD6_INDEX_SHIFT) | \
	 ((u32)(value) << EMMC_CMD6_VALUE_SHIFT))

_LONG_CALL_ u32 EMMCCard_GetOCR_CMD1(void);
_LONG_CALL_ u32 EMMCCard_GetCID(void);
_LONG_CALL_ u32 EMMCCard_AssignRCA(void);

_LONG_CALL_ u32 EMMCCard_GetCSD(void);
_LONG_CALL_ u32 EMMCCard_GetExtCSD(void);
_LONG_CALL_ u32 EMMCCard_SetBusWidth(u8 bus_width);
_LONG_CALL_ u32 EMMCCard_SpeedNegotiate(void);

/* Read EXT_CSD into a 32-byte-aligned ≥ EMMC_EXT_CSD_LENGTH buffer with
 * no card_info side effects (for diagnostic / shell use). */
_LONG_CALL_ u32 EMMCCard_ReadExtCSD(u8 *buf);

_LONG_CALL_ u32 EMMCCard_Erase(u32 StartBlock, u32 EndBlock);

_LONG_CALL_ const char *EMMCCard_SpeedStr(u8 speed);

#endif /* _AMEBA_EMMC_CARD_H */
