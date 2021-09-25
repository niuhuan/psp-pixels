
MODE = REL

TARGET = browser
EXTRA_TARGETS = EBOOT.PBP
PSP_EBOOT_ICON = res/ICON0.PNG
PSP_EBOOT_TITLE = MISC

PSP_FW_VERSION = 200

INCDIR =
LIBDIR =

ARCHFLAGS =-mgp32 -mlong32 -msingle-float -mabi=eabi
CFLAGS = -Wall -fomit-frame-pointer $(ARCHFLAGS) -G3
ifeq ($(MODE),REL)
CFLAGS += -O3
else
CFLAGS += -O0 -ggdb -gdwarf-2
endif
CXXFLAGS = $(CFLAGS) -fno-exceptions -fno-rtti
ASFLAGS	= $(CFLAGS)

OBJS =                      \
        src/main.o \
        src/browser.o \
        src/viewer/viewer.o \
        src/hardwares/pad.o \
        src/hardwares/screen.o \
        src/fonts/latin_8x8.o \
        src/fonts/sjis_10.o \
        src/fonts/unicode_0.o \
        src/fonts/unicode_12.o \
        src/fonts/unicode_16.o \



LIBS = -lpsppower  -ljpeg -lpng -lz -lm

PSPSDK	:= $(shell psp-config --pspsdk-path)
include $(PSPSDK)/lib/build.mak

DST = $(INSTALL_PREFIX)
ifeq ($(DST),)
DST = dist
endif

install : all
	rm -rf "${DST}"
	mkdir "${DST}"
	cp -rf EBOOT.PBP res/UNI12.FNT res/UNI16.FNT "${DST}"

