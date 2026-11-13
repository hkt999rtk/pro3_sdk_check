/*
 * audio_vqe_test.c — 3rd-party VQE algorithm reference test.
 * NOT a production code path. This harness is selected manually from the
 * audio examples menu (entry "audio_vqe_test") and exists only to validate
 * the AEC/AGC/NS algorithms in isolation against a known input/output
 * reference.
 * For production audio pipelines see module_audio.c, which queues raw PCM
 * into pcm_rx_cache from the ISR and runs VQE on the dedicated
 * audio_rx_handle_thread.
 */

#include <stdint.h>
#include <stdio.h>
#include "audio_api.h"
#include "vfs.h"
#include "vfs_fatfs.h"
#include "ff.h"
#include "ASP.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

#define AUDIO_DMA_PAGE_NUM  AUDIO_PNUM_4
#define AUDIO_DMA_PAGE_SIZE (640)

#define TX_PAGE_SIZE    AUDIO_DMA_PAGE_SIZE //64*N bytes, max: 4095. 128, 4032
#define TX_PAGE_NUM     AUDIO_DMA_PAGE_NUM

#define RX_PAGE_SIZE    AUDIO_DMA_PAGE_SIZE //64*N bytes, max: 4095. 128, 4032
#define RX_PAGE_NUM     AUDIO_DMA_PAGE_NUM

#define FRAME_SIZE      (AUDIO_DMA_PAGE_SIZE/2)

//#define FRAME_LENGTH_MS(samplerate, wordlength)    ((AUDIO_DMA_PAGE_SIZE / wordlength) / (samplerate / 1000)) // 1 sample = wordlength(bytes)

// #define AUDIO_LOOPBACK

#if !defined(AUDIO_LOOPBACK)
#define PLAY_FROM_FILE
#endif

#define VQE_INPUT_FILE   "0:pink_noise.bin"
#define VQE_OUTPUT_FILE  "0:asp_rx.bin"

#define AEC_LOG_FLAG
#define CTASP_INFO_PREFIX   "[ASP Inf]"

#if defined(AEC_LOG_FLAG)
#define CTASP_INFO(...)                                         \
     do {                                                       \
             printf (CTASP_INFO_PREFIX""__VA_ARGS__);           \
     }while(0)
#else
#define CTASP_INFO(...)                                         \
     do {                                                       \
     }while(0)
#endif

typedef struct audio_ctx_s {
	audio_t *audio_obj ;

} paudio_ctx ;


static uint8_t _txdata[TX_PAGE_SIZE * TX_PAGE_NUM]__attribute__((aligned(CACHE_LINE_SIZE)));
static uint8_t _rxdata[RX_PAGE_SIZE * RX_PAGE_NUM]__attribute__((aligned(CACHE_LINE_SIZE)));

void rx_audio_handler(u32 ptr, u8 *rx_buf) ;
void tx_audio_handler(u32 ptr, u8 *rx_buf) ;


paudio_ctx audio_ctx ;
audio_t audio_obj ;

volatile int16_t audio_rx_state = 0, audio_tx_state = 0 ;
volatile uint32_t audio_rx_test_count = 0, audio_tx_test_count = 0;

#define AUDIO_TEST_DURATION  (30 * 50 * 1)

#if defined(PLAY_FROM_FILE)
FIL file_rd;
FIL file_wr;
static int file_rd_ok = 0;
static int file_wr_ok = 0;
void *fs ;
int16_t mic_write_buffer [AUDIO_TEST_DURATION * FRAME_SIZE], mic_write_offset = 0 ;
int16_t spk_read_buffer [AUDIO_TEST_DURATION * FRAME_SIZE], spk_read_offset = 0 ;
#endif

#if defined(AUDIO_LOOPBACK)
#define AUDIO_DELAY_LOOPBACK_TEST   5
int16_t audio_rdwr_buffer [2 * AUDIO_DELAY_LOOPBACK_TEST][FRAME_SIZE], rd_offset = 0, wr_offset = AUDIO_DELAY_LOOPBACK_TEST ;
#endif

