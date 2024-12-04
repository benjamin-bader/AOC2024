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

    Point& operator=(const Point& other)
    {
        x = other.x;
        y = other.y;
        return *this;
    }

    Point& operator=(Point&& other) noexcept
    {
        x = other.x;
        y = other.y;
        return *this;
    }

    bool operator==(const Point& other) const
    {
        return x == other.x && y == other.y;
    }

    Point operator+(const Point& other) const
    {
        return Point{x + other.x, y + other.y};
    }
};

using Board = vector<vector<char>>;

namespace
{

constexpr const char* kInputFile = "day04/day04.input";

Board read_board()
{
    string line;
    ifstream file(kInputFile);
    Board board;
    while (getline(file, line))
    {
        board.push_back(vector<char>(line.cbegin(), line.cend()));
    }
    return board;
}

auto all_points(const Board& board)
{
    size_t num_rows = board.size();
    size_t num_cols = board[0].size();
    size_t num_cells = num_rows * num_cols;

    auto to_point = [num_cols](size_t n) {
        int x = static_cast<int>(n) % static_cast<int>(num_cols);
        int y = static_cast<int>(n) / static_cast<int>(num_cols);
        return Point{x, y};
    };

    return views::iota(static_cast<size_t>(0), num_cells) | views::transform(to_point);
}

bool point_in_board(const Board& board, Point p)
{
    return p.y >= 0
        && p.y < static_cast<int>(board.size())
        && p.x >= 0
        && p.x < static_cast<int>(board[0].size());
}

char char_at(const Board& board, Point p)
{
    return board[p.y][p.x];
}

bool find_xmas(const Board& board, Point cur, Point dir, string_view next)
{
    if (next.empty())
    {
        return true;
    }

    if (!point_in_board(board, cur) || char_at(board, cur) != next[0])
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

    char ul = char_at(board, {cur.x - 1, cur.y - 1});
    char ur = char_at(board, {cur.x + 1, cur.y - 1});
    char ll = char_at(board, {cur.x - 1, cur.y + 1});
    char lr = char_at(board, {cur.x + 1, cur.y + 1});

    auto dirs = vector<Point>{{0, 1}, {1, 0}, {0, -1}, {-1, 0}};

    if (char_at(board, cur) != 'A')
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

    for (auto p : all_points(board))
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
    auto possible_points = all_points(board) | views::filter([&](Point p) {
        return p.x != 0
            && p.y != 0
            && p.x != static_cast<int>(board[0].size()) - 1
            && p.y != static_cast<int>(board.size()) - 1;
    });

    auto num_x_mas = ranges::fold_left(possible_points, 0, [&](int acc, Point p) {
        return acc + (find_mas_in_x_shape(board, p) ? 1 : 0);
    });

    return to_string(num_x_mas);
}

} // namespace day04
