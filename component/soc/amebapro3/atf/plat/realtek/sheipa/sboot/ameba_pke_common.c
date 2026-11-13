#define __FPU_PRESENT			1
#define __CORTEX_A				7
#include "cmsis_ca/core_ca.h"
#include "basic_types.h"
#include "hal_platform.h"
#include "sysreg_syson.h"

#include "ameba_pke.h"

#include <stdio.h>
#include <string.h>

PKE_TypeDef *pke_get_pke_addr(void)
{
	return (PKE_TypeDef *)PKE_REG_BASE_S;
}

/**
 * @brief  Lock PKE hardware sema, and clear PKE memory
 * @retval RTK_SUCCESS
 * @retval PKE_MUTEX_TIMEOUT
 */
void pke_lock_mutex(void)
{
	PKE_TypeDef *PKE = pke_get_pke_addr();
	int mutex_timeout = PKE_MUTEX_TIMEOUT;
	int timeout_cnt = 0;

	while (1) {
		if (PKE_GET_MUTEX_STATUS(PKE->PKE_MUTEX) == 0) {
			break;
		}

		if ((timeout_cnt++) >= mutex_timeout) {
			timeout_cnt = 0;
		}
	}
}

/**
 * @brief  Unlock PKE hardware sema
 * @retval RTK_SUCCESS
 */
void pke_unlock_mutex(void)
{
	PKE_TypeDef *PKE = pke_get_pke_addr();

	/* There may be some temporary parameters in these memories that affect the calculation results and secure.
	   Thus clear PKE memory when start use engine */
	memset((void *)PKE_MMEM_BASE, 0, 0x800);
	memset((void *)PKE_TMEM_BASE, 0, 0x800);

	PKE->PKE_MUTEX = 0;
}

/**
 * @brief  Set full op_size memory.
 * @param  dst: data in little endian
 * @param  src: data in little endian
 * @param  len: data length in bytes
 * @param  op_size: operands full memory size in bytes, fill high address with zeros
 * @retval RTK_SUCCESS
 */
int pke_engine_write(void *dst, const void *src, size_t len, size_t op_size)
{
	memcpy(dst, src, len);

	if (len < op_size) {
		memset((void *)((uint8_t *)dst + len), 0, op_size - len);
	}

	return RTK_SUCCESS;
}

/**
 * @brief  Set PKE function entry
 * @param  function_id: PKE_Other_Function_ID
 * @retval RTK_SUCCESS
 */
int pke_function_set_entry(uint8_t function_id)
{
	return pke_engine_write(PKE_ECC_TMEM_FUNCTION_ENTRY, &function_id, 1, 4);
}

/**
 * @brief  Reverse buffer in Byte
 * @param  arr: target buffer address
 * @param  len: buffer length in bytes
 * @retval None
 */
void pke_reverse_array(uint8_t *arr, size_t len)
{
	int start = 0;
	int end = len - 1;
	uint8_t temp;

	if (arr == NULL || len <= 0) {
		return;
	}

	while (start < end) {
		temp = arr[start];
		arr[start] = arr[end];
		arr[end] = temp;
		start++;
		end--;
	}
}
