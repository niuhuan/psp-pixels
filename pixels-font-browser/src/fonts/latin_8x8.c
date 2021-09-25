#include "latin_8x8.h"
#include "latin_8x8_font.c"

/* 每个字符占8个字节 */
void screenDrawCharLatin8x8(int x, int y, SCREEN_RGB_UNIT color, unsigned char ch) {
    int bitPositionY, bitPositionX;
    const unsigned char *characterLine;
    SCREEN_RGB_UNIT *screenLine;
    for (bitPositionY = 0; bitPositionY < 8; bitPositionY++) {
        if ((y + bitPositionY >= 0) && (y + bitPositionY < SCREEN_HEIGHT)) {       // 只处理屏幕之内的像素
            characterLine = latin_8x8_font + ch * 8 + bitPositionY;
            screenLine = screenGetBackBuf() + (y + bitPositionY) * SCREEN_LINE_SIZE;
            for (bitPositionX = 0; bitPositionX < 8; bitPositionX++) {
                if (x + bitPositionX >= 0 && x + bitPositionX < SCREEN_WIDTH) {   // 只处理屏幕之内的像素
                    if (((0x80 >> bitPositionX) & *characterLine)) {
                        screenLine[x + bitPositionX] = color;
                    }
                }
            }
        }
    }
}

void screenDrawStringLatin8x8(int x, int y, SCREEN_RGB_UNIT color, unsigned char *str) {
    int offsetX = 0;
    int offsetY = 0;
    while (*str) {
        if (*str == 10) {
            offsetX = 0;
            offsetY++;
        } else
            screenDrawCharLatin8x8(x + (offsetX++) * LATIN_8X8_FONT_WIDTH, y + (offsetY * LATIN_8X8_FONT_HEIGHT), color,
                                   *str);
        str++;
    }
}
