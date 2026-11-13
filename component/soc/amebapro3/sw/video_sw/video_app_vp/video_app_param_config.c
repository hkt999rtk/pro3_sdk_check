#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "ameba_soc.h"
#include "video_common_type.h"

/********************************************************************************/
/*  Include Video User Scenario                                                 */
/********************************************************************************/

/* please make sure only 1 user scenario is included as default parameter */

//#include "video_scenario/scenario_1.h"
#include "video_scenario/scenario_2.h"


/********************************************************************************/
/*  Auto Configuration                                                          */
/********************************************************************************/

/* resolution width and height lookup macro */
#define GET_RESOLUTION_WIDTH(res) ( \
    (res) == VIDEO_QCIF ? 176 : \
    (res) == VIDEO_CIF  ? 352 : \
    (res) == VIDEO_WVGA ? 640 : \
    (res) == VIDEO_VGA  ? 640 : \
    (res) == VIDEO_D1   ? 720 : \
    (res) == VIDEO_HD   ? 1280 : \
    (res) == VIDEO_FHD  ? 1920 : \
    (res) == VIDEO_3M   ? 2048 : \
    (res) == VIDEO_5M   ? 2592 : \
    (res) == VIDEO_2K   ? 2560 : 0)

#define GET_RESOLUTION_HEIGHT(res) ( \
    (res) == VIDEO_QCIF ? 144 : \
    (res) == VIDEO_CIF  ? 288 : \
    (res) == VIDEO_WVGA ? 360 : \
    (res) == VIDEO_VGA  ? 480 : \
    (res) == VIDEO_D1   ? 480 : \
    (res) == VIDEO_HD   ? 720 : \
    (res) == VIDEO_FHD  ? 1080 : \
    (res) == VIDEO_3M   ? 1536 : \
    (res) == VIDEO_5M   ? 1944 : \
    (res) == VIDEO_2K   ? 1440 : 0)

/* Video channel resolution definitions */
#if defined(V1_RESOLUTION) && (V1_RESOLUTION != VIDEO_NON_STANDARD)
#define V1_WIDTH    GET_RESOLUTION_WIDTH(V1_RESOLUTION)
#define V1_HEIGHT   GET_RESOLUTION_HEIGHT(V1_RESOLUTION)
#endif

#if defined(V2_RESOLUTION) && (V2_RESOLUTION != VIDEO_NON_STANDARD)
#define V2_WIDTH    GET_RESOLUTION_WIDTH(V2_RESOLUTION)
#define V2_HEIGHT   GET_RESOLUTION_HEIGHT(V2_RESOLUTION)
#endif

#if defined(V3_RESOLUTION) && (V3_RESOLUTION != VIDEO_NON_STANDARD)
#define V3_WIDTH    GET_RESOLUTION_WIDTH(V3_RESOLUTION)
#define V3_HEIGHT   GET_RESOLUTION_HEIGHT(V3_RESOLUTION)
#endif

#if defined(V4_RESOLUTION) && (V4_RESOLUTION != VIDEO_NON_STANDARD)
#define V4_WIDTH    GET_RESOLUTION_WIDTH(V4_RESOLUTION)
#define V4_HEIGHT   GET_RESOLUTION_HEIGHT(V4_RESOLUTION)
#endif

#include "module_encoder.h"
#include "module_isp.h"
#include "video_app_param_config.h"

static void print_help(void)
{
	printf("Usage: program [options]\n");
	printf("ISP parameters:\n");
	printf("  --ainr=on/off\n\n");
	printf("  --isp[n]:w=width,h=height,fps=fps,comp=compress,fmt=format\n\n");
	printf("Encoder parameters:\n");
	printf("  --enc[n]:w=width,h=height,fps=fps,gop=gop,bps=bps,rcmode=rcmode,minqp=minqp,maxqp=maxqp,profile=profile,level=level,tier=tier,qlevel=qlevel,osd=idx,mos=idx,scale=scale_up,decomp=decompress,fmt=codec_fmt\n\n");
	printf("Example:\n");
	printf("  --ainr=1\n");
	printf("  --isp0:w=1920,h=1080,fps=30,comp=0,fmt=1\n");
	printf("  --enc0:w=1920,h=1080,fps=30,gop=30,bps=2000000,rcmode=3,minqp=26,maxqp=48,qlevel=5,osd=0,mos=0,scale=1,decomp=0,fmt=5\n");
}

