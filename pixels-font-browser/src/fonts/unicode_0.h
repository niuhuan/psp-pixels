#ifndef UNICODE_0_H_
#define UNICODE_0_H_

#include <tdefs.h>

/**
 * UCS2转UTF8
 * @param ucs2_str
 * @param utf8_str
 */
void UCS2toUTF8(unsigned short *ucs2_str, unsigned char *utf8_str);

/**
 * UTF8转UCS2
 * @param utf8_str
 * @param ucs2_str
 * @return
 */
BOOL UTF8toUCS2(unsigned char *utf8_str, unsigned short *ucs2_str);

#endif /* UNICODE_0_H_ */
