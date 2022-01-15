#!/bin/bash

FFMPEGDIR=`pwd`
FFMPEG=ffmpeg
NDK=/home/wlliang/android/android-ndk-r23b
TOOLCHAIN=${NDK}/toolchains/llvm/prebuilt/linux-x86_64/
API=21

if [ "$1" == "64" ];then
	ARCH=aarch64
	CPU=armv8-a
	CC=$TOOLCHAIN/bin/aarch64-linux-android$API-clang
	CXX=$TOOLCHAIN/bin/aarch64-linux-android$API-clang++
	SYSROOT=$NDK/toolchains/llvm/prebuilt/linux-x86_64/sysroot
	CROSS_PREFIX=$TOOLCHAIN/bin/aarch64-linux-android-
	PREFIX=$(pwd)/android/$CPU
	OPTIMIZE_CFLAGS="-march=$CPU"
else
	ARCH=arm
	CPU=armv7-a
	CC=$TOOLCHAIN/bin/armv7a-linux-androideabi$API-clang
	CXX=$TOOLCHAIN/bin/armv7a-linux-androideabi$API-clang++
	SYSROOT=$NDK/toolchains/llvm/prebuilt/linux-x86_64/sysroot
	CROSS_PREFIX=$TOOLCHAIN/bin/arm-linux-androideabi-
	PREFIX=$(pwd)/android/$CPU
	OPTIMIZE_CFLAGS="-mfloat-abi=softfp -mfpu=vfp -marm -march=$CPU "
fi
STRIP=${TOOLCHAIN}/bin/llvm-strip
BUILD=${FFMPEGDIR}/ffmpeg_build_${ARCH}


clean()
{
	rm ${PREFIX}  -rf
}


#编辑FFmpeg目录里的configure文件:
#SLIBNAME_WITH_MAJOR='$(SLIBNAME).$(LIBMAJOR)'
#LIB_INSTALL_EXTRA_CMD='$$(RANLIB) "$(LIBDIR)/$(LIBNAME)"'
#SLIB_INSTALL_NAME='$(SLIBNAME_WITH_VERSION)'
#SLIB_INSTALL_LINKS='$(SLIBNAME_WITH_MAJOR) $(SLIBNAME)'
#修改为
#SLIBNAME_WITH_MAJOR='$(SLIBPREF)$(FULLNAME)-$(LIBMAJOR)$(SLIBSUF)'
#LIB_INSTALL_EXTRA_CMD='$$(RANLIB) "$(LIBDIR)/$(LIBNAME)"'
#SLIB_INSTALL_NAME='$(SLIBNAME_WITH_MAJOR)'
#SLIB_INSTALL_LINKS='$(SLIBNAME)'


function build_ffmpeg_for_android
{
	cd ${FFMPEG}
	./configure \
	--prefix=$PREFIX \
	--disable-neon \
	--disable-hwaccels \
	--disable-gpl \
	--disable-postproc \
	--enable-shared \
	--enable-jni \
	--disable-mediacodec \
	--disable-decoder=h264_mediacodec \
	--disable-static \
	--disable-doc \
	--disable-ffmpeg \
	--disable-ffplay \
	--disable-ffprobe \
	--disable-avdevice \
	--disable-doc \
	--disable-symver \
	--cross-prefix=$CROSS_PREFIX \
	--target-os=android \
	--arch=$ARCH \
	--cpu=$CPU \
	--cc=$CC \
	--cxx=$CXX \
	--strip=$STRIP \
	--enable-cross-compile \
	--sysroot=$SYSROOT \
	--extra-cflags="-Os -fpic $OPTIMIZE_CFLAGS" \
	--extra-ldflags="$ADDI_LDFLAGS" \
	$ADDITIONAL_CONFIGURE_FLAG
	make clean
	make
	make install
	cd ${FFMPEGDIR}
}

if [ "$1" == "clean" ];then
	clean
else
	build_ffmpeg_for_android
fi
