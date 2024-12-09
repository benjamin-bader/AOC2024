#include "day03.h"

#include "parsers.h"

#include <algorithm>
#include <fstream>
#include <iostream>
#include <memory>
#include <regex>
#include <sstream>
#include <string>

using namespace std;

namespace day03
{

namespace
{

constexpr const char* kInputFile = "day03/day03.input";
constexpr const char* kTestInput = "xmul(2,4)&mul[3,7]!^don't()_mul(5,5)+mul(32,64](mul(11,8)undo()?mul(8,5))";

const regex kMulExpr{R"(mul\((\d{1,3}),(\d{1,3})\))"};
const regex kMulExprWithConditionals{R"(do\(\)|mul\((\d{1,3}),(\d{1,3})\)|don't\(\))"};

unique_ptr<istream> get_input()
{
    if (g_test_input)
    {
        return make_unique<stringstream>(kTestInput);
    }

    return make_unique<ifstream>(kInputFile);
}

string read_input()
{
    auto input = get_input();
    return parsers::String(*input);
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
