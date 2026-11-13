/******************************************************************************
*
* Copyright(c) 2007 - 2025 Realtek Corporation. All rights reserved.
*
******************************************************************************/
#include "ameba_soc.h"
#include "FreeRTOS.h"
#include "task.h"
#include "platform_autoconf.h"

#if 0 /* RAMDISK_TODO */
#include "msc/inc/usbd_msc.h"
#include "fatfs_ramdisk_api.h"
#endif

#if SAVE_AUDIO_DATA
#include "audio_tool_command.h"
#endif
#include "audio_tool_init.h"
#include "audio_http_server.h"

#define AUDIO_TRANSFER_FUNC 0

#if AUDIO_TRANSFER_FUNC
void audio_data_transfer(void *param)
{
#if defined(configENABLE_TRUSTZONE) && (configENABLE_TRUSTZONE == 1)
	rtw_create_secure_context(configMINIMAL_SECURE_STACK_SIZE);
#endif
	char raw_path[64];
	char txt_path[64];
	uint8_t rBufferr;
	char tBufferr[10];
	FILE  *raw_record_file;
	FILE  *txt_record_file;
	char raw_record_filen[32] = "std1ktone_8kHz.raw";
	char txt_record_filen[32] = "std1ktone_8kHz.txt";
	int record_flag = 1;
	int total_read_count = 0;
	int total_write_count = 0;
	int br, bw;

	snprintf(raw_path, sizeof(raw_path), "audio_sd:/%s", raw_record_filen);
	printf("read file %s\r\n", raw_path);
	raw_record_file = fopen(raw_path, "r");
	if (!raw_record_file) {
		printf("Open SD file failed\r\n");
		goto exit;
	}
	snprintf(txt_path, sizeof(txt_path), "audio_sd:/%s", txt_record_filen);
	printf("txt file %s\r\n", txt_path);
	txt_record_file = fopen(txt_path, "w");
	if (!txt_record_file) {
		printf("Open SD file failed\r\n");
		fclose(raw_record_file);
		goto exit;
	}

	while (record_flag) {
		for (int i = 0; i < 160; i++) {

			br = fread(&rBufferr, 1, 1, raw_record_file);

			if (br <= 0) {
				printf("Read END\r\n");
				fclose(raw_record_file);
				fclose(txt_record_file);
				record_flag = 0;
				break;
			} else {
				int buffer_len = 0;
				total_read_count += br;
				memset(tBufferr, 0x0, 10);
				buffer_len = snprintf(tBufferr, 10, "0x%02x, ", rBufferr);
				if (i % 16 == 15) {
					printf("buffer_len = %d\r\n", buffer_len);
					tBufferr[buffer_len] = '\r';
					tBufferr[buffer_len + 1] = '\n';
					tBufferr[buffer_len + 2] = '\0';
				}
				bw = fwrite(tBufferr, 1, strlen(tBufferr), txt_record_file);

				if (bw <= 0) {
					printf("Write END\r\n");
					fclose(raw_record_file);
					fclose(txt_record_file);
					record_flag = 0;
					break;
				}
				total_write_count += bw;
			}
		}
		printf("total_read_count = %d, br = %d\r\n", total_read_count, br);
		printf("total_write_count = %d, bw = %d\r\n", total_write_count, bw);
		vTaskDelay(1);
	}
exit:
	vTaskDelete(NULL);
}
#endif

char file_name[20] = "test";
int audio_tftp_port = AUDIO_TFTP_HOST_PORT;
char audio_tftp_ip[16] = AUDIO_TFTP_HOST_IP_ADDR;
static FILE  *m_record_file_RX;
static FILE  *m_record_file_TX;
static FILE  *m_record_file_ASP;
static FILE  *m_record_file_TXASP;

#define RECORD_WRITE_BUF_SIZE  (32 * 1024)   /* 32 KB per buffer = ~50 frames */
#define REC_CH_RX   0
#define REC_CH_TX   1
#define REC_CH_ASP  2
#define REC_CH_TXASP 3
#define REC_NUM_CH  4

static FILE    *m_rec_file[REC_NUM_CH];       /* indexed by REC_CH_* */
static uint8_t *m_wbuf[REC_NUM_CH][2];        /* ping-pong buffers */
static uint32_t m_wbuf_idx[REC_NUM_CH];       /* fill index in active buffer */
static volatile int m_wbuf_active[REC_NUM_CH]; /* which buf audio is filling (0 or 1) */
static volatile int m_wbuf_flush[REC_NUM_CH];  /* which buf writer must flush (-1=none) */
static volatile int m_wbuf_flush_len[REC_NUM_CH];

static SemaphoreHandle_t m_writer_wake;
static TaskHandle_t m_writer_task_handle = NULL;
static char ram_record_file_RX[64];
static char ram_record_file_TX[64];
static char ram_record_file_ASP[64];
static char ram_record_file_TXASP[64];
static int recored_count = 0;

static xSemaphoreHandle  ram_dump_sd_sema = NULL;
static xSemaphoreHandle  ram_upload_tftp_sema = NULL;

#define MAX_TAG_LEN 32
#define MAXIMUM_FILE_SIZE 100
static char audio_disk_tag[MAX_TAG_LEN];
static uint8_t audio_disk_initialed = 0;

static void audio_sd_writer_task(void *param)
{
	while (1) {
		xSemaphoreTake(m_writer_wake, portMAX_DELAY);
		for (int ch = 0; ch < REC_NUM_CH; ch++) {
			int fi = m_wbuf_flush[ch];
			if (fi >= 0 && m_rec_file[ch]) {
				fwrite(m_wbuf[ch][fi], 1, m_wbuf_flush_len[ch], m_rec_file[ch]);
				m_wbuf_flush[ch] = -1;
			}
		}
		if (record_state == 0) {
			break;
		}
	}
	m_writer_task_handle = NULL;
	vTaskDelete(NULL);
}

