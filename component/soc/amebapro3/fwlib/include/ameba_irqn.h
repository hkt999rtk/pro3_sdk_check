#ifndef __AMEBA_IRQN_H__
#define __AMEBA_IRQN_H__

/* This file is AUTO GENERATED */

/** @defgroup NPIRQn_enum
  * @{
  */

#if defined (CONFIG_ARM_CORE_CM4_NP)
enum NPIRQn {
	/******  Processor Exceptions Numbers ********/
	NonMaskableInt_IRQn     = -14, /*!< 2 Non Maskable Interrupt */
	HardFault_IRQn          = -13, /*!< 3 Hard Fault, all classes of Fault */
	MemoryManagement_IRQn   = -12, /*!< 4 Cortex-M3 Memory Management Interrupt */
	BusFault_IRQn           = -11, /*!< 5 Cortex-M3 Bus Fault Interrupt */
	UsageFault_IRQn         = -10, /*!< 6 Cortex-M3 Usage Fault Interrupt */
	SVCall_IRQn             =  -5, /*!< 11 Cortex-M3 SV Call Interrupt  */
	DebugMonitor_IRQn       =  -4, /*!< 12 Cortex-M3 Debug Monitor Interrupt */
	PendSV_IRQn             =  -2, /*!< 14 Cortex-M3 Pend SV Interrupt */
	SysTick_IRQn            =  -1, /*!< 15 Cortex-M3 System Tick Interrupt */

	/******  Specific Interrupt Numbers ************/
	IPC_NP_IRQ              =   0,
	WDG_NP_IRQ              =   1,
	VSFC_SW_IRQ             =   2,
	VSFC_PROF_IRQ           =   3,
	UART0_IRQ               =   4,
	UART1_IRQ               =   5,
	UART2_IRQ               =   6,
	BTUART_IRQ              =   7,
	LOG_UART_IRQ            =   8,
	I2C0_IRQ                =   9,
	I2C1_IRQ                =  10,
	I2C2_IRQ                =  11,
	I2C3_IRQ                =  12,
	SPIM0_IRQ               =  13,
	SPIS0_IRQ               =  14,
	SPIM1_IRQ               =  15,
	SPIS1_IRQ               =  16,
	PWM_IRQ                 =  17,
	GTIMER0_IRQ             =  18,
	GTIMER1_IRQ             =  19,
	GTIMER2_IRQ             =  20,
	GTIMER3_IRQ             =  21,
	GDMA1_CH0_IRQ           =  22,
	GDMA1_CH1_IRQ           =  23,
	GDMA1_CH2_IRQ           =  24,
	GDMA1_CH3_IRQ           =  25,
	GDMA1_CH4_IRQ           =  26,
	GDMA1_CH5_IRQ           =  27,
	GDMA1_CH6_IRQ           =  28,
	GDMA1_CH7_IRQ           =  29,
	GDMA0_CH0_IRQ           =  30,
	GDMA0_CH1_IRQ           =  31,
	GDMA0_CH2_IRQ           =  32,
	GDMA0_CH3_IRQ           =  33,
	ADC_IRQ                 =  34,
	PON_GPIO_IRQ            =  35,
	GPIO_IRQ                =  36,
	SGPIO_IRQ               =  37,
	SPORT0_IRQ              =  38,
	SPORT1_IRQ              =  39,
	ENCODER_IRQ             =  40,
	ENCODER_REMAP_IRQ       =  41,
	ISP_ALL_IRQ             =  42,
	ISP_REMAP_IRQ           =  43,
	NN_IRQ                  =  44,
	GE_IRQ                  =  45,
	TILE_SCALER_IRQ         =  46,
	AID_IRQ                 =  47,
	WL_DMA_IRQ              =  48,
	WL_PROTOCOL_IRQ         =  49,
	WIFI_FISR_FESR_IRQ      =  50,
	WIFI_FTSR_MAILBOX_IRQ   =  51,
	BT_SCB_IRQ              =  52,
	BT_WAKE_HOST_IRQ        =  53,
	RTC_IRQ                 =  54,
	AON_TIMER_IRQ           =  55,
	AON_COMP_IRQ            =  56,
	AON_GPIO_IRQ            =  57,
	TRNG_IRQ                =  58,
	PKE_IRQ                 =  59,
	AES_IRQ                 =  60,
	AES_DMA_CH0_IRQ         =  61,
	HMAC_DMA_CH1_IRQ        =  62,
	FLASH_SEC_IRQ           =  63,
	GMAC_IRQ                =  64,
	USBOTG_IRQ              =  65,
	SPIC_IRQ                =  66,
	PSRAMC_IRQ              =  67,
	SDMMC_IRQ               =  68,
	DDRC_FE_IRQ             =  69,
	DDRC_DRAM_IRQ           =  70,
	RXI300_CORE_IRQ         =  71,
	RXI300_WIFI_IRQ         =  72,
	BOD_IRQ                 =  73,
	ISP0_IRQ                =  74,
	ISP1_IRQ                =  75,
	ISP2_IRQ                =  76,
	ISP3_IRQ                =  77,
	ISP4_IRQ                =  78,
	ISP5_IRQ                =  79,
	ISP6_IRQ                =  80,
	ISP7_IRQ                =  81,
	ISP8_IRQ                =  82,
	OCP_IRQ                 =  83,
	PMC_IRQ                 =  84,
};