static void parse_parameter_set(char *params, isp_params_t *isp_param, enc_parameter_t *enc_param)
{
	char *token;
	char *inner_saveptr;
	char *kv_saveptr;

	while ((token = strtok_r(params, ",", &inner_saveptr)) != NULL) {
		params = NULL;

		char *param_name = strtok_r(token, "=", &kv_saveptr);
		char *value = strtok_r(NULL, "=", &kv_saveptr);

		if (!value) {
			continue;
		}

		if (isp_param) {
			if (strcmp(param_name, "w") == 0) {
				isp_param->width = atoi(value);
			} else if (strcmp(param_name, "h") == 0) {
				isp_param->height = atoi(value);
			} else if (strcmp(param_name, "fps") == 0) {
				isp_param->fps = atoi(value);
			} else if (strcmp(param_name, "fmt") == 0) {
				isp_param->format = atoi(value);
			} else if (strcmp(param_name, "comp") == 0) {
				isp_param->compress_en = atoi(value);
			} else if (strcmp(param_name, "slice") == 0) {
				isp_param->slice_mode_en = atoi(value);
			}
		} else {
			if (strcmp(param_name, "w") == 0) {
				enc_param->width = atoi(value);
			} else if (strcmp(param_name, "h") == 0) {
				enc_param->height = atoi(value);
			} else if (strcmp(param_name, "fps") == 0) {
				enc_param->fps = atoi(value);
			} else if (strcmp(param_name, "gop") == 0) {
				enc_param->gop = atoi(value);
			} else if (strcmp(param_name, "bps") == 0) {
				enc_param->bps = atoi(value);
			} else if (strcmp(param_name, "rcmode") == 0) {
				enc_param->rcMode = atoi(value);
			} else if (strcmp(param_name, "minqp") == 0) {
				enc_param->minQp = atoi(value);
			} else if (strcmp(param_name, "maxqp") == 0) {
				enc_param->maxQp = atoi(value);
			} else if (strcmp(param_name, "rcfps") == 0) {
				unsigned long frame_rate_num = 0;
				unsigned long frame_rate_denom = 0;
				if (sscanf(value, "%lu:%lu", &frame_rate_num, &frame_rate_denom) == 2) {
					enc_param->initRc.flags |= VPAP_ENC_RC_EXT_FRAME_RATE;
					enc_param->initRc.frameRateNum = (uint32_t)frame_rate_num;
					enc_param->initRc.frameRateDenom = (uint32_t)frame_rate_denom;
				}
			} else if (strcmp(param_name, "qphdr") == 0) {
				enc_param->initRc.flags |= VPAP_ENC_RC_EXT_QP_HDR;
				enc_param->initRc.qpHdr = atoi(value);
			} else if (strcmp(param_name, "miniqp") == 0) {
				enc_param->initRc.flags |= VPAP_ENC_RC_EXT_I_QP_RANGE;
				enc_param->initRc.minIQp = atoi(value);
			} else if (strcmp(param_name, "maxiqp") == 0) {
				enc_param->initRc.flags |= VPAP_ENC_RC_EXT_I_QP_RANGE;
				enc_param->initRc.maxIQp = atoi(value);
			} else if (strcmp(param_name, "intraqpdelta") == 0) {
				enc_param->initRc.flags |= VPAP_ENC_RC_EXT_INTRA_QP_DELTA;
				enc_param->initRc.intraQpDelta = atoi(value);
			} else if (strcmp(param_name, "fixediqp") == 0) {
				enc_param->initRc.flags |= VPAP_ENC_RC_EXT_FIXED_INTRA_QP;
				enc_param->initRc.fixedIntraQp = atoi(value);
			} else if (strcmp(param_name, "picrc") == 0) {
				enc_param->initRc.flags |= VPAP_ENC_RC_EXT_PIC_RC;
				enc_param->initRc.pictureRc = atoi(value);
			} else if (strcmp(param_name, "ctbrc") == 0) {
				enc_param->initRc.flags |= VPAP_ENC_RC_EXT_CTB_RC;
				enc_param->initRc.ctbRc = atoi(value);
			} else if (strcmp(param_name, "blockrc") == 0) {
				enc_param->initRc.flags |= VPAP_ENC_RC_EXT_BLOCK_RC_SIZE;
				enc_param->initRc.blockRCSize = atoi(value);
			} else if (strcmp(param_name, "picskip") == 0) {
				enc_param->initRc.flags |= VPAP_ENC_RC_EXT_PIC_SKIP;
				enc_param->initRc.pictureSkip = atoi(value);
			} else if (strcmp(param_name, "hrd") == 0) {
				enc_param->initRc.flags |= VPAP_ENC_RC_EXT_HRD;
				enc_param->initRc.hrd = atoi(value);
			} else if (strcmp(param_name, "cpb") == 0) {
				enc_param->initRc.flags |= VPAP_ENC_RC_EXT_CPB;
				enc_param->initRc.hrdCpbSize = atoi(value);
			} else if (strcmp(param_name, "cpbmax") == 0) {
				enc_param->initRc.flags |= VPAP_ENC_RC_EXT_CPB;
				enc_param->initRc.cpbMaxRate = atoi(value);
			} else if (strcmp(param_name, "bitratewin") == 0) {
				enc_param->initRc.flags |= VPAP_ENC_RC_EXT_BITRATE_WINDOW;
				enc_param->initRc.bitrateWindow = atoi(value);
			} else if (strcmp(param_name, "tolmoving") == 0) {
				enc_param->initRc.flags |= VPAP_ENC_RC_EXT_MOVING_BITRATE;
				enc_param->initRc.tolMovingBitRate = atoi(value);
			} else if (strcmp(param_name, "monitor") == 0) {
				enc_param->initRc.flags |= VPAP_ENC_RC_EXT_MOVING_BITRATE;
				enc_param->initRc.monitorFrames = atoi(value);
			} else if (strcmp(param_name, "filler") == 0) {
				enc_param->initRc.flags |= VPAP_ENC_RC_EXT_FILLER_DATA;
				enc_param->initRc.fillerData = atoi(value);
			} else if (strcmp(param_name, "crf") == 0) {
				enc_param->initRc.flags |= VPAP_ENC_RC_EXT_CRF;
				enc_param->initRc.crf = atoi(value);
			} else if (strcmp(param_name, "vbr") == 0) {
				enc_param->initRc.flags |= VPAP_ENC_RC_EXT_VBR;
				enc_param->initRc.vbr = atoi(value);
			} else if (strcmp(param_name, "profile") == 0) {
				enc_param->profile = atoi(value);
				enc_param->profileSet = 1;
			} else if (strcmp(param_name, "level") == 0) {
				enc_param->level = atoi(value);
				enc_param->levelSet = 1;
			} else if (strcmp(param_name, "tier") == 0) {
				enc_param->tier = atoi(value);
				enc_param->tierSet = 1;
			} else if (strcmp(param_name, "qlevel") == 0) {
				enc_param->qlevel = atoi(value);
			} else if (strcmp(param_name, "fmt") == 0) {
				enc_param->codec_fmt = atoi(value);
			} else if (strcmp(param_name, "slice") == 0) {
				enc_param->initCoding.flags |= VPAP_ENC_CODING_SLICE_SIZE;
				enc_param->initCoding.sliceSize = atoi(value);
			} else if (strcmp(param_name, "sei") == 0) {
				enc_param->initCoding.flags |= VPAP_ENC_CODING_SEI;
				enc_param->initCoding.seiMessages = atoi(value);
			} else if (strcmp(param_name, "seiuser") == 0) {
				enc_param->initCoding.flags |= VPAP_ENC_CODING_SEI_USER_DATA;
				enc_param->initCoding.seiUserData = atoi(value);
			} else if (strcmp(param_name, "fullrange") == 0) {
				enc_param->initCoding.flags |= VPAP_ENC_CODING_FULL_RANGE;
				enc_param->initCoding.vuiVideoFullRange = atoi(value);
			} else if (strcmp(param_name, "deblock") == 0) {
				enc_param->initCoding.flags |= VPAP_ENC_CODING_DEBLOCK;
				enc_param->initCoding.disableDeblockingFilter = atoi(value);
			} else if (strcmp(param_name, "tcoffset") == 0) {
				enc_param->initCoding.flags |= VPAP_ENC_CODING_DEBLOCK;
				enc_param->initCoding.tcOffset = atoi(value);
			} else if (strcmp(param_name, "betaoffset") == 0) {
				enc_param->initCoding.flags |= VPAP_ENC_CODING_DEBLOCK;
				enc_param->initCoding.betaOffset = atoi(value);
			} else if (strcmp(param_name, "sao") == 0) {
				enc_param->initCoding.flags |= VPAP_ENC_CODING_SAO;
				enc_param->initCoding.enableSao = atoi(value);
			} else if (strcmp(param_name, "cabac") == 0) {
				enc_param->initCoding.flags |= VPAP_ENC_CODING_CABAC;
				enc_param->initCoding.enableCabac = atoi(value);
			} else if (strcmp(param_name, "sarw") == 0) {
				enc_param->initCoding.flags |= VPAP_ENC_CODING_SAR;
				enc_param->initCoding.sampleAspectRatioWidth = atoi(value);
			} else if (strcmp(param_name, "sarh") == 0) {
				enc_param->initCoding.flags |= VPAP_ENC_CODING_SAR;
				enc_param->initCoding.sampleAspectRatioHeight = atoi(value);
			} else if (strcmp(param_name, "cir") == 0) {
				unsigned long start = 0;
				unsigned long interval = 0;
				if (sscanf(value, "%lu:%lu", &start, &interval) == 2) {
					enc_param->initCoding.flags |= VPAP_ENC_CODING_CIR;
					enc_param->initCoding.cirStart = (uint32_t)start;
					enc_param->initCoding.cirInterval = (uint32_t)interval;
				}
			} else if (strncmp(param_name, "roi", 3) == 0 &&
					   param_name[3] >= '0' && param_name[3] < ('0' + VPAP_ENC_MAX_ROI) &&
					   param_name[4] == '\0') {
				unsigned long enable = 0;
				unsigned long top = 0;
				unsigned long left = 0;
				unsigned long bottom = 0;
				unsigned long right = 0;
				long delta_qp = 0;
				long qp = 0;
				int idx = param_name[3] - '0';
				if (sscanf(value, "%lu:%lu:%lu:%lu:%lu:%ld:%ld",
						   &enable, &top, &left, &bottom, &right, &delta_qp, &qp) == 7) {
					vpap_shared_enc_roi_t *roi = &enc_param->initCoding.roi[idx];
					enc_param->initCoding.flags |= VPAP_ENC_CODING_ROI;
					roi->enable = (uint32_t)enable;
					roi->top = (uint32_t)top;
					roi->left = (uint32_t)left;
					roi->bottom = (uint32_t)bottom;
					roi->right = (uint32_t)right;
					roi->deltaQp = (int32_t)delta_qp;
					roi->qp = (int32_t)qp;
				}
			} else if (strcmp(param_name, "osd") == 0) {
				int idx = atoi(value);
				if (idx >= 0 && idx < MAX_OVERLAY_NUM) {
					enc_param->osd_overlay[idx].osdOverlayEnable = 1;
				}
			} else if (strcmp(param_name, "mos") == 0) {
				int idx = atoi(value);
				if (idx >= 0 && idx < MAX_MOSAIC_NUM) {
					enc_param->mosaic[idx].mosOverlayEnable = 1;
				}
			} else if (strcmp(param_name, "scale") == 0) {
				enc_param->cs.cropScaleEnable = atoi(value);
			} else if (strcmp(param_name, "decomp") == 0) {
				enc_param->decompEnable = atoi(value);
			} else if (strcmp(param_name, "lowlatency") == 0) {
				enc_param->lowLatencyEnable = atoi(value);
			}
		}
	}
}

