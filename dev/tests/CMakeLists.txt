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

set(IOS_API_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/api_ios.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/api_ios.h
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/api_ios_wrapper.hpp
)

set(HOST_IOS_SOURCE_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/host_apple_ios.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/host_apple_ios.h
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
set(IMAGELOADER_IOS_SOURCE_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/imload_ios.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/imload_ios.h
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
    ${IOS_API_FILES}
    ${HOST_IOS_SOURCE_FILES}
    ${HOST_LINUX_WAYLAND_SOURCE_FILES}
    ${HOST_LINUX_X11_SOURCE_FILES}
    ${HOST_WEB_EMSCRIPTEN_SOURCE_FILES}
    ${HOST_WIN_WINAPI_SOURCE_FILES}
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
            if(NOT USE_HEADLESS)
                set(HOST_SOURCE_FILES ${HOST_IOS_SOURCE_FILES} ${IOS_API_FILES})
            else()
                set(HOST_SOURCE_FILES ${HOST_IOS_SOURCE_FILES})
            endif() # iOS
        else()
            if(NOT USE_HEADLESS)
                set(HOST_SOURCE_FILES ${HOST_MACOS_SOURCE_FILES} ${MACOS_API_FILES})
            else()
                set(HOST_SOURCE_FILES ${HOST_MACOS_SOURCE_FILES})
            endif() # MacOS
        endif() 
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
            if(USE_HEADLESS)
                set(IMAGELOADER_SOURCE_FILES ${IMAGELOADER_IOS_SOURCE_FILES} ${IOS_API_FILES})
            else()
                set(IMAGELOADER_SOURCE_FILES ${IMAGELOADER_IOS_SOURCE_FILES})
            endif() # iOS
        else()
            if(USE_HEADLESS)
                set(IMAGELOADER_SOURCE_FILES ${IMAGELOADER_MACOS_SOURCE_FILES} ${MACOS_API_FILES})
            else()
                set(IMAGELOADER_SOURCE_FILES ${IMAGELOADER_MACOS_SOURCE_FILES})
            endif() # MacOS
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

if(EMSCRIPTEN)
    # build Cache: libpng, zlib
    execute_process(COMMAND "${EMSCRIPTEN_ROOT_PATH}/embuilder${EMCC_SUFFIX}" build libpng zlib)
    set(CMAKE_EXECUTABLE_SUFFIX .html)
endif()

set(EXE_NAME test_mh)

add_executable(${EXE_NAME} test_mh.cpp ${HOST_SOURCE_FILES} ${GPU_SOURCE_FILES} ${IMAGELOADER_SOURCE_FILES} ${SOURCES})

target_include_directories(${EXE_NAME} PRIVATE ../src)

if(USE_HEADLESS)
    target_compile_definitions(${EXE_NAME} PRIVATE OLC_USE_HEADLESS=1)
endif()

if(APPLE)
    if(CMAKE_SYSTEM_NAME STREQUAL "iOS")
        target_link_libraries(${EXE_NAME} PRIVATE "-framework AppKit")
        target_link_libraries(${EXE_NAME} PRIVATE "-framework GLKit")
        target_link_libraries(${EXE_NAME} PRIVATE "-framework UIKit")
        target_link_libraries(${EXE_NAME} PRIVATE "-framework Metal")
        target_link_libraries(${EXE_NAME} PRIVATE "-framework MetalKit")
        target_link_libraries(${EXE_NAME} PRIVATE "-framework QuartzCore")
        target_link_libraries(${EXE_NAME} PRIVATE "-framework OpenGLES")
        target_link_libraries(${EXE_NAME} PRIVATE "-framework Foundation")
    else()
        target_link_libraries(${EXE_NAME} PRIVATE "-framework Metal")
        target_link_libraries(${EXE_NAME} PRIVATE "-framework MetalKit")
        target_link_libraries(${EXE_NAME} PRIVATE "-framework AppKit")
        target_link_libraries(${EXE_NAME} PRIVATE "-framework QuartzCore")
        target_link_libraries(${EXE_NAME} PRIVATE "-framework OpenGL")
        target_link_libraries(${EXE_NAME} PRIVATE "-framework Foundation")
    endif()
endif() # APPLE
    
if(UNIX AND NOT APPLE AND NOT EMSCRIPTEN)
    if(BUILD_WAYLAND)
        find_package(PkgConfig REQUIRED)
        pkg_check_modules(XKBCOMMON REQUIRED xkbcommon)
        pkg_check_modules(WAYLAND_CLIENT REQUIRED wayland-client)
        pkg_check_modules(WAYLAND_EGL REQUIRED wayland-egl)
        pkg_check_modules(EGL REQUIRED egl)

        find_package(OpenGL REQUIRED)
        find_package(PNG REQUIRED)
        find_package(Threads REQUIRED)

        if(NOT BUILD_EXAMPLES)
            # Find wayland-scanner
            find_program(WAYLAND_SCANNER wayland-scanner REQUIRED)

            # Check for wayland protocols
            set(XDG_SHELL_PROTOCOL "/usr/share/wayland-protocols/stable/xdg-shell/xdg-shell.xml")
            set(XDG_DECORATION_PROTOCOL "/usr/share/wayland-protocols/unstable/xdg-decoration/xdg-decoration-unstable-v1.xml")

            if(NOT EXISTS ${XDG_SHELL_PROTOCOL})
                message(FATAL_ERROR "xdg-shell protocol not found. You may need to install wayland-protocols via your package manager")
            endif()

            if(NOT EXISTS ${XDG_DECORATION_PROTOCOL})
                message(FATAL_ERROR "xdg-decoration protocol not found. You may need to install wayland-protocols via your package manager")
            endif()

            # Generate wayland protocol files
            set(XDG_SHELL_C ${CMAKE_CURRENT_BINARY_DIR}/xdg-shell.c)
            set(XDG_SHELL_H ${CMAKE_CURRENT_BINARY_DIR}/xdg-shell.h)
            set(XDG_DECORATION_C ${CMAKE_CURRENT_BINARY_DIR}/xdg-decoration.c)
            set(XDG_DECORATION_H ${CMAKE_CURRENT_BINARY_DIR}/xdg-decoration.h)

            add_custom_command(
                OUTPUT ${XDG_SHELL_C}
                COMMAND ${WAYLAND_SCANNER} private-code ${XDG_SHELL_PROTOCOL} ${XDG_SHELL_C}
                DEPENDS ${XDG_SHELL_PROTOCOL}
                VERBATIM
            )

            add_custom_command(
                OUTPUT ${XDG_SHELL_H}
                COMMAND ${WAYLAND_SCANNER} client-header ${XDG_SHELL_PROTOCOL} ${XDG_SHELL_H}
                DEPENDS ${XDG_SHELL_PROTOCOL}
                VERBATIM
            )

            add_custom_command(
                OUTPUT ${XDG_DECORATION_C}
                COMMAND ${WAYLAND_SCANNER} private-code ${XDG_DECORATION_PROTOCOL} ${XDG_DECORATION_C}
                DEPENDS ${XDG_DECORATION_PROTOCOL}
                VERBATIM
            )

            add_custom_command(
                OUTPUT ${XDG_DECORATION_H}
                COMMAND ${WAYLAND_SCANNER} client-header ${XDG_DECORATION_PROTOCOL} ${XDG_DECORATION_H}
                DEPENDS ${XDG_DECORATION_PROTOCOL}
                VERBATIM
            )

            # Create a library for the wayland protocol files
            add_library(wayland_protocols STATIC
                ${XDG_SHELL_C}
                ${XDG_SHELL_H}
                ${XDG_DECORATION_C}
                ${XDG_DECORATION_H}
            )

            target_include_directories(wayland_protocols PUBLIC
                ${CMAKE_CURRENT_BINARY_DIR}
            )
        endif() # if not build_examples
        
        target_compile_definitions(${EXE_NAME} PRIVATE OLC_HOST=3)

        target_link_libraries(${EXE_NAME} PRIVATE
            wayland_protocols
            ${XKBCOMMON_LIBRARIES}
            ${WAYLAND_CLIENT_LIBRARIES}
            ${WAYLAND_EGL_LIBRARIES}
            ${EGL_LIBRARIES}
            OpenGL::GL
            PNG::PNG
            Threads::Threads
        )            

    else() # x11
        target_link_libraries(${EXE_NAME} PRIVATE png)
        target_link_libraries(${EXE_NAME} PRIVATE GL)
        target_link_libraries(${EXE_NAME} PRIVATE pthread)
        target_link_libraries(${EXE_NAME} PRIVATE X11)
    endif()
endif()

if(WIN32 AND NOT MSVC AND NOT EMSCRIPTEN)
    target_link_libraries(${EXE_NAME} PRIVATE user32)
    target_link_libraries(${EXE_NAME} PRIVATE gdi32)
    target_link_libraries(${EXE_NAME} PRIVATE dwmapi)
    target_link_libraries(${EXE_NAME} PRIVATE gdiplus)
    target_link_libraries(${EXE_NAME} PRIVATE shlwapi)
    target_link_libraries(${EXE_NAME} PRIVATE opengl32)
endif()
    
if(EMSCRIPTEN)
    target_link_options(${EXE_NAME} PRIVATE -sASYNCIFY)
    target_link_options(${EXE_NAME} PRIVATE -sALLOW_MEMORY_GROWTH=1)
    target_link_options(${EXE_NAME} PRIVATE -sSTACK_SIZE=1048576)
    target_link_options(${EXE_NAME} PRIVATE -sEXPORTED_RUNTIME_METHODS=HEAPF32)
    target_link_options(${EXE_NAME} PRIVATE -sMAX_WEBGL_VERSION=2)
    target_link_options(${EXE_NAME} PRIVATE -sMIN_WEBGL_VERSION=2)
    target_link_options(${EXE_NAME} PRIVATE -sUSE_LIBPNG=1)
    target_link_options(${EXE_NAME} PRIVATE -sLLD_REPORT_UNDEFINED)
    target_link_options(${EXE_NAME} PRIVATE --preload-file ${CMAKE_SOURCE_DIR}/examples/assets@assets)
endif()

if(NOT EMSCRIPTEN)
    # Copy assets when not Web platform
    add_custom_command(
        TARGET ${EXE_NAME}
        POST_BUILD
        COMMAND ${CMAKE_COMMAND}
        ARGS -E copy_directory ${CMAKE_SOURCE_DIR}/examples/assets ${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_CFG_INTDIR}/assets
    )        
endif()



