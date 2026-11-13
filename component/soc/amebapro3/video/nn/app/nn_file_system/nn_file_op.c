//--------------------------------------------------------------------------------------
// VIPNN file usage
//--------------------------------------------------------------------------------------
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "nn_file_op.h"

/*
typedef enum _viphal_file_mode {
	FILE_CREATE    = 0,
	FILE_APPEND,
	FILE_READ,
	FILE_CREATETEXT,
	FILE_APPENDTEXT,
	FILE_READTEXT,
} viphal_file_mode_e;
*/
void *nn_f_open(char *name, int mode)
{
	FILE *fp = NULL;
	if (mode == 4) {
		fp = fopen(name, "a+");
	} else {
		fp = fopen(name, "rb");
	}
	return (void *)fp;
}

void nn_f_close(void *fr)
{
	fclose((FILE *)fr);
}

int nn_f_read(void *fr, void *data, int size)
{
	int ret_size = 0;
	ret_size = fread(data, size, 1, (FILE *)fr);
	return ret_size;
}

int nn_f_seek(void *fr, int offset, int pos)
{
	int ret = 0;
	ret = fseek((FILE *)fr, offset, pos);
	return ret;
}

int nn_f_tell(void *fr)
{
	int ret = 0;
	ret = ftell((FILE *)fr);
	return ret;
}
