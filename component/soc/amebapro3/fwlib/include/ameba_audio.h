/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _AMEBA_AUDIO_H_
#define _AMEBA_AUDIO_H_

/* AUTO_GEN_START */


/** @addtogroup Ameba_Periph_Driver
* @{
*/
/** @defgroup AUDIO_CODEC
* @brief AUDIO_CODEC driver modules
* @{
*/


/** @defgroup audio_control_0
 * @brief
 * @{
 */
#define AUDIO_CODEC_BIT_AUDIO_IP_EN                 ((u32)0x00000001 << 0)                   /*!< R/W     0 nan */
#define AUDIO_CODEC_BIT_DAAD_LPBK_EN                ((u32)0x00000001 << 1)                   /*!< R/W     0 Digital DAC --> ADC loop back control1'b0: Disable1'b1: Enable */
#define AUDIO_CODEC_BIT_ADDA_LPBK_EN                ((u32)0x00000001 << 2)                   /*!< R/W     0 Digital ADC --> DAC loop back control1'b0: Disable1'b1: Enable */
#define AUDIO_CODEC_BIT_AUDIO_CONTROL_0_DUMMY       ((u32)0x00000001 << 3)                   /*!< R/W     0 DUMMY */
#define AUDIO_CODEC_BIT_RWS_SEQ_EN                  ((u32)0x00000001 << 6)                   /*!< R/W     0 rws_seq_en[0]: 0:disable, 1:enable */
#define AUDIO_CODEC_MASK_AUDIO_DBG_SEL              ((u32)0x0000001F << 13)                  /*!< R/W     0 Debug probe selcetion:5'd0:da_L_dbg_bus_a5'd1:da_L_dbg_bus_b5'd2:da_L_dbg_bus_c5'd6:ad_0_dbg_bus_a5'd7:ad_0_dbg_bus_b5'd8:ad_0_dbg_bus_c5'd9:ad_0_dbg_bus_d5'd10:ad_1_dbg_bus_a5'd11:ad_1_dbg_bus_b5'd12:ad_1_dbg_bus_c5'd13:ad_1_dbg_bus_d5'd14:ad_2_dbg_bus_a5'd15:ad_2_dbg_bus_b5'd16:ad_2_dbg_bus_c5'd17:ad_2_dbg_bus_d5'd22:debug_i2s_0_rx5'd23:debug_i2s_0_tx5'd26:debug_bus_a5'd27:debug_bus_bother:reserved */
#define AUDIO_CODEC_AUDIO_DBG_SEL(x)                ((u32)(((x) & 0x0000001F) << 13))
#define AUDIO_CODEC_GET_AUDIO_DBG_SEL(x)            ((u32)(((x >> 13) & 0x0000001F)))
/** @} */


/** @defgroup audio_control_1
 * @brief
 * @{
 */
#define AUDIO_CODEC_MASK_SEL_BB_CK_DEPOP            ((u32)0x00000003 << 0)                   /*!< R/W     1 sel_bb_ck_depop:2'b00:40MHz/10242'b01:40MHz/20482'b10:40MHz/40962'b11:40MHz/8192 */
#define AUDIO_CODEC_SEL_BB_CK_DEPOP(x)              ((u32)(((x) & 0x00000003) << 0))
#define AUDIO_CODEC_GET_SEL_BB_CK_DEPOP(x)          ((u32)(((x >> 0) & 0x00000003)))
#define AUDIO_CODEC_BIT_BB_CK_DEPOP_EN              ((u32)0x00000001 << 2)                   /*!< R/W     0 bb_ck_depop_en */
#define AUDIO_CODEC_BIT_CKX_MICBIAS_EN              ((u32)0x00000001 << 3)                   /*!< R/W     0 ckx_micbias_en */
#define AUDIO_CODEC_BIT_PDM_CLK_DOUBLE              ((u32)0x00000001 << 4)                   /*!< R/W     0 pdm_clk_double */
#define AUDIO_CODEC_MASK_PDM_CLK_SEL                ((u32)0x00000003 << 5)                   /*!< R/W     0 PDM clock selection2'b0: 5MHz2'b1: 2.5MHz2'b2: 6.67MHz */
#define AUDIO_CODEC_PDM_CLK_SEL(x)                  ((u32)(((x) & 0x00000003) << 5))
#define AUDIO_CODEC_GET_PDM_CLK_SEL(x)              ((u32)(((x >> 5) & 0x00000003)))
#define AUDIO_CODEC_MASK_PDM_CH_SWAP                ((u32)0x00000003 << 7)                   /*!< R/W   0x2 PDM DATA channel SWAP:2'b00: L/L2'b01: L/R2'b10: R/L2'b11: R/R */
#define AUDIO_CODEC_PDM_CH_SWAP(x)                  ((u32)(((x) & 0x00000003) << 7))
#define AUDIO_CODEC_GET_PDM_CH_SWAP(x)              ((u32)(((x >> 7) & 0x00000003)))
#define AUDIO_CODEC_BIT_PDM_DATA_PHASE_SEL          ((u32)0x00000001 << 9)                   /*!< R/W     1 pdm_data_phase_sel:1: add delay in PDM DATA  */
#define AUDIO_CODEC_BIT_PDM_GAIN_SHIFT_EN           ((u32)0x00000001 << 10)                  /*!< R/W     1 pdm_gain_shift_en:1: backoff 6 dB to prevent overloading for PDM */
#define AUDIO_CODEC_BIT_PDM_CLK_INV_SEL             ((u32)0x00000001 << 11)                  /*!< R/W     0 pdm_clk_inv_sel:1: pdm latch L/R clock with falling edge */
#define AUDIO_CODEC_BIT_I2S_DATA_RND_EN             ((u32)0x00000001 << 12)                  /*!< R/W     1 i2s_data_rnd_en:1: i2s data with rounding  */
#define AUDIO_CODEC_MASK_AUDIO_CONTROL_1_DUMMY      ((u32)0x0000000F << 13)                  /*!< R/W     0 DUMMY */
#define AUDIO_CODEC_AUDIO_CONTROL_1_DUMMY(x)        ((u32)(((x) & 0x0000000F) << 13))
#define AUDIO_CODEC_GET_AUDIO_CONTROL_1_DUMMY(x)    ((u32)(((x >> 13) & 0x0000000F)))
/** @} */


/** @defgroup clock_control_1
 * @brief
 * @{
 */
#define AUDIO_CODEC_BIT_DA_L_EN                     ((u32)0x00000001 << 0)                   /*!< R/W     0 DAC l channel clock enable */
#define AUDIO_CODEC_BIT_CLOCK_CONTROL_1_DUMMY0      ((u32)0x00000001 << 1)                   /*!< R/W     0 DUMMY */
#define AUDIO_CODEC_BIT_MOD_L_EN                    ((u32)0x00000001 << 2)                   /*!< R/W     0 SDM l channel clock enable */
#define AUDIO_CODEC_BIT_CLOCK_CONTROL_1_DUMMY1      ((u32)0x00000001 << 3)                   /*!< R/W     0 DUMMY */
#define AUDIO_CODEC_BIT_DA_ANA_CLK_EN               ((u32)0x00000001 << 4)                   /*!< R/W     0 DAC/ADC analog clock enable */
#define AUDIO_CODEC_BIT_DA_FIFO_EN                  ((u32)0x00000001 << 5)                   /*!< R/W     0 DAC FIFO clock enable */
#define AUDIO_CODEC_BIT_AD_0_EN                     ((u32)0x00000001 << 6)                   /*!< R/W     0 ADC channel 0 clock enable */
#define AUDIO_CODEC_BIT_AD_1_EN                     ((u32)0x00000001 << 7)                   /*!< R/W     0 ADC channel 1 clock enable */
#define AUDIO_CODEC_BIT_AD_2_EN                     ((u32)0x00000001 << 8)                   /*!< R/W     0 ADC channel 2 clock enable */
#define AUDIO_CODEC_BIT_AD_0_FIFO_EN                ((u32)0x00000001 << 14)                  /*!< R/W     0 ADC channel 0 FIFO clock enable */
#define AUDIO_CODEC_BIT_AD_1_FIFO_EN                ((u32)0x00000001 << 15)                  /*!< R/W     0 ADC channel 1 FIFO clock enable */
#define AUDIO_CODEC_BIT_AD_2_FIFO_EN                ((u32)0x00000001 << 16)                  /*!< R/W     0 ADC channel 2 FIFO clock enable */
#define AUDIO_CODEC_BIT_AD_ANA_CLK_EN               ((u32)0x00000001 << 22)                  /*!< R/W     0 ADC analog clock enable */
/** @} */


/** @defgroup clock_control_2
 * @brief
 * @{
 */
#define AUDIO_CODEC_BIT_AD_ANA_0_EN                 ((u32)0x00000001 << 0)                   /*!< R/W     0 ADC filter channel 0 clock enable: analog ADC path */
#define AUDIO_CODEC_BIT_AD_ANA_1_EN                 ((u32)0x00000001 << 1)                   /*!< R/W     0 ADC filter channel 1 clock enable: analog ADC path */
#define AUDIO_CODEC_BIT_AD_ANA_2_EN                 ((u32)0x00000001 << 2)                   /*!< R/W     0 ADC filter channel 2 clock enable: analog ADC path */
#define AUDIO_CODEC_BIT_DMIC_0_EN                   ((u32)0x00000001 << 8)                   /*!< R/W     0 ADC filter channel 0 clock enable: dmic path */
#define AUDIO_CODEC_BIT_DMIC_1_EN                   ((u32)0x00000001 << 9)                   /*!< R/W     0 ADC filter channel 1 clock enable: dmic path */
#define AUDIO_CODEC_BIT_DMIC_2_EN                   ((u32)0x00000001 << 10)                  /*!< R/W     0 ADC filter channel 2 clock enable: dmic path */
#define AUDIO_CODEC_BIT_AD_0_EQ_EN                  ((u32)0x00000001 << 16)                  /*!< R/W     0 adc channel 0 EQ clock en */
#define AUDIO_CODEC_BIT_AD_1_EQ_EN                  ((u32)0x00000001 << 17)                  /*!< R/W     0 adc channel 1 EQ clock en */
#define AUDIO_CODEC_BIT_AD_2_EQ_EN                  ((u32)0x00000001 << 18)                  /*!< R/W     0 adc channel 2 EQ clock en */
#define AUDIO_CODEC_BIT_DA_L_EQ_EN                  ((u32)0x00000001 << 20)                  /*!< R/W     0 dac channel L EQ clock en */
/** @} */


/** @defgroup clock_control_3
 * @brief
 * @{
 */
#define AUDIO_CODEC_MASK_DMIC1_CLK_SEL              ((u32)0x00000007 << 0)                   /*!< R/W     1 Set clock of digital microphone3'b000: 5MHz3'b001: 2.5MHz3'b010: 1.25MHz3'b011: 625kHz3'b100: 312.5kHz3'b101: Reseved3'b110: Reserved3'b111: 769.2kHz */
#define AUDIO_CODEC_DMIC1_CLK_SEL(x)                ((u32)(((x) & 0x00000007) << 0))
#define AUDIO_CODEC_GET_DMIC1_CLK_SEL(x)            ((u32)(((x >> 0) & 0x00000007)))
#define AUDIO_CODEC_BIT_DMIC1_CLK_EN                ((u32)0x00000001 << 3)                   /*!< R/W     0 Digital microphone clock enable */
/** @} */


/** @defgroup clock_control_4
 * @brief
 * @{
 */
#define AUDIO_CODEC_MASK_SAMPLE_RATE_0              ((u32)0x0000000F << 0)                   /*!< R/W     0 Set sample rate source 04'h0: 48K4'h1: 96K4'h2: 192K4'h3: 32K4'h4: 176.4K4'h5: 16K4'h6: Reserved4'h7: 8K4'h8: 44.1K4'h9: 88.2K4'ha:24K4'hb: 12K4'hc: 22.05K4'hd: 11.025K4'he~4'hf: Reserved */
#define AUDIO_CODEC_SAMPLE_RATE_0(x)                ((u32)(((x) & 0x0000000F) << 0))
#define AUDIO_CODEC_GET_SAMPLE_RATE_0(x)            ((u32)(((x >> 0) & 0x0000000F)))
#define AUDIO_CODEC_MASK_SAMPLE_RATE_1              ((u32)0x0000000F << 4)                   /*!< R/W     0 Set sample rate source 14'h0: 48K4'h1: 96K4'h2: 192K4'h3: 32K4'h4: 176.4K4'h5: 16K4'h6: Reserved4'h7: 8K4'h8: 44.1K4'h9: 88.2K4'ha:24K4'hb: 12K4'hc: 22.05K4'hd: 11.025K4'he~4'hf: Reserved */
#define AUDIO_CODEC_SAMPLE_RATE_1(x)                ((u32)(((x) & 0x0000000F) << 4))
#define AUDIO_CODEC_GET_SAMPLE_RATE_1(x)            ((u32)(((x >> 4) & 0x0000000F)))
#define AUDIO_CODEC_MASK_SAMPLE_RATE_2              ((u32)0x0000000F << 8)                   /*!< R/W     0 Set sample rate source 24'h0: 48K4'h1: 96K4'h2: 192K4'h3: 32K4'h4: 176.4K4'h5: 16K4'h6: Reserved4'h7: 8K4'h8: 44.1K4'h9: 88.2K4'ha:24K4'hb: 12K4'hc: 22.05K4'hd: 11.025K4'he~4'hf: Reserved */
#define AUDIO_CODEC_SAMPLE_RATE_2(x)                ((u32)(((x) & 0x0000000F) << 8))
#define AUDIO_CODEC_GET_SAMPLE_RATE_2(x)            ((u32)(((x >> 8) & 0x0000000F)))
#define AUDIO_CODEC_MASK_SAMPLE_RATE_3              ((u32)0x0000000F << 12)                  /*!< R/W     0 Set sample rate source 34'h0: 48K4'h1: 96K4'h2: 192K4'h3: 32K4'h4: 176.4K4'h5: 16K4'h6: Reserved4'h7: 8K4'h8: 44.1K4'h9: 88.2K4'ha:24K4'hb: 12K4'hc: 22.05K4'hd: 11.025K4'he~4'hf: Reserved */
#define AUDIO_CODEC_SAMPLE_RATE_3(x)                ((u32)(((x) & 0x0000000F) << 12))
#define AUDIO_CODEC_GET_SAMPLE_RATE_3(x)            ((u32)(((x >> 12) & 0x0000000F)))
/** @} */


/** @defgroup clock_control_5
 * @brief
 * @{
 */
#define AUDIO_CODEC_MASK_ADC_0_FS_SRC_SEL           ((u32)0x00000003 << 0)                   /*!< R/W     0 Channel 0 ADC path sample rate Source selection2'b00: Source 02'b01: Source 12'b10: Source 22'b11: Reserved */
#define AUDIO_CODEC_ADC_0_FS_SRC_SEL(x)             ((u32)(((x) & 0x00000003) << 0))
#define AUDIO_CODEC_GET_ADC_0_FS_SRC_SEL(x)         ((u32)(((x >> 0) & 0x00000003)))
#define AUDIO_CODEC_MASK_ADC_1_FS_SRC_SEL           ((u32)0x00000003 << 2)                   /*!< R/W     0 Channel 1 ADC path sample rate Source selection2'b00: Source 02'b01: Source 12'b10: Source 22'b11: Reserved */
#define AUDIO_CODEC_ADC_1_FS_SRC_SEL(x)             ((u32)(((x) & 0x00000003) << 2))
#define AUDIO_CODEC_GET_ADC_1_FS_SRC_SEL(x)         ((u32)(((x >> 2) & 0x00000003)))
#define AUDIO_CODEC_MASK_ADC_2_FS_SRC_SEL           ((u32)0x00000003 << 4)                   /*!< R/W     0 Channel 2 ADC path sample rate Source selection2'b00: Source 02'b01: Source 12'b10: Source 22'b11: Reserved */
#define AUDIO_CODEC_ADC_2_FS_SRC_SEL(x)             ((u32)(((x) & 0x00000003) << 4))
#define AUDIO_CODEC_GET_ADC_2_FS_SRC_SEL(x)         ((u32)(((x >> 4) & 0x00000003)))
#define AUDIO_CODEC_MASK_DAC_L_FS_SRC_SEL           ((u32)0x00000003 << 16)                  /*!< R/W     0 DAC path sample rate Source selection2'b00: Source 02'b01: Source 12'b10: Source 22'b11: Reserved */
#define AUDIO_CODEC_DAC_L_FS_SRC_SEL(x)             ((u32)(((x) & 0x00000003) << 16))
#define AUDIO_CODEC_GET_DAC_L_FS_SRC_SEL(x)         ((u32)(((x >> 16) & 0x00000003)))
/** @} */


/** @defgroup clock_control_6
 * @brief
 * @{
 */
#define AUDIO_CODEC_BIT_ADC_0_ASRC_EN               ((u32)0x00000001 << 0)                   /*!< R/W     0 Channel 0 ADC path ASRC enable0: Disable1: EnableIf ASRC is enabled, sample_rate becomes useless. */
#define AUDIO_CODEC_BIT_ADC_1_ASRC_EN               ((u32)0x00000001 << 1)                   /*!< R/W     0 Channel 1 ADC path ASRC enable0: Disable1: EnableIf ASRC is enabled, sample_rate becomes useless. */
#define AUDIO_CODEC_BIT_ADC_2_ASRC_EN               ((u32)0x00000001 << 2)                   /*!< R/W     0 Channel 2 ADC path ASRC enable0: Disable1: EnableIf ASRC is enabled, sample_rate becomes useless. */
#define AUDIO_CODEC_BIT_DAC_L_ASRC_EN               ((u32)0x00000001 << 8)                   /*!< R/W     0 DAC path ASRC enable0: Disable1: EnableIf ASRC is enabled, sample_rate becomes useless. */
/** @} */


/** @defgroup clock_control_7
 * @brief
 * @{
 */
#define AUDIO_CODEC_BIT_DA_ANA_CLK_SEL              ((u32)0x00000001 << 0)                   /*!< R/W     0 Set clk_da_ana phase1'b0: Inphase1'b1: Inverse phase */
#define AUDIO_CODEC_BIT_AD_ANA_CLK_SEL              ((u32)0x00000001 << 1)                   /*!< R/W     0 Set clk_ad_ana phase1'b0: Inphase1'b1: Inverse phase */
#define AUDIO_CODEC_BIT_AD_LATCH_PHASE              ((u32)0x00000001 << 2)                   /*!< R/W     0 Set latch adc data phase1'b0: Inphase1'b1: Inverse phase */
#define AUDIO_CODEC_MASK_ANA_CLK_RATE_SEL           ((u32)0x00000003 << 3)                   /*!< R/W     0 Set DA/AD analog clock0: 10MHz1: 5MHz2: 2.5MHz3: Reseved */
#define AUDIO_CODEC_ANA_CLK_RATE_SEL(x)             ((u32)(((x) & 0x00000003) << 3))
#define AUDIO_CODEC_GET_ANA_CLK_RATE_SEL(x)         ((u32)(((x >> 3) & 0x00000003)))
#define AUDIO_CODEC_MASK_AD_LPF_CLK_RATE_SEL        ((u32)0x00000003 << 5)                   /*!< R/W     0 Set AD LPF clock0: 10MHz1: 5MHz2: 2.5MHz3: Reseved  */
#define AUDIO_CODEC_AD_LPF_CLK_RATE_SEL(x)          ((u32)(((x) & 0x00000003) << 5))
#define AUDIO_CODEC_GET_AD_LPF_CLK_RATE_SEL(x)      ((u32)(((x >> 5) & 0x00000003)))
#define AUDIO_CODEC_MASK_ADC_0_DMIC_LPF_CLK_SEL     ((u32)0x00000003 << 7)                   /*!< R/W     0 Channel 0 ADC path DMIC LPF clock2'b00: 10M2'b01: 5M2'b10: 2.5M2'b11: 769.2*2K */
#define AUDIO_CODEC_ADC_0_DMIC_LPF_CLK_SEL(x)       ((u32)(((x) & 0x00000003) << 7))
#define AUDIO_CODEC_GET_ADC_0_DMIC_LPF_CLK_SEL(x)   ((u32)(((x >> 7) & 0x00000003)))
#define AUDIO_CODEC_MASK_ADC_1_DMIC_LPF_CLK_SEL     ((u32)0x00000003 << 9)                   /*!< R/W     0 Channel 1 ADC path DMIC LPF clock2'b00: 10M2'b01: 5M2'b10: 2.5M2'b11: 769.2*2K */
#define AUDIO_CODEC_ADC_1_DMIC_LPF_CLK_SEL(x)       ((u32)(((x) & 0x00000003) << 9))
#define AUDIO_CODEC_GET_ADC_1_DMIC_LPF_CLK_SEL(x)   ((u32)(((x >> 9) & 0x00000003)))
#define AUDIO_CODEC_MASK_ADC_2_DMIC_LPF_CLK_SEL     ((u32)0x00000003 << 11)                  /*!< R/W     0 Channel 2 ADC path DMIC LPF clock2'b00: 10M2'b01: 5M2'b10: 2.5M2'b11: 769.2*2K */
#define AUDIO_CODEC_ADC_2_DMIC_LPF_CLK_SEL(x)       ((u32)(((x) & 0x00000003) << 11))
#define AUDIO_CODEC_GET_ADC_2_DMIC_LPF_CLK_SEL(x)   ((u32)(((x >> 11) & 0x00000003)))
#define AUDIO_CODEC_BIT_ANA_CLK_PHASE_SEL           ((u32)0x00000001 << 23)                  /*!< R/W     0 1:analog clock shift 80MHz phase, 0:disable */
#define AUDIO_CODEC_BIT_AD_ANA_OSR_DOUBLE           ((u32)0x00000001 << 24)                  /*!< R/W     0 1:latch adc data with 10MHz clock */
/** @} */


