INCLUDE "layout.ld"

__vp_stack_size__ = 8192;

__vp_msp_top__ = ORIGIN(VP_DTCM) + LENGTH(VP_DTCM) - 4;
__vp_msp_limit__ = __vp_msp_top__ - __vp_stack_size__ + 4;
__StackTop = __vp_msp_top__;
__StackLim = __vp_msp_limit__;

SECTIONS
{

	/* .data and .bss will place in DDR */
	.ddr.stage1.data :
	{
		. = ALIGN (4);
		__vp_s1_data_start__ = .;
		__ipc_table_start__ = .;
		KEEP(*(.ipc.table.data))
		__ipc_table_end__ = .;	
		*(.data*)
		*(.hal.rom*.data*)
		LONG(0xCAFEBABE)
		FILL(0xCAFEBABE)
		. = ALIGN (32);
		__vp_s1_data_end__ = .;
	} > VP_DDR
	
	.ddr.data :
	{
		. = ALIGN (4);
		__vp_data_start__ = .;
		*(.bf.data*)
		LONG(0xCAFEBABE)
		FILL(0xCAFEBABE)
		. = ALIGN (32);
		__vp_data_end__ = .;
	} > VP_DDR
	
	.ddr.bss (NOLOAD) :
	{
		. = ALIGN (4);
		__bss_start__ = .;
		*(.bss*)
		*(.hal.rom*.bss*)
		. = ALIGN (4);
		__bss_end__ = .;
		__bss_bf_start__ = .;
		*(.bf.bss*)
		__bss_bf_end__ = .;
		__dram_heap_buffer_start__ = .;
	} > VP_DDR
	
	.ddr.heap_ext (NOLOAD) :
	{
		. = ALIGN (8);
		__dram_heap_extend_start__ = .;
		. = ORIGIN(VP_DDR_HEAP_EXT) + LENGTH(VP_DDR_HEAP_EXT);
		__dram_heap_extend_end__ = .;
	} > VP_DDR_HEAP_EXT		

	/* STAGE1 VP firmware */
	.psram.stage1.dummy :
	{
		vp_loading_stage1_start = .;
		LONG(0xCAFEBABE)
		FILL(0xCAFEBABE)
		. = ALIGN (256);
	} > VP_PSRAM
		
	.psram.stage1.vector :
	{
		. = ALIGN (256);
		vp_loading_vector = .;
		KEEP(*(.isr_vector))
		KEEP(*(.vectors_table_text))
	} > VP_PSRAM
	
	.psram.stage1.text :
	{
		*(.text*)
		*(.hal.rom*.text*)
	} > VP_PSRAM
	
	.psram.stage1.rodata :
	{
		. = ALIGN (4);
		
		__cmd_table_start__ = .;
		KEEP(*(.cmd.table.data))
		__cmd_table_end__ = .;		
	
		*(.rodata*)
		*(.hal.rom*.rodata*)
	} > VP_PSRAM
	
	.preinit_array :
	{
		PROVIDE_HIDDEN (__preinit_array_start = .);
		KEEP (*(.preinit_array*))
		PROVIDE_HIDDEN (__preinit_array_end = .);
	} > VP_PSRAM

	.init_array :
	{
		PROVIDE_HIDDEN (__init_array_start = .);
		KEEP (*(SORT(.init_array.*)))
		KEEP (*(.init_array*))
		PROVIDE_HIDDEN (__init_array_end = .);
	} > VP_PSRAM

	.fini_array :
	{
		PROVIDE_HIDDEN (__fini_array_start = .);
		KEEP (*(SORT(.fini_array.*)))
		KEEP (*(.fini_array*))
		PROVIDE_HIDDEN (__fini_array_end = .);
	} > VP_PSRAM

	.ARM.extab :
	{
		*(.ARM.extab* .gnu.linkonce.armextab.*)
	} > VP_PSRAM

	.ARM.exidx :
	{
		*(.ARM.exidx* .gnu.linkonce.armexidx.*)
		. = ALIGN (4);
	} > VP_PSRAM

	.psram.bkup.stage1.data :
	{
		. = ALIGN (4);
		__bkup_s1_data_start__ = .;
		LONG(0xCAFEBABE)
		FILL(0xCAFEBABE)
		. += (__vp_s1_data_end__ - __vp_s1_data_start__);	
		__bkup_s1_data_end__ = .;
	} > VP_PSRAM	
	
	/* OTHERS VP firmware */
	/* HEADER */
	.psram.header :
	{
		. = ALIGN (128);
		. += 32;	
	} > VP_PSRAM
	
	.psram.text :
	{
		vp_loading_start = .;		
		*(.bf.text*)
	} > VP_PSRAM

	.psram.rodata :
	{
		. = ALIGN (4);
		*(.bf.rodata*)
		. = ALIGN (4);
	} > VP_PSRAM

	/* copy to this space in postbuild */
	.psram.bkup.data :
	{
		. = ALIGN (4);
		__bkup_data_start__ = .;
		LONG(0xCAFEBABE)
		FILL(0xCAFEBABE)
		. += (__vp_data_end__ - __vp_data_start__);
		__bkup_data_end__ = .;
	} > VP_PSRAM
}

__dram_heap_buffer_end__  = ORIGIN(VP_DDR) + LENGTH(VP_DDR);
__dram_heap_buffer_size__ = __dram_heap_buffer_end__ - __dram_heap_buffer_start__;
__dram_heap_extend_size__ = __dram_heap_extend_end__ - __dram_heap_extend_start__;

ASSERT(__dram_heap_buffer_end__ == __dram_heap_extend_start__,
       "ERROR: VP HEAP EXT Start not correct!")
	   
/* used in init */	   
__heap_start__ = __dram_heap_buffer_start__;
__heap_end__ = __dram_heap_extend_end__;	   
