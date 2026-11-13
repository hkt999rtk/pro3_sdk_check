#include "mmu_attributes.h"
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <ameba_soc.h>

// ----------------------------------------------------------------------------
//  Type Definitions
// ----------------------------------------------------------------------------

typedef enum {
	MMU_MAP_SECTION = 0, // Use 1MB Section (Level 1)
	MMU_MAP_PAGE_4K      // Use 4KB Small Page (Level 2)
} mmu_map_type_t;

typedef struct {
	uint32_t start_addr;
	uint32_t end_addr;
	uint32_t attributes;     // Section attr OR L2 Page attr (use L2_ macros)
	mmu_map_type_t type;     // Mapping type
	uint32_t *l2_table_base; // Pointer to L2 Table array (NULL for Section)
} mmu_region_t;

// ----------------------------------------------------------------------------
//  Memory Reservation
// ----------------------------------------------------------------------------

// A single Coarse Page Table manages 1MB (256 entries * 4 bytes = 1KB).
// Must be 1KB (0x400) aligned.
#define L2_TABLE_SIZE_ENTRIES 256

// [L2 Table Pool]
// Dedicated L2 table for the 1MB block starting at 0x28000000
__attribute__((section(".mmu_l2_tbl"), aligned(0x400)))
uint32_t l2_table_sram_0x280[L2_TABLE_SIZE_ENTRIES];

// L1 Translation Table Base (Must be 16KB aligned)
#define TTB_SIZE 4096
__attribute__((section(".mmu_tbl"), aligned(0x4000)))
uint32_t MMUTable[TTB_SIZE];

// ----------------------------------------------------------------------------
//  User Configuration Area
// ----------------------------------------------------------------------------

static const mmu_region_t mmu_map[] = {
	// 1. Boot ROM / GIC (Section)
	{
		0x00000000, 0x07FFFFFF,
		MMU_SECTION_TYPE | MEM_DEVICE_SHARED | AP_PRIV_RW | EXEC_NEVER | DOMAIN(0),
		MMU_MAP_SECTION, NULL
	},

	// 2. Flash auto mode (Section)
	{
		0x08000000, 0x0FFFFFFF,
		MMU_SECTION_TYPE | MEM_NORMAL_WBWA | AP_ALL_RO | EXEC_ALLOWED | DOMAIN(0),
		MMU_MAP_SECTION, NULL
	},

	// 3. PSRAM (Section)
	{
		0x20000000, 0x203FFFFF,
		MMU_SECTION_TYPE | MEM_NORMAL_WBWA | AP_ALL_RW | EXEC_ALLOWED | SHAREABLE | DOMAIN(15),
		MMU_MAP_SECTION, NULL
	},

	// 3.1 PSRAM RET (Section)
	{
		0x20600000, 0x206FFFFF,
		MMU_SECTION_TYPE | MEM_NORMAL_WBWA | AP_ALL_RW | EXEC_ALLOWED | SHAREABLE | DOMAIN(15),
		MMU_MAP_SECTION, NULL
	},

	// 3.2 PSRAM NN RET (Section)
	{
		0x20700000, 0x207FFFFF,
		MMU_SECTION_TYPE | MEM_NORMAL_WBWA | AP_ALL_RW | EXEC_ALLOWED | SHAREABLE | DOMAIN(15),
		MMU_MAP_SECTION, NULL
	},

	// 4. SRAM Region A [Page Mode]
	// Range: 0x28000000 - 0x28001FFF (Inside 0x280xxxxx block)
	// Note: Shares l2_table_sram_0x280 with Region B
	{
		0x28000000, 0x28001FFF,
		MMU_PAGE_SMALL_TYPE | L2_MEM_NORMAL_WB | L2_AP_ALL_RW | L2_EXEC_ALLOWED | L2_NON_SHAREABLE,
		MMU_MAP_PAGE_4K, l2_table_sram_0x280
	},

	// 5. SRAM Region B [Page Mode]
	// Range: 0x2803A000 - 0x2803FFFF (Inside 0x280xxxxx block)
	// Note: Must use the SAME L2 table array because it's in the same 1MB L1 block.
	{
		0x2803A000, 0x2803FFFF,
		MMU_PAGE_SMALL_TYPE | L2_MEM_NORMAL_WB | L2_AP_ALL_RW | L2_EXEC_ALLOWED | L2_NON_SHAREABLE,
		MMU_MAP_PAGE_4K, l2_table_sram_0x280
	},

	// 7. SPIC (Section)
	{
		0x29000000, 0x290FFFFF,
		MMU_SECTION_TYPE | MEM_DEVICE_SHARED | AP_PRIV_RW | EXEC_NEVER | DOMAIN(0),
		MMU_MAP_SECTION, NULL
	},

	// 9. Peripherals (Section)
	{
		0x40000000, 0x7FFFFFFF,
		MMU_SECTION_TYPE | MEM_DEVICE_SHARED | AP_PRIV_RW | EXEC_NEVER | DOMAIN(0),
		MMU_MAP_SECTION, NULL
	},

	// 10. DRAM (Section)
	{
		0x80000000, 0xBFFFFFFF,
		MMU_SECTION_TYPE | MEM_NORMAL_WBWA | AP_ALL_RW | EXEC_ALLOWED | SHAREABLE | DOMAIN(15),
		MMU_MAP_SECTION, NULL
	}
};

