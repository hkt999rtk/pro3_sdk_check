/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/** @addtogroup OTPC
  * @verbatim
  *****************************************************************************************
  * logical map space total size 2048byte ,only 1024 byte used now
  *****************************************************************************************
  *
  * 0x00~0x07	8bytes system autoload for software
  * 0x08~0x01F	24bytes  autoload for hardware
  * 0x20~0xCF	WIFI calibration data
  * 0xD0~0x11F	WIFI HCI CIS
  * 0x130~0x13F	WIFI SW/RF  Reserved
  * 0x140~0x1AF	112bytes  autoload for hardware
  * 0x1B0~0x1FF	80bytes BT calibration data
  * 0x1B0~0x1FF	80bytes BT calibration data
  * 0x200~0x20F	ADC calibration data
  * 0x210~0x21F	USB HCI
  * 0x220~0x22F	captouch calibration data
  * 0x230~0x2FF	RSVD for user use
  * 0x300~0x3FF	Realtek RSVD
  * 0x400~0x7FF	No define, SW API can't access.
  *
  *****************************************************************************************
  * physical map (1280B)
  *****************************************************************************************
  *
  * 0x000~0x1FE	For logical efuse, user can read
  * 0x200~0x3FF	Security related function
  * 0x400~0x500 	User define physical area
  *
  *****************************************************************************************
  * USER Section
  *****************************************************************************************
  * can be changed after write
  *
  *0x230~0x2FF	RSVD for user use
  *
  *****************************************************************************************
  * OTP Section (32B)
  *****************************************************************************************
  *
  * can not be changed after write
  *
  *0x400~0x500 	User define physical area
  *
  *****************************************************************************************
  * @endverbatim
  */


#ifndef _AMEBA_OTPC_H_
#define _AMEBA_OTPC_H_



/** @addtogroup Ameba_Periph_Driver
* @{
*/

/** @defgroup OTPC
* @brief OTPC driver modules
* @{
*/

/* Registers Definitions --------------------------------------------------------*/

/** @defgroup OTPC_Register_Definitions OTPC Register Definitions
 * @{
*/
/** @defgroup OTP_CTRL_0
 * @brief
 * @{
 */
#define OTPC_BIT_OTP_ENT       ((u32)0x00000001 << 31) /*!<R/W 1  OTP IP enable time (OTP SPEC Tcs >10us) For input clock 12MHz 0:24us 1:36us default value : 1 */
#define OTPC_BIT_OTP_RDT       ((u32)0x00000001 << 27) /*!<R/W 1  OTP Clock cycle time (OTP SPEC Tcyc >80ns) For input clock 12MHz 0:166.66ns 1:249.9ns default value : 1 */
#define OTPC_MASK_OTP_TPPS     ((u32)0x00000003 << 24) /*!<R/W 0x2  OTP Program Setup time and Recovery time. In the unit of cycle time. OTP SPEC 5us< Tpps <20us For input clock 12MHz 0 : 5us,  1 : 10us,  2 : 12.5us,  3 : 20us, */
#define OTPC_MASK_OTP_PGTS     ((u32)0x0000000F << 20) /*!<R/W 0011  OTP Program Setup time and Recovery time. In the unit of cycle time. OTP SPEC 5us< Tpps=Tppr <100us For input clock 12MHz 0 : 5us, 1 : 11us, 2 : 17us, 3 : 23us, 4 : 30us, 5 : 36us , 6 : 42us , 7 : 49us 8 : 55us, 9 : 61us , A : 68us , B : 74us C : 80us, D : 87us , E : 93us , F : 100us default value : 3 */
#define OTPC_BIT_OTP_BURST     ((u32)0x00000001 << 19) /*!<R/W 0  Efuse Read / Write Burst Mode Enable 0 : Disable 1 : Enable */
#define OTPC_MASK_OTP_TEST_SEL ((u32)0x00000007 << 16) /*!<R/W 0  OTP Test Mode Selection: 000: User Mode 001: Initial Margin Read Mode 010: Program Margin Read Mode 100: Auto Repair Margin Read Mode (Only in Pro2) 101: Repair Margin Read Mode 110: Repair Check Mode 111: Repair Program Mode Bit18=0:User mode and Test mode Bit18=1: Use Repair mode */
#define OTP_TEST_SEL(x)  	   (((u32)((x) & 0x00000007) << 16))
#define OTPC_BIT_OTP_TROW_EN   ((u32)0x00000001 << 15) /*!<R/W 0  OTP Test Row Select: 0: Test Row Function Disable 1: Test Row Function Enable */
#define OTPC_BIT_OTP_ERR_FLAG  ((u32)0x00000001 << 14) /*!<R 0  OTP Error Flag: When OTP read/write and test mode mapping process Error, this flag will be set to 1  */
#define OTPC_BIT_OTP_TCOL_EN   ((u32)0x00000001 << 13) /*!<R/W 0  OTP Test Column Select: 0: Test Column Function Disable 1: Test Column Function Enable */
#define OTPC_BIT_OTP_HTEMP_EN  ((u32)0x00000001 << 12) /*!<R/W 0  OTP High Temperature Test Mode Enable 0: High Temperature Test ModeDisable 1: High Temperature Test Mode Enable */
#define OTPC_BIT_OTP_DSB_EN    ((u32)0x00000001 << 11) /*!<R/W 0  0: OTP Deep Standby Mode Disable 1 : OTP Deep Standby Mode Enable  */
/** @} */

