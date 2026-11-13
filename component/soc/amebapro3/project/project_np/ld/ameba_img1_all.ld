#include "platform_autoconf.h"
INCLUDE "layout.ld"
INCLUDE "rom_symbol.ld"

__StackTop = __np_ssram_romstk_top__;
__StackLim = __np_ssram_romstk_lim__;

__StackTopNS = __np_ssram_stk_ns_top__;
__StackLimNS = __np_ssram_stk_ns_lim__;

SECTIONS
{
	/* DTCM reserved for RTT */
	.rtt.bss (NOLOAD) :
	{
		__rtt_cb_start__ = .;
		KEEP(*(.bss._SEGGER_RTT))
		__rtt_cb_end__ = .;
		KEEP(*SEGGER_RTT.o*(.bss.*))
	} > NP_DTCM_RTT
	
	.mailbox :
	{
		KEEP(*(.mailbox))
	} > NP_SSRAM_MAILBX

	/* image1: boot image start */
	/* image1 entry, this section should in RAM and fixed address for ROM */
	.ram_image1.entry :
	{
		__ram_image1_text_start__ = .;
		__ram_start_table_start__ = .;
		KEEP(*(SORT(.image1.entry.data*)))
		KEEP(*(.image1.validate.rodata*))
		*(.image1.export.symb)
	} > NP_SSRAM_LOADER_S

	/*  Add . to assign the start address of the section */
	/*  to prevent the change of the start address by ld doing section alignment */
	.ram_image1.text . :
	{
		/* image1 text */
		*(.boot.ram.text*)
		*(.sramdram.only.text*)
		*(.text*)
		*(.boot.rodata*)
		*(.rodata*)
		. = ALIGN (4);
	} > NP_SSRAM_LOADER_S

	.ram_image1.data . :
	{
		__ram_image1_data_start__ = .;
		KEEP(*(.boot.ram.data*))
		KEEP(*(.data*))
		. = ALIGN(16);
		__ram_image1_data_end__ = .;

		__ram_image1_text_end__ = .;
	} > NP_SSRAM_LOADER_S

	.ram_image1.bss . (NOLOAD) :
	{
		__image1_bss_start__ = .;
		KEEP(*(.boot.ram.bss*))
		KEEP(*(.bss*))
		__image1_bss_end__ = .;
	} > NP_SSRAM_LOADER_S

	/* image2: normal image start */
	.ram_image2.entry :
	{
		__ram_image2_text_start__ = .;
		__image2_entry_func__ = .;
		KEEP(*(SORT(.image2.entry.data*)))

	} > NP_SSRAM
	
#if defined(CONFIG_TRUSTZONE_EN)
	/* image3 TZ RAM: define entry symbol for image1->image3 jump */
	.ram_image3.text :
	{
		__image3_entry_func__ = .;
	} > NP_PSRAM_NSC
#endif	
}
