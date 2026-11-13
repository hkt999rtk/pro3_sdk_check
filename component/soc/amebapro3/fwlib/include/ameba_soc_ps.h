/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _AMEBA_SOCPS_H_
#define _AMEBA_SOCPS_H_

typedef uint32_t (*WAKEEVENT_HOOK_FUN)(uint32_t, void *param_ptr);

enum wakeup_mask {
	WAKEUP_NULL	= 0,
	WAKEUP_FP	= 1,
	WAKEUP_NP	= 2,
	WAKEUP_AP	= 3,
};

enum xtal_mode_sleep {
	XTAL_OFF = 0,
	XTAL_LPS_Without_40M = 1,
	XTAL_LPS_With_40M = 2,
	XTAL_Normal = 3,
	XTAL_HP = 4,
};


typedef struct {
	u32 Module;
	enum wakeup_mask Wakeup;
	WAKEEVENT_HOOK_FUN event_init_hook_fun;
	WAKEEVENT_HOOK_FUN event_start_hook_fun;
	WAKEEVENT_HOOK_FUN event_handle_hook_fun;
} WakeEvent_TypeDef;

typedef struct {
	u32 Module;
	u32 MEM_SD;
	u32 MEM_DS;
	u32 MEM_LS;
} MEMMode_TypeDef;

extern void SOCPS_SleepPG(void);
extern void SOCPS_SleepCG(void);
extern void SOCPS_SleepCG_FULLMAC(void);
extern void SOCPS_DeepSleep(void);

extern void SOCPS_sleepInit(void);
//extern void SOCPS_AP_suspend_config(u32 Protection);
//extern void SOCPS_AP_resume_config(u32 Protection);
//extern void SOCPS_PSRAM_ClkSet(u8 Source);
//extern void SOCPS_Voltage_Switch(u8 sleep);

//extern int SOCPS_AONWakeReason(void);

//extern void SOCPS_SetNPWakeEvent(u32 Option, u32 NewStatus);
//extern void SOCPS_SetAPWakeEvent(u32 Option, u32 NewStatus);
//extern void SOCPS_DeepSleep_RAM(void);
//extern void SOCPS_HWSleepFilter(u32 enable);

#endif  //_AMEBA_SOCPS_H_
