cmake_minimum_required(VERSION 3.24)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(aoc2024 VERSION 0.1.0 LANGUAGES CXX)

add_subdirectory(base)
add_subdirectory(day01)

add_executable(aoc2024 main.cpp)

target_link_libraries(aoc2024 PRIVATE base day01)
