# For more information about using CMake with Android Studio, read the
# documentation: https://d.android.com/studio/projects/add-native-code.html

# Sets the minimum version of CMake required to build the native library.

cmake_minimum_required(VERSION 3.10.2)
# set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O3 -std=c99")
# Declares and names the project.

project("opengles")

# https://blog.csdn.net/baidu_35757025/article/details/78409949
# https://blog.csdn.net/tom_221x/article/details/61210028
include_directories(
        src/main/cpp/libpng
)

add_definitions(-D__ANDROID__)
add_library(
        png STATIC

        libpng/png.c
        libpng/pngget.c
        libpng/pngread.c
        libpng/pngrutil.c
        libpng/pngtrans.c
        libpng/pngwtran.c
        libpng/pngmem.c
        libpng/pngrio.c
        libpng/pngset.c
        libpng/pngwio.c
        libpng/pngwutil.c
        libpng/pngerror.c
        libpng/pngpread.c
        libpng/pngrtran.c
        libpng/pngwrite.c
        libpng/image.c
)
set_target_properties(
        png
        PROPERTIES
        ARCHIVE_OUTPUT_DIRECTORY
        "${CMAKE_CURRENT_SOURCE_DIR}/${ANDROID_ABI}"
)
target_link_libraries(
        png
        z
)


# Creates and names a library, sets it as either STATIC
# or SHARED, and provides the relative paths to its source code.
# You can define multiple libraries, and CMake builds them for you.
# Gradle automatically packages shared libraries with your APK.
add_library( # Sets the name of the library.
        opengles

        # Sets the library as a shared library.
        SHARED

        # Provides a relative path to your source file(s).
        com_example_opengles_ShowPngActivity.cpp
        wapped_shader_program.cpp
        egl_core.cpp
        wapped_texture.cpp
        show_png_conductor.cpp
        png_decoder.cpp
        )


# Searches for a specified prebuilt library and stores the path as a
# variable. Because CMake includes system libraries in the search path by
# default, you only need to specify the name of the public NDK library
# you want to add. CMake verifies that the library exists before
# completing its build.
find_library( # Sets the name of the path variable.
        log-lib

        # Specifies the name of the NDK library that
        # you want CMake to locate.
        log
        )

# Specifies libraries CMake should link to your target library. You
# can link multiple libraries, such as libraries you define in this
# build script, prebuilt third-party libraries, or system libraries.
target_link_libraries( # Specifies the target library.
        opengles

        GLESv2
        EGL
        png
        android

        # Links the target library to the log library
        # included in the NDK.
        ${log-lib})