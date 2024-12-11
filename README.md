# Advent of Code

## To build:

```
cmake -S . -B build -G Ninja
cmake --build build
chmod +x build/aoc2024
```

## To run:

### All tests:

```
# Note: this requires the Secret Passphrase
gpg -o inputs.zip --decrypt inputs.zip.gpg
unzip inputs.zip
./build/aoc2024
```

### Specific day/part

```
build/aoc2024 <day> <part>
```

```
# For example, day 1, part 2:
build/aoc2024 1 2
```

Options (must come before day/part numbers):

`-t`: Run on example input, not on the encrypted input files.
`-v`: Enable verbose output.  Add more v's for greater verbosity.

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
#   day12.input
#
# (it will also update CMakeLists.txt and some codegen to plug the new problem into the harness)
```

Copy the day's input, paste it in to the new day's input file, then run PackInputs.cmake to include it in the encrypted archive.