#define VQE_JOB_Q_DEPTH   8
#define VQE_TASK_STACK    (24 * 1024)
#define VQE_TASK_PRIO     (tskIDLE_PRIORITY + 5)

typedef struct {
	int16_t mic[FRAME_SIZE];   /* near-end (mic) capture */
	int16_t ref[FRAME_SIZE];   /* far-end (AEC reference) */
} vqe_rx_job_t;

static QueueHandle_t  rx_job_q        = NULL;  /* RX ISR -> worker */
static QueueHandle_t  tx_ready_q      = NULL;  /* worker -> TX ISR (processed playout) */
static TaskHandle_t   vqe_worker      = NULL;
static volatile int   vqe_worker_run  = 0;
static volatile int   vqe_worker_done = 0;

static void vqe_worker_thread(void *param);


int16_t initialise_audio(int32_t driver_sample_rate)
{
	audio_ctx.audio_obj = &audio_obj ;

	CTAEC_cfg_t RX_AEC ;
	CTAGC_cfg_t RX_AGC ;
	CTNS_cfg_t RX_NS ;

	CTAGC_cfg_t TX_AGC ;
	CTNS_cfg_t TX_NS ;

	memset(&RX_NS, 0, sizeof(RX_NS)) ;
	memset(&RX_AEC, 0, sizeof(RX_AEC)) ;
	memset(&RX_AGC, 0, sizeof(RX_AGC)) ;

	memset(&TX_NS, 0, sizeof(TX_NS)) ;
	memset(&TX_AGC, 0, sizeof(TX_AGC)) ;

	RX_NS.NS_EN = 1;
	RX_NS.NSLevel = 10 ;
	RX_NS.HPFEnable = 0;

	RX_AGC.AGC_EN = 1;
	RX_AGC.AGCMode = CT_ALC;
	RX_AGC.ReferenceLvl = 0;
	RX_AGC.RatioFormat = 1;    // Ratio format: 0 => integer, range 1~50, 1 => 8.8 fix point, range 26~50*256 (mapping 26/256~50)
	RX_AGC.AttackTime = 10;
	RX_AGC.ReleaseTime = 50;

	RX_AGC. Ratio[0] = (int16_t)((50 * 256));
	RX_AGC. Ratio[1] = (int16_t)((50 * 256));
	RX_AGC. Ratio[2] = (int16_t)((50 * 256));
	RX_AGC.Threshold [0] = 39 ; // Threshold1, Threshold2, NoiseGateLvl
	RX_AGC.Threshold [1] = 70 ;
	RX_AGC.Threshold [2] = 80 ;

	RX_AGC.NoiseFloorAdaptEnable = 1;
	RX_AGC.RMSDetectorEnable = 1;
	RX_AGC.MaxGainLimit = 30;

	RX_AEC.AEC_EN =  1;
	RX_AEC.EchoTailLen = 60;
	RX_AEC.CNGEnable =  1;
	RX_AEC.PPLevel = 4;
	RX_AEC.DTControl = 1;

	TX_NS.NS_EN = 1;
	TX_NS.NSLevel = 10 ;

	TX_AGC.AGC_EN = 1;
	TX_AGC.AGCMode = CT_ALC;
	TX_AGC.ReferenceLvl = 0;
	TX_AGC.RatioFormat = 1;    // Ratio format: 0 => integer, range 1~50, 1 => 8.8 fix point, range 26~50*256 (mapping 26/256~50)
	TX_AGC.AttackTime = 10;
	TX_AGC.ReleaseTime = 50;

	TX_AGC. Ratio[0] = (int16_t)((50 * 256));
	TX_AGC. Ratio[1] = (int16_t)((50 * 256));
	TX_AGC. Ratio[2] = (int16_t)((50 * 256));
	TX_AGC.Threshold [0] = 39 ; // Threshold1, Threshold2, NoiseGateLvl
	TX_AGC.Threshold [1] = 70 ;
	TX_AGC.Threshold [2] = 80 ;
	TX_AGC.NoiseFloorAdaptEnable = 1;
	TX_AGC.RMSDetectorEnable = 1;
	TX_AGC.MaxGainLimit = 30;

#if defined(PLAY_FROM_FILE)
	memset(mic_write_buffer, 0,  AUDIO_TEST_DURATION * FRAME_SIZE * 2) ;
	mic_write_offset = 0 ;
	memset(spk_read_buffer, 0,  AUDIO_TEST_DURATION * FRAME_SIZE * 2) ;
	spk_read_offset = 0 ;

	int ret = vfs_target_register("fat", VFS_TARGET_SD, VFS_RW) ;

	CTASP_INFO("ret = %d\n", ret) ;

	UINT br;
	if (f_open(&file_rd, VQE_INPUT_FILE, FA_READ) == FR_OK) {
		file_rd_ok = 1;
		CTASP_INFO("playout from file: %s\n", VQE_INPUT_FILE) ;
	} else {
		CTASP_INFO("ERROR: failed to open input file: %s\n", VQE_INPUT_FILE) ;
	}
	if (file_rd_ok) {
		for (int count = 0 ; count < AUDIO_TEST_DURATION ; count ++) {
			f_read(&file_rd, &spk_read_buffer [count * FRAME_SIZE], AUDIO_DMA_PAGE_SIZE, &br);
			if (br < AUDIO_DMA_PAGE_SIZE) {
				f_lseek(&file_rd, 0);
				f_read(&file_rd, &spk_read_buffer [count * FRAME_SIZE], AUDIO_DMA_PAGE_SIZE, &br);
			}
		}
	}

	if (f_open(&file_wr, VQE_OUTPUT_FILE, FA_WRITE | FA_CREATE_ALWAYS) == FR_OK) {
		file_wr_ok = 1;
		CTASP_INFO("mic record file: %s\n", VQE_OUTPUT_FILE) ;
	} else {
		CTASP_INFO("ERROR: failed to open output file: %s\n", VQE_OUTPUT_FILE) ;
	}
	mic_write_offset = 0 ;
	spk_read_offset = 0 ;

#endif

	int32_t vqe_sample_hz;
	switch ((audio_sr)driver_sample_rate) {
	case ASR_8KHZ:
		vqe_sample_hz = 8000;
		break;
	case ASR_16KHZ:
		vqe_sample_hz = 16000;
		break;
	default:
		CTASP_INFO("Unsupported driver_sample_rate=%ld; defaulting to 16 kHz\n",
				   (long)driver_sample_rate);
		vqe_sample_hz = 16000;
		break;
	}

	VQE_SND_init(FRAME_SIZE, (int32_t)vqe_sample_hz, &RX_AEC, &RX_AGC, &RX_NS, NULL, NULL, NULL, 1.0, 1) ;

	VQE_RCV_NS_init((int32_t)vqe_sample_hz, &TX_NS) ;
	VQE_RCV_AGC_init((int32_t)vqe_sample_hz, &TX_AGC) ;

	audio_output_mode output_mode = OUTPUT_SINGLE_ENDED;
	audio_input_mode input_mode = LEFT_DMIC ;

	audio_init(&audio_obj, output_mode, input_mode) ;

	audio_set_param(&audio_obj, (audio_sr)driver_sample_rate, WL_16BIT) ;

	audio_set_dma_buffer(&audio_obj, (u8 *)_txdata, (u8 *)_rxdata, TX_PAGE_SIZE, AUDIO_DMA_PAGE_NUM);

	audio_l_dmic_gain(&audio_obj, DMIC_BOOST_24DB) ;

	audio_tx_irq_handler(&audio_obj, tx_audio_handler, &audio_ctx) ;

	audio_rx_irq_handler(&audio_obj, rx_audio_handler, &audio_ctx) ;

	/* set up the VQE worker task + buffers BEFORE the DMA starts */
	rx_job_q   = xQueueCreate(VQE_JOB_Q_DEPTH, sizeof(vqe_rx_job_t)) ;
	tx_ready_q = xQueueCreate(VQE_JOB_Q_DEPTH, sizeof(int16_t) * FRAME_SIZE) ;
	if (!rx_job_q || !tx_ready_q) {
		CTASP_INFO("VQE queue create fail\n") ;
		return -1 ;
	}

#if defined(PLAY_FROM_FILE)
	/* pre-fill a few processed playout frames so the first TX interrupts have data */
	for (int k = 0; k < VQE_JOB_Q_DEPTH / 2; k++) {
		int16_t txframe[FRAME_SIZE];
		memcpy(txframe, &spk_read_buffer [spk_read_offset * FRAME_SIZE], AUDIO_DMA_PAGE_SIZE) ;
		spk_read_offset = (spk_read_offset + 1) % AUDIO_TEST_DURATION ;
		VQE_RCV_NS_process(FRAME_SIZE, txframe) ;
		VQE_RCV_AGC_process(FRAME_SIZE, txframe) ;
		xQueueSend(tx_ready_q, txframe, 0) ;
	}
#endif

	vqe_worker_run = 1 ;
	if (xTaskCreate(vqe_worker_thread, "vqe_worker", VQE_TASK_STACK, NULL, VQE_TASK_PRIO, &vqe_worker) != pdPASS) {
		CTASP_INFO("VQE worker task create fail\n") ;
		return -1 ;
	}

	audio_tx_start(&audio_obj) ;
	audio_rx_start(&audio_obj) ;

	audio_rx_state = 1 ;
	audio_tx_state = 1 ;
	audio_rx_test_count = 0 ;
	audio_tx_test_count = 0 ;

	return 0 ;
}

