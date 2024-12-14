#include "day14.h"

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

namespace day14
{

namespace
{

constexpr const char* kInputFile = "day14/day14.input";
constexpr const char* kTestInput = "TODO";

[[maybe_unused]]
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
    return "TODO";
}

string PartTwo::solve()
{
    return "TODO";
}

} // namespace day14
