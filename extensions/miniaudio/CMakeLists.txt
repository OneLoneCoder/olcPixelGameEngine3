# Fetch 3rd Party miniaudio library
# Fetch miniaudio.h
FetchContent_Declare(
    miniaudio
    URL "https://raw.githubusercontent.com/mackron/miniaudio/0.11.25/miniaudio.h"
    DOWNLOAD_NO_EXTRACT ON
    SOURCE_DIR "${CMAKE_BINARY_DIR}/_deps/miniaudio-src"
)

FetchContent_MakeAvailable(miniaudio)

file(GLOB SOURCES "*.cpp")

# loop through each source file
foreach(source ${SOURCES})
    get_filename_component(EXE_NAME ${source} NAME_WE)
    add_example(${EXE_NAME} ${source})
    target_include_directories(${EXE_NAME} PRIVATE "${CMAKE_BINARY_DIR}/_deps/miniaudio-src")
    handle_assets(${EXE_NAME} ${CMAKE_CURRENT_SOURCE_DIR}/assets)
endforeach()
