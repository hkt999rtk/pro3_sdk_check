#include "isp_main.h"
#include "isp_regs.h"
#include <errno.h>
#include <FreeRTOS.h>
#include <stream_buffer.h>
#include <ameba_soc.h>


#if 1
#define ispdbeg_printf_prod ispdbeg_printf
#define ispwarn_printf_prod ispwarn_printf
#define isperro_printf_prod isperro_printf
#define isp_printf_prod isp_printf
#else
#define ispdbeg_printf_prod(fmt, arg...)
#define ispwarn_printf_prod(fmt, arg...)
#define isperro_printf_prod(fmt, arg...)
#define isp_printf_prod(fmt, arg...)
#endif

#if FIXP_STREAM
extern StreamBufferHandle_t xStreamBuffer_consumer[stream_count];
extern StreamBufferHandle_t xStreamBuffer_producer[stream_count];
#else
extern struct isp_pipe pipe_isp[stream_count];
#endif

void erase_buffer(struct isp_io *io, int stream_index, int frame_index)
{
	int buf_size = io->io_buffers[stream_index][frame_index].out_buf.length;
	int buf_addr = io->io_buffers[stream_index][frame_index].out_buf.phy_addr;
	unsigned char *tmp = (unsigned char *)buf_addr;
	tmp[0] = tmp[3] = 0;
	tmp[buf_size / 2] = tmp[buf_size / 2 + 3] = 0;
	tmp[buf_size - 1 - 3] = tmp[buf_size - 1] = 0;
	ispdbeg_printf_prod("s%d, f%d value %d %d %d %d %d %d \r\n", stream_index, frame_index, tmp[0], tmp[3], tmp[buf_size / 2], tmp[buf_size / 2 + 3],
						tmp[buf_size - 1 - 3], tmp[buf_size - 1]);
}

void check_buffer(struct isp_io *io, int stream_index, int frame_index)
{
	int buf_size = io->io_buffers[stream_index][frame_index].out_buf.length;
	int buf_addr = io->io_buffers[stream_index][frame_index].out_buf.phy_addr;
	DCache_CleanInvalidate((uint32_t)buf_addr, buf_size);

	unsigned char *tmp = (unsigned char *)buf_addr;

#if 1
	unsigned char decimation[64];
	unsigned char *src = (unsigned char *)buf_addr;
	int dec_seg = (buf_size >> 4);
	for (int i = 0; i < 16 - 1; i++) {
		decimation[i] = *src;
		src += dec_seg;
	}
	decimation[16 - 1] = 0;
	ispdbeg_printf_prod("s%d, f%d CRC(addr: 0x%X, size:%d) = 0x%X \r\n", stream_index, frame_index, buf_addr, buf_size, crc32_calc((void *)(decimation), 16));
	//ispdbeg_printf_prod("s%d, f%d value %d %d %d %d %d %d CRC(addr: 0x%X, size:%d) = 0x%X \r\n", stream_index, frame_index, tmp[0], tmp[3], tmp[buf_size/2], tmp[buf_size/2+3], tmp[buf_size-1-3], tmp[buf_size-1], buf_addr, buf_size, crc32_calc((void *)(decimation), 16));
#else
	ispdbeg_printf_prod("s%d, f%d value %d %d %d %d %d %d \r\n", stream_index, frame_index, tmp[0], tmp[3], tmp[buf_size / 2], tmp[buf_size / 2 + 3],
						tmp[buf_size - 1 - 3], tmp[buf_size - 1]);
#endif
}

static int release_buffer(struct isp_io *io, int stream_index, int frame_index)
{
	void *base;
	uint32_t offset;
	uint32_t value, value_before;

	if (!io || !io->initialized) {
		return RTS_RETURN(RTS_E_NULL_POINT);
	}

	base = io->base;

	erase_buffer(io, stream_index, frame_index);
	offset = VIDEO_STREAM0_BUFFER_STATUS + stream_index * 0x4;
	value_before = read_reg(base, offset);
	if (stream_index < 4) {
		value = (0x0030 << frame_index * 8);
	} else {
		value = (0x0018 << frame_index * 8);
	}
	write_reg(base, value, offset);
	ispdbeg_printf_prod("offset 0x%X: 0x%X(before) 0x%X(write) 0x%X(after) str=%d frm=%d\n", offset, value_before, value, read_reg(base, offset), stream_index,
						frame_index);

	vTaskDelay(300);
	ispdbeg_printf_prod("offset 0x%X: 0x%X(after 300ms) str=%d frm=%d\n", offset, read_reg(base, offset), stream_index, frame_index);

	return RTS_OK;
}

