ENTRY(Reset_Handler)

INCLUDE "layout.ld"

__km4_msp_size__ = 4096;

__km4_msp_top__ = ORIGIN(NP_DTCM) + LENGTH(NP_DTCM) - 4;
__km4_msp_limit__ = __km4_msp_top__ - __km4_msp_size__ + 4;
__StackTop = __km4_msp_top__;
__StackLim = __km4_msp_limit__;

__vsram_start__ = ORIGIN(VSRAM);
__vsram_end__ = ORIGIN(VSRAM) + LENGTH(VSRAM);

__ddr_start__ = ORIGIN(DDR);
__ddr_end__ = ORIGIN(DDR) + LENGTH(DDR);

SECTIONS
{
	.sram.text :
	{
		*(.sram.text*)
	} > NP_SSRAM		
	
	.sram.rodata :
	{
		*(.sram.rodata*)
	} > NP_SSRAM
	
	.sram.data :
	{
		*(.sram.data*)
	} > NP_SSRAM

	.noncache.data :
	{
		*(.noncache.data)
	} > NP_SSRAM_NC

	.ram.text :
	{
		KEEP(*(.isr_vector))
		KEEP(*(.vectors_table_text))
		*(.text*)
		*(.*.text*)
	} > NP_ITCM		
	
	.ram.rodata :
	{
		*(.rodata*)
		*(.*.rodata*)
	} > NP_SSRAM
	
	/* data & bss should have not any content */
	.ram.data :
	{
		__cmd_table_start__ = .;
		KEEP(*(.cmd.table.data))
		__cmd_table_end__ = .;
		__ipc_table_start__ = .;
		KEEP(*(.ipc.table.data))
		__ipc_table_end__ = .;
		*(.data*)
		*(.*.data*)
	} > NP_SSRAM
	
 	.ram.bss :
	{
		__bss_start__ = .;
    	*(.bss*)
		*(.*.bss*)
		__bss_end__ = .;
	} > NP_SSRAM
    

}
