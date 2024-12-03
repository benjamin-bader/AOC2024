# Advent of Code

## To build:

cmake -S . -B build -G Ninja
cmake --build build
chmod +x build/aoc2024

## To run:

### All tests:

build/aoc2024

### Specific day/part

build/aoc2024 <day> <part>

For example, day 1, part 2:
build/aoc2024 1 2

## To add a new day's problems:

Run:

```
cmake -D DAY=## -P NewDay.cmake
# for example: cmake -D DAY=12 -P NewDay.cmake

# will result in a new directory structure:
# day12/
#   CMakeLists.txt
#   day12.cpp
#   day12.h
```

Then update CMakeLists.txt, adding an `add_day` directive:

```
add_day(day12)
```
