INCLUDE "layout.ld"

__fp_msp_top__ = __fp_msp_end__ - 4;
__fp_msp_limit__ = __fp_msp_start__;
__StackTop = __fp_msp_top__;
__StackLim = __fp_msp_limit__;

SECTIONS
{
	/* STAGE1 VP firmware */
	.ssram.dummy :
	{
		fp_loading_start = .;
		LONG(0xBABEFACE)
		FILL(0xBABEFACE)
		. = ALIGN (256);
	} > FP_SSRAM
		
	.ssram.vector :
	{
		. = ALIGN (256);
		fp_loading_vector = .;
		KEEP(*(.isr_vector))
		KEEP(*(.vectors_table_text))
	} > FP_SSRAM
	
	.ssram.text :
	{
		. = ALIGN (4);
		__text_start__ = .;
		*(*.text*)
		__text_end__ = .;
	} > FP_SSRAM

	.ssram.rodata :
	{
		. = ALIGN (4);
		
		__rodata_start__ = .;
		__cmd_table_start__ = .;
		KEEP(*(.cmd.table.data))
		__cmd_table_end__ = .;	
		
		*(*.rodata*)
		__rodata_end__ = .;
	} > FP_SSRAM

	.preinit_array :
	{
		PROVIDE_HIDDEN (__preinit_array_start = .);
		KEEP (*(.preinit_array*))
		PROVIDE_HIDDEN (__preinit_array_end = .);
	} > FP_SSRAM

	.init_array :
	{
		PROVIDE_HIDDEN (__init_array_start = .);
		KEEP (*(SORT(.init_array.*)))
		KEEP (*(.init_array*))
		PROVIDE_HIDDEN (__init_array_end = .);
	} > FP_SSRAM

	.fini_array :
	{
		PROVIDE_HIDDEN (__fini_array_start = .);
		KEEP (*(SORT(.fini_array.*)))
		KEEP (*(.fini_array*))
		PROVIDE_HIDDEN (__fini_array_end = .);
	} > FP_SSRAM

	.ARM.extab :
	{
		*(.ARM.extab* .gnu.linkonce.armextab.*)
	} > FP_SSRAM

	.ARM.exidx :
	{
		*(.ARM.exidx* .gnu.linkonce.armexidx.*)
	} > FP_SSRAM

	.ssram.data :
	{
		. = ALIGN (4);
		
		__data_start__ = .;
		__ipc_table_start__ = .;
		KEEP(*(.ipc.table.data))
		__ipc_table_end__ = .;
		
		*(*.data*)
		__data_end__ = .;
	} > FP_SSRAM
	
	.ssram.bss (NOLOAD) :
	{
		. = ALIGN (4);
		__bss_start__ = .;
		*(*.bss*)
		__bss_end__ = .;
		__ssram_heap_buffer_start__ = .;
	} > FP_SSRAM

	.ssram.heap_ext (NOLOAD) :
	{
		. = ALIGN (8);
		__ssram_heap_extend_start__ = .;
		. = ORIGIN(FP_SSRAM_HEAP_EXT) + LENGTH(FP_SSRAM_HEAP_EXT);
		__ssram_heap_extend_end__ = .;
	} > FP_SSRAM_HEAP_EXT	
}

__ssram_heap_buffer_end__  = ORIGIN(FP_SSRAM) + LENGTH(FP_SSRAM);
__ssram_heap_buffer_size__ = __ssram_heap_buffer_end__ - __ssram_heap_buffer_start__;
__ssram_heap_extend_size__ = __ssram_heap_extend_end__ - __ssram_heap_extend_start__;

ASSERT(__ssram_heap_buffer_end__ == __ssram_heap_extend_start__,
       "ERROR: FP HEAP EXT Start not correct!")
	   
/* used in init */	   
__heap_start__ = __ssram_heap_buffer_start__;
__heap_end__ = __ssram_heap_extend_end__;	   