int16_t close_rx_audio()
{
	if (vqe_worker_run) {
		vqe_worker_run = 0 ;
		while (!vqe_worker_done) {
			vTaskDelay(1) ;
		}
	}

#if defined(PLAY_FROM_FILE)
	UINT bw;
	CTASP_INFO("close_rx_audio()\n") ;
	if (file_wr_ok) {
		for (int count = 0 ; count < AUDIO_TEST_DURATION; count ++) {
			f_write(&file_wr, &mic_write_buffer [count * FRAME_SIZE], AUDIO_DMA_PAGE_SIZE, &bw);
			if (bw < AUDIO_DMA_PAGE_SIZE) {
				CTASP_INFO("error in file write: %d\n", bw) ;
			}
		}
		f_close(&file_wr);
		file_wr_ok = 0;
	}
#endif
	VQE_SND_destroy() ;

	CTASP_INFO("rx-audio VQE-test done()\n") ;

	return 0 ;
}

int16_t close_tx_audio()
{
#if defined(PLAY_FROM_FILE)
	CTASP_INFO("close_tx_audio()\n") ;
	if (file_rd_ok) {
		f_close(&file_rd) ;
		file_rd_ok = 0;
	}
#endif
	VQE_RCV_NS_destroy() ;
	VQE_RCV_AGC_destroy() ;

	audio_deinit(&audio_obj) ;

	if (rx_job_q) {
		vQueueDelete(rx_job_q) ;
		rx_job_q = NULL ;
	}
	if (tx_ready_q) {
		vQueueDelete(tx_ready_q) ;
		tx_ready_q = NULL ;
	}

	CTASP_INFO("tx-audio VQE-test done()\n") ;

	return 0 ;
}

