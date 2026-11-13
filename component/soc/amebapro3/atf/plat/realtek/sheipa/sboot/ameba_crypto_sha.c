#define __FPU_PRESENT			1
#define __CORTEX_A				7
#include "cmsis_ca/core_ca.h"
#include "basic_types.h"
#include "hal_platform.h"
#include "sysreg_syson.h"

#include "ameba_vector.h"
#include "ameba_gdma.h"
#include "ameba_crypto.h"
#include "ameba_key_management_hmac.h"
#include "ameba_sha.h"

#include <stdio.h>
#include <string.h>

extern void flush_dcache_range(uintptr_t addr, size_t size);

SHA_TypeDef *SHA2_Get_SHA_Addr(void)
{
	return (SHA_TypeDef *)HMAC_REG_BASE_S;
}

KEY_MANAGEMENT_HMAC_TypeDef *SHA2_get_km_hmac_addr(void)
{
	return (KEY_MANAGEMENT_HMAC_TypeDef *)HMAC_KEY_REG_BASE_S;
}

GDMA_TypeDef *SHA2_Get_DMA_Addr(void)
{
	return (GDMA_TypeDef *)SEC_DMA_REG_BASE_S;
}

int SHA2_Restore(SHA_context *ctx)
{
	GDMA_TypeDef *GDMA = SHA2_Get_DMA_Addr();
	SHA_TypeDef *SHA = SHA2_Get_SHA_Addr();
	u32 i = 0;
	u32 temp;
	int ret = RTK_SUCCESS;

	GDMA->CH[SHA_DMA_CH_NUM].CTL_LOW = ctx->DMA_CTL_LOW;
	/* set input length register */
	SHA->SHA_HASH_INPUT_LEN_x[3] = ctx->total_len;

	/* set sha mode */
	temp = SHA->SHA_CONTROL;
	temp &= ~SHA_MASK_SHA2_MODE;
	temp |= SHA_SHA2_MODE(ctx->sha_mode);

	/* set hash or hmac mode */
	if (ctx->hmac_mode == SHA_HMAC_MODE) {
		temp |= SHA_BIT_HMAC_MODE;
		/* set hash_key_len */
		temp &= ~SHA_MASK_HMAC_KEY_SIZE;
		switch (ctx->key_len_bits) {
		case SHA_HMAC_KEY_BIT_128:
			temp |= SHA_HMAC_KEY_SIZE(SHA_HMAC_KEY_SIZE_128);
			break;
		case SHA_HMAC_KEY_BIT_192:
			temp |= SHA_HMAC_KEY_SIZE(SHA_HMAC_KEY_SIZE_192);
			break;
		case SHA_HMAC_KEY_BIT_256:
			temp |= SHA_HMAC_KEY_SIZE(SHA_HMAC_KEY_SIZE_256);
			break;
		default:
			ret = RTK_FAIL;
			goto exit;
		}
	} else {
		temp &= ~SHA_BIT_HMAC_MODE;
	}
	SHA->SHA_CONTROL = temp;

	/* set digest register */
	for (i = 0; i < 16; i ++) {
		SHA->SHA_DIGESTx[i] = ctx->state[i];
	}

exit:
	return ret;
}

void SHA2_Save(SHA_context *ctx)
{
	GDMA_TypeDef *GDMA = SHA2_Get_DMA_Addr();
	SHA_TypeDef *SHA = SHA2_Get_SHA_Addr();
	u32 i = 0;

	ctx->DMA_CTL_LOW = GDMA->CH[SHA_DMA_CH_NUM].CTL_LOW;
	ctx->total_len = SHA->SHA_HASH_INPUT_LEN_x[3];

	for (i = 0; i < 16; i ++) {
		ctx->state[i] = SHA->SHA_DIGESTx[i];
	}
	return;
}

