/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "ameba_soc.h"
#include "ameba_audio.h"

static const char *TAG = "CODEC";
/** @addtogroup Ameba_Periph_Driver
  * @{
  */

/** @defgroup AUDIO
* @brief AUDIO driver modules
* @{
*/

/** @defgroup AUDIO_CODEC
* @brief AUDIO_CODEC driver modules
* @{
*/

/* Exported functions ------------------------------------------------------------*/
/** @defgroup AUDIO_CODEC_Exported_Functions AUDIO_CODEC Exported Functions
  * @{
  */

/**
  * @brief  Get audio codec secure or non-secure base address.
  * @return  Audio codec secure or non-secure base address.
  */
AUDIO_CODEC_TypeDef *AUDIO_CODEC_GetAddr(void)
{
	if (TrustZone_IsSecure()) {
		return (AUDIO_CODEC_TypeDef *)AUDIO_DIG_REG_BASE_S;
	} else {
		return (AUDIO_CODEC_TypeDef *)AUDIO_DIG_REG_BASE;
	}

}

/**
  * @brief  Get analog audio secure or non-secure base address.
  * @return  Analog audio secure or non-secure base address.
  */
AUD_TypeDef *AUDIO_AUD_GetAddr(void)
{
	if (TrustZone_IsSecure()) {
		return (AUD_TypeDef *)AUDIO_ANA_REG_BASE_S;
	} else {
		return (AUD_TypeDef *)AUDIO_ANA_REG_BASE;
	}
}

/**
  * @brief  Enable or disable audio codec IP.
  * @param  newstate: audio IP enable or disable.
  *			 This parameter can be one of the following values:
  *			   @arg ENABLE
  *			   @arg DISABLE
  * @return  None
  */
void AUDIO_CODEC_SetAudioIP(u32 newstate)
{
	AUDIO_CODEC_TypeDef *audio_base = AUDIO_CODEC_GetAddr();

	if (newstate == ENABLE) {
		audio_base->AUDIO_CONTROL_0 |= AUDIO_CODEC_BIT_AUDIO_IP_EN;
	} else {
		audio_base->AUDIO_CONTROL_0 &= ~AUDIO_CODEC_BIT_AUDIO_IP_EN;
	}
}


/**
  * @brief  Enable or disable I2S IP.
  * @param  newstate: I2S IP enable or disable.
  *			 This parameter can be one of the following values:
  *			   @arg ENABLE
  *			   @arg DISABLE
  * @return  None
  */
void AUDIO_CODEC_SetI2SIP(u32 newstate)
{
	AUDIO_CODEC_TypeDef *audio_base = AUDIO_CODEC_GetAddr();

	if (newstate == ENABLE) {
		audio_base->I2S_0_CONTROL |= AUDIO_CODEC_BIT_I2S_0_RST_N_REG;
	} else {
		audio_base->I2S_0_CONTROL &= ~AUDIO_CODEC_BIT_I2S_0_RST_N_REG;
	}
}

/**
  * @brief  Select I2S master source.
  * @param  src: source
  *          This parameter can be one of the following values:
  *            @arg INTERNAL_SPORT
  *            @arg EXTERNAL_I2S
  * @return  None
  */
void AUDIO_CODEC_SetI2SSRC(u32 src)
{
	assert_param(IS_CODEC_I2S_MASTER_SRC(src));

	AUDIO_CODEC_TypeDef *audio_base = AUDIO_CODEC_GetAddr();

	if (src == INTERNAL_SPORT) {
		audio_base->I2S_0_CONTROL &= ~AUDIO_CODEC_BIT_I2S_0_MASTER_SEL;
	} else {
		audio_base->I2S_0_CONTROL |= AUDIO_CODEC_BIT_I2S_0_MASTER_SEL;
	}
}

/**
  * @brief  Enable or disable ad/da ananlog clock.
  * @param  newstate: enable or disable dac/adc analog clock.
  *			 This parameter can be one of the following values:
  *			   @arg ENABLE
  *			   @arg DISABLE
  * @return  None
  */
void AUDIO_CODEC_SetANAClk(u32 newstate)
{
	AUDIO_CODEC_TypeDef *audio_base = AUDIO_CODEC_GetAddr();

	if (newstate == ENABLE) {
		audio_base->CLOCK_CONTROL_1 |= AUDIO_CODEC_BIT_DA_ANA_CLK_EN | AUDIO_CODEC_BIT_AD_ANA_CLK_EN;
	} else {
		audio_base->CLOCK_CONTROL_1 &= ~(AUDIO_CODEC_BIT_DA_ANA_CLK_EN | AUDIO_CODEC_BIT_AD_ANA_CLK_EN);
	}
}

/**
  * @brief  Select i2s rx tdm mode
  * @param  tdmmode: select adc input path channel
  *			 This parameter can be one of the following values:
  *			   @arg I2S_NOTDM
  *			   @arg I2S_TDM4
  * @return  None
  */
void AUDIO_CODEC_SetI2SRXTDM(u32 tdmmode)
{
	assert_param(IS_CODEC_I2S_TDM_SEL(tdmmode));

	AUDIO_CODEC_TypeDef *audio_base = AUDIO_CODEC_GetAddr();

	audio_base->I2S_0_CONTROL &= ~(AUDIO_CODEC_MASK_I2S_0_TDM_MODE_RX);
	audio_base->I2S_0_CONTROL |= AUDIO_CODEC_I2S_0_TDM_MODE_RX(tdmmode);

	audio_base->I2S_0_CONTROL_1 &= ~(AUDIO_CODEC_MASK_I2S_0_DATA_CH0_SEL_RX | AUDIO_CODEC_MASK_I2S_0_DATA_CH1_SEL_RX | AUDIO_CODEC_MASK_I2S_0_DATA_CH2_SEL_RX |
									 AUDIO_CODEC_BIT_I2S_0_DATA_CH0_RX_DISABLE | AUDIO_CODEC_BIT_I2S_0_DATA_CH1_RX_DISABLE | AUDIO_CODEC_BIT_I2S_0_DATA_CH2_RX_DISABLE);

	if (tdmmode == I2S_NOTDM) {
		audio_base->I2S_0_CONTROL_1 |= (AUDIO_CODEC_I2S_0_DATA_CH0_SEL_RX(0) | AUDIO_CODEC_I2S_0_DATA_CH1_SEL_RX(1) | AUDIO_CODEC_BIT_I2S_0_DATA_CH2_RX_DISABLE);
	} else if (tdmmode == I2S_TDM4) {
		audio_base->I2S_0_CONTROL_1 |= (AUDIO_CODEC_I2S_0_DATA_CH0_SEL_RX(0) | AUDIO_CODEC_I2S_0_DATA_CH1_SEL_RX(1) | AUDIO_CODEC_I2S_0_DATA_CH2_SEL_RX(2));
	} else {

	}
}

/**
  * @brief  Default I2S initialization parameter.
  * @return  None
  */
void AUDIO_CODEC_I2S_StructInit(I2S_InitTypeDef *I2S_Struct)
{
	I2S_Struct->CODEC_SelI2STxSR = SR_48K;
	I2S_Struct->CODEC_SelI2STxWordLen = WL_16;
	I2S_Struct->CODEC_SelI2STxCHLen = CL_32;
	I2S_Struct->CODEC_SelI2STxCH = CH_LR;
	I2S_Struct->CODEC_SelI2STxDataFormat = DF_I2S;
	I2S_Struct->CODEC_SelI2SRxSR = SR_48K;
	I2S_Struct->CODEC_SelI2SRxWordLen = WL_16;
	I2S_Struct->CODEC_SelI2SRxCHLen = CL_32;
	I2S_Struct->CODEC_SelI2SRxDataFormat = DF_I2S;
	I2S_Struct->CODEC_SelRxI2STdm = I2S_NOTDM;

}

/**
  * @brief  Set I2S initialization parameter.
  * @param  path: audio work path.
  *			 This parameter can be one of the following values:
  *			   @arg DACPATH
  *			   @arg ADCPATH
  * @return  None
  */
void AUDIO_CODEC_SetI2SParameters(u32 path, I2S_InitTypeDef *I2S_Struct)
{
	assert_param(IS_CODEC_WORK_PATH(path));

	AUDIO_CODEC_TypeDef *audio_base = AUDIO_CODEC_GetAddr();

	if (path == DACPATH) {
		assert_param(IS_CODEC_CL_SEL(I2S_Struct->CODEC_SelI2STxCHLen));
		assert_param(IS_CODEC_WL_SEL(I2S_Struct->CODEC_SelI2STxWordLen));
		assert_param(IS_CODEC_DATA_FMT(I2S_Struct->CODEC_SelI2STxDataFormat));
		assert_param(IS_CODEC_TXCHN_SEL(I2S_Struct->CODEC_SelI2STxCH));

		audio_base->I2S_0_CONTROL &= ~(AUDIO_CODEC_MASK_I2S_0_CH_LEN_SEL_TX | AUDIO_CODEC_MASK_I2S_0_DATA_LEN_SEL_TX |
									   AUDIO_CODEC_MASK_I2S_0_DATA_FORMAT_SEL_TX | AUDIO_CODEC_MASK_I2S_0_DATA_CH_SEL_TX);

		audio_base->I2S_0_CONTROL |= (AUDIO_CODEC_I2S_0_CH_LEN_SEL_TX(I2S_Struct->CODEC_SelI2STxCHLen) | AUDIO_CODEC_I2S_0_DATA_LEN_SEL_TX(
										  I2S_Struct->CODEC_SelI2STxWordLen) |
									  AUDIO_CODEC_I2S_0_DATA_FORMAT_SEL_TX(I2S_Struct->CODEC_SelI2STxDataFormat) | AUDIO_CODEC_I2S_0_DATA_CH_SEL_TX(I2S_Struct->CODEC_SelI2STxCH));
	} else {
		assert_param(IS_CODEC_CL_SEL(I2S_Struct->CODEC_SelI2SRxCHLen));
		assert_param(IS_CODEC_WL_SEL(I2S_Struct->CODEC_SelI2SRxWordLen));
		assert_param(IS_CODEC_DATA_FMT(I2S_Struct->CODEC_SelI2SRxDataFormat));

		audio_base->I2S_0_CONTROL &= ~(AUDIO_CODEC_MASK_I2S_0_CH_LEN_SEL_RX | AUDIO_CODEC_MASK_I2S_0_DATA_LEN_SEL_RX |
									   AUDIO_CODEC_MASK_I2S_0_DATA_FORMAT_SEL_RX);
		audio_base->I2S_0_CONTROL |= (AUDIO_CODEC_I2S_0_CH_LEN_SEL_RX(I2S_Struct->CODEC_SelI2SRxCHLen) | AUDIO_CODEC_I2S_0_DATA_LEN_SEL_RX(
										  I2S_Struct->CODEC_SelI2SRxWordLen) |
									  AUDIO_CODEC_I2S_0_DATA_FORMAT_SEL_RX(I2S_Struct->CODEC_SelI2SRxDataFormat));

		AUDIO_CODEC_SetI2SRXTDM(I2S_Struct->CODEC_SelRxI2STdm);
	}
}

/**
  * @brief  Select adc sample rate and source, all adc share souce and sample rate.
  * @param  src: source
  *          This parameter can be one of the following values:
  *            @arg SOURCE0
  *            @arg SOURCE1
  *            @arg SOURCE2
  * 		   @arg SOURCE3
  * @param  sr: sample rate
  * @return  None
  */
void AUDIO_CODEC_SetADCSRSrc(u32 src, u32 sr)
{
	assert_param(IS_CODEC_SR_SRC(src));
	assert_param(IS_CODEC_SR_SEL(sr));

	AUDIO_CODEC_TypeDef *audio_base = AUDIO_CODEC_GetAddr();

	audio_base->CLOCK_CONTROL_5 &= ~(AUDIO_CODEC_MASK_ADC_0_FS_SRC_SEL | AUDIO_CODEC_MASK_ADC_1_FS_SRC_SEL | AUDIO_CODEC_MASK_ADC_2_FS_SRC_SEL);
	audio_base->CLOCK_CONTROL_5 |= AUDIO_CODEC_ADC_0_FS_SRC_SEL(src) | AUDIO_CODEC_ADC_1_FS_SRC_SEL(src) | AUDIO_CODEC_ADC_2_FS_SRC_SEL(src);

	if (src == SOURCE0) {
		audio_base->CLOCK_CONTROL_4 &= ~AUDIO_CODEC_MASK_SAMPLE_RATE_0;
		audio_base->CLOCK_CONTROL_4 |= AUDIO_CODEC_SAMPLE_RATE_0(sr);
	} else if (src == SOURCE1) {
		audio_base->CLOCK_CONTROL_4 &= ~AUDIO_CODEC_MASK_SAMPLE_RATE_1;
		audio_base->CLOCK_CONTROL_4 |= AUDIO_CODEC_SAMPLE_RATE_1(sr);
	} else if (src == SOURCE2) {
		audio_base->CLOCK_CONTROL_4 &= ~AUDIO_CODEC_MASK_SAMPLE_RATE_2;
		audio_base->CLOCK_CONTROL_4 |= AUDIO_CODEC_SAMPLE_RATE_2(sr);
	} else {
		audio_base->CLOCK_CONTROL_4 &= ~AUDIO_CODEC_MASK_SAMPLE_RATE_3;
		audio_base->CLOCK_CONTROL_4 |= AUDIO_CODEC_SAMPLE_RATE_3(sr);
	}
}

