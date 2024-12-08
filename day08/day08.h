#pragma once

#include "base.h"

#include <string>

namespace day08
{

struct PartOne : Problem
{
    static constexpr const char* expected = "423";

    std::string solve() override;
};

struct PartTwo : Problem
{
    static constexpr const char* expected = "1287";

    std::string solve() override;
};

} // namespace day08
