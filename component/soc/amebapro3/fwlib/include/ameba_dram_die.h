/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _AMEBA_DRAM_DIE_H
#define _AMEBA_DRAM_DIE_H

/* =========================================================================
 * JEDEC DDR3 Mode Register field encoding (JESD79-3F)
 *
 * Use these to compose DRAM_DIE_OPS_TypeDef.mr[0..3] entries; each die
 * then reads as a list of JEDEC fields instead of an opaque hex value.
 * ========================================================================= */

/* ---- MR0 (Burst Length / CAS Latency / Write Recovery / Precharge PD) ---- */

/* Burst Length (A[1:0]) */
#define JEDEC_MR0_BL_8              ((u32)0x0 << 0)   /*!< Fixed BL8 */
#define JEDEC_MR0_BL_OTF            ((u32)0x1 << 0)   /*!< BC4 or BL8 on-the-fly via A12 */
#define JEDEC_MR0_BL_BC4            ((u32)0x2 << 0)   /*!< Fixed BC4 */

/* Read Burst Type (A[3]) */
#define JEDEC_MR0_RBT_SEQUENTIAL    ((u32)0x0 << 3)
#define JEDEC_MR0_RBT_INTERLEAVE    ((u32)0x1 << 3)

/* CAS Latency — encoded across A[6:4] and A[2]. Per JESD79-3F Table 6. */
#define JEDEC_MR0_CL_5              ((u32)((0x1 << 4) | (0x0 << 2)))  /*!< CL=5 */
#define JEDEC_MR0_CL_6              ((u32)((0x2 << 4) | (0x0 << 2)))  /*!< CL=6 */
#define JEDEC_MR0_CL_7              ((u32)((0x3 << 4) | (0x0 << 2)))  /*!< CL=7 */
#define JEDEC_MR0_CL_8              ((u32)((0x4 << 4) | (0x0 << 2)))  /*!< CL=8 */
#define JEDEC_MR0_CL_9              ((u32)((0x5 << 4) | (0x0 << 2)))  /*!< CL=9 */
#define JEDEC_MR0_CL_10             ((u32)((0x6 << 4) | (0x0 << 2)))  /*!< CL=10 */
#define JEDEC_MR0_CL_11             ((u32)((0x7 << 4) | (0x0 << 2)))  /*!< CL=11 */
#define JEDEC_MR0_CL_12             ((u32)((0x0 << 4) | (0x1 << 2)))  /*!< CL=12 */
#define JEDEC_MR0_CL_13             ((u32)((0x1 << 4) | (0x1 << 2)))  /*!< CL=13 */
#define JEDEC_MR0_CL_14             ((u32)((0x2 << 4) | (0x1 << 2)))  /*!< CL=14 */

/* Test Mode (A[7]) */
#define JEDEC_MR0_TM_NORMAL         ((u32)0x0 << 7)

/* DLL Reset (A[8]) — write 1 for a one-shot DLL reset, self-clears */
#define JEDEC_MR0_DLL_RESET         ((u32)0x1 << 8)

/* Write Recovery for auto-precharge (A[11:9]) */
#define JEDEC_MR0_WR_16             ((u32)0x0 << 9)
#define JEDEC_MR0_WR_5              ((u32)0x1 << 9)
#define JEDEC_MR0_WR_6              ((u32)0x2 << 9)
#define JEDEC_MR0_WR_7              ((u32)0x3 << 9)
#define JEDEC_MR0_WR_8              ((u32)0x4 << 9)
#define JEDEC_MR0_WR_10             ((u32)0x5 << 9)
#define JEDEC_MR0_WR_12             ((u32)0x6 << 9)
#define JEDEC_MR0_WR_14             ((u32)0x7 << 9)

/* Precharge Power-Down (A[12]) */
#define JEDEC_MR0_PPD_SLOW          ((u32)0x0 << 12)  /*!< DLL off during precharge PD */
#define JEDEC_MR0_PPD_FAST          ((u32)0x1 << 12)  /*!< DLL on during precharge PD  */

