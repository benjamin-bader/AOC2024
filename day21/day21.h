#pragma once

#include "base.h"

#include <string>

namespace day21
{

struct PartOne : Problem
{
    static constexpr const char* expected = "184716";

    std::string solve() override;
};

struct PartTwo : Problem
{
    static constexpr const char* expected = "229403562787554";

    std::string solve() override;
};

} // namespace day21
