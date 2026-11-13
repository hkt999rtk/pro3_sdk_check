MEMORY
{

	/* FPGA ITCM NP : 512K  VP : 16K */
	NP_ITCM (rwx) :                         ORIGIN = 0x00000000, LENGTH = 512K
	

	/* FPGA DTCM NP : 64K  VP : 32K */
	NP_DTCM (rwx) :                         ORIGIN = 0x00100000, LENGTH = 64K	

	/* SYSTEM SRAM */
	/****************************************************************************/
	SYS_SRAM (rwx) :                 		ORIGIN = 0x28000000, LENGTH = 512K
	/****************************************************************************/
	NP_SSRAM_MSP    (rwx) :					ORIGIN = 0x28000000, LENGTH = 4K
	NP_SSRAM_ROMBSS (rwx) :					ORIGIN = 0x28001000, LENGTH = 3K
	NP_SSRAM_MAILBX (rwx) :					ORIGIN = 0x28001C00, LENGTH = 1K
	NP_SSRAM_NSBAN0 (rwx) :					ORIGIN = 0x28002000, LENGTH = 4K
	NP_SSRAM_NSBAN1 (rwx) :					ORIGIN = 0x28003000, LENGTH = 4K
	NP_SSRAM_NSBAN2 (rwx) :					ORIGIN = 0x28004020, LENGTH = 32K-32
	NP_SSRAM (rwx) :						ORIGIN = 0x2800C020, LENGTH = 320K-32
	NP_SSRAM_NC (rwx) :						ORIGIN = 0x2805C000, LENGTH = 8K
	IPC_SSRAM (rwx) :						ORIGIN = 0x2805E000, LENGTH = 4K
	SHM_SSRAM (rwx) :						ORIGIN = 0x2805F000, LENGTH = 4K
	FP_SSRAM (rwx) :						ORIGIN = 0x28060020, LENGTH = 124K-32
	FP_SSRAM_MSP (rwx) :					ORIGIN = 0x2807F000, LENGTH = 4K
	
	/* SYSTEM SRAM SECURE */
	/****************************************************************************/
	SYS_SRAM_S (rwx) :                 		ORIGIN = 0x38000000, LENGTH = 512K
	/****************************************************************************/
	NP_SSRAM_ROMBSS_S  (rwx) :				ORIGIN = 0x38002000, LENGTH = 4K
	NP_SSRAM_MSP_S     (rwx) :				ORIGIN = 0x38003000, LENGTH = 4K
	NP_SSRAM_LOADER_S  (rwx) :				ORIGIN = 0x38004020, LENGTH = 32K-32
}

__np_ssram_nc_start__ = ORIGIN(NP_SSRAM_NC);
__np_ssram_nc_end__ = ORIGIN(NP_SSRAM_NC) + LENGTH(NP_SSRAM_NC);

__np_ssram_start__ = ORIGIN(NP_SSRAM);
__np_ssram_end__ = ORIGIN(NP_SSRAM) + LENGTH(NP_SSRAM);

__fp_ssram_start__ = ORIGIN(FP_SSRAM) - 32;
__fp_ssram_vector__ = __fp_ssram_start__ + 256;
__fp_ssram_end__ = ORIGIN(FP_SSRAM) + LENGTH(FP_SSRAM);

__fp_msp_start__ = ORIGIN(FP_SSRAM_MSP);
__fp_msp_end__ = ORIGIN(FP_SSRAM_MSP) + LENGTH(FP_SSRAM_MSP);

__ipc_ssram_start__ = ORIGIN(IPC_SSRAM);
__ipc_ssram_end__ = ORIGIN(IPC_SSRAM) + LENGTH(IPC_SSRAM);

__shm_ssram_start__ = ORIGIN(SHM_SSRAM);
__shm_ssram_end__ = ORIGIN(SHM_SSRAM) + LENGTH(SHM_SSRAM);

__np_ssram_romstk_start__ = ORIGIN(NP_SSRAM_MSP_S);
__np_ssram_romstk_end__ = ORIGIN(NP_SSRAM_MSP_S) + LENGTH(NP_SSRAM_MSP_S);
__np_ssram_romstk_lim__ = __np_ssram_romstk_start__;
__np_ssram_romstk_top__ = __np_ssram_romstk_end__ - 8;

__ap_psram_vector__ = 0x20000000;
__vp_psram_vector__ = 0x20000000;
__ap_ddr_ro_start__ = 0x80000000;
__ddr_end__ = 0x80000000 + 64M;