static void parse_global_ainr_switch(const char *arg, isp_params_t isp_params[])
{
	char *params = strchr(arg, '=') + 1;
	int ainr_en = atoi(params);
	for (int i = 0; i < VIDEO_ISP_VALID_CH; i++) {
		isp_params[i].ai_denoise_en = ainr_en;
	}
}

static void parse_arguments(char *arg_str, isp_enc_ch_en_t *v_ch_en, isp_params_t isp_params[], enc_parameter_t enc_params[])
{
	char *arg;
	char *outer_saveptr;

	while ((arg = strtok_r(arg_str, " ", &outer_saveptr)) != NULL) {
		arg_str = NULL;

		if (strcmp(arg, "-h") == 0 || strcmp(arg, "--help") == 0) {
			print_help();
		}

		int is_isp = strncmp(arg, "--isp", 5) == 0;
		int is_enc = strncmp(arg, "--enc", 5) == 0;
		int is_ainr = strncmp(arg, "--ainr", 6) == 0;

		if (is_isp) {
			int index = arg[5] - '0';
			if (index < 0 || index >= VIDEO_ISP_VALID_CH) {
				continue;
			}
			v_ch_en->isp[index] = 1;
			isp_params[index].stream_id = index;

			char *params = strchr(arg, ':') + 1;
			parse_parameter_set(params, &isp_params[index], NULL);
		} else if (is_enc) {
			int index = arg[5] - '0';
			if (index < 0 || index >= VIDEO_ENC_VALID_CH) {
				continue;
			}
			v_ch_en->enc[index] = 1;
			enc_params[index].stream_id = index;

			char *params = strchr(arg, ':') + 1;
			parse_parameter_set(params, NULL, &enc_params[index]);
		} else if (is_ainr) {
			parse_global_ainr_switch(arg, isp_params);
		}
	}
}

