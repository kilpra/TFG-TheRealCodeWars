add_library(Game 
            Tile.cpp
            Dashboard.cpp
            Soldier.cpp
            Utils.cpp
            )

target_include_directories(Game INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})