/**
  * @brief  Select dac sample rate and source .
  * @param  src: source
  *          This parameter can be one of the following values:
  *            @arg SOURCE0
  *            @arg SOURCE1
  *            @arg SOURCE2
  * 		   @arg SOURCE3
  * @param  sr: sample rate
  * @return  None
  */
void AUDIO_CODEC_SetDACSRSrc(u32 src, u32 sr)
{
	assert_param(IS_CODEC_SR_SRC(src));
	assert_param(IS_CODEC_SR_SEL(sr));

	AUDIO_CODEC_TypeDef *audio_base = AUDIO_CODEC_GetAddr();

	audio_base->CLOCK_CONTROL_5 &= ~AUDIO_CODEC_MASK_DAC_L_FS_SRC_SEL;
	audio_base->CLOCK_CONTROL_5 |= AUDIO_CODEC_DAC_L_FS_SRC_SEL(src);

	if (src == SOURCE0) {
		audio_base->CLOCK_CONTROL_4 &= ~AUDIO_CODEC_MASK_SAMPLE_RATE_0;
		audio_base->CLOCK_CONTROL_4 |= AUDIO_CODEC_SAMPLE_RATE_0(sr);
	} else if (src == SOURCE1) {
		audio_base->CLOCK_CONTROL_4 &= ~AUDIO_CODEC_MASK_SAMPLE_RATE_1;
		audio_base->CLOCK_CONTROL_4 |= AUDIO_CODEC_SAMPLE_RATE_1(sr);
	} else if (src == SOURCE2) {
		audio_base->CLOCK_CONTROL_4 &= ~AUDIO_CODEC_MASK_SAMPLE_RATE_2;
		audio_base->CLOCK_CONTROL_4 |= AUDIO_CODEC_SAMPLE_RATE_2(sr);
	} else {
		audio_base->CLOCK_CONTROL_4 &= ~AUDIO_CODEC_MASK_SAMPLE_RATE_3;
		audio_base->CLOCK_CONTROL_4 |= AUDIO_CODEC_SAMPLE_RATE_3(sr);
	}
}

/**
  * @brief  Enable or disable per AD and AD fifo channel clock.
  * @param  ad_chn: select ad channel.
  *			 This parameter can be one of the following values:
  *			   @arg ADCHN0
  *			   @arg ADCHN1
  *			   @arg ADCHN2
  * @param  newstate: enable or disable per ad channel.
  *			 This parameter can be one of the following values:
  *			   @arg ENABLE
  *			   @arg DISABLE
  * @return  None
  */
void AUDIO_CODEC_EnableADCFifo(u32 ad_chn, u32 newstate)
{
	assert_param(IS_CODEC_ADCHN_SEL(ad_chn));

	AUDIO_CODEC_TypeDef *audio_base = AUDIO_CODEC_GetAddr();

	switch (ad_chn) {

	case ADCHN0:
		if (newstate == ENABLE) {
			audio_base->CLOCK_CONTROL_1 |= AUDIO_CODEC_BIT_AD_0_FIFO_EN;
		} else {
			audio_base->CLOCK_CONTROL_1 &= ~AUDIO_CODEC_BIT_AD_0_FIFO_EN;
		}
		break;
	case ADCHN1:
		if (newstate == ENABLE) {
			audio_base->CLOCK_CONTROL_1 |= AUDIO_CODEC_BIT_AD_1_FIFO_EN;
		} else {
			audio_base->CLOCK_CONTROL_1 &= ~AUDIO_CODEC_BIT_AD_1_FIFO_EN;
		}
		break;
	case ADCHN2:
		if (newstate == ENABLE) {
			audio_base->CLOCK_CONTROL_1 |= AUDIO_CODEC_BIT_AD_2_FIFO_EN;
		} else {
			audio_base->CLOCK_CONTROL_1 &= ~AUDIO_CODEC_BIT_AD_2_FIFO_EN;
		}
		break;
	default:
		break;
	}
}


/**
  * @brief  Enable or disable per AD and AD channel clock.
  * @param  ad_chn: select ad channel.
  *			 This parameter can be one of the following values:
  *			   @arg ADCHN0
  *			   @arg ADCHN1
  *			   @arg ADCHN2
  * @param  newstate: enable or disable per ad channel.
  *			 This parameter can be one of the following values:
  *			   @arg ENABLE
  *			   @arg DISABLE
  * @return  None
  */
void AUDIO_CODEC_EnableADC(u32 ad_chn, u32 newstate)
{
	assert_param(IS_CODEC_ADCHN_SEL(ad_chn));

	AUDIO_CODEC_TypeDef *audio_base = AUDIO_CODEC_GetAddr();

	switch (ad_chn) {

	case ADCHN0:
		if (newstate == ENABLE) {
			audio_base->CLOCK_CONTROL_1 |= AUDIO_CODEC_BIT_AD_0_EN;
		} else {
			audio_base->CLOCK_CONTROL_1 &= ~ AUDIO_CODEC_BIT_AD_0_EN;
		}
		break;

	case ADCHN1:
		if (newstate == ENABLE) {
			audio_base->CLOCK_CONTROL_1 |= AUDIO_CODEC_BIT_AD_1_EN;
		} else {
			audio_base->CLOCK_CONTROL_1 &= ~ AUDIO_CODEC_BIT_AD_1_EN;
		}
		break;

	case ADCHN2:
		if (newstate == ENABLE) {
			audio_base->CLOCK_CONTROL_1 |= AUDIO_CODEC_BIT_AD_2_EN;
		} else {
			audio_base->CLOCK_CONTROL_1 &= ~ AUDIO_CODEC_BIT_AD_2_EN;
		}
		break;

	default:
		break;
	}
}


/**
  * @brief  Set the gain of ADC digital volume.
  * @param  adc_sel: select adc number.
  *          This parameter can be one of the following values:
  *            @arg ADC0
  *            @arg ADC1
  *            @arg ADC2
  * @param  Gain: the value of gain.
  *          This parameter can be -17.625dB~48dB in 0.375dB step
  *            @arg 0x00 -17.625dB
  *            @arg 0xaf 48dB
  * @retval None
  */
void AUDIO_CODEC_SetADCVolume(u32 adc_sel, u32 gain)
{
	assert_param(IS_CODEC_ADC_SEL(adc_sel));

	AUDIO_CODEC_TypeDef *audio_base = AUDIO_CODEC_GetAddr();

	audio_base->ADC_CH_CTRL[adc_sel].CODEC_ADC_x_CONTROL_1 &= ~AUDIO_CODEC_MASK_ADC_x_AD_GAIN;
	audio_base->ADC_CH_CTRL[adc_sel].CODEC_ADC_x_CONTROL_1 |= AUDIO_CODEC_ADC_x_AD_GAIN(gain) ;
}


/**
  * @brief  Set per ADC channel HPF mode and select HPF FC
  * @param  adc_sel: select adc channel.
  *			 This parameter can be one of the following values:
  *			   @arg ADC0
  *			   @arg ADC1
  *			   @arg ADC2
  * @param  fc: select high pass filter fc
  *			 This parameter can be one of the following values:
  *			   @arg 0：5e10^-3 Fs
  *			   @arg 1：2.5e10^-3 Fs
  *			   @arg 2：1.125e10^-3 Fs
  *			   @arg 3: 6.25e10^-4 Fs
  *			   @arg 4: 3.125e10^-4 Fs
  *			   @arg 5: 1.5625e10^-4 Fs
  *			   @arg 6: 7.8125e10^-5 Fs
  *			   @arg 7: 3.90625e10^-5 Fs
  * @param  newstate: enable or disable per ad channel HPF mode
  *			 This parameter can be one of the following values:
  *			   @arg ENABLE
  *			   @arg DISABLE
  * @return  None
  */
void AUDIO_CODEC_SetADCHPF(u32 adc_sel, u32 fc, u32 newstate)
{
	assert_param(IS_CODEC_ADC_SEL(adc_sel));

	AUDIO_CODEC_TypeDef *audio_base = AUDIO_CODEC_GetAddr();

	if (newstate == ENABLE) {
		audio_base->ADC_CH_CTRL[adc_sel].CODEC_ADC_x_CONTROL_0 &= ~AUDIO_CODEC_MASK_ADC_x_DCHPF_FC_SEL;
		audio_base->ADC_CH_CTRL[adc_sel].CODEC_ADC_x_CONTROL_0 |= AUDIO_CODEC_BIT_ADC_x_DCHPF_EN | AUDIO_CODEC_ADC_x_DCHPF_FC_SEL(fc);
	} else {
		audio_base->ADC_CH_CTRL[adc_sel].CODEC_ADC_x_CONTROL_0 &= ~AUDIO_CODEC_BIT_ADC_x_DCHPF_EN;
	}
}

/**
  * @brief  Select I2S and set ADC ASRC mode according to the sample rate.
  * @param  adc_sel: select adc channel.
  *          This parameter can be one of the following values:
  *            @arg ADC0
  *            @arg ADC1
  *            @arg ADC2
  * @param  sr: the value of sample rate.
  * 			Uplink only support: 8/11.025/12/16/22.05/24/32/44.1/48/88.2/96KHz
  * 		   @arg SR_8K
  *  		   @arg SR_11P025K
  *   		   @arg SR_12K
  *   		   @arg SR_16K
  *   		   @arg SR_22P05K
  *   		   @arg SR_24K
  *   		   @arg SR_32K
  *   		   @arg SR_44P1K
  *   		   @arg SR_48K
  *   		   @arg SR_88P2K
  * 		   @arg SR_96K
  * @param  newstate: enable or disable ASRC mode.
  *			 This parameter can be one of the following values:
  *			   @arg DISABLE
  *			   @arg ENABLE
  * @retval None
  */
void AUDIO_CODEC_SetADCASRC(u32 adc_sel, u32 sr, u32 newstate)
{
	assert_param(IS_CODEC_ADC_SEL(adc_sel));
	assert_param(IS_CODEC_ADC_ASRC_SEL(sr));

	AUDIO_CODEC_TypeDef *audio_base = AUDIO_CODEC_GetAddr();

	if (sr == SR_96K || sr == SR_88P2K) {
		audio_base->ASRC_CONTROL_2 &= ~AUDIO_CODEC_MASK_ASRC_RATE_SEL_RX_0;
		audio_base->ASRC_CONTROL_2 |= AUDIO_CODEC_ASRC_RATE_SEL_RX_0(1);

	} else {

		audio_base->ASRC_CONTROL_2 &= ~AUDIO_CODEC_MASK_ASRC_RATE_SEL_RX_0;
		audio_base->ASRC_CONTROL_2 |= AUDIO_CODEC_ASRC_RATE_SEL_RX_0(0);

	}

	switch (adc_sel) {

	case ADC0:
		if (newstate == ENABLE) {
			audio_base->CLOCK_CONTROL_6 |= AUDIO_CODEC_BIT_ADC_0_ASRC_EN;
		} else {
			audio_base->CLOCK_CONTROL_6 &= ~AUDIO_CODEC_BIT_ADC_0_ASRC_EN;
		}
		break;

	case ADC1:
		if (newstate == ENABLE) {
			audio_base->CLOCK_CONTROL_6 |= AUDIO_CODEC_BIT_ADC_1_ASRC_EN;
		} else {
			audio_base->CLOCK_CONTROL_6 &= ~AUDIO_CODEC_BIT_ADC_1_ASRC_EN;
		}
		break;

	case ADC2:
		if (newstate == ENABLE) {
			audio_base->CLOCK_CONTROL_6 |= AUDIO_CODEC_BIT_ADC_2_ASRC_EN;
		} else {
			audio_base->CLOCK_CONTROL_6 &= ~AUDIO_CODEC_BIT_ADC_2_ASRC_EN;
		}
		break;

	default:
		break;
	}
}

/**
  * @brief  Mute ADC path.
  * @param  adc_sel: select adc channel.
  *			 This parameter can be one of the following values:
  *			   @arg ADC0
  *			   @arg ADC1
  *			   @arg ADC2
  * @param  newstate: mute or unmute option for ad channel
  *			 This parameter can be one of the following values:
  *			   @arg UNMUTE
  *			   @arg MUTE
  * @return  None
  */
void AUDIO_CODEC_SetADCMute(u32 adc_sel, u32 newstate)
{
	assert_param(IS_CODEC_ADC_SEL(adc_sel));
	assert_param(IS_CODEC_MUTE_SEL(newstate));

	AUDIO_CODEC_TypeDef *audio_base = AUDIO_CODEC_GetAddr();

	if (newstate == MUTE) {
		audio_base->ADC_CH_CTRL[adc_sel].CODEC_ADC_x_CONTROL_0 |= AUDIO_CODEC_BIT_ADC_x_AD_MUTE;
	} else {
		audio_base->ADC_CH_CTRL[adc_sel].CODEC_ADC_x_CONTROL_0 &= ~AUDIO_CODEC_BIT_ADC_x_AD_MUTE;
	}
}