/** @defgroup OTP_CTRL_1
 * @brief
 * @{
 */
#define OTPC_BIT_OTP_PRCT_CTRL  ((u32)0x00000001 << 17) /*!<R/W 1  OTP Protect Mechanism Control This bit is used to control CRC check protect mechanism is enabled or not. 0: Disable CRC check report generation 1: Enable CRC check report generation */
#define OTPC_BIT_OTP_CNTZ_CTRL  ((u32)0x00000001 << 16) /*!<R/W 1  OTP Keys Counting Zero Bit Check Control This bit is used to control conting zero check mechanism is enabled or not. 0: Disable keys counting zero bit check mechanism 1: Enable keys counting zero bit check mechanism */
#define OTPC_BIT_OTP_RB_CTRL    ((u32)0x00000001 << 14) /*!<R/W 0  OTP Reboot (related to PTRIM) Control After Repair Program Operation 0: Disable reboot poreation after Repair Program 1: Enable reboot poreation after Repair Program */
#define OTPC_MASK_OTP_DLY2SHDN  ((u32)0x00000003 << 12) /*!<R/W 0  OTP delay time to idle or shutdown state after a requested command done. Consider about level shift between For input clock 12MHz 2'b00: 10us 2'b01: 20us 2'b10: 30us 2'b11: 50us */
#define OTPC_MASK_OTP_PGTW      ((u32)0x00000003 << 4)  /*!<R/W 1  OTP Program Pulse width Time: (OTP SPEC 10us<Tpw<15us) For Input clock 40Mhz 0 : 10us , 1 : 12.5us, 2 : 15us, 3: 15us, default value : 1 */
#define OTPC_MASK_OTP_VD        ((u32)0x00000003 << 2)  /*!<R/W 0  RSVD */
#define OTPC_MASK_OTP_PGITV     ((u32)0x00000003 << 0)  /*!<R/W 1  OTP Program Pulse Interval Time : (OTP SPEC 1us<Tpwi<5us) For Input clock 40Mhz 0 : 1us, 1 : 3us, 2 : 5us, 3 : 5us, default value : 1 */
/** @} */

/** @defgroup OTP_PMC_DBG_CTRL
 * @brief
 * @{
 */

