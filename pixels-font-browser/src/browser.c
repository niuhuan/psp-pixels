#include <pspiofilemgr.h>
#include "string.h"
#include "browser.h"
#include "hardwares/screen.h"
#include "fonts/latin_8x8.h"
#include "fonts/unicode_12.h"
#include "hardwares/pad.h"
#include "viewer/viewer.h"


/* 界面内可见颜色 */
#define BROWSER_BG_COLOR SCREEN_RGB(24,24,33)     // 背景色
#define BROWSER_FM_COLOR SCREEN_RGB(86,86,94)       // 边框颜色
#define BROWSER_TX_COLOR SCREEN_RGB(248,248,248)     // 文本颜色
#define BROWSER_AL_COLOR SCREEN_RGB(255,255,255)     // 光标文本颜色
#define BROWSER_AL_BG_COLOR SCREEN_RGB(62,62,70)    // 光标文本底色
#define BROWSER_RM_COLOR SCREEN_RGB(150,150,150)     // 提示文本颜色


#define BROWSER_MS_BG_COLOR SCREEN_RGB(50,50,60)              // 背景色
#define BROWSER_MS_FM_COLOR SCREEN_RGB(110,110,120)           // 边框颜色
#define BROWSER_MS_RM_COLOR SCREEN_RGB(200,200,210)           // 边框颜色
#define BROWSER_MS_TX_COLOR SCREEN_RGB(255,255,255)           // 边框颜色

/* 绘图信息 */
#define BROWSER_STRING_FUNCTION screenDrawStringUTF8_12
#define BROWSER_STRING_FONT_HEIGHT 12
#define BROWSER_STRING_FONT_WIDTH 12

/* 路径限制 */
#define MAX_PATH 512            // 文件夹路径最大容量512(char)
#define MAX_NAME 256            // 单个文件文件名最大256(char)
#define MAX_ENTRY 2048          // 每个文件夹最多2048个文件

/* 路径定义 */
#define BROWSER_ROOT_DIR "ms0:/"
#define BROWSER_PARENT_SEPARATOR ".."
#define BROWSER_DIR_SEPARATOR_CHAR '/'
#define BROWSER_DIR_SEPARATOR_STRING "/"

/* 文件夹信息 */
char browser_cache_current_dir[MAX_PATH];       // 当前所展示的文件夹
int browser_cache_count;                        // 当前所展示的文件夹 有多少项目
SceIoDirent browser_cache_dirent[MAX_ENTRY];    // 当前所展示的文件夹 项目
char *browser_cache_entries[MAX_NAME];          // 当前所展示的文件夹 项目展示名称
int browser_cache_cursor_index;                 // 浏览器选中的文件
int browser_cache_list_offset;                  // 浏览器窗口第一个文件


/* 刷新文件夹缓存 */
void browserSetDir(char *dir) {
    strcpy(browser_cache_current_dir, dir);
    browser_cache_list_offset = 0;
    browser_cache_cursor_index = 0;
    browser_cache_count = 0;
    // 读取文件夹内容
    if (strcmp(browser_cache_current_dir, BROWSER_ROOT_DIR)) {
        // 不是根目录加上 ..
        memset(&browser_cache_dirent[browser_cache_count], 0x00, sizeof(SceIoDirent));
        strcpy(browser_cache_dirent[browser_cache_count].d_name, BROWSER_PARENT_SEPARATOR);
        browser_cache_dirent[browser_cache_count].d_stat.st_attr = FIO_SO_IFDIR;
        browser_cache_count++;
    }
    int fd = sceIoDopen(browser_cache_current_dir);
    while (browser_cache_count < MAX_ENTRY) {
        memset(&browser_cache_dirent[browser_cache_count], 0x00, sizeof(SceIoDirent));
        if (sceIoDread(fd, &browser_cache_dirent[browser_cache_count]) <= 0)
            break;
        if (browser_cache_dirent[browser_cache_count].d_name[0] == '.')
            continue;
        else
            browser_cache_count++;
    }
    sceIoDclose(fd);
    // 遍历读取到的内容, 如果是文件夹则排在前面, 并且名字增加 '/'
    int p, k = 0;
    for (p = 0; p < browser_cache_count; p++) {
        if (browser_cache_dirent[p].d_stat.st_attr == FIO_SO_IFDIR) {
            if (strcmp(browser_cache_dirent[p].d_name, BROWSER_PARENT_SEPARATOR))
                strcat(browser_cache_dirent[p].d_name, BROWSER_DIR_SEPARATOR_STRING);
            browser_cache_entries[k++] = browser_cache_dirent[p].d_name;
        }
    }
    for (p = 0; p < browser_cache_count; p++) {
        if (browser_cache_dirent[p].d_stat.st_attr != FIO_SO_IFDIR) {
            browser_cache_entries[k++] = browser_cache_dirent[p].d_name;
        }
    }
}

