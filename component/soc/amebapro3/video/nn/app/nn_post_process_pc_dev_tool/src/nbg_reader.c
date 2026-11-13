#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "module_vipnn.h"
#include "nbg_reader.h"

#include "NBGParser/nbinfo_util.h"
#include "NBGParser/nbg_parser.h"
#include "NBGParser/vip_lite.h"
#include "NBGParser/nbg_lk_buffer.h"

#define ENUM_NAME_SIMPLE_MAP(_ENUM) case _ENUM: return #_ENUM

void print_line_of_stars(int length)
{
	int star;
	for (star = 0; star < length; star++) {
		printf("*");
	}
	printf("\n");
}

char *get_data_format_name(vip_uint32_t data_format)
{
	switch (data_format) {
		ENUM_NAME_SIMPLE_MAP(VIP_BUFFER_FORMAT_FP32);
		ENUM_NAME_SIMPLE_MAP(VIP_BUFFER_FORMAT_FP16);
		ENUM_NAME_SIMPLE_MAP(VIP_BUFFER_FORMAT_UINT8);
		ENUM_NAME_SIMPLE_MAP(VIP_BUFFER_FORMAT_INT8);
		ENUM_NAME_SIMPLE_MAP(VIP_BUFFER_FORMAT_UINT16);
		ENUM_NAME_SIMPLE_MAP(VIP_BUFFER_FORMAT_INT16);
		ENUM_NAME_SIMPLE_MAP(VIP_BUFFER_FORMAT_CHAR);
		ENUM_NAME_SIMPLE_MAP(VIP_BUFFER_FORMAT_BFP16);
		ENUM_NAME_SIMPLE_MAP(VIP_BUFFER_FORMAT_INT32);
		ENUM_NAME_SIMPLE_MAP(VIP_BUFFER_FORMAT_UINT32);
		ENUM_NAME_SIMPLE_MAP(VIP_BUFFER_FORMAT_INT64);
		ENUM_NAME_SIMPLE_MAP(VIP_BUFFER_FORMAT_UINT64);
		ENUM_NAME_SIMPLE_MAP(VIP_BUFFER_FORMAT_FP64);
		ENUM_NAME_SIMPLE_MAP(VIP_BUFFER_FORMAT_INT4);
		ENUM_NAME_SIMPLE_MAP(VIP_BUFFER_FORMAT_UINT4);
		ENUM_NAME_SIMPLE_MAP(VIP_BUFFER_FORMAT_BOOL8);
	default:
		return "UNKNOWN_VIP_BUFFER_FORMAT";
	}
}

char *get_data_type_name(vip_uint32_t data_type)
{
	switch (data_type) {
		ENUM_NAME_SIMPLE_MAP(VIP_BUFFER_TYPE_TENSOR);
		ENUM_NAME_SIMPLE_MAP(VIP_BUFFER_TYPE_IMAGE);
		ENUM_NAME_SIMPLE_MAP(VIP_BUFFER_TYPE_ARRAY);
		ENUM_NAME_SIMPLE_MAP(VIP_BUFFER_TYPE_SCALAR);
	default:
		return "UNKNOWN_VIP_BUFFER_TYPE";
	}
}

char *get_quantize_format_name(vip_uint32_t quantization_format)
{
	switch (quantization_format) {
		ENUM_NAME_SIMPLE_MAP(VIP_BUFFER_QUANTIZE_NONE);
		ENUM_NAME_SIMPLE_MAP(VIP_BUFFER_QUANTIZE_DYNAMIC_FIXED_POINT);
		ENUM_NAME_SIMPLE_MAP(VIP_BUFFER_QUANTIZE_TF_ASYMM);
	default:
		return "UNKNOWN_FORMAT";
	}
}

vip_uint32_t get_file_size(const char *name)
{
	FILE *fp = fopen(name, "rb");
	vip_uint32_t size = 0;

	if (fp != NULL) {
		fseek(fp, 0, SEEK_END);
		size = ftell(fp);

		fclose(fp);
	} else {
		printf("Checking file %s failed.\n", name);
	}

	return size;
}

vip_uint32_t read_file(const char *name, void *dst)
{
	FILE *fp = fopen(name, "rb");
	vip_uint32_t size = 0;
	int ret = 0;

	if (fp != NULL) {
		fseek(fp, 0, SEEK_END);
		size = ftell(fp);

		fseek(fp, 0, SEEK_SET);
		ret = fread(dst, size, 1, fp);

		fclose(fp);
	}

	return size;
}

