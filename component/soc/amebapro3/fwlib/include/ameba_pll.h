/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _AMEBA_PLL_H_
#define _AMEBA_PLL_H_

/** @defgroup PLL_Ops PLL Operations vtable
  * @{
  */
typedef struct PLL_DeviceTag PLL_Device;

typedef struct {
	void (*Init)(PLL_Device *pll);
	void (*Deinit)(PLL_Device *pll);
	void (*SetFreq)(PLL_Device *pll, u32 Freq_MHz);
	u32(*ClkGet)(PLL_Device *pll);
	void (*GetParam)(PLL_Device *pll, u32 *DivN, u32 *F0N, u32 *F0F);
} PLL_Ops;
/** @} */

/** @defgroup PLL_Device PLL Device Structure
  * @{
  */
struct PLL_DeviceTag {
	const PLL_Ops *ops;
	volatile uint32_t *CTRL0;
	volatile uint32_t *CTRL1;
	volatile uint32_t *CTRL2;
	volatile uint32_t *CTRL3;
	u32 ISO_BIT;
};
/** @} */

/* SYSTEM PLL (SYS1/SYS2/PERI) common CTRL0 bit positions */
#define PLL_CTRL0_POW_ERC		((u32)0x00000001 << 31)
#define PLL_CTRL0_POW_DIV_IN	((u32)0x00000001 << 30)
#define PLL_CTRL0_POW_PLL		((u32)0x00000001 << 29)
#define PLL_CTRL0_CK_EN			((u32)0x00000001 << 12)
#define PLL_CTRL0_CK_EN_D4		((u32)0x00000001 << 11)
#define PLL_CTRL0_DOG_SEL_MASK	((u32)0x00000007 << 8)
#define PLL_CTRL0_DOG_SEL(x)	((u32)(((x) & 0x00000007) << 8))

/* SYSTEM PLL common CTRL1/CTRL3 bit positions */
#define PLL_CTRL1_CLEAR_MASK	((u32)0x0000000F << 28 | (u32)0x0000000F << 24 | (u32)0x00000001 << 15 | \
								 (u32)0x0000003F << 5  | (u32)0x0000000F << 1)
#define PLL_CTRL1_SET(divn, div_sel)	(((divn) & 0x3F) << 5 | ((div_sel) & 0xF) << 1)
#define PLL_CTRL1_TRIG			((u32)0x00000001 << 15)
#define PLL_CTRL3_CLEAR_MASK	((u32)0x00001FFF << 19 | (u32)0x00000007 << 16)
#define PLL_CTRL3_SET(f0f, f0n)	(((f0f) & 0x1FFF) << 19 | ((f0n) & 0x7) << 16)

/* Vtable instances */
extern const PLL_Ops SYS_PLL_Ops;
extern const PLL_Ops CPU_PLL_Ops;

/* PLL device instances */
extern PLL_Device PLL_SYS1;
extern PLL_Device PLL_SYS2;
extern PLL_Device PLL_PERI;
extern PLL_Device PLL_CPU;

/* Unified PLL API */
static inline void PLL_Init(PLL_Device *pll)
{
	pll->ops->Init(pll);
}
static inline void PLL_Deinit(PLL_Device *pll)
{
	pll->ops->Deinit(pll);
}
static inline void PLL_SetFreq(PLL_Device *pll, u32 Freq_MHz)
{
	pll->ops->SetFreq(pll, Freq_MHz);
}
static inline u32  PLL_ClkGet(PLL_Device *pll)
{
	return pll->ops->ClkGet(pll);
}
static inline void PLL_GetParam(PLL_Device *pll, u32 *DivN, u32 *F0N, u32 *F0F)
{
	pll->ops->GetParam(pll, DivN, F0N, F0F);
}

/* Frequency calculation */
void PLL_CalFreqParam(u32 freq_in, u32 freq_out, u32 *N, u32 *FN, u32 *FF);
u32  PLL_CalFreqOut(u32 freq_in, u32 N, u32 FN, u32 FF);

/* Bandgap */
void PLL_Bandgap_Enable(void);
void PLL_Bandgap_Disable(void);

/* PLL wrappers */
void CPUPLL_Init(void);
void CPUPLL_Deinit(void);
void CPUPLL_SetFreq(u32 Freq_MHz);
u32  CPUPLL_ClkGet(void);

void SYS1PLL_Init(void);
void SYS1PLL_Deinit(void);
void SYS1PLL_SetFreq(u32 Freq_MHz);
u32 SYS1_PLL_ClkGet(void);

void SYS2PLL_Init(void);
void SYS2PLL_Deinit(void);
void SYS2PLL_SetFreq(u32 Freq_MHz);
u32 SYS2_PLL_ClkGet(void);

void PERIPLL_Init(void);
void PERIPLL_Deinit(void);
void PERIPLL_SetFreq(u32 Freq_MHz);
u32 PERI_PLL_ClkGet(void);

/* USB2 PLL: fixed-frequency (480MHz) UTMI PHY output, no SetFreq/vtable */
int USB2PLL_Init(void);
void USB2PLL_Deinit(void);
u32  USB2_PLL_ClkGet(void);

/* 1 when KM4VP brings the USB2 PLL up early to clock other peripherals in 4K ISP mode, so its
   480MHz output must stay powered independently of USB device teardown. Keep main.c and the
   USB HAL in sync by testing this macro rather than re-deriving the condition in each file. */
#if defined(CONFIG_VIDEO_SW) && (CONFIG_VIDEO_SW == 1) && \
	((defined(CONFIG_4K_DEV) && (CONFIG_4K_DEV == 1)) || (defined(CONFIG_4K_ENA) && (CONFIG_4K_ENA == 1)))
#define USB2_PLL_SHARED_BY_4K		1
#else
#define USB2_PLL_SHARED_BY_4K		0
#endif

#endif /* _AMEBA_PLL_H_ */
