#pragma once

#include "base.h"

#include <string>

namespace day15
{

struct PartOne : Problem
{
    static constexpr const char* expected = "1485257";

    std::string solve() override;
};

struct PartTwo : Problem
{
    static constexpr const char* expected = "1475512";

    std::string solve() override;
};

} // namespace day15
