ENTRY(Reset_Handler)

INCLUDE "rom_layout.ld"

SECTIONS
{
	/* addr fixed */
	.skip :
	{
		. = 0x130;
	} > NP_ITCM_ROM
	.stub.text :
	{
		/* code is position independent */
		__rom_stub_start__ = .;
		/* 8K space for rom stubs, 1024 APIs */
		*(.stub.text)
		__rom_stub_end__ = .;
	} > NP_ITCM_ROM
}