const char *video_cmdline[] = {

	"--ainr=1 "
	"--isp0:w=2688,h=1520,fps=30,fmt=1 ",

	"--ainr=0 "
	"--isp0:w=2688,h=1520,fps=30,fmt=1 ",

	"--ainr=1 "
	"--isp0:w=2688,h=1520,fps=30,fmt=1 "
	"--enc0:w=2688,h=1520,fps=30,gop=30,bps=2000000,rcmode=3,minqp=26,maxqp=48,fmt=1 ",

	"--ainr=0 "
	"--isp0:w=2688,h=1520,fps=30,fmt=1 "
	"--enc0:w=2688,h=1520,fps=30,gop=30,bps=2000000,rcmode=3,minqp=26,maxqp=48,fmt=1 ",

	/* AI denoise */
	"--ainr=1 "
	/* ch0 */
	"--isp0:w=2688,h=1520,fps=30,comp=0,fmt=1 "
	"--enc0:w=2688,h=1520,fps=30,gop=30,bps=2000000,rcmode=3,minqp=26,maxqp=48,qlevel=5,osd=0,osd=1,mos=2,mos=3,decomp=0,fmt=5 "
	/* ch1 */
	"--isp1:w=1280,h=720,fps=30,fmt=1 "
	"--enc1:w=1280,h=720,fps=30,gop=30,bps=1000000,rcmode=3,minqp=26,maxqp=48,osd=0,osd=1,mos=2,mos=3,scale=0,fmt=2 "
	/* ch4 */
	"--isp4:w=640,h=480,fps=15,fmt=5 ",

	/* AI denoise */
	"--ainr=0 "
	/* ch0 */
	"--isp0:w=2688,h=1520,fps=30,comp=0,fmt=1 "
	"--enc0:w=2688,h=1520,fps=30,gop=30,bps=2000000,rcmode=3,minqp=26,maxqp=48,qlevel=5,osd=0,osd=1,mos=2,mos=3,decomp=0,fmt=5 "
	/* ch1 */
	"--isp1:w=1280,h=720,fps=30,fmt=1 "
	"--enc1:w=1280,h=720,fps=30,gop=30,bps=1000000,rcmode=3,minqp=26,maxqp=48,osd=0,osd=1,mos=2,mos=3,scale=0,fmt=2 "
	/* ch4 */
	"--isp4:w=640,h=480,fps=15,fmt=5 ",

	/********************************************************************************/
	/*  TEST COMMAND 0                                                              */
	/********************************************************************************/
	/* AI denoise */
	"--ainr=0 "
	/* ch0 */
	"--isp0:w=2560,h=1440,fps=30,comp=0,fmt=1 "
	"--enc0:w=2560,h=1440,fps=30,gop=30,bps=2000000,rcmode=3,minqp=26,maxqp=48,qlevel=5,osd=0,osd=1,mos=2,mos=3,decomp=0,fmt=5 "
	/* ch1 */
	"--isp1:w=1280,h=720,fps=30,fmt=1 "
	"--enc1:w=1280,h=720,fps=30,gop=30,bps=1000000,rcmode=3,minqp=26,maxqp=48,osd=0,osd=1,mos=2,mos=3,scale=0,fmt=2 "
	/* ch2 */
	"--isp2:w=1920,h=1080,fps=1,fmt=1 "
	"--enc2:w=1920,h=1080,fps=1,gop=1,bps=1000000,rcmode=3,minqp=26,maxqp=48,qlevel=5,fmt=4 "
	/* ch3 */
	"--isp3:w=640,h=480,fps=10,fmt=1 "
	/* ch4 */
	"--isp4:w=640,h=480,fps=10,fmt=5 ",

	/********************************************************************************/
	/*  TEST COMMAND 1                                                              */
	/********************************************************************************/
	/* AI denoise */
	"--ainr=0 "
	/* ch0 */
	"--isp0:w=2560,h=1440,fps=30,comp=1,fmt=1 "
	"--enc0:w=2560,h=1440,fps=30,gop=30,bps=2000000,rcmode=3,minqp=26,maxqp=48,qlevel=5,osd=0,osd=1,mos=2,mos=3,decomp=1,fmt=5 "
	/* ch1 */
	"--isp1:w=1280,h=720,fps=30,fmt=1 "
	"--enc1:w=1280,h=720,fps=30,gop=30,bps=1000000,rcmode=3,minqp=26,maxqp=48,osd=0,osd=1,mos=2,mos=3,scale=0,fmt=2 "
	/* ch2 */
	"--isp2:w=1920,h=1080,fps=1,fmt=1 "
	"--enc2:w=1920,h=1080,fps=1,gop=1,bps=1000000,rcmode=3,minqp=26,maxqp=48,qlevel=5,fmt=4 "
	/* ch3 */
	"--isp3:w=640,h=480,fps=10,fmt=1 "
	/* ch4 */
	"--isp4:w=640,h=480,fps=10,fmt=5 ",

	/********************************************************************************/
	/*  TEST COMMAND 2                                                              */
	/********************************************************************************/
	/* AI denoise */
	"--ainr=0 "
	/* ch0 */
	"--isp0:w=2560,h=1440,fps=30,comp=0,fmt=1 "
	"--enc0:w=2560,h=1440,fps=30,gop=30,bps=2000000,rcmode=3,minqp=26,maxqp=48,qlevel=5,osd=0,osd=1,mos=2,mos=3,decomp=0,fmt=5 "
	/* ch1 */
	"--isp1:w=1280,h=720,fps=30,fmt=1 "
	"--enc1:w=1280,h=720,fps=30,gop=30,bps=1000000,rcmode=3,minqp=26,maxqp=48,osd=0,osd=1,mos=2,mos=3,scale=1,fmt=2 "
	/* ch2 */
	"--isp2:w=1920,h=1080,fps=1,fmt=1 "
	"--enc2:w=1920,h=1080,fps=1,gop=1,bps=1000000,rcmode=3,minqp=26,maxqp=48,qlevel=5,fmt=4 "
	/* ch3 */
	"--isp3:w=640,h=480,fps=10,fmt=1 "
	/* ch4 */
	"--isp4:w=640,h=480,fps=10,fmt=5 ",

	/********************************************************************************/
	/*  TEST COMMAND 3                                                              */
	/********************************************************************************/
	/* AI denoise */
	"--ainr=0 "
	/* ch0 */
	"--isp0:w=2560,h=1440,fps=30,comp=1,fmt=1 "
	"--enc0:w=2560,h=1440,fps=30,gop=30,bps=2000000,rcmode=3,minqp=26,maxqp=48,qlevel=5,osd=0,osd=1,mos=2,mos=3,decomp=1,fmt=5 "
	/* ch1 */
	"--isp1:w=1280,h=720,fps=30,fmt=1 "
	"--enc1:w=1280,h=720,fps=30,gop=30,bps=1000000,rcmode=3,minqp=26,maxqp=48,osd=0,osd=1,mos=2,mos=3,scale=1,fmt=2 "
	/* ch2 */
	"--isp2:w=1920,h=1080,fps=1,fmt=1 "
	"--enc2:w=1920,h=1080,fps=1,gop=1,bps=1000000,rcmode=3,minqp=26,maxqp=48,qlevel=5,fmt=4 "
	/* ch3 */
	"--isp3:w=640,h=480,fps=10,fmt=1 "
	/* ch4 */
	"--isp4:w=640,h=480,fps=10,fmt=5 ",

	/********************************************************************************/
	/*  TEST COMMAND 4                                                              */
	/********************************************************************************/
	/* AI denoise */
	"--ainr=0 "
	/* ch0 */
	"--isp0:w=1920,h=1080,fps=30,comp=1,fmt=1 "
	"--enc0:w=1920,h=1080,fps=30,gop=30,bps=2000000,rcmode=3,minqp=26,maxqp=48,qlevel=5,osd=0,osd=1,mos=2,mos=3,decomp=1,fmt=5 "
	/* ch1 */
	"--isp1:w=1280,h=720,fps=30,fmt=1 "
	"--enc1:w=1280,h=720,fps=30,gop=30,bps=1000000,rcmode=3,minqp=26,maxqp=48,osd=0,osd=1,mos=2,mos=3,scale=1,fmt=2 "
	/* ch2 */
	"--isp2:w=1920,h=1080,fps=1,fmt=1 "
	"--enc2:w=1920,h=1080,fps=1,gop=1,bps=1000000,rcmode=3,minqp=26,maxqp=48,qlevel=5,fmt=4 "
	/* ch3 */
	"--isp3:w=640,h=480,fps=10,fmt=1 "
	/* ch4 */
	"--isp4:w=640,h=480,fps=10,fmt=5 ",


	/********************************************************************************/
	/*  TEST COMMAND 5 - Slice Mode with RTT - Single Channel                       */
	/********************************************************************************/
	/* AI denoise */
	"--ainr=0 "
	/* ch0 */
	"--isp0:w=1920,h=1080,fps=30,comp=0,slice=1,fmt=1 "
	"--enc0:w=1920,h=1080,fps=30,gop=30,bps=2000000,rcmode=3,minqp=26,maxqp=48,qlevel=5,decomp=0,lowlatency=1,fmt=2 ",

	/********************************************************************************/
	/*  TEST COMMAND 6 - Slice Mode with RTT - Multi Channel                        */
	/********************************************************************************/
	/* AI denoise */
	"--ainr=0 "
	/* ch0 */
	"--isp0:w=1920,h=1080,fps=30,comp=0,slice=1,fmt=1 "
	"--enc0:w=1920,h=1080,fps=30,gop=30,bps=2000000,rcmode=3,minqp=26,maxqp=48,qlevel=5,decomp=0,lowlatency=1,fmt=2 "
	/* ch1 */
	"--isp1:w=1280,h=720,fps=30,fmt=1 "
	"--enc1:w=1280,h=720,fps=30,gop=30,bps=1000000,rcmode=3,minqp=26,maxqp=48,fmt=2 "
	/* ch3 */
	"--isp3:w=640,h=480,fps=10,fmt=1 "
	/* ch4 */
	"--isp4:w=640,h=480,fps=10,fmt=5 ",

	/********************************************************************************/
	/*  TEST COMMAND 7 - Slice Mode with Compression - Single Channel               */
	/********************************************************************************/
	/* AI denoise */
	"--ainr=0 "
	/* ch0 */
	"--isp0:w=1920,h=1080,fps=30,comp=1,slice=1,fmt=1 "
	"--enc0:w=1920,h=1080,fps=30,gop=30,bps=2000000,rcmode=3,minqp=26,maxqp=48,qlevel=5,decomp=1,lowlatency=1,fmt=2 ",

	/********************************************************************************/
	/*  TEST COMMAND 8 - Slice Mode with Compression - Multi Channel                */
	/********************************************************************************/
	/* AI denoise */
	"--ainr=0 "
	/* ch0 */
	"--isp0:w=1920,h=1080,fps=30,comp=1,slice=1,fmt=1 "
	"--enc0:w=1920,h=1080,fps=30,gop=30,bps=2000000,rcmode=3,minqp=26,maxqp=48,qlevel=5,decomp=1,lowlatency=1,fmt=2 "
	/* ch1 */
	"--isp1:w=1280,h=720,fps=30,fmt=1 "
	"--enc1:w=1280,h=720,fps=30,gop=30,bps=1000000,rcmode=3,minqp=26,maxqp=48,fmt=2 "
	/* ch3 */
	"--isp3:w=640,h=480,fps=10,fmt=1 "
	/* ch4 */
	"--isp4:w=640,h=480,fps=10,fmt=5 ",

	/********************************************************************************/
	/*  TEST COMMAND 9 - Real Sensor Test                                           */
	/********************************************************************************/
	/* AI denoise */
	"--ainr=0 "
	/* ch0 */
	"--isp0:w=1280,h=720,fps=30,fmt=1 "
	"--enc0:w=1280,h=720,fps=30,gop=30,bps=2000000,rcmode=3,minqp=26,maxqp=48,qlevel=5,fmt=5 "
	/* ch1 */
	"--isp1:w=640,h=480,fps=30,fmt=1 "
	"--enc1:w=640,h=480,fps=30,gop=30,bps=1000000,rcmode=3,minqp=26,maxqp=48,fmt=2 "
	/* ch2 */
	"--isp2:w=640,h=480,fps=1,fmt=1 "
	"--enc2:w=640,h=480,fps=1,gop=1,bps=1000000,rcmode=3,minqp=26,maxqp=48,qlevel=5,fmt=4 "
	/* ch3 */
	"--isp3:w=640,h=480,fps=10,fmt=1 "
	/* ch4 */
	"--isp4:w=640,h=480,fps=10,fmt=5 ",

};


