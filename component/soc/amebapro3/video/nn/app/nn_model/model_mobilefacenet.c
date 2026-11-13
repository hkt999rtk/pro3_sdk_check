//---------------------------------------------------------
// Face Recognition - MobileFaceNet
//---------------------------------------------------------

#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "ameba_soc.h"

#include "img_process/img_process.h"
#include "nn_utils/quantize.h"
#include "nn_utils/tensor.h"
#include "mmf2_module.h"
#include "module_vipnn.h"
#include "math.h"
#include "arm_math.h"

static char *mbfacenet_filename = "sd:/mobilefacenet_asymu8.nb";

#define DBG_LEVEL	LOG_ERR

#define LOG_OFF		4
#define LOG_ERR		3
#define LOG_MSG		2
#define LOG_INF		1
#define LOG_ALL		0

#define dprintf(level, ...) if(level >= DBG_LEVEL) printf(__VA_ARGS__)

void *mbfacenet_get_network_filename(void)
{
	return (void *)mbfacenet_filename;	// fix name for NN model binary
}

void mbfacenet_set_network_filename(char *fn)
{
	mbfacenet_filename = fn;
}

// before face recognition, the face should be aligned according to arcface reference landmark
// https://github.com/deepinsight/insightface/blob/master/python-package/insightface/utils/face_align.py
static float arcface_lm[5][2] = {
	{38.2946, 51.6963},
	{73.5318, 51.5014},
	{56.0252, 71.7366},
	{41.5493, 92.3655},
	{70.7299, 92.2041},
};

//--------CASCADE INPUT SETUP-------------------------------------------
static void mbfacenet_input_setup(void *last_res, int i, nn_data_param_t *input_param)
{
	/* setup landmark and source face result */
	facedetect_res_t *fd_res = (facedetect_res_t *)last_res;

	dprintf(LOG_MSG, "[mfn setup] %p, %d\n\r", fd_res, i);
	dprintf(LOG_MSG, "[mfn setup] w %d\n\r", input_param->img.width);
	dprintf(LOG_MSG, "[mfn setup] h %d\n\r", input_param->img.height);
	for (int k = 0; k < 6; k++) {
		dprintf(LOG_MSG, "[mfn setup] result %d %f\n\r", k, fd_res[i].result[k]);
	}

	for (int x = 0; x < 5; x++) {
		input_param->img.landmark.pos[x].x = fd_res[i].landmark.pos[x].x * input_param->img.width;
		input_param->img.landmark.pos[x].y = fd_res[i].landmark.pos[x].y * input_param->img.height;
	}
	input_param->priv = (void *)&fd_res[i];

	dprintf(LOG_MSG, "[mfn setup] update pic w %d, h %d\n\r", input_param->img.width, input_param->img.height);
}

//--------PRE PROCESS-------------------------------------------
static float face_length_thresh = 0.25;
static int face_in_width, face_in_height;
int mbfacenet_preprocess(void *data_in, nn_data_param_t *data_param, void *tensor_in, nn_tensor_param_t *tensor_param)
{
	void **tensor = (void **)tensor_in;
	rect_t roi = {0, 0, data_param->img.width, data_param->img.height};
	facedetect_res_t *fd_res = (facedetect_res_t *)data_param->priv;

	if (fd_res) {
		roi.xmin = (int)(fd_res->res.top_x * data_param->img.width);
		roi.ymin = (int)(fd_res->res.top_y * data_param->img.height);
		roi.xmax = (int)(fd_res->res.bot_x * data_param->img.width);
		roi.ymax = (int)(fd_res->res.bot_y * data_param->img.height);
	}

	face_in_width  = tensor_param->dim[0].size[0];
	face_in_height = tensor_param->dim[0].size[1];

	landmarki_t *landmark = &data_param->img.landmark;

	img_t img_in, img_out;
	img_in.width  = data_param->img.width;
	img_in.height = data_param->img.height;
	img_out.width  = face_in_width;
	img_out.height = face_in_height;

	img_in.data = (uint8_t *)data_in;
	img_out.data = (uint8_t *)tensor[0];
	dprintf(LOG_MSG, "[mfn pre]src %d %d, dst %d %d\n\r", img_in.width, img_in.height, img_out.width, img_out.height);
	dprintf(LOG_MSG, "[mfn pre]roi %d %d %d %d \n\r", roi.xmin, roi.ymin, roi.xmax, roi.ymax);

	rotate_t rcfg;
	rcfg.cx = (landmark->pos[0].x + landmark->pos[1].x + landmark->pos[3].x + landmark->pos[4].x) / 4; // set point between eyes and mouth as rotation center x
	rcfg.cy = (landmark->pos[0].y + landmark->pos[1].y + landmark->pos[3].y + landmark->pos[4].y) / 4; // set point between eyes and mouth as rotation center y

	// check the face is big enough; if yes, it's worth doing face alignment
	float face_length = fd_res ? fd_res->res.bot_y - fd_res->res.top_y : 0.0f;

	if (face_length > face_length_thresh && (rcfg.cx != 0 || rcfg.cy != 0)) {
		// face alignment
		dprintf(LOG_INF, "[mfn pre]do face alignment\n\r");
		int len_adj = (landmark->pos[0].x - landmark->pos[1].x);
		int len_opp = (landmark->pos[0].y - landmark->pos[1].y);
		rcfg.angle = atan((float)len_opp / (float)len_adj);
		dprintf(LOG_MSG, "center %d,%d rotate %f \n\r", rcfg.cx, rcfg.cy, rcfg.angle * 180 / 3.1415926);
		img_rotation_fix(&img_in, &roi, &rcfg, &img_out, &arcface_lm[2]);
	} else {
		// resize without alignment
		dprintf(LOG_INF, "[mfn pre]resize to NN input size\n\r");
		img_roi_resize_planar(&img_in, &roi, &img_out);
	}

	DCache_Clean((uint32_t)img_out.data, img_out.width * img_out.height * 3);

	return PP_USE_RESULT;
}

