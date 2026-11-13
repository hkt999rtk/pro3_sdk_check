ENTRY(Reset_Handler)

INCLUDE "layout.ld"

__km4_msp_size__ = 4096;

__km4_msp_top__ = ORIGIN(NP_DTCM) + LENGTH(NP_DTCM) - 4;
__km4_msp_limit__ = __km4_msp_top__ - __km4_msp_size__ + 4;

__StackTop = __km4_msp_top__;
__StackLim = __km4_msp_limit__;

SECTIONS
{
	/* use option to build freertos heap on ddr */
	.ddr.bss :
	{
		__ddr_bss_start__ = .;
		*(.ddr.bss*)
		__ddr_bss_end__ = .;
	} > HEAP_DDR

	.boot.ro :
	{
		KEEP(*(.isr_vector))
		KEEP(*(.vectors_table_text))	
		KEEP(*(.init.fpga))
		KEEP(*cmse_nonsecure_call.o(.text*))
		KEEP(*ameba_psram*(.text*))
		KEEP(*ameba_psram*(.rodata*))
	} > NP_ITCM	

	.boot.rw :
	{
		KEEP(*ameba_psram*(.data*))
		KEEP(*ameba_psram*(.bss*))
	} > NP_ITCM	

	.sram.text :
	{
		*(.sram.text*)
		*(.sram.only.text*)	
	} > NP_SSRAM

	.sram.rodata :
	{
		*(.sram.rodata*)
	} > NP_SSRAM
	
	.sram.data :
	{
		*(.sram.data*)
		*(.sram.only.data*)	
	} > NP_SSRAM	

	.noncache.data :
	{
		*(.noncache.data)
	} > NP_SSRAM_NC

	.ram.text :
	{
		*(.text*)
		*(.*.text*)
	} > NP_PSRAM		
	
	.ram.rodata :
	{
		*(.rodata*)
		*(.*.rodata*)
	} > NP_PSRAM
	
	/* data & bss should have not any content */
	.ram.data :
	{
		__cmd_table_start__ = .;
		KEEP(*console_sim*(.cmd.table.data))
		KEEP(*(.cmd.table.data))
		__cmd_table_end__ = .;
		__ipc_table_start__ = .;
		KEEP(*(.ipc.table.data))
		__ipc_table_end__ = .;
		*(.data*)
		*(.*.data*)
	} > NP_PSRAM
	
 	.ram.bss :
	{
		__bss_start__ = .;
    	*(.bss*)
		*(.*.bss*)
		__bss_end__ = .;
	} > NP_PSRAM
}
