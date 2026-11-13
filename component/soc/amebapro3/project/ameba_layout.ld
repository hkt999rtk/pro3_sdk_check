/*
 * @file ameba_layout.ld
 * @brief Memory Layout Linker Script Template
 *
 * This file is processed by GCC preprocessor to generate ameba_layout.ld
 * Usage: gcc -E -P -I. -Icomponent/soc/amebapro3/project ameba_layout.ld.S -o ameba_layout.ld
 *
 * The memory layout is configured via ameba_layout.h
 */

#include "ameba_layout.h"

MEMORY
{

	/* ASIC ITCM NP : 256K(ROM) 16K(RAM)  VP : 16K */
	NP_ITCM_ROM          (rwx) : ORIGIN = NP_ITCM_ROM_ORIGIN, LENGTH = NP_ITCM_ROM_LENGTH
	NP_ITCM              (rwx) : ORIGIN = NP_ITCM_ORIGIN, LENGTH = NP_ITCM_LENGTH
	VP_ITCM              (rwx) : ORIGIN = VP_ITCM_ORIGIN, LENGTH = VP_ITCM_LENGTH


	/* ASIC DTCM NP : 16K  VP : 16K */
	NP_DTCM              (rwx) : ORIGIN = NP_DTCM_ORIGIN, LENGTH = NP_DTCM_LENGTH
	NP_DTCM_BSS          (rwx) : ORIGIN = NP_DTCM_BSS_ORIGIN, LENGTH = NP_DTCM_BSS_LENGTH
	NP_DTCM_NOINIT       (rwx) : ORIGIN = NP_DTCM_NOINIT_ORIGIN, LENGTH = NP_DTCM_NOINIT_LENGTH
	NP_DTCM_RTT          (rwx) : ORIGIN = NP_DTCM_RTT_ORIGIN, LENGTH = NP_DTCM_RTT_LENGTH
	VP_DTCM              (rwx) : ORIGIN = VP_DTCM_ORIGIN, LENGTH = VP_DTCM_LENGTH

	/* SYSTEM SRAM */
	/****************************************************************************/
	SYS_SRAM             (rwx) : ORIGIN = SYS_SRAM_ORIGIN, LENGTH = SYS_SRAM_LENGTH
	/****************************************************************************/
	FP_SSRAM_INIT        (rwx) : ORIGIN = FP_SSRAM_INIT_ORIGIN, LENGTH = FP_SSRAM_INIT_LENGTH
	NP_SSRAM_MSP         (rwx) : ORIGIN = NP_SSRAM_MSP_ORIGIN, LENGTH = NP_SSRAM_MSP_LENGTH
	NP_SSRAM_ROMBSS      (rwx) : ORIGIN = NP_SSRAM_ROMBSS_ORIGIN, LENGTH = NP_SSRAM_ROMBSS_LENGTH
	NP_SSRAM_MAILBX      (rwx) : ORIGIN = NP_SSRAM_MAILBX_ORIGIN, LENGTH = NP_SSRAM_MAILBX_LENGTH

	/* only for verification, not for normal using */
	NP_SSRAM_VERIFY      (rwx) : ORIGIN = NP_SSRAM_VERIFY_ORIGIN, LENGTH = NP_SSRAM_VERIFY_LENGTH

	NP_SSRAM             (rwx) : ORIGIN = NP_SSRAM_ORIGIN, LENGTH = NP_SSRAM_LENGTH
	NP_SSRAM_NC          (rwx) : ORIGIN = NP_SSRAM_NC_ORIGIN, LENGTH = NP_SSRAM_NC_LENGTH
	AP_SSRAM_RET         (rwx) : ORIGIN = AP_SSRAM_RET_ORIGIN, LENGTH = AP_SSRAM_RET_LENGTH

	IPC_SSRAM            (rwx) : ORIGIN = IPC_SSRAM_ORIGIN, LENGTH = IPC_SSRAM_LENGTH
	SHM_SSRAM            (rwx) : ORIGIN = SHM_SSRAM_ORIGIN, LENGTH = SHM_SSRAM_LENGTH

	FP_SSRAM             (rwx) : ORIGIN = FP_SSRAM_ORIGIN, LENGTH = FP_SSRAM_LENGTH
	FP_SSRAM_HEAP_EXT    (rwx) : ORIGIN = FP_SSRAM_HEAP_EXT_ORIGIN, LENGTH = FP_SSRAM_HEAP_EXT_LENGTH
	FP_SSRAM_MSP         (rwx) : ORIGIN = FP_SSRAM_MSP_ORIGIN, LENGTH = FP_SSRAM_MSP_LENGTH

	/* LA mode option2 */
	/* FP_SSRAM           (rwx) : ORIGIN = 0x28040020, LENGTH = 252K-64K-32 */
	/* FP_SSRAM_MSP       (rwx) : ORIGIN = 0x2806F000, LENGTH = 4K */

	/* SYSTEM SRAM SECURE */
	/****************************************************************************/
	SYS_SRAM_S           (rwx) : ORIGIN = SYS_SRAM_S_ORIGIN, LENGTH = SYS_SRAM_S_LENGTH
	/****************************************************************************/
	NP_SSRAM_ROMBSS_S    (rwx) : ORIGIN = NP_SSRAM_ROMBSS_S_ORIGIN, LENGTH = NP_SSRAM_ROMBSS_S_LENGTH
	NP_SSRAM_MSP_S       (rwx) : ORIGIN = NP_SSRAM_MSP_S_ORIGIN, LENGTH = NP_SSRAM_MSP_S_LENGTH
	NP_SSRAM_LOADER_S    (rwx) : ORIGIN = NP_SSRAM_LOADER_S_ORIGIN, LENGTH = NP_SSRAM_LOADER_S_LENGTH
	NP_SSRAM_MPBSS_S     (rwx) : ORIGIN = NP_SSRAM_MPBSS_S_ORIGIN, LENGTH = NP_SSRAM_MPBSS_S_LENGTH


	/* BT SHARE RAM */
	/****************************************************************************/
	BT_SHRAM             (rwx) : ORIGIN = BT_SHRAM_ORIGIN, LENGTH = BT_SHRAM_LENGTH
	/****************************************************************************/

	/* PSRAM */
	/****************************************************************************/
	PSRAM                (rwx) : ORIGIN = PSRAM_ORIGIN, LENGTH = PSRAM_LENGTH
	/****************************************************************************/
	NP_PSRAM             (rwx) : ORIGIN = NP_PSRAM_ORIGIN, LENGTH = NP_PSRAM_LENGTH
	NP_PSRAM_HEAP_EXT    (rwx) : ORIGIN = NP_PSRAM_HEAP_EXT_ORIGIN, LENGTH = NP_PSRAM_HEAP_EXT_LENGTH
	/* LA mode IQ dump use this, option1 */
	/* NP_PSRAM           (rwx) : ORIGIN = 0x20080020, LENGTH = 2M - 512K - 32 */
	VP_PSRAM             (rwx) : ORIGIN = VP_PSRAM_ORIGIN, LENGTH = VP_PSRAM_LENGTH
	AP_PSRAM_RET         (rwx) : ORIGIN = AP_PSRAM_RET_ORIGIN, LENGTH = AP_PSRAM_RET_LENGTH
	AP_PSRAM_FCS_RET     (rwx) : ORIGIN = AP_PSRAM_RET_ORIGIN + AP_PSRAM_RET_LENGTH - AP_PSRAM_RET_FCS_LENGTH, LENGTH = AP_PSRAM_RET_FCS_LENGTH
	VP_PSRAM_NN          (rwx) : ORIGIN = VP_PSRAM_NN_ORIGIN, LENGTH = VP_PSRAM_NN_LENGTH

	/* PSRAM SECURE */
	/****************************************************************************/
	PSRAM_S              (rwx) : ORIGIN = PSRAM_S_ORIGIN, LENGTH = PSRAM_S_LENGTH
	/****************************************************************************/
#if defined(CONFIG_TRUSTZONE) && (CONFIG_TRUSTZONE == 1)
	NP_PSRAM_NSC         (rwx) : ORIGIN = NP_PSRAM_NSC_ORIGIN, LENGTH = NP_PSRAM_NSC_LENGTH
	NP_PSRAM_S           (rwx) : ORIGIN = NP_PSRAM_S_ORIGIN, LENGTH = NP_PSRAM_S_LENGTH
#endif

	AP_PSRAM_BL1         (rwx) : ORIGIN = AP_PSRAM_BL1_ORIGIN, LENGTH = AP_PSRAM_BL1_LENGTH
	AP_PSRAM_SHARE       (rwx) : ORIGIN = AP_PSRAM_SHARE_ORIGIN, LENGTH = AP_PSRAM_SHARE_LENGTH

	/* VIDEO SRAM */
	/****************************************************************************/
	VSRAM                (rwx) : ORIGIN = VSRAM_ORIGIN, LENGTH = VSRAM_LENGTH
	/****************************************************************************/
	VP_VSRAM             (rwx) : ORIGIN = VP_VSRAM_ORIGIN, LENGTH = VP_VSRAM_LENGTH
	TS_VSRAM             (rwx) : ORIGIN = TS_VSRAM_ORIGIN, LENGTH = TS_VSRAM_LENGTH
	COM_VSRAM            (rwx) : ORIGIN = COM_VSRAM_ORIGIN, LENGTH = COM_VSRAM_LENGTH
	SLC_VSRAM            (rwx) : ORIGIN = SLC_VSRAM_ORIGIN, LENGTH = SLC_VSRAM_LENGTH

	/* DDR */
	/****************************************************************************/
	DDR                  (rwx) : ORIGIN = DDR_ORIGIN, LENGTH = DDR_LENGTH
	/****************************************************************************/
	AP_DDR_RO            (rwx) : ORIGIN = AP_DDR_RO_ORIGIN, LENGTH = AP_DDR_RO_LENGTH
	AP_DDR               (rwx) : ORIGIN = AP_DDR_ORIGIN, LENGTH = AP_DDR_LENGTH
	AP_DDR_HEAP_EXT      (rwx) : ORIGIN = AP_DDR_HEAP_EXT_ORIGIN, LENGTH = AP_DDR_HEAP_EXT_LENGTH
	AP_DDR_STK           (rwx) : ORIGIN = AP_DDR_STK_ORIGIN, LENGTH = AP_DDR_STK_LENGTH
	VP_DDR               (rwx) : ORIGIN = VP_DDR_ORIGIN, LENGTH = VP_DDR_LENGTH
	VP_DDR_HEAP_EXT      (rwx) : ORIGIN = VP_DDR_HEAP_EXT_ORIGIN, LENGTH = VP_DDR_HEAP_EXT_LENGTH
	NN_DDR               (rwx) : ORIGIN = NN_DDR_ORIGIN, LENGTH = NN_DDR_LENGTH
	/* DDR SECURE */
	/****************************************************************************/
	DDR_S                (rwx) : ORIGIN = DDR_S_ORIGIN, LENGTH = DDR_S_LENGTH
	/****************************************************************************/
	AP_DDR_BL2_32        (rwx) : ORIGIN = AP_DDR_BL2_32_ORIGIN, LENGTH = AP_DDR_BL2_32_LENGTH
	AP_DDR_OPTEE         (rwx) : ORIGIN = AP_DDR_OPTEE_ORIGIN, LENGTH = AP_DDR_OPTEE_LENGTH
	AP_DDR_SHM           (rwx) : ORIGIN = AP_DDR_SHM_ORIGIN, LENGTH = AP_DDR_SHM_LENGTH
	AP_DDR_OPTEE_SHM     (rwx) : ORIGIN = AP_DDR_OPTEE_SHM_ORIGIN, LENGTH = AP_DDR_OPTEE_SHM_LENGTH
	AP_DDR_FIP           (rwx) : ORIGIN = AP_DDR_FIP_ORIGIN, LENGTH = AP_DDR_FIP_LENGTH


	/* XIP */
	/****************************************************************************/
	XIP                  (rwx) : ORIGIN = XIP_ORIGIN, LENGTH = XIP_LENGTH
	/****************************************************************************/

	/* XIP REMAP */
	/****************************************************************************/
	XIP_REMAP            (rwx) : ORIGIN = XIP_REMAP_ORIGIN, LENGTH = XIP_REMAP_LENGTH
	/****************************************************************************/
	KM4_BOOT             (rwx) : ORIGIN = 0x0C000020, LENGTH = 0x00800000 - 0x20
	KM4NP_IMG2           (rwx) : ORIGIN = 0x0C000020, LENGTH = 0x00700000 - 0x20
	KM0_IMG2             (rwx) : ORIGIN = 0x0C700020, LENGTH = 0x00100000 - 0x20
	KM4VP_IMG2           (rwx) : ORIGIN = 0x0C800020, LENGTH = 0x00800000 - 0x20
	CA32_IMG2            (rwx) : ORIGIN = 0x0D000020, LENGTH = 0x01000000 - 0x20
	KM4NP_IMG3           (rwx) : ORIGIN = 0x0E000020, LENGTH = 0x00300000 - 0x20

	BTRACE               (rx)  : ORIGIN = BTRACE_ORIGIN, LENGTH = BTRACE_LENGTH
	CTRACE               (rx)  : ORIGIN = CTRACE_ORIGIN, LENGTH = CTRACE_LENGTH
}

