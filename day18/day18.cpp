#include "day18.h"

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
#include <optional>
#include <queue>
#include <ranges>
#include <regex>
#include <sstream>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

using namespace std;

namespace day18
{

namespace
{

struct Params
{
    int w;
    int h;
    int s;
};

constexpr const int kWidth = 7;
constexpr const int kHeight = 7;

constexpr const Params kTestParams{7, 7, 12};
constexpr const Params kRealParams{71, 71, 1024};

constexpr const char* kInputFile = "day18/day18.input";
constexpr const char* kTestInput = R"(
5,4
4,2
4,5
3,0
2,1
6,3
2,4
1,5
0,6
3,3
2,6
5,1
1,2
5,5
2,5
6,5
1,4
0,4
6,4
1,1
6,1
1,0
0,5
1,6
2,0)";

struct Puzzle
{
    Params params;
    vector<Point> blocks;
    Board board;
};

[[maybe_unused]]
unique_ptr<istream> get_input()
{
    if (g_test_input)
    {
        return make_unique<stringstream>(kTestInput);
    }

    return make_unique<ifstream>(kInputFile);
}

const Params get_params()
{
    return g_test_input ? kTestParams : kRealParams;
}

vector<Point> dijkstra(const Board& board, Point start, Point end)
{
    unordered_map<Point, size_t> dist;
    unordered_map<Point, Point> prev;

    // pq is a min-queue - smallest element first.
    // pairs compare lexicographically, so weight comes first.
    priority_queue<pair<size_t, Point>, vector<pair<size_t, Point>>, greater<>> pq;


    dist.reserve(board.num_rows() * board.num_cols());
    for (auto p : board.all_points())
    {
        dist[p] = numeric_limits<size_t>::max();
    }

    pq.push({0, start});
    dist[start] = 0;

    while (!pq.empty())
    {
        auto [w, p] = pq.top();
        pq.pop();

        for (const auto& n : board.cardinal_neighbors(p))
        {
            if (board[n] == '#')
            {
                continue;
            }

            size_t alt = dist[p] + 1;
            if (alt < dist[n])
            {
                dist[n] = alt;
                prev[n] = p;
                pq.push({alt, n});
            }
        }
    }

    vector<Point> path{end};
    Point cur = end;
    do
    {
        cur = prev[cur];
        path.push_back(cur);
    } while (cur != start);

    reverse(path.begin(), path.end());

    return path;
}

Puzzle read_input(const Params& params)
{
    regex re(R"((\d+),(\d+))");
    auto line_to_point = [&re](const string& line)
    {
        smatch sm;
        if (!regex_match(line, sm, re))
        {
            throw runtime_error("Invalid input");
        }
        return Point{stoi(sm[1]), stoi(sm[2])};
    };

    auto in = get_input();
    auto points_vec = parsers::Lines(*in, line_to_point);\

    vector<vector<char>> grid;
    for (int y = 0; y < params.h; ++y)
    {
        vector<char> row(params.w, '.');
        grid.push_back(std::move(row));
    }

    for (auto p : points_vec | views::take(params.s))
    {
        grid[p.y()][p.x()] = '#';
    }

    Board board(std::move(grid));

    return Puzzle{params, points_vec, board};
}

bool is_reachable(const Board& board, const Point& start, const Point& end)
{
    unordered_set<Point> visited;
    deque<Point> q{start};

    while (!q.empty())
    {
        auto p = q.front();
        q.pop_front();

        if (p == end)
        {
            return true;
        }

        if (auto [_, inserted] = visited.insert(p); !inserted)
        {
            continue;
        }

        for (const auto& n : board.cardinal_neighbors(p))
        {
            if (board[n] == '#')
            {
                continue;
            }

            q.push_back(n);
        }
    }

    return false;
}

} // namespace

string PartOne::solve()
{
    auto params = get_params();
    auto puzzle = read_input(params);
    dbg() << puzzle.board << endl;
    Point start{0, 0};
    Point end{params.w - 1, params.h - 1};
    // auto path = astar(blocks, start, end);
    // Board copy = blocks;
    // for (const auto& p : path)
    // {
    //     copy[p] = 'O';
    // }
    // dbg() << copy << endl;
    // return path.empty() ? "none" : to_string(path.size() - 1); // counting steps, so the start point doesn't count

    auto path = dijkstra(puzzle.board, start, end);

    puzzle.board.draw(dbg(), path);

    //auto it = min_element(paths.begin(), paths.end(), [](const auto& lhs, const auto& rhs) { return lhs.size() < rhs.size(); });

    return to_string(path.size() - 1);

}

string PartTwo::solve()
{
    auto params = get_params();
    auto puzzle = read_input(params);

    size_t applied = params.s;
    while (is_reachable(puzzle.board, Point{0, 0}, Point{params.w - 1, params.h - 1}))
    {
        puzzle.board[puzzle.blocks[applied++]] = '#';
    }

    Point p = puzzle.blocks[applied - 1];
    stringstream ss;
    ss << p.x() << ',' << p.y();


    return ss.str();
}

} // namespace day18