/** @defgroup asrc_control_0
 * @brief
 * @{
 */
#define AUDIO_CODEC_MASK_ASRC_RATE_SEL_TX           ((u32)0x00000003 << 0)                   /*!< R/W     0 0: fs supports 4kHz ~ 60kHz1: fs supports 60kHz ~ 120kHz2: fs supports 120kHz ~ 200kHz3: Reserved */
#define AUDIO_CODEC_ASRC_RATE_SEL_TX(x)             ((u32)(((x) & 0x00000003) << 0))
#define AUDIO_CODEC_GET_ASRC_RATE_SEL_TX(x)         ((u32)(((x >> 0) & 0x00000003)))
#define AUDIO_CODEC_MASK_ASRC_GAIN_SEL_TX           ((u32)0x00000003 << 2)                   /*!< R/W   0x3 ASRC convergence rate: larger is faster but more noisy */
#define AUDIO_CODEC_ASRC_GAIN_SEL_TX(x)             ((u32)(((x) & 0x00000003) << 2))
#define AUDIO_CODEC_GET_ASRC_GAIN_SEL_TX(x)         ((u32)(((x >> 2) & 0x00000003)))
#define AUDIO_CODEC_BIT_ASRC_AUTO_ADJUST_TX         ((u32)0x00000001 << 4)                   /*!< R/W     1 HW auto adjust convergence rate0: Disable1: Enable */
/** @} */


/** @defgroup asrc_control_1
 * @brief
 * @{
 */
#define AUDIO_CODEC_MASK_ASRC_SDM_INTI_TX           ((u32)0x00FFFFFF << 0)                   /*!< R/W     0 Set  initial value of tracked frequency:fs<=48kHz:asrc_sdm_inti = ((256*fs/20MHz)-0.5)*2^24fs> 48kHz:asrc_sdm_inti = ((128*fs/20MHz)-0.5)*2^24 */
#define AUDIO_CODEC_ASRC_SDM_INTI_TX(x)             ((u32)(((x) & 0x00FFFFFF) << 0))
#define AUDIO_CODEC_GET_ASRC_SDM_INTI_TX(x)         ((u32)(((x >> 0) & 0x00FFFFFF)))
/** @} */


/** @defgroup asrc_control_2
 * @brief
 * @{
 */
#define AUDIO_CODEC_MASK_ASRC_RATE_SEL_RX_0         ((u32)0x00000003 << 0)                   /*!< R/W     0 0: fs supports 4kHz ~ 60kHz1: fs supports 60kHz ~ 120kHz2/3: Reserved */
#define AUDIO_CODEC_ASRC_RATE_SEL_RX_0(x)           ((u32)(((x) & 0x00000003) << 0))
#define AUDIO_CODEC_GET_ASRC_RATE_SEL_RX_0(x)       ((u32)(((x >> 0) & 0x00000003)))
#define AUDIO_CODEC_MASK_ASRC_GAIN_SEL_RX_0         ((u32)0x00000003 << 2)                   /*!< R/W   0x3 ASRC convergence rate: larger is faster but more noisy */
#define AUDIO_CODEC_ASRC_GAIN_SEL_RX_0(x)           ((u32)(((x) & 0x00000003) << 2))
#define AUDIO_CODEC_GET_ASRC_GAIN_SEL_RX_0(x)       ((u32)(((x >> 2) & 0x00000003)))
#define AUDIO_CODEC_BIT_ASRC_AUTO_ADJUST_RX_0       ((u32)0x00000001 << 4)                   /*!< R/W     1 HW auto adjust convergence rate0: Disable1: Enable */
/** @} */


/** @defgroup asrc_control_3
 * @brief
 * @{
 */
#define AUDIO_CODEC_MASK_ASRC_SDM_INTI_RX_0         ((u32)0x00FFFFFF << 0)                   /*!< R/W     0 Set  initial value of tracked frequency:fs<=48kHz:asrc_sdm_inti = ((256*fs/20MHz)-0.5)*2^24fs> 48kHz:asrc_sdm_inti = ((128*fs/20MHz)-0.5)*2^24 */
#define AUDIO_CODEC_ASRC_SDM_INTI_RX_0(x)           ((u32)(((x) & 0x00FFFFFF) << 0))
#define AUDIO_CODEC_GET_ASRC_SDM_INTI_RX_0(x)       ((u32)(((x >> 0) & 0x00FFFFFF)))
/** @} */


/** @defgroup i2s_0_control
 * @brief
 * @{
 */
#define AUDIO_CODEC_BIT_I2S_0_RST_N_REG             ((u32)0x00000001 << 0)                   /*!< R/W     0 I2S 01'b1: rst_n to audio digital IP is de-asserted1'b0: rst_n to audio digital IP is asserted  */
#define AUDIO_CODEC_BIT_I2S_0_INV_SCLK              ((u32)0x00000001 << 1)                   /*!< R/W     0 I2S 01'b1: I2S/PCM bit clock is inverted  */
#define AUDIO_CODEC_BIT_I2S_0_SELF_LPBK_EN          ((u32)0x00000001 << 2)                   /*!< R/W     0 I2S 01'b1: internal loopback mode is enabled  */
#define AUDIO_CODEC_BIT_I2S_0_SAME_LRC_EN           ((u32)0x00000001 << 3)                   /*!< R/W     0 I2S 01'b1: LRC_TX share to LRC_RX  */
#define AUDIO_CODEC_MASK_I2S_0_TDM_MODE_RX          ((u32)0x00000003 << 4)                   /*!< R/W     0 I2S 0 Rx channel TDM mode0: 2 channel 1: 4 channel2/3: reserved */
#define AUDIO_CODEC_I2S_0_TDM_MODE_RX(x)            ((u32)(((x) & 0x00000003) << 4))
#define AUDIO_CODEC_GET_I2S_0_TDM_MODE_RX(x)        ((u32)(((x >> 4) & 0x00000003)))
#define AUDIO_CODEC_MASK_I2S_0_DATA_FORMAT_SEL_TX   ((u32)0x00000003 << 6)                   /*!< R/W     0 I2S 0 Tx channel data format2'b00: I2S2'b01: left-justified2'b10: PCM mode A2'b11: PCM mode B */
#define AUDIO_CODEC_I2S_0_DATA_FORMAT_SEL_TX(x)     ((u32)(((x) & 0x00000003) << 6))
#define AUDIO_CODEC_GET_I2S_0_DATA_FORMAT_SEL_TX(x) ((u32)(((x >> 6) & 0x00000003)))
#define AUDIO_CODEC_MASK_I2S_0_DATA_FORMAT_SEL_RX   ((u32)0x00000003 << 8)                   /*!< R/W     0 I2S 0 Rx channel data format2'b00: I2S2'b01: left-justified2'b10: PCM mode A2'b11: PCM mode B */
#define AUDIO_CODEC_I2S_0_DATA_FORMAT_SEL_RX(x)     ((u32)(((x) & 0x00000003) << 8))
#define AUDIO_CODEC_GET_I2S_0_DATA_FORMAT_SEL_RX(x) ((u32)(((x >> 8) & 0x00000003)))
#define AUDIO_CODEC_MASK_I2S_0_DATA_LEN_SEL_TX      ((u32)0x00000003 << 10)                  /*!< R/W     0 I2S 0 Tx channel data length2'b00: 16 bits2'b10: 24 bits2'b11: 8 bits */
#define AUDIO_CODEC_I2S_0_DATA_LEN_SEL_TX(x)        ((u32)(((x) & 0x00000003) << 10))
#define AUDIO_CODEC_GET_I2S_0_DATA_LEN_SEL_TX(x)    ((u32)(((x >> 10) & 0x00000003)))
#define AUDIO_CODEC_MASK_I2S_0_DATA_LEN_SEL_RX      ((u32)0x00000003 << 12)                  /*!< R/W     0 I2S 0 Rx channel data length2'b00: 16 bits2'b10: 24 bits2'b11: 8 bits */
#define AUDIO_CODEC_I2S_0_DATA_LEN_SEL_RX(x)        ((u32)(((x) & 0x00000003) << 12))
#define AUDIO_CODEC_GET_I2S_0_DATA_LEN_SEL_RX(x)    ((u32)(((x >> 12) & 0x00000003)))
#define AUDIO_CODEC_MASK_I2S_0_CH_LEN_SEL_TX        ((u32)0x00000003 << 14)                  /*!< R/W     0 I2S 0 Tx channel channel length2'b00: 16 bits2'b01: 32 bits2'b10: 24 bits2'b11: 8 bits */
#define AUDIO_CODEC_I2S_0_CH_LEN_SEL_TX(x)          ((u32)(((x) & 0x00000003) << 14))
#define AUDIO_CODEC_GET_I2S_0_CH_LEN_SEL_TX(x)      ((u32)(((x >> 14) & 0x00000003)))
#define AUDIO_CODEC_MASK_I2S_0_CH_LEN_SEL_RX        ((u32)0x00000003 << 16)                  /*!< R/W     0 I2S 0 Rx channel chennel length2'b00: 16 bits2'b01: 32 bits2'b10: 24 bits2'b11: 8 bits */
#define AUDIO_CODEC_I2S_0_CH_LEN_SEL_RX(x)          ((u32)(((x) & 0x00000003) << 16))
#define AUDIO_CODEC_GET_I2S_0_CH_LEN_SEL_RX(x)      ((u32)(((x >> 16) & 0x00000003)))
#define AUDIO_CODEC_MASK_I2S_0_DATA_CH_SEL_TX       ((u32)0x00000003 << 18)                  /*!< R/W     0 I2S 0 Tx channel data channel2'b00: L/R2'b01: R/L2'b10: L/L2'b11: R/R */
#define AUDIO_CODEC_I2S_0_DATA_CH_SEL_TX(x)         ((u32)(((x) & 0x00000003) << 18))
#define AUDIO_CODEC_GET_I2S_0_DATA_CH_SEL_TX(x)     ((u32)(((x >> 18) & 0x00000003)))
#define AUDIO_CODEC_BIT_I2S_0_MASTER_SEL            ((u32)0x00000001 << 20)                  /*!< R/W     0 I2S 0 master source sel1'b0: sport internal1'b1: external  I2S */
/** @} */


/** @defgroup i2s_0_control_1
 * @brief
 * @{
 */
#define AUDIO_CODEC_MASK_I2S_0_DATA_CH0_SEL_RX      ((u32)0x00000007 << 0)                   /*!< R/W     0 I2S 0 Rx channel data channel 00: CH01: CH12: CH23/4/5/6/7: reserved */
#define AUDIO_CODEC_I2S_0_DATA_CH0_SEL_RX(x)        ((u32)(((x) & 0x00000007) << 0))
#define AUDIO_CODEC_GET_I2S_0_DATA_CH0_SEL_RX(x)    ((u32)(((x >> 0) & 0x00000007)))
#define AUDIO_CODEC_MASK_I2S_0_DATA_CH1_SEL_RX      ((u32)0x00000007 << 3)                   /*!< R/W     0 I2S 0 Rx channel data channel 10: CH01: CH12: CH23/4/5/6/7: reserved */
#define AUDIO_CODEC_I2S_0_DATA_CH1_SEL_RX(x)        ((u32)(((x) & 0x00000007) << 3))
#define AUDIO_CODEC_GET_I2S_0_DATA_CH1_SEL_RX(x)    ((u32)(((x >> 3) & 0x00000007)))
#define AUDIO_CODEC_MASK_I2S_0_DATA_CH2_SEL_RX      ((u32)0x00000007 << 6)                   /*!< R/W     0 I2S 0 Rx channel data channel 20: CH01: CH12: CH23/4/5/6/7: reserved */
#define AUDIO_CODEC_I2S_0_DATA_CH2_SEL_RX(x)        ((u32)(((x) & 0x00000007) << 6))
#define AUDIO_CODEC_GET_I2S_0_DATA_CH2_SEL_RX(x)    ((u32)(((x >> 6) & 0x00000007)))
#define AUDIO_CODEC_BIT_I2S_0_DATA_CH0_RX_DISABLE   ((u32)0x00000001 << 24)                  /*!< R/W     0 I2S 0 Rx channel data channel 01: disabe0: enable */
#define AUDIO_CODEC_BIT_I2S_0_DATA_CH1_RX_DISABLE   ((u32)0x00000001 << 25)                  /*!< R/W     0 I2S 0 Rx channel data channel 11: disabe0: enable */
#define AUDIO_CODEC_BIT_I2S_0_DATA_CH2_RX_DISABLE   ((u32)0x00000001 << 26)                  /*!< R/W     0 I2S 0 Rx channel data channel 21: disabe0: enable */
/** @} */


/** @defgroup adc_x_control_0
 * @brief
 * @{
 */
#define AUDIO_CODEC_MASK_ADC_x_DMIC_SRC_SEL         ((u32)0x00000007 << 0)                   /*!< R/W     0 Channel 0 DMIC source selection3'b000: MIC 1 rising3'b001: MIC 1 fallingother: reserved */
#define AUDIO_CODEC_ADC_x_DMIC_SRC_SEL(x)           ((u32)(((x) & 0x00000007) << 0))
#define AUDIO_CODEC_GET_ADC_x_DMIC_SRC_SEL(x)       ((u32)(((x >> 0) & 0x00000007)))
#define AUDIO_CODEC_BIT_ADC_x_DMIC_LPF2ND_EN        ((u32)0x00000001 << 3)                   /*!< R/W     1 Channel 0 DMIC path SRC 2nd LPF control0: Disable1: Enable */
#define AUDIO_CODEC_BIT_ADC_x_DMIC_LPF1ST_EN        ((u32)0x00000001 << 4)                   /*!< R/W     1 Channel 0 DMIC path SRC 1st LPF control0: Disable1: Enable */
#define AUDIO_CODEC_MASK_ADC_x_DMIC_LPF1ST_FC_SEL   ((u32)0x00000003 << 5)                   /*!< R/W     0 Channel 0 DMIC path SRC 1st LPF FC  */
#define AUDIO_CODEC_ADC_x_DMIC_LPF1ST_FC_SEL(x)     ((u32)(((x) & 0x00000003) << 5))
#define AUDIO_CODEC_GET_ADC_x_DMIC_LPF1ST_FC_SEL(x) ((u32)(((x >> 5) & 0x00000003)))
#define AUDIO_CODEC_BIT_ADC_x_DMIC_MIX_MUTE         ((u32)0x00000001 << 7)                   /*!< R/W     1 Channel 0 DMIC input path mute0: Un-Mute1: Mute */
#define AUDIO_CODEC_MASK_ADC_x_AD_SRC_SEL           ((u32)0x00000007 << 8)                   /*!< R/W     0 Channel 0 ANA ADC source selection3'd0: ADC 1others: reserved */
#define AUDIO_CODEC_ADC_x_AD_SRC_SEL(x)             ((u32)(((x) & 0x00000007) << 8))
#define AUDIO_CODEC_GET_ADC_x_AD_SRC_SEL(x)         ((u32)(((x >> 8) & 0x00000007)))
#define AUDIO_CODEC_BIT_ADC_x_AD_LPF2ND_EN          ((u32)0x00000001 << 11)                  /*!< R/W     1 Channel 0 ADC path SRC 2nd LPF control0: Disable1: Enable */
#define AUDIO_CODEC_MASK_ADC_x_AD_LPF2ND_FC_SEL     ((u32)0x00000003 << 12)                  /*!< R/W     0 Channel 0 ADC path SRC 2nd LPF FC */
#define AUDIO_CODEC_ADC_x_AD_LPF2ND_FC_SEL(x)       ((u32)(((x) & 0x00000003) << 12))
#define AUDIO_CODEC_GET_ADC_x_AD_LPF2ND_FC_SEL(x)   ((u32)(((x >> 12) & 0x00000003)))
#define AUDIO_CODEC_BIT_ADC_x_AD_LPF1ST_EN          ((u32)0x00000001 << 14)                  /*!< R/W     1 Channel 0 ADC path SRC 1st LPF control0: Disable1: Enable */
#define AUDIO_CODEC_MASK_ADC_x_AD_LPF1ST_FC_SEL     ((u32)0x00000003 << 15)                  /*!< R/W     0 Channel 0 ADC path SRC 1st LPF FC  */
#define AUDIO_CODEC_ADC_x_AD_LPF1ST_FC_SEL(x)       ((u32)(((x) & 0x00000003) << 15))
#define AUDIO_CODEC_GET_ADC_x_AD_LPF1ST_FC_SEL(x)   ((u32)(((x >> 15) & 0x00000003)))
#define AUDIO_CODEC_BIT_ADC_x_AD_MIX_MUTE           ((u32)0x00000001 << 17)                  /*!< R/W     1 Channel 0 ADC input path mute0: Un-Mute1: Mute */
#define AUDIO_CODEC_MASK_ADC_x_AD_ZDET_FUNC         ((u32)0x00000003 << 18)                  /*!< R/W   0x2 Channel 0 ADC path zero detection function select2'b00: immediate change2'b01: zero detection & immediate change2'b10: zdet & step2'b11: zdet & timeout step */
#define AUDIO_CODEC_ADC_x_AD_ZDET_FUNC(x)           ((u32)(((x) & 0x00000003) << 18))
#define AUDIO_CODEC_GET_ADC_x_AD_ZDET_FUNC(x)       ((u32)(((x >> 18) & 0x00000003)))
#define AUDIO_CODEC_MASK_ADC_x_AD_ZDET_TOUT         ((u32)0x00000003 << 20)                  /*!< R/W     0 Channel 0 ADC path zero detection time out select2'b00: 1024*16 samples2'b01: 1024*32 samples2'b10: 1024*64 samples2'b11: 64 samples */
#define AUDIO_CODEC_ADC_x_AD_ZDET_TOUT(x)           ((u32)(((x) & 0x00000003) << 20))
#define AUDIO_CODEC_GET_ADC_x_AD_ZDET_TOUT(x)       ((u32)(((x >> 20) & 0x00000003)))
#define AUDIO_CODEC_BIT_ADC_x_AD_MUTE               ((u32)0x00000001 << 22)                  /*!< R/W     0 Channel 0 ADC path mute0: Un-Mute1: Mute */
#define AUDIO_CODEC_MASK_ADC_x_DECI_SRC_SEL         ((u32)0x00000003 << 24)                  /*!< R/W     0 Channel 0 ADC path decimation source select0: ADC1: reserved2: MUSIC3: reserved */
#define AUDIO_CODEC_ADC_x_DECI_SRC_SEL(x)           ((u32)(((x) & 0x00000003) << 24))
#define AUDIO_CODEC_GET_ADC_x_DECI_SRC_SEL(x)       ((u32)(((x >> 24) & 0x00000003)))
#define AUDIO_CODEC_BIT_ADC_x_DCHPF_EN              ((u32)0x00000001 << 26)                  /*!< R/W     0 Channel 0 ADC path high pass filter enable control (filter DC)0: Disable1: Enable */
#define AUDIO_CODEC_MASK_ADC_x_DCHPF_FC_SEL         ((u32)0x00000007 << 27)                  /*!< R/W   0x5 Channel 0 ADC path high pass filter Fc */
#define AUDIO_CODEC_ADC_x_DCHPF_FC_SEL(x)           ((u32)(((x) & 0x00000007) << 27))
#define AUDIO_CODEC_GET_ADC_x_DCHPF_FC_SEL(x)       ((u32)(((x >> 27) & 0x00000007)))
#define AUDIO_CODEC_BIT_ADC_x_AD_FLT_TYPE           ((u32)0x00000001 << 30)                  /*!< R/W     0 Channel 0 ADC path decimation filter type 1'b0: audio, 1'b1 voice */
/** @} */


/** @defgroup adc_x_control_1
 * @brief
 * @{
 */
#define AUDIO_CODEC_MASK_ADC_x_AD_GAIN              ((u32)0x000001FF << 0)                   /*!< R/W   0x2f Channel 0 ADC digital volume -17.625dB ~ 48dB in 0.375dB step9'h00: -17.625dB...9'h2f: 0dB9'h30: 0.375dB...9'haf: 48dB...9'h10f: 84dB */
#define AUDIO_CODEC_ADC_x_AD_GAIN(x)                ((u32)(((x) & 0x000001FF) << 0))
#define AUDIO_CODEC_GET_ADC_x_AD_GAIN(x)            ((u32)(((x >> 0) & 0x000001FF)))
#define AUDIO_CODEC_MASK_DUMMY                      ((u32)0x00000007 << 9)                   /*!< R/W     0 Dummy */
#define AUDIO_CODEC_DUMMY(x)                        ((u32)(((x) & 0x00000007) << 9))
#define AUDIO_CODEC_GET_DUMMY(x)                    ((u32)(((x >> 9) & 0x00000007)))
#define AUDIO_CODEC_MASK_ADC_x_RPTR_HOLD_           ((u32)0x0000000F << 12)                  /*!< R/W     0 Channel 0 I2S read point hold numberrptr_hold  = (I2S_fs/ad_fs) - 1 */
#define AUDIO_CODEC_ADC_x_RPTR_HOLD_(x)             ((u32)(((x) & 0x0000000F) << 12))
#define AUDIO_CODEC_GET_ADC_x_RPTR_HOLD_(x)         ((u32)(((x >> 12) & 0x0000000F)))
#define AUDIO_CODEC_BIT_ADC_x_FIFO_KEEP_ONE         ((u32)0x00000001 << 16)                  /*!< R/W     0 Channel 0 i2s sample buffering 0: 4 samples, 1: 1 sample */
/** @} */


