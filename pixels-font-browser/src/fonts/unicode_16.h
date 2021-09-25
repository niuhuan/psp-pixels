#ifndef UNICODE_16_H_
#define UNICODE_16_H_

#include "../hardwares/screen.h"

/**
 * 打印字符, 输入编码为UCS2
 * @param x
 * @param y
 * @param color
 * @param str
 */
void screenDrawStringUSC2_16(int x, int y, SCREEN_RGB_UNIT color, unsigned short* str);

/**
 * 打印字符, 输入编码为UTF8
 * @param x
 * @param y
 * @param color
 * @param str
 */
void screenDrawStringUTF8_16(int x, int y, SCREEN_RGB_UNIT color, unsigned char* str);

#endif /* UNICODE_16_H_ */