__np_dtcm_bss_start__ = ORIGIN(NP_DTCM_BSS);
__np_dtcm_bss_end__ = ORIGIN(NP_DTCM_BSS) + LENGTH(NP_DTCM_BSS);

/* MPC NS SSRAM start -------------> */
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

__np_ssram_mbx_start = ORIGIN(NP_SSRAM_MAILBX);
__np_ssram_mbx_end__ = ORIGIN(NP_SSRAM_MAILBX) + LENGTH(NP_SSRAM_MAILBX);

/* MPC NS SSRAM end   <------------- */

__np_ssram_loader_s_start__ = ORIGIN(NP_SSRAM_LOADER_S) - 32;
__np_ssram_loader_s_end__ = ORIGIN(NP_SSRAM_LOADER_S) + LENGTH(NP_SSRAM_LOADER_S);

__np_ssram_verify_start__ = ORIGIN(NP_SSRAM_VERIFY);
__np_ssram_verify_end__ = ORIGIN(NP_SSRAM_VERIFY) + LENGTH(NP_SSRAM_VERIFY);

/* MPC NS SSRAM/BTSHRAM start -------------> */
__np_ssram_start__ = ORIGIN(NP_SSRAM);
__np_ssram_end__ = ORIGIN(NP_SSRAM) + LENGTH(NP_SSRAM);

