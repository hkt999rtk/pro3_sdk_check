/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _AMEBA_PLL_TRACKING_H_
#define _AMEBA_PLL_TRACKING_H_

/* PERIPLL tracking control register field definitions */
/** @defgroup PERIPLL_TRACKING_CTRL0
 * @{
 **/
#define PERIPLL_TRACKING_MASK_CNT1      ((u32)0x0000FFFF << 16)
#define PERIPLL_TRACKING_CNT1(x)        (((u32)((x) & 0x0000FFFF)) << 16)
#define PERIPLL_TRACKING_GET_CNT1(x)    (((u32)(x) >> 16) & 0x0000FFFF)
#define PERIPLL_TRACKING_BIT_EN         ((u32)0x00000001 << 0)

/** @defgroup PERIPLL_TRACKING_CTRL1
 * @{
 **/
#define PERIPLL_TRACKING_MASK_CNT2      ((u32)0x0000FFFF << 16)
#define PERIPLL_TRACKING_CNT2(x)        (((u32)((x) & 0x0000FFFF)) << 16)
#define PERIPLL_TRACKING_GET_CNT2(x)    (((u32)(x) >> 16) & 0x0000FFFF)
#define PERIPLL_TRACKING_MASK_CNT3      ((u32)0x0000FFFF << 0)
#define PERIPLL_TRACKING_CNT3(x)        (((u32)((x) & 0x0000FFFF)) << 0)
#define PERIPLL_TRACKING_GET_CNT3(x)    (((u32)(x) >> 0) & 0x0000FFFF)

/* DDRPLL tracking control register field definitions */
/** @defgroup DDRPLL_TRACKING_CTRL0
 * @{
 **/
#define DDRPLL_TRACKING_MASK_CNT1      ((u32)0x0000FFFF << 16)
#define DDRPLL_TRACKING_CNT1(x)        (((u32)((x) & 0x0000FFFF)) << 16)
#define DDRPLL_TRACKING_GET_CNT1(x)    (((u32)(x) >> 16) & 0x0000FFFF)
#define DDRPLL_TRACKING_BIT_EN         ((u32)0x00000001 << 0)

/** @defgroup DDRPLL_TRACKING_CTRL1
 * @{
 **/
#define DDRPLL_TRACKING_MASK_CNT2      ((u32)0x0000FFFF << 16)
#define DDRPLL_TRACKING_CNT2(x)        (((u32)((x) & 0x0000FFFF)) << 16)
#define DDRPLL_TRACKING_GET_CNT2(x)    (((u32)(x) >> 16) & 0x0000FFFF)
#define DDRPLL_TRACKING_MASK_CNT3      ((u32)0x0000FFFF << 0)
#define DDRPLL_TRACKING_CNT3(x)        (((u32)((x) & 0x0000FFFF)) << 0)
#define DDRPLL_TRACKING_GET_CNT3(x)    (((u32)(x) >> 0) & 0x0000FFFF)

/**
  * @brief  SYSTEM PLL Table Content
  */
typedef union {
	struct {
		u64 en_ssc   : 1;   /* [0]    */
		u64 tbase    : 12;  /* [12:1] */
		u64 f0f_sdm  : 13;  /* [25:13] */
		u64 f0n_sdm  : 3;   /* [28:26] */
		u64 divn_sdm : 6;   /* [34:29] */
		u64 trigger  : 1;   /* [35] */
		u64 reserved : 28;  /* [63:36] reserved */
	};
	u64 value;            /* raw 64-bit view of the entry */
} SYSPLL_Table_Content_TypeDef;

/**
  * @brief  System PLL tracking registers
  *
  */
typedef union {
	struct {
		__IO uint32_t ENTRY_0;          /*!< SYSPLL_LUT bits [31:0], offset 0x00 */
		__IO uint32_t ENTRY_1;          /*!< SYSPLL_LUT bits [63:32], offset 0x04 */
	};
	SYSPLL_Table_Content_TypeDef entry;
} SYSPLL_TRACKING_ENTRY_TypeDef;

/**
  * @brief  Complete SYS1PLL tracking register block
  *
  */
typedef struct {
	SYSPLL_TRACKING_ENTRY_TypeDef    ENTRY[42];  /*!< LUT entries (raw words or decoded) */
	__IO uint32_t                   CTRL0;      /*!< REG_SYS1PLL_TRACKING_CTRL0 */
	__IO uint32_t                   CTRL1;      /*!< REG_SYS1PLL_TRACKING_CTRL1 */
} SYSPLL_TRACKING_CONTROL;




/**
  * @brief  DDR PLL Table Content
  */
typedef union {
	struct {
		u64 en_ssc       : 1;   /* [0]    */
		u64 flag_initial : 1;   /* [1]    */
		u64 f_code_t     : 11;  /* [12:2] */
		u64 n_code_t     : 8;   /* [20:13] */
		u64 f_code       : 11;  /* [31:21] */
		u64 n_code       : 8;   /* [39:32] */
		u64 trigger      : 1;   /* [40]   */
		u64 reserved     : 23;  /* [63:41] reserved */
	};
	u64 value;            /* raw 64-bit view of the entry */
} DDRPLL_Table_Content_TypeDef;

