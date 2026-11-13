MEMORY
{

	/* FPGA ITCM NP : 512K  VP : 16K */
	AP_REMAP (rwx) :                        ORIGIN = 0x00000000, LENGTH = 256K
	NP_ITCM (rwx) :                         ORIGIN = 0x00000000, LENGTH = 512K
	VP_ITCM (rwx) :                         ORIGIN = 0x6F080000, LENGTH = 16K
	

	/* FPGA DTCM NP : 64K  VP : 32K */
	NP_DTCM (rwx) :                         ORIGIN = 0x00100000, LENGTH = 64K	
	VP_DTCM (rwx) :                         ORIGIN = 0x6F090000, LENGTH = 32K	

	/* SYSTEM SRAM */
	/****************************************************************************/
	SYS_SRAM (rwx) :                 		ORIGIN = 0x28000000, LENGTH = 512K
	/****************************************************************************/
	NP_SSRAM_NC (rwx) :						ORIGIN = 0x28000000, LENGTH = 16K
	NP_SSRAM (rwx) :						ORIGIN = 0x28004000, LENGTH = 342K
	FP_SSRAM (rwx) :						ORIGIN = 0x28059800, LENGTH = 146K
	IPC_SSRAM (rwx) :						ORIGIN = 0x2807E000, LENGTH = 4K
	SHM_SSRAM (rwx) :						ORIGIN = 0x2807F000, LENGTH = 4K
	
	
	/* PSRAM */
	/****************************************************************************/
	PSRAM (rwx) :                  			ORIGIN = 0x20000000, LENGTH = 4M 
	/****************************************************************************/

	/* VIDEO SRAM */
	/****************************************************************************/
	VSRAM (rwx) :                  		    ORIGIN = 0x6F000000, LENGTH = 192K
	/****************************************************************************/
	
	/* DDR */
	/****************************************************************************/
	DDR (rwx) :                    			ORIGIN = 0x80000000, LENGTH = 64M
	/****************************************************************************/
	NN_DDR (rwx) :                    		ORIGIN = 0x84000000, LENGTH = 16M
	EN_DDR (rwx) :                    		ORIGIN = 0x85000000, LENGTH = 32M
}

__np_ssram_nc_start__ = ORIGIN(NP_SSRAM_NC);
__np_ssram_nc_end__ = ORIGIN(NP_SSRAM_NC) + LENGTH(NP_SSRAM_NC);

__fp_ssram_start__ = ORIGIN(FP_SSRAM);
__fp_ssram_end__ = ORIGIN(FP_SSRAM) + LENGTH(FP_SSRAM);

__ipc_ssram_start__ = ORIGIN(IPC_SSRAM);
__ipc_ssram_end__ = ORIGIN(IPC_SSRAM) + LENGTH(IPC_SSRAM);

__shm_ssram_start__ = ORIGIN(SHM_SSRAM);
__shm_ssram_end__ = ORIGIN(SHM_SSRAM) + LENGTH(SHM_SSRAM);

__nn_ddr_start__ = ORIGIN(NN_DDR);
__nn_ddr_end__ = ORIGIN(NN_DDR) + LENGTH(NN_DDR);

__en_ddr_start__ = ORIGIN(EN_DDR);
__en_ddr_end__ = ORIGIN(EN_DDR) + LENGTH(EN_DDR);
