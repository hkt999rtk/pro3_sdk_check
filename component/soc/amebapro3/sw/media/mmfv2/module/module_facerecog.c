/******************************************************************************
*
* Copyright(c) 2007 - 2025 Realtek Corporation. All rights reserved.
*
******************************************************************************/

#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ameba_soc.h"
#include "arm_math.h"
#include "mmf2_module.h"
#include "mmf2_mediatime_pro3.h"
#include "module_facerecog.h"
#include "module_vipnn.h"

#define DBG_LEVEL   LOG_ERR

#define LOG_OFF     4
#define LOG_ERR     3
#define LOG_MSG     2
#define LOG_INF     1
#define LOG_ALL     0

#define dprintf(level, ...) if ((level) >= DBG_LEVEL) do { printf(__VA_ARGS__); } while (0)

#define FACE_FEATURE_FILE "vfs:/face_feature.bin"

static uint32_t nn_measure_tick[8];
#define TICK_INIT()
#define TICK_GET() (uint32_t)mm_read_mediatime_ms()

#define NN_MEASURE_INIT(n) do { nn_measure_tick[n] = 0; TICK_INIT(); } while (0)

static uint32_t facerecog_crc32(const uint8_t *message, uint32_t len)
{
	uint32_t crc = 0xFFFFFFFF;

	for (uint32_t i = 0; i < len; i++) {
		uint32_t byte = message[i];
		crc = crc ^ byte;
		for (int j = 7; j >= 0; j--) {
			uint32_t mask = -(crc & 1);
			crc = (crc >> 1) ^ (0xEDB88320 & mask);
		}
	}

	return ~crc;
}

static char *facerecog_float_to_str(float value, char *buf, size_t len, int decimals)
{
	uint32_t scale = 1;
	uint32_t scaled;
	uint32_t whole;
	uint32_t frac;
	int negative = 0;
	char frac_buf[8];

	if (len == 0) {
		return buf;
	}

	if (value != value) {
		snprintf(buf, len, "nan");
		return buf;
	}

	if (decimals < 0) {
		decimals = 0;
	} else if (decimals > 6) {
		decimals = 6;
	}

	if (value < 0.0f) {
		negative = 1;
		value = -value;
	}

	for (int i = 0; i < decimals; i++) {
		scale *= 10;
	}

	scaled = (uint32_t)(value * (float)scale + 0.5f);
	whole = scaled / scale;
	frac = scaled % scale;

	if (decimals == 0) {
		snprintf(buf, len, "%s%lu", negative ? "-" : "", (unsigned long)whole);
		return buf;
	}

	for (int i = decimals - 1; i >= 0; i--) {
		frac_buf[i] = (char)('0' + (frac % 10));
		frac /= 10;
	}
	frac_buf[decimals] = '\0';

	snprintf(buf, len, "%s%lu.%s", negative ? "-" : "", (unsigned long)whole, frac_buf);
	return buf;
}

void facerecog_dump_feature(void *feature, int feature_num)
{
	(void)feature;
	dprintf(LOG_INF, "register feature number %d\n\r", feature_num);
}

void facerecog_dump_frc(void *frc_info, int frc_num)
{
	(void)frc_info;
	(void)frc_num;
	dprintf(LOG_INF, "--------------------\n\r");
}

float similarity1(float *a, float *b, int dim)
{
	float sim = 0.0f;
	float tmp;

	for (int i = 0; i < dim; i++) {
		float diff = a[i] - b[i];
		sim += diff * diff;
	}

	arm_sqrt_f32(sim, &tmp);
	return tmp;
}

float similarity2(float *a, float *b, int dim)
{
	float sim = 0.0f;

	for (int i = 0; i < dim; i++) {
		float diff = a[i] - b[i];
		sim += diff * diff;
	}

	return sim;
}

static float sqrt_arm(float x)
{
	float tmp;
	arm_sqrt_f32(x, &tmp);
	return tmp;
}

#define FACERECOG_REDUCE_SQRT 1
#if FACERECOG_REDUCE_SQRT
#define SIMILARITY similarity2
#define THRES(x) ((x) * (x))
#define SIMVAL(x) (sqrt_arm(x))
#else
#define SIMILARITY similarity1
#define THRES(x) (x)
#define SIMVAL(x) (x)
#endif

void facerecog_feature_dump(float *buf, int size)
{
	char value_str[16];

	__disable_irq();
	for (int i = 0; i < size; i += 16) {
		for (int x = 0; x < 16 && (i + x) < size; x++) {
			printf(" %s", facerecog_float_to_str(buf[i + x], value_str, sizeof(value_str), 2));
		}
		printf("\n\r");
	}
	__enable_irq();
}

