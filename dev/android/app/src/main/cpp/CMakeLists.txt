cmake_minimum_required(VERSION 3.22)
project(game_native)

add_library(native_app_glue STATIC
        ${ANDROID_NDK}/sources/android/native_app_glue/android_native_app_glue.c)
target_include_directories(native_app_glue PUBLIC
        ${ANDROID_NDK}/sources/android/native_app_glue)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -u ANativeActivity_onCreate")
add_library(${PROJECT_NAME} SHARED game.cpp)

target_link_libraries(${PROJECT_NAME}
        android
        native_app_glue
        log
        EGL
        GLESv3
        jnigraphics
)
