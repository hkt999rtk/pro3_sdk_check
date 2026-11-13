#include "isp_main.h"
#include <errno.h>
#include <FreeRTOS.h>
#include <stream_buffer.h>
#include "isp_regs.h"
#include <ameba_soc.h>

#if 1
#define ispdbeg_printf_cons ispdbeg_printf
#define ispwarn_printf_cons ispwarn_printf
#define isperro_printf_cons isperro_printf
#define isp_printf_cons isp_printf
#else
#define ispdbeg_printf_cons(fmt, arg...)
#define ispwarn_printf_cons(fmt, arg...)
#define isperro_printf_cons(fmt, arg...)
#define isp_printf_cons(fmt, arg...)
#endif
#if FIXP_STREAM
extern StreamBufferHandle_t xStreamBuffer_consumer[stream_count];
extern StreamBufferHandle_t xStreamBuffer_producer[stream_count];
#else
extern struct isp_pipe pipe_isp[stream_count];
#endif
extern int frame_num;
#if 0
static void *isp_read_output_begin(struct isp_io *io, int stream_index,
								   int frame_index)
{
	struct io_buffer *current_buffer;

	current_buffer = &io->io_buffers[stream_index][frame_index];
	if (!io || !io->initialized) {
		return NULL;
	}
	if (!current_buffer->out_buf.length) {
		return NULL;
	}
	rtsc_mem_sync_cpu(current_buffer->out_buf.phy_addr,
					  current_buffer->out_buf.length);
	return current_buffer->out_buf.vm_addr;
}

static void isp_read_output_finish(struct isp_io *io, int stream_index,
								   int frame_index)
{
	struct io_buffer *current_buffer;

	current_buffer = &io->io_buffers[stream_index][frame_index];
	if (!io || !io->initialized || !current_buffer->out_buf.length) {
		return;
	}
	rtsc_mem_sync_device(current_buffer->out_buf.phy_addr,
						 current_buffer->out_buf.length);
}
#endif

#if 1
static int write_image(void *buf, size_t size, void *priv, int stream_index,
					   int count)
{
	int ret;
	FILE *fp;
	char name[128] = {0};

	ret = sprintf(name,
				  "D:\\oscaryang\\00_Test\\out_buffer_stream%d_frame%d_%d.raw",
				  stream_index, *(uint32_t *)priv, count);
	ispdbeg_printf_cons("save frame to %s, size= %d\n", name, size);
	fp = fopen(name, "w");
	if (!fp) {
		return RTS_RETURN(RTS_E_OPEN_FAIL);
	}
	if (size >= 960 * 1024) {
		size = 960 * 1024;
	}
	ret = fwrite(buf, 1, size, fp);
	ispdbeg_printf_cons("write done write_size=%d\n", ret);
	if (ret != size) {
		return RTS_RETURN(RTS_E_WRITE_FAIL);
	}
	fflush(fp);
	fclose(fp);

	return RTS_OK;
}

static int save_image(struct isp_io *io, int buf_index, int stream_index, int count)
{
	void *buf;
	int ret;
	struct io_buffer *current_io_buffer;
	int buf_size;

	current_io_buffer = io->io_buffers[stream_index];

	buf_size = current_io_buffer[buf_index].out_buf.length;
	DCache_CleanInvalidate((uint32_t)current_io_buffer[buf_index].out_buf.phy_addr, buf_size);
	ispdbeg_printf_cons("stream%d, frame_index=%d, buf_size=%d\n", stream_index, buf_index, buf_size);
	buf = (void *)current_io_buffer[buf_index].out_buf.phy_addr;


	if (!buf) {
		isperro_printf_cons("begin write input fail\n");
		ret = RTS_RETURN(RTS_E_NULL_POINT);
		return ret;
	}
	ret = write_image(buf, current_io_buffer[buf_index].out_buf.length,
					  &buf_index, stream_index, count);
	if (ret) {
		return ret;
	}
	return RTS_OK;
}
#endif

