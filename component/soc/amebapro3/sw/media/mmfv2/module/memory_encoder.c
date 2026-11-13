/******************************************************************************
*
* Copyright(c) 2007 - 2025 Realtek Corporation. All rights reserved.
*
******************************************************************************/
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

#include "memory_encoder.h"

#define NONBLOCK_ALLOC 1
//#define DEBUG_ON 1

#if defined(DEBUG_ON) && (DEBUG_ON==1)
#define MEMDBG(...) printf(__VA_ARGS__)
#else
#define MEMDBG(...)
#endif

#define USING_TAIL_SPACE

void *memory_init(int size, int block_size)
{
	struct encoder_buffer *eb = (struct encoder_buffer *)malloc(sizeof(struct encoder_buffer));
	if (!eb) {
		return NULL;
	}
	memset(eb, 0, sizeof(struct encoder_buffer));

	eb->mem_size = size;
	eb->blk_size = block_size;
	eb->pool = (uint8_t *)malloc(size);
	if (!eb->pool) {
		goto memory_init_fail;
	}

#if defined(USING_TAIL_SPACE)
	eb->tail_blk_idx = -1;
#endif

	eb->max_pool_blk = size / block_size;
	eb->min_rest_blk = eb->rest_pool_blk = eb->max_pool_blk;

	eb->pool_table = (uint8_t *)malloc(eb->max_pool_blk);
	if (!eb->pool_table) {
		goto memory_init_fail;
	}
	memset(eb->pool_table, 0, eb->max_pool_blk);

	eb->blk_serial = 1; // init value

	eb->memory_lock = xSemaphoreCreateMutex();

	MEMDBG("pool size %x, block size %x inited\n\r", size, block_size);
	return eb;
memory_init_fail:
	if (eb->pool_table) {
		free(eb->pool_table);
	}
	if (eb->pool) {
		free(eb->pool);
	}
	if (eb) {
		free(eb);
	}
	return NULL;
}

void memory_deinit(void *ctx)
{
	struct encoder_buffer *eb = (struct encoder_buffer *)ctx;
	if (eb == NULL) {
		return;
	}
	vSemaphoreDelete(eb->memory_lock);
	if (eb->pool_table) {
		free(eb->pool_table);
	}
	if (eb->pool) {
		free(eb->pool);
	}
	if (eb) {
		free(eb);
	}
}

int addr2poolblkidx(void *ctx, void *addr)
{
	struct encoder_buffer *eb = (struct encoder_buffer *)ctx;
	uint32_t curr = (uint32_t)addr;
	uint32_t start = (uint32_t)eb->pool;

	return (curr - start) / eb->blk_size;
}

void *poolblkidx2addr(void *ctx, int idx)
{
	struct encoder_buffer *eb = (struct encoder_buffer *)ctx;
	return (void *)(eb->pool + idx * eb->blk_size);
}

