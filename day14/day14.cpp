#include "day14.h"

#include "parsers.h"
#include "point.h"

#include <algorithm>
#include <exception>
#include <fstream>
#include <iostream>
#include <memory>
#include <numeric>
#include <ranges>
#include <regex>
#include <sstream>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

using namespace std;

namespace day14
{

namespace
{

constexpr const char* kInputFile = "day14/day14.input";
constexpr const char* kTestInput = R"(
p=0,4 v=3,-3
p=6,3 v=-1,-3
p=10,3 v=-1,2
p=2,0 v=2,-1
p=0,0 v=1,3
p=3,0 v=-2,-2
p=7,6 v=-1,-3
p=3,0 v=-1,-2
p=9,3 v=2,3
p=7,3 v=-1,2
p=2,4 v=2,-3
p=9,5 v=-3,-3
)";

struct Params
{
    int w;
    int h;
    int iters;
    int midX;
    int midY;

    constexpr Params(int w, int h, int iters)
        : w(w)
        , h(h)
        , iters(iters)
        , midX(w / 2)
        , midY(h / 2)
    {}
};

constexpr const Params kRealParams{101, 103, 100};
constexpr const Params kTestParams{11, 7, 100};

[[maybe_unused]]
unique_ptr<istream> get_input()
{
    if (g_test_input)
    {
        return make_unique<stringstream>(kTestInput);
    }

    return make_unique<ifstream>(kInputFile);
}

struct Robot
{
    Point p;
    Point v;
};

struct Problem
{
    Params params;
    vector<Robot> robots;

    Problem(Params params, vector<Robot>&& pvs)
        : params(params)
        , robots(std::move(pvs))
    {}
};

Problem read_input()
{
    const regex expr{R"(p=(-?\d+),(-?\d+) v=(-?\d+),(-?\d+))"};
    vector<Robot> result;

    auto input = get_input();
    for (auto line : parsers::Lines(*input))
    {
        smatch sm;
        if (!regex_match(line, sm, expr))
        {
            dbg() << "womp womp; line=" << line << endl;
            throw invalid_argument{"Unexpected input"};
        }

        Robot pv;
        pv.p = {stoi(sm[1]), stoi(sm[2])};
        pv.v = {stoi(sm[3]), stoi(sm[4])};

        result.push_back(pv);
    }

    return Problem(g_test_input ? kTestParams : kRealParams, std::move(result));
}

// A modulo operation that always returns positive results, even when
// the dividend is negative.  This so happens to be exactly how we
// can compute wraparound positions for robots with negative coordinates.
int posmod(int a, int b)
{
    return (a % b + b) % b;
}

Point predict_position(const Robot& pv, const Params& params)
{
    Point v = { pv.v.x() * params.iters, pv.v.y() * params.iters };
    Point p = pv.p + v;

    int x = posmod(p.x(), params.w);
    int y = posmod(p.y(), params.h);

    // dbg() << "Predicted position: " << pv.p << " -> " << Point{x, y} << endl;

    return {x, y};
}

// Returns the quadrant ID:
// -1: mid (ignore these)
// 0: top left
// 1: top right
// 2: bottom left
// 3: bottom right
int quadrant_for_pos(const Point& p, const Params& params)
{
    if (p.x() == params.midX || p.y() == params.midY)
    {
        return -1;
    }


    return ((p.y() > params.midY) << 1) | (p.x() > params.midX);
}

size_t find_tree(Problem& problem)
{
    vector<bool> grid(static_cast<size_t>(problem.params.h * problem.params.w), false);

    const auto as_coord = [&](const Point& p) {
        return static_cast<size_t>((p.y() * problem.params.h) + p.x());
    };

    const auto print = [&]() {
        for (auto y = 0; y < problem.params.h; ++y) {
            for (auto x = 0; x < problem.params.w; ++x) {
                size_t coord = as_coord({x, y});
                dbg() << (grid[coord] ? '#' : '.');
            }
            dbg() << '\n';
        }
        dbg() << endl;
    };

    const auto find_filled_block = [&](const Point& p) {
        // find a 5x5 block that's all filled; it's likely
        // that finding such a block means we've found the
        // tree.

        int min_x = p.x() - 2;
        int min_y = p.y() - 2;
        int max_x = p.x() + 2;
        int max_y = p.y() + 2;

        if (min_x < 0 || min_y < 0 || max_x >= problem.params.w || max_y >= problem.params.h)
        {
            return false;
        }

        for (int y = min_y; y <= max_y; ++y)
        {
            for (int x = min_x; x <= max_x; ++x)
            {
                if (!grid[as_coord({x, y})])
                {
                    return false;
                }
            }
        }

        return true;
    };

    for (size_t iters = 1; iters < 100000000000; ++iters)
    {
        fill_n(grid.begin(), grid.size(), false);

        for (auto& robot : problem.robots)
        {
            robot.p = predict_position(robot, problem.params);
            size_t coord = static_cast<size_t>((robot.p.y() * problem.params.h) + robot.p.x());
            grid[coord] = true;
        }

        for (const auto& robot : problem.robots)
        {
            if (find_filled_block(robot.p))
            {
                // Maybe?
                print();
                return iters;
            }
        }
    }

    dbg(LogLevel::ERROR) << "No tree candidate position found" << endl;

    return 0;
}

} // namespace

string PartOne::solve()
{
    auto problem = read_input();

    array<unsigned long, 4> quadrants{0};
    for (const auto& robot : problem.robots)
    {
        Point p = predict_position(robot, problem.params);
        int q = quadrant_for_pos(p, problem.params);
        if (q >= 0)
        {
            quadrants[static_cast<size_t>(q)]++;
        }
    }

    unsigned long result = 1;
    for (auto q : quadrants)
    {
        result *= q;
    }

    return to_string(result);
}

string PartTwo::solve()
{
    auto problem = read_input();
    problem.params.iters = 1; // gotta step one by one; 1000 at a time only works for part 1

    return to_string(find_tree(problem));
}

} // namespace day14
