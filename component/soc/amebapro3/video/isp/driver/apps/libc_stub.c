#include <stdio.h>

int raise(int signo)
{
	printf("raise signal %d\n", signo);
	while (1)
		;
}