int SHA2_Get_Final_Digest(SHA_context *ctx, u8 *out)
{
	SHA_TypeDef *SHA = SHA2_Get_SHA_Addr();
	/* [FIH] Use volatile to prevent compiler optimization & register caching */
	volatile u32 i = 0;
	u32 target_words = 0;
	int ret = RTK_FAIL;

	switch (ctx->sha_mode) {
	case SHA_224:
		target_words = 7;
		for (i = 0; i < 7; i++) {
			SHA_PUT_UINT32_BE(SHA->SHA_DIGESTx[2 * i], out, 4 * i);
		}
		break;
	case SHA_256:
		target_words = 8;
		for (i = 0; i < 8; i++) {
			SHA_PUT_UINT32_BE(SHA->SHA_DIGESTx[2 * i], out, 4 * i);
		}
		break;
	case SHA_384:
		target_words = 12;
		for (i = 0; i < 12; i++) {
			SHA_PUT_UINT32_BE(SHA->SHA_DIGESTx[i], out, 4 * i);
		}
		break;
	case SHA_512:
		target_words = 16;
		for (i = 0; i < 16; i++) {
			SHA_PUT_UINT32_BE(SHA->SHA_DIGESTx[i], out, 4 * i);
		}
		break;
	default:
		ret = RTK_FAIL;
		goto exit;
	}

	/*
	 * [FIH Check] Loop Integrity Verification
	 * Ensure the loop executed exactly the expected number of times.
	 * If an attacker glitches the loop comparison to exit early,
	 * 'i' will not match 'target_words'.
	 */
	if (i == target_words && target_words != 0) {
		ret = RTK_SUCCESS;
	} else {
		/* Optional: Re-randomize output on failure to destroy partial data */
		ret = RTK_FAIL;
	}

exit:
	return ret;
}

void SHA_StructInit(SHA_InitTypeDef *SHA_InitStruct)
{
	SHA_InitStruct->HmacMode = 0;
	SHA_InitStruct->EngineMode = SHA_DMA_MODE;
	SHA_InitStruct->IcgEn = ENABLE;
	SHA_InitStruct->DmaMode = SHA_READ_MODE;
}

int SHA2_Init(SHA_InitTypeDef *SHA_InitStruct, SHA_context *ctx)
{
	u32 Temp = 0;
	SHA_TypeDef *SHA = SHA2_Get_SHA_Addr();
	u32 init_mask;
	int ret = RTK_SUCCESS;

	init_mask = SHA_BIT_SLAVE_MODE | SHA_BIT_ICG_ENABLE | SHA_BIT_DMA_MODE | SHA_BIT_HMAC_MODE | SHA_MASK_SHA2_MODE;

	Temp = SHA->SHA_CONTROL;
	Temp &= ~init_mask;

	if (SHA_InitStruct->EngineMode == SHA_SLAVE_MODE) {
		Temp |= SHA_BIT_SLAVE_MODE;
	}

	if (SHA_InitStruct->IcgEn == ENABLE) {
		Temp |= SHA_BIT_ICG_ENABLE;
	}

	if (SHA_InitStruct->DmaMode == SHA_COPY_MODE) {
		Temp |= SHA_BIT_DMA_MODE;
	}

	if (SHA_InitStruct->HmacMode == SHA_HMAC_MODE) {
		Temp |= SHA_BIT_HMAC_MODE;
	}

	switch (ctx->sha_mode) {
	case SHA_224:
		Temp |= SHA_SHA2_MODE(SHA_224);
		ctx->block_size = SHA_BLOCK_256;
		ctx->ilen_bytes = SHA_ILEN_BYTES_256;
		ctx->digest_len = SHA_DIGEST_224;
		break;
	case SHA_256:
		Temp |= SHA_SHA2_MODE(SHA_256);
		ctx->block_size = SHA_BLOCK_256;
		ctx->ilen_bytes = SHA_ILEN_BYTES_256;
		ctx->digest_len = SHA_DIGEST_256;
		break;
	case SHA_384:
		Temp |= SHA_SHA2_MODE(SHA_384);
		ctx->block_size = SHA_BLOCK_512;
		ctx->ilen_bytes = SHA_ILEN_BYTES_512;
		ctx->digest_len = SHA_DIGEST_384;
		break;
	case SHA_512:
		Temp |= SHA_SHA2_MODE(SHA_512);
		ctx->block_size = SHA_BLOCK_512;
		ctx->ilen_bytes = SHA_ILEN_BYTES_512;
		ctx->digest_len = SHA_DIGEST_512;
		break;
	default:
		ret = RTK_FAIL;
		goto exit;
	}

	SHA->SHA_CONTROL = Temp;
	SHA->SHA_HASH_INPUT_LEN_x[3] = ctx->total_len;
exit:
	return ret;
}

