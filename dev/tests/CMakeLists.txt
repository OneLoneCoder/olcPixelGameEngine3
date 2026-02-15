cmake_minimum_required(VERSION 3.19)
project(PGE_MultiHeader)

# Set C++ Standards
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

option(BUILD_WAYLAND "Build example programs with Wayland" OFF)

# Source files that are common to all platforms
set(COMMON_SOURCE_FILES
    test_mh.cpp
    ../src/core.cpp
    ../src/core.h
    ../src/api_opengl.cpp
    ../src/api_opengl.h
    ../src/config.h
    ../src/draw.cpp
    ../src/draw.h
    ../src/font.cpp
    ../src/font.h
    ../src/gpu_iface.cpp
    ../src/gpu_iface.h
    ../src/gpu_opengl33.cpp
    ../src/gpu_opengl33.h
    ../src/gputask.h
    ../src/host_iface.h
    ../src/hw_input.h
    ../src/hw_keyboard.cpp
    ../src/hw_keyboard.h
    ../src/hw_mouse.cpp
    ../src/hw_mouse.h
    ../src/hw_touch.h
    ../src/image.cpp
    ../src/image.h
    ../src/imload_iface.h
    ../src/matrix3d.h
    ../src/matrix4d.h
    ../src/olcpge3.h
    ../src/pixel.h
    ../src/polyform.h
    ../src/transform2d.h
    ../src/vector2d.h
    ../src/window.cpp
    ../src/window.h
)

# Platform specific sources
if(ANDROID)
    set(HOST_SOURCE_FILES ../src/host_android.cpp ../src/host_android.h)
endif() # Android

if(APPLE)
    if(CMAKE_SYSTEM_NAME STREQUAL "iOS")
        # TODO: Building for iOS
    else()
        set(HOST_SOURCE_FILES
            ../src/api_macos.cpp
            ../src/api_macos.h
            ../src/api_macos_wrapper.hpp
            ../src/host_apple_macos.cpp
            ../src/host_apple_macos.h
        )
    endif() # MacOS
endif() # Apple

if(UNIX AND NOT APPLE AND NOT EMSCRIPTEN)
    if(BUILD_WAYLAND) # wayland
        set(HOST_SOURCE_FILES
            ../src/host_lin_wayland.cpp
            ../src/host_lin_wayland.h
        )
    else() # x11
        set(HOST_SOURCE_FILES
            ../src/host_lin_x11.cpp
            ../src/host_lin_x11.h
        )
    endif()
endif() # LINUX

if(EMSCRIPTEN)
    set(HOST_SOURCE_FILES
        ../src/host_web_emscripten.cpp
        ../src/host_web_emscripten.h
    )
endif() # emscripten

if(WIN32 AND NOT EMSCRIPTEN)
    set(HOST_SOURCE_FILES
        ../src/host_win_winapi.cpp
        ../src/host_win_winapi.h
    )
endif() # windows

# Image Loader source files
if(USE_STB)
    # TODO: stb related
else()
    if(ANDROID)
        set(IMAGELOADER_SOURCE_FILES
            ../src/imload_android.cpp
            ../src/imload_android.h
        )
    endif()

    if(APPLE)
        if(CMAKE_SYSTEM_NAME STREQUAL "iOS")
            # TODO: Building for iOS
        else()
            set(IMAGELOADER_SOURCE_FILES
                ../src/imload_macos.cpp
                ../src/imload_macos.h
            )
        endif()
    endif()

    if(UNIX AND NOT APPLE AND NOT EMSCRIPTEN)
        set(IMAGELOADER_SOURCE_FILES
            ../src/imload_lib_png.cpp
            ../src/imload_lib_png.h
        )
    endif()

    if(WIN32 AND NOT EMSCRIPTEN)
        set(IMAGELOADER_SOURCE_FILES
            ../src/imload_wingdi.cpp
            ../src/imload_wingdi.h
        )
    endif() 

    if(EMSCRIPTEN)
        set(IMAGELOADER_SOURCE_FILES
            ../src/imload_lib_png.cpp
            ../src/imload_lib_png.h
        )
    endif()
endif()


if(EMSCRIPTEN)
    # build Cache: libpng, zlib
    execute_process(COMMAND "${EMSCRIPTEN_ROOT_PATH}/embuilder${EMCC_SUFFIX}" build libpng zlib)
    set(CMAKE_EXECUTABLE_SUFFIX .html)
endif()

set(EXE_NAME test_mh)

add_executable(${EXE_NAME} ${HOST_SOURCE_FILES} ${IMAGELOADER_SOURCE_FILES} ${COMMON_SOURCE_FILES})

target_include_directories(${EXE_NAME} PRIVATE ../src)

if(APPLE)
    if(CMAKE_SYSTEM_NAME STREQUAL "iOS")
        # TODO: Building for iOS
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
        ARGS -E copy_directory ${CMAKE__SOURCE_DIR}/examples/assets ${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_CFG_INTDIR}/assets
    )        
endif()



