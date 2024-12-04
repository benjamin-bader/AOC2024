configure_file("${CMAKE_CURRENT_SOURCE_DIR}/day02.input" "${CMAKE_CURRENT_BINARY_DIR}/day02.input")

add_library(day02 STATIC day02.h day02.cpp)
target_link_libraries(day02 PRIVATE base)

add_warnings(day02)
