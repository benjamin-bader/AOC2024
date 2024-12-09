#pragma once

#include <concepts>
#include <string>
#include <type_traits>
#include <utility>

extern bool g_verbose;

std::ostream& dbg();

class Problem
{
public:
    virtual ~Problem() = default;

    virtual std::string solve() = 0;
};

template <typename P>
concept IsProblem = std::derived_from<P, Problem>;

template <typename P>
concept HasSolution = IsProblem<P> && requires
{
    { P::expected } -> std::convertible_to<std::string>;
};