static void display_parameters(isp_enc_ch_en_t *v_ch_en, isp_params_t isp_params[], enc_parameter_t enc_params[])
{
	for (int i = 0; i < VIDEO_ISP_VALID_CH; i++) {
		if (v_ch_en->isp[i]) {
			printf("ISP %d Parameters: AINR=%d, Width=%lu, Height=%lu, FPS=%lu, Slot=%lu, Buff=%lu, Stream ID=%lu, Format=%lu\n",
				   i, isp_params[i].ai_denoise_en, isp_params[i].width, isp_params[i].height, isp_params[i].fps, isp_params[i].slot_num,
				   isp_params[i].buff_num, isp_params[i].stream_id, isp_params[i].format);
		} else {
			// printf("ISP %d OFF\n", i);
		}
	}
	for (int i = 0; i < VIDEO_ENC_VALID_CH; i++) {
		if (v_ch_en->enc[i]) {
			printf("Encoder %d Parameters: Width=%lu, Height=%lu, FPS=%lu, GOP=%lu, BPS=%lu, RC Mode=%lu, Min QP=%lu, Max QP=%lu, Intra QP Delta=%d, Profile=%d, Level=%lu, Tier=%lu, QLevel=%d, Output Type=%lu\n",
				   i, enc_params[i].width, enc_params[i].height, enc_params[i].fps, enc_params[i].gop, enc_params[i].bps,
				   enc_params[i].rcMode, enc_params[i].minQp, enc_params[i].maxQp, enc_params[i].intraQpDelta,
				   enc_params[i].profile, enc_params[i].level, enc_params[i].tier, enc_params[i].qlevel,
				   enc_params[i].codec_fmt);
		} else {
			// printf("ENC %d OFF\n", i);
		}
	}
}

int video_test_case_idx = 0;

static void video_params_struct_setup(char *vcmd, isp_enc_ch_en_t *v_ch_en, isp_params_t *isp_params, enc_parameter_t *enc_params)
{
	// create a copy of the command string because strtok modifies the string in place
	char command_copy[2048];
	strncpy(command_copy, vcmd, sizeof(command_copy) - 1);
	command_copy[sizeof(command_copy) - 1] = '\0';

	parse_arguments(command_copy, v_ch_en, isp_params, enc_params);

	return;
}

