/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _AMEBA_TRAFFIC_GEN_H_
#define _AMEBA_TRAFFIC_GEN_H_

/** @defgroup TRAFFIC_GEN
  * @brief TRAFFIC_GEN driver modules
  * @{
  */


/**************************************************************************//**
 * @defgroup AMEBAPRO3_TRAFFIC_GEN
 * @{
 * @brief AMEBAPRO3_TRAFFIC_GEN Register Declaration
 *****************************************************************************/

typedef struct {
	__IO uint32_t TG_PROF_RDWR;                             /*!<  Register,  Address offset: 0x0 */
	__IO uint32_t TG_PROF_STLVL;                            /*!<  Register,  Address offset: 0x4 */
	__IO uint32_t TG_PROF_FULL;                             /*!<  Register,  Address offset: 0x8 */
	__IO uint32_t TG_PROF_ISLVL;                            /*!<  Register,  Address offset: 0xC */
	__IO uint32_t TG_PROF_TXNLMT;                           /*!<  Register,  Address offset: 0x10 */
	__IO uint32_t TG_PROF_RATE_C;                           /*!<  Register,  Address offset: 0x14 */
	__IO uint32_t TG_PROF_RATE_S;                           /*!<  Register,  Address offset: 0x18 */
	__IO uint32_t TG_PROF_AXLEN;                            /*!<  Register,  Address offset: 0x1C */
	__IO uint32_t TG_PROF_FMTIME;                           /*!<  Register,  Address offset: 0x20 */
	__IO uint32_t TG_PROF_FMSIZE;                           /*!<  Register,  Address offset: 0x24 */
	__IO uint32_t TG_PROF_AWV;                              /*!<  Register,  Address offset: 0x28 */
	__IO uint32_t TG_PROF_WIV;                              /*!<  Register,  Address offset: 0x2C */
	__IO uint32_t TG_PROF_WBV;                              /*!<  Register,  Address offset: 0x30 */
	__IO uint32_t TG_PROF_BR_RBR;                           /*!<  Register,  Address offset: 0x34 */
	__IO uint32_t TG_PROF_LINK;                             /*!<  Register,  Address offset: 0x38 */
	__IO uint32_t TG_PROF_ADRM;                             /*!<  Register,  Address offset: 0x3C */
	__IO uint32_t TG_PROF_BADR;                             /*!<  Register,  Address offset: 0x40 */
	__IO uint32_t TG_PROF_YADR_ADRMSK;                      /*!<  Register,  Address offset: 0x44 */
	__IO uint32_t TG_PROF_XADRR;                            /*!<  Register,  Address offset: 0x48 */
	__IO uint32_t TG_PROF_ADRSTR;                           /*!<  Register,  Address offset: 0x4C */
	__IO uint32_t TG_PROF_IDM;                              /*!<  Register,  Address offset: 0x50 */
	__IO uint32_t TG_PROF_ID_MAXID;                         /*!<  Register,  Address offset: 0x54 */
	__IO uint32_t TG_PROF_WDM;                              /*!<  Register,  Address offset: 0x58 */
	__IO uint32_t TG_PROF_WD_MAXWD;                         /*!<  Register,  Address offset: 0x5C */
	__IO uint32_t TG_PROF_AXCACHE;                          /*!<  Register,  Address offset: 0x60 */
	__IO uint32_t TG_PROF_AXLOCK;                           /*!<  Register,  Address offset: 0x64 */
	__IO uint32_t TG_PROF_AXPROT;                           /*!<  Register,  Address offset: 0x68 */
	__IO uint32_t TG_PROF_AXUSER;                           /*!<  Register,  Address offset: 0x6C */
	__IO uint32_t TG_PROF_AXREGION;                         /*!<  Register,  Address offset: 0x70 */
	__IO uint32_t TG_PROF_AXQOS;                            /*!<  Register,  Address offset: 0x74 */
	__IO uint32_t RSVD1[2];                                 /*!<  Reserved,  Address offset: 0x78 - 0x7C*/
} TG_profile_register_t;

typedef struct {
	__IO uint32_t TG_PROF_START;                            /*!<  Register,  Address offset: 0x00 */
	__IO uint32_t TG_PROF_STOP;                             /*!<  Register,  Address offset: 0x04 */
	__IO uint32_t TG_INTR;                                  /*!<  Register,  Address offset: 0x08 */
	__IO uint32_t TG_IRQ_MSK;                               /*!<  Register,  Address offset: 0x0C */
	__IO uint32_t TG_SEED_INIT;                             /*!<  Register,  Address offset: 0x10 */
	__IO uint32_t TG_SEED_AX;                               /*!<  Register,  Address offset: 0x14 */
	__IO uint32_t TG_SEED_WD;                               /*!<  Register,  Address offset: 0x18 */
	__IO uint32_t RSVD0[57];                                /*!<  Reserved,  Address offset: 0x1C - 0xFC*/
	__IO TG_profile_register_t TG_PROF[1];
} TRAFFIC_GEN_TypeDef;

_LONG_CALL_ void TG_Init(int port, int write, u32 addr, u32 size, u32 burst_length, u32 *pattern);
_LONG_CALL_ void TG_Start(void);
_LONG_CALL_ void TG_Stop(void);
_LONG_CALL_ int  TG_WaitDone(void);
_LONG_CALL_ void TG_SetTimeMode(u32 time_cycles);
_LONG_CALL_ void TG_SetBandwidth(u32 target_bps);
_LONG_CALL_ void TG_DumpProfile(void);
_LONG_CALL_ void TG_DumpPMU(void);
_LONG_CALL_ int  TG_IsDone(void);

#endif