// Helper: Create L1 Coarse Page Table Descriptor
#define MMU_L1_COARSE_TYPE  (0x1)
// Domain is in bits [8:5]
#define MMU_L1_COARSE_DESC(base, domain)  ((base & 0xFFFFFC00) | ((domain & 0xF) << 5) | MMU_L1_COARSE_TYPE)
// Check if an entry is already a coarse table
#define MMU_L1_IS_COARSE(desc)            ((desc & 0x3) == MMU_L1_COARSE_TYPE)

void init_mmu_table(void)
{
	uint32_t i, j;
	uint32_t num_regions = sizeof(mmu_map) / sizeof(mmu_region_t);

	// 1. Clear Global L1 Table (Fault)
	for (i = 0; i < TTB_SIZE; i++) {
		MMUTable[i] = 0;
	}

	// 2. Iterate through configuration
	for (i = 0; i < num_regions; i++) {
		uint32_t start_addr = mmu_map[i].start_addr;
		uint32_t end_addr   = mmu_map[i].end_addr;
		uint32_t attr       = mmu_map[i].attributes;
		mmu_map_type_t type = mmu_map[i].type;

		// Calculate L1 Indices (1MB granularity)
		uint32_t start_idx_l1 = start_addr >> 20;
		uint32_t end_idx_l1   = end_addr   >> 20;

		if (type == MMU_MAP_SECTION) {
			// ----------------------------------------------------
			// Handle Level 1 Section (1MB)
			// ----------------------------------------------------
			for (j = start_idx_l1; j <= end_idx_l1; j++) {
				MMUTable[j] = (j << 20) | attr;
			}
		} else if (type == MMU_MAP_PAGE_4K) {
			// ----------------------------------------------------
			// Handle Level 2 Small Page (4KB)
			// ----------------------------------------------------
			uint32_t *l2_base = mmu_map[i].l2_table_base;

			if (l2_base == NULL) {
				continue;
			}

			// Default Domain for Pages (L1 descriptor holds domain).
			// Adjust this if you need different domains for different L2 tables.
			uint32_t domain = 0;

			for (j = start_idx_l1; j <= end_idx_l1; j++) {

				uint32_t current_l1_desc = MMUTable[j];
				uint32_t l2_phys_base = (uint32_t)(uintptr_t)l2_base;
				int is_new_table = 1;

				// Step A: Check if L1 is already configured for this L2 table
				if (MMU_L1_IS_COARSE(current_l1_desc)) {
					// Extract existing base address from descriptor
					uint32_t existing_base = current_l1_desc & 0xFFFFFC00;
					if (existing_base == (l2_phys_base & 0xFFFFFC00)) {
						is_new_table = 0; // Existing table, do not overwrite L1 or clear L2
					}
				}

				// Step B: Initialize L1 and L2 only if it's a new table
				if (is_new_table) {
					// 1. Write L1 Descriptor
					MMUTable[j] = MMU_L1_COARSE_DESC(l2_phys_base, domain);

					// 2. Clear L2 Table (Initialize to Fault)
					// Note: Assuming l2_base array corresponds exactly to this 1MB block
					for (int k = 0; k < 256; k++) {
						l2_base[k] = 0;
					}
				}

				// Step C: Fill/Update L2 Table Entries
				// This logic is additive. It updates specific entries without clearing others.
				uint32_t page_addr_base = j << 20; // Base address of this 1MB block

				for (int k = 0; k < 256; k++) {
					uint32_t page_addr = page_addr_base + (k * 4096);

					if (page_addr >= start_addr && page_addr <= end_addr) {
						l2_base[k] = (page_addr & 0xFFFFF000) | attr;
					}
					// DO NOT write 'else { l2_base[k] = 0; }' here!
					// Doing so would wipe out configurations from other regions sharing this table.
				}
			}
		}
	}
}

