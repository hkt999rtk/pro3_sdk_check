/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _AMEBA_CACHE_H_
#define _AMEBA_CACHE_H_


/** @addtogroup AmebaPro3_Platform
  * @{
  */

/** @defgroup CACHE
  * @brief CACHE modules
  * @{
  */

/** @addtogroup CACHE
  * @verbatim
  *****************************************************************************************
  * Introduction
  *****************************************************************************************
  *		-just support read cache.
  *		-32K bytes - KM4,
  *
  *****************************************************************************************
  * how to use
  *****************************************************************************************
  *		Cache_Enable: enable/disable cache
  *		Cache_Flush: flush cache, you should Cache_Flush after flash write or flash erase
  *****************************************************************************************
  * @endverbatim
  */

/** @defgroup CACHE_Type_define
  * @{
  */
#define DATA_CACHE						((u32)0x00000000)
#define CODE_CACHE						((u32)0x00000001)
/**
  * @}
  */

/** @defgroup CACHE_Line_Size_define
  * @{
  */
#define CM_CACHE_LINE_SIZE              32
#define CA_CACHE_LINE_SIZE              64
/**
  * @}
  */


/** @defgroup CACHE_Way_define
  * @{
  */
#define CACHE_WWR_1WAY			((u32)0x00000000)
#define CACHE_WWR_2WAY			((u32)0x00000001)
#define CACHE_WWR_3WAY			((u32)0x00000002)
#define CACHE_WWR_4WAY			((u32)0x00000003)
/**
  * @}
  */

/* Exported functions --------------------------------------------------------*/
/** @defgroup CACHE_Exported_Functions FLash Cache Exported Functions
  * @{
  */

#if defined(CONFIG_ARM_CORE_CM4) || defined(CONFIG_ARM_CORE_CM0)
#define CORTEX_M_CACHE
#undef  CORTEX_A_CACHE
#define CACHE_LINE_SIZE         CM_CACHE_LINE_SIZE
#elif defined(CONFIG_ARM_CORE_CA32)
#define CORTEX_A_CACHE
#undef  CORTEX_M_CACHE
#define CACHE_LINE_SIZE         CA_CACHE_LINE_SIZE
#else
#undef  CORTEX_A_CACHE
#undef  CORTEX_M_CACHE
#endif

/** @defgroup CACHE_Line_Aligned_define
  * @{
  */
#define CACHE_LINE_ADDR_MSK				(~(CACHE_LINE_SIZE - 1U))
#define CACHE_LINE_ALIGMENT(x)			(((u32)(x) + (CACHE_LINE_SIZE - 1U)) & CACHE_LINE_ADDR_MSK)
#define CACHE_LINE_ALIGNMENT(x)			(((u32)(x) + (CACHE_LINE_SIZE - 1U)) & CACHE_LINE_ADDR_MSK)

#define IS_CACHE_LINE_ALIGNED(x)		(((x) & (CACHE_LINE_SIZE - 1U)) == 0)
#define IS_CACHE_LINE_ALIGNED_ADDR(x)	IS_CACHE_LINE_ALIGNED(x)
#define IS_CACHE_LINE_ALIGNED_SIZE(x)	IS_CACHE_LINE_ALIGNED(x)
/**
  * @}
  */

#if defined(CORTEX_M_CACHE)
/**
  * @brief  Enable Icache.
  */
__STATIC_INLINE
void ICache_Enable(void)
{
	SCB_EnableICache();
}

/**
  * @brief  Disable Icache.
  */
__STATIC_INLINE
void ICache_Disable(void)
{
	SCB_DisableICache();
}

/**
  * @brief  Invalidate Icache.
  */
__STATIC_INLINE
void ICache_Invalidate(void)
{
	SCB_InvalidateICache();
}

/**
  * @brief  Check DCache Enabled or not.
  */
__STATIC_INLINE
u32 DCache_IsEnabled(void)
{
	return ((SCB->CCR & (u32)SCB_CCR_DC_Msk) ? 1 : 0);
}

/**
  * @brief  Enable Dcache.
  */
__STATIC_INLINE
void DCache_Enable(void)
{
	if (!DCache_IsEnabled()) { //if D-Cache is enabled, no need to invalidate D-Cache
		SCB_EnableDCache();
	}
}

/**
  * @brief  Disable Dcache.
  */
__STATIC_INLINE
void DCache_Disable(void)
{
	SCB_DisableDCache();
}

/**
  * @brief   D-Cache Invalidate by address.
  * @details Invalidates D-Cache for the given address
  * @param  Address    address (aligned to 32-byte boundary)
  * @param  Bytes   size of memory block (in number of bytes)
  *
  * @note  Dcache will be restored from memory.
  * @note  This can be used after DMA Rx, and CPU read DMA data from DMA buffer.
  * @note  if Address is 0xFFFFFFFF, it means dont care, it was used when all Dcache be Invalidated.
  */
