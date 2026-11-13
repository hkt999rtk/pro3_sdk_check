/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _AMEBA_DEBUGTIMER_H_
#define _AMEBA_DEBUGTIMER_H_

/* No Extra debugtimer in Amebapro3, use DWT to simulate, not supoort in Cortex A */

#if defined(CONFIG_ARM_CORE_CM0)
#error "NOT SUPPORT CORTEX M0, please add cortex M0 implementation"
#endif


/* Exported constants --------------------------------------------------------*/

/* group: DEBUGTIMER_Exported_Functions */
/* Since all the members in the group are __STATIC_INLINE which will not be extracted to the doxygen doc,
  original comment has been deleted to avoid there is nothing displayed under the group. */


#if defined(CONFIG_ARM_CORE_CA32)

/**
  * @brief  Enables or disables the DebugTimer peripheral
  *   The debug timer is a free run timer works at 1MHZ.
  * @param  NewState: new state of the Debug timer peripheral.
  *   This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
__STATIC_INLINE void DTimer_Cmd(bool Newstatus)
{
	// do nothing, GIC has been initized to 10Mhz
	(void)Newstatus;
}

/**
  * @brief  Get the u32 passed time.
  * @param  None
  * @retval current counter
  */
__STATIC_INLINE u32 DTimesPassedtime32_Get(u32 OldTime)
{
	u32 MHZ = __get_CNTFRQ() / 1000000;
	u32 curr = (u32)((__get_CNTPCT() / MHZ) & 0xffffffff);

	if (curr < OldTime) {
		return (curr - OldTime + 0xFFFFFFFF + 1);
	} else {
		return (curr - OldTime);
	}

}

/**
  * @brief  Get the u64 passed time.
  * @param  None
  * @retval current counter
  */
__STATIC_INLINE u64 DTimesPassedtime64_Get(u64 OldTime)
{
	u32 MHZ = __get_CNTFRQ() / 1000000;
	u64 curr = __get_CNTPCT() / MHZ;	// 1us unit
	return (curr - OldTime);
}

/**
  * @brief  Get the Debug timer u32 current Counter.
  * @param  None
  * @retval current counter
  */
__STATIC_INLINE u32 DTimestamp_Get(void)
{
	u32 MHZ = __get_CNTFRQ() / 1000000;
	return (u32)((__get_CNTPCT() / MHZ) & 0xffffffff);
}

/**
  * @brief  Get the Debug timer u64 current Counter.
  * @param  None
  * @retval current counter
  */
__STATIC_INLINE u64 DTimestamp64_Get(void)
{
	u32 MHZ = __get_CNTFRQ() / 1000000;
	return __get_CNTPCT() / MHZ;
}

/**
  * @brief  Set the Debug timer current Counter.
  * @param  NewValue: new debug counter.
  * @retval current counter
  */
__STATIC_INLINE void DTimestamp_Set(u64 NewValue)
{
	// do nothing, GIC not only used by debugtimer
	(void)NewValue;
}

/**
  * @brief  Get the Debug timer atom value.
  *   The read action won't trigger Atom increase
  * @param  None
  * @retval atom counter
  */
__STATIC_INLINE u32 DTimer_AtomGet(void)
{
	return 0;
}

/**
  * @brief  Get the Debug timer atom value + 1.
  *   The read action will trigger Atom increase
  * @param  None
  * @retval atom counter
  */
__STATIC_INLINE u32 DTimer_AtomIncGet(void)
{
	return 0;
}

/**
  * @brief  Set the Debug timer atom value .
  * @param  New value
  * @retval None
  */
__STATIC_INLINE void DTimer_AtomSet(u32 NewValue)
{
	(void)NewValue;
}

/**
  * @brief  Get the Debug timer Scratch value.
  * @param  None
  * @retval atom scratch
  */
__STATIC_INLINE u32 DTimer_ScratchGet(void)
{
	return 0;
}

/**
  * @brief  Set the Debug timer Scratch value .
  * @param  New value
  * @retval None
  */
