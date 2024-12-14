#include "day04.h"

#include "board.h"
#include "parsers.h"
#include "point.h"

#include <algorithm>
#include <fstream>
#include <iostream>
#include <memory>
#include <ranges>
#include <string>
#include <string_view>
#include <unordered_map>
#include <unordered_set>
#include <vector>

using namespace std;

namespace day04
{

namespace
{

constexpr const char* kInputFile = "day04/day04.input";
constexpr const char* kTestInput = R"(MMMSXXMASM
MSAMXMSMSA
AMXSXMAAMM
MSAMASMSMX
XMASAMXAMM
XXAMMXXAMA
SMSMSASXSS
SAXAMASAAA
MAMMMXMMMM
MXMXAXMASX)";

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
    auto board = parsers::Lines(*input, [](string s) { return vector<char>(s.cbegin(), s.cend()); });
    return {std::move(board)};
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

    char ul = board.at({cur.x() - 1, cur.y() - 1});
    char ur = board.at({cur.x() + 1, cur.y() - 1});
    char ll = board.at({cur.x() - 1, cur.y() + 1});
    char lr = board.at({cur.x() + 1, cur.y() + 1});

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
        return p.x() != 0
            && p.y() != 0
            && p.x() != board.num_cols() - 1
            && p.y() != board.num_rows() - 1;
    });

    auto num_x_mas = ranges::fold_left(possible_points, 0, [&](int acc, Point p) {
        return acc + (find_mas_in_x_shape(board, p) ? 1 : 0);
    });

    return to_string(num_x_mas);
}

} // namespace day04