/** @defgroup dac_l_control_0
 * @brief
 * @{
 */
#define AUDIO_CODEC_MASK_DAC_L_DA_GAIN              ((u32)0x000000FF << 0)                   /*!< R/W   0xaf Channel L DAC path dvol gain control (0.375dB/step)8'hFF: 30dB8'h00: -65.625dB  */
#define AUDIO_CODEC_DAC_L_DA_GAIN(x)                ((u32)(((x) & 0x000000FF) << 0))
#define AUDIO_CODEC_GET_DAC_L_DA_GAIN(x)            ((u32)(((x >> 0) & 0x000000FF)))
#define AUDIO_CODEC_BIT_DAC_L_DA_SRC_SEL            ((u32)0x00000001 << 16)                  /*!< R/W     0 Channel L DAC path input sel 0:I2S L channel 1: test tone */
#define AUDIO_CODEC_BIT_DAC_L_CONTROL_0_DUMMY       ((u32)0x00000001 << 17)                  /*!< R/W     0 DUMMY */
#define AUDIO_CODEC_BIT_DAC_L_TEST_TONE_EN          ((u32)0x00000001 << 20)                  /*!< R/W     0 Channel L DAC path test tone enable */
#define AUDIO_CODEC_MASK_DAC_L_TEST_FC_SEL          ((u32)0x0000007F << 21)                  /*!< R/W   0x0 Channel L DAC path test tone frquency: (fs/192)*(tone_fc_sel+1)Hz  */
#define AUDIO_CODEC_DAC_L_TEST_FC_SEL(x)            ((u32)(((x) & 0x0000007F) << 21))
#define AUDIO_CODEC_GET_DAC_L_TEST_FC_SEL(x)        ((u32)(((x >> 21) & 0x0000007F)))
#define AUDIO_CODEC_MASK_DAC_L_TEST_GAIN_SEL        ((u32)0x0000000F << 28)                  /*!< R/W   0x0 Channel L DAC path test tone gain control 0 -6.02*(gain_sel)dB  */
#define AUDIO_CODEC_DAC_L_TEST_GAIN_SEL(x)          ((u32)(((x) & 0x0000000F) << 28))
#define AUDIO_CODEC_GET_DAC_L_TEST_GAIN_SEL(x)      ((u32)(((x >> 28) & 0x0000000F)))
/** @} */


/** @defgroup dac_l_control_1
 * @brief
 * @{
 */
#define AUDIO_CODEC_MASK_DAC_L_DA_ZDET_FUNC         ((u32)0x00000003 << 0)                   /*!< R/W   0x2 Channel L DAC path zero detection function select2'b00: immediate change2'b01: zero detection & immediate change2'b10: zdet & step2'b11: zdet & timeout step */
#define AUDIO_CODEC_DAC_L_DA_ZDET_FUNC(x)           ((u32)(((x) & 0x00000003) << 0))
#define AUDIO_CODEC_GET_DAC_L_DA_ZDET_FUNC(x)       ((u32)(((x >> 0) & 0x00000003)))
#define AUDIO_CODEC_MASK_DAC_L_DA_ZDET_TOUT         ((u32)0x00000003 << 2)                   /*!< R/W     0 Channel L DAC path zero detection time out select2'b00: 1024*16 samples2'b01: 1024*32 samples2'b10: 1024*64 samples2'b11: 64 samples */
#define AUDIO_CODEC_DAC_L_DA_ZDET_TOUT(x)           ((u32)(((x) & 0x00000003) << 2))
#define AUDIO_CODEC_GET_DAC_L_DA_ZDET_TOUT(x)       ((u32)(((x >> 2) & 0x00000003)))
#define AUDIO_CODEC_BIT_DAC_L_DA_MUTE               ((u32)0x00000001 << 4)                   /*!< R/W     0 Channel L DAC path dvol mute enable1'b0: unmute1'b1: mute   */
#define AUDIO_CODEC_BIT_DAC_L_DAHPF_EN              ((u32)0x00000001 << 5)                   /*!< R/W     0 Channel L DAC path Narrow-band 1st HPF enable control0: Disable1: Enable */
#define AUDIO_CODEC_MASK_DAC_L_DA_DITHER_SEL        ((u32)0x00000003 << 6)                   /*!< R/W     0 Channel L DAC path dither select2'b00: Disable2'b01: LSB2'b10: LSB+12'b11: LSB+2    */
#define AUDIO_CODEC_DAC_L_DA_DITHER_SEL(x)          ((u32)(((x) & 0x00000003) << 6))
#define AUDIO_CODEC_GET_DAC_L_DA_DITHER_SEL(x)      ((u32)(((x >> 6) & 0x00000003)))
#define AUDIO_CODEC_BIT_DAC_L_DMIX_MUTE_DA          ((u32)0x00000001 << 8)                   /*!< R/W     0 Channel L DAC path 128fs-domain mixer da path mute enable1'b0: unmute1'b1: mute    */
#define AUDIO_CODEC_BIT_DAC_L_CONTROL_1_DUMMY0      ((u32)0x00000001 << 9)                   /*!< R/W     0 DUMMY */
#define AUDIO_CODEC_BIT_DAC_L_DMIX_MUTE_DC          ((u32)0x00000001 << 10)                  /*!< R/W     0 Channel L DAC path 128fs-domain mixer a DC from dac_l_tone_compensation_offset1'b0: unmute1'b1: mute    */
#define AUDIO_CODEC_BIT_DAC_L_MUSIC_MUTE_EN         ((u32)0x00000001 << 11)                  /*!< R/W     0 Channel L DAC path music output mute enable1'b0: unmute1'b1: mute  */
#define AUDIO_CODEC_BIT_DAC_L_PDM_EN                ((u32)0x00000001 << 13)                  /*!< R/W     1 Channel L DAC path PDM0: Disable1: Enable    */
#define AUDIO_CODEC_BIT_DAC_L_SDM_EXTEND_FB_EN      ((u32)0x00000001 << 14)                  /*!< R/W     1 Channel L DAC path feedback extend (for debug purpose; default need turn on)0: Disable1: Enable   */
#define AUDIO_CODEC_BIT_DAC_L_SDM_EF_EN             ((u32)0x00000001 << 15)                  /*!< R/W     1 Channel L DAC path 2nd order SDM enable0: Disable1: Enable   */
#define AUDIO_CODEC_MASK_DAC_L_SDM_DITHER_SEL       ((u32)0x00000003 << 16)                  /*!< R/W     0 Channel L DAC path SDM dither select2'b00: Disable2'b01: LSB2'b10: LSB+12'b11: LSB+2    */
#define AUDIO_CODEC_DAC_L_SDM_DITHER_SEL(x)         ((u32)(((x) & 0x00000003) << 16))
#define AUDIO_CODEC_GET_DAC_L_SDM_DITHER_SEL(x)     ((u32)(((x >> 16) & 0x00000003)))
#define AUDIO_CODEC_MASK_DAC_L_DA_FLT_TYPE          ((u32)0x00000003 << 18)                  /*!< R/W     0 Channel L DAC path interpolation filter type  2'd0:Normal, 2'd1: reserved, 2'd2: reserved, 2'd3:Voice */
#define AUDIO_CODEC_DAC_L_DA_FLT_TYPE(x)            ((u32)(((x) & 0x00000003) << 18))
#define AUDIO_CODEC_GET_DAC_L_DA_FLT_TYPE(x)        ((u32)(((x >> 18) & 0x00000003)))
#define AUDIO_CODEC_BIT_DAC_L_CONTROL_1_DUMMY1      ((u32)0x00000001 << 21)                  /*!< R/W     0 DUMMY */
#define AUDIO_CODEC_MASK_DAC_L_HPF_FC_SEL           ((u32)0x00000003 << 22)                  /*!< R/W     1 Channel L DAC path HPF filte cut-off freq. 00: (3.125e-4)*fs, 01: (1.5625e-4)*fs, 10: (7.8125e-5)*fs, 11: (3.90625e-5)*fs */
#define AUDIO_CODEC_DAC_L_HPF_FC_SEL(x)             ((u32)(((x) & 0x00000003) << 22))
#define AUDIO_CODEC_GET_DAC_L_HPF_FC_SEL(x)         ((u32)(((x >> 22) & 0x00000003)))
/** @} */


/** @defgroup dac_l_control_2
 * @brief
 * @{
 */
#define AUDIO_CODEC_MASK_DAC_L_DC_OFFSET            ((u32)0x0000FFFF << 0)                   /*!< R/W     0 Channel L DAC path 128fs-domain mixe a DC compensation for spur issue 2's complemetformat: S(16,16)range: -6 ~ -96dBFS */
#define AUDIO_CODEC_DAC_L_DC_OFFSET(x)              ((u32)(((x) & 0x0000FFFF) << 0))
#define AUDIO_CODEC_GET_DAC_L_DC_OFFSET(x)          ((u32)(((x >> 0) & 0x0000FFFF)))
#define AUDIO_CODEC_MASK_DAC_L_CONTROL_2_DUMMY0     ((u32)0x0000000F << 16)                  /*!< R/W     0 DUMMY */
#define AUDIO_CODEC_DAC_L_CONTROL_2_DUMMY0(x)       ((u32)(((x) & 0x0000000F) << 16))
#define AUDIO_CODEC_GET_DAC_L_CONTROL_2_DUMMY0(x)   ((u32)(((x >> 16) & 0x0000000F)))
#define AUDIO_CODEC_MASK_DAC_L_CONTROL_2_DUMMY1     ((u32)0x0000000F << 20)                  /*!< R/W   0x7 DUMMY */
#define AUDIO_CODEC_DAC_L_CONTROL_2_DUMMY1(x)       ((u32)(((x) & 0x0000000F) << 20))
#define AUDIO_CODEC_GET_DAC_L_CONTROL_2_DUMMY1(x)   ((u32)(((x >> 20) & 0x0000000F)))
/** @} */


/** @defgroup adc_align_control_0
 * @brief
 * @{
 */
#define AUDIO_CODEC_BIT_SP_AD_FIFO_ALIGN_EN         ((u32)0x00000001 << 0)                   /*!< R/W     0 1:Uplink fifo will align with I2S, 0:disable */
#define AUDIO_CODEC_BIT_SP_AD_ALIGN_EN              ((u32)0x00000001 << 2)                   /*!< R/W     0 1:Uplink channel will align with I2S, 0:disable */
/** @} */


/** @defgroup adc_align_control
 * @brief
 * @{
 */
#define AUDIO_CODEC_MASK_ADC_0_ALIGN_CH_SEL         ((u32)0x00000007 << 0)                   /*!< R/W     0 Uplink CH0 align channel selection: 0:CH0, 1:CH1, 2:CH2, others:reserved */
#define AUDIO_CODEC_ADC_0_ALIGN_CH_SEL(x)           ((u32)(((x) & 0x00000007) << 0))
#define AUDIO_CODEC_GET_ADC_0_ALIGN_CH_SEL(x)       ((u32)(((x >> 0) & 0x00000007)))
#define AUDIO_CODEC_BIT_ADC_0_ALIGN_EN              ((u32)0x00000001 << 3)                   /*!< R/W     0 Uplink CH0 align enable */
#define AUDIO_CODEC_MASK_ADC_1_ALIGN_CH_SEL         ((u32)0x00000007 << 4)                   /*!< R/W     1 Uplink CH1 align channel selection: 0:CH0, 1:CH1, 2:CH2, others:reserved */
#define AUDIO_CODEC_ADC_1_ALIGN_CH_SEL(x)           ((u32)(((x) & 0x00000007) << 4))
#define AUDIO_CODEC_GET_ADC_1_ALIGN_CH_SEL(x)       ((u32)(((x >> 4) & 0x00000007)))
#define AUDIO_CODEC_BIT_ADC_1_ALIGN_EN              ((u32)0x00000001 << 7)                   /*!< R/W     0 Uplink CH1 align enable */
#define AUDIO_CODEC_MASK_ADC_2_ALIGN_CH_SEL         ((u32)0x00000007 << 8)                   /*!< R/W   0x2 Uplink CH2 align channel selection: 0:CH0, 1:CH1, 2:CH2, others:reserved */
#define AUDIO_CODEC_ADC_2_ALIGN_CH_SEL(x)           ((u32)(((x) & 0x00000007) << 8))
#define AUDIO_CODEC_GET_ADC_2_ALIGN_CH_SEL(x)       ((u32)(((x >> 8) & 0x00000007)))
#define AUDIO_CODEC_BIT_ADC_2_ALIGN_EN              ((u32)0x00000001 << 11)                  /*!< R/W     0 Uplink CH2 align enable */
/** @} */


/** @defgroup dac_mix_control
 * @brief
 * @{
 */
#define AUDIO_CODEC_MASK_DAC_MIX_CONTROL_DUMMY      ((u32)0x0000000F << 0)                   /*!< R/W     0 DUMMY */
#define AUDIO_CODEC_DAC_MIX_CONTROL_DUMMY(x)        ((u32)(((x) & 0x0000000F) << 0))
#define AUDIO_CODEC_GET_DAC_MIX_CONTROL_DUMMY(x)    ((u32)(((x >> 0) & 0x0000000F)))
/** @} */


/** @defgroup adc_x_silence_control
 * @brief
 * @{
 */
#define AUDIO_CODEC_BIT_ADC_x_SILENCE_DET_EN        ((u32)0x00000001 << 0)                   /*!< R/W     0 Channel 0 ADC path silence detection enable0: Disable1: Enable */
#define AUDIO_CODEC_MASK_ADC_x_SILENCE_LEVEL_SEL    ((u32)0x00000007 << 1)                   /*!< R/W     1 Channel 0 ADC path silence detection threshold3'b000: -54db3'b001: -60db3'b010: -66db3'b011: -72db3'b100: -78db3'b101: -84db3'b110: -90db3'b111: -96db */
#define AUDIO_CODEC_ADC_x_SILENCE_LEVEL_SEL(x)      ((u32)(((x) & 0x00000007) << 1))
#define AUDIO_CODEC_GET_ADC_x_SILENCE_LEVEL_SEL(x)  ((u32)(((x >> 1) & 0x00000007)))
#define AUDIO_CODEC_MASK_ADC_x_SILENCE_DEBOUNCE_SEL ((u32)0x00000007 << 4)                   /*!< R/W   0x3 Channel 0 ADC path silence detection debounce (48K)3'b000: 80ms3'b001: 160ms3'b010: 320ms3'b011: 640ms3'b100: 1.28s3'b101: 2.56s3'b110: 5.12s3'b111: 0.16ms  */
#define AUDIO_CODEC_ADC_x_SILENCE_DEBOUNCE_SEL(x)   ((u32)(((x) & 0x00000007) << 4))
#define AUDIO_CODEC_GET_ADC_x_SILENCE_DEBOUNCE_SEL(x) ((u32)(((x >> 4) & 0x00000007)))
/** @} */


/** @defgroup dac_l_silence_control
 * @brief
 * @{
 */
#define AUDIO_CODEC_BIT_DAC_L_SILENCE_DET_EN        ((u32)0x00000001 << 0)                   /*!< R/W     0 Channel L DAC path silence detection enable0: Disable1: Enable */
#define AUDIO_CODEC_MASK_DAC_L_SILENCE_LEVEL_SEL    ((u32)0x00000007 << 1)                   /*!< R/W     1 Channel L DAC path silence detection threshold3'b000: -78db3'b001: -84db3'b010: -90db3'b011: -102db3'b100: -108db3'b101: -114db3'b110: -126db3'b111: -132db */
#define AUDIO_CODEC_DAC_L_SILENCE_LEVEL_SEL(x)      ((u32)(((x) & 0x00000007) << 1))
#define AUDIO_CODEC_GET_DAC_L_SILENCE_LEVEL_SEL(x)  ((u32)(((x >> 1) & 0x00000007)))
#define AUDIO_CODEC_MASK_DAC_L_SILENCE_DEBOUNCE_SEL ((u32)0x00000007 << 4)                   /*!< R/W   0x3 Channel L DAC path silence detection debounce (48K)3'b000: 80ms3'b001: 160ms3'b010: 320ms3'b011: 640ms3'b100: 1.28s3'b101: 2.56s3'b110: 5.12s3'b111: 0.16ms  */
#define AUDIO_CODEC_DAC_L_SILENCE_DEBOUNCE_SEL(x)   ((u32)(((x) & 0x00000007) << 4))
#define AUDIO_CODEC_GET_DAC_L_SILENCE_DEBOUNCE_SEL(x) ((u32)(((x >> 4) & 0x00000007)))
/** @} */


/** @defgroup dac_l_eq_ctrl
 * @brief
 * @{
 */
#define AUDIO_CODEC_BIT_DAC_L_BIQUAD_EN_0           ((u32)0x00000001 << 0)                   /*!< R/W     0 DAC L channel EQ 0-band biquad en  0: disable  1: enable */
#define AUDIO_CODEC_BIT_DAC_L_BIQUAD_EN_1           ((u32)0x00000001 << 1)                   /*!< R/W     0 DAC L channel EQ 1-band biquad en  0: disable  1: enable */
#define AUDIO_CODEC_BIT_DAC_L_BIQUAD_EN_2           ((u32)0x00000001 << 2)                   /*!< R/W     0 DAC L channel EQ 2-band biquad en  0: disable  1: enable */
#define AUDIO_CODEC_BIT_DAC_L_BIQUAD_EN_3           ((u32)0x00000001 << 3)                   /*!< R/W     0 DAC L channel EQ 3-band biquad en  0: disable  1: enable */
#define AUDIO_CODEC_BIT_DAC_L_BIQUAD_EN_4           ((u32)0x00000001 << 4)                   /*!< R/W     0 DAC L channel EQ 4-band biquad en  0: disable  1: enable */
/** @} */


/** @defgroup dac_l_biquad_h0_0
 * @brief
 * @{
 */
#define AUDIO_CODEC_MASK_DAC_L_BIQUAD_H0_0          ((u32)0x1FFFFFFF << 0)                   /*!< R/W   0x2000000 DAC L channel EQ 0-band coef. h0 2's complement in 4.25 format, i.e. the range is from -8~7.99.   */
#define AUDIO_CODEC_DAC_L_BIQUAD_H0_0(x)            ((u32)(((x) & 0x1FFFFFFF) << 0))
#define AUDIO_CODEC_GET_DAC_L_BIQUAD_H0_0(x)        ((u32)(((x >> 0) & 0x1FFFFFFF)))
/** @} */


/** @defgroup dac_l_biquad_b1_0
 * @brief
 * @{
 */
#define AUDIO_CODEC_MASK_DAC_L_BIQUAD_B1_0          ((u32)0x1FFFFFFF << 0)                   /*!< R/W     0 DAC L channel EQ 0-band coef. b1 2's complement in 4.25 format, i.e. the range is from -8~7.99.   */
#define AUDIO_CODEC_DAC_L_BIQUAD_B1_0(x)            ((u32)(((x) & 0x1FFFFFFF) << 0))
#define AUDIO_CODEC_GET_DAC_L_BIQUAD_B1_0(x)        ((u32)(((x >> 0) & 0x1FFFFFFF)))
/** @} */


/** @defgroup dac_l_biquad_b2_0
 * @brief
 * @{
 */
#define AUDIO_CODEC_MASK_DAC_L_BIQUAD_B2_0          ((u32)0x1FFFFFFF << 0)                   /*!< R/W     0 DAC L channel EQ 0-band coef. b2 2's complement in 4.25 format, i.e. the range is from -8~7.99.   */
#define AUDIO_CODEC_DAC_L_BIQUAD_B2_0(x)            ((u32)(((x) & 0x1FFFFFFF) << 0))
#define AUDIO_CODEC_GET_DAC_L_BIQUAD_B2_0(x)        ((u32)(((x >> 0) & 0x1FFFFFFF)))
/** @} */


/** @defgroup dac_l_biquad_a1_0
 * @brief
 * @{
 */
#define AUDIO_CODEC_MASK_DAC_L_BIQUAD_A1_0          ((u32)0x1FFFFFFF << 0)                   /*!< R/W     0 DAC L channel EQ 0-band coef. a1 2's complement in 4.25 format, i.e. the range is from -8~7.99.   */
#define AUDIO_CODEC_DAC_L_BIQUAD_A1_0(x)            ((u32)(((x) & 0x1FFFFFFF) << 0))
#define AUDIO_CODEC_GET_DAC_L_BIQUAD_A1_0(x)        ((u32)(((x >> 0) & 0x1FFFFFFF)))
/** @} */


/** @defgroup dac_l_biquad_a2_0
 * @brief
 * @{
 */
#define AUDIO_CODEC_MASK_DAC_L_BIQUAD_A2_0          ((u32)0x1FFFFFFF << 0)                   /*!< R/W     0 DAC L channel EQ 0-band coef. a2 2's complement in 4.25 format, i.e. the range is from -8~7.99.   */
#define AUDIO_CODEC_DAC_L_BIQUAD_A2_0(x)            ((u32)(((x) & 0x1FFFFFFF) << 0))
#define AUDIO_CODEC_GET_DAC_L_BIQUAD_A2_0(x)        ((u32)(((x >> 0) & 0x1FFFFFFF)))
/** @} */


/** @defgroup dac_l_biquad_h0_1
 * @brief
 * @{
 */