__STATIC_INLINE void DTimer_ScratchSet(u32 NewValue)
{
	(void)NewValue;
}
/* end of the function group */

#else
/* for convert tick to 1us */
__STATIC_INLINE u32 DTimer_ClkGet(void)
{
#if defined(CONFIG_BUILD_ROM)
	extern u32 DelayClkGet(void);
	return DelayClkGet();
#else
	extern u32 CPU_ClkGet(void);
	return CPU_ClkGet();
#endif
}

/**
  * @brief  Enables or disables the DebugTimer peripheral
  *   The debug timer is a free run timer works at 1MHZ.
  * @param  NewState: new state of the Debug timer peripheral.
  *   This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
__STATIC_INLINE void DTimer_Cmd(bool Newstatus)
{
	if (Newstatus == ENABLE) {
		CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
		DWT->CYCCNT = 0;
		DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;
	} else {
		CoreDebug->DEMCR &= (~CoreDebug_DEMCR_TRCENA_Msk);
		DWT->CTRL &= (~DWT_CTRL_CYCCNTENA_Msk);
	}
}

/**
  * @brief  Get the u32 passed time.
  * @param  None
  * @retval current counter
  */
__STATIC_INLINE u32 DTimesPassedtime32_Get(u32 OldTime)
{
	u32 MHZ = DTimer_ClkGet() / 1000000;
	u32 curr = DWT->CYCCNT / MHZ;		// convert to 1us
	if (curr < OldTime) {
		return (curr - OldTime + (0xFFFFFFFF / MHZ) + 1);
	} else {
		return (curr - OldTime);
	}

}

/**
  * @brief  Get the u64 passed time.
  * @param  None
  * @retval current counter
  */
__STATIC_INLINE u64 DTimesPassedtime64_Get(u64 OldTime)
{
	return DTimesPassedtime32_Get((u32)OldTime);
}

/**
  * @brief  Get the Debug timer u32 current Counter.
  * @param  None
  * @retval current counter
  */
__STATIC_INLINE u32 DTimestamp_Get(void)
{
	u32 MHZ = DTimer_ClkGet() / 1000000;
	u32 curr = DWT->CYCCNT / MHZ;		// convert to 1us

	return curr;
}

/**
  * @brief  Get the Debug timer u64 current Counter.
  * @param  None
  * @retval current counter
  */
__STATIC_INLINE u64 DTimestamp64_Get(void)
{
	return (u64)DTimestamp_Get();
}

/**
  * @brief  Set the Debug timer current Counter.
  * @param  NewValue: new debug counter.
  * @retval current counter
  */
__STATIC_INLINE void DTimestamp_Set(u64 NewValue)
{
	DWT->CYCCNT = (u32)NewValue;
}

/**
  * @brief  Get the Debug timer atom value.
  *   The read action won't trigger Atom increase
  * @param  None
  * @retval atom counter
  */
__STATIC_INLINE u32 DTimer_AtomGet(void)
{
	return 0;
}

/**
  * @brief  Get the Debug timer atom value + 1.
  *   The read action will trigger Atom increase
  * @param  None
  * @retval atom counter
  */
__STATIC_INLINE u32 DTimer_AtomIncGet(void)
{
	return 0;
}

/**
  * @brief  Set the Debug timer atom value .
  * @param  New value
  * @retval None
  */
__STATIC_INLINE void DTimer_AtomSet(u32 NewValue)
{
	(void)NewValue;
}

/**
  * @brief  Get the Debug timer Scratch value.
  * @param  None
  * @retval atom scratch
  */
__STATIC_INLINE u32 DTimer_ScratchGet(void)
{
	return 0;
}

/**
  * @brief  Set the Debug timer Scratch value .
  * @param  New value
  * @retval None
  */
__STATIC_INLINE void DTimer_ScratchSet(u32 NewValue)
{
	(void)NewValue;
}
/* end of the function group */

#endif

/* MANUAL_GEN_END */
#endif /* _AMEBA_DEBUGTIMER_H_ */
