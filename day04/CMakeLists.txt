configure_file("${CMAKE_CURRENT_SOURCE_DIR}/day04.input" "${CMAKE_CURRENT_BINARY_DIR}/day04.input" COPYONLY)

add_library(day04 STATIC day04.h day04.cpp)
target_link_libraries(day04 PRIVATE base)

add_warnings(day04)