#define AON_TIM_IRQ  AON_TIMER_IRQ
#define AON_WAKEPIN_IRQ  AON_GPIO_IRQ
#endif

/**
  * @}
  */

/** @defgroup VPIRQn_enum
  * @{
  */

#if defined (CONFIG_ARM_CORE_CM4_VP)
enum VPIRQn {
	/******  Processor Exceptions Numbers ********/
	NonMaskableInt_IRQn     = -14, /*!< 2 Non Maskable Interrupt */
	HardFault_IRQn          = -13, /*!< 3 Hard Fault, all classes of Fault */
	MemoryManagement_IRQn   = -12, /*!< 4 Cortex-M3 Memory Management Interrupt */
	BusFault_IRQn           = -11, /*!< 5 Cortex-M3 Bus Fault Interrupt */
	UsageFault_IRQn         = -10, /*!< 6 Cortex-M3 Usage Fault Interrupt */
	SVCall_IRQn             =  -5, /*!< 11 Cortex-M3 SV Call Interrupt  */
	DebugMonitor_IRQn       =  -4, /*!< 12 Cortex-M3 Debug Monitor Interrupt */
	PendSV_IRQn             =  -2, /*!< 14 Cortex-M3 Pend SV Interrupt */
	SysTick_IRQn            =  -1, /*!< 15 Cortex-M3 System Tick Interrupt */

