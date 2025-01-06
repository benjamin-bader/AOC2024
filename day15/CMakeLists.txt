add_library(day15 STATIC day15.h day15.cpp warehouse.h warehouse.cpp)
target_link_libraries(day15 PRIVATE base)

add_warnings(day15)
