add_library(day05 STATIC day05.h day05.cpp)
target_link_libraries(day05 PRIVATE base)
target_compile_definitions(day05 PRIVATE BOOST_SPIRIT_DEBUG=1)

add_warnings(day05)