void config_input(nbg_parser_data_t *nbg_data, nn_tensor_param_t *input_param)
{
	vip_uint32_t i = 0, j = 0;
	vip_uint32_t dim_count;
	vip_uint32_t dim_size[MAX_NUM_DIMS];
	vip_uint32_t data_format;
	vip_uint32_t data_type;
	vip_uint32_t quan_format;
	vip_uint32_t fixed_pos;
	vip_float_t tf_scale;
	vip_uint32_t tf_zerop;
	vip_char_t *name = VIP_NULL;
	vip_uint32_t input_count = 0;
	vip_uint32_t name_size = 0;
	nbg_fmt_descriptor_t *nbg_desc = &nbg_data->nbg_desc;

	vip_print("\n");
	print_line_of_stars(80);
	vip_print("Input Table\n");
	print_line_of_stars(80);
	nbg_parser_query_network(nbg_data, VIP_NETWORK_PROP_INPUT_COUNT, &input_count, sizeof(vip_uint32_t));
	input_param->count = input_count;  //

	for (i = 0; i < input_count; i++) {
		nbg_parser_query_input(nbg_data, i, VIP_BUFFER_PROP_QUANT_FORMAT,
							   &quan_format, sizeof(vip_uint32_t));
		nbg_parser_query_input(nbg_data, i, VIP_BUFFER_PROP_NUM_OF_DIMENSION,
							   &dim_count, sizeof(vip_uint32_t));
		nbg_parser_query_input(nbg_data, i, VIP_BUFFER_PROP_SIZES_OF_DIMENSION, dim_size,
							   sizeof(vip_uint32_t) * dim_count);
		nbg_parser_query_input(nbg_data, i, VIP_BUFFER_PROP_DATA_FORMAT, &data_format,
							   sizeof(vip_uint32_t));
		nbg_parser_query_input(nbg_data, i, VIP_BUFFER_PROP_FIXED_POINT_POS, &fixed_pos,
							   sizeof(vip_uint32_t));
		nbg_parser_query_input(nbg_data, i, VIP_BUFFER_PROP_TF_SCALE, &tf_scale,
							   sizeof(vip_float_t));
		nbg_parser_query_input(nbg_data, i, VIP_BUFFER_PROP_TF_ZERO_POINT, &tf_zerop,
							   sizeof(vip_uint32_t));
		nbg_parser_query_input(nbg_data, i, NBG_PARSER_BUFFER_PROP_DATA_TYPE, &data_type,
							   sizeof(vip_uint32_t));
		nbg_parser_query_input(nbg_data, i, NBG_PARSER_BUFFER_PROP_NAME_SIZE, &name_size,
							   sizeof(vip_uint32_t));
		if (name_size > 0) {
			viphal_allocate_memory(name_size, (void **)&name);
			nbg_parser_query_input(nbg_data, i, VIP_BUFFER_PROP_NAME, name, name_size);
		}
		vip_print("Input %d\n", i);

		vip_print("%-40s%40u\n", "Dim Count:", dim_count);
		for (j = 0; j < dim_count; j++) {
			vip_char_t buffer[100];
			viphal_os_snprint(buffer, 100, "Size of Dim[%d]:", j);
			vip_print("%-40s%40u\n", buffer, dim_size[j]);
			input_param->dim[i].size[j] = dim_size[j];  //
		}
		vip_print("%-40s%40s\n", "Data Format:", get_data_format_name(data_format));
		vip_print("%-40s%40s\n", "Data Type:", get_data_type_name(data_type));
		vip_print("%-40s%40s\n", "Quantization Format:", get_quantize_format_name(quan_format));
		if (VIP_BUFFER_QUANTIZE_DYNAMIC_FIXED_POINT == quan_format) {
			vip_print("%-40s%40i\n", "Fixed Point Pos:", fixed_pos);
		} else if (VIP_BUFFER_QUANTIZE_TF_ASYMM == quan_format) {
			vip_print("%-40s%40f\n", "TF Scale:", tf_scale);
			vip_print("%-40s%40i\n", "TF Zeropoint:", tf_zerop);
		}
		vip_print("%-40s%40lld\n", "Memory Size (bytes):",
				  VIP_ALIGN(nbg_get_input_size(nbg_desc, i), vipdMEMORY_ALIGN_SIZE));
		if (name_size > 0) {
			vip_print("%-40s%40s\n", "input name:", name);
			viphal_free_memory(name);
			name = VIP_NULL;
		}
		print_line_of_stars(80);
	}
}

