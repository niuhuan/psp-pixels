#include <string.h>
#include "../hardwares/pad.h"
#include "viewer.h"

#include "load_bmp.c"
#include "load_jpeg.c"
#include "load_png.c"
#include "stretcher.c"

char viewerShowImage(char* filePath) {
    /* 文件路径相关 */
    char *p, fileName[1024], fileExt[128];
    // 获取文件名
    p = strrchr(filePath, '/');
    if (!p) {
        return 0;
    }
    strcpy(fileName, ++p);
    // 获取拓展名
    p = strrchr(fileName, '.');
    if (!p) {
        return 0;
    }
    strcpy(fileExt, ++p);
    // 判断是否是可识别图片格式,并且加载图片
    int imageWidth, imageHeight;
    SCREEN_RGB_UNIT *imageData;
    if (!strcmp("BMP", fileExt) || !strcmp("bmp", fileExt)) {
        if (!viewerLoadBMP(filePath, &imageWidth, &imageHeight, &imageData)) {
            return 0;
        }
    } else if (!strcmp("JPG", fileExt) || !strcmp("jpg", fileExt) || !strcmp("JPEG", fileExt) || !strcmp("jpeg", fileExt)) {
        if (!viewerLoadJPEG(filePath, &imageWidth, &imageHeight, &imageData)) {
            return 0;
        }
    } else if (!strcmp("PNG", fileExt) || !strcmp("png", fileExt)) {
        if (!viewerLoadPNG(filePath, &imageWidth, &imageHeight, &imageData)) {
            return 0;
        }
    } else {
        return 0;
    }
    int showWidth, showHeight;
    SCREEN_RGB_UNIT *showData;
    if (imageWidth > SCREEN_WIDTH || imageHeight > SCREEN_HEIGHT) {
        showWidth = SCREEN_WIDTH;
        showHeight = SCREEN_HEIGHT;
        showData = (SCREEN_RGB_UNIT *) do_Stretch_Linear_Ratio((unsigned char *) imageData, imageWidth, imageHeight, &showWidth, &showHeight, sizeof(SCREEN_RGB_UNIT) * 8);
    } else {
        showWidth = imageWidth;
        showHeight = imageHeight;
        showData = (SCREEN_RGB_UNIT *) malloc(sizeof(SCREEN_RGB_UNIT) * showWidth * showHeight);
        memcpy(showData, imageData, sizeof(SCREEN_RGB_UNIT) * showWidth * showHeight);
    }
    screenFillColor(0);
    screenFillImage((SCREEN_WIDTH - showWidth) / 2, (SCREEN_HEIGHT - showHeight) / 2, showWidth, showHeight, showData);
    screenFlip();
    free(imageData);
    free(showData);
    padManagerButtonsWait();
    return 1;
}