	/******  Specific Interrupt Numbers ************/
	IPC_MP_IRQ              =   0,
	WDG_MP_IRQ              =   1,
	UART0_IRQ               =   2,
	UART1_IRQ               =   3,
	UART2_IRQ               =   4,
	BTUART_IRQ              =   5,
	LOG_UART_IRQ            =   6,
	I2C0_IRQ                =   7,
	I2C1_IRQ                =   8,
	I2C2_IRQ                =   9,
	I2C3_IRQ                =  10,
	SPIM0_IRQ               =  11,
	SPIS0_IRQ               =  12,
	SPIM1_IRQ               =  13,
	SPIS1_IRQ               =  14,
	PWM_IRQ                 =  15,
	GTIMER0_IRQ             =  16,
	GTIMER1_IRQ             =  17,
	GTIMER2_IRQ             =  18,
	GTIMER3_IRQ             =  19,
	GDMA1_CH0_IRQ           =  20,
	GDMA1_CH1_IRQ           =  21,
	GDMA1_CH2_IRQ           =  22,
	GDMA1_CH3_IRQ           =  23,
	GDMA1_CH4_IRQ           =  24,
	GDMA1_CH5_IRQ           =  25,
	GDMA1_CH6_IRQ           =  26,
	GDMA1_CH7_IRQ           =  27,
	ADC_IRQ                 =  28,
	PON_GPIO_IRQ            =  29,
	GPIO_IRQ                =  30,
	SGPIO_IRQ               =  31,
	SPORT0_IRQ              =  32,
	SPORT1_IRQ              =  33,
	ENCODER_IRQ             =  34,
	ENCODER_REMAP_IRQ       =  35,
	ISP_ALL_IRQ             =  36,
	ISP_REMAP_IRQ           =  37,
	NN_IRQ                  =  38,
	GE_IRQ                  =  39,
	TILE_SCALER_IRQ         =  40,
	AID_IRQ                 =  41,
	RTC_IRQ                 =  42,
	AON_TIMER_IRQ           =  43,
	AON_COMP_IRQ            =  44,
	AON_GPIO_IRQ            =  45,
	TRNG_IRQ                =  46,
	PKE_IRQ                 =  47,
	AES_IRQ                 =  48,
	AES_DMA_CH0_IRQ         =  49,
	HMAC_DMA_CH1_IRQ        =  50,
	FLASH_SEC_IRQ           =  51,
	GMAC_IRQ                =  52,
	USBOTG_IRQ              =  53,
	SPIC_IRQ                =  54,
	PSRAMC_IRQ              =  55,
	SDMMC_IRQ               =  56,
	DDRC_FE_IRQ             =  57,
	DDRC_DRAM_IRQ           =  58,
	RXI300_CORE_IRQ         =  59,
	RXI300_WIFI_IRQ         =  60,
	BOD_IRQ                 =  61,
	ISP0_IRQ                =  62,
	ISP1_IRQ                =  63,
	ISP2_IRQ                =  64,
	ISP3_IRQ                =  65,
	ISP4_IRQ                =  66,
	ISP5_IRQ                =  67,
	ISP6_IRQ                =  68,
	ISP7_IRQ                =  69,
	ISP8_IRQ                =  70,
	OCP_IRQ                 =  71,
	PMC_IRQ                 =  72,
};

#endif

/**
  * @}
  */

/** @defgroup FPIRQn_enum
  * @{
  */

#if defined (CONFIG_ARM_CORE_CM0)
enum FPIRQn {
	/******  Processor Exceptions Numbers ********/
	NonMaskableInt_IRQn     = -14, /*!< 2 Non Maskable Interrupt */
	HardFault_IRQn          = -13, /*!< 3 Hard Fault, all classes of Fault */
	MemoryManagement_IRQn   = -12, /*!< 4 Cortex-M3 Memory Management Interrupt */
	BusFault_IRQn           = -11, /*!< 5 Cortex-M3 Bus Fault Interrupt */
	UsageFault_IRQn         = -10, /*!< 6 Cortex-M3 Usage Fault Interrupt */
	SVCall_IRQn             =  -5, /*!< 11 Cortex-M3 SV Call Interrupt  */
	DebugMonitor_IRQn       =  -4, /*!< 12 Cortex-M3 Debug Monitor Interrupt */
	PendSV_IRQn             =  -2, /*!< 14 Cortex-M3 Pend SV Interrupt */
	SysTick_IRQn            =  -1, /*!< 15 Cortex-M3 System Tick Interrupt */

