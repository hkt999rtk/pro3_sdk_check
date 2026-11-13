#include "ameba_soc.h"

#ifndef TS_HAL_H
#define TS_HAL_H

#define TS_DEBUG_LOG 1

#define TS_BASE TILE_SCALER_REG_BASE

#define TS_INT_ENB_REG		0x00
#define TS_INT_MASK_REG		0x04

#define TS_OPERATION_REG	0x0C
#define TS_IMG_FORMAT_REG	0x10
#define TS_CSC_PAR0_REG		0x14
#define TS_CSC_PAR1_REG		0x18
#define TS_CSC_PAR2_REG		0x1C
#define TS_CSC_COEF0_REG	0x20
#define TS_CSC_COEF1_REG	0x24
#define TS_CSC_COEF2_REG	0x28
#define TS_CSC_COEF3_REG	0x2C
#define TS_CSC_COEF4_REG	0x30
#define TS_COMPN0_PAR0_REG	0x34
#define TS_COMPN0_PAR1_REG	0x38

#define TS_COMPN0_LPF0_REG	0x40
#define TS_COMPN0_LPF1_REG	0x44
#define TS_COMPN0_LPF2_REG	0x48
#define TS_COMPN0_HOR0_REG	0x4C
#define TS_COMPN0_HOR1_REG	0x50
#define TS_COMPN0_HOR2_REG	0x54
#define TS_COMPN0_HOR3_REG	0x58
#define TS_COMPN0_HOR4_REG	0x5C
#define TS_COMPN0_HOR5_REG	0x60
#define TS_COMPN0_HOR6_REG	0x64
#define TS_COMPN0_HOR7_REG	0x68
#define TS_COMPN0_HOR8_REG	0x6C
#define TS_COMPN0_HOR9_REG	0x70
#define TS_COMPN0_HOR10_REG	0x74
#define TS_COMPN0_IN_TILE_HEIGHT_REG	0x78
#define TS_COMPN0_IN_TILE_WODTH_REG		0x7C
#define TS_COMPN0_VER0_REG	0x80
#define TS_COMPN0_VER1_REG	0x84
#define TS_COMPN0_VER2_REG	0x88
#define TS_COMPN0_VER3_REG	0x8C
#define TS_COMPN0_VER4_REG	0x90
#define TS_COMPN0_VER5_REG	0x94
#define TS_COMPN0_VER6_REG	0x98
#define TS_COMPN0_VER7_REG	0x9C
#define TS_COMPN0_VER8_REG	0xA0
#define TS_COMPN0_VER9_REG	0xA4
#define TS_COMPN0_VER10_REG	0xA8
#define TS_COMPN0_IN_ADDR_REG	0xAC
#define TS_COMPN0_OUT_ADDR_REG	0xB0
#define TS_COMPN0_STRIDE_REG	0xB4
#define TS_COMPN0_OUT_TILE_SIZE_REG	0xB8
#define TS_COMPN0_WRAP_STRIDE_REG	0xBC

#define TS_COMPN1_PAR0_REG	0xC4
#define TS_COMPN1_PAR1_REG	0xC8

#define TS_COMPN1_LPF0_REG	0xD0
#define TS_COMPN1_LPF1_REG	0xD4
#define TS_COMPN1_LPF2_REG	0xD8
#define TS_COMPN1_HOR0_REG	0xDC
#define TS_COMPN1_HOR1_REG	0xE0
#define TS_COMPN1_HOR2_REG	0xE4
#define TS_COMPN1_HOR3_REG	0xE8
#define TS_COMPN1_HOR4_REG	0xEC
#define TS_COMPN1_HOR5_REG	0xF0
#define TS_COMPN1_HOR6_REG	0xF4
#define TS_COMPN1_HOR7_REG	0xF8
#define TS_COMPN1_HOR8_REG	0xFC
#define TS_COMPN1_HOR9_REG	0x100
#define TS_COMPN1_HOR10_REG	0x104
#define TS_COMPN1_IN_TILE_HEIGHT_REG	0x108
#define TS_COMPN1_IN_TILE_WODTH_REG		0x10C
#define TS_COMPN1_VER0_REG	0x110
#define TS_COMPN1_VER1_REG	0x114
#define TS_COMPN1_VER2_REG	0x118
#define TS_COMPN1_VER3_REG	0x11C
#define TS_COMPN1_VER4_REG	0x120
#define TS_COMPN1_VER5_REG	0x124
#define TS_COMPN1_VER6_REG	0x128
#define TS_COMPN1_VER7_REG	0x12C
#define TS_COMPN1_VER8_REG	0x130
#define TS_COMPN1_VER9_REG	0x134
#define TS_COMPN1_VER10_REG	0x138
#define TS_COMPN1_IN_ADDR_REG	0x13C
#define TS_COMPN1_OUT_ADDR_REG	0x140
#define TS_COMPN1_STRIDE_REG	0x144
#define TS_COMPN1_OUT_TILE_SIZE_REG	0x148
#define TS_COMPN1_WRAP_STRIDE_REG	0x14C

