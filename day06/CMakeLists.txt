configure_file("${CMAKE_CURRENT_SOURCE_DIR}/day06.input" "${CMAKE_CURRENT_BINARY_DIR}/day06.input" COPYONLY)

add_library(day06 STATIC day06.h day06.cpp)
target_link_libraries(day06 PRIVATE base)

add_warnings(day06)
