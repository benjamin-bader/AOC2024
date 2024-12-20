#include "day19.h"

#include <algorithm>
#include <array>
#include <exception>
#include <fstream>
#include <iostream>
#include <memory>
#include <numeric>
#include <ranges>
#include <sstream>
#include <string>
#include <string_view>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

using namespace std;

namespace day19
{

namespace
{

constexpr const char* kInputFile = "day19/day19.input";
constexpr const char* kTestInput = R"(
r, wr, b, g, bwu, rb, gb, br

brwrr
bggr
gbbr
rrbgbr
ubwu
bwurrg
brgr
bbrgwb)";

struct Puzzle
{
    vector<string> towels;
    vector<string> patterns;
};

[[maybe_unused]]
unique_ptr<istream> get_input()
{
    if (g_test_input)
    {
        return make_unique<stringstream>(kTestInput);
    }

    return make_unique<ifstream>(kInputFile);
}

Puzzle read_input()
{
    auto in = get_input();
    *in >> ws;

    vector<string> towels;
    string line;
    if (!getline(*in, line) || line.empty())
    {
        throw invalid_argument("Invalid input");
    }

    stringstream ss(line);
    string tok;
    while (ss >> tok)
    {
        if (tok.back() == ',')
        {
            tok.pop_back();
        }
        towels.push_back(tok);
    }

    if (!getline(*in, line) || !line.empty())
    {
        throw invalid_argument("Expected an empty line");
    }

    vector<string> patterns;
    while (getline(*in, line) && !line.empty())
    {
        patterns.push_back(line);
    }

    return Puzzle{towels, patterns};
}

bool can_form_pattern(const unordered_set<string>& towel_set, const string& pattern)
{
    size_t n = pattern.size();
    vector<bool> dp(n + 1, false);

    dp[0] = true;

    for (size_t i = 1; i <= n; ++i)
    {
        for (size_t j = 0; j < i; ++j)
        {
            if (dp[j] && towel_set.contains(pattern.substr(j, i - j)))
            {
                dp[i] = true;
                break;
            }
        }
    }

    return dp[n];
}

class Counter
{
    unordered_map<string_view, size_t> counts_;

public:
    size_t count_combos(const vector<string>& towels, string_view pattern)
    {
        if (pattern.empty())
        {
            return 1;
        }

        if (auto it = counts_.find(pattern); it != counts_.end())
        {
            return it->second;
        }

        size_t num_combos = 0;
        for (const auto& towel : towels)
        {
            if (pattern.starts_with(towel))
            {
                num_combos += count_combos(towels, pattern.substr(towel.size()));
            }
        }

        counts_[pattern] = num_combos;

        return num_combos;
    }
};

} // namespace

string PartOne::solve()
{
    Puzzle p = read_input();
    size_t num_possible = 0;
    unordered_set<string> towel_set(p.towels.begin(), p.towels.end());
    for (const auto& pattern : p.patterns)
    {
        if (can_form_pattern(towel_set, pattern))
        {
            ++num_possible;
        }
    }
    return to_string(num_possible);
}

string PartTwo::solve()
{
    Puzzle p = read_input();

    size_t count = 0;
    for (const auto& pattern : p.patterns)
    {
        Counter counter;
        count += counter.count_combos(p.towels, pattern);
    }

    return to_string(count);
}

} // namespace day19
