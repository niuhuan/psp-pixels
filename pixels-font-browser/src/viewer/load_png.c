#include <png.h>
#include "viewer.h"

char viewerLoadPNG(char *filePath, int *imageWidth, int *imageHeight, SCREEN_RGB_UNIT **imageData) {
    /* 初始化PNG信息和指针 */
    png_structp png_ptr = NULL;
    png_infop info_ptr = NULL;
    png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL );
    if (!png_ptr) {
        return 0;
    }
    info_ptr = png_create_info_struct(png_ptr);
    if (!info_ptr) {
        png_destroy_read_struct(&png_ptr, NULL, NULL );
        return 0;
    }
    /* 文件指针 */
    FILE*fp = fopen(filePath, "rb");
    if (!fp) {
        png_destroy_write_struct(&png_ptr, &info_ptr);
        return 0;
    }
    /* 校验PNG头 */
    char buf[4];
    if (fread(buf, 1, 4, fp) < 4) {
        png_destroy_write_struct(&png_ptr, &info_ptr);
        fclose(fp);
        return 0;
    }
    if(png_sig_cmp((png_bytep) buf, (png_size_t) 0, 4)){
        png_destroy_write_struct(&png_ptr, &info_ptr);
        fclose(fp);
        return 0;
    }
    rewind(fp);
    /* 读PNG */
    png_init_io(png_ptr, fp);                          // 初始化PNG文件的IO
    png_read_info(png_ptr, info_ptr);                  // 读取PNG头
    png_uint_32 width, height;
    int bit_depth, color_type;
    png_get_IHDR(png_ptr, info_ptr,                    // 必要信息
            &width,                                    // 获取宽度
            &height,                                   // 获取高度
            &bit_depth,                                // 获取位深
            &color_type,                               // 获取颜色类型
            NULL, NULL, NULL );
    if (!width || !height || !bit_depth || !color_type || width * height * sizeof(SCREEN_RGB_UNIT) > VIEWER_MAX_IMAGE_DATA) {
        png_destroy_read_struct(&png_ptr, &info_ptr, NULL );
        fclose(fp);
        return 0;
    }
    png_color_16p pBackground;
    if (png_get_valid(png_ptr, info_ptr, PNG_INFO_bKGD)) {
        png_get_bKGD(png_ptr, info_ptr, &pBackground);
    }
    if (color_type == PNG_COLOR_TYPE_PALETTE)
        png_set_palette_to_rgb(png_ptr);                               // 要求转换索引颜色到RGB
    if (color_type == PNG_COLOR_TYPE_GRAY && bit_depth < 8)
        png_set_expand_gray_1_2_4_to_8(png_ptr);                       // 要求位深度强制8bit
    if (bit_depth == 16)
        png_set_strip_16(png_ptr);                                     // 要求位深度强制8bit
    if (png_get_valid(png_ptr, info_ptr, PNG_INFO_tRNS))
        png_set_tRNS_to_alpha(png_ptr);                                // 1位透明度转8位透明度
    if (color_type == PNG_COLOR_TYPE_GRAY || color_type == PNG_COLOR_TYPE_GRAY_ALPHA)
        png_set_gray_to_rgb(png_ptr);                                  // 灰度必须转换成RGB
    png_read_update_info(png_ptr, info_ptr);
    /* 读取文件 */
    *imageWidth = width;
    *imageHeight = height;
    *imageData = (SCREEN_RGB_UNIT*) malloc(sizeof(SCREEN_RGB_UNIT) * *imageWidth * *imageHeight);
    int x, y;
    SCREEN_RGB_UNIT *line;
    png_byte *png_buff = (png_bytep) png_malloc(png_ptr, width << 2); // 开拓空间
    png_uint_32 *png_line = (png_uint_32 *) png_buff;
    for (y = 0; y < *imageHeight; y++) {
        png_read_row(png_ptr, png_buff, NULL );
        line = *imageData + (*imageWidth * y);
        for (x = 0; x < *imageWidth; x++) {
            line[x] = SCREEN_RGB(png_line[x]&0xFF,png_line[x]>>8&0xFF,png_line[x]>>16&0xFF);
        }
    }
    /* 读取结束 */
    png_read_end(png_ptr, info_ptr);
    png_destroy_read_struct(&png_ptr, &info_ptr, NULL );
    fclose(fp);
    return 1;
}
