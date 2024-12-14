#include "day10.h"

#include "board.h"
#include "parsers.h"
#include "point.h"

#include <algorithm>
#include <deque>
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

using namespace std;

namespace day10
{

namespace
{

constexpr const char* kInputFile = "day10/day10.input";
constexpr const char* kTestInput = R"(89010123
78121874
87430965
96549874
45678903
32019012
01329801
10456732)";

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
    return {std::move(lines)};
}

struct TrailHash
{
    size_t operator()(const vector<Point>& trail) const
    {
        size_t seed = 1;
        for (const auto& p : trail)
        {
            hash_combine(seed, p);
        }
        return seed;
    }
};

using Trail = vector<Point>;
using TrailSet = unordered_set<Trail, TrailHash>;

void find_trails(const Board& board, deque<Point>& stack, TrailSet& trails)
{
    // PRECONDITION: stack is not empty

    Point cur = stack.back();
    int id = board[cur];
    int next_id = id + 1;

    for (const auto& neighbor : board.cardinal_neighbors(cur))
    {
        if (board[neighbor] == next_id)
        {
            stack.push_back(neighbor);

            if (next_id == '9')
            {
                // we found a trail
                trails.emplace(stack.begin(), stack.end());
            }
            else
            {
                find_trails(board, stack, trails);
            }

            stack.pop_back();
        }
    }
}

TrailSet find_all_trails(const Board& board)
{
    TrailSet trails;
    deque<Point> stack;

    for (int y = 0; y < board.num_rows(); y++)
    {
        for (int x = 0; x < board.num_cols(); x++)
        {
            Point p{x, y};
            if (board[p] == '0')
            {
                stack.push_back(p);
                find_trails(board, stack, trails);
                stack.pop_back();
            }
        }
    }

    return trails;
}

} // namespace

string PartOne::solve()
{
    auto board = read_board();
    auto trails = find_all_trails(board);

    unordered_map<Point, unordered_set<Point>> nines_by_trailheads;
    for (const auto& trail : trails)
    {
        nines_by_trailheads[trail.front()].insert(trail.back());
    }

    auto sum = ranges::fold_left(
        nines_by_trailheads | views::transform([](const auto& entry) { return entry.second.size(); }),
        0ULL,
        plus<>{}
    );

    return to_string(sum);
}

string PartTwo::solve()
{
    auto board = read_board();
    auto trails = find_all_trails(board);

    return to_string(trails.size());
}

} // namespace day10