void *memory_alloc(void *ctx, uint32_t size)
{
	struct encoder_buffer *eb = (struct encoder_buffer *)ctx;

	int blksize = (size + eb->blk_size - 1) / eb->blk_size;
	int not_avail = 0;
	int i;
	void *ret_addr = NULL;
	do {
		xSemaphoreTake(eb->memory_lock, portMAX_DELAY);

		if (eb->pool_blk_idx + blksize > eb->max_pool_blk) {
#if defined(USING_TAIL_SPACE)
			//printf("!");
			eb->tail_blk_idx = eb->pool_blk_idx;
#endif
			eb->pool_blk_idx = 0;
		}
		not_avail = 0;
		for (i = eb->pool_blk_idx; i < eb->pool_blk_idx + blksize; i++) {
			not_avail += eb->pool_table[i];
		}

		// may have enough space to alloc
		if (not_avail && eb->rest_pool_blk > blksize) {
			int pool_blk_idx = 0;
			//	dump_info((void*)eb);
			//  rt_printf("scan again, size = %d\n\r", size);
			while (pool_blk_idx + blksize < eb->max_pool_blk) {
				int avail_cnt = 0;
				for (i = pool_blk_idx; i < pool_blk_idx + blksize; i++) {
					if (eb->pool_table[i] != 0) {
						pool_blk_idx = i + 1;
						break;
					} else {
						avail_cnt++;
					}
				}
				if (avail_cnt == blksize) {
					eb->pool_blk_idx = pool_blk_idx;
					not_avail = 0;

					//	rt_printf("found space @ %d\n\r", eb->pool_blk_idx);
					break;
				}
			}
		}


		if (!not_avail) {
			for (i = eb->pool_blk_idx; i < eb->pool_blk_idx + blksize; i++) {
				eb->pool_table[i] = eb->blk_serial;
			}
			eb->blk_serial++;
			if (eb->blk_serial > 255) {
				eb->blk_serial = 1;    // reset block serial
			}

			ret_addr = poolblkidx2addr(eb, eb->pool_blk_idx);
			eb->pool_blk_idx += blksize;

			eb->rest_pool_blk -= blksize;

			if (eb->rest_pool_blk < eb->min_rest_blk) {
				eb->min_rest_blk = eb->rest_pool_blk;
			}

			xSemaphoreGive(eb->memory_lock);
			break;
		}

		xSemaphoreGive(eb->memory_lock);
#if defined(NONBLOCK_ALLOC) && (NONBLOCK_ALLOC==1)
	} while (0);
#else
		// wait 10ms, release cpu from this task
		rtw_msleep_os(10);
		MEMDBG("block wait %x\n\r", size);
		//dump_info((void*)eb);
	}
	while (not_avail);
#endif

	return ret_addr;
}

// get current allocating block count
uint32_t get_current_block_counts(struct encoder_buffer *eb, int idx)
{
	int curr_serial = eb->pool_table[idx];
	int cnt = 0;

	for (int i = idx; i < eb->max_pool_blk; i++) {
		if (eb->pool_table[i] == curr_serial) {
			cnt++;
		} else {
			break;
		}
	}

	return cnt;
}

uint8_t *memory_realloc(void *ctx, uint8_t *addr, uint32_t size)
{
	struct encoder_buffer *eb = (struct encoder_buffer *)ctx;
	int blkidx, old_blkidx, new_blkidx;

	if (!addr) {
		return NULL;
	}

	blkidx = addr2poolblkidx(eb, (void *)addr);

#if defined(USING_TAIL_SPACE)
	// using memcpy to fill tail space
	if ((blkidx == 0) && (eb->tail_blk_idx != -1)) {
		// alloc from tail space
		int blksize = (size + eb->blk_size - 1) / eb->blk_size;
		void *tail_addr = NULL;

		xSemaphoreTake(eb->memory_lock, portMAX_DELAY);
		if (eb->tail_blk_idx + blksize <= eb->max_pool_blk) {
			// set empty block to active
			int not_avail = 0;
			for (int i = eb->tail_blk_idx; i < eb->tail_blk_idx + blksize; i++) {
				not_avail += eb->pool_table[i];
			}

			// may have enough space to alloc
			if (not_avail == 0) {
				//printf(".");
				// reserve the space
				for (int i = eb->tail_blk_idx; i < eb->tail_blk_idx + blksize; i++) {
					eb->pool_table[i] = eb->pool_table[0];
				}

				tail_addr = poolblkidx2addr(eb, eb->tail_blk_idx);
				eb->tail_blk_idx += blksize;
				eb->rest_pool_blk -= blksize;
				// copy data to tail
				memcpy(tail_addr, addr, size);

				// free current space
				int blkcount = get_current_block_counts(eb, 0);
				for (int i = 0; i < blkcount; i++) {
					eb->pool_table[i] = 0;
				}
				eb->pool_blk_idx = 0;

				eb->rest_pool_blk += blkcount;

				if (eb->rest_pool_blk < eb->min_rest_blk) {
					eb->min_rest_blk = eb->rest_pool_blk;
				}

				xSemaphoreGive(eb->memory_lock);

				return tail_addr;
			} else {
				//printf("N");
				eb->tail_blk_idx = -1;
			}
		} else {
			//printf("o");
			eb->tail_blk_idx = -1;
		}

		xSemaphoreGive(eb->memory_lock);
	}
#endif

	// if size is bigger than allocated?? a fatal condition
	old_blkidx = blkidx + get_current_block_counts(eb, blkidx);
	new_blkidx = blkidx + (size + eb->blk_size - 1) / eb->blk_size;

	// at least one block
	//if(new_blkidx == blkidx)
	//    new_blkidx++;

	xSemaphoreTake(eb->memory_lock, portMAX_DELAY);
	for (int i = new_blkidx; i < old_blkidx; i++) {
		eb->pool_table[i] = 0;
	}
	// realloc happen @ last allocating space
	if (old_blkidx == eb->pool_blk_idx) {
		eb->pool_blk_idx = new_blkidx;
	}
	eb->rest_pool_blk += (old_blkidx - new_blkidx);
	xSemaphoreGive(eb->memory_lock);

	// free memory case
	if (new_blkidx == blkidx) {
		return NULL;
	} else {
		return addr;
	}
}

