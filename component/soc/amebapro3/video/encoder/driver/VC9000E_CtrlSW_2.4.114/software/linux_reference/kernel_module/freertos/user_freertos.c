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

#include "user_freertos.h"
#include "io_tools.h"
#include "memalloc_freertos.h"
#include "vcx_driver_freertos.h"
#include "vcx_vcmd_priv_freertos.h"

unsigned long vcmd_isr_polling = 1;
int vsi_kloglvl = LOGLVL_ERROR;
char *enc_dev_n = "vsi_vcx";
unsigned long ddr_offset;
u32 arbiter_weight = 0x1d;
u32 arbiter_urgent;
u32 arbiter_timewindow = 0x1d;
u32 arbiter_bw_overflow;
unsigned long sw_timeout_time = SW_TIMEOUT_TIME_FOR_ARBITER;

int hantroenc_normal_init(rtos_vpu_dev *_vdev);
int hantroenc_vcmd_init(rtos_vpu_dev *_vdev);

//extern u8 user_freertos_vcmd_en;
static const u8 user_freertos_vcmd_en = 1;
rtos_vpu_dev g_vpu_dev[VPU_FD_NUM] = {{NULL,}, };

static SemaphoreHandle_t dev_open_mutex;

/** All drivers's probe should be added and be called by main() explicitly,
	* just call once
	*/
int encoder_rtos_driver_init(void)
{
	memset(g_vpu_dev, 0, sizeof(g_vpu_dev));

	//Init the memory allocator firstly
	if (memalloc_init(&g_vpu_dev[MEM_FD]) != 0) {
		PDEBUG("memalloc_init error\n");
		ASSERT(0);
	}
	g_vpu_dev[MEM_FD].name = MEMALLOC_MODULE_PATH;
	g_vpu_dev[MEM_FD].fd = MEM_FD << VPU_FD_TYPE_BIT;

	//encoder device
	if (user_freertos_vcmd_en == 0) {
		if (hantroenc_normal_init(&g_vpu_dev[ENC_FD]) != 0) {
			PDEBUG("hantroenc_init error\n");
			ASSERT(0);
		}
	} else {
		printf("hantroenc_vcmd_init...\n");
		if (hantroenc_vcmd_init(&g_vpu_dev[ENC_FD]) != 0) {
			PDEBUG("hantroenc_vcmd_init error\n");
			ASSERT(0);
		}
		printf("hantroenc vcmd init successfully\n");
	}
	g_vpu_dev[ENC_FD].name = ENC_MODULE_PATH;
	g_vpu_dev[ENC_FD].fd = ENC_FD << VPU_FD_TYPE_BIT;

	dev_open_mutex = xSemaphoreCreateMutex();

	return 0;
}

/** coressponding to encoder_rtos_driver_init, if you wanto recall encoder_rtos_driver_init,
	* please call this function to de-init firstly.
	*/
void encoder_rtos_driver_deinit(void) {
	int i;
	rtos_vpu_dev *p_dev;

	xSemaphoreTake(dev_open_mutex, portMAX_DELAY);
	for (i=0; i<VPU_FD_NUM; i++) {
		p_dev = &g_vpu_dev[i];
		p_dev->_cleanup();
	}
	memset(g_vpu_dev, 0, sizeof(g_vpu_dev));
	xSemaphoreGive(dev_open_mutex);

	vSemaphoreDelete(dev_open_mutex);
}

/** Open a device. -1 means open failed.
	*/
int freertos_open(const char *dev_name, int flag)
{
	int i;
	int fd = 0;
	int type = 0;
	rtos_vpu_dev *p_dev = NULL;

	(void)flag;
	for (i=0; i<VPU_FD_NUM; i++) {
		p_dev = &g_vpu_dev[i];
		if (p_dev->name == NULL) {
			continue;
		}

		if (!strcmp(dev_name, p_dev->name)) {
			xSemaphoreTake(dev_open_mutex, portMAX_DELAY);
			fd = (p_dev->fd + 1) & VPU_FD_CNT_MASK;
			type = p_dev->fd & VPU_FD_TYPE_MASK;
			p_dev->fd = type | fd;

			p_dev->ref_cnt++;
			if (p_dev->ref_cnt == 1) {
				p_dev->_open(NULL, &p_dev->filp);
			}
			xSemaphoreGive(dev_open_mutex);
			return p_dev->fd;
		}
	}
	return -1;
}

/** Get device handle via fd. NULL means failed.
	*/
static rtos_vpu_dev *_get_dev(int fd) {
	unsigned int type = (unsigned int)fd;
	rtos_vpu_dev *p_dev;

	type >>= VPU_FD_TYPE_BIT;

	if (type >= VPU_FD_NUM) {
		PDEBUG("freertos_release fd 0x%x error\n", fd);
		return NULL;
	}

	p_dev = &g_vpu_dev[type];
	if (p_dev->ref_cnt == 0) {
		printf("error: no open operation to this dev yet.\r\n");
		p_dev = NULL;
	}

	return p_dev;
}

/** Close a device.
	*/
void freertos_close(int fd) {
	rtos_vpu_dev *p_dev;

	xSemaphoreTake(dev_open_mutex, portMAX_DELAY);
	p_dev = _get_dev(fd);
	if (p_dev) {
		if (p_dev->ref_cnt) {
			p_dev->ref_cnt--;
			if (p_dev->ref_cnt == 0) {
				p_dev->_release(NULL, &p_dev->filp);
			}
		}
	}
	xSemaphoreGive(dev_open_mutex);
}

/** ioctl.
	*/
long freertos_ioctl(int fd, unsigned int cmd, void *arg) {
	rtos_vpu_dev *p_dev;
	rtos_vpu_filp filp;

	p_dev = _get_dev(fd);
	if (p_dev == NULL) {
		return -EFAULT;
	}
	filp = p_dev->filp;
	filp.fd = fd;
	return p_dev->_ioctl(&filp, cmd, arg);
}

/** mmap.
	*/
void *freertos_mmap(unsigned long long pa, unsigned long sz) {
	return DirectMemoryMap(pa, sz);
}

