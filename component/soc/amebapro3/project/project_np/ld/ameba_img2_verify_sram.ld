INCLUDE "layout.ld"
/* INCLUDE "rom_symbol.ld" */

__StackTop = __np_ssram_romstk_top__;
__StackLim = __np_ssram_romstk_lim__;

SECTIONS
{
	.ram_image2.entry :
	{
		__ssram_load_addr__ = .;
		__ram_image2_text_start__ = .;
		__image2_entry_func__ = .;
		KEEP(*(SORT(.image2.entry.data*)))

	} > NP_SSRAM
	
	/* psram section only used by ft2 verify */
	.psram.text :
	{
		__psram_load_addr__ = .;
		. = ALIGN (4);
		KEEP(*task_mcu*(.text*))
		KEEP(*rts_camera*(.text*))
		KEEP(*rts-dma-contig*(.text*))
		KEEP(*rts_mem*(.text*))
		KEEP(*reset-rts3915*(.text*))
		KEEP(*reset-rtsmem3915*(.text*))
		KEEP(*rlx_sh_irq*(.text*))
		KEEP(*rt_time*(.text*))
		KEEP(*rtl8735b_i2c_simple*(.text*))
		KEEP(*rtl8735b_gpio_simple*(.text*))
		KEEP(*rtx_mutex*(.text*))
		KEEP(*rtscam*(.text*))
		KEEP(*hal_isp*(.text*))
		KEEP(*reg_api*(.text*))
		KEEP(*template_hal*(.text*))
		KEEP(*isp*(.text*))
		KEEP(*ft2_audio*(.text*))
	} > NP_PSRAM

	.psram.rodata :
	{
		. = ALIGN (4);
		KEEP(*task_mcu*(.rodata*))
		KEEP(*rts_camera*(.rodata*))
		KEEP(*rts-dma-contig*(.rodata*))
		KEEP(*rts_mem*(.rodata*))
		KEEP(*reset-rts3915*(.rodata*))
		KEEP(*reset-rtsmem3915*(.rodata*))
		KEEP(*rlx_sh_irq*(.rodata*))
		KEEP(*rt_time*(.rodata*))
		KEEP(*rtl8735b_i2c_simple*(.rodata*))
		KEEP(*rtl8735b_gpio_simple*(.rodata*))
		KEEP(*rtx_mutex*(.rodata*))
		KEEP(*rtscam*(.rodata*))
		KEEP(*hal_isp*(.rodata*))
		KEEP(*reg_api*(.rodata*))
		KEEP(*template_hal*(.rodata*))
		KEEP(*isp*(.rodata*))
		KEEP(*ft2_audio*(.rodata*))
	} > NP_PSRAM	

	.psram.data :
	{
		. = ALIGN (4);		
		KEEP(*task_mcu*(.data*))
		KEEP(*rts_camera*(.data*))
		KEEP(*rts-dma-contig*(.data*))
		KEEP(*rts_mem*(.data*))
		KEEP(*reset-rts3915*(.data*))
		KEEP(*reset-rtsmem3915*(.data*))
		KEEP(*rlx_sh_irq*(.data*))
		KEEP(*rt_time*(.data*))
		KEEP(*rtl8735b_i2c_simple*(.data*))
		KEEP(*rtl8735b_gpio_simple*(.data*))
		KEEP(*rtx_mutex*(.data*))
		KEEP(*rtscam*(.data*))
		KEEP(*hal_isp*(.data*))
		KEEP(*reg_api*(.data*))
		KEEP(*template_hal*(.data*))
		KEEP(*isp*(.data*))
		KEEP(*ft2_audio*(.data*))
	} > NP_PSRAM
	
	.psram.bss (NOLOAD) :
	{
		. = ALIGN (4);
		__bss_start__ = .;
		KEEP(*task_mcu*(.bss*))
		KEEP(*rts_camera*(.bss*))
		KEEP(*rts-dma-contig*(.bss*))
		KEEP(*rts_mem*(.bss*))
		KEEP(*reset-rts3915*(.bss*))
		KEEP(*reset-rtsmem3915*(.bss*))
		KEEP(*rlx_sh_irq*(.bss*))
		KEEP(*rt_time*(.bss*))
		KEEP(*rtl8735b_i2c_simple*(.bss*))
		KEEP(*rtl8735b_gpio_simple*(.bss*))
		KEEP(*rtx_mutex*(.bss*))
		KEEP(*rtscam*(.bss*))
		KEEP(*hal_isp*(.bss*))
		KEEP(*reg_api*(.bss*))
		KEEP(*template_hal*(.bss*))
		KEEP(*isp*(.bss*))
		KEEP(*ft2_audio*(.bss*))
		__bss_end__ = .;
	} > NP_PSRAM

	.ssram.text :
	{
		. = ALIGN (4);
		*(.text*)
		*(*.text*)
	} > NP_SSRAM

	.ssram.rodata :
	{
		. = ALIGN (4);
		__cmd_table_start__ = .;
		KEEP(*(.cmd.table.data))
		__cmd_table_end__ = .;			
		*(.rodata*)
		*(*.rodata*)
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

	.noncache.data :
	{
		*(.noncache.data)
	} > NP_SSRAM_NC

	.ssram.data :
	{
		. = ALIGN (4);
		__ipc_table_start__ = .;
		KEEP(*(.ipc.table.data))
		__ipc_table_end__ = .;			
		*(.data*)
		*(*.data*)
	} > NP_SSRAM
	
	.ssram.bss (NOLOAD) :
	{
		. = ALIGN (4);
		__sram_bss_start__ = .;
		*(.bss*)
		*(*.bss*)
		__sram_bss_end__ = .;
	} > NP_SSRAM
}
