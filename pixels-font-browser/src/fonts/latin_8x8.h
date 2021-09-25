#ifndef LATIN_8X8_H_
#define LATIN_8X8_H_

#define LATIN_8X8_FONT_HEIGHT 8
#define LATIN_8X8_FONT_WIDTH 8

#include "../hardwares/screen.h"

/**
 * 打印拉丁文(英语法语等) 文字大小为8x8
 *
 * @param x
 * @param y
 * @param color
 * @param str
 */
void screenDrawStringLatin8x8(int x, int y, SCREEN_RGB_UNIT color, unsigned char *str);

#endif /* LATIN_8X8_H_ */
