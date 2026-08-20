cmake_minimum_required(VERSION 3.15)
project(
    docked
    VERSION 1.0
    LANGUAGES CXX
)

include(FetchContent)
FetchContent_Declare(
    cli
    GIT_REPOSITORY https://github.com/daniele77/cli.git
    GIT_TAG v2.1.0
)
FetchContent_MakeAvailable(
    cli
)

add_executable(
    docked
    src/main.cpp
)

target_link_libraries(docked PRIVATE cli::cli)
