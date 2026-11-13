/******************************************************************************\
|* Copyright (c) 2017-2024 by Vivante Corporation.  All Rights Reserved.      *|
|*                                                                            *|
|* The material in this file is confidential and contains trade secrets of    *|
|* of Vivante Corporation.  This is proprietary information owned by Vivante  *|
|* Corporation.  No part of this work may be disclosed, reproduced, copied,   *|
|* transmitted, or used in any way for any purpose, without the express       *|
|* written permission of Vivante Corporation.                                 *|
|*                                                                            *|
\******************************************************************************/

#include <nbg_fmt_util.h>
#include <vip_lite.h>

#if (vpmdENABLE_DEBUG_LOG > 2)
static char* create_network_from_str(
    vip_uint32_t from
    )
{
    switch (from) {
    case NBG_CREATE_NETWORK_FROM_FLASH: return "flash";
    case NBG_CREATE_NETWORK_FROM_FILE: return "file";
    case NBG_CREATE_NETWORK_FROM_MEMORY: return "memory";
    default:
        return "UNKNOWN";
    }
}
#endif

/******************** For binary file loading *******************/
void nbg_fmt_reader_open(
    nbg_fmt_reader_t *reader,
    void *data,
    vip_size_t size,
    void *file,
    vip_uint32_t creat_type
    )
{
    reader->current_data = reader->data = (vip_char_t *)data;
    reader->total_size = size;
    reader->offset = 0;
    reader->file = (viphal_file_t)file;
    if (reader->data != VIP_NULL) {
        if (creat_type & VIP_CREATE_NETWORK_FROM_FLASH) {
            reader->create_from = NBG_CREATE_NETWORK_FROM_FLASH;
        }
        else {
            reader->create_from = NBG_CREATE_NETWORK_FROM_MEMORY;
        }
    }
    else {
        reader->create_from = NBG_CREATE_NETWORK_FROM_FILE;
    }
    vip_info("network create from=%s, nbg file size=%d\n",
            create_network_from_str(reader->create_from), size);
}

void nbg_fmt_reader_close(
    nbg_fmt_reader_t *reader
    )
{
    reader->current_data = reader->data = VIP_NULL;
    reader->total_size = reader->offset = 0;
    reader->file = VIP_NULL;
}

/*
 * @brief
 * Rewind the current position to the beginning.
 */
void nbg_fmt_reader_rewind(
    nbg_fmt_reader_t *reader
    )
{
    if (reader->create_from == NBG_CREATE_NETWORK_FROM_FILE) {
        viphal_os_seek_file(0, VIPHAL_FILE_SET, reader->file);
    }
    else {
        reader->current_data = reader->data;
    }
    reader->offset = 0;
}

/*
 * @brief
 * Move the read pointer forward by Step.
 */
vip_status_e nbg_fmt_reader_forward(
    nbg_fmt_reader_t *reader,
    vip_size_t step
    )
{
    vip_status_e error = VIP_SUCCESS;

    if (reader->create_from == NBG_CREATE_NETWORK_FROM_FILE) {
        error = viphal_os_seek_file(step, VIPHAL_FILE_CUR, reader->file);
        reader->offset += step;
    }
    else {
        if ((reader->offset + step) >= reader->total_size) {
            vip_error("fail offset=%d + step=%d < reader total size=%d\n",
                         reader->offset, step, reader->total_size);
            error = VIP_ERROR_INVALID_ARGUMENTS;
        }
        else {
            reader->offset += step;
            reader->current_data = reader->data + reader->offset;
        }
    }

    return error;
}

/*
 * @brief
 * Move the read pointer backward by Step.
 */
vip_status_e nbg_fmt_reader_backward(
    nbg_fmt_reader_t *reader,
    vip_size_t step
    )
{
    vip_status_e error = VIP_SUCCESS;

    if (reader->create_from == NBG_CREATE_NETWORK_FROM_FILE) {
        error = viphal_os_seek_file(step, VIPHAL_FILE_CUR | VIPHAL_FILE_BACKWARD, reader->file);
        reader->offset -= step;
    }
    else {
        if (reader->offset < step) {
            vip_error("fail reader->offset=%d > Step=%d\n", reader->offset, step);
            error = VIP_ERROR_INVALID_ARGUMENTS;
        }
        else {
            reader->offset -= step;
            reader->current_data = reader->data + reader->offset;
        }
    }

    return error;
}

/*
 * @brief
 * Move the read pointer to a new location.
 */
