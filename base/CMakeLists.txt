add_library(base STATIC base.cpp dawg.cpp numbers.cpp)
target_include_directories(base INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(base PUBLIC fmt::fmt Boost::headers)

target_sources(base
    INTERFACE FILE_SET HEADERS
    FILES
        base.h
        board.h
        dawg.h
        hash.h
        numbers.h
        parsers.h
        point.h
        turtle.h
)
