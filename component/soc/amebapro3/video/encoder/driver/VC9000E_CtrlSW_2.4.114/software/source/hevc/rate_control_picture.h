/*------------------------------------------------------------------------------
--                                                                                                                               --
--       This software is confidential and proprietary and may be used                                   --
--        only as expressly authorized by a licensing agreement from                                     --
--                                                                                                                               --
--                            Verisilicon.                                                                                    --
--                                                                                                                               --
--                   (C) COPYRIGHT 2014 VERISILICON                                                            --
--                            ALL RIGHTS RESERVED                                                                    --
--                                                                                                                               --
--                 The entire notice above must be reproduced                                                 --
--                  on all copies and should not be removed.                                                    --
--                                                                                                                               --
--------------------------------------------------------------------------------*/

#ifndef RATE_CONTROL_PICTURE_H
#define RATE_CONTROL_PICTURE_H

#include "base_type.h"
#include "sw_picture.h"
#include "enccommon.h"
#include "hevcSei.h"

#ifdef VSB_TEMP_TEST
#include "video_statistic.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

//#include "enccommon.h"
//#include "H264Sei.h"


//#define RCP_PRINT_INTFO

#ifdef RCP_FIXED_POINT_OPT
#define FIXED_FRACTIONAL_BITS 24
#define LL_FIXED	(1<<FIXED_FRACTIONAL_BITS)
#define LL_HALF 	(1<<(FIXED_FRACTIONAL_BITS-1))
#define RCP_DATA_TYPE(x) ((i64)(x * (float)LL_FIXED + 0.5))

#define Q31_FIXED_BITS	31
#define Q31_FIXED_ONE	((i64)1<<Q31_FIXED_BITS)
#define Q31_FIXED_HALF  ((i64)1<<(Q31_FIXED_BITS-1))
#else
#define LL_FIXED	1.0
#define LL_HALF 	0.5
#define RCP_DATA_TYPE(x) x
#define Q31_FIXED_ONE	1.0
#define Q31_FIXED_HALF  0.5
#endif

#define RC_SIMPLE_MODEL
#define RC_CPB_QP_REFINE

enum { VCENCRC_OVERFLOW = -1 };

#define RC_CBR_HRD \
  1 /* 1 = Constant bit rate model. Must use filler
                         * data to conform */
//#define FEEDBACK_QPDELTA

#define CTRL_LEVELS 7       /* DO NOT CHANGE THIS */
#define CHECK_POINTS_MAX 10 /* DO NOT CHANGE THIS */
#define RC_TABLE_LENGTH 10  /* DO NOT CHANGE THIS */
#define INTRA_QPDELTA_DEFAULT (-3)

#ifndef CTBRC_STRENGTH
#define QP_FRACTIONAL_BITS 0
#else
#define QP_FRACTIONAL_BITS 8
#endif

#define QP_DEFAULT 26

#define TOL_CTB_RC_FIX_POINT 7

#define RC_MOVING_AVERAGE_FRAMES 60

#define LEAST_MONITOR_FRAME 3
#define I32_MAX ((i32)0x7FFFFFFF)
#define CLIP_I32_ADD(a, b) ((a) > (I32_MAX - (b)) ? I32_MAX : (a) + (b))

#define IS_CTBRC(x) ((x)&3)
#define IS_CTBRC_FOR_QUALITY(x) ((x)&1)
#define IS_CTBRC_FOR_BITRATE(x) ((x)&2)
#define CLR_CTBRC_FOR_BITRATE(x) \
  { (x) &= (~2); }

#define CTB_RC_QP_STEP_FIXP 16
#define CTB_RC_ROW_FACTOR_FIXP 16
enum {
  SIMPLE_SCENE,
  NORMAL_SCENE,
  COMPLEX_SCENE,
};

typedef enum {
  NORMAL_MOTION,
  STATIC_MOTION,
  STATIC_TO_LARGE_MOTION,
  LARGE_TO_STATIC_MOTION,
} motion_type;

typedef enum {
  LAYER_B = 0,
  LAYER_P,
  LAYER_I,
  LAYER_BREF_0,
  LAYER_BREF_1,
  LAYER_BREF_2,
  
} predLayer;

