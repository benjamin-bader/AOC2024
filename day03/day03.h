#pragma once

#include "base.h"

#include <string>

namespace day03
{

struct PartOne : Problem
{
    static constexpr const char* expected = "173731097";

    std::string solve() override;
};

struct PartTwo : Problem
{
    static constexpr const char* expected = "93729253";

    std::string solve() override;
};

} // namespace day03
