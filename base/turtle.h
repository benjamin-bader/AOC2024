#pragma once

#include "hash.h"
#include "point.h"

#include <compare>
#include <iostream>

class Turtle
{
    Point p_;
    Point d_;

public:
    Turtle() = default;
    constexpr Turtle(Point p, Point d) : p_(p), d_(d) {}
    constexpr Turtle(const Turtle&) = default;
    constexpr Turtle(Turtle&&) noexcept = default;

    Turtle& operator=(const Turtle&) = default;
    Turtle& operator=(Turtle&&) noexcept = default;

    Point position() const
    {
        return p_;
    }

    Point direction() const
    {
        return d_;
    }

    Turtle move_forward() const
    {
        return {p_ + d_, d_};
    }

    Turtle turn_right() const
    {
        using namespace Dir;

        if (d_ == UP) { return {p_, RIGHT}; }
        if (d_ == RIGHT) { return {p_, DOWN}; }
        if (d_ == DOWN) { return {p_, LEFT}; }
        if (d_ == LEFT) { return {p_, UP}; }
        throw "Invalid direction";
    }

    auto operator<=>(const Turtle&) const = default;
};

inline std::ostream& operator<<(std::ostream& os, const Turtle& t)
{
    return os << "T(" << t.position() << ", " << t.direction() << ")";
}

namespace std
{

template <>
struct hash<Turtle>
{
    size_t operator()(const Turtle& t) const
    {
        size_t seed = 1;
        hash_combine(seed, t.position());
        hash_combine(seed, t.direction());
        return seed;
    }
};

} // namespace std