typedef struct {
  i32 frame[120];
  i32 length;
  i32 count;
  i32 pos;
  i32 frameRateNumer;
  i32 frameRateDenom;
} rc_ma_s;
typedef struct {
  u32 intraCu8Num[120];
  u32 skipCu8Num[120];
  u32 PBFrame4NRdCost[120];
  i32 length;
  i32 count;
  i32 pos;
  u32 average_intraCu8Num;
  u32 average_skipCu8Num;
  u32 average_PBFrame4NRdCost;
} rc_ma_char;
typedef struct {
  i64 a1;                        /* model parameter */
  i64 a2;                        /* model parameter */
  i32 qp_prev;                   /* previous QP */
  i32 qs[RC_TABLE_LENGTH + 1];   /* quantization step size */
  i32 bits[RC_TABLE_LENGTH + 1]; /* Number of bits needed to code residual */
  i32 pos;                       /* current position */
  i32 len;                       /* current lenght */
  i32 zero_div;                  /* a1 divisor is 0 */
  i32 cbr;
  i32 weight;
  i32 prevFrameBits;
  i32 prevTargetBits;
} linReg_s;

typedef struct {
  i32 wordError[CTRL_LEVELS];          /* Check point error bit */
  i32 qpChange[CTRL_LEVELS];           /* Check point qp difference */
  i32 wordCntTarget[CHECK_POINTS_MAX]; /* Required bit count */
  i32 wordCntPrev[CHECK_POINTS_MAX];   /* Real bit count */
  i32 checkPointDistance;
  i32 checkPoints;
} hevcQpCtrl_s;

/* Virtual buffer */
typedef struct {
  i32 bufferSize;      /* size of the virtual buffer */
  i32 movingMinRate;   /* moving min bitrate */
  i32 movingMaxRate;   /* moving max bitrate */
  i32 maxBitRate;      /* cpb max bitrate */
  i32 bufferRate;      /* bitPerPic with maxBitRate */
  i32 bitRate;         /* input bit rate per second */
  i32 bitPerPic;       /* average number of bits per picture */
  i32 picTimeInc;      /* timeInc since last coded picture */
  i32 timeScale;       /* input frame rate numerator */
  i32 unitsInTic;      /* input frame rate denominator */
  i32 virtualBitCnt;   /* virtual (channel) bit count */
  i32 realBitCnt;      /* real bit count */
  i32 bufferOccupancy; /* number of bits in the buffer */
  i32 skipFrameTarget; /* how many frames should be skipped in a row */
  i32 skippedFrames;   /* how many frames have been skipped in a row */
  i32 nonZeroTarget;
  i32 bucketFullness; /* Leaky Bucket fullness */
  i32 bucketLevel;    /* Leaky Bucket fullness + virtualBitCnt */
  /* new rate control */
  i32 windowRem;
  i32 seconds;        /* Full seconds elapsed */
  i32 averageBitRate; /* This buffer average bitrate for full seconds */
} rcVirtualBuffer_s;

typedef struct {
  i32 x0;
  i32 x1;
  i32 xMin;
  i32 started;
  i32 preFrameMad;
  ptr_t ctbMemPreAddr;
  u32* ctbMemPreVirtualAddr;
  i32 frameCnt;
  i32 idxType;
  i32 refCount;
  i32 valid;
} ctbRcModel_s;

typedef struct {
  ptr_t ctbMemCurAddr;
  u32* ctbMemCurVirtualAddr;
  u32 ctbRcModelsIdx;
} ctbRcCur_s;

typedef struct {
  ctbRcModel_s models[2+MAX_CORE_NUM]; //for 3(I,P,B) + max_core_num -1
  i32 qpSumForRc;
  i32 qpStep;
  i32 ctbRcRowQpDeltaRange;
  i32 rowFactor;
  ctbRcCur_s Cur[MAX_CORE_NUM];  //for max_core_num
  i32 updateModelIdx; //Which model index to save current reg data to
} ctbRateControl_s;

typedef struct {
  RCP_64bit coeffMin;
  RCP_64bit coeff;
  RCP_64bit count;
  RCP_64bit decay;
  RCP_64bit offset;

  RCP_64bit shortTermCost;
  RCP_64bit shortTermCostDecay;
  RCP_64bit shortTermCostCount;
  RCP_64bit cost;

  RCP_64bit longTermCost;
  RCP_64bit longTermCostDecay;
  RCP_64bit longTermCostCount;

  i32 qp;
  i32 qpPrev;            // previous qp of same predId
  i32 prevFrameBits;     // previous frameBits of same predId
  i32 prevTargetBits;    // previous targetPicSize of same predId
  i8 qpDelta;            // hierarchical qp delta for current frame pred type
  i8 qpDeltaPrev;
  i8 usePrevQp;
} rcPredictor;

