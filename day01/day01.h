#pragma once

#include "base.h"

#include <string>

namespace day01
{

struct PartOne : Problem
{
    static constexpr const char* expected = "1830467";

    std::string solve() override;
};

struct PartTwo : Problem
{
    static constexpr const char* expected = "26674158";

    std::string solve() override;
};

}