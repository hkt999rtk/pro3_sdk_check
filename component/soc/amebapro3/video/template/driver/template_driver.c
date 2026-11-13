#define NULL 0

void * template_driver_open(void)
{
	return NULL;
}

int template_driver_close(void* ctx)
{
	return 0;
}

int template_driver_read(void *ctx, int *buf, int len)
{
	return 0;
}

int template_driver_write(void *ctx, int *buf, int len)
{
	return 0;
}

int template_driver_ctrl(void *ctx, int *buf, int len)
{
	return 0;
}

