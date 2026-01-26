if(NOT TARGET game-activity::game-activity)
add_library(game-activity::game-activity STATIC IMPORTED)
set_target_properties(game-activity::game-activity PROPERTIES
    IMPORTED_LOCATION "/home/diego/.gradle/caches/9.1.0/transforms/6a32a9201af73907674ac930cbb228eb/transformed/games-activity-4.0.0/prefab/modules/game-activity/libs/android.x86/libgame-activity.a"
    INTERFACE_INCLUDE_DIRECTORIES "/home/diego/.gradle/caches/9.1.0/transforms/6a32a9201af73907674ac930cbb228eb/transformed/games-activity-4.0.0/prefab/modules/game-activity/include"
    INTERFACE_LINK_LIBRARIES ""
)
endif()

if(NOT TARGET game-activity::game-activity_static)
add_library(game-activity::game-activity_static STATIC IMPORTED)
set_target_properties(game-activity::game-activity_static PROPERTIES
    IMPORTED_LOCATION "/home/diego/.gradle/caches/9.1.0/transforms/6a32a9201af73907674ac930cbb228eb/transformed/games-activity-4.0.0/prefab/modules/game-activity_static/libs/android.x86/libgame-activity_static.a"
    INTERFACE_INCLUDE_DIRECTORIES "/home/diego/.gradle/caches/9.1.0/transforms/6a32a9201af73907674ac930cbb228eb/transformed/games-activity-4.0.0/prefab/modules/game-activity_static/include"
    INTERFACE_LINK_LIBRARIES ""
)
endif()

