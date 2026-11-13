ENTRY(Reset_Handler)

INCLUDE "layout.ld"

SECTIONS
{
	/* -------- entry + NSC gateway + bulk text (region per CONFIG_IMG3_*) -- */
	.ram_image3.entry :
	{
		__image3_entry_func__ = .;
		KEEP(*ameba_img3_start*(.image3.entry.data))
		__image3_real_entry_func__ = .;
		KEEP(*(.image3.entry.data))
		KEEP(*(.image3.nsc_entry.text*))
		__ram_tz_end__ = .;
	} > NP_PSRAM_NSC

	.gnu.sgstubs :
	{
		. = ALIGN(32);
		__ram_image3_nsc_start__ = .;
		*(.gnu.sgstubs*)
		. = ALIGN(32);
	} > NP_PSRAM_NSC

	__ram_image3_nsc_end__ = ADDR(.gnu.sgstubs) + SIZEOF(.gnu.sgstubs);


	.ram_image3.text :
	{
		__ram_image3_text_start__ = .;
		*(*.text*)
		*(*.rodata*)
		. = ALIGN(32);
	} > NP_PSRAM_S

	.ram_image3.data :
	{
		__ram_image3_data_start__ = .;
		*(*.data*)
		__ram_image3_data_end__ = .;

		. = ALIGN(32);
		__ram_image3_text_end__ = .;
	} > NP_PSRAM_S

	.ram_image3.bss (NOLOAD):
	{
		__rom_bss_start__ = .;
		*lib_rom.a:*(*.bss*)
		__rom_bss_end__ = .;
		__pro3_bss_start__ = .;
		*amebapro3*(*.bss*)
		__pro3_bss_end__ = .;
		/* common part of Ameba Series Image3 */
		__image3_bss_start__ = .;
		*(*.bss*)
		*(COMMON)
		__image3_bss_end__ = .;
		end = .;
		. = . + 32;
		__image3_heap_start__ = .;
	} > NP_PSRAM_S

	/* image2: normal image start */
	.ram_image2.entry :
	{
		__image2_entry_func__ = .;
	} > NP_SSRAM

}

__psram_s_load_addr__ = __np_psram_nsc_start__;