cmake_minimum_required(VERSION 3.19)
project(PGE_Examples)

# Set C++ Standards
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

option(BUILD_WAYLAND "Build example programs with Wayland" OFF)
option(USE_STB "Use stb_image.h for image loading" OFF)

######################################################################
# Directories
file(GLOB SOURCES "*.cpp")

if(EMSCRIPTEN)
    # build Cache: libpng, zlib
    execute_process(COMMAND "${EMSCRIPTEN_ROOT_PATH}/embuilder${EMCC_SUFFIX}" build libpng zlib)
    set(CMAKE_EXECUTABLE_SUFFIX .html)
endif()

if(UNIX AND BUILD_WAYLAND)
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(XKBCOMMON REQUIRED xkbcommon)
    pkg_check_modules(WAYLAND_CLIENT REQUIRED wayland-client)
    pkg_check_modules(WAYLAND_EGL REQUIRED wayland-egl)
    pkg_check_modules(EGL REQUIRED egl)

    find_package(OpenGL REQUIRED)
    find_package(PNG REQUIRED)
    find_package(Threads REQUIRED)

    # Find wayland-scanner
    find_program(WAYLAND_SCANNER wayland-scanner REQUIRED)

    # Check for wayland protocols
    set(XDG_SHELL_PROTOCOL "/usr/share/wayland-protocols/stable/xdg-shell/xdg-shell.xml")
    set(XDG_DECORATION_PROTOCOL "/usr/share/wayland-protocols/unstable/xdg-decoration/xdg-decoration-unstable-v1.xml")
    set(POINTER_WARP_PROTOCOL "/usr/share/wayland-protocols/staging/pointer-warp/pointer-warp-v1.xml")
    set(CURSOR_SHAPE_PROTOCOL "/usr/share/wayland-protocols/staging/cursor-shape/cursor-shape-v1.xml")
    set(TABLET_PROTOCOL "/usr/share/wayland-protocols/stable/tablet/tablet-v2.xml")

    if(NOT EXISTS ${XDG_SHELL_PROTOCOL})
        message(FATAL_ERROR "xdg-shell protocol not found. You may need to install wayland-protocols via your package manager")
    endif()

    if(NOT EXISTS ${XDG_DECORATION_PROTOCOL})
        message(FATAL_ERROR "xdg-decoration protocol not found. You may need to install wayland-protocols via your package manager")
    endif()

    if(NOT EXISTS ${POINTER_WARP_PROTOCOL})
        message(FATAL_ERROR "pointer-warp protocol not found. You may need to install wayland-protocols via your package manager")
    endif()

    if(NOT EXISTS ${CURSOR_SHAPE_PROTOCOL})
        message(FATAL_ERROR "cursor-shape protocol not found. You may need to install wayland-protocols via your package manager")
    endif()

    if(NOT EXISTS ${TABLET_PROTOCOL})
       message(FATAL_ERROR "tablet protocol not found. You may need to install wayland-protocols via your package manager")
    endif()

    # Generate wayland protocol files
    set(XDG_SHELL_C ${CMAKE_CURRENT_BINARY_DIR}/xdg-shell.c)
    set(XDG_SHELL_H ${CMAKE_CURRENT_BINARY_DIR}/xdg-shell.h)
    set(XDG_DECORATION_C ${CMAKE_CURRENT_BINARY_DIR}/xdg-decoration.c)
    set(XDG_DECORATION_H ${CMAKE_CURRENT_BINARY_DIR}/xdg-decoration.h)
    set(POINTER_WARP_C ${CMAKE_CURRENT_BINARY_DIR}/pointer-warp.c)
    set(POINTER_WARP_H ${CMAKE_CURRENT_BINARY_DIR}/pointer-warp.h)
    set(CURSOR_SHAPE_C ${CMAKE_CURRENT_BINARY_DIR}/cursor-shape.c)
    set(CURSOR_SHAPE_H ${CMAKE_CURRENT_BINARY_DIR}/cursor-shape.h)
    set(TABLET_C ${CMAKE_CURRENT_BINARY_DIR}/tablet.c)
    set(TABLET_H ${CMAKE_CURRENT_BINARY_DIR}/tablet.h)

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

    add_custom_command(
        OUTPUT ${POINTER_WARP_C}
        COMMAND ${WAYLAND_SCANNER} private-code ${POINTER_WARP_PROTOCOL} ${POINTER_WARP_C}
        DEPENDS ${POINTER_WARP_PROTOCOL}
        VERBATIM
    )

    add_custom_command(
        OUTPUT ${POINTER_WARP_H}
        COMMAND ${WAYLAND_SCANNER} client-header ${POINTER_WARP_PROTOCOL} ${POINTER_WARP_H}
        DEPENDS ${POINTER_WARP_PROTOCOL}
        VERBATIM
    )

    add_custom_command(
        OUTPUT ${CURSOR_SHAPE_C}
        COMMAND ${WAYLAND_SCANNER} private-code ${CURSOR_SHAPE_PROTOCOL} ${CURSOR_SHAPE_C}
        DEPENDS ${CURSOR_SHAPE_PROTOCOL}
        VERBATIM
    )

    add_custom_command(
        OUTPUT ${CURSOR_SHAPE_H}
        COMMAND ${WAYLAND_SCANNER} client-header ${CURSOR_SHAPE_PROTOCOL} ${CURSOR_SHAPE_H}
        DEPENDS ${CURSOR_SHAPE_PROTOCOL}
        VERBATIM
    )

    add_custom_command(
        OUTPUT ${TABLET_C}
        COMMAND ${WAYLAND_SCANNER} private-code ${TABLET_PROTOCOL} ${TABLET_C}
        DEPENDS ${TABLET_PROTOCOL}
        VERBATIM
    )

    add_custom_command(
        OUTPUT ${TABLET_H}
        COMMAND ${WAYLAND_SCANNER} client-header ${TABLET_PROTOCOL} ${TABLET_H}
        DEPENDS ${TABLET_PROTOCOL}
        VERBATIM
    )

    # Create a library for the wayland protocol files
    add_library(wayland_protocols STATIC
        ${XDG_SHELL_C}
        ${XDG_SHELL_H}
        ${XDG_DECORATION_C}
        ${XDG_DECORATION_H}
        ${POINTER_WARP_C}
        ${POINTER_WARP_H}
        ${CURSOR_SHAPE_C}
        ${CURSOR_SHAPE_H}
        ${TABLET_C}
        ${TABLET_H}
    )

    target_include_directories(wayland_protocols PUBLIC
        ${CMAKE_CURRENT_BINARY_DIR}
    )
