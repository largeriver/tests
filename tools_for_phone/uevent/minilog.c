#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

#include <sys/time.h>
void mini_log_printf(const char *fmt, ...)
{
   va_list ap;

	struct timeval t;
	gettimeofday(&t,NULL);
	fprintf(stderr,"[%8u.%3u]",(unsigned int)t.tv_sec,(unsigned int)t.tv_usec/1000);
	
	va_start(ap, fmt);
	vfprintf(stderr, fmt, ap);
	va_end(ap);

	fprintf(stderr,"\n");
}