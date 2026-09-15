cmake_minimum_required(VERSION 3.19)
project(olcPixelGameEngine VERSION 3.0.0)

option(BUILD_EXAMPLES "Build example programs" OFF)
option(BUILD_EXTENSIONS "Build extension example programs" OFF)
option(BUILD_TEST "Build development test program" OFF)

# We're kind of forced into havng a sub-root rather than having a proper
# cmake at the root of the repo, so we have this variable to know where
# the root is in order to accomplish other builds.
get_filename_component(REPO_ROOT_DIR "${CMAKE_CURRENT_SOURCE_DIR}" DIRECTORY)

# Make sure we have the header tool
include(FetchContent)
FetchContent_Declare(
    olcGimmeHead
    GIT_REPOSITORY https://github.com/OneLoneCoder/olcGimmeHead.git
    GIT_TAG 75e71cc66d165afdda436d5c8fc0ac97904011f7
)

FetchContent_MakeAvailable(olcGimmeHead)

file(GLOB sources
    "src/*.cpp"
    "src/*.hpp"
    "src/*.h"
    )

if(NOT EMSCRIPTEN)
    add_custom_command(
        OUTPUT olcPixelGameEngine3.h
        COMMAND ${olcGimmeHead_BINARY_DIR}/${CMAKE_CFG_INTDIR}/gimme-head${CMAKE_EXECUTABLE_SUFFIX} ${CMAKE_CURRENT_SOURCE_DIR}/src/sh_template.h ${CMAKE_BINARY_DIR}/olcPixelGameEngine3.h
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/src
        DEPENDS ${sources}
        DEPENDS ${olcGimmeHead_BINARY_DIR}/${CMAKE_CFG_INTDIR}/gimme-head${CMAKE_EXECUTABLE_SUFFIX}
    )
else()
    add_custom_command(
        OUTPUT olcPixelGameEngine3.h
        COMMAND node ${olcGimmeHead_BINARY_DIR}/gimme-head ${CMAKE_CURRENT_SOURCE_DIR}/src/sh_template.h ${CMAKE_BINARY_DIR}/olcPixelGameEngine3.h
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/src
        DEPENDS ${sources}
        DEPENDS ${olcGimmeHead_BINARY_DIR}/gimme-head
    )
endif()

add_custom_target(CopyHeader
    COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_BINARY_DIR}/olcPixelGameEngine3.h ${REPO_ROOT_DIR}/olcPixelGameEngine3.h
    DEPENDS olcPixelGameEngine3.h
)

add_library(olcPixelGameEngine3 INTERFACE olcPixelGameEngine3.h)
add_dependencies(olcPixelGameEngine3 CopyHeader)

include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/Helpers.cmake)

if(BUILD_EXAMPLES OR BUILD_TEST OR BUILD_EXTENSIONS)
    # see: cmake/Helpers.cmake
    prepare_platforms()
endif()

if(BUILD_EXAMPLES)
    add_subdirectory(${REPO_ROOT_DIR}/examples examples)
endif()

if(BUILD_EXTENSIONS)
    add_subdirectory(${REPO_ROOT_DIR}/extensions extensions)
endif()

if(BUILD_TEST)
    add_subdirectory(tests)
endif()