int search_face_identity(void *p, face_feature_res_t *ff, float thres)
{
	frc_ctx_t *ctx = (frc_ctx_t *)p;
	int res_id = -1;
	float min = 128.0f;

	thres = THRES(thres);

	for (int i = 0; i < ctx->face_data.reg_feature_num; i++) {
		float sim = SIMILARITY(ctx->face_data.reg_feature[i].feature, ff->feature, MAX_FACE_FEATURE_DIM);
		char sim_str[16];
		dprintf(LOG_MSG, "id %d, name %s, sim %s\n\r", i, ctx->face_data.reg_name[i], facerecog_float_to_str(SIMVAL(sim), sim_str, sizeof(sim_str), 6));
		if (sim <= thres && min > sim) {
			res_id = i;
			min = sim;
		}
	}

	return res_id;
}

int facerecog_handle(void *p, void *input, void *output)
{
	(void)output;
	frc_ctx_t *ctx = (frc_ctx_t *)p;
	mm_queue_item_t *input_item = (mm_queue_item_t *)input;
	vipnn_out_buf_t *last_out = (vipnn_out_buf_t *)input_item->data_addr;
	int ff_cnt = last_out->res_cnt;

	if (ff_cnt > MAX_FRC_REG_NUM) {
		ff_cnt = MAX_FRC_REG_NUM;
	}

	char thres_str[16];
	dprintf(LOG_INF, "run recognition obj %d, thres %s\n\r", ff_cnt, facerecog_float_to_str(ctx->params.sim_thres, thres_str, sizeof(thres_str), 6));

	if (ctx->mode == FRC_REGISTER && ff_cnt > 1) {
		dprintf(LOG_ERR, "too many objects in register mode\n\r");
		return 0;
	}

	if (ctx->mode == FRC_RECOGNITION) {
		int face_id = -1;
		face_feature_res_t *ff_res = (face_feature_res_t *)&last_out->res[0];

		for (int i = 0; i < ff_cnt; i++) {
			if (ctx->face_data.reg_feature_num != 0) {
				face_id = search_face_identity(ctx, &ff_res[i], ctx->params.sim_thres);
				dprintf(LOG_MSG, "--------> %s\n\r", face_id >= 0 ? ctx->face_data.reg_name[face_id] : "unknown");
			} else {
				dprintf(LOG_MSG, "--------> no registered face data\n\r");
			}

			ctx->draw_ctx.obj_name[i] = face_id >= 0 ? ctx->face_data.reg_name[face_id] : (char *)"unknown";
			memcpy(&ctx->draw_ctx.bbox[i], &ff_res[i].result[2], sizeof(frc_bbox_t));
		}

		ctx->draw_ctx.pic_width = last_out->input_param->img.width;
		ctx->draw_ctx.pic_height = last_out->input_param->img.height;
		ctx->draw_ctx.obj_cnt = ff_cnt;

		if (ctx->draw) {
			ctx->draw(&ctx->draw_ctx, NULL);
		}
	} else if (ctx->mode == FRC_REGISTER) {
		face_feature_res_t *ff_res = (face_feature_res_t *)&last_out->res[0];
		if (ctx->face_data.reg_feature_num >= MAX_FRC_REG_NUM) {
			dprintf(LOG_ERR, "register fail, out of face feature space\n\r");
		} else if (ff_cnt == 1) {
			memcpy(ctx->face_data.reg_feature[ctx->face_data.reg_feature_num].feature, ff_res[0].feature, sizeof(ff_res->feature));
			strncpy(ctx->face_data.reg_name[ctx->face_data.reg_feature_num], ctx->tmp_reg_name, sizeof(ctx->tmp_reg_name) - 1);
			ctx->face_data.reg_name[ctx->face_data.reg_feature_num][sizeof(ctx->tmp_reg_name) - 1] = '\0';
			ctx->face_data.reg_feature_num++;
		}
		ctx->mode = FRC_RECOGNITION;
	}

	return 0;
}

static int facerecog_load_feature(void *p)
{
	frc_ctx_t *ctx = (frc_ctx_t *)p;
	uint8_t *buf = (uint8_t *)&ctx->face_data;
	FILE *fp = fopen(FACE_FEATURE_FILE, "r");

	if (!fp) {
		dprintf(LOG_ERR, "cannot open %s\n\r", FACE_FEATURE_FILE);
		return -1;
	}

	size_t read_size = fread(buf, 1, sizeof(face_data_t), fp);
	fclose(fp);
	if (read_size != sizeof(face_data_t)) {
		printf("[%s] read failed\n\r", __func__);
		ctx->face_data.reg_feature_num = 0;
		return -1;
	}

	uint32_t crc32_tmp = facerecog_crc32(buf, sizeof(face_data_t) - sizeof(uint32_t));
	if (ctx->face_data.crc32 != crc32_tmp) {
		printf("[%s] invalid content\n\r", __func__);
		ctx->face_data.reg_feature_num = 0;
		return -1;
	}

	printf("loaded %d registered face feature(s) from %s\n\r", ctx->face_data.reg_feature_num, FACE_FEATURE_FILE);
	return 0;
}

