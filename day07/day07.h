#pragma once

#include "base.h"

#include <string>

namespace day07
{

struct PartOne : Problem
{
    static constexpr const char* expected = "12553187650171";

    std::string solve() override;
};

struct PartTwo : Problem
{
    static constexpr const char* expected = "96779702119491";

    std::string solve() override;
};

} // namespace day07
