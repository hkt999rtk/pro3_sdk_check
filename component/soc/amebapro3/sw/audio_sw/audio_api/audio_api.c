/******************************************************************************
*
* Copyright(c) 2007 - 2025 Realtek Corporation. All rights reserved.
*
******************************************************************************/
#include "ameba_soc.h"
#include "ameba_sport.h"
#include "ameba_audio.h"
#include "audio_api.h"

#define SPORT_INDEX 0

static const char *const TAG = "AUDIO";
static audio_t *audio_obj;
static uint32_t dmic_clk_set;
static uint32_t dmic_data_set;

static u32 sp_tx_complete(void *data)
{
	audio_t *obj = (audio_t *)data;
	u8 *pbuf;

	if (obj) {
		GDMA_ClearINT(obj->gdma_tx.GDMA_Index, obj->gdma_tx.GDMA_ChNum);
		pbuf = obj->tx_buf + (obj->tx_page_idx * obj->page_size);
		obj->tx_handler(obj->tx_handler_arg, pbuf);
		AUDIO_SP_TXGDMA_Restart(obj->gdma_tx.GDMA_Index, obj->gdma_tx.GDMA_ChNum,
								(u32)pbuf,
								obj->page_size);
	}

	return 0;
}

static u32 sp_rx_complete(void *data)
{
	audio_t *obj = (audio_t *)data;
	u8 *pbuf;

	if (obj) {
		GDMA_ClearINT(obj->gdma_rx.GDMA_Index, obj->gdma_rx.GDMA_ChNum);
		pbuf = obj->rx_buf + (obj->rx_page_idx * obj->page_size);
		obj->rx_handler(obj->rx_handler_arg, pbuf);
		AUDIO_SP_RXGDMA_Restart(obj->gdma_rx.GDMA_Index, obj->gdma_rx.GDMA_ChNum,
								(u32)(obj->rx_buf + (obj->rx_page_idx * obj->page_size)),
								obj->page_size);
	}

	return 0;
}

