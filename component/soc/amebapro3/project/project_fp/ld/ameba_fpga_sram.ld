
INCLUDE "layout.ld"

__km0_msp_size__ = 4096;

__km0_msp_top__ = ORIGIN(FP_SSRAM) + LENGTH(FP_SSRAM) - 4;
__km0_msp_limit__ = __km0_msp_top__ - __km0_msp_size__ + 4;
__StackTop = __km0_msp_top__;

SECTIONS
{
	.ram.text :
	{
		KEEP(*(.isr_vector))
		KEEP(*(.vectors_table_text))
		*(.text*)
		*(.*.text*)
	} > FP_SSRAM		
	
	.ram.rodata :
	{
		*(.rodata*)
		*(.*.rodata*)
	} > FP_SSRAM
	
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
	} > FP_SSRAM
	
 	.ram.bss :
	{
		__bss_start__ = .;
    	*(.bss*)
		*(.*.bss*)
		__bss_end__ = .;
	} > FP_SSRAM
}
