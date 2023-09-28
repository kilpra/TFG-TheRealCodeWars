add_library(Server
            Config.cpp
            Server.cpp
            )

include(FetchContent)

FetchContent_Declare(httplib URL https://github.com/yhirose/cpp-httplib/archive/refs/heads/master.zip DOWNLOAD_EXTRACT_TIMESTAMP TRUE)
FetchContent_MakeAvailable(httplib)

FetchContent_Declare(json URL https://github.com/nlohmann/json/releases/download/v3.11.2/json.tar.xz DOWNLOAD_EXTRACT_TIMESTAMP TRUE)
FetchContent_MakeAvailable(json)

target_link_libraries(Server PUBLIC
                      httplib::httplib
                      nlohmann_json::nlohmann_json
                     )


target_include_directories(Server INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})