void audio_init(audio_t *obj, audio_output_mode output_mode, audio_input_mode input_mode)
{
	if (obj->audio_initialized == 0) {
		obj->sport_idx = SPORT_INDEX;
		obj->input_mode = input_mode;
		obj->output_mode = output_mode;

		RCC_PeriphClockDividerSet(SYS_SPORT_CLK, 1, 1);
		RCC_PeriphClockSourceSet(SYS_SPORT_CLK, XTAL40M);
		RCC_PeriphClockSourceSet(AUDIO_CODEC_CLK, XTAL40M);

		/* Enable sport0/1, gdma0/1 clk and rst */
		RCC_PeriphClockCmd(APBPeriph_SPORT0, APBPeriph_SPORT0_CLOCK, ENABLE);
		RCC_PeriphClockCmd(APBPeriph_SPORT1, APBPeriph_SPORT1_CLOCK, ENABLE);
		RCC_PeriphClockCmd(APBPeriph_GDMA0, APBPeriph_GDMA0_CLOCK, ENABLE);
		RCC_PeriphClockCmd(APBPeriph_GDMA1, APBPeriph_GDMA1_CLOCK, ENABLE);

		/* Enable digital codec clk and rst */
		RCC_PeriphClockCmd(APBPeriph_NULL, APBPeriph_AC_CLOCK, ENABLE);
		RCC_PeriphClockCmd(APBPeriph_NULL, APBPeriph_AUDIO_CODEC_CLOCK, ENABLE);
		RCC_PeriphClockCmd(APBPeriph_NULL, APBPeriph_AUDIO_CODEC_PCLOCK, ENABLE);
		RCC_PeriphClockCmd(APBPeriph_AC_PCLK, APBPeriph_CLOCK_NULL, ENABLE);
		RCC_PeriphClockCmd(APBPeriph_AC_MCLK, APBPeriph_CLOCK_NULL, ENABLE);

		/* Enable analog codec */
		CORE_S_BASE->SYS_AUDIO_CTRL |= CORE_S_BIT_SYS_CODEC_PORB | (BIT(12));
		REGU_BASE->REGU_MISC |= REGU_BIT_AUDIO_STB_EN;

		/* Enable platform clk settings */
		RCC_PeriphClockCmd(APBPeriph_PLFM_CORE_SEC_CLK, APBPeriph_CBUS_SEC_CLOCK, ENABLE);
		RCC_PeriphClockCmd(NULL, APBPeriph_CBUS_SYS_MEM_CLOCK, ENABLE);

		/* Initialize SPORT TX */
		AUDIO_SP_StructInit(&obj->sp_init_tx);
		obj->sp_init_tx.SP_SelClk = PERI_ClkGet(CLKID_AUDIO_SPORT);
		obj->sp_init_tx.SP_SR = SP_48K;
		obj->sp_init_tx.SP_SelTDM = SP_TX_NOTDM;
		obj->sp_init_tx.SP_SelFIFO = SP_TX_FIFO2;
		obj->sp_init_tx.SP_SelChLen = SP_TXCL_32;
		obj->sp_init_tx.SP_SelWordLen = SP_TXWL_16;
		AUDIO_SP_Init(obj->sport_idx, SP_DIR_TX, &obj->sp_init_tx);
		RTK_LOGI(TAG, "init SPORT %d tx done\r\n", obj->sport_idx);

		/* Initialize SPORT RX */
		AUDIO_SP_StructInit(&obj->sp_init_rx);
		obj->sp_init_rx.SP_SelClk = PERI_ClkGet(CLKID_AUDIO_SPORT);
		obj->sp_init_rx.SP_SR = SP_48K;
		obj->sp_init_rx.SP_SelTDM = SP_RX_NOTDM;
		obj->sp_init_rx.SP_SelFIFO = SP_RX_FIFO2;
		obj->sp_init_rx.SP_SelChLen = SP_RXCL_32;
		obj->sp_init_rx.SP_SelWordLen = SP_RXWL_16;
		AUDIO_SP_Init(obj->sport_idx, SP_DIR_RX, &obj->sp_init_rx);
		RTK_LOGI(TAG, "init SPORT %d rx done\r\n", obj->sport_idx);

		/* Set SPORT as Master */
		AUDIO_SP_SetMasterSlave(obj->sport_idx, MASTER);

		/* Power on CODEC */
		AUDIO_CODEC_SetLDOMode(AUD_POWER_ON);
		AUDIO_CODEC_I2S_StructInit(&obj->i2s_init);
		obj->i2s_init.CODEC_SelRxI2STdm = I2S_NOTDM;
		obj->i2s_init.CODEC_SelI2STxSR = SR_48K;
		obj->i2s_init.CODEC_SelI2STxWordLen = SP_TXWL_16;
		obj->i2s_init.CODEC_SelI2SRxSR = SR_48K;
		obj->i2s_init.CODEC_SelI2SRxWordLen = SP_RXWL_16;
		RTK_LOGI(TAG, "init CODEC done\r\n");

		/* Configure output */
		RTK_LOGI(TAG, "set output_mode = %d\r\n", output_mode);
		if (output_mode != OUTPUT_DISABLE) {
			AUDIO_CODEC_Playback(APP_LINE_OUT, &obj->i2s_init);
			AUDIO_CODEC_SetDACSrc(I2SL);
			AUDIO_CODEC_SetDACASRC(SR_48K, ENABLE);
			AUDIO_CODEC_SetDACVolume(DAC_L, 0xAF);  /* 0dB */
			AUDIO_CODEC_SetDACMute(DAC_L, UNMUTE);
			AUDIO_CODEC_SetDACPowerMode(DAC_L, AUD_POWER_ON);
			AUDIO_CODEC_SetLineOutPowerMode(DAC_L, AUD_POWER_ON);
			if (output_mode == OUTPUT_SINGLE_ENDED) {
				AUDIO_CODEC_SetLineOutMode(DAC_L, AUD_SINGLE);
			} else if (output_mode == OUTPUT_DIFFERENTIAL) {
				AUDIO_CODEC_SetLineOutMode(DAC_L, AUD_DIFF);
			}
			AUDIO_CODEC_SetLineOutMute(DAC_L, AUD_DACIN, UNMUTE);
			obj->dac_l_dmute_en = DISABLE;
		}

		/* Configure input */
		RTK_LOGI(TAG, "set input_mode = %d\r\n", input_mode);
		if (input_mode != INPUT_DISABLE) {
			if (input_mode == LEFT_DMIC || input_mode == RIGHT_DMIC || input_mode == STEREO_DMIC) {
				Pinmux_Config(_PC_6, PINMUX_FUNCTION_DMIC);
				Pinmux_Config(_PC_10, PINMUX_FUNCTION_DMIC);
				AUDIO_CODEC_Record(APP_DMIC_RECORD, &obj->i2s_init);
			} else {
				AUDIO_CODEC_Record(APP_AMIC_RECORD, &obj->i2s_init);
			}
			AUDIO_CODEC_SetADCVolume(ADC0, 0x2F);
			AUDIO_CODEC_SetADCVolume(ADC1, 0x2F);
			AUDIO_CODEC_SetADCMute(ADC0, UNMUTE);
			AUDIO_CODEC_SetADCMute(ADC1, UNMUTE);
			AUDIO_CODEC_SetADCASRC(ADC0, SR_48K, ENABLE);
			AUDIO_CODEC_SetADCASRC(ADC1, SR_48K, ENABLE);
			AUDIO_CODEC_SetMicBiasPowerMode(AUD_POWER_ON);
			AUDIO_CODEC_SetMicBstGain(AMIC1, MICBST_GAIN_40DB);
			obj->adc_l_dmute_en = DISABLE;
			obj->adc_r_dmute_en = DISABLE;
		}

		obj->audio_initialized = 1;
		obj->tx_started = 0;
		obj->rx_started = 0;
		obj->tx_err_cnt = 0;
		obj->rx_err_cnt = 0;
		audio_obj = obj;
	}

}