void memory_free(void *ctx, uint8_t *addr)
{
	struct encoder_buffer *eb = (struct encoder_buffer *)ctx;

	if (!addr) {
		return;
	}

	int blkidx = addr2poolblkidx(ctx, addr);
	int blkcount;

	xSemaphoreTake(eb->memory_lock, portMAX_DELAY);

	blkcount = get_current_block_counts(eb, blkidx);
	for (int i = blkidx; i < blkidx + blkcount; i++) {
		eb->pool_table[i] = 0;
	}

	// trace back free space
#if 0
	int idx = eb->pool_blk_idx;
	while (eb->pool_table[idx] == 0) {
		idx -= 1;
		if (idx < 0)	{
			idx += eb->max_pool_blk;
		}
	}
	// talbe[idx] != 0, blk_idx indicate to first empty block
	eb->pool_blk_idx = idx + 1;
#endif

	// merge if blkidx + blkcount == pool_blk_idx
	if (blkidx + blkcount == eb->pool_blk_idx) {
		eb->pool_blk_idx = blkidx;
	}

	eb->rest_pool_blk += blkcount;
	xSemaphoreGive(eb->memory_lock);
}

void dump_pool_table(uint8_t *table, int max)
{
	printf("pool table:\n\r");
	for (int i = 0; i < max; i += 16) {
		printf("%05x:\t", i);
		for (int j = 0; j < 16; j++) {
			printf("%02x ", table[i + j]);
		}
		printf("\n\r");
	}
}

void dump_info(void *ctx)
{
	struct encoder_buffer *eb = (struct encoder_buffer *)ctx;
	printf("========================\n\r");
	dump_pool_table(eb->pool_table, eb->max_pool_blk);
	printf("pool  address = \t%p \r\n", eb->pool);
	printf("talbe address = \t%p \r\n", eb->pool_table);
	printf("max pool block = \t%lu \r\n", eb->max_pool_blk);
	printf("curr block idx = \t%lu \r\n", eb->pool_blk_idx);
	printf("rest pool block = \t%lu \r\n", eb->rest_pool_blk);
	printf("block serial   = \t%lu \r\n", eb->blk_serial);
	printf("min rest block = \t%lu \r\n", eb->min_rest_blk);
	printf("block size     = \t%lu \r\n", eb->blk_size);
	printf("pool size      = \t%lu \r\n", eb->mem_size);
	printf("========================\n\r");
}

#define TEST 1

#if TEST

#define SHARE_POOL 1

void *eb1;
void *eb2;


QueueHandle_t 		src1_qid;
QueueHandle_t 		src2_qid;