void config_output(nbg_parser_data_t *nbg_data, nn_tensor_param_t *output_param)
{
	vip_uint32_t i = 0, j = 0;
	vip_uint32_t dim_count;
	vip_uint32_t dim_size[MAX_NUM_DIMS];
	vip_uint32_t data_format;
	vip_uint32_t data_type;
	vip_uint32_t quan_format;
	vip_uint32_t fixed_pos;
	vip_float_t tf_scale;
	vip_uint32_t tf_zerop;
	vip_char_t *name = VIP_NULL;
	vip_uint32_t output_count = 0;
	vip_uint32_t name_size = 0;
	nbg_fmt_descriptor_t *nbg_desc = &nbg_data->nbg_desc;

	vip_print("\n");
	print_line_of_stars(80);
	vip_print("Output Table\n");
	print_line_of_stars(80);
	nbg_parser_query_network(nbg_data, VIP_NETWORK_PROP_OUTPUT_COUNT, &output_count, sizeof(vip_uint32_t));
	output_param->count = output_count;

	for (i = 0; i < output_count; i++) {
		nbg_parser_query_output(nbg_data, i, VIP_BUFFER_PROP_QUANT_FORMAT, &quan_format,
								sizeof(vip_uint32_t));
		nbg_parser_query_output(nbg_data, i, VIP_BUFFER_PROP_NUM_OF_DIMENSION, &dim_count,
								sizeof(vip_uint32_t));
		nbg_parser_query_output(nbg_data, i, VIP_BUFFER_PROP_SIZES_OF_DIMENSION, dim_size,
								sizeof(vip_uint32_t) * dim_count);
		nbg_parser_query_output(nbg_data, i, VIP_BUFFER_PROP_DATA_FORMAT, &data_format,
								sizeof(vip_uint32_t));
		nbg_parser_query_output(nbg_data, i, VIP_BUFFER_PROP_FIXED_POINT_POS, &fixed_pos,
								sizeof(vip_uint32_t));
		nbg_parser_query_output(nbg_data, i, VIP_BUFFER_PROP_TF_SCALE, &tf_scale,
								sizeof(vip_float_t));
		nbg_parser_query_output(nbg_data, i, VIP_BUFFER_PROP_TF_ZERO_POINT, &tf_zerop,
								sizeof(vip_uint32_t));
		nbg_parser_query_output(nbg_data, i, NBG_PARSER_BUFFER_PROP_DATA_TYPE, &data_type,
								sizeof(vip_uint32_t));
		nbg_parser_query_output(nbg_data, i, NBG_PARSER_BUFFER_PROP_NAME_SIZE, &name_size,
								sizeof(vip_uint32_t));

		if (name_size > 0) {
			viphal_allocate_memory(name_size, (void **)&name);
			nbg_parser_query_output(nbg_data, i, VIP_BUFFER_PROP_NAME, name, name_size);
		}

		vip_print("Output %d\n", i);
		vip_print("%-40s%40u\n", "Dim Count:", dim_count);
		output_param->dim[i].num = dim_count;
		for (j = 0; j < dim_count; j++) {
			vip_char_t buffer[100];
			viphal_os_snprint(buffer, 100, "Size of Dim[%d]:", j);
			vip_print("%-40s%40u\n", buffer, dim_size[j]);
			output_param->dim[i].size[j] = dim_size[j];
		}
		vip_print("%-40s%40s\n", "Data Format:", get_data_format_name(data_format));
		vip_print("%-40s%40s\n", "Data Type:", get_data_type_name(data_type));
		vip_print("%-40s%40s\n", "Quantization Format:", get_quantize_format_name(quan_format));
		if (VIP_BUFFER_QUANTIZE_DYNAMIC_FIXED_POINT == quan_format) {
			vip_print("%-40s%40i\n", "Fixed Point Pos:", fixed_pos);
		} else if (VIP_BUFFER_QUANTIZE_TF_ASYMM == quan_format) {
			vip_print("%-40s%40f\n", "TF Scale:", tf_scale);
			vip_print("%-40s%40i\n", "TF Zeropoint:", tf_zerop);
		}
		vip_print("%-40s%40lld\n", "Memory Size (bytes):",
				  VIP_ALIGN(nbg_get_output_size(nbg_desc, i), vipdMEMORY_ALIGN_SIZE));
		if (name_size > 0) {
			vip_print("%-40s%40s\n", "output name:", name);
			viphal_free_memory(name);
			name = VIP_NULL;
		}

		print_line_of_stars(80);

		/* configuration */
		output_param->format[i].buf_type = data_format;
		output_param->format[i].type = quan_format;
		output_param->format[i].scale = tf_scale;
		output_param->format[i].zero_point = tf_zerop;
		output_param->format[i].fix_point_pos = fixed_pos;
	}
}

void config_param_from_nb_file(char *nb_file_name, nn_tensor_param_t *input_param, nn_tensor_param_t *output_param)
{
	uint8_t *buffer = NULL;
	nbg_parser_data_t *nbg = VIP_NULL;
	vip_status_e status = VIP_SUCCESS;
	uint32_t vip_size = 0;
	int ret = 0;

	ret = get_binary_data(nb_file_name, &vip_size, (void **)&buffer);
	if (ret < 0) {
		printf("failed to read NBG file %s\n", nb_file_name);
		return;
	}

	status = nbg_parser_init(buffer, vip_size, "nbinfo", &nbg);
	if (status != VIP_SUCCESS) {
		printf("failed to initialize nbg parser\n");
		return;
	}

	if (buffer != NULL) {
		free(buffer);
		buffer = NULL;
	}

	// input
	config_input(nbg, input_param);

	// output
	config_output(nbg, output_param);

	if (nbg != VIP_NULL) {
		nbg_parser_destroy(nbg);
		nbg = VIP_NULL;
	}
}
