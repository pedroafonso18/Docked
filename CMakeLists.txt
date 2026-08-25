cmake_minimum_required(VERSION 3.15)
project(
    docked
    VERSION 1.0
    LANGUAGES CXX
)

find_package(Boost REQUIRED COMPONENTS program_options)

add_executable(
    docked
    src/main.cpp
)

target_link_libraries(
    docked PRIVATE Boost::program_options
)