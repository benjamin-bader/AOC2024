#include "day01.h"

#include <algorithm>
#include <fstream>
#include <iostream>
#include <map>
#include <memory>
#include <numeric>
#include <ranges>
#include <sstream>
#include <vector>

namespace day01
{

using namespace std;

namespace
{

constexpr const char* kInputFile = "day01/day01.input";
constexpr const char* kTestInput = R"(3   4
4   3
2   5
1   3
3   9
3   3)";

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
    vector<int> left, right;
    auto input = get_input();

    int x, y;
    while (*input >> x >> y)
    {
        left.push_back(x);
        right.push_back(y);
    }

    sort(left.begin(), left.end());
    sort(right.begin(), right.end());

    auto diff = [](tuple<int, int> xy) { return abs(get<0>(xy) - get<1>(xy)); };

    // we _were_ using zip_transform, but AppleClang sux and doesn't have it
    auto xs = views::zip(left, right) | views::transform(diff);
    return to_string(accumulate(xs.begin(), xs.end(), 0L));
}

string PartTwo::solve()
{
    vector<int> left;
    map<int, int> right_column_counts;
    auto input = get_input();

    int x, y;
    while (*input >> x >> y)
    {
        left.push_back(x);
        right_column_counts[y]++;
    }

    long score = 0;
    for (int n : left)
    {
        score += (n * right_column_counts[n]);
    }

    return to_string(score);
}

} // namespace day01
