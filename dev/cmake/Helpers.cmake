# This file contains a set of repeated commands. Consolidated
# here to simplify changes in the future and to reduce the
# potential for bugs which may arise from code duplication.

option(BUILD_WAYLAND "Build example programs with Wayland" OFF)
option(USE_STB "Use stb_image.h for image loading" OFF)
option(DISABLE_LIBDECOR "Disable libdecor usage in wayland builds" OFF)
option(FORCE_WAYLAND_LIBDECOR "Force wayland builds to only use libdecor" OFF)

# Platform specific flags and commands.
function(prepare_platforms)

    if(EMSCRIPTEN)
        # build Cache: libpng, zlib
        execute_process(COMMAND "${EMSCRIPTEN_ROOT_PATH}/embuilder${EMCC_SUFFIX}" build libpng zlib)
    endif()

    if(UNIX AND BUILD_WAYLAND)
        find_package(PkgConfig REQUIRED)
        pkg_check_modules(XKBCOMMON REQUIRED xkbcommon)
        pkg_check_modules(WAYLAND_CLIENT REQUIRED wayland-client)
        pkg_check_modules(WAYLAND_EGL REQUIRED wayland-egl)
        pkg_check_modules(EGL REQUIRED egl)
        pkg_check_modules(WAYLAND_CURSOR REQUIRED wayland-cursor)
        pkg_check_modules(LIBDECOR REQUIRED IMPORTED_TARGET libdecor-0)

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
        generate_wayland_protocol("pointer-warp" "/usr/share/wayland-protocols/staging/pointer-warp/pointer-warp-v1.xml")

        # Create a library for the wayland protocol files
        add_library(wayland_protocols INTERFACE)
        target_link_libraries(wayland_protocols INTERFACE
            wayland_xdg-shell
            wayland_pointer-warp
        )

        target_include_directories(wayland_protocols INTERFACE
            ${CMAKE_CURRENT_BINARY_DIR}
        )
    endif()

endfunction()

