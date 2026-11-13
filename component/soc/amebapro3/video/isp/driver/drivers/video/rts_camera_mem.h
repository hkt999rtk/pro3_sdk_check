/*
 * Copyright 2024 Realtek
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _RTS_CAMERA_MEM_H
#define _RTS_CAMERA_MEM_H

#include <stdint.h>
#include <stddef.h>

/* Memory attribute flags (upper 16 bits of the alloc flag) */
#define RTS_MEM_ATTR_CACHE     (0 << 16)
#define RTS_MEM_ATTR_NOCACHE   (1 << 16)

int rtscam_mem_init(uint32_t base, uint32_t size);
void rtscam_mem_cleanup(void);
uint8_t *rtscam_mem_alloc(size_t size, uint32_t *phy_addr,
						  uint32_t mem_attr, const char *info);
void rtscam_mem_free(void *virt_addr);
void rtscam_mem_flush_cache(void *virt_addr, size_t size);
void rtscam_mem_inv_cache(void *virt_addr, size_t size);

#endif /* _RTS_CAMERA_MEM_H */
