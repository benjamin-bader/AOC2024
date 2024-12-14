#pragma once

#include "base.h"

#include <string>

namespace day14
{

struct PartOne : Problem
{
    static constexpr const char* expected = "221142636";

    std::string solve() override;
};

struct PartTwo : Problem
{
    static constexpr const char* expected = "7916";

    std::string solve() override;
};

} // namespace day14
