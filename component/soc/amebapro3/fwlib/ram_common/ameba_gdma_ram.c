/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "ameba_soc.h"

static const char *TAG = "GDMA";
static u8 GDMA0_ChAlloc_Bits = 0;
static u8 GDMA1_ChAlloc_Bits = 0;

static u8 GDMA0_IrqNum[8] = {
#if defined (CONFIG_ARM_CORE_CM4_NP) || defined (CONFIG_ARM_CORE_CA32) || defined (CONFIG_ARM_CORE_CM0)
	/* CA32/NP/VP GDMA IRQ */
	GDMA0_CH0_IRQ,
	GDMA0_CH1_IRQ,
	GDMA0_CH2_IRQ,
	GDMA0_CH3_IRQ,
#endif
};

static u8 GDMA1_IrqNum[8] = {
#if defined (CONFIG_ARM_CORE_CM4_NP) || defined (CONFIG_ARM_CORE_CA32) || defined (CONFIG_ARM_CORE_CM4_VP)
	GDMA1_CH0_IRQ,
	GDMA1_CH1_IRQ,
	GDMA1_CH2_IRQ,
	GDMA1_CH3_IRQ,
	GDMA1_CH4_IRQ,
	GDMA1_CH5_IRQ,
	GDMA1_CH6_IRQ,
	GDMA1_CH7_IRQ,
#endif
};


/** @addtogroup Ameba_Periph_Driver
  * @{
  */

/** @defgroup GDMA
  * @brief	GDMA driver modules
  * @{
  */
/** @brief  Validate index/channel and return reg base. */
__weak
GDMA_TypeDef *GDMA_GetRegBase(u8 GDMA_Index, u8 GDMA_ChNum)
{
	GDMA_TypeDef *GDMA;

	assert_param(IS_GDMA_Index(GDMA_Index));
	if (GDMA_Index) {
		assert_param(IS_GDMA1_ChannelNum(GDMA_ChNum));
		GDMA = ((GDMA_TypeDef *) GDMA1_BASE);
		if (TrustZone_IsSecure()) {
			GDMA = ((GDMA_TypeDef *) GDMA1_REG_BASE_S);
		}
	} else {
		assert_param(IS_GDMA0_ChannelNum(GDMA_ChNum));
		GDMA = ((GDMA_TypeDef *) GDMA0_BASE);
		if (TrustZone_IsSecure()) {
			GDMA = ((GDMA_TypeDef *) GDMA0_REG_BASE_S);
		}
	}

	return GDMA;
}

/* Exported functions --------------------------------------------------------*/
/** @defgroup GDMA_Exported_Functions GDMA Exported Functions
  * @{
  */

/** @brief  Fill GDMA_InitStruct with default values. */
__weak
void GDMA_StructInit(PGDMA_InitTypeDef GDMA_InitStruct)
{
	_memset((void *)GDMA_InitStruct, 0, sizeof(GDMA_InitTypeDef));

	GDMA_InitStruct->GDMA_SrcDataWidth = TrWidthFourBytes;
	GDMA_InitStruct->GDMA_DstDataWidth = TrWidthFourBytes;
	GDMA_InitStruct->GDMA_SrcMsize = MsizeEight;
	GDMA_InitStruct->GDMA_DstMsize = MsizeEight;

	GDMA_InitStruct->SecureTransfer = 0x1;
}

