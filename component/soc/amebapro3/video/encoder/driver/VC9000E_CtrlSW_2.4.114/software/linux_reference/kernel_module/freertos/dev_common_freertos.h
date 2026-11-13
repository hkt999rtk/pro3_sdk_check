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

#ifndef _DEV_COMMON_FREERTOS_H_
#define _DEV_COMMON_FREERTOS_H_

#ifdef __cplusplus
extern "C" {
#endif

/**********Driver for FreeRTOS based on the method of Linux's calling**********/
#define __u8                          u8
#define __i8                          i8
#define __u32                         u32
#define __i32                         i32

#define container_of(p, type, member)          \
({  void *__tp = (void *)(p);                  \
    ((type *)(__tp - offsetof(type, member)));})

//PCIE
#define pci_get_device(a, b, c)       (void *)0
#define pci_enable_device(a)          0
#define pci_disable_device(a)         0
#define pci_resource_start(a, b)      0
#define pci_resource_len(a, b)        0
/*--------------------------------*/
/* IO */
//#define O_RDONLY (1)
//#define O_WRONLY (1 << 1)
//#define O_RDWR (O_RDONLY | O_WRONLY)
//#define O_SYNC (1 << 2)
#undef PROT_READ
#undef PROT_WRITE
#undef PROT_EXEC
#define PROT_READ 0x1
#define PROT_WRITE 0x2
#define PROT_EXEC 0x4
#define PROT_SEM 0x8
#define PROT_NONE 0x0

#define __iomem
#define __user
#define __force

#undef MAP_FAILED
#define MAP_FAILED                              NULL
#undef NULL
#define NULL                                    ((void *)0)
#define getpagesize()                           (1)
#define ioremap_nocache(addr, size)             (addr)
#define ioremap(addr, size)                     (addr)
#define iounmap(addr)
//#define mmap(va, size, access, flag, fd, pa)    DirectMemoryMap(pa, size)
#define mmap(va, size, access, flag, fd, pa)    freertos_mmap(pa, size)
#define munmap(pRegs, size)
#define vmalloc(a)                              pvPortMalloc(a) //malloc(a);
#define vfree(a)                                vPortFree(a) //free(a);
#define probe(a)                                encoder_rtos_driver_init(a)
#define open(name, flag)                        freertos_open(name, flag)
#define ioctl(fd, cmd, arg)                     freertos_ioctl(fd, cmd, arg)
#define close(fd)                               freertos_close(fd)
#define KERN_INFO
#define KERN_ERR
#define KERN_WARNING
#define KERN_DEBUG
#define printk printf//PDEBUG
#define access_ok(a, b, c)                      (1)
#define __init
#define __exit
/*
 * Let any architecture override either of the following before
 * including this file.
 */
#ifndef _IOC_SIZEBITS
#define _IOC_SIZEBITS                 14
#endif

#ifndef _IOC_DIRBITS
#define _IOC_DIRBITS                  2
#endif
#define _IOC_NRBITS                   8
#define _IOC_TYPEBITS                 8

#define _IOC_NRMASK                   ((1 << _IOC_NRBITS) - 1)
#define _IOC_TYPEMASK                 ((1 << _IOC_TYPEBITS) - 1)
#define _IOC_SIZEMASK                 ((1 << _IOC_SIZEBITS) - 1)
#define _IOC_DIRMASK                  ((1 << _IOC_DIRBITS) - 1)

#define _IOC_NRSHIFT 0
#define _IOC_TYPESHIFT                (_IOC_NRSHIFT + _IOC_NRBITS)
#define _IOC_SIZESHIFT                (_IOC_TYPESHIFT + _IOC_TYPEBITS)
#define _IOC_DIRSHIFT                 (_IOC_SIZESHIFT + _IOC_SIZEBITS)

/*
	 * Direction bits, which any architecture can choose to override
	 * before including this file.
	 */
#ifndef _IOC_NONE
#define _IOC_NONE                     0U
#endif

#ifndef _IOC_WRITE
#define _IOC_WRITE                    1U
#endif

#ifndef _IOC_READ
#define _IOC_READ                     2U
#endif

#define _IOC(dir, type, nr, size)     (((dir) << _IOC_DIRSHIFT)               \
                                       | ((type) << _IOC_TYPESHIFT)           \
                                       | ((nr) << _IOC_NRSHIFT)               \
                                       | ((size) << _IOC_SIZESHIFT))

#define _IOC_TYPECHECK(t)             (sizeof(t))

