configure_file("${CMAKE_CURRENT_SOURCE_DIR}/day08.input" "${CMAKE_CURRENT_BINARY_DIR}/day08.input" COPYONLY)

add_library(day08 STATIC day08.h day08.cpp)
target_link_libraries(day08 PRIVATE base)

add_warnings(day08)