/** @brief  Initialize GDMA channel registers from GDMA_InitStruct. */
__weak
void GDMA_Init(u8 GDMA_Index, u8 GDMA_ChNum, PGDMA_InitTypeDef GDMA_InitStruct)
{
	u32 CtlxLow = 0;
	u32 CtlxUp = 0;
	u32 CfgxLow = 0;
	u32 CfgxUp = 0;
	u32 GdmaSrcHsIndex = 0, GdmaDestHsIndex = 0;
	u32 ChEn = BIT(GDMA_ChNum) | BIT((GDMA_ChNum + 8));
	u32 BlockSize = GDMA_InitStruct->GDMA_BlockSize;
	GDMA_TypeDef *GDMA;

	RCC_PeriphClockCmd(APBPeriph_PLFM_CORE_SEC_CLK, APBPeriph_CBUS_SEC_CLOCK, ENABLE);
	RCC_PeriphClockCmd(NULL, APBPeriph_CBUS_SYS_MEM_CLOCK, ENABLE);
	if (GDMA_Index) {
		RCC_PeriphClockCmd(APBPeriph_GDMA1, APBPeriph_GDMA1_CLOCK, ENABLE);
	} else {
		RCC_PeriphClockCmd(APBPeriph_GDMA0, APBPeriph_GDMA0_CLOCK, ENABLE);
	}

	GDMA = GDMA_GetRegBase(GDMA_Index, GDMA_ChNum);

	/* secure transfer must be configured before other registers */
	if (GDMA_InitStruct->SecureTransfer) {
		GDMA->CH[GDMA_ChNum].CFG_HIGH &= ~BIT_CFGX_UP_SEC_DISABLE;
	} else {
		GDMA->CH[GDMA_ChNum].CFG_HIGH |= BIT_CFGX_UP_SEC_DISABLE;
	}

	//GDMA->DmaCfgReg = 1;

	if (GDMA->ChEnReg & ChEn) {
		RTK_LOGW(TAG, "Channel had used; Disable Channel!!!!\n");
		GDMA_Cmd(GDMA_Index, GDMA_ChNum, DISABLE);
	}

	GDMA_ClearINT(GDMA_Index, GDMA_ChNum);

	GDMA->CH[GDMA_ChNum].SAR = GDMA_InitStruct->GDMA_SrcAddr;
	GDMA->CH[GDMA_ChNum].DAR = GDMA_InitStruct->GDMA_DstAddr;

	CtlxLow = BIT_CTLX_LO_INT_EN |
			  (GDMA_InitStruct->GDMA_DstDataWidth << 1) |
			  (GDMA_InitStruct->GDMA_SrcDataWidth << 4) |
			  (GDMA_InitStruct->GDMA_DstInc << 7) |
			  (GDMA_InitStruct->GDMA_SrcInc << 9) |
			  (GDMA_InitStruct->GDMA_DstMsize << 11) |
			  (GDMA_InitStruct->GDMA_SrcMsize << 14) |
			  (GDMA_InitStruct->GDMA_DIR << 20) |
			  (GDMA_InitStruct->GDMA_LlpDstEn << 27) |
			  (GDMA_InitStruct->GDMA_LlpSrcEn << 28);


	CtlxUp = (BlockSize & BIT_CTLX_UP_BLOCK_BS) /*|
		BIT_CTLX_UP_DONE*/;

	GDMA->CH[GDMA_ChNum].CTL_LOW = CtlxLow;
	GDMA->CH[GDMA_ChNum].CTL_HIGH = CtlxUp;

	GDMA->CH[GDMA_ChNum].RPT_PTTN = GDMA_InitStruct->GDMA_RepeatPattern;

	CfgxLow = (GDMA_InitStruct->GDMA_ReloadSrc << 30) |
			  (GDMA_InitStruct->GDMA_ReloadDst << 31) |
			  (GDMA_InitStruct->GDMA_ChPrior << 4);

	CfgxUp = GDMA->CH[GDMA_ChNum].CFG_HIGH;
	CfgxUp &= ~(BIT_CFGX_UP_SRC_PER | BIT_CFGX_UP_DEST_PER | GDMA_BIT_ExtendedSRC_PER1 | GDMA_BIT_ExtendedSRC_PER2 | GDMA_BIT_ExtendedDEST_PER1 |
				GDMA_BIT_ExtendedDEST_PER2);

	GdmaSrcHsIndex = GDMA_InitStruct->GDMA_SrcHandshakeInterface;
	if ((GdmaSrcHsIndex >= 16) && (GdmaSrcHsIndex < 32)) {
		/* PER1 */
		GDMA_InitStruct->GDMA_SrcHandshakeInterface = (GDMA_InitStruct->GDMA_SrcHandshakeInterface & 0xf);
		CfgxUp |= (GDMA_InitStruct->GDMA_SrcHandshakeInterface << 7) | GDMA_BIT_ExtendedSRC_PER1;
	} else if ((GdmaSrcHsIndex >= 32) && (GdmaSrcHsIndex < 64)) {
		/* PER2 */
		GDMA_InitStruct->GDMA_SrcHandshakeInterface = (GDMA_InitStruct->GDMA_SrcHandshakeInterface & 0xf);
		CfgxUp |= (GDMA_InitStruct->GDMA_SrcHandshakeInterface << 7) | GDMA_BIT_ExtendedSRC_PER2;
	} else {
		/* PER */
		CfgxUp |= (GDMA_InitStruct->GDMA_SrcHandshakeInterface << 7);
	}

	GdmaDestHsIndex = GDMA_InitStruct->GDMA_DstHandshakeInterface;
	if ((GdmaDestHsIndex >= 16) && (GdmaDestHsIndex < 32)) {
		/* PER1 */
		GDMA_InitStruct->GDMA_DstHandshakeInterface = (GDMA_InitStruct->GDMA_DstHandshakeInterface & 0xf);
		CfgxUp |= (GDMA_InitStruct->GDMA_DstHandshakeInterface << 11) | GDMA_BIT_ExtendedDEST_PER1;
	} else if ((GdmaDestHsIndex >= 32) && (GdmaDestHsIndex < 64)) {
		/* PER2 */
		GDMA_InitStruct->GDMA_DstHandshakeInterface = (GDMA_InitStruct->GDMA_DstHandshakeInterface & 0xf);
		CfgxUp |= (GDMA_InitStruct->GDMA_DstHandshakeInterface << 11) | GDMA_BIT_ExtendedDEST_PER2;
	} else {
		/* PER */
		CfgxUp |= (GDMA_InitStruct->GDMA_DstHandshakeInterface << 11);
	}

	if (GdmaSrcHsIndex <= 29) {
		/* set GDMA_SEL bit in PON system control */
		if (GDMA_Index) {
			SYSON_BASE->PON_GDMA_CTRL &= ~(1 << GdmaSrcHsIndex);
		} else {
			SYSON_BASE->PON_GDMA_CTRL |= (1 << GdmaSrcHsIndex);
		}
	}

	GDMA->CH[GDMA_ChNum].CFG_LOW = CfgxLow;
	GDMA->CH[GDMA_ChNum].CFG_HIGH = CfgxUp;

	GDMA_INTConfig(GDMA_Index, GDMA_ChNum, GDMA_InitStruct->GDMA_IsrType, ENABLE);
}

