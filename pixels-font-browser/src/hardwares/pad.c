#include "pad.h"

SceCtrlData padData;

void padInit() {
    sceCtrlSetSamplingCycle(0);
    sceCtrlSetSamplingMode(PSP_CTRL_MODE_ANALOG);
    // PSP_CTRL_MODE_DIGITAL PSP_CTRL_MODE_ANALOG
}

SceCtrlData padReadAll() {
    sceCtrlReadBufferPositive(&padData, 1);
    return padData;
}

unsigned int pad_mb_old_buttons = 0;                           // 记录上一次的按键
unsigned int pad_mb_old_typed_timestamp = 0;                   // 记录上一次的按键 按下的时间
unsigned int pad_mb_old_action_timestamp = 0;                  // 记录上一次的按键 发挥作用时的时间

/* 清除按键信息 */
void padManagerButtonsClean() {
    if (pad_mb_old_buttons != 0) {
        pad_mb_old_buttons = 0;
        pad_mb_old_typed_timestamp = 0;
        pad_mb_old_action_timestamp = 0;
    }
}

/* 文件管理器等菜单界面使用 */
int padManagerButtonsWait() {
    unsigned int currentButton = 0;                            // 本次的按键
    while (1) {
        SceCtrlData pad = padReadAll();
        currentButton = pad.Buttons;
        if (currentButton) {
            // 使得不支持多个按键被同时按下
            if (pad_mb_old_buttons && !(pad_mb_old_buttons & currentButton)) {
                continue;
            }
            if (pad_mb_old_buttons == currentButton) {
                if (pad.TimeStamp - pad_mb_old_typed_timestamp < 400000 || pad.TimeStamp - pad_mb_old_action_timestamp < 50000) {
                    continue;
                } else {
                    pad_mb_old_buttons = currentButton;
                    pad_mb_old_action_timestamp = pad.TimeStamp;
                    return currentButton;
                }
            } else {
                pad_mb_old_buttons = currentButton;
                pad_mb_old_typed_timestamp = pad.TimeStamp;
                pad_mb_old_action_timestamp = pad.TimeStamp;
                return currentButton;
            }
        } else {
            padManagerButtonsClean(); // 清除按键信息,使得下一个按键立刻响应
            continue;
        }
    }
    return 0;
}