#define AUDIO_CODEC_MASK_DAC_L_BIQUAD_H0_1          ((u32)0x1FFFFFFF << 0)                   /*!< R/W   0x2000000 DAC L channel EQ 1-band coef. h0 2's complement in 4.25 format, i.e. the range is from -8~7.99.   */
#define AUDIO_CODEC_DAC_L_BIQUAD_H0_1(x)            ((u32)(((x) & 0x1FFFFFFF) << 0))
#define AUDIO_CODEC_GET_DAC_L_BIQUAD_H0_1(x)        ((u32)(((x >> 0) & 0x1FFFFFFF)))
/** @} */


/** @defgroup dac_l_biquad_b1_1
 * @brief
 * @{
 */
#define AUDIO_CODEC_MASK_DAC_L_BIQUAD_B1_1          ((u32)0x1FFFFFFF << 0)                   /*!< R/W     0 DAC L channel EQ 1-band coef. b1 2's complement in 4.25 format, i.e. the range is from -8~7.99.   */
#define AUDIO_CODEC_DAC_L_BIQUAD_B1_1(x)            ((u32)(((x) & 0x1FFFFFFF) << 0))
#define AUDIO_CODEC_GET_DAC_L_BIQUAD_B1_1(x)        ((u32)(((x >> 0) & 0x1FFFFFFF)))
/** @} */


/** @defgroup dac_l_biquad_b2_1
 * @brief
 * @{
 */
#define AUDIO_CODEC_MASK_DAC_L_BIQUAD_B2_1          ((u32)0x1FFFFFFF << 0)                   /*!< R/W     0 DAC L channel EQ 1-band coef. b2 2's complement in 4.25 format, i.e. the range is from -8~7.99.   */
#define AUDIO_CODEC_DAC_L_BIQUAD_B2_1(x)            ((u32)(((x) & 0x1FFFFFFF) << 0))
#define AUDIO_CODEC_GET_DAC_L_BIQUAD_B2_1(x)        ((u32)(((x >> 0) & 0x1FFFFFFF)))
/** @} */


/** @defgroup dac_l_biquad_a1_1
 * @brief
 * @{
 */
#define AUDIO_CODEC_MASK_DAC_L_BIQUAD_A1_1          ((u32)0x1FFFFFFF << 0)                   /*!< R/W     0 DAC L channel EQ 1-band coef. a1 2's complement in 4.25 format, i.e. the range is from -8~7.99.   */
#define AUDIO_CODEC_DAC_L_BIQUAD_A1_1(x)            ((u32)(((x) & 0x1FFFFFFF) << 0))
#define AUDIO_CODEC_GET_DAC_L_BIQUAD_A1_1(x)        ((u32)(((x >> 0) & 0x1FFFFFFF)))
/** @} */


/** @defgroup dac_l_biquad_a2_1
 * @brief
 * @{
 */
#define AUDIO_CODEC_MASK_DAC_L_BIQUAD_A2_1          ((u32)0x1FFFFFFF << 0)                   /*!< R/W     0 DAC L channel EQ 1-band coef. a2 2's complement in 4.25 format, i.e. the range is from -8~7.99.   */
#define AUDIO_CODEC_DAC_L_BIQUAD_A2_1(x)            ((u32)(((x) & 0x1FFFFFFF) << 0))
#define AUDIO_CODEC_GET_DAC_L_BIQUAD_A2_1(x)        ((u32)(((x >> 0) & 0x1FFFFFFF)))
/** @} */


/** @defgroup dac_l_biquad_h0_2
 * @brief
 * @{
 */
#define AUDIO_CODEC_MASK_DAC_L_BIQUAD_H0_2          ((u32)0x1FFFFFFF << 0)                   /*!< R/W   0x2000000 DAC L channel EQ 2-band coef. h0 2's complement in 4.25 format, i.e. the range is from -8~7.99.   */
#define AUDIO_CODEC_DAC_L_BIQUAD_H0_2(x)            ((u32)(((x) & 0x1FFFFFFF) << 0))
#define AUDIO_CODEC_GET_DAC_L_BIQUAD_H0_2(x)        ((u32)(((x >> 0) & 0x1FFFFFFF)))
/** @} */


/** @defgroup dac_l_biquad_b1_2
 * @brief
 * @{
 */
#define AUDIO_CODEC_MASK_DAC_L_BIQUAD_B1_2          ((u32)0x1FFFFFFF << 0)                   /*!< R/W     0 DAC L channel EQ 2-band coef. b1 2's complement in 4.25 format, i.e. the range is from -8~7.99.   */
#define AUDIO_CODEC_DAC_L_BIQUAD_B1_2(x)            ((u32)(((x) & 0x1FFFFFFF) << 0))
#define AUDIO_CODEC_GET_DAC_L_BIQUAD_B1_2(x)        ((u32)(((x >> 0) & 0x1FFFFFFF)))
/** @} */


/** @defgroup dac_l_biquad_b2_2
 * @brief
 * @{
 */
#define AUDIO_CODEC_MASK_DAC_L_BIQUAD_B2_2          ((u32)0x1FFFFFFF << 0)                   /*!< R/W     0 DAC L channel EQ 2-band coef. b2 2's complement in 4.25 format, i.e. the range is from -8~7.99.   */
#define AUDIO_CODEC_DAC_L_BIQUAD_B2_2(x)            ((u32)(((x) & 0x1FFFFFFF) << 0))
#define AUDIO_CODEC_GET_DAC_L_BIQUAD_B2_2(x)        ((u32)(((x >> 0) & 0x1FFFFFFF)))
/** @} */


/** @defgroup dac_l_biquad_a1_2
 * @brief
 * @{
 */
#define AUDIO_CODEC_MASK_DAC_L_BIQUAD_A1_2          ((u32)0x1FFFFFFF << 0)                   /*!< R/W     0 DAC L channel EQ 2-band coef. a1 2's complement in 4.25 format, i.e. the range is from -8~7.99.   */
#define AUDIO_CODEC_DAC_L_BIQUAD_A1_2(x)            ((u32)(((x) & 0x1FFFFFFF) << 0))
#define AUDIO_CODEC_GET_DAC_L_BIQUAD_A1_2(x)        ((u32)(((x >> 0) & 0x1FFFFFFF)))
/** @} */


/** @defgroup dac_l_biquad_a2_2
 * @brief
 * @{
 */
#define AUDIO_CODEC_MASK_DAC_L_BIQUAD_A2_2          ((u32)0x1FFFFFFF << 0)                   /*!< R/W     0 DAC L channel EQ 2-band coef. a2 2's complement in 4.25 format, i.e. the range is from -8~7.99.   */
#define AUDIO_CODEC_DAC_L_BIQUAD_A2_2(x)            ((u32)(((x) & 0x1FFFFFFF) << 0))
#define AUDIO_CODEC_GET_DAC_L_BIQUAD_A2_2(x)        ((u32)(((x >> 0) & 0x1FFFFFFF)))
/** @} */


/** @defgroup dac_l_biquad_h0_3
 * @brief
 * @{
 */
#define AUDIO_CODEC_MASK_DAC_L_BIQUAD_H0_3          ((u32)0x1FFFFFFF << 0)                   /*!< R/W   0x2000000 DAC L channel EQ 3-band coef. h0 2's complement in 4.25 format, i.e. the range is from -8~7.99.   */
#define AUDIO_CODEC_DAC_L_BIQUAD_H0_3(x)            ((u32)(((x) & 0x1FFFFFFF) << 0))
#define AUDIO_CODEC_GET_DAC_L_BIQUAD_H0_3(x)        ((u32)(((x >> 0) & 0x1FFFFFFF)))
/** @} */


/** @defgroup dac_l_biquad_b1_3
 * @brief
 * @{
 */
#define AUDIO_CODEC_MASK_DAC_L_BIQUAD_B1_3          ((u32)0x1FFFFFFF << 0)                   /*!< R/W     0 DAC L channel EQ 3-band coef. b1 2's complement in 4.25 format, i.e. the range is from -8~7.99.   */
#define AUDIO_CODEC_DAC_L_BIQUAD_B1_3(x)            ((u32)(((x) & 0x1FFFFFFF) << 0))
#define AUDIO_CODEC_GET_DAC_L_BIQUAD_B1_3(x)        ((u32)(((x >> 0) & 0x1FFFFFFF)))
/** @} */


/** @defgroup dac_l_biquad_b2_3
 * @brief
 * @{
 */
#define AUDIO_CODEC_MASK_DAC_L_BIQUAD_B2_3          ((u32)0x1FFFFFFF << 0)                   /*!< R/W     0 DAC L channel EQ 3-band coef. b2 2's complement in 4.25 format, i.e. the range is from -8~7.99.   */
#define AUDIO_CODEC_DAC_L_BIQUAD_B2_3(x)            ((u32)(((x) & 0x1FFFFFFF) << 0))
#define AUDIO_CODEC_GET_DAC_L_BIQUAD_B2_3(x)        ((u32)(((x >> 0) & 0x1FFFFFFF)))
/** @} */


/** @defgroup dac_l_biquad_a1_3
 * @brief
 * @{
 */
#define AUDIO_CODEC_MASK_DAC_L_BIQUAD_A1_3          ((u32)0x1FFFFFFF << 0)                   /*!< R/W     0 DAC L channel EQ 3-band coef. a1 2's complement in 4.25 format, i.e. the range is from -8~7.99.   */
#define AUDIO_CODEC_DAC_L_BIQUAD_A1_3(x)            ((u32)(((x) & 0x1FFFFFFF) << 0))
#define AUDIO_CODEC_GET_DAC_L_BIQUAD_A1_3(x)        ((u32)(((x >> 0) & 0x1FFFFFFF)))
/** @} */


/** @defgroup dac_l_biquad_a2_3
 * @brief
 * @{
 */
#define AUDIO_CODEC_MASK_DAC_L_BIQUAD_A2_3          ((u32)0x1FFFFFFF << 0)                   /*!< R/W     0 DAC L channel EQ 3-band coef. a2 2's complement in 4.25 format, i.e. the range is from -8~7.99.   */
#define AUDIO_CODEC_DAC_L_BIQUAD_A2_3(x)            ((u32)(((x) & 0x1FFFFFFF) << 0))
#define AUDIO_CODEC_GET_DAC_L_BIQUAD_A2_3(x)        ((u32)(((x >> 0) & 0x1FFFFFFF)))
/** @} */


/** @defgroup dac_l_biquad_h0_4
 * @brief
 * @{
 */
#define AUDIO_CODEC_MASK_DAC_L_BIQUAD_H0_4          ((u32)0x1FFFFFFF << 0)                   /*!< R/W   0x2000000 DAC L channel EQ 4-band coef. h0 2's complement in 4.25 format, i.e. the range is from -8~7.99.   */
#define AUDIO_CODEC_DAC_L_BIQUAD_H0_4(x)            ((u32)(((x) & 0x1FFFFFFF) << 0))
#define AUDIO_CODEC_GET_DAC_L_BIQUAD_H0_4(x)        ((u32)(((x >> 0) & 0x1FFFFFFF)))
/** @} */


/** @defgroup dac_l_biquad_b1_4
 * @brief
 * @{
 */
#define AUDIO_CODEC_MASK_DAC_L_BIQUAD_B1_4          ((u32)0x1FFFFFFF << 0)                   /*!< R/W     0 DAC L channel EQ 4-band coef. b1 2's complement in 4.25 format, i.e. the range is from -8~7.99.   */
#define AUDIO_CODEC_DAC_L_BIQUAD_B1_4(x)            ((u32)(((x) & 0x1FFFFFFF) << 0))
#define AUDIO_CODEC_GET_DAC_L_BIQUAD_B1_4(x)        ((u32)(((x >> 0) & 0x1FFFFFFF)))
/** @} */


/** @defgroup dac_l_biquad_b2_4
 * @brief
 * @{
 */
#define AUDIO_CODEC_MASK_DAC_L_BIQUAD_B2_4          ((u32)0x1FFFFFFF << 0)                   /*!< R/W     0 DAC L channel EQ 4-band coef. b2 2's complement in 4.25 format, i.e. the range is from -8~7.99.   */
#define AUDIO_CODEC_DAC_L_BIQUAD_B2_4(x)            ((u32)(((x) & 0x1FFFFFFF) << 0))
#define AUDIO_CODEC_GET_DAC_L_BIQUAD_B2_4(x)        ((u32)(((x >> 0) & 0x1FFFFFFF)))
/** @} */


/** @defgroup dac_l_biquad_a1_4
 * @brief
 * @{
 */
#define AUDIO_CODEC_MASK_DAC_L_BIQUAD_A1_4          ((u32)0x1FFFFFFF << 0)                   /*!< R/W     0 DAC L channel EQ 4-band coef. a1 2's complement in 4.25 format, i.e. the range is from -8~7.99.   */
#define AUDIO_CODEC_DAC_L_BIQUAD_A1_4(x)            ((u32)(((x) & 0x1FFFFFFF) << 0))
#define AUDIO_CODEC_GET_DAC_L_BIQUAD_A1_4(x)        ((u32)(((x >> 0) & 0x1FFFFFFF)))
/** @} */


/** @defgroup dac_l_biquad_a2_4
 * @brief
 * @{
 */
#define AUDIO_CODEC_MASK_DAC_L_BIQUAD_A2_4          ((u32)0x1FFFFFFF << 0)                   /*!< R/W     0 DAC L channel EQ 4-band coef. a2 2's complement in 4.25 format, i.e. the range is from -8~7.99.   */
#define AUDIO_CODEC_DAC_L_BIQUAD_A2_4(x)            ((u32)(((x) & 0x1FFFFFFF) << 0))
#define AUDIO_CODEC_GET_DAC_L_BIQUAD_A2_4(x)        ((u32)(((x >> 0) & 0x1FFFFFFF)))
/** @} */


/** @defgroup dac_r_eq_ctrl
 * @brief
 * @{
 */
#define AUDIO_CODEC_BIT_DAC_R_BIQUAD_EN_0           ((u32)0x00000001 << 0)                   /*!< R/W     0 reserved */
/** @} */


/** @defgroup adc_0_eq_ctrl
 * @brief
 * @{
 */
#define AUDIO_CODEC_BIT_ADC_0_BIQUAD_EN_0           ((u32)0x00000001 << 0)                   /*!< R/W     0 ADC channel 0 EQ 0-band biquad en  0: disable  1: enable */
#define AUDIO_CODEC_BIT_ADC_0_BIQUAD_EN_1           ((u32)0x00000001 << 1)                   /*!< R/W     0 ADC channel 0 EQ 1-band biquad en  0: disable  1: enable */
#define AUDIO_CODEC_BIT_ADC_0_BIQUAD_EN_2           ((u32)0x00000001 << 2)                   /*!< R/W     0 ADC channel 0 EQ 2-band biquad en  0: disable  1: enable */
#define AUDIO_CODEC_BIT_ADC_0_BIQUAD_EN_3           ((u32)0x00000001 << 3)                   /*!< R/W     0 ADC channel 0 EQ 3-band biquad en  0: disable  1: enable */
#define AUDIO_CODEC_BIT_ADC_0_BIQUAD_EN_4           ((u32)0x00000001 << 4)                   /*!< R/W     0 ADC channel 0 EQ 4-band biquad en  0: disable  1: enable */
/** @} */


/** @defgroup adc_0_biquad_h0_0
 * @brief
 * @{
 */
#define AUDIO_CODEC_MASK_ADC_0_BIQUAD_H0_0          ((u32)0x1FFFFFFF << 0)                   /*!< R/W   0x2000000 ADC channel 0 EQ 0-band coef. h0 2's complement in 4.25 format, i.e. the range is from -8~7.99.   */
#define AUDIO_CODEC_ADC_0_BIQUAD_H0_0(x)            ((u32)(((x) & 0x1FFFFFFF) << 0))
#define AUDIO_CODEC_GET_ADC_0_BIQUAD_H0_0(x)        ((u32)(((x >> 0) & 0x1FFFFFFF)))
/** @} */


/** @defgroup adc_0_biquad_b1_0
 * @brief
 * @{
 */
#define AUDIO_CODEC_MASK_ADC_0_BIQUAD_B1_0          ((u32)0x1FFFFFFF << 0)                   /*!< R/W     0 ADC channel 0 EQ 0-band coef. b1 2's complement in 4.25 format, i.e. the range is from -8~7.99.   */
#define AUDIO_CODEC_ADC_0_BIQUAD_B1_0(x)            ((u32)(((x) & 0x1FFFFFFF) << 0))
#define AUDIO_CODEC_GET_ADC_0_BIQUAD_B1_0(x)        ((u32)(((x >> 0) & 0x1FFFFFFF)))
/** @} */


/** @defgroup adc_0_biquad_b2_0
 * @brief
 * @{
 */
#define AUDIO_CODEC_MASK_ADC_0_BIQUAD_B2_0          ((u32)0x1FFFFFFF << 0)                   /*!< R/W     0 ADC channel 0 EQ 0-band coef. b2 2's complement in 4.25 format, i.e. the range is from -8~7.99.   */
#define AUDIO_CODEC_ADC_0_BIQUAD_B2_0(x)            ((u32)(((x) & 0x1FFFFFFF) << 0))
#define AUDIO_CODEC_GET_ADC_0_BIQUAD_B2_0(x)        ((u32)(((x >> 0) & 0x1FFFFFFF)))
/** @} */


/** @defgroup adc_0_biquad_a1_0
 * @brief
 * @{
 */
#define AUDIO_CODEC_MASK_ADC_0_BIQUAD_A1_0          ((u32)0x1FFFFFFF << 0)                   /*!< R/W     0 ADC channel 0 EQ 0-band coef. a1 2's complement in 4.25 format, i.e. the range is from -8~7.99.   */
#define AUDIO_CODEC_ADC_0_BIQUAD_A1_0(x)            ((u32)(((x) & 0x1FFFFFFF) << 0))
#define AUDIO_CODEC_GET_ADC_0_BIQUAD_A1_0(x)        ((u32)(((x >> 0) & 0x1FFFFFFF)))
/** @} */


/** @defgroup adc_0_biquad_a2_0
 * @brief
 * @{
 */
#define AUDIO_CODEC_MASK_ADC_0_BIQUAD_A2_0          ((u32)0x1FFFFFFF << 0)                   /*!< R/W     0 ADC channel 0 EQ 0-band coef. a2 2's complement in 4.25 format, i.e. the range is from -8~7.99.   */
#define AUDIO_CODEC_ADC_0_BIQUAD_A2_0(x)            ((u32)(((x) & 0x1FFFFFFF) << 0))
#define AUDIO_CODEC_GET_ADC_0_BIQUAD_A2_0(x)        ((u32)(((x >> 0) & 0x1FFFFFFF)))
/** @} */


/** @defgroup adc_0_biquad_h0_1
 * @brief
 * @{
 */
#define AUDIO_CODEC_MASK_ADC_0_BIQUAD_H0_1          ((u32)0x1FFFFFFF << 0)                   /*!< R/W   0x2000000 ADC channel 0 EQ 1-band coef. h0 2's complement in 4.25 format, i.e. the range is from -8~7.99.   */
#define AUDIO_CODEC_ADC_0_BIQUAD_H0_1(x)            ((u32)(((x) & 0x1FFFFFFF) << 0))
#define AUDIO_CODEC_GET_ADC_0_BIQUAD_H0_1(x)        ((u32)(((x >> 0) & 0x1FFFFFFF)))
/** @} */


/** @defgroup adc_0_biquad_b1_1
 * @brief
 * @{
 */
#define AUDIO_CODEC_MASK_ADC_0_BIQUAD_B1_1          ((u32)0x1FFFFFFF << 0)                   /*!< R/W     0 ADC channel 0 EQ 1-band coef. b1 2's complement in 4.25 format, i.e. the range is from -8~7.99.   */
#define AUDIO_CODEC_ADC_0_BIQUAD_B1_1(x)            ((u32)(((x) & 0x1FFFFFFF) << 0))
#define AUDIO_CODEC_GET_ADC_0_BIQUAD_B1_1(x)        ((u32)(((x >> 0) & 0x1FFFFFFF)))
/** @} */


/** @defgroup adc_0_biquad_b2_1
 * @brief
 * @{
 */
#define AUDIO_CODEC_MASK_ADC_0_BIQUAD_B2_1          ((u32)0x1FFFFFFF << 0)                   /*!< R/W     0 ADC channel 0 EQ 1-band coef. b2 2's complement in 4.25 format, i.e. the range is from -8~7.99.   */
#define AUDIO_CODEC_ADC_0_BIQUAD_B2_1(x)            ((u32)(((x) & 0x1FFFFFFF) << 0))
#define AUDIO_CODEC_GET_ADC_0_BIQUAD_B2_1(x)        ((u32)(((x >> 0) & 0x1FFFFFFF)))
/** @} */


/** @defgroup adc_0_biquad_a1_1
 * @brief
 * @{
 */
#define AUDIO_CODEC_MASK_ADC_0_BIQUAD_A1_1          ((u32)0x1FFFFFFF << 0)                   /*!< R/W     0 ADC channel 0 EQ 1-band coef. a1 2's complement in 4.25 format, i.e. the range is from -8~7.99.   */
#define AUDIO_CODEC_ADC_0_BIQUAD_A1_1(x)            ((u32)(((x) & 0x1FFFFFFF) << 0))
#define AUDIO_CODEC_GET_ADC_0_BIQUAD_A1_1(x)        ((u32)(((x >> 0) & 0x1FFFFFFF)))
/** @} */


/** @defgroup adc_0_biquad_a2_1
 * @brief
 * @{
 */