/** @brief  Route SPI DMA handshake to master or slave path. */
__weak
void GDMA_SPI_Select(u8 SPI_Index, bool Is_SPI_Master)
{
	u32 sel_bit = (SPI_Index % 2) ? SYSON_BIT_HS_SPI1_DMA_SEL : SYSON_BIT_HS_SPI0_DMA_SEL;

	if (Is_SPI_Master) {
		SYSON_BASE->PON_GDMA_CTRL &= ~sel_bit;
	} else {
		SYSON_BASE->PON_GDMA_CTRL |= sel_bit;
	}
}

/** @brief  Configure LLP linked-list for multi-block transfer.
  * @note   round=0: linear list (last node ends); round=1: circular list.
  */
__weak
void GDMA_SetLLP(u8 GDMA_Index, u8 GDMA_ChNum, u32 MultiBlockCount, struct GDMA_CH_LLI *pGdmaChLli, u32 round)
{
	u32 CtlxLow, CtlxUp;
	PGDMA_CH_LLI_ELE pLliEle = &pGdmaChLli->LliEle;
	GDMA_TypeDef *GDMA;

	GDMA = GDMA_GetRegBase(GDMA_Index, GDMA_ChNum);
	//RTK_LOGI(TAG, "Block Count %lu\n", MultiBlockCount);

	CtlxLow = GDMA->CH[GDMA_ChNum].CTL_LOW;
	CtlxUp = GDMA->CH[GDMA_ChNum].CTL_HIGH;

	CtlxUp = (pGdmaChLli->BlockSize & BIT_CTLX_UP_BLOCK_BS) | /*BIT_CTLX_UP_DONE | */CtlxUp;
	GDMA->CH[GDMA_ChNum].CTL_HIGH = CtlxUp;

	GDMA->CH[GDMA_ChNum].LLP = (u32) & (pGdmaChLli->LliEle);

	while (MultiBlockCount > 0) {
		pLliEle = &pGdmaChLli->LliEle;

		/* Clear LLP enable on the last element of a linear list */
		if ((MultiBlockCount == 1) && (round == 0)) {
			CtlxLow &= ~(BIT_CTLX_LO_LLP_DST_EN |
						 BIT_CTLX_LO_LLP_SRC_EN);
		}
		CtlxUp &= ~(BIT_CTLX_UP_BLOCK_BS);
		CtlxUp |= (pGdmaChLli->BlockSize & BIT_CTLX_UP_BLOCK_BS);

		pLliEle->CtlxLow = CtlxLow;
		pLliEle->CtlxUp = CtlxUp;
		pLliEle->Llpx = (u32)&pGdmaChLli->pNextLli->LliEle;

		DCache_CleanInvalidate((u32)pGdmaChLli, sizeof(struct GDMA_CH_LLI));

		pGdmaChLli = pGdmaChLli->pNextLli;
		MultiBlockCount--;
	}
}

