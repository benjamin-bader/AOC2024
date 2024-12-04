configure_file("${CMAKE_CURRENT_SOURCE_DIR}/day01.input" "${CMAKE_CURRENT_BINARY_DIR}/day01.input")

add_library(day01 STATIC day01.h day01.cpp)
target_link_libraries(day01 PRIVATE base)

add_warnings(day01)
