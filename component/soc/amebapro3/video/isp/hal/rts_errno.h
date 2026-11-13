/******************************************************************************
*
* Copyright(c) 2007 - 2025 Realtek Corporation. All rights reserved.
*
******************************************************************************/
#ifndef _RTS_ERRNO_H_
#define _RTS_ERRNO_H_

#ifndef RTS_RETURN
#define RTS_RETURN(e) (-(e))
#endif

#ifndef RTS_IS_ERR
#define RTS_IS_ERR(r)  ((r) < 0)
#endif

#ifndef RTS_ERRNO
#define RTS_ERRNO(r) ((r) < 0 ? (-(r)) : (r))
#endif

enum RTS_ERRNO {
	RTS_OK   = 0,
	RTS_FAIL = 1,
	RTS_E_NULL_POINT  = 10,
	RTS_E_NO_MEMORY,
	RTS_E_NOT_REALIZED,
	RTS_E_EXIST,
	RTS_E_NOT_EXIST,
	RTS_E_NOT_FOUND,
	RTS_E_NOT_REGISTERED,
	RTS_E_NOT_READY,
	RTS_E_EMPTY,
	RTS_E_NOT_EMPTY,
	RTS_E_FULL,
	RTS_E_INVALID_ARG,
	RTS_E_INVALID_TYPE,
	RTS_E_NOT_MATCH,
	RTS_E_OPEN_FAIL,
	RTS_E_CLOSE_FAIL,
	RTS_E_NOT_OPEN,
	RTS_E_GET_FAIL,
	RTS_E_SET_FAIL,
	RTS_E_NOT_SUPPORT,
	RTS_E_INITIALIZED,
	RTS_E_NOT_INITIALIZED,
	RTS_E_INUSE,
	RTS_E_WRITE_FAIL,
	RTS_E_READ_FAIL,
	RTS_E_LOCK_FAIL,
	RTS_E_INVALID_LENGTH,
	RTS_E_TIMEOUT,
	RTS_E_THREAD_FAIL,
	RTS_E_OVERFLOW,
	RTS_E_BIG,
	RTS_E_PERM,
	RTS_E_NO_ENT,
	RTS_E_IO,
	RTS_E_AGAIN,
	RTS_E_NO_DEV,
	RTS_E_NO_SYS,
	RTS_E_IOCTL,
	RTS_E_LESS_DATA,
	RTS_E_OUT_OF_RANGE,
	RTS_E_INVALID_DATA,
	RTS_E_UNKNOWN,
	RTS_EXIT
};

#endif /* _RTS_ERRNO_H_ */
