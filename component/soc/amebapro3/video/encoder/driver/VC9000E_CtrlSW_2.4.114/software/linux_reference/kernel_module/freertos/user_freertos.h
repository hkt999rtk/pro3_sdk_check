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
#ifndef _USER_FREERTOS_H_
#define _USER_FREERTOS_H_

#include "base_type.h"

#include <stddef.h> // size_t
#include <stdint.h>
#include "osal.h"

//u32, MSB 4bit for device type, LSB 28bit for accuml count
enum {
       ENC_FD,
       // DEC_FD,
       MEM_FD,

       VPU_FD_MAX
};
#define VPU_FD_NUM (VPU_FD_MAX)

#define VPU_FD_TYPE_BIT  (28)
#define VPU_FD_TYPE_MASK (0xFU << VPU_FD_TYPE_BIT)
#define VPU_FD_CNT_MASK  (~VPU_FD_TYPE_MASK)

#define VPU_FILP_MAX_NUM    (16)

typedef struct {
    unsigned int                f_flags;
    void                        *private_data;
    int                         fd;
}rtos_vpu_filp;

typedef struct {
    void *ctx;      //context of kernel driver
    char *name;
    u32 fd;                 //bit[31..28]: fd type, bit[27..0]: open count
    rtos_vpu_filp filp;
    unsigned int ref_cnt;
    int (*_open)(void *inode, rtos_vpu_filp *filp);
    int (*_release)(void *inode, rtos_vpu_filp *filp);
    long (*_ioctl)(rtos_vpu_filp *filp, unsigned int cmd, void *arg);
    void (*_cleanup)(void);
}rtos_vpu_dev;

int encoder_rtos_driver_init(void);
int freertos_open(const char *dev_name, int flag);
void freertos_close(int fd);
long freertos_ioctl(int filp, unsigned int cmd, void *arg);
void encoder_rtos_driver_deinit(void);
void *freertos_mmap(unsigned long long pa, unsigned long sz);

#endif /* _USER_FREERTOS_H_ */