/* used to create numbers */
#define _IO(type, nr)                 _IOC(_IOC_NONE, (type), (nr), 0)
#define _IOR(type, nr, size)          _IOC(_IOC_READ, (type), (nr), (_IOC_TYPECHECK(size)))
#define _IOW(type, nr, size)          _IOC(_IOC_WRITE, (type), (nr), (_IOC_TYPECHECK(size)))
#define _IOWR(type, nr, size)         _IOC(_IOC_READ | _IOC_WRITE, (type), (nr), (_IOC_TYPECHECK(size)))
#define _IOR_BAD(type, nr, size)      _IOC(_IOC_READ, (type), (nr), sizeof(size))
#define _IOW_BAD(type, nr, size)      _IOC(_IOC_WRITE, (type), (nr), sizeof(size))

/* used to decode ioctl numbers.. */
#define _IOC_DIR(nr)                  (((nr) >> _IOC_DIRSHIFT) & _IOC_DIRMASK)
#define _IOC_TYPE(nr)                 (((nr) >> _IOC_TYPESHIFT) & _IOC_TYPEMASK)
#define _IOC_NR(nr)                   (((nr) >> _IOC_NRSHIFT) & _IOC_NRMASK)
#define _IOC_SIZE(nr)                 (((nr) >> _IOC_SIZESHIFT) & _IOC_SIZEMASK)

/* ...and for the drivers/sound files... */
#define IOC_IN                        (_IOC_WRITE << _IOC_DIRSHIFT)
#define IOC_OUT                       (_IOC_READ << _IOC_DIRSHIFT)
#define IOC_INOUT                     ((_IOC_WRITE | _IOC_READ) << _IOC_DIRSHIFT)
#define IOCSIZE_MASK                  (_IOC_SIZEMASK << _IOC_SIZESHIFT)
#define IOCSIZE_SHIFT                 (_IOC_SIZESHIFT)

#define DECLARE_WAIT_QUEUE_HEAD(a)

/* === Kernel/User space === */
#define copy_from_user(des, src, n)   (memcpy(des, src, n), 0)
#define copy_to_user(des, src, n)     (memcpy(des, src, n), 0)
#define __put_user(val, user)         (*(user) = (val))
#define __get_user(val, user)         ((val) = *(user))

/* === Interrupt === */
/*********************request_irq, disable_irq, enable_irq need to be provided by customer*********************/
#define request_irq(irq, isr, flag, name, data)   __IntRegisterIRQ(irq, isr, data)
#define disable_irq(i)                            __GICIntDisableIRQ(i)
#define enable_irq(i)                             __GICIntEnableIRQ(i)
#define clear_irq(i)                              __IntClearIRQStatus(i)
#define free_irq(i, data)
#define irqreturn_t                               void
#define mutex_trylock(a)                          pthread_mutex_trylock(a)
#define mutex_destroy(a)                          pthread_mutex_destroy(a)
#define IRQ_RETVAL(a)
#define IRQ_HANDLED
#define IRQ_NONE
#define register_chrdev(m, name, op)              (0)
#define unregister_chrdev(m, name)
#define IRQF_SHARED                               1
#define IRQF_DISABLED                             0x00000020
#define request_mem_region(addr, size, name)      (1)
#define release_mem_region(addr, size)
#define kill_fasync(queue, sig, flag)

#define ERR_OS_FAIL       (0xffff)
#define ERESTARTSYS       ERR_OS_FAIL
#ifdef EFAULT
#undef EFAULT
#endif
#define EFAULT            ERR_OS_FAIL
#ifdef ENOTTY
#undef ENOTTY
#endif
#define ENOTTY            ERR_OS_FAIL
#ifdef EINVAL
#undef EINVAL
#endif
#define EINVAL            ERR_OS_FAIL
#ifdef EBUSY
#undef EBUSY
#endif
#define EBUSY             ERR_OS_FAIL

/* kernel sync objects */
/**********the atomic operations need to be provided by customer**********/
//#define atomic_t                           __attribute__((section("cpu_dram"), aligned(4))) xmp_atomic_int_t//i32
//#define ATOMIC_INIT(a)                     XMP_ATOMIC_INT_INITIALIZER(a)//a
//#define atomic_inc(a)                      xmp_atomic_int_increment(a,1)//((*(a))++)
//#define atomic_read(a)                     xmp_atomic_int_value(a)//(a)
#define atomic_t          i32
#define ATOMIC_INIT(a)    a
#define atomic_inc(a)     ((*(a))++)
#define atomic_read(a)    (a)
typedef int               sig_atomic_t;
//typedef int sigset_t;
#define sigemptyset(set)
#define sigaddset(set, sig)
#define sigsuspend(set)
#define sigwait(set, signo)

