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

    function(generate_wayland_protocol PROTOCOL_TARGET PROTOCOL_FILE)
        if(NOT EXISTS ${PROTOCOL_FILE})
            message(FATAL_ERROR ${PROTOCOL_FILE} " not found. You may need to install wayland-protocols via your package manager")
        endif()

        set(C_FILENAME "${CMAKE_CURRENT_BINARY_DIR}/${PROTOCOL_TARGET}.c")
        set(H_FILENAME "${CMAKE_CURRENT_BINARY_DIR}/${PROTOCOL_TARGET}.h")

        add_custom_command(
            OUTPUT ${C_FILENAME}
            COMMAND ${WAYLAND_SCANNER} private-code ${PROTOCOL_FILE} ${C_FILENAME}
            DEPENDS ${PROTOCOL_FILE}
            VERBATIM
        )

        add_custom_command(
            OUTPUT ${H_FILENAME}
            COMMAND ${WAYLAND_SCANNER} client-header ${PROTOCOL_FILE} ${H_FILENAME}
            DEPENDS ${PROTOCOL_FILE}
            VERBATIM
        )

        set_source_files_properties(${C_FILENAME} ${H_FILENAME} PROPERTIES GENERATED TRUE)

        add_library(wayland_${PROTOCOL_TARGET} STATIC ${C_FILENAME} ${H_FILENAME})
    endfunction()

    generate_wayland_protocol("xdg-shell" "/usr/share/wayland-protocols/stable/xdg-shell/xdg-shell.xml")
    generate_wayland_protocol("xdg-decoration" "/usr/share/wayland-protocols/unstable/xdg-decoration/xdg-decoration-unstable-v1.xml")
    generate_wayland_protocol("pointer-warp" "/usr/share/wayland-protocols/staging/pointer-warp/pointer-warp-v1.xml")
    generate_wayland_protocol("cursor-shape" "/usr/share/wayland-protocols/staging/cursor-shape/cursor-shape-v1.xml")
    generate_wayland_protocol("tablet" "/usr/share/wayland-protocols/stable/tablet/tablet-v2.xml")

    # Create a library for the wayland protocol files
    add_library(wayland_protocols INTERFACE)
    target_link_libraries(wayland_protocols INTERFACE
        wayland_xdg-shell
        wayland_xdg-decoration
        wayland_pointer-warp
        wayland_cursor-shape
        wayland_tablet
    )

    target_include_directories(wayland_protocols INTERFACE
        ${CMAKE_CURRENT_BINARY_DIR}
    )
endif()

foreach(source ${SOURCES})
    get_filename_component(EXE_NAME ${source} NAME_WE)
    add_executable(${EXE_NAME} ${source})
    target_link_libraries(${EXE_NAME} PRIVATE olcPixelGameEngine3)
    target_include_directories(${EXE_NAME} PRIVATE ../)
    target_include_directories(${EXE_NAME} PRIVATE ${miniaudio_SOURCE_DIR})

    if(USE_STB)
        target_compile_definitions(${EXE_NAME} PRIVATE OLC_USE_STB_IMAGE=1)
        target_include_directories(${EXE_NAME} PRIVATE ${stb_SOURCE_DIR})
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
