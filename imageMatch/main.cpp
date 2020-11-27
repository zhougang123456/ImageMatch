#include <iostream>
#include "windows.h"
#include "ImageMatch.hpp"
#include <time.h>

#define IMAGE_HEIGHT_MAX 1080

static inline uint64_t get_time()
{
    time_t clock;
    timeval now;
    struct tm tm;
    SYSTEMTIME wtm;
    GetLocalTime(&wtm);
    tm.tm_year = wtm.wYear - 1900;
    tm.tm_mon = wtm.wMonth - 1;
    tm.tm_mday = wtm.wDay;
    tm.tm_hour = wtm.wHour;
    tm.tm_min = wtm.wMinute;
    tm.tm_sec = wtm.wSecond;
    tm.tm_isdst = -1;
    clock = mktime(&tm);
    now.tv_sec = clock;
    now.tv_usec = wtm.wMilliseconds * 1000;
    return (uint64_t)now.tv_sec * 1000000 + (uint64_t)now.tv_usec;
}

unsigned char* readBmp()
{
    char file_str[200];
    static int bmp_id = 0;
    sprintf(file_str, "d:\\tmp\\%u.bmp", ++bmp_id);
    FILE* fp;
    if ((fp = fopen(file_str, "rb")) == NULL)  //以二进制的方式打开文件
    {
        return NULL;
    }
    if (fseek(fp, sizeof(BITMAPFILEHEADER), 0))  //跳过BITMAPFILEHEADE
    {
        return NULL;
    }
    BITMAPINFOHEADER infoHead;
    fread(&infoHead, sizeof(BITMAPINFOHEADER), 1, fp);   //从fp中读取BITMAPINFOHEADER信息到infoHead中,同时fp的指针移动
    int bmpwidth = infoHead.biWidth;

    int bmpheight = infoHead.biHeight;

    if (bmpheight < 0) {
        bmpheight *= -1;
    }
    int linebyte = bmpwidth * 4; //计算每行的字节数，24：该图片是24位的bmp图，3：确保不丢失像素

    unsigned char* pBmpBuf = new unsigned char[linebyte * bmpheight];

    fread(pBmpBuf, sizeof(char), linebyte * bmpheight, fp);
    fclose(fp);   //关闭文件
    return pBmpBuf;
}

int main()
{
    ImageMatch* match = new ImageMatch(IMAGE_HEIGHT_MAX);
    int i = 1;
    while (true) {
        unsigned char* img1 = readBmp();
        unsigned char* img2 = readBmp();
        if (img1 == NULL || img2 == NULL) {
            printf("img is null\n");
            return 0;
        }
        INT32 start = (INT32)get_time() / 1000;
        int vector = match->do_match((Pixel32Bit*)img1, 0, 0, 1920, 1080, (Pixel32Bit*)img2, 0, 0, 1920, 1080);
        INT32 end = (INT32)get_time() / 1000;
        printf("match image %d %d, match vector is %d cost time %d\n", i, i + 1, vector, end - start);
        i += 2;
        Sleep(2);
    }
}