#pragma once

#include "base.h"

#include <string>

namespace day19
{

struct PartOne : Problem
{
    static constexpr const char* expected = "247";

    std::string solve() override;
};

struct PartTwo : Problem
{
    static constexpr const char* expected = "692596560138745";

    std::string solve() override;
};

} // namespace day19
