#pragma once

#include "base.h"

#include <string>

namespace day02
{

struct PartOne : Problem
{
    static constexpr const char* expected = "639";

    std::string solve() override;
};

struct PartTwo : Problem
{
    static constexpr const char* expected = "674";

    std::string solve() override;
};

}