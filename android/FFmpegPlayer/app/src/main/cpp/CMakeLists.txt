cmake_minimum_required(VERSION 3.10.2)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=gnu++11")

set(jnilibs ${CMAKE_SOURCE_DIR}/lib)
set(libname ffmpegplayer)
include_directories(
        include
        glm
        interface
        ${CMAKE_SOURCE_DIR}/utils
        ${CMAKE_SOURCE_DIR}/ffmpeg
        ${CMAKE_SOURCE_DIR}/opensles
        ${CMAKE_SOURCE_DIR}/egl
        ${CMAKE_SOURCE_DIR}/opengles
)
link_directories(${jnilibs}/${ANDROID_ABI})
file(GLOB src-files ${CMAKE_SOURCE_DIR}/*.cpp)
file(GLOB ffmpeg-files ${CMAKE_SOURCE_DIR}/ffmpeg/*.cpp)
file(GLOB audio-render-files ${CMAKE_SOURCE_DIR}/render/audio/*.cpp)
file(GLOB video-render-files ${CMAKE_SOURCE_DIR}/render/video/*.cpp)
file(GLOB utils-files ${CMAKE_SOURCE_DIR}/utils/*.cpp)
file(GLOB opensles-files ${CMAKE_SOURCE_DIR}/opensles/*.cpp)
file(GLOB egl-files ${CMAKE_SOURCE_DIR}/egl/*.cpp)
file(GLOB opengles-files ${CMAKE_SOURCE_DIR}/opengles/*.cpp)
file(GLOB scale-files ${CMAKE_SOURCE_DIR}/scale/*.cpp)

project("ffmpegplayer")
add_definitions("-Wall -lpthread -g")
add_library(${libname}
        SHARED
        ${src-files}
        ${utils-files}
        ${audio-render-files}
        ${video-render-files}
        ${ffmpeg-files}
        ${opensles-files}
        ${opengles-files}
        ${egl-files}
        ${scale-files})

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
        yuv_static
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