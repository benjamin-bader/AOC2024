#pragma once

#include "base.h"

#include <string>

namespace day18
{

struct PartOne : Problem
{
    static constexpr const char* expected = "372";

    std::string solve() override;
};

struct PartTwo : Problem
{
    static constexpr const char* expected = "25,6";

    std::string solve() override;
};

} // namespace day18