__np_ssram_nc_start__ = ORIGIN(NP_SSRAM_NC);
__np_ssram_nc_end__ = ORIGIN(NP_SSRAM_NC) + LENGTH(NP_SSRAM_NC);

__ap_ssram_ret_start__ = ORIGIN(AP_SSRAM_RET);
__ap_ssram_ret_end__ = ORIGIN(AP_SSRAM_RET) + LENGTH(AP_SSRAM_RET);

__ipc_ssram_start__ = ORIGIN(IPC_SSRAM);
__ipc_ssram_end__ = ORIGIN(IPC_SSRAM) + LENGTH(IPC_SSRAM);

__shm_ssram_start__ = ORIGIN(SHM_SSRAM);
__shm_ssram_end__ = ORIGIN(SHM_SSRAM) + LENGTH(SHM_SSRAM);

__fp_ssram_start__ = ORIGIN(FP_SSRAM) - 32;
__fp_ssram_vector__ = __fp_ssram_start__ + 256;
__fp_ssram_end__ = ORIGIN(FP_SSRAM) + LENGTH(FP_SSRAM);
/* FP is in WFI and do nothing during ROM stage, use this section for PQC stack 64KB */
__np_ssram_pqc_rom_msp_top__ = __fp_ssram_end__;

__fp_msp_start__ = ORIGIN(FP_SSRAM_MSP);
__fp_msp_end__ = ORIGIN(FP_SSRAM_MSP) + LENGTH(FP_SSRAM_MSP);

