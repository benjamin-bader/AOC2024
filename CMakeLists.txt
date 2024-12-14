cmake_minimum_required(VERSION 3.24)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(aoc2024 VERSION 0.1.0 LANGUAGES CXX)

macro(add_warnings TARGET)
    target_compile_options(
        "${TARGET}"
        PRIVATE
        $<$<CXX_COMPILER_ID:MSVC>:/W4 /WX>
        $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wall -Wextra -Wpedantic -Werror -Wconversion>
    )
endmacro()

add_subdirectory(base)

add_executable(aoc2024 main.cpp)
target_link_libraries(aoc2024 PRIVATE base)
target_sources(aoc2024
    PRIVATE
    FILE_SET HEADERS FILES
        solutions.h
)

add_warnings(aoc2024)

# GCC and Clang on macOS both spuriously warn about duplicate 'base' libraries,
# so we suppress that warning.  GCC doesn't warn about this on linux in CI, for
# whatever reason, so we'll only suppress it on Apple platforms.
if(APPLE)
    target_link_options(aoc2024 PRIVATE LINKER:-no_warn_duplicate_libraries)
endif()

set(DAYS )
macro(add_day DAY)
    add_subdirectory(${DAY})
    target_link_libraries(aoc2024 PRIVATE ${DAY})
    list(APPEND DAYS "${DAY}")
endmacro()

macro(generate_solutions)
    set(SOLUTION_FILE "${CMAKE_CURRENT_BINARY_DIR}/solutions.cpp")
    file(COPY_FILE "${CMAKE_CURRENT_SOURCE_DIR}/solutions.cpp.in" "${SOLUTION_FILE}")

    foreach(DAY ${DAYS})
        file(APPEND "${SOLUTION_FILE}" "#include \"${DAY}/${DAY}.h\"\n")
    endforeach()

    file(APPEND "${SOLUTION_FILE}" "void register_solutions() {\n")
    foreach(DAY ${DAYS})
        string(REPLACE "day" "" DAY_NUM "${DAY}")
        while(${DAY_NUM} MATCHES "^0")
            string(SUBSTRING "${DAY_NUM}" 1 -1 DAY_NUM)
        endwhile()
        file(APPEND "${SOLUTION_FILE}" "    register_solution<${DAY}::PartOne>(${DAY_NUM}, 1);\n")
        file(APPEND "${SOLUTION_FILE}" "    register_solution<${DAY}::PartTwo>(${DAY_NUM}, 2);\n")
    endforeach()

    file(APPEND "${SOLUTION_FILE}" "}\n")

    target_sources(aoc2024 PRIVATE "${SOLUTION_FILE}")
endmacro()

cmake_language(DEFER CALL generate_solutions)

add_day(day01)
add_day(day02)
add_day(day03)
add_day(day04)
add_day(day05)
add_day(day06)
add_day(day07)
add_day(day08)
add_day(day09)
add_day(day10)
add_day(day11)
add_day(day12)
add_day(day13)
add_day(day14)
