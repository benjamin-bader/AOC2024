#pragma once

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
};

inline bool operator==(const Point& lhs, const Point& rhs)
{
    return (lhs.x() == rhs.x()) && lhs.y() == rhs.y();
}

inline bool operator!=(const Point& lhs, const Point& rhs)
{
    return !(lhs == rhs);
}
