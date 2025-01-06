#pragma once

#include <cmath>
#include <concepts>
#include <numeric>

namespace Numbers
{

template <std::unsigned_integral T>
constexpr std::size_t count_digits(T n)
{
    if (n == 0)
    {
        return 1;
    }

    std::size_t count = 0;
    while (n > 0)
    {
        n /= 10;
        count++;
    }

    return count;
}

template <std::unsigned_integral T>
constexpr T shl10(T operand, std::size_t shift)
{
    T tmp = shift;
    while (tmp > 0)
    {
        operand *= 10;
        tmp--;
    }
    return operand;
}

};