	/******  Specific Interrupt Numbers ************/
	IPC_FW_IRQ              =   0,
	WDG_FW_IRQ              =   1,
	UART0_IRQ               =   2,
	UART1_IRQ               =   3,
	UART2_IRQ               =   4,
	BTUART_IRQ              =   5,
	LOG_UART_IRQ            =   6,
	I2C0_IRQ                =   7,
	I2C1_IRQ                =   8,
	I2C2_IRQ                =   9,
	I2C3_IRQ                =  10,
	SPIM0_IRQ               =  11,
	SPIS0_IRQ               =  12,
	SPIM1_IRQ               =  13,
	SPIS1_IRQ               =  14,
	PWM_IRQ                 =  15,
	GTIMER0_IRQ             =  16,
	GTIMER1_IRQ             =  17,
	GTIMER2_IRQ             =  18,
	GTIMER3_IRQ             =  19,
	GDMA0_CH0_IRQ           =  20,
	GDMA0_CH1_IRQ           =  21,
	GDMA0_CH2_IRQ           =  22,
	GDMA0_CH3_IRQ           =  23,
	ADC_IRQ                 =  24,
	PON_GPIO_IRQ            =  25,
	GPIO_IRQ                =  26,
	SGPIO_IRQ               =  27,
	WL_DMA_IRQ              =  28,
	WL_PROTOCOL_IRQ         =  29,
	WIFI_FISR_FESR_IRQ      =  30,
	WIFI_FTSR_MAILBOX_IRQ   =  31,
	BT_SCB_IRQ              =  32,
	BT_WAKE_HOST_IRQ        =  33,
	RTC_IRQ                 =  34,
	AON_TIMER_IRQ           =  35,
	AON_COMP_IRQ            =  36,
	AON_GPIO_IRQ            =  37,
	TRNG_IRQ                =  38,
	PKE_IRQ                 =  39,
	AES_IRQ                 =  40,
	AES_DMA_CH0_IRQ         =  41,
	HMAC_DMA_CH1_IRQ        =  42,
	FLASH_SEC_IRQ           =  43,
	SPIC_IRQ                =  44,
	PSRAMC_IRQ              =  45,
	RXI300_CORE_IRQ         =  46,
	RXI300_WIFI_IRQ         =  47,
	BOD_IRQ                 =  48,
	OCP_IRQ                 =  49,
	PMC_IRQ                 =  50,
};

#endif

/**
  * @}
  */

/** @defgroup APIRQn_enum
  * @{
  */

#if defined (CONFIG_ARM_CORE_CA32)
enum APIRQn {
	/******  Processor Exceptions Numbers ********/
	SGI0_Edge               =   0, /* software generated interrupt 0 (recommended non-secure) */
	SGI1_Edge               =   1, /* software generated interrupt 2 (recommended non-secure) */
	SGI2_Edge               =   2, /* software generated interrupt 3 (recommended non-secure) */
	SGI3_Edge               =   3, /* software generated interrupt 4 (recommended non-secure) */
	SGI4_Edge               =   4, /* software generated interrupt 5 (recommended non-secure) */
	SGI5_Edge               =   5, /* software generated interrupt 6 (recommended non-secure) */
	SGI6_Edge               =   6, /* software generated interrupt 7 (recommended non-secure) */
	SGI7_Edge               =   7, /* software generated interrupt 8 (recommended non-secure) */
	SGI8_Edge               =   8, /* software generated interrupt 9 (recommended secure) */
	SGI9_Edge               =   9, /* software generated interrupt 4 (recommended secure) */
	SGI10_Edge              =  10, /* software generated interrupt 5 (recommended secure) */
	SGI11_Edge              =  11, /* software generated interrupt 6 (recommended secure) */
	SGI12_Edge              =  12, /* software generated interrupt 7 (recommended secure) */
	SGI13_Edge              =  13, /* software generated interrupt 8 (recommended secure) */
	SGI14_Edge              =  14, /* software generated interrupt 9 (recommended secure) */
	SGI15_Edge              =  15, /* software generated interrupt 9 (recommended secure) */

	PPI6_IRQ                =  25, /* Virtual maintenance interrupt. */
	PPI5_IRQ                =  26, /* Hypervisor timer event. */
	PPI4_IRQ                =  27, /* Virtual timer event. */
	PPI0_IRQ                =  28, /* Legacy nFIQ signal. Not used. */
	PPI1_IRQ                =  29, /* Secure physical timer event. */
	PPI2_IRQ                =  30, /* Non-secure physical timer event. */
	PPI3_IRQ                =  31, /* Legacy nIRQ signal. Not used. */