/*
 * Encoder output buffer pool sizing
 *
 * mem_frame_size serves two roles:
 *   1. per-frame allocation size in the ring-buffer pool (memory_alloc)
 *   2. pEncIn->outBufSize passed to the hardware encoder (VCEncStrmEncode /
 *      JpegEncEncode).  If outBufSize is too small the hardware returns
 *      VCENC_OUTPUT_BUFFER_OVERFLOW; the error path cannot reset the encoder
 *      state, leading to a hang on subsequent encode calls.
 *
 * --- H.264 / H.265 ---
 *
 *     streamBufRefSize = 4 × input_frame_size,  clamped to [11 KB, 12 MB]
 *     input_frame_size = w × h × 3/2  (YUV420)
 *     VCENC_STREAM_MIN_BUF0_SIZE = 11 × 1024  (hevcencapi.h)
 *
 *   For a per-frame ring-buffer slot mem_frame_size is the larger of:
 *     a. Bitrate-based I-frame estimate: (peak_bps / 8 / fps) × ENC_I_FRAME_FACTOR
 *        Accurate under active rate control in steady-state.
 *     b. Resolution floor: w × h / 2
 *        Guards against startup I-frames (RC has no statistics yet) and
 *        quality-based modes (CRF/CQP) where bitrate is unconstrained.
 *     c. VCENC_STREAM_MIN_BUF0_SIZE = 11 KB (hardware minimum)
 *
 *   peak_bps derivation from rcMode:
 *     VBR/CVBR → maxBps (codec may reach ceiling)
 *     ABR      → bps × 1.5 (allows short-term burst above average)
 *     CBR/others → bps as-is
 *
 *   mem_total_size = (peak_bps / 8) × ENC_MEM_BUF_SEC + mem_frame_size × 2
 *   ENC_MEM_BUF_SEC is 1 s for 4K DEV and 3 s otherwise. This bitrate window
 *   is additional backlog capacity, not the maximum I-frame size. Each encode
 *   reserves mem_frame_size contiguous bytes; the completed allocation is
 *   resized to the actual bitstream length before downstream consumption.
 *   For 2160x3840 at 2 Mbps (peak_bps) and 20 fps, the aligned frame size is
 *   4,147,200 bytes and the 4K pool is 8,544,400 bytes before block truncation.
 *   The +2-frame margin already scales with the I-frame/resolution estimate.
 *   The shorter 4K window saves DDR for RAW/rotation/NV12 buffers. It is not a
 *   guaranteed stall tolerance: retained output and fragmentation can prevent
 *   the next contiguous frame allocation. A frame exceeding mem_frame_size
 *   is a separate hardware output-overflow risk. Validate scene changes and
 *   slow consumers on hardware before increasing the pool or frame estimate.
 *
 * --- JPEG ---
 *
 *     "1 byte/pixel is enough for most images.  Some extra is needed for
 *      testing purposes (noise input)"
 *     streamBufTotalSize = w × h × 1.2  (safety margin)
 *     JPEGENC_STREAM_MIN_BUF0_SIZE = 1 × 1024  (jpegencapi.h)
 *
 *   mem_frame_size = max(streamBufTotalSize, JPEGENC_STREAM_MIN_BUF0_SIZE)
 *   mem_total_size = mem_frame_size × 2  (capture / pipeline / spare)
 */

#define ENC_MEM_BLOCK_SIZE           (10 * 1024)
#if defined(CONFIG_4K_DEV) && (CONFIG_4K_DEV==1)
#define ENC_MEM_BUF_SEC              1              /* streaming buffer window in seconds; see sizing note above */
#else
#define ENC_MEM_BUF_SEC              3              /* streaming buffer window in seconds; see sizing note above */
#endif
#define ENC_I_FRAME_FACTOR           6              /* worst-case I-frame vs. average P-frame */
#define ENC_ABR_BURST_NUM            3
#define ENC_ABR_BURST_DEN            2              /* ABR peak headroom: avg × 1.5 */
/* VCENC_STREAM_MIN_BUF0_SIZE   defined in hevcencapi.h  (11 × 1024) */
/* JPEGENC_STREAM_MIN_BUF0_SIZE defined in jpegencapi.h  (1 × 1024)  */

static uint32_t enc_align_up(uint32_t val, uint32_t align)
{
	return ((val + align - 1) / align) * align;
}

void video_enc_buf_pool_setup(enc_parameter_t *enc_params)
{
	for (int i = 0; i < VIDEO_ENC_VALID_CH; i++) {
		enc_params[i].mem_info.mem_block_size = ENC_MEM_BLOCK_SIZE;

		if (enc_params[i].codec_fmt == VIDEO_JPEG) {
			/* official formula: 2 bytes/pixel covers worst-case JPEG output */
			uint32_t frame_ref = (uint32_t)((uint64_t)enc_params[i].width * enc_params[i].height * 6 / 5);
			if (frame_ref < JPEGENC_STREAM_MIN_BUF0_SIZE) {
				frame_ref = JPEGENC_STREAM_MIN_BUF0_SIZE;
			}
			enc_params[i].mem_info.mem_frame_size = enc_align_up(frame_ref, ENC_MEM_BLOCK_SIZE);
			/* 2-frame ring: capture + pipeline + spare */
			enc_params[i].mem_info.mem_total_size = enc_params[i].mem_info.mem_frame_size * 2;
		} else {
			/* derive peak bitrate from rate-control mode */
			uint32_t peak_bps = enc_params[i].bps;
			if ((enc_params[i].rcMode == ENC_VBR || enc_params[i].rcMode == ENC_CVBR) && enc_params[i].maxBps > peak_bps) {
				peak_bps = enc_params[i].maxBps;
			} else if (enc_params[i].rcMode == ENC_ABR) {
				peak_bps = peak_bps * ENC_ABR_BURST_NUM / ENC_ABR_BURST_DEN;
			}

			uint32_t fps = enc_params[i].fps > 0 ? enc_params[i].fps : 30;

			/* (a) bitrate-based I-frame estimate */
			uint32_t bps_est = (peak_bps / (8 * fps)) * ENC_I_FRAME_FACTOR;
			/* (b) resolution floor: proven safe bound (matches test bench reference) */
			uint32_t res_floor = enc_params[i].width * enc_params[i].height / 2;
			/* (c) hardware minimum from hevcencapi.h */
			uint32_t hw_min = VCENC_STREAM_MIN_BUF0_SIZE;

			uint32_t frame_ref = bps_est > res_floor ? bps_est : res_floor;
			if (frame_ref < hw_min) {
				frame_ref = hw_min;
			}
			enc_params[i].mem_info.mem_frame_size = enc_align_up(frame_ref, ENC_MEM_BLOCK_SIZE);

			enc_params[i].mem_info.mem_total_size = (peak_bps / 8) * ENC_MEM_BUF_SEC
													+ enc_params[i].mem_info.mem_frame_size * 2;
		}
	}
}

#if 0  /* original sizing (kept for reference) */
static void video_enc_buf_pool_setup_legacy(enc_parameter_t *enc_params)
{
	for (int i = 0; i < VIDEO_ENC_VALID_CH; i++) {
		enc_params[i].mem_info.mem_block_size = (10 * 1024);
		if (enc_params[i].codec_fmt == VIDEO_JPEG) {
			enc_params[i].mem_info.mem_frame_size = enc_params[i].width * enc_params[i].height * 3 / 2;
			enc_params[i].mem_info.mem_total_size = enc_params[i].mem_info.mem_frame_size * 2;
		} else {
			enc_params[i].mem_info.mem_frame_size = enc_params[i].width * enc_params[i].height / 2;
			enc_params[i].mem_info.mem_total_size = enc_params[i].mem_info.mem_frame_size + (enc_params[i].bps / 8) * 10;
		}
	}
}
#endif


