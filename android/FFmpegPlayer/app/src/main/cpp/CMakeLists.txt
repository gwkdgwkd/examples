cmake_minimum_required(VERSION 3.10.2)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=gnu++11")

set(jnilibs ${CMAKE_SOURCE_DIR}/lib)
set(libname ffmpegplayer)
include_directories(
        include
        ${CMAKE_SOURCE_DIR}/util
)
link_directories(${jnilibs}/${ANDROID_ABI})
file(GLOB src-files ${CMAKE_SOURCE_DIR}/*.cpp)

project("ffmpegplayer")
add_library(${libname} SHARED ${src-files})

find_library(
        log-lib
        log)
set(third-party-libs
        avformat
        avcodec
        avfilter
        swresample
        swscale
        avutil
        )
set(native-libs
        android
        EGL
        GLESv3
        OpenSLES
        log
        m
        z
        )

target_link_libraries(${libname} ${log-lib} ${third-party-libs} ${native-libs})