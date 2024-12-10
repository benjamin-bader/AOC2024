#pragma once

#include "base.h"

#include <string>

namespace day09
{

struct PartOne : Problem
{
    static constexpr const char* expected = "6353658451014";

    std::string solve() override;
};

struct PartTwo : Problem
{
    static constexpr const char* expected = "6382582136592";

    std::string solve() override;
};

} // namespace day09
