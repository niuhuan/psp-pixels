#include <string.h>
#include "screen.h"


/*
 * 0x44000000 显存开始
 * 0x44399999 显存结束
 * 5551 DispBuf=0x0x44000000 BackBuf=0x44044000
 * 8888 DispBuf=0x0x44000000 BackBuf=0x44088000
 */

SCREEN_RGB_UNIT *DispBuf = (SCREEN_RGB_UNIT *) (char *) 0x44000000;
SCREEN_RGB_UNIT *BackBuf = (SCREEN_RGB_UNIT *) (char *) (0x44000000 +
                                                         (sizeof(SCREEN_RGB_UNIT) * SCREEN_LINE_SIZE * SCREEN_HEIGHT));
SCREEN_RGB_UNIT *SwapBuf;

void screenInit() {
    sceDisplaySetMode(0, SCREEN_WIDTH, SCREEN_HEIGHT);
    sceDisplayWaitVblankStart();
    sceDisplaySetFrameBuf(0, 0, 0, 1);
}

void screenFlip() {
    sceDisplaySetFrameBuf(BackBuf, SCREEN_LINE_SIZE, SCREEN_PIXEL_FORMAT, PSP_DISPLAY_SETBUF_NEXTFRAME);
    SwapBuf = DispBuf;
    DispBuf = BackBuf;
    BackBuf = SwapBuf;
    sceDisplayWaitVblankStart();
}

void screenWaitForNextFrame() {
    sceDisplayWaitVblankStart();
}

SCREEN_RGB_UNIT *screenGetBackBuf() {
    return BackBuf;
}

void screenFillColor(SCREEN_RGB_UNIT color) {
    const max = (SCREEN_LINE_SIZE * SCREEN_HEIGHT);
    int i;
    for (i = 0; i < max; ++i) {
        BackBuf[i] = color;
    }
}

void screenFillRect(int x, int y, int width, int height, SCREEN_RGB_UNIT color) {
    int yOffset = 0 > y ? 0 : y;
    int xOffset = 0 > x ? 0 : x;
    int yLimit = SCREEN_HEIGHT < y + height ? SCREEN_HEIGHT : y + height;
    int xLimit = SCREEN_WIDTH < x + width ? SCREEN_WIDTH : x + width;
    int yPosition, xPosition;
    for (yPosition = yOffset; yPosition < yLimit; ++yPosition) {
        SCREEN_RGB_UNIT *row = ((SCREEN_RGB_UNIT *) BackBuf) + yPosition * SCREEN_LINE_SIZE;
        for (xPosition = xOffset; xPosition < xLimit; ++xPosition) {
            row[xPosition] = color;
        }
    }
}

void screenDrawRect(int x, int y, int width, int height, SCREEN_RGB_UNIT color) {
    int yMaxLine = y + height - 1;
    int xMaxLine = x + width - 1;
    int yOffset = 0 > y ? 0 : y;
    int xOffset = 0 > x ? 0 : x;
    int yLimit = SCREEN_HEIGHT < y + height ? SCREEN_HEIGHT : y + height;
    int xLimit = SCREEN_WIDTH < x + width ? SCREEN_WIDTH : x + width;
    int yPosition, xPosition;
    for (yPosition = yOffset; yPosition < yLimit; ++yPosition) {
        SCREEN_RGB_UNIT *row = BackBuf + yPosition * SCREEN_LINE_SIZE;
        for (xPosition = xOffset; xPosition < xLimit; ++xPosition) {
            if (xPosition == x || xPosition == xMaxLine || yPosition == y || yPosition == yMaxLine)
                row[xPosition] = color;
        }
    }
}

void screenFillImage(int x, int y, int width, int height, SCREEN_RGB_UNIT *buff) {
    int yOffset = 0 > y ? 0 : y;
    int xOffset = 0 > x ? 0 : x;
    int yLimit = SCREEN_HEIGHT < y + height ? SCREEN_HEIGHT : y + height;
    int xLimit = SCREEN_WIDTH < x + width ? SCREEN_WIDTH : x + width;
    int yPosition, xPosition;
    for (yPosition = yOffset; yPosition < yLimit; ++yPosition) {
        SCREEN_RGB_UNIT *row = ((SCREEN_RGB_UNIT *) BackBuf) + yPosition * SCREEN_LINE_SIZE;
        int buffY = yPosition - y;
        for (xPosition = xOffset; xPosition < xLimit; ++xPosition) {
            int buffX = xPosition - x;
            row[xPosition] = buff[buffY * width + buffX];
        }
    }
}


