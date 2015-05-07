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

 * Proximity
 * ---------
 *
 * The distance value is measured in centimeters.  Note that some proximity
 * sensors only support a binary "close" or "far" measurement.  In this case,
 * the sensor should report its maxRange value in the "far" state and a value
 * less than maxRange in the "near" state.

 0 == close
 1 == far

 *
 * Light
 * -----
 *
 * The light sensor value is returned in SI lux units.
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




#include "hardware/hardware.h"
#include "hardware/sensors.h"



static    struct sensors_control_device_t * scd = NULL;
static    struct sensors_data_device_t   * sdd  = NULL;
static int req_exit = 0;

void sighandler(int sig){
    fprintf(stderr,"got sig:%d\n",sig);
    req_exit = 1;
    if(scd){
        scd->wake(scd);
    }
}

void usage(const char * program){
    fprintf(stderr,"%s options\n",program);
    fprintf(stderr,"options:\n");
    fprintf(stderr,"\t-h    print this help\n");
    fprintf(stderr,"\t-g    enable g-sensor\n");
    fprintf(stderr,"\t-l    enable l-sensor\n");
    fprintf(stderr,"\t-d    enable d-sensor\n");
    fprintf(stderr,"\t-D <delay in ms> deault 200ms \n");
    fprintf(stderr,"\t-L <library path>  \n");
    fprintf(stderr,"-------------------\n");
    fprintf(stderr,"Note0: Dont enable l-sensor and d-sensor meanwhile \n");
    fprintf(stderr,"Note1: To terminate the program, use: kill -2 <pid> \n");
    
}

