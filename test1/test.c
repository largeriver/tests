#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <errno.h>
int main(int argc,const char* argv[])
{
   for(int i=1; i < argc;i++){
	int j = atoi(argv[i]);
	printf("error:%d	%s\n",j,(const char*)strerror(j));
	}

	return 0;	
}