/**
  * @brief  Set per adc mix mute type and state.
  * @param  adc_sel: select adc input path channel
  *			 This parameter can be one of the following values:
  *			   @arg ADC0
  *			   @arg ADC1
  *			   @arg ADC2
  * @param  type: adc mix mute path select
  *			 This parameter can be one of the following values:
  *			   @arg DMIC_MIX_MUTE
  *			   @arg AD_MIX_MUTE
  * @param  newstate: mute or unmute option for ad channel
  *			 This parameter can be one of the following values:
  *			   @arg UNMUTE
  *			   @arg MUTE
  * @return  None
  */
void AUDIO_CODEC_SetADCMixMute(u32 adc_sel, u32 mix_mute_type, u32 newstate)
{
	assert_param(IS_CODEC_ADC_SEL(adc_sel));
	assert_param(IS_CODEC_ADC_MIXMUTE_SEL(mix_mute_type));
	assert_param(IS_CODEC_MUTE_SEL(newstate));

	AUDIO_CODEC_TypeDef *audio_base = AUDIO_CODEC_GetAddr();

	if (newstate == UNMUTE) {
		if (mix_mute_type == DMIC_MIX_MUTE) {
			audio_base->ADC_CH_CTRL[adc_sel].CODEC_ADC_x_CONTROL_0 &= ~AUDIO_CODEC_BIT_ADC_x_DMIC_MIX_MUTE;
		} else {
			audio_base->ADC_CH_CTRL[adc_sel].CODEC_ADC_x_CONTROL_0 &= ~AUDIO_CODEC_BIT_ADC_x_AD_MIX_MUTE;
		}
	} else {
		if (mix_mute_type == DMIC_MIX_MUTE) {
			audio_base->ADC_CH_CTRL[adc_sel].CODEC_ADC_x_CONTROL_0 |= AUDIO_CODEC_BIT_ADC_x_DMIC_MIX_MUTE;
		} else {
			audio_base->ADC_CH_CTRL[adc_sel].CODEC_ADC_x_CONTROL_0 |= AUDIO_CODEC_BIT_ADC_x_AD_MIX_MUTE;
		}
	}
}

/**
  * @brief  Select analog ad source for per ad channel .
  * @param  ad_chn: select per analog adc channel src.
  *			 This parameter can be one of the following values:
  *			   @arg ADCHN0
  *			   @arg ADCHN1
  *			   @arg ADCHN2
  * @param  amic_sel: select per analog adc channel src.
  *			 This parameter can be one of the following values:
  *			   @arg AMIC1
  * @return  None
  */
void AUDIO_CODEC_SetADCANASrc(u32 ad_chn)
{
	assert_param(IS_CODEC_ADCHN_SEL(ad_chn));

	AUDIO_CODEC_TypeDef *audio_base = AUDIO_CODEC_GetAddr();

	audio_base->ADC_CH_CTRL[ad_chn].CODEC_ADC_x_CONTROL_0 &= ~AUDIO_CODEC_MASK_ADC_x_AD_SRC_SEL;
	audio_base->ADC_CH_CTRL[ad_chn].CODEC_ADC_x_CONTROL_0 |= AUDIO_CODEC_ADC_x_AD_SRC_SEL(AMIC1 - 1);
}


/**
  * @brief  Enable or disable per adc analog ad filter.
  * @param  adc_sel: select ad ana channel
  *			 This parameter can be one of the following values:
  *			   @arg ADC0
  *			   @arg ADC1
  *			   @arg ADC2
  * @param  newstate: enable or disable per ad ana channel.
  *			 This parameter can be one of the following values:
  *			   @arg ENABLE
  *			   @arg DISABLE
  * @return  None
  */
void AUDIO_CODEC_SetADCANAFilter(u32 adc_sel, u32 newstate)
{
	assert_param(IS_CODEC_ADC_SEL(adc_sel));

	AUDIO_CODEC_TypeDef *audio_base = AUDIO_CODEC_GetAddr();

	switch (adc_sel) {

	case ADC0:
		if (newstate == ENABLE) {
			audio_base->CLOCK_CONTROL_2 |= AUDIO_CODEC_BIT_AD_ANA_0_EN;
		} else {
			audio_base->CLOCK_CONTROL_2 &= ~AUDIO_CODEC_BIT_AD_ANA_0_EN;
		}
		break;

	case ADC1:
		if (newstate == ENABLE) {
			audio_base->CLOCK_CONTROL_2 |= AUDIO_CODEC_BIT_AD_ANA_1_EN;
		} else {
			audio_base->CLOCK_CONTROL_2 &= ~AUDIO_CODEC_BIT_AD_ANA_1_EN;
		}
		break;

	case ADC2:
		if (newstate == ENABLE) {
			audio_base->CLOCK_CONTROL_2 |= AUDIO_CODEC_BIT_AD_ANA_2_EN;
		} else {
			audio_base->CLOCK_CONTROL_2 &= ~AUDIO_CODEC_BIT_AD_ANA_2_EN;
		}
		break;

	default:
		break;

	}
}



/**
  * @brief  Enable per dimc channel filter.
  * @param  adc_sel: select ad ana channel
  *			 This parameter can be one of the following values:
  *			   @arg ADC0
  *			   @arg ADC1
  *			   @arg ADC2
  * @param  newstate: enable or disable per ad ana channel.
  *			 This parameter can be one of the following values:
  *			   @arg ENABLE
  *			   @arg DISABLE
  * @return  None
  */
void AUDIO_CODEC_SetADCDmicFilter(u32 adc_sel, u32 newstate)
{
	assert_param(IS_CODEC_ADC_SEL(adc_sel));

	AUDIO_CODEC_TypeDef *audio_base = AUDIO_CODEC_GetAddr();

	switch (adc_sel) {

	case ADC0:
		if (newstate == ENABLE) {
			audio_base->CLOCK_CONTROL_2 |= AUDIO_CODEC_BIT_DMIC_0_EN;
		} else {
			audio_base->CLOCK_CONTROL_2 &= ~AUDIO_CODEC_BIT_DMIC_0_EN;
		}
		break;

	case ADC1:
		if (newstate == ENABLE) {
			audio_base->CLOCK_CONTROL_2 |= AUDIO_CODEC_BIT_DMIC_1_EN;
		} else {
			audio_base->CLOCK_CONTROL_2 &= ~AUDIO_CODEC_BIT_DMIC_1_EN;
		}
		break;

	case ADC2:
		if (newstate == ENABLE) {
			audio_base->CLOCK_CONTROL_2 |= AUDIO_CODEC_BIT_DMIC_2_EN;
		} else {
			audio_base->CLOCK_CONTROL_2 &= ~AUDIO_CODEC_BIT_DMIC_2_EN;
		}
		break;

	default:
		break;
	}
}

/**
  * @brief  Enable and select DMIC clk.
  * @param  clk: dmic clk select
  *          This parameter can be one of the following values:
  *            @arg DMIC_5M
  *            @arg DMIC_2P5M
  *            @arg DMIC_1P25M
  *            @arg DMIC_625K
  *            @arg DMIC_312P5K
  *            @arg DMIC_769P2K
  * @param  newstate: dmic clock enable or disable
  *			 This parameter can be one of the following values:
  *			   @arg DISABLE
  *			   @arg ENABLE
  * @return  None
  */
void AUDIO_CODEC_SetDmicClk(u32 clk, u32 newstate)
{
	assert_param(IS_CODEC_DMIC_CLOCK_SEL(clk));

	AUDIO_CODEC_TypeDef *audio_base = AUDIO_CODEC_GetAddr();

	if (newstate == ENABLE) {
		audio_base->CLOCK_CONTROL_3 &= ~AUDIO_CODEC_MASK_DMIC1_CLK_SEL;
		audio_base->CLOCK_CONTROL_3 |= AUDIO_CODEC_BIT_DMIC1_CLK_EN | AUDIO_CODEC_DMIC1_CLK_SEL(clk);
	} else {
		audio_base->CLOCK_CONTROL_3 &= ~AUDIO_CODEC_BIT_DMIC1_CLK_EN;
	}
}

/**
  * @brief  Set dmic channel source.
  * @param  ad_chn: select adc input path channel
  *			 This parameter can be one of the following values:
  *			   @arg ADCHN0
  *			   @arg ADCHN1
  *			   @arg ADCHN2
  * @param  dmic_sel: mute or unmute option for per DMIC mix.
  *			 This parameter can be one of the following values:
  *			   @arg DMIC1
  *			   @arg DMIC2		TODO mic 1 rising, mic 1 falling
  * @return  None
  */
void AUDIO_CODEC_SetDmicSrc(u32 ad_chn, u32 dmic_sel)
{
	assert_param(IS_CODEC_ADCHN_SEL(ad_chn));
	assert_param(IS_CODEC_DMIC_SEL(dmic_sel));

	AUDIO_CODEC_TypeDef *audio_base = AUDIO_CODEC_GetAddr();

	audio_base->ADC_CH_CTRL[ad_chn].CODEC_ADC_x_CONTROL_0 &= ~AUDIO_CODEC_MASK_ADC_x_DMIC_SRC_SEL;
	audio_base->ADC_CH_CTRL[ad_chn].CODEC_ADC_x_CONTROL_0 |= AUDIO_CODEC_ADC_x_DMIC_SRC_SEL(dmic_sel - 1);
}

/**
  * @brief  Set DAC source.
  * @param  dac_l_src: dac l source.
  *			 This parameter can be one of the following values:
  *			   @arg I2SL
  *			   @arg TESTTONE
  */
void AUDIO_CODEC_SetDACSrc(u32 dac_l_src)
{
	assert_param(IS_CODEC_DAC_SRC_SEL(dac_l_src));

	AUDIO_CODEC_TypeDef *audio_base = AUDIO_CODEC_GetAddr();

	audio_base->DAC_L_CONTROL_0 &= ~AUDIO_CODEC_BIT_DAC_L_DA_SRC_SEL;

	if (dac_l_src == TESTTONE) {
		audio_base->DAC_L_CONTROL_0 |= AUDIO_CODEC_BIT_DAC_L_DA_SRC_SEL;
	}


}

/**
  * @brief  Enable or disable da fifo channel clock.
  * @param  newstate: enable or disable da channel.
  *			 This parameter can be one of the following values:
  *			   @arg ENABLE
  *			   @arg DISABLE
  * @return  None
  */
void AUDIO_CODEC_EnableDACFifo(u32 newstate)
{
	AUDIO_CODEC_TypeDef *audio_base = AUDIO_CODEC_GetAddr();

	if (newstate == ENABLE) {
		audio_base->CLOCK_CONTROL_1 |= AUDIO_CODEC_BIT_DA_FIFO_EN;
	} else {
		audio_base->CLOCK_CONTROL_1 &= ~AUDIO_CODEC_BIT_DA_FIFO_EN;
	}
}

/**
  * @brief  Enable or disable da and da channel clock.
  * @param  channel: select dac channel.
  *			 This parameter can be one of the following values:
  *			   @arg DAC_L
  * @param  newstate: enable or disable da channel.
  *			 This parameter can be one of the following values:
  *			   @arg ENABLE
  *			   @arg DISABLE
  * @return  None
  */
void AUDIO_CODEC_EnableDAC(u32 channel, u32 newstate)
{
	assert_param(IS_CODEC_DAC_SEL(channel));

	AUDIO_CODEC_TypeDef *audio_base = AUDIO_CODEC_GetAddr();

	if (newstate == ENABLE) {
		audio_base->CLOCK_CONTROL_1 |= AUDIO_CODEC_BIT_DA_L_EN | AUDIO_CODEC_BIT_MOD_L_EN;
	} else {
		audio_base->CLOCK_CONTROL_1 &= ~(AUDIO_CODEC_BIT_DA_L_EN | AUDIO_CODEC_BIT_MOD_L_EN);
	}
}

/**
  * @brief  Mute DAC path.
  * @param  channel: select dac channel.
  *			 This parameter can be one of the following values:
  *			   @arg DAC_L
  * @param  newstate: mute or unmute option for da channel
  *			 This parameter can be one of the following values:
  *			   @arg UNMUTE: unmute
  *			   @arg MUTE: mute
  */
void AUDIO_CODEC_SetDACMute(u32 channel, u32 newstate)
{
	assert_param(IS_CODEC_DAC_SEL(channel));
	assert_param(IS_CODEC_MUTE_SEL(newstate));

	AUDIO_CODEC_TypeDef *audio_base = AUDIO_CODEC_GetAddr();

	if (newstate == UNMUTE) {
		audio_base->DAC_L_CONTROL_1 &= ~AUDIO_CODEC_BIT_DAC_L_DA_MUTE;
	} else {
		audio_base->DAC_L_CONTROL_1 |= AUDIO_CODEC_BIT_DAC_L_DA_MUTE;
	}
}