static void rec_buf_push(int ch, const int16_t *data, uint32_t bytes)
{
	if (!m_wbuf[ch][0]) {
		return;    /* channel not active */
	}
	int active = m_wbuf_active[ch];

	/* If this write would overflow the active buffer, swap to the other one first. */
	if (m_wbuf_idx[ch] + bytes > RECORD_WRITE_BUF_SIZE) {
		int next = 1 - active;
		if (m_wbuf_flush[ch] < 0) {
			m_wbuf_flush_len[ch] = m_wbuf_idx[ch];
			m_wbuf_flush[ch]     = active;
			m_wbuf_active[ch]    = next;
			active               = next;
			m_wbuf_idx[ch]       = 0;
			xSemaphoreGive(m_writer_wake);
		} else {
			return;
		}
	}

	memcpy(m_wbuf[ch][active] + m_wbuf_idx[ch], data, bytes);
	m_wbuf_idx[ch] += bytes;
}

static void register_audio_disk_tag(const char *tag_name, int tag_len)
{
	// register the audio disk and http server for audio data dump
	httpd_register_audio_disk_tag(tag_name, tag_len);
	if (tag_len < MAX_TAG_LEN) {
		memset(audio_disk_tag, 0, MAX_TAG_LEN);
		snprintf(audio_disk_tag, MAX_TAG_LEN, "%s:/", tag_name);
		// memcpy(audio_disk_tag, tag_name, tag_len);
		printf("audio disk name %s\r\n", audio_disk_tag);
		audio_disk_initialed = 1;
	} else if (audio_disk_initialed) {
		printf("Invlaid tag_len %d >= %d. Use the previous disk name %s\r\n", tag_len, MAX_TAG_LEN, audio_disk_tag);
		audio_disk_initialed = 1;
	} else {
		printf("Invlaid tag_len %d >= %d.\r\n", tag_len, MAX_TAG_LEN);
		audio_disk_initialed = 0;
	}
}

static void audio_fatfs_drv_open(void)
{
#if 0 /* RAMDISK_TODO: SD mounted at boot */
	int res;
	vfs_init();

	res = vfs_target_register((const char *)audio_disk_tag, VFS_TARGET_RAM, VFS_RW);
	if (res < 0) {
		printf("fatfs_ram_init fail (%d)\n\r", res);
		return ;
	} else {
		printf("fatfs_ram_init open\n\r");
	}

	res = vfs_target_register("audio_sd", VFS_TARGET_SD, VFS_RW);
	if (res < 0) {
		printf("fatfs_sd_init fail (%d)\n\r", res);
		return ;
	} else {
		printf("fatfs_sd_init open\n\r");
	}
	return ;
#endif
}

static void audio_fatfs_drv_close(void)
{
#if 0 /* RAMDISK_TODO */
	vfs_target_unregister((const char *)audio_disk_tag);
	vfs_target_unregister("audio_sd");
	printf("fatfs_ram_init close\n\r");
	vfs_deinit();
#endif
}

/* channel → (record_type flag, filename suffix, file-name buffer) */
static const struct {
	int flag;
	const char *suffix;
	char *namebuf;
} ch_info[REC_NUM_CH] = {
	[REC_CH_RX]    = { RECORD_RX_DATA,   "RX",   ram_record_file_RX   },
	[REC_CH_TX]    = { RECORD_TX_DATA,   "TX",   ram_record_file_TX   },
	[REC_CH_ASP]   = { RECORD_ASP_DATA,  "ASP",  ram_record_file_ASP  },
	[REC_CH_TXASP] = { RECORD_TXASP_DATA, "TXASP", ram_record_file_TXASP},
};

