cmake_minimum_required(VERSION 3.10.2)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=gnu++11")

set(jnilibs ${CMAKE_SOURCE_DIR}/lib)
set(libname ffmpegplayer)
include_directories(
        include
        ${CMAKE_SOURCE_DIR}/utils
        ${CMAKE_SOURCE_DIR}/opensles
)
link_directories(${jnilibs}/${ANDROID_ABI})
file(GLOB src-files ${CMAKE_SOURCE_DIR}/*.cpp)
file(GLOB utils-files ${CMAKE_SOURCE_DIR}/utils/*.cpp)
file(GLOB audio-render-files ${CMAKE_SOURCE_DIR}/render/audio/*.cpp)
file(GLOB video-render-files ${CMAKE_SOURCE_DIR}/render/video/*.cpp)

project("ffmpegplayer")
add_library(${libname}
        SHARED
        ${src-files}
        ${utils-files}
        ${audio-render-files}
        ${video-render-files})

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