void SHA2_DMA_Init(SHA_context *ctx)
{
	GDMA_TypeDef *GDMA = SHA2_Get_DMA_Addr();
	u32 CtlxLow = 0;
	u32 GDMA_SrcMsize = 0;

	/* Fill in DARx register */
	GDMA->CH[SHA_DMA_CH_NUM].DAR = 0;

	/* Process CTLx */
	if ((ctx->sha_mode == SHA_224) || (ctx->sha_mode == SHA_256)) {
		GDMA_SrcMsize = MsizeEight;
	} else if ((ctx->sha_mode == SHA_384) || (ctx->sha_mode == SHA_512)) {
		GDMA_SrcMsize = MsizeSixteen;
	}
	CtlxLow = BIT_CTLX_LO_INT_EN |
			  (SHA_DMA_DST_TR_WIDTH << 1) |
			  (SHA_DMA_SRC_TR_WIDTH << 4) |
			  (MsizeOne << 11) |
			  (GDMA_SrcMsize << 14);

	/* Fill in CTLx register */
	GDMA->CH[SHA_DMA_CH_NUM].CTL_LOW = CtlxLow;
	GDMA->MASK_TFR |= SHA_DMA_CH_MASK | GDMA_CH_EN_WE(SHA_DMA_CH_MASK);
	GDMA->MASK_BLOCK |= SHA_DMA_CH_MASK  | GDMA_CH_EN_WE(SHA_DMA_CH_MASK);
}

int SHA2_SlaveEnd(void)
{
	SHA_TypeDef *SHA = SHA2_Get_SHA_Addr();
	u32 loopWait = SHA_SLAVE_TIMEOUT;
	int ret = RTK_SUCCESS;

	SHA->SHA_CONTROL |= SHA_BIT_SLAVE_MODE;
	SHA->SHA_HMAC_BLOCK_CONTROL |= SHA_BIT_END_SLAVE_INPUT;

	while (1) {
		if (!(SHA->SHA_STATUS & SHA_BIT_ENGINE_BUSY)) {
			break;
		}

		loopWait--;
		if (loopWait == 0) {
			ret = _ERRNO_CRYPTO_SLAVE_TIME_OUT;
			goto exit;
		}
	}

exit:
	SHA->SHA_CONTROL &= ~SHA_BIT_SLAVE_MODE;
	return ret;
}

int SHA2_DmaStart(u32 SrcAddr, const u8 *dst, u32 block_ts)
{
	GDMA_TypeDef *GDMA = SHA2_Get_DMA_Addr();
	int ret = RTK_SUCCESS;
	u32 loopWait = SHA_DMA_TIMEOUT;

	flush_dcache_range(SrcAddr, block_ts << SHA_DMA_SRC_TR_WIDTH);
	if (dst != NULL) {
		flush_dcache_range((u32)dst, block_ts << SHA_DMA_DST_TR_WIDTH);
	}

	GDMA->CH[SHA_DMA_CH_NUM].SAR = (u32) SrcAddr;
	GDMA->CH[SHA_DMA_CH_NUM].DAR = (u32) dst;
	GDMA->CH[SHA_DMA_CH_NUM].CTL_HIGH = block_ts;

	GDMA->CH[SHA_DMA_CH_NUM].CFG_HIGH &= ~BIT_CFGX_UP_SEC_DISABLE;
	GDMA->ChEnReg |= GDMA_CH_EN(1 << SHA_DMA_CH_NUM) | GDMA_CH_EN_WE(1 << SHA_DMA_CH_NUM);

	while (1) {
		if (GDMA->STATUS_TFR & SHA_DMA_CH_MASK) {
			break;
		}
		if (GDMA->StatusInt & ErrType) {
			ret = _ERRNO_CRYPTO_DMA_ERR;
			goto exit;
		}

		loopWait--;
		if (loopWait == 0) {
			ret = _ERRNO_CRYPTO_DMA_ERR;
			goto exit;
		}
	}
	if (dst != NULL) {
		flush_dcache_range((u32)dst, block_ts << SHA_DMA_DST_TR_WIDTH);
	}

exit:
	/* clear transfer complete interrupt */
	GDMA->CLEAR_TFR   |=  SHA_DMA_CH_MASK;
	GDMA->CLEAR_BLOCK |=  SHA_DMA_CH_MASK;

	GDMA->CH[SHA_DMA_CH_NUM].CFG_HIGH |= BIT_CFGX_UP_SEC_DISABLE;
	return ret;
}

