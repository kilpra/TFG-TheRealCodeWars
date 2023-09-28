cmake_minimum_required(VERSION 3.15)

# set the project name
project(TheRealCodeWars)

set(CMAKE_CXX_STANDARD  20)

# search and add SQLite3
include(FetchContent)

FetchContent_Declare(SQLite3 URL https://www.sqlite.org/2023/sqlite-amalgamation-3420000.zip DOWNLOAD_EXTRACT_TIMESTAMP TRUE)
FetchContent_MakeAvailable(SQLite3)


# add the libraries paths
add_subdirectory(server)
add_subdirectory(game)

# add the executable
add_executable(TheRealCodeWars TheRealCodeWars.cpp)

# add the libraries
target_link_libraries(TheRealCodeWars PUBLIC
                      Server
                      Game
                      SQLite3
                     )

# add the binary tree to the search path for include files
target_include_directories(TheRealCodeWars PUBLIC
                           "${PROJECT_BINARY_DIR}"
                           )