/** @brief  Clear specified pending interrupt(s) and return which were active. */
__weak  u32
GDMA_ClearINTPendingBit(u8 GDMA_Index, u8 GDMA_ChNum, u32 GDMA_IT)
{
	uint32_t temp_bit = 0;
	uint32_t temp_isr = 0;
	GDMA_TypeDef *GDMA;

	assert_param(IS_GDMA_CONFIG_IT(GDMA_IT));

	GDMA = GDMA_GetRegBase(GDMA_Index, GDMA_ChNum);

	if ((GDMA->RAW_TFR & BIT(GDMA_ChNum)) && (GDMA->STATUS_TFR & BIT(GDMA_ChNum))) {
		temp_isr |= (TransferType & GDMA_IT);
	}
	if ((GDMA->RAW_BLOCK & BIT(GDMA_ChNum)) && (GDMA->STATUS_BLOCK & BIT(GDMA_ChNum))) {
		temp_isr |= (BlockType & GDMA_IT);
	}
	if ((GDMA->RAW_SRC_TRAN & BIT(GDMA_ChNum)) && (GDMA->STATUS_SRC_TRAN & BIT(GDMA_ChNum))) {
		temp_isr |= (SrcTransferType & GDMA_IT);
	}
	if ((GDMA->RAW_DST_TRAN & BIT(GDMA_ChNum)) && (GDMA->STATUS_DST_TRAN & BIT(GDMA_ChNum))) {
		temp_isr |= (DstTransferType & GDMA_IT);
	}
	if ((GDMA->RAW_ERR & BIT(GDMA_ChNum)) && (GDMA->STATUS_ERR & BIT(GDMA_ChNum))) {
		temp_isr |= (ErrType & GDMA_IT);
	}

	temp_bit = BIT(GDMA_ChNum) | BIT(GDMA_ChNum + 8);

	if (GDMA_IT & TransferType) {
		GDMA->CLEAR_TFR = temp_bit;
	}

	if (GDMA_IT & BlockType) {
		GDMA->CLEAR_BLOCK = temp_bit;
	}

	if (GDMA_IT & SrcTransferType) {
		GDMA->CLEAR_SRC_TRAN = temp_bit;
	}

	if (GDMA_IT & DstTransferType) {
		GDMA->CLEAR_DST_TRAN = temp_bit;
	}

	if (GDMA_IT & ErrType) {
		GDMA->CLEAR_ERR = temp_bit;
	}

	return temp_isr;
}

/** @brief  Clear all pending interrupts and return which were active. */
__weak  u32
GDMA_ClearINT(u8 GDMA_Index, u8 GDMA_ChNum)
{
	uint32_t temp_bit = 0;
	uint32_t temp_isr = 0;
	GDMA_TypeDef *GDMA;

	GDMA = GDMA_GetRegBase(GDMA_Index, GDMA_ChNum);

	if ((GDMA->RAW_TFR & BIT(GDMA_ChNum)) && (GDMA->STATUS_TFR & BIT(GDMA_ChNum))) {
		temp_isr |= TransferType;
	}
	if ((GDMA->RAW_BLOCK & BIT(GDMA_ChNum)) && (GDMA->STATUS_BLOCK & BIT(GDMA_ChNum))) {
		temp_isr |= BlockType;
	}
	if ((GDMA->RAW_SRC_TRAN & BIT(GDMA_ChNum)) && (GDMA->STATUS_SRC_TRAN & BIT(GDMA_ChNum))) {
		temp_isr |= SrcTransferType;
	}
	if ((GDMA->RAW_DST_TRAN & BIT(GDMA_ChNum)) && (GDMA->STATUS_DST_TRAN & BIT(GDMA_ChNum))) {
		temp_isr |= DstTransferType;
	}
	if ((GDMA->RAW_ERR & BIT(GDMA_ChNum)) && (GDMA->STATUS_ERR & BIT(GDMA_ChNum))) {
		temp_isr |= ErrType;
	}

	temp_bit = BIT(GDMA_ChNum) | BIT(GDMA_ChNum + 8);

	GDMA->CLEAR_TFR = temp_bit;
	GDMA->CLEAR_BLOCK = temp_bit;
	GDMA->CLEAR_SRC_TRAN = temp_bit;
	GDMA->CLEAR_DST_TRAN = temp_bit;
	GDMA->CLEAR_ERR = temp_bit;

	return temp_isr;
}