static int consumer_write_message(int *message, int stream_index)
{
	int *write_pipe;

	if (*message < 0 || *message >= stream_count) {
		isperro_printf_cons("Consumer thread send message fail, index only 0~stream_count \n");
		return RTS_RETURN(RTS_E_SEND_FAIL);
	}

#if FIXP_STREAM
	if (xStreamBufferSend(xStreamBuffer_producer[stream_index], message, sizeof(*message), RTOS_SB_TIMEOUT) != sizeof(*message)) {
#else
	if (xQueueSend(pipe_isp[stream_index].producerQueue, message, 0) != pdPASS) {
#endif
		isperro_printf_cons("Consumer thread send message fail(stream_index%d, message=%d)\n", stream_index, *message);
		return RTS_RETURN(RTS_E_SEND_FAIL);
	}
	ispdbeg_printf_cons("Consumer(User) thread send message success(stream_index%d, message=%d)\n", stream_index, *message);
	return RTS_OK;
}

static int consumer_read_message(int *message, int stream_index)
{
	int message_read = -1;
	ssize_t bytes_read = -1;

#if FIXP_STREAM
	int *read_pipe;
	bytes_read = xStreamBufferReceive(xStreamBuffer_consumer[stream_index], &message_read, sizeof(message_read), RTOS_SB_TIMEOUT);
	if (bytes_read == 0) {
		ispwarn_printf_cons("No more data, bytes_read=%d, message_read=%d\r\n", bytes_read, message_read);
		return RTS_RETURN(RTS_E_NOMESSAGE);
	} else if (bytes_read < 0) {
		isperro_printf_cons("bytes_read=%d, message_read=%d (shouldn't happen.)\r\n", bytes_read, message_read);
		if (errno == EAGAIN || errno == EWOULDBLOCK) {
			return RTS_RETURN(RTS_E_NOMESSAGE);
		} else {
			return RTS_RETURN(RTS_E_RECEIVE_FAIL);
		}
	} else {
#else
	if (xQueueReceive(pipe_isp[stream_index].consumerQueue, &message_read,
					  0) != pdPASS) {
		return RTS_RETURN(RTS_E_NOMESSAGE);
	} else {
#endif
		if (message_read < 0 || message_read >= stream_count) {
			isperro_printf_cons("Consumer thread receive message fail (shouldn't happen.), bytes_read=%d, message_read=%d\r\n", bytes_read, message_read);
			return RTS_RETURN(RTS_E_RECEIVE_FAIL);
		}
	}
	*message = message_read;
	ispdbeg_printf_cons("consumer(User) thread receive message success(stream_index%d, bytes_read=%d, message=%d)\n", stream_index, bytes_read, *message);
	return RTS_OK;
}
static int grid_size = 30;
static int grid_dire = 0;
static void buffer_handle_isp0(struct isp_io *io, int frame_index)
{
	static int strm0 = 0;
	save_image(io, frame_index, 0, strm0);
	void *base = io->base;
	if (grid_dire) {
		grid_size += 2;
	} else {
		grid_size -= 2;
	}
	if (grid_size > 30) {
		grid_size = 30;
		grid_dire = 0;
	}
	if (grid_size < 8) {
		grid_size = 8;
		grid_dire = 1;
	}
	write_reg(base, grid_size + (grid_size << 16), 0x6048);
	isp_printf("----------------0x%X grid_size=%d\r\n", read_reg(base, 0x6048), grid_size);

	strm0++;
}

static void buffer_handle_isp1(struct isp_io *io, int frame_index)
{
	static int strm1 = 0;
	save_image(io, frame_index, 1, strm1);
	strm1++;
}

static void buffer_handle_isp2(struct isp_io *io, int frame_index)
{
	static int strm2 = 0;
	save_image(io, frame_index, 2, strm2);
	strm2++;
}

static void buffer_handle_isp3(struct isp_io *io, int frame_index)
{
	static int strm3 = 0;
	save_image(io, frame_index, 3, strm3);
	strm3++;
}

static void buffer_handle_isp4(struct isp_io *io, int frame_index)
{
	static int strm4 = 0;
	save_image(io, frame_index, 4, strm4);
	strm4++;
}

static int buffer_handle(struct isp_io *io, int frame_index, int stream_index)
{
	int ret;
	int message = frame_index;

	if (stream_index == 0) {
		buffer_handle_isp0(io, frame_index);
	} else if (stream_index == 1) {
		buffer_handle_isp1(io, frame_index);
	} else if (stream_index == 2) {
		buffer_handle_isp2(io, frame_index);
	} else if (stream_index == 3) {
		buffer_handle_isp3(io, frame_index);
	} else if (stream_index == 4) {
		buffer_handle_isp4(io, frame_index);
	}
	/*buffer handle done*/
	ret = consumer_write_message(&message, stream_index);
	if (ret) {
		return ret;
	}
	ispdbeg_printf_cons("The consumer thread handle buffer%d done!\n", frame_index);
	return RTS_OK;
}

void consume_buffer(void *arg)
{
	int frame_index = -1;
	int ret;
	struct isp_io *io = (struct isp_io *)arg;

	ispdbeg_printf_cons("Consumer thread created successfully!\n");

	while (1) {
		for (int i = 0; i < stream_count; i++) {
			ret = consumer_read_message(&frame_index, i);
			isp_printf_cons("consumer_read_message, stream = %d, ret = %d, get frame_index = %d\n", i, ret, frame_index);
			if (ret == RTS_RETURN(RTS_E_RECEIVE_FAIL)) {
				break;
			} else if (ret == RTS_RETURN(RTS_E_NOMESSAGE)) {
				continue;
			} else {
				ispdbeg_printf_cons("consume thread start handle buffer%d.\n",
									frame_index);
				ret = buffer_handle(io, frame_index, i);
				if (ret) {
					isperro_printf_cons("buffer handle fail, ret = %d\n", ret);
					break;
				}
			}
		}
		if (ret == RTS_RETURN(RTS_E_RECEIVE_FAIL)) {
			break;
		}
		vTaskDelay(1);
	}

	ispdbeg_printf_cons("exit consumer thread.\n");
	return NULL;
}
