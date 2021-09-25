#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include <jpeglib.h>
#include "viewer.h"

char viewerLoadJPEG(char *filePath, int *imageWidth, int *imageHeight, SCREEN_RGB_UNIT **imageData) {
    // 读文件
    FILE *file = fopen(filePath, "rb");
    if (file == NULL ) {
        return 0;
    }
    /* 校验JPEG头 */
    unsigned char buf[2];
    if (fread(buf, 1, 2, file) < 2) {
        fclose(file);
        return 0;
    }
    if (buf[0] != 0xff || buf[1] != 0xd8) {
        fclose(file);
        return 0;
    }
    rewind(file);
    //
    struct jpeg_decompress_struct info;                     // for our jpeg info
    struct jpeg_error_mgr jerr;
    info.err = jpeg_std_error(&jerr);
    jpeg_create_decompress(&info);                          // fills info structure
    jpeg_stdio_src(&info, file);                            // void
    if (jpeg_read_header(&info, 1) != JPEG_HEADER_OK) {
        jpeg_destroy_decompress(&info);
        fclose(file);
        return 0;
    }
    if (!jpeg_start_decompress(&info)) {
        jpeg_destroy_decompress(&info);
        fclose(file);
        return 0;
    }
    if (info.num_components != 3 && info.num_components != 4) {  // 3 = RGB, 4 = RGB
        jpeg_destroy_decompress(&info);
        fclose(file);
        return 0;
    }
    *imageWidth = info.output_width;
    *imageHeight = info.output_height;
    *imageData = (SCREEN_RGB_UNIT *) malloc(sizeof(SCREEN_RGB_UNIT) * *imageWidth * *imageHeight);
    int x, y;
    SCREEN_RGB_UNIT *line;
    unsigned char *rowptr = malloc(info.num_components * *imageWidth * *imageHeight);
    for (y = 0; y < *imageHeight; y++) {
        line = *imageData + (*imageWidth * y);
        jpeg_read_scanlines(&info, &rowptr, 1);
        for (x = 0; x < *imageWidth; x++) {
            if (info.num_components == 3) {
                line[x] = SCREEN_RGB(rowptr[3 * x + 0],rowptr[3 * x + 1],rowptr[3 * x + 2]);
            } else if (info.num_components == 4) {
                line[x] = SCREEN_RGB(rowptr[4 * x + 0],rowptr[ 4 * x + 1],rowptr[4 * x + 2]);
            }
        }
    }
    jpeg_finish_decompress(&info);
    fclose(file);
    return 1;
}