static void video_params_from_cmdline(isp_enc_ch_en_t *v_ch_en, char *user_cmd, isp_params_t *isp_params, enc_parameter_t *enc_params)
{
	char *exe_cmd = NULL;
	if (user_cmd) {
		exe_cmd = user_cmd;
	} else {
		int num_commands = sizeof(video_cmdline) / sizeof(video_cmdline[0]);
		if (video_test_case_idx < 0 || video_test_case_idx >= num_commands) {
			printf("error: wrong test num. [num_cmd,test_id]=[%d,%d]\r\n", num_commands, video_test_case_idx);
			return;
		}
		// printf("%s\n", video_cmdline[video_test_case_idx]);
		printf("have %d command, exec %d\r\n", num_commands, video_test_case_idx);
		exe_cmd = (char *)video_cmdline[video_test_case_idx];
	}

	video_params_struct_setup(exe_cmd, v_ch_en, isp_params, enc_params);
	video_enc_buf_pool_setup(enc_params);

	return;
}

static int video_param_conflict_check(isp_params_t *isp_param, enc_parameter_t *enc_param)
{
	for (int ch = 0; ch < VIDEO_ENC_VALID_CH; ch++) {

		/* crop and scale-up check */
		if ((ch != 1) && enc_param[ch].cs.cropScaleEnable) {
			printf("error: crop and scale-up can only be enabled on ch1\r\n");
			goto error;
		}

		/* compression check */
		if ((ch != 0) && (isp_param[ch].compress_en | enc_param[ch].decompEnable)) {
			printf("error: compression can only be enabled on ch0\r\n");
			goto error;
		}
		if ((ch == 0) && (isp_param[ch].compress_en ^ enc_param[ch].decompEnable)) {
			printf("error: compression should be both enabled/disabled on isp and encoder simultaneously\r\n");
			goto error;
		}

		/* slice mode, low latency check */
		if ((ch != 0) && (isp_param[ch].slice_mode_en | enc_param[ch].lowLatencyEnable)) {
			printf("error: slice mode can only be enabled on ch0\r\n");
			goto error;
		}
		if ((ch == 0) && (isp_param[ch].slice_mode_en ^ enc_param[ch].lowLatencyEnable)) {
			printf("error: slice mode and low latency should be both enabled/disabled on isp and encoder simultaneously\r\n");
			goto error;
		}

	}

	/* slice mode and crop scale-up cannot be enabled simultaneously, it's too risky */
	if (isp_param[0].slice_mode_en & enc_param[1].cs.cropScaleEnable) {
		printf("error: slice mode and crop scale-up cannot be enabled simultaneously, it's too risky\r\n");
		goto error;
	}

	return 0;

error:
	return -1;
}

void default_video_param(isp_params_t *isp_param, enc_parameter_t *enc_param)
{
	memset(isp_param, 0, sizeof(isp_params_t) * VIDEO_ISP_VALID_CH);
	memset(enc_param, 0, sizeof(enc_parameter_t) * VIDEO_ENC_VALID_CH);

	/* V1 ISP */
	isp_param[0].stream_id = V1_STREAM_ID;
	isp_param[0].width = V1_WIDTH;
	isp_param[0].height = V1_HEIGHT;
	isp_param[0].fps = V1_FPS;
#if ISP_TUNING_MODE
	isp_param[0].slot_num = 1;
#else
	isp_param[0].slot_num = 2;
#endif
	isp_param[0].buff_num = 2;
	isp_param[0].format = ISP_FORMAT_YUV420_SEMIPLANAR;
	isp_param[0].ai_denoise_en = ISP_AI_DENOISE_EN;
	/* V1 ENC */
	enc_param[0].stream_id = V1_STREAM_ID;
	enc_param[0].width = V1_WIDTH;
	enc_param[0].height = V1_HEIGHT;
	enc_param[0].fps = V1_FPS;
	enc_param[0].gop = V1_FPS;
	enc_param[0].codec_fmt = V1_TYPE;
	enc_param[0].bps = V1_BITRATE;
	enc_param[0].minQp = ENC_MIN_QP;
	enc_param[0].maxQp = ENC_MAX_QP;
	enc_param[0].rcMode = ENC_RC_MODE;
	enc_param[0].qlevel = ENC_JPEG_QLEVEL;
	enc_param[0].osd_overlay[0].osdOverlayEnable = V1_OSD_EN;
	enc_param[0].osd_overlay[1].osdOverlayEnable = V1_OSD_EN;
	enc_param[0].mosaic[0].mosOverlayEnable = V1_MOSAIC_EN;
	enc_param[0].mosaic[1].mosOverlayEnable = V1_MOSAIC_EN;
	enc_param[0].cs.cropScaleEnable = V1_CROP_SCALE_EN;


	/* V2 ISP */
	isp_param[1].stream_id = V2_STREAM_ID;
	isp_param[1].width = V2_WIDTH;
	isp_param[1].height = V2_HEIGHT;
	isp_param[1].fps = V2_FPS;
	isp_param[1].slot_num = 2;
	isp_param[1].buff_num = 2;
	isp_param[1].format = ISP_FORMAT_YUV420_SEMIPLANAR;
	isp_param[1].ai_denoise_en = ISP_AI_DENOISE_EN;
	/* V2 ENC */
	enc_param[1].stream_id = V2_STREAM_ID;
	enc_param[1].width = V2_WIDTH;
	enc_param[1].height = V2_HEIGHT;
	enc_param[1].fps = V2_FPS;
	enc_param[1].gop = V2_FPS;
	enc_param[1].codec_fmt = V2_TYPE;
	enc_param[1].bps = V2_BITRATE;
	enc_param[1].minQp = ENC_MIN_QP;
	enc_param[1].maxQp = ENC_MAX_QP;
	enc_param[1].rcMode = ENC_RC_MODE;
	enc_param[1].qlevel = ENC_JPEG_QLEVEL;
	enc_param[1].osd_overlay[0].osdOverlayEnable = V2_OSD_EN;
	enc_param[1].osd_overlay[1].osdOverlayEnable = V2_OSD_EN;
	enc_param[1].mosaic[0].mosOverlayEnable = V2_MOSAIC_EN;
	enc_param[1].mosaic[1].mosOverlayEnable = V2_MOSAIC_EN;


	/* V3 ISP */
	isp_param[2].stream_id = V3_STREAM_ID;
	isp_param[2].width = V3_WIDTH;
	isp_param[2].height = V3_HEIGHT;
	isp_param[2].fps = V3_FPS;
	isp_param[2].slot_num = 2;
	isp_param[2].buff_num = 2;
	isp_param[2].format = ISP_FORMAT_YUV420_SEMIPLANAR;
	isp_param[2].ai_denoise_en = ISP_AI_DENOISE_EN;
	/* V3 ENC */
	enc_param[2].stream_id = V3_STREAM_ID;
	enc_param[2].width = V3_WIDTH;
	enc_param[2].height = V3_HEIGHT;
	enc_param[2].fps = V3_FPS;
	enc_param[2].gop = V3_FPS;
	enc_param[2].codec_fmt = V3_TYPE;
	enc_param[2].bps = V3_BITRATE;
	enc_param[2].minQp = ENC_MIN_QP;
	enc_param[2].maxQp = ENC_MAX_QP;
	enc_param[2].rcMode = ENC_RC_MODE;
	enc_param[2].qlevel = ENC_JPEG_QLEVEL;


	/* V4 ISP */
	isp_param[3].stream_id = V4_STREAM_ID;
	isp_param[3].width = V4_WIDTH;
	isp_param[3].height = V4_HEIGHT;
	isp_param[3].fps = V4_FPS;
	isp_param[3].slot_num = 2;
	isp_param[3].buff_num = 2;
	isp_param[3].format = ISP_FORMAT_YUV420_SEMIPLANAR;
	isp_param[3].ai_denoise_en = ISP_AI_DENOISE_EN;


	/* V5 ISP */
	isp_param[4].stream_id = V5_STREAM_ID;
	isp_param[4].width = V5_WIDTH;
	isp_param[4].height = V5_HEIGHT;
	isp_param[4].fps = V5_FPS;
	isp_param[4].slot_num = 2;
	isp_param[4].buff_num = 2;
	isp_param[4].format = ISP_FORMAT_RGB888_PLANAR;
	isp_param[4].ai_denoise_en = ISP_AI_DENOISE_EN;

}

