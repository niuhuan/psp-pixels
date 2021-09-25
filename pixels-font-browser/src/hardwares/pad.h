#ifndef PAD_H_
#define PAD_H_

#include <pspctrl.h>

/**
 * 初始化按键
 */
void padInit();

/**
 * 读取按键信息
 *
 * @return
 */
SceCtrlData padReadAll();

/**
 * 等待一个按键被按下
 * @return
 */
int padManagerButtonsWait();

#endif /* PAD_H_ */
