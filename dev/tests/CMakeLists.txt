cmake_minimum_required(VERSION 3.19)
project(PGE_MultiHeader)

# Set C++ Standards
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

option(BUILD_WAYLAND "Build example programs with Wayland" OFF)
option(USE_HEADLESS "Build in headless mode" OFF)

# GPU specific sources
set(GPU_HEADLESS_SOURCE_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/gpu_none.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/gpu_none.h
)

set(GPU_OPENGL33_SOURCE_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/api_opengl.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/api_opengl.h
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/gpu_opengl33.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/gpu_opengl33.h
)

# Platform specific sources
set(HOST_HEADLESS_SOURCE_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/host_none.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/host_none.h
)
set(HOST_ANDROID_SOURCE_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/host_android.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/host_android.h
)

set(MACOS_API_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/api_macos.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/api_macos.h
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/api_macos_wrapper.hpp
)

set(HOST_MACOS_SOURCE_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/host_apple_macos.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/host_apple_macos.h
)
set(HOST_LINUX_WAYLAND_SOURCE_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/host_lin_wayland.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/host_lin_wayland.h
)
set(HOST_LINUX_X11_SOURCE_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/host_lin_x11.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/host_lin_x11.h
)
set(HOST_WEB_EMSCRIPTEN_SOURCE_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/host_web_emscripten.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/host_web_emscripten.h
)
set(HOST_WIN_WINAPI_SOURCE_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/host_win_winapi.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/host_win_winapi.h
)
set(HOST_WXWIDGETS_SOURCE_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/host_wxwidgets.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/host_wdwidgets.h
)
set(IMAGELOADER_ANDROID_SOURCE_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/imload_android.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/imload_android.h
)
set(IMAGELOADER_LIBPNG_SOURCE_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/imload_lib_png.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/imload_lib_png.h
)
set(IMAGELOADER_MACOS_SOURCE_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/imload_macos.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/imload_macos.h
)
set(IMAGELOADER_STB_IMAGE_SOURCE_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/imload_stb_image.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/imload_stb_image.h
)
set(IMAGELOADER_WINGDI_SOURCE_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/imload_wingdi.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/imload_wingdi.h
)

file(
    GLOB SOURCES
    ../src/*.c
    ../src/*.cpp
    ../src/*.h
    ../src/*.hpp
)
list(
    REMOVE_ITEM SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/sh_template.h
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/olcpge3.h
    ${GPU_HEADLESS_SOURCE_FILES}
    ${GPU_OPENGL33_SOURCE_FILES}
    ${HOST_HEADLESS_SOURCE_FILES}
    ${HOST_ANDROID_SOURCE_FILES}
    ${MACOS_API_FILES}
    ${HOST_MACOS_SOURCE_FILES}
    ${HOST_LINUX_WAYLAND_SOURCE_FILES}
    ${HOST_LINUX_X11_SOURCE_FILES}
    ${HOST_WEB_EMSCRIPTEN_SOURCE_FILES}
    ${HOST_WIN_WINAPI_SOURCE_FILES}
    ${HOST_WXWIDGETS_SOURCE_FILES}
    ${IMAGELOADER_ANDROID_SOURCE_FILES}
    ${IMAGELOADER_MACOS_SOURCE_FILES}
    ${IMAGELOADER_LIBPNG_SOURCE_FILES}
    ${IMAGELOADER_STB_IMAGE_SOURCE_FILES}
    ${IMAGELOADER_WINGDI_SOURCE_FILES}
)
    
if(USE_HEADLESS)
    set(HOST_SOURCE_FILES ${HOST_HEADLESS_SOURCE_FILES})
    set(GPU_SOURCE_FILES ${GPU_HEADLESS_SOURCE_FILES})
else()
    
    set(GPU_SOURCE_FILES ${GPU_OPENGL33_SOURCE_FILES})
    
    if(ANDROID)
        set(HOST_SOURCE_FILES ${HOST_ANDROID_SOURCE_FILES})
    endif() # Android

    if(APPLE)
        if(CMAKE_SYSTEM_NAME STREQUAL "iOS")
            # TODO: Building for iOS
        else()
            if(NOT USE_HEADLESS)
                set(HOST_SOURCE_FILES ${HOST_MACOS_SOURCE_FILES} ${MACOS_API_FILES})
            else()
                set(HOST_SOURCE_FILES ${HOST_MACOS_SOURCE_FILES})
            endif()
        endif() # MacOS
    endif() # Apple

    if(UNIX AND NOT APPLE AND NOT EMSCRIPTEN)
        if(BUILD_WAYLAND) # wayland
            set(HOST_SOURCE_FILES ${HOST_LINUX_WAYLAND_SOURCE_FILES})
        else() # x11
            set(HOST_SOURCE_FILES ${HOST_LINUX_X11_SOURCE_FILES})
        endif()
    endif() # LINUX

    if(EMSCRIPTEN)
        set(HOST_SOURCE_FILES ${HOST_WEB_EMSCRIPTEN_SOURCE_FILES})
    endif() # emscripten

    if(WIN32 AND NOT EMSCRIPTEN)
        set(HOST_SOURCE_FILES ${HOST_WIN_WINAPI_SOURCE_FILES})
    endif() # windows
endif()

# Image Loader source files
if(USE_STB)
    set(IMAGELOADER_SOURCE_FILES ${IMAGELOADER_STB_IMAGE_SOURCE_FILES})
else()
    if(ANDROID)
        set(IMAGELOADER_SOURCE_FILES ${IMAGELOADER_ANDROID_SOURCE_FILES})
    endif()

    if(APPLE)
        if(CMAKE_SYSTEM_NAME STREQUAL "iOS")
            # TODO: Building for iOS
        else()
            if(USE_HEADLESS)
                set(IMAGELOADER_SOURCE_FILES ${IMAGELOADER_MACOS_SOURCE_FILES} ${MACOS_API_FILES})
            else()
                set(IMAGELOADER_SOURCE_FILES ${IMAGELOADER_MACOS_SOURCE_FILES})
            endif()
        endif()
    endif()

    if(UNIX AND NOT APPLE AND NOT EMSCRIPTEN)
        set(IMAGELOADER_SOURCE_FILES ${IMAGELOADER_LIBPNG_SOURCE_FILES})
    endif()

    if(WIN32 AND NOT EMSCRIPTEN)
        set(IMAGELOADER_SOURCE_FILES ${IMAGELOADER_WINGDI_SOURCE_FILES})
    endif() 

    if(EMSCRIPTEN)
        set(IMAGELOADER_SOURCE_FILES ${IMAGELOADER_LIBPNG_SOURCE_FILES})
    endif()
endif()

# Instruct our helper that we want to use the multi-header
set(MULTI_HEADER true)

# See: cmake/Helpers.cmake
add_example(test_mh test_mh.cpp ${HOST_SOURCE_FILES} ${GPU_SOURCE_FILES} ${IMAGELOADER_SOURCE_FILES} ${SOURCES})

# Include the example assets, for convenient usage in our test
# See: cmake/Helpers.cmake
handle_assets(test_mh ${CMAKE_SOURCE_DIR}/examples/assets)

target_include_directories(test_mh PRIVATE ../src)

