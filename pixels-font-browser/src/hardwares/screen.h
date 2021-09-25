#ifndef PIXELS_FONT_BROWSER_SCREEN_H
#define PIXELS_FONT_BROWSER_SCREEN_H

#define SCREEN_WIDTH  480
#define SCREEN_HEIGHT 272
#define SCREEN_LINE_SIZE 512

#include <pspdisplay.h>

/* 屏幕的色彩模式 */

// 8888应用更为广泛, 色彩数更多

// RGBA 8888
#define SCREEN_PIXEL_FORMAT PSP_DISPLAY_PIXEL_FORMAT_8888
#define SCREEN_RGB_UNIT int
#define SCREEN_RGB(r, g, b) ((r)|((g)<<8)|((b)<<16))

/*
// 5551
#define SCREEN_PIXEL_FORMAT PSP_DISPLAY_PIXEL_FORMAT_5551
#define SCREEN_RGB_UNIT short
#define SCREEN_RGB(r,g,b) ((((b>>3) & 0x1F)<<10)|(((g>>3) & 0x1F)<<5)|(((r>>3) & 0x1F)<<0)|0x8000)
*/

/**
 * 初始化显示
 */
void screenInit();

/**
 * 显示翻转（显示绘制好的图形）
 */
void screenFlip();

/**
 * 等待屏幕绘制完成下一帧
 */
void screenWaitForNextFrame();

/* 以下操作flip之后才能显示在屏幕上 */

/**
 * 获取显示内存区 (比如打印文字, 直接操作内存会更快一些)
 *
 * @return
 */
SCREEN_RGB_UNIT *screenGetBackBuf();

/**
 * 将屏幕填充成一个颜色
 *
 * @param color
 */
void screenFillColor(SCREEN_RGB_UNIT color);

/**
 * 在屏幕上绘制一个矩形并填充
 *
 * @param x
 * @param y
 * @param width
 * @param height
 * @param color
 */
void screenFillRect(int x, int y, int width, int height, SCREEN_RGB_UNIT color);

/**
 * 在屏幕上绘制一个矩形, 空心
 * @param x
 * @param y
 * @param width
 * @param height
 * @param color
 */
void screenDrawRect(int x, int y, int width, int height, SCREEN_RGB_UNIT color);

/**
 * 在屏幕中绘制一个图片
 *
 * @param x
 * @param y
 * @param width
 * @param height
 * @param buff
 */
void screenFillImage(int x, int y, int width, int height, SCREEN_RGB_UNIT *buff);

#endif //PIXELS_FONT_BROWSER_SCREEN_H
