#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <linux/fb.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <arpa/inet.h>

//display 24bit bmp
//14byte文件头
typedef struct
{
    char cfType[2];//文件类型，"BM"(0x4D42)         
    long cfSize;//文件大小（字节）         
    long cfReserved;//保留，值为0     
    long cfoffBits;//数据区相对于文件头的偏移量（字节）    
}__attribute__((packed)) BITMAPFILEHEADER;

////__attribute__((packed))的作用是告诉编译器取消结构在编译过程中的优化对齐，按照实际占用字节数进行对齐
////40byte信息头
//typedef struct
//{
//    char ciSize[4];//BITMAPFILEHEADER所占的字节数
//    long ciWidth;//       
//    long ciHeight;//       
//    char ciPlanes[2];//目标设备的位平面数，值为1
//    int ciBitCount;//每个像素的位数
//    char ciCompress[4];//压缩说明
//    char ciSizeImage[4];//用字节表示的图像大小，该数据必须是4的倍数    
//    char ciXPelsPerMeter[4];//目标设备的水平像素数/米
//    char ciYPelsPerMeter[4];//目标设备的垂直像素数/米
//    char ciClrUsed[4]; //位图使用调色板的颜色数   
//    char ciClrImportant[4]; //指定重要的颜色数，当该域的值等于颜色数时（或者等于0时），表示所有颜色都一样重要
//}__attribute__((packed)) BITMAPINFOHEADER;

typedef unsigned long DWORD;
typedef unsigned short WORD;
typedef long LONG;

typedef struct tagBITMAPINFOHEADER{
    DWORD      ciSize;
    LONG       ciWidth;
    LONG       ciHeight;
    WORD       ciPlanes;
    WORD       ciBitCount;
    DWORD      ciCompression;
    DWORD      ciSizeImage;
    LONG       ciXPelsPerMeter;
    LONG       ciYPelsPerMeter;
    DWORD      ciClrUsed;
    DWORD      ciClrImportant;
} __attribute__((packed)) BITMAPINFOHEADER;



typedef struct
{
    unsigned char blue;
    unsigned char green;
    unsigned char red;
}__attribute__((packed)) PIXEL;//颜色模式，RGBA

BITMAPFILEHEADER FileHead;
BITMAPINFOHEADER InfoHead;

static char *fbp = 0;
static int xres = 0;
static int yres = 0;
static int bits_per_pixel = 0;

int show_bmp(const char* bmppath);

int main ( int argc, char *argv[] )
{
    int fbfd = 0;
    struct fb_var_screeninfo vinfo;
    struct fb_fix_screeninfo finfo;
    uint32_t screensize = 0;

	fprintf(stderr,"This program was compiled at %s %s\n",__DATE__,__TIME__);
	fprintf(stderr,"Author: bergtian@gmail.com\n");

    if(argc != 2){
        printf("usage: %s filename\n",argv[0]);
        return 1;
    }
     
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
    printf("finfo: smem_len=%d\n",finfo.smem_len);
    printf("finfo: id=%s\n",finfo.id);


    if (ioctl(fbfd, FBIOGET_VSCREENINFO, &vinfo))
    {
        printf("Error: reading variable information.\n");
        exit(3);
    }

    printf("vinfo: %d x %d, %dbpp\n", vinfo.xres, vinfo.yres, vinfo.bits_per_pixel );
    printf("vinfo: virtual %d x %d\n", vinfo.xres_virtual, vinfo.yres_virtual);
    printf("vinfo: left_margin %d;right_margin %d\n", vinfo.left_margin, vinfo.right_margin);
    printf("vinfo: upper_margin %d;lower_margin %d\n", vinfo.upper_margin, vinfo.lower_margin);
    printf("vinfo: hsync_len %d;vsync_len %d\n", vinfo.hsync_len, vinfo.vsync_len);

    xres = vinfo.xres;
    yres = vinfo.yres;
    bits_per_pixel = vinfo.bits_per_pixel;

    //计算屏幕的总大小（字节）
    screensize = vinfo.xres * vinfo.yres * vinfo.bits_per_pixel / 8;
    printf("screensize=%u\n",screensize);
 

    //内存映射
    fbp = (char *)mmap(0, screensize, PROT_READ | PROT_WRITE, MAP_SHARED, fbfd, 0);
    if ((int)fbp == -1)
    {
        printf("Error: failed to map framebuffer device to memory.\n");
        exit(4);
    }
    
    show_bmp(argv[1]);

    usleep(20*1000*1000);

    munmap(fbp, screensize);
    close(fbfd);
    return 0;
}

