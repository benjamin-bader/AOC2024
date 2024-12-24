#pragma once

#include "base.h"

#include <string>

namespace day23
{

struct PartOne : Problem
{
    static constexpr const char* expected = "1175";

    std::string solve() override;
};

struct PartTwo : Problem
{
    static constexpr const char* expected = "bw,dr,du,ha,mm,ov,pj,qh,tz,uv,vq,wq,xw";

    std::string solve() override;
};

} // namespace day23
