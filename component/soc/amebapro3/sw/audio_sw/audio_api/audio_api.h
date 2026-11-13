/** mbed Microcontroller Library
  ******************************************************************************
  * @file    audio_api.h
  * @author
  * @version V1.0.0
  * @brief
  ******************************************************************************
  * @attention
  *
  * Copyright(c) 2006 - 2025 Realtek Corporation. All rights reserved.
  *
  * SPDX-License-Identifier: Apache-2.0
  *
  * Licensed under the Apache License, Version 2.0 (the License); you may
  * not use this file except in compliance with the License.
  * You may obtain a copy of the License at
  *
  * http://www.apache.org/licenses/LICENSE-2.0
  *
  * Unless required by applicable law or agreed to in writing, software
  * distributed under the License is distributed on an AS IS BASIS, WITHOUT
  * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  */

#ifndef __AUDIO_API_H__
#define __AUDIO_API_H__

#include "device.h"
#include "PinNames.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
	WL_16BIT = WL_16,
	WL_24BIT = WL_24,
} audio_wl;

typedef enum {
	ASR_8KHZ = SR_8K,
	ASR_16KHZ = SR_16K,
	ASR_32KHZ = SR_32K,
	ASR_44p1KHZ = SR_44P1K,
	ASR_48KHZ = SR_48K,
	ASR_88p2KHZ = SR_88P2K,
	ASR_96KHZ = SR_96K
} audio_sr;

typedef enum {
	OUTPUT_DISABLE = 0,
	OUTPUT_SINGLE_ENDED = 1,
	OUTPUT_DIFFERENTIAL = 2,
	OUTPUT_CAPLESS = 3
} audio_output_mode;

typedef enum {
	INPUT_DISABLE = 0,
	LINE_IN_MODE = 1,
	MIC_DIFFERENTIAL = 2,
	MIC_SINGLE_EDNED = 3,
	LEFT_DMIC = 4,
	RIGHT_DMIC = 5,
	STEREO_DMIC = 6
} audio_input_mode;

typedef enum {
	MIC_0DB = MICBST_GAIN_0DB,
	MIC_20DB = MICBST_GAIN_20DB,
	MIC_30DB = MICBST_GAIN_30DB,
	MIC_40DB = MICBST_GAIN_40DB
} audio_mic_gain;

typedef enum {
	DVOL_ADC_0DB = 0x2F,
	DVOL_DAC_0DB = 0xAF,
	SIDETONE_0DB = 0x1F
} audio_dvol_0db;

typedef enum {
	AUDIO_PNUM_2 = 2,
	AUDIO_PNUM_3 = 3,
	AUDIO_PNUM_4 = 4
} audio_page_num;

typedef enum {
	DMIC_BOOST_0DB = 0,
	DMIC_BOOST_12DB = 1,
	DMIC_BOOST_24DB = 2,
	DMIC_BOOST_36DB = 3
} audio_dmic_gain;

typedef enum {
	A_MONO = 1,
	A_STEREO = 2
} audio_ch;
#if defined(CONFIG_AMEBAPRO3)
typedef enum {
	HPF_FS_0 = 0,
	HPF_FS_1 = 1,
	HPF_FS_2 = 2,
	HPF_FS_3 = 3,
	HPF_FS_4 = 4,
	HPF_FS_5 = 5,
	HPF_FS_6 = 6,
	HPF_FS_7 = 7
} audio_hpf_fc;

typedef enum {
	EQ_0  = ADCEQBD0,
	EQ_1  = ADCEQBD1,
	EQ_2  = ADCEQBD2,
	EQ_3  = ADCEQBD3,
	EQ_4  = ADCEQBD4
} audio_eq;
#endif

typedef uint8_t audio_sidetone_boost_t;

/**
  \brief Describe audio tx and rx diff parameter
*/
typedef struct audio_diff_para_s {
	BOOL diff_en;
	audio_wl tx_wl;
	audio_wl rx_wl;
	audio_sr tx_sr;
	audio_sr rx_sr;
	audio_ch tx_ch;
	audio_ch rx_ch;
} audio_diff_para_t, *paudio_diff_para_t;

typedef void (*audio_irq_handler)(u32 arg, u8 *pbuf);

struct audio_s {
	SP_InitTypeDef      sp_init_tx;
	SP_InitTypeDef      sp_init_rx;

	I2S_InitTypeDef     i2s_init;

