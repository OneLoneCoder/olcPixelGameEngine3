# Fetch 3rd Party miniaudio library
# Fetch miniaudio.h
FetchContent_Declare(
    miniaudio
    URL "https://raw.githubusercontent.com/mackron/miniaudio/0.11.25/miniaudio.h"
    DOWNLOAD_NO_EXTRACT ON
    SOURCE_DIR "${CMAKE_BINARY_DIR}/_deps/miniaudio-src"
)

FetchContent_MakeAvailable(miniaudio)

add_example(ext_demo_Miniaudio demo_Miniaudio.cpp)

target_include_directories(ext_demo_Miniaudio PRIVATE "${CMAKE_BINARY_DIR}/_deps/miniaudio-src")
handle_assets(ext_demo_Miniaudio ${CMAKE_CURRENT_SOURCE_DIR}/assets)
