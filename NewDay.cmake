# Requires a DAY argument that is a number, at most two digits.
# The number will be left-padded with a zero if it is less than 10.

# left-pad the number
string(LENGTH ${DAY} DAY_LENGTH)
if(${DAY_LENGTH} EQUAL 1)
    set(DAY "0${DAY}")
endif()

set(DAY_NAME "day${DAY}")
set(DAY_PATH "${CMAKE_CURRENT_SOURCE_DIR}/${DAY_NAME}")
set(BASE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/base")

set(OLD_NUMBER "${NUMBER}")
set(NUMBER "${DAY}")

configure_file(
    "${BASE_PATH}/DayCMakeListsTemplate.txt.in"
    "${DAY_PATH}/CMakeLists.txt"
    @ONLY
)

configure_file(
    "${BASE_PATH}/day_template.h.in"
    "${DAY_PATH}/${DAY_NAME}.h"
    @ONLY
)

configure_file(
    "${BASE_PATH}/day_template.cpp.in"
    "${DAY_PATH}/${DAY_NAME}.cpp"
    @ONLY
)

file(TOUCH "${DAY_PATH}/${DAY_NAME}.input")

if("${OLD_NUMBER}" STREQUAL "")
    unset(OLD_NUMBER)
else()
    set(NUMBER "${OLD_NUMBER}")
endif()
