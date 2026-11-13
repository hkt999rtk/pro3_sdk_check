#ifndef MMU_ATTRIBUTES_H
#define MMU_ATTRIBUTES_H

#include <stdint.h>

/* ============================================================================
 *  LEVEL 1: TABLE DESCRIPTOR TYPES
 * ============================================================================ */
#define MMU_DESC_TYPE_MASK      (0x03)
// [1:0] Descriptor Type
#define MMU_L1_TYPE_FAULT       (0x00)
#define MMU_L1_TYPE_COARSE      (0x01) // Descriptor pointing to an L2 Table
#define MMU_L1_TYPE_SECTION     (0x02) // Directly maps 1MB

/* ============================================================================
 *  PART A: LEVEL 1 SECTION ATTRIBUTES (1MB MAPPING)
 *  Use these when mmu_region_t.type == MMU_MAP_SECTION
 * ============================================================================ */
/*
 * Section Entry Structure (Short Descriptor):
 * [17] nG, [16] S, [15] AP[2], [14:12] TEX, [11:10] AP[1:0], [8:5] Domain,
 * [4] XN, [3] C, [2] B, [1:0] 10
 */

// Basic Type (Mandatory for Section)
#define MMU_SECTION_TYPE        (MMU_L1_TYPE_SECTION)

// 1. Memory Type (Section)
#define MEM_STRONGLY_ORDERED    (0x0 << 2)              // TEX=000, C=0, B=0
#define MEM_DEVICE_SHARED       (0x1 << 2)              // TEX=000, C=0, B=1
#define MEM_NORMAL_WT           (0x2 << 2)              // TEX=000, C=1, B=0
#define MEM_NORMAL_WB           (0x3 << 2)              // TEX=000, C=1, B=1 (Inner/Outer WB, no alloc)
#define MEM_NORMAL_NC           (0x1 << 12)             // TEX=001, C=0, B=0
#define MEM_NORMAL_WBWA         (0x5 << 12 | 0x3 << 2)  // TEX=101, C=0, B=1 (Typical WBWA)

// 2. Access Permissions (Section) -> AP[2] at bit 15, AP[1:0] at 11:10
#define AP_NO_ACCESS            (0x0 << 10)
#define AP_PRIV_RW              (0x1 << 10)
#define AP_PRIV_RW_USER_RO      (0x2 << 10)
#define AP_ALL_RW               (0x3 << 10)
#define AP_PRIV_RO              (0x1 << 15 | 0x1 << 10)
#define AP_ALL_RO               (0x1 << 15 | 0x2 << 10)

// 3. Execution (Section) -> XN at bit 4
#define EXEC_ALLOWED            (0x0)
#define EXEC_NEVER              (0x1 << 4)

// 4. Shareability (Section) -> S at bit 16
#define NON_SHAREABLE           (0x0)
#define SHAREABLE               (0x1 << 16)

// 5. Domain (Section only) -> Domain at bit 8:5
#define DOMAIN(x)               ((x & 0xF) << 5)


/* ============================================================================
 *  PART B: LEVEL 2 SMALL PAGE ATTRIBUTES (4KB MAPPING)
 *  Use these when mmu_region_t.type == MMU_MAP_PAGE_4K
 *  [WARNING]: Bit positions differ from Sections! Do not mix!
 * ============================================================================ */
/*
 * Small Page Entry Structure (Short Descriptor):
 * [31:12] Base Addr
 * [11]    nG (Global)
 * [10]    S  (Shareable)  <-- Different pos
 * [9]     AP[2]           <-- Different pos
 * [8:6]   TEX             <-- Different pos
 * [5:4]   AP[1:0]         <-- Different pos
 * [3]     C
 * [2]     B
 * [1]     1 (Small Page)
 * [0]     XN              <-- Different pos
 */

// Basic Type (Mandatory for Small Page)
// Bit[1]=1. Note: Bit[0] is XN, so base type is just 0x2.
#define MMU_PAGE_SMALL_TYPE     (0x2)

// 1. Memory Type (L2 Page) -> TEX at [8:6], C at [3], B at [2]
#define L2_MEM_STRONGLY_ORDERED (0x0 << 6 | 0x0 << 2) // TEX=000, C=0, B=0
#define L2_MEM_DEVICE_SHARED    (0x0 << 6 | 0x1 << 2) // TEX=000, C=0, B=1
#define L2_MEM_NORMAL_WT        (0x0 << 6 | 0x2 << 2) // TEX=000, C=1, B=0
#define L2_MEM_NORMAL_WB        (0x0 << 6 | 0x3 << 2) // TEX=000, C=1, B=1
#define L2_MEM_NORMAL_NC        (0x1 << 6 | 0x0 << 2) // TEX=001, C=0, B=0
#define L2_MEM_NORMAL_WBWA      (0x5 << 6 | 0x1 << 2) // TEX=101, C=0, B=1

// 2. Access Permissions (L2 Page) -> AP[2] at bit 9, AP[1:0] at 5:4
#define L2_AP_NO_ACCESS         (0x0 << 4)
#define L2_AP_PRIV_RW           (0x1 << 4)
#define L2_AP_PRIV_RW_USER_RO   (0x2 << 4)
#define L2_AP_ALL_RW            (0x3 << 4)
#define L2_AP_PRIV_RO           (0x1 << 9 | 0x1 << 4)
#define L2_AP_ALL_RO            (0x1 << 9 | 0x2 << 4)

// 3. Execution (L2 Page) -> XN at bit 0
#define L2_EXEC_ALLOWED         (0x0)
#define L2_EXEC_NEVER           (0x1 << 0)

// 4. Shareability (L2 Page) -> S at bit 10
#define L2_NON_SHAREABLE        (0x0)
#define L2_SHAREABLE            (0x1 << 10)

// Note: Domain is NOT configured in L2 descriptors.
// It is configured in the L1 Coarse Descriptor pointing to this table.

#endif // MMU_ATTRIBUTES_H