#define TS_COMPN2_PAR0_REG	0x154
#define TS_COMPN2_PAR1_REG	0x158

#define TS_COMPN2_LPF0_REG	0x160
#define TS_COMPN2_LPF1_REG	0x164
#define TS_COMPN2_LPF2_REG	0x168
#define TS_COMPN2_HOR0_REG	0x16C
#define TS_COMPN2_HOR1_REG	0x170
#define TS_COMPN2_HOR2_REG	0x174
#define TS_COMPN2_HOR3_REG	0x178
#define TS_COMPN2_HOR4_REG	0x17C
#define TS_COMPN2_HOR5_REG	0x180
#define TS_COMPN2_HOR6_REG	0x184
#define TS_COMPN2_HOR7_REG	0x188
#define TS_COMPN2_HOR8_REG	0x18C
#define TS_COMPN2_HOR9_REG	0x190
#define TS_COMPN2_HOR10_REG	0x194
#define TS_COMPN2_IN_TILE_HEIGHT_REG	0x198
#define TS_COMPN2_IN_TILE_WODTH_REG		0x19C
#define TS_COMPN2_VER0_REG	0x1A0
#define TS_COMPN2_VER1_REG	0x1A4
#define TS_COMPN2_VER2_REG	0x1A8
#define TS_COMPN2_VER3_REG	0x1AC
#define TS_COMPN2_VER4_REG	0x1B0
#define TS_COMPN2_VER5_REG	0x1B4
#define TS_COMPN2_VER6_REG	0x1B8
#define TS_COMPN2_VER7_REG	0x1BC
#define TS_COMPN2_VER8_REG	0x1C0
#define TS_COMPN2_VER9_REG	0x1C4
#define TS_COMPN2_VER10_REG	0x1C8
#define TS_COMPN2_IN_ADDR_REG	0x1CC
#define TS_COMPN2_OUT_ADDR_REG	0x1D0
#define TS_COMPN2_STRIDE_REG	0x1D4
#define TS_COMPN2_OUT_TILE_SIZE_REG	0x1D8
#define TS_LINE_CNT_THR_REG	0x1DC
#define TS_TILE_CNT_THR_REG	0x1E0
#define TS_COMPN2_WRAP_STRIDE_REG	0x1E4

#define COOR_FIX_FR 12
#define COOR_FIX_INT 5
#define FIXED_U5_12CONST(A,B) ((unsigned long)((A<<COOR_FIX_FR) + ((B + (1/(pow(2, COOR_FIX_FR+1))))*(pow(2, COOR_FIX_FR)))) & 0x1FFFF)

#define INT_START_FIX_FR 9
#define FIXED_S5_9CONST(A,B) ((signed int) (A>=0 ? (((A)<<INT_START_FIX_FR) + ((B + (1/(pow(2, INT_START_FIX_FR+1))))*(pow(2, INT_START_FIX_FR)))) : (((A)<<INT_START_FIX_FR) + ((B - (1/(pow(2, INT_START_FIX_FR+1))))*(pow(2, INT_START_FIX_FR))))) & 0x3FFF)

