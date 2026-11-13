#include "hevcencapi.h"

void init_video_config(VCEncConfig *param)
{
	memset(param, 0, sizeof(VCEncConfig));
	param->profile = VCENC_ADAPTIVE_PROFILE; //(VCEncProfile)cml->profile;
	param->level = VCENC_AUTO_LEVEL; //(VCEncLevel)cml->level;
	param->width = 176; //cml->width;
	param->height = 144; //cml->height;
	param->frameRateNum = 30; //cml->outputRateNumer;
	param->frameRateDenom = 1; //cml->outputRateDenom;
	param->refFrameAmount = 1; //cml->intraPicRate, cml->interlacedFrame
	param->bitDepthLuma = 8; //cml->bitDepthLuma;
	param->bitDepthChroma = 8; //cml->bitDepthChroma;
	param->enableSsim = 1; //cml->ssim;
	param->enablePsnr = 1; //cml->psnr;
	param->maxTLayers = 1;
	param->rdoLevel = 2; //CLIP3(1, 3, cml->rdoLevel) - 1; //header offset 0x3A ~ 0x42 (sps seg.)
	param->exp_of_input_alignment = 6; //cml->exp_of_input_alignment;
	param->exp_of_ref_alignment = 6; //cml->exp_of_ref_alignment;
	param->exp_of_ref_ch_alignment = 6; //cml->exp_of_ref_ch_alignment;
	param->exp_of_aqinfo_alignment = 6; //cml->exp_of_aqinfo_alignment;
	param->log2MaxPicOrderCntLsb = 16; //cml->log2MaxPicOrderCntLsb;
	param->log2MaxFrameNum = 12; //cml->log2MaxFrameNum;
	param->parallelCoreNum = 1; //cml->parallelCoreNum;
	param->cuInfoVersion = -1; //cml->cuInfoVersion;
	param->gopSize = 1; //tb->gopSize;
#if 1 //V114 only
	param->gopMaxBSize = -255; //cml->gopMaxBSize;
#endif
	param->numRefP = 1; //cml->numRefP;
	param->extSramLumHeightBwd = 16; //cml->extSramLumHeightBwd;
	param->extSramChrHeightBwd = 8; //cml->extSramChrHeightBwd;
	param->extSramLumHeightFwd = 16; //cml->extSramLumHeightFwd;
	param->extSramChrHeightFwd = 8; //cml->extSramChrHeightFwd;
	param->AXIAlignment = -1; //cml->AXIAlignment;
	param->irqTypeMask = 0x01f4; //cml->irqTypeMask;
	param->irqTypeCutreeMask = 0x01f4; //cml->irqTypeCutreeMask;
	param->codedChromaIdc = 1; //cml->codedChromaIdc;
	param->writeReconToDDR = 1; //(cml->writeReconToDDR || cml->intraPicRate != 1);
	param->burstMaxLength = 16; //cml->burstMaxLength;
	param->enableTMVP = -255; //1; //cml->enableTMVP; //header offset 0x3E[6] (sps seg.)
	param->num_tile_columns = 1; //cml->num_tile_columns;
	param->num_tile_rows = 1; //cml->num_tile_rows;
	param->loop_filter_across_tiles_enabled_flag =
		1; //cml->loop_filter_across_tiles_enabled_flag;
	param->useVcmd = 1; //cml->useVcmd;
}

int HevcInitRateConfigs(VCEncRateCtrl *rcCfg)
{
	rcCfg->blockRCSize = 2; //cml->blockRCSize;
	rcCfg->qpHdr = -1; //cml->qpHdr;
	rcCfg->bitVarRangeI = 10000; //cml->bitVarRangeI;
	rcCfg->bitVarRangeP = 10000; //cml->bitVarRangeP;
	rcCfg->bitVarRangeB = 10000; //cml->bitVarRangeB;
	rcCfg->u32StaticSceneIbitPercent = 80; //cml->u32StaticSceneIbitPercent;
	rcCfg->rcMode = 5; //cml->rcMode;
	rcCfg->tolRcUnderflow = 50; //cml->tolRcUnderflow;
	rcCfg->changePos = 90; //cml->changePos;
	return 0;
}

int HevcInitPreConfigs(VCEncPreProcessingCfg *preProcCfg)
{
	preProcCfg->osdMapInput = "osdMapInput.yuv";

	return 0;
}

int VideoInitGopConfigs(int gopSize, VCEncGopConfig *gopCfg,
						u8 *gopCfgOffset)
{
	VCEncGopPicConfig *cfg = NULL;
	gopCfg->special_size = 0;
	gopCfg->ltrcnt = 0;
	/* gopCfgOffset: keep record of gopSize¡¦s offset in pGopPicCfg */
	if (gopCfgOffset) {
		gopCfgOffset[gopSize] = gopCfg->size;
	}
	/* gop_config : #Frame1: P 1 0 0.578 0 1 -1 1 */
	cfg = &(gopCfg->pGopPicCfg[gopCfg->size++]);
	cfg->poc = 1;
	cfg->QpOffset = 0;
	cfg->QpFactor = 0.760263; //sqrt(0.578)
	cfg->temporalId = 0;
	cfg->codingType = 1;
	cfg->nonReference = 0;
	cfg->numRefPics = 1;
	cfg->refPics->ref_pic = -1;
	cfg->refPics->used_by_cur = 1;
	return 0;
}
