add_library(base STATIC base.cpp)
target_include_directories(base INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})

target_sources(base
    INTERFACE FILE_SET HEADERS
    FILES
        base.h
        board.h
        hash.h
        parsers.h
        point.h
        turtle.h
)
