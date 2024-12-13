#include "day12.h"

#include "board.h"
#include "parsers.h"
#include "point.h"

#include <algorithm>
#include <deque>
#include <exception>
#include <execution>
#include <fstream>
#include <iostream>
#include <memory>
#include <numeric>
#include <ranges>
#include <set>
#include <sstream>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

using namespace std;

namespace day12
{

namespace
{

constexpr const char* kInputFile = "day12/day12.input";
constexpr const char* kTestInput = R"(
RRRRIICCFF
RRRRIICCCF
VVRRRCCFFF
VVRCCCJFFF
VVVVCJJCFE
VVIVCCJJEE
VVIIICJJEE
MIIIIIJJEE
MIIISIJEEE
MMMISSJEEE)";

// claimed to be 436
constexpr const char* kRedditSuggestion = R"(
0.000
00.00
0.000
000.0
00000)";

[[maybe_unused]]
unique_ptr<istream> get_input()
{
    if (g_test_input)
    {
        return make_unique<stringstream>(kTestInput);
    }

    return make_unique<ifstream>(kInputFile);
}

shared_ptr<Board> read_board()
{
    auto input = get_input();
    auto lines = parsers::Lines(*input, parsers::Chars);

    return make_shared<Board>(move(lines));
}

class UnionFind
{
    vector<int> parent_;
    vector<int> rank_;
    int num_rows_;
    int num_cols_;

public:
    UnionFind(int num_rows, int num_cols)
        : parent_(num_rows * num_cols)
        , rank_(num_rows * num_cols, 1)
        , num_rows_(num_rows)
        , num_cols_(num_cols)
    {
        iota(parent_.begin(), parent_.end(), 0);
    }

    Point find(Point p)
    {
        if (p.x() < 0 || p.x() >= num_cols_ || p.y() < 0 || p.y() >= num_rows_)
        {
            return {-1, -1};
        }

        int i = point_to_index(p);
        return index_to_point(find(i));
    }

    void unite(Point x, Point y)
    {
        int ix = point_to_index(x);
        int iy = point_to_index(y);
        unite(ix, iy);
    }

private:
    int find(int i)
    {
        if (parent_[i] == i)
        {
            return i;
        }

        return parent_[i] = find(parent_[i]);
    }

    void unite(int x, int y)
    {
        int rx = find(x);
        int ry = find(y);

        if (rx == ry)
        {
            return;
        }

        if (rank_[rx] < rank_[ry])
        {
            swap(rx, ry);
        }

        parent_[ry] = rx;
        rank_[rx] += rank_[ry];
    }

    int point_to_index(Point p)
    {
        return (p.y() * num_cols_) + p.x();
    }

    Point index_to_point(int i)
    {
        return Point{i % num_cols_, i / num_cols_};
    }
};

class Region
{
    Point repr_;
    char id_;
    size_t area_;
    size_t perimeter_;
    size_t num_sides_;

public:
    Region(Point repr, char id, size_t area, size_t perimeter, size_t num_sides)
        : repr_(repr), id_(id), area_(area), perimeter_(perimeter), num_sides_(num_sides)
    {
    }

    Region(const Region&) = default;
    Region(Region&&) noexcept = default;

    char id() const
    {
        return id_;
    }

    size_t area() const
    {
        return area_;
    }

    size_t perimeter() const
    {
        return perimeter_;
    }

    size_t num_sides() const
    {
        return num_sides_;
    }
};

shared_ptr<Region> make_region(UnionFind& uf, const shared_ptr<const Board>& board, const set<Point>& points, char id)
{
    // we'll count the area, perimeter, and number of sides here in advance.

    size_t area = points.size();

    size_t perimeter = 0;
    size_t sides = 0;
    int max_x = -1;
    int min_x = numeric_limits<int>::max();
    int max_y = -1;
    int min_y = numeric_limits<int>::max();

    for (const auto& p : points)
    {
        if (p.x() < min_x) min_x = p.x();
        if (p.x() > max_x) max_x = p.x();
        if (p.y() < min_y) min_y = p.y();
        if (p.y() > max_y) max_y = p.y();

        perimeter += 4;
        for (const auto neighbor : board->cardinal_neighbors(p))
        {
            if (board->at(neighbor) == id)
            {
                perimeter--;
            }
        }
    }

    Point repr = uf.find(*points.begin());
    for (int y = min_y - 1; y < max_y + 1; ++y)
    {
        for (int x = min_x - 1; x < max_x + 1; ++x)
        {
            // check for corners
            // corners can look like:
            //
            // x.    xx    .x
            // .. or x. or x.
            //
            // that last configuration is actually two corners, _provided that_ two opposing corners
            // are in-group.

            size_t num_us = 0;

            auto r1 = uf.find({x, y});         // upper left
            auto r2 = uf.find({x + 1, y});     // upper right
            auto r3 = uf.find({x, y + 1});     // lower left
            auto r4 = uf.find({x + 1, y + 1}); // lower right

            if (r1 == repr) ++num_us;
            if (r2 == repr) ++num_us;
            if (r3 == repr) ++num_us;
            if (r4 == repr) ++num_us;

            if (num_us == 1 || num_us == 3)
            {
                ++sides;
            }
            else if (num_us == 2 && (r1 == r4 || r2 == r3))
            {
                sides += 2;
            }
        }
    }

    return make_shared<Region>(repr, id, area, perimeter, sides);
}

vector<shared_ptr<Region>> find_regions(const shared_ptr<const Board>& board)
{
    UnionFind uf{board->num_rows(), board->num_cols()};
    for (int y = 0; y < board->num_rows(); ++y)
    {
        for (int x = 0; x < board->num_cols(); ++x)
        {
            Point cur{x, y};
            Point right = cur + Dir::RIGHT;
            Point down = cur + Dir::DOWN;

            if (board->in_bounds(right) && board->at(cur) == board->at(right))
            {
                uf.unite(cur, right);
            }

            if (board->in_bounds(down) && board->at(cur) == board->at(down))
            {
                uf.unite(cur, down);
            }
        }
    }

    unordered_map<Point, set<Point>> regions_by_point;
    for (Point p : board->all_points())
    {
        Point root = uf.find(p);
        regions_by_point[root].insert(p);
    }

    vector<shared_ptr<Region>> regions;
    regions.reserve(regions_by_point.size());

    for (auto& [root, points] : regions_by_point)
    {
        char id = board->at(root);
        auto region = make_region(uf, board, points, id);
        regions.push_back(region);
    }

    return regions;
}

} // namespace

string PartOne::solve()
{
    const shared_ptr<const Board> board = read_board();
    const vector<shared_ptr<Region>> regions = find_regions(board);

    auto total_price = transform_reduce(
        regions.begin(), regions.end(),
        static_cast<uintmax_t>(0),
        plus<>{},
        [](const auto& r) { return static_cast<uintmax_t>(r->area()) * r->perimeter(); }
    );

    return to_string(total_price);
}

string PartTwo::solve()
{
    // 750xxx is too low
    // 827988 is too high
    // next guess: 815788

    auto board = read_board();
    const vector<shared_ptr<Region>> regions = find_regions(board);

    auto total_price = transform_reduce(
        regions.begin(), regions.end(),
        static_cast<uintmax_t>(0),
        plus<>{},
        [](const auto& r) { return static_cast<uintmax_t>(r->area()) * r->num_sides(); }
    );

    return to_string(total_price);
}

} // namespace day12
