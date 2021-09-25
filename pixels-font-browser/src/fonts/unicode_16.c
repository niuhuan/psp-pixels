#include <stdio.h>
#include "unicode_0.h"
#include "unicode_16.h"

void screenDrawStringUSC2_16(int x, int y, SCREEN_RGB_UNIT color, unsigned short* str) {
    FILE *fp = fopen("UNI16.FNT", "rb");
    if (!fp) {
        return;
    }
    int charOffsetX = x;
    int charOffsetY = y;
    unsigned short *ucs2_ppoint = str;
    while (*ucs2_ppoint) {
        if (*ucs2_ppoint == 10) {
            charOffsetX = x;
            charOffsetY += 17;
            ucs2_ppoint += 1;
            continue;
        }
        unsigned short fd[16];
        fseek(fp, *ucs2_ppoint * 16 * sizeof(unsigned short), SEEK_SET);
        fread(fd, sizeof(unsigned short), 16, fp);
        unsigned int bitPositionY, bitPositionX;
        for (bitPositionY = 0; bitPositionY < 16; ++bitPositionY) {
            SCREEN_RGB_UNIT* row = screenGetBackBuf() + (charOffsetY + bitPositionY) * SCREEN_LINE_SIZE;
            for (bitPositionX = 0; bitPositionX < 16; ++bitPositionX) {
                if (charOffsetX + bitPositionX >= 0 && charOffsetX + bitPositionX < SCREEN_WIDTH && charOffsetY + bitPositionY >= 0 && charOffsetY + bitPositionY) {
                    if (1 << bitPositionX & fd[bitPositionY]) {
                        row[charOffsetX + bitPositionX] = color;
                    }
                }
            }
        }
        if ((0x0000 <= *ucs2_ppoint && *ucs2_ppoint <= 0x00FF) || (0xFF61 <= *ucs2_ppoint && *ucs2_ppoint <= 0xFF9F) || (0xFFE8 <= *ucs2_ppoint && *ucs2_ppoint <= 0xFFEE))
            charOffsetX += 8;
        else
            charOffsetX += 16;
        ucs2_ppoint += 1;
    }
    fclose(fp);
}

void screenDrawStringUTF8_16(int x, int y, SCREEN_RGB_UNIT color, unsigned char* str) {
    unsigned short ucs2_str[200];
    UTF8toUCS2(str, ucs2_str);
    screenDrawStringUSC2_16(x, y, color, ucs2_str);
}