	GDMA_InitTypeDef    gdma_tx;
	GDMA_InitTypeDef    gdma_rx;
	u8                  *tx_buf;
	u8                  *rx_buf;
	u32                 page_size;
	u8                  page_num;
	u8                  tx_page_idx;
	u8                  rx_page_idx;

	audio_irq_handler   tx_handler;
	u32                 tx_handler_arg;
	audio_irq_handler   rx_handler;
	u32                 rx_handler_arg;
	audio_irq_handler   err_handler;
	u32                 err_handler_arg;

	BOOL                audio_initialized;
	BOOL                dac_l_dmute_en;
	BOOL                adc_l_dmute_en;
	BOOL                adc_r_dmute_en;
	u8                  tx_started;
	u8                  rx_started;
	u8                  tx_err_cnt;
	u8                  rx_err_cnt;
	u8                  sport_idx;
	audio_input_mode    input_mode;
	audio_output_mode   output_mode;
};

typedef struct audio_s audio_t;

/**
  * @brief  Initializes the AUDIO device, include clock/function/interrupt/AUDIO registers.
  * @param  obj: Audio object define in application software.
  * @param  output_mode: Select the output mode.
  *            @arg OUTPUT_DISABLE: Disable audio output.
  *            @arg OUTPUT_SINGLE_ENDED: Single-Ended mode.
  *            @arg OUTPUT_DIFFERENTIAL: Differential mode.
  *            @arg OUTPUT_CAPLESS: Capless mode.
  * @param  input_mode: Select the input mode.
  *            @arg INPUT_DISABLE: Disable audio input.
  *            @arg LINE_IN_MODE: Line in mode.
  *            @arg MIC_DIFFERENTIAL: MIC differential mode.
  *            @arg MIC_SINGLE_EDNED: MIC Single-Ended mode.
  * @retval none
  */
void audio_init(audio_t *obj, audio_output_mode output_mode, audio_input_mode input_mode);

/**
  * @brief  Deinitializes the AUDIO device, include function/interrupt/AUDIO registers.
  * @param  obj: Audio object define in application software.
  * @retval none
  */
void audio_deinit(audio_t *obj);

/**
  * @brief  Set the audio sample rate and word length.
  * @param  obj: Audio object define in application software.
  * @param  sample_rate: this parameter can be one of the following values:
  *            @arg ASR_8KHZ: sample rate is 8kHz
  *            @arg ASR_16KHZ: sample rate is 16kHz
  *            @arg ASR_32KHZ: sample rate is 32kHz
  *            @arg ASR_44p1KHZ: sample rate is 44.1kHz
  *            @arg ASR_48KHZ: sample rate is 48kHz
  *            @arg ASR_88p2KHZ: sample rate is 88.2kHz
  *            @arg ASR_96KHZ: sample rate is 96kHz
  * @param  word_length: this parameter can be one of the following values:
  *            @arg WL_8BIT: sample bit is 8 bit
  *            @arg WL_16BIT: sample bit is 16 bit
  *            @arg WL_24BIT: sample bit is 24 bit (World length is 24bits but ADC and DAC are 16bits)
  * @retval none
  */
void audio_set_param(audio_t *obj, audio_sr sample_rate, audio_wl word_length);

/**
  * @brief  Set the buffer address.
  * @param  obj: Audio object define in application software.
  * @param  tx_buf: Set the address of tx buffer. Need cache-line aligned (64 bytes on CA32).
  * @param  rx_buf: Set the address of rx buffer. Need cache-line aligned (64 bytes on CA32).
  * @param  page_size: This unit is byte.
  * @param  page_num: This page num.
  * @retval none
  */
void audio_set_dma_buffer(audio_t *obj, u8 *tx_buf, u8 *rx_buf, u32 page_size, audio_page_num page_num);

/**
  * @brief  Set the audio sample rate and word length.
  * @param  obj: Audio object define in application software.
  * @param  sample_rate: this parameter can be one of the following values:
  *            @arg ASR_8KHZ: sample rate is 8kHz
  *            @arg ASR_16KHZ: sample rate is 16kHz
  *            @arg ASR_32KHZ: sample rate is 32kHz
  *            @arg ASR_44p1KHZ: sample rate is 44.1kHz
  *            @arg ASR_48KHZ: sample rate is 48kHz
  *            @arg ASR_88p2KHZ: sample rate is 88.2kHz
  *            @arg ASR_96KHZ: sample rate is 96kHz
  * @param  word_length: this parameter can be one of the following values:
  *            @arg WL_8BIT: sample bit is 8 bit
  *            @arg WL_16BIT: sample bit is 16 bit
  *            @arg WL_24BIT: sample bit is 24 bit (World length is 24bits but ADC and DAC are 16bits)
  * @param  tx_ch: select tx channel:
  *            @arg A_MONO: mono
  *            @arg A_STEREO: stereo
  * @param  rx_ch: select rx channel:
  *            @arg A_MONO: mono
  *            @arg A_STEREO: stereo
  * @retval none
  */
