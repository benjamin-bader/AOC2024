#pragma once

#include "hash.h"

#include <array>
#include <concepts>
#include <iostream>
#include <numeric>

template <std::signed_integral T>
class BasicPoint
{
    T x_;
    T y_;

public:

    constexpr BasicPoint() : BasicPoint(0, 0) {}
    constexpr BasicPoint(T x, T y) : x_(x), y_(y) {}
    constexpr BasicPoint(const BasicPoint&) = default;
    constexpr BasicPoint(BasicPoint&&) noexcept = default;

    BasicPoint& operator=(const BasicPoint&) = default;
    BasicPoint& operator=(BasicPoint&&) noexcept = default;

    constexpr T x() const
    {
        return x_;
    }

    constexpr T y() const
    {
        return y_;
    }

    constexpr BasicPoint operator+(const BasicPoint& other) const
    {
        return {x_ + other.x_, y_ + other.y_};
    }

    BasicPoint& operator+=(const BasicPoint& other)
    {
        x_ += other.x_;
        y_ += other.y_;
        return *this;
    }

    constexpr BasicPoint operator-(const BasicPoint& other) const
    {
        return {x_ - other.x_, y_ - other.y_};
    }

    BasicPoint& operator-=(const BasicPoint& other)
    {
        x_ -= other.x_;
        y_ -= other.y_;
        return *this;
    }

    constexpr auto operator<=>(const BasicPoint& other) const = default;
};

template <std::signed_integral T>
std::ostream& operator<<(std::ostream& os, const BasicPoint<T>& p)
{
    return os << "(" << p.x() << ", " << p.y() << ")";
}

namespace std
{

template <std::signed_integral T>
struct hash<BasicPoint<T>>
{
    size_t operator()(const BasicPoint<T>& p) const
    {
        size_t seed = 1;
        hash_combine(seed, p.x());
        hash_combine(seed, p.y());
        return seed;
    }
};

} // namespace std

using Point = BasicPoint<int>;

namespace Dir
{

constexpr Point UP{0, -1};
constexpr Point DOWN{0, 1};
constexpr Point LEFT{-1, 0};
constexpr Point RIGHT{1, 0};

constexpr const auto CARDINALS = std::array {UP, RIGHT, DOWN, LEFT };

constexpr const auto ALL = std::array {UP, RIGHT, DOWN, LEFT, UP + RIGHT, UP + LEFT, DOWN + RIGHT, DOWN + LEFT};

constexpr bool is_vertical(const Point& p)
{
    return p.x() == 0;
}

constexpr bool is_horizontal(const Point& p)
{
    return p.y() == 0;
}

} // namespace Dir
