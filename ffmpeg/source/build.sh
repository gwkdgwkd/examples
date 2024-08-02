#!/bin/bash

#https://trac.ffmpeg.org/wiki/CompilationGuide/Ubuntu#RevertingChangesMadebyThisGuideCompile%20FFmpeg%20on%20Ubuntu,%20Debian,%20or%20Mint

FFMPEGDIR=`pwd`
BUILD=${FFMPEGDIR}/ffmpeg_build

YASM=yasm-1.3.0
YASMTAR=${YASM}.tar.gz
NASM=nasm-2.15.05
NASMTAR=${NASM}.tar.bz2
LIBX264=x264
LIBX265TAR=x265.tar.bz2
LIBFDKAAC=fdk-aac
LIBOPUS=opus
LIBVPX=libvpx
LIBAOM=aom
LIBSVTAV1=SVT-AV1
LIBDAV1D=dav1d
LIBVMAF=vmaf-2.1.1
LIBVMAFTAR=v2.1.1.tar.gz
FFMPEG=ffmpeg
FFMPEGTAR=ffmpeg-snapshot.tar.bz2

clean()
{
	rm ${YASM} ${YASMTAR} ${NASM} ${NASMTAR} ${LIBX264} ${LIBX265TAR} \ 
	${LIBFDKAAC} ${LIBOPUS} ${LIBVPX} ${LIBSVTAV1} ${LIBDAV1D} ${LIBVMAF} ${LIBVMAFTAR} \ 
	${FFMPEG} ${FFMPEGTAR} ${BUILD} ${LIBAOM} -rf
}

build_yasm()
{
	# Yasm
	if [ ! -e ${YASMTAR} ];then
		wget http://www.tortall.net/projects/yasm/releases/yasm-1.3.0.tar.gz
	fi
	if [ ! -e ${YASM} ];then
		tar xzvf yasm-1.3.0.tar.gz
	fi
	cd ${YASM}
	./configure --prefix="$BUILD" --bindir="$BUILD/bin"
	make && make install
	#make disclean

	cd ${FFMPEGDIR}
}
build_nasm()
{
	# nasm
	# sudo apt-get install nasm
	if [ ! -e ${NASMTAR} ];then
        wget https://www.nasm.us/pub/nasm/releasebuilds/2.15.05/nasm-2.15.05.tar.bz2
	fi
	if [ ! -e ${NASM} ];then
		tar xjvf ${NASMTAR}
	fi
	cd ${NASM}
	./configure --prefix="$BUILD" --bindir="$BUILD/bin"
	make && make install

	cd ${FFMPEGDIR}
}
build_x264()
{
	# libx264
	# H.264 video encoder. See the H.264 Encoding Guide for more information and usage examples.
	# Requires ffmpeg to be configured with --enable-gpl --enable-libx264. 
	# sudo apt-get install libx264-dev
	if [ ! -e ${LIBX264} ];then
        git -C x264 pull 2> /dev/null || git clone --depth 1 https://code.videolan.org/videolan/x264.git
	fi
	cd ${LIBX264}
	PATH="$BUILD/bin:$PATH" ./configure --prefix="$BUILD" --bindir="$BUILD/bin" --enable-static --enable-pic
	PATH="$BUILD/bin:$PATH" make && make install
	#make disclean

	cd ${FFMPEGDIR}
}
build_x265()
{
    # libx265
	# H.265/HEVC video encoder. See the H.265 Encoding Guide for more information and usage examples.
	# Requires ffmpeg to be configured with --enable-gpl --enable-libx265. 
	# sudo apt-get install libx265-dev libnuma-dev
    # sudo apt-get install libnuma-dev
	if [ ! -e ${LIBX265TAR} ];then
        wget -O x265.tar.bz2 https://bitbucket.org/multicoreware/x265_git/get/master.tar.bz2
		tar xjvf ${LIBX265TAR}
    fi
    cd multicoreware*/build/linux
    PATH="$BUILD/bin:$PATH" cmake -G "Unix Makefiles" -DCMAKE_INSTALL_PREFIX="$BUILD" -DENABLE_SHARED=off ../../source
	PATH="$BUILD/bin:$PATH" make && make install
}
build_vpx()
{
	# libvpx
	# VP8/VP9 video encoder/decoder. See the VP9 Video Encoding Guide for more information and usage examples.
	# Requires ffmpeg to be configured with --enable-libvpx. 
	# sudo apt-get install libvpx-dev
	if [ ! -e ${LIBVPX} ];then
		git -C libvpx pull 2> /dev/null || git clone --depth 1 https://chromium.googlesource.com/webm/libvpx.git
	fi
	cd ${LIBVPX}
	PATH="$BUILD/bin:$PATH" ./configure --prefix="$BUILD" --disable-examples --disable-unit-tests --enable-vp9-highbitdepth --as=yasm
	PATH="$BUILD/bin:$PATH" make && make install
	#make clean

	cd ${FFMPEGDIR}
}
build_fdk-aac()
{
	# libfdk-aac
	#  AAC audio encoder. See the AAC Audio Encoding Guide for more information and usage examples.
	# Requires ffmpeg to be configured with --enable-libfdk-aac (and --enable-nonfree if you also included --enable-gpl). 
	# sudo apt-get install libfdk-aac-dev
	if [ ! -e ${LIBFDKAAC} ];then
		git -C fdk-aac pull 2> /dev/null || git clone --depth 1 https://github.com/mstorsjo/fdk-aac
	fi

	cd ${LIBFDKAAC}
	autoreconf -fiv
	autoreconf -fiv && ./configure --prefix="$BUILD" --disable-shared
	make && make install
	#make disclean

	cd ${FFMPEGDIR}
}
build_opus()
{
	# libopus
	# Opus audio decoder and encoder.
	# Requires ffmpeg to be configured with --enable-libopus. 
	# sudo apt-get install libopus-dev
	if [ ! -e ${LIBOPUS} ];then
		git -C opus pull 2> /dev/null || git clone --depth 1 https://github.com/xiph/opus.git
	fi
	cd ${LIBOPUS}
	./autogen.sh && ./configure --prefix="$BUILD" --disable-shared
	make && make install
	#make disclean

	cd ${FFMPEGDIR}
}

