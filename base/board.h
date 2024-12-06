#pragma once

#include "point.h"

#include <ranges>
#include <vector>

template <typename T>
class BasicBoard
{
    std::vector<std::vector<T>> contents_;
    int num_rows_;
    int num_cols_;

public:
    BasicBoard(const std::vector<std::vector<T>>& contents)
        : contents_(contents)
        , num_rows_(static_cast<int>(contents.size()))
        , num_cols_(static_cast<int>(contents[0].size()))
    {}

    BasicBoard(std::vector<std::vector<T>>&& contents)
        : contents_(contents)
        , num_rows_(static_cast<int>(contents.size()))
        , num_cols_(static_cast<int>(contents[0].size()))
    {}

    BasicBoard(const BasicBoard&) = delete;
    BasicBoard(BasicBoard&&) noexcept = default;

    int num_rows() const
    {
        return num_rows_;
    }

    int num_cols() const
    {
        return num_cols_;
    }

    T at(const Point& p) const
    {
        return contents_[p.y()][p.x()];
    }

    bool in_bounds(Point p) const
    {
        return p.y() >= 0
            && p.y() < num_rows_
            && p.x() >= 0
            && p.x() < num_cols_;
    }

    auto all_points() const
    {
        int cols = num_cols();

        auto to_point = [cols](int n) {
            int x = n % cols;
            int y = n / cols;
            return Point{x, y};
        };

        return std::views::iota(0, num_rows() * num_cols()) | std::views::transform(to_point);
    }
};

using Board = BasicBoard<char>;