int main(int argc,char *argv[])
{
//sensors

    struct hw_module_t * hm = NULL;
    struct sensors_module_t *hm_sensors = NULL;
    struct sensor_t const * sensors_list = NULL;
    uint32_t sensors_num = 0;

    native_handle_t* nh = NULL;

    int lsensor_enable = 0;
    int gsensor_enable = 0;
    int dsensor_enable = 0;

    int lsensor_handle = -1;
    int gsensor_handle = -1;
    int dsensor_handle = -1;

    int delay = 200;
    
    int ret = 0;
    uint32_t i = 0;
    uint32_t active_count = 0;

    const char* lib_path=NULL;



    struct sigaction act;
    sigemptyset(&act.sa_mask);
    act.sa_flags = 0;
    act.sa_handler = (void(*)(int))&sighandler;
    if ( sigaction(SIGINT, &act, NULL) != 0 ){
        fprintf(stderr,"reg SIGINT(%d) failed\n",SIGINT);
        return -1;
    }

    int opt;
    while ((opt = getopt(argc, argv, "hdlgD:L:")) != -1) {
       switch (opt) {

       case 'd':
           dsensor_enable = 1;
           break;
       case 'l':
           lsensor_enable = 1;
           break;
       case 'g':
           gsensor_enable = 1;
           break;
       case 'D':
           delay = atoi(optarg);
           if(delay <=0)
                delay = 200;
           break;    
       case 'L':
           lib_path = strdup(optarg);
           break;               
       default: /* '?' */
       case 'h':
           usage(argv[0]);
           return 0;
       }
   }
//	
//	   if(lsensor_enable && dsensor_enable){
//	        dsensor_enable = 0; 
//	        fprintf(stderr,"disable d-sensor\n");
//	   }
    if(optind == 1){
        usage(argv[0]);
        return 0;
    }

    
    ret = hw_get_module("sensors",(const struct hw_module_t **)&hm,lib_path);
    if(ret < 0){
        fprintf(stderr,"get module failed:%d\n",ret);
        return -2;
    }

    hm_sensors = (struct sensors_module_t *)hm;

    sensors_num = hm_sensors->get_sensors_list(hm_sensors,&sensors_list);
    if(sensors_num <=0){
          fprintf(stderr,"no sensor ret = %d\n",ret);
          return -2;
    }
    for(i = 0; i< sensors_num;i++){
        fprintf(stderr,"sensor %s, vendor %s, version %d, handle %d\n",sensors_list[i].name,sensors_list[i].vendor,sensors_list[i].version,sensors_list[i].handle);
        switch(sensors_list[i].type){
        case SENSOR_TYPE_PROXIMITY:
            dsensor_handle = sensors_list[i].handle;
            break;
        case SENSOR_TYPE_LIGHT:
            lsensor_handle = sensors_list[i].handle;
            break;
        case SENSOR_TYPE_ACCELEROMETER:
            gsensor_handle = sensors_list[i].handle;
            break;            
        }
    }

    ret = sensors_control_open(hm,&scd);
    if(ret < 0){
        fprintf(stderr,"sensors_control_open failed:%d\n",ret);
        return -3;
    }


    nh = scd->open_data_source(scd);
    if(!nh){
        fprintf(stderr,"open_data_source failed\n");
        goto failed_active;
    }
 
    if(dsensor_handle >=0 && dsensor_enable){
        ret = scd->activate(scd,dsensor_handle,dsensor_enable);
        if(ret < 0){
            fprintf(stderr,"d-sensor %s failed:%d\n",dsensor_enable?"active":"deactive",ret);
        } else{
            fprintf(stderr,"d-sensor %s successed\n",dsensor_enable?"active":"deactive");
            active_count++;
        }
    }

    if(lsensor_handle >=0 && lsensor_enable){
        ret = scd->activate(scd,lsensor_handle,lsensor_enable);
        if(ret < 0){
            fprintf(stderr,"l-sensor %s failed:%d\n",lsensor_enable?"active":"deactive",ret);
        } else{
            fprintf(stderr,"l-sensor %s successed\n",lsensor_enable?"active":"deactive");
            active_count++;
        }
    }

    if(gsensor_handle >=0 && gsensor_enable){
        ret = scd->activate(scd,gsensor_handle,gsensor_enable);
        if(ret < 0){
            fprintf(stderr,"g-sensor %s failed:%d\n",gsensor_enable?"active":"deactive",ret);
        } else{
            fprintf(stderr,"g-sensor %s successed\n",gsensor_enable?"active":"deactive");
            active_count++;
        }
    }

    if(active_count == 0){
        fprintf(stderr,"no sesenosr avtived\n");
        goto failed_active;
    }

    ret = scd->set_delay(scd,(int32_t)delay);
    if(ret < 0){
        fprintf(stderr,"set_delay failed:%d\n",ret);
        goto failed_active;
    }else{
        fprintf(stderr,"set_delay %d\n",delay);
    }
    

    ret = sensors_data_open(hm,&sdd);
    if(ret < 0){
        fprintf(stderr,"sensors_data_open failed:%d\n",ret);
        goto failed_sdd;
        
    }    
    
    ret = sdd->data_open(sdd,nh);
    if(ret < 0){
        fprintf(stderr,"data_open failed:%d\n",ret);
        goto failed_sdd;
    }  
    nh = 0;


    while(!req_exit){
        sensors_data_t data;
        memset(&data,0,sizeof(data));
        ret = sdd->poll(sdd,&data);
        if(ret < 0){
            fprintf(stderr,"poll failed:%d\n",ret);
            continue;
        }


        if(ret == dsensor_handle) 
            fprintf(stderr,"proximity    :% 6f\n",data.distance);
        else if(ret == lsensor_handle) 
            fprintf(stderr,"light           :% 6f\n",data.light);
        else if(ret == gsensor_handle) 
            fprintf(stderr,"acceleration:% 6f\t% 6f\t% 6f\n",data.acceleration.x,data.acceleration.y,data.acceleration.z);
        else
            fprintf(stderr,"UNKNOWN %d\n",ret);
            
    }

    

    ret = sdd->data_close(sdd);
    if(ret < 0){
        fprintf(stderr,"data_close failed:%d\n",ret);
    }  
failed_sdd:
    if(sdd)
        sensors_data_close(sdd);
    sdd = NULL;


failed_active:
    
    if(scd->close_data_source){
        scd->close_data_source(scd);
    }
    sensors_control_close(scd);
    scd  = NULL;

     return 0;
}