void rx_audio_handler(u32 ctx, u8 *buf)
{
	paudio_ctx *audio_ctx = (paudio_ctx *)ctx;
	audio_t *audio_obj = (audio_t *)audio_ctx->audio_obj;
	BaseType_t hpw = pdFALSE;
	vqe_rx_job_t job;

	/* Grab the mic page and the far-end reference, then hand off to the worker task. */
	memcpy(job.mic, buf, 2 * FRAME_SIZE) ;
	audio_set_rx_page(audio_obj);

#if defined(PLAY_FROM_FILE)
	int16_t farend_buffer_offset = (spk_read_offset > 0) ? (spk_read_offset - 1) : (AUDIO_TEST_DURATION - 1) ;
	memcpy(job.ref, &spk_read_buffer [farend_buffer_offset * FRAME_SIZE], 2 * FRAME_SIZE) ;
#else
	memset(job.ref, 0, 2 * FRAME_SIZE) ;
#endif

	if (rx_job_q) {
		xQueueSendFromISR(rx_job_q, &job, &hpw) ;
	}

	if (audio_rx_test_count > AUDIO_TEST_DURATION) {
		audio_rx_state = 2 ;
	}
	audio_rx_test_count ++ ;

	portYIELD_FROM_ISR(hpw);
	return;
}