#define IMG_FIX_FR 0
#define FIXED_S9_0CONST(A,B) ((signed int) (A>=0 ? ((A<<IMG_FIX_FR) + ((B + (1/(pow(2, IMG_FIX_FR+1))))*(pow(2, IMG_FIX_FR)))) : (((A)<<IMG_FIX_FR) + ((B - (1/(pow(2, IMG_FIX_FR+1))))*(pow(2, IMG_FIX_FR))))) & 0x1FF)
#define FIXED_S10_0CONST(A,B) ((signed int) (A>=0 ? ((A<<IMG_FIX_FR) + ((B + (1/(pow(2, IMG_FIX_FR+1))))*(pow(2, IMG_FIX_FR)))) : (((A)<<IMG_FIX_FR) + ((B - (1/(pow(2, IMG_FIX_FR+1))))*(pow(2, IMG_FIX_FR))))) & 0x3FF)
#define FIXED_S11_0CONST(A,B) ((signed int) (A>=0 ? ((A<<IMG_FIX_FR) + ((B + (1/(pow(2, IMG_FIX_FR+1))))*(pow(2, IMG_FIX_FR)))) : (((A)<<IMG_FIX_FR) + ((B - (1/(pow(2, IMG_FIX_FR+1))))*(pow(2, IMG_FIX_FR))))) & 0x7FF)
#define FIXED_S14_0CONST(A,B) ((signed int) (A>=0 ? ((A<<IMG_FIX_FR) + ((B + (1/(pow(2, IMG_FIX_FR+1))))*(pow(2, IMG_FIX_FR)))) : (((A)<<IMG_FIX_FR) + ((B - (1/(pow(2, IMG_FIX_FR+1))))*(pow(2, IMG_FIX_FR))))) & 0x3FFF)
#define FIXED_S17_0CONST(A,B) ((signed int) (A>=0 ? ((A<<IMG_FIX_FR) + ((B + (1/(pow(2, IMG_FIX_FR+1))))*(pow(2, IMG_FIX_FR)))) : (((A)<<IMG_FIX_FR) + ((B - (1/(pow(2, IMG_FIX_FR+1))))*(pow(2, IMG_FIX_FR))))) & 0x1FFFF)

#define CSC_FIX_FR 8
#define FIXED_S3_8CONST(A,B) ((signed int) (A>=0 ? ((A<<CSC_FIX_FR) + ((B + (1/(pow(2, CSC_FIX_FR+1))))*(pow(2, CSC_FIX_FR)))) : (((A)<<CSC_FIX_FR) + ((B - (1/(pow(2, CSC_FIX_FR+1))))*(pow(2, CSC_FIX_FR))))) & 0x7FF)
#define FIXED_S2_8CONST(A,B) ((signed int) (A>=0 ? ((A<<CSC_FIX_FR) + ((B + (1/(pow(2, CSC_FIX_FR+1))))*(pow(2, CSC_FIX_FR)))) : (((A)<<CSC_FIX_FR) + ((B - (1/(pow(2, CSC_FIX_FR+1))))*(pow(2, CSC_FIX_FR))))) & 0x3FF)

typedef enum {					//YW		YH			UW		UH			VW		VH
	TS_FORMAT_P420 = (0 << 28) | ((2 << 24) | (2 << 16)) | ((1 << 12) | (1 << 8)) | ((1 << 4) | (1 << 0)),
	TS_FORMAT_P422 = (0 << 28) | ((2 << 24) | (2 << 16)) | ((1 << 12) | (2 << 8)) | ((1 << 4) | (2 << 0)),
	TS_FORMAT_P444 = (0 << 28) | ((2 << 24) | (2 << 16)) | ((2 << 12) | (2 << 8)) | ((2 << 4) | (2 << 0)),
	TS_FORMAT_UYVY = (1 << 28) | ((2 << 24) | (2 << 16)) | ((1 << 12) | (2 << 8)) | ((1 << 4) | (2 << 0)),
	TS_FORMAT_YUYV = (2 << 28) | ((2 << 24) | (2 << 16)) | ((1 << 12) | (2 << 8)) | ((1 << 4) | (2 << 0)),
	TS_FORMAT_YUV444 = (3 << 28) | ((2 << 24) | (2 << 16)) | ((2 << 12) | (2 << 8)) | ((2 << 4) | (2 << 0)),
	TS_FORMAT_S420 = (6 << 28) | ((2 << 24) | (2 << 16)) | ((1 << 12) | (1 << 8)) | ((1 << 4) | (1 << 0)),
	TS_FORMAT_S422 = (6 << 28) | ((2 << 24) | (2 << 16)) | ((1 << 12) | (2 << 8)) | ((1 << 4) | (2 << 0)),
	TS_FORMAT_S444 = (6 << 28) | ((2 << 24) | (2 << 16)) | ((2 << 12) | (2 << 8)) | ((2 << 4) | (2 << 0)),
	//RW		RH			GW		GH			BW		BH
	TS_FORMAT_RGB888 = (4 << 28) | ((2 << 24) | (2 << 16)) | ((2 << 12) | (2 << 8)) | ((2 << 4) | (2 << 0)),
	TS_FORMAT_ARGB8888 = (5 << 28) | ((2 << 24) | (2 << 16)) | ((2 << 12) | (2 << 8)) | ((2 << 4) | (2 << 0)),

	TS_FORMAT_P_RGB888 = (7 << 28) | ((2 << 24) | (2 << 16)) | ((2 << 12) | (2 << 8)) | ((2 << 4) | (2 << 0)),

} TS_FORMAT;

