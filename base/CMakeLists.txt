add_library(base INTERFACE)
target_include_directories(base INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})

target_sources(base
    INTERFACE FILE_SET HEADERS
    FILES
        base.h
        board.h
        parsers.h
        point.h
)
