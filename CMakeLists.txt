cmake_minimum_required(VERSION 3.24)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(aoc2024 VERSION 0.1.0 LANGUAGES CXX)

include(FetchContent)
FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/refs/tags/v1.15.2.zip
)

set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()

macro(add_test_case DAY PART)
    set(_TEST_EXE "${DAY}_${PART}")
endmacro()

add_subdirectory(base)
add_subdirectory(day01)

add_executable(aoc2024 main.cpp)

target_link_libraries(aoc2024 PRIVATE base day01)