__ssram_start__ = ORIGIN(SYS_SRAM);
__ssram_end__ = ORIGIN(SYS_SRAM) + LENGTH(SYS_SRAM);

__bt_shram_start__ = ORIGIN(BT_SHRAM);
__bt_shram_end__ = ORIGIN(BT_SHRAM) + LENGTH(BT_SHRAM);

/* MPC NS SSRAM/BTSHRAM end   <------------- */

__np_nand_boot_temp__ = ORIGIN(SYS_SRAM) + LENGTH(SYS_SRAM) - 128K;

__vsram_start__ = ORIGIN(VSRAM);
__vsram_end__ = ORIGIN(VSRAM) + LENGTH(VSRAM);

__com_vsram_start__ = ORIGIN(COM_VSRAM);
__com_vsram_end__ = ORIGIN(COM_VSRAM) + LENGTH(COM_VSRAM);

__ts_vsram_start__ = ORIGIN(TS_VSRAM);
__ts_vsram_end__ = ORIGIN(TS_VSRAM) + LENGTH(TS_VSRAM);

__slice_vsram_start__ = ORIGIN(SLC_VSRAM);
__slice_vsram_end__ = ORIGIN(SLC_VSRAM) + LENGTH(SLC_VSRAM);

__np_psram_start__ = ORIGIN(NP_PSRAM);
__np_psram_end__ = ORIGIN(NP_PSRAM) + LENGTH(NP_PSRAM);

