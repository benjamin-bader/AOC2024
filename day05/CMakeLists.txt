configure_file("${CMAKE_CURRENT_SOURCE_DIR}/day05.input" "${CMAKE_CURRENT_BINARY_DIR}/day05.input" COPYONLY)

add_library(day05 STATIC day05.h day05.cpp)
target_link_libraries(day05 PRIVATE base)

add_warnings(day05)
