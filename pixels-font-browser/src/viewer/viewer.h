#ifndef VIEWER_H_
#define VIEWER_H_


#include "../hardwares/screen.h"

#define VIEWER_MAX_IMAGE_DATA 0x400000

char viewerShowImage(char* filePath);

#endif /* VIEWER_H_ */
