#include "day10.h"

#include <algorithm>
#include <exception>
#include <fstream>
#include <iostream>
#include <memory>
#include <numeric>
#include <ranges>
#include <sstream>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

using namespace std;

namespace day10
{

namespace
{

constexpr const char* kInputFile = "day10/day10.input";
constexpr const char* kTestInput = R"(89010123
78121874
87430965
96549874
45678903
32019012
01329801
10456732)";

unique_ptr<istream> get_input()
{
    if (g_test_input)
    {
        return make_unique<stringstream>(kTestInput);
    }

    return make_unique<ifstream>(kInputFile);
}

} // namespace

string PartOne::solve()
{
    (void)get_input();
    return "TODO";
}

string PartTwo::solve()
{
    return "TODO";
}

} // namespace day10
