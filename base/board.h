#pragma once

#include "point.h"

#include <array>
#include <cassert>
#include <iostream>
#include <ranges>
#include <string>
#include <vector>

template <typename T>
class BasicBoard
{
    std::vector<std::vector<T>> contents_;
    int num_rows_;
    int num_cols_;

    template <typename U>
    friend std::ostream& operator<<(std::ostream& os, const BasicBoard<U>& board);

    template <typename U>
    friend std::istream& operator>>(std::istream& is, BasicBoard<U>& board);

public:
    BasicBoard()
        : contents_{}
        , num_rows_{0}
        , num_cols_{0}
    {}

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

    BasicBoard(const BasicBoard&) = default;
    BasicBoard(BasicBoard&&) noexcept = default;

    BasicBoard& operator=(const BasicBoard&) = default;
    BasicBoard& operator=(BasicBoard&&) noexcept = default;

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
        assert(p.x() >= 0);
        assert(p.y() >= 0);
        return contents_[static_cast<size_t>(p.y())][static_cast<size_t>(p.x())];
    }

    T& at(const Point& p)
    {
        assert(p.x() >= 0);
        assert(p.y() >= 0);
        return contents_[static_cast<size_t>(p.y())][static_cast<size_t>(p.x())];
    }

    const T& operator[](const Point& p) const
    {
        assert(p.x() >= 0);
        assert(p.y() >= 0);
        return contents_[static_cast<size_t>(p.y())][static_cast<size_t>(p.x())];
    }

    T& operator[](const Point& p)
    {
        assert(p.x() >= 0);
        assert(p.y() >= 0);
        return contents_[static_cast<size_t>(p.y())][static_cast<size_t>(p.x())];
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

    auto cardinal_neighbors(Point point) const
    {
        return Dir::CARDINALS
            | std::views::transform([point](Point dir) { return point + dir; })
            | std::views::filter([this](Point p) { return in_bounds(p); });
    }
};

template <typename T>
std::ostream& operator<<(std::ostream& out, const BasicBoard<T>& board)
{
    for (int y = 0; y < board.num_rows(); ++y)
    {
        for (int x = 0; x < board.num_cols(); ++x)
        {
            out << board.contents_[y][x];
        }
        out << '\n';
    }

    return out;
}

template <typename T>
std::istream& operator>>(std::istream& in, BasicBoard<T>& board)
{
    in >> std::ws;

    std::string line;
    while (getline(in, line) && !line.empty())
    {
        board.contents_.emplace_back(line.begin(), line.end());
    }

    board.num_rows_ = static_cast<int>(board.contents_.size());
    board.num_cols_ = static_cast<int>(board.contents_[0].size());

    return in;
}

using Board = BasicBoard<char>;