/**
  * @brief  Mute DAC mix path DA.
  * @param  channel: select dac channel.
  *			 This parameter can be one of the following values:
  *			   @arg DAC_L
  * @param  newstate: mute or unmute option for da channel
  *			 This parameter can be one of the following values:
  *			   @arg UNMUTE: unmute
  *			   @arg MUTE: mute
  */
void AUDIO_CODEC_SetDACMixMute(u32 channel, u32 newstate)
{
	assert_param(IS_CODEC_DAC_SEL(channel));
	assert_param(IS_CODEC_MUTE_SEL(newstate));

	AUDIO_CODEC_TypeDef *audio_base = AUDIO_CODEC_GetAddr();

	if (newstate == UNMUTE) {
		audio_base->DAC_L_CONTROL_1 &= ~AUDIO_CODEC_BIT_DAC_L_DMIX_MUTE_DA;
	} else {
		audio_base->DAC_L_CONTROL_1 |= AUDIO_CODEC_BIT_DAC_L_DMIX_MUTE_DA;
	}
}

/**
  * @brief  Mute DAC music path.
  * @param  channel: select dac channel.
  *			 This parameter can be one of the following values:
  *			   @arg DAC_L
  * @param  newstate: mute or unmute option for da channel
  *			 This parameter can be one of the following values:
  *			   @arg UNMUTE: unmute
  *			   @arg MUTE: mute
  */
void AUDIO_CODEC_SetDACMusicMute(u32 channel, u32 newstate)
{
	assert_param(IS_CODEC_DAC_SEL(channel));
	assert_param(IS_CODEC_MUTE_SEL(newstate));

	AUDIO_CODEC_TypeDef *audio_base = AUDIO_CODEC_GetAddr();

	if (newstate == UNMUTE) {
		audio_base->DAC_L_CONTROL_1 &= ~AUDIO_CODEC_BIT_DAC_L_MUSIC_MUTE_EN;
	} else {
		audio_base->DAC_L_CONTROL_1 |= AUDIO_CODEC_BIT_DAC_L_MUSIC_MUTE_EN;
	}
}

/**
  * @brief  Set the gain of DAC digital volume.
  * @param  channel: the value of dac path.
  *          This parameter can be one of the following values:
  *            @arg 0: DAC_L
  * @param  Gain: the value of gain.
  *          This parameter can be -62.5dB~0dB in 0.375dB step
  *            @arg 0x00 -65.625dB
  *            @arg 0xaf 0dB
  * 		   @arg 0xff 30dB
  * @retval None
  */
void AUDIO_CODEC_SetDACVolume(u32 channel, u32 gain)
{
	assert_param(IS_CODEC_DAC_SEL(channel));

	AUDIO_CODEC_TypeDef *audio_base = AUDIO_CODEC_GetAddr();

	audio_base->DAC_L_CONTROL_0 &= ~ AUDIO_CODEC_MASK_DAC_L_DA_GAIN;
	audio_base->DAC_L_CONTROL_0 |= AUDIO_CODEC_DAC_L_DA_GAIN(gain);
}

/**
  * @brief  Set DAC HPF enable.
  * @param  channel: the value of dac path.
  *          This parameter can be one of the following values:
  *            @arg 0: DAC_L
  * @param  newstate: enable or disable per ad channel HPF mode
  *			 This parameter can be one of the following values:
  *			   @arg ENABLE
  *			   @arg DISABLE
  * @return  None
  */
void AUDIO_CODEC_SetDACHPF(u32 channel, u32 newstate)
{
	assert_param(IS_CODEC_DAC_SEL(channel));

	AUDIO_CODEC_TypeDef *audio_base = AUDIO_CODEC_GetAddr();

	if (newstate == ENABLE) {
		audio_base->DAC_L_CONTROL_1 |= AUDIO_CODEC_BIT_DAC_L_DAHPF_EN;
	} else {
		audio_base->DAC_L_CONTROL_1 &= ~AUDIO_CODEC_BIT_DAC_L_DAHPF_EN;
	}

}


/**
  * @brief  Enable or disable DAC ASRC mode and select asrc rate.
  * @param  sr: sample rate
  * @param  newstate: enable or disable per ad channel HPF mode
  *			 This parameter can be one of the following values:
  *			   @arg ENABLE
  *			   @arg DISABLE
  * @retval None
  */
void AUDIO_CODEC_SetDACASRC(u32 sr, u32 newstate)
{
	assert_param(IS_CODEC_SR_SEL(sr));

	AUDIO_CODEC_TypeDef *audio_base = AUDIO_CODEC_GetAddr();

	if (newstate == ENABLE) {

		audio_base->CLOCK_CONTROL_6 |= AUDIO_CODEC_BIT_DAC_L_ASRC_EN;
		audio_base->ASRC_CONTROL_0 &= ~AUDIO_CODEC_MASK_ASRC_RATE_SEL_TX;

		if (sr == SR_192K || sr == SR_176P4K) {
			audio_base->ASRC_CONTROL_0 |= AUDIO_CODEC_ASRC_RATE_SEL_TX(2);
		} else if (sr == SR_96K || sr == SR_88P2K) {
			audio_base->ASRC_CONTROL_0 |= AUDIO_CODEC_ASRC_RATE_SEL_TX(1);
		} else {
			audio_base->ASRC_CONTROL_0 |= AUDIO_CODEC_ASRC_RATE_SEL_TX(0);
		}
	} else {
		audio_base->CLOCK_CONTROL_6 &= ~AUDIO_CODEC_BIT_DAC_L_ASRC_EN;
	}
}

/**
  * @brief  Enable and select or disalbe PDM clk.
  * @param  channel: the value of dac path.
  *          This parameter can be one of the following values:
  *            @arg 0: DAC_L
  * @param  clk: PDM clk select
  *          This parameter can be one of the following values:
  *            @arg PDM_5M
  *            @arg PDM_2P5M
  *            @arg PDM_6P67M
  * @param  newstate: pdm enable or disable
  *			 This parameter can be one of the following values:
  *			   @arg DISABLE
  *			   @arg ENABLE
  * @return  None
  */
void AUDIO_CODEC_SetPDMClk(u32 channel, u32 clk, u32 newstate)
{
	assert_param(IS_CODEC_DAC_SEL(channel));
	assert_param(IS_CODEC_PDM_CLOCK_SEL(clk));

	AUDIO_CODEC_TypeDef *audio_base = AUDIO_CODEC_GetAddr();

	audio_base->AUDIO_CONTROL_1 &= ~AUDIO_CODEC_MASK_PDM_CLK_SEL;
	audio_base->AUDIO_CONTROL_1 |= AUDIO_CODEC_PDM_CLK_SEL(clk);

	if (newstate == ENABLE) {
		audio_base->DAC_L_CONTROL_1 |= AUDIO_CODEC_BIT_DAC_L_PDM_EN;
	} else {
		audio_base->DAC_L_CONTROL_1 &= ~AUDIO_CODEC_BIT_DAC_L_PDM_EN;
	}


}

/**
  * @brief  DAC Testtone mode.
  * @param  fc: the value of test frequency.
  *          This parameter can be one of the following values:
  *          @arg fc
  *          frequency= (fs/192)*(fc+1)Hz
  * @param  newstate: the status of testtone mode.
  *          This parameter can be the following values:
  *            @arg ENABLE
  *            @arg DISABLE
  * @retval None
  */

void AUDIO_CODEC_SetTesttone(u32 fc, u32 newstate)
{
	AUDIO_CODEC_TypeDef *audio_base = AUDIO_CODEC_GetAddr();

	if (newstate == ENABLE) {
		audio_base->DAC_L_CONTROL_0 &= ~AUDIO_CODEC_MASK_DAC_L_TEST_FC_SEL;
		audio_base->DAC_L_CONTROL_0 |= AUDIO_CODEC_BIT_DAC_L_TEST_TONE_EN | (AUDIO_CODEC_DAC_L_TEST_FC_SEL(fc));
	} else {
		audio_base->DAC_L_CONTROL_0 &= ~AUDIO_CODEC_BIT_DAC_L_TEST_TONE_EN;
	}
}

/**
  * @brief  Set i2s loopback mode.
  * @param  newstate: Enable or disable loopback mode
  *          This parameter can be the following values:
  *            @arg ENABLE
  *            @arg DISABLE
  * @retval None
  */
void AUDIO_CODEC_SetI2SLPBK(u32 newstate)
{
	AUDIO_CODEC_TypeDef *audio_base = AUDIO_CODEC_GetAddr();

	if (newstate == ENABLE) {
		audio_base->I2S_0_CONTROL |= AUDIO_CODEC_BIT_I2S_0_SELF_LPBK_EN;
	} else {
		audio_base->I2S_0_CONTROL &= ~AUDIO_CODEC_BIT_I2S_0_SELF_LPBK_EN;
	}
}


/**
  * @brief  Set daad loopback mode.
  * @param  newstate: the status of testtone mode.
  *          This parameter can be the following values:
  *            @arg ENABLE
  *            @arg DISABLE
  * @retval None
  */
void AUDIO_CODEC_SetDAADLPBK(u32 newstate)
{
	AUDIO_CODEC_TypeDef *audio_base = AUDIO_CODEC_GetAddr();

	if (newstate == ENABLE) {
		audio_base->AUDIO_CONTROL_0 |= AUDIO_CODEC_BIT_DAAD_LPBK_EN;
	} else {
		audio_base->AUDIO_CONTROL_0 &= ~AUDIO_CODEC_BIT_DAAD_LPBK_EN;
	}
}

/**
  * @brief  Set adda loopback mode.
  * @param  newstate: the status of testtone mode.
  *          This parameter can be the following values:
  *            @arg ENABLE
  *            @arg DISABLE
  * @retval None
  */
void AUDIO_CODEC_SetADDALPBK(u32 newstate)
{
	AUDIO_CODEC_TypeDef *audio_base = AUDIO_CODEC_GetAddr();

	if (newstate == ENABLE) {
		audio_base->AUDIO_CONTROL_0 |= AUDIO_CODEC_BIT_ADDA_LPBK_EN;
	} else {
		audio_base->AUDIO_CONTROL_0 &= ~AUDIO_CODEC_BIT_ADDA_LPBK_EN;
	}
}

/**
  * @brief  Select ADC path decimation source
  * @param  adc_sel: select adc channel
  *			 This parameter can be one of the following values:
  *			   @arg ADC0
  *			   @arg ADC1
  *			   @arg ADC2
  * @param  type: select decimation source
  *			 This parameter can be one of the following values:
  *			   @arg AUDIO_ADC
  *			   @arg MUSIC
  * @return  None
  */
void AUDIO_CODEC_SetADCDecimationSrc(u32 adc_sel, u32 type)
{
	assert_param(IS_CODEC_ADC_SEL(adc_sel));
	assert_param(IS_CODEC_ADC_DCM_SEL(type));

	AUDIO_CODEC_TypeDef *audio_base = AUDIO_CODEC_GetAddr();

	audio_base->ADC_CH_CTRL[adc_sel].CODEC_ADC_x_CONTROL_0 &= ~(AUDIO_CODEC_MASK_ADC_x_DECI_SRC_SEL);
	audio_base->ADC_CH_CTRL[adc_sel].CODEC_ADC_x_CONTROL_0 |=	AUDIO_CODEC_ADC_x_DECI_SRC_SEL(type);

}


/**
  * @brief  Set DAC path zero detection function.
  * @param  channel: select dac channel.
  *			 This parameter can be one of the following values:
  *			   @arg DACL
  * @param  type: zero detection function select
  *			 This parameter can be one of the following values:
  *			   @arg IME: immediate change
  *			   @arg ZDET_IME: zero detection & immediate change
  *			   @arg ZDET_STEP: zdet & step
  *			   @arg ZDET_TIMEOUT: zdet & timeout step
  */
void AUDIO_CODEC_SetDACZDET(u32 channel, u32 type)
{
	assert_param(IS_CODEC_DAC_SEL(channel));
	assert_param(IS_CODEC_ZDET_SEL(type));

	AUDIO_CODEC_TypeDef *audio_base = AUDIO_CODEC_GetAddr();

	audio_base->DAC_L_CONTROL_1 &= ~AUDIO_CODEC_MASK_DAC_L_DA_ZDET_FUNC;
	audio_base->DAC_L_CONTROL_1 |= AUDIO_CODEC_DAC_L_DA_ZDET_FUNC(type);
}

/**
  * @brief  Set DAC path zero detection time out select.
  * @param  channel: select dac channel.
  *			 This parameter can be one of the following values:
  *			   @arg DACL
  * @param  time_out: zero detection time out select select
  *			 This parameter can be one of the following values:
  *			   @arg 0: 1024*16 samples
  *			   @arg 1: 1024*32 samples
  *			   @arg 2: 1024*64 samples
  *			   @arg 3: 64 samples
  */
