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

#include "frameworks.h"
#include "memalloc_freertos.h"

#include "ameba_soc.h"

#ifdef __XTENSA_FREERTOS__
u32 g_vcx_int_enable_mask = 0;
#endif

volatile u32 g_spin_lock_cnt = 0;

/**
 * @brief register isr handler for specified CPU irq, and enable it.
 * \param [in] u32 irq: irq num of CPU.
 * \param [in] IRQHandler isr_handler: ISR function.
 * \param [in] void *data: context data of ISR.
 */
av_unused int __IntRegisterIRQ(u32 irq, IRQHandler isr_handler, void *data)
{
	//This is a reference code.
	//Customer need to realize it based on target SoC Interrupt System.

#ifdef __XTENSA_FREERTOS__
	//CPU interrupt handler
	//_xtos_set_interrupt_handle
	u32 irq_mask = 1 << irq;
	u32 enabled_int = 0;

	if ((xthal_get_intenable() & irq_mask) == 0) {
		/* Clear MCPU DB2 interrupt before enable */
		xthal_set_intclear(irq_mask);
		/* Assign Interrupt handler */
		xt_set_interrupt_handler(irq, (xt_handler)isr_handler,
					 data);

		xt_ints_on(irq_mask);

		enabled_int = xthal_get_intenable();
		PDEBUG("INTs are enabled: 0x%x\n", enabled_int);
	}
#endif

	/* enable CPU irq */
	InterruptRegister(isr_handler, AMEBAPRO3_ENC_IRQ_NUM, (u32)data, INT_PRI_MIDDLE);

	return 0;
}

/**
 * @brief enable all VPU IRQs for CPU or GIC.
 */
void __IntEnableIRQs()
{
	//This is a reference code.
	//Customer need to realize it based on target SoC Interrupt System.

#ifdef __XTENSA_FREERTOS__
      *((volatile u32 *)SYS_REG_INT_EN) = ioread32((void *)SYS_REG_INT_EN) |
											g_vcx_int_enable_mask;
#endif

    /* pro3 only have one VPU */
	InterruptEn(AMEBAPRO3_ENC_IRQ_NUM ,0);
}

/**
 * @brief disable all VPU IRQs for CPU or GIC.
 */
void __IntDisableIRQs()
{
	//This is a reference code.
	//Customer need to realize it based on target SoC Interrupt System.
#ifdef __XTENSA_FREERTOS__
    *((volatile uint32_t *)SYS_REG_INT_EN) &= ~g_vcx_int_enable_mask;
#endif

    /* pro3 only have one VPU */
	InterruptDis(AMEBAPRO3_ENC_IRQ_NUM);
}

/**
 * @brief enable specified IRQ of GIC (Generic Interrupt Controller).
 */
void __GICIntEnableIRQ(u32 irq)
{
	//This is a reference code.
	//Customer need to realize it based on target SoC Interrupt System.
	//If there is no GIC, this function could be empty.

#ifdef __XTENSA_FREERTOS__
	//sys reg interrupt enable
	u32 irq_mask = 1 << irq;
	u32 curr_en;

	*((volatile uint32_t *)SYS_REG_INT_STAT) = irq_mask; // clear this irq

	curr_en = ioread32((void *)SYS_REG_INT_EN) | irq_mask;
	*((volatile uint32_t *)SYS_REG_INT_EN) = curr_en; // enable this irq

	g_vcx_int_enable_mask |= irq_mask;
#endif

    /* pro3 only have one VPU */
	InterruptEn(AMEBAPRO3_ENC_IRQ_NUM ,0);
}

/**
 * @brief disable specified IRQ of GIC.
 */
av_unused void __GICIntDisableIRQ(u32 irq)
{
	//This is a reference code.
	//Customer need to realize it based on target SoC Interrupt System.
	//If there is no GIC, this function could be empty.

#ifdef __XTENSA_FREERTOS__
	//sys reg interrupt disable
	u32 irq_mask = 1 << irq;
	u32 curr_en;

	*((volatile uint32_t *)SYS_REG_INT_STAT) = irq_mask; // clear this irq

	curr_en = ioread32((void *)SYS_REG_INT_EN) & (~irq_mask);
	*((volatile uint32_t *)SYS_REG_INT_EN) = curr_en; // disable this irq
#endif

	/* pro3 only have one VPU */
	InterruptDis(AMEBAPRO3_ENC_IRQ_NUM);
}