#define AUDIO_CODEC_MASK_ADC_0_BIQUAD_A2_1          ((u32)0x1FFFFFFF << 0)                   /*!< R/W     0 ADC channel 0 EQ 1-band coef. a2 2's complement in 4.25 format, i.e. the range is from -8~7.99.   */
#define AUDIO_CODEC_ADC_0_BIQUAD_A2_1(x)            ((u32)(((x) & 0x1FFFFFFF) << 0))
#define AUDIO_CODEC_GET_ADC_0_BIQUAD_A2_1(x)        ((u32)(((x >> 0) & 0x1FFFFFFF)))
/** @} */


/** @defgroup adc_0_biquad_h0_2
 * @brief
 * @{
 */
#define AUDIO_CODEC_MASK_ADC_0_BIQUAD_H0_2          ((u32)0x1FFFFFFF << 0)                   /*!< R/W   0x2000000 ADC channel 0 EQ 2-band coef. h0 2's complement in 4.25 format, i.e. the range is from -8~7.99.   */
#define AUDIO_CODEC_ADC_0_BIQUAD_H0_2(x)            ((u32)(((x) & 0x1FFFFFFF) << 0))
#define AUDIO_CODEC_GET_ADC_0_BIQUAD_H0_2(x)        ((u32)(((x >> 0) & 0x1FFFFFFF)))
/** @} */


/** @defgroup adc_0_biquad_b1_2
 * @brief
 * @{
 */
#define AUDIO_CODEC_MASK_ADC_0_BIQUAD_B1_2          ((u32)0x1FFFFFFF << 0)                   /*!< R/W     0 ADC channel 0 EQ 2-band coef. b1 2's complement in 4.25 format, i.e. the range is from -8~7.99.   */
#define AUDIO_CODEC_ADC_0_BIQUAD_B1_2(x)            ((u32)(((x) & 0x1FFFFFFF) << 0))
#define AUDIO_CODEC_GET_ADC_0_BIQUAD_B1_2(x)        ((u32)(((x >> 0) & 0x1FFFFFFF)))
/** @} */


/** @defgroup adc_0_biquad_b2_2
 * @brief
 * @{
 */
#define AUDIO_CODEC_MASK_ADC_0_BIQUAD_B2_2          ((u32)0x1FFFFFFF << 0)                   /*!< R/W     0 ADC channel 0 EQ 2-band coef. b2 2's complement in 4.25 format, i.e. the range is from -8~7.99.   */
#define AUDIO_CODEC_ADC_0_BIQUAD_B2_2(x)            ((u32)(((x) & 0x1FFFFFFF) << 0))
#define AUDIO_CODEC_GET_ADC_0_BIQUAD_B2_2(x)        ((u32)(((x >> 0) & 0x1FFFFFFF)))
/** @} */


/** @defgroup adc_0_biquad_a1_2
 * @brief
 * @{
 */
#define AUDIO_CODEC_MASK_ADC_0_BIQUAD_A1_2          ((u32)0x1FFFFFFF << 0)                   /*!< R/W     0 ADC channel 0 EQ 2-band coef. a1 2's complement in 4.25 format, i.e. the range is from -8~7.99.   */
#define AUDIO_CODEC_ADC_0_BIQUAD_A1_2(x)            ((u32)(((x) & 0x1FFFFFFF) << 0))
#define AUDIO_CODEC_GET_ADC_0_BIQUAD_A1_2(x)        ((u32)(((x >> 0) & 0x1FFFFFFF)))
/** @} */


/** @defgroup adc_0_biquad_a2_2
 * @brief
 * @{
 */
#define AUDIO_CODEC_MASK_ADC_0_BIQUAD_A2_2          ((u32)0x1FFFFFFF << 0)                   /*!< R/W     0 ADC channel 0 EQ 2-band coef. a2 2's complement in 4.25 format, i.e. the range is from -8~7.99.   */
#define AUDIO_CODEC_ADC_0_BIQUAD_A2_2(x)            ((u32)(((x) & 0x1FFFFFFF) << 0))
#define AUDIO_CODEC_GET_ADC_0_BIQUAD_A2_2(x)        ((u32)(((x >> 0) & 0x1FFFFFFF)))
/** @} */


/** @defgroup adc_0_biquad_h0_3
 * @brief
 * @{
 */
#define AUDIO_CODEC_MASK_ADC_0_BIQUAD_H0_3          ((u32)0x1FFFFFFF << 0)                   /*!< R/W   0x2000000 ADC channel 0 EQ 3-band coef. h0 2's complement in 4.25 format, i.e. the range is from -8~7.99.   */
#define AUDIO_CODEC_ADC_0_BIQUAD_H0_3(x)            ((u32)(((x) & 0x1FFFFFFF) << 0))
#define AUDIO_CODEC_GET_ADC_0_BIQUAD_H0_3(x)        ((u32)(((x >> 0) & 0x1FFFFFFF)))
/** @} */


/** @defgroup adc_0_biquad_b1_3
 * @brief
 * @{
 */
#define AUDIO_CODEC_MASK_ADC_0_BIQUAD_B1_3          ((u32)0x1FFFFFFF << 0)                   /*!< R/W     0 ADC channel 0 EQ 3-band coef. b1 2's complement in 4.25 format, i.e. the range is from -8~7.99.   */
#define AUDIO_CODEC_ADC_0_BIQUAD_B1_3(x)            ((u32)(((x) & 0x1FFFFFFF) << 0))
#define AUDIO_CODEC_GET_ADC_0_BIQUAD_B1_3(x)        ((u32)(((x >> 0) & 0x1FFFFFFF)))
/** @} */


/** @defgroup adc_0_biquad_b2_3
 * @brief
 * @{
 */
#define AUDIO_CODEC_MASK_ADC_0_BIQUAD_B2_3          ((u32)0x1FFFFFFF << 0)                   /*!< R/W     0 ADC channel 0 EQ 3-band coef. b2 2's complement in 4.25 format, i.e. the range is from -8~7.99.   */
#define AUDIO_CODEC_ADC_0_BIQUAD_B2_3(x)            ((u32)(((x) & 0x1FFFFFFF) << 0))
#define AUDIO_CODEC_GET_ADC_0_BIQUAD_B2_3(x)        ((u32)(((x >> 0) & 0x1FFFFFFF)))
/** @} */


/** @defgroup adc_0_biquad_a1_3
 * @brief
 * @{
 */
#define AUDIO_CODEC_MASK_ADC_0_BIQUAD_A1_3          ((u32)0x1FFFFFFF << 0)                   /*!< R/W     0 ADC channel 0 EQ 3-band coef. a1 2's complement in 4.25 format, i.e. the range is from -8~7.99.   */
#define AUDIO_CODEC_ADC_0_BIQUAD_A1_3(x)            ((u32)(((x) & 0x1FFFFFFF) << 0))
#define AUDIO_CODEC_GET_ADC_0_BIQUAD_A1_3(x)        ((u32)(((x >> 0) & 0x1FFFFFFF)))
/** @} */


/** @defgroup adc_0_biquad_a2_3
 * @brief
 * @{
 */
#define AUDIO_CODEC_MASK_ADC_0_BIQUAD_A2_3          ((u32)0x1FFFFFFF << 0)                   /*!< R/W     0 ADC channel 0 EQ 3-band coef. a2 2's complement in 4.25 format, i.e. the range is from -8~7.99.   */
#define AUDIO_CODEC_ADC_0_BIQUAD_A2_3(x)            ((u32)(((x) & 0x1FFFFFFF) << 0))
#define AUDIO_CODEC_GET_ADC_0_BIQUAD_A2_3(x)        ((u32)(((x >> 0) & 0x1FFFFFFF)))
/** @} */


/** @defgroup adc_0_biquad_h0_4
 * @brief
 * @{
 */
#define AUDIO_CODEC_MASK_ADC_0_BIQUAD_H0_4          ((u32)0x1FFFFFFF << 0)                   /*!< R/W   0x2000000 ADC channel 0 EQ 4-band coef. h0 2's complement in 4.25 format, i.e. the range is from -8~7.99.   */
#define AUDIO_CODEC_ADC_0_BIQUAD_H0_4(x)            ((u32)(((x) & 0x1FFFFFFF) << 0))
#define AUDIO_CODEC_GET_ADC_0_BIQUAD_H0_4(x)        ((u32)(((x >> 0) & 0x1FFFFFFF)))
/** @} */


/** @defgroup adc_0_biquad_b1_4
 * @brief
 * @{
 */
#define AUDIO_CODEC_MASK_ADC_0_BIQUAD_B1_4          ((u32)0x1FFFFFFF << 0)                   /*!< R/W     0 ADC channel 0 EQ 4-band coef. b1 2's complement in 4.25 format, i.e. the range is from -8~7.99.   */
#define AUDIO_CODEC_ADC_0_BIQUAD_B1_4(x)            ((u32)(((x) & 0x1FFFFFFF) << 0))
#define AUDIO_CODEC_GET_ADC_0_BIQUAD_B1_4(x)        ((u32)(((x >> 0) & 0x1FFFFFFF)))
/** @} */


/** @defgroup adc_0_biquad_b2_4
 * @brief
 * @{
 */
#define AUDIO_CODEC_MASK_ADC_0_BIQUAD_B2_4          ((u32)0x1FFFFFFF << 0)                   /*!< R/W     0 ADC channel 0 EQ 4-band coef. b2 2's complement in 4.25 format, i.e. the range is from -8~7.99.   */
#define AUDIO_CODEC_ADC_0_BIQUAD_B2_4(x)            ((u32)(((x) & 0x1FFFFFFF) << 0))
#define AUDIO_CODEC_GET_ADC_0_BIQUAD_B2_4(x)        ((u32)(((x >> 0) & 0x1FFFFFFF)))
/** @} */


/** @defgroup adc_0_biquad_a1_4
 * @brief
 * @{
 */
#define AUDIO_CODEC_MASK_ADC_0_BIQUAD_A1_4          ((u32)0x1FFFFFFF << 0)                   /*!< R/W     0 ADC channel 0 EQ 4-band coef. a1 2's complement in 4.25 format, i.e. the range is from -8~7.99.   */
#define AUDIO_CODEC_ADC_0_BIQUAD_A1_4(x)            ((u32)(((x) & 0x1FFFFFFF) << 0))
#define AUDIO_CODEC_GET_ADC_0_BIQUAD_A1_4(x)        ((u32)(((x >> 0) & 0x1FFFFFFF)))
/** @} */


/** @defgroup adc_0_biquad_a2_4
 * @brief
 * @{
 */
#define AUDIO_CODEC_MASK_ADC_0_BIQUAD_A2_4          ((u32)0x1FFFFFFF << 0)                   /*!< R/W     0 ADC channel 0 EQ 4-band coef. a2 2's complement in 4.25 format, i.e. the range is from -8~7.99.   */
#define AUDIO_CODEC_ADC_0_BIQUAD_A2_4(x)            ((u32)(((x) & 0x1FFFFFFF) << 0))
#define AUDIO_CODEC_GET_ADC_0_BIQUAD_A2_4(x)        ((u32)(((x >> 0) & 0x1FFFFFFF)))
/** @} */


/** @defgroup adc_1_eq_ctrl
 * @brief
 * @{
 */
#define AUDIO_CODEC_BIT_ADC_1_BIQUAD_EN_0           ((u32)0x00000001 << 0)                   /*!< R/W     0 ADC channel 1 EQ 0-band biquad en  0: disable  1: enable */
#define AUDIO_CODEC_BIT_ADC_1_BIQUAD_EN_1           ((u32)0x00000001 << 1)                   /*!< R/W     0 ADC channel 1 EQ 1-band biquad en  0: disable  1: enable */
#define AUDIO_CODEC_BIT_ADC_1_BIQUAD_EN_2           ((u32)0x00000001 << 2)                   /*!< R/W     0 ADC channel 1 EQ 2-band biquad en  0: disable  1: enable */
#define AUDIO_CODEC_BIT_ADC_1_BIQUAD_EN_3           ((u32)0x00000001 << 3)                   /*!< R/W     0 ADC channel 1 EQ 3-band biquad en  0: disable  1: enable */
#define AUDIO_CODEC_BIT_ADC_1_BIQUAD_EN_4           ((u32)0x00000001 << 4)                   /*!< R/W     0 ADC channel 1 EQ 4-band biquad en  0: disable  1: enable */
/** @} */


/** @defgroup adc_1_biquad_h0_0
 * @brief
 * @{
 */
#define AUDIO_CODEC_MASK_ADC_1_BIQUAD_H0_0          ((u32)0x1FFFFFFF << 0)                   /*!< R/W   0x2000000 ADC channel 1 EQ 0-band coef. h0 2's complement in 4.25 format, i.e. the range is from -8~7.99.   */
#define AUDIO_CODEC_ADC_1_BIQUAD_H0_0(x)            ((u32)(((x) & 0x1FFFFFFF) << 0))
#define AUDIO_CODEC_GET_ADC_1_BIQUAD_H0_0(x)        ((u32)(((x >> 0) & 0x1FFFFFFF)))
/** @} */


/** @defgroup adc_1_biquad_b1_0
 * @brief
 * @{
 */
#define AUDIO_CODEC_MASK_ADC_1_BIQUAD_B1_0          ((u32)0x1FFFFFFF << 0)                   /*!< R/W     0 ADC channel 1 EQ 0-band coef. b1 2's complement in 4.25 format, i.e. the range is from -8~7.99.   */
#define AUDIO_CODEC_ADC_1_BIQUAD_B1_0(x)            ((u32)(((x) & 0x1FFFFFFF) << 0))
#define AUDIO_CODEC_GET_ADC_1_BIQUAD_B1_0(x)        ((u32)(((x >> 0) & 0x1FFFFFFF)))
/** @} */


/** @defgroup adc_1_biquad_b2_0
 * @brief
 * @{
 */
#define AUDIO_CODEC_MASK_ADC_1_BIQUAD_B2_0          ((u32)0x1FFFFFFF << 0)                   /*!< R/W     0 ADC channel 1 EQ 0-band coef. b2 2's complement in 4.25 format, i.e. the range is from -8~7.99.   */
#define AUDIO_CODEC_ADC_1_BIQUAD_B2_0(x)            ((u32)(((x) & 0x1FFFFFFF) << 0))
#define AUDIO_CODEC_GET_ADC_1_BIQUAD_B2_0(x)        ((u32)(((x >> 0) & 0x1FFFFFFF)))
/** @} */


/** @defgroup adc_1_biquad_a1_0
 * @brief
 * @{
 */
#define AUDIO_CODEC_MASK_ADC_1_BIQUAD_A1_0          ((u32)0x1FFFFFFF << 0)                   /*!< R/W     0 ADC channel 1 EQ 0-band coef. a1 2's complement in 4.25 format, i.e. the range is from -8~7.99.   */
#define AUDIO_CODEC_ADC_1_BIQUAD_A1_0(x)            ((u32)(((x) & 0x1FFFFFFF) << 0))
#define AUDIO_CODEC_GET_ADC_1_BIQUAD_A1_0(x)        ((u32)(((x >> 0) & 0x1FFFFFFF)))
/** @} */


/** @defgroup adc_1_biquad_a2_0
 * @brief
 * @{
 */
#define AUDIO_CODEC_MASK_ADC_1_BIQUAD_A2_0          ((u32)0x1FFFFFFF << 0)                   /*!< R/W     0 ADC channel 1 EQ 0-band coef. a2 2's complement in 4.25 format, i.e. the range is from -8~7.99.   */
#define AUDIO_CODEC_ADC_1_BIQUAD_A2_0(x)            ((u32)(((x) & 0x1FFFFFFF) << 0))
#define AUDIO_CODEC_GET_ADC_1_BIQUAD_A2_0(x)        ((u32)(((x >> 0) & 0x1FFFFFFF)))
/** @} */


/** @defgroup adc_1_biquad_h0_1
 * @brief
 * @{
 */
#define AUDIO_CODEC_MASK_ADC_1_BIQUAD_H0_1          ((u32)0x1FFFFFFF << 0)                   /*!< R/W   0x2000000 ADC channel 1 EQ 1-band coef. h0 2's complement in 4.25 format, i.e. the range is from -8~7.99.   */
#define AUDIO_CODEC_ADC_1_BIQUAD_H0_1(x)            ((u32)(((x) & 0x1FFFFFFF) << 0))
#define AUDIO_CODEC_GET_ADC_1_BIQUAD_H0_1(x)        ((u32)(((x >> 0) & 0x1FFFFFFF)))
/** @} */


/** @defgroup adc_1_biquad_b1_1
 * @brief
 * @{
 */
#define AUDIO_CODEC_MASK_ADC_1_BIQUAD_B1_1          ((u32)0x1FFFFFFF << 0)                   /*!< R/W     0 ADC channel 1 EQ 1-band coef. b1 2's complement in 4.25 format, i.e. the range is from -8~7.99.   */
#define AUDIO_CODEC_ADC_1_BIQUAD_B1_1(x)            ((u32)(((x) & 0x1FFFFFFF) << 0))
#define AUDIO_CODEC_GET_ADC_1_BIQUAD_B1_1(x)        ((u32)(((x >> 0) & 0x1FFFFFFF)))
/** @} */


/** @defgroup adc_1_biquad_b2_1
 * @brief
 * @{
 */
#define AUDIO_CODEC_MASK_ADC_1_BIQUAD_B2_1          ((u32)0x1FFFFFFF << 0)                   /*!< R/W     0 ADC channel 1 EQ 1-band coef. b2 2's complement in 4.25 format, i.e. the range is from -8~7.99.   */
#define AUDIO_CODEC_ADC_1_BIQUAD_B2_1(x)            ((u32)(((x) & 0x1FFFFFFF) << 0))
#define AUDIO_CODEC_GET_ADC_1_BIQUAD_B2_1(x)        ((u32)(((x >> 0) & 0x1FFFFFFF)))
/** @} */


/** @defgroup adc_1_biquad_a1_1
 * @brief
 * @{
 */
#define AUDIO_CODEC_MASK_ADC_1_BIQUAD_A1_1          ((u32)0x1FFFFFFF << 0)                   /*!< R/W     0 ADC channel 1 EQ 1-band coef. a1 2's complement in 4.25 format, i.e. the range is from -8~7.99.   */
#define AUDIO_CODEC_ADC_1_BIQUAD_A1_1(x)            ((u32)(((x) & 0x1FFFFFFF) << 0))
#define AUDIO_CODEC_GET_ADC_1_BIQUAD_A1_1(x)        ((u32)(((x >> 0) & 0x1FFFFFFF)))
/** @} */


/** @defgroup adc_1_biquad_a2_1
 * @brief
 * @{
 */
#define AUDIO_CODEC_MASK_ADC_1_BIQUAD_A2_1          ((u32)0x1FFFFFFF << 0)                   /*!< R/W     0 ADC channel 1 EQ 1-band coef. a2 2's complement in 4.25 format, i.e. the range is from -8~7.99.   */
#define AUDIO_CODEC_ADC_1_BIQUAD_A2_1(x)            ((u32)(((x) & 0x1FFFFFFF) << 0))
#define AUDIO_CODEC_GET_ADC_1_BIQUAD_A2_1(x)        ((u32)(((x >> 0) & 0x1FFFFFFF)))
/** @} */


/** @defgroup adc_1_biquad_h0_2
 * @brief
 * @{
 */
#define AUDIO_CODEC_MASK_ADC_1_BIQUAD_H0_2          ((u32)0x1FFFFFFF << 0)                   /*!< R/W   0x2000000 ADC channel 1 EQ 2-band coef. h0 2's complement in 4.25 format, i.e. the range is from -8~7.99.   */
#define AUDIO_CODEC_ADC_1_BIQUAD_H0_2(x)            ((u32)(((x) & 0x1FFFFFFF) << 0))
#define AUDIO_CODEC_GET_ADC_1_BIQUAD_H0_2(x)        ((u32)(((x >> 0) & 0x1FFFFFFF)))
/** @} */


/** @defgroup adc_1_biquad_b1_2
 * @brief
 * @{
 */
#define AUDIO_CODEC_MASK_ADC_1_BIQUAD_B1_2          ((u32)0x1FFFFFFF << 0)                   /*!< R/W     0 ADC channel 1 EQ 2-band coef. b1 2's complement in 4.25 format, i.e. the range is from -8~7.99.   */
#define AUDIO_CODEC_ADC_1_BIQUAD_B1_2(x)            ((u32)(((x) & 0x1FFFFFFF) << 0))
#define AUDIO_CODEC_GET_ADC_1_BIQUAD_B1_2(x)        ((u32)(((x >> 0) & 0x1FFFFFFF)))
/** @} */


/** @defgroup adc_1_biquad_b2_2
 * @brief
 * @{
 */
#define AUDIO_CODEC_MASK_ADC_1_BIQUAD_B2_2          ((u32)0x1FFFFFFF << 0)                   /*!< R/W     0 ADC channel 1 EQ 2-band coef. b2 2's complement in 4.25 format, i.e. the range is from -8~7.99.   */
#define AUDIO_CODEC_ADC_1_BIQUAD_B2_2(x)            ((u32)(((x) & 0x1FFFFFFF) << 0))
#define AUDIO_CODEC_GET_ADC_1_BIQUAD_B2_2(x)        ((u32)(((x >> 0) & 0x1FFFFFFF)))
/** @} */


/** @defgroup adc_1_biquad_a1_2
 * @brief
 * @{
 */