/* ---- MR1 (DLL / DIC / RTT_NOM / AL / WL / TDQS / Qoff) ---- */

/* DLL (A[0]) — note JEDEC polarity: 0 = enabled, 1 = disabled */
#define JEDEC_MR1_DLL_ENABLED       ((u32)0x0 << 0)
#define JEDEC_MR1_DLL_DISABLED      ((u32)0x1 << 0)

/* Output Driver Impedance Control (A[5,1]) */
#define JEDEC_MR1_DIC_RZQ_6         ((u32)((0x0 << 5) | (0x0 << 1)))  /*!< 40Ω */
#define JEDEC_MR1_DIC_RZQ_7         ((u32)((0x0 << 5) | (0x1 << 1)))  /*!< 34Ω */

/* On-Die Termination, Nominal (A[9,6,2]) */
#define JEDEC_MR1_RTT_NOM_DISABLED  ((u32)((0x0 << 9) | (0x0 << 6) | (0x0 << 2)))
#define JEDEC_MR1_RTT_NOM_RZQ_4     ((u32)((0x0 << 9) | (0x0 << 6) | (0x1 << 2)))  /*!< 60Ω */
#define JEDEC_MR1_RTT_NOM_RZQ_2     ((u32)((0x0 << 9) | (0x1 << 6) | (0x0 << 2)))  /*!< 120Ω */
#define JEDEC_MR1_RTT_NOM_RZQ_6     ((u32)((0x0 << 9) | (0x1 << 6) | (0x1 << 2)))  /*!< 40Ω */
#define JEDEC_MR1_RTT_NOM_RZQ_12    ((u32)((0x1 << 9) | (0x0 << 6) | (0x0 << 2)))  /*!< 20Ω */
#define JEDEC_MR1_RTT_NOM_RZQ_8     ((u32)((0x1 << 9) | (0x0 << 6) | (0x1 << 2)))  /*!< 30Ω */

/* Additive Latency (A[4:3]) */
#define JEDEC_MR1_AL_0              ((u32)0x0 << 3)
#define JEDEC_MR1_AL_CL_MINUS_1     ((u32)0x1 << 3)
#define JEDEC_MR1_AL_CL_MINUS_2     ((u32)0x2 << 3)

/* Write Leveling (A[7]) */
#define JEDEC_MR1_WL_DISABLED       ((u32)0x0 << 7)
#define JEDEC_MR1_WL_ENABLED        ((u32)0x1 << 7)

/* TDQS (A[11]) — x8 DRAM only */
#define JEDEC_MR1_TDQS_DISABLED     ((u32)0x0 << 11)
#define JEDEC_MR1_TDQS_ENABLED      ((u32)0x1 << 11)

/* Output enable (A[12]) — note polarity: 0 = enabled */
#define JEDEC_MR1_QOFF_ENABLED      ((u32)0x0 << 12)
#define JEDEC_MR1_QOFF_DISABLED     ((u32)0x1 << 12)

/* ---- MR2 (PASR / CWL / ASR / SRT / RTT_WR) ---- */

/* Partial Array Self Refresh (A[2:0]) - only the listed banks keep
 * refreshing in self-refresh; everything else decays */
#define JEDEC_MR2_PASR_FULL         ((u32)0x0 << 0)  /*!< banks 0-7 */
#define JEDEC_MR2_PASR_HALF         ((u32)0x1 << 0)  /*!< banks 0-3 */
#define JEDEC_MR2_PASR_QUARTER      ((u32)0x2 << 0)  /*!< banks 0-1 */
#define JEDEC_MR2_PASR_EIGHTH       ((u32)0x3 << 0)  /*!< bank 0    */
#define JEDEC_MR2_PASR_3QUARTER     ((u32)0x4 << 0)  /*!< banks 2-7 */
#define JEDEC_MR2_PASR_HALF_HI      ((u32)0x5 << 0)  /*!< banks 4-7 */
#define JEDEC_MR2_PASR_QUARTER_HI   ((u32)0x6 << 0)  /*!< banks 6-7 */
#define JEDEC_MR2_PASR_EIGHTH_HI    ((u32)0x7 << 0)  /*!< bank 7    */
#define JEDEC_MR2_PASR_MASK         ((u32)0x7 << 0)