/* =========================================================================
 *   for modify existing mmu table
 * =========================================================================
 */

// Address Masks & Attribute Clearing Masks
#define SECTION_PA_MASK         (0xFFF00000)
#define SECTION_ATTR_MASK       (~SECTION_PA_MASK)

#define PAGE_L2_BASE_MASK       (0xFFFFFC00)
#define PAGE_4K_PA_MASK         (0xFFFFF000)
#define PAGE_ATTR_MASK          (~PAGE_4K_PA_MASK)

// Hardware Operations (Synchronization Barriers)
static inline void dsb(void)
{
	__asm volatile("dsb sy" : : : "memory");
}
static inline void isb(void)
{
	__asm volatile("isb sy" : : : "memory");
}
static inline void invalidate_tlb_all(void)
{
	__asm volatile("mcr p15, 0, %0, c8, c7, 0" : : "r"(0) : "memory");
}

static inline uint32_t system_disable_interrupts(void)
{
	uint32_t cpsr;
	__asm volatile("mrs %0, cpsr \n cpsid if \n" : "=r"(cpsr) : : "memory");
	return cpsr;
}
static inline void system_restore_interrupts(uint32_t cpsr)
{
	__asm volatile("msr cpsr_c, %0" : : "r"(cpsr) : "memory");
}

void dcache_clean_invalidate_range(uint32_t start_addr, uint32_t size)
{
	DCache_CleanInvalidate(start_addr, size);
}

/* =========================================================================
 * 2. Abstract Universal Attributes & Conversion Functions
 * ========================================================================= */

/**
 * @brief Universal abstract attributes (Hardware-independent)
 * Represents typical combinations of Access Permissions (AP) and Memory Types (TEX, C, B)
 */
typedef enum {
	// Device Memory (Typically used for MMIO peripherals, Non-cacheable, Strongly Ordered/Bufferable)
	ATTR_DEVICE_PRIV_RW,         // Privileged RW, User No Access (e.g., System Registers)
	ATTR_DEVICE_FULL_RW,         // Privileged RW, User RW (e.g., User-accessible peripherals)

	// Normal Memory - Non-Cacheable (Typically used for DMA buffers)
	ATTR_NORMAL_NC_PRIV_RW,      // Privileged RW, User No Access, Non-cacheable
	ATTR_NORMAL_NC_FULL_RW,      // Privileged RW, User RW, Non-cacheable

	// Normal Memory - Write-Back Cacheable (Typically used for standard RAM, Code, Data)
	ATTR_NORMAL_WB_PRIV_RW,      // Privileged RW, User No Access, Cacheable (Kernel RAM)
	ATTR_NORMAL_WB_FULL_RW,      // Privileged RW, User RW, Cacheable (User RAM)
	ATTR_NORMAL_WB_FULL_RO,      // Privileged RO, User RO, Cacheable (Code / Read-Only Data)
} mmu_attr_t;

/**
 * @brief Converts universal attribute to L1 (1MB Section) hardware bits
 * L1 Section Bit Layout:
 * AP[2]=bit[15], TEX[2:0]=bit[14:12], AP[1:0]=bit[11:10], C=bit[3], B=bit[2]
 */
static uint32_t convert_to_l1_attr(mmu_attr_t attr)
{
	uint32_t hw_attr = 0x2; // Base Type = 0x2 (Section)

	switch (attr) {
	case ATTR_DEVICE_PRIV_RW:
		// AP: Priv RW/User NA (AP[2]=0, AP[1:0]=1) -> 0x0400
		// Mem: Shared Device (TEX=0, C=0, B=1) -> 0x0004
		hw_attr |= (0x0400 | 0x0004);
		break;
	case ATTR_DEVICE_FULL_RW:
		// AP: Full RW (AP[2]=0, AP[1:0]=3) -> 0x0C00
		// Mem: Shared Device -> 0x0004
		hw_attr |= (0x0C00 | 0x0004);
		break;
	case ATTR_NORMAL_NC_PRIV_RW:
		// AP: Priv RW/User NA -> 0x0400
		// Mem: Normal Non-Cacheable (TEX=1, C=0, B=0) -> 0x1000
		hw_attr |= (0x0400 | 0x1000);
		break;
	case ATTR_NORMAL_NC_FULL_RW:
		// AP: Full RW -> 0x0C00
		// Mem: Normal Non-Cacheable -> 0x1000
		hw_attr |= (0x0C00 | 0x1000);
		break;
	case ATTR_NORMAL_WB_PRIV_RW:
		// AP: Priv RW/User NA -> 0x0400
		// Mem: Normal WB Cacheable (TEX=1, C=1, B=1) -> 0x100C
		hw_attr |= (0x0400 | 0x100C);
		break;
	case ATTR_NORMAL_WB_FULL_RW:
		// AP: Full RW -> 0x0C00
		// Mem: Normal WB Cacheable -> 0x100C
		hw_attr |= (0x0C00 | 0x100C);
		break;
	case ATTR_NORMAL_WB_FULL_RO:
		// AP: Full RO (AP[2]=1, AP[1:0]=3) -> 0x8C00
		// Mem: Normal WB Cacheable -> 0x100C
		hw_attr |= (0x8C00 | 0x100C);
		break;
	}
	return hw_attr;
}

