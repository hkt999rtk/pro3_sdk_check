INCLUDE "layout.ld"
INCLUDE "rom_symbol.ld"

__StackTop = __np_ssram_romstk_top__;
__StackLim = __np_ssram_romstk_lim__;

SECTIONS
{
	.ram_image2.entry :
	{
		__ram_image2_text_start__ = .;
		__image2_entry_func__ = .;
		KEEP(*(SORT(.image2.entry.data*)))

	} > NP_PSRAM
	
	.psram.text :
	{
		. = ALIGN (4);
		*(*.text*)
	} > NP_PSRAM

	.psram.rodata :
	{
		. = ALIGN (4);
		*(*.rodata*)
	} > NP_PSRAM

	.psram.data :
	{
		. = ALIGN (4);
		__cmd_table_start__ = .;
		KEEP(*(.cmd.table.data))
		__cmd_table_end__ = .;			
		*(*.data*)
	} > NP_PSRAM
	
	.psram.bss :
	{
		. = ALIGN (4);
		__bss_start__ = .;
		*(*.bss*)
		__bss_end__ = .;
	} > NP_PSRAM
	
}
