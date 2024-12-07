configure_file("${CMAKE_CURRENT_SOURCE_DIR}/day07.input" "${CMAKE_CURRENT_BINARY_DIR}/day07.input" COPYONLY)

add_library(day07 STATIC day07.h day07.cpp)
target_link_libraries(day07 PRIVATE base)

add_warnings(day07)
