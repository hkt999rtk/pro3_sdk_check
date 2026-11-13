INCLUDE "layout.ld"
/* INCLUDE "rom_symbol.ld" */

__StackTop = __np_ssram_romstk_top__;
__StackLim = __np_ssram_romstk_lim__;
__StackTopNS = __np_ssram_stk_ns_top__;
__StackLimNS = __np_ssram_stk_ns_lim__;

SECTIONS
{
	.ram_image2.entry :
	{
		__ssram_load_addr__ = .;
		__ram_image2_text_start__ = .;
		__image2_entry_func__ = .;
		KEEP(*(SORT(.image2.entry.data*)))

	} > NP_SSRAM
	
	.ssram.text :
	{
		. = ALIGN (4);
		*(.sram.only.text*)
		KEEP(*ameba_psram*(.text*))
		KEEP(*ameba_pmu*(.text*))
		KEEP(*ameba_pmc*(.text*))
		KEEP(*ameba_pmc_lib*(.text*))
		KEEP(task*(.text*))
		KEEP(port*(.text*))
		KEEP(*portasm*(.text*))
		KEEP(*ameba_rcc_rom*(.text*))
		KEEP(*ameba_loguart_rom*(.text*))
		KEEP(*ameba_timer*(.text*))
		KEEP(*ameba_systimer*(.text*))
		KEEP(*ameba_shell_rom*(.text*))
		KEEP(*diag*(.text*))
		KEEP(log*(.text*))
		KEEP(*char2num*(.text*))
		KEEP(*memchr*(.text*))
		KEEP(*memcmp*(.text*))
		KEEP(*memcpy*(.text*))
		KEEP(*memmove*(.text*))
		KEEP(*memset*(.text*))
		KEEP(*sscanf_minimal*(.text*))
		KEEP(*strcmp*(.text*))
		KEEP(*strcpy*(.text*))
		KEEP(*stricmp*(.text*))
		KEEP(*strlen*(.text*))
		KEEP(*strncpy*(.text*))
		KEEP(*strtol*(.text*))
		KEEP(*strtoul*(.text*))
		KEEP(*strupr*(.text*))
		KEEP(*strwrap*(.text*))
		KEEP(*ameba_delay*(.text*))
		KEEP(*ameba_clk*(.text*))
		KEEP(*ameba_arch*(.text*))
		KEEP(*ameba_pll*(.text*))
		KEEP(*ameba_vector*(.text*))
	} > NP_SSRAM

	.ssram.rodata :
	{
		. = ALIGN (4);
		*(.sram.only.rodata*)
		KEEP(*ameba_psram*(.rodata*))
		KEEP(*ameba_pmu*(.rodata*))
		KEEP(*ameba_pmc*(.rodata*))
		KEEP(*ameba_pmc_lib*(.rodata*))
		KEEP(task*(.rodata*))
		KEEP(port*(.rodata*))
		KEEP(*portasm*(.rodata*))
		KEEP(*ameba_rcc_rom*(.rodata*))
		KEEP(*ameba_loguart_rom*(.rodata*))
		KEEP(*ameba_timer*(.rodata*))
		KEEP(*ameba_systimer*(.rodata*))
		KEEP(*ameba_shell_rom*(.rodata*))
		KEEP(*diag*(.rodata*))
		KEEP(log*(.rodata*))
		KEEP(*char2num*(.rodata*))
		KEEP(*memchr*(.rodata*))
		KEEP(*memcmp*(.rodata*))
		KEEP(*memcpy*(.rodata*))
		KEEP(*memmove*(.rodata*))
		KEEP(*memset*(.rodata*))
		KEEP(*sscanf_minimal*(.rodata*))
		KEEP(*strcmp*(.rodata*))
		KEEP(*strcpy*(.rodata*))
		KEEP(*stricmp*(.rodata*))
		KEEP(*strlen*(.rodata*))
		KEEP(*strncpy*(.rodata*))
		KEEP(*strtol*(.rodata*))
		KEEP(*strtoul*(.rodata*))
		KEEP(*strupr*(.rodata*))
		KEEP(*strwrap*(.rodata*))
		KEEP(*ameba_delay*(.rodata*))
		KEEP(*ameba_clk*(.rodata*))
		KEEP(*ameba_arch*(.rodata*))
		KEEP(*ameba_pll*(.rodata*))
		KEEP(*ameba_vector*(.rodata*))
	} > NP_SSRAM

	.preinit_array :
	{
		PROVIDE_HIDDEN (__preinit_array_start = .);
		KEEP (*(.preinit_array*))
		PROVIDE_HIDDEN (__preinit_array_end = .);
	} > NP_SSRAM

	.init_array :
	{
		PROVIDE_HIDDEN (__init_array_start = .);
		KEEP (*(SORT(.init_array.*)))
		KEEP (*(.init_array*))
		PROVIDE_HIDDEN (__init_array_end = .);
	} > NP_SSRAM

	.fini_array :
	{
		PROVIDE_HIDDEN (__fini_array_start = .);
		KEEP (*(SORT(.fini_array.*)))
		KEEP (*(.fini_array*))
		PROVIDE_HIDDEN (__fini_array_end = .);
	} > NP_SSRAM

	.ARM.extab :
	{
		*(.ARM.extab* .gnu.linkonce.armextab.*)
	} > NP_SSRAM

	.ARM.exidx :
	{
		*(.ARM.exidx* .gnu.linkonce.armexidx.*)
	} > NP_SSRAM	
	
	.ssram.data :
	{
		. = ALIGN (4);
		*(.sram.only.data*)
		KEEP(*ameba_psram*(.data*))
		KEEP(*ameba_pmu*(.data*))
		KEEP(*ameba_pmc*(.data*))
		KEEP(*ameba_pmc_lib*(.data*))
		KEEP(task*(.data*))
		KEEP(port*(.data*))
		KEEP(*portasm*(.data*))
		KEEP(*ameba_rcc_rom*(.data*))
		KEEP(*ameba_loguart_rom*(.data*))
		KEEP(*ameba_timer*(.data*))
		KEEP(*ameba_systimer*(.data*))
		KEEP(*ameba_shell_rom*(.data*))
		KEEP(*diag*(.data*))
		KEEP(log*(.data*))
		KEEP(*char2num*(.data*))
		KEEP(*memchr*(.data*))
		KEEP(*memcmp*(.data*))
		KEEP(*memcpy*(.data*))
		KEEP(*memmove*(.data*))
		KEEP(*memset*(.data*))
		KEEP(*sscanf_minimal*(.data*))
		KEEP(*strcmp*(.data*))
		KEEP(*strcpy*(.data*))
		KEEP(*stricmp*(.data*))
		KEEP(*strlen*(.data*))
		KEEP(*strncpy*(.data*))
		KEEP(*strtol*(.data*))
		KEEP(*strtoul*(.data*))
		KEEP(*strupr*(.data*))
		KEEP(*strwrap*(.data*))
		KEEP(*ameba_delay*(.data*))
		KEEP(*ameba_clk*(.data*))
		KEEP(*ameba_arch*(.data*))
		KEEP(*ameba_pll*(.data*))
		KEEP(*ameba_vector*(.data*))
	} > NP_SSRAM
	
	.ssram.bss (NOLOAD) :
	{
		. = ALIGN (4);
		__sram_bss_start__ = .;
		*(.sram.only.bss*)
		KEEP(*ameba_psram*(.bss*))
		KEEP(*ameba_pmu*(.bss*))
		KEEP(*ameba_pmc*(.bss*))
		KEEP(*ameba_pmc_lib*(.bss*))
		KEEP(task*(.bss*))
		KEEP(port*(.bss*))
		KEEP(*portasm*(.bss*))
		KEEP(*ameba_rcc_rom*(.bss*))
		KEEP(*ameba_loguart_rom*(.bss*))
		KEEP(*ameba_timer*(.bss*))
		KEEP(*ameba_systimer*(.bss*))
		KEEP(*ameba_shell_rom*(.bss*))
		KEEP(*diag*(.bss*))
		KEEP(log*(.bss*))
		KEEP(*char2num*(.bss*))
		KEEP(*memchr*(.bss*))
		KEEP(*memcmp*(.bss*))
		KEEP(*memcpy*(.bss*))
		KEEP(*memmove*(.bss*))
		KEEP(*memset*(.bss*))
		KEEP(*sscanf_minimal*(.bss*))
		KEEP(*strcmp*(.bss*))
		KEEP(*strcpy*(.bss*))
		KEEP(*stricmp*(.bss*))
		KEEP(*strlen*(.bss*))
		KEEP(*strncpy*(.bss*))
		KEEP(*strtol*(.bss*))
		KEEP(*strtoul*(.bss*))
		KEEP(*strupr*(.bss*))
		KEEP(*strwrap*(.bss*))
		KEEP(*ameba_delay*(.bss*))
		KEEP(*ameba_clk*(.bss*))
		KEEP(*ameba_arch*(.bss*))
		KEEP(*ameba_pll*(.bss*))
		KEEP(*ameba_vector*(.bss*))
		__sram_bss_end__ = .;
	} > NP_SSRAM		


	.psram.text :
	{
		__psram_load_addr__ = .;
		. = ALIGN (4);
		*(*.text*)
	} > NP_PSRAM

	.psram.rodata :
	{
		. = ALIGN (4);
		
		__cmd_table_start__ = .;
		KEEP(*(.cmd.table.data))
		__cmd_table_end__ = .;	
		
		*(*.rodata*)
	} > NP_PSRAM	

	.noncache.data :
	{
		*(.noncache.data)
	} > NP_SSRAM_NC	

	.psram.data :
	{
		. = ALIGN (4);		
		__ipc_table_start__ = .;
		KEEP(*(.ipc.table.data))
		__ipc_table_end__ = .;		
		*(*.data*)
	} > NP_PSRAM
	
	.psram.bss (NOLOAD) :
	{
		. = ALIGN (4);
		__bss_start__ = .;
		*(*.bss*)
		__bss_end__ = .;
		__psram_heap_buffer_start__ = .;
	} > NP_PSRAM
	
	.psram.heap_ext (NOLOAD) :
	{
		. = ALIGN (8);
		__psram_heap_extend_start__ = .;
		. = ORIGIN(NP_PSRAM_HEAP_EXT) + LENGTH(NP_PSRAM_HEAP_EXT);
		__psram_heap_extend_end__ = .;
	} > NP_PSRAM_HEAP_EXT
	
	.bluetooth_trace.text :
	{
		__btrace_start__ = .;
		*(.BTTRACE)
		__btrace_end__ = .;
	} > BTRACE

	.coex_trace.text :
	{
		__ctrace_start__ = .;
		*(.COEXTRACE)
		__ctrace_end__ = .;
	} > CTRACE
}

__psram_heap_buffer_end__  = ORIGIN(NP_PSRAM) + LENGTH(NP_PSRAM);
__psram_heap_buffer_size__ = __psram_heap_buffer_end__ - __psram_heap_buffer_start__;
__psram_heap_extend_size__ = __psram_heap_extend_end__ - __psram_heap_extend_start__;

ASSERT(__psram_heap_buffer_end__ == __psram_heap_extend_start__,
       "ERROR: NP HEAP EXT Start not correct!")
	   
/* used in init */	   
__heap_start__ = __psram_heap_buffer_start__;
__heap_end__ = __psram_heap_extend_end__;		   
