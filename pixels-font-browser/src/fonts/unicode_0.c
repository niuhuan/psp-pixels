#include "unicode_0.h"

void UCS2toUTF8(unsigned short *ucs2_str, unsigned char *utf8_str) {
    unsigned char *utf8_point = utf8_str;
    unsigned short *ucs2_point = ucs2_str;
    while (*ucs2_point) {
        if (0x0080 > *ucs2_point) {
            /* 1 byte UTF-8 Character.*/
            *utf8_point = (unsigned char) *ucs2_point;
            utf8_str += 1;
        } else if (0x0800 > *ucs2_point) {
            /*2 bytes UTF-8 Character.*/
            *utf8_point = ((unsigned char) (*ucs2_point >> 6)) | 0xc0;
            *(utf8_point + 1) = ((unsigned char) (*ucs2_point & 0x003F)) | 0x80;
            utf8_str += 2;
        } else {
            /* 3 bytes UTF-8 Character .*/
            *utf8_point = ((unsigned char) (*ucs2_point >> 12)) | 0xE0;
            *(utf8_point + 1) = ((unsigned char) ((*ucs2_point & 0x0FC0) >> 6)) | 0x80;
            *(utf8_point + 2) = ((unsigned char) (*ucs2_point & 0x003F)) | 0x80;
            utf8_str += 3;
        }
        ucs2_point++;
    }
    *utf8_point = (unsigned char) 0;
}

BOOL UTF8toUCS2(unsigned char *utf8_str, unsigned short *ucs2_str) {
    unsigned char *utf8_point = utf8_str;
    unsigned short *ucs2_point = ucs2_str;
    unsigned short temp1, temp2;
    while (*utf8_point) {
        if (0x00 == (*utf8_point & 0x80)) {
            /* 1 byte UTF-8 Charater.*/
            /* --------------------- */
            //1字节 0xxxxxxx
            //0x80=1000,0000，判断最高位是否为0，如果为0，那么是ASCII字符
            //不需要处理，直接拷贝即可
            /* --------------------- */
            *ucs2_point = *utf8_point;
            utf8_point += 1;
            ucs2_point += 1;
        } else if (0xc0 == (*utf8_point & 0xe0) && 0x80 == (*(utf8_point + 1) & 0xc0)) {
            /* 2 bytes UTF-8 Charater.*/
            /* --------------------- */
            //2字节 110xxxxx 10xxxxxx
            //0xe0=1110,0000
            //0xc0=1100,0000
            /* --------------------- */
            //0x1f=0001,1111，获得第一个字节的后5位
            temp1 = (unsigned short) (*utf8_point & 0x1f);
            //左移6位
            temp1 <<= 6;
            //0x3f=0011,1111，获得第二个字节的后6位
            //加上上面的5位一共有11位
            temp1 |= (unsigned short) (*(utf8_point + 1) & 0x3f);
            *ucs2_point = temp1;
            utf8_point += 2;
            ucs2_point += 1;
        } else if (0xe0 == (*utf8_point & 0xf0) && 0x80 == (*(utf8_point + 1) & 0xc0) && 0x80 == (*(utf8_point + 2) & 0xc0)) {
            /* 3bytes UTF-8 Charater.*/
            /* --------------------- */
            //3字节 1110xxxx 10xxxxxx 10xxxxxx
            //中文要进入这一个分支
            /* --------------------- */
            //0x0f=0000,1111
            //取出第一个字节的低4位
            temp1 = (unsigned short) (*utf8_point & 0x0f);
            temp1 <<= 12;
            //0x3f=0011,1111
            //取得第二个字节的低6位
            temp2 = (unsigned short) (*(utf8_point + 1) & 0x3F);
            temp2 <<= 6;
            //取得第三个字节的低6位，最后组成16位
            temp1 = temp1 | temp2 | (unsigned short) (*(utf8_point + 2) & 0x3F);
            *ucs2_point = temp1;
            //移动到下一个字符
            utf8_point += 3;
            ucs2_point += 1;
        } else {
            /* unrecognize byte. */
            *ucs2_point = 0x22e0;
            //直接退出循环
            return 0;
        }
    }
    *ucs2_point = (unsigned short) 0;
    return 1;
}