static float inner_product_arm(float *a, float *b, int len)
{
	float tmp;
	arm_dot_prod_f32(a, b, len, &tmp);
	return tmp;
}

static float sqrt_arm(float x)
{
	float tmp;
	arm_sqrt_f32(x, &tmp);
	return tmp;
}

static int mbfacenet_get_tensor_element_count(nn_tensor_dim_t *dim)
{
	int count = 1;

	for (int i = 0; i < dim->num; i++) {
		count *= dim->size[i];
	}

	return count;
}

#if DBG_LEVEL <= LOG_INF
static void face_feature_dump(int log_level, float *pfeature, int num)
{
	if (log_level >= DBG_LEVEL) {
		printf("--------------------------------------------------\r\n");
		printf("face feature dump\r\n");
		printf("--------------------------------------------------\r\n");
		for (int i = 0; i < num; i++) {
			printf("%6.2f ", pfeature[i]);
			if ((i + 1) % 16 == 0) {
				printf("\r\n");
			}
		}
		printf("--------------------------------------------------\r\n");
	}
}
#endif

//--------POST PROCESS-------------------------------------------

int mbfacenet_postprocess(void *tensor_out, nn_tensor_param_t *param, void *res, uint32_t res_buf_size)
{
	void **tensor = (void **)tensor_out;

	face_feature_res_t *ff_res = (face_feature_res_t *)res;
	facedetect_res_t *fd_res = (facedetect_res_t *)param->priv;

	if (res_buf_size < sizeof(face_feature_res_t)) {
		dprintf(LOG_ERR, "face feature result buffer too small\r\n");
		return 0;
	}

	// decode
	uint8_t *ff_tensor = (uint8_t *)tensor[0];
	int face_feature_dim = mbfacenet_get_tensor_element_count(&param->dim[0]);
	dprintf(LOG_INF, "face feature dim: %d\r\n", face_feature_dim);

	if (face_feature_dim > MAX_FACE_FEATURE_DIM) {
		dprintf(LOG_ERR, "face feature dim %d exceeds max %d\r\n", face_feature_dim, MAX_FACE_FEATURE_DIM);
		face_feature_dim = MAX_FACE_FEATURE_DIM;
	}

	float face_feature[MAX_FACE_FEATURE_DIM] = {0};
	for (int i = 0; i < face_feature_dim; i++) {
		face_feature[i] = get_tensor_value(ff_tensor, i, &param->format[0]);
	}

	// normalize
	float l2_norm = sqrt_arm(inner_product_arm(face_feature, face_feature, face_feature_dim));
	if (l2_norm <= 0.0f || l2_norm != l2_norm) {
		dprintf(LOG_ERR, "invalid face feature norm\r\n");
		return 0;
	}

	float scale = 1 / l2_norm;
	arm_scale_f32(face_feature, scale, face_feature, face_feature_dim);

	// dump result
#if DBG_LEVEL <= LOG_INF
	face_feature_dump(LOG_INF, face_feature, face_feature_dim);
#endif

	// copy bbox and face feature vector
	memset(ff_res, 0, sizeof(face_feature_res_t));
	if (fd_res) {
		memcpy(ff_res->result, fd_res->result, sizeof(fd_res->result));
	}
	memcpy(ff_res->feature, face_feature, face_feature_dim * sizeof(float));

	// return handled object count
	return 1;
}

nnmodel_t mbfacenet_fwfs = {
	.nb 			= mbfacenet_get_network_filename,
	.set_nb_file    = mbfacenet_set_network_filename,
	.preprocess 	= mbfacenet_preprocess,
	.postprocess 	= mbfacenet_postprocess,
	.cas_in_setup	= mbfacenet_input_setup,
	.model_src 		= MODEL_SRC_FILE,

	.name = "MBFACENET"
};