static void audio_open_record_file(void)
{
	char path[128];

	WAVE_HEADER w_header;
	w_header.w_header.fccID[0] = 'R';
	w_header.w_header.fccID[1] = 'I';
	w_header.w_header.fccID[2] = 'F';
	w_header.w_header.fccID[3] = 'F';
	w_header.w_header.fccType[0] = 'W';
	w_header.w_header.fccType[1] = 'A';
	w_header.w_header.fccType[2] = 'V';
	w_header.w_header.fccType[3] = 'E';
	w_header.w_header.dwSize = record_frame_count * 2 * FRAME_LEN + 36;
	w_header.w_fmt.fccID[0] = 'f';
	w_header.w_fmt.fccID[1] = 'm';
	w_header.w_fmt.fccID[2] = 't';
	w_header.w_fmt.fccID[3] = ' ';
	w_header.w_fmt.dwSize = 16;
	w_header.w_fmt.wFormatTag = 1;
	w_header.w_fmt.wChannels = 1;
	w_header.w_fmt.dwSamplesPerSec = playing_sample_rate;
	w_header.w_fmt.dwAvgBytesPerSec = w_header.w_fmt.dwSamplesPerSec * 2;
	w_header.w_fmt.wBlockAlign = 2;
	w_header.w_fmt.uiBitsPerSample = 16;
	w_header.w_data.fccID[0] = 'd';
	w_header.w_data.fccID[1] = 'a';
	w_header.w_data.fccID[2] = 't';
	w_header.w_data.fccID[3] = 'a';
	w_header.w_data.dwSize = record_frame_count * 2 * FRAME_LEN;

	// printf("\n\r=== FATFS Example (RAM DISK) ===\n\r");
	// printf("\n\r=== RAM FS Read/Write test ===\n\r");

	/* Init writer state */
	for (int ch = 0; ch < REC_NUM_CH; ch++) {
		m_rec_file[ch]   = NULL;
		m_wbuf[ch][0]    = m_wbuf[ch][1] = NULL;
		m_wbuf_idx[ch]   = 0;
		m_wbuf_active[ch] = 0;
		m_wbuf_flush[ch] = -1;
		m_wbuf_flush_len[ch] = 0;
	}

	recored_count++;
	for (int ch = 0; ch < REC_NUM_CH; ch++) {
		if (!(record_type & ch_info[ch].flag)) {
			continue;
		}
		snprintf(ch_info[ch].namebuf, 63, "%s_%s%03d.wav", file_name, ch_info[ch].suffix, recored_count);
		printf("record file name: %s\n\r", ch_info[ch].namebuf);
		memset(path, 0, sizeof(path));
		snprintf(path, sizeof(path), "%s%s", audio_disk_tag, ch_info[ch].namebuf);
		printf("record file name: %s\n\r", path);

		m_rec_file[ch] = fopen(path, "w");
		printf("open record file name: %s\n\r", path);
		if (!m_rec_file[ch]) {
			printf("open file (%s) fail.\n\r", path);
			goto open_fail;
		}
		m_wbuf[ch][0] = (uint8_t *)malloc(RECORD_WRITE_BUF_SIZE);
		m_wbuf[ch][1] = (uint8_t *)malloc(RECORD_WRITE_BUF_SIZE);
		if (!m_wbuf[ch][0] || !m_wbuf[ch][1]) {
			printf("record buf malloc fail ch%d\n\r", ch);
			goto open_fail;
		}
		fwrite(&w_header, sizeof(WAVE_HEADER), 1, m_rec_file[ch]);
		printf("record file name: %s\n\n\r", path);
	}

	m_record_file_RX    = m_rec_file[REC_CH_RX];
	m_record_file_TX    = m_rec_file[REC_CH_TX];
	m_record_file_ASP   = m_rec_file[REC_CH_ASP];
	m_record_file_TXASP = m_rec_file[REC_CH_TXASP];

	/* Create semaphore and launch writer task */
	if (!m_writer_wake) {
		m_writer_wake = xSemaphoreCreateBinary();
	}
	if (!m_writer_task_handle) {
		xTaskCreate(audio_sd_writer_task, "rec_wr", 4096, NULL, tskIDLE_PRIORITY + 4, &m_writer_task_handle);
	}

	record_state = 2;
	return;

open_fail:
	for (int i = 0; i < REC_NUM_CH; i++) {
		if (m_wbuf[i][0]) {
			free(m_wbuf[i][0]);
			m_wbuf[i][0] = NULL;
		}
		if (m_wbuf[i][1]) {
			free(m_wbuf[i][1]);
			m_wbuf[i][1] = NULL;
		}
		if (m_rec_file[i]) {
			fclose(m_rec_file[i]);
			m_rec_file[i] = NULL;
		}
	}
	record_state = 0;
	return;
}

static void audio_close_record_file(void)
{
	if (m_writer_task_handle) {
		xSemaphoreGive(m_writer_wake);
		uint32_t t = xTaskGetTickCount();
		while (m_writer_task_handle && record_state == 0 &&
			   (xTaskGetTickCount() - t) < 10000) {
			vTaskDelay(10);
		}
	}

	for (int ch = 0; ch < REC_NUM_CH; ch++) {
		if (!m_rec_file[ch]) {
			continue;
		}
		int active = m_wbuf_active[ch];
		if (m_wbuf[ch][active] && m_wbuf_idx[ch] > 0) {
			fwrite(m_wbuf[ch][active], 1, m_wbuf_idx[ch], m_rec_file[ch]);
		}
		uint32_t flush_wait_t = xTaskGetTickCount();
		while (m_wbuf_flush[ch] >= 0 && (xTaskGetTickCount() - flush_wait_t) < 2000) {
			vTaskDelay(5);
		}
		if (m_wbuf[ch][0]) {
			free(m_wbuf[ch][0]);
			m_wbuf[ch][0] = NULL;
		}
		if (m_wbuf[ch][1]) {
			free(m_wbuf[ch][1]);
			m_wbuf[ch][1] = NULL;
		}

		int res = fclose(m_rec_file[ch]);
		if (res) {
			printf("close file (%s) fail.\n\r", ch_info[ch].namebuf);
		} else {
			printf("close file (%s) success.\n\r", ch_info[ch].namebuf);
		}
		m_rec_file[ch] = NULL;
	}

	if (audiocopy_status & SD_SAVE_EN) {
		xSemaphoreGive(ram_dump_sd_sema);
		audiocopy_status |= SD_SAVE_START;
	}
	if (audiocopy_status & TFTP_UPLOAD_EN) {
		xSemaphoreGive(ram_upload_tftp_sema);
		audiocopy_status |= TFTP_UPLOAD_START;
	}
	return;
}

static void audio_record_write_file_RX(int16_t *record)
{
	rec_buf_push(REC_CH_RX, record, 2 * FRAME_LEN);
	return;
}

static void audio_record_write_file_TX(int16_t *record)
{
	rec_buf_push(REC_CH_TX, record, 2 * FRAME_LEN);
	return;
}

static void audio_record_write_file_ASP(int16_t *record)
{
	rec_buf_push(REC_CH_ASP, record, 2 * FRAME_LEN);
	return;
}

static void audio_record_write_file_TXASP(int16_t *record)
{
	rec_buf_push(REC_CH_TXASP, record, 2 * FRAME_LEN);
	return;
}