void audio_deinit(audio_t *obj)
{
	if (obj->audio_initialized == 1) {
		if (obj->tx_started) {
			audio_tx_stop(obj);
		}
		if (obj->rx_started) {
			audio_rx_stop(obj);
		}
		AUDIO_SP_Unregister(obj->sport_idx, SP_DIR_TX);
		AUDIO_SP_Unregister(obj->sport_idx, SP_DIR_RX);

		AUDIO_CODEC_SetMicBstPowerMode(AMIC1, SHUTDOWN);
		AUDIO_CODEC_SetDACPowerMode(DAC_L, AUD_POWER_DOWN);
		AUDIO_CODEC_SetLineOutPowerMode(DAC_L, AUD_POWER_DOWN);
		AUDIO_CODEC_SetMicBiasPowerMode(AUD_POWER_DOWN);
		AUDIO_CODEC_SetLDOMode(AUD_POWER_DOWN);

		AUDIO_CODEC_SetAudioIP(DISABLE);
		AUDIO_CODEC_SetI2SIP(DISABLE);

		RCC_PeriphClockCmd(APBPeriph_SPORT0, APBPeriph_SPORT0_CLOCK, DISABLE);
		RCC_PeriphClockCmd(APBPeriph_NULL, APBPeriph_AUDIO_CODEC_CLOCK, DISABLE);
		RCC_PeriphClockCmd(APBPeriph_NULL, APBPeriph_AUDIO_CODEC_PCLOCK, DISABLE);
		RCC_PeriphClockCmd(APBPeriph_AC_PCLK, APBPeriph_CLOCK_NULL, DISABLE);
		RCC_PeriphClockCmd(APBPeriph_AC_MCLK, APBPeriph_CLOCK_NULL, DISABLE);

		obj->audio_initialized = 0;
		audio_obj = NULL;
	}
}

void audio_dac_clk(audio_t *obj, BOOL dac_clk_en)
{
	if (obj->audio_initialized == 1) {
		AUDIO_CODEC_EnableDACFifo(ENABLE);
	}
}

void audio_adc_clk(audio_t *obj, BOOL adc_l_clk_en, BOOL adc_r_clk_en)
{
	if (obj->audio_initialized == 1) {
		AUDIO_CODEC_EnableADCFifo(ADC0, adc_l_clk_en);
		AUDIO_CODEC_EnableADCFifo(ADC1, adc_r_clk_en);
	}
}