void AUDIO_CODEC_SetDACZDETTimeOut(u32 channel, u32 time_out)
{
	assert_param(IS_CODEC_DAC_SEL(channel));

	AUDIO_CODEC_TypeDef *audio_base = AUDIO_CODEC_GetAddr();

	audio_base->DAC_L_CONTROL_1 &= ~AUDIO_CODEC_MASK_DAC_L_DA_ZDET_TOUT;
	audio_base->DAC_L_CONTROL_1 |= AUDIO_CODEC_DAC_L_DA_ZDET_TOUT(time_out);

}


/**
  * @brief  Set ADC path zero detection function.
  * @param  channel: select dac channel.
  *			 This parameter can be one of the following values:
  *			   @arg ADC0
  *			   @arg ADC1
  *			   @arg ADC2
  * @param  type: zero detection function select
  *			 This parameter can be one of the following values:
  *			   @arg IME: immediate change
  *			   @arg ZDET_IME: zero detection & immediate change
  *			   @arg ZDET_STEP: zdet & step
  *			   @arg ZDET_TIMEOUT: zdet & timeout step
  */
void AUDIO_CODEC_SetADCZDET(u32 adc_sel, u32 type)
{
	assert_param(IS_CODEC_ADC_SEL(adc_sel));
	assert_param(IS_CODEC_ZDET_SEL(type));

	AUDIO_CODEC_TypeDef *audio_base = AUDIO_CODEC_GetAddr();

	audio_base->ADC_CH_CTRL[adc_sel].CODEC_ADC_x_CONTROL_0 &= ~AUDIO_CODEC_MASK_ADC_x_AD_ZDET_FUNC;
	audio_base->ADC_CH_CTRL[adc_sel].CODEC_ADC_x_CONTROL_0 |= AUDIO_CODEC_ADC_x_AD_ZDET_FUNC(type);
}


/**
  * @brief	Set ADC path zero detection function.
  * @param	channel: select dac channel.
  * 		 This parameter can be one of the following values:
  * 		   @arg ADC0
  * 		   @arg ADC1
  * 		   @arg ADC2
  * @param	time_out: zero detection time out select select
  * 		 This parameter can be one of the following values:
  * 		   @arg 0: 1024*16 samples
  * 		   @arg 1: 1024*32 samples
  * 		   @arg 2: 1024*64 samples
  * 		   @arg 3: 64 samples
  */
void AUDIO_CODEC_SetADCZDETTimeOut(u32 adc_sel, u32 time_out)
{
	assert_param(IS_CODEC_ADC_SEL(adc_sel));

	AUDIO_CODEC_TypeDef *audio_base = AUDIO_CODEC_GetAddr();

	audio_base->ADC_CH_CTRL[adc_sel].CODEC_ADC_x_CONTROL_0 &= ~AUDIO_CODEC_MASK_ADC_x_AD_ZDET_TOUT;
	audio_base->ADC_CH_CTRL[adc_sel].CODEC_ADC_x_CONTROL_0 |= AUDIO_CODEC_ADC_x_AD_ZDET_TOUT(time_out);
}

/**
  * @brief	Enable or disable ADC path silence detection.
  * @param	adc_sel: select adc channel.
  * 		 This parameter can be one of the following values:
  * 		   @arg ADC0
  * 		   @arg ADC1
  * 		   @arg ADC2
  * @param	newstate: status of silence detection mode
  * 		 This parameter can be one of the following values:
  * 		   @arg ENABLE
  * 		   @arg DISABLE
  */
void AUDIO_CODEC_SetADCSilenceDet(u32 adc_sel, u32 newstate)
{
	assert_param(IS_CODEC_ADC_SEL(adc_sel));

	AUDIO_CODEC_TypeDef *audio_base = AUDIO_CODEC_GetAddr();

	if (newstate == ENABLE) {
		audio_base->ADC_x_SILENCE_CONTROL[adc_sel] |= AUDIO_CODEC_BIT_ADC_x_SILENCE_DET_EN;
	} else {
		audio_base->ADC_x_SILENCE_CONTROL[adc_sel] &= ~AUDIO_CODEC_BIT_ADC_x_SILENCE_DET_EN;
	}

}

/**
  * @brief	Select ADC path silence detection level.
  * @param	adc_sel: select adc channel.
  * 		 This parameter can be one of the following values:
  * 		   @arg ADC0
  * 		   @arg ADC1
  * 		   @arg ADC2
  * @param	level_det: silence detection level threshold
  * 		 This parameter can be one of the following values:
  * 		   @arg 3'b000: -54db
  * 		   @arg 3'b001: -60db
  * 		   @arg 3'b010: -66db
  * 		   @arg 3'b011: -72db
  * 		   @arg 3'b100: -78db
  * 		   @arg 3'b101: -84db
  * 		   @arg 3'b110: -90db
  * 		   @arg 3'b111: -96db
  */
void AUDIO_CODEC_SelADCSilenceLevel(u32 adc_sel, u32 level_det)
{
	assert_param(IS_CODEC_ADC_SEL(adc_sel));

	AUDIO_CODEC_TypeDef *audio_base = AUDIO_CODEC_GetAddr();

	audio_base->ADC_x_SILENCE_CONTROL[adc_sel] &= ~AUDIO_CODEC_MASK_ADC_x_SILENCE_DEBOUNCE_SEL;
	audio_base->ADC_x_SILENCE_CONTROL[adc_sel] |= AUDIO_CODEC_ADC_x_SILENCE_LEVEL_SEL(level_det);

}

/**
  * @brief	Select ADC path silence detection debounce duration.
  * @param	adc_sel: select adc channel.
  * 		 This parameter can be one of the following values:
  * 		   @arg ADC0
  * 		   @arg ADC1
  * 		   @arg ADC2
  * @param	debounce_sel: debounce duration
  * 		 This parameter can be one of the following values:
  * 		   @arg 3'b000: 80ms
  * 		   @arg 3'b001: 160ms
  * 		   @arg 3'b010: 320ms
  * 		   @arg 3'b011: 640ms
  * 		   @arg 3'b100: 1.28s
  * 		   @arg 3'b101: 2.56s
  * 		   @arg 3'b110: 5.12s
  * 		   @arg 3'b111: 0.16ms
  */
void AUDIO_CODEC_SelADCSilenceDebounce(u32 adc_sel, u32 debounce_sel)
{
	assert_param(IS_CODEC_ADC_SEL(adc_sel));

	AUDIO_CODEC_TypeDef *audio_base = AUDIO_CODEC_GetAddr();

	audio_base->ADC_x_SILENCE_CONTROL[adc_sel] &= ~AUDIO_CODEC_MASK_ADC_x_SILENCE_DEBOUNCE_SEL;
	audio_base->ADC_x_SILENCE_CONTROL[adc_sel] |= AUDIO_CODEC_ADC_x_SILENCE_DEBOUNCE_SEL(debounce_sel);

}

/**
  * @brief	Enable or disable DAC path silence detection.
  * @param	channel: select DAC channel.
  * 		 This parameter can be one of the following values:
  * 		   @arg DAC_L
  * @param	newstate: status of silence detection mode
  * 		 This parameter can be one of the following values:
  * 		   @arg ENABLE
  * 		   @arg DISABLE
  */
void AUDIO_CODEC_SetDACSilenceDet(u32 channel, u32 newstate)
{
	assert_param(IS_CODEC_DAC_SEL(channel));

	AUDIO_CODEC_TypeDef *audio_base = AUDIO_CODEC_GetAddr();

	if (newstate == ENABLE) {
		audio_base->DAC_L_SILENCE_CONTROL |= AUDIO_CODEC_BIT_DAC_L_SILENCE_DET_EN;
	} else {
		audio_base->DAC_L_SILENCE_CONTROL &= ~AUDIO_CODEC_BIT_DAC_L_SILENCE_DET_EN;
	}

}

/**
  * @brief	Select DAC path silence detection level.
  * @param	channel: select adc channel.
  * 		 This parameter can be one of the following values:
  * 		   @arg DAC_L
  * @param	level_det: silence detection level threshold
  * 		 This parameter can be one of the following values:
  * 		   @arg 3'b000: -78db
  * 		   @arg 3'b001: -84db
  * 		   @arg 3'b010: -90db
  * 		   @arg 3'b011: -102db
  * 		   @arg 3'b100: -108db
  * 		   @arg 3'b101: -114db
  * 		   @arg 3'b110: -126db
  * 		   @arg 3'b111: -132db
  */
void AUDIO_CODEC_SelDACSilenceLevel(u32 channel, u32 level_det)
{
	assert_param(IS_CODEC_DAC_SEL(channel));

	AUDIO_CODEC_TypeDef *audio_base = AUDIO_CODEC_GetAddr();

	audio_base->DAC_L_SILENCE_CONTROL &= ~AUDIO_CODEC_MASK_DAC_L_SILENCE_DEBOUNCE_SEL;
	audio_base->DAC_L_SILENCE_CONTROL |= AUDIO_CODEC_DAC_L_SILENCE_LEVEL_SEL(level_det);

}

/**
  * @brief	Select DAC path silence detection debounce duration.
  * @param	channel: select dac channel.
  * 		 This parameter can be one of the following values:
  * 		   @arg DAC_L
  * @param	debounce_sel: debounce duration
  * 		 This parameter can be one of the following values:
  * 		   @arg 3'b000: 80ms
  * 		   @arg 3'b001: 160ms
  * 		   @arg 3'b010: 320ms
  * 		   @arg 3'b011: 640ms
  * 		   @arg 3'b100: 1.28s
  * 		   @arg 3'b101: 2.56s
  * 		   @arg 3'b110: 5.12s
  * 		   @arg 3'b111: 0.16ms
  */
void AUDIO_CODEC_SelDACSilenceDebounce(u32 channel, u32 debounce_sel)
{
	assert_param(IS_CODEC_DAC_SEL(channel));

	AUDIO_CODEC_TypeDef *audio_base = AUDIO_CODEC_GetAddr();

	audio_base->DAC_L_SILENCE_CONTROL &= ~AUDIO_CODEC_MASK_DAC_L_SILENCE_DEBOUNCE_SEL;
	audio_base->DAC_L_SILENCE_CONTROL |= AUDIO_CODEC_DAC_L_SILENCE_DEBOUNCE_SEL(debounce_sel);

}

/**
  * @brief  Enable or disable adc channel EQ clk.
  * @param  ad_chn: select adc channel
  *          This parameter can be one of the following values:
  *            @arg ADCHN0
  *            @arg ADCHN1
  *            @arg ADCHN2
  * @param  newstate: Enable or disable adc channel EQ clk
  *          This parameter can be one of the following values:
  *            @arg ENABLE
  *            @arg DISABLE
  * @return  None
  */
void AUDIO_CODEC_SetADCEQClk(u32 ad_chn, u32 newstate)
{
	assert_param(IS_CODEC_ADCHN_SEL(ad_chn));

	AUDIO_CODEC_TypeDef *audio_base = AUDIO_CODEC_GetAddr();

	switch (ad_chn) {
	case ADCHN0:
		if (newstate == ENABLE) {
			audio_base->CLOCK_CONTROL_2 |= AUDIO_CODEC_BIT_AD_0_EQ_EN;
		} else {
			audio_base->CLOCK_CONTROL_2 &= ~AUDIO_CODEC_BIT_AD_0_EQ_EN;
		}
		break;

	case ADCHN1:
		if (newstate == ENABLE) {
			audio_base->CLOCK_CONTROL_2 |= AUDIO_CODEC_BIT_AD_1_EQ_EN;
		} else {
			audio_base->CLOCK_CONTROL_2 &= ~AUDIO_CODEC_BIT_AD_1_EQ_EN;
		}
		break;

	case ADCHN2:
		if (newstate == ENABLE) {
			audio_base->CLOCK_CONTROL_2 |= AUDIO_CODEC_BIT_AD_2_EQ_EN;
		} else {
			audio_base->CLOCK_CONTROL_2 &= ~AUDIO_CODEC_BIT_AD_2_EQ_EN;
		}
		break;

	default:
		break;
	}
}

/**
  * @brief  Enable or disable adc channel EQ band.
  * @param  adc_sel: select adc channel
  *          This parameter can be one of the following values:
  *            @arg ADCHN0
  *            @arg ADCHN1
  *            @arg ADCHN2
  * @param  band_sel: select EQ band of each adc channel
  *          This parameter can be one of the following values:
  *            @arg ADCEQBD0
  *            @arg ADCEQBD1
  *            @arg ADCEQBD2
  *            @arg ADCEQBD3
  *            @arg ADCEQBD4
  * @param  newstate: Enable or disable adc channel EQ band
  *          This parameter can be one of the following values:
  *            @arg ENABLE
  *            @arg DISABLE
  * @return  None
  */
