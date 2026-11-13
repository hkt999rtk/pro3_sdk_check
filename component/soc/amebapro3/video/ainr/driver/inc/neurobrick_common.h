/****************************************************************************
*
*    The MIT License (MIT)
*
*    Copyright (c) 2023 - 2024 Vivante Corporation
*
*    Permission is hereby granted, free of charge, to any person obtaining a
*    copy of this software and associated documentation files (the "Software"),
*    to deal in the Software without restriction, including without limitation
*    the rights to use, copy, modify, merge, publish, distribute, sublicense,
*    and/or sell copies of the Software, and to permit persons to whom the
*    Software is furnished to do so, subject to the following conditions:
*
*    The above copyright notice and this permission notice shall be included in
*    all copies or substantial portions of the Software.
*
*    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
*    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
*    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
*    AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
*    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
*    FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
*    DEALINGS IN THE SOFTWARE.
*
*****************************************************************************
*
*    Note: This software is released under dual MIT and GPL licenses. A
*    recipient may use this file under the terms of either the MIT license or
*    GPL License. If you wish to use only one license not the other, you can
*    indicate your decision by deleting one of the above license notices in your
*    version of this file.
*
*****************************************************************************/

#ifndef _NEURO_BRICK_COMMON_H
#define _NEURO_BRICK_COMMON_H

#define VERSION_MAJOR         0

#define VERSION_MINOR         1

#define VERSION_SUB_MINOR     0

#define VERSION_PATCH   "999d3cb"

#ifdef  __cplusplus
extern "C" {
#endif

typedef unsigned char       nb_uint8_t;

/*! \brief An 16-bit unsigned value.
 * \version 1.0
 */
typedef unsigned short      nb_uint16_t;

/*! \brief An 32-bit unsigned value.
 * \version 1.0
 */
typedef unsigned int        nb_uint32_t;

/*! \brief An 64-bit unsigned value.
 * \version 1.0
 */
typedef unsigned long long  nb_uint64_t;

/*! \brief An 8-bit signed value.
 * \version 1.0
 */
typedef signed char         nb_int8_t;

/*! \brief An 16-bit signed value.
 * \version 1.0
 */
typedef signed short        nb_int16_t;

/*! \brief An 32-bit signed value.
 * \version 1.0
 */
typedef signed int          nb_int32_t;

/*! \brief An 64-bit signed value.
 * \ingroup group_global
 * \version 1.0
 */
typedef signed long long    nb_int64_t;

/*! \brief An 8 bit ASCII character.
 * \version 1.0
 */
typedef char                nb_char_t;

/*! \brief An 32 bit float value.
 * \version 1.0
 */
typedef float               nb_float_t;

/*! \brief Sets the standard enumeration type size to be a fixed quantity.
 * \version 1.0
 */
typedef nb_int32_t         nb_enum;

/*! \brief a void pointer.
 * \version 1.0
 */
typedef void               *nb_ptr;

/*! \brief A 64-bit float value (aka double).
 */
typedef double              nb_float64_t;

/*! \brief address type.
 * \version 1.0
 */
typedef unsigned long long  nb_address_t;

typedef enum _nb_bool_e {
	/*! \brief The "false" value. */
	nb_false_e = 0,
	/*! \brief The "true" value. */
	nb_true_e,
} nb_bool_e;

typedef enum _nb_status {
	/*!< \brief Indicates the failure */
	NB_ERROR_FAILURE               = -5,
	/*!< \brief Indicates it's supported by driver implementation */
	NB_ERROR_NOT_SUPPORTED         = -4,
	/*!< \brief Indicates some arguments are not valid */
	NB_ERROR_INVALID_ARGUMENTS     = -3,
	/*!< \brief Indicates there are some IO related error */
	NB_ERROR_IO                    = -2,
	/*!< \brief Indicates NeroBrick timeout, could be NeroBrick stuck somewhere */
	NB_ERROR_TIMEOUT               = -1,
	/*!< \brief Indicates the execution is successfuly */
	NB_SUCCESS                     =  0,
} nb_status_e;

#define IN
#define OUT

/*! \brief A zero value for pointer
 *\version 1.0
 */
#ifndef NB_NULL
#define NB_NULL 0
#endif

#define nbOnError(func)  \
    status = func; \
    if (status != NB_SUCCESS) { \
        goto onError; \
    }

#define nbGoOnError(func)  \
    status = func; \
    goto onError;

#define nbIsNULL(object)  \
    if (NB_NULL == object) { \
        status = NB_ERROR_INVALID_ARGUMENTS; \
        goto onError; \
    }

#ifndef nbdENABLE_DEBUG_LOG
#define nbdENABLE_DEBUG_LOG                3
#endif

#define PRINTF(level, ...)  printf("%s[%d], ",  __FUNCTION__, __LINE__); \
                            printf(__VA_ARGS__);

#if (nbdENABLE_DEBUG_LOG == 4)
#define nb_debug(...)    PRINTF(NB_LOG_DEBUG, __VA_ARGS__)
#define nb_info(...)     PRINTF(NB_LOG_INFO, __VA_ARGS__)
#define nb_warning(...)  PRINTF(NB_LOG_WARN, __VA_ARGS__)
#define nb_error(...)    PRINTF(NB_LOG_ERROR, __VA_ARGS__)
#elif (nbdENABLE_DEBUG_LOG == 3)
#define nb_debug(...)
#define nb_info(...)     PRINTF(NB_LOG_INFO, __VA_ARGS__)
#define nb_warning(...)  PRINTF(NB_LOG_WARN, __VA_ARGS__)
#define nb_error(...)    PRINTF(NB_LOG_ERROR, __VA_ARGS__)
#elif (nbdENABLE_DEBUG_LOG == 2)
#define nb_debug(...)
#define nb_info(...)
#define nb_warning(...)  PRINTF(NB_LOG_WARN, __VA_ARGS__)
#define nb_error(...)    PRINTF(NB_LOG_ERROR, __VA_ARGS__)
#elif (nbdENABLE_DEBUG_LOG == 1)
#define nb_debug(...)
#define nb_info(...)
#define nb_warning(...)
#define nb_error(...)    PRINTF(NB_LOG_ERROR, __VA_ARGS__)
#else
#define nb_debug(...)
#define nb_info(...)
#define nb_warning(...)
#define nb_error(...)
#endif

#ifdef  __cplusplus
}
#endif

#endif