# Add an example to the project.
function(add_example EXAMPLE_NAME)

    # Set C++ Standards
    set(CMAKE_CXX_STANDARD 20)
    set(CMAKE_CXX_STANDARD_REQUIRED ON)
    set(CMAKE_CXX_EXTENSIONS OFF)
    
    # Every argument beyond EXAMPLE_NAME is assumed source
    set(EXAMPLE_SOURCE ${ARGN})

    add_executable(${EXAMPLE_NAME} ${EXAMPLE_SOURCE})
    
    if(NOT MULTI_HEADER)
        target_link_libraries(${EXAMPLE_NAME} PRIVATE olcPixelGameEngine3)
        target_include_directories(${EXAMPLE_NAME} PRIVATE ${REPO_ROOT_DIR})
    endif()
    
    if(USE_STB)
        target_compile_definitions(${EXAMPLE_NAME} PRIVATE OLC_USE_STB_IMAGE=1)
    endif()

    if(USE_HEADLESS)
        target_compile_definitions(${EXAMPLE_NAME} PRIVATE OLC_USE_HEADLESS=1)
    endif()

    if(APPLE)
        if(CMAKE_SYSTEM_NAME STREQUAL "iOS")
            target_link_libraries(${EXAMPLE_NAME} PRIVATE "-framework UIKit")
            target_link_libraries(${EXAMPLE_NAME} PRIVATE "-framework Foundation")
            target_link_libraries(${EXAMPLE_NAME} PRIVATE "-framework CoreGraphics")
            target_link_libraries(${EXAMPLE_NAME} PRIVATE "-framework QuartzCore")
            target_link_libraries(${EXAMPLE_NAME} PRIVATE "-framework Metal")
            target_link_libraries(${EXAMPLE_NAME} PRIVATE "-framework MetalKit")
            target_link_libraries(${EXAMPLE_NAME} PRIVATE "-framework OpenGLES")
            target_link_libraries(${EXAMPLE_NAME} PRIVATE "-framework GLKit")
            
        else()
            target_link_libraries(${EXAMPLE_NAME} PRIVATE "-framework Metal")
            target_link_libraries(${EXAMPLE_NAME} PRIVATE "-framework MetalKit")
            target_link_libraries(${EXAMPLE_NAME} PRIVATE "-framework AppKit")
            target_link_libraries(${EXAMPLE_NAME} PRIVATE "-framework QuartzCore")
            target_link_libraries(${EXAMPLE_NAME} PRIVATE "-framework OpenGL")
            target_link_libraries(${EXAMPLE_NAME} PRIVATE "-framework Foundation")
        endif()
    endif()
    
    if(UNIX AND NOT APPLE AND NOT EMSCRIPTEN)
        if(BUILD_WAYLAND)
            target_compile_definitions(${EXAMPLE_NAME} PRIVATE OLC_HOST=3)

            target_link_libraries(${EXAMPLE_NAME} PRIVATE
                wayland_protocols
                ${XKBCOMMON_LIBRARIES}
                ${WAYLAND_CLIENT_LIBRARIES}
                ${WAYLAND_EGL_LIBRARIES}
                ${WAYLAND_CURSOR_LIBRARIES}
                ${EGL_LIBRARIES}
                OpenGL::GL
                PNG::PNG
                Threads::Threads
            )

            target_link_libraries(${EXAMPLE_NAME} PRIVATE PkgConfig::LIBDECOR )
            target_include_directories(${EXAMPLE_NAME} PRIVATE ${LIBDECOR_INCLUDE_DIRS} )

        else() # x11
            target_link_libraries(${EXAMPLE_NAME} PRIVATE png)
            target_link_libraries(${EXAMPLE_NAME} PRIVATE GL)
            target_link_libraries(${EXAMPLE_NAME} PRIVATE pthread)
            target_link_libraries(${EXAMPLE_NAME} PRIVATE X11)
            target_link_libraries(${EXAMPLE_NAME} PRIVATE Xi)
        endif()
        
    endif()

    if(WIN32 AND NOT MSVC AND NOT EMSCRIPTEN)
        target_link_libraries(${EXAMPLE_NAME} PRIVATE user32)
        target_link_libraries(${EXAMPLE_NAME} PRIVATE gdi32)
        target_link_libraries(${EXAMPLE_NAME} PRIVATE dwmapi)
        target_link_libraries(${EXAMPLE_NAME} PRIVATE gdiplus)
        target_link_libraries(${EXAMPLE_NAME} PRIVATE shlwapi)
        target_link_libraries(${EXAMPLE_NAME} PRIVATE opengl32)

        target_compile_options(${EXAMPLE_NAME} PRIVATE -fmax-errors=5)
    endif()
    
    if(EMSCRIPTEN)
        set_target_properties(${EXAMPLE_NAME} PROPERTIES SUFFIX ".html")

        target_link_options(${EXAMPLE_NAME} PRIVATE -sASYNCIFY)
        target_link_options(${EXAMPLE_NAME} PRIVATE -sALLOW_MEMORY_GROWTH=1)
        target_link_options(${EXAMPLE_NAME} PRIVATE -sSTACK_SIZE=1048576)
        target_link_options(${EXAMPLE_NAME} PRIVATE -sEXPORTED_RUNTIME_METHODS=HEAPF32)
        target_link_options(${EXAMPLE_NAME} PRIVATE -sMAX_WEBGL_VERSION=2)
        target_link_options(${EXAMPLE_NAME} PRIVATE -sMIN_WEBGL_VERSION=2)
        target_link_options(${EXAMPLE_NAME} PRIVATE -sUSE_LIBPNG=1)
        target_link_options(${EXAMPLE_NAME} PRIVATE -sLLD_REPORT_UNDEFINED)
    endif()
endfunction()

function(handle_assets TARGET_NAME ASSETS_DIR)

    if(EMSCRIPTEN)
        target_link_options(${TARGET_NAME} PRIVATE --preload-file ${ASSETS_DIR}@assets)
    else()
        # Copy assets when not Web platform
        add_custom_command(
            TARGET ${TARGET_NAME}
            POST_BUILD
            COMMAND ${CMAKE_COMMAND}
            ARGS -E copy_directory ${ASSETS_DIR} ${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_CFG_INTDIR}/assets
        )        
    endif()

endfunction()