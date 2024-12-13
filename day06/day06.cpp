#include "day06.h"

#include "board.h"
#include "parsers.h"
#include "point.h"
#include "turtle.h"

#include <algorithm>
#include <execution>
#include <fstream>
#include <iostream>
#include <memory>
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
......#...)";

unique_ptr<istream> get_input()
{
    if (g_test_input)
    {
        return make_unique<stringstream>(kTestInput);
    }

    return make_unique<ifstream>(kInputFile);
}

Board read_board()
{
    auto input = get_input();
    auto lines = parsers::Lines(*input, parsers::Chars);
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

    auto simple_path_points = simple_path
        | views::transform(&Turtle::position)
        | views::filter([&](Point p) { return p != start; })
        | ranges::to<unordered_set>();


    auto num_loops = transform_reduce(
        execution::par_unseq,
        simple_path_points.begin(), simple_path_points.end(),
        0,
        plus<>{},
        [&](Point p) {
            auto result = walk(board, start, Dir::UP, p);
            return result.loop_detected ? 1 : 0;;
        }
    );

    return to_string(num_loops);
}

} // namespace day06
