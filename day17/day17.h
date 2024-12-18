#pragma once

#include "base.h"

#include <string>

namespace day17
{

struct PartOne : Problem
{
    static constexpr const char* expected = "1,5,0,1,7,4,1,0,3";

    std::string solve() override;
};

struct PartTwo : Problem
{
    static constexpr const char* expected = "47910079998866";

    std::string solve() override;
};

} // namespace day17