/* 绘制界面的逻辑 */
void browserRenderMainFrame() {
    /* 线框位置 */
    int frameTop = 25;                                       // 上边25像素
    int frameHeight = SCREEN_HEIGHT - frameTop - 23;         // 下边23像素
    int frameLeft = 17;                                      // 左边17像素
    int frameWidth = SCREEN_WIDTH - frameLeft - 17;          // 右边17像素
    int frameLineWidth = 2;
    /* 内容区 */
    int contentTop = frameTop + frameLineWidth;
    int contentLeft = frameLeft + frameLineWidth;
    int contentHeight = frameHeight - frameLineWidth * 2;
    int contentWidth = frameWidth - frameLineWidth * 2;
    /* 列表 */
    int itemFrameMarginVertical = 5;                          // 列表中项目于边框垂直间距
    int itemFrameMarginHorizontal = 10;                       // 列表中项目于边框水平间距
    int itemLineHeight = BROWSER_STRING_FONT_HEIGHT;                            // 列表行高
    int itemTopOffset = contentTop + itemFrameMarginVertical;                   // 第一行的位置
    int itemLeftOffset = contentLeft + itemFrameMarginHorizontal;               // 第一行的位置
    int browser_cache_list_limit = ((contentHeight - (itemFrameMarginVertical * 2)) /
                                    itemLineHeight);  // 列表显示多少行=(内容区高度-上下垂直间距)/行高
    /* 确定列表内被显示的区间，如果光标所在位置不在显示区间，则修改显示区见  */
    if (browser_cache_cursor_index + 1 - browser_cache_list_offset > browser_cache_list_limit) {
        browser_cache_list_offset = browser_cache_cursor_index + 1 - browser_cache_list_limit;
    } else if (browser_cache_cursor_index < browser_cache_list_offset) {
        browser_cache_list_offset = browser_cache_cursor_index;
    }
    /* 滚动条 */
    int scrollBarRegionHeight = contentHeight;                                    // 滚动条区域为内容区高度
    int scrollBarRegionWidth = 15;                                                 // 滚动条区域宽度
    int scrollBarRegionTop = contentTop;
    int scrollBarRegionLeft = contentLeft + contentWidth - scrollBarRegionWidth;   // 滚动条置于内容区最右侧
    char enableScrollBar = browser_cache_count > browser_cache_list_limit;         // 如果列表文件大于能显示的个数，则需要显示滚动条
    /* 被选中项目的阴影 */
    int shadowSpreadVertical = 1;
    int shadowLeft = contentLeft + 3; // 阴影左间距为3
    int shadowWidth =
            contentWidth - 3 * 2 - (enableScrollBar ? scrollBarRegionWidth : 0); // 阴影左右间距为3，如果显示滚动条则相对显示曲右间距更大
    /* 绘图 */
    screenFillColor(BROWSER_BG_COLOR);
    // 绘制滚动条
    if (enableScrollBar) {
        screenDrawRect(scrollBarRegionLeft, frameTop, 2, frameHeight, BROWSER_FM_COLOR); // 左侧为宽度为2的垂直实线
        // 除去2像素直线剩余区域宽度
        int maxListOffset = browser_cache_count - browser_cache_list_limit;       // 滚动条在最下面时的offset值
        int pixelPosition = (browser_cache_list_offset * 100) / maxListOffset;    // 当前offset的百分比
        screenFillRect(scrollBarRegionLeft + 2 + 1,         // 滚动条相对实线距离1
                       scrollBarRegionTop + 1                              // 滚动条相对顶部距离1
                       + pixelPosition,                            // 另外增加百分比偏移
                       scrollBarRegionWidth - (2 + 1) - 1,                  // 滚动条相对右侧距离1
                       scrollBarRegionHeight - 1 - 1                       // 滚动条相对底部距离1
                       - 100,                                      // 另外增加100百分比偏移
                       BROWSER_FM_COLOR);
    }
    // 绘制列表显示区间的前一个文件
    if (browser_cache_list_offset) {
        BROWSER_STRING_FUNCTION(itemLeftOffset,
                                itemTopOffset - itemLineHeight, BROWSER_AL_COLOR,
                                (unsigned char *) browser_cache_entries[browser_cache_list_offset - 1]);
    }
    // 绘制列表显示区间
    int p;
    int e = browser_cache_list_offset + browser_cache_list_limit < browser_cache_count ? browser_cache_list_offset +
                                                                                         browser_cache_list_limit
                                                                                       : browser_cache_count;
    for (p = browser_cache_list_offset; p < e; p++) {
        if (p == browser_cache_cursor_index) {
            screenFillRect(shadowLeft, itemTopOffset - shadowSpreadVertical, shadowWidth,
                           itemLineHeight + shadowSpreadVertical, BROWSER_AL_BG_COLOR);
            BROWSER_STRING_FUNCTION(itemLeftOffset, itemTopOffset,
                                    BROWSER_AL_COLOR, (unsigned char *) browser_cache_entries[p]);
        } else {
            BROWSER_STRING_FUNCTION(itemLeftOffset, itemTopOffset,
                                    BROWSER_TX_COLOR, (unsigned char *) browser_cache_entries[p]);
        }
        itemTopOffset += itemLineHeight;
    }
    // 绘制列表显示区间的后一个文件
    if (p < browser_cache_count) {
        BROWSER_STRING_FUNCTION(itemLeftOffset, itemTopOffset,
                                BROWSER_TX_COLOR, (unsigned char *) browser_cache_entries[p]);
    }
    // 涂掉处理超出内容区的文字
    screenFillRect(0, 0, SCREEN_WIDTH, frameTop, BROWSER_BG_COLOR);
    screenFillRect(0, frameTop + frameHeight, SCREEN_WIDTH, SCREEN_HEIGHT - frameTop - frameHeight, BROWSER_BG_COLOR);
    // 画线框
    int frameLineWidthIndex;
    for (frameLineWidthIndex = 0; frameLineWidthIndex < frameLineWidth; frameLineWidthIndex++) {
        screenDrawRect(frameLeft + frameLineWidthIndex, frameTop + frameLineWidthIndex,
                       frameWidth - 2 * frameLineWidthIndex, frameHeight - 2 * frameLineWidthIndex, BROWSER_FM_COLOR);
    }
    // 显示当前文件夹路径
    BROWSER_STRING_FUNCTION(frameLeft, frameTop - itemLineHeight - 3,
                            BROWSER_RM_COLOR, (unsigned char *) browser_cache_current_dir);
    // 显示线框下方的提示
    screenDrawStringLatin8x8(frameLeft,
                            frameTop + frameHeight + 3, BROWSER_RM_COLOR,
                            (unsigned char *) ("\1:SELECT \2:CANCEL \3:REFRESH \4:PARENT"));
}