/**
  * @brief  DDR PLL tracking registers
  *
  */
typedef union {
	struct {
		__IO uint32_t ENTRY_0;          /*!< DDRPLL_LUT bits [31:0], offset 0x00 */
		__IO uint32_t ENTRY_1;          /*!< DDRPLL_LUT bits [63:32], offset 0x04 */
	};
	DDRPLL_Table_Content_TypeDef entry;
} DDRPLL_TRACKING_ENTRY_TypeDef;

/**
  * @brief  Complete DDRPLL tracking register block
  *
  */
typedef struct {
	DDRPLL_TRACKING_ENTRY_TypeDef   ENTRY[42];  /*!< LUT entries (raw words or decoded) */
	__IO uint32_t                   CTRL0;      /*!< REG_DDRPLL_TRACKING_CTRL0 */
	__IO uint32_t                   CTRL1;      /*!< REG_DDRPLL_TRACKING_CTRL1 */
} DDRPLL_TRACKING_CONTROL;


/* SYS1 PLL */
void PLLTrack_SYS1_LoadPLLTable(u32 channel_num, u32 divn, u32 f0n, u32 f0f);
void PLLTrack_SYS1_EnablePLLTrackFeature(void);
void PLLTrack_SYS1_SetTrigger(u32 channel_num);
void PLLTrack_SYS1_UnsetTrigger(u32 channel_num);
void PLLTrack_SYS1_EnableSSC(u32 channel_num);
void PLLTrack_SYS1_DisableSSC(u32 channel_num);
void PLLTrack_SYS1_SetCNT1(u32 num);
void PLLTrack_SYS1_SetCNT2(u32 num);
void PLLTrack_SYS1_SetCNT3(u32 num);
void PLLTrack_SYS1_SetTBASE(u32 channel_num, u32 tbase);

/* SYS2 PLL */
void PLLTrack_SYS2_LoadPLLTable(u32 channel_num, u32 divn, u32 f0n, u32 f0f);
void PLLTrack_SYS2_EnablePLLTrackFeature(void);
void PLLTrack_SYS2_SetTrigger(u32 channel_num);
void PLLTrack_SYS2_UnsetTrigger(u32 channel_num);
void PLLTrack_SYS2_EnableSSC(u32 channel_num);
void PLLTrack_SYS2_DisableSSC(u32 channel_num);
void PLLTrack_SYS2_SetCNT1(u32 num);
void PLLTrack_SYS2_SetCNT2(u32 num);
void PLLTrack_SYS2_SetCNT3(u32 num);
void PLLTrack_SYS2_SetTBASE(u32 channel_num, u32 tbase);

/* PERIPLL */
int  PLLTrack_PERI_ChannelToIndex(u32 channel_num);
void PLLTrack_PERI_LoadPLLTable(u32 channel_num, u32 divn, u32 f0n, u32 f0f);
void PLLTrack_PERI_EnablePLLTrackFeature(void);
void PLLTrack_PERI_SetTrigger(u32 channel_num);
void PLLTrack_PERI_UnsetTrigger(u32 channel_num);
void PLLTrack_PERI_EnableSSC(u32 channel_num);
void PLLTrack_PERI_DisableSSC(u32 channel_num);
void PLLTrack_PERI_SetCNT1(u32 num);
void PLLTrack_PERI_SetCNT2(u32 num);
void PLLTrack_PERI_SetCNT3(u32 num);
void PLLTrack_PERI_SetTBASE(u32 channel_num, u32 tbase);

/* DDR PLL */
void PLLTrack_DDR_LoadPLLTable(u32 channel_num, u32 n_code, u32 f_code, u32 n_code_t, u32 f_code_t);
void PLLTrack_DDR_EnablePLLTrackFeature(void);
void PLLTrack_DDR_DisablePLLTrackFeature(void);
void PLLTrack_DDR_LoadFromSystemRegs(void);
void PLLTrack_DDR_LoadFromCRT(void);
void PLLTrack_DDR_SetTrigger(u32 channel_num);
void PLLTrack_DDR_UnsetTrigger(u32 channel_num);
void PLLTrack_DDR_EnableSSC(u32 channel_num);
void PLLTrack_DDR_DisableSSC(u32 channel_num);
void PLLTrack_DDR_SetFlagInitial(u32 channel_num);
void PLLTrack_DDR_UnsetFlagInitial(u32 channel_num);
void PLLTrack_DDR_SetOC_EN(void);
void PLLTrack_DDR_UnsetOC_EN(void);
void PLLTrack_DDR_SetCNT1(u32 num);
void PLLTrack_DDR_SetCNT2(u32 num);
void PLLTrack_DDR_SetCNT3(u32 num);

#endif
