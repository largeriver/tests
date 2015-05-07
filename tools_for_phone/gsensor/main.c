/* Acceleration
* ------------
*
*  All values are in SI units (m/s^2) and measure the acceleration of the
*  device minus the force of gravity.
*  
*  x: Acceleration minus Gx on the x-axis 
*  y: Acceleration minus Gy on the y-axis 
*  z: Acceleration minus Gz on the z-axis
*  
*  Examples:
*    When the device lies flat on a table and is pushed on its left side
*    toward the right, the x acceleration value is positive.
    平放时，X，Y都应该0。从左到右或者从上到下应该为正数

*    
*    When the device lies flat on a table, the acceleration value is +9.81,
*    which correspond to the acceleration of the device (0 m/s^2) minus the
*    force of gravity (-9.81 m/s^2).
    平放时，加速度值为9.81
*    
*    When the device lies flat on a table and is pushed toward the sky, the
*    acceleration value is greater than +9.81, which correspond to the
*    acceleration of the device (+A m/s^2) minus the force of 
*    gravity (-9.81 m/s^2).

*    
*/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <signal.h>
int fd = 0;
int req_exit = 0;

void sighandler(int sig){
    fprintf(stderr,"got sig:%d\n",sig);
    req_exit = 1;
    if(fd >0){
        write(fd,"wake",4);
    }
}


int main(void)
{
    char msg[128];int ret = 0;
    int x,y,z;
    float fx,fy,fz;
    
    struct sigaction act;
    sigemptyset(&act.sa_mask);
    act.sa_flags = 0;
    act.sa_handler = (void(*)(int))&sighandler;
    if ( sigaction(SIGINT, &act, NULL) != 0 ){
        fprintf(stderr,"reg SIGINT(%d) failed",SIGINT);
        return -1;
    }

	fprintf(stderr,"This program was compiled at %s %s\n",__DATE__,__TIME__);
	fprintf(stderr,"Author: bergtian@gmail.com\n");



    fd = open("/dev/g-sensor",O_RDONLY);
    
    if(fd < 0){
        fprintf(stderr, "open device failed:%s\n",strerror(errno));
        return -1;
    }
    
    while (!req_exit) {
        ret = read(fd,msg,128);
        if(ret <= 0)
            break;
        
        msg[ret] = 0;
        if(0 == strncmp("acceleration",msg,12)){
            ret = sscanf(msg,"acceleration:%d:%d:%d",&x,&y,&z);
            if(ret != 3) continue;
            fx=(x - 981000)/100000;
            fy=(y - 981000)/100000;
            fz=(z)/100000;
            printf("% 6.2fg\t% 6.2fg\t% 6.2gf\n",fx,fy,fz);
        }
    }

    close(fd);

    return 0;
}
