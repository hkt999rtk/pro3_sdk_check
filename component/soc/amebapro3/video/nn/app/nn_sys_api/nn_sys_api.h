/******************************************************************************
*
* Copyright(c) 2007 - 2025 Realtek Corporation. All rights reserved.
*
******************************************************************************/

#ifndef _NN_SYS_API_H_
#define _NN_SYS_API_H_

#include <stdint.h>

/* Layout of an NN model kept in the standby-retained PSRAM region
 * (__vp_psram_nn_start__ .. __vp_psram_nn_end__).
 *
 * PSRAM keeps power across standby but its contents are otherwise
 * undefined, so the magic number is used to tell a valid retained model
 * apart from random power-on data. The header is placed at
 * __vp_psram_nn_start__ and the model binary follows immediately after. */
#define NN_RETENTION_MODEL_MAGIC  0x4D4E4E52u  /* 'R','N','N','M' */

typedef struct nn_retention_model_s {
	uint32_t magic;       /* must equal NN_RETENTION_MODEL_MAGIC */
	uint32_t model_size;  /* model binary length in bytes */
	uint8_t  model[];     /* model binary, model_size bytes, starts here */
} nn_retention_model_t;

void nn_init(void);

/**
 * @brief Disable the NPU clocks and functions and clear the local initialization state.
 * @note Stop NPU activity and release driver resources first. Callers must serialize
 *       access and coordinate NPU ownership across cores. Shared fast PCLK stays enabled.
 */
void nn_deinit(void);

/**
 * @brief Query the initialization state recorded by this core's HAL.
 * @return 1 after nn_init() completes, or 0 initially and after nn_deinit().
 * @note Does not check hardware or driver/inference readiness, or track changes
 *       made by another core or by power management. Serialize access with init/deinit.
 */
int nn_is_inited(void);

#if defined(CONFIG_ARM_CORE_CA32) && (CONFIG_ARM_CORE_CA32 == 1)
/* Read a model from the file system (e.g. "vfs:/xxx.nb") and store it,
 * together with an nn_retention_model_t header, into the standby-retained
 * PSRAM region. Intended to be called by the AP application before entering
 * standby so the VP can reload the model from PSRAM on wake.
 *
 * Returns 0 on success, -1 on failure (open/read error, or the header plus
 * model would not fit before __vp_psram_nn_end__). */
int nn_retention_model_save(const char *model_path);
#endif

#endif
