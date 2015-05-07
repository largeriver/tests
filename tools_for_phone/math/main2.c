#define _GNU_SOURCE
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

int main (int argc,char* argv[]){
	int min = -84;
	int max = 12;
	int db=-84,old=-84;
	
	
	int i;
	for(i = 1;i<= 0xFFFF;i++){
		db= 20*log10f((float)i/(float)0x4000);
		if(old != db){
			if(db%5 ==0)
				fprintf(stderr,"\n");
			fprintf(stderr,"% 6d/*% 3d*/, ",i-1,old);				
			old=db;
		}
	}
		
	fprintf(stderr,"\n");
	return 0;
}
