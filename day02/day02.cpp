#include "day02.h"

#include "parsers.h"

#include <algorithm>
#include <fstream>
#include <iostream>
#include <map>
#include <ranges>
#include <sstream>
#include <string>
#include <vector>

namespace day02
{

using namespace std;
using namespace std::string_view_literals;

using Report = vector<int>;

namespace
{

Report from_string(const string& s)
{
    Report report;
    stringstream ss(s);
    int x;
    while (ss >> x)
    {
        report.push_back(x);
    }
    return report;
}

vector<Report> read_reports()
{
    vector<Report> reports;

    ifstream input("day02/day02.input");
    return parsers::Lines(input, from_string);
}

bool is_increasing(const Report& xs)
{
    return is_sorted(xs.begin(), xs.end());
};

bool is_decreasing(const Report& xs)
{
    return is_sorted(xs.rbegin(), xs.rend());
};

bool is_ordered(const Report& xs)
{
    return is_increasing(xs) || is_decreasing(xs);
};

bool is_gradual(const Report& xs)
{
    for (size_t i = 1; i < xs.size(); ++i)
    {
        auto delta = abs(xs[i] - xs[i - 1]);
        if (delta < 1 || delta > 3) return false;
    }

    return true;
};

bool is_safe(const Report& xs)
{
    return is_ordered(xs) && is_gradual(xs);
};

bool is_safe_with_dampener(const Report& xs)
{
    if (is_safe(xs)) return true;

    for (size_t i = 0; i < xs.size(); i++)
    {
        Report modified_xs = xs;
        modified_xs.erase(modified_xs.begin() + i);
        if (is_safe(modified_xs)) return true;
    }

    return false;
};

} // namespace

string PartOne::solve()
{
    auto reports = read_reports();

    auto safe_reports = reports | views::filter(is_safe);

    return to_string(ranges::distance(safe_reports));
}

std::string PartTwo::solve()
{
    auto reports = read_reports();

    auto safe_reports = reports | views::filter(is_safe_with_dampener);

    return to_string(ranges::distance(safe_reports));
}

} // namespace day02