static void audio_mic_record(int16_t *speaker_data_TX, int16_t *speaker_data_TXASP, int16_t *mic_data_RX, int16_t *mic_data_ASP)
{
	static int record_percent;
	if (record_state == 3) {
		record_state = 0;
		audio_close_record_file();
	} else if (record_state == 2) {
		if (frame_count > 0) {
			frame_count --;
			if (record_frame_count >= 100) {
				int pct = (record_frame_count - frame_count) * 100 / record_frame_count;
				if (pct > 100) {
					pct = 100;
				}
				if (pct >= record_percent + 1) {
					record_percent = pct;
					printf("*");
					if ((record_percent % 10) == 0) {
						printf(" %d%% is done!\n\r", record_percent);
					}
				}
			}
			if (record_type & RECORD_RX_DATA) {
				audio_record_write_file_RX(mic_data_RX);
			}
			if (record_type & RECORD_TX_DATA) {
				audio_record_write_file_TX(speaker_data_TX);
			}
			if (record_type & RECORD_ASP_DATA) {
				audio_record_write_file_ASP(mic_data_ASP);
			}
			if (record_type & RECORD_TXASP_DATA) {
				audio_record_write_file_TXASP(speaker_data_TXASP);
			}
		} else if (frame_count == 0) {
			record_state = 3;
		}
	} else if (record_state == 1) {
		audio_open_record_file();
		record_percent = 0;
	}
}

void audio_save_mass_storage_thread(void *param)
{
#if 0 /* RAMDISK_TODO */
	int status = 0;
	struct msc_opts *disk_operation = NULL;
#if defined(configENABLE_TRUSTZONE) && (configENABLE_TRUSTZONE == 1)
	rtw_create_secure_context(configMINIMAL_SECURE_STACK_SIZE);
#endif
	_usb_init();

	status = wait_usb_ready();
	if (status != USBD_INIT_OK) {
		if (status == USBD_NOT_ATTACHED) {
			printf("\r\n NO USB device attached\n");
		} else {
			printf("\r\n USB init fail\n");
		}
		goto exit;
	}

	disk_operation = malloc(sizeof(struct msc_opts));
	if (disk_operation == NULL) {
		printf("\r\n disk_operation malloc fail\n");
		goto exit;
	}

	disk_operation->disk_init = usb_ram_init;
	disk_operation->disk_deinit = usb_ram_deinit;
	disk_operation->disk_getcapacity = usb_ram_getcapacity;
	disk_operation->disk_read = usb_ram_readblocks;
	disk_operation->disk_write = usb_ram_writeblocks;

	// load usb mass storage driver
	status = usbd_msc_init(MSC_NBR_BUFHD, MSC_BUFLEN, disk_operation);

	if (status) {
		printf("USB MSC driver load fail.\n");
	} else {
		printf("USB MSC driver load done, Available heap [0x%x]\n", xPortGetFreeHeapSize());
	}

exit:
#endif
	vTaskDelete(NULL);
}