void AUDIO_CODEC_SetADCEQBand(u32 ad_chn, u32 band_sel, u32 newstate)
{
	assert_param(IS_CODEC_ADCHN_SEL(ad_chn));
	assert_param(IS_CODEC_ADC_EQ_Band_SEL(band_sel));

	AUDIO_CODEC_TypeDef *audio_base = AUDIO_CODEC_GetAddr();

	switch (ad_chn) {
	case ADCHN0:
		if (band_sel == ADCEQBD0) {
			if (newstate == ENABLE) {
				audio_base->ADC_0_EQ_CTRL |= AUDIO_CODEC_BIT_ADC_0_BIQUAD_EN_0;
			} else {
				audio_base->ADC_0_EQ_CTRL &= ~AUDIO_CODEC_BIT_ADC_0_BIQUAD_EN_0;
			}
		} else if (band_sel == ADCEQBD1) {
			if (newstate == ENABLE) {
				audio_base->ADC_0_EQ_CTRL |= AUDIO_CODEC_BIT_ADC_0_BIQUAD_EN_1;
			} else {
				audio_base->ADC_0_EQ_CTRL &= ~AUDIO_CODEC_BIT_ADC_0_BIQUAD_EN_1;
			}
		} else if (band_sel == ADCEQBD2) {
			if (newstate == ENABLE) {
				audio_base->ADC_0_EQ_CTRL |= AUDIO_CODEC_BIT_ADC_0_BIQUAD_EN_2;
			} else {
				audio_base->ADC_0_EQ_CTRL &= ~AUDIO_CODEC_BIT_ADC_0_BIQUAD_EN_2;
			}
		} else if (band_sel == ADCEQBD3) {
			if (newstate == ENABLE) {
				audio_base->ADC_0_EQ_CTRL |= AUDIO_CODEC_BIT_ADC_0_BIQUAD_EN_3;
			} else {
				audio_base->ADC_0_EQ_CTRL &= ~AUDIO_CODEC_BIT_ADC_0_BIQUAD_EN_3;
			}
		} else if (band_sel == ADCEQBD4) {
			if (newstate == ENABLE) {
				audio_base->ADC_0_EQ_CTRL |= AUDIO_CODEC_BIT_ADC_0_BIQUAD_EN_4;
			} else {
				audio_base->ADC_0_EQ_CTRL &= ~AUDIO_CODEC_BIT_ADC_0_BIQUAD_EN_4;
			}
		} else {
			return;
		}
		break;

	case ADCHN1:
		if (band_sel == ADCEQBD0) {
			if (newstate == ENABLE) {
				audio_base->ADC_1_EQ_CTRL |= AUDIO_CODEC_BIT_ADC_1_BIQUAD_EN_0;
			} else {
				audio_base->ADC_1_EQ_CTRL &= ~AUDIO_CODEC_BIT_ADC_1_BIQUAD_EN_0;
			}
		} else if (band_sel == ADCEQBD1) {
			if (newstate == ENABLE) {
				audio_base->ADC_1_EQ_CTRL |= AUDIO_CODEC_BIT_ADC_1_BIQUAD_EN_1;
			} else {
				audio_base->ADC_1_EQ_CTRL &= ~AUDIO_CODEC_BIT_ADC_1_BIQUAD_EN_1;
			}
		} else if (band_sel == ADCEQBD2) {
			if (newstate == ENABLE) {
				audio_base->ADC_1_EQ_CTRL |= AUDIO_CODEC_BIT_ADC_1_BIQUAD_EN_2;
			} else {
				audio_base->ADC_1_EQ_CTRL &= ~AUDIO_CODEC_BIT_ADC_1_BIQUAD_EN_2;
			}
		} else if (band_sel == ADCEQBD3) {
			if (newstate == ENABLE) {
				audio_base->ADC_1_EQ_CTRL |= AUDIO_CODEC_BIT_ADC_1_BIQUAD_EN_3;
			} else {
				audio_base->ADC_1_EQ_CTRL &= ~AUDIO_CODEC_BIT_ADC_1_BIQUAD_EN_3;
			}
		} else if (band_sel == ADCEQBD4) {
			if (newstate == ENABLE) {
				audio_base->ADC_1_EQ_CTRL |= AUDIO_CODEC_BIT_ADC_1_BIQUAD_EN_4;
			} else {
				audio_base->ADC_1_EQ_CTRL &= ~AUDIO_CODEC_BIT_ADC_1_BIQUAD_EN_4;
			}
		} else {
			return;
		}
		break;


	case ADCHN2:
		if (band_sel == ADCEQBD0) {
			if (newstate == ENABLE) {
				audio_base->ADC_2_EQ_CTRL |= AUDIO_CODEC_BIT_ADC_2_BIQUAD_EN_0;
			} else {
				audio_base->ADC_2_EQ_CTRL &= ~AUDIO_CODEC_BIT_ADC_2_BIQUAD_EN_0;
			}
		} else if (band_sel == ADCEQBD1) {
			if (newstate == ENABLE) {
				audio_base->ADC_2_EQ_CTRL |= AUDIO_CODEC_BIT_ADC_2_BIQUAD_EN_1;
			} else {
				audio_base->ADC_2_EQ_CTRL &= ~AUDIO_CODEC_BIT_ADC_2_BIQUAD_EN_1;
			}
		} else if (band_sel == ADCEQBD2) {
			if (newstate == ENABLE) {
				audio_base->ADC_2_EQ_CTRL |= AUDIO_CODEC_BIT_ADC_2_BIQUAD_EN_2;
			} else {
				audio_base->ADC_2_EQ_CTRL &= ~AUDIO_CODEC_BIT_ADC_2_BIQUAD_EN_2;
			}
		} else if (band_sel == ADCEQBD3) {
			if (newstate == ENABLE) {
				audio_base->ADC_2_EQ_CTRL |= AUDIO_CODEC_BIT_ADC_2_BIQUAD_EN_3;
			} else {
				audio_base->ADC_2_EQ_CTRL &= ~AUDIO_CODEC_BIT_ADC_2_BIQUAD_EN_3;
			}
		} else if (band_sel == ADCEQBD4) {
			if (newstate == ENABLE) {
				audio_base->ADC_2_EQ_CTRL |= AUDIO_CODEC_BIT_ADC_2_BIQUAD_EN_4;
			} else {
				audio_base->ADC_2_EQ_CTRL &= ~AUDIO_CODEC_BIT_ADC_2_BIQUAD_EN_4;
			}
		} else {
			return;
		}
		break;

	default:
		break;
	}
}

/**
  * @brief  set EQ band as filter for ADC path
  * @param  ad_chn: select adc channel
  *          This parameter can be one of the following values:
  *            @arg ADCHN0
  *            @arg ADCHN1
  *            @arg ADCHN2
  * @param  band_sel: select EQ band of each adc channel
  *          This parameter can be one of the following values:
  *            @arg ADCEQBD0
  *            @arg ADCEQBD1
  *            @arg ADCEQBD2
  *            @arg ADCEQBD3
  *            @arg ADCEQBD4
  * @param  EQFilterCoefPoint: it is a struct pointer which points a struct that includes coefficients of filter.
  * @return  None
  */
void AUDIO_CODEC_SetADCEQFilter(u32 ad_chn, u32 band_sel, CODEC_EQFilterCoef *EQFilterCoefPoint)
{
	assert_param(IS_CODEC_ADCHN_SEL(ad_chn));
	assert_param(IS_CODEC_ADC_EQ_Band_SEL(band_sel));

	AUDIO_CODEC_TypeDef *audio_base = AUDIO_CODEC_GetAddr();
	AUDIO_CODEC_EQ_BAND_TypeDef *eq_base_addr = NULL;

	switch (ad_chn) {
	case ADCHN0:
		eq_base_addr = (AUDIO_CODEC_EQ_BAND_TypeDef *)audio_base->ADC_0_EQ_BAND;
		break;

	case ADCHN1:
		eq_base_addr = (AUDIO_CODEC_EQ_BAND_TypeDef *)audio_base->ADC_1_EQ_BAND;
		break;

	case ADCHN2:
		eq_base_addr = (AUDIO_CODEC_EQ_BAND_TypeDef *)audio_base->ADC_2_EQ_BAND;
		break;

	default:
		break;
	}

	eq_base_addr[band_sel].CODEC_BIQUAD_H0_x  = EQFilterCoefPoint->H0_Q;
	eq_base_addr[band_sel].CODEC_BIQUAD_B1_x  = EQFilterCoefPoint->B1_Q;
	eq_base_addr[band_sel].CODEC_BIQUAD_B2_x  = EQFilterCoefPoint->B2_Q;
	eq_base_addr[band_sel].CODEC_BIQUAD_A1_x  = EQFilterCoefPoint->A1_Q;
	eq_base_addr[band_sel].CODEC_BIQUAD_A2_x  = EQFilterCoefPoint->A2_Q;
}

/**
  * @brief  Enable or disable dac channel EQ clk.
  * @param  da_chn: select dac channel
  *          This parameter can be one of the following values:
  *            @arg DAC_L
  * @param  newstate: Enable or disable adc channel EQ clk
  *          This parameter can be one of the following values:
  *            @arg ENABLE
  *            @arg DISABLE
  * @return  None
  */
void AUDIO_CODEC_SetDACEQClk(u32 da_chn, u32 newstate)
{
	assert_param(IS_CODEC_DAC_SEL(da_chn));

	AUDIO_CODEC_TypeDef *audio_base = AUDIO_CODEC_GetAddr();

	if (newstate == ENABLE) {
		audio_base->CLOCK_CONTROL_2 |= AUDIO_CODEC_BIT_DA_L_EQ_EN;
	} else {
		audio_base->CLOCK_CONTROL_2 &= ~AUDIO_CODEC_BIT_DA_L_EQ_EN;
	}

}

/**
  * @brief  Enable or disable adc channel EQ band.
  * @param  da_chn: select adc channel
  *          This parameter can be one of the following values:
  *            @arg DAC_L
  * @param  band_sel: select EQ band of each adc channel
  *          This parameter can be one of the following values:
  *            @arg DACEQBD0
  *            @arg DACEQBD1
  *            @arg DACEQBD2
  *            @arg DACEQBD3
  *            @arg DACEQBD4
  * @param  newstate: Enable or disable adc channel EQ band
  *          This parameter can be one of the following values:
  *            @arg ENABLE
  *            @arg DISABLE
  * @return  None
  */
void AUDIO_CODEC_SetDACEQBand(u32 da_chn, u32 band_sel, u32 newstate)
{
	assert_param(IS_CODEC_DAC_SEL(da_chn));
	assert_param(IS_CODEC_DAC_EQ_Band_SEL(band_sel));

	AUDIO_CODEC_TypeDef *audio_base = AUDIO_CODEC_GetAddr();


	switch (band_sel) {
	case DACEQBD0:
		if (newstate == ENABLE) {
			audio_base->DAC_L_EQ_CTRL |= AUDIO_CODEC_BIT_DAC_L_BIQUAD_EN_0;
		} else {
			audio_base->DAC_L_EQ_CTRL &= ~AUDIO_CODEC_BIT_DAC_L_BIQUAD_EN_0;
		}
		break;

	case DACEQBD1:
		if (newstate == ENABLE) {
			audio_base->DAC_L_EQ_CTRL |= AUDIO_CODEC_BIT_DAC_L_BIQUAD_EN_1;
		} else {
			audio_base->DAC_L_EQ_CTRL &= ~AUDIO_CODEC_BIT_DAC_L_BIQUAD_EN_1;
		}
		break;


	case DACEQBD2:
		if (newstate == ENABLE) {
			audio_base->DAC_L_EQ_CTRL |= AUDIO_CODEC_BIT_DAC_L_BIQUAD_EN_2;
		} else {
			audio_base->DAC_L_EQ_CTRL &= ~AUDIO_CODEC_BIT_DAC_L_BIQUAD_EN_2;
		}
		break;

	case DACEQBD3:
		if (newstate == ENABLE) {
			audio_base->DAC_L_EQ_CTRL |= AUDIO_CODEC_BIT_DAC_L_BIQUAD_EN_3;
		} else {
			audio_base->DAC_L_EQ_CTRL &= ~AUDIO_CODEC_BIT_DAC_L_BIQUAD_EN_3;
		}
		break;


	case DACEQBD4:
		if (newstate == ENABLE) {
			audio_base->DAC_L_EQ_CTRL |= AUDIO_CODEC_BIT_DAC_L_BIQUAD_EN_4;
		} else {
			audio_base->DAC_L_EQ_CTRL &= ~AUDIO_CODEC_BIT_DAC_L_BIQUAD_EN_4;
		}
		break;

	default:
		break;
	}

}

/**
  * @brief  set EQ band as filter for DAC path
  * @param  ad_chn: select adc channel
  *          This parameter can be one of the following values:
  *            @arg DAC_L
  * @param  band_sel: select EQ band of each adc channel
  *          This parameter can be one of the following values:
  *            @arg DACEQBD0
  *            @arg DACEQBD1
  *            @arg DACEQBD2
  *            @arg DACEQBD3
  *            @arg DACEQBD4
  * @param  EQFilterCoefPoint: it is a struct pointer which points a struct that includes coefficients of filter.
  * @return  None
  */