/* CAS Write Latency (A[5:3]) */
#define JEDEC_MR2_CWL_5             ((u32)0x0 << 3)
#define JEDEC_MR2_CWL_6             ((u32)0x1 << 3)
#define JEDEC_MR2_CWL_7             ((u32)0x2 << 3)
#define JEDEC_MR2_CWL_8             ((u32)0x3 << 3)
#define JEDEC_MR2_CWL_9             ((u32)0x4 << 3)
#define JEDEC_MR2_CWL_10            ((u32)0x5 << 3)
#define JEDEC_MR2_CWL_11            ((u32)0x6 << 3)
#define JEDEC_MR2_CWL_12            ((u32)0x7 << 3)

/* Auto Self Refresh (A[6]) */
#define JEDEC_MR2_ASR_MANUAL        ((u32)0x0 << 6)
#define JEDEC_MR2_ASR_AUTO          ((u32)0x1 << 6)

/* Self-Refresh Temperature range (A[7]) */
#define JEDEC_MR2_SRT_NORMAL        ((u32)0x0 << 7)
#define JEDEC_MR2_SRT_EXTENDED      ((u32)0x1 << 7)

/* Dynamic ODT / Write termination (A[10:9]) */
#define JEDEC_MR2_RTT_WR_DISABLED   ((u32)0x0 << 9)
#define JEDEC_MR2_RTT_WR_RZQ_4      ((u32)0x1 << 9)   /*!< 60Ω */
#define JEDEC_MR2_RTT_WR_RZQ_2      ((u32)0x2 << 9)   /*!< 120Ω */

/* ---- MR3 (MPR) ---- */

/* Multi-Purpose Register location (A[1:0]) */
#define JEDEC_MR3_MPR_LOC_PREDEF    ((u32)0x0 << 0)

/* MPR operation (A[2]) */
#define JEDEC_MR3_MPR_NORMAL        ((u32)0x0 << 2)
#define JEDEC_MR3_MPR_ENABLED       ((u32)0x1 << 2)

/**
  * @brief  DRAM die operations and descriptor.
  *         Packages the per-part JEDEC Mode Register values together with
  *         optional function pointers so alternative DRAM dies (different
  *         speed bins, a new part, eventually a different DDR generation)
  *         can be dropped in by supplying a new instance of this struct.
  *
  *         DDR3 dies only need to populate @c mr[0..3] (MR0..MR3). Dies
  *         that expose additional JEDEC MRs (MR4..MR6 on DDR4/LPDDR4) can
  *         be added with an extended instance and an EmitLMR override.
  */
typedef struct DRAM_DIE_OPS_s DRAM_DIE_OPS_TypeDef;

struct DRAM_DIE_OPS_s {
	/* Descriptor */
	const char *name;           /*!< Human name, e.g. "KVR16S11/8" */
	const char *jedec_spec;     /*!< JEDEC classification, e.g. "DDR3-1866" */
	u32         mr[4];          /*!< MR0..MR3 values (DDR3 JEDEC) */

	/* Operations — may be NULL to use the default DDR3 behavior. */
	void (*DumpInfo)(const DRAM_DIE_OPS_TypeDef *self);
};

extern const DRAM_DIE_OPS_TypeDef DRAM_DIE_KVR16S11_8;    /*!< Kingston KVR16S11/8 DDR3L-100 */
extern const DRAM_DIE_OPS_TypeDef DRAM_DIE_M15T1G1664A;   /*!< ESMT DDR3L-1866 part      */

_LONG_CALL_ void DRAM_DIE_DumpInfo(const DRAM_DIE_OPS_TypeDef *die);
_LONG_CALL_ void DRAM_Die_Power(u32 NewState);

#endif /* _AMEBA_DRAM_DIE_H */
