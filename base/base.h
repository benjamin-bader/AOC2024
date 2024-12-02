#pragma once

#include <concepts>
#include <string>
#include <type_traits>
#include <utility>

class Problem
{
public:
    virtual ~Problem() = default;

    virtual std::string solve() = 0;
};

template <typename P>
concept HasSolution = std::derived_from<P, Problem> && requires
{
    { P::expected } -> std::convertible_to<std::string>;
};
