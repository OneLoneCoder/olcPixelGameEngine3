cmake_minimum_required(VERSION 3.22)
project(game_native)

find_package(game-activity REQUIRED CONFIG)

add_library(${PROJECT_NAME} SHARED game.cpp)

target_link_libraries(${PROJECT_NAME}
        PRIVATE
        -Wl,--whole-archive game-activity::game-activity_static -Wl,--no-whole-archive
        android
        log
        EGL
        GLESv3
        jnigraphics
)
