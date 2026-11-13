ENTRY(Reset_Handler)

INCLUDE "rom_layout.ld"

__StackTop = __np_ssram_romstk_top__;
__StackLim = __np_ssram_romstk_lim__;
__LoaderTop = __np_ssram_loader_s_start__;
__LoaderLim = __np_ssram_loader_s_end__;

SECTIONS
{
	/* addr fixed */
	.rom.text :
	{
		KEEP(*(.vectors_table))

		. = 0x130;
		__rom_stub_start__ = .;
		/* 8K space for rom stubs, 1024 APIs */
		KEEP(*(.stub.bin))
		. = 0x2140;	
		__rom_stub_end__ = .;
		__rom_xo_start__ = .;
		/* list xo file here, should add extra compile option */
		KEEP(SORT(*ameba_boot_hci_s*)(.hal.rom.text*))
		KEEP(SORT(*ameba_boot_rom_s*)(.hal.rom.text*))
		KEEP(SORT(*ameba_crypto_rsip*)(.hal.rom.text*))
		KEEP(SORT(*ameba_flashclk_rom_s*)(.hal.rom.text*))
		KEEP(SORT(*ameba_rsip_rom*)(.hal.rom.text*))
		KEEP(SORT(*ameba_secure_boot_s*)(.hal.rom.text*))
		KEEP(SORT(*ameba_vector_rom*)(.hal.rom.text*))
		
		KEEP(SORT(*ameba_crypto*)(.hal.rom.text*))
		KEEP(SORT(*ameba_trng*)(.hal.rom.text*))
		KEEP(SORT(*ameba_pke*)(.hal.rom.text*))
		KEEP(SORT(*ameba_rsip*)(.hal.rom.text*))
		
		KEEP(SORT(*ameba_syscfg*)(.hal.rom.text*))
		KEEP(SORT(*ameba_platform*)(.hal.rom.text*))
		KEEP(SORT(*ameba_rcc*)(.hal.rom.text*))
		KEEP(SORT(*ameba_shell*)(.hal.rom.text*))
		KEEP(SORT(*ameba_monitor*)(.hal.rom.text*))
		KEEP(SORT(*ameba_wdg*)(.hal.rom.text*))
		
		KEEP(SORT(*bootutil_misc*)(.hal.rom.text*))
		KEEP(SORT(*fault_injection_hardening*)(.hal.rom.text*))
		KEEP(SORT(*fault_injection_hardening_delay_rng*)(.hal.rom.text*))
		
		KEEP(SORT(*ameba_dump_stack*)(.hal.rom.text*))
		KEEP(SORT(*ameba_v8m_crashdump*)(.hal.rom.text*))
		. = 0x10000;
		__rom_xo_end__ = .;
		__km0_rom_start__ = .;
		KEEP(SORT(*)(.km0.rom))
		
		. = 0x10200;
		__km0_rom_end__ = .;
		KEEP(SORT(*)(.vectors_table_text))
		/*KEEP(SORT(*)(.hal.rom.text*))*/
		/*list rom objects to ensure link order */
		KEEP(SORT(*usbd_core_rom*)(.hal.rom.text*))
		KEEP(SORT(*usbd_dbg_rom*)(.hal.rom.text*))
		KEEP(SORT(*usbd_download_class_rom*)(.hal.rom.text*))
		KEEP(SORT(*usbd_download_rom*)(.hal.rom.text*))
		KEEP(SORT(*usbd_hal_rom*)(.hal.rom.text*))
		KEEP(SORT(*usbd_otp_rom*)(.hal.rom.text*))
		KEEP(SORT(*usbd_pcd_rom*)(.hal.rom.text*))
		
		KEEP(SORT(*xmodem_rom*)(.hal.rom.text*))
		KEEP(SORT(*xmodem_uart_rom*)(.hal.rom.text*))
		KEEP(SORT(*xmodem_update_rom*)(.hal.rom.text*))
		
		KEEP(SORT(*ameba_aontimer_rom*)(.hal.rom.text*))
		KEEP(SORT(*ameba_assert_rom*)(.hal.rom.text*))
		KEEP(SORT(*ameba_backup_reg*)(.hal.rom.text*))
		KEEP(SORT(*ameba_clk_rom*)(.hal.rom.text*))
		KEEP(SORT(*ameba_delay_rom*)(.hal.rom.text*))
		KEEP(SORT(*ameba_flash_rom*)(.hal.rom.text*))
		KEEP(SORT(*ameba_gdma_rom*)(.hal.rom.text*))
		KEEP(SORT(*ameba_loguart_rom*)(.hal.rom.text*))
		KEEP(SORT(*ameba_mpu_rom*)(.hal.rom.text*))
		KEEP(SORT(*ameba_pinmux_rom*)(.hal.rom.text*))
		KEEP(SORT(*ameba_sema_rom*)(.hal.rom.text*))
		KEEP(SORT(*ameba_spinand_rom*)(.hal.rom.text*))
		KEEP(SORT(*ameba_uart_rom*)(.hal.rom.text*))
		
		KEEP(SORT(*diag*)(.hal.rom.text*))
		KEEP(SORT(*log*)(.hal.rom.text*))
		
		KEEP(SORT(*char2num*)(.hal.rom.text*))
		KEEP(SORT(*memchr*)(.hal.rom.text*))
		KEEP(SORT(*memcmp*)(.hal.rom.text*))
		KEEP(SORT(*memcpy*)(.hal.rom.text*))
		KEEP(SORT(*memmove*)(.hal.rom.text*))
		KEEP(SORT(*memset*)(.hal.rom.text*))
		KEEP(SORT(*sscanf_minimal*)(.hal.rom.text*))
		KEEP(SORT(*strcmp*)(.hal.rom.text*))
		KEEP(SORT(*strcpy*)(.hal.rom.text*))
		KEEP(SORT(*stricmp*)(.hal.rom.text*))
		KEEP(SORT(*strlen*)(.hal.rom.text*))
		KEEP(SORT(*strncpy*)(.hal.rom.text*))
		KEEP(SORT(*strtol*)(.hal.rom.text*))	
		KEEP(SORT(*strtoul*)(.hal.rom.text*))
		KEEP(SORT(*strupr*)(.hal.rom.text*))
		KEEP(SORT(*strwrap*)(.hal.rom.text*))
		
		KEEP(SORT(*)(.hal.rom.text*))
		KEEP(SORT(*)(.libc.rom.text*))
		KEEP(SORT(*)(.dsp.rom.text*))
		KEEP(SORT(*)(.share.rom.text*))

	} > NP_ITCM_ROM
	
	/* will be not shown in rom stub */
	/* sync new added section to ameba_rom_bin.ld also */
	.rom.hidden.text :
	{
		KEEP(SORT(*)(.pqc.rom.text*))
		
	} > NP_ITCM_ROM

	.rom.rodata :
	{
		. = ALIGN (128);
		KEEP(*(.rom_vector_table1))
		KEEP(SORT(*)(.sim.rom.rodata*))
		
		/*KEEP(SORT(*)(.hal.rom.rodata*))*/
		KEEP(SORT(*ameba_v8m_crashdump*)(.hal.rom.rodata*))
		KEEP(SORT(*usbd_download_class_rom*)(.hal.rom.rodata*))
		KEEP(SORT(*usbd_download_rom*)(.hal.rom.rodata*))
		KEEP(SORT(*xmodem_rom*)(.hal.rom.rodata*))
		KEEP(SORT(*ameba_flash_rom*)(.hal.rom.rodata*))
		KEEP(SORT(*ameba_gdma_rom*)(.hal.rom.rodata*))
		KEEP(SORT(*ameba_loguart_rom*)(.hal.rom.rodata*))
		KEEP(SORT(*ameba_mpu_rom*)(.hal.rom.rodata*))
		KEEP(SORT(*ameba_pinmux_rom*)(.hal.rom.rodata*))
		KEEP(SORT(*ameba_spinand_rom*)(.hal.rom.rodata*))
		KEEP(SORT(*ameba_wdg*)(.hal.rom.rodata*))
		KEEP(SORT(*ameba_boot_hci_s*)(.hal.rom.rodata*))
		KEEP(SORT(*ameba_boot_rom_s*)(.hal.rom.rodata*))
		KEEP(SORT(*log*)(.hal.rom.rodata*))
				
		
		/*KEEP(SORT(*)(.hal.rom.bank.rodata*))*/
		KEEP(SORT(*fault_injection_hardening*)(.hal.rom.bank.rodata*))
		KEEP(SORT(*ameba_monitor_rom*)(.hal.rom.bank.rodata*))
		
		KEEP(SORT(*)(.libc.rom.rodata*))

		/* to compatiable with IAR compiler, the rom.rodata (especial the text string) must be located at the bottom of ROM */
		/*KEEP(SORT(*)(.rom.rodata*))*/

		/*KEEP(SORT(*)(.rodata*))*/
		KEEP(SORT(*ameba_dump_stack*)(.rom.rodata*))
		KEEP(SORT(*ameba_v8m_crashdump*)(.rom.rodata*))
		KEEP(SORT(*usbd_core_rom*)(.rom.rodata*))
		KEEP(SORT(*usbd_download_class_rom*)(.rom.rodata*))
		KEEP(SORT(*usbd_download_rom*)(.rom.rodata*))
		KEEP(SORT(*usbd_hal_rom*)(.rom.rodata*))
		KEEP(SORT(*usbd_otp_rom*)(.rom.rodata*))
		KEEP(SORT(*usbd_pcd_rom*)(.rom.rodata*))
		KEEP(SORT(*xmodem_update_rom*)(.rom.rodata*))
		KEEP(SORT(*ameba_assert_rom*)(.rom.rodata*))
		KEEP(SORT(*ameba_backup_reg*)(.rom.rodata*))
		KEEP(SORT(*ameba_clk_rom*)(.rom.rodata*))
		KEEP(SORT(*ameba_crypto.*)(.rom.rodata*))
		KEEP(SORT(*ameba_crypto_aes*)(.rom.rodata*))
		KEEP(SORT(*ameba_crypto_sha*)(.rom.rodata*))
		KEEP(SORT(*ameba_flash_rom*)(.rom.rodata*))
		KEEP(SORT(*ameba_gdma_rom*)(.rom.rodata*))
		KEEP(SORT(*ameba_loguart_rom*)(.rom.rodata*))
		KEEP(SORT(*ameba_monitor_rom*)(.rom.rodata*))
		KEEP(SORT(*ameba_mpu_rom*)(.rom.rodata*))
		KEEP(SORT(*ameba_pinmux_rom*)(.rom.rodata*))
		KEEP(SORT(*ameba_pke_common*)(.rom.rodata*))
		KEEP(SORT(*ameba_pke_ecc*)(.rom.rodata*))
		KEEP(SORT(*ameba_platform_rom*)(.rom.rodata*))
		KEEP(SORT(*ameba_rcc_rom*)(.rom.rodata*))
		KEEP(SORT(*ameba_shell_rom*)(.rom.rodata*))
		KEEP(SORT(*ameba_spinand_rom*)(.rom.rodata*))
		KEEP(SORT(*ameba_syscfg_rom*)(.rom.rodata*))
		KEEP(SORT(*ameba_uart_rom*)(.rom.rodata*))
		KEEP(SORT(*ameba_boot_hci_s*)(.rom.rodata*))
		KEEP(SORT(*ameba_boot_rom_s*)(.rom.rodata*))
		KEEP(SORT(*ameba_crypto_rsip*)(.rom.rodata*))
		KEEP(SORT(*ameba_flashclk_rom_s*)(.rom.rodata*))
		KEEP(SORT(*ameba_rsip_rom*)(.rom.rodata*))
		KEEP(SORT(*ameba_secure_boot_s*)(.rom.rodata*))
		KEEP(SORT(*ameba_vector_rom*)(.rom.rodata*))
		KEEP(SORT(*log*)(.rom.rodata*))
		
		KEEP(SORT(*)(.rom.rodata*))
		KEEP(SORT(*)(.rodata*))
		KEEP(SORT(*)(.pqc.rom.rodata*))
		
	} > NP_ITCM_ROM

	/* addr fixed */
	.hal.rom.bss :
	{
		__rom_bss_start__ = .;
		KEEP(*diag.o*(.hal.rom.bss*))
		KEEP(*ameba_flash_rom.o*(.hal.rom.bss*))
		KEEP(*shell_rom.o*(.hal.rom.bss*))
		KEEP(*ameba_crypto_rom.o*(.hal.rom.bss*))
		KEEP(SORT(*)(.hal.rom.bss*))
		
		
		. = ALIGN (32);
		KEEP(SORT(*)(.ram_vector_table1))
		KEEP(SORT(*)(.ram_vector_table2))
		KEEP(SORT(*)(.ram_vector_table3))
		
		__rom_bss_end__ = .;

	} >  NP_SSRAM_ROMBSS

	/* addr fixed */
	.hal.s_rom.bss :
	{
		__rom_bss_start_s__ = .;
		KEEP(SORT(*)(.hal.rom.bank.bss*))
		KEEP(SORT(*)(.pqc.rom.bss*))
		__rom_bss_end_s__ = .;

	} >  NP_SSRAM_ROMBSS_S

	/* addr fixed */
	.rom.noinit :
	{
		__rom_noinit_start_s__ = .;
		KEEP(SORT(*)(.hal.rom.noinit))
		__rom_noinit_end_s__ = .;

	} >  NP_DTCM_NOINIT

	/* image1 entry, this section should in RAM and fixed address for ROM */
	.ram_image1.entry :
	{
		__ram_start_table_start__ = .;
		KEEP(*(SORT(.image1.entry.data*)))
		KEEP(*(.image1.validate.rodata*))
	} > NP_SSRAM

	/* NS func entry, this section should in NS_ROM and fixed address for S_ROM */
	.ns_rom.rodata :
	{
		__rom_entry_ns_start__ = ABSOLUTE(.);
	} > NP_ITCM_ROM

	/* data & bss should have not any content */
	.ram.data :
	{
		KEEP(*(.data*))
	} > NP_SSRAM
	
 	.ram.bss :
	{
    	KEEP(*(.bss*))
	} > NP_SSRAM
	
	.rom.text.libgcc :
	{
		KEEP(*(.text*))
	} > NP_ITCM_ROM
	
	.other :
	{
		KEEP(*(*.rom*))
		KEEP(*(*.text*))
		KEEP(*(*.rodata*))
		KEEP(*(*.data*))
		KEEP(*(*.bss*))
	} > NP_SSRAM

	/* retention RAM entry, this section should in RAM and fixed address for ROM */
	.ram_retention.entry :
	{
		__retention_entry_func__ = ABSOLUTE(.);
	} > NP_SSRAM
}