/** @brief  Enable or disable the specified GDMA interrupt sources. */
__weak  void
GDMA_INTConfig(u8 GDMA_Index, u8 GDMA_ChNum, u32 GDMA_IT, u32 NewState)
{
	u32 temp_bit = 0;
	GDMA_TypeDef *GDMA;

	assert_param(IS_GDMA_CONFIG_IT(GDMA_IT));

	GDMA = GDMA_GetRegBase(GDMA_Index, GDMA_ChNum);
	if (NewState != DISABLE) {
		temp_bit = BIT(GDMA_ChNum) | BIT(GDMA_ChNum + 8);

		if (GDMA_IT & TransferType) {
			GDMA->MASK_TFR |= temp_bit;
		}

		if (GDMA_IT & BlockType) {
			GDMA->MASK_BLOCK |= temp_bit;
		}

		if (GDMA_IT & SrcTransferType) {
			GDMA->MASK_SRC_TRAN |= temp_bit;
		}

		if (GDMA_IT & DstTransferType) {
			GDMA->MASK_DST_TRAN |= temp_bit;
		}

		if (GDMA_IT & ErrType) {
			GDMA->MASK_ERR |= temp_bit;
		}
	} else {
		temp_bit = ~(BIT(GDMA_ChNum) | BIT(GDMA_ChNum + 8));

		if (GDMA_IT & TransferType) {
			GDMA->MASK_TFR &= temp_bit;
		}

		if (GDMA_IT & BlockType) {
			GDMA->MASK_BLOCK &= temp_bit;
		}

		if (GDMA_IT & SrcTransferType) {
			GDMA->MASK_SRC_TRAN &= temp_bit;
		}

		if (GDMA_IT & DstTransferType) {
			GDMA->MASK_DST_TRAN &= temp_bit;
		}

		if (GDMA_IT & ErrType) {
			GDMA->MASK_ERR &= temp_bit;
		}
	}
}

/** @brief  Enable or disable a GDMA channel. */
__weak  void
GDMA_Cmd(u8 GDMA_Index, u8 GDMA_ChNum, u32 NewState)
{
	u32 ValTemp = 0;
	GDMA_TypeDef *GDMA;

	GDMA = GDMA_GetRegBase(GDMA_Index, GDMA_ChNum);

	/* ChEnWE mask bit enables atomic update of ChEn without read-modify-write */
	ValTemp |= BIT(GDMA_ChNum + 8);

	if (NewState == ENABLE) {
		ValTemp |= BIT(GDMA_ChNum);
	} else {
		ValTemp &= ~ BIT(GDMA_ChNum);
	}

	GDMA->ChEnReg = ValTemp;
}

/** @brief  Clear auto-reload bit(s) in CFGx (CLEAN_RELOAD_SRC / DST / both). */
__weak  void
GDMA_ChCleanAutoReload(u8 GDMA_Index, u8 GDMA_ChNum, u32 CleanType)
{
	u32 CfgxLow;
	GDMA_TypeDef *GDMA;

	GDMA = GDMA_GetRegBase(GDMA_Index, GDMA_ChNum);
	CfgxLow = GDMA->CH[GDMA_ChNum].CFG_LOW;

	if (CleanType == CLEAN_RELOAD_SRC) {
		CfgxLow &= ~BIT_CFGX_LO_RELOAD_SRC;
	} else if (CleanType == CLEAN_RELOAD_DST) {
		CfgxLow &= ~BIT_CFGX_LO_RELOAD_DST;
	} else {
		CfgxLow &= ~BIT_CFGX_LO_RELOAD_SRC;
		CfgxLow &= ~BIT_CFGX_LO_RELOAD_DST;
	}

	GDMA->CH[GDMA_ChNum].CFG_LOW = CfgxLow;
}

