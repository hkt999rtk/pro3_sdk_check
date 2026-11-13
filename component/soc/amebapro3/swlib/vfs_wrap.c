#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>

/**************************************************
* default dummy FILE api wrap for compiler
*
**************************************************/

#ifndef __weak
#define __weak __attribute__((weak))
#endif

__weak
FILE *__wrap_fopen(const char *filename, const char *mode)
{
	(void)filename;
	(void)mode;
	errno = ENOSYS;
	return NULL;
}

__weak
int __wrap_fclose(FILE *stream)
{
	(void)stream;
	errno = ENOSYS;
	return -1;
}

__weak
size_t __wrap_fread(void *ptr, size_t size, size_t count, FILE *stream)
{
	(void)ptr;
	(void)size;
	(void)count;
	(void)stream;
	errno = ENOSYS;
	return (size_t)0;
}

__weak
size_t __wrap_fwrite(const void *ptr, size_t size, size_t count, FILE *stream)
{
	(void)ptr;
	(void)size;
	(void)count;
	(void)stream;
	errno = ENOSYS;
	return (size_t)0;
}

__weak
int __wrap_fseek(FILE *stream, long int offset, int origin)
{
	(void)stream;
	(void)offset;
	(void)origin;
	errno = ENOSYS;
	return -1;
}

__weak
void __wrap_rewind(FILE *stream)
{
	(void)stream;
	errno = ENOSYS;
}

__weak
int __wrap_fgetpos(FILE *stream, fpos_t *p)
{
	(void)stream;
	(void)p;
	errno = ENOSYS;
	return -1;
}

__weak
int __wrap_fsetpos(FILE *stream, fpos_t *p)
{
	(void)stream;
	(void)p;
	errno = ENOSYS;
	return -1;
}

__weak
int __wrap_fflush(FILE *stream)
{
	(void)stream;
	errno = ENOSYS;
	return -1;
}

__weak
int __wrap_remove(const char *filename)
{
	(void)filename;
	errno = ENOSYS;
	return -1;
}

__weak
int __wrap_rename(const char *oldname, const char *newname)
{
	(void)oldname;
	(void)newname;
	errno = ENOSYS;
	return -1;
}

__weak
int __wrap_feof(FILE *stream)
{
	(void)stream;
	return EOF;
}

__weak
int __wrap_ferror(FILE *stream)
{
	(void)stream;
	errno = ENOSYS;
	return -1;
}

__weak
long int __wrap_ftell(FILE *stream)
{
	(void)stream;
	errno = ENOSYS;
	return (long int) -1;
}

__weak
int __wrap_ftruncate(int fd, off_t length)
{
	(void)fd;
	(void)length;
	errno = ENOSYS;
	return -1;
}

__weak
int __wrap_fgetc(FILE *stream)
{
	(void)stream;
	errno = ENOSYS;
	return EOF;
}

__weak
int __wrap_fputc(int character, FILE *stream)
{
	(void)character;
	(void)stream;
	errno = ENOSYS;
	return -1;
}

__weak
int __wrap_fputs(const char *str, FILE *stream)
{
	(void)str;
	(void)stream;
	errno = ENOSYS;
	return -1;
}

__weak
char *__wrap_fgets(char *str, int num, FILE *stream)
{
	(void)str;
	(void)num;
	(void)stream;
	errno = ENOSYS;
	return NULL;
}

__weak
void *__wrap_opendir(const char *name)
{
	(void)name;
	errno = ENOSYS;
	return NULL;
}

__weak
void *__wrap_readdir(void *pdir)
{
	(void)pdir;
	errno = ENOSYS;
	return NULL;
}

__weak
int __wrap_closedir(void *dirp)
{
	(void)dirp;
	errno = ENOSYS;
	return -1;
}

__weak
int __wrap_scandir(const char *dirp, void ***namelist,
				   int (*filter)(const void *),
				   int (*compar)(const void **, const void **))
{
	(void)dirp;
	(void)namelist;
	(void)filter;
	(void)compar;
	errno = ENOSYS;
	return -1;
}

__weak
int __wrap_rmdir(const char *path)
{
	(void)path;
	errno = ENOSYS;
	return -1;
}

__weak
int __wrap_mkdir(const char *pathname, mode_t mode)
{
	(void)pathname;
	(void)mode;
	errno = ENOSYS;
	return -1;
}

__weak
int __wrap_access(const char *pathname, int mode)
{
	(void)pathname;
	(void)mode;
	errno = ENOSYS;
	return -1;
}
