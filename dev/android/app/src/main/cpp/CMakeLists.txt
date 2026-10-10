cmake_minimum_required(VERSION 3.22)
project(olcpixelgameengine3)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

option(USE_STB "Use stb_image.h for image loading" OFF)

add_library(native_app_glue STATIC
        ${ANDROID_NDK}/sources/android/native_app_glue/android_native_app_glue.c)
target_include_directories(native_app_glue PUBLIC
        ${ANDROID_NDK}/sources/android/native_app_glue)

set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -u ANativeActivity_onCreate")

file(GLOB_RECURSE SOURCES "*.cpp" "*.h")
add_library(${PROJECT_NAME} SHARED ${SOURCES})

if(USE_STB)
    target_compile_definitions(${PROJECT_NAME} PRIVATE OLC_USE_STB_IMAGE=1)
endif()

target_link_libraries(${PROJECT_NAME}
        android
        native_app_glue
        log
        EGL
        GLESv3
        jnigraphics
)