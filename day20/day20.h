#pragma once

#include "base.h"

#include <string>

namespace day20
{

struct PartOne : Problem
{
    static constexpr const char* expected = "1351";

    std::string solve() override;
};

struct PartTwo : Problem
{
    static constexpr const char* expected = "966130";

    std::string solve() override;
};

} // namespace day20