void tx_audio_handler(u32 ctx, u8 *buf)
{
	uint8_t *ptx_buf ;
	paudio_ctx *audio_ctx = (paudio_ctx *)ctx;
	audio_t *audio_obj = (audio_t *)audio_ctx->audio_obj;
	BaseType_t hpw = pdFALSE;
	int16_t txframe[FRAME_SIZE];

	ptx_buf = (uint8_t *)audio_get_tx_page_adr(audio_obj);
	if (!ptx_buf) {
		return;
	}

	/* Copy the next ready playout frame into the DMA page. */
	if (tx_ready_q && xQueueReceiveFromISR(tx_ready_q, txframe, &hpw) == pdTRUE) {
		memcpy(ptx_buf, txframe, 2 * FRAME_SIZE) ;
	} else {
		memset(ptx_buf, 0, 2 * FRAME_SIZE) ;   /* underrun -> silence */
	}

	audio_set_tx_page(audio_obj, (uint8_t *)ptx_buf);

	if (audio_tx_test_count > AUDIO_TEST_DURATION) {
		audio_tx_state = 2 ;
	}
	audio_tx_test_count ++ ;

	portYIELD_FROM_ISR(hpw);
	return;
}

static void vqe_worker_thread(void *param)
{
	(void)param;
	vqe_rx_job_t job;
	int16_t rxout[FRAME_SIZE];
	int16_t txframe[FRAME_SIZE];

	vqe_worker_done = 0;
	while (vqe_worker_run) {
		if (xQueueReceive(rx_job_q, &job, pdMS_TO_TICKS(50)) != pdTRUE) {
			continue;
		}

		/* RX path: AEC / NS / AGC on the mic, using the far-end reference. */
		VQE_SND_process(job.ref, job.mic, NULL, rxout) ;
#if defined(PLAY_FROM_FILE)
		memcpy(&mic_write_buffer [mic_write_offset * FRAME_SIZE], rxout, AUDIO_DMA_PAGE_SIZE) ;
		mic_write_offset = (mic_write_offset + 1) % AUDIO_TEST_DURATION ;

		/* TX path: NS / AGC on the next playout frame, queued for the TX ISR. */
		memcpy(txframe, &spk_read_buffer [spk_read_offset * FRAME_SIZE], AUDIO_DMA_PAGE_SIZE) ;
		spk_read_offset = (spk_read_offset + 1) % AUDIO_TEST_DURATION ;
		VQE_RCV_NS_process(FRAME_SIZE, txframe) ;
		VQE_RCV_AGC_process(FRAME_SIZE, txframe) ;
		if (tx_ready_q) {
			xQueueSend(tx_ready_q, txframe, 0) ;
		}
#endif
	}

	vqe_worker_done = 1;
	vqe_worker = NULL;
	vTaskDelete(NULL);
}

void audio_vqe_test()
{
	CTASP_INFO("audio VQE-test ()\n") ;
	ASP_print_version();
	initialise_audio(ASR_16KHZ) ;

	/* States: 0 = idle, 1 = running, 2 = stop-requested by ISR, 3 = done. */
	while (audio_rx_state != 3 || audio_tx_state != 3) {
		if (audio_rx_state == 2) {
			audio_rx_state = 3;
			CTASP_INFO("rxaudio_rx_test_count = %lu\n",
					   (unsigned long)audio_rx_test_count) ;
			audio_rx_stop(&audio_obj) ;
			close_rx_audio() ;
		}
		if (audio_tx_state == 2) {
			audio_tx_state = 3;
			CTASP_INFO("txaudio_tx_test_count = %lu\n",
					   (unsigned long)audio_tx_test_count) ;
			audio_tx_stop(&audio_obj) ;
			close_tx_audio() ;
		}
		vTaskDelay(10);
	}
}

MEDIA_APP_TABLE_SECTION
const AUDIO_APP_TABLE aapp_audio_vqe_test[] = {
	{
		.a_func = audio_vqe_test,
		.a_func_name = "audio_vqe_test",
	}
};
