ENTRY(Reset_Handler)

INCLUDE "rom_layout.ld"

SECTIONS
{
	.rom.text : { } > NP_ITCM_ROM
	.rom.hidden.text : { } > NP_ITCM_ROM
 	.rom.rodata : { } > NP_ITCM_ROM
	.hal.rom.bss : { } > NP_SSRAM_ROMBSS
	.hal.s_rom.bss : { } > NP_SSRAM_ROMBSS_S

	/* image1 entry, this section should in RAM and fixed address for ROM */
	.ram_image1.entry :
	{
		__ram_start_table_start__ = .;
		KEEP(*(SORT(.image1.entry.data*)))
		KEEP(*(.image1.validate.rodata*))
	} > NP_SSRAM

	/* NS func entry, this section should in NS_ROM and fixed address for S_ROM */
	.ns_rom.rodata :
	{
		__rom_entry_ns_start__ = ABSOLUTE(.);
	} > NP_ITCM_ROM

	/* data & bss should have not any content */

	.ram.data :
	{
		KEEP(*(.data*))
	} > NP_SSRAM
	.ram.bss :
	{
		KEEP(*(.bss*))
	} > NP_SSRAM

	.rom.text.libgcc :
	{
		KEEP(*(.text*))
	} > NP_ITCM_ROM
}