/**
 * @brief Converts universal attribute to L2 (4KB Small Page) hardware bits
 * L2 Small Page Bit Layout:
 * AP[2]=bit[9], TEX[2:0]=bit[8:6], AP[1:0]=bit[5:4], C=bit[3], B=bit[2]
 */
static uint32_t convert_to_l2_attr(mmu_attr_t attr)
{
	uint32_t hw_attr = 0x2; // Base Type = 0x2 (Small Page, assuming no XN bit used here)

	switch (attr) {
	case ATTR_DEVICE_PRIV_RW:
		// AP: Priv RW/User NA (AP[2]=0, AP[1:0]=1) -> 0x0010
		// Mem: Shared Device (TEX=0, C=0, B=1) -> 0x0004
		hw_attr |= (0x0010 | 0x0004);
		break;
	case ATTR_DEVICE_FULL_RW:
		// AP: Full RW (AP[2]=0, AP[1:0]=3) -> 0x0030
		// Mem: Shared Device -> 0x0004
		hw_attr |= (0x0030 | 0x0004);
		break;
	case ATTR_NORMAL_NC_PRIV_RW:
		// AP: Priv RW/User NA -> 0x0010
		// Mem: Normal Non-Cacheable (TEX=1, C=0, B=0) -> 0x0040
		hw_attr |= (0x0010 | 0x0040);
		break;
	case ATTR_NORMAL_NC_FULL_RW:
		// AP: Full RW -> 0x0030
		// Mem: Normal Non-Cacheable -> 0x0040
		hw_attr |= (0x0030 | 0x0040);
		break;
	case ATTR_NORMAL_WB_PRIV_RW:
		// AP: Priv RW/User NA -> 0x0010
		// Mem: Normal WB Cacheable (TEX=1, C=1, B=1) -> 0x004C
		hw_attr |= (0x0010 | 0x004C);
		break;
	case ATTR_NORMAL_WB_FULL_RW:
		// AP: Full RW -> 0x0030
		// Mem: Normal WB Cacheable -> 0x004C
		hw_attr |= (0x0030 | 0x004C);
		break;
	case ATTR_NORMAL_WB_FULL_RO:
		// AP: Full RO (AP[2]=1, AP[1:0]=3) -> 0x0230
		// Mem: Normal WB Cacheable -> 0x004C
		hw_attr |= (0x0230 | 0x004C);
		break;
	}
	return hw_attr;
}

/* =========================================================================
 * 3. Core API: Automatically Apply Attributes based on Target Type
 * ========================================================================= */

/**
 * @brief Dynamically modifies MMU attributes for a specific address (Unified Attribute Parameter)
 * @param target_vaddr Target virtual address
 * @param new_attr Universal abstract attribute (e.g., ATTR_NORMAL_WB_FULL_RO)
 * @return true on success, false on error (e.g., unmapped memory)
 */
