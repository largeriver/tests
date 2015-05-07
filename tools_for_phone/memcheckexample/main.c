#include <stdlib.h>
#include <stdio.h>
int main(int argc,char *argv[]){

	void * p1 = malloc(100);
	((int*)p1)[0] = 1;
	
free(p1);
	return 0;
}
