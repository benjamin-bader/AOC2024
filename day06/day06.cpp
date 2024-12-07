#include "day06.h"

#include "board.h"
#include "parsers.h"
#include "point.h"
#include "turtle.h"

#include <algorithm>
#include <fstream>
#include <iostream>
#include <ranges>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

using namespace std;

namespace day06
{

namespace
{

constexpr const char* kInputFile = "day06/day06.input";

constexpr const char* kTestInput = R"(
....#.....
.........#
..........
..#.......
.......#..
..........
.#..^.....
........#.
#.........
......#...
)";

Board read_board()
{
    ifstream input(kInputFile);
    // stringstream input{string{kTestInput}.substr(1)};
    auto lines = parsers::Lines(input, parsers::Chars);
    return {lines};
}

Point find_start(const Board& board)
{
    auto start = board.all_points() | views::filter([&](Point p) { return board.at(p) == '^'; });
    if (ranges::distance(start) != 1)
    {
        throw "No start found";
    }
    return *start.begin();
}

struct WalkResult
{
    unordered_set<Turtle> visited;
    bool loop_detected;

    size_t size() const
    {
        return visited.size();
    }
};

WalkResult walk(const Board& board, Point start, Point dir, Point obstacle = Point{-1, -1})
{
    unordered_set<Turtle> visited;

    Turtle cur{start, dir};
    while (board.in_bounds(cur.position()))
    {
        auto [_, did_insert] = visited.insert(cur);
        if (!did_insert)
        {
            // loop detected
            return {visited, true};
        }

        Point next = cur.position() + cur.direction();
        if (!board.in_bounds(next))
        {
            // we're done
            break;
        }

        if (board[next] == '#' || next == obstacle)
        {
            cur = cur.turn_right();
        }
        else
        {
            cur = cur.move_forward();
        }
    }

    return {move(visited), false};
}

} // namespace

string PartOne::solve()
{
    auto board = read_board();
    auto start = find_start(board);
    auto result = walk(board, start, Dir::UP);
    auto positions = result.visited
        | views::transform(&Turtle::position)
        | ranges::to<unordered_set>();

    return to_string(positions.size());
}

string PartTwo::solve()
{
    auto board = read_board();
    auto start = find_start(board);
    auto simple_path = walk(board, start, Dir::UP).visited;

    auto loops = simple_path
        | views::transform(&Turtle::position)
        | views::filter([&](Point p) { return p != start; })
        | ranges::to<unordered_set>()
        | views::filter([&](Point p) {
            auto result = walk(board, start, Dir::UP, p);
            return result.loop_detected;
        });

    return to_string(ranges::distance(loops));
}

} // namespace day06
