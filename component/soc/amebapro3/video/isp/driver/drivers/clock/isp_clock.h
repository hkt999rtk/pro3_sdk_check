#ifndef _CLOCK_H_
#define _CLOCK_H_

typedef void *rtscam_rtos_clk_t;

rtscam_rtos_clk_t rtscam_rtos_clk_get(const char *id);
void rtscam_rtos_clk_put(rtscam_rtos_clk_t clk);
int rtscam_rtos_clk_enable(rtscam_rtos_clk_t clk);
int rtscam_rtos_clk_disable(rtscam_rtos_clk_t clk);
int rtscam_rtos_clk_set_rate(rtscam_rtos_clk_t clk, unsigned long rate);
unsigned long rtscam_rtos_clk_get_rate(rtscam_rtos_clk_t clk);
long rtscam_rtos_clk_round_rate(rtscam_rtos_clk_t clk, unsigned long rate);
#endif /* _CLOCK_H_ */
