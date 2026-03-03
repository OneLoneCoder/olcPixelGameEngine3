cmake_minimum_required(VERSION 3.19)
project(olcPixelGameEngine VERSION 3.0.0)

option(BUILD_EXAMPLES "Build example programs" OFF)
option(USE_STB "Use stb_image.h for image loading" OFF)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Make sure we have the header tool
include(FetchContent)
FetchContent_Declare(
    olcGimmeHead
    GIT_REPOSITORY https://github.com/OneLoneCoder/olcGimmeHead.git
    GIT_TAG 75e71cc66d165afdda436d5c8fc0ac97904011f7
)

FetchContent_MakeAvailable(olcGimmeHead)

file(GLOB sources
    "dev/src/*.cpp"
    "dev/src/*.hpp"
    "dev/src/*.h"
    )

if(NOT EMSCRIPTEN)
    add_custom_command(
        OUTPUT olcPixelGameEngine3.h
        COMMAND ${olcGimmeHead_BINARY_DIR}/${CMAKE_CFG_INTDIR}/gimme-head${CMAKE_EXECUTABLE_SUFFIX} ${CMAKE_CURRENT_SOURCE_DIR}/dev/src/sh_template.h ${CMAKE_BINARY_DIR}/olcPixelGameEngine3.h
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/dev/src
        DEPENDS ${sources}
        DEPENDS ${olcGimmeHead_BINARY_DIR}/${CMAKE_CFG_INTDIR}/gimme-head${CMAKE_EXECUTABLE_SUFFIX}
    )
else()
    add_custom_command(
        OUTPUT olcPixelGameEngine3.h
        COMMAND node ${olcGimmeHead_BINARY_DIR}/gimme-head ${CMAKE_CURRENT_SOURCE_DIR}/dev/src/sh_template.h ${CMAKE_BINARY_DIR}/olcPixelGameEngine3.h
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/dev/src
        DEPENDS ${sources}
        DEPENDS ${olcGimmeHead_BINARY_DIR}/gimme-head
    )
endif()

add_custom_target(CopyHeader
    COMMAND ${CMAKE_COMMAND} -E copy olcPixelGameEngine3.h ${CMAKE_CURRENT_SOURCE_DIR}/olcPixelGameEngine3.h
    DEPENDS olcPixelGameEngine3.h
)

add_library(olcPixelGameEngine3 INTERFACE olcPixelGameEngine3.h)
add_dependencies(olcPixelGameEngine3 CopyHeader)

# Get 3rd Party Libs
if(BUILD_EXAMPLES OR BUILD_TEST)
    FetchContent_Declare(
        miniaudio
        GIT_REPOSITORY https://github.com/mackron/miniaudio.git
        GIT_TAG        0.11.25
        EXCLUDE_FROM_ALL
    )

    FetchContent_MakeAvailable(miniaudio)

    if(USE_STB)
        FetchContent_Declare(
            stb
            GIT_REPOSITORY https://github.com/nothings/stb.git
            GIT_TAG        f1c79c02822848a9bed4315b12c8c8f3761e1296
            EXCLUDE_FROM_ALL
        )

        FetchContent_MakeAvailable(stb)
    endif()
endif()

if(BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()

if(BUILD_TEST)
    add_subdirectory(dev/tests)
endif()