void AUDIO_CODEC_SetDACEQFilter(u32 da_chn, u32 band_sel, CODEC_EQFilterCoef *EQFilterCoefPoint)
{
	assert_param(IS_CODEC_DAC_SEL(da_chn));
	assert_param(IS_CODEC_DAC_EQ_Band_SEL(band_sel));

	AUDIO_CODEC_TypeDef *audio_base = AUDIO_CODEC_GetAddr();
	AUDIO_CODEC_EQ_BAND_TypeDef *eq_base_addr = NULL;

	eq_base_addr = (AUDIO_CODEC_EQ_BAND_TypeDef *)audio_base->DAC_L_EQ_BAND;

	eq_base_addr[band_sel].CODEC_BIQUAD_H0_x  = EQFilterCoefPoint->H0_Q;
	eq_base_addr[band_sel].CODEC_BIQUAD_B1_x  = EQFilterCoefPoint->B1_Q;
	eq_base_addr[band_sel].CODEC_BIQUAD_B2_x  = EQFilterCoefPoint->B2_Q;
	eq_base_addr[band_sel].CODEC_BIQUAD_A1_x  = EQFilterCoefPoint->A1_Q;
	eq_base_addr[band_sel].CODEC_BIQUAD_A2_x  = EQFilterCoefPoint->A2_Q;

}

/**
  * @brief Load AUDIO_VREF calibration value from OTP to register.
  * @retval none
  */
static void AUDIO_CODEC_VREF_Calib(void)
{
	CORE_S_TypeDef *CORE_S = CORE_S_BASE;

	u8 otp_val;
	u32 otp_addr = 0x6B0;		// TODO

	OTP_Read8(otp_addr, &otp_val);

	if (otp_val == 0xFF) {
		// use default value
		otp_val = 0x10;  // 0x0A;		// 5b01010
		// return;
	}

	CORE_S->AUDIO_ANA_CTRL1 &= ~(CORE_S_MASK_AUDIO_LDO_TUNE_4_0_);
	CORE_S->AUDIO_ANA_CTRL1 |= CORE_S_AUDIO_LDO_TUNE_4_0_(otp_val);		// 5b10000

}

/**
  * @brief Audio Codec LDO power on or power down.
  * @param  ldo_mode: LDO power on or power down
  *          This parameter can be one of the following values:
  *            @arg AUD_POWER_ON:
  *            @arg AUD_POWER_DOWN:
  * @return  None
  */
void AUDIO_CODEC_SetLDOMode(u32 powermode)
{
	CORE_S_TypeDef *CORE_S = CORE_S_BASE;

	assert_param(IS_LDO_POWER_SEL(powermode));

	RTK_LOGI(TAG, "enable usb bandgap mainpwr\r\n");
	BANDGAP_BASE->USB2_BG_CTRL |= (BANDGAP_BIT_MAINPWR | BANDGAP_BIT_BG_ON_USB2);

	if (powermode == AUD_POWER_ON) {
		CORE_S->AUDIO_ANA_CTRL3 |= CORE_S_BIT_AUDIO_POWADDACK;
		/* LDO power on */
		CORE_S->AUDIO_ANA_CTRL1 &= ~(CORE_S_BIT_AUDIO_LDO_DISCHARGE_);
		CORE_S->AUDIO_ANA_CTRL1 |= (CORE_S_BIT_AUDIO_LDO_POW | CORE_S_BIT_AUDIO_LDO_PREC);
		DelayMs(2);
		CORE_S->AUDIO_ANA_CTRL1 &= ~(CORE_S_BIT_AUDIO_LDO_PREC);
		DelayUs(10);
		CORE_S->AUDIO_ANA_CTRL1 |= CORE_S_BIT_AUDIO_LDO_POW_OP9V;
		DelayUs(990);
		CORE_S->AUDIO_ANA_CTRL1 &= ~(CORE_S_BIT_AUDIO_LDO_EM);
		CORE_S->AUDIO_ANA_CTRL2 |= CORE_S_BIT_AUDIO_MBIAS_POW;		// turn on after LDO_EM = 0 and before CODEC Power is on
		AUDIO_CODEC_VREF_Calib();
	} else {
		CORE_S->AUDIO_ANA_CTRL2 &= ~(CORE_S_BIT_AUDIO_MBIAS_POW);	// turn off before LDO power down
		CORE_S->AUDIO_ANA_CTRL3 &= ~(CORE_S_BIT_AUDIO_POWADDACK);
		CORE_S->AUDIO_ANA_CTRL1 &= ~(CORE_S_BIT_AUDIO_LDO_POW_OP9V);
		DelayUs(10);
		CORE_S->AUDIO_ANA_CTRL1 &= ~(CORE_S_BIT_AUDIO_LDO_POW);
		CORE_S->AUDIO_ANA_CTRL1 |= CORE_S_BIT_AUDIO_LDO_PREC;
		CORE_S->AUDIO_ANA_CTRL1 |= CORE_S_BIT_AUDIO_LDO_DISCHARGE_;
		DelayMs(9);
		CORE_S->AUDIO_ANA_CTRL1 &= ~(CORE_S_BIT_AUDIO_LDO_PREC);
		CORE_S->AUDIO_ANA_CTRL1 &= ~(CORE_S_BIT_AUDIO_LDO_DISCHARGE_);
	}
}

/**
  * @brief Audio Codec micbias power on or power down.
  * @param  micbias_mode: micbias power on or power down
  *          This parameter can be one of the following values:
  *            @arg AUD_POWER_ON:
  *            @arg AUD_POWER_DOWN:
  * @return  None
  */
void AUDIO_CODEC_SetMicBiasPowerMode(u32 powermode)
{
	assert_param(IS_CODEC_POWER_SEL(powermode));

	AUDIO_CODEC_TypeDef *audio_base = AUDIO_CODEC_GetAddr();
	CORE_S_TypeDef *CORE_S = CORE_S_BASE;

	if (powermode == AUD_POWER_ON) {
		CORE_S->AUDIO_ANA_CTRL3 |= CORE_S_BIT_AUDIO_MICBIAS_POW;
		CORE_S->AUDIO_ANA_CTRL2 &= ~(CORE_S_BIT_AUDIO_MICBIAS_COMP);

		audio_base->AUDIO_CONTROL_1 |= AUDIO_CODEC_BIT_CKX_MICBIAS_EN;

		CORE_S->AUDIO_ANA_CTRL3 &= ~(CORE_S_MASK_AUDIO_MICBIAS_VSET_3_0_);		// use default value: 0

		CORE_S->AUDIO_ANA_CTRL2 &= ~(CORE_S_MASK_AUDIO_MBIAS_ISEL_MICBIAS_2_0__);
		CORE_S->AUDIO_ANA_CTRL2 |= CORE_S_AUDIO_MBIAS_ISEL_MICBIAS_2_0__(2);
	} else {
		CORE_S->AUDIO_ANA_CTRL3 &= ~(CORE_S_BIT_AUDIO_MICBIAS_POW);
	}
}

/**
  * @brief  Set micbst power on or down.
  * @param  amic_sel: select amic channel
  *			 This parameter can be one of the following values:
  *			   @arg AMIC1
  * @param  powermode: power on or down mode
  *			 This parameter can be one of the following values:
  *			   @arg NORMALPOWER
  *			   @arg LOWPOWER
  *			   @arg SHUTDOWN
  * @return  None
  */
void AUDIO_CODEC_SetMicBstPowerMode(u32 amic_sel, u32 powermode)
{
	assert_param(IS_CODEC_AMIC_SEL(amic_sel));
	assert_param(IS_CODEC_POWER_MODE_SEL(powermode));

	CORE_S_TypeDef *CORE_S = CORE_S_BASE;

	if (powermode == SHUTDOWN) {

		CORE_S->AUDIO_ANA_CTRL3 &= ~(CORE_S_BIT_AUDIO_MICBST_POW);
		CORE_S->AUDIO_ANA_CTRL1 &= ~(CORE_S_BIT_AUDIO_DTSDM_POW_);

		return;
	}

	if (powermode == NORMALPOWER) {
		CORE_S->AUDIO_ANA_CTRL2 &= ~(CORE_S_BIT_AUDIO_LPMODE_EN);
	}

	if (powermode == LOWPOWER) {
		CORE_S->AUDIO_ANA_CTRL2 |= CORE_S_BIT_AUDIO_LPMODE_EN;
	}

	CORE_S->AUDIO_ANA_CTRL3 |= CORE_S_BIT_AUDIO_MICBST_POW;

	CORE_S->AUDIO_ANA_CTRL2 &= ~(CORE_S_MASK_AUDIO_MBIAS_ISEL_MICBST_2_0_);
	CORE_S->AUDIO_ANA_CTRL2 |= CORE_S_AUDIO_MBIAS_ISEL_MICBST_2_0_(2);

	CORE_S->AUDIO_ANA_CTRL1 |= (CORE_S_BIT_AUDIO_DTSDM_CKXEN | CORE_S_BIT_AUDIO_DTSDM_POW_);

	CORE_S->AUDIO_ANA_CTRL2 &= ~(CORE_S_MASK_AUDIO_MBIAS_ISEL_DTSDM_2_0_ | CORE_S_MASK_AUDIO_MBIAS_ISEL_DTSDM_INT1_2_0__);
	CORE_S->AUDIO_ANA_CTRL2 |= (CORE_S_AUDIO_MBIAS_ISEL_DTSDM_2_0_(2) | CORE_S_AUDIO_MBIAS_ISEL_DTSDM_INT1_2_0__(2));
}

/**
  * @brief  Set per micbst mute or unmute.
  * @param  amic_sel: select amic channel
  *			 This parameter can be one of the following values:
  *			   @arg AMIC1
  * @param  newstate: mute or unmute option for ad channel
  *			 This parameter can be one of the following values:
  *			   @arg UNMUTE: unmute
  *			   @arg MUTE: mute
  * @return  None
  */
void AUDIO_CODEC_SetMicBstChnMute(u32 amic_sel, u32 newstate)
{
	assert_param(IS_CODEC_AMIC_SEL(amic_sel));

	CORE_S_TypeDef *CORE_S = CORE_S_BASE;

	if (newstate == UNMUTE) {
		CORE_S->AUDIO_ANA_CTRL3 &= ~(CORE_S_BIT_AUDIO_MICBST_MUTE);
	} else {
		CORE_S->AUDIO_ANA_CTRL3 |= CORE_S_BIT_AUDIO_MICBST_MUTE;
	}

}


/**
  * @brief  Set micbst single-end or differential mode.
   * @param  amic_sel: select amic channel
  *			 This parameter can be one of the following values:
  *			   @arg AMIC1
  * @param  mode: micbst single-end or differential mode
  *			 This parameter can be one of the following values:
  *			   @arg AUD_SINGLE
  *			   @arg AUD_DIFF
  * @return  None
  */
void AUDIO_CODEC_SetMicBstInputMode(u32 amic_sel, u32 mode)
{
	assert_param(IS_CODEC_AMIC_SEL(amic_sel));

	CORE_S_TypeDef *CORE_S = CORE_S_BASE;

	if (mode == AUD_SINGLE) {
		CORE_S->AUDIO_ANA_CTRL3 &= ~(CORE_S_BIT_AUDIO_MICBST_ENDF);
	} else {
		CORE_S->AUDIO_ANA_CTRL3 |= CORE_S_BIT_AUDIO_MICBST_ENDF;
	}
}

/**
  * @brief  Set micbst gain.
  * @param  amic_sel: amic select
  *          This parameter can be one of the following values:
  *            @arg AMIC1
  * @param  gain: ADC channel micbst gain select
  *          This parameter can be one of the following values:
  *            @arg MICBST_GAIN_0DB: 0dB
  *            @arg MICBST_Gain_5dB: 5dB
  *            ...
  *            @arg MICBST_Gain_40dB: 40dB
  * @note ADC digital volume is 0dB~+40dB in 5dB step.
  * @return  None
  */
void AUDIO_CODEC_SetMicBstGain(u32 amic_sel, u32 gain)
{
	assert_param(IS_CODEC_AMIC_SEL(amic_sel));

	CORE_S_TypeDef *CORE_S = CORE_S_BASE;

	CORE_S->AUDIO_ANA_CTRL3 &= ~(CORE_S_MASK_AUDIO_MICBST_GSEL_3_0_);
	CORE_S->AUDIO_ANA_CTRL3 |= CORE_S_AUDIO_MICBST_GSEL_3_0_(gain);

}

/**
  * @brief  Set DAC power mode.
  * @param  channel: the value of dac path.
  *          This parameter can be one of the following values:
  *            @arg 0: DAC_L
  * @param  newstate: enable or disable dac power.
  *			 This parameter can be one of the following values:
  *			   @arg AUD_POWER_ON
  *			   @arg AUD_POWER_DOWN
  */
void AUDIO_CODEC_SetDACPowerMode(u32 channel, u32 powermode)
{
	assert_param(IS_CODEC_DAC_SEL(channel));
	assert_param(IS_CODEC_POWER_SEL(powermode));

	CORE_S_TypeDef *CORE_S = CORE_S_BASE;

	if (powermode == AUD_POWER_ON) {
		CORE_S->AUDIO_ANA_CTRL1 |= (CORE_S_BIT_AUDIO_DAC_CKXEN | CORE_S_BIT_AUDIO_DAC_POW);
		CORE_S->AUDIO_ANA_CTRL1 &= ~(CORE_S_BIT_AUDIO_DAC_CKXSEL);		// select ckx2/4
		CORE_S->AUDIO_ANA_CTRL2 &= ~(CORE_S_MASK_AUDIO_MBIAS_ISEL_DAC_2_0_);
		CORE_S->AUDIO_ANA_CTRL2 |= CORE_S_AUDIO_MBIAS_ISEL_DAC_2_0_(2);
	} else {
		CORE_S->AUDIO_ANA_CTRL1 &= ~(CORE_S_BIT_AUDIO_DAC_CKXEN | CORE_S_BIT_AUDIO_DAC_POW);
	}

}

