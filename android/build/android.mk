-include config.mk

all: Makefile
	PATH=$(BUILD_PATH) make

Makefile:
	LD=$(LD) AR=$(AR) AS=$(AS) NM=$(NM) RANLIB=$(RANLIB) SIZE=$(SIZE) STRIP=$(STRIP) CC=$(CC) CXX=$(CXX) \
	OBJDUMP=$(OBJDUMP) CPPFLAGS=$(CPPFLAGS) LDFLAGS=$(LDFLAGS) PATH=$(BUILD_PATH) \
	../../configure --target=arm-eabi --host=arm-linux


