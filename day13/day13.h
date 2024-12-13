#pragma once

#include "base.h"

#include <string>

namespace day13
{

struct PartOne : Problem
{
    static constexpr const char* expected = "35997";

    std::string solve() override;
};

struct PartTwo : Problem
{
    static constexpr const char* expected = "82510994362072";

    std::string solve() override;
};

} // namespace day13