__STATIC_INLINE
void DCache_Invalidate(u32 Address, u32 Bytes)
{
	u32 addr = Address, len = Bytes;

	if (DCache_IsEnabled() == 0) {
		return;
	}

	if ((Address == 0xFFFFFFFF) && (Bytes == 0xFFFFFFFF)) {
		SCB_InvalidateDCache();
	} else {
		if (!IS_CACHE_LINE_ALIGNED(Address)) {
			addr = Address & CACHE_LINE_ADDR_MSK; //region is [Address, Address + Bytes)
			len = Address - addr + Bytes; // need add length of [addr, Address)
		}

		SCB_InvalidateDCache_by_Addr((u32 *)addr, len);
	}
}

/**
  * @brief   D-Cache Clean by address
  * @details Cleans D-Cache for the given address
  * @param  Address    address (aligned to 32-byte boundary)
  * @param  Bytes   size of memory block (in number of bytes)
  *
  * @note  Dcache will be write back to memory.
  * @note  This can be used before DMA Tx, after CPU write data to DMA buffer.
  * @note  if Address is 0xFFFFFFFF, it means dont care, it was used when all Dcache be cleaned.
  * @note  AmebaD cache is default read allocation and write through, so clean is not needed.
  */
__STATIC_INLINE
void DCache_Clean(u32 Address, u32 Bytes)
{
	u32 addr = Address, len = Bytes;

	if (DCache_IsEnabled() == 0) {
		return;
	}

	if ((Address == 0xFFFFFFFF) && (Bytes == 0xFFFFFFFF)) {
		SCB_CleanDCache();
	} else {
		if (!IS_CACHE_LINE_ALIGNED(Address)) {
			addr = Address & CACHE_LINE_ADDR_MSK; //region is [Address, Address + Bytes)
			len = Address - addr + Bytes; // need add length of [addr, Address)
		}

		SCB_CleanDCache_by_Addr((u32 *)addr, len);
	}
}


/**
  * @brief   D-Cache Clean and Invalidate by address
  * @details Cleans and invalidates D_Cache for the given address
  * @param  Address    address (aligned to 32-byte boundary)
  * @param  Bytes   size of memory block (in number of bytes)
  *
  * @note  This can be used when you want to write back cache data and then Invalidate cache.
  * @note  if Address is 0xFFFFFFFF, it means dont care, it was used when all Dcache be cleaned.
  */
__STATIC_INLINE
void DCache_CleanInvalidate(u32 Address, u32 Bytes)
{
	u32 addr = Address, len = Bytes;

	if (DCache_IsEnabled() == 0) {
		return;
	}

	if ((Address == 0xFFFFFFFF) && (Bytes == 0xFFFFFFFF)) {
		SCB_CleanInvalidateDCache();
	} else {
		if (!IS_CACHE_LINE_ALIGNED(Address)) {
			addr = Address & CACHE_LINE_ADDR_MSK; //region is [Address, Address + Bytes)
			len = Address - addr + Bytes; // need add length of [addr, Address)
		}

		SCB_CleanInvalidateDCache_by_Addr((u32 *)addr, len);
	}
}

/**
  * @brief  Disable/Enable I/D cache.
  * @param  Enable
  *   This parameter can be any combination of the following values:
  *		 @arg ENABLE cache enable & SPIC read 16bytes every read command
  *		 @arg DISABLE cache disable & SPIC read 4bytes every read command
  */
__STATIC_INLINE
void Cache_Enable(u32 Enable)
{
	if (Enable) {
		ICache_Enable();
		DCache_Enable();
	} else {
		ICache_Disable();
		DCache_Disable();
	}
}

#if defined (__ARM_FEATURE_CMSE) && (__ARM_FEATURE_CMSE == 3U)
/**
  * @brief  Disable/Enable Nonsecure I/D cache by secure world.
  * @param  Enable
  *   This parameter can be any combination of the following values:
  *		 @arg ENABLE cache enable & SPIC read 16bytes every read command
  *		 @arg DISABLE cache disable & SPIC read 4bytes every read command
  */
__STATIC_INLINE
void Cache_Enable_NS(u32 Enable)
{
	if (Enable) {
		SCB_EnableICache_NS();
		SCB_EnableDCache_NS();
	} else {
		SCB_DisableICache_NS();
		SCB_DisableDCache_NS();
	}
}
#endif

/**
  * @brief  flush I/D cache.
  */
__STATIC_INLINE
void Cache_Flush(void)
{
	SCB_InvalidateICache();
	SCB_InvalidateDCache();
}

/**
  * @brief  set dcache ways for restrict range under secure world.
  * @param  Dcwrr_base: restrict range base address, 64KB align.
  * @param  Dcwrr_top: restrict range top address, 64KB align.
  * @param  way_restrict: ways can be used for restrict range.
  *          This parameter can be one of the following values:
  *            CACHE_WWR_1WAY: 1 way for restrict range.
  *            CACHE_WWR_2WAY: 2 way for restrict range.
  *            CACHE_WWR_3WAY: 3 way for restrict range.
  *            CACHE_WWR_4WAY: 4 way for restrict range.
  * @retval None
  * @note: This function can only be called under secure world
  */
