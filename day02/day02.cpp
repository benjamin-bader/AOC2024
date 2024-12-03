#include "day02.h"

#include <algorithm>
#include <fstream>
#include <iostream>
#include <map>
#include <numeric>
#include <ranges>
#include <sstream>
#include <string>
#include <vector>

namespace day02
{

using namespace std;

namespace
{

vector<vector<int>> read_reports()
{
    vector<vector<int>> reports;

    ifstream input("day02/day02.input");
    string line;

    while (getline(input, line))
    {
        stringstream ss(line);
        vector<int> report;
        int x;
        while (ss >> x)
        {
            report.push_back(x);
        }
        reports.push_back(report);
    }

    return reports;
}

bool is_increasing(const vector<int>& xs) {
    return is_sorted(xs.begin(), xs.end());
};

bool is_decreasing(const vector<int>& xs) {
    return is_sorted(xs.rbegin(), xs.rend());
};

bool is_ordered(const vector<int>& xs) {
    return is_increasing(xs) || is_decreasing(xs);
};

bool is_gradual(const vector<int>& nums) {
    //auto deltas = views::iota(static_cast<size_t>(1), nums.size()) | views::transform([&](int i) { return abs(nums[i] - nums[i - 1]); });

    vector<int> deltas;
    for (size_t i = 1; i < nums.size(); i++)
    {
        deltas.push_back(abs(nums[i] - nums[i - 1]));
    }

    for (auto d : deltas)
    {
        if (d < 1 || d > 3) return false;
    }

    return true;
};

} // namespace

string PartOne::solve()
{
    auto reports = read_reports();

    auto safe_reports = reports | views::filter(is_ordered) | views::filter(is_gradual);

    return to_string(ranges::distance(safe_reports));
}

std::string PartTwo::solve()
{
    auto reports = read_reports();

    auto is_safe = [&](const vector<int>& report) {
        return is_ordered(report) && is_gradual(report);
    };

    auto is_safe_with_problem_dampener = [&](const vector<int>& report) {
        if (is_safe(report)) return true;

        for (size_t i = 0; i < report.size(); i++)
        {
            vector<int> modified_report = report;
            modified_report.erase(modified_report.begin() + i);
            if (is_safe(modified_report)) return true;
        }

        return false;
    };

    auto safe_reports = reports | views::filter(is_safe_with_problem_dampener);

    return to_string(ranges::distance(safe_reports));
}

} // namespace day01
