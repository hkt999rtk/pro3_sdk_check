#ifndef _ISP_HAL_H_INC_
#define _ISP_HAL_H_INC_

void *map_register(uint32_t offset, uint32_t length);
void unmap_register(void *base, uint32_t length);
int hal_reset_isp(void);
int hal_clk_set_isp(int enable);

#ifndef barrier
#define barrier() __asm__ __volatile__("" : : : "memory")
#endif

static inline uint32_t read_reg(void *base, uint32_t offset)
{
	uint32_t value;

	value = *(volatile uint32_t *)((uint8_t *)base + offset);
	barrier();
	return value;
}

static inline void write_reg(void *base, uint32_t value, uint32_t offset)
{
	barrier();
	*(volatile uint32_t *)((uint8_t *)base + offset) = value;
}

static inline void clr_reg_bit(void *base, int bit, uint32_t offset)
{
	uint32_t v;

	v = read_reg(base, offset) & ~(1 << bit);
	write_reg(base, v, offset);
}

static inline void set_reg_bit(void *base, int bit, uint32_t offset)
{
	uint32_t v;

	v = read_reg(base, offset) | (1 << bit);
	write_reg(base, v, offset);
}

static inline void write_reg_mask(void *base, uint32_t value, uint32_t offset,
								  uint32_t mask)
{
	uint32_t v;

	v = (read_reg(base, offset) & ~mask) | (value & mask);
	write_reg(base, v, offset);
}

#endif /* _ISP_HAL_H_INC_ */
