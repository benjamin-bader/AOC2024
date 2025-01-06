add_library(day13 STATIC day13.h day13.cpp)
target_link_libraries(day13 PRIVATE base)
target_compile_definitions(day13 PRIVATE BOOST_SPIRIT_DEBUG=1)

add_warnings(day13)
