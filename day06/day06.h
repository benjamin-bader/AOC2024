#pragma once

#include "base.h"

#include <string>

namespace day06
{

struct PartOne : Problem
{
    static constexpr const char* expected = "5080";

    std::string solve() override;
};

struct PartTwo : Problem
{
    static constexpr const char* expected = "1919";

    std::string solve() override;
};

} // namespace day06
