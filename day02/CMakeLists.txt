add_library(day02 STATIC day02.h day02.cpp)
target_link_libraries(day02 PRIVATE base)

add_warnings(day02)
