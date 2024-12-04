#include "day04.h"

#include <algorithm>
#include <fstream>
#include <iostream>
#include <ranges>
#include <string>
#include <string_view>
#include <unordered_map>
#include <unordered_set>
#include <vector>

using namespace std;

namespace day04
{

struct Point
{
    int x;
    int y;

    Point(int x, int y) : x(x), y(y) {}
    Point(const Point& other) : x(other.x), y(other.y) {}
    Point(Point&& other) noexcept : x(other.x), y(other.y) {}

    Point& operator=(const Point& other) = default;
    Point& operator=(Point&& other) noexcept = default;

    bool operator==(const Point& other) const = default;

    Point operator+(const Point& other) const
    {
        return Point{x + other.x, y + other.y};
    }
};

class Board
{
    vector<vector<char>> contents_;
    int num_rows_;
    int num_cols_;

public:
    Board(vector<vector<char>> contents)
        : contents_(contents)
        , num_rows_(static_cast<int>(contents.size()))
        , num_cols_(static_cast<int>(contents[0].size()))
    {}

    int num_rows() const
    {
        return num_rows_;
    }

    int num_cols() const
    {
        return num_cols_;
    }

    char at(Point p) const
    {
        return contents_[p.y][p.x];
    }

    bool in_bounds(Point p) const
    {
        return p.y >= 0
            && p.y < num_rows_
            && p.x >= 0
            && p.x < num_cols_;
    }

    auto all_points() const
    {
        int cols = num_cols();

        auto to_point = [cols](int n) {
            int x = n % cols;
            int y = n / cols;
            return Point{x, y};
        };

        return views::iota(0, num_rows() * num_cols()) | views::transform(to_point);
    }
};

namespace
{

constexpr const char* kInputFile = "day04/day04.input";

Board read_board()
{
    string line;
    ifstream file(kInputFile);
    vector<vector<char>> board;
    while (getline(file, line))
    {
        board.push_back(vector<char>(line.cbegin(), line.cend()));
    }
    return {board};
}

bool find_xmas(const Board& board, Point cur, Point dir, string_view next)
{
    if (next.empty())
    {
        return true;
    }

    if (!board.in_bounds(cur) || board.at(cur) != next[0])
    {
        return false;
    }

    return find_xmas(board, cur + dir, dir, next.substr(1));
}

bool find_mas_in_x_shape(const Board& board, Point cur)
{
    // We're looking for two MAS in an X shape
    // for example:
    //
    // M   S      S   S
    //   A    or    A
    // M   S      M   M
    //
    // or similar.

    // PRECONDITION: cur is not on the outermost edge of the board

    char ul = board.at({cur.x - 1, cur.y - 1});
    char ur = board.at({cur.x + 1, cur.y - 1});
    char ll = board.at({cur.x - 1, cur.y + 1});
    char lr = board.at({cur.x + 1, cur.y + 1});

    if (board.at(cur) != 'A')
    {
        return false;
    }

    bool ul_to_lr = ((ul == 'M' && lr == 'S') || (ul == 'S' && lr == 'M'));
    bool ll_to_ur = ((ll == 'M' && ur == 'S') || (ll == 'S' && ur == 'M'));

    return ul_to_lr && ll_to_ur;
}

} // namespace

string PartOne::solve()
{
    const Board board = read_board();

    vector<Point> dirs{{0, 1}, {1, 0}, {0, -1}, {-1, 0}, {1, 1}, {1, -1}, {-1, 1}, {-1, -1}};
    size_t num_xmases = 0;

    for (auto p : board.all_points())
    {
        for (auto& dir : dirs)
        {
            num_xmases += find_xmas(board, p, dir, "XMAS") ? 1 : 0;
        }
    }

    return to_string(num_xmases);
}

string PartTwo::solve()
{
    const Board board = read_board();
    auto possible_points = board.all_points() | views::filter([&](Point p) {
        return p.x != 0
            && p.y != 0
            && p.x != board.num_cols() - 1
            && p.y != board.num_rows() - 1;
    });

    auto num_x_mas = ranges::fold_left(possible_points, 0, [&](int acc, Point p) {
        return acc + (find_mas_in_x_shape(board, p) ? 1 : 0);
    });

    return to_string(num_x_mas);
}

} // namespace day04
