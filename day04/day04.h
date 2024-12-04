#pragma once

#include "base.h"

#include <string>

namespace day04
{

struct PartOne : Problem
{
    static constexpr const char* expected = "2532";

    std::string solve() override;
};

struct PartTwo : Problem
{
    static constexpr const char* expected = "1941";

    std::string solve() override;
};

} // namespace day04