static u32 audio_sr_to_sport_sr(audio_sr sr)
{
	switch (sr) {
	case SR_48K:
		return SP_48K;
	case SR_96K:
		return SP_96K;
	case SR_32K:
		return SP_32K;
	case SR_16K:
		return SP_16K;
	case SR_8K:
		return SP_8K;
	case SR_44P1K:
		return SP_44P1K;
	case SR_88P2K:
		return SP_88P2K;
	default:
		return SP_48K;
	}
}

void audio_set_param(audio_t *obj, audio_sr sample_rate, audio_wl word_length)
{
	audio_set_param_adv(obj, sample_rate, word_length, A_MONO, A_MONO);
}

void audio_set_param_adv(audio_t *obj, audio_sr sample_rate, audio_wl word_length, audio_ch tx_ch, audio_ch rx_ch)
{
	if (obj->audio_initialized == 1) {
		AUDIO_SP_TXStart(obj->sport_idx, DISABLE);
		AUDIO_SP_Unregister(obj->sport_idx, SP_DIR_TX);
		AUDIO_SP_RXStart(obj->sport_idx, DISABLE);
		AUDIO_SP_Unregister(obj->sport_idx, SP_DIR_RX);

		u32 sport_sr = audio_sr_to_sport_sr(sample_rate);

		obj->sp_init_tx.SP_SelClk = PERI_ClkGet(CLKID_AUDIO_SPORT);
		obj->sp_init_tx.SP_SR = sport_sr;
		obj->sp_init_tx.SP_SelWordLen = (word_length == WL_16BIT) ? SP_TXWL_16 : SP_TXWL_24;
		obj->sp_init_tx.SP_SelI2SMonoStereo = (tx_ch == A_MONO) ? SP_CH_MONO : SP_CH_STEREO;
		AUDIO_SP_Init(obj->sport_idx, SP_DIR_TX, &obj->sp_init_tx);

		obj->sp_init_rx.SP_SelClk = PERI_ClkGet(CLKID_AUDIO_SPORT);
		obj->sp_init_rx.SP_SR = sport_sr;
		obj->sp_init_rx.SP_SelWordLen = (word_length == WL_16BIT) ? SP_RXWL_16 : SP_RXWL_24;
		obj->sp_init_rx.SP_SelI2SMonoStereo = (rx_ch == A_MONO) ? SP_CH_MONO : SP_CH_STEREO;
		AUDIO_SP_Init(obj->sport_idx, SP_DIR_RX, &obj->sp_init_rx);

		AUDIO_SP_SetMasterSlave(obj->sport_idx, MASTER);

		obj->i2s_init.CODEC_SelI2STxSR = sample_rate;
		obj->i2s_init.CODEC_SelI2STxWordLen = word_length;
		obj->i2s_init.CODEC_SelI2SRxSR = sample_rate;
		obj->i2s_init.CODEC_SelI2SRxWordLen = word_length;

		AUDIO_CODEC_SetADCASRC(ADC0, sample_rate, ENABLE);
		AUDIO_CODEC_SetADCASRC(ADC1, sample_rate, ENABLE);
		AUDIO_CODEC_SetDACASRC(sample_rate, ENABLE);
	}
}

void audio_set_dma_buffer(audio_t *obj, u8 *tx_buf, u8 *rx_buf, u32 page_size, audio_page_num page_num)
{
	if (obj->audio_initialized == 1) {
		obj->tx_buf = tx_buf;
		obj->rx_buf = rx_buf;
		obj->page_size = page_size;
		obj->page_num = page_num;
		obj->tx_page_idx = 0;
		obj->rx_page_idx = 0;
	}
}

void audio_tx_irq_handler(audio_t *obj, audio_irq_handler tx_handler, u32 *arg)
{
	if (obj->audio_initialized == 1) {
		obj->tx_handler = tx_handler;
		obj->tx_handler_arg = (u32)arg;
	}
}

