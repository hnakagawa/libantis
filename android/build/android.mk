-include config.mk

all: Makefile
	PATH=$(BUILD_PATH) make

Makefile:
	LD=$(LD) AR=$(AR) AS=$(AS) NM=$(NM) RANLIB=$(RANLIB) SIZE=$(SIZE) STRIP=$(STRIP) CC=$(CC) CXX=$(CXX) \
	OBJDUMP=$(OBJDUMP) CPPFLAGS=$(CPPFLAGS) LDFLAGS=$(LDFLAGS) PATH=$(BUILD_PATH) \
	../../configure --target=arm-eabi --host=arm-linux

install: libantistrophe.a ../antistrophe-jni/include/strophe.h ../antistrophe-jni/include/antistrophe.h
	cp libantistrophe.a ../antistrophe-jni/obj/local/armeabi/

../antistrophe-jni/include/strophe.h: ../../strophe.h
	cp $^ ../antistrophe-jni/include/

../antistrophe-jni/include/antistrophe.h: ../../antistrophe.h
	cp $^ ../antistrophe-jni/include/
