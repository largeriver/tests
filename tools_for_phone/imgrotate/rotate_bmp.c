#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <linux/fb.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <time.h>

static unsigned char *fbp = 0;
static int xres = 0;
static int yres = 0;
static int bits_per_pixel = 0;
struct fb_var_screeninfo vinfo;
struct fb_fix_screeninfo finfo;
int  times=1;   

void rotate_bmp(unsigned int *buffer,unsigned char * fbp,int xres,int yres, int times);
void rotate_bmp_hit(unsigned int *buffer,unsigned char *fbp, int xres, int yres,int times);
void write_continue(unsigned int *buffer, unsigned char *fbp,int xres,int yres, int times);
static inline int gettime(void);


int main ( int argc, char *argv[] )
{
    struct timeval t;
		int curTime,curTime2;
    int fbfd = 0;
    uint32_t screensize = 0;
  
     //查看编译时间
	  fprintf(stdout,"rotate_bmp was compiled at %s %s\n",__DATE__,__TIME__);	  
    //打开显示设备
    fbfd = open("/dev/graphics/fb0", O_RDWR); 
    if (!fbfd)
    {
        printf("Error: cannot open framebuffer device.\n");
        exit(1);
    }

    if (ioctl(fbfd, FBIOGET_FSCREENINFO, &finfo))
    {
        printf("Error：reading fixed information.\n");
        exit(2);
    }

    if (ioctl(fbfd, FBIOGET_VSCREENINFO, &vinfo))
    {
        printf("Error: reading variable information.\n");
        exit(3);
    } 

    xres = vinfo.xres;
    yres = vinfo.yres;
    bits_per_pixel = vinfo.bits_per_pixel;
    
    if(2!=argc)
    {
    		printf("usage: %s  <integer> #continuous circulation\n",argv[0]);
    		return 1;    		
    }
    
     times=atoi(argv[1]);
     
    //计算屏幕的总大小（字节）
     screensize = vinfo.xres * vinfo.yres * vinfo.bits_per_pixel / 8;
   
    //内存映射
	    fbp = (unsigned char *)mmap(0, screensize, PROT_READ | PROT_WRITE, MAP_SHARED, fbfd, 0);
	    if ((int)fbp == -1)
	    {
	        printf("Error: failed to map framebuffer device to memory.\n");
	        exit(4);
	    }
         
     unsigned int *buffer=(uint32_t*)malloc(xres*yres*sizeof(uint32_t));	
     printf("rotate 90 degree,read framebuffer contitue\n");  	
		
     curTime=gettime();
     //不命中，旋转90度
     rotate_bmp(buffer,fbp,xres,yres,times);
      
   
     curTime2=gettime(); 
     printf("times:%d,curTime2-curTime=%d ms\n\n",times,curTime2-curTime);
   
     
     printf("optimize \n");    
		 curTime=gettime();
     //命中，优化
     rotate_bmp_hit(buffer,fbp,xres,yres,times);  
     
     curTime2=gettime(); 
     printf("times:%d,curTime2-curTime=%d ms\n\n",times,curTime2-curTime);     
  
     printf("rotate 90 degree,write buffer continue\n");    
		 curTime=gettime();
     //写连续，优化
     write_continue(buffer,fbp,xres,yres,times);      
     curTime2=gettime(); 
     printf("times:%d,curTime2-curTime=%d ms\n",times,curTime2-curTime);   
     
     free(buffer);
     munmap(fbp, screensize);
     close(fbfd);
     return 0;
}


void write_continue(unsigned int *buffer, unsigned char *fbp,int xres,int yres, int times)
{
		int r,c;
		unsigned int *fb_ptr=(uint32_t*)fbp;
		while(times--)
		{	   	
			for(r=0;r<xres;r++)
				for(c=0;c<yres;c++)
						buffer[r*yres+c]=fb_ptr[c*xres+r];		// 写连续
		}		    
}


void rotate_bmp(unsigned int *buffer, unsigned char *fbp,int xres,int yres, int times)
{
		int i,j;
		unsigned int *fb_ptr=(uint32_t*)fbp;
		while(times--)
		{	   	
			for(j=0;j<yres;j++)
				for(i=0;i<xres;i++)
						buffer[i*yres+j]=fb_ptr[j*xres+i];		//旋转90度,读连续
		}		    
}

void rotate_bmp_hit(unsigned int *buffer, unsigned char *fbp, int xres, int yres,int times)
{
    int i,j;
    unsigned int *fb_ptr=(uint32_t*)fbp;
   // memcpy(buffer,fb_ptr,xres*yres);
    while(times--)
	  {
	  	int k=0;
			for(j=0;j<yres;j++)
				for(i=0;i<xres;i++)
				{
						buffer[k]=fb_ptr[k];    //优化
						k++;	
				}		
	  }
}

static inline int gettime(void)
{
		struct timeval t;
		int curTime;
		gettimeofday(&t, NULL);
		curTime = t.tv_sec;
    curTime *= 1000;               
    curTime += t.tv_usec / 1000;   
    return(curTime);
}