void audio_rx_irq_handler(audio_t *obj, audio_irq_handler rx_handler, u32 *arg)
{
	if (obj->audio_initialized == 1) {
		obj->rx_handler = rx_handler;
		obj->rx_handler_arg = (u32)arg;
	}
}

void audio_err_irq_handler(audio_t *obj, audio_irq_handler err_handler, u32 *arg)
{
	if (obj->audio_initialized == 1) {
		obj->err_handler = err_handler;
		obj->err_handler_arg = (u32)arg;
	}
}

u8 *audio_get_tx_page_adr(audio_t *obj)
{
	if (obj->audio_initialized == 1 && obj->tx_buf != NULL) {
		return obj->tx_buf + (obj->tx_page_idx * obj->page_size);
	}
	return NULL;
}

void audio_set_tx_page(audio_t *obj, u8 *pbuf)
{
	if (obj->audio_initialized == 1) {
		obj->tx_page_idx = (obj->tx_page_idx + 1) % obj->page_num;
	}
}

void audio_set_rx_page(audio_t *obj)
{
	if (obj->audio_initialized == 1) {
		obj->rx_page_idx = (obj->rx_page_idx + 1) % obj->page_num;
	}
}

void audio_tx_start(audio_t *obj)
{
	if (obj->audio_initialized == 1 && obj->tx_started == 0) {
		audio_clean_error_cnt(obj);

		AUDIO_SP_DmaCmd(obj->sport_idx, ENABLE);

		GDMA_StructInit(&obj->gdma_tx);
		obj->gdma_tx.GDMA_Index = 1;
		obj->gdma_tx.SecureTransfer = ENABLE;

		AUDIO_SP_TXGDMA_Init(obj->sport_idx, GDMA_INT, &obj->gdma_tx, obj,
							 (IRQ_FUN)sp_tx_complete, obj->tx_buf, obj->page_size);
		AUDIO_SP_TXStart(obj->sport_idx, ENABLE);
		obj->tx_started = 1;
	}
}

void audio_rx_start(audio_t *obj)
{
	if (obj->audio_initialized == 1 && obj->rx_started == 0) {
		audio_clean_error_cnt(obj);

		AUDIO_SP_DmaCmd(obj->sport_idx, ENABLE);

		GDMA_StructInit(&obj->gdma_rx);
		obj->gdma_rx.GDMA_Index = 1;
		obj->gdma_rx.SecureTransfer = ENABLE;

		AUDIO_SP_RXGDMA_Init(obj->sport_idx, GDMA_INT, &obj->gdma_rx, obj,
							 (IRQ_FUN)sp_rx_complete, obj->rx_buf, obj->page_size);
		AUDIO_SP_RXStart(obj->sport_idx, ENABLE);
		obj->rx_started = 1;
	}
}

void audio_trx_start(audio_t *obj)
{
	if (obj->audio_initialized == 1) {
		audio_clean_error_cnt(obj);

		AUDIO_SP_DmaCmd(obj->sport_idx, ENABLE);

		if (obj->rx_started == 0) {
			GDMA_StructInit(&obj->gdma_rx);
			obj->gdma_rx.GDMA_Index = 1;
			obj->gdma_rx.SecureTransfer = ENABLE;

			AUDIO_SP_RXGDMA_Init(obj->sport_idx, GDMA_INT, &obj->gdma_rx, obj,
								 (IRQ_FUN)sp_rx_complete, obj->rx_buf, obj->page_size);
			AUDIO_SP_RXStart(obj->sport_idx, ENABLE);
			obj->rx_started = 1;
		}
		if (obj->tx_started == 0) {
			GDMA_StructInit(&obj->gdma_tx);
			obj->gdma_tx.GDMA_Index = 1;
			obj->gdma_tx.SecureTransfer = ENABLE;

			AUDIO_SP_TXGDMA_Init(obj->sport_idx, GDMA_INT, &obj->gdma_tx, obj,
								 (IRQ_FUN)sp_tx_complete, obj->tx_buf, obj->page_size);
			AUDIO_SP_TXStart(obj->sport_idx, ENABLE);
			obj->tx_started = 1;
		}
	}
}

