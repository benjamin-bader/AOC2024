configure_file("${CMAKE_CURRENT_SOURCE_DIR}/day09.input" "${CMAKE_CURRENT_BINARY_DIR}/day09.input" COPYONLY)

add_library(day09 STATIC day09.h day09.cpp)
target_link_libraries(day09 PRIVATE base)

add_warnings(day09)