#include "os_wrapper.h"

/* ---- spin_lock ---- */
#define spinlock_t                    rtos_mutex_t
#define spin_lock_init(a)             rtos_mutex_create(a)

#define isr_spin_lock_irqsave(a, b)
#define isr_spin_unlock_irqrestore(a, b)

#define spin_lock_irqsave(a, b)                     \
  {                                                 \
    b = irq_disable_save();                         \
  }
#define spin_unlock_irqrestore(a, b)                \
  {                                                 \
    irq_enable_restore(b);                          \
  }

#define spin_lock(a)                  rtos_mutex_take(*(a), portMAX_DELAY)
#define spin_unlock(a)                rtos_mutex_give(*(a))

#define getpid()                      ((int)xTaskGetCurrentTaskHandle())

/* ---- timer ---- */
#define timer_setup(timer, fn, data)                  \
({                                                    \
    if ((timer) == NULL) {                            \
      timer = xTimerCreate("",                        \
                            1000,                     \
                            0,                        \
                            data,                     \
                            fn);                      \
    }                                                 \
})

#define add_timer(timer, ms)                          \
({                                                    \
      xTimerChangePeriod(timer,                       \
                         pdMS_TO_TICKS(ms),           \
                         portMAX_DELAY);              \
})

#define del_timer(timer)                              \
({                                                    \
      xTimerStop(timer,                               \
                 portMAX_DELAY);                      \
})

#define isr_add_timer(timer, ms)                      \
({                                                    \
      if (__get_xPSR() & 0x1ff != 0) {                \
        xTimerChangePeriodFromISR(timer,              \
                                  pdMS_TO_TICKS(ms),  \
                                  portMAX_DELAY);     \
      } else {                                        \
        xTimerChangePeriod(timer,                     \
                           pdMS_TO_TICKS(ms),         \
                           portMAX_DELAY);            \
      }                                               \
})

#define isr_del_timer(timer)                          \
({                                                    \
      if (__get_xPSR() & 0x1ff != 0) {                \
        xTimerStopFromISR(timer,                      \
                          portMAX_DELAY);             \
      }                                               \
      else {                                          \
        xTimerStop(timer,                             \
                   portMAX_DELAY);                    \
      }                                               \
})

#define destroy_timer(timer)                          \
({                                                    \
      if (timer) {                                    \
        xTimerDelete(timer,                           \
                     portMAX_DELAY);                  \
      }                                               \
})

#define mod_timer(timer, ms)        add_timer(timer, ms)
#define isr_mod_timer(timer, ms)    isr_add_timer(timer, ms)

#define mdelay(ms)               vTaskDelay(pdMS_TO_TICKS((ms)))
#define msleep(ms)               mdelay(ms)

/* ---- kernel thread ---- */
#define IS_ERR(e)                ((e) == NULL)

/* ---- wait queue ---- */
#define ENC_EVENT_BIT0              (1 << 0)
#define wait_queue_head_t           EventGroupHandle_t
#define init_waitqueue_head(w)      \
({                                  \
        *(w) = xEventGroupCreate(); \
})
#define wait_event_interruptible(w, cond)               \
({                                                      \
    int __ret = 0;                                      \
    while (!(cond)) {                                   \
        /* use event group wait to release cpu */       \
        /* after woken up, check condition until true */\
        EventBits_t bit_state = xEventGroupWaitBits(    \
            w,                                          \
            ENC_EVENT_BIT0,                             \
            pdTRUE,                                     \
            pdFALSE,                                    \
            33);                                        \
    }                                                   \
    __ret;                                              \
})
#define wake_up_interruptible_all(a)                    \
({                                                      \
    BaseType_t xHigherPriorityTaskWoken, xResult;       \
    xResult = xEventGroupSetBitsFromISR(                \
        *(a),                                           \
        ENC_EVENT_BIT0,                                 \
        &xHigherPriorityTaskWoken);                     \
    if (xResult != pdFAIL) {                            \
        portYIELD_FROM_ISR( xHigherPriorityTaskWoken ); \
    }                                                   \
})


/* ---- counting semaphore ---- */
#define sema_init(s, v)               (*(s) = xSemaphoreCreateCounting((v), (v)))
#define down_interruptible(s)                         \
({                                                    \
    int __ret = 0;                                    \
    __ret = (xSemaphoreTake(*(s), portMAX_DELAY)-1); \
    __ret;                                            \
})
#define up(s)                         xSemaphoreGive(*(s))

#ifdef __cplusplus
}
#endif

#endif /* _DEV_COMMON_FREERTOS_H_ */
