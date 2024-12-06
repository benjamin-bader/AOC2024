#include "day03.h"

#include "parsers.h"

#include <algorithm>
#include <fstream>
#include <iostream>
#include <regex>
#include <sstream>
#include <string>

using namespace std;

namespace day03
{

namespace
{

constexpr const char* kInputFile = "day03/day03.input";

const regex kMulExpr{R"(mul\((\d{1,3}),(\d{1,3})\))"};
const regex kMulExprWithConditionals{R"(do\(\)|mul\((\d{1,3}),(\d{1,3})\)|don't\(\))"};

string read_input()
{
    ifstream file(kInputFile);
    return parsers::String(file);
}

} // namespace

string PartOne::solve()
{
    string input = read_input();

    sregex_iterator it(input.cbegin(), input.cend(), kMulExpr);
    sregex_iterator end{};
    auto total = ranges::fold_left(it, end, 0ull, [&](unsigned long long acc, const smatch& m) {
        return acc + (stoull(m[1]) * stoull(m[2]));
    });
    
    return to_string(total);
}

string PartTwo::solve()
{
    string input = read_input();
    bool doing = true;

    sregex_iterator it(input.cbegin(), input.cend(), kMulExprWithConditionals);
    sregex_iterator end{};
    auto total = ranges::fold_left(it, end, 0ull, [&](unsigned long long acc, const smatch& m) {
        if (m[0] == "do()")
        {
            doing = true;
        }
        else if (m[0] == "don't()")
        {
            doing = false;
        }
        else if (doing)
        {
            acc += (stoull(m[1]) * stoull(m[2]));
        }
        return acc;
    });

    return to_string(total);
}

} // namespace day03