vip_status_e nbg_fmt_reader_locate(
    nbg_fmt_reader_t *reader,
    vip_size_t location
    )
{
    vip_status_e error = VIP_SUCCESS;

    if (reader->create_from == NBG_CREATE_NETWORK_FROM_FILE) {
        error = viphal_os_seek_file(location, VIPHAL_FILE_SET, reader->file);
        reader->offset = location;
    }
    else if ((reader->create_from == NBG_CREATE_NETWORK_FROM_MEMORY) ||
             (reader->create_from == NBG_CREATE_NETWORK_FROM_FLASH)) {
        if (location >= reader->total_size) {
            vip_error("fail Location=%d < reader total size=%d\n", location, reader->total_size);
            error = VIP_ERROR_INVALID_ARGUMENTS;
        }
        else {
            reader->offset = location;
            reader->current_data = reader->data + reader->offset;
        }
    }
    else {
        vip_error("not support this create network type=%d\n", reader->create_from);
        error = VIP_ERROR_INVALID_ARGUMENTS;
    }

    return error;
}

/*********** Read different types of data. ***********/
/*
 * @brief
 * Read a byte.
 */
vip_int8_t nbg_fmt_read_byte(
    nbg_fmt_reader_t *reader,
    vip_uint32_t auto_move
    )
{
    vip_status_e status = VIP_SUCCESS;
    vip_int8_t data = 0;
    if (reader->create_from == NBG_CREATE_NETWORK_FROM_FILE) {
        status = viphal_os_read_file(&data, sizeof(data), reader->file);
        if (status != VIP_SUCCESS) {
            data = 0;
            vip_error("fail to read byte\n");
        }
        if (!auto_move) {
            nbg_fmt_reader_backward(reader, sizeof(data));
        }
        else {
            reader->offset += sizeof(vip_int8_t);
        }
    }
    else if ((reader->create_from == NBG_CREATE_NETWORK_FROM_MEMORY) ||
             (reader->create_from == NBG_CREATE_NETWORK_FROM_FLASH)) {
        data = *((vip_int8_t *)reader->current_data);
        if (auto_move != 0) {
            reader->offset += sizeof(vip_int8_t);
            reader->current_data = reader->data + reader->offset;
        }
    }
    else {
        vip_error("not support this create network type=%d\n", reader->create_from);
        data = -1;
    }

    return data;
}

vip_uint32_t nbg_fmt_read_uInt(
    nbg_fmt_reader_t *reader,
    vip_uint32_t auto_move
    )
{
    vip_status_e status = VIP_SUCCESS;
    vip_uint32_t data = 0;

    if (reader->create_from == NBG_CREATE_NETWORK_FROM_FILE) {
        status = viphal_os_read_file(&data, sizeof(data), reader->file);
        if (status != VIP_SUCCESS) {
            data = 0;
            vip_error("fail to read uInt\n");
        }
        if (!auto_move) {
            nbg_fmt_reader_backward(reader, sizeof(data));
        }
        else {
            reader->offset += sizeof(vip_uint32_t);
        }
    }
    else if ((reader->create_from == NBG_CREATE_NETWORK_FROM_MEMORY) ||
             (reader->create_from == NBG_CREATE_NETWORK_FROM_FLASH)) {
        data = *((vip_uint32_t *)reader->current_data);
        if (auto_move != 0) {
            reader->offset += sizeof(vip_uint32_t);
            reader->current_data = reader->data + reader->offset;
        }
    }
    else {
        vip_error("not support this create network type=%d\n", reader->create_from);
        data = 0;
    }

    return data;
}

vip_status_e nbg_fmt_read_data(
    nbg_fmt_reader_t *reader,
    vip_uint8_t **memory,
    vip_size_t size,
    const vip_char_t *name
    )
{
    vip_status_e status = VIP_SUCCESS;

    if (reader->create_from == NBG_CREATE_NETWORK_FROM_FILE) {
        status = viphal_os_read_file(*memory, size, reader->file);
        if (status != VIP_SUCCESS) {
            vip_error("fail to read data name=%s\n", name);
        }
        reader->offset += size;
        nbg_fmt_reader_backward(reader, size);
    }
    else if (reader->create_from == NBG_CREATE_NETWORK_FROM_FLASH) {
        if (reader->offset + size <= reader->total_size) {
            *memory = (vip_uint8_t*)reader->current_data;
        }
        else {
            vip_error("fail offset+Size large than NBG file total size=%d, name=%s\n",
                        reader->total_size, name);
            return VIP_ERROR_OUT_OF_RESOURCE;
        }
    }
    else if (reader->create_from == NBG_CREATE_NETWORK_FROM_MEMORY) {
        viphal_copy_memory(*memory, reader->current_data, size);
    }
    else {
        vip_error("not support this create network type=%d name=%s\n",
                    reader->create_from, name);
        status = VIP_ERROR_OUT_OF_RESOURCE;
    }

    return status;
}

