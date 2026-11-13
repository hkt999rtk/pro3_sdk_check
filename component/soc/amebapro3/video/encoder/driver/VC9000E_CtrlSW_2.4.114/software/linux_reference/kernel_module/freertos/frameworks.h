/*------------------------------------------------------------------------------
--        Copyright (c) 2015, VeriSilicon Inc. All rights reserved            --

--                                                                            --
-- This software is confidential and proprietary and may be used only as      --
--   expressly authorized by VeriSilicon in a written licensing agreement.    --
--                                                                            --
--         This entire notice must be reproduced on all copies                --
--                       and may not be removed.                              --
--                                                                            --
--------------------------------------------------------------------------------
-- Redistribution and use in source and binary forms, with or without         --
-- modification, are permitted provided that the following conditions are met:--
-- * Redistributions of source code must retain the above copyright notice,   --
--       this list of conditions and the following disclaimer.                --
-- * Redistributions in binary form must reproduce the above copyright        --
--       notice, this list of conditions and the following disclaimer in the  --
--       documentation and/or other materials provided with the distribution. --
-- * Neither the names of VeriSilicon nor the names of its contributors may be--
--       used to endorse or promote products derived from this software       --
--       without specific prior written permission.                           --
--------------------------------------------------------------------------------
-- THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"--
-- AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE  --
-- IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE --
-- ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE  --
-- LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR        --
-- CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF       --
-- SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS   --
-- INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN    --
-- CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)    --
-- ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE --
-- POSSIBILITY OF SUCH DAMAGE.                                                --
--------------------------------------------------------------------------------
--------------------------------------------------------------------------------
*/

#ifndef __FRAMEWORKS_FREERTOS_
#define __FRAMEWORKS_FREERTOS_

#ifdef __cplusplus
extern "C" {
#endif

#include "osal.h"
#include "io_tools.h"
#include "device_platform_config.h"

#ifdef __XTENSA_FREERTOS__
//Netint xtensa cpu and FreeRTOS OS
#include "xtensa_api.h"
//#include "../src/libxmp/xmp-library.h" //For atomic operations
#include <xtensa/xtruntime.h> //interrupt for xtensa
#endif

#define SYS_REG_INT_TOP_BASE (0x02800000)
//ENCODER
#if 1
/* All Encoder Modules' interrupt will be connected to CPU IRQ 8 */
#define CPU_INT_IRQ   8

#define SYS_INT_MASK_SLICE0 (0x110) //VCE+IM
#define SYS_INT_MASK_SLICE1 (0x220) //VCE+IM
#define SYS_REG_INT_VAL (SYS_REG_INT_TOP_BASE + 0x3c)
#define SYS_REG_INT_STAT (SYS_REG_INT_TOP_BASE + 0x40)
#define SYS_REG_INT_EN (SYS_REG_INT_TOP_BASE + 0x44)
#else
//DECODER
#define CPU_INT_IRQ 9
#define SYS_INT_MASK (0x001)
#define SYS_REG_INT_VAL (SYS_REG_INT_TOP_BASE + 0x48)
#define SYS_REG_INT_STAT (SYS_REG_INT_TOP_BASE + 0x4c)
#define SYS_REG_INT_EN (SYS_REG_INT_TOP_BASE + 0x50)
#endif

/* Interrupt */
/*********************irq functions need to be provided by customer*********************/
/**
 * @brief register isr handler for specified CPU irq, and enable it.
 * \param [in] u32 irq: irq num of CPU.
 * \param [in] IRQHandler isr_handler: ISR function.
 * \param [in] void *data: context data of ISR.
 */
int __IntRegisterIRQ(u32 irq, IRQHandler isr_handler, void *data);
/**
 * @brief enable all VPU IRQs for CPU or GIC.
 */
void __IntEnableIRQs();
/**
 * @brief disable all VPU IRQs for CPU or GIC.
 */
void __IntDisableIRQs();
/**
 * @brief enable specified IRQ of GIC (Generic Interrupt Controller).
 */
void __GICIntEnableIRQ(u32 irq);
/**
 * @brief disable specified IRQ of GIC.
 */
void __GICIntDisableIRQ(u32 irq);
/**
 * @brief Read & clear VPU IRQs of CPU or GIC, and
 *            convert it to irq bit-mask of VPU device.
 *        One bit corresponds to one VPU device:
 *               Bit 0: VPU device 0; Bit 1: VPU device 1; and so on.
 *        For each bit: 1: interrupt triggered by corresponding VPU device;
 *               0: no interrupt triggered by corresponding VPU device.
 *
 */
u32 __IntReadIRQs();
/**
 * @brief clear specified IRQ for CPU/GIC.
 */
void __IntClearIRQStatus(u32 irq);


/* Cache */
/*********************cache functions need to be provided by customer*********************/
/**
 * @brief Flush cache with specified address and size.
 * If cpu arch not support such flush, this function could be empty.
 */
void __CacheFlushMem(u8 *va, u32 bytes);
/**
 * @brief Flush all cache of CPU.
 * If cpu arch supports cache flush with specified address and size,
 * this function could be empty.
 */
void __CacheFlushAll();
/**
 * @brief Invalidate cache with specified address and size.
 * If cpu arch not support such invalidate, this function should
 *  invalidate all cache.
 */
void __CacheInvalidateMem(u8 *va, u32 bytes);



/* Contiguous memory */
/**
 * @brief Allocate a "linear" memory.
 * \param [out] ptr_t *ba: bus-address of allocated mem.
 * \param [out] void **va: virtual-address of allocated mem.
 * \param [in] u32 bytes: size (in byte) of buffer to allocate.
 */
void __MemAllocLinear(ptr_t *ba, void **va, u32 bytes);
/**
 * @brief Free a "linear" memory.
 * \param [in] ptr_t ba: bus-address of mem to free.
 * \param [in] void *ba: virtual-address of mem to free.
 * \param [in] u32 bytes: size (in byte) of mem to free.
 */
void __MemFreeLinear(ptr_t ba, void *va, u32 bytes);
#ifdef __cplusplus
}
#endif

#endif /*__FRAMEWORKS_FREERTOS_ */
