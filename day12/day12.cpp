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
constexpr const char* kTestInput = R"(RRRRIICCFF
RRRRIICCCF
VVRRRCCFFF
VVRCCCJFFF
VVVVCJJCFE
VVIVCCJJEE
VVIIICJJEE
MIIIIIJJEE
MIIISIJEEE
MMMISSJEEE)";

constexpr const char* kShouldBeThreeEightSix = R"(AAAAAA
AAABBA
AAABBA
ABBAAA
ABBAAA
AAAAAA)";

[[maybe_unused]]
unique_ptr<istream> get_input()
{
    if (g_test_input)
    {
        return make_unique<stringstream>(kShouldBeThreeEightSix);
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
    int num_cols_;

public:
    UnionFind(int num_rows, int num_cols)
        : parent_(num_rows * num_cols)
        , rank_(num_rows * num_cols, 1)
        , num_cols_(num_cols)
    {
        iota(parent_.begin(), parent_.end(), 0);
    }

    int find(Point p)
    {
        int i = point_to_index(p);
        return find(i);
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
};

class Region
{
    std::shared_ptr<const Board> board_;
    set<Point> points_;
    char id_;
    size_t perimeter_;

public:
    Region(const shared_ptr<const Board>& board, set<Point>&& points, char id, size_t perimeter)
        : board_(board), points_(move(points)), id_(id), perimeter_(perimeter)
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
        return points_.size();
    }

    size_t perimeter() const
    {
        return perimeter_;
    }

    size_t count_sides() const
    {
        // Walk the perimeter of the region, counting the number of sides.
        // Each time we change direction, we'll increment the number of sides.
        // Starting with the least element in the region will allow us to
        // always begin going "right", because the smallest point will always
        // be a top-left corner.

        Point min = *points_.begin();

        auto next_dir = [](Point d) {
            if (d == Dir::RIGHT) return Dir::DOWN;
            if (d == Dir::DOWN) return Dir::LEFT;
            if (d == Dir::LEFT) return Dir::UP;
            if (d == Dir::UP) return Dir::RIGHT;
            throw "Invalid direction";
        };

        auto prev_dir = [](Point d) {
            if (d == Dir::RIGHT) return Dir::UP;
            if (d == Dir::DOWN) return Dir::RIGHT;
            if (d == Dir::LEFT) return Dir::DOWN;
            if (d == Dir::UP) return Dir::LEFT;
            throw "Invalid direction";
        };

        // I'm not confident that this loop will terminate, so let's keep
        // a set of point+direction pairs that we've visited.
        set<pair<Point, Point>> visited;

        // hackity hack - we don't have a great way to detect "holes" in a region,
        // so we will keep track of each "outside" ID we encounter while establishing
        // the perimeter.  If, at the end, there's only one, then this region is
        // surrounded and so we'll double the number of sides returned.  Yes, it's
        // gross, but it will make our summation work.
        set<char> surrounding_ids;
        set<Point> surrounding_points;

        size_t num_sides = 0;
        Point cur = min;
        Point dir = Dir::RIGHT;
        do
        {
            Point left = cur + prev_dir(dir);
            Point straight = cur + dir;

            if (auto [_, inserted] = visited.insert({cur, dir}); !inserted)
            {
                // we've visited this point in this direction before
                dbg() << "I haz an infinite loop :( point=" << cur << " dir=" << dir << endl;
                throw std::logic_error{"Infinite loop detected"};
            }

            // even if 'left_id' is the out-of-bounds sentinel, we still want to
            // add it to the set of surrounding IDs.  Regions touching the edge of
            // the board are, for our purposes, not wholly surrounded by another region.

            char left_id = board_->in_bounds(left) ? board_->at(left) : '*';
            if (left_id != id_)
            {
                surrounding_ids.insert(left_id);
            }

            if (left_id == id_)
            {
                dbg(LogLevel::DEBUG) << "turning left" << endl;
                cur = left;
                dir = prev_dir(dir);
                ++num_sides;
            }
            else if (board_->in_bounds(straight) && board_->at(straight) == id_)
            {
                dbg(LogLevel::DEBUG) << "going straight" << endl;
                cur = straight;
                // no direction change, so don't increment sides
            }
            else
            {
                // we can't turn left, or go straight, so we must turn right
                dbg(LogLevel::DEBUG) << "turning right" << endl;
                dir = next_dir(dir);
                ++num_sides;
            }
        } while (cur != min || dir != Dir::RIGHT);

        dbg(LogLevel::DEBUG) << "num_sides: id=" << id_ << " num_sides=" << num_sides << endl;

        if (surrounding_ids.size() == 1)
        {
            dbg() << "region " << id() << " at " << min << " is surrounded!" << endl;
            return num_sides * 2;
        }

        return num_sides;
    }
};

shared_ptr<Region> explore_region(const shared_ptr<const Board>& board, unordered_set<Point>& visited, char id, Point origin)
{
    deque<Point> q;
    q.push_back(origin);

    set<Point> region_points;

    size_t perimeter = 0;
    while (!q.empty())
    {
        Point p = q.front();
        q.pop_front();

        if (auto [_, inserted] = visited.insert(p); !inserted)
        {
            continue;
        }

        region_points.insert(p);
        perimeter += 4;

        for (const auto neighbor : board->cardinal_neighbors(p))
        {
            if (board->at(neighbor) == id)
            {
                perimeter--;
                q.push_back(neighbor);
            }
        }
    }

    return make_shared<Region>(board, move(region_points), id, perimeter);
}

} // namespace

string PartOne::solve()
{
    const shared_ptr<const Board> board = read_board();
    unordered_set<Point> visited;
    vector<shared_ptr<Region>> regions;

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

    for (auto p : board->all_points())
    {
        if (auto it = visited.find(p); it != visited.end())
        {
            continue;
        }

        char id = board->at(p);
        regions.push_back(explore_region(board, visited, id, p));
    }

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
    auto board = read_board();
    unordered_set<Point> visited;
    vector<shared_ptr<Region>> regions;

    unordered_map<Point, shared_ptr<Region>> regions_by_point;

    for (auto p : board->all_points())
    {
        if (auto it = visited.find(p); it != visited.end())
        {
            continue;
        }

        char id = board->at(p);
        regions.push_back(explore_region(board, visited, id, p));
    }

    auto total_price = transform_reduce(
        regions.begin(), regions.end(),
        static_cast<uintmax_t>(0),
        plus<>{},
        [](const auto& r) { return static_cast<uintmax_t>(r->area()) * r->count_sides(); }
    );

    return to_string(total_price);
}

} // namespace day12
