#pragma once

#include "base.h"

#include <string>

namespace day05
{

struct PartOne : Problem
{
    static constexpr const char* expected = "5955";

    std::string solve() override;
};

struct PartTwo : Problem
{
    static constexpr const char* expected = "4030";

    std::string solve() override;
};

} // namespace day05
