#ifndef _RTSCAM_ISP_POWER_H_INC_
#define _RTSCAM_ISP_POWER_H_INC_

struct rtscam_isp_snr_power {
	int32_t type; /* enum rtscam_isp_snr_pwr_type */
	const char *name;
	void *handle; /* can be power, gpio, clk */
	int current_value;
};

struct rtscam_isp_power {
	struct rtscam_isp_snr_power powers[6];

	bool has_pmu;
	void *pwrctl_gpio_handle;
};

int rtscam_isp_power_init(struct rtscam_isp_power *power, bool has_pmu);
void rtscam_isp_power_cleanup(struct rtscam_isp_power *power);
int rtscam_isp_power_set(struct rtscam_isp_power *power,
						 struct rtscam_isp_snr_pwr *powers);

#endif /* _RTSCAM_ISP_POWER_H_INC_ */