#define AUDIO_CODEC_MASK_ADC_1_BIQUAD_A1_2          ((u32)0x1FFFFFFF << 0)                   /*!< R/W     0 ADC channel 1 EQ 2-band coef. a1 2's complement in 4.25 format, i.e. the range is from -8~7.99.   */
#define AUDIO_CODEC_ADC_1_BIQUAD_A1_2(x)            ((u32)(((x) & 0x1FFFFFFF) << 0))
#define AUDIO_CODEC_GET_ADC_1_BIQUAD_A1_2(x)        ((u32)(((x >> 0) & 0x1FFFFFFF)))
/** @} */


/** @defgroup adc_1_biquad_a2_2
 * @brief
 * @{
 */
#define AUDIO_CODEC_MASK_ADC_1_BIQUAD_A2_2          ((u32)0x1FFFFFFF << 0)                   /*!< R/W     0 ADC channel 1 EQ 2-band coef. a2 2's complement in 4.25 format, i.e. the range is from -8~7.99.   */
#define AUDIO_CODEC_ADC_1_BIQUAD_A2_2(x)            ((u32)(((x) & 0x1FFFFFFF) << 0))
#define AUDIO_CODEC_GET_ADC_1_BIQUAD_A2_2(x)        ((u32)(((x >> 0) & 0x1FFFFFFF)))
/** @} */


/** @defgroup adc_1_biquad_h0_3
 * @brief
 * @{
 */
#define AUDIO_CODEC_MASK_ADC_1_BIQUAD_H0_3          ((u32)0x1FFFFFFF << 0)                   /*!< R/W   0x2000000 ADC channel 1 EQ 3-band coef. h0 2's complement in 4.25 format, i.e. the range is from -8~7.99.   */
#define AUDIO_CODEC_ADC_1_BIQUAD_H0_3(x)            ((u32)(((x) & 0x1FFFFFFF) << 0))
#define AUDIO_CODEC_GET_ADC_1_BIQUAD_H0_3(x)        ((u32)(((x >> 0) & 0x1FFFFFFF)))
/** @} */


/** @defgroup adc_1_biquad_b1_3
 * @brief
 * @{
 */
#define AUDIO_CODEC_MASK_ADC_1_BIQUAD_B1_3          ((u32)0x1FFFFFFF << 0)                   /*!< R/W     0 ADC channel 1 EQ 3-band coef. b1 2's complement in 4.25 format, i.e. the range is from -8~7.99.   */
#define AUDIO_CODEC_ADC_1_BIQUAD_B1_3(x)            ((u32)(((x) & 0x1FFFFFFF) << 0))
#define AUDIO_CODEC_GET_ADC_1_BIQUAD_B1_3(x)        ((u32)(((x >> 0) & 0x1FFFFFFF)))
/** @} */


/** @defgroup adc_1_biquad_b2_3
 * @brief
 * @{
 */
#define AUDIO_CODEC_MASK_ADC_1_BIQUAD_B2_3          ((u32)0x1FFFFFFF << 0)                   /*!< R/W     0 ADC channel 1 EQ 3-band coef. b2 2's complement in 4.25 format, i.e. the range is from -8~7.99.   */
#define AUDIO_CODEC_ADC_1_BIQUAD_B2_3(x)            ((u32)(((x) & 0x1FFFFFFF) << 0))
#define AUDIO_CODEC_GET_ADC_1_BIQUAD_B2_3(x)        ((u32)(((x >> 0) & 0x1FFFFFFF)))
/** @} */


/** @defgroup adc_1_biquad_a1_3
 * @brief
 * @{
 */
#define AUDIO_CODEC_MASK_ADC_1_BIQUAD_A1_3          ((u32)0x1FFFFFFF << 0)                   /*!< R/W     0 ADC channel 1 EQ 3-band coef. a1 2's complement in 4.25 format, i.e. the range is from -8~7.99.   */
#define AUDIO_CODEC_ADC_1_BIQUAD_A1_3(x)            ((u32)(((x) & 0x1FFFFFFF) << 0))
#define AUDIO_CODEC_GET_ADC_1_BIQUAD_A1_3(x)        ((u32)(((x >> 0) & 0x1FFFFFFF)))
/** @} */


/** @defgroup adc_1_biquad_a2_3
 * @brief
 * @{
 */
#define AUDIO_CODEC_MASK_ADC_1_BIQUAD_A2_3          ((u32)0x1FFFFFFF << 0)                   /*!< R/W     0 ADC channel 1 EQ 3-band coef. a2 2's complement in 4.25 format, i.e. the range is from -8~7.99.   */
#define AUDIO_CODEC_ADC_1_BIQUAD_A2_3(x)            ((u32)(((x) & 0x1FFFFFFF) << 0))
#define AUDIO_CODEC_GET_ADC_1_BIQUAD_A2_3(x)        ((u32)(((x >> 0) & 0x1FFFFFFF)))
/** @} */


/** @defgroup adc_1_biquad_h0_4
 * @brief
 * @{
 */
#define AUDIO_CODEC_MASK_ADC_1_BIQUAD_H0_4          ((u32)0x1FFFFFFF << 0)                   /*!< R/W   0x2000000 ADC channel 1 EQ 4-band coef. h0 2's complement in 4.25 format, i.e. the range is from -8~7.99.   */
#define AUDIO_CODEC_ADC_1_BIQUAD_H0_4(x)            ((u32)(((x) & 0x1FFFFFFF) << 0))
#define AUDIO_CODEC_GET_ADC_1_BIQUAD_H0_4(x)        ((u32)(((x >> 0) & 0x1FFFFFFF)))
/** @} */


/** @defgroup adc_1_biquad_b1_4
 * @brief
 * @{
 */
#define AUDIO_CODEC_MASK_ADC_1_BIQUAD_B1_4          ((u32)0x1FFFFFFF << 0)                   /*!< R/W     0 ADC channel 1 EQ 4-band coef. b1 2's complement in 4.25 format, i.e. the range is from -8~7.99.   */
#define AUDIO_CODEC_ADC_1_BIQUAD_B1_4(x)            ((u32)(((x) & 0x1FFFFFFF) << 0))
#define AUDIO_CODEC_GET_ADC_1_BIQUAD_B1_4(x)        ((u32)(((x >> 0) & 0x1FFFFFFF)))
/** @} */


/** @defgroup adc_1_biquad_b2_4
 * @brief
 * @{
 */
#define AUDIO_CODEC_MASK_ADC_1_BIQUAD_B2_4          ((u32)0x1FFFFFFF << 0)                   /*!< R/W     0 ADC channel 1 EQ 4-band coef. b2 2's complement in 4.25 format, i.e. the range is from -8~7.99.   */
#define AUDIO_CODEC_ADC_1_BIQUAD_B2_4(x)            ((u32)(((x) & 0x1FFFFFFF) << 0))
#define AUDIO_CODEC_GET_ADC_1_BIQUAD_B2_4(x)        ((u32)(((x >> 0) & 0x1FFFFFFF)))
/** @} */


/** @defgroup adc_1_biquad_a1_4
 * @brief
 * @{
 */
#define AUDIO_CODEC_MASK_ADC_1_BIQUAD_A1_4          ((u32)0x1FFFFFFF << 0)                   /*!< R/W     0 ADC channel 1 EQ 4-band coef. a1 2's complement in 4.25 format, i.e. the range is from -8~7.99.   */
#define AUDIO_CODEC_ADC_1_BIQUAD_A1_4(x)            ((u32)(((x) & 0x1FFFFFFF) << 0))
#define AUDIO_CODEC_GET_ADC_1_BIQUAD_A1_4(x)        ((u32)(((x >> 0) & 0x1FFFFFFF)))
/** @} */


/** @defgroup adc_1_biquad_a2_4
 * @brief
 * @{
 */
#define AUDIO_CODEC_MASK_ADC_1_BIQUAD_A2_4          ((u32)0x1FFFFFFF << 0)                   /*!< R/W     0 ADC channel 1 EQ 4-band coef. a2 2's complement in 4.25 format, i.e. the range is from -8~7.99.   */
#define AUDIO_CODEC_ADC_1_BIQUAD_A2_4(x)            ((u32)(((x) & 0x1FFFFFFF) << 0))
#define AUDIO_CODEC_GET_ADC_1_BIQUAD_A2_4(x)        ((u32)(((x >> 0) & 0x1FFFFFFF)))
/** @} */


/** @defgroup adc_2_eq_ctrl
 * @brief
 * @{
 */
#define AUDIO_CODEC_BIT_ADC_2_BIQUAD_EN_0           ((u32)0x00000001 << 0)                   /*!< R/W     0 ADC channel 2 EQ 0-band biquad en  0: disable  1: enable */
#define AUDIO_CODEC_BIT_ADC_2_BIQUAD_EN_1           ((u32)0x00000001 << 1)                   /*!< R/W     0 ADC channel 2 EQ 1-band biquad en  0: disable  1: enable */
#define AUDIO_CODEC_BIT_ADC_2_BIQUAD_EN_2           ((u32)0x00000001 << 2)                   /*!< R/W     0 ADC channel 2 EQ 2-band biquad en  0: disable  1: enable */
#define AUDIO_CODEC_BIT_ADC_2_BIQUAD_EN_3           ((u32)0x00000001 << 3)                   /*!< R/W     0 ADC channel 2 EQ 3-band biquad en  0: disable  1: enable */
#define AUDIO_CODEC_BIT_ADC_2_BIQUAD_EN_4           ((u32)0x00000001 << 4)                   /*!< R/W     0 ADC channel 2 EQ 4-band biquad en  0: disable  1: enable */
/** @} */


/** @defgroup adc_2_biquad_h0_0
 * @brief
 * @{
 */
#define AUDIO_CODEC_MASK_ADC_2_BIQUAD_H0_0          ((u32)0x1FFFFFFF << 0)                   /*!< R/W   0x2000000 ADC channel 2 EQ 0-band coef. h0 2's complement in 4.25 format, i.e. the range is from -8~7.99.   */
#define AUDIO_CODEC_ADC_2_BIQUAD_H0_0(x)            ((u32)(((x) & 0x1FFFFFFF) << 0))
#define AUDIO_CODEC_GET_ADC_2_BIQUAD_H0_0(x)        ((u32)(((x >> 0) & 0x1FFFFFFF)))
/** @} */


/** @defgroup adc_2_biquad_b1_0
 * @brief
 * @{
 */
#define AUDIO_CODEC_MASK_ADC_2_BIQUAD_B1_0          ((u32)0x1FFFFFFF << 0)                   /*!< R/W     0 ADC channel 2 EQ 0-band coef. b1 2's complement in 4.25 format, i.e. the range is from -8~7.99.   */
#define AUDIO_CODEC_ADC_2_BIQUAD_B1_0(x)            ((u32)(((x) & 0x1FFFFFFF) << 0))
#define AUDIO_CODEC_GET_ADC_2_BIQUAD_B1_0(x)        ((u32)(((x >> 0) & 0x1FFFFFFF)))
/** @} */


/** @defgroup adc_2_biquad_b2_0
 * @brief
 * @{
 */
#define AUDIO_CODEC_MASK_ADC_2_BIQUAD_B2_0          ((u32)0x1FFFFFFF << 0)                   /*!< R/W     0 ADC channel 2 EQ 0-band coef. b2 2's complement in 4.25 format, i.e. the range is from -8~7.99.   */
#define AUDIO_CODEC_ADC_2_BIQUAD_B2_0(x)            ((u32)(((x) & 0x1FFFFFFF) << 0))
#define AUDIO_CODEC_GET_ADC_2_BIQUAD_B2_0(x)        ((u32)(((x >> 0) & 0x1FFFFFFF)))
/** @} */


/** @defgroup adc_2_biquad_a1_0
 * @brief
 * @{
 */
#define AUDIO_CODEC_MASK_ADC_2_BIQUAD_A1_0          ((u32)0x1FFFFFFF << 0)                   /*!< R/W     0 ADC channel 2 EQ 0-band coef. a1 2's complement in 4.25 format, i.e. the range is from -8~7.99.   */
#define AUDIO_CODEC_ADC_2_BIQUAD_A1_0(x)            ((u32)(((x) & 0x1FFFFFFF) << 0))
#define AUDIO_CODEC_GET_ADC_2_BIQUAD_A1_0(x)        ((u32)(((x >> 0) & 0x1FFFFFFF)))
/** @} */


/** @defgroup adc_2_biquad_a2_0
 * @brief
 * @{
 */
#define AUDIO_CODEC_MASK_ADC_2_BIQUAD_A2_0          ((u32)0x1FFFFFFF << 0)                   /*!< R/W     0 ADC channel 2 EQ 0-band coef. a2 2's complement in 4.25 format, i.e. the range is from -8~7.99.   */
#define AUDIO_CODEC_ADC_2_BIQUAD_A2_0(x)            ((u32)(((x) & 0x1FFFFFFF) << 0))
#define AUDIO_CODEC_GET_ADC_2_BIQUAD_A2_0(x)        ((u32)(((x >> 0) & 0x1FFFFFFF)))
/** @} */


/** @defgroup adc_2_biquad_h0_1
 * @brief
 * @{
 */
#define AUDIO_CODEC_MASK_ADC_2_BIQUAD_H0_1          ((u32)0x1FFFFFFF << 0)                   /*!< R/W   0x2000000 ADC channel 2 EQ 1-band coef. h0 2's complement in 4.25 format, i.e. the range is from -8~7.99.   */
#define AUDIO_CODEC_ADC_2_BIQUAD_H0_1(x)            ((u32)(((x) & 0x1FFFFFFF) << 0))
#define AUDIO_CODEC_GET_ADC_2_BIQUAD_H0_1(x)        ((u32)(((x >> 0) & 0x1FFFFFFF)))
/** @} */


/** @defgroup adc_2_biquad_b1_1
 * @brief
 * @{
 */
#define AUDIO_CODEC_MASK_ADC_2_BIQUAD_B1_1          ((u32)0x1FFFFFFF << 0)                   /*!< R/W     0 ADC channel 2 EQ 1-band coef. b1 2's complement in 4.25 format, i.e. the range is from -8~7.99.   */
#define AUDIO_CODEC_ADC_2_BIQUAD_B1_1(x)            ((u32)(((x) & 0x1FFFFFFF) << 0))
#define AUDIO_CODEC_GET_ADC_2_BIQUAD_B1_1(x)        ((u32)(((x >> 0) & 0x1FFFFFFF)))
/** @} */


/** @defgroup adc_2_biquad_b2_1
 * @brief
 * @{
 */
#define AUDIO_CODEC_MASK_ADC_2_BIQUAD_B2_1          ((u32)0x1FFFFFFF << 0)                   /*!< R/W     0 ADC channel 2 EQ 1-band coef. b2 2's complement in 4.25 format, i.e. the range is from -8~7.99.   */
#define AUDIO_CODEC_ADC_2_BIQUAD_B2_1(x)            ((u32)(((x) & 0x1FFFFFFF) << 0))
#define AUDIO_CODEC_GET_ADC_2_BIQUAD_B2_1(x)        ((u32)(((x >> 0) & 0x1FFFFFFF)))
/** @} */


/** @defgroup adc_2_biquad_a1_1
 * @brief
 * @{
 */
#define AUDIO_CODEC_MASK_ADC_2_BIQUAD_A1_1          ((u32)0x1FFFFFFF << 0)                   /*!< R/W     0 ADC channel 2 EQ 1-band coef. a1 2's complement in 4.25 format, i.e. the range is from -8~7.99.   */
#define AUDIO_CODEC_ADC_2_BIQUAD_A1_1(x)            ((u32)(((x) & 0x1FFFFFFF) << 0))
#define AUDIO_CODEC_GET_ADC_2_BIQUAD_A1_1(x)        ((u32)(((x >> 0) & 0x1FFFFFFF)))
/** @} */


/** @defgroup adc_2_biquad_a2_1
 * @brief
 * @{
 */
#define AUDIO_CODEC_MASK_ADC_2_BIQUAD_A2_1          ((u32)0x1FFFFFFF << 0)                   /*!< R/W     0 ADC channel 2 EQ 1-band coef. a2 2's complement in 4.25 format, i.e. the range is from -8~7.99.   */
#define AUDIO_CODEC_ADC_2_BIQUAD_A2_1(x)            ((u32)(((x) & 0x1FFFFFFF) << 0))
#define AUDIO_CODEC_GET_ADC_2_BIQUAD_A2_1(x)        ((u32)(((x >> 0) & 0x1FFFFFFF)))
/** @} */


/** @defgroup adc_2_biquad_h0_2
 * @brief
 * @{
 */
#define AUDIO_CODEC_MASK_ADC_2_BIQUAD_H0_2          ((u32)0x1FFFFFFF << 0)                   /*!< R/W   0x2000000 ADC channel 2 EQ 2-band coef. h0 2's complement in 4.25 format, i.e. the range is from -8~7.99.   */
#define AUDIO_CODEC_ADC_2_BIQUAD_H0_2(x)            ((u32)(((x) & 0x1FFFFFFF) << 0))
#define AUDIO_CODEC_GET_ADC_2_BIQUAD_H0_2(x)        ((u32)(((x >> 0) & 0x1FFFFFFF)))
/** @} */


/** @defgroup adc_2_biquad_b1_2
 * @brief
 * @{
 */
#define AUDIO_CODEC_MASK_ADC_2_BIQUAD_B1_2          ((u32)0x1FFFFFFF << 0)                   /*!< R/W     0 ADC channel 2 EQ 2-band coef. b1 2's complement in 4.25 format, i.e. the range is from -8~7.99.   */
#define AUDIO_CODEC_ADC_2_BIQUAD_B1_2(x)            ((u32)(((x) & 0x1FFFFFFF) << 0))
#define AUDIO_CODEC_GET_ADC_2_BIQUAD_B1_2(x)        ((u32)(((x >> 0) & 0x1FFFFFFF)))
/** @} */


/** @defgroup adc_2_biquad_b2_2
 * @brief
 * @{
 */
#define AUDIO_CODEC_MASK_ADC_2_BIQUAD_B2_2          ((u32)0x1FFFFFFF << 0)                   /*!< R/W     0 ADC channel 2 EQ 2-band coef. b2 2's complement in 4.25 format, i.e. the range is from -8~7.99.   */
#define AUDIO_CODEC_ADC_2_BIQUAD_B2_2(x)            ((u32)(((x) & 0x1FFFFFFF) << 0))
#define AUDIO_CODEC_GET_ADC_2_BIQUAD_B2_2(x)        ((u32)(((x >> 0) & 0x1FFFFFFF)))
/** @} */


/** @defgroup adc_2_biquad_a1_2
 * @brief
 * @{
 */
#define AUDIO_CODEC_MASK_ADC_2_BIQUAD_A1_2          ((u32)0x1FFFFFFF << 0)                   /*!< R/W     0 ADC channel 2 EQ 2-band coef. a1 2's complement in 4.25 format, i.e. the range is from -8~7.99.   */
#define AUDIO_CODEC_ADC_2_BIQUAD_A1_2(x)            ((u32)(((x) & 0x1FFFFFFF) << 0))
#define AUDIO_CODEC_GET_ADC_2_BIQUAD_A1_2(x)        ((u32)(((x >> 0) & 0x1FFFFFFF)))
/** @} */


/** @defgroup adc_2_biquad_a2_2
 * @brief
 * @{
 */
#define AUDIO_CODEC_MASK_ADC_2_BIQUAD_A2_2          ((u32)0x1FFFFFFF << 0)                   /*!< R/W     0 ADC channel 2 EQ 2-band coef. a2 2's complement in 4.25 format, i.e. the range is from -8~7.99.   */
#define AUDIO_CODEC_ADC_2_BIQUAD_A2_2(x)            ((u32)(((x) & 0x1FFFFFFF) << 0))
#define AUDIO_CODEC_GET_ADC_2_BIQUAD_A2_2(x)        ((u32)(((x >> 0) & 0x1FFFFFFF)))
/** @} */


/** @defgroup adc_2_biquad_h0_3
 * @brief
 * @{
 */
#define AUDIO_CODEC_MASK_ADC_2_BIQUAD_H0_3          ((u32)0x1FFFFFFF << 0)                   /*!< R/W   0x2000000 ADC channel 2 EQ 3-band coef. h0 2's complement in 4.25 format, i.e. the range is from -8~7.99.   */
#define AUDIO_CODEC_ADC_2_BIQUAD_H0_3(x)            ((u32)(((x) & 0x1FFFFFFF) << 0))
#define AUDIO_CODEC_GET_ADC_2_BIQUAD_H0_3(x)        ((u32)(((x >> 0) & 0x1FFFFFFF)))
/** @} */


/** @defgroup adc_2_biquad_b1_3
 * @brief
 * @{
 */
#define AUDIO_CODEC_MASK_ADC_2_BIQUAD_B1_3          ((u32)0x1FFFFFFF << 0)                   /*!< R/W     0 ADC channel 2 EQ 3-band coef. b1 2's complement in 4.25 format, i.e. the range is from -8~7.99.   */
#define AUDIO_CODEC_ADC_2_BIQUAD_B1_3(x)            ((u32)(((x) & 0x1FFFFFFF) << 0))
#define AUDIO_CODEC_GET_ADC_2_BIQUAD_B1_3(x)        ((u32)(((x >> 0) & 0x1FFFFFFF)))
/** @} */


/** @defgroup adc_2_biquad_b2_3
 * @brief
 * @{
 */
#define AUDIO_CODEC_MASK_ADC_2_BIQUAD_B2_3          ((u32)0x1FFFFFFF << 0)                   /*!< R/W     0 ADC channel 2 EQ 3-band coef. b2 2's complement in 4.25 format, i.e. the range is from -8~7.99.   */
#define AUDIO_CODEC_ADC_2_BIQUAD_B2_3(x)            ((u32)(((x) & 0x1FFFFFFF) << 0))
#define AUDIO_CODEC_GET_ADC_2_BIQUAD_B2_3(x)        ((u32)(((x >> 0) & 0x1FFFFFFF)))
/** @} */


