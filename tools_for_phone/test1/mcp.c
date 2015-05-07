#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <malloc.h>

#define LEN (600 * 1024)
static  void * mymemcpy(void *dest, const void * src, int size)
{
	unsigned *pd = dest;
	const unsigned *ps = src;
	size = size / 4;
	while (size--)
	{
		*pd++ = *ps++;
	}

	return dest;
}

int main(int argc, char *argv[])
{
	if (atoi(argv[1]) == 1) {
	int i = 0;
	char *src = (char *)malloc(LEN);
	char *dest = (char *)malloc(LEN);
	while (i < 1000) {
		i++;

	memcpy(dest, src, LEN);
	memcpy(src, dest, LEN);
	}
	if (src[77] == dest[77] && src[56] == dest[56])	
		printf("ok\n");

	return 0;
	}
	
	if (atoi(argv[1]) == 2)
	{
			
		int i = 0;
		unsigned *src = (unsigned *)malloc(LEN);
		unsigned *dest = (unsigned *)malloc(LEN);
		while (i < 1000) {
			i++;
		mymemcpy(dest, src, LEN);
		mymemcpy(src, dest, LEN);
		}
#if 1
	if (src[77] == dest[77] && src[56] == dest[56])	
		printf("ok\n");
#endif
		return 0;
	}
}