void audio_tx_stop(audio_t *obj)
{
	if (obj->audio_initialized == 1) {
		AUDIO_SP_TXStart(obj->sport_idx, DISABLE);
		GDMA_ClearINT(obj->gdma_tx.GDMA_Index, obj->gdma_tx.GDMA_ChNum);
		GDMA_Cmd(obj->gdma_tx.GDMA_Index, obj->gdma_tx.GDMA_ChNum, DISABLE);
		GDMA_ChnlFree(obj->gdma_tx.GDMA_Index, obj->gdma_tx.GDMA_ChNum);
		obj->tx_started = 0;
	}
}

void audio_rx_stop(audio_t *obj)
{
	if (obj->audio_initialized == 1) {
		AUDIO_SP_RXStart(obj->sport_idx, DISABLE);
		GDMA_ClearINT(obj->gdma_rx.GDMA_Index, obj->gdma_rx.GDMA_ChNum);
		GDMA_Cmd(obj->gdma_rx.GDMA_Index, obj->gdma_rx.GDMA_ChNum, DISABLE);
		GDMA_ChnlFree(obj->gdma_rx.GDMA_Index, obj->gdma_rx.GDMA_ChNum);
		obj->rx_started = 0;
	}
}

void audio_trx_stop(audio_t *obj)
{
	if (obj->audio_initialized == 1) {
		audio_tx_stop(obj);
		audio_rx_stop(obj);
	}
}

u8 audio_get_tx_error_cnt(audio_t *obj)
{
	if (obj->audio_initialized == 1) {
		return obj->tx_err_cnt;
	}
	return 0;
}

u8 audio_get_rx_error_cnt(audio_t *obj)
{
	if (obj->audio_initialized == 1) {
		return obj->rx_err_cnt;
	}
	return 0;
}

void audio_clean_error_cnt(audio_t *obj)
{
	if (obj->audio_initialized == 1) {
		obj->tx_err_cnt = 0;
		obj->rx_err_cnt = 0;
	}
}

void audio_sport_loopback(audio_t *obj, BOOL en)
{
	if (obj->audio_initialized == 1) {
		AUDIO_SP_SetSelfLPBK(obj->sport_idx);
	}
}

void audio_mic_analog_mute(audio_t *obj, BOOL en)
{
	if (obj->audio_initialized == 1) {
		AUDIO_CODEC_SetMicBstChnMute(AMIC1, en);
	}
}

void audio_mic_analog_gain(audio_t *obj, BOOL en, audio_mic_gain mic_gain)
{
	if (obj->audio_initialized == 1 && en) {
		AUDIO_CODEC_SetMicBstGain(AMIC1, mic_gain);
	}
}

void audio_hpo_amplifier(audio_t *obj, BOOL en)
{
	if (obj->audio_initialized == 1) {
		if (en) {
			AUDIO_CODEC_SetLineOutPowerMode(DAC_L, AUD_POWER_ON);
		} else {
			AUDIO_CODEC_SetLineOutPowerMode(DAC_L, AUD_POWER_DOWN);
		}
	}
}

void audio_adc_digital_vol(audio_t *obj, u8 step)
{
	if (obj->audio_initialized == 1) {
		if (step > 0x7F) {
			RTK_LOGE(TAG, " The adc step exceeds the maximum value 0x7F. \r\n");
		} else {
			AUDIO_CODEC_SetADCVolume(ADC0, step);
			AUDIO_CODEC_SetADCVolume(ADC1, step);
		}
	}
}

void audio_adc_digital_mute(audio_t *obj, BOOL mute_en)
{
	if (obj->audio_initialized == 1) {
		AUDIO_CODEC_SetADCMute(ADC0, mute_en);
		obj->adc_l_dmute_en = mute_en;
		AUDIO_CODEC_SetADCMute(ADC1, mute_en);
		obj->adc_r_dmute_en = mute_en;
	}
}

void audio_headphone_analog_mute(audio_t *obj, BOOL en)
{
	if (obj->audio_initialized == 1) {
		AUDIO_CODEC_SetDACMute(DAC_L, en);
	}
}