void audio_set_param_adv(audio_t *obj, audio_sr sample_rate, audio_wl word_length, audio_ch tx_ch, audio_ch rx_ch);

/**
  * @brief  Set tx interrupt handler.
  * @param  obj: Audio object define in application software.
  * @param  tx_handler: User defined IRQ callback function.
  * @param  arg:  User defined IRQ callback parameter.
  * @retval none
  */
void audio_tx_irq_handler(audio_t *obj, audio_irq_handler tx_handler, u32 *arg);

/**
  * @brief  Set rx interrupt handler.
  * @param  obj: Audio object define in application software.
  * @param  rx_handler: User defined IRQ callback function.
  * @param  arg:  User defined IRQ callback parameter.
  * @retval none
  */
void audio_rx_irq_handler(audio_t *obj, audio_irq_handler rx_handler, u32 *arg);

/**
  * @brief  Get the current tx page address.
  * @param  obj: Audio object define in application software.
  * @retval Tx page address
  */
u8 *audio_get_tx_page_adr(audio_t *obj);

/**
  * @brief  Inform Audio the tx page data of the channel is ready.
  * @param  obj: Audio object define in application software.
  * @param  pbuf: tx buffer adderss.
  * @retval none
  */
void audio_set_tx_page(audio_t *obj, u8 *pbuf);

/**
  * @brief  Inform Audio that finish receiving the rx page data of the channel.
  * @param  obj: Audio object define in application software.
  * @retval none
  */
void audio_set_rx_page(audio_t *obj);

/**
  * @brief  Start the tx transmission.
  * @param  obj: Audio object define in application software.
  * @retval none
  */
void audio_tx_start(audio_t *obj);

/**
  * @brief  Start the rx transmission.
  * @param  obj: Audio object define in application software.
  * @retval none
  */
void audio_rx_start(audio_t *obj);

/**
  * @brief  Start the tx and rx transmission.
  * @param  obj: Audio object define in application software.
  * @retval none
  */
void audio_trx_start(audio_t *obj);

/**
  * @brief  Stop the tx transmission.
  * @param  obj: Audio object define in application software.
  * @retval none
  */
void audio_tx_stop(audio_t *obj);

/**
  * @brief  Stop the rx transmission.
  * @param  obj: Audio object define in application software.
  * @retval none
  */
void audio_rx_stop(audio_t *obj);

/**
  * @brief  Stop the tx and rx transmission.
  * @param  obj: Audio object define in application software.
  * @retval none
  */
void audio_trx_stop(audio_t *obj);

/**
  * @brief  Get the error count of tx transmission when happen the interrupt.
  * @param  obj: Audio object define in application software.
  * @retval Tx error count
  */
u8 audio_get_tx_error_cnt(audio_t *obj);

/**
  * @brief  Get the error count of rx transmission when happen the interrupt.
  * @param  obj: Audio object define in application software.
  * @retval Rx error count
  */
u8 audio_get_rx_error_cnt(audio_t *obj);

/**
  * @brief  clean the error count.
  * @param  obj: Audio object define in application software.
  * @retval none
  */
void audio_clean_error_cnt(audio_t *obj);

/**
  * @brief  Make the data from tx to rx and bypass the audio codec.
  * @param  obj: Audio object define in application software.
  * @retval none
  */
void audio_sport_loopback(audio_t *obj, BOOL en);

/**
  * @brief  Control the mute of the microphone.
  * @param  obj: Audio object define in application software.
  * @param  en: To enable or disable mute.
  * @retval none
  */
void audio_mic_analog_mute(audio_t *obj, BOOL en);

