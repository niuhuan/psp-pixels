#include "sjis_10.h"
#include "sjis_10_font_hankaku.c"
#include "sjis_10_font_zenkaku.c"

// by kwn
void screenDrawChar10hankaku(int x, int y, SCREEN_RGB_UNIT color, unsigned char ch) {
    unsigned char c = ch;
    // mapping
    if (c < 0x20)
        c = 0;
    else if (c < 0x80)
        c -= 0x20;
    else if (c < 0xa0)
        c = 0;
    else
        c -= 0x40;
    int bitPositonY, bitPositonX;
    const unsigned char *characterLine;
    SCREEN_RGB_UNIT *screenLine;
    for (bitPositonY = 0; bitPositonY < 10; bitPositonY++) {
        if (y + bitPositonY >= 0 && y + bitPositonY < SCREEN_HEIGHT) {
            characterLine = font10hankaku + c * 10 + bitPositonY;
            screenLine = screenGetBackBuf() + (y + bitPositonY) * SCREEN_LINE_SIZE;
            for (bitPositonX = 0; bitPositonX < 5; bitPositonX++) {
                if (x + bitPositonX >= 0 && x + bitPositonX < SCREEN_WIDTH) {
                    if ((1 << bitPositonX) & *characterLine) {
                        screenLine[x + bitPositonX] = color;
                    }
                }
            }
        }
    }
}

// by kwn
void screenDrawChar10zenkaku(int x, int y, SCREEN_RGB_UNIT color, const unsigned char u, unsigned char d) {
    // ELISA100.FNTに存在しない文字
    const unsigned short font404[] = { 0xA2AF, 11, 0xA2C2, 8, 0xA2D1, 11, 0xA2EB, 7, 0xA2FA, 4, 0xA3A1, 15, 0xA3BA, 7, 0xA3DB, 6, 0xA3FB, 4, 0xA4F4, 11, 0xA5F7, 8, 0xA6B9, 8, 0xA6D9, 38, 0xA7C2, 15, 0xA7F2, 13, 0xA8C1, 720, 0xCFD4, 43, 0xF4A5, 1030, 0, 0 };
    unsigned short *cfont;
    unsigned long n;
    unsigned short code;
    int i;
    // SJISコードの生成
    code = u;
    code = (code << 8) + d;
    // SJISからEUCに変換
    if (code >= 0xE000)
        code -= 0x4000;
    code = ((((code >> 8) & 0xFF) - 0x81) << 9) + (code & 0x00FF);
    if ((code & 0x00FF) >= 0x80)
        code--;
    if ((code & 0x00FF) >= 0x9E)
        code += 0x62;
    else
        code -= 0x40;
    code += 0x2121 + 0x8080;
    // EUCから恵梨沙フォントの番号を生成
    n = (((code >> 8) & 0xFF) - 0xA1) * (0xFF - 0xA1) + (code & 0xFF) - 0xA1;
    i = 0;
    while (font404[i]) {
        if (code >= font404[i]) {
            if (code <= font404[i] + font404[i + 1] - 1) {
                n = -1;
                break;
            } else {
                n -= font404[i + 1];
            }
        }
        i += 2;
    }
    cfont = (unsigned short *) &font10zenkaku[n * 10];
    // 绘制到屏幕
    int bitPositonY, bitPositonX;
    unsigned short *characterLine;
    SCREEN_RGB_UNIT* screenLine;
    for (bitPositonY = 0; bitPositonY < 10; bitPositonY++) {
        if (y + bitPositonY >= 0 && y + bitPositonY < SCREEN_HEIGHT) {
            characterLine = cfont + bitPositonY;
            screenLine = screenGetBackBuf() + (y + bitPositonY) * SCREEN_LINE_SIZE;
            for (bitPositonX = 0; bitPositonX < 10; bitPositonX++) {
                if (x + bitPositonX >= 0 && x + bitPositonX < SCREEN_WIDTH) {
                    if ((1 << bitPositonX) & *characterLine) {
                        screenLine[x + bitPositonX] = color;
                    }
                }
            }
        }
    }
}

void screenDrawStringSJIS10(int x, int y, SCREEN_RGB_UNIT color, unsigned char* str) {
    const unsigned char fontHeight = 10;
    const unsigned char fontWidthHankaku = 5;
    const unsigned char fontWidthZenkaku = 10;
    int xOffset = x;
    int yOffset = y;
    unsigned char ch = 0, bef = 0;
    while (*str != 0) {
        ch = *str++;
        if (bef != 0) {
            if (xOffset < SCREEN_WIDTH && yOffset < SCREEN_HEIGHT)
                screenDrawChar10zenkaku(xOffset, yOffset, color, bef, ch);
            xOffset += fontWidthZenkaku;
        } else {
            if (((ch >= 0x80) && (ch < 0xa0)) || (ch >= 0xe0)) {
                bef = ch;
            } else {
                if (ch == '\n') {
                    xOffset = x;
                    yOffset += fontHeight;
                } else {
                    if (xOffset < SCREEN_WIDTH && yOffset < SCREEN_HEIGHT)
                        screenDrawChar10hankaku(xOffset, yOffset, color, ch);
                    xOffset += fontWidthHankaku;
                }
            }
        }
    }
}