/** @brief  Set channel source address. */
__weak
void GDMA_SetSrcAddr(u8 GDMA_Index, u8 GDMA_ChNum, u32 SrcAddr)
{
	GDMA_TypeDef *GDMA;

	GDMA = GDMA_GetRegBase(GDMA_Index, GDMA_ChNum);
	GDMA->CH[GDMA_ChNum].SAR = SrcAddr;
}

/** @brief  Get current source address (RSAR, live during transfer). */
__weak
u32 GDMA_GetSrcAddr(u8 GDMA_Index, u8 GDMA_ChNum)
{
	GDMA_TypeDef *GDMA;

	GDMA = GDMA_GetRegBase(GDMA_Index, GDMA_ChNum);
	return GDMA->CH[GDMA_ChNum].RSAR;
}

/** @brief  Get current destination address (RDAR, live during transfer). */
__weak
u32 GDMA_GetDstAddr(u8 GDMA_Index, u8 GDMA_ChNum)
{
	GDMA_TypeDef *GDMA;

	GDMA = GDMA_GetRegBase(GDMA_Index, GDMA_ChNum);
	return GDMA->CH[GDMA_ChNum].RDAR;
}

/** @brief  Set channel destination address. */
__weak
void GDMA_SetDstAddr(u8 GDMA_Index, u8 GDMA_ChNum, u32 DstAddr)
{
	GDMA_TypeDef *GDMA;

	GDMA = GDMA_GetRegBase(GDMA_Index, GDMA_ChNum);
	GDMA->CH[GDMA_ChNum].DAR = DstAddr;
}

/** @brief  Set GDMA block size (CTL_HIGH[BIT_CTLX_UP_BLOCK_BS]). */
__weak
void GDMA_SetBlkSize(u8 GDMA_Index, u8 GDMA_ChNum, u32 BlkSize)
{
	u32 BlkSizeTemp = 0;
	GDMA_TypeDef *GDMA;

	GDMA = GDMA_GetRegBase(GDMA_Index, GDMA_ChNum);
	BlkSizeTemp = GDMA->CH[GDMA_ChNum].CTL_HIGH;

	BlkSizeTemp &= ~BIT_CTLX_UP_BLOCK_BS;
	BlkSizeTemp |= (BlkSize & BIT_CTLX_UP_BLOCK_BS);

	GDMA->CH[GDMA_ChNum].CTL_HIGH = BlkSizeTemp;
}

/** @brief  Get block size configured for this channel. */
__weak
u32 GDMA_GetBlkSize(u8 GDMA_Index, u8 GDMA_ChNum)
{
	u32 BlkSize = 0;
	GDMA_TypeDef *GDMA;

	GDMA = GDMA_GetRegBase(GDMA_Index, GDMA_ChNum);
	BlkSize = GDMA->CH[GDMA_ChNum].CTL_HIGH;

	BlkSize &= BIT_CTLX_UP_BLOCK_BS;

	return BlkSize;
}

/** @brief  Mark channel as allocated and register its IRQ handler. */
__weak void GDMA_ChnlRegister(u8 GDMA_Index, u8 GDMA_ChNum, IRQ_FUN IrqFun, u32 IrqData, u32 IrqPriority)
{
	u8 IrqNum = 0;

	assert_param(IS_GDMA_Index(GDMA_Index));
	if (GDMA_Index) {
		assert_param(IS_GDMA1_ChannelNum(GDMA_ChNum));
		GDMA1_ChAlloc_Bits |= BIT(GDMA_ChNum);
		IrqNum = GDMA1_IrqNum[GDMA_ChNum];
	} else {
		assert_param(IS_GDMA0_ChannelNum(GDMA_ChNum));
		GDMA0_ChAlloc_Bits |= BIT(GDMA_ChNum);
		IrqNum = GDMA0_IrqNum[GDMA_ChNum];
	}

	if (IrqFun != NULL) {
		InterruptRegister(IrqFun, IrqNum, IrqData, IrqPriority);
		InterruptEn(IrqNum, IrqPriority);
	}

}