#define TS_P_RGB888_MASK 0x0F

typedef enum {
	INPUT_FORMAT = 0,
	OUTPUT_FORMAT,
	SCALING_ORDER,
	CSC_ORDER,
	IN0_HEIGHT,
	IN0_WIDTH,
	OUT0_HEIGHT,
	OUT0_WIDTH,
	IN1_HEIGHT,
	IN1_WIDTH,
	OUT1_HEIGHT,
	OUT1_WIDTH,
	IN2_HEIGHT,
	IN2_WIDTH,
	OUT2_HEIGHT,
	OUT2_WIDTH,
	INTP_H0_STEP,
	INTP_H1_STEP,
	INTP_H2_STEP,
	INTP_V0_STEP,
	INTP_V1_STEP,
	INTP_V2_STEP,
	START_H0,
	START_H1,
	START_H2,
	START_V0,
	START_V1,
	START_V2,
	CSC_MODE,
	C00,
	C01,
	C02,
	C10,
	C11,
	C12,
	C20,
	C21,
	C22,
	IF0,
	IF1,
	IF2,
	OF0,
	OF1,
	OF2,
	C00_,
	C01_,
	C02_,
	C10_,
	C11_,
	C12_,
	C20_,
	C21_,
	C22_,
	IF0_,
	IF1_,
	IF2_,
	OF0_,
	OF1_,
	OF2_,
	LPF_H0,
	LPF_H1,
	LPF_H2,
	LPF_V0,
	LPF_V1,
	LPF_V2,
	L00,
	L01,
	L02,
	L03,
	INTP00,
	INTP01,
	INTP02,
	INTP03,
	INTP10,
	INTP11,
	INTP12,
	INTP13,
	INTP20,
	INTP21,
	INTP22,
	INTP23,
	INTP30,
	INTP31,
	INTP32,
	INTP33,
	INTP40,
	INTP41,
	INTP42,
	INTP43,
	PARAM_NUM,
} TS_INPUT_PARAM;

typedef enum {
	Output_mode = 0,
	C_in_tile_height_0,
	C_in_tile_height_1,
	C_in_tile_height_2,
	C_in_tile_width_0,
	C_in_tile_width_1,
	C_in_tile_width_2,
	C_vint_mode_0,
	C_vint_mode_1,
	C_vint_mode_2,
	C_hint_mode_0,
	C_hint_mode_1,
	C_hint_mode_2,
	C_inaddr_0,
	C_inaddr_1,
	C_inaddr_2,
	C_outaddr_0,
	C_outaddr_1,
	C_outaddr_2,
	C_outstride_0,
	C_outstride_1,
	C_outstride_2,
	C_instride_0,
	C_instride_1,
	C_instride_2,
	C_out_tile_height_0,
	C_out_tile_height_1,
	C_out_tile_height_2,
	C_out_tile_width_0,
	C_out_tile_width_1,
	C_out_tile_width_2,
	OTHER_PARAM_NUM,
} TS_INPUT_OTHER_PARAM;

typedef struct {
	char *input_file;
	char *output_file;
	u32 in_f;
	u32 out_f;
} ts_test_case_t;

typedef struct {
	u32 id;
	char *out_str;
	int val;
} ts_input_param_t;

struct ts_info_t {
	int in_f;
	int out_f;
	u8 *buf;
	u8 *out_buf;
	int in_w;
	int in_h;
	int out_w;
	int out_h;
	int lpf_en;
	int tile_en;
	int start_0;
	int scale_order;
	int wrap_en;
	int in_frame_w; //stride_in
	int in_frame_h;
	int in_ROI_left;
	int in_ROI_top;
	int in_stride_negative;
	int out_frame_w; //stride_out
	int out_frame_h;
	int out_ROI_left;
	int out_ROI_top;
	int out_stride_negative;
	int tile_w;
	int tile_h;
	int wrap_num;
};

int TS_Init(void);
int TS_Deinit(void);
u32 TS_GetINTStatus(void);
void TS_INTClear(u32 val);
void TS_DumpInputParam(void);
void TS_ConvertIF2OF(struct ts_info_t ts_info);

#endif
