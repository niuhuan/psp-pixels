#ifndef SJIS_10_H_
#define SJIS_10_H_


#include "../hardwares/screen.h"

/**
 * 打印日文, 输入编码为 SJIS10, 半角5x10, 全角 10x10
 *
 * @param x
 * @param y
 * @param color
 * @param str
 */
void screenDrawStringSJIS10(int x, int y, SCREEN_RGB_UNIT color, unsigned char* str);

#endif /* SJIS_10_H_ */
