configure_file("${CMAKE_CURRENT_SOURCE_DIR}/day03.input" "${CMAKE_CURRENT_BINARY_DIR}/day03.input" COPYONLY)

add_library(day03 STATIC day03.h day03.cpp)
target_link_libraries(day03 PRIVATE base)

add_warnings(day03)
