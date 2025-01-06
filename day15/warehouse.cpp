#include "warehouse.h"

#include <ranges>
#include <utility>
#include <vector>

using namespace std;

namespace day15
{

Warehouse::Warehouse(Board&& board, string&& steps, Point robot)
        : board_(std::move(board))
        , steps_(std::move(steps))
        , robot_(robot)
    {}

void Warehouse::move_robot(Point dir)
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
        std::swap(board_[c], board_[c + reverse]);
        c += reverse;
    }

    robot_ = next;
}

void Warehouse::apply_all_steps()
{
    for (char c : steps_)
    {
        Point d = dir_from_char(c);
        move_robot(d);
    }
}

void Warehouse::apply_all_steps_wide()
{
    for (char c : steps_)
    {
        Point d = dir_from_char(c);
        if (can_move(robot_, d))
        {
            move_wide(robot_, d);
            robot_ += d;
        }
        const Warehouse& w = *this;
        dbg("After step {}", w);
    }
}

uintmax_t Warehouse::gps() const
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

Point Warehouse::dir_from_char(char c)
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

bool Warehouse::can_move(Point p, Point dir)
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

void Warehouse::move_wide(Point p, Point dir)
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

std::ostream& operator<<(std::ostream& out, const Warehouse& w)
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

} // namespace day15
