cmake_minimum_required(VERSION 3.15)
project(
    docked
    VERSION 1.0
    LANGUAGES CXX
)

find_package(Boost REQUIRED COMPONENTS program_options)

file(GLOB_RECURSE DOCKED_SOURCES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
)

add_executable(
    docked
    ${DOCKED_SOURCES}
)

target_link_libraries(
    docked PRIVATE Boost::program_options
)