#if defined(CONFIG_TRUSTZONE) && (CONFIG_TRUSTZONE == 1)
/* MPC NS PSRAM start -------------> */
__np_psram_nsc_start__ = ORIGIN(NP_PSRAM_NSC);
__np_psram_nsc_end__ = ORIGIN(NP_PSRAM_NSC) + LENGTH(NP_PSRAM_NSC);
/* MPC NS PSRAM end   <------------- */
#endif

/* MPC NS PSRAM start -------------> */
__vp_psram_start__ = ORIGIN(VP_PSRAM) - 32;
__vp_psram_vector__ = __vp_psram_start__ + 256;
__vp_psram_end__ = ORIGIN(VP_PSRAM) + LENGTH(VP_PSRAM);

__ap_psram_ret_start__ = ORIGIN(AP_PSRAM_RET);
__ap_psram_ret_end__ = ORIGIN(AP_PSRAM_RET) + LENGTH(AP_PSRAM_RET);

__vp_psram_nn_start__ = ORIGIN(VP_PSRAM_NN);
__vp_psram_nn_end__ = ORIGIN(VP_PSRAM_NN) + LENGTH(VP_PSRAM_NN);

__psram_start__ = ORIGIN(PSRAM);
__psram_end__ = ORIGIN(PSRAM) + LENGTH(PSRAM);

/* MPC NS PSRAM end   <------------- */

__ap_psram_bl1_start__ = ORIGIN(AP_PSRAM_BL1);
__ap_psram_vector__ = __ap_psram_bl1_start__ + 32;
__ap_psram_bl1_end__ = ORIGIN(AP_PSRAM_BL1) + LENGTH(AP_PSRAM_BL1);

__ap_ddr_ro_start__ = ORIGIN(AP_DDR_RO);
__ap_ddr_ro_end__ = ORIGIN(AP_DDR_RO) + LENGTH(AP_DDR_RO);

__ap_ddr_start__ = ORIGIN(AP_DDR);
__ap_ddr_end__ = ORIGIN(AP_DDR) + LENGTH(AP_DDR);

__vp_ddr_start__ = ORIGIN(VP_DDR);
__vp_ddr_end__ = ORIGIN(VP_DDR) + LENGTH(VP_DDR);

__nn_ddr_start__ = ORIGIN(NN_DDR);
__nn_ddr_end__ = ORIGIN(NN_DDR) + LENGTH(NN_DDR);

__ddr_start__ = ORIGIN(DDR);
__ddr_end__ = ORIGIN(DDR) + LENGTH(DDR);

__km4np_img2_start__ = ORIGIN(KM4NP_IMG2);
__km4np_img2_end__ = ORIGIN(KM4NP_IMG2) + LENGTH(KM4NP_IMG2);
__km4np_img3_start__ = ORIGIN(KM4NP_IMG3);
__km4np_img3_end__ = ORIGIN(KM4NP_IMG3) + LENGTH(KM4NP_IMG3);
__km0_img2_start__ = ORIGIN(KM0_IMG2);
__km0_img2_end__ = ORIGIN(KM0_IMG2) + LENGTH(KM0_IMG2);
__km4vp_img2_start__ = ORIGIN(KM4VP_IMG2);
__km4vp_img2_end__ = ORIGIN(KM4VP_IMG2) + LENGTH(KM4VP_IMG2);
__ca32_img2_start__ = ORIGIN(CA32_IMG2);
__ca32_img2_end__ = ORIGIN(CA32_IMG2) + LENGTH(CA32_IMG2);