static int producer_write_message(int *message, int stream_index)
{
	int *write_pipe;

	if (*message < 0 || *message >= stream_count) {
		isperro_printf_prod("Producer thread send message fail, range: 0~stream_count\n");
		return RTS_RETURN(RTS_E_SEND_FAIL);
	}

#if FIXP_STREAM
	if (xStreamBufferSend(xStreamBuffer_consumer[stream_index], message, sizeof(*message), RTOS_SB_TIMEOUT) != sizeof(*message)) {
#else
	if (xQueueSend(pipe_isp[stream_index].consumerQueue, message, 0) != pdPASS) {
#endif
		isperro_printf_prod("Producer thread send message fail\n");
		return RTS_RETURN(RTS_E_SEND_FAIL);
	}
	ispdbeg_printf_prod("Producer(ISP) thread send message success(stream_index%d, message=%d)\n", stream_index, *message);
	return RTS_OK;
}


static int send_buffer(struct isp_io *io, int stream_index, int frame_index)
{
	if (!io || !io->initialized) {
		return RTS_RETURN(RTS_E_NULL_POINT);
	}

	if (frame_index < 0 || frame_index >= stream_count) {
		isperro_printf_prod("Invalid index value: %d. Only 0~stream_count are allowed.\n",
							frame_index);
		return RTS_RETURN(RTS_E_SEND_FAIL);
	}
	if (producer_write_message(&frame_index, stream_index)) {
		return RTS_RETURN(RTS_E_SEND_FAIL);
	}
	check_buffer(io, stream_index, frame_index);
	ispdbeg_printf_prod("send thread sent buffer_stream%d_frame%d\n", stream_index,
						frame_index);
	return RTS_OK;
}

static int producer_read_message(int *message, int stream_index)
{
	int message_read = -1;

#if FIXP_STREAM
	ssize_t bytes_read = -1;
	int *read_pipe;

	bytes_read = xStreamBufferReceive(xStreamBuffer_producer[stream_index], &message_read, sizeof(message_read), RTOS_SB_TIMEOUT);
	if (bytes_read == 0) {
		ispwarn_printf_prod("No more data(stream_index%d)\n", stream_index);
		return RTS_RETURN(RTS_E_NOMESSAGE);
	} else if (bytes_read < 0) {
		isperro_printf_prod("bytes_read=%d, message_read=%d (shouldn't happen.)\r\n", bytes_read, message_read);
		if (errno == EAGAIN || errno == EWOULDBLOCK) {
			return RTS_RETURN(RTS_E_NOMESSAGE);
		} else {
			return RTS_RETURN(RTS_E_RECEIVE_FAIL);
		}
	} else {
#else
	if (xQueueReceive(pipe_isp[stream_index].producerQueue, &message_read,
					  0) != pdPASS) {
		return RTS_RETURN(RTS_E_NOMESSAGE);
	} else {
#endif
		if (message_read < 0 || message_read >= stream_count) {
			isperro_printf_prod("Producer thread receive message fail (shouldn't happen.). message_read=%d\n", message_read);
			return RTS_RETURN(RTS_E_RECEIVE_FAIL);
		}
	}
	*message = message_read;
	ispdbeg_printf_prod("Producer(ISP) thread receive message success(stream_index%d, message=%d)\n", stream_index, *message);
	return RTS_OK;
}

int clear_error_flag(struct isp_io *io, int stream_index, int frame_index)
{
	void *base;
	uint32_t offset;
	uint32_t value;

	if (!io || !io->initialized) {
		return RTS_RETURN(RTS_E_NULL_POINT);
	}

	base = io->base;

	offset = VIDEO_STREAM0_BUFFER_STATUS + stream_index * 0x4;
	value = (0x0010 << frame_index * 8);
	write_reg(base, value, offset);

	return RTS_OK;
}

int clear_error_flag_rgb(struct isp_io *io, int stream_index, int frame_index)
{
	void *base;
	uint32_t offset;
	uint32_t value;

	if (!io || !io->initialized) {
		return RTS_RETURN(RTS_E_NULL_POINT);
	}

	base = io->base;

	offset = VIDEO_STREAM0_BUFFER_STATUS + stream_index * 0x4;
	value = (0x0008 << frame_index * 8);
	write_reg(base, value, offset);

	return RTS_OK;
}

static int isp_monitor_AXI_buffer(struct isp_io *io, int stream_index)
{
	void *base;
	int i;
	uint32_t flag = 0;
	uint32_t status = 0;
	uint32_t length_overflow_flag;
	uint32_t count_overflow_flag;
	uint32_t axi_int_flag;
	uint32_t buffer_status_flag;
	uint32_t buffer_error_flag;

	base = io->base;

	length_overflow_flag = 0x30000 << (2 * stream_index);
	count_overflow_flag = 0x04 << (4 * stream_index);
	axi_int_flag = 0x08 << (4 * stream_index);

	flag = read_reg(base, VIDEO_AXI_INT_FLAG);
	ispdbeg_printf_prod("ISP stream flag is 0x%X\n", flag);
	if (flag & length_overflow_flag) {
		write_reg(base, length_overflow_flag, VIDEO_AXI_INT_FLAG);
		ispdbeg_printf_prod("length overflow flag: 0x%X (stream %d) \r\n", flag, stream_index);
		return RTS_RETURN(RTS_E_NOT_READY);
	}
	if (flag & count_overflow_flag) {
		write_reg(base, count_overflow_flag,
				  VIDEO_AXI_INT_FLAG); /*clear count overflow flag*/
		ispdbeg_printf_prod("count overflow flag: 0x%X (stream %d) \r\n", flag, stream_index);
	}
	if (flag & axi_int_flag) {
		write_reg(base, axi_int_flag, VIDEO_AXI_INT_FLAG);

		status =
			read_reg(base, VIDEO_STREAM0_BUFFER_STATUS + 0x4 * stream_index);

		for (i = 0; i < buffer_count; i++) {
			buffer_status_flag = 0x20 << (8 * i);
			buffer_error_flag = 0x10 << (8 * i);
			if (status & buffer_status_flag) {
				if (status & buffer_error_flag) {
					clear_error_flag(io, stream_index, i);
					//release_buffer(io, stream_index, i);
					ispdbeg_printf_prod("release_buffer: stream%d status: 0x%x, buffer_status_flag: 0x%x, frame_index=%d\n", stream_index, status, buffer_status_flag, i);
				} else {
					//int buf_size = io->io_buffers[stream_index][i].out_y_size + io->io_buffers[stream_index][i].out_uv_size;
					//DCache_CleanInvalidate((uint32_t)io->io_buffers[stream_index][i].out_buf.phy_addr, buf_size);

					int frame_index = i;
					if (!producer_write_message(&frame_index, stream_index)) {
						check_buffer(io, stream_index, frame_index);
						ispdbeg_printf_prod("send_buffer(ok): stream%d status: 0x%x, buffer_status_flag: 0x%x, frame_index=%d\n", stream_index, status, buffer_status_flag, i);
					} else {
						ispdbeg_printf_prod("send_buffer(fail): stream%d status: 0x%x, buffer_status_flag: 0x%x, frame_index=%d\n", stream_index, status, buffer_status_flag, i);
					}
				}
			}
		}

		return RTS_OK;
	}
	return RTS_RETURN(RTS_E_NOT_READY);
}

static int isp_monitor_AXI_buffer_rgb(struct isp_io *io)
{
	void *base;
	int i;
	uint32_t flag = 0;
	uint32_t status = 0;
	uint32_t length_overflow_flag;
	uint32_t count_overflow_flag;
	uint32_t axi_int_flag;
	uint32_t buffer_status_flag;
	uint32_t buffer_error_flag;
	int stream_index = 4;

	base = io->base;

	length_overflow_flag = 0xE0;
	count_overflow_flag = 0x08;
	axi_int_flag = 0x10;

	flag = read_reg(base, 0xF0264);
	ispdbeg_printf_prod("ISP stream RGB flag is 0x%X\n", flag);
	if (flag & length_overflow_flag) {
		write_reg(base, length_overflow_flag, 0xF0264);
		ispdbeg_printf_prod("length overflow flag: 0x%X (stream %d) \r\n", flag, stream_index);
		return RTS_RETURN(RTS_E_NOT_READY);
	}
	if (flag & count_overflow_flag) {
		write_reg(base, count_overflow_flag,
				  0xF0264); /*clear count overflow flag*/
		ispdbeg_printf_prod("count overflow flag: 0x%X (stream %d) \r\n", flag, stream_index);
	}
	if (flag & axi_int_flag) {
		write_reg(base, axi_int_flag, 0xF0264);

		status =
			read_reg(base, 0xF0090);

		for (i = 0; i < buffer_count; i++) {
			buffer_status_flag = 0x10 << (8 * i);
			buffer_error_flag  = 0x08 << (8 * i);
			if (status & buffer_status_flag) {
				if (status & buffer_error_flag) {
					clear_error_flag_rgb(io, stream_index, i);
					//release_buffer(io, stream_index, i);
					ispdbeg_printf_prod("release_buffer: stream%d status: 0x%x, buffer_status_flag: 0x%x, frame_index=%d\n", stream_index, status, buffer_status_flag, i);
				} else {
					//int buf_size = io->io_buffers[stream_index][i].out_y_size + io->io_buffers[stream_index][i].out_uv_size;
					//DCache_CleanInvalidate((uint32_t)io->io_buffers[stream_index][i].out_buf.phy_addr, buf_size);

					int frame_index = i;
					if (!producer_write_message(&frame_index, stream_index)) {
						check_buffer(io, stream_index, frame_index);
						ispdbeg_printf_prod("send_buffer(ok): stream%d status: 0x%x, buffer_status_flag: 0x%x, frame_index=%d\n", stream_index, status, buffer_status_flag, i);
					} else {
						ispdbeg_printf_prod("send_buffer(fail): stream%d status: 0x%x, buffer_status_flag: 0x%x, frame_index=%d\n", stream_index, status, buffer_status_flag, i);
					}
				}
			}
		}

		return RTS_OK;
	}
	return RTS_RETURN(RTS_E_NOT_READY);
}

static int isp_monitor_consumer_status(struct isp_io *io, int stream_index)
{
	int ret;
	int buffer_index;

	while (1) {
		ret = producer_read_message(&buffer_index, stream_index);
		if (ret == RTS_OK) {
			release_buffer(io, stream_index, buffer_index);
		} else if (ret == RTS_RETURN(RTS_E_NOMESSAGE)) {
			break;
		} else {
			return RTS_RETURN(RTS_E_RECEIVE_FAIL);
		}
	}
	return RTS_OK;
}

static int isp_wait_completion_multi(struct isp_io *io, int stream_index)
{
	int ret = 0;

	if (!io || !io->initialized) {
		return RTS_RETURN(RTS_E_NULL_POINT);
	}

	while (1) {
		ret = isp_monitor_consumer_status(io, stream_index);
		if (ret) {
			isperro_printf_prod(
				"producer thread ISP monitor consumer status fail, ret is %d\n",
				ret);
			return ret;
		}

		if (stream_index < 4) {
			ret = isp_monitor_AXI_buffer(io, stream_index);
		} else {
			ret = isp_monitor_AXI_buffer_rgb(io);
		}
		if (ret == RTS_RETURN(RTS_E_NOT_READY) || ret == RTS_OK) {
			break;
		} else {
			isperro_printf_prod("producer thread ISP monitor AXI_buffer status fail\n");
			return ret;
		}
	}
	if (0) {
		return RTS_EXIT;
	}
	return RTS_OK;
}

void producer_ISP(void *arg)
{
	int ret;
	struct isp_io *io = (struct isp_io *)arg;

	ispdbeg_printf_prod("Producer thread created successfully!\n");
	while (1) {
		for (int i = 0; i < stream_count; i++) {
			ret = isp_wait_completion_multi(io, i);
			isp_printf_prod("isp_wait_completion_multi, stream = %d, ret = %d\n", i, ret);
			if (ret) {
				if (ret != RTS_EXIT)
					isperro_printf_prod(
						"producer thread ISP%d wait isp completion fail, ret = %d\n",
						i, ret);
				break;
			}
		}
		if (ret) {
			break;
		}
		vTaskDelay(1);
	}

	ispdbeg_printf_prod("producer thread exit!\n");
	return NULL;
}
