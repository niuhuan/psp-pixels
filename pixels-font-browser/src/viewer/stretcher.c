#include <malloc.h>

/*参数为：
 *返回图片的宽度(w_Dest),
 *返回图片的高度(h_Dest),
 *返回图片的位深(bit_depth),
 *源图片的RGB数据(src),
 *源图片的宽度(w_Src),
 *源图片的高度(h_Src)
 */
unsigned char *
do_Stretch_Linear(unsigned char *src, int src_width, int src_height, int to_width, int to_height, int bit_depth) {
    int sw = src_width - 1, sh = src_height - 1, dw = to_width - 1, dh = to_height - 1;
    int B, N, x, y;
    int nPixelSize = bit_depth / 8;
    unsigned char *pLinePrev, *pLineNext;
    unsigned char *pDest = malloc(to_width * to_height * bit_depth / 8);
    unsigned char *tmp;
    unsigned char *pA, *pB, *pC, *pD;
    int i, j, k;
    for (i = 0; i <= dh; ++i) {
        tmp = pDest + i * to_width * nPixelSize;
        y = i * sh / dh;
        N = dh - i * sh % dh;
        pLinePrev = src + (y++) * src_width * nPixelSize;
        //pLinePrev =(unsigned char *)aSrc->m_bitBuf+((y++)*aSrc->m_width*nPixelSize);
        pLineNext = (N == dh) ? pLinePrev : src + y * src_width * nPixelSize;
        //pLineNext = ( N == dh ) ? pLinePrev : (unsigned char *)aSrc->m_bitBuf+(y*aSrc->m_width*nPixelSize);
        for (j = 0; j <= dw; ++j) {
            x = j * sw / dw * nPixelSize;
            B = dw - j * sw % dw;
            pA = pLinePrev + x;
            pB = pA + nPixelSize;
            pC = pLineNext + x;
            pD = pC + nPixelSize;
            if (B == dw) {
                pB = pA;
                pD = pC;
            }
            for (k = 0; k < nPixelSize; ++k) {
                *tmp++ = (unsigned char) (int) ((B * N * (*pA++ - *pB - *pC + *pD) + dw * N * *pB++ + dh * B * *pC++ + (dw * dh - dh * B - dw * N) * *pD++ + dw * dh / 2) / (dw * dh));
            }
        }
    }
    return pDest;
}

unsigned char *
do_Stretch_Linear_Ratio(unsigned char *src, int src_width, int src_height, int* to_width, int* to_height, int bit_depth) {
    double width_ratio = ((double) *to_width) / ((double) src_width);
    double height_ratio = ((double) *to_height) / ((double) src_height);
    if (width_ratio >= 1 && height_ratio >= 1) {
        *to_width = src_width;
        *to_height = src_height;
    } else {
        double ratio = (width_ratio <= height_ratio) ? width_ratio : height_ratio;
        *to_width = (int) (ratio * ((double) src_width));
        *to_height = (int) (ratio * ((double) src_height));
    }
    return do_Stretch_Linear(src, src_width, src_height, *to_width, *to_height, bit_depth);
}


