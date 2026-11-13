/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _AMEBA_SD_CARD_H
#define _AMEBA_SD_CARD_H

#include "ameba_sd_host.h"

/** @brief SD_Command_Index
  */
typedef enum {
	SD_CMD_GoIdleSte           = 0,
	EMMC_CMD_SendOpCond        = 1,   // CMD only for EMMC
	SD_CMD_AllSendCid          = 2,
	SD_CMD_SendRelAddr         = 3,
	SD_CMD_SetDsr              = 4,
	SD_CMD_SwitchFunc          = 6,
	SD_CMD_SetBusWidth         = 6,   // ACMD6
	SD_CMD_SelDeselCard        = 7,
	SD_CMD_SendIfCond          = 8,   // CMD only for SD card
	EMMC_CMD_SendExtCsd        = 8,   // CMD only for EMMC
	SD_CMD_SendCsd             = 9,
	SD_CMD_SendCid             = 10,
	SD_CMD_VolSwitch           = 11,  // CMD only for SD card
	SD_CMD_StopXsmission       = 12,
	SD_CMD_SendSts             = 13,
	SD_CMD_SetBlklen           = 16,
	SD_CMD_RdSingleBlk         = 17,
	SD_CMD_RdMulBlk            = 18,
	SD_CMD_SendTuningBlk       = 19,  // CMD only for SD card
	EMMC_CMD_SendTuningBlk     = 21,  // CMD only for eMMC HS200
	SD_CMD_SendNumWrBlks       = 22,  // ACMD22
	SD_CMD_SetBlkCnt           = 23,
	SD_CMD_SetWrBlkEraseCnt    = 23,  // ACMD23
	SD_CMD_WrBlk               = 24,
	SD_CMD_WrMulBlk            = 25,
	SD_CMD_ProgCsd             = 27,
	SD_CMD_EraseBlkSt          = 32,  // CMD only for SD card
	SD_CMD_EraseBlkEd          = 33,  // CMD only for SD card
	EMMC_CMD_EraseAddrSt       = 35,  // CMD only for EMMC
	EMMC_CMD_EraseAddrEd       = 36,  // CMD only for EMMC
	SD_CMD_Erase               = 38,
	SD_CMD_SdSendOpCond        = 41,  // ACMD41 cmd only for SD card
	SD_CMD_SendScr             = 51,  // ACMD51 cmd only for SD card
	SD_CMD_AppCmd              = 55   // CMD only for SD card
} SD_COMMAND;

/** @brief SD_Result Definition
  */
typedef enum {
	SD_OK = 0,
	SD_NODISK,
	SD_INSERT,
	SD_INITERR,
	SD_PROTECTED,
	SD_ERROR,
} SD_RESULT;

/** @brief Detected card type */
typedef enum {
	CARD_TYPE_NONE = 0,
	CARD_TYPE_SD   = 1,
	CARD_TYPE_EMMC = 2,
} CARD_TYPE;

/** @brief SD CardInfo Structure Definition
  */
typedef struct {
	u8 csd[SD_HOST_CSD_LENGTH]; 	/*!< Store the card-specific data(CSD) of the current SD card. */
	u16 rca;				        /*!< Store the relative address(RCA) of the current SD card. */
	u8 is_sdhc_sdxc;		        /*!< Specify the current card is SDSC or SDHC/SDXC. */
	u8 sd_spec_ver;			        /*!< Specify the physical layer specification version of current
							        	card, which would be a value of @ref SD_Specification_Version */
	u32 capacity;			        /*!< Specify the capacity of current card. Unit: KByte */
	u32 read_bl_len;		        /*!< Specify max. read data block length of current card. Unit: byte */
	u32 write_bl_len;		        /*!< Specify max. write data block length. Unit: byte */

	u8 sig_level;			        /*!< Specify current signal level, 0: 3.3v, 1: 1.8v */
	u8 bus_spd;				        /*!< Specify current bus speed, which would be a value of @ref SD_Access_Mode */
	SD_RESULT sd_status;	        /*!< Specify current sd status, which would be a value of @ref SD_Card_Status */
	u8 card_type;                   /*!< CARD_TYPE_NONE/SD/EMMC */

	u32 serial;                     /*!< CID product serial number (PSN), for the init summary */
	u8 speed_class;                 /*!< SD Status SPEED_CLASS (SD only): 0/1/2/3/4 = C0/C2/C4/C6/C10 */
	u8 uhs_grade;                   /*!< SD Status UHS_SPEED_GRADE (SD only): 1 = U1, 3 = U3 */
	u8 video_class;                 /*!< SD Status VIDEO_SPEED_CLASS (SD only): 6/10/30/60/90 = V6..V90 */
	u8 app_class;                   /*!< SD Status APP_PERF_CLASS (SD only): 1 = A1, 2 = A2 */

	volatile u8 wait_interrupts;
	void (*card_insert_cb)(void);
	void (*card_remove_cb)(void);
	void (*transfer_done_cb)(void);
	void (*transfer_task_yield_cb)(void);
	// u8 dma_buf[SD_HOST_BUFFER_LENGTH] __attribute__((aligned(32)));	/*!< DMA buffer, 32 byte-alignment */
} SD_CardInfo;

/** @defgroup SD_Specification_Version
  * @{
  */
#define SD_SPEC_V101					0
#define SD_SPEC_V110					1
#define SD_SPEC_V200 					2
#define SD_SPEC_V300					3
/**
  * @}
  */

/** @defgroup SD_Access_Mode
  * @{
  */
