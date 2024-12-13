#pragma once

#include "base.h"

#include <string>

namespace day12
{

struct PartOne : Problem
{
    static constexpr const char* expected = "1377008";

    std::string solve() override;
};

struct PartTwo : Problem
{
    static constexpr const char* expected = "815788";

    std::string solve() override;
};

} // namespace day12
