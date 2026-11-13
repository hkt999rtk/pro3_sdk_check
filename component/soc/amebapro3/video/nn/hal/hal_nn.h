/******************************************************************************
*
* Copyright(c) 2007 - 2025 Realtek Corporation. All rights reserved.
*
******************************************************************************/

#ifndef _HAL_NN_H_
#define _HAL_NN_H_

#ifdef __cplusplus
extern "C" {
#endif

/* Callers must serialize these APIs and coordinate NPU ownership across cores. */

/**
 * @brief Initialize NPU clocks and functions; repeated calls have no effect.
 * @note Call NN_DeInit() before reinitializing after hardware state is lost.
 */
void NN_Init(void);

/**
 * @brief Disable NPU clocks and functions and clear the local initialization state.
 * @note Stop NPU activity and release driver resources before calling.
 *       Always disables hardware, even if the local state is uninitialized.
 *       The shared fast PCLK remains enabled for AINR and the encoder.
 */
void NN_DeInit(void);

/**
 * @brief Query the initialization state recorded by this core's HAL.
 * @return 1 after NN_Init() completes, or 0 initially and after NN_DeInit().
 * @note Does not check hardware or driver/inference readiness, and does not
 *       track changes made by another core or by power management.
 */
int NN_IsInited(void);

#ifdef __cplusplus
}
#endif

#endif /* _HAL_NN_H_ */