/**
  * @brief  Set lineout power mode.
  * @param  channel: the value of dac path.
  *          This parameter can be one of the following values:
  *            @arg 0: DAC_L
  * @param  newstate: enable or disable dac power.
  *			 This parameter can be one of the following values:
  *			   @arg AUD_POWER_ON
  *			   @arg AUD_POWER_DOWN
  */
void AUDIO_CODEC_SetLineOutPowerMode(u32 channel, u32 powermode)
{
	assert_param(IS_CODEC_DAC_SEL(channel));
	assert_param(IS_CODEC_POWER_SEL(powermode));

	CORE_S_TypeDef *CORE_S = CORE_S_BASE;

	if (powermode == AUD_POWER_ON) {
		CORE_S->AUDIO_ANA_CTRL2 |= CORE_S_BIT_AUDIO_LO_POW;
		CORE_S->AUDIO_ANA_CTRL1 &= ~(CORE_S_BIT_AUDIO_LO_ENDP);
		CORE_S->AUDIO_ANA_CTRL2 &= ~(CORE_S_MASK_AUDIO_MBIAS_ISEL_LO_2_0_);
		CORE_S->AUDIO_ANA_CTRL2 |= CORE_S_AUDIO_MBIAS_ISEL_LO_2_0_(2);
	} else {
		CORE_S->AUDIO_ANA_CTRL2 &= ~(CORE_S_BIT_AUDIO_LO_POW);
	}

}

/**
  * @brief  Set Line out single-end or differential mode.
  * @param  channel: the value of dac path.
  *          This parameter can be one of the following values:
  *            @arg 0: DAC_L
  * @param  mode:
  *			 This parameter can be one of the following values:
  *			   @arg AUD_SINGLE
  *			   @arg AUD_DIFF
  * @return  None
  */
void AUDIO_CODEC_SetLineOutMode(u32 channel, u32 mode)
{
	assert_param(IS_CODEC_DAC_SEL(channel));

	CORE_S_TypeDef *CORE_S = CORE_S_BASE;

	if (mode == AUD_SINGLE) {
		CORE_S->AUDIO_ANA_CTRL2 |= CORE_S_BIT_AUDIO_LO_SE;
	} else {
		CORE_S->AUDIO_ANA_CTRL2 &= ~(CORE_S_BIT_AUDIO_LO_SE);
	}

}

/**
  * @brief  Set Line out mute control: dac in or analog in.
  * @param  channel: the value of dac path.
  *          This parameter can be one of the following values:
  *            @arg 0: DAC_L
  * @param  type: dac in or analog in.
  *			 This parameter can be one of the following values:
  *			   @arg AUD_DACIN
  *			   @arg AUD_ANALOGIN
  * @param  newstate:
  *			 This parameter can be one of the following values:
  *			   @arg UNMUTE
  *			   @arg MUTE
  * @return  None
  */
void AUDIO_CODEC_SetLineOutMute(u32 channel, u32 type, u32 newstate)
{
	assert_param(IS_CODEC_DAC_SEL(channel));

	CORE_S_TypeDef *CORE_S = CORE_S_BASE;

	if (newstate == UNMUTE) {
		CORE_S->AUDIO_ANA_CTRL1 &= ~(CORE_S_MASK_AUDIO_LO_M_1_0_);
		CORE_S->AUDIO_ANA_CTRL1 |= CORE_S_AUDIO_LO_M_1_0_(type);
	} else {
		CORE_S->AUDIO_ANA_CTRL1 |= CORE_S_MASK_AUDIO_LO_M_1_0_;
	}

}

/**
  * @brief	Audio codec record flow for test.
  */
void AUDIO_CODEC_Record(u32 type, I2S_InitTypeDef *I2S_InitStruct)
{
	DiagPrintf("record setting \r\n");

	u32 rx_tdm_type = I2S_InitStruct->CODEC_SelRxI2STdm;

	AUDIO_CODEC_SetAudioIP(ENABLE);
	AUDIO_CODEC_SetI2SIP(ENABLE);
	AUDIO_CODEC_SetI2SSRC(INTERNAL_SPORT);
	AUDIO_CODEC_SetADCSRSrc(SOURCE1, I2S_InitStruct->CODEC_SelI2SRxSR);
	AUDIO_CODEC_SetANAClk(ENABLE);
	AUDIO_CODEC_SetI2SParameters(ADCPATH, I2S_InitStruct);

	if (rx_tdm_type == I2S_NOTDM || rx_tdm_type == I2S_TDM4) {

		AUDIO_CODEC_EnableADC(ADCHN0, ENABLE);
		AUDIO_CODEC_EnableADC(ADCHN1, ENABLE);

		AUDIO_CODEC_EnableADCFifo(ADCHN0, ENABLE);
		AUDIO_CODEC_EnableADCFifo(ADCHN1, ENABLE);

		AUDIO_CODEC_SetADCVolume(ADC0, 0X2F);
		AUDIO_CODEC_SetADCVolume(ADC1, 0X2F);

		AUDIO_CODEC_SetADCHPF(ADC0, 3, ENABLE);
		AUDIO_CODEC_SetADCHPF(ADC1, 3, ENABLE);

		AUDIO_CODEC_SetADCMute(ADC0, UNMUTE);
		AUDIO_CODEC_SetADCMute(ADC1, UNMUTE);
	}

	if (rx_tdm_type == I2S_TDM4) {

		AUDIO_CODEC_EnableADC(ADCHN2, ENABLE);
		AUDIO_CODEC_EnableADCFifo(ADCHN2, ENABLE);
		AUDIO_CODEC_SetADCVolume(ADC2, 0X2F);
		AUDIO_CODEC_SetADCHPF(ADC2, 3, ENABLE);
		AUDIO_CODEC_SetADCMute(ADC2, UNMUTE);

	}

	if (type == APP_DAAD_LPBK) {

		if (rx_tdm_type == I2S_NOTDM || rx_tdm_type == I2S_TDM4) {

			AUDIO_CODEC_SetADCMixMute(ADC0, AD_MIX_MUTE, UNMUTE);
			AUDIO_CODEC_SetADCMixMute(ADC1, AD_MIX_MUTE, UNMUTE);

			AUDIO_CODEC_SetADCANAFilter(ADC0, ENABLE);
			AUDIO_CODEC_SetADCANAFilter(ADC1, ENABLE);
		}

		if (rx_tdm_type == I2S_TDM4) {

			AUDIO_CODEC_SetADCMixMute(ADC2, AD_MIX_MUTE, UNMUTE);
			AUDIO_CODEC_SetADCANAFilter(ADC2, ENABLE);

		}

	}

	if (type == APP_AMIC_RECORD) {

		AUDIO_CODEC_SetMicBiasPowerMode(AUD_POWER_ON);

		AUDIO_CODEC_SetMicBstPowerMode(AMIC1, NORMALPOWER);
		AUDIO_CODEC_SetMicBstChnMute(AMIC1, UNMUTE);
		RTK_LOGI(TAG, "AMIC Diff\r\n");
		AUDIO_CODEC_SetMicBstInputMode(AMIC1, AUD_DIFF);
		AUDIO_CODEC_SetMicBstGain(AMIC1, MICBST_GAIN_0DB);

		if (rx_tdm_type == I2S_NOTDM || rx_tdm_type == I2S_TDM4) {

			AUDIO_CODEC_SetADCMixMute(ADC0, AD_MIX_MUTE, UNMUTE);
			AUDIO_CODEC_SetADCMixMute(ADC1, AD_MIX_MUTE, UNMUTE);

			AUDIO_CODEC_SetADCANAFilter(ADC0, ENABLE);
			AUDIO_CODEC_SetADCANAFilter(ADC1, ENABLE);

			AUDIO_CODEC_SetADCANASrc(ADCHN0);
			AUDIO_CODEC_SetADCANASrc(ADCHN1);

		}

		if (rx_tdm_type == I2S_TDM4) {

			AUDIO_CODEC_SetADCMixMute(ADC2, AD_MIX_MUTE, UNMUTE);
			AUDIO_CODEC_SetADCANAFilter(ADC2, ENABLE);
			AUDIO_CODEC_SetADCANASrc(ADCHN2);

		}

	}


	if (type == APP_DMIC_RECORD) {

		AUDIO_CODEC_SetDmicClk(DMIC_2P5M, ENABLE);

		if (rx_tdm_type == I2S_NOTDM || rx_tdm_type == I2S_TDM4) {

			AUDIO_CODEC_SetADCMixMute(ADC0, DMIC_MIX_MUTE, UNMUTE);
			AUDIO_CODEC_SetADCMixMute(ADC1, DMIC_MIX_MUTE, UNMUTE);

			AUDIO_CODEC_SetADCDmicFilter(ADC0, ENABLE);
			AUDIO_CODEC_SetADCDmicFilter(ADC1, ENABLE);

			AUDIO_CODEC_SetDmicSrc(ADCHN0, DMIC1);
			AUDIO_CODEC_SetDmicSrc(ADCHN1, DMIC2);
		}

		if (rx_tdm_type == I2S_TDM4) {

			// AUDIO_CODEC_SetADCMixMute(ADC2, DMIC_MIX_MUTE, UNMUTE);
			// AUDIO_CODEC_SetADCDmicFilter(ADC2, ENABLE);
			// AUDIO_CODEC_SetDmicSrc(ADCHN2, DMIC1);


			AUDIO_CODEC_SetMicBiasPowerMode(AUD_POWER_ON);

			AUDIO_CODEC_SetMicBstPowerMode(AMIC1, NORMALPOWER);
			AUDIO_CODEC_SetMicBstChnMute(AMIC1, UNMUTE);
			RTK_LOGI(TAG, "AMIC Diff\r\n");
			AUDIO_CODEC_SetMicBstInputMode(AMIC1, AUD_DIFF);
			AUDIO_CODEC_SetMicBstGain(AMIC1, MICBST_GAIN_0DB);

			AUDIO_CODEC_SetADCMixMute(ADC2, AD_MIX_MUTE, UNMUTE);
			AUDIO_CODEC_SetADCANAFilter(ADC2, ENABLE);
			AUDIO_CODEC_SetADCANASrc(ADCHN2);
		}
	}
}

/**
  * @brief	Audio codec playback flow for test.
  */
void AUDIO_CODEC_Playback(u32 type, I2S_InitTypeDef *I2S_InitStruct)
{
	DiagPrintf("playback setting \r\n");

	AUDIO_CODEC_SetAudioIP(ENABLE);
	AUDIO_CODEC_SetI2SIP(ENABLE);
	AUDIO_CODEC_SetI2SSRC(INTERNAL_SPORT);
	AUDIO_CODEC_SetI2SParameters(DACPATH, I2S_InitStruct); //根据实际情况再具体设定
	AUDIO_CODEC_SetDACSRSrc(SOURCE0, I2S_InitStruct->CODEC_SelI2STxSR);
	AUDIO_CODEC_SetANAClk(ENABLE);
	AUDIO_CODEC_EnableDAC(DAC_L, ENABLE);
	AUDIO_CODEC_EnableDACFifo(ENABLE);
	AUDIO_CODEC_SetDACMute(DAC_L, UNMUTE);
	AUDIO_CODEC_SetDACVolume(DAC_L, 0xAF);
	AUDIO_CODEC_SetDACHPF(DAC_L, ENABLE);

	if (type == APP_DAAD_LPBK) {
		AUDIO_CODEC_SetDACSrc(I2SL); // for lineout
	}

	if (type == APP_LINE_OUT) {
		AUDIO_CODEC_SetDACSrc(I2SL);
		AUDIO_CODEC_SetDACPowerMode(DAC_L, AUD_POWER_ON);
		AUDIO_CODEC_SetLineOutPowerMode(DAC_L, AUD_POWER_ON);
		RTK_LOGI(TAG, "Lineout diff\r\n");
		AUDIO_CODEC_SetLineOutMode(DAC_L, AUD_DIFF);
		AUDIO_CODEC_SetLineOutMute(DAC_L, AUD_DACIN, UNMUTE);
	}

	if (type == APP_PDM_OUT) {
		AUDIO_CODEC_SetDACSrc(I2SL);
		AUDIO_CODEC_SetPDMClk(DAC_L, PDM_5M, ENABLE);
	}

	if (type == APP_TESTTONE) {
		AUDIO_CODEC_SetDACSrc(TESTTONE);
		AUDIO_CODEC_SetTesttone(3, ENABLE);
	}
}

/**
  * @}
  */

/** @} */

/** @} */

/** @} */