/** @defgroup adc_2_biquad_a1_3
 * @brief
 * @{
 */
#define AUDIO_CODEC_MASK_ADC_2_BIQUAD_A1_3          ((u32)0x1FFFFFFF << 0)                   /*!< R/W     0 ADC channel 2 EQ 3-band coef. a1 2's complement in 4.25 format, i.e. the range is from -8~7.99.   */
#define AUDIO_CODEC_ADC_2_BIQUAD_A1_3(x)            ((u32)(((x) & 0x1FFFFFFF) << 0))
#define AUDIO_CODEC_GET_ADC_2_BIQUAD_A1_3(x)        ((u32)(((x >> 0) & 0x1FFFFFFF)))
/** @} */


/** @defgroup adc_2_biquad_a2_3
 * @brief
 * @{
 */
#define AUDIO_CODEC_MASK_ADC_2_BIQUAD_A2_3          ((u32)0x1FFFFFFF << 0)                   /*!< R/W     0 ADC channel 2 EQ 3-band coef. a2 2's complement in 4.25 format, i.e. the range is from -8~7.99.   */
#define AUDIO_CODEC_ADC_2_BIQUAD_A2_3(x)            ((u32)(((x) & 0x1FFFFFFF) << 0))
#define AUDIO_CODEC_GET_ADC_2_BIQUAD_A2_3(x)        ((u32)(((x >> 0) & 0x1FFFFFFF)))
/** @} */


/** @defgroup adc_2_biquad_h0_4
 * @brief
 * @{
 */
#define AUDIO_CODEC_MASK_ADC_2_BIQUAD_H0_4          ((u32)0x1FFFFFFF << 0)                   /*!< R/W   0x2000000 ADC channel 2 EQ 4-band coef. h0 2's complement in 4.25 format, i.e. the range is from -8~7.99.   */
#define AUDIO_CODEC_ADC_2_BIQUAD_H0_4(x)            ((u32)(((x) & 0x1FFFFFFF) << 0))
#define AUDIO_CODEC_GET_ADC_2_BIQUAD_H0_4(x)        ((u32)(((x >> 0) & 0x1FFFFFFF)))
/** @} */


/** @defgroup adc_2_biquad_b1_4
 * @brief
 * @{
 */
#define AUDIO_CODEC_MASK_ADC_2_BIQUAD_B1_4          ((u32)0x1FFFFFFF << 0)                   /*!< R/W     0 ADC channel 2 EQ 4-band coef. b1 2's complement in 4.25 format, i.e. the range is from -8~7.99.   */
#define AUDIO_CODEC_ADC_2_BIQUAD_B1_4(x)            ((u32)(((x) & 0x1FFFFFFF) << 0))
#define AUDIO_CODEC_GET_ADC_2_BIQUAD_B1_4(x)        ((u32)(((x >> 0) & 0x1FFFFFFF)))
/** @} */


/** @defgroup adc_2_biquad_b2_4
 * @brief
 * @{
 */
#define AUDIO_CODEC_MASK_ADC_2_BIQUAD_B2_4          ((u32)0x1FFFFFFF << 0)                   /*!< R/W     0 ADC channel 2 EQ 4-band coef. b2 2's complement in 4.25 format, i.e. the range is from -8~7.99.   */
#define AUDIO_CODEC_ADC_2_BIQUAD_B2_4(x)            ((u32)(((x) & 0x1FFFFFFF) << 0))
#define AUDIO_CODEC_GET_ADC_2_BIQUAD_B2_4(x)        ((u32)(((x >> 0) & 0x1FFFFFFF)))
/** @} */


/** @defgroup adc_2_biquad_a1_4
 * @brief
 * @{
 */
#define AUDIO_CODEC_MASK_ADC_2_BIQUAD_A1_4          ((u32)0x1FFFFFFF << 0)                   /*!< R/W     0 ADC channel 2 EQ 4-band coef. a1 2's complement in 4.25 format, i.e. the range is from -8~7.99.   */
#define AUDIO_CODEC_ADC_2_BIQUAD_A1_4(x)            ((u32)(((x) & 0x1FFFFFFF) << 0))
#define AUDIO_CODEC_GET_ADC_2_BIQUAD_A1_4(x)        ((u32)(((x >> 0) & 0x1FFFFFFF)))
/** @} */


/** @defgroup adc_2_biquad_a2_4
 * @brief
 * @{
 */
#define AUDIO_CODEC_MASK_ADC_2_BIQUAD_A2_4          ((u32)0x1FFFFFFF << 0)                   /*!< R/W     0 ADC channel 2 EQ 4-band coef. a2 2's complement in 4.25 format, i.e. the range is from -8~7.99.   */
#define AUDIO_CODEC_ADC_2_BIQUAD_A2_4(x)            ((u32)(((x) & 0x1FFFFFFF) << 0))
#define AUDIO_CODEC_GET_ADC_2_BIQUAD_A2_4(x)        ((u32)(((x >> 0) & 0x1FFFFFFF)))
/** @} */


/** @defgroup ana_read
 * @brief
 * @{
 */
#define AUDIO_CODEC_BIT_MICBIAS_OC                  ((u32)0x00000001 << 0)                   /*!< R     nan The status flag of MICBIAS over-current protection */
/** @} */


/** @defgroup adc_0_lpf_rd
 * @brief
 * @{
 */
#define AUDIO_CODEC_MASK_ADC_0_LPF_RD               ((u32)0x0007FFFF << 0)                   /*!< R     nan CH 0 ADC LPF out valus */
#define AUDIO_CODEC_ADC_0_LPF_RD(x)                 ((u32)(((x) & 0x0007FFFF) << 0))
#define AUDIO_CODEC_GET_ADC_0_LPF_RD(x)             ((u32)(((x >> 0) & 0x0007FFFF)))
/** @} */


/** @defgroup adc_1_lpf_rd
 * @brief
 * @{
 */
#define AUDIO_CODEC_MASK_ADC_1_LPF_RD               ((u32)0x0007FFFF << 0)                   /*!< R     nan CH 1 ADC LPF out valus */
#define AUDIO_CODEC_ADC_1_LPF_RD(x)                 ((u32)(((x) & 0x0007FFFF) << 0))
#define AUDIO_CODEC_GET_ADC_1_LPF_RD(x)             ((u32)(((x >> 0) & 0x0007FFFF)))
/** @} */


/** @defgroup adc_2_lpf_rd
 * @brief
 * @{
 */
#define AUDIO_CODEC_MASK_ADC_2_LPF_RD               ((u32)0x0007FFFF << 0)                   /*!< R     nan CH 2 ADC LPF out valus */
#define AUDIO_CODEC_ADC_2_LPF_RD(x)                 ((u32)(((x) & 0x0007FFFF) << 0))
#define AUDIO_CODEC_GET_ADC_2_LPF_RD(x)             ((u32)(((x >> 0) & 0x0007FFFF)))
/** @} */


/** @defgroup silence_inform
 * @brief
 * @{
 */
#define AUDIO_CODEC_BIT_DAC_L_SILENCE_DET_O         ((u32)0x00000001 << 0)                   /*!< R     nan dac_l_silence data status (result of silence detection) 1'b0: not dac_l_silence data1'b1: dac_l_silence is detected  */
#define AUDIO_CODEC_BIT_DAC_L_SILENCE_DET_STATUS    ((u32)0x00000001 << 1)                   /*!< R     nan Ongoing status of dac_l_silence detection1'b0: dac_l_silence detection is resting (clock is gating)1'b1: dac_l_silence detection is working  */
#define AUDIO_CODEC_BIT_ADC_0_SILENCE_DET_O         ((u32)0x00000001 << 4)                   /*!< R     nan adc_0_silencedata status (result of silence detection)1'b0: not adc_0_silencedata1'b1: adc_0_silenceis detected  */
#define AUDIO_CODEC_BIT_ADC_0_SILENCE_DET_STATUS    ((u32)0x00000001 << 5)                   /*!< R     nan Ongoing status of adc_0_silencedetection1'b0: adc_0_silencedetection is resting (clock is gating)1'b1: adc_0_silencedetection is working  */
#define AUDIO_CODEC_BIT_ADC_1_SILENCE_DET_O         ((u32)0x00000001 << 6)                   /*!< R     nan adc_1_silencedata status (result of silence detection)1'b0: not adc_1_silencedata1'b1: adc_1_silenceis detected  */
#define AUDIO_CODEC_BIT_ADC_1_SILENCE_DET_STATUS    ((u32)0x00000001 << 7)                   /*!< R     nan Ongoing status of adc_1_silencedetection1'b0: adc_1_silencedetection is resting (clock is gating)1'b1: adc_1_silencedetection is working  */
#define AUDIO_CODEC_BIT_ADC_2_SILENCE_DET_O         ((u32)0x00000001 << 8)                   /*!< R     nan adc_2_silencedata status (result of silence detection)1'b0: not adc_1_silencedata1'b1: adc_1_silenceis detected  */
#define AUDIO_CODEC_BIT_ADC_2_SILENCE_DET_STATUS    ((u32)0x00000001 << 9)                   /*!< R     nan Ongoing status of adc_2_silencedetection1'b0: adc_1_silencedetection is resting (clock is gating)1'b1: adc_1_silencedetection is working  */
/** @} */


/** @defgroup audio_ro_dummy1
 * @brief
 * @{
 */
#define AUDIO_CODEC_MASK_AUDIO_RO_DUMMY1            ((u32)0xFFFFFFFF << 0)                   /*!< R     nan nan */
#define AUDIO_CODEC_AUDIO_RO_DUMMY1(x)              ((u32)(((x) & 0xFFFFFFFF) << 0))
#define AUDIO_CODEC_GET_AUDIO_RO_DUMMY1(x)          ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */




/** @defgroup AUDIO_CODEC_Exported_Types AUDIO_CODEC Exported Types
* @{
*/

/** @brief AUDIO port
*/
typedef struct {
	__IO uint32_t CODEC_ADC_x_CONTROL_0;  /*!< Register */
	__IO uint32_t CODEC_ADC_x_CONTROL_1;  /*!< Register */
} AUDIO_CODEC_ADC_CH_CTRL_TypeDef;
/** @brief AUDIO port
*/
typedef struct {
	__IO uint32_t CODEC_BIQUAD_H0_x;  /*!< Register */
	__IO uint32_t CODEC_BIQUAD_B1_x;  /*!< Register */
	__IO uint32_t CODEC_BIQUAD_B2_x;  /*!< Register */
	__IO uint32_t CODEC_BIQUAD_A1_x;  /*!< Register */
	__IO uint32_t CODEC_BIQUAD_A2_x;  /*!< Register */
} AUDIO_CODEC_EQ_BAND_TypeDef;

/**
* @brief AUDIO_CODEC Register Declaration
*/

typedef struct {
	__IO uint32_t AUDIO_CONTROL_0;                              /*!< Register,  Address offset:0x0000 */
	__IO uint32_t AUDIO_CONTROL_1;                              /*!< Register,  Address offset:0x0004 */
	__IO uint32_t CLOCK_CONTROL_1;                              /*!< Register,  Address offset:0x0008 */
	__IO uint32_t CLOCK_CONTROL_2;                              /*!< Register,  Address offset:0x000C */
	__IO uint32_t CLOCK_CONTROL_3;                              /*!< Register,  Address offset:0x0010 */
	__IO uint32_t CLOCK_CONTROL_4;                              /*!< Register,  Address offset:0x0014 */
	__IO uint32_t CLOCK_CONTROL_5;                              /*!< Register,  Address offset:0x0018 */
	__IO uint32_t CLOCK_CONTROL_6;                              /*!< Register,  Address offset:0x001C */
	__IO uint32_t CLOCK_CONTROL_7;                              /*!< Register,  Address offset:0x0020 */
	__IO uint32_t ASRC_CONTROL_0;                               /*!< Register,  Address offset:0x0024 */
	__IO uint32_t ASRC_CONTROL_1;                               /*!< Register,  Address offset:0x0028 */
	__IO uint32_t ASRC_CONTROL_2;                               /*!< Register,  Address offset:0x002C */
	__IO uint32_t ASRC_CONTROL_3;                               /*!< Register,  Address offset:0x0030 */
	__IO uint32_t ASRC_CONTROL_4;                               /*!< Register,  Address offset:0x0034 */
	__IO uint32_t ASRC_CONTROL_5;                               /*!< Register,  Address offset:0x0038 */
	__IO uint32_t I2S_0_CONTROL;                                /*!< Register,  Address offset:0x003C */
	__IO uint32_t I2S_0_CONTROL_1;                              /*!< Register,  Address offset:0x0040 */
	__IO uint32_t I2S_1_CONTROL;                                /*!< Register,  Address offset:0x0044 */
	__IO uint32_t I2S_1_CONTROL_1;                              /*!< Register,  Address offset:0x0048 */
	__IO uint32_t I2S_AD_SEL_CONTROL;                           /*!< Register,  Address offset:0x004C */
	AUDIO_CODEC_ADC_CH_CTRL_TypeDef ADC_CH_CTRL[8];             /*!< Register,  Address offset:0x0050-0x008C*/
	__IO uint32_t DAC_L_CONTROL_0;                              /*!< Register,  Address offset:0x0090 */
	__IO uint32_t DAC_L_CONTROL_1;                              /*!< Register,  Address offset:0x0094 */
	__IO uint32_t DAC_L_CONTROL_2;                              /*!< Register,  Address offset:0x0098 */
	uint32_t RSVD_NO_FUNCTION0[4];
	__IO uint32_t ADC_ALIGN_CONTROL_0;                          /*!< Register,  Address offset:0x00AC */
	__IO uint32_t ADC_ALIGN_CONTROL;                            /*!< Register,  Address offset:0x00B0 */
	uint32_t RSVD_NO_FUNCTION1[2];
	__IO uint32_t DAC_MIX_CONTROL;                              /*!< Register,  Address offset:0x00BC */
	uint32_t RSVD_NO_FUNCTION2[16];
	__IO uint32_t ADC_x_SILENCE_CONTROL[8];                     /*!< Register,  Address offset:0x0100-0x011C */
	__IO uint32_t DAC_L_SILENCE_CONTROL;                        /*!< Register,  Address offset:0x0120 */
	uint32_t RSVD_NO_FUNCTION3[55];
	__IO uint32_t DAC_L_EQ_CTRL;                                /*!< Register,  Address offset:0x0200 */
	AUDIO_CODEC_EQ_BAND_TypeDef DAC_L_EQ_BAND[5];               /*!< Register,  Address offset:0x0204-0x0264 */
	uint32_t RSVD_NO_FUNCTION4[38];
	__IO uint32_t DAC_R_EQ_CTRL;                                /*!< Register,  Address offset:0x0300 */
	uint32_t RSVD_NO_FUNCTION5[63];
	__IO uint32_t ADC_0_EQ_CTRL;                                /*!< Register,  Address offset:0x0400 */
	AUDIO_CODEC_EQ_BAND_TypeDef ADC_0_EQ_BAND[5];               /*!< Register,  Address offset:0x0404-0x0464 */
	__IO uint32_t ADC_1_EQ_CTRL;                                /*!< Register,  Address offset:0x0468 */
	AUDIO_CODEC_EQ_BAND_TypeDef ADC_1_EQ_BAND[5];               /*!< Register,  Address offset:0x046C-0x04CC */
	uint32_t RSVD_NO_FUNCTION6[12];
	__IO uint32_t ADC_2_EQ_CTRL;                                /*!< Register,  Address offset:0x0500 */
	AUDIO_CODEC_EQ_BAND_TypeDef ADC_2_EQ_BAND[5];               /*!< Register,  Address offset:0x0504-0x0564 */
	__IO uint32_t ADC_3_EQ_CTRL;                                /*!< Register,  Address offset:0x0568 */
	AUDIO_CODEC_EQ_BAND_TypeDef ADC_3_EQ_BAND[5];               /*!< Register,  Address offset:0x056C-0x05CC */
	uint32_t RSVD_NO_FUNCTION7[12];
	__IO uint32_t ADC_4_EQ_CTRL;                                /*!< Register,  Address offset:0x0600 */
	AUDIO_CODEC_EQ_BAND_TypeDef ADC_4_EQ_BAND[5];               /*!< Register,  Address offset:0x0604-0x0664 */
	__IO uint32_t ADC_5_EQ_CTRL;                                /*!< Register,  Address offset:0x0668 */
	AUDIO_CODEC_EQ_BAND_TypeDef ADC_5_EQ_BAND[5];               /*!< Register,  Address offset:0x066C-0x06CC */
	uint32_t RSVD_NO_FUNCTION8[12];
	__IO uint32_t VAD_CTRL;                                     /*!< Register,  Address offset:0x0700 */
	__IO uint32_t VAD_VOICE_LEVEL;                              /*!< Register,  Address offset:0x0704 */
	__IO uint32_t VAD_DET_THR;                                  /*!< Register,  Address offset:0x0708 */
	__IO uint32_t VAD_INTR_EN;                                  /*!< Register,  Address offset:0x070C */
	__IO uint32_t DAC_DRE_CONTROL_0;                            /*!< Register,  Address offset:0x0710 */
	uint32_t RSVD_NO_FUNCTION9[59];
	__I  uint32_t ANA_READ;                                     /*!< Register,  Address offset:0x0800 */
	__I  uint32_t ADC_x_LPF_RD[8];                              /*!< Register,  Address offset:0x0804-0x0820 */
	__I  uint32_t SILENCE_INFORM;                               /*!< Register,  Address offset:0x0824 */
	uint32_t RSVD_NO_FUNCTION10[54];
	__I  uint32_t AUDIO_RO_DUMMY1;                              /*!< Register,  Address offset:0x0900 */
} AUDIO_CODEC_TypeDef;

/** @} */


/* AUTO_GEN_END */

/* MANUAL_GEN_START */
#ifdef __cplusplus
extern "C" {
#endif


//Please add your defination here


/* Exported types ------------------------------------------------------------*/
/** @defgroup AUDIO_CODEC_Exported_Types AUDIO_CODEC Exported Types
  * @{
  */

/**
 * @brief AUDIO_CODEC Init Structure Definition
 */
typedef struct {

	u32 CODEC_SelI2STxSR;				/*!< Specifies the AUDIO CODEC TX Sample rate
										This parameter can be a value of @ref AUDIO_CODEC_Sample_Rate */

	u32 CODEC_SelI2STxWordLen; 			/*!< Specifies the AUDIO CODEC TX Word length
										This parameter can be a value of @ref AUDIO_CODEC_Word_Length */

	u32 CODEC_SelI2STxCHLen;			/*!< Specifies the AUDIO SPORT TX Channel length
										This parameter can be a value of @ref AUDIO_CODEC_Channel_Length  */

	u32 CODEC_SelI2STxCH;				/*!< Specifies the AUDIO SPORT TX Channel L/R
										This parameter can be a value of @ref AUDIO_CODEC_Tx_Channel  */

	u32 CODEC_SelI2STxDataFormat;		/*!< Specifies the AUDIO CODEC I2S RX selection of data fornat
										This parameter can be a value of @ref AUDIO_CODEC_Interface_Format */

	u32 CODEC_SelI2SRxSR;				/*!< Specifies the AUDIO CODEC RX Sample rate
										This parameter can be a value of @ref AUDIO_CODEC_Sample_Rate */

	u32 CODEC_SelI2SRxWordLen; 			/*!< Specifies the AUDIO CODEC RX Word length
										This parameter can be a value of @ref AUDIO_CODEC_Word_Length */

	u32 CODEC_SelI2SRxCHLen;			/*!< Specifies the AUDIO SPORT RX Channel length
										This parameter can be a value of @ref AUDIO_CODEC_Channel_Length  */

	u32 CODEC_SelI2SRxDataFormat;		/*!< Specifies the AUDIO CODEC I2S RX selection of data fornat
										This parameter can be a value of @ref AUDIO_CODEC_Interface_Format */

	u32 CODEC_SelRxI2STdm;				/*!< Specifies the AUDIO CODEC Rx I2S TDM MODE
										This parameter can be a value of @ref AUDIO_CODEC_I2S_TDM_Mode */

} I2S_InitTypeDef;

/**
 * @brief AUDIO_CODEC EQ Filter coefficients Structure Definition
 */
typedef struct {
	u32 H0_Q;
	u32 B1_Q;
	u32 B2_Q;
	u32 A1_Q;
	u32 A2_Q;
} CODEC_EQFilterCoef;
/**
  * @}
  */

/* Exported constants ------------------------------------------------------------*/
/** @defgroup AUDIO_CODEC_Exported_Constants AUDIO_CODEC Exported Constants
  * @{
  */

/** @defgroup AUDIO_CODEC_Sample_Rate
  * @{
  */
#define SR_48K			((u32)0x00000000)
#define SR_96K			((u32)0x00000001)
#define SR_192K			((u32)0x00000002)
#define SR_32K			((u32)0x00000003)
#define SR_176P4K		((u32)0x00000004)
#define SR_16K			((u32)0x00000005)
#define SR_8K			((u32)0x00000007)
#define SR_44P1K		((u32)0x00000008)
#define SR_88P2K		((u32)0x00000009)
#define SR_24K			((u32)0x0000000a)
#define SR_12K			((u32)0x0000000b)
#define SR_22P05K		((u32)0x0000000c)
#define SR_11P025K		((u32)0x0000000d)

#define IS_CODEC_SR_SEL(SEL)   (((SEL) == SR_48K) || \
								((SEL) == SR_96K) || \
								((SEL) == SR_192K) || \
								((SEL) == SR_32K) || \
								((SEL) == SR_176P4K) || \
								((SEL) == SR_16K) || \
								((SEL) == SR_8K) || \
								((SEL) == SR_44P1K) || \
								((SEL) == SR_88P2K) || \
								((SEL) == SR_24K) || \
								((SEL) == SR_12K) || \
								((SEL) == SR_22P05K) || \
								((SEL) == SR_11P025K))