void audio_dac_digital_vol(audio_t *obj, u8 step)
{
	if (obj->audio_initialized == 1) {
		if (step > DVOL_DAC_0DB) {
			RTK_LOGE(TAG, " The dac step exceeds the maximum value 0xAF. \r\n");
		} else {
			AUDIO_CODEC_SetDACVolume(DAC_L, step);
		}
	}
}

void audio_dac_digital_mute(audio_t *obj, BOOL mute_en)
{
	if (obj->audio_initialized == 1) {
		AUDIO_CODEC_SetDACMute(DAC_L, mute_en);
		obj->dac_l_dmute_en = mute_en;
	}
}

void audio_mic_bias_ctrl(audio_t *obj, BOOL en)
{
	if (obj->audio_initialized == 1) {
		if (en) {
			AUDIO_CODEC_SetMicBiasPowerMode(AUD_POWER_ON);
		} else {
			AUDIO_CODEC_SetMicBiasPowerMode(AUD_POWER_DOWN);
		}
	}
}

void audio_adda_loopback(audio_t *obj, BOOL en)
{
	if (obj->audio_initialized == 1) {
		AUDIO_CODEC_SetADDALPBK(en);
	}
}

void audio_dmic_pinmux(audio_t *obj, PinName dmic_clk, PinName dmic_data)
{
	Pinmux_Config(dmic_clk, PINMUX_FUNCTION_DMIC);
	Pinmux_Config(dmic_data, PINMUX_FUNCTION_DMIC);
	dmic_clk_set = dmic_clk;
	dmic_data_set = dmic_data;
	(void)obj;
}

void audio_dmic_depinmux(audio_t *obj, PinName dmic_clk, PinName dmic_data)
{
	if (dmic_clk_set != 0) {
		Pinmux_Config(dmic_clk, PINMUX_FUNCTION_GPIO);
		dmic_clk_set = 0;
	}
	if (dmic_data_set != 0) {
		Pinmux_Config(dmic_data, PINMUX_FUNCTION_GPIO);
		dmic_data_set = 0;
	}
	(void)obj;
}

static void audio_dmic_poweroffcheck(void)
{
	if (dmic_clk_set != 0) {
		Pinmux_Config(dmic_clk_set, PINMUX_FUNCTION_GPIO);
		dmic_clk_set = 0;
	}
	if (dmic_data_set != 0) {
		Pinmux_Config(dmic_data_set, PINMUX_FUNCTION_GPIO);
		dmic_data_set = 0;
	}
}

void audio_l_dmic_gain(audio_t *obj, audio_dmic_gain dmic_gain)
{
	if (obj->audio_initialized == 1) {
		u8 gain_step = 0x2F + (dmic_gain * 8);
		if (gain_step > 0x7F) {
			gain_step = 0x7F;
		}
		AUDIO_CODEC_SetADCVolume(ADC0, gain_step);
	}
}

void audio_r_dmic_gain(audio_t *obj, audio_dmic_gain dmic_gain)
{
	if (obj->audio_initialized == 1) {
		u8 gain_step = 0x2F + (dmic_gain * 8);
		if (gain_step > 0x7F) {
			gain_step = 0x7F;
		}
		AUDIO_CODEC_SetADCVolume(ADC1, gain_step);
	}
}

void audio_set_diff_tx_rx_param(audio_t *obj, audio_diff_para_t *pdiff_para)
{
	if (obj->audio_initialized == 1 && pdiff_para != NULL) {
		obj->sp_init_tx.SP_SR = pdiff_para->tx_sr;
		obj->sp_init_tx.SP_SelWordLen = (pdiff_para->tx_wl == WL_16BIT) ? SP_TXWL_16 : SP_TXWL_24;
		obj->sp_init_tx.SP_SelI2SMonoStereo = (pdiff_para->tx_ch == A_MONO) ? SP_CH_MONO : SP_CH_STEREO;
		AUDIO_SP_Init(obj->sport_idx, SP_DIR_TX, &obj->sp_init_tx);

		obj->sp_init_rx.SP_SR = pdiff_para->rx_sr;
		obj->sp_init_rx.SP_SelWordLen = (pdiff_para->rx_wl == WL_16BIT) ? SP_RXWL_16 : SP_RXWL_24;
		obj->sp_init_rx.SP_SelI2SMonoStereo = (pdiff_para->rx_ch == A_MONO) ? SP_CH_MONO : SP_CH_STEREO;
		AUDIO_SP_Init(obj->sport_idx, SP_DIR_RX, &obj->sp_init_rx);

		obj->i2s_init.CODEC_SelI2STxSR = pdiff_para->tx_sr;
		obj->i2s_init.CODEC_SelI2STxWordLen = pdiff_para->tx_wl;
		obj->i2s_init.CODEC_SelI2SRxSR = pdiff_para->rx_sr;
		obj->i2s_init.CODEC_SelI2SRxWordLen = pdiff_para->rx_wl;
	}
}