/**
  * @brief  Control the boost gain of the microphone.
  * @param  obj: Audio object define in application software.
  * @param  en: To enable or disable.
  * @param  mic_gain: Select the microphone gain.
  *            @arg MIC_0DB: 0dB.
  *            @arg MIC_20DB: 20dB.
  *            @arg MIC_30DB: 30dB.
  *            @arg MIC_40DB: 40dB.
  * @retval none
  */
void audio_mic_analog_gain(audio_t *obj, BOOL en, audio_mic_gain mic_gain);

/**
  * @brief  Control the digital gain of ADC.
  * @param  obj: Audio object define in application software.
  * @param  step: The digital volume. Every Step is 0.375dB.
  *            @arg 0x7F: 30dB.
  *            @arg ...
  *            @arg 0x2F: 0dB.
  *            @arg ...
  *            @arg 0x00: -17.625dB.
  * @retval none
  */
void audio_adc_digital_vol(audio_t *obj, u8 step);

/**
  * @brief  Control the digital mute of ADC.
  * @param  obj: Audio object define in application software.
  * @param  mute_en: To enable or disable mute.
  * @retval none
  */
void audio_adc_digital_mute(audio_t *obj, BOOL mute_en);

/**
  * @brief  Control the mute of the headphone.
  * @param  obj: Audio object define in application software.
  * @param  en: To enable or disable mute.
  * @retval none
  */
void audio_headphone_analog_mute(audio_t *obj, BOOL en);

/**
  * @brief  Control the digital gain of DAC.
  * @param  obj: Audio object define in application software.
  * @param  step: The digital volume. Every Step is 0.375dB.
  *            @arg 0xAF: 0dB.
  *            @arg 0xAE: -0.375dB.
  *            @arg ...
  *            @arg 0x00: -65.625dB.
  * @retval none
  */
void audio_dac_digital_vol(audio_t *obj, u8 step);

/**
  * @brief  Control the digital mute of DAC.
  * @param  obj: Audio object define in application software.
  * @param  mute_en: To enable or disable mute.
  * @retval none
  */
void audio_dac_digital_mute(audio_t *obj, BOOL mute_en);

/**
  * @brief  Control the mic bias.
  * @param  obj: Audio object define in application software.
  * @param  en: 0 is disable, 1 is enable.
  * @retval none
  */
void audio_mic_bias_ctrl(audio_t *obj, BOOL en);

/**
  * @brief  Control the amplifier of the headphone. The default value is enabled.
  * @param  obj: Audio object define in application software.
  * @param  en: 0 is disable, 1 is enable.
  * @retval none
  */
void audio_hpo_amplifier(audio_t *obj, BOOL en);


/**
  * @brief  Make ADC data to DAC data.
  * @param  obj: Audio object define in application software.
  * @param  en: 0 is disable, 1 is enable.
  * @retval none
  */
void audio_adda_loopback(audio_t *obj, BOOL en);

/**
  * @brief  Make ADC data to DAC data.
  * @param  obj: Audio object define in application software.
  * @param  dmic_clk: PE_0 or PE_2
  * @param  dmic_data: PE_4
  * @retval none
  */
void audio_dmic_pinmux(audio_t *obj, PinName dmic_clk, PinName dmic_data);

/**
  * @brief  Make ADC data to DAC data.
  * @param  obj: Audio object define in application software.
  * @param  dmic_clk: PE_0 or PE_2
  * @param  dmic_data: PE_4
  * @retval none
  */

void audio_dmic_depinmux(audio_t *obj, PinName dmic_clk, PinName dmic_data);
/**
  * @brief  Make TX and RX different rate.
  * @param  obj: Audio object define in application software.
  * @param  diff_para: Set parameters.
  * @retval none
  */
void audio_set_diff_tx_rx_param(audio_t *obj, audio_diff_para_t *pdiff_para);

#if defined(CONFIG_AMEBAPRO3)
void audio_err_irq_handler(audio_t *obj, audio_irq_handler err_handler, u32 *arg);
/**
  * @brief  Control the ADC high pass filter of the left channel..
  * @param  obj: Audio object define in application software.
  * @param  en:  Control the HPF: 0 is disable, 1 is enable.
  * @param  audio_hpf_fc: Set the cutoff frequency.
  *                      -0:  The cutoff frequency is 0.005*fs.
  *                      -1:  The cutoff frequency is 0.0025*fs.
  *                      -2:  The cutoff frequency is 0.00125*fs.
  *                      -3:  The cutoff frequency is 0.000625*fs.
  *                      -4:  The cutoff frequency is 0.0003125*fs.
  *                      -5:  The cutoff frequency is 0.00015625*fs.
  *                      -6:  The cutoff frequency is 0.000078125*fs.
  *                      -7:  The cutoff frequency is 0.0000390625*fs.
  * @retval none
  */
