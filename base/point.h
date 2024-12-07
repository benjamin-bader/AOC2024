#pragma once

#include "hash.h"

#include <iostream>

class Point
{
    int x_;
    int y_;

public:

    Point() = default;
    constexpr Point(int x, int y) : x_(x), y_(y) {}
    constexpr Point(const Point&) = default;
    constexpr Point(Point&&) noexcept = default;

    Point& operator=(const Point&) = default;
    Point& operator=(Point&&) noexcept = default;

    int x() const
    {
        return x_;
    }

    int y() const
    {
        return y_;
    }

    Point operator+(const Point& other) const
    {
        return {x_ + other.x_, y_ + other.y_};
    }

    auto operator<=>(const Point& other) const = default;

    bool operator==(const Point& other) const
    {
        return x_ == other.x_ && y_ == other.y_;
    }
};

inline std::ostream& operator<<(std::ostream& os, const Point& p)
{
    return os << "(" << p.x() << ", " << p.y() << ")";
}

namespace std
{

template <>
struct hash<Point>
{
    size_t operator()(const Point& p) const
    {
        size_t seed = 1;
        hash_combine(seed, p.x());
        hash_combine(seed, p.y());
        return seed;
    }
};

} // namespace std

namespace Dir
{

constexpr Point UP{0, -1};
constexpr Point DOWN{0, 1};
constexpr Point LEFT{-1, 0};
constexpr Point RIGHT{1, 0};

} // namespace Dir
