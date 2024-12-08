#pragma once

#include "board.h"

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <tuple>
#include <type_traits>
#include <utility>
#include <vector>

namespace parsers
{

template <typename TIn, typename TOut>
using Parser = TOut (*)(TIn);

inline std::vector<std::string> Lines(std::istream& in)
{
    std::vector<std::string> lines;
    std::string line;
    while (std::getline(in, line))
    {
        lines.push_back(std::move(line));
    }
    return lines;
}

inline std::vector<std::string> Lines(const std::string& s)
{
    std::istringstream ss(s);
    return Lines(ss);
}

template <typename F>
std::vector<std::invoke_result_t<F, std::string>> Lines(std::istream& in, F&& fn)
{
    std::vector<std::invoke_result_t<F, std::string>> lines;
    std::string line;
    while (std::getline(in, line))
    {
        auto result = fn(line);
        lines.push_back(std::move(result));
    }
    return lines;
}

template <typename F>
std::vector<std::invoke_result_t<F, std::string>> Lines(const std::string& s, F&& fn)
{
    std::istringstream ss(s);
    return Lines(ss, fn);
}

inline std::string String(std::istream& in)
{
    std::stringstream ss;
    ss << in.rdbuf();
    return ss.str();
}

inline std::vector<char> Chars(const std::string& s)
{
    return std::vector<char>(s.begin(), s.end());
}

template <typename TIn, typename ...TOut>
Parser<TIn, std::tuple<TOut...>> Sequence(Parser<TIn, TOut>... parsers)
{
    return [parsers...](TIn& in) {
        return std::make_tuple(parsers(in)...);
    };
}

template <typename TIn, typename TMiddle, typename TOut>
Parser<TIn, TOut> Compose(Parser<TIn, TMiddle> p1, Parser<TMiddle, TOut> p2)
{
    return [p1, p2](TIn& in) {
        TMiddle middle = p1(in);
        return p2(middle);
    };
}

} // namespace parsers