__STATIC_INLINE
void DCache_Way_Set(u32 Dcwrr_base, u32 Dcwrr_top, u32 way_restrict)
{
	u32 temp;

	temp = SCnSCB->ACTLR;

	temp &= (~(0x3 << 20));
	temp |= (way_restrict << 20);
	SCnSCB->ACTLR = temp;

	RIDR->DCWRR_BASE = Dcwrr_base;
	RIDR->DCWRR_TOP = Dcwrr_top;
}

/**
  * @brief  set icache ways for restrict range under secure world
  * @param  Icwrr_base: restrict range base address, 64KB align.
  * @param  Icwrr_top: restrict range top address, 64KB align.
  * @param  way_restrict: ways can be used for restrict range.
  *          This parameter can be one of the following values:
  *            CACHE_WWR_1WAY: 1 way for restrict range.
  *            CACHE_WWR_2WAY: 2 way for restrict range.
  *            CACHE_WWR_3WAY: 3 way for restrict range.
  *            CACHE_WWR_4WAY: 4 way for restrict range.
  * @retval None
  * @note: This function can only be called under secure world
  */
__STATIC_INLINE
void ICache_Way_Set(u32 Icwrr_base, u32 Icwrr_top, u32 way_restrict)
{
	u32 temp;

	temp = SCnSCB->ACTLR;

	temp &= (~(0x3 << 12));
	temp |= (way_restrict << 12);
	SCnSCB->ACTLR = temp;

	RIDR->ICWRR_BASE = Icwrr_base;
	RIDR->ICWRR_TOP = Icwrr_top;
}
#elif defined(CORTEX_A_CACHE)

__STATIC_INLINE
void ICache_Enable(void)
{
	__set_SCTLR(__get_SCTLR() | SCTLR_I_Msk);
	__ISB();
}

__STATIC_INLINE
void ICache_Disable(void)
{
	__set_SCTLR(__get_SCTLR() & (~SCTLR_I_Msk));
	__ISB();
}

__STATIC_INLINE
void ICache_Invalidate(void)
{
	L1C_InvalidateICacheAll();
}

__STATIC_INLINE
u32 DCache_IsEnabled(void)
{
	return ((__get_SCTLR() & SCTLR_C_Msk) ? 1 : 0);
}


__STATIC_INLINE
void DCache_Enable(void)
{
	__set_SCTLR(__get_SCTLR() | SCTLR_C_Msk);
	__ISB();
}

__STATIC_INLINE
void DCache_Disable(void)
{
	__set_SCTLR(__get_SCTLR() & (~SCTLR_C_Msk));
	__ISB();
}

__STATIC_INLINE
void DCache_Invalidate(u32 Address, u32 Bytes)
{
	Bytes += Address & (CACHE_LINE_SIZE - 1);
	for (u32 offset = 0; offset < Bytes; offset += CACHE_LINE_SIZE) {
		L1C_InvalidateDCacheMVA((void *)(Address + offset));
	}
}

__STATIC_INLINE
void DCache_Clean(u32 Address, u32 Bytes)
{
	Bytes += Address & (CACHE_LINE_SIZE - 1);
	for (u32 offset = 0; offset < Bytes; offset += CACHE_LINE_SIZE) {
		L1C_CleanDCacheMVA((void *)(Address + offset));
	}
}

__STATIC_INLINE
void DCache_CleanInvalidate(u32 Address, u32 Bytes)
{
	Bytes += Address & (CACHE_LINE_SIZE - 1);
	for (u32 offset = 0; offset < Bytes; offset += CACHE_LINE_SIZE) {
		L1C_CleanInvalidateDCacheMVA((void *)(Address + offset));
	}
}

__STATIC_INLINE
void Cache_Enable(u32 Enable)
{
	if (Enable) {
		L1C_EnableCaches();
	} else {
		L1C_DisableCaches();
	}
}

__STATIC_INLINE
void Cache_Flush(void)
{
	L1C_CleanInvalidateDCacheAll();
}

__STATIC_INLINE
void DCache_Way_Set(u32 Dcwrr_base, u32 Dcwrr_top, u32 way_restrict)
{
	(void) Dcwrr_base;
	(void) Dcwrr_top;
	(void) way_restrict;
}

__STATIC_INLINE
void ICache_Way_Set(u32 Icwrr_base, u32 Icwrr_top, u32 way_restrict)
{
	(void) Icwrr_base;
	(void) Icwrr_top;
	(void) way_restrict;
}
#else
#error "NO CACHE API implement"
#endif


/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */
#endif //_AMEBAPRO3_CACHE_H_
