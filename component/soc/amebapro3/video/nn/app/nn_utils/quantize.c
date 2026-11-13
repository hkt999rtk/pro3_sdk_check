//------------------------------------------------------------------------------
// QUANTIZE
//------------------------------------------------------------------------------
#include <stdint.h>
#include "quantize.h"

float u8_to_f(uint8_t val, uint8_t zero_p, float scale)
{
	return  scale * ((float)val - (float)zero_p);
}

float s8_to_f(int8_t val, int q)
{
	if (q > 0) {
		return (float)val / (float)(1 << q);
	} else {
		return (float)val * (float)(1 << -q);
	}
}

float s16_to_f(int16_t val, int q)
{
	if (q > 0) {
		return (float)val / (float)(1 << q);
	} else {
		return (float)val * (float)(1 << -q);
	}
}

float bf16_to_f(__fp16 val)
{
	return (float)val;
}

static float bfp16_to_f(uint16_t val)
{
	union {
		uint32_t u;
		float f;
	} out;

	out.u = (uint32_t)val << 16;
	return out.f;
}

static float fp64_to_f(double val)
{
	return (float)val;
}

static float dfp_to_f(float val, int q)
{
	if (q > 0) {
		return val / (float)(1 << q);
	} else {
		return val * (float)(1 << -q);
	}
}

static int8_t sign_extend_int4(uint8_t val)
{
	val &= 0x0f;
	return (val & 0x08) ? (int8_t)(val | 0xf0) : (int8_t)val;
}

static float affine_to_f(float val, int zero_p, float scale)
{
	return ((float)val - (float)zero_p) * scale;
}

static float q_integer_to_f(void *val, int buf_type)
{
	switch (buf_type) {
	case VIP_BUFFER_FORMAT_INT4:
		return (float)sign_extend_int4(*(uint8_t *)val);
	case VIP_BUFFER_FORMAT_UINT4:
		return (float)(*(uint8_t *)val & 0x0f);
	case VIP_BUFFER_FORMAT_INT8:
		return (float)(*(int8_t *)val);
	case VIP_BUFFER_FORMAT_CHAR:
		return (float)(*(char *)val);
	case VIP_BUFFER_FORMAT_UINT8:
	case VIP_BUFFER_FORMAT_BOOL8:
		return (float)(*(uint8_t *)val);
	case VIP_BUFFER_FORMAT_INT16:
		return (float)(*(int16_t *)val);
	case VIP_BUFFER_FORMAT_UINT16:
		return (float)(*(uint16_t *)val);
	case VIP_BUFFER_FORMAT_INT32:
		return (float)(*(int32_t *)val);
	case VIP_BUFFER_FORMAT_UINT32:
		return (float)(*(uint32_t *)val);
	case VIP_BUFFER_FORMAT_INT64:
		return (float)(*(int64_t *)val);
	case VIP_BUFFER_FORMAT_UINT64:
		return (float)(*(uint64_t *)val);
	default:
		return 0.0f;
	}
}

float q2f(void *val, nn_tensor_format_t *fmt)
{
	switch (fmt->type) {
	case VIP_BUFFER_QUANTIZE_DYNAMIC_FIXED_POINT:
		switch (fmt->buf_type) {
		case VIP_BUFFER_FORMAT_INT4:
		case VIP_BUFFER_FORMAT_UINT4:
		case VIP_BUFFER_FORMAT_INT8:
		case VIP_BUFFER_FORMAT_CHAR:
		case VIP_BUFFER_FORMAT_UINT8:
		case VIP_BUFFER_FORMAT_BOOL8:
		case VIP_BUFFER_FORMAT_INT16:
		case VIP_BUFFER_FORMAT_UINT16:
		case VIP_BUFFER_FORMAT_INT32:
		case VIP_BUFFER_FORMAT_UINT32:
		case VIP_BUFFER_FORMAT_INT64:
		case VIP_BUFFER_FORMAT_UINT64:
			return dfp_to_f(q_integer_to_f(val, fmt->buf_type), fmt->fix_point_pos);
		default:
			break;
		}
		break;
	case VIP_BUFFER_QUANTIZE_TF_ASYMM:
		switch (fmt->buf_type) {
		case VIP_BUFFER_FORMAT_INT4:
		case VIP_BUFFER_FORMAT_UINT4:
		case VIP_BUFFER_FORMAT_INT8:
		case VIP_BUFFER_FORMAT_CHAR:
		case VIP_BUFFER_FORMAT_UINT8:
		case VIP_BUFFER_FORMAT_BOOL8:
		case VIP_BUFFER_FORMAT_INT16:
		case VIP_BUFFER_FORMAT_UINT16:
		case VIP_BUFFER_FORMAT_INT32:
		case VIP_BUFFER_FORMAT_UINT32:
		case VIP_BUFFER_FORMAT_INT64:
		case VIP_BUFFER_FORMAT_UINT64:
			return affine_to_f(q_integer_to_f(val, fmt->buf_type), fmt->zero_point, fmt->scale);
		default:
			break;
		}
		break;
	case VIP_BUFFER_QUANTIZE_NONE:
		switch (fmt->buf_type) {
		case VIP_BUFFER_FORMAT_FP16:
			return bf16_to_f(*(__fp16 *)val);
		case VIP_BUFFER_FORMAT_BFP16:
			return bfp16_to_f(*(uint16_t *)val);
		case VIP_BUFFER_FORMAT_FP32:
			return *(float *)val;
		case VIP_BUFFER_FORMAT_FP64:
			return fp64_to_f(*(double *)val);
		case VIP_BUFFER_FORMAT_INT4:
		case VIP_BUFFER_FORMAT_UINT4:
		case VIP_BUFFER_FORMAT_INT8:
		case VIP_BUFFER_FORMAT_CHAR:
		case VIP_BUFFER_FORMAT_UINT8:
		case VIP_BUFFER_FORMAT_BOOL8:
		case VIP_BUFFER_FORMAT_INT16:
		case VIP_BUFFER_FORMAT_UINT16:
		case VIP_BUFFER_FORMAT_INT32:
		case VIP_BUFFER_FORMAT_UINT32:
		case VIP_BUFFER_FORMAT_INT64:
		case VIP_BUFFER_FORMAT_UINT64:
			return q_integer_to_f(val, fmt->buf_type);
		default:
			break;
		}
		break;
	default:
		break;
	}

	return 0.0f;
}

uint8_t f_to_u8(float val, uint8_t zero_p, float scale)
{
	return (uint8_t)(val / scale) + zero_p;
}

int8_t f_to_s8(float val, int q)
{
	if (q > 0) {
		return (int8_t)(val * (1 << q));
	} else {
		return (int8_t)(val / (1 << -q));
	}
}

int16_t f_to_s16(float val, int q)
{
	if (q > 0) {
		return (int16_t)(val * (1 << q));
	} else {
		return (int16_t)(val / (1 << -q));
	}
}

__fp16 f_to_bf16(float val)
{
	return (__fp16)val;
}
