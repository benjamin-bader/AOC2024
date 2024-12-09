add_library(day03 STATIC day03.h day03.cpp)
target_link_libraries(day03 PRIVATE base)

add_warnings(day03)
