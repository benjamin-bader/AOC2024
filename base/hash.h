#pragma once

#include <numeric>
#include <utility>

template <typename T>
inline void hash_combine(std::size_t& seed, const T& input)
{
    // this is xorshift*, roughly
    seed ^= std::hash<T>()(input);
    seed ^= seed >> 12;
    seed ^= seed << 25;
    seed ^= seed >> 27;
    seed *= 0x2545F4914F6CDD1D;
}