/* 显示一条消息 */
void browserShowMessage(char *message) {
    browserRenderMainFrame();
    int messageContentHeight = 60;
    int messageContentWidth = 200;
    int messageContentY = (SCREEN_HEIGHT - messageContentHeight) / 2;
    int messageContentX = (SCREEN_WIDTH - messageContentWidth) / 2;
    int messageShadowHeight = messageContentHeight + 40;
    int messageShadowWidth = messageContentWidth + 30;
    int messageShadowY = (SCREEN_HEIGHT - messageShadowHeight) / 2;
    int messageShadowX = (SCREEN_WIDTH - messageShadowWidth) / 2;
    int messageFrameBroderWidth = 2;
    int messageRemarkMargin = 2;
    screenFillRect(messageShadowX, messageShadowY, messageShadowWidth, messageShadowHeight, BROWSER_MS_BG_COLOR);
    int i;
    for (i = 1; i < messageFrameBroderWidth + 1; i++) {
        screenDrawRect(messageContentX - i, messageContentY - i, messageContentWidth + i * 2,
                       messageContentHeight + i * 2, BROWSER_MS_FM_COLOR);
    }
    BROWSER_STRING_FUNCTION(messageContentX - messageFrameBroderWidth,
                            messageContentY - messageFrameBroderWidth - BROWSER_STRING_FONT_HEIGHT -
                            messageRemarkMargin, BROWSER_MS_RM_COLOR, (unsigned char *) "MESSAGE:");
    char *mrm = "\1:CONFIRM";
    screenDrawStringLatin8x8(
            messageContentX + messageContentWidth + messageFrameBroderWidth - strlen(mrm) * BROWSER_STRING_FONT_WIDTH,
            messageContentY + messageContentHeight + messageFrameBroderWidth + messageRemarkMargin, BROWSER_MS_RM_COLOR,
            (unsigned char *) mrm);
    BROWSER_STRING_FUNCTION(messageContentX + 15, messageContentY + (messageContentHeight - 20) / 2,
                            BROWSER_MS_TX_COLOR, (unsigned char *) message);
    screenFlip();
    while (1) {
        if (padManagerButtonsWait() == PSP_CTRL_CIRCLE)
            break;
    }
}