int show_bmp(const char* bmppath)
{
    FILE *fp;
    int rc;
    int line_x, line_y;
    long int location = 0, BytesPerLine = 0;
    //char tmp[1024*10];

    printf("\nsizeof header=%d\n", sizeof(BITMAPFILEHEADER));
    printf("into show_bmp function\n");


    fp = fopen( bmppath, "rb" );
    if (fp == NULL)
    {
        return( -1 );
    }  

    rc = fread( &FileHead, sizeof(BITMAPFILEHEADER),1, fp );
    if ( rc != 1)
    {
        printf("read header error!\n");
        fclose( fp );
        return( -2 );
    }

    if (memcmp(FileHead.cfType, "BM", 2) != 0)
    {
        printf("it's not a BMP file\n");
        fclose( fp );
        return( -3 );
    }

    rc = fread( (char *)&InfoHead, sizeof(BITMAPINFOHEADER),1, fp );
    if ( rc != 1) {
        printf("read infoheader error!\n");
        fclose( fp );
        return( -4 );
    }

    printf("ciSize:%u\n",((uint32_t)(InfoHead.ciSize)));
    printf("ciWidth:%u\n",((uint32_t)(InfoHead.ciWidth)));
    printf("ciHeight:%u\n",((uint32_t)(InfoHead.ciHeight)));
    printf("ciPlanes:%u\n",((uint32_t)(InfoHead.ciPlanes)));
    printf("ciBitCount:%u\n",((uint32_t)(InfoHead.ciBitCount)));
    printf("ciCompression:%u\n",((uint32_t)(InfoHead.ciCompression)));
    printf("ciSizeImage:%u\n",((uint32_t)(InfoHead.ciSizeImage)));
    printf("ciXPelsPerMeter:%u\n",((uint32_t)(InfoHead.ciXPelsPerMeter)));
    printf("ciYPelsPerMeter:%u\n",((uint32_t)(InfoHead.ciYPelsPerMeter)));
    printf("ciClrUsed:%u\n",((uint32_t)(InfoHead.ciClrUsed)));
    printf("ciClrImportant:%u\n",((uint32_t)(InfoHead.ciClrImportant)));


    //跳转的数据区
    fseek(fp, FileHead.cfoffBits, SEEK_SET);
    //每行字节数
    BytesPerLine = (InfoHead.ciWidth * InfoHead.ciBitCount + 31) / 32 * 4;    

    line_x = line_y = 0;
    //向framebuffer中写BMP图片
    while(!feof(fp))
    {
        PIXEL pix;
        uint32_t tmp = 0;
        rc = fread( (char *)&pix, 1, sizeof(PIXEL), fp);
        if (rc != sizeof(PIXEL)){ 
            break; 
        }
        location = line_x * bits_per_pixel / 8 + (InfoHead.ciHeight - line_y - 1) * xres * bits_per_pixel / 8;

        //显示每一个像素
        tmp=pix.red<<0 | pix.green<<8 | pix.blue<<16;
        *((uint32_t*)(fbp + location)) = tmp;

        line_x++;
        if (line_x == InfoHead.ciWidth )
        {
            line_x = 0;
            line_y++;

            if(line_y == InfoHead.ciHeight)
            {
                break;
            }
        }
    }
    fclose( fp );

    
    return( 0 );
}