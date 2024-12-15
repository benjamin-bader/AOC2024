#include "day15.h"

#include "board.h"
#include "point.h"

#include <algorithm>
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

namespace day15
{

namespace
{

class Warehouse
{
    Board board_;
    string steps_;
    Point robot_;

    friend ostream& operator<<(ostream& out, const Warehouse& w);

public:
    Warehouse(Board&& board, string&& steps, Point robot)
        : board_(std::move(board))
        , steps_(std::move(steps))
        , robot_(robot)
    {}

    void move_robot(Point dir)
    {
        bool has_space = false;

        Point next = robot_ + dir;
        Point c = robot_ + dir;
        while (board_[c] != '#')
        {
            if (board_[c] == '.')
            {
                has_space = true;
                break;
            }
            c += dir;
        }

        if (!has_space)
        {
            // can't move
            return;
        }

        Point reverse{-dir.x(), -dir.y()};
        while (c != robot_)
        {
            swap(board_[c], board_[c + reverse]);
            c += reverse;
        }

        robot_ = next;
    }

    void apply_all_steps()
    {
        for (char c : steps_)
        {
            Point d = dir_from_char(c);
            move_robot(d);
        }
    }

    void apply_all_steps_wide()
    {
        for (char c : steps_)
        {
            Point d = dir_from_char(c);
            if (can_move(robot_, d))
            {
                move_wide(robot_, d);
                robot_ += d;
            }
            dbg(LogLevel::DEBUG) << "After step " << c << endl << *this << endl;
        }
    }

    uintmax_t gps() const
    {
        vector<Point> boxes = board_.all_points()
            | views::filter([this](auto p) { return board_[p] == 'O' || board_[p] == '['; })
            | ranges::to<vector<Point>>();

        return transform_reduce(
            boxes.begin(), boxes.end(),
            0ULL,
            plus<>{},
            [](Point p) { return static_cast<uintmax_t>(p.y()) * 100 + static_cast<uintmax_t>(p.x()); }
        );
    }

private:
    Point dir_from_char(char c)
    {
        switch (c)
        {
            case '^': return Dir::UP;
            case '>': return Dir::RIGHT;
            case 'v': return Dir::DOWN;
            case '<': return Dir::LEFT;
            default: throw invalid_argument("Invalid direction character");
        }
    }

    bool can_move(Point p, Point dir)
    {
        Point next = p + dir;

        char c_next = board_[next];
        if (c_next == '#')
        {
            return false;
        }

        if (c_next == '.')
        {
            return true;
        }

        if (c_next == '[')
        {
            return can_move(next, dir) && (dir == Dir::LEFT || can_move(next + Dir::RIGHT, dir));
        }

        if (c_next == ']')
        {
            return can_move(next, dir) && (dir == Dir::RIGHT || can_move(next + Dir::LEFT, dir));
        }

        throw invalid_argument("Invalid board state");
    }

    void move_wide(Point p, Point dir)
    {
        // PRECONDITION: can_move has already returned true

        Point next = p + dir;

        char n = board_[next];

        if (n == '.')
        {
            swap(board_[p], board_[next]);
        }
        else if (n == '[')
        {
            move_wide(next, dir);
            if (Dir::is_vertical(dir)) move_wide(next + Dir::RIGHT, dir);
            swap(board_[p], board_[next]);
        }
        else if (n == ']')
        {
            move_wide(next, dir);
            if (Dir::is_vertical(dir)) move_wide(next + Dir::LEFT, dir);
            swap(board_[p], board_[next]);
        }
    }
};

ostream& operator<<(ostream& out, const Warehouse& w)
{
    out << w.steps_.size() << " steps:\n";
    for (Point p : w.board_.all_points())
    {
        char c = w.board_[p];

        // if p is at the robot, highlight the cell yellow:
        if (p == w.robot_)
        {
            out << "\033[1;33m" << c << "\033[0m";
        }
        else
        {
            out << c;
        }

        if (p.x() == w.board_.num_cols() - 1)
        {
            out << '\n';
        }
    }
    return out;
}

constexpr const char* kInputFile = "day15/day15.input";
constexpr const char* kTestInput = R"(
##########
#..O..O.O#
#......O.#
#.OO..O.O#
#..O@..O.#
#O#..O...#
#O..O..O.#
#.OO.O.OO#
#....O...#
##########

<vv>^<v^>v>^vv^v>v<>v^v<v<^vv<<<^><<><>>v<vvv<>^v^>^<<<><<v<<<v^vv^v>^
vvv<<^>^v^^><<>>><>^<<><^vv^^<>vvv<>><^^v>^>vv<>v<<<<v<^v>^<^^>>>^<v<v
><>vv>v^v^<>><>>>><^^>vv>v<^^^>>v^v^<^^>v^^>v^<^v>v<>>v^v^<v>v^^<^^vv<
<<v<^>>^^^^>>>v^<>vvv^><v<<<>^^^vv^<vvv>^>v<^^^^v<>^>vvvv><>>v^<<^^^^^
^><^><>>><>^^<<^^v>>><^<v>^<vv>>v>>>^v><>^v><<<<v>>v<v<v>vvv>^<><<>^><
^>><>^v<><^vvv<^^<><v<<<<<><^v<<<><<<^^<v<^^^><^>>^<v^><<<^>>^v<v^v<v^
>^>>^v>vv>^<<^v<>><<><<v<<v><>v<^vv<<<>^^v^>^^>>><<^v>>v^v><^^>>^<>vv^
<><^^>^^^<><vvvvv^v<v<<>^v<v>v<<^><<><<><<<^^<<<^<<>><<><^^^>^^<>^>v<>
^^>vv<^v^v<vv>^<><v<^v>^^^>>>^^vvv^>vvv<>>>^<^>>>>>^<<^v>^vvv<>^<><<v>
v^^>>><<^^<>>^v^<v^vv<>v^<<>^<^v^v><^<<<><<^<v><v<>vv>>v><v^<vv<>v^<<^)";

[[maybe_unused]]
unique_ptr<istream> get_input()
{
    if (g_test_input)
    {
        return make_unique<stringstream>(kTestInput);
    }

    return make_unique<ifstream>(kInputFile);
}

Warehouse read_input()
{
    auto input = get_input();

    *input >> ws;

    vector<vector<char>> lines;
    string line;
    while (getline(*input, line) && !line.empty())
    {
        lines.emplace_back(line.begin(), line.end());
    }

    string steps;
    while (getline(*input, line))
    {
        steps += line;
    }

    Board board{lines};

    Point robot;
    for (Point p : board.all_points())
    {
        if (board[p] == '@')
        {
            robot = p;
            break;
        }
    }

    return {std::move(board), std::move(steps), robot};
}

Warehouse read_wide_input()
{
    auto input = get_input();

    *input >> ws;

    vector<vector<char>> lines;
    string line;
    while (getline(*input, line) && !line.empty())
    {
        vector<char> wide_line;
        wide_line.reserve(line.size() * 2);

        for (char c : line)
        {
            switch (c)
            {
            case '#':
                wide_line.push_back('#');
                wide_line.push_back('#');
                break;
            case '.':
                wide_line.push_back('.');
                wide_line.push_back('.');
                break;
            case 'O':
                wide_line.push_back('[');
                wide_line.push_back(']');
                break;
            case '@':
                wide_line.push_back('@');
                wide_line.push_back('.');
                break;
            }
        }

        lines.push_back(std::move(wide_line));
    }

    string steps;
    while (getline(*input, line))
    {
        steps += line;
    }

    Board board{std::move(lines)};

    Point robot;
    for (Point p : board.all_points())
    {
        if (board[p] == '@')
        {
            robot = p;
            break;
        }
    }

    return {std::move(board), std::move(steps), robot};
}

} // namespace

string PartOne::solve()
{
    Warehouse w = read_input();
    dbg() << "Before" << endl << w << endl;
    w.apply_all_steps();
    dbg() << "After" << endl << w << endl;
    return to_string(w.gps());
}

string PartTwo::solve()
{
    Warehouse w = read_wide_input();
    dbg() << "Before" << endl << w << endl;
    w.apply_all_steps_wide();
    dbg() << "After" << endl << w << endl;
    return to_string(w.gps());
}

} // namespace day15
