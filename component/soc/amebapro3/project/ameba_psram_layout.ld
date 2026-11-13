MEMORY
{

	/* ASIC ITCM NP : 256K(ROM) 16K(RAM)  VP : 16K */
	NP_ITCM_ROM (rwx) :                     ORIGIN = 0x00000000, LENGTH = 256K
	NP_ITCM (rwx) :                         ORIGIN = 0x00040000, LENGTH = 16K
	VP_ITCM (rwx) :                         ORIGIN = 0x6F080000, LENGTH = 16K
	

	/* ASIC DTCM NP : 16K  VP : 16K */
	NP_DTCM (rwx) :                         ORIGIN = 0x00100000, LENGTH = 16K
	NP_DTCM_BSS    (rwx) :                  ORIGIN = 0x00100000, LENGTH = 4K
	NP_DTCM_NOINIT (rwx) :                 	ORIGIN = 0x00101000, LENGTH = 1K
	NP_DTCM_RTT (rwx) :                  	ORIGIN = 0x00101400, LENGTH = 11K
	VP_DTCM (rwx) :                         ORIGIN = 0x6F090000, LENGTH = 16K	

	/* SYSTEM SRAM */
	/****************************************************************************/
	SYS_SRAM (rwx) :                 		ORIGIN = 0x28000000, LENGTH = 512K
	/****************************************************************************/
	FP_SSRAM_INIT	(rwx) :					ORIGIN = 0x28000000, LENGTH = 4K
	NP_SSRAM_MSP    (rwx) :					ORIGIN = 0x28000000, LENGTH = 4K
	NP_SSRAM_ROMBSS (rwx) :					ORIGIN = 0x28001000, LENGTH = 3K
	NP_SSRAM_MAILBX (rwx) :					ORIGIN = 0x28001C00, LENGTH = 1K
	NP_SSRAM_NSBAN0 (rwx) :					ORIGIN = 0x28002000, LENGTH = 4K
	NP_SSRAM_NSBAN1 (rwx) :					ORIGIN = 0x28003000, LENGTH = 4K
	NP_SSRAM_NSBAN2 (rwx) :					ORIGIN = 0x28004020, LENGTH = 128K-32
	/* only for verification, not for normal using */
	NP_SSRAM_VERIFY (rwx) :					ORIGIN = 0x28024000, LENGTH = 360K
	
	NP_SSRAM (rwx) :						ORIGIN = 0x28024000, LENGTH = 68K
	NP_SSRAM_NC (rwx) :						ORIGIN = 0x28035000, LENGTH = 32K
	FP_SSRAM (rwx) :						ORIGIN = 0x2803D020, LENGTH = 256K-32
	FP_SSRAM_MSP (rwx) :					ORIGIN = 0x2807D000, LENGTH = 4K
	IPC_SSRAM (rwx) :						ORIGIN = 0x2807E000, LENGTH = 4K
	SHM_SSRAM (rwx) :						ORIGIN = 0x2807F000, LENGTH = 4K

	/* SYSTEM SRAM SECURE */
	/****************************************************************************/
	SYS_SRAM_S (rwx) :                 		ORIGIN = 0x38000000, LENGTH = 512K
	/****************************************************************************/
	NP_SSRAM_ROMBSS_S  (rwx) :				ORIGIN = 0x38002000, LENGTH = 4K
	NP_SSRAM_MSP_S     (rwx) :				ORIGIN = 0x38003000, LENGTH = 4K
	NP_SSRAM_LOADER_S  (rwx) :				ORIGIN = 0x38004020, LENGTH = 128K-32
	NP_SSRAM_MPBSS_S (rwx) :				ORIGIN = 0x38024000, LENGTH = 68K+32K+256K		/* only for verification test */
	
	/* BT SHARE RAM */
	/****************************************************************************/
	BT_SHRAM (rwx) :                 		ORIGIN = 0x28080000, LENGTH = 128K
	/****************************************************************************/	
	
	
	/* PSRAM */
	/****************************************************************************/
	PSRAM (rwx) :                  			ORIGIN = 0x20000000, LENGTH = 4M 
	/****************************************************************************/
	NP_PSRAM (rwx) :						ORIGIN = 0x20000020, LENGTH = 2M - 32
	VP_PSRAM (rwx) :						ORIGIN = 0x20200020, LENGTH = 2M - 192K - 64
	AP_PSRAM_NSBAN0 (rwx) :					ORIGIN = 0x203CFFE0, LENGTH = 188K + 32
	AP_PSRAM_NSBAN1 (rwx) :					ORIGIN = 0x203FF000, LENGTH = 4K

	/* PSRAM SECURE */
	/****************************************************************************/
	PSRAM_S (rwx) :                  		ORIGIN = 0x30000000, LENGTH = 4M 
	/****************************************************************************/
	AP_PSRAM_BL1 (rwx) :					ORIGIN = 0x303CFFE0, LENGTH = 188K + 32
	AP_PSRAM_SHARE (rwx) :					ORIGIN = 0x303FF000, LENGTH = 4K

	/* VIDEO SRAM */
	/****************************************************************************/
	VSRAM (rwx) :                  		    ORIGIN = 0x6F000000, LENGTH = 192K
	/****************************************************************************/
	VP_VSRAM (rwx) :                  		ORIGIN = 0x6F000000, LENGTH = 192K
	
	/* FAKE DDR */
	/****************************************************************************/
	DDR (rwx) :                    			ORIGIN = 0x20400000, LENGTH = 4M
	/****************************************************************************/
	VP_DDR (rwx) :                    		ORIGIN = 0x20400000, LENGTH = 4M
	
	
	/* XIP */
	/****************************************************************************/
	XIP (rwx) :                    			ORIGIN = 0x08000000, LENGTH = 64M
	/****************************************************************************/		
	
	/* XIP REMAP */
	/****************************************************************************/
	XIP_REMAP (rwx) :                    	ORIGIN = 0x0C000000, LENGTH = 64M
	/****************************************************************************/
}