int video_app_params_parsing(isp_enc_ch_en_t *v_ch_en, char *user_cmd, isp_params_t *isp_param, enc_parameter_t *enc_param)
{
	/* fill the structure by default parameter */
	default_video_param(isp_param, enc_param);

	/* if video command line is not NULL, parse the command to overwrite isp and encoder structure */
	if (user_cmd != NULL) {
		video_params_from_cmdline(v_ch_en, user_cmd, isp_param, enc_param);
	}

	/* params conflict check */
	if (video_param_conflict_check(isp_param, enc_param) != 0) {
		printf("video param have conflict! check fail\r\n");
		return -1;
	}

	display_parameters(v_ch_en, isp_param, enc_param);

	printf("video app params parsing done.");
	return 0;
}

u32 CmdVideoParamTest(IN u16 argc, IN u8 *argv[])
{
	(void)argc;
	if (_strcmp((const char *)argv[0], "list") == 0) {
		int num_commands = sizeof(video_cmdline) / sizeof(video_cmdline[0]);
		for (int i = 0; i < num_commands; i++) {
			printf("=========================================\r\n");
			printf("TEST CASE %d\r\n", i);
			printf("=========================================\r\n");
			printf("%s\r\n\r\n", video_cmdline[i]);
		}
	} else if (_strcmp((const char *)argv[0], "set") == 0) {
		int idx = atoi((const char *)argv[1]);
		int num_commands = sizeof(video_cmdline) / sizeof(video_cmdline[0]);
		if (idx < 0 || idx >= num_commands) {
			printf("error: wrong video params index\r\n");
		} else {
			printf("set video params index to %d\r\n", idx);
			video_test_case_idx = idx;
		}
	}

	return _TRUE;
}

CMD_TABLE_DATA_SECTION
const COMMAND_TABLE   video_param_test_cmd[] = {
	{
		"VPM", CmdVideoParamTest
	},
};

#if defined(CONFIG_FAST_CAMERA_START) && (CONFIG_FAST_CAMERA_START != 0)
#include "video_fcs_params.h"

/* VIDEO_RGB maps to RGB888 planar (5); all others use NV12/YUV420 (1). */
static uint32_t fcs_isp_fmt(uint32_t video_fmt)
{
	return (video_fmt == VIDEO_RGB) ? 5u : 1u;
}

/*
 * Fill isp_param / enc_param arrays directly from the PSRAM FCS struct,
 * bypassing the string command parser.
 *
 * The caller must have validated p with fcs_params_valid() before calling.
 * Channels V1-V3 have both an ISP channel and an encoder.
 * Channels V4-V5 have only an ISP channel (raw NV12 / RGB output).
 */
int video_app_fcs_params_load(const fcs_persistent_params_t *p,
							  isp_enc_ch_en_t *v_ch_en,
							  isp_params_t *isp_param,
							  enc_parameter_t *enc_param)
{
	int i;

	default_video_param(isp_param, enc_param);
	memset(v_ch_en, 0, sizeof(isp_enc_ch_en_t));

	for (i = 0; i < VIDEO_ISP_VALID_CH; i++) {
		const fcs_ch_params_t *c = &p->ch[i];
		if ((p->channel_enable_mask & (1UL << i)) == 0) {
			continue;
		}

		isp_param[i].stream_id    = (uint32_t)i;
		isp_param[i].width        = c->width;
		isp_param[i].height       = c->height;
		isp_param[i].fps          = c->fps;
		isp_param[i].slot_num     = 2;
		isp_param[i].format       = fcs_isp_fmt(c->format);
		isp_param[i].ai_denoise_en = (int)p->ainr_ena;
		v_ch_en->isp[i] = 1;
	}

	for (i = 0; i < VIDEO_ENC_VALID_CH; i++) {
		const fcs_ch_params_t *c = &p->ch[i];
		if ((p->channel_enable_mask & (1UL << i)) == 0) {
			continue;
		}

		/* channels with VIDEO_NV12 or VIDEO_RGB have no encoder */
		if (c->format == VIDEO_NV12 || c->format == VIDEO_RGB) {
			continue;
		}

		enc_param[i].stream_id  = (uint32_t)i;
		enc_param[i].codec_fmt  = c->format;
		enc_param[i].width      = c->width;
		enc_param[i].height     = c->height;
		enc_param[i].fps        = c->fps;
		enc_param[i].gop        = c->gop;
		enc_param[i].bps        = c->bps;
		enc_param[i].rcMode     = c->rc_mode;
		enc_param[i].minQp      = c->min_qp;
		enc_param[i].maxQp      = c->max_qp;
		enc_param[i].qlevel     = (int)c->qlevel;
		v_ch_en->enc[i] = 1;
	}

	/* same validation the string-parsing path enforces before applying params */
	if (video_param_conflict_check(isp_param, enc_param) != 0) {
		return -1;
	}

	video_enc_buf_pool_setup(enc_param);
	return 0;
}
#endif