void audio_ram_sd_thread(void *param)
{
#if 0 /* RAMDISK_TODO */
#if defined(configENABLE_TRUSTZONE) && (configENABLE_TRUSTZONE == 1)
	rtw_create_secure_context(configMINIMAL_SECURE_STACK_SIZE);
#endif
	FILE  *m_ram_file;
	FILE  *m_sd_file;
	char ram_path[128];
	char sd_path[128];
	char ram_r_file[128];
	char sd_r_file[128];
	int br, bw;
	int16_t ramBufferr[RECORD_WORDS];
	int record_flag = 0;
	int total_read_count = 0;
	int total_write_count = 0;
	int transformcount = 0;
	int record_percent = 0;
	int type_select = 0;

	while (1) {
		if ((xSemaphoreTake(ram_dump_sd_sema, portMAX_DELAY) == pdTRUE) && (audiocopy_status & SD_SAVE_START)) {
			printf("Start trans data from ram to sd card\r\n");
			type_select = RECORD_MIN;

			while (type_select <= RECORD_MAX) {
				record_flag = 0;
				switch (type_select) {
				case RECORD_RX_DATA: {
					if (record_type & RECORD_RX_DATA) {
						record_flag = 1;
						//ram disk data
						snprintf(ram_r_file, 63, "%s_RX%03d.wav", file_name, recored_count);
						printf("RAM record file name: %s\n\r", ram_r_file);

						snprintf(ram_path, sizeof(ram_path), "%s%s", audio_disk_tag, ram_r_file);
						printf("RAM record file name: %s\n\r", ram_path);

						m_ram_file = fopen(ram_path, "r");
						if (!m_ram_file) {
							audiocopy_status &= ~SD_SAVE_START;
							printf("Open RAM file failed\r\n");
							goto open_file_fail;
						}

						//sd card data
						snprintf(sd_r_file, 63, "%s_RX%03d.wav", file_name, recored_count);
						printf("SD record file name: %s\n\r", sd_r_file);

						snprintf(sd_path, sizeof(sd_path), "audio_sd:/%s", sd_r_file);
						printf("SD record file name: %s\n\r", sd_path);

						m_sd_file = fopen(sd_path, "w");
						if (!m_sd_file) {
							printf("Open SD file failed\r\n");
							audiocopy_status &= ~SD_SAVE_START;
							fclose(m_ram_file);
							goto open_file_fail;
						}
					}
					break;
				}
				case RECORD_TX_DATA:
					if (record_type & RECORD_TX_DATA) {
						record_flag = 1;
						//ram disk data
						snprintf(ram_r_file, 63, "%s_TX%03d.wav", file_name, recored_count);
						printf("RAM record file name: %s\n\r", ram_r_file);

						snprintf(ram_path, sizeof(ram_path), "%s%s", audio_disk_tag, ram_r_file);
						printf("RAM record file name: %s\n\r", ram_path);

						m_ram_file = fopen(ram_path, "r");
						if (!m_ram_file) {
							audiocopy_status &= ~SD_SAVE_START;
							printf("Open RAM file failed\r\n");
							goto open_file_fail;
						}

						//sd card data
						snprintf(sd_r_file, 63, "%s_TX%03d.wav", file_name, recored_count);
						printf("SD record file name: %s\n\r", sd_r_file);

						snprintf(sd_path, sizeof(sd_path), "audio_sd:/%s", sd_r_file);
						printf("SD record file name: %s\n\r", sd_path);

						m_sd_file = fopen(sd_path, "w");
						if (!m_sd_file) {
							printf("Open SD file failed\r\n");
							audiocopy_status &= ~SD_SAVE_START;
							fclose(m_ram_file);
							goto open_file_fail;
						}
					}
					break;
				case RECORD_ASP_DATA:
					if (record_type & RECORD_ASP_DATA) {
						record_flag = 1;
						//ram disk data
						snprintf(ram_r_file, 63, "%s_ASP%03d.wav", file_name, recored_count);
						printf("RAM record file name: %s\n\r", ram_r_file);

						snprintf(ram_path, sizeof(ram_path), "%s%s", audio_disk_tag, ram_r_file);
						printf("RAM record file name: %s\n\r", ram_path);

						m_ram_file = fopen(ram_path, "r");
						if (!m_ram_file) {
							audiocopy_status &= ~SD_SAVE_START;
							printf("Open RAM file failed\r\n");
							goto open_file_fail;
						}

						//sd card data
						snprintf(sd_r_file, 63, "%s_ASP%03d.wav", file_name, recored_count);
						printf("SD record file name: %s\n\r", sd_r_file);

						snprintf(sd_path, sizeof(sd_path), "audio_sd:/%s", sd_r_file);
						printf("SD record file name: %s\n\r", sd_path);

						m_sd_file = fopen(sd_path, "w");
						if (!m_sd_file) {
							printf("Open SD file failed\r\n");
							audiocopy_status &= ~SD_SAVE_START;
							fclose(m_ram_file);
							goto open_file_fail;
						}
					}
					break;
				case RECORD_TXASP_DATA:
					if (record_type & RECORD_TXASP_DATA) {
						record_flag = 1;
						//ram disk data
						snprintf(ram_r_file, 63, "%s_TXASP%03d.wav", file_name, recored_count);
						printf("RAM record file name: %s\n\r", ram_r_file);

						snprintf(ram_path, sizeof(ram_path), "%s%s", audio_disk_tag, ram_r_file);
						printf("RAM record file name: %s\n\r", ram_path);

						m_ram_file = fopen(ram_path, "r");
						if (!m_ram_file) {
							audiocopy_status &= ~SD_SAVE_START;
							printf("Open RAM file failed\r\n");
							goto open_file_fail;
						}

						//sd card data
						snprintf(sd_r_file, 63, "%s_TXASP%03d.wav", file_name, recored_count);
						printf("SD record file name: %s\n\r", sd_r_file);

						snprintf(sd_path, sizeof(sd_path), "audio_sd:/%s", sd_r_file);
						printf("SD record file name: %s\n\r", sd_path);

						m_sd_file = fopen(sd_path, "w");
						if (!m_sd_file) {
							printf("Open SD file failed\r\n");
							audiocopy_status &= ~SD_SAVE_START;
							fclose(m_ram_file);
							goto open_file_fail;
						}
					}
					break;
				}

				total_write_count = 0;
				total_read_count = 0;
				transformcount = 0;
				record_percent = 0;
				printf("Transforming\r\n");
				//record loop
				while (record_flag) {
					if (((FRAME_LEN / RECORD_WORDS) * (record_frame_count)) >= 100) {
						if ((transformcount / (((FRAME_LEN / RECORD_WORDS) * (record_frame_count)) / 100)) >= (record_percent + 1)) {
							record_percent = (transformcount / (((FRAME_LEN / RECORD_WORDS) * (record_frame_count)) / 100));
							printf("*");
							if ((record_percent % 10) == 0) {
								printf(" %d%% is done!\n\r", record_percent);
							}
						}
					}
					transformcount ++;
					memset(ramBufferr, 0x0, RECORD_WORDS * 2);
					br = fread(ramBufferr, 1, RECORD_WORDS * 2, m_ram_file);
					if (br <= 0) {
						printf("\r\nRead END\r\n");
						printf("READ %d, WRITE: %d\r\n", total_read_count, total_write_count);
						fclose(m_ram_file);
						fclose(m_sd_file);
						record_flag = 0;
						break;
					} else {
						total_read_count += br;
						bw = fwrite(ramBufferr, 1, br, m_sd_file);

						if (bw <= 0) {
							printf("\r\nWrite END\r\n");
							printf("READ %d, WRITE: %d\r\n", total_read_count, total_write_count);
							fclose(m_ram_file);
							fclose(m_sd_file);
							record_flag = 0;
							break;
						}
						total_write_count += bw;
					}

					vTaskDelay(1);
				}
				type_select = type_select << 1;
			}
open_file_fail:
			printf("Start trans data from ram to sd card END\r\n");
			audiocopy_status &= ~SD_SAVE_START;
		}
	}
#endif
	vTaskDelete(NULL);
}

FILE  *tftp_ram_file;
char tftp_ram_path[128];
char tftp_ram_r_file[128];
void tftp_audio_send_handler(unsigned char *buffer, int *len, unsigned int index)
{
	int br;
	memset(buffer, 0x0, BLOCK_SIZE - 4);
	br = fread(buffer, 1, BLOCK_SIZE - 4, tftp_ram_file);
	if (br <= 0) {
		printf("\r\nRead ERROR\r\n");
		*len = 0;
	} else {
		*len = br;
	}
	vTaskDelay(30);
}