typedef struct {
  true_e picRc;
  u32 ctbRc;         /* ctb header qp can vary, check point rc */
  true_e picSkip;    /* Frame Skip enable */
  true_e fillerData; /* if enable fiiler data when hrd off */
  i32 tolUnderflow;  /* tolerance of underflow percentage for better objective quality */
  true_e hrd;        /* HRD restrictions followed or not */
  true_e vbr;        /* Variable Bit Rate Control by qpMin */
  i32 vbrOn;         /* vbr mode turn on or off by condition */
  i32 rcMode;        /* one of CVBR, CBR, VBR, ABR, CRF, CQP */
  u32 fillerIdx;
  i32 picArea;
  i32 ctbPerPic;   /* Number of macroblock per picture */
  i32 ctbRows;     /* ctb rows in picture */
  i32 ctbCols;     /* ctb columns in picture */
  i32 ctbSize;     /* ctb size */
  i32 coeffCntMax; /* Number of coeff per picture */
  i32 nonZeroCnt;
  i32 srcPrm; /* Source parameter */
  i32 qpSum;  /* Qp sum counter */
  i32 qpNum;
  RCP_32bit averageQp;
  u32 sliceTypeCur;
  u32 sliceTypePrev;
  true_e frameCoded; /* Pic coded information */
  true_e skipGop;    /* skip rest frames in current GOP */
  i32 fixedQp;       /* Pic header qp when fixed */
  i32 qpHdr;         /* Pic header qp of current voded picture */
  i32 qpMin;         /* Pic header minimum qp for current picture */
  i32 qpMax;         /* Pic header maximum qp for current picture */
  i32 qpMinI;        /* Pic header minimum qp for I frame, user set */
  i32 qpMaxI;        /* Pic header maximum qp for I frame, user set */
  i32 qpMinPB;       /* Pic header minimum qp for P/B frame, user set */
  i32 qpMaxPB;       /* Pic header maximum qp for P/B frame, user set */
  i32 qpHdrPrev;     /* Pic header qp of previous coded picture */
  i32 qpLastCoded;   /* Quantization parameter of last coded mb */
  i32 qpTarget;      /* Target quantrization parameter */
  i32 qpISlice; /* qp of I slice, not changed until I qp is calculated by I slice model */

  u32 estTimeInc;
  i32 outRateNum;
  i32 outRateDenom;
  i32 gDelaySum;
  i32 gInitialDelay;
  i32 gInitialDoffs;
  hevcQpCtrl_s qpCtrl;
  rcVirtualBuffer_s virtualBuffer;
  sei_s sei;
  i32 gBufferMin, gBufferMax;
  /* new rate control */
  linReg_s gop; /* Data for GOP */
  //linReg_s linReg_BFrame;       /* Data for R-Q model for B frames */
  //linReg_s rError_BFrame;       /* Rate prediction error for B frames(bits) */
  i32 targetPicSize;
  i32 gopHeadTargetPicSize; /* record targetPicSize of GOP headFrame (I/P) for pass-2 */
  i32 targetGopSize;    /* target gop bit size */
  i32 predGopBits;
  i32 actualGopSize;    /* current gop bit size so far */
  i32 encodedGopFrames; /* encoded frames number of gop */
#if VBR_RC
  i32 previousGopSize; /* previous gop bit size */
  i32 currentGopCost;
  i32 previousGopCost;
  i32 sceneComplexType;
  i32 bitCntBank;
#endif
  i32 bitVarRangeI;
  i32 bitVarRangeP;
  i32 bitVarRangeB;
  i32 minPicSizeI;
  i32 maxPicSizeI;
  i32 minPicSizeP;
  i32 maxPicSizeP;
  i32 minPicSizeB;
  i32 maxPicSizeB;
  i32 frameBitCnt;
  i32 tolMovingBitRate;
//  float f_tolMovingBitRate;	//floating tolMovingBitRate seems no use
  i32 monitorFrames;
  float tolCtbRcInter;
  float tolCtbRcIntra;
  u32 maxIprop;
  u32 minIprop;
  i32 changePos;
  RCP_64bit qpFactor;
  /* for gop rate control */
  i32 gopQpSum; /* Sum of current GOP inter frame QPs */
  i32 gopQpDiv;
  i32 gopPQpSum; /* Sum of current GOP inter frame QPs */
  i32 gopPQpNum;
  i32 gopBitCnt;    /* Current GOP bit count so far */
  i32 gopAvgBitCnt; /* Previous GOP average bit count */
  u32 idrPoc; /* accumulated Poc of IDR frame, encoding count == output count @ IDR */
  u32 frameCnt;
  i32 bitrateWindow;
  i32 windowLen;     /* Bitrate window which tries to match target */
  i32 intraInterval; /* Distance between two previous I-frames */
  i32 intraIntervalCtr;
  i32 intraQpDelta;
  i32 longTermQpDelta; /* QP delta of the frame using long term reference */
  i32 frameQpDelta;
  u32 fixedIntraQp;
  i32 bpp;
  /**  sse of each frame got from hardware in one GOP. */
  i32 hierarchial_sse[8][8];
  /** if 1, software will adjust bit allocation in GOP according to hierarchial_sse[][]. */
  i32 smooth_psnr_in_gop;
  /** how many frames in one GOP. */
  i32 hierarchial_bit_allocation_GOP_size;
  /** GOP size of last GOP. */
  i32 hierarchial_bit_allocation_GOP_size_previous;
  i32 encoded_frame_number;
  u32 gopPoc;
  u32 picSkipAllowed;

  /* CPB stuffs */
  i32 minPicSize;         // min target size
  i32 maxPicSize;         // max target size
  i32 encodedFramesBits;  // accumulated encoded frames bit count

  //CTB_RC
  u32 ctbRcQpDeltaReverse;
  u32 ctbRcBitsMin;
  u32 ctbRcBitsMax;
  u32 ctbRctotalLcuBit;
  u32 bitsRatio;
  u32 ctbRcThrdMin;
  u32 ctbRcThrdMax;
  i32 seqStart;
  u32 rcQpDeltaRange;
  u32 rcBaseMBComplexity;
  i32 picQpDeltaMin;
  i32 picQpDeltaMax;
  i32 resetIntraPicQp;
  i32 inputSceneChange;

  rc_ma_s ma;
  rc_ma_char ma_char;

  u32 rcPicComplexity;
  RCP_64bit complexity;

  i32 minIQp;
  i32 finiteQp;
  i32 gopMulti;
  i32 gopLastBitCnt;
  i32 intraframeBitCnt;

  u32 intraCu8Num;
  u32 skipCu8Num;
  u32 PBFrame4NRdCost;
  RCP_32bit reciprocalOfNumBlk8; /* Multiply this factor to get blk8x8 avg */

  u32 codingType;

  i32 i32MaxPicSize;
  u32 u32PFrameByQPStep;
  i32 qpHdrPrevGop;
  i32 qpHdrPrevI;
  u32 u32PrevIBitCnt;
  u32 u32StaticSceneIbitPercent;

  ctbRateControl_s ctbRateCtrl;

  i32 crf;                   /*CRF constant*/
  RCP_32bit rateFactorConstant; /* rate factor constant */
#ifndef RCP_FIXED_POINT_OPT
  double pbOffset;
  double ipOffset;
#endif

  u64 crf_iCostAvg;
#ifndef RCP_FIXED_POINT_OPT
  double crf_iQpAvg;
#endif
  int crf_iFrames;

  i32 pass;

  RCP_32bit pass1CurCost;
  i32 agopPass1GopPicIdx;
  RCP_32bit pass1GopCost[4];
  RCP_32bit pass1AvgCost[4];

  i32 pass1GopFrameNum[4];
  i32 pass1FrameNum[4];
  /* Layer separated predictor */
  i32 gopLayerFrameNum[6];
  rcPredictor rcPred[6];
  i32 predLayerId;
  u32 hieQpDeltaEnable;

  /* derived */
  true_e cbr_flag; /* cbr_flag in HRD parameters */

  /* experimental for visual tuning*/
  u8 IFrameQpVisualTuning;
  u8 pass1EstCostAll;
  u8 visualLmdTuning;
  u8 rcVisualTuning;
  u8 initialQpTuning;

  /* Motion Related Tune */
  motion_type sceneMotionType;
  u32 curMotionScores[2][2];
  u32 preMotionScoreMax[4];
  true_e qpToSizeStatic;

  u32 coreNum;
} vcencRateControl_s;

/*------------------------------------------------------------------------------
      Function prototypes
  ------------------------------------------------------------------------------*/
#ifndef RATE_CONTROL_BUILD_SUPPORT
#define VCEncInitRc VCEncInitQp
#define VCEncBeforePicRc VCEncBeforeCQP
#define VCEncAfterPicRc(rc, nonZeroCnt, byteCnt, qpSum, qpNum) (0)
#define HevcFillerRc(rc, frameCnt) (0)
#define rcCalculate(a, b, c) ((c) == 0 ? 0 : ((a) * (b) / (c)))
#else
bool_e VCEncInitRc(vcencRateControl_s *rc, u32 newStream);
void VCEncBeforePicRc(vcencRateControl_s *rc, u32 timeInc, u32 sliceType,
                      bool use_ltr_cur, struct sw_picture *);
i32 VCEncAfterPicRc(vcencRateControl_s *rc, u32 nonZeroCnt, u32 byteCnt,
                    u32 qpSum, u32 qpNum);
u32 HevcFillerRc(vcencRateControl_s *rc, u32 frameCnt);
i32 rcCalculate(i32 a, i32 b, i32 c);
#endif

#ifdef __cplusplus
}
#endif

#endif