#define IS_CODEC_ADC_ASRC_SEL(SEL)	(((SEL) == SR_48K) || \
									((SEL) == SR_96K) || \
									((SEL) == SR_32K) || \
									((SEL) == SR_16K) || \
									((SEL) == SR_8K) || \
									((SEL) == SR_44P1K) || \
									((SEL) == SR_88P2K) || \
									((SEL) == SR_24K) || \
									((SEL) == SR_12K) || \
									((SEL) == SR_22P05K) || \
									((SEL) == SR_11P025K))
/**
  * @}
  */

/** @defgroup AUDIO_CODEC_Word_Length
  * @{
  */
#define WL_16			((u32)0x00000000)
#define WL_24			((u32)0x00000002)
#define WL_8			((u32)0x00000003)

#define IS_CODEC_WL_SEL(SEL)   (((SEL) == WL_16) || \
								((SEL) == WL_24) || \
								((SEL) == WL_8))
/**
  * @}
  */

/** @defgroup AUDIO_CODEC_Channel_Length
  * @{
  */
#define CL_16			((u32)0x00000000)
#define CL_32			((u32)0x00000001)
#define CL_24			((u32)0x00000002)
#define CL_8			((u32)0x00000003)

#define IS_CODEC_CL_SEL(SEL)   (((SEL) == CL_16) || \
								((SEL) == CL_32) || \
								((SEL) == CL_24) || \
								((SEL) == CL_8))
/**
  * @}
  */

/** @defgroup AUDIO_CODEC_Tx_Channel
 * @{
 */
#define CH_LR						((u32)0x00000000)
#define CH_RL						((u32)0x00000001)
#define CH_LL						((u32)0x00000002)
#define CH_RR						((u32)0x00000003)

#define IS_CODEC_TXCHN_SEL(SEL)    (((SEL) == CH_LR) || \
									((SEL) == CH_RL) || \
									((SEL) == CH_LL) || \
									((SEL) == CH_RR))
/**
  * @}
  */

/** @defgroup AUDIO_CODEC_Interface_Format
  * @{
  */
#define DF_I2S							((u32)0x00000000)
#define DF_LEFT							((u32)0x00000001)
#define DF_PCM_A						((u32)0x00000002)
#define DF_PCM_B						((u32)0x00000003)

#define IS_CODEC_DATA_FMT(FORMAT) (((FORMAT) == DF_I2S)	|| \
								   ((FORMAT) == DF_LEFT) || \
								   ((FORMAT) == DF_PCM_A) || \
								   ((FORMAT) == DF_PCM_B))
/**
  * @}
  */

/** @defgroup AUDIO_CODEC_I2S_TDM_Mode
  * @{
  */
#define I2S_NOTDM						    ((u32)0x00000000)
#define I2S_TDM4							((u32)0x00000001)

#define IS_CODEC_I2S_TDM_SEL(SEL)  (((SEL) == I2S_NOTDM) || \
									((SEL) == I2S_TDM4))
/**
  * @}
  */

/** @defgroup AUDIO_CODEC_AMIC
 * @{
 */
#define AMIC1							((u32)0x00000001)

#define IS_CODEC_AMIC_SEL(SEL) (((SEL) == AMIC1))

/**
  * @}
  */

/** @defgroup AUDIO_CODEC_ADC
  * @{
  */
#define ADC0							((u32)0x00000000)
#define ADC1							((u32)0x00000001)
#define ADC2 							((u32)0x00000002)

#define IS_CODEC_ADC_SEL(SEL) (((SEL) == ADC0) || \
								((SEL) == ADC1) || \
								((SEL) == ADC2))
/**
  * @}
  */

/** @defgroup AUDIO_CODEC_ADC_Channel
  * @{
  */
#define ADCHN0							((u32)0x00000000)
#define ADCHN1							((u32)0x00000001)
#define ADCHN2							((u32)0x00000002)
#define IS_CODEC_ADCHN_SEL(SEL) (((SEL) == ADCHN0) || \
								((SEL) == ADCHN1) || \
								((SEL) == ADCHN2))
/**
* @}
*/

/** @defgroup AUDIO_CODEC_Power
  * @{
  */
#define NORMALPOWER						((u32)0x00000001)
#define LOWPOWER						((u32)0x00000002)
#define SHUTDOWN						((u32)0x00000003)

#define IS_CODEC_POWER_MODE_SEL(SEL) (((SEL) == NORMALPOWER) || \
							((SEL) == LOWPOWER) || \
							((SEL) == SHUTDOWN))
/**
  * @}
  */

/** @defgroup AUDIO_CODEC_DMIC
  * @{
  */
#define DMIC1							((u32)0x00000001)
#define DMIC2							((u32)0x00000002)

#define IS_CODEC_DMIC_SEL(SEL) (((SEL) == DMIC1) || \
							((SEL) == DMIC2))
/**
  * @}
  */

/** @defgroup AUDIO_CODEC_DMIC_Clock
  * @{
  */
#define DMIC_5M			((u32)0x00000000)
#define DMIC_2P5M		((u32)0x00000001)
#define DMIC_1P25M		((u32)0x00000002)
#define DMIC_625K		((u32)0x00000003)
#define DMIC_312P5K		((u32)0x00000004)
#define DMIC_769P2K		((u32)0x00000007)

#define IS_CODEC_DMIC_CLOCK_SEL(SEL) (((SEL) == DMIC_5M) || \
							((SEL) == DMIC_2P5M) || \
							((SEL) == DMIC_1P25M) || \
							((SEL) == DMIC_625K) || \
							((SEL) == DMIC_312P5K) || \
							((SEL) == DMIC_769P2K))
/**
  * @}
  */

/** @defgroup AUDIO_CODEC_PDM_Clock
 * @{
 */
#define PDM_5M			((u32)0x00000000)
#define PDM_2P5M		((u32)0x00000001)
#define PDM_6P67M		((u32)0x00000002)

#define IS_CODEC_PDM_CLOCK_SEL(SEL) (((SEL) == PDM_5M) || \
							((SEL) == PDM_2P5M) || \
							((SEL) == PDM_6P67M))

/**
  * @}
  */


/** @defgroup AUDIO_CODEC_ADC_EQ_Band
  * @{
  */
#define ADCEQBD0							((u32)0x00000000)
#define ADCEQBD1							((u32)0x00000001)
#define ADCEQBD2							((u32)0x00000002)
#define ADCEQBD3							((u32)0x00000003)
#define ADCEQBD4							((u32)0x00000004)

#define IS_CODEC_ADC_EQ_Band_SEL(SEL) (((SEL) == ADCEQBD0) || \
									((SEL) == ADCEQBD1) || \
									((SEL) == ADCEQBD2) || \
									((SEL) == ADCEQBD3) || \
									((SEL) == ADCEQBD4))
/**
* @}
*/

/** @defgroup AUDIO_CODEC_DAC_EQ_Band
  * @{
  */
#define DACEQBD0							((u32)0x00000000)
#define DACEQBD1							((u32)0x00000001)
#define DACEQBD2							((u32)0x00000002)
#define DACEQBD3							((u32)0x00000003)
#define DACEQBD4							((u32)0x00000004)

#define IS_CODEC_DAC_EQ_Band_SEL(SEL) (((SEL) == DACEQBD0) || \
								((SEL) == DACEQBD1) || \
								((SEL) == DACEQBD2) || \
								((SEL) == DACEQBD3)|| \
								((SEL) == DACEQBD4))
/**
* @}
*/

/** @defgroup AUDIO_CODEC_Sample_Rate_Source
  * @{
  */
#define SOURCE0			((u32)0x00000000)
#define SOURCE1			((u32)0x00000001)
#define SOURCE2			((u32)0x00000002)
#define SOURCE3 		((u32)0x00000003)

#define IS_CODEC_SR_SRC(SEL) (((SEL) == SOURCE0) || \
								((SEL) == SOURCE1) || \
								((SEL) == SOURCE2) || \
								((SEL) == SOURCE3))
/**
  * @}
  */


/** @defgroup AUDIO_CODEC_PATH
  * @{
  */
#define DACPATH			((u32)0x00000001)
#define ADCPATH			((u32)0x00000002)

#define IS_CODEC_WORK_PATH(SEL) (((SEL) == DACPATH) || \
								((SEL) == ADCPATH))
/**
  * @}
  */

/** @defgroup AUDIO_CODEC_ADC_Mix_Select
* @{
*/
#define DMIC_MIX_MUTE				((u32)0x00000001)
#define AD_MIX_MUTE					((u32)0x00000002)

#define IS_CODEC_ADC_MIXMUTE_SEL(SEL) (((SEL) == DMIC_MIX_MUTE) || \
								((SEL) == AD_MIX_MUTE))
/**
* @}
*/

/** @defgroup AUDIO_CODEC_Micbst_Gain
  * @{
  */
#define MICBST_GAIN_0DB			((u32)0x00000000)
#define MICBST_GAIN_5DB			((u32)0x00000001)
#define MICBST_GAIN_10DB		((u32)0x00000002)
#define MICBST_GAIN_15DB		((u32)0x00000003)
#define MICBST_GAIN_20DB		((u32)0x00000004)
#define MICBST_GAIN_25DB		((u32)0x00000005)
#define MICBST_GAIN_30DB		((u32)0x00000006)
#define MICBST_GAIN_35DB		((u32)0x00000007)
#define MICBST_GAIN_40DB		((u32)0x00000008)

#define IS_CODEC_MICBST_GAIN_SEL(SEL) (((SEL) == MICBST_GAIN_0DB) || \
							((SEL) == MICBST_GAIN_5DB) || \
							((SEL) == MICBST_GAIN_10DB) || \
							((SEL) == MICBST_GAIN_15DB) || \
							((SEL) == MICBST_GAIN_20DB) || \
							((SEL) == MICBST_GAIN_25DB) || \
							((SEL) == MICBST_GAIN_30DB) || \
							((SEL) == MICBST_GAIN_35DB) || \
							((SEL) == MICBST_GAIN_40DB))
/**
  * @}
  */


/** @defgroup AUDIO_CODEC_Mute
  * @{
  */
#define UNMUTE			((u32)0x00000000)
#define MUTE			((u32)0x00000001)

#define IS_CODEC_MUTE_SEL(SEL) (((SEL) == UNMUTE) || \
							((SEL) == MUTE))
/**
  * @}
  */

/** @defgroup AUDIO_CODEC_DAC_Channel
  * @{
  */
#define DAC_L			((u32)0x00000000)
#define DAC_R			((u32)0x00000001)

#define IS_CODEC_DAC_SEL(SEL) (((SEL) == DAC_L))

/**
  * @}
  */

/** @defgroup AUDIO_CODEC_DAC_Source
 * @{
 */
#define I2SL					((u32)0x00000000)
#define TESTTONE				((u32)0x00000001)

#define IS_CODEC_DAC_SRC_SEL(SEL) (((SEL) == I2SL) || \
									((SEL) == TESTTONE))
/**
  * @}
  */

/** @defgroup AUDIO_CODEC_I2S_Master_Source
  * @{
  */
#define INTERNAL_SPORT			((u32)0x00000000)
#define EXTERNAL_I2S			((u32)0x00000001)

#define IS_CODEC_I2S_MASTER_SRC(SEL) (((SEL) == INTERNAL_SPORT) || \
								((SEL) == EXTERNAL_I2S))
/**
  * @}
  */

/** @defgroup AUDIO_CODEC_Application
  * @{
  */

#define APP_AMIC_RECORD ((u32)0x00000000)
#define APP_DMIC_RECORD ((u32)0x00000001)
#define APP_LINE_OUT	((u32)0x00000002)
#define APP_PDM_OUT		((u32)0x00000003)
#define APP_TESTTONE	((u32)0x00000004)
#define APP_DAAD_LPBK	((u32)0x00000005)
#define APP_ADDA_LPBK	((u32)0x00000006)

#define IS_CODEC_APP_SEL(SEL) (((SEL) == APP_AMIC_RECORD) || \
							   ((SEL) == APP_DMIC_RECORD) || \
							   ((SEL) == APP_LINE_OUT) || \
							   ((SEL) == APP_PDM_OUT) || \
							   ((SEL) == APP_TESTTONE) || \
							   ((SEL) == APP_DAAD_LPBK) || \
							   ((SEL) == APP_ADDA_LPBK))
/**
  * @}
  */

/** @defgroup AUDIO_CODEC_PAD
 * @{
 */
#define PAD_DACL ((u32)0x00000000)
#define PAD_MIC1 ((u32)0x00000001)
/**
  * @}
  */

/** @defgroup AUDIO_CODEC_Zero_Detection_Function_Select
  * @{
  */
#define IME				 ((u32)0x00000000)
#define ZDET_IME		 ((u32)0x00000001)
#define ZDET_STEP		 ((u32)0x00000002)
#define ZDET_TIMEOUT	 ((u32)0x00000003)

#define IS_CODEC_ZDET_SEL(SEL) (((SEL) == IME) || \
								((SEL) == ZDET_IME) || \
								((SEL) == ZDET_STEP) || \
								((SEL) == ZDET_TIMEOUT))

/**
  * @}
  */


/** @defgroup AUDIO_CODEC_ADC_Decimation_Source
 * @{
 */
#define AUDIO_ADC			((u32)0x00000000)
#define ANC					((u32)0x00000001)
#define MUSIC				((u32)0x00000002)
#define ANCANDMUSIC			((u32)0x00000003)

#define IS_CODEC_ADC_DCM_SEL(SEL) (((SEL) == AUDIO_ADC) || \
									((SEL) == MUSIC))

/**
  * @}
  */

/**
  * @}
  */

/* Exported functions ------------------------------------------------------------*/
/** @defgroup AUDIO_CODEC_Exported_Functions AUDIO_CODEC Exported Functions
  * @{
  */

_LONG_CALL_ void AUDIO_CODEC_SetAudioIP(u32 newstate);
_LONG_CALL_ void AUDIO_CODEC_SetI2SIP(u32 newstate);
_LONG_CALL_ void AUDIO_CODEC_SetI2SSRC(u32 src);
_LONG_CALL_ void AUDIO_CODEC_SetANAClk(u32 newstate);
_LONG_CALL_ void AUDIO_CODEC_SetI2SRXTDM(u32 tdmmode);
_LONG_CALL_ void AUDIO_CODEC_I2S_StructInit(I2S_InitTypeDef *I2S_Struct);
_LONG_CALL_ void AUDIO_CODEC_SetI2SParameters(u32 path, I2S_InitTypeDef *I2S_Struct);
_LONG_CALL_ void AUDIO_CODEC_SetADCSRSrc(u32 src, u32 sr);
_LONG_CALL_ void AUDIO_CODEC_SetDACSRSrc(u32 src, u32 sr);
_LONG_CALL_ void AUDIO_CODEC_DisPAD(u32 path);
_LONG_CALL_ void AUDIO_CODEC_SetLDOMode(u32 powermode);
_LONG_CALL_ void AUDIO_CODEC_SetMicBiasPowerMode(u32 powermode);
_LONG_CALL_ void AUDIO_CODEC_SetMicBstPowerMode(u32 amic_sel, u32 powermode);
_LONG_CALL_ void AUDIO_CODEC_SetMicBstChnMute(u32 amic_sel, u32 newstate);
_LONG_CALL_ void AUDIO_CODEC_SetMicBstInputMode(u32 amic_sel, u32 mode);
_LONG_CALL_ void AUDIO_CODEC_EnableADC(u32 ad_chn, u32 newstate);
_LONG_CALL_ void AUDIO_CODEC_SetADCVolume(u32 adc_sel, u32 gain);
_LONG_CALL_ void AUDIO_CODEC_SetADCHPF(u32 adc_sel, u32 fc, u32 newstate);
_LONG_CALL_ void AUDIO_CODEC_SetADCASRC(u32 adc_sel, u32 sr, u32 newstate);
_LONG_CALL_ void AUDIO_CODEC_SetADCMute(u32 adc_sel, u32 newstate);
_LONG_CALL_ void AUDIO_CODEC_SetADCMixMute(u32 adc_sel, u32 mix_mute_type, u32 newstate);
_LONG_CALL_ void AUDIO_CODEC_SetADCANASrc(u32 ad_chn);
_LONG_CALL_ void AUDIO_CODEC_SetADCANAFilter(u32 adc_sel, u32 newstate);
_LONG_CALL_ void AUDIO_CODEC_SetADCDmicFilter(u32 adc_sel, u32 newstate);
_LONG_CALL_ void AUDIO_CODEC_SetDmicClk(u32 clk, u32 newstate);
_LONG_CALL_ void AUDIO_CODEC_SetDmicSrc(u32 ad_chn, u32 dmic_sel);
_LONG_CALL_ void AUDIO_CODEC_SetDACSrc(u32 dac_l_src);
_LONG_CALL_ void AUDIO_CODEC_SetADCSrc(u32 adc_sel);
_LONG_CALL_ void AUDIO_CODEC_EnableDAC(u32 channel, u32 newstate);
_LONG_CALL_ void AUDIO_CODEC_SetDACMute(u32 channel, u32 newstate);
_LONG_CALL_ void AUDIO_CODEC_SetDACVolume(u32 channel, u32 gain);
_LONG_CALL_ void AUDIO_CODEC_SetDACHPF(u32 channel, u32 newstate);
_LONG_CALL_ void AUDIO_CODEC_SetDACASRC(u32 sr, u32 newstate);
_LONG_CALL_ void AUDIO_CODEC_SetMicBstGain(u32 amic_sel, u32 gain);
_LONG_CALL_ void AUDIO_CODEC_SetDACPowerMode(u32 channel, u32 powermode);
_LONG_CALL_ void AUDIO_CODEC_SetLineOutPowerMode(u32 channel, u32 powermode);
_LONG_CALL_ void AUDIO_CODEC_SetLineOutMode(u32 channel, u32 mode);
_LONG_CALL_ void AUDIO_CODEC_SetLineOutMute(u32 channel, u32 type, u32 newstate);
_LONG_CALL_ void AUDIO_CODEC_SetPDMClk(u32 channel, u32 clk, u32 newstate);
_LONG_CALL_ void AUDIO_CODEC_SetTesttone(u32 fc, u32 newstate);
_LONG_CALL_ void AUDIO_CODEC_SetI2SLPBK(u32 newstate);
_LONG_CALL_ void AUDIO_CODEC_SetDAADLPBK(u32 newstate);
_LONG_CALL_ void AUDIO_CODEC_SetADDALPBK(u32 newstate);
_LONG_CALL_ void AUDIO_CODEC_SetADCDecimationSrc(u32 adc_sel, u32 type);
_LONG_CALL_ void AUDIO_CODEC_SetDACZDET(u32 channel, u32 type);
_LONG_CALL_ void AUDIO_CODEC_SetDACZDETTimeOut(u32 channel, u32 time_out);
_LONG_CALL_ void AUDIO_CODEC_SetADCZDET(u32 adc_sel, u32 type);
_LONG_CALL_ void AUDIO_CODEC_SetADCZDETTimeOut(u32 adc_sel, u32 time_out);
_LONG_CALL_ void AUDIO_CODEC_SetADCSilenceDet(u32 adc_sel, u32 newstate);
_LONG_CALL_ void AUDIO_CODEC_SelADCSilenceLevel(u32 adc_sel, u32 level_det);
_LONG_CALL_ void AUDIO_CODEC_SelADCSilenceDebounce(u32 adc_sel, u32 debounce_sel);
_LONG_CALL_ void AUDIO_CODEC_SetADCEQClk(u32 ad_chn, u32 newstate);
_LONG_CALL_ void AUDIO_CODEC_SetADCEQBand(u32 ad_chn, u32 band_sel, u32 newstate);
_LONG_CALL_ void AUDIO_CODEC_SetADCEQFilter(u32 ad_chn, u32 band_sel, CODEC_EQFilterCoef *EQFilterCoefPoint);
_LONG_CALL_ void AUDIO_CODEC_SetDACEQClk(u32 da_chn, u32 newstate);
_LONG_CALL_ void AUDIO_CODEC_SetDACEQBand(u32 da_chn, u32 band_sel, u32 newstate);
_LONG_CALL_ void AUDIO_CODEC_SetDACEQFilter(u32 da_chn, u32 band_sel, CODEC_EQFilterCoef *EQFilterCoefPoint);
_LONG_CALL_ void AUDIO_CODEC_Record(u32 type, I2S_InitTypeDef *I2S_InitStruct);
_LONG_CALL_ void AUDIO_CODEC_Playback(u32 type, I2S_InitTypeDef *I2S_InitStruct);
_LONG_CALL_ void AUDIO_CODEC_EnableADCFifo(u32 ad_chn, u32 newstate);
_LONG_CALL_ void AUDIO_CODEC_EnableDACFifo(u32 newstate);
/**
  * @}
  */


#ifdef __cplusplus
}
#endif

/* MANUAL_GEN_END */

/** @} */

/** @} */
#endif  // AMEBA_CODEC_1028_H