/** @brief  Mark channel as free and unregister its IRQ handler. */
static void GDMA_ChnlUnRegister(u8 GDMA_Index, u8 GDMA_ChNum)
{
	u8 IrqNum = 0;

	assert_param(IS_GDMA_Index(GDMA_Index));
	if (GDMA_Index) {
		assert_param(IS_GDMA1_ChannelNum(GDMA_ChNum));
		GDMA1_ChAlloc_Bits &= ~BIT(GDMA_ChNum);
		IrqNum = GDMA1_IrqNum[GDMA_ChNum];
	} else {
		assert_param(IS_GDMA0_ChannelNum(GDMA_ChNum));
		GDMA0_ChAlloc_Bits &= ~BIT(GDMA_ChNum);
		IrqNum = GDMA0_IrqNum[GDMA_ChNum];
	}

	InterruptDis(IrqNum);
	InterruptUnRegister(IrqNum);
}

/** @brief  Allocate a free channel; returns channel number or 0xFF if none available. */
__weak  u8
GDMA_ChnlAlloc(u32 GDMA_Index, IRQ_FUN IrqFun, u32 IrqData, u32 IrqPriority)
{
	u32 found = 0;
	u32 GDMA_ChNum = 0xFF;
	u8 ValTemp = 0;
	u8 MaxDmaChnl = 0;

	assert_param(IS_GDMA_Index(GDMA_Index));

	if (GDMA_Index) {
		ValTemp = GDMA1_ChAlloc_Bits;
		MaxDmaChnl = MAX_GDMA1_CHNL;
	} else {
		ValTemp = GDMA0_ChAlloc_Bits;
		MaxDmaChnl = MAX_GDMA0_CHNL;
	}

	for (GDMA_ChNum = 0; GDMA_ChNum < MaxDmaChnl; GDMA_ChNum++) {
		if ((ValTemp & BIT(GDMA_ChNum)) == 0) {
			found = 1;
			break;
		}
	}

	if (found) {
		GDMA_ChnlRegister(GDMA_Index, GDMA_ChNum, IrqFun, IrqData, IrqPriority);
	} else {
		GDMA_ChNum = 0xFF;
	}
	return GDMA_ChNum;
}

/** @brief  Release a previously allocated channel. */
__weak  u8
GDMA_ChnlFree(u8 GDMA_Index, u8 GDMA_ChNum)
{
	GDMA_TypeDef *GDMA = NULL;
	u8 ret = _FALSE;

	GDMA = GDMA_GetRegBase(GDMA_Index, GDMA_ChNum);
	GDMA_ChnlUnRegister(GDMA_Index, GDMA_ChNum);

	ret = _TRUE;
	(void)GDMA;
	return ret;
}

/** @brief  Return IRQ number for a channel. */
__weak  u8
GDMA_GetIrqNum(u8 GDMA_Index, u8 GDMA_ChNum)
{
	u8 IrqNum;

	assert_param(IS_GDMA_Index(GDMA_Index));

	if (GDMA_Index) {
		IrqNum = GDMA1_IrqNum[GDMA_ChNum];
	} else {
		IrqNum = GDMA0_IrqNum[GDMA_ChNum];
	}

	return IrqNum;
}

/** @brief  Set channel arbitration priority (CFG_LOW[7:4]). */
__weak  void
GDMA_SetChnlPriority(u8 GDMA_Index, u8 GDMA_ChNum, u32 ChnlPriority)
{
	u32 CfgxLow;
	GDMA_TypeDef *GDMA;

	GDMA = GDMA_GetRegBase(GDMA_Index, GDMA_ChNum);
	CfgxLow = GDMA->CH[GDMA_ChNum].CFG_LOW;

	CfgxLow &= ~BIT_CFGX_CH_PRIOR;
	CfgxLow |= SET_CFGX_CH_PRIOR(ChnlPriority);
	GDMA->CH[GDMA_ChNum].CFG_LOW = CfgxLow;

}

/** @brief  Suspend a channel (set CH_SUSP in CFG_LOW). */
__weak  void
GDMA_Suspend(u8 GDMA_Index, u8 GDMA_ChNum)
{
	GDMA_TypeDef *GDMA;

	GDMA = GDMA_GetRegBase(GDMA_Index, GDMA_ChNum);
	GDMA->CH[GDMA_ChNum].CFG_LOW |= BIT_CFGX_LO_CH_SUSP;
}