endif()

foreach(source ${SOURCES})
    get_filename_component(EXE_NAME ${source} NAME_WE)
    add_executable(${EXE_NAME} ${source})
    target_link_libraries(${EXE_NAME} PRIVATE olcPixelGameEngine3)
    
    if(USE_STB)
        target_compile_definitions(${EXE_NAME} PRIVATE OLC_USE_STB_IMAGE=1)
    endif()

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
    endif()
    
    if(UNIX AND NOT APPLE AND NOT EMSCRIPTEN)
        if(BUILD_WAYLAND)
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
        endif()
        
    endif()

    if(WIN32 AND NOT MSVC AND NOT EMSCRIPTEN)
        target_link_libraries(${EXE_NAME} PRIVATE user32)
        target_link_libraries(${EXE_NAME} PRIVATE gdi32)
        target_link_libraries(${EXE_NAME} PRIVATE dwmapi)
        target_link_libraries(${EXE_NAME} PRIVATE gdiplus)
        target_link_libraries(${EXE_NAME} PRIVATE shlwapi)
        target_link_libraries(${EXE_NAME} PRIVATE opengl32)

        target_compile_options(${EXE_NAME} PRIVATE -fmax-errors=5)
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
        target_link_options(${EXE_NAME} PRIVATE --preload-file ${CMAKE_CURRENT_SOURCE_DIR}/assets@assets)
    endif()

    if(NOT EMSCRIPTEN)
        # Copy assets when not Web platform
        add_custom_command(
            TARGET ${EXE_NAME}
            POST_BUILD
            COMMAND ${CMAKE_COMMAND}
            ARGS -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/assets ${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_CFG_INTDIR}/assets
        )        
    endif()
endforeach()