/**
 * @brief Read & clear VPU IRQs of CPU or GIC, and
 *            convert it to irq bit-mask of VPU device.
 *        One bit corresponds to one VPU device:
 *               Bit 0: VPU device 0; Bit 1: VPU device 1; and so on.
 *        For each bit: 1: interrupt triggered by corresponding VPU device;
 *               0: no interrupt triggered by corresponding VPU device.
 *
 */
u32 __IntReadIRQs()
{
	//This is a reference code.
	//Customer need to realize it based on target SoC Interrupt System.
	u32 irq_bits = 0;

#ifdef __XTENSA_FREERTOS__
	u32 intr_val;

    intr_val = (u32)ioread32((void *)SYS_REG_INT_STAT);

    *((volatile uint32_t *)SYS_REG_INT_STAT) = intr_val;
    intr_val = (u32)ioread32((void *)SYS_REG_INT_VAL);

    PDEBUG("GIC interrupt value is 0x%x\n", intr_val);
    irq = 0;
    while (intr_val)
    {
      if (intr_val & 1)
      {
        for (i = 0; i < total_vcmd_core_num; i++)
        {
          if (vcmd_core_array[i].vcmd_irq == irq)
          {
            //found out the trigger source cor for interrupt
            irq_bits |= 1 << i;
            break;
          }
        }
      }
      /* next irq */
      intr_val >>= 1;
      irq++;
    }
#endif

    /* pro3 only have one VPU */
    irq_bits |= 1;

	return irq_bits;
}


/**
 * @brief clear specified IRQ for CPU/GIC.
 */
void __IntClearIRQStatus(u32 irq)
{
	//This is a reference code.
	//Customer need to realize it based on target SoC Interrupt System.

#ifdef __XTENSA_FREERTOS__
	//sys reg interrupt clear
	u32 irq_mask = 1 << irq;

	*((volatile uint32_t *)SYS_REG_INT_STAT) = irq_mask; // clear this irq
#endif

    /* pro3 do nothing here */
}

/**
 * @brief Flush cache with specified address and size.
 * If cpu arch not support such flush, this function could be empty.
 */
void __CacheFlushMem(u8 *va, u32 bytes)
{
	//for customer to realize it according to the CPU arch
	DCache_CleanInvalidate((u32)va, bytes);
}

/**
 * @brief Flush all cache of CPU.
 * If cpu arch supports cache flush with specified address and size,
 * this function could be empty.
 */
void __CacheFlushAll()
{
	//for customer to realize it according to the CPU arch
	u32 prev_status = irq_disable_save();
	DCache_CleanInvalidate(0xFFFFFFFF, 0xFFFFFFFF);
	irq_enable_restore(prev_status);
}

/**
 * @brief Invalidate cache with specified address and size.
 * If cpu arch not support such invalidate, this function should
 *  invalidate all cache.
 */
void __CacheInvalidateMem(u8 *va, u32 bytes)
{
	//for customer to realize it according to the CPU arch
	DCache_Invalidate((u32)va, bytes);
}

/**
 * @brief Allocate a "linear" memory.
 * \param [out] ptr_t *ba: bus-address of allocated mem.
 * \param [out] void **va: virtual-address of allocated mem.
 * \param [in] u32 bytes: size (in byte) of buffer to allocate.
 */
void __MemAllocLinear(ptr_t *ba, void **va, u32 bytes)
{
	//This is a reference based on memalloc driver.
	//Customer need to realize it based on target mem allocator.

	*va = NULL;
	*ba = AllocMemForIODevice(bytes);
	if (*ba) {
		*va = DirectMemoryMap(*ba, bytes);
	}
}

/**
 * @brief Free a "linear" memory.
 * \param [in] ptr_t ba: bus-address of mem to free.
 * \param [in] void *ba: virtual-address of mem to free.
 * \param [in] u32 bytes: size (in byte) of mem to free.
 */
void __MemFreeLinear(ptr_t ba, void *va, u32 bytes)
{
	//This is a reference based on memalloc driver.
	//Customer need to realize it based on target mem allocator.
	FreeMemForIODevice(ba);
}


