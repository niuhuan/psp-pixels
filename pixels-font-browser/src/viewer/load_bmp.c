#include <stdio.h>
#include <malloc.h>
#include <psptypes.h>

typedef u8  BMP_BYTE;                    // 定义BYTE为一个字节的类型
typedef u16 BMP_WORD;                    // 定义WORD为两个字节的类型
typedef u32 BMP_DWORD;                   // 定义DWORD为四个字节的类型

/* BM 开始 */
#define VIEWER_BMP_HEADER 0x4d42

/* 文件头 */
typedef struct VIEWER_BMP_FILE_HEADER_STRUCT {
    BMP_DWORD bSize;                                     // 文件的大小
    BMP_WORD bReserved1;                                 // 保留值,必须设置为0
    BMP_WORD bReserved2;                                 // 保留值,必须设置为0
    BMP_DWORD bOffset;                                   // 图像数据位开始的偏移量
} VIEWER_BMP_FILE_HEADER;                                // 14 字节

/* 信息部分 */
typedef struct VIEWER_BMP_INFO_STRUCT {
    BMP_DWORD bInfoSize;                                 // 信息头的大小
    BMP_DWORD bWidth;                                    // 图像的宽度
    BMP_DWORD bHeight;                                   // 图像的高度
    BMP_WORD bPlanes;                                    // 图像的位面数
    BMP_WORD bBitCount;                                  // 每个像素的位数
    BMP_DWORD bCompression;                              // 压缩类型
    BMP_DWORD bmpImageSize;                              // 图像的大小,以字节为单位
    BMP_DWORD bXPelsPerMeter;                            // 水平分辨率
    BMP_DWORD bYPelsPerMeter;                            // 垂直分辨率
    BMP_DWORD bClrUsed;                                  // 使用的色彩数
    BMP_DWORD bClrImportant;                             // 重要的颜色数
} VIEWER_BMP_INFO;                                       // 40 字节

char viewerLoadBMP(char* filePath, int* imageWidth, int* imageHeight, SCREEN_RGB_UNIT** imageData) {
    BMP_WORD bType;
    VIEWER_BMP_FILE_HEADER bmpFileHeader;
    VIEWER_BMP_INFO bmpInfo;
    FILE *fp;
    if ((fp = fopen(filePath, "rb")) == NULL ) {
        return 0;
    }
    fseek(fp, 0, SEEK_END);
    long size = ftell(fp);
    rewind(fp);
    if (size < 54) {
        fclose(fp);
        return 0;
    }
    fread(&bType, 2, 1, fp);
    fread(&bmpFileHeader, 12, 1, fp);
    fread(&bmpInfo, 40, 1, fp);
    if (bType != VIEWER_BMP_HEADER                                      // 不是BMP
    || bmpFileHeader.bSize > VIEWER_MAX_IMAGE_DATA                      // 图片太大
    || bmpFileHeader.bSize != size                                      // 受损图片
    || bmpFileHeader.bOffset != 0x36                                    // 不支持的格式
    || bmpInfo.bBitCount != 24                                          // 无法处理的色深
    || bmpInfo.bmpImageSize != bmpInfo.bWidth * bmpInfo.bHeight * 3     // 无法处理的色深
            ) {
        fclose(fp);
        return 0;
    }
    /* 读取BMP */
    int x, y;
    SCREEN_RGB_UNIT *line;
    BMP_BYTE buf[3];
    *imageWidth = bmpInfo.bWidth;
    *imageHeight = bmpInfo.bHeight;
    *imageData = (SCREEN_RGB_UNIT *) malloc(sizeof(SCREEN_RGB_UNIT) * *imageWidth * *imageHeight);
    for (y = 0; y < *imageHeight; y++) {
        line = *imageData + (*imageWidth * (*imageHeight - 1 - y));
        for (x = 0; x < *imageWidth; x++) {
            fread(buf, 3, 1, fp);
            line[x] = SCREEN_RGB(buf[2],buf[1],buf[0]);
        }
    }
    fclose(fp);
    return 1;
}