#define OTPC_MASK_OTP_DBG_GRP_SEL ((u32)0x00000003 << 10) /*!<R/W 0   OTP debug group select */
#define OTPC_BIT_OTP_DBG_EN       ((u32)0x00000001 << 9)  /*!<R/W 0  OTP debug enable */
#define OTPC_BIT_OPT_PWR_CTRL_GNT ((u32)0x00000001 << 8)  /*!<R/W 0  The grant control for OTP power control register (OTP_PWR_CTRL) 1: Enable access to OTP power control register 0: Disable access to OTP power control register */
#define OTPC_MASK_OTP_BURN_GNT    ((u32)0x000000FF << 0)  /*!<R/W 0  The Grant Code for OTP Program Operation Need write 0x69 value before any program operation to OTP controller  */
#define OTP_BURN_GNT(x)           (((u32)((x) & 0x000000FF) << 0))
/** @} */

/** @defgroup OTP_AS_CTRL_A
 * @brief
 * @{
 */

#define OTPC_MASK_OTP_MODE_SEL       ((u32)0x00000003 << 30) /*!<R/W 0  OTP controller Mode (command) selection Program to this field could trigger OTP controller to do related command operation. 2'b00 :Read 2'b10 :Write 2'b01 :Autoload Enable 2'b11 :Compare Secure Data Enable */
#define OTP_MODE_SEL(x)              (((u32)((x) & 0x00000003) << 30))
#define OTPC_BIT_OTP_RDY             ((u32)0x00000001 << 29) /*!<R/W1C 0  OTP Operation Ready bit 0: OTP controller has NOT done a requested command. 1: OTP controller has done a requested command. Note : HW set , SW clear Note before any operations, HW/SW should clear ready bit first Note: For autoload operation, this bit is not used for autoload done status. Autoload done status of each zone is shown in 0x40009128[6:4] */
#define OTPC_BIT_OTP_COMP_RESULT     ((u32)0x00000001 << 28) /*!<R/W1C 0  compare result 0 : PASS 1 : FAIL Note : HW set , SW clear */
#define OTPC_BIT_OTP_OP_EN           ((u32)0x00000001 << 27) /*!<W1Once 0  OTP controller operation enable control This bit could be set to 1 by software and automatically cleared to 0 by hardware 0: OTP controller does NOT perforam any operations even B_OTP_MODE_SEL is given by a valid value. 1: OTP controller performs corresponding operation according to B_OTP_MODE_SEL, B_OTP_ADDR and B_OTP_DATA */
#define OTPC_MASK_OTP_ADDR           ((u32)0x000007FF << 16) /*!<R/W 0  Access Address */
#define OTP_ADDR(x)          		 (((u32)((x) & 0x000007FF) << 16))
#define OTPC_MASK_OTP_RDATA          ((u32)0x000000FF << 8)  /*!<R/W 0  Access Data [15:8] : Read Data */
#define OTP_RDATA(x)          		 (((u32)((x) & 0x000000FF) << 8))
#define GET_OTP_RDATA(x)      	     ((u32)(((x >> 8) & 0x000000FF)))
#define OTPC_MASK_OTP_WDATA          ((u32)0x000000FF << 0)  /*!<R/W 0  Access Data [7:0] : Write Data */
#define OTP_WDATA(x)          		 (((u32)((x) & 0x000000FF) << 0))
/** @} */

/** @defgroup OTP_SCAN_TEST_A
 * @brief
 * @{
 */
#define OTPC_BIT_AX_EF_CRES_SEL   ((u32)0x00000001 << 31) /*!<R/W 0  1: for debug , 0: for normal */
#define OTPC_BIT_AX_EF_RSVD       ((u32)0x00000001 << 30) /*!<R/W 0  Reserved */
#define OTPC_MASK_AX_EF_SCAN_SADR ((u32)0x000007FF << 19) /*!<R/W 0  EFuse Scan start Address (unit in byte) */
#define OTPC_MASK_AX_EF_SCAN_EADR ((u32)0x000007FF << 8)  /*!<R/W 0  EFuse Scan End address */
#define OTPC_BIT_AX_EF_SCAN_TRPT  ((u32)0x00000001 << 7)  /*!<R/W1C 0  Test Scan Report: 1 : Fail, 0 : OK */
#define OTPC_MASK_AX_EF_SCAN_FTHR ((u32)0x0000007F << 0)  /*!<R/W 0  Threshold number of bit error If [6:0]=1 and scan check have any cell bit wasn't equal 1b'1(default). It will report Fail. If [6:0]=2, it mean allow one cell bit fail while HW scan was finlish and the [6:0] will write to 0 and then check result([7])  */
/** @} */