void audio_ram_tftp_thread(void *param)
{
#if 0 /* RAMDISK_TODO */
// #if defined(configENABLE_TRUSTZONE) && (configENABLE_TRUSTZONE == 1)
// 	rtw_create_secure_context(configMINIMAL_SECURE_STACK_SIZE);
// #endif
	int type_select = 0;
	tftp tftp_info;

	while (1) {
		if ((xSemaphoreTake(ram_upload_tftp_sema, portMAX_DELAY) == pdTRUE) && (audiocopy_status & TFTP_UPLOAD_START)) {
			printf("Start upload data by TFTP\r\n");
			type_select = RECORD_MIN;

			//setting tftp information
			tftp_info.send_handle = tftp_audio_send_handler;
			tftp_info.tftp_mode = AUDIO_TFTP_MODE;
			tftp_info.tftp_host = audio_tftp_ip;
			tftp_info.tftp_port = audio_tftp_port;
			tftp_info.tftp_op = WRQ;//FOR READ OPERATION
			tftp_info.tftp_retry_num = 5;
			tftp_info.tftp_timeout = 10;//second
			printf("tftp retry time = %d timeout = %d seconds\r\n", tftp_info.tftp_retry_num, tftp_info.tftp_timeout);

			while (type_select <= RECORD_MAX) {
				switch (type_select) {
				case RECORD_RX_DATA: {
					if (record_type & RECORD_RX_DATA) {
						//ram disk data
						snprintf(tftp_ram_r_file, 63, "%s_RX%03d.wav", file_name, recored_count);
						printf("RAM record file name: %s\n\r", tftp_ram_r_file);

						snprintf(tftp_ram_path, sizeof(tftp_ram_path), "%s%s", audio_disk_tag, tftp_ram_r_file);
						printf("RAM record file name: %s\n\r", tftp_ram_path);

						tftp_ram_file = fopen(tftp_ram_path, "r");
						if (!tftp_ram_file) {
							audiocopy_status &= ~TFTP_UPLOAD_START;
							printf("Open RAM file failed\r\n");
							goto open_file_fail;
						}

						tftp_info.tftp_file_name = tftp_ram_r_file;
						printf("TFTP File name = %s\r\n", tftp_info.tftp_file_name);
						if (tftp_client_start(&tftp_info) == 0) {
							printf("Send file successful\r\n");
						} else {
							printf("Send file fail\r\n");
						}
						fclose(tftp_ram_file);
					}
					break;
				}
				case RECORD_TX_DATA:
					if (record_type & RECORD_TX_DATA) {
						//ram disk data
						snprintf(tftp_ram_r_file, 63, "%s_TX%03d.wav", file_name, recored_count);
						printf("RAM record file name: %s\n\r", tftp_ram_r_file);

						snprintf(tftp_ram_path, sizeof(tftp_ram_path), "%s%s", audio_disk_tag, tftp_ram_r_file);
						printf("RAM record file name: %s\n\r", tftp_ram_path);

						tftp_info.tftp_file_name = tftp_ram_r_file;
						printf("TFTP File name = %s\r\n", tftp_info.tftp_file_name);

						tftp_ram_file = fopen(tftp_ram_path, "r");
						if (!tftp_ram_file) {
							audiocopy_status &= ~TFTP_UPLOAD_START;
							printf("Open RAM file failed\r\n");
							goto open_file_fail;
						}

						tftp_info.tftp_file_name = tftp_ram_r_file;
						printf("TFTP File name = %s\r\n", tftp_info.tftp_file_name);
						if (tftp_client_start(&tftp_info) == 0) {
							printf("Send file successful\r\n");
						} else {
							printf("Send file fail\r\n");
						}
						fclose(tftp_ram_file);
					}
					break;
				case RECORD_ASP_DATA:
					if (record_type & RECORD_ASP_DATA) {
						//ram disk data
						snprintf(tftp_ram_r_file, 63, "%s_ASP%03d.wav", file_name, recored_count);
						printf("RAM record file name: %s\n\r", tftp_ram_r_file);

						snprintf(tftp_ram_path, sizeof(tftp_ram_path), "%s%s", audio_disk_tag, tftp_ram_r_file);
						printf("RAM record file name: %s\n\r", tftp_ram_path);

						tftp_info.tftp_file_name = tftp_ram_r_file;
						printf("TFTP File name = %s\r\n", tftp_info.tftp_file_name);

						tftp_ram_file = fopen(tftp_ram_path, "r");
						if (!tftp_ram_file) {
							audiocopy_status &= ~TFTP_UPLOAD_START;
							printf("Open RAM file failed\r\n");
							goto open_file_fail;
						}

						tftp_info.tftp_file_name = tftp_ram_r_file;
						printf("TFTP File name = %s\r\n", tftp_info.tftp_file_name);
						if (tftp_client_start(&tftp_info) == 0) {
							printf("Send file successful\r\n");
						} else {
							printf("Send file fail\r\n");
						}
						fclose(tftp_ram_file);
					}
					break;
				case RECORD_TXASP_DATA:
					if (record_type & RECORD_TXASP_DATA) {
						//ram disk data
						snprintf(tftp_ram_r_file, 63, "%s_TXASP%03d.wav", file_name, recored_count);
						printf("RAM record file name: %s\n\r", tftp_ram_r_file);

						snprintf(tftp_ram_path, sizeof(tftp_ram_path), "%s%s", audio_disk_tag, tftp_ram_r_file);
						printf("RAM record file name: %s\n\r", tftp_ram_path);

						tftp_info.tftp_file_name = tftp_ram_r_file;
						printf("TFTP File name = %s\r\n", tftp_info.tftp_file_name);

						tftp_ram_file = fopen(tftp_ram_path, "r");
						if (!tftp_ram_file) {
							audiocopy_status &= ~TFTP_UPLOAD_START;
							printf("Open RAM file failed\r\n");
							goto open_file_fail;
						}

						tftp_info.tftp_file_name = tftp_ram_r_file;
						printf("TFTP File name = %s\r\n", tftp_info.tftp_file_name);
						if (tftp_client_start(&tftp_info) == 0) {
							printf("Send file successful\r\n");
						} else {
							printf("Send file fail\r\n");
						}
						fclose(tftp_ram_file);
					}
					break;
				}
				vTaskDelay(10);
				type_select = type_select << 1;
			}
open_file_fail:
			printf("Start upload data by TFTP END\r\n");
			audiocopy_status &= ~TFTP_UPLOAD_START;
		}
	}
	vTaskDelete(NULL);
#endif
}

