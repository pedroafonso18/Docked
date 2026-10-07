cmake_minimum_required(VERSION 3.15)
project(
    docked
    VERSION 1.0
    LANGUAGES CXX
)

find_package(Boost REQUIRED COMPONENTS program_options)
find_package(GTest QUIET)

if (NOT GTest_FOUND)
    include(FetchContent)

    FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/refs/tags/v1.16.0.zip
    )

    set(INSTALL_GTEST OFF CACHE BOOL "" FORCE)
    set(BUILD_GMOCK OFF CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(googletest)
endif()

file(GLOB_RECURSE DOCKED_SOURCES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
)

list(FILTER DOCKED_SOURCES EXCLUDE REGEX ".*/src/main\\.cpp$")

add_library(
    docked_core
    ${DOCKED_SOURCES}
)

target_include_directories(
    docked_core PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}/src"
)

target_link_libraries(
    docked_core PUBLIC Boost::program_options
)

add_executable(
    docked
    src/main.cpp
)

target_link_libraries(
    docked PRIVATE docked_core
)

enable_testing()

file(GLOB_RECURSE DOCKED_TEST_SOURCES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/tests/*.cpp"
)

add_executable(
    docked_tests
    ${DOCKED_TEST_SOURCES}
)

target_include_directories(
    docked_tests PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/src"
    "${CMAKE_CURRENT_SOURCE_DIR}/tests"
)

target_link_libraries(
    docked_tests PRIVATE
    docked_core
    GTest::gtest_main
)

add_test(
    NAME docked_tests
    COMMAND docked_tests
)