int SHA2_ProcessDma(SHA_context *ctx, const u8 *input, u8 *dst, u32 bodylen)
{
	int ret = RTK_FAIL;
	SHA_TypeDef *SHA = (SHA_TypeDef *)SHA2_Get_SHA_Addr();
	u32 block_ts = SHA_DMA_MAX_BLOCK_TS_256;
	u32 SrcAddr = (u32)input;

	if ((ctx->sha_mode == SHA_384) || (ctx->sha_mode == SHA_512)) {
		block_ts = SHA_DMA_MAX_BLOCK_TS_512;
	}

	if (bodylen == 0) {
		return ret;
	}

	/* In read-only mode, when the first update length less than SHA-BLOCK size,
	   HMAC cannot perform DMA calculations after setting the first block bit,
	   resulting in the i-pad stage being skipped.
	   Therefore, it is necessary to place this initial step in the SHA2_ProcessDma. */
	if (ctx->seq_hash_first == TRUE) {
		SHA->SHA_HMAC_BLOCK_CONTROL |= SHA_BIT_FIRST_BLOCK;
		ctx->seq_hash_first = FALSE;
	}

	while (bodylen > block_ts) {
		ret = SHA2_DmaStart(SrcAddr, dst, block_ts);
		if (ret != RTK_SUCCESS) {
			return ret;
		}
		dst += block_ts << SHA_DMA_SRC_TR_WIDTH;
		bodylen -= block_ts << SHA_DMA_SRC_TR_WIDTH;
		SrcAddr += (block_ts << SHA_DMA_SRC_TR_WIDTH);
	}

	block_ts = bodylen >> SHA_DMA_SRC_TR_WIDTH;
	ret = SHA2_DmaStart(SrcAddr, dst, block_ts);

	return ret;
}

void SHA2_lock_mutex(void)
{
	SHA_TypeDef *SHA = SHA2_Get_SHA_Addr();
	int mutex_timeout = SHA_MUTEX_TIMEOUT;
	int timeout_cnt = 0;

	while (1) {
		if (SHA_GET_ENGINE_MUTEX_STATUS(SHA->SHA_MUTEX_STATUS) == 0) {
			break;
		}

		if ((timeout_cnt++) >= mutex_timeout) {
			timeout_cnt = 0;
		}
	}
}

void SHA2_unlock_mutex(void)
{
	SHA_TypeDef *SHA = (SHA_TypeDef *)SHA2_Get_SHA_Addr();

	SHA->SHA_MUTEX_STATUS = 0;
}

int SHA2_hmac_load_key(u8 key_id, u32 key_len_bits)
{
	u32 timeout = 100000;
	KEY_MANAGEMENT_HMAC_TypeDef *KEY = SHA2_get_km_hmac_addr();

	KEY->KM_HMAC_KEY_SEL_NUMBER = key_id;
	KEY->KM_HMAC_KEY_SETUP_TRIGGER |= KM_HMAC_BIT_KEY_SETUP_TRIGGER_KEY_MANAGER_IDLE;

	while (1) {
		if (KEY->KM_HMAC_KEY_SETUP_TRIGGER & KM_HMAC_BIT_KEY_SETUP_TRIGGER_KEY_MANAGER_IDLE) {
			break;
		}
		timeout--;
		if (timeout == 0) {
			return _ERRNO_CRYPTO_KEY_LOAD_ERR;
			break;
		}
	}

	return RTK_SUCCESS;
}

