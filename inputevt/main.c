#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <linux/input.h>
#include <string.h>


#define test_bit(bitnum,mask) (( 1 << bitnum%8) & mask[(bitnum) >> 3])

void usage(const char * pname)
{
	printf("usage: %s [-r read-number] <input device>\n",pname);
	printf("example: %s -r 100 /dev/input/event0\n",pname);
	printf("example: %s -r 100 event0\n",pname);
}
	
int main(int argc,char* argv[])
{
	
	int fd;
  	int read_num = 0, opt = 0;
  	

	fprintf(stderr,"input device test v0.1\n");
	fprintf(stderr,"This program was compiled at %s %s\n",__DATE__,__TIME__);
	fprintf(stderr,"Author: bergtian@gmail.com\n");

	   
   while ((opt = getopt(argc, argv, "hr:")) != -1) {
       switch (opt) {
       case 'r':
       		read_num = atoi(optarg);
       		break;
       case 'h':
       default:
           usage(argv[0]);
           return 0;
       }
   }
	
	if (optind >= argc) {
		  usage(argv[0]);
          return -1;
    }
    
	char devpath[256] = "\0";
	if(argv[optind][0] != '/'){
		strcpy(devpath,"/dev/input/");
	}
	strcat(devpath,argv[optind]);
	printf("event driver: %s\n", devpath);

// if((file = open(str, O_RDWR|O_NONBLOCK)) < 0)
	if((fd = open(devpath, O_RDWR)) < 0)
	{
		perror("device can not open");
		return -2;
	}

	
	//Listing 1. Sample EVIOCGVERSION Function
	/* ioctl() accesses the underlying driver */
	int version = 0;
	if (ioctl(fd, EVIOCGVERSION, &version)) {
	    perror("EVIOCGVERSION");
	}
	
	/* the EVIOCGVERSION ioctl() returns an int */
	/* so we unpack it and display it */
	printf("\tversion is %d.%d.%d\n",
	       version >> 16, (version >> 8) & 0xff,
	       version & 0xff);

	
	//Listing 3. Sample EVIOCGID ioctl 
	/* suck out some device information */
	struct input_id device_info;
	if(ioctl(fd, EVIOCGID, &device_info)) {
	    perror("EVIOCGVERSION");
	}
	
	/* the EVIOCGID ioctl() returns input_devinfo
	 * structure - see <linux/input.h>
	 * So we work through the various elements,
	 * displaying each of them
	 */
	printf("\tvendor %04hx product %04hx version %04hx",
	       device_info.vendor, device_info.product,
	       device_info.version);
	switch ( device_info.bustype)
	{
	 case BUS_PCI :
	     printf(" is on a PCI bus\n");
	     break;
	 case BUS_USB :
	     printf(" is on a Universal Serial Bus\n");
	     break;
	 case BUS_I2C :
	     printf(" is on BUS_I2C\n");
	     break;	     
	default:
		printf(" is on an unknow bus %x\n",(unsigned int)device_info.bustype);
		break;	     
	}

	//Listing 4. get name 
	char name[256]= "Unknown";
	if(ioctl(fd, EVIOCGNAME(sizeof(name)), name) < 0) {
	    perror("EVIOCGNAME");
	}
	printf("\tname is %s\n", name);
	
	//Listing 5. Using EVIOCGPHYS for Topology Information
	char phys[256]= "\0";
	if(ioctl(fd, EVIOCGPHYS(sizeof(phys)), phys) < 0) {
	    //perror("EVIOCGPHYS ");
	}
	printf("\tphys is %s\n", phys);
	
	//Listing 6. Finding a Unique Identifier
	char uniq[256]= "\0";
	if(ioctl(fd, EVIOCGUNIQ(sizeof(uniq)), uniq) < 0) {
	    //perror("EVIOCGUNIQ");
	}
	printf("\tidentity is %s\n",  uniq);
	
	printf("\n");
	//Listing 7. Determining Features with EVIOCGBIT
	unsigned char evtype_b[(EV_MAX >> 3) +1];
	memset(evtype_b, 0, sizeof(evtype_b));
	if (ioctl(fd, EVIOCGBIT(0, EV_MAX), evtype_b) < 0) {
	    perror("EVIOCGBIT");
	}
	
	printf("Supported event types %x:\n",evtype_b[0]);
	int yalv = 0;

	for (yalv = 0; yalv < EV_MAX; yalv++) {
	    if (test_bit(yalv, evtype_b)) {
	        /* the bit is set in the event types list */
	        printf("  Event type 0x%02x ", yalv);
	        switch ( yalv)
	            {
	            case EV_SYN :
	                printf(" (Synch Events)\n");
	                break;
	            case EV_KEY :
	                printf(" (Keys or Buttons)\n");
	                break;
	            case EV_REL :
	                printf(" (Relative Axes)\n");
	                break;
	            case EV_ABS :
	                printf(" (Absolute Axes)\n");
	                break;
	            case EV_MSC :
	                printf(" (Miscellaneous)\n");
	                break;
	            case EV_LED :
	                printf(" (LEDs)\n");
	                break;
	            case EV_SND :
	                printf(" (Sounds)\n");
	                break;
	            case EV_REP :
	                printf(" (Repeat)\n");
	                break;
	            case EV_FF :
	            case EV_FF_STATUS:
	                printf(" (Force Feedback)\n");
	                break;
	            case EV_PWR:
	                printf(" (Power Management)\n");
	                break;
	            default:
	                printf(" (Unknown: 0x%04hx)\n",
	             yalv);
	            }
	    }
	}
	

	
	//Listing 11. Using EVIOCGLED
	unsigned char led_b[64];
	memset(led_b, 0, sizeof(led_b));
	if(ioctl(fd, EVIOCGLED(sizeof(led_b)), led_b)<0){
		perror("EVIOCGLED");
	    return -11;
	}
	
	for (yalv = 0; yalv < LED_MAX; yalv++) {
	    if (test_bit(yalv, led_b)) {
	        /* the bit is set in the LED state */
	        printf("  LED 0x%02x ", yalv);
	        switch ( yalv)
	            {
	            case LED_NUML :
	                printf(" (Num Lock)\n");
	                break;
	            case LED_CAPSL :
	                printf(" (Caps Lock)\n");
	                break;
	            /* other LEDs not shown here*/
	            default:
	                printf(" (Unknown LED: 0x%04hx)\n",
	                       yalv);
	            }
	    }
	}


    printf("\n");
    //Listing 8. Checking for Busy Spots
    /* how many bytes were read */
    size_t rb;
    /* the events (up to 64 at once) */
    struct input_event ev[64];

    while(read_num-- > 0){
        rb=read(fd,ev,sizeof(struct input_event)*64);
        if (rb < (int) sizeof(struct input_event)) {
            perror("evtest: short read");
            return -10;
        }

        for (yalv = 0;
            yalv < (int) (rb / sizeof(struct input_event));
            yalv++)
        {
            printf("%ld.%06ld ",ev[yalv].time.tv_sec,ev[yalv].time.tv_usec);
            printf("type %d code %03d 0x%03x value %d\n",
                ev[yalv].type,
                ev[yalv].code,ev[yalv].code,ev[yalv].value);
         	if (0 == ev[yalv].type) printf("\n");                
        }
        //printf("\n");
    }

	close(fd);
	return 0;
}


