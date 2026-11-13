//------------------------------------------------------------------------------
// TENSOR
//------------------------------------------------------------------------------
#include <stdint.h>
#include "quantize.h"

int get_element_size(nn_tensor_format_t *fmt)
{
	int es = 1;
	switch (fmt->buf_type) {
	case VIP_BUFFER_FORMAT_FP32:
	case VIP_BUFFER_FORMAT_INT32:
	case VIP_BUFFER_FORMAT_UINT32:
		es = 4;
		break;
	case VIP_BUFFER_FORMAT_INT16:
	case VIP_BUFFER_FORMAT_UINT16:
	case VIP_BUFFER_FORMAT_FP16:
	case VIP_BUFFER_FORMAT_BFP16:
		es = 2;
		break;
	case VIP_BUFFER_FORMAT_FP64:
	case VIP_BUFFER_FORMAT_INT64:
	case VIP_BUFFER_FORMAT_UINT64:
		es = 8;
		break;
	case VIP_BUFFER_FORMAT_INT8:
	case VIP_BUFFER_FORMAT_CHAR:
	case VIP_BUFFER_FORMAT_UINT8:
	case VIP_BUFFER_FORMAT_BOOL8:
	case VIP_BUFFER_FORMAT_INT4:
	case VIP_BUFFER_FORMAT_UINT4:
		es = 1;
		break;
	default:
		break;
	}

	return es;
}

float get_tensor_value(void *data, int idx, nn_tensor_format_t *fmt)
{
	int es = get_element_size(fmt);
	return q2f((uint8_t *)data + idx * es, fmt);
}

void *get_tensor_addr(void *data, int idx, nn_tensor_format_t *fmt)
{
	int es = get_element_size(fmt);
	uint8_t *data8 = (uint8_t *)data + idx * es;
	return (void *)data8;
}