void gotoParentDir() {
    char *p;
    p = strrchr(browser_cache_current_dir, BROWSER_DIR_SEPARATOR_CHAR);
    *--p = 0;
    p = strrchr(browser_cache_current_dir, BROWSER_DIR_SEPARATOR_CHAR);
    *++p = 0;
    browserSetDir(browser_cache_current_dir);
}

void doSelectFile(char *path) {
    if (!viewerShowImage(path)) {
        browserShowMessage("This image is \n      nonsupport!");
    }
}


void browserLoop() {
    browserSetDir(BROWSER_ROOT_DIR);
    while (1) {
        browserRenderMainFrame();
        screenFlip();
        /* 当操作上下左右或者LR时移动光标所指向的文件,当时⚪者×时进行相应操作 */
        switch (padManagerButtonsWait()) {
            case PSP_CTRL_UP:
                // 选择上一个文件
                browser_cache_cursor_index =
                        (browser_cache_cursor_index - 1 + browser_cache_count) % browser_cache_count;
                break;
            case PSP_CTRL_DOWN:
                // 选择下一个文件
                browser_cache_cursor_index = (browser_cache_cursor_index + 1) % browser_cache_count;
                break;
            case PSP_CTRL_LEFT:
            case PSP_CTRL_LTRIGGER:
                // 上一页
                browser_cache_cursor_index -= 10;
                if (browser_cache_cursor_index < 0) {
                    browser_cache_cursor_index = 0;
                }
                break;
            case PSP_CTRL_RIGHT:
            case PSP_CTRL_RTRIGGER:
                // 下一页
                browser_cache_cursor_index += 10;
                if (browser_cache_cursor_index >= browser_cache_count) {
                    browser_cache_cursor_index = browser_cache_count - 1;
                }
                break;
            case PSP_CTRL_CIRCLE:
                // 取出光标中的内容
                if (!strcmp(browser_cache_entries[browser_cache_cursor_index], BROWSER_PARENT_SEPARATOR)) {
                    // 如果是 ".." 则返回上级目录, 将char置为0作为字符串结尾, C语言特性
                    gotoParentDir();
                } else if (browser_cache_entries[browser_cache_cursor_index][
                                   strlen(browser_cache_entries[browser_cache_cursor_index]) - 1] == '/') {
                    // 如果以'/'结尾 则进入目录
                    strcat(browser_cache_current_dir, browser_cache_entries[browser_cache_cursor_index]);
                    browserSetDir(browser_cache_current_dir);
                } else {
                    char path[MAX_PATH];
                    strcpy(path, browser_cache_current_dir);
                    strcat(path, browser_cache_entries[browser_cache_cursor_index]);
                    doSelectFile(path);
                }
                break;
            case PSP_CTRL_SQUARE:
                // 刷新
                browserSetDir(browser_cache_current_dir);
                break;
            case PSP_CTRL_CROSS:
            case PSP_CTRL_TRIANGLE:
                // 上级目录按钮
                if (strcmp(browser_cache_current_dir, BROWSER_ROOT_DIR)) {
                    gotoParentDir();
                } else {
                    browserSetDir(browser_cache_current_dir);
                }
                break;
            default:
                screenWaitForNextFrame();
                break;
        }
    }
}