build_aom()
{
	# libaom
	# AV1 video encoder/decoder:
	# Warning: libaom does not yet appear to have a stable API, so compilation of libavcodec/libaomenc.c may occasionally fail. Just wait a day or two for us to catch up with these annoying changes, re-download ffmpeg-snapshot.tar.bz2, and try again. Or skip libaom altogether.
	if [ ! -e ${LIBAOM} ];then
		git -C aom pull 2> /dev/null || git clone --depth 1 https://aomedia.googlesource.com/aom
	fi
	mkdir -p aom_build && cd aom_build
	PATH="$BUILD/bin:$PATH" cmake -G "Unix Makefiles" -DCMAKE_INSTALL_PREFIX="$BUILD" -DENABLE_TESTS=OFF -DENABLE_NASM=on ../${LIBAOM}
	PATH="$BUILD/bin:$PATH" make && make install
	cd ${FFMPEGDIR}	
}

build_svtav1()
{
	# libsvtav1
	# AV1 video encoder/decoder. Only the encoder is supported by FFmpeg, so building of the decoder is disabled.
	# Requires ffmpeg to be configured with --enable-libsvtav1. 
	if [ ! -e ${LIBSVTAV1} ];then
		git -C SVT-AV1 pull 2> /dev/null || git clone https://gitlab.com/AOMediaCodec/SVT-AV1.git
	fi
	mkdir -p ${LIBSVTAV1}/build && cd ${LIBSVTAV1}/build
	PATH="$BUILD/bin:$PATH" cmake -G "Unix Makefiles" -DCMAKE_INSTALL_PREFIX="$BUILD" -DCMAKE_BUILD_TYPE=Release -DBUILD_DEC=OFF -DBUILD_SHARED_LIBS=OFF ..
	PATH="$BUILD/bin:$PATH" make && make install
	cd ${FFMPEGDIR}
}