#define SD_SPEED_DS						0 // 3.3V Function 0
#define SD_SPEED_HS						1 // 3.3V Function 1
#define SD_SPEED_SDR12					2 // 1.8V Function 0
#define SD_SPEED_SDR25					3 // 1.8V Function 1
#define SD_SPEED_SDR50					4 // 1.8V Function 2
#define SD_SPEED_SDR104					5 // 1.8V Function 3
#define SD_SPEED_DDR50					6 // 1.8V Function 4
#define SD_SPEED_HS200					7 // eMMC only: 1.8V, ≤200 MHz SDR, CMD21 tuning
#define SD_KEEP_CUR_SPEED				15
/**
  * @}
  */

/** @defgroup SD_Block_Size
  * @{
  */
#define SD_BLOCK_SIZE					512    //Bytes
/**
  * @}
  */

/** @defgroup SDIO_RESP4_CMD
  * @{
  */
#define SD_APP_CMD						BIT(5)
/**
  * @}
  */

/** @defgroup SDIO_RESP0_CMD
  * @{
  */
#define SD_ADDRESS_ERROR				BIT(6)
#define SD_BLOCK_LEN_ERROR				BIT(5)
#define SD_WP_VIOLATION					BIT(2)
/**
  * @}
  */

/** @defgroup SDXC_Power_Control_ACMD41
  * @{
  */
#define SD_POWER_SAVING					0
#define SD_MAX_PERFORM					1
/**
  * @}
  */

/** @defgroup SD_Switch_18_Request_ACMD41
  * @{
  */
#define SD_USE_CUR_VOL					0
#define SD_SWITCH_18V					1
/**
  * @}
  */

/** @defgroup SD_Operation_Mode_CMD6
  * @{
  */
#define SD_CMD6_CHECK_MODE				0
#define SD_CMD6_SWITCH_MODE				1
/**
  * @}
  */

/** @defgroup SD_Capacity_Support_ACMD41
  * @{
  */
#define SD_SUPPORT_SDSC_ONLY			0
#define SD_SUPPORT_SDHC_SDXC			1
/**
  * @}
  */


#define SD_CD_SAMPLE_MS        5
#define SD_CD_STABLE_SAMPLES   5

/* Exported functions --------------------------------------------------------*/
/** @defgroup SDHOST_Exported_Functions SDHOST Exported Functions
  * @{
  */

/* Module / Power */
_LONG_CALL_ void SDCard_PowerEn(u8 enable);
_LONG_CALL_ void SDCard_ModuleInit(void);

/* Card Identification Commands */
_LONG_CALL_ u32 SDCard_ResetCard(void);
_LONG_CALL_ u32 SDCard_VoltageCheck(u8 *voltage_mismatch);
_LONG_CALL_ u32 SDCard_GetOCR(u8 voltage_mismatch);
_LONG_CALL_ u32 SDCard_GetCID(void);
_LONG_CALL_ u32 SDCard_GetRCA(void);
_LONG_CALL_ u32 SDCard_GetCSD(void);

/* Card Configuration Commands */
_LONG_CALL_ u32 SDCard_SelectDeselect(u8 select);
_LONG_CALL_ u32 SDCard_SetBusWidth(u8 bus_width);
_LONG_CALL_ u32 SDCard_GetSCR(void);
_LONG_CALL_ u32 SDCard_GetStatus(void);
_LONG_CALL_ u32 SDCard_SwitchFunction(u8 mode, u8 speed, u8 *buf_32align);
_LONG_CALL_ u32 SDCard_QuerySupportedSpeeds(u8 *pbuf);

/* Data Transfer */
_LONG_CALL_ u32 SDCard_ReadBlock(uint8_t *readbuff, uint32_t BlockIdx);
_LONG_CALL_ u32 SDCard_ReadMultiBlocks(uint8_t *readbuff, uint32_t BlockIdx, uint32_t NumberOfBlocks);
_LONG_CALL_ u32 SDCard_WriteBlock(uint8_t *writebuff, uint32_t BlockIdx);
_LONG_CALL_ u32 SDCard_WriteMultiBlocks(uint8_t *writebuff, uint32_t BlockIdx, uint32_t NumberOfBlocks);
_LONG_CALL_ u32 SDCard_ReadMultiBlocks_Start(uint8_t *readbuff, uint32_t BlockIdx, uint32_t NumberOfBlocks);
_LONG_CALL_ u32 SDCard_WriteMultiBlocks_Start(uint8_t *writebuff, uint32_t BlockIdx, uint32_t NumberOfBlocks);
_LONG_CALL_ u32 SDCard_WaitDMA(u8 cmd, u32 timeout_us);
_LONG_CALL_ u32 SDCard_Erase(uint32_t StartBlock, uint32_t EndBlock);
_LONG_CALL_ u32 SDCard_StopTransfer(void);

/* Card Detect */
_LONG_CALL_ bool SDCard_CheckCardPresent(void);

/* Response Validation */
_LONG_CALL_ u32 SDCard_CmdRespError(u8 resp_type, u8 cmd);

/* Utilities */
_LONG_CALL_ const char *SDCard_SpeedStr(u8 speed);
_LONG_CALL_ const char *SDCard_VolStr(u8 sig_level);

/* Helpers shared with eMMC protocol layer (not for general client use) */
_LONG_CALL_ u8 *SDCard_AllocAlignedBuf(u8 **raw_buf);
_LONG_CALL_ u32 SDCard_chk_xfer_int(u32 timeout_us);
_LONG_CALL_ u32 SDCard_CheckR2Response(u8 *pbuf);

/**
  * @}
  */


/**
  * @}
  */

/**
  * @}
  */

#endif