void audio_tool_flow_init(void *param)
{
// #if defined(configENABLE_TRUSTZONE) && (configENABLE_TRUSTZONE == 1)
// 	rtw_create_secure_context(configMINIMAL_SECURE_STACK_SIZE);
// #endif
	// register the audio disk
	register_audio_disk_tag("sd", strlen("sd"));
	ram_dump_sd_sema = xSemaphoreCreateBinary();
	ram_upload_tftp_sema = xSemaphoreCreateBinary();

#if 0 /* RAMDISK_TODO: SD is already mounted at boot */
	audio_fatfs_drv_open();
#endif
	audio_save_ctx = mm_module_open(&audio_module);
	if (audio_save_ctx) {
		mm_module_ctrl(audio_save_ctx, CMD_AUDIO_SET_PARAMS, (int)&audio_save_params);
		mm_module_ctrl(audio_save_ctx, CMD_AUDIO_SET_TXASP_PARAM, (int)&tx_asp_params);
		mm_module_ctrl(audio_save_ctx, CMD_AUDIO_SET_RXASP_PARAM, (int)&rx_asp_params);
		mm_module_ctrl(audio_save_ctx, CMD_AUDIO_RUN_AEC, 1);
		mm_module_ctrl(audio_save_ctx, CMD_AUDIO_RUN_AGC, AUDIO_TX_MASK | AUDIO_RX_MASK);
		mm_module_ctrl(audio_save_ctx, CMD_AUDIO_RUN_NS, AUDIO_TX_MASK | AUDIO_RX_MASK);
		mm_module_ctrl(audio_save_ctx, MM_CMD_SET_QUEUE_LEN, 800);
		mm_module_ctrl(audio_save_ctx, MM_CMD_INIT_QUEUE_ITEMS, MMQI_FLAG_STATIC);
		mm_module_ctrl(audio_save_ctx, CMD_AUDIO_APPLY, 0);
		mm_module_ctrl(audio_save_ctx, CMD_AUDIO_SET_MIC_RECORD_FUN, (int)(audio_mic_record));
	} else {
		printf("audio open fail\n\r");
		goto audio_save_init_fail;
	}

	afft_test_ctx = mm_module_open(&afft_module);
	if (afft_test_ctx) {
		mm_module_ctrl(afft_test_ctx, CMD_AFFT_SET_PARAMS, (int)&afft_test_params);
		mm_module_ctrl(afft_test_ctx, CMD_AFFT_SET_OUTPUT, 1);
		mm_module_ctrl(afft_test_ctx, MM_CMD_SET_QUEUE_LEN, 6);
		mm_module_ctrl(afft_test_ctx, MM_CMD_INIT_QUEUE_ITEMS, MMQI_FLAG_STATIC);
		mm_module_ctrl(afft_test_ctx, CMD_AFFT_APPLY, 0);
		mm_module_ctrl(afft_test_ctx, CMD_AFFT_SHOWN, 0);
	} else {
		printf("AFFT open fail\n\r");
		goto audio_save_init_fail;
	}

	printf("AFFT MODULE opened\n\r");

	pcm_tone_ctx = mm_module_open(&tone_module);
	if (pcm_tone_ctx) {
		mm_module_ctrl(pcm_tone_ctx, CMD_TONE_SET_PARAMS, (int)&pcm_tone_params);
		mm_module_ctrl(pcm_tone_ctx, MM_CMD_SET_QUEUE_LEN, 6);
		mm_module_ctrl(pcm_tone_ctx, MM_CMD_INIT_QUEUE_ITEMS, MMQI_FLAG_STATIC);
		mm_module_ctrl(pcm_tone_ctx, CMD_TONE_APPLY, 0);
		//mm_module_ctrl(pcm_tone_ctx, CMD_TONE_STREAMING, 1);  // streamming on
	} else {
		printf("TONE open fail\n\r");
		goto audio_save_init_fail;
	}

	array_t array = {0};
	array.data_addr = (uint32_t) music_sr16k;
	array.data_len = (uint32_t) music_sr16k_pcm_len;
	array_pcm_ctx = mm_module_open(&array_module);
	if (array_pcm_ctx) {
		mm_module_ctrl(array_pcm_ctx, CMD_ARRAY_SET_PARAMS, (int)&pcm16k_array_params);
		mm_module_ctrl(array_pcm_ctx, CMD_ARRAY_SET_ARRAY, (int)&array);
		mm_module_ctrl(array_pcm_ctx, MM_CMD_SET_QUEUE_LEN, 6);
		mm_module_ctrl(array_pcm_ctx, MM_CMD_INIT_QUEUE_ITEMS, MMQI_FLAG_DYNAMIC);
		mm_module_ctrl(array_pcm_ctx, CMD_ARRAY_APPLY, 0);
		//mm_module_ctrl(array_pcm_ctx, CMD_ARRAY_STREAMING, 1);    // streamming on
	} else {
		printf("ARRAY open fail\n\r");
		goto audio_save_init_fail;
	}

#if 0 /* SKYNET_TODO */
	p2p_audio_ctx = mm_module_open(&p2p_audio_module);
	if (p2p_audio_ctx) {
		mm_module_ctrl(p2p_audio_ctx, CMD_P2P_AUDIO_SET_PARAMS, (int)&p2p_audio_params);
		mm_module_ctrl(p2p_audio_ctx, CMD_P2P_AUDIO_APPLY, 1);
		mm_module_ctrl(p2p_audio_ctx, CMD_P2P_AUDIO_STREAMING, 0);
	} else {
		printf("P2P AUDIO open fail\n\r");
		goto audio_save_init_fail;
	}
#else
	null_save_ctx = mm_module_open(&null_module);
	if (null_save_ctx) {
		mm_module_ctrl(null_save_ctx, MM_CMD_SET_QUEUE_LEN, 6);
		mm_module_ctrl(null_save_ctx, MM_CMD_INIT_QUEUE_ITEMS, MMQI_FLAG_STATIC);
		mm_module_ctrl(null_save_ctx, CMD_NULL_SET_APPLY, 0);
	} else {
		printf("null sink open fail\n\r");
		goto audio_save_init_fail;
	}
#endif

	printf("\r\n   siso_audio_afft create  \r\n");
	siso_audio_afft = siso_create();
	if (siso_audio_afft) {
		siso_ctrl(siso_audio_afft, MMIC_CMD_ADD_INPUT, (uint32_t)audio_save_ctx, 0);
		siso_ctrl(siso_audio_afft, MMIC_CMD_ADD_OUTPUT, (uint32_t)afft_test_ctx, 0);
		siso_ctrl(siso_audio_afft, MMIC_CMD_SET_TASKPRIORITY, 2, 0);
		printf("siso_start siso_audio_afft\n\r");
	} else {
		printf("siso2 open fail\n\r");
		goto audio_save_init_fail;
	}

	printf("\r\n   mimo_array_audio create  \r\n");
	mimo_aarray_audio = mimo_create();
	if (mimo_aarray_audio) {
		mimo_ctrl(mimo_aarray_audio, MMIC_CMD_ADD_INPUT0, (uint32_t)afft_test_ctx, 0);
		mimo_ctrl(mimo_aarray_audio, MMIC_CMD_ADD_INPUT1, (uint32_t)array_pcm_ctx, 0);
		mimo_ctrl(mimo_aarray_audio, MMIC_CMD_ADD_INPUT2, (uint32_t)pcm_tone_ctx, 0);
#if 0 /* SKYNET_TODO: restore p2p_audio_ctx as OUTPUT0 when Skynet is available */
		mimo_ctrl(mimo_aarray_audio, MMIC_CMD_ADD_OUTPUT0, (uint32_t)p2p_audio_ctx, MMIC_DEP_INPUT0);//for audio record and streaming
#else
		mimo_ctrl(mimo_aarray_audio, MMIC_CMD_ADD_OUTPUT0, (uint32_t)null_save_ctx,  MMIC_DEP_INPUT0);//drain afft
#endif
		mimo_ctrl(mimo_aarray_audio, MMIC_CMD_ADD_OUTPUT1, (uint32_t)audio_save_ctx, MMIC_DEP_INPUT0);//for audio playback
		mimo_ctrl(mimo_aarray_audio, MMIC_CMD_ADD_OUTPUT2, (uint32_t)audio_save_ctx, MMIC_DEP_INPUT1);//for audio playmusic
		mimo_ctrl(mimo_aarray_audio, MMIC_CMD_ADD_OUTPUT3, (uint32_t)audio_save_ctx, MMIC_DEP_INPUT2);//for audio playtone
		mimo_ctrl(mimo_aarray_audio, MMIC_CMD_SET_TASKPRIORITY, 2, 0);
		mimo_start(mimo_aarray_audio);
		printf("mimo_start mimo_aarray_audio\n\r");
		mimo_pause(mimo_aarray_audio, MM_OUTPUT1 | MM_OUTPUT2 | MM_OUTPUT3); //disable audio playback and playtone
		printf("mimo_pause mimo_aarray_audio MM_OUTPUT1, MM_OUTPUT2 and MM_OUTPUT3\n\r");
	} else {
		printf("siso2 open fail\n\r");
		goto audio_save_init_fail;
	}
	// start MIC data streaming loopback to Speaker to prevent the first sound after initialing audio test tool
	siso_start(siso_audio_afft);
	printf("audio_set_done~~~~~\r\n");

#if 0 /* RAMDISK_TODO */
	if (xTaskCreate(audio_save_mass_storage_thread, ((const char *)"mass_stor"), 2048, NULL, tskIDLE_PRIORITY + 5, NULL) != pdPASS) {
		printf("\r\n audio_save_mass_storage_thread: Create Task Error\n");
	}
#endif

#if 0 /* RAMDISK_TODO */
	if (xTaskCreate(audio_ram_sd_thread, ((const char *)"ram_sd"), 1024 * 8, NULL, tskIDLE_PRIORITY + 5, NULL) != pdPASS) {
		printf("\r\n audio_ram_sd_thread: Create Task Error\n");
	}

	if (xTaskCreate(audio_ram_tftp_thread, ((const char *)"ram_tftp"), 1024 * 8, NULL, tskIDLE_PRIORITY + 5, NULL) != pdPASS) {
		printf("\r\n audio_ram_tftp_thread: Create Task Error\n");
	}
#endif

	// if opening http already please comment this and see audio_http_server.h
	audio_httpd_create();

#if AUDIO_TRANSFER_FUNC
	if (xTaskCreate(audio_data_transfer, ((const char *)"au_trans"), 1024 * 8, NULL, tskIDLE_PRIORITY + 1, NULL) != pdPASS) {
		printf("\r\n audio_data_transfer: Create Task Error\n");
	}
#endif
	vTaskDelay(5000);
#if 0 /* SKYNET_TODO */
#if P2P_ENABLE
	//skynet_device_run();
#endif
#endif
	while (1) {
		vTaskDelay(1000);
	}

audio_save_init_fail:
	printf("MODULE OPEN FAILED\r\n");
	vTaskDelete(NULL);
}

void audio_tool_init(void)
{
	/*user can start their own task here*/
	if (xTaskCreate(audio_tool_flow_init, ((const char *)"au_test"), 1024 * 16, NULL, tskIDLE_PRIORITY + 1, NULL) != pdPASS) {
		printf("\r\n audio_tool_flow_init: Create Task Error\n");
	}
}

MEDIA_APP_TABLE_SECTION
const AUDIO_APP_TABLE aapp_audio_test_tool[] = {
	{
		.a_func = audio_tool_init,
		.a_func_name = "audio_test_tool",
	}
};
