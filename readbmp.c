#include "readbmp.h"

#define BM 19778 //bmp文件的类型

/* To get the width and height of the bmp file */
void getWidthAndHeight(FILE* fpbmp, long *width, long *height)
{
     fseek(fpbmp, 18L, SEEK_SET);
     fread(width, sizeof(char), 4, fpbmp);
     fread(height, sizeof(char), 4, fpbmp);
}

/* To get the OffSet of header to data part */
unsigned int bmpHeaderPartLength(FILE* fpbmp)
{
    unsigned int offSet = 0;
    fseek(fpbmp, 10L, SEEK_SET);
    fread(&offSet, sizeof(char), 4, fpbmp);

    return offSet;
}

/* read a bmp file */
void readBMP(const char *bmpFile)
{
    int i, j;
    unsigned char *bmpPixelTmp = NULL;
    long width = 0;
    long height = 0;
    unsigned int dataOffset = 0;
    int addZero = 0;  //8bit bmp图片宽度必须为4的倍数，否则每行末尾补0
    FILE *fpbmp;
    short fileType = 0;

    fpbmp = fopen(bmpFile, "rb");
    if(NULL == fpbmp)
    {
        printf("failed to open %s\n", bmpFile);
        return;
    }
    //判断是否是bmp文件
    fseek(fpbmp, 0L, SEEK_SET);
    fread(&fileType, sizeof(short), 1, fpbmp);
    if(BM != fileType)
    {
        printf("%s is not a bmp file\n", bmpFile);
        return;
    }

    getWidthAndHeight(fpbmp, &width, &height); //获取图片高度和宽度
    //宽度不为4的倍数则计算补零个数
    if(0 != width % 4)
    {
        addZero = 4 - width % 4;
    }
    /* get the OffSet of header to data part */
    dataOffset = bmpHeaderPartLength(fpbmp);
    /* 获取图片位数，本例只读8bit */
    unsigned short biBitCount = 0;
    fseek(fpbmp, 28L, SEEK_SET);
    fread(&biBitCount, sizeof(char), 2, fpbmp);
    if(8 != biBitCount)
    {
        printf("%s is not a 8 gray image", bmpFile);
    }
    //存图片数据的数组
    unsigned char bmpPixel[height][width];
    //读取图片数据的临时变量
    bmpPixelTmp = (unsigned char*)malloc(sizeof(char)*(width+addZero)*height);
    if (NULL == bmpPixelTmp)
    {
        fprintf(stderr, "Data allocation failed.!!!\n");
        exit(1);
    }
    //fread可能无法一次读完
    int readCount = 0;
    while(sizeof(char) * width * height - readCount)
    {
        fseek(fpbmp, dataOffset + readCount, SEEK_SET);
        readCount += fread((bmpPixelTmp + readCount), sizeof(char), (sizeof(char) * width * height - readCount), fpbmp);
    }
    //存入数组中
    for(i = 0; i < height; i++)
    {
        for(j = 0; j < width; j++)
        {
            bmpPixel[i][j] = bmpPixelTmp[(width+addZero)*(height-1-i)+j];
        }
    }


/* 打开注释可将数组打印到文件上,用于测试，可用matlab直接打开bmp文件进行验证 */
  FILE *fpLog;
    fpLog = fopen("log.txt", "w+");
    char tmpLog[5] = {0};

    for(i = 0; i < height; i++)
    {
        for(j = 0; j < width; j++)
        {
            sprintf(tmpLog, "%3d ", bmpPixel[i][j]);

            fputs(tmpLog, fpLog);
        }
        fputs("\n", fpLog);
     }

    free(bmpPixelTmp);
}
