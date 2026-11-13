/******************************************************************************\
|* Copyright (c) 2017-2024 by Vivante Corporation.  All Rights Reserved.      *|
|*                                                                            *|
|* The material in this file is confidential and contains trade secrets of    *|
|* of Vivante Corporation.  This is proprietary information owned by Vivante  *|
|* Corporation.  No part of this work may be disclosed, reproduced, copied,   *|
|* transmitted, or used in any way for any purpose, without the express       *|
|* written permission of Vivante Corporation.                                 *|
|*                                                                            *|
\******************************************************************************/

#ifndef __VIP_HAL_OS_PORT_H__
#define __VIP_HAL_OS_PORT_H__

#include <vip_lite_config.h>

typedef void *viphal_file_t;

typedef enum _viphal_file_mode {
	FILE_CREATE    = 0,
	FILE_APPEND,
	FILE_READ,
	FILE_CREATETEXT,
	FILE_APPENDTEXT,
	FILE_READTEXT,
} viphal_file_mode_e;

typedef enum _viphal_file_seek {
	VIPHAL_FILE_CUR      = 0x0,
	VIPHAL_FILE_SET      = 0x1,
	VIPHAL_FILE_END      = 0x2,
	VIPHAL_FILE_BACKWARD = 0x4,
} viphal_file_seek_e;

#define VIPPTR_TO_UINT32(p) \
( \
    (vip_uint32_t) (viphal_uintptr_t) (p)\
)

#define VIPUINT32_TO_PTR(u) \
( \
    (vip_ptr) (viphal_uintptr_t) (u)\
)

#define VIPPTR_TO_UINT64(p) \
( \
    (vip_uint64_t) (viphal_uintptr_t) (p)\
)

#define VIPUINT64_TO_PTR(u) \
( \
    (vip_ptr) (viphal_uintptr_t) (u)\
)

#define vip_print(...)    viphal_os_print(VIP_LOG_PRINT, __VA_ARGS__)
#define vip_debug(...)
#define vip_info(...)
#define vip_warning(...)
#define vip_error(...)    viphal_os_print(VIP_LOG_ERROR, __VA_ARGS__)

/*
@brief open a file.
@param name, file name.
@param mode, opem mode.
*/
viphal_file_t viphal_os_open_file(
	vip_char_t *name,
	viphal_file_mode_e mode
);

vip_status_e viphal_os_read_file(
	void *dst,
	vip_size_t size,
	viphal_file_t file
);

vip_status_e viphal_os_write_file(
	void *data,
	vip_size_t size,
	viphal_file_t file
);

/*
@brief seek file location.
*/
vip_status_e viphal_os_seek_file(
	vip_size_t offset,
	viphal_file_seek_e pos,
	viphal_file_t  file
);

vip_size_t viphal_os_get_file_size(
	const vip_char_t *name
);

void viphal_os_close_file(
	viphal_file_t file
);

/*
@brief allocate memory from system.
@param size, the size of memory.
@param memory, memory object.
*/
vip_status_e viphal_allocate_memory(
	IN vip_size_t Size,
	OUT void **memory
);

/*
@brief free memory.
@param memory, memory object.
*/
vip_status_e viphal_free_memory(
	IN void *memory
);

/*
@brief copy src to dst.
@param size the size of memory should be copy.
*/
vip_status_e viphal_copy_memory(
	IN void *dst,
	IN const void *src,
	IN vip_size_t size
);

/*
@brief zero memory.
@param size the size of memory should be zero.
*/
vip_status_e viphal_zero_memory(
	IN void *memory,
	IN vip_size_t size
);

/*
@brief copy string src to dest.
@param dest, destation buffer.
@param src, resource buffer.
*/
vip_char_t *viphal_os_strcpy(
	vip_char_t *dest,
	vip_char_t *src
);

vip_uint32_t viphal_os_strlen(
	const vip_char_t *str
);

/*
@brief print message on console
@param message, which message should be print.
*/
vip_status_e viphal_os_print(
	vip_log_level log_level,
	IN const vip_char_t *message,
	...
);

vip_status_e viphal_os_snprint(
	OUT     vip_char_t *buffer,
	IN const vip_uint32_t size,
	IN  const vip_char_t *message,
	...
);
#endif /* __GC_VIP_USER_OS_H__ */
