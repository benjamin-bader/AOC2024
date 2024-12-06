#include "day05.h"

#include <algorithm>
#include <deque>
#include <fstream>
#include <iostream>
#include <map>
#include <ranges>
#include <set>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

using namespace std;

namespace day05
{

namespace
{

constexpr const char* kInputFile = "day05/day05.input";

const string kTestInput = R"(47|53
97|13
97|61
97|47
75|29
61|13
75|53
29|13
97|29
53|29
61|53
97|53
61|29
47|13
75|47
97|75
47|61
75|61
47|29
75|13
53|13

75,47,61,53,29
97,61,53,29,13
75,29,13
75,97,47,61,53
61,13,29
97,13,75,29,47
)";

using Page = int;
using Update = vector<Page>;

class Rules
{
    // for  each page, the set of pages that the page must appear before
    // in a valid update.
    map<Page, set<Page>> requirements_by_page;

public:
    Rules() = default;
    Rules(const Rules&) = default;
    Rules(Rules&&) = default;

    Rules& operator=(const Rules&) = default;
    Rules& operator=(Rules&&) = default;

    void add_rule(Page must_come_before, Page must_come_after)
    {
        requirements_by_page[must_come_before].insert(must_come_after);
    }

    bool is_ordered(Page before, Page after)
    {
        const auto& requirements = requirements_by_page[after];
        return requirements.find(before) == requirements.end();
    }

    bool is_update_valid(const Update& update)
    {
        for (size_t i = 0; i < update.size() - 1; ++i)
        {
            for (size_t j = i + 1; j < update.size(); ++j)
            {
                if (!is_ordered(update[i], update[j]))
                {
                    return false;
                }
            }
        }

        return true;
    }
};

pair<Rules, vector<Update>> read_input()
{
    Rules rules;
    vector<Update> updates;

    ifstream input(kInputFile);
    // istringstream input(string{kTestInput});

    string line;
    while (getline(input, line))
    {
        if (line.empty())
        {
            break;
        }

        size_t pos = line.find("|");
        Page before = stoi(line.substr(0, pos));
        Page after = stoi(line.substr(pos + 1));
        rules.add_rule(before, after);
    }

    while (getline(input, line))
    {
        if (line.empty())
        {
            break;
        }

        Update update;
        string page_string;
        stringstream ss(line);
        while (getline(ss, page_string, ','))
        {
            update.push_back(stoi(page_string));
        }

        updates.push_back(move(update));
    }

    return {move(rules), move(updates)};
}

} // namespace

string PartOne::solve()
{
    auto [rules, updates] = read_input();

    auto get_middle_page = [](const Update& update) {
        if (update.size() % 2 == 0) throw "uh oh";
        return update[update.size() / 2];
    };

    int sum = 0;
    for (const auto& update : updates)
    {
        if (rules.is_update_valid(update))
        {
            sum += get_middle_page(update);
        }

    }

    return to_string(sum);
}

string PartTwo::solve()
{
    auto [rules, updates] = read_input();

    auto is_incorrect_update = [&](const Update& update) {
        return !rules.is_update_valid(update);
    };

    auto sort_update = [&](Update& update) {
        sort(update.begin(), update.end(), [&](Page lhs, Page rhs) { return rules.is_ordered(lhs, rhs); });
        return update;
    };

    auto get_middle_page = [](const Update& update) {
        if (update.size() % 2 == 0) throw "uh oh";
        return update[update.size() / 2];
    };

    auto fixed_incorrect_updates = updates
        | views::filter(is_incorrect_update)
        | views::transform(sort_update)
        | views::transform(get_middle_page);

    return to_string(ranges::fold_left(fixed_incorrect_updates, 0, plus<>{}));
}

} // namespace day05
