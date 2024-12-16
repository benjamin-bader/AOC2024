#pragma once

#include "base.h"

#include <string>

namespace day16
{

struct PartOne : Problem
{
    static constexpr const char* expected = "85420";

    std::string solve() override;
};

struct PartTwo : Problem
{
    static constexpr const char* expected = "492";

    std::string solve() override;
};

} // namespace day16
