#pragma once

#include "base.h"

#include <string>

namespace day22
{

struct PartOne : Problem
{
    static constexpr const char* expected = "14273043166";

    std::string solve() override;
};

struct PartTwo : Problem
{
    static constexpr const char* expected = "1667";

    std::string solve() override;
};

} // namespace day22
