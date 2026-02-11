cmake_minimum_required(VERSION 3.20)
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

if(UNIX AND (BUILD_DRM OR BUILD_WAYLAND))
    # Find required packages
    find_package(PkgConfig REQUIRED)

    pkg_check_modules(XKBCOMMON REQUIRED IMPORTED_TARGET xkbcommon)
    pkg_check_modules(EGL REQUIRED IMPORTED_TARGET egl)
    find_package(Threads REQUIRED)
    
    if(NOT USE_STB)
        find_package(PNG REQUIRED)
    endif()
    
    if(BUILD_DRM)    
        pkg_check_modules(DRM REQUIRED IMPORTED_TARGET libdrm)
        pkg_check_modules(GBM REQUIRED IMPORTED_TARGET gbm)
        pkg_check_modules(GLES REQUIRED IMPORTED_TARGET glesv2)
        pkg_check_modules(LIBINPUT REQUIRED IMPORTED_TARGET libinput)
        pkg_check_modules(LIBUDEV REQUIRED IMPORTED_TARGET libudev)
    endif()

    if(BUILD_WAYLAND)
        pkg_check_modules(WAYLAND_CLIENT REQUIRED IMPORTED_TARGET wayland-client)
        pkg_check_modules(WAYLAND_EGL REQUIRED IMPORTED_TARGET wayland-egl)

        find_package(OpenGL REQUIRED)
    
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
    endif()


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

            target_link_libraries(${EXE_NAME} PRIVATE wayland_protocols)
            target_link_libraries(${EXE_NAME} PRIVATE ${XKBCOMMON_LIBRARIES})
            target_link_libraries(${EXE_NAME} PRIVATE ${WAYLAND_CLIENT_LIBRARIES})
            target_link_libraries(${EXE_NAME} PRIVATE ${WAYLAND_EGL_LIBRARIES})
            target_link_libraries(${EXE_NAME} PRIVATE ${EGL_LIBRARIES})
            target_link_libraries(${EXE_NAME} PRIVATE OpenGL::GL)
            target_link_libraries(${EXE_NAME} PRIVATE Threads::Threads)
            
            if(NOT USE_STB)
                target_link_libraries(${EXE_NAME} PRIVATE PNG::PNG)
            endif()
        elseif(BUILD_DRM)
            target_compile_definitions(${EXE_NAME} PRIVATE OLC_HOST=8)
            
            target_link_libraries(${EXE_NAME} PRIVATE PkgConfig::DRM)
            target_link_libraries(${EXE_NAME} PRIVATE PkgConfig::GBM)
            target_link_libraries(${EXE_NAME} PRIVATE PkgConfig::EGL)
            target_link_libraries(${EXE_NAME} PRIVATE PkgConfig::GLES)
            target_link_libraries(${EXE_NAME} PRIVATE PkgConfig::LIBINPUT)
            target_link_libraries(${EXE_NAME} PRIVATE PkgConfig::LIBUDEV)
            target_link_libraries(${EXE_NAME} PRIVATE PkgConfig::XKBCOMMON)
            target_link_libraries(${EXE_NAME} PRIVATE Threads::Threads)

            if(NOT USE_STB)
                target_link_libraries(${EXE_NAME} PRIVATE PNG::PNG)
            endif()

        else() # x11
            target_link_libraries(${EXE_NAME} PRIVATE GL)
            target_link_libraries(${EXE_NAME} PRIVATE pthread)
            target_link_libraries(${EXE_NAME} PRIVATE X11)
            
            if(NOT USE_STB)
                target_link_libraries(${EXE_NAME} PRIVATE png)
            endif()
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
