#pragma once

#include <concepts>
#include <iostream>
#include <string>
#include <type_traits>
#include <utility>

#include <fmt/format.h>
#include <fmt/ostream.h>
#include <fmt/std.h>

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

template <typename... T>
void dbg(const fmt::format_string<T...> message, T&&... args)
{
    dbg(LogLevel::INFO, message, std::forward<T>(args)...);
}

template <typename... T>
void dbg(LogLevel level, const fmt::format_string<T...> message, T&&... args)
{
    if (g_verbose >= static_cast<int>(level))
    {
        fmt::println(message, std::forward<T>(args)...);
    }
}

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
