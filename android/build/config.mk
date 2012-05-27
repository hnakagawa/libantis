ANDROID_ROOT ?= /path/to/android_source
NDK_ROOT ?= /path/to/android_ndk
NDK_VER ?= 8
PLATFORM_VER ?= 9

LD = arm-linux-androideabi-ld
AR = arm-linux-androideabi-ar
AS = arm-linux-androideabi-as
NM = arm-linux-androideabi-nm
RANLIB = arm-linux-androideabi-ranlib
SIZE = arm-linux-androideabi-size
STRIP = arm-linux-androideabi-strip
CC = arm-linux-androideabi-gcc
CXX = arm-linux-androideabi-g++
OBJDUMP = arm-linux-androideabi-objdump

CPPFLAGS = "--sysroot=$(NDK_ROOT)/platforms/android-$(PLATFORM_VER)/arch-arm -I$(NDK_ROOT)/platforms/android-$(PLATFORM_VER)/arch-arm/usr/include/ -I$(ANDROID_ROOT)/external/openssl/include/ -I$(ANDROID_ROOT)/out/target/product/generic/obj/include/libexpat -I$(ANDROID_ROOT)/bionic/libc/private/ -D_ANDROID"
LDFLAGS = "--sysroot=$(NDK_ROOT)/platforms/android-$(PLATFORM_VER)/arch-arm -Wl,-rpath-link=$(NDK_ROOT)/platforms/android-$(PLATFORM_VER)/arch-arm/usr/lib/ -L$(NDK_ROOT)/platforms/android-$(PLATFORM_VER)/arch-arm/usr/lib/ -L$(ANDROID_ROOT)/out/target/product/generic/obj/STATIC_LIBRARIES/libssl_intermediates/ -L$(ANDROID_ROOT)/out/target/product/generic/obj/STATIC_LIBRARIES/libcrypto_intermediates/ -L$(ANDROID_ROOT)/out/target/product/generic/obj/SHARED_LIBRARIES/libexpat_intermediates/LINKED/"

BUILD_PATH = $(NDK_ROOT)/toolchains/arm-linux-androideabi-4.4.3/prebuilt/linux-x86/bin:$(PATH)
