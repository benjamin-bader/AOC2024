#include "day01.h"

#include <algorithm>
#include <fstream>
#include <iostream>
#include <map>
#include <numeric>
#include <ranges>
#include <vector>

namespace day01
{

using namespace std;

string PartOne::solve()
{
    vector<int> left, right;
    ifstream input("day01/day01.input");

    int x, y;
    while (input >> x >> y)
    {
        left.push_back(x);
        right.push_back(y);
    }

    sort(left.begin(), left.end());
    sort(right.begin(), right.end());

    auto diff = [](int x, int y) { return abs(x - y); };
    auto xs = views::zip_transform(diff, left, right);
    return to_string(accumulate(xs.begin(), xs.end(), 0L));
}

string PartTwo::solve()
{
    vector<int> left;
    map<int, int> right_column_counts;
    ifstream input("day01/day01.input");

    int x, y;
    while (input >> x >> y)
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
