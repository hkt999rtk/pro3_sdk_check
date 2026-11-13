#ifndef _ISP_REG_H_INC_
#define _ISP_REG_H_INC_

#include <stdint.h>
#include <rts_isp_errno.h>

#ifndef barrier
#define barrier() __asm__ __volatile__("" : : : "memory")
#endif

extern void *isp_io_base;

static inline uint32_t isp_read_reg(uint32_t offset)
{
	uint32_t value;

	value = *(volatile uint32_t *)(isp_io_base + offset);
	barrier();
	return value;
}

static inline void isp_write_reg(uint32_t value, uint32_t offset)
{
	barrier();
	*(volatile uint32_t *)(isp_io_base + offset) = value;
}

static inline void isp_clr_reg_bit(int bit, uint32_t offset)
{
	uint32_t v;

	v = isp_read_reg(offset) & ~(1 << bit);
	isp_write_reg(v, offset);
}

static inline void isp_set_reg_bit(int bit, uint32_t offset)
{
	uint32_t v;

	v = isp_read_reg(offset) | (1 << bit);
	isp_write_reg(v, offset);
}

static inline void isp_write_reg_mask(uint32_t value, uint32_t offset,
									  uint32_t mask)
{
	uint32_t v;

	v = (isp_read_reg(offset) & ~mask) | (value & mask);
	isp_write_reg(v, offset);
}

uint32_t isp_read_vreg(uint32_t offset);
float isp_read_vreg_float(uint32_t offset);
void isp_write_vreg(uint32_t value, uint32_t offset);
void isp_write_vreg_float(float value, uint32_t offset);
void isp_write_vreg_mask(uint32_t value, uint32_t offset, uint32_t mask);

int isp_regcfg_init(void);
void isp_regcfg_cleanup(void);
void isp_regcfg_write(uint32_t value, uint32_t offset);
uint32_t *isp_regcfg_batch_req(uint32_t offset, uint32_t num);
void isp_regcfg_batch_write(uint32_t value);
int isp_regcfg_start(void);
int isp_regcfg_complete(void);
int isp_regcfg_available(void);

static inline void isp_regcfg_write_data(uint32_t value, uint32_t *data)
{
	*data = value;
}

/* estimation */
struct isp_hw_mod_info {
	uint32_t isp_id;
	int32_t chan_offset;
};

typedef struct isp_hw_mod_info *FW_EST_MOD_INFO;

#define _CONCAT_REGISTER(MOD, FIELD) MOD##_##FIELD
#define CONCAT_REGISTER(MOD, FIELD) _CONCAT_REGISTER(MOD, FIELD)

#define _CONCAT_REGISTER_0(MOD, FIELD) MOD##_##FIELD##_0
#define CONCAT_REGISTER_0(MOD, FIELD) _CONCAT_REGISTER_0(MOD, FIELD)

#define READ_REG(mod_info, FIELD) \
	isp_read_reg(CONCAT_REGISTER(MOD_NAME, FIELD) + mod_info->chan_offset)

#define READ_REG_ARRAY_I(mod_info, FIELD, index) \
	isp_read_reg(CONCAT_REGISTER_0(MOD_NAME, FIELD) + (index) * 4)

#define WRITE_REG(mod_info, FIELD, value)                       \
	isp_write_reg(value, CONCAT_REGISTER(MOD_NAME, FIELD) + \
				     mod_info->chan_offset)

#define WRITE_REG_ARRAY_I(mod_info, FIELD, index, value) \
	isp_write_reg(value, CONCAT_REGISTER_0(MOD_NAME, FIELD) + (index) * 4)

#define READ_VIRT_REG(mod_info, REGNAME) isp_read_vreg(REGNAME)

#define WRITE_VIRT_REG(mod_info, REGNAME, value) isp_write_vreg(value, REGNAME)

#endif /* _ISP_REG_H_INC_ */