/** @} */
/* Exported types --------------------------------------------------------*/

/** @defgroup OTPC_Exported_Types OTPC Exported Types
* @{
*/

/**
* @brief OTPC Register Declaration
*/


typedef struct {
	__IO uint32_t OTP_CTRL_0          ;  /*!< Register,  Address offset:0x000 */
	__IO uint32_t OTP_CTRL_1          ;  /*!< Register,  Address offset:0x004 */
	__IO uint32_t OTP_PMC_DBG_CTRL    ;  /*!< Register,  Address offset:0x008 */
	__IO uint32_t OTP_AS_CTRL_A       ;  /*!< Register,  Address offset:0x00C */
	__IO uint32_t OTP_SCAN_TEST_A     ;  /*!< Register,  Address offset:0x010 */

} OTPC_TypeDef;
/** @} */


/* Exported constants --------------------------------------------------------*/

/** @defgroup OTPC_Exported_Constants OTPC Exported Constants
 * @{
 **/

/** @defgroup OTPC_Logical_Definitions
  * @{
  */

#define OTP_LMAP_LEN								0x800 /*!< logical map len in byte 0x400~0x7FF resvd*/
#define OTP_MTP_USER_START							0x2D0 /*!< logical map MTP user start address*/
#define OTP_MTP_USER_END							0x2FF /*!< logical map MTP user end address*/
#define OTP_MTP_USER_LEN							(OTP_MTP_USER_END - OTP_MTP_USER_START+1) /*!< logical map MTP user section len in byte 0x2D0~0x2FF*/
/** @} */

/** @defgroup OTPC_Phsysical_Definitions
  * @{
  */

/*  physical EFUSE len */
#define OTP_REAL_CONTENT_LEN						0x800
#define AVAILABLE_EFUSE_ADDR(addr)					(addr < OTP_REAL_CONTENT_LEN)
/* physical EFUSE write forbid */
#define OTP_LOGICAL_MAP_HW_SIZE						0x280 /*!< logical mapping efuse len in physical address */
#define OTP_USER_START								0x380 /*!< physical map user start address*/
#define OTP_USER_END								0x800 /*!< physical map user end address*/
#define OTP_USER_LEN								(OTP_USER_END - OTP_USER_START+1) /*!< physical map user section len in byte 0x380~0x4FF*/
/** @} */
/** @} */

/** @} */
/** @} */

#define OTPC_PHY_ROM_PATCH_ZONE					((u32)0x00000001 << 2)
#define ROM_PATCH_LENGTH							0x200


//Please add your defination here
enum OTP_OPMode {
	OTP_USER_MODE = 0,
	OTP_PGR_MODE = 2,
};

#define OTP_POLL_TIMES								20000
#define OTP_ACCESS_PWD								0x69
/*meaningless for AmebaD2*/
#define L25EOUTVOLTAGE								7


/* OTPC_OLD_Definitions */
#define EFUSE_MAP_LEN							OTP_LMAP_LEN
#define EFUSE_MAX_SECTION_8711B						OTP_MAX_SECTION
#define EFUSE_REAL_CONTENT_LEN						OTP_REAL_CONTENT_LEN
#define EFUSE_OOB_PROTECT_BYTES						(EFUSE_REAL_CONTENT_LEN - OTP_LOGICAL_MAP_HW_SIZE)


/* OTPC_LOGICAL_Format_definitions */
#define OTP_LTYP0									0x00UL
#define OTP_LTYP1									0x01UL
#define OTP_LTYP2									0x02UL
#define OTP_LTYP3									0x03UL
#define OTP_GET_LTYP(x)								((u32)(((x >> 28) & 0x0000000F)))

