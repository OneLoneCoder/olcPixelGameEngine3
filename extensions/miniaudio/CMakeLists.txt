# Fetch 3rd Party miniaudio library
FetchContent_Declare(
    miniaudio
    GIT_REPOSITORY https://github.com/mackron/miniaudio.git
    GIT_TAG 0.11.25
)

FetchContent_MakeAvailable(miniaudio)

add_example(ext_demo_Miniaudio demo_Miniaudio.cpp)

target_include_directories(ext_demo_Miniaudio PRIVATE ${miniaudio_SOURCE_DIR})

handle_assets(ext_demo_Miniaudio ${CMAKE_CURRENT_SOURCE_DIR}/assets)
