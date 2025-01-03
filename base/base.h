#pragma once

#include <concepts>
#include <iostream>
#include <string>
#include <type_traits>
#include <utility>

extern bool g_test_input;
extern int g_verbose;

enum class LogLevel
{
    ERROR = 0,
    INFO,
    DEBUG,
    TRACE
};

std::ostream& dbg(LogLevel level = LogLevel::INFO);

constexpr std::size_t operator "" _z(unsigned long long n)
{
    return static_cast<std::size_t>(n);
}

constexpr std::uintmax_t operator "" _um(unsigned long long n)
{
    return static_cast<std::uintmax_t>(n);
}

std::size_t stoz(const std::string& str);

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