static int facerecog_save_feature(void *p)
{
	frc_ctx_t *ctx = (frc_ctx_t *)p;
	uint8_t *buf = (uint8_t *)&ctx->face_data;
	FILE *fp;

	ctx->face_data.crc32 = facerecog_crc32(buf, sizeof(face_data_t) - sizeof(uint32_t));

	fp = fopen(FACE_FEATURE_FILE, "w");
	if (!fp) {
		printf("[%s] open %s failed\n\r", __func__, FACE_FEATURE_FILE);
		return -1;
	}

	size_t write_size = fwrite(buf, 1, sizeof(face_data_t), fp);
	fclose(fp);
	if (write_size != sizeof(face_data_t)) {
		printf("[%s] write failed\n\r", __func__);
		return -1;
	}

	printf("saved %d registered face feature(s) to %s\n\r", ctx->face_data.reg_feature_num, FACE_FEATURE_FILE);
	return 0;
}

static void facerecog_reset_feature(void *p)
{
	frc_ctx_t *ctx = (frc_ctx_t *)p;
	ctx->face_data.reg_feature_num = 0;
}

static void facerecog_list_feature(void *p)
{
	frc_ctx_t *ctx = (frc_ctx_t *)p;
	int feature_num = ctx->face_data.reg_feature_num;

	if (feature_num < 0 || feature_num > MAX_FRC_REG_NUM) {
		dprintf(LOG_ERR, "invalid face feature number %d\n\r", feature_num);
		return;
	}

	printf("registered face feature number %d\n\r", feature_num);
	for (int i = 0; i < feature_num; i++) {
		printf("[%02d] %s\n\r", i, ctx->face_data.reg_name[i]);
	}
}

int facerecog_control(void *p, int cmd, int arg)
{
	frc_ctx_t *ctx = (frc_ctx_t *)p;

	switch (cmd) {
	case CMD_FRC_SET_PARAMS:
		memcpy(&ctx->params, (void *)arg, sizeof(frc_param_t));
		break;
	case CMD_FRC_SET_THRES100:
		ctx->params.sim_thres = (float)arg / 100.0f;
		break;
	case CMD_FRC_SET_OSD_DRAW:
		ctx->draw = (draw_func_t)arg;
		break;
	case CMD_FRC_REGISTER_MODE:
		ctx->mode = FRC_REGISTER;
		if (arg) {
			strncpy(ctx->tmp_reg_name, (char *)arg, sizeof(ctx->tmp_reg_name) - 1);
			ctx->tmp_reg_name[sizeof(ctx->tmp_reg_name) - 1] = '\0';
			dprintf(LOG_INF, "reg mode %d, reg name %s\n\r", ctx->mode, ctx->tmp_reg_name);
		} else {
			dprintf(LOG_INF, "reg mode %d\n\r", ctx->mode);
		}
		break;
	case CMD_FRC_RECOGNITION_MODE:
		ctx->mode = FRC_RECOGNITION;
		dprintf(LOG_INF, "rec mode %d\n\r", ctx->mode);
		break;
	case CMD_FRC_LOAD_FEATURES:
		return facerecog_load_feature(p);
	case CMD_FRC_SAVE_FEATURES:
		return facerecog_save_feature(p);
	case CMD_FRC_RESET_FEATURES:
		facerecog_reset_feature(p);
		break;
	case CMD_FRC_LIST_FEATURES:
		facerecog_list_feature(p);
		break;
	}

	return 0;
}

void *facerecog_destroy(void *p)
{
	frc_ctx_t *ctx = (frc_ctx_t *)p;

	if (ctx) {
		free(ctx);
	}

	return NULL;
}

void *facerecog_create(void *parent)
{
	(void)parent;
	frc_ctx_t *ctx = (frc_ctx_t *)malloc(sizeof(frc_ctx_t));

	if (!ctx) {
		return NULL;
	}
	memset(ctx, 0, sizeof(frc_ctx_t));
	ctx->params.sim_thres = 0.99f;

	NN_MEASURE_INIT(0);
	NN_MEASURE_INIT(1);
	NN_MEASURE_INIT(2);
	NN_MEASURE_INIT(3);
	NN_MEASURE_INIT(4);

	return ctx;
}

mm_module_t facerecog_module = {
	.create = facerecog_create,
	.destroy = facerecog_destroy,
	.control = facerecog_control,
	.handle = facerecog_handle,

	.output_type = MM_TYPE_NONE,
	.module_type = MM_TYPE_VDSP,
	.name = "facerecog"
};
