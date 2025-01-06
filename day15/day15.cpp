#include "day15.h"

#include "board.h"
#include "point.h"
#include "warehouse.h"

#include <algorithm>
#include <exception>
#include <fstream>
#include <iostream>
#include <memory>
#include <numeric>
#include <ranges>
#include <sstream>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

#include <fmt/ostream.h>

using namespace std;

namespace day15
{

namespace
{

constexpr const char* kInputFile = "day15/day15.input";
constexpr const char* kTestInput = R"(
##########
#..O..O.O#
#......O.#
#.OO..O.O#
#..O@..O.#
#O#..O...#
#O..O..O.#
#.OO.O.OO#
#....O...#
##########

<vv>^<v^>v>^vv^v>v<>v^v<v<^vv<<<^><<><>>v<vvv<>^v^>^<<<><<v<<<v^vv^v>^
vvv<<^>^v^^><<>>><>^<<><^vv^^<>vvv<>><^^v>^>vv<>v<<<<v<^v>^<^^>>>^<v<v
><>vv>v^v^<>><>>>><^^>vv>v<^^^>>v^v^<^^>v^^>v^<^v>v<>>v^v^<v>v^^<^^vv<
<<v<^>>^^^^>>>v^<>vvv^><v<<<>^^^vv^<vvv>^>v<^^^^v<>^>vvvv><>>v^<<^^^^^
^><^><>>><>^^<<^^v>>><^<v>^<vv>>v>>>^v><>^v><<<<v>>v<v<v>vvv>^<><<>^><
^>><>^v<><^vvv<^^<><v<<<<<><^v<<<><<<^^<v<^^^><^>>^<v^><<<^>>^v<v^v<v^
>^>>^v>vv>^<<^v<>><<><<v<<v><>v<^vv<<<>^^v^>^^>>><<^v>>v^v><^^>>^<>vv^
<><^^>^^^<><vvvvv^v<v<<>^v<v>v<<^><<><<><<<^^<<<^<<>><<><^^^>^^<>^>v<>
^^>vv<^v^v<vv>^<><v<^v>^^^>>>^^vvv^>vvv<>>>^<^>>>>>^<<^v>^vvv<>^<><<v>
v^^>>><<^^<>>^v^<v^vv<>v^<<>^<^v^v><^<<<><<^<v><v<>vv>>v><v^<vv<>v^<<^)";

[[maybe_unused]]
unique_ptr<istream> get_input()
{
    if (g_test_input)
    {
        return make_unique<stringstream>(kTestInput);
    }

    return make_unique<ifstream>(kInputFile);
}

Warehouse read_input()
{
    auto input = get_input();

    *input >> ws;

    vector<vector<char>> lines;
    string line;
    while (getline(*input, line) && !line.empty())
    {
        lines.emplace_back(line.begin(), line.end());
    }

    string steps;
    while (getline(*input, line))
    {
        steps += line;
    }

    Board board{lines};

    Point robot;
    for (Point p : board.all_points())
    {
        if (board[p] == '@')
        {
            robot = p;
            break;
        }
    }

    return {std::move(board), std::move(steps), robot};
}

Warehouse read_wide_input()
{
    auto input = get_input();

    *input >> ws;

    vector<vector<char>> lines;
    string line;
    while (getline(*input, line) && !line.empty())
    {
        vector<char> wide_line;
        wide_line.reserve(line.size() * 2);

        for (char c : line)
        {
            switch (c)
            {
            case '#':
                wide_line.push_back('#');
                wide_line.push_back('#');
                break;
            case '.':
                wide_line.push_back('.');
                wide_line.push_back('.');
                break;
            case 'O':
                wide_line.push_back('[');
                wide_line.push_back(']');
                break;
            case '@':
                wide_line.push_back('@');
                wide_line.push_back('.');
                break;
            }
        }

        lines.push_back(std::move(wide_line));
    }

    string steps;
    while (getline(*input, line))
    {
        steps += line;
    }

    Board board{std::move(lines)};

    Point robot;
    for (Point p : board.all_points())
    {
        if (board[p] == '@')
        {
            robot = p;
            break;
        }
    }

    return {std::move(board), std::move(steps), robot};
}

} // namespace

string PartOne::solve()
{
    Warehouse w = read_input();
    dbg("Before {}", w);
    w.apply_all_steps();
    dbg("After {}", w);
    return to_string(w.gps());
}

string PartTwo::solve()
{
    Warehouse w = read_wide_input();
    dbg("Before {}", w);
    w.apply_all_steps_wide();
    dbg("After {}", w);
    return to_string(w.gps());
}

} // namespace day15

