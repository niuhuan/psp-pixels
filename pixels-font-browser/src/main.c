
#include <pspkernel.h>
#include "hardwares/screen.h"
#include "hardwares/pad.h"
#include "browser.h"

PSP_MODULE_INFO("MISC", PSP_MODULE_USER, 1, 0);
PSP_MAIN_THREAD_ATTR(PSP_THREAD_ATTR_USER);
PSP_HEAP_SIZE_MAX();


int main() {
    screenInit();
    padInit();
    browserLoop();
    sceKernelExitGame();
    return 0;
}