__np_dtcm_bss_start__ = ORIGIN(NP_DTCM_BSS);
__np_dtcm_bss_end__ = ORIGIN(NP_DTCM_BSS) + LENGTH(NP_DTCM_BSS);

__fp_ssram_init_start__ = ORIGIN(FP_SSRAM_INIT);
__fp_ssram_init_end__ = ORIGIN(FP_SSRAM_INIT) + LENGTH(FP_SSRAM_INIT);

__np_ssram_rombss_start__ = ORIGIN(NP_SSRAM_ROMBSS);
__np_ssram_rombss_end__ = ORIGIN(NP_SSRAM_ROMBSS) + LENGTH(NP_SSRAM_ROMBSS);

__np_ssram_romstk_start__ = ORIGIN(NP_SSRAM_MSP_S);
__np_ssram_romstk_end__ = ORIGIN(NP_SSRAM_MSP_S) + LENGTH(NP_SSRAM_MSP_S);
__np_ssram_romstk_lim__ = __np_ssram_romstk_start__;
__np_ssram_romstk_top__ = __np_ssram_romstk_end__ - 8;

__np_ssram_stk_ns_start = ORIGIN(NP_SSRAM_MSP);
__np_ssram_stk_ns_end__ = ORIGIN(NP_SSRAM_MSP) + LENGTH(NP_SSRAM_MSP);
__np_ssram_stk_ns_lim__ = __np_ssram_stk_ns_start;
__np_ssram_stk_ns_top__ = __np_ssram_stk_ns_end__ - 8;

__np_ssram_loader_s_start__ = ORIGIN(NP_SSRAM_LOADER_S) - 32;
__np_ssram_loader_s_end__ = ORIGIN(NP_SSRAM_LOADER_S) + LENGTH(NP_SSRAM_LOADER_S);

__np_ssram_verify_start__ = ORIGIN(NP_SSRAM_VERIFY);
__np_ssram_verify_end__ = ORIGIN(NP_SSRAM_VERIFY) + LENGTH(NP_SSRAM_VERIFY);

__np_ssram_nc_start__ = ORIGIN(NP_SSRAM_NC);
__np_ssram_nc_end__ = ORIGIN(NP_SSRAM_NC) + LENGTH(NP_SSRAM_NC);

__vsram_start__ = ORIGIN(VSRAM);
__vsram_end__ = ORIGIN(VSRAM) + LENGTH(VSRAM);

__np_psram_start__ = ORIGIN(NP_PSRAM);
__np_psram_end__ = ORIGIN(NP_PSRAM) + LENGTH(NP_PSRAM);

__vp_psram_start__ = ORIGIN(VP_PSRAM) - 32;
__vp_psram_vector__ = __vp_psram_start__ + 256;
__vp_psram_end__ = ORIGIN(VP_PSRAM) + LENGTH(VP_PSRAM);

__vp_ddr_start__ = ORIGIN(VP_DDR);
__vp_ddr_end__ = ORIGIN(VP_DDR) + LENGTH(VP_DDR);

__fp_ssram_start__ = ORIGIN(FP_SSRAM) - 32;
__fp_ssram_vector__ = __fp_ssram_start__ + 256;
__fp_ssram_end__ = ORIGIN(FP_SSRAM) + LENGTH(FP_SSRAM);

__fp_msp_start__ = ORIGIN(FP_SSRAM_MSP);
__fp_msp_end__ = ORIGIN(FP_SSRAM_MSP) + LENGTH(FP_SSRAM_MSP);

__ipc_ssram_start__ = ORIGIN(IPC_SSRAM);
__ipc_ssram_end__ = ORIGIN(IPC_SSRAM) + LENGTH(IPC_SSRAM);

__shm_ssram_start__ = ORIGIN(SHM_SSRAM);
__shm_ssram_end__ = ORIGIN(SHM_SSRAM) + LENGTH(SHM_SSRAM);

__np_nand_boot_temp__ = ORIGIN(SYS_SRAM) + LENGTH(SYS_SRAM) - 128K;

__ap_psram_bl1_start__ = ORIGIN(AP_PSRAM_BL1);
__ap_psram_vector__ = __ap_psram_bl1_start__ + 32;
__ap_psram_bl1_end__ = ORIGIN(AP_PSRAM_BL1) + LENGTH(AP_PSRAM_BL1);

__ap_ddr_ro_start__ = ORIGIN(AP_DDR_RO);
__ap_ddr_ro_end__ = ORIGIN(AP_DDR_RO) + LENGTH(AP_DDR_RO);

__ap_ddr_start__ = ORIGIN(AP_DDR);
__ap_ddr_end__ = ORIGIN(AP_DDR) + LENGTH(AP_DDR);

__nn_ddr_start__ = ORIGIN(NN_DDR);
__nn_ddr_end__ = ORIGIN(NN_DDR) + LENGTH(NN_DDR);

__nn_mdl_start__ = ORIGIN(MDL_DDR);
__nn_mdl_end__ = ORIGIN(MDL_DDR) + LENGTH(MDL_DDR);

__en_ddr_start__ = ORIGIN(EN_DDR);
__en_ddr_end__ = ORIGIN(EN_DDR) + LENGTH(EN_DDR);

__ddr_start__ = ORIGIN(DDR);
__ddr_end__ = ORIGIN(DDR) + LENGTH(DDR);