#define OTP_LBASE_EFUSE								0x07UL
#define OTP_LBASE_PAD								0x06UL
#define OTP_LBASE_REG								0x0FUL
#define OTP_LBASE_ROM								0x0EUL

#define OTP_GET_LTYP1_DATA(x)						((u32)(((x >> 16) & 0x000000FF)))
#define OTP_GET_LTYP1_BASE(x)						((u32)(((x >> 12) & 0x0000000F)))
#define OTP_GET_LTYP1_OFFSET(x)						((u32)(((x >> 0) & 0x00000FFF)))

#define OTP_GET_LTYP2_LEN(x)						((u32)((((x >> 24) & 0x0000000F) + 1) << 2))
#define OTP_SET_LTYP2_LEN(x)						((u32)( ((x & 0x03F) >> 2) - 1))
#define OTP_GET_LTYP2_BASE(x)						((u32)(((x >> 12) & 0x0000000F)))
#define OTP_GET_LTYP2_OFFSET(x)						((u32)(((x >> 0) & 0x00000FFF)))

#define OTP_GET_LTYP3_OFFSET(x)						((u32)(((x >> 0) & 0x0FFFFFFF)))

#define OTP_REGPATCH_BASE							0x4100C000
#define OTP_REGPATCH_END							(OTP_REGPATCH_BASE + 0x5FF)
#define OTP_IS_REGPATCH(x)							((x >= OTP_REGPATCH_BASE) && (x < OTP_REGPATCH_END))

#define OTP_PADPATCH_BASE							0x4100C800
#define OTP_PADPATCH_END							(OTP_PADPATCH_BASE+0x1FF)
#define OTP_IS_PADPATCH(x)							((x >= OTP_PADPATCH_BASE) && (x < OTP_PADPATCH_END))

#define OTP_SUPPERPATCH_BASE						0x40000000
#define OTP_SUPPERPATCH_END							0x50000000
#define OTP_IS_SUPPERPATCH(x)						((x >= OTP_SUPPERPATCH_BASE) && (x < OTP_SUPPERPATCH_END))

#define OTP_RTKPATCH_BASE							0x500
#define OTP_RTKPATCH_END							0x6FC

#define OTP_LPGPKT_SIZE 							16


/////////////////////////////////////////////////////////////////

#define OTP_DEFAULT_SLPCEN_DLY    100
#define OTP_DEFAULT_PCEN_DLY    500
#define OTP_DEFAULT_WR_DLY      500
#define OTP_DEFAULT_RD_DLY      100
#define OTP_RW_TO_CNT           1000
#define OTP_RD_CHK_INTV         100

/**
  \brief  The data structure for an OTP port HAL operations.
*/
typedef struct otp_adapter_s {
	uint32_t        slpc_en_dly;                /* power cut enable small delay count (us) */
	uint32_t        pc_en_dly;                  /* power cut enable delay count (us) */
	uint32_t        wr_dly;                     /* write delay count (us) */
	uint32_t        rd_dly;                     /* read delay count (us) */
} otp_adapter_t, *potp_adapter_t;

/**
  \brief  The data structure of the stubs functions of the UART HAL functions in ROM.
*/

_LONG_CALL_ int OTP_LogicalRead(u8 *pbuf, u32 laddr, u32 len);
_LONG_CALL_ int OTP_LogicalWrite(u32 laddr, u32 cnts, u8 *data);
_LONG_CALL_ u32 OTP_LogicalGetRemain(void);
_LONG_CALL_ u32 OTP_GetCRC(void);
_LONG_CALL_ int OTP_SetCRC(void);
_LONG_CALL_ int OTP_Read8(u32 Addr, u8 *Data);
_LONG_CALL_ int OTP_Write8(u32 Addr, u8 Data);
_LONG_CALL_ int OTP_WritePhyBuf(u32 addr, const u8 *data, u32 len);
_LONG_CALL_ int OTP_ClearPhyBit(u32 addr, u8 bit);
_LONG_CALL_ int OTP_SetLogBit(u32 addr, u8 bit, u8 val);


#endif