void audio_adc_l_hpf(audio_t *obj, BOOL en, audio_hpf_fc hpf_fc)
{
	if (obj->audio_initialized == 1) {
		AUDIO_CODEC_SetADCHPF(ADC0, hpf_fc, en);
	}
}

void audio_adc_r_hpf(audio_t *obj, BOOL en, audio_hpf_fc hpf_fc)
{
	if (obj->audio_initialized == 1) {
		AUDIO_CODEC_SetADCHPF(ADC1, hpf_fc, en);
	}
}

void audio_input_l_eq(audio_t *obj, audio_eq eq, BOOL en, u32 h0, u32 b1, u32 b2, u32 a1, u32 a2)
{
	if (obj->audio_initialized == 1) {
		CODEC_EQFilterCoef coef;
		coef.H0_Q = h0;
		coef.B1_Q = b1;
		coef.B2_Q = b2;
		coef.A1_Q = a1;
		coef.A2_Q = a2;
		AUDIO_CODEC_SetADCEQClk(ADC0, ENABLE);
		AUDIO_CODEC_SetADCEQFilter(ADC0, eq, &coef);
		AUDIO_CODEC_SetADCEQBand(ADC0, eq, en);
	}
}

void audio_input_r_eq(audio_t *obj, audio_eq eq, BOOL en, u32 h0, u32 b1, u32 b2, u32 a1, u32 a2)
{
	if (obj->audio_initialized == 1) {
		CODEC_EQFilterCoef coef;
		coef.H0_Q = h0;
		coef.B1_Q = b1;
		coef.B2_Q = b2;
		coef.A1_Q = a1;
		coef.A2_Q = a2;
		AUDIO_CODEC_SetADCEQClk(ADC1, ENABLE);
		AUDIO_CODEC_SetADCEQFilter(ADC1, eq, &coef);
		AUDIO_CODEC_SetADCEQBand(ADC1, eq, en);
	}
}

void audio_output_l_eq(audio_t *obj, audio_eq eq, BOOL en, u32 h0, u32 b1, u32 b2, u32 a1, u32 a2)
{
	if (obj->audio_initialized == 1) {
		CODEC_EQFilterCoef coef;
		coef.H0_Q = h0;
		coef.B1_Q = b1;
		coef.B2_Q = b2;
		coef.A1_Q = a1;
		coef.A2_Q = a2;
		AUDIO_CODEC_SetDACEQClk(DAC_L, ENABLE);
		AUDIO_CODEC_SetDACEQFilter(DAC_L, eq, &coef);
		AUDIO_CODEC_SetDACEQBand(DAC_L, eq, en);
	}
}

uint32_t audio_get_tx_start_status(audio_t *obj)
{
	if (obj->audio_initialized == 1) {
		return obj->tx_started;
	}
	return 0;
}

uint32_t audio_get_rx_start_status(audio_t *obj)
{
	if (obj->audio_initialized == 1) {
		return obj->rx_started;
	}
	return 0;
}

void audio_poweroff(void)
{
	if (audio_obj && audio_obj->audio_initialized == 1) {
		audio_trx_stop(audio_obj);
		audio_deinit(audio_obj);
		audio_clean_error_cnt(audio_obj);

		audio_obj->audio_initialized = 0;
		audio_obj = NULL;
	}
	audio_dmic_poweroffcheck();
}