int crypto_sha2_init(SHA_context *ctx, u32 SHAtype)
{
	SHA_InitTypeDef SHA_InitStruct;
	int ret = RTK_SUCCESS;

	SHA2_lock_mutex();

	memset(ctx, 0, sizeof(SHA_context));

	ctx->sha_mode = SHAtype;
	ctx->total_len = 0;
	ctx->dma_copy = SHA_READ_MODE;
	ctx->seq_hash_first = TRUE;

	SHA_StructInit(&SHA_InitStruct);
	ret = SHA2_Init(&SHA_InitStruct, ctx);
	if (ret != RTK_SUCCESS) {
		goto exit;
	}

	SHA2_DMA_Init(ctx);

	SHA2_Save(ctx);

exit:
	SHA2_unlock_mutex();
	return ret;
}

int crypto_sha2_update(SHA_context *ctx, const u8 *input, u8 *dst, size_t len)
{
	SHA_TypeDef *SHA = SHA2_Get_SHA_Addr();
	int ret = RTK_SUCCESS;
	int buf_empty_bytes = 0;
	int bodylen;
	int restlen;
	int buf_pos = ctx->buf_used_bytes;;

	/* if use copy mode, the input data must be an integer multiple of the block size, and buffer must be empty */
	if (dst != NULL) {
		if ((len % ctx->block_size) != 0 || ctx->buf_used_bytes != 0) {
			ret = _ERRNO_CRYPTO_MESSAGE_LEN_ERR;
			return ret;
		}
	}

	SHA2_lock_mutex();

	ret = SHA2_Restore(ctx);
	if (ret != RTK_SUCCESS) {
		goto exit;
	}

	if (ctx->hmac_mode == SHA_HMAC_MODE) {
		ret = SHA2_hmac_load_key(ctx->key_id, ctx->key_len_bits);
		if (ret != RTK_SUCCESS) {
			goto exit;
		}
	}

	/* add message length */
	SHA->SHA_HASH_INPUT_LEN_x[3] += len;

	if (dst != NULL) {
		/* set copy mode */
		SHA->SHA_CONTROL |= SHA_BIT_DMA_MODE;
		ctx->dma_copy = SHA_COPY_MODE;

		/* copy mode only support sha-block len */
		if (len > 0) {
			ret =  SHA2_ProcessDma(ctx, input, dst, len);
			if (ret != RTK_SUCCESS) {
				goto exit;
			}
		}
	} else {
		/* set read-only mode */
		SHA->SHA_CONTROL &= ~SHA_BIT_DMA_MODE;
		ctx->dma_copy = SHA_READ_MODE;

		buf_empty_bytes = ctx->block_size - buf_pos;

		if ((int)len < buf_empty_bytes) {
			memcpy((void *)(&ctx->buffer[buf_pos]), (const void *)input, len);
			ctx->buf_used_bytes += len;
		} else {
			/* 1. process previous update saved msg buffer */
			if (buf_empty_bytes != 0) {
				memcpy((void *)(&ctx->buffer[buf_pos]), (const void *)input, buf_empty_bytes);
				len -= buf_empty_bytes;
				input += buf_empty_bytes;
			}
			ret =  SHA2_ProcessDma(ctx, (u8 *)(ctx->buffer), 0, ctx->block_size);
			if (ret != RTK_SUCCESS) {
				goto exit;
			}
			ctx->buf_used_bytes = 0;

			/* 2. send out all blocksize-byte align message, this part should larger than sha block size */
			restlen = len & (ctx->block_size - 1);
			if (len > (u32) restlen) {
				bodylen = len - restlen;
				ret =  SHA2_ProcessDma(ctx, input, 0, bodylen);
				if (ret != RTK_SUCCESS) {
					goto exit;
				}
				input += bodylen;
			}

			/* 3. backup the rest */
			if (restlen > 0) {
				memcpy((void *)(&ctx->buffer[0]), (const void *)input, restlen);
			}
			ctx->buf_used_bytes = restlen;
		}
	}
	SHA2_Save(ctx);
exit:
	SHA2_unlock_mutex();
	return ret;
}

