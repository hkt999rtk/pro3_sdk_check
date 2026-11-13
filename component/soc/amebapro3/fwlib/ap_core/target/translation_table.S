/*
 * Copyright (c) 2020 Realtek Semiconductor Corp.	All rights reserved.
 *
 * Author: PSP Software Group
 */

/*****************************************************************************/
/**
* @file translation_table.s
*
* @addtogroup a32_boot_code
* @{
* <h2> translation_table.S </h2>
* translation_table.S contains a static page table required by MMU for
* cortex-A7. This translation table is flat mapped (input address = output
* address) with default memory attributes defined for sheipa platform. It
* utilizes short descriptor translation table format with each section defining
* 1MB of memory.
*
* The overview of translation table memory attributes is described below.
*
*|                       | Memory Range            | Definition in Translation Table   |
*|-----------------------|-------------------------|-----------------------------------|
*| Boot ROM / GIC        | 0x00000000 - 0x0FFFFFFF | Device Memory                     | 0x100
*| Reserved              | 0x10000000 - 0x1FFFFFFF | Unassigned                        | 0x100
*| PSRAM                 | 0x20000000 - 0x203FFFFF | Normal write-back Cacheable       | 0x4
*| Reserved              | 0x20400000 - 0x27FFFFFF | Normal write-back Cacheable       | 0x7C
*| SRAM                  | 0x28000000 - 0x280FFFFF | Normal write-back Cacheable       | 0x1
*| Reserved              | 0x28100000 - 0x28FFFFFF | Normal write-back Cacheable       | 0xF
*| SPIC                  | 0x29000000 - 0x290FFFFF | Normal write-back Cacheable       | 0x1
*| Reserved              | 0x29100000 - 0x3FFFFFFF | Unassigned                        | 0x16F
*| Peripheral subsystem  | 0x40000000 - 0x7FFFFFFF | Device Memory                     | 0x400
*| DRAM                  | 0x80000000 - 0xFFFFFFFF | Normal write-back Cacheable       | 0x800
*
******************************************************************************/
	.globl  MMUTable

	.section .mmu_tbl,"a"

MMUTable:
	/* Each table entry occupies one 32-bit word and there are
	 * 4096 entries, so the entire table takes up 16KB.
	 * Each entry covers a 1MB section.
	 */
.set SECT, 0

.rept	0x0100				/* 0x00000000 - 0x0fffffff (Boot ROM) */
.word	SECT + 0xc06		/* S=b0 TEX=b000 AP=b11, Domain=b0, C=b0, B=b1 */
.set	SECT, SECT+0x100000
.endr

.rept	0x0100				/* 0x10000000 - 0x1fffffff (unassigned/reserved) */
.word	SECT + 0x0			/* S=b0 TEX=b000 AP=b00, Domain=b0, C=b0, B=b0 */
.set	SECT, SECT+0x100000
.endr

.rept	0x0004				/* 0x20000000 - 0x203FFFFF (PSRAM) */
.word	SECT + 0x15de6		/* S=b1 TEX=b101 AP=b11, Domain=b1111, C=b0, B=b1 */
							/* Shareable, inner and outer write back, write allocate */
.set	SECT, SECT+0x100000
.endr

.rept	0x007C				/* 0x20400000 - 0x27FFFFFF (unassigned/reserved) */
.word	SECT + 0x0			/* S=b0 TEX=b000 AP=b00, Domain=b0, C=b0, B=b0 */
.set	SECT, SECT+0x100000
.endr

				 			/* 0x28000000 - 0x280FFFFF (SRAM) */
.word	SECT + 0xc0e		/* S=b0 TEX=b000 AP=b11, Domain=b0, C=b1, B=b1 */
							/* Non Shareable, Outer and Inner write-back, no allocate on write */
.set	SECT, SECT+0x100000

.rept	0xF					/* 0x28100000 - 0x28FFFFFF (unassigned/reserved) */
.word	SECT + 0x0			/* S=b0 TEX=b000 AP=b00, Domain=b0, C=b0, B=b0 */
.set	SECT, SECT+0x100000
.endr

							/* 0x29000000 - 0x290FFFFF (Device Memory) */
.word	SECT + 0xc06		/* S=b0 TEX=b000 AP=b11, Domain=b0, C=b0, B=b1 */
.set	SECT, SECT+0x100000


.rept	0x016F				/* 0x28100000 - 0x3FFFFFFF (unassigned/reserved) */
.word	SECT + 0x0			/* S=b0 TEX=b000 AP=b00, Domain=b0, C=b0, B=b0 */
.set	SECT, SECT+0x100000
.endr


.rept	0x0400				/* 0x40000000 - 0x7FFFFFFF (Device Memory) */
.word	SECT + 0xc06		/* S=b0 TEX=b000 AP=b11, Domain=b0, C=b0, B=b1 */
.set	SECT, SECT+0x100000
.endr

.rept	0x0800				/* 0x80000000 - 0xffffffff (DRAM Cacheable) */
.word	SECT + 0x15de6		/* S=b1 TEX=b101 AP=b11, Domain=b1111, C=b0, B=b1 */
							/* Shareable, inner and outer write back, write allocate */
.set	SECT, SECT+0x100000
.endr

.end
/**
* @} End of "addtogroup a32_boot_code".
*/
