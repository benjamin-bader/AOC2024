#pragma once

#include "base.h"

#include <string>

namespace day11
{

struct PartOne : Problem
{
    static constexpr const char* expected = "186203";

    std::string solve() override;
};

struct PartTwo : Problem
{
    static constexpr const char* expected = "221291560078593";

    std::string solve() override;
};

} // namespace day11
