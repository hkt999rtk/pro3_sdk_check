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
	NP_SSRAM_STK_NS (rwx) :					ORIGIN = 0x28000000, LENGTH = 8K
	NP_SSRAM_ROMSTK (rwx) :					ORIGIN = 0x28000000, LENGTH = 8K
	NP_SSRAM_ROMBSS (rwx) :					ORIGIN = 0x28002000, LENGTH = 8K
	NP_SSRAM_LOADER (rwx) :					ORIGIN = 0x28004020, LENGTH = 128K-32
	NP_SSRAM_NC (rwx) :						ORIGIN = 0x28024000, LENGTH = 32K
	NP_SSRAM (rwx) :						ORIGIN = 0x2802C000, LENGTH = 72K
	FP_SSRAM (rwx) :						ORIGIN = 0x2803E000, LENGTH = 256K
	IPC_SSRAM (rwx) :						ORIGIN = 0x2807E000, LENGTH = 4K
	SHM_SSRAM (rwx) :						ORIGIN = 0x2807F000, LENGTH = 4K
	
	
	/* PSRAM */
	/****************************************************************************/
	PSRAM (rwx) :                  			ORIGIN = 0x20000000, LENGTH = 4M 
	/****************************************************************************/
	NP_PSRAM_NSC (rwx):						ORIGIN = 0x20000000, LENGTH = 16K
	NP_PSRAM_S (rwx) :						ORIGIN = 0x20004000, LENGTH = 240K
	
	NP_PSRAM (rwx) :						ORIGIN = 0x20040000, LENGTH = 1024K+640K	
	VP_PSRAM (rwx) :						ORIGIN = 0x201E0000, LENGTH = 1024K+960K
	AP_PSRAM_NSBAN (rwx) :					ORIGIN = 0x203D0000, LENGTH = 128K
	AP_PSRAM2_NABAN (rwx) :					ORIGIN = 0x203F0000, LENGTH = 64K

	AP_PSRAM (rwx) :						ORIGIN = 0x303D0000, LENGTH = 128K
	AP_PSRAM2 (rwx) :						ORIGIN = 0x303F0000, LENGTH = 64K
	
	AP_DDR_RO (rwx) :						ORIGIN = 0x303D0000, LENGTH = 64K
	AP_DDR (rwx) :							ORIGIN = 0x303E0000, LENGTH = 64K
	AP_DDR_STK (rwx) :                 		ORIGIN = 0x303F0000, LENGTH = 64K

	/* VIDEO SRAM */
	/****************************************************************************/
	VSRAM (rwx) :                  		    ORIGIN = 0x6F000000, LENGTH = 192K
	/****************************************************************************/
	VP_VSRAM (rwx) :                  		ORIGIN = 0x6F000000, LENGTH = 192K
	TS_VSRAM (rwx) :                        ORIGIN = 0x6F000000, LENGTH = 48K
	COM_VSRAM (rwx) :                       ORIGIN = 0x6F00C000, LENGTH = 144K
	SLC_VSRAM (rwx) :                       ORIGIN = 0x6F000000, LENGTH = 192K

	
	/* DDR */
	/****************************************************************************/
	DDR (rwx) :                    			ORIGIN = 0x80000000, LENGTH = 128M
	/****************************************************************************/
	EN_DDR (rwx) :                    		ORIGIN = 0x80000000, LENGTH = 32M
	VP_DDR (rwx) :                    		ORIGIN = 0x82000000, LENGTH = 4M
	HEAP_DDR (rwx) :                    	ORIGIN = 0x82400000, LENGTH = 76M
	TG_DDR (rwx) :	                        ORIGIN = 0x87000000, LENGTH = 4M
	NN_DDR (rwx) :                    		ORIGIN = 0x87400000, LENGTH = 8M
	MDL_DDR (rwx) :                    		ORIGIN = 0x87C00000, LENGTH = 4M
}

__np_ssram_nc_start__ = ORIGIN(NP_SSRAM_NC);
__np_ssram_nc_end__ = ORIGIN(NP_SSRAM_NC) + LENGTH(NP_SSRAM_NC);

__vsram_start__ = ORIGIN(VSRAM);
__vsram_end__ = ORIGIN(VSRAM) + LENGTH(VSRAM);

__com_vsram_start__ = ORIGIN(COM_VSRAM);
__com_vsram_end__ = ORIGIN(COM_VSRAM) + LENGTH(COM_VSRAM);

__ts_vsram_start__ = ORIGIN(TS_VSRAM);
__ts_vsram_end__ = ORIGIN(TS_VSRAM) + LENGTH(TS_VSRAM);

__slice_vsram_start__ = ORIGIN(SLC_VSRAM);
__slice_vsram_end__ = ORIGIN(SLC_VSRAM) + LENGTH(SLC_VSRAM);

__np_ssram_ns_stk_lim__ = ORIGIN(NP_SSRAM_STK_NS);
__np_ssram_ns_stk_top__ = ORIGIN(NP_SSRAM_STK_NS) + LENGTH(NP_SSRAM_STK_NS) - 8;

__np_psram_nsc_start__ = ORIGIN(NP_PSRAM_NSC);
__np_psram_nsc_end__ = ORIGIN(NP_PSRAM_NSC) + LENGTH(NP_PSRAM_NSC);

__np_psram_start__ = ORIGIN(NP_PSRAM);
__np_psram_end__ = ORIGIN(NP_PSRAM) + LENGTH(NP_PSRAM);

__vp_psram_start__ = ORIGIN(VP_PSRAM);
__vp_psram_end__ = ORIGIN(VP_PSRAM) + LENGTH(VP_PSRAM);

__vp_ddr_start__ = ORIGIN(VP_DDR);
__vp_ddr_end__ = ORIGIN(VP_DDR) + LENGTH(VP_DDR);

__fp_ssram_start__ = ORIGIN(FP_SSRAM);
__fp_ssram_end__ = ORIGIN(FP_SSRAM) + LENGTH(FP_SSRAM);

__ap_psram_start__ = ORIGIN(AP_PSRAM);
__ap_psram_end__ = ORIGIN(AP_PSRAM) + LENGTH(AP_PSRAM);

__ipc_ssram_start__ = ORIGIN(IPC_SSRAM);
__ipc_ssram_end__ = ORIGIN(IPC_SSRAM) + LENGTH(IPC_SSRAM);

__shm_ssram_start__ = ORIGIN(SHM_SSRAM);
__shm_ssram_end__ = ORIGIN(SHM_SSRAM) + LENGTH(SHM_SSRAM);

__nn_ddr_start__ = ORIGIN(NN_DDR);
__nn_ddr_end__ = ORIGIN(NN_DDR) + LENGTH(NN_DDR);

__nn_mdl_start__ = ORIGIN(MDL_DDR);
__nn_mdl_end__ = ORIGIN(MDL_DDR) + LENGTH(MDL_DDR);

__en_ddr_start__ = ORIGIN(EN_DDR);
__en_ddr_end__ = ORIGIN(EN_DDR) + LENGTH(EN_DDR);

__heap_ddr_start__ = ORIGIN(HEAP_DDR);
__heap_ddr_end__ = ORIGIN(HEAP_DDR) + LENGTH(HEAP_DDR);

__ddr_start__ = ORIGIN(DDR);
__ddr_end__ = ORIGIN(DDR) + LENGTH(DDR);