vip_status_e nbg_fmt_read_data_copy(
    nbg_fmt_reader_t *reader,
    void *memory,
    vip_size_t size,
    const vip_char_t *name
    )
{
    vip_status_e status = VIP_SUCCESS;

    if (reader->create_from == NBG_CREATE_NETWORK_FROM_FILE) {
        status = viphal_os_read_file(memory, size, reader->file);
        if (status != VIP_SUCCESS) {
            vip_error("fail to read data copy name=%s\n", name);
        }
        reader->offset += size;
        nbg_fmt_reader_backward(reader, size);
    }
    else if ((reader->create_from == NBG_CREATE_NETWORK_FROM_MEMORY) ||
             (reader->create_from == NBG_CREATE_NETWORK_FROM_FLASH)) {
        if (reader->offset + size <= reader->total_size) {
            viphal_copy_memory(memory, reader->current_data, size);
        }
        else {
            vip_error("fail data copy offset+Size large than NBG file total size=%d name=%s\n",
                        reader->total_size, name);
            status = VIP_ERROR_OUT_OF_RESOURCE;
        }
    }
    else {
        vip_error("not support this create network type=%d name=%s\n",
                     reader->create_from, name);
        status = VIP_ERROR_OUT_OF_RESOURCE;
    }

    return status;
}

/*
@brief Get the byte of buffer format.
@param format
*/
vip_uint32_t nbg_get_buf_format_byte(
    vip_enum format
    )
{
    vip_uint32_t size = 1;
    switch (format) {
    case VIP_BUFFER_FORMAT_FP32:
        size = sizeof(vip_float_t);
        break;
    case VIP_BUFFER_FORMAT_UINT32:
    case VIP_BUFFER_FORMAT_INT32:
        size = sizeof(vip_int32_t);
        break;
    case VIP_BUFFER_FORMAT_FP16:
    case VIP_BUFFER_FORMAT_BFP16:
    case VIP_BUFFER_FORMAT_UINT16:
    case VIP_BUFFER_FORMAT_INT16:
        size = sizeof(vip_int16_t);
        break;

    case VIP_BUFFER_FORMAT_UINT8:
    case VIP_BUFFER_FORMAT_INT8:
    case VIP_BUFFER_FORMAT_BOOL8:
        size = sizeof(vip_int8_t);
        break;

    case VIP_BUFFER_FORMAT_FP64:
    case VIP_BUFFER_FORMAT_UINT64:
    case VIP_BUFFER_FORMAT_INT64:
        size = sizeof(vip_int64_t);
        break;

    case VIP_BUFFER_FORMAT_INT4:
    case VIP_BUFFER_FORMAT_UINT4:
        size = 0;
        break;

    default:
        vip_error("viplite doesn't support this buffer format=%d\n", format);
        break;
    }

    return size;
}

/*
@brief Get the bit of buffer format.
@param format
*/
vip_uint32_t nbg_get_buf_format_bit(
    vip_enum format
    )
{
    vip_uint32_t size = 1;
    switch (format) {
    case VIP_BUFFER_FORMAT_FP32:
        size = sizeof(vip_float_t) * BITS_PER_BYTE;
        break;
    case VIP_BUFFER_FORMAT_UINT32:
    case VIP_BUFFER_FORMAT_INT32:
        size = sizeof(vip_int32_t) * BITS_PER_BYTE;
        break;
    case VIP_BUFFER_FORMAT_FP16:
    case VIP_BUFFER_FORMAT_BFP16:
    case VIP_BUFFER_FORMAT_UINT16:
    case VIP_BUFFER_FORMAT_INT16:
        size = sizeof(vip_int16_t) * BITS_PER_BYTE;
        break;

    case VIP_BUFFER_FORMAT_UINT8:
    case VIP_BUFFER_FORMAT_INT8:
    case VIP_BUFFER_FORMAT_BOOL8:
        size = sizeof(vip_int8_t) * BITS_PER_BYTE;
        break;

    case VIP_BUFFER_FORMAT_FP64:
    case VIP_BUFFER_FORMAT_UINT64:
    case VIP_BUFFER_FORMAT_INT64:
        size = sizeof(vip_int64_t) * BITS_PER_BYTE;
        break;

    case VIP_BUFFER_FORMAT_INT4:
    case VIP_BUFFER_FORMAT_UINT4:
        size = BITS_PER_BYTE / 2;
        break;

    default:
        vip_error("viplite doesn't support this buffer format=%d\n", format);
        break;
    }

    return size;
}

vip_uint32_t nbg_get_nn_command_size(
    nbg_fmt_descriptor_t* nbg_desc
    )
{
    vip_uint32_t cmd_size = 0;

    if (NBG_NN_COMMAND_SIZE_192 == nbg_desc->net_info.attribute.nn_command_size) {
        cmd_size = CMD_SIZE_192;
    }
    else {
        cmd_size = CMD_SIZE_128;
    }

    return cmd_size;
}

vip_uint32_t nbg_get_tp_command_size(
    nbg_fmt_descriptor_t* nbg_desc
    )
{
    vip_uint32_t cmd_size = 0;

    if (NBG_TP_COMMAND_SIZE_192 == nbg_desc->net_info.attribute.tp_command_size) {
        cmd_size = CMD_SIZE_192;
    }
    else {
        cmd_size = CMD_SIZE_128;
    }

    return cmd_size;
}

vip_char_t* nbg_get_operation_name(
    vip_uint32_t type
    )
{
    switch (type) {
    case NBG_OPERATION_TYPE_SH: return "SH";
    case NBG_OPERATION_TYPE_NN: return "NN";
    case NBG_OPERATION_TYPE_TP: return "TP";
    case NBG_OPERATION_TYPE_SW: return "SW";
    case NBG_OPERATION_TYPE_SC: return "SC";
    case NBG_OPERATION_TYPE_SP: return "SP";
    case NBG_OPERATION_TYPE_NN_LUT: return "NN_LUT";
    case NBG_OPERATION_TYPE_NT: return "NT";
    case NBG_OPERATION_TYPE_INIT: return "Init";
    case NBG_OPERATION_TYPE_END: return "End";
    default:
        return "UNKNOWN_OP";
    }
}

vip_size_t nbg_get_input_size(
    nbg_fmt_descriptor_t* nbg_desc,
    vip_uint32_t input_index
    )
{
    vip_uint32_t i = 0;
    vip_size_t size = 0;
    nbg_desc_input_output_t *input = VIP_NULL;

    input = &nbg_desc->inputs[input_index];
    size = nbg_get_buf_format_bit((vip_enum)input->data_format);
    for (i = 0; i < input->dim_count; i++) {
        size *= input->dim_size[i];
        if (0 == i) {
            /* round up */
            size = (size + BITS_PER_BYTE - 1) / BITS_PER_BYTE;
        }
    }

    return size;
}

vip_size_t nbg_get_output_size(
    nbg_fmt_descriptor_t* nbg_desc,
    vip_uint32_t output_index
    )
{
    vip_uint32_t i = 0;
    vip_size_t size = 0;
    nbg_desc_input_output_t* output = VIP_NULL;

    output = &nbg_desc->outputs[output_index];
    size = nbg_get_buf_format_bit((vip_enum)output->data_format);
    for (i = 0; i < output->dim_count; i++) {
        size *= output->dim_size[i];
        if (0 == i) {
            /* round up */
            size = (size + BITS_PER_BYTE - 1) / BITS_PER_BYTE;
        }
    }

    return size;
}

vip_bool_e nbg_operation_is_nn_engine(
    vip_uint32_t op_type
    )
{
    vip_bool_e is_nn = vip_false_e;
    switch (op_type) {
    case NBG_OPERATION_TYPE_NN:
    case NBG_OPERATION_TYPE_SP:
    case NBG_OPERATION_TYPE_NN_LUT:
    case NBG_OPERATION_TYPE_NT:
        is_nn = vip_true_e;
        break;
    default:
        break;
    }

    return is_nn;
}

vip_status_e nbg_fmt_get_version(
    nbg_fmt_reader_t *reader,
    vip_uint32_t *version
    )
{
    vip_status_e status = VIP_SUCCESS;

    if (!version) {
        vip_error("version is null\n");
        vipGoOnError(VIP_ERROR_INVALID_ARGUMENTS);
    }
    nbg_fmt_reader_locate(reader, 4);   /* offset 4 is version. */
    *version = nbg_fmt_read_uInt(reader, 1);

onError:
    return status;
}

vip_status_e nbg_fmt_read(
    nbg_fmt_reader_t *reader,
    nbg_fmt_descriptor_t *nbg_desc
    )
{
    vip_status_e status = VIP_SUCCESS;
    vip_uint32_t version = 0;

    vipOnError(nbg_fmt_get_version(reader, &version));

    if ((version & 0x00010000) == 0x00010000) {
        vipOnError(nbg_fmt_read_v1(reader, nbg_desc));
    }
    else if ((version & 0x00020000) == 0x00020000) {
        vipOnError(nbg_fmt_read_v2(reader, nbg_desc));
    }
    else {
        vipGoOnError(VIP_ERROR_NOT_SUPPORTED);
    }
    return status;

onError:
    vip_error("fail to read nbg version=0x%8x, status=%d\n", version, status);
    return status;
}

#if SIZE_CHECK_ERROR
vip_uint32_t size_check_error(void)
{
    vip_print("%s[%d] size check error\n", __FUNCTION__, __LINE__);

    return 0;
}
#endif