bool mmu_dynamic_modify_mixed_entry(uint32_t target_vaddr, mmu_attr_t new_attr)
{
	uint32_t l1_index = target_vaddr >> 20;
	uint32_t saved_cpsr;

	// Pre-read the L1 descriptor to determine its type
	uint32_t l1_desc = MMUTable[l1_index];
	uint32_t l1_type = l1_desc & MMU_DESC_TYPE_MASK;

	if (l1_type == MMU_L1_TYPE_FAULT) {
		return false; // Not mapped
	}

	saved_cpsr = system_disable_interrupts();

	if (l1_type == MMU_L1_TYPE_SECTION) {
		// Handle 1MB Section
		uint32_t section_base = target_vaddr & SECTION_PA_MASK;
		uint32_t hw_l1_attr = convert_to_l1_attr(new_attr); // Convert to L1 format internally

		dcache_clean_invalidate_range(section_base, 1024 * 1024);

		// Retain the Physical Address and apply the newly calculated L1 attributes
		uint32_t new_desc = (l1_desc & SECTION_PA_MASK) | hw_l1_attr;

		// Note: Retain Domain bits if your system utilizes them (e.g., new_desc |= (l1_desc & 0x01E0))
		MMUTable[l1_index] = new_desc;

	} else if (l1_type == MMU_L1_TYPE_COARSE) {
		// Handle 4KB Page
		uint32_t *l2_table = (uint32_t *)(l1_desc & PAGE_L2_BASE_MASK);
		uint32_t l2_index = (target_vaddr >> 12) & 0xFF;

		uint32_t l2_desc = l2_table[l2_index];
		uint32_t page_base = target_vaddr & PAGE_4K_PA_MASK;

		if ((l2_desc & MMU_DESC_TYPE_MASK) == 0) {
			system_restore_interrupts(saved_cpsr);
			return false;
		}

		uint32_t hw_l2_attr = convert_to_l2_attr(new_attr); // Convert to L2 format internally

		dcache_clean_invalidate_range(page_base, 4096);

		// Retain the Physical Address and apply the newly calculated L2 attributes
		uint32_t new_desc = (l2_desc & PAGE_4K_PA_MASK) | hw_l2_attr;
		l2_table[l2_index] = new_desc;
	}

	// Hardware Synchronization
	dsb();
	invalidate_tlb_all();
	dsb();
	isb();

	system_restore_interrupts(saved_cpsr);
	return true;
}

/**
 * @brief Modifies MMU attributes to Read-Only for a memory range.
 *        Automatically detects whether each block is 1MB Section or 4KB Page
 *        and advances accordingly.
 * @param target_vaddr Start virtual address
 * @param size         Size in bytes of the range to modify
 * @return true if all mapped blocks were modified successfully, false if any block failed
 */
bool mmu_dynamic_modify_readonly(uint32_t target_vaddr, uint32_t size)
{
	uint32_t addr = target_vaddr;
	uint32_t end = target_vaddr + size;
	bool result = true;

	while (addr < end) {
		uint32_t l1_type = MMUTable[addr >> 20] & MMU_DESC_TYPE_MASK;
		uint32_t step;

		if (l1_type == MMU_L1_TYPE_SECTION) {
			step = 1024 * 1024; // 1MB
		} else if (l1_type == MMU_L1_TYPE_COARSE) {
			step = 4096;        // 4KB
		} else {
			// Not mapped, skip to next 1MB boundary
			addr = (addr & ~0xFFFFF) + 0x100000;
			continue;
		}

		if (!mmu_dynamic_modify_mixed_entry(addr, ATTR_NORMAL_WB_FULL_RO)) {
			result = false;
		}

		// Advance to the start of the next block
		addr = (addr & ~(step - 1)) + step;
	}

	return result;
}

/**
 * @brief Modifies MMU attributes to Non-Cacheable for a memory range.
 *        Automatically detects whether each block is 1MB Section or 4KB Page
 *        and advances accordingly.
 * @param target_vaddr Start virtual address
 * @param size         Size in bytes of the range to modify
 * @return true if all mapped blocks were modified successfully, false if any block failed
 */
bool mmu_dynamic_modify_noncache(uint32_t target_vaddr, uint32_t size)
{
	uint32_t addr = target_vaddr;
	uint32_t end = target_vaddr + size;
	bool result = true;

	while (addr < end) {
		uint32_t l1_type = MMUTable[addr >> 20] & MMU_DESC_TYPE_MASK;
		uint32_t step;

		if (l1_type == MMU_L1_TYPE_SECTION) {
			step = 1024 * 1024; // 1MB
		} else if (l1_type == MMU_L1_TYPE_COARSE) {
			step = 4096;        // 4KB
		} else {
			// Not mapped, skip to next 1MB boundary
			addr = (addr & ~0xFFFFF) + 0x100000;
			continue;
		}

		if (!mmu_dynamic_modify_mixed_entry(addr, ATTR_NORMAL_NC_FULL_RW)) {
			result = false;
		}

		// Advance to the start of the next block
		addr = (addr & ~(step - 1)) + step;
	}

	return result;
}