int crypto_sha2_final(SHA_context *ctx, u8 *output)
{
	int ret = RTK_FAIL;
	SHA_TypeDef *SHA = SHA2_Get_SHA_Addr();
	volatile uint32_t check_val;

	SHA2_lock_mutex();
	ret = SHA2_Restore(ctx);
	if (ret != RTK_SUCCESS) {
		goto exit;
	}

	/* incase of full zero caculation, we have set first_block bit then use slave mode */
	if (ctx->seq_hash_first == TRUE) {
		SHA->SHA_HMAC_BLOCK_CONTROL |= SHA_BIT_FIRST_BLOCK;
		ctx->seq_hash_first = FALSE;
	}

	if (ctx->hmac_mode == SHA_HMAC_MODE) {
		ret = SHA2_hmac_load_key(ctx->key_id, ctx->key_len_bits);
		if (ret != RTK_SUCCESS) {
			goto exit;
		}
	}

	/* [FIH] Set Last Block and Verify */
	SHA->SHA_HMAC_BLOCK_CONTROL |= SHA_BIT_LAST_BLOCK;
	check_val = SHA->SHA_HMAC_BLOCK_CONTROL;
	if ((check_val & SHA_BIT_LAST_BLOCK) != SHA_BIT_LAST_BLOCK) {
		ret = RTK_FAIL;
		goto exit;
	}

	if (ctx->buf_used_bytes != 0) {
		/* only in read-only mode and buffer is not empty condition, we use DMA. (copy mode do not save message to buffer) */
		SHA->SHA_CONTROL &= ~SHA_BIT_DMA_MODE;
		ret = SHA2_ProcessDma(ctx, ctx->buffer, NULL, ctx->buf_used_bytes);
	} else {
		/* when buffer is empty, we use slave-mode */
		ret = SHA2_SlaveEnd();
	}

	/* Check DMA/Slave result */
	if (ret != RTK_SUCCESS) {
		goto exit;
	}
	ret = SHA2_Get_Final_Digest(ctx, output);
exit:
	SHA->SHA_HMAC_BLOCK_CONTROL &= ~SHA_BIT_LAST_BLOCK;
	SHA2_unlock_mutex();
	return ret;
}

int crypto_hmac_sha2_init(SHA_context *ctx, u32 SHAtype, u8 key_id, u32 key_len_bits)
{
	int ret = RTK_SUCCESS;

	SHA2_lock_mutex();

	memset(ctx, 0, sizeof(SHA_context));

	switch (SHAtype) {
	case SHA_224:
	case SHA_256:
		ctx->total_len = 0x40;
		break;
	case SHA_384:
	case SHA_512:
		ctx->total_len = 0x80;
		break;
	}

	ctx->sha_mode  = SHAtype;
	ctx->dma_copy  = SHA_READ_MODE;
	ctx->seq_hash_first = TRUE;
	ctx->hmac_mode = SHA_HMAC_MODE;
	ctx->key_len_bits = key_len_bits;
	ctx->key_id = key_id;

	SHA_InitTypeDef SHA_InitStruct;
	SHA_StructInit(&SHA_InitStruct);
	SHA_InitStruct.HmacMode = SHA_HMAC_MODE;
	ret = SHA2_Init(&SHA_InitStruct, ctx);
	if (ret != RTK_SUCCESS) {
		goto exit;
	}
	SHA2_DMA_Init(ctx);

	SHA2_Save(ctx);
exit:
	SHA2_unlock_mutex();
	return RTK_SUCCESS;
}

int crypto_hmac_sha2_update(SHA_context *ctx, const u8 *input, u8 *dst, size_t len)
{
	return crypto_sha2_update(ctx, input, dst, len);
}

int crypto_hmac_sha2_final(SHA_context *ctx, u8 *output)
{
	return crypto_sha2_final(ctx, output);
}
