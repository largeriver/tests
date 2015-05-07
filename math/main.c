#define _GNU_SOURCE
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

int main (int argc,char* argv[]){
	int min = -84;
	int max = 12;
	double v;
	
	int i;
	for(i=min;i<=max;i++){
		if(i%5 ==0)
			fprintf(stderr,"\n");
		v=exp10((double)i/(double)20.0)*0x4000;
		fprintf(stderr,"% 6d/*% 3d*/, ",(int)v,i);
	}
	fprintf(stderr,"\n");
	return 0;
}