void audio_adc_l_hpf(audio_t *obj, BOOL en, audio_hpf_fc hpf_fc);

/**
  * @brief  Control the ADC high pass filter of the right channel..
  * @param  obj: Audio object define in application software.
  * @param  en:  Control the HPF: 0 is disable, 1 is enable.
  * @param  audio_hpf_fc: Set the cutoff frequency.
  *                      -0:  The cutoff frequency is 0.005*fs.
  *                      -1:  The cutoff frequency is 0.0025*fs.
  *                      -2:  The cutoff frequency is 0.00125*fs.
  *                      -3:  The cutoff frequency is 0.000625*fs.
  *                      -4:  The cutoff frequency is 0.0003125*fs.
  *                      -5:  The cutoff frequency is 0.00015625*fs.
  *                      -6:  The cutoff frequency is 0.000078125*fs.
  *                      -7:  The cutoff frequency is 0.0000390625*fs.
  * @retval none
  */
void audio_adc_r_hpf(audio_t *obj, BOOL en, audio_hpf_fc hpf_fc);

/**
 * @brief Set input 5-band equalizers for the left channel
 * @param  obj: Audio object define in application software.
 * @param  eq: Select the EQ number.
 * @param  en: 0 is disable, 1 is enable.
 * @param  h0: eq parameter.
 * @param  b0: eq parameter.
 * @param  b1: eq parameter.
 * @param  a0: eq parameter.
 * @param  a1: eq parameter.
 *  @retval none
 */
void audio_input_l_eq(audio_t *obj, audio_eq eq, BOOL en, u32 h0, u32 b0, u32 b1, u32 a0, u32 a1);

/**
 * @brief Set input 5-band equalizers for the right channel
 * @param  obj: Audio object define in application software.
 * @param  eq: Select the EQ number.
 * @param  en: 0 is disable, 1 is enable.
 * @param  h0: eq parameter.
 * @param  b0: eq parameter.
 * @param  b1: eq parameter.
 * @param  a0: eq parameter.
 * @param  a1: eq parameter.
 *  @retval none
 */
void audio_input_r_eq(audio_t *obj, audio_eq eq, BOOL en, u32 h0, u32 b0, u32 b1, u32 a0, u32 a1);

/**
 * @brief Set output 5-band equalizers for the left channel
 * @param  obj: Audio object define in application software.
 * @param  eq: Select the EQ number.
 * @param  en: 0 is disable, 1 is enable.
 * @param  h0: eq parameter.
 * @param  b0: eq parameter.
 * @param  b1: eq parameter.
 * @param  a0: eq parameter.
 * @param  a1: eq parameter.
 *  @retval none
 */
void audio_output_l_eq(audio_t *obj, audio_eq eq, BOOL en, u32 h0, u32 b0, u32 b1, u32 a0, u32 a1);

/**
  * @brief  Control left channel digital microphone gain .
  * @param  obj: Audio object define in application software.
  * @param  dmic_gain: 0,12,24,36 dB
  * @retval none
  */
void audio_l_dmic_gain(audio_t *obj, audio_dmic_gain dmic_gain);

/**
  * @brief  Control right channel digital microphone gain .
  * @param  obj: Audio object define in application software.
  * @param  dmic_gain: 0,12,24,36 dB
  * @retval none
  */
void audio_r_dmic_gain(audio_t *obj, audio_dmic_gain dmic_gain);

void audio_set_param_adv(audio_t *obj, audio_sr sample_rate, audio_wl word_length, audio_ch tx_ch, audio_ch rx_ch);

uint32_t audio_get_tx_start_status(audio_t *obj);
uint32_t audio_get_rx_start_status(audio_t *obj);
void audio_dac_clk(audio_t *obj, BOOL dac_clk_en);
void audio_adc_clk(audio_t *obj, BOOL adc_l_clk_en, BOOL adc_r_clk_en);
void audio_poweroff(void);
#endif

/*\@}*/

#ifdef __cplusplus
}
#endif

typedef struct _AUDIO_APP_TABLE_ {
	void(*a_func)(void);
	char *a_func_name;
} AUDIO_APP_TABLE, *PAUDIO_APP_TABLE;

#endif