/** @brief  Resume a suspended channel. */
__weak  void
GDMA_Resume(u8 GDMA_Index, u8 GDMA_ChNum)
{
	GDMA_TypeDef *GDMA;

	GDMA = GDMA_GetRegBase(GDMA_Index, GDMA_ChNum);
	GDMA->CH[GDMA_ChNum].CFG_LOW &= ~BIT_CFGX_LO_CH_SUSP;
}

/**
  * @brief  Abort a channel.
  * @param  GDMA_Index: Always 0.
  * @param  GDMA_ChNum: GDMA channel number.
  * @return TRUE if the channel is successfully aborted, FALSE otherwise.
  */
__weak  u8
GDMA_Abort(u8 GDMA_Index, u8 GDMA_ChNum)
{
	u32 timeout = 500;
	GDMA_TypeDef *GDMA;

	GDMA = GDMA_GetRegBase(GDMA_Index, GDMA_ChNum);

	GDMA_Suspend(GDMA_Index, GDMA_ChNum);
	/*If ChEnReg[GDMA_ChNum] is not equal to 0, it means that
	  the channel is working and the Suspend status must be checked.*/
	while (timeout--) {
		if ((GDMA_GET_CH_EN(GDMA->ChEnReg) & BIT(GDMA_ChNum)) == 0 || \
			BIT_CFGX_LO_GET_CH_INACTIVE(GDMA->CH[GDMA_ChNum].CFG_LOW)) {
			GDMA_Cmd(GDMA_Index, GDMA_ChNum, DISABLE);
			return TRUE;
		}
	}

	/* The loop ran out: timeout has wrapped to 0xFFFFFFFF, so it cannot be tested here.
	 * The channel is still active, resume is required. */
	GDMA_Resume(GDMA_Index, GDMA_ChNum);
	return FALSE;
}

/**
  * @brief  Get whether the channel FIFO is empty.
  * @param  GDMA_Index: Always 0.
  * @param  GDMA_ChNum: GDMA channel number.
  * @return TRUE if the channel FIFO is empty, FALSE otherwise.
  */
__weak  u8
GDMA_ChnlFIFOIsEmpty(u8 GDMA_Index, u8 GDMA_ChNum)
{
	GDMA_TypeDef *GDMA;

	GDMA = GDMA_GetRegBase(GDMA_Index, GDMA_ChNum);

	if (GDMA->CH[GDMA_ChNum].CFG_LOW & BIT_CFGX_LO_FIFO_EMPTY) {
		return TRUE;
	}

	return FALSE;
}

/**
  * @brief  Set Source Gather.
  * @param  GDMA_Index: Always 0.
  * @param  GDMA_ChNum: GDMA channel number.
  * @param  Src_GatherCount: Source continuous transfer count between consecutive gather boundaries.
  * @param  Src_GatherInterval: Source gather interval.
  * @note   AmebaPro3 GDMA does not support source gather, this is an empty stub kept for API alignment.
  */
__weak
void GDMA_SourceGather(u8 GDMA_Index, u8 GDMA_ChNum, u32 Src_GatherCount, u32 Src_GatherInterval)
{
	UNUSED(GDMA_Index);
	UNUSED(GDMA_ChNum);
	UNUSED(Src_GatherCount);
	UNUSED(Src_GatherInterval);
}

/**
  * @brief  Set destination Scatter.
  * @param  GDMA_Index: Always 0.
  * @param  GDMA_ChNum: GDMA channel number.
  * @param  Dst_ScatterCount: Destination contiguous transfer count between successive scatter boundaries.
  * @param  Dst_ScatterInterval: Destination scatter interval.
  * @note   AmebaPro3 GDMA does not support destination scatter, this is an empty stub kept for API alignment.
  */
__weak
void GDMA_DestinationScatter(u8 GDMA_Index, u8 GDMA_ChNum, u32 Dst_ScatterCount, u32 Dst_ScatterInterval)
{
	UNUSED(GDMA_Index);
	UNUSED(GDMA_ChNum);
	UNUSED(Dst_ScatterCount);
	UNUSED(Dst_ScatterInterval);
}

/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */
