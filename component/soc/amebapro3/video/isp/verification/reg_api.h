uint32_t read_reg_global(int addr);
void write_reg_global(int value, int addr);

uint32_t read_reg(void *base, int def);

uint32_t read_reg_dbg(void *base, int def);

void write_reg(void *base, int value, int def);
void write_reg_dbg(void *base, int value, int def);

void write_reg_mask(uint32_t base, uint32_t value, uint32_t offset, uint32_t mask);
void write_reg_mask_dbg(uint32_t base, uint32_t value, uint32_t offset, uint32_t mask);
