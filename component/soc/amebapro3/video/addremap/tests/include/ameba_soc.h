#ifndef TEST_AMEBA_SOC_H
#define TEST_AMEBA_SOC_H
#include <stdint.h>
typedef uint32_t u32;
typedef uint16_t u16;
typedef uint8_t u8;
#define ISP_REG_BASE 0x60000000U
#define REMAP_ISP_REG_BASE 0x60100000U
#define REMAP_ENC_REG_BASE 0x6010A000U
#define VIDEO_MEM_BASE 0x6F000000U
#define BIT(n) (1U << (n))
u32 test_read32(u32 base, u32 offset);
void test_write32(u32 base, u32 offset, u32 value);
#define HAL_READ32 test_read32
#define HAL_WRITE32 test_write32
#define DiagPrintf(...) ((void)0)
#endif