	/******  Specific Interrupt Numbers ************/
	IPC_AP_IRQ              =  32,
	WDG_AP_IRQ              =  33,
	VSFC_SW_IRQ             =  34,
	VSFC_PROF_IRQ           =  35,
	UART0_IRQ               =  36,
	UART1_IRQ               =  37,
	UART2_IRQ               =  38,
	BTUART_IRQ              =  39,
	LOG_UART_IRQ            =  40,
	I2C0_IRQ                =  41,
	I2C1_IRQ                =  42,
	I2C2_IRQ                =  43,
	I2C3_IRQ                =  44,
	SPIM0_IRQ               =  45,
	SPIS0_IRQ               =  46,
	SPIM1_IRQ               =  47,
	SPIS1_IRQ               =  48,
	PWM_IRQ                 =  49,
	GTIMER0_IRQ             =  50,
	GTIMER1_IRQ             =  51,
	GTIMER2_IRQ             =  52,
	GTIMER3_IRQ             =  53,
	GDMA1_CH0_IRQ           =  54,
	GDMA1_CH1_IRQ           =  55,
	GDMA1_CH2_IRQ           =  56,
	GDMA1_CH3_IRQ           =  57,
	GDMA1_CH4_IRQ           =  58,
	GDMA1_CH5_IRQ           =  59,
	GDMA1_CH6_IRQ           =  60,
	GDMA1_CH7_IRQ           =  61,
	GDMA0_CH0_IRQ           =  62,
	GDMA0_CH1_IRQ           =  63,
	GDMA0_CH2_IRQ           =  64,
	GDMA0_CH3_IRQ           =  65,
	ADC_IRQ                 =  66,
	PON_GPIO_IRQ            =  67,
	GPIO_IRQ                =  68,
	SGPIO_IRQ               =  69,
	SPORT0_IRQ              =  70,
	SPORT1_IRQ              =  71,
	ENCODER_IRQ             =  72,
	ENCODER_REMAP_IRQ       =  73,
	ISP_ALL_IRQ             =  74,
	ISP_REMAP_IRQ           =  75,
	NN_IRQ                  =  76,
	GE_IRQ                  =  77,
	TILE_SCALER_IRQ         =  78,
	AID_IRQ                 =  79,
	WL_DMA_IRQ              =  80,
	WL_PROTOCOL_IRQ         =  81,
	WIFI_FISR_FESR_IRQ      =  82,
	WIFI_FTSR_MAILBOX_IRQ   =  83,
	BT_SCB_IRQ              =  84,
	BT_WAKE_HOST_IRQ        =  85,
	RTC_IRQ                 =  86,
	AON_TIMER_IRQ           =  87,
	AON_COMP_IRQ            =  88,
	AON_GPIO_IRQ            =  89,
	TRNG_IRQ                =  90,
	PKE_IRQ                 =  91,
	AES_IRQ                 =  92,
	AES_DMA_CH0_IRQ         =  93,
	HMAC_DMA_CH1_IRQ        =  94,
	FLASH_SEC_IRQ           =  95,
	GMAC_IRQ                =  96,
	USBOTG_IRQ              =  97,
	SPIC_IRQ                =  98,
	PSRAMC_IRQ              =  99,
	SDMMC_IRQ               = 100,
	DDRC_FE_IRQ             = 101,
	DDRC_DRAM_IRQ           = 102,
	RXI300_CORE_IRQ         = 103,
	RXI300_WIFI_IRQ         = 104,
	BOD_IRQ                 = 105,
	ISP0_IRQ                = 106,
	ISP1_IRQ                = 107,
	ISP2_IRQ                = 108,
	ISP3_IRQ                = 109,
	ISP4_IRQ                = 110,
	ISP5_IRQ                = 111,
	ISP6_IRQ                = 112,
	ISP7_IRQ                = 113,
	ISP8_IRQ                = 114,
	OCP_IRQ                 = 115,
	PMC_IRQ                 = 116,
};

#endif

/**
  * @}
  */

#endif //__AMEBA_IRQN_H__