int video_len[4] = {100 * 1024 + 35, 2025, 3000, 123 * 1024 - 555};
void test001(void *dummy)
{
	(void)dummy;
	int size;
	int cnt = 0;
	while (1) {
		// simulate generate video stream
		uint8_t *video = (uint8_t *)memory_alloc(eb1, 200 * 1024);

		vTaskDelay(30);

		size = video_len[cnt & 3] + cnt % 1024;
		cnt++;
		video = (uint8_t *)memory_realloc(eb1, video, size);
		taskENTER_CRITICAL();
		printf("001 video %lx\n\r", (uint32_t)video);
		taskEXIT_CRITICAL();
		xQueueSend(src1_qid, (void *)&video, 0xFFFFFFFF);
	}
}

int audio_len[4] = {2020, 2025, 1234, 3000};
void test002(void *dummy)
{
	(void)dummy;
	void *eb;
	int size;
	int cnt = 0;

#if SHARE_POOL==1
	eb = eb1;
#else
	eb = eb2;
#endif

	while (1) {
		// simulate generate audio stream
		uint8_t *audio = (uint8_t *)memory_alloc(eb, 20 * 1024);

		vTaskDelay(100);

		size = audio_len[cnt & 3] + cnt % 1024;
		cnt++;
		audio = (uint8_t *)memory_realloc(eb, audio, size);
		taskENTER_CRITICAL();
		printf("002 audio %lx\n\r", (uint32_t)audio);
		taskEXIT_CRITICAL();
		xQueueSend(src2_qid, (void *)&audio, 0xFFFFFFFF);
	}
}

void test003(void *dummy)
{
	(void)dummy;
	int cnt = 0;
	uint8_t *video = NULL, *audio = NULL;
	// simulate consume audio/vidoe stream
	while (1) {
#if SHARE_POOL==1
		int v_ready = 0, a_ready = 0;
		v_ready = xQueueReceive(src1_qid, (void *)&video, 0) == pdTRUE;
		a_ready = xQueueReceive(src2_qid, (void *)&audio, 0) == pdTRUE;

		if (!v_ready && !a_ready) {
			vTaskDelay(10);
			continue;
		}
#else
		if (xQueueReceive(src1_qid, (void *)&video, 0xFFFFFFFF) != pdTRUE) {
			continue;
		}

		if (xQueueReceive(src2_qid, (void *)&audio, 0xFFFFFFFF) != pdTRUE) {
			continue;
		}
#endif

#if SHARE_POOL==1
		if (v_ready) {
			memory_free(eb1, video);
		}
		if (a_ready) {
			memory_free(eb1, audio);
		}
#else
		memory_free(eb1, video);
		memory_free(eb2, audio);
#endif

		taskENTER_CRITICAL();
		printf("003 vudio %lx\n\r", (uint32_t)video);
		printf("003 audio %lx\n\r", (uint32_t)audio);
		taskEXIT_CRITICAL();

		video = audio = NULL;

		cnt++;

		if (cnt % 128 == 0) {
			taskENTER_CRITICAL();
			dump_info(eb1);
#if SHARE_POOL==0
			dump_info(eb2);
#endif
			taskEXIT_CRITICAL();
		}
	}
}

void test_memory(void)
{
	printf("test memory\n\r");
	//
#if SHARE_POOL==1
	eb1 = memory_init(512 * 1024, 512);
#else
	eb1 = memory_init(2 * 1024 * 1024, 512);
#endif
	eb2 = memory_init(512 * 1024, 256);

	src1_qid = xQueueCreate(8, sizeof(uint8_t *));
	src2_qid = xQueueCreate(8, sizeof(uint8_t *));

	xTaskCreate(test001, "001", 1024, NULL, 2, NULL);
	xTaskCreate(test002, "002", 1024, NULL, 2, NULL);
	xTaskCreate(test003, "003", 1024, NULL, 2, NULL);
}

#endif
