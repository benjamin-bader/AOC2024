add_library(day07 STATIC day07.h day07.cpp)
target_link_libraries(day07 PRIVATE base)
target_compile_definitions(day07 PRIVATE BOOST_SPIRIT_DEBUG=1)

add_warnings(day07)
