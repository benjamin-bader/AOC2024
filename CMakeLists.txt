cmake_minimum_required(VERSION 3.24)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(aoc2024 VERSION 0.1.0 LANGUAGES CXX)

add_subdirectory(base)

add_executable(aoc2024 main.cpp)

target_link_libraries(aoc2024 PRIVATE base day01 day02)

macro(add_day DAY)
    add_subdirectory(${DAY})
    target_link_libraries(aoc2024 PRIVATE ${DAY})
endmacro()

add_day(day01)
add_day(day02)
