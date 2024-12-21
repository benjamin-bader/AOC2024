#include "day20.h"

#include "board.h"
#include "parsers.h"
#include "point.h"

#include <algorithm>
#include <exception>
#include <execution>
#include <fstream>
#include <iostream>
#include <memory>
#include <numeric>
#include <optional>
#include <queue>
#include <ranges>
#include <sstream>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

using namespace std;

namespace day20
{

namespace
{

constexpr const char* kInputFile = "day20/day20.input";
constexpr const char* kTestInput = R"(
###############
#...#...#.....#
#.#.#.#.#.###.#
#S#...#.#.#...#
#######.#.#.###
#######.#.#...#
#######.#.###.#
###..E#...#...#
###.#######.###
#...###...#...#
#.#####.#.###.#
#.#...#.#.#...#
#.#.#.#.#.#.###
#...#...#...###
###############)";

[[maybe_unused]]
unique_ptr<istream> get_input()
{
    if (g_test_input)
    {
        return make_unique<stringstream>(kTestInput);
    }

    return make_unique<ifstream>(kInputFile);
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

    vector<Point> path;

    path.reserve(dist[end]);
    for (Point cur = end; cur != start; cur = prev[cur])
    {
        path.push_back(cur);
    }
    path.push_back(start);

    reverse(path.begin(), path.end());

    return path;
}

} // namespace

string PartOne::solve()
{
    auto input = get_input();
    auto board = parsers::Board(*input);
    Point start, end;
    for (auto p : board.all_points())
    {
        if (board[p] == 'S')
        {
            start = p;
        }
        else if (board[p] == 'E')
        {
            end = p;
        }
    }

    vector<Point> vanilla_path = dijkstra(board, start, end);

    unordered_map<Point, size_t> indices_by_point;
    for (size_t i = 0; i < vanilla_path.size(); ++i)
    {
        indices_by_point[vanilla_path[i]] = i;
    }

    // Find all pairs that are separated by one wall.
    //
    // A map of what valid targets are; 'x' is a point,
    // '*' are valid targets;
    //
    //  ..*..
    //  .*#*.
    //  *#x#*
    //  .*#*.
    //  ..*..

    size_t num_good_cheats = 0;

    auto cheat_paths = array {
        // Going straight in a single direction
        make_pair(Point(1, 0), Point(2, 0)),
        make_pair(Point(-1, 0), Point(-2, 0)),
        make_pair(Point(0, 1), Point(0, 2)),
        make_pair(Point(0, -1), Point(0, -2)),

        // Going diagonal - two entries for each cardinal direction.
        make_pair(Point(1, 0), Point(1, 1)),
        make_pair(Point(1, 0), Point(1, -1)),
        make_pair(Point(-1, 0), Point(-1, 1)),
        make_pair(Point(-1, 0), Point(-1, -1)),
        make_pair(Point(0, 1), Point(1, 1)),
        make_pair(Point(0, 1), Point(-1, 1)),
        make_pair(Point(0, -1), Point(1, -1)),
        make_pair(Point(0, -1), Point(-1, -1)),
    };

    for (size_t i = 0; i < vanilla_path.size(); ++i)
    {
        const Point& p = vanilla_path[i];

        for (const auto& [via, to] : cheat_paths)
        {
            auto wall = p + via;
            auto dest = p + to;

            if (!board.in_bounds(wall) || board[wall] != '#')
            {
                continue;
            }

            auto it = indices_by_point.find(dest);
            if (it == indices_by_point.end() || it->second < i)
            {
                continue;
            }

            auto sv = it->second - i - 2;
            if (sv >= 100)
            {
                dbg() << "Cheat at " << wall << "maybe saves " << (it->second - i - 2) << " moves" << endl;
                ++num_good_cheats;
            }
        }
    }

    return to_string(num_good_cheats);
}

string PartTwo::solve()
{
    auto input = get_input();
    auto board = parsers::Board(*input);
    Point start, end;
    for (auto p : board.all_points())
    {
        if (board[p] == 'S')
        {
            start = p;
        }
        else if (board[p] == 'E')
        {
            end = p;
        }
    }

    vector<Point> vanilla_path = dijkstra(board, start, end);

    size_t num_good_cheats = 0;
    for (int i = 0; i < static_cast<int>(vanilla_path.size()) - 1; i++)
    {
        for (int j = 1; j < static_cast<int>(vanilla_path.size()); j++)
        {
            const Point& a = vanilla_path[static_cast<size_t>(i)];
            const Point& b = vanilla_path[static_cast<size_t>(j)];

            int d = a.manhattan_distance(b);
            if (d <= 20)
            {
                auto savings = max(0, j - i - d);
                if (savings >= 100)
                {
                    dbg() << "cheat from " << a << " to " << b << " saves " << savings << endl;
                    num_good_cheats++;
                }
            }
        }
    }

    return to_string(num_good_cheats);
}

} // namespace day20