build_dav1d()
{
	# libdav1d
	# AV1 decoder, much faster than the one provided by libaom.
	# Requires ffmpeg to be configured with --enable-libdav1d. 
	# sudo apt-get install libdav1d-dev
	# sudo apt-get install python3-pip && pip3 install --user meson
	if [ ! -e ${LIBDAV1D} ];then
		git -C dav1d pull 2> /dev/null || git clone --depth 1 https://code.videolan.org/videolan/dav1d.git
	fi
	mkdir -p ${LIBDAV1D}/build && cd ${LIBDAV1D}/build
	PATH="$BUILD/bin:$PATH" meson setup -Denable_tools=false -Denable_tests=false --default-library=static .. --prefix "$BUILD" --libdir="$BUILD/lib"
	ninja && ninja install
	cd ${FFMPEGDIR}
}

build_vmaf()
{
	# libvmaf
	# Library for calculating the ​VMAF video quality metric. Requires ffmpeg to be configured with --enable-libvmaf. Currently ​an issue in libvmaf also requires FFmpeg to be built with --ld="g++" for a static build to succeed.
	if [ ! -e ${LIBVMAFTAR} ];then
		wget https://github.com/Netflix/vmaf/archive/v2.1.1.tar.gz
	fi
	if [ ! -e ${LIBVMAF} ];then
		tar xvf ${LIBVMAFTAR}
	fi
	mkdir -p ${LIBVMAF}/libvmaf/build && cd ${LIBVMAF}/libvmaf/build
	./configure --prefix="$BUILD" --bindir="$BUILD/bin"
	PATH="$BUILD/bin:$PATH" meson setup -Denable_tests=false -Denable_docs=false --buildtype=release --default-library=static .. --prefix "$BUILD" --bindir="$BUILD/bin" --libdir="$BUILD/lib"
	ninja && ninja install
	cd ${FFMPEGDIR}
}

build_ffmpeg()
{
	if [ ! -e ${FFMPEGTAR} ];then
		wget -O ffmpeg-snapshot.tar.bz2 https://ffmpeg.org/releases/ffmpeg-snapshot.tar.bz2
	fi
	if [ ! -e ${FFMPEG} ];then
		tar xjvf ${FFMPEGTAR}
	fi
	cd ${FFMPEG}

	PATH="$BUILD/bin:$PATH" PKG_CONFIG_PATH="$BUILD/lib/pkgconfig" ./configure \
	--prefix="$BUILD" \
	--pkg-config-flags="--static" \
	--extra-cflags="-I$BUILD/include" \
	--extra-ldflags="-L$BUILD/lib" \
	--extra-libs="-lpthread -lm" \
	--ld="g++" \
	--bindir="$BUILD/bin" \
	--enable-gpl \
	--enable-gnutls \
	--enable-libaom \
	--enable-libass \
	--enable-libfdk-aac \
	--enable-libfreetype \
	--enable-libmp3lame \
	--enable-libopus \
	--enable-libsvtav1 \
	--enable-libdav1d \
	--enable-libvorbis \
	--enable-libvpx \
	--enable-libx264 \
	--enable-libx265 \
	--enable-nonfree
	PATH="$BUILD/bin:$PATH" make &&	make install && hash -r
	# make distclean

	cd ${FFMPEGDIR}
}

build()
{
	# sudo apt-get update -qq && sudo apt-get -y install \
	# autoconf \
	# automake \
	# build-essential \
	# cmake \
	# git-core \
	# libass-dev \
	# libfreetype6-dev \
	# libgnutls28-dev \
	# libmp3lame-dev \
	# libsdl2-dev \
	# libtool \
	# libva-dev \
	# libvdpau-dev \
	# libvorbis-dev \
	# libxcb1-dev \
	# libxcb-shm0-dev \
	# libxcb-xfixes0-dev \
	# meson \
	# ninja-build \
	# pkg-config \
	# texinfo \
	# wget \
	# yasm \
	# zlib1g-dev

	mkdir -p ffmpeg_build
	cd ${FFMPEGDIR}

	build_yasm
	build_nasm
	build_x264
	build_x265
	build_vpx
	build_fdk-aac
	build_opus
	build_aom
	build_svtav1
	build_dav1d
	build_vmaf
	build_ffmpeg
}

if [ "$1" == "clean" ];then
	clean
elif [ "$1" == "" ];then
	build
else
	build_"$1"
fi

