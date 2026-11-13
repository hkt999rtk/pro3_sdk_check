/*
 * Copyright (C) 2020 Realtek Semiconductor Corp.	All Rights Reserved.
 *
 * Author: PSP Software Group
 */

INCLUDE "layout.ld"

/* Linker script to place sections and symbol values. Should be used together
 * with other linker script that defines memory regions FLASH and RAM.
 * It references following symbols, which must be defined in code:
 * _reset_init : Entry of reset handler
 *
 * It defines following symbols, which code can use without definition:
 * __exidx_start
 * __exidx_end
 * __etext
 * __data_start__
 * __preinit_array_start
 * __preinit_array_end
 * __init_array_start
 * __init_array_end
 * __fini_array_start
 * __fini_array_end
 * __data_end__
 * __bss_start__
 * __bss_end__
 * __end__
 * end
 * __HeapLimit
 * __StackLimit
 * __StackTop
 * __stack
 */
ENTRY(_vector_table)

SECTIONS
{
	.text :
	{
		. = ALIGN(0x4);
		*(.freertos_vectors)
		*(.vectors)
		*(.boot)
		__text_start__ = .;
		*(*.text)		/* .text sections (code) */
		*(*.text*)		/* .text* sections (code) */
		__text_end__ = .;
		__cmd_table_start__ = .;
		KEEP(*(.cmd.table.data))
		__cmd_table_end__ = .;		
		
		*(*.rodata)		/* .rodata sections (constants, strings, etc.) */
		*(*.rodata*)		/* .rodata* sections (constants, strings, etc.) */
		*(.glue_7)		/* glue arm to thumb code */
		*(.glue_7t)		/* glue thumb to arm code */
		*(.vfp11_veneer)
		*(.eh_frame)
		KEEP (*(.init))
		KEEP (*(.fini))
		. = ALIGN(4);
		 __etext = .;
	} > AP_DDR_RO

	.ARM.extab :
	{
		*(.ARM.extab* .gnu.linkonce.armextab.*)
	} > AP_DDR_RO

	__exidx_start = .;
	.ARM.exidx :
	{
		*(.ARM.exidx* .gnu.linkonce.armexidx.*)
	} > AP_DDR_RO
	__exidx_end = .;

	.ctors :
	{
		__CTOR_LIST__ = .;
		/* gcc uses crtbegin.o to find the start of
		   the constructors, so we make sure it is
		   first.  Because this is a wildcard, it
		   doesn't matter if the user does not
		   actually link against crtbegin.o; the
		   linker won't look for a file to match a
		   wildcard.  The wildcard also means that it
		   doesn't matter which directory crtbegin.o
		   is in.  */
		KEEP (*crtbegin.o(.ctors))
		KEEP (*crtbegin?.o(.ctors))
		/* We don't want to include the .ctor section from
		   from the crtend.o file until after the sorted ctors.
		   The .ctor section from the crtend file contains the
		   end of ctors marker and it must be last */
		KEEP (*(EXCLUDE_FILE(*crtend?.o *crtend.o) .ctors))
		KEEP (*(SORT(.ctors.*)))
		KEEP (*(.ctors))
		__CTOR_END__ = .;
	} > AP_DDR_RO

	.dtors :
	{
		__DTOR_LIST__ = .;
		KEEP (*crtbegin.o(.dtors))
		KEEP (*crtbegin?.o(.dtors))
		KEEP (*(EXCLUDE_FILE(*crtend?.o *crtend.o) .dtors))
		KEEP (*(SORT(.dtors.*)))
		KEEP (*(.dtors))
		__DTOR_END__ = .;
	} > AP_DDR_RO

	.preinit_array :
	{
		PROVIDE_HIDDEN (__preinit_array_start = .);
		KEEP (*(.preinit_array*))
		PROVIDE_HIDDEN (__preinit_array_end = .);
	} > AP_DDR_RO

	.init_array :
	{
		PROVIDE_HIDDEN (__init_array_start = .);
		KEEP (*(SORT(.init_array.*)))
		KEEP (*(.init_array*))
		PROVIDE_HIDDEN (__init_array_end = .);
	} > AP_DDR_RO

	.fini_array :
	{
		PROVIDE_HIDDEN (__fini_array_start = .);
		KEEP (*(SORT(.fini_array.*)))
		KEEP (*(.fini_array*))
		PROVIDE_HIDDEN (__fini_array_end = .);
	} > AP_DDR_RO

	.mmu_tbl (ALIGN(0x4000)) :
	{
		__mmu_tbl_start = .;
		*(.mmu_tbl)
		__mmu_tbl_end = .;
		__mmu_l2_tbl_start = .;
		*(.mmu_l2_tbl)
		__mmu_l2_tbl_end = .;		
	} > AP_DDR_RO

	__data_load_addr__ = .;

	.data :
	{
		. = ALIGN(32);
		__data_start__ = .;
		
		__ipc_table_start__ = .;
		KEEP(*(.ipc.table.data))
		__ipc_table_end__ = .;		

		__media_app_table_start__ = .;
		KEEP(*(.media.app.data))
		__media_app_table_end__ = .;
		
		. = ALIGN(4);
		*(*.data)
		*(*.data*)
		. = ALIGN(4);

		/* All data end */
		__data_end__ = .;
	} > AP_DDR AT>AP_DDR_RO

	.bss (NOLOAD) :
	{
		__bss_start__ = .;
		*(*.bss)
		*(*.bss.*)
		*(COMMON)
		__bss_end__ = .;
	} > AP_DDR

	.ret.ssram (NOLOAD) :
	{
		__ret_start__ = .;
		*(.ret.ssram*)
		__ret_end__ = .;
	} > AP_SSRAM_RET

	.ret.psram (NOLOAD) :
	{
		__ret_start__ = .;
		*(.ret.psram*)
		__ret_end__ = .;
		__ret_fcs_start__ = __ret_start__ + LENGTH(AP_PSRAM_RET) - LENGTH(AP_PSRAM_FCS_RET);
		*(.fcs.psram*)
		__ret_fcs_end__ = __ret_start__ + LENGTH(AP_PSRAM_RET);
		
	} > AP_PSRAM_RET

	ASSERT(__ret_end__ <= __ret_fcs_start__, "ERROR: The AP user retention data is too large and overlaps with the FCS retention parameter")


	.heap (NOLOAD) :
	{
		/* libc heap (optional), libc heap APIs defaultly redirect to FreeRTOS heap*/
		__end__ = .;
		end = __end__;
		__HeapBase = .;
		KEEP (*(.heap))
		__HeapLimit = .;
		__heap_limit = .;	/* Add for _sbrk */
		__dram_heap_buffer_start__ = .;
	} > AP_DDR
	
	.heap_ext (NOLOAD) :
	{
		/* reserved for FreeRTOS heap */
		. = ALIGN (8);
		__dram_heap_extend_start__ = .;
		. = ORIGIN(AP_DDR_HEAP_EXT) + LENGTH(AP_DDR_HEAP_EXT);
		__dram_heap_extend_end__ = .;
	} > AP_DDR_HEAP_EXT			

	.stack (NOLOAD) :
	{
		. = ALIGN(16);
		__StackLimit = .;
		*(.stack);
	} > AP_DDR_STK

	.ARM.attributes 0 : { KEEP (*(.ARM.attributes)) KEEP (*(.gnu.attributes)) }

    /* Initialize the stack at the end of the memory block. */
    __StackTop = .;
    __stack = __StackTop;
    __StackLimit = __StackLimit;

    ASSERT(__StackLimit >= __HeapLimit, "region data overflowed with stack and heap")

    PROVIDE(__stack = __StackTop);

    _end = ALIGN(0x10);
}

__dram_heap_buffer_end__  = ORIGIN(AP_DDR) + LENGTH(AP_DDR);
__dram_heap_buffer_size__ = __dram_heap_buffer_end__ - __dram_heap_buffer_start__;
__dram_heap_extend_size__ = __dram_heap_extend_end__ - __dram_heap_extend_start__;

ASSERT(__dram_heap_buffer_end__ == __dram_heap_extend_start__,
       "ERROR: AP HEAP EXT Start not correct!")
	   
/* used in init */	   
__heap_start__ = __dram_heap_buffer_start__;
__heap_end__ = __dram_heap_extend_end__;	   
