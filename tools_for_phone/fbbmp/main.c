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
//14byte�ļ�ͷ
typedef struct
{
    char cfType[2];//�ļ����ͣ�"BM"(0x4D42)         
    long cfSize;//�ļ���С���ֽڣ�         
    long cfReserved;//������ֵΪ0     
    long cfoffBits;//������������ļ�ͷ��ƫ�������ֽڣ�    
}__attribute__((packed)) BITMAPFILEHEADER;

////__attribute__((packed))�������Ǹ��߱�����ȡ���ṹ�ڱ�������е��Ż����룬����ʵ��ռ���ֽ������ж���
////40byte��Ϣͷ
//typedef struct
//{
//    char ciSize[4];//BITMAPFILEHEADER��ռ���ֽ���
//    long ciWidth;//       
//    long ciHeight;//       
//    char ciPlanes[2];//Ŀ���豸��λƽ������ֵΪ1
//    int ciBitCount;//ÿ�����ص�λ��
//    char ciCompress[4];//ѹ��˵��
//    char ciSizeImage[4];//���ֽڱ�ʾ��ͼ���С�������ݱ�����4�ı���    
//    char ciXPelsPerMeter[4];//Ŀ���豸��ˮƽ������/��
//    char ciYPelsPerMeter[4];//Ŀ���豸�Ĵ�ֱ������/��
//    char ciClrUsed[4]; //λͼʹ�õ�ɫ�����ɫ��   
//    char ciClrImportant[4]; //ָ����Ҫ����ɫ�����������ֵ������ɫ��ʱ�����ߵ���0ʱ������ʾ������ɫ��һ����Ҫ
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
}__attribute__((packed)) PIXEL;//��ɫģʽ��RGBA

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
     
    //����ʾ�豸
    fbfd = open("/dev/graphics/fb0", O_RDWR);
    if (!fbfd)
    {
        printf("Error: cannot open framebuffer device.\n");
        exit(1);
    }

    if (ioctl(fbfd, FBIOGET_FSCREENINFO, &finfo))
    {
        printf("Error��reading fixed information.\n");
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

    //������Ļ���ܴ�С���ֽڣ�
    screensize = vinfo.xres * vinfo.yres * vinfo.bits_per_pixel / 8;
    printf("screensize=%u\n",screensize);
 

    //�ڴ�ӳ��
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


    //��ת��������
    fseek(fp, FileHead.cfoffBits, SEEK_SET);
    //ÿ���ֽ���
    BytesPerLine = (InfoHead.ciWidth * InfoHead.ciBitCount + 31) / 32 * 4;    

    line_x = line_y = 0;
    //��framebuffer��дBMPͼƬ
    while(!feof(fp))
    {
        PIXEL pix;
        uint32_t tmp = 0;
        rc = fread( (char *)&pix, 1, sizeof(PIXEL), fp);
        if (rc != sizeof(PIXEL)){ 
            break; 
        }
        location = line_x * bits_per_pixel / 8 + (InfoHead.ciHeight - line_y - 1) * xres * bits_per_pixel / 8;

        //��ʾÿһ������
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