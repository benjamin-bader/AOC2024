#pragma once

#include "base.h"
#include "board.h"
#include "point.h"

#include <format>
#include <iostream>
#include <string>

namespace day15
{

class Warehouse
{
    Board board_;
    std::string steps_;
    Point robot_;

    friend std::ostream& operator<<(std::ostream& out, const Warehouse& w);

public:
    Warehouse(Board&& board, std::string&& steps, Point robot);

    void move_robot(Point dir);
    void apply_all_steps();
    void apply_all_steps_wide();

    uintmax_t gps() const;

private:
    Point dir_from_char(char c);

    bool can_move(Point p, Point dir);

    void move_wide(Point p, Point dir);
};

std::ostream& operator<<(std::ostream& out, const Warehouse& w);

} // namespace day15

template <>
struct fmt::formatter<day15::Warehouse> : fmt::ostream_formatter {};
