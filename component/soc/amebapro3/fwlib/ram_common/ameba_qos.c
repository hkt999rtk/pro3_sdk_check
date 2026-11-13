#include "ameba_soc.h"
#include <stdio.h>
#include <string.h>

#include "ameba_qos.h"

//static const char *TAG = "QOS";

void QOS_SetAWOutstanding(TRAFFIC_QOS_TypeDef *qos, int outstanding)
{
	qos->MAX_OST |= MAX_AW_OST(outstanding);
}

void QOS_SetAROutstanding(TRAFFIC_QOS_TypeDef *qos, int outstanding)
{
	qos->MAX_OST |= MAX_AR_OST(outstanding);
}

void QOS_EnableAWOutstandingRegulation(TRAFFIC_QOS_TypeDef *qos, int enable)
{
	if (enable) {
		qos->QOS_CTRL |= BIT_EN_AW_OST_CTRL;
	} else {
		qos->QOS_CTRL &= ~BIT_EN_AW_OST_CTRL;
	}
}


void QOS_EnableAROutstandingRegulation(TRAFFIC_QOS_TypeDef *qos, int enable)
{
	if (enable) {
		qos->QOS_CTRL |= BIT_EN_AR_OST_CTRL;
	} else {
		qos->QOS_CTRL &= ~BIT_EN_AR_OST_CTRL;
	}
}

void QOS_SetAWPeakRateData(TRAFFIC_QOS_TypeDef *qos, int peak_rate_data)
{
	// If don't know how much peak rate data, just put peak_rate_data to largest value - 0xffffffff
	// If peak_rate_data set to 0, it will skip peak rate mode (which is only average mode)
	qos->AW_PEAK_RATE_DATA = peak_rate_data;
}

void QOS_SetARPeakRateData(TRAFFIC_QOS_TypeDef *qos, int peak_rate_data)
{
	qos->AR_PEAK_RATE_DATA = peak_rate_data;
}

void QOS_SetAWTFrameLen(TRAFFIC_QOS_TypeDef *qos, int total_len, int p1_len)
{
	// Total Frame Length is <total_len> cycle, and peak rate mode is <p1_len> cycle.
	qos->AW_TFRAME_LEN = total_len;
	qos->AW_TFRAME_P1_LEN = p1_len;
}

void QOS_SetARTFrameLen(TRAFFIC_QOS_TypeDef *qos, int total_len, int p1_len)
{
	qos->AR_TFRAME_LEN = total_len;
	qos->AR_TFRAME_P1_LEN = p1_len;
}

void QOS_SetAWSucc(TRAFFIC_QOS_TypeDef *qos, int max_succ, int min_succ)
{
	// Master txn pass through when min_succ <= beat count <= max succ, and pass it until counter became negative value.
	qos->AW_CONT_CTRL = AW_MAX_SUCC(max_succ) | AW_MIN_SUCC(min_succ);
}

void QOS_SetARSucc(TRAFFIC_QOS_TypeDef *qos, int max_succ, int min_succ)
{
	qos->AR_CONT_CTRL = AR_MAX_SUCC(max_succ) | AR_MIN_SUCC(min_succ);
}

void QOS_SetAWBandwidthRate(TRAFFIC_QOS_TypeDef *qos, int peak_rate_n, int peak_rate_d, int avg_rate_n, int avg_rate_d)
{
	// Bandwidth limited to <peak_rate_n>/<peak_rate_d during> peak rate mode, and <avg_rate_n>/<avg_rate_d> during average mode.
	qos->AW_RATE = AW_AVG_RATE_D(avg_rate_d) | AW_AVG_RATE_N(avg_rate_n) | AW_PEAK_RATE_D(peak_rate_d) | AW_PEAK_RATE_N(peak_rate_n);
}

void QOS_SetARBandwidthRate(TRAFFIC_QOS_TypeDef *qos, int peak_rate_n, int peak_rate_d, int avg_rate_n, int avg_rate_d)
{
	// Bandwidth limited to <peak_rate_n>/<peak_rate_d during> peak rate mode, and <avg_rate_n>/<avg_rate_d> during average mode.
	qos->AR_RATE = AR_AVG_RATE_D(avg_rate_d) | AR_AVG_RATE_N(avg_rate_n) | AR_PEAK_RATE_D(peak_rate_d) | AR_PEAK_RATE_N(peak_rate_n);
}

void QOS_EnableAWBandwidthRegulation(TRAFFIC_QOS_TypeDef *qos, int enable)
{
	if (enable) {
		qos->QOS_CTRL |= (BIT_EN_AW_RATE_CTRL | BIT_EN_AW_TFRAME_CTRL);
	} else {
		qos->QOS_CTRL &= ~(BIT_EN_AW_RATE_CTRL | BIT_EN_AW_TFRAME_CTRL);
	}
}


void QOS_EnableARBandwidthRegulation(TRAFFIC_QOS_TypeDef *qos, int enable)
{
	if (enable) {
		qos->QOS_CTRL |= (BIT_EN_AR_RATE_CTRL | BIT_EN_AR_TFRAME_CTRL);
	} else {
		qos->QOS_CTRL &= ~(BIT_EN_AR_RATE_CTRL | BIT_EN_AR_TFRAME_CTRL);
	}
}
