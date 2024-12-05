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

ostream& operator<<(ostream& os, const Update& update)
{
    os << "[";
    bool needs_comma = false;
    for (Page p : update)
    {
        if (needs_comma)
        {
            os << ", ";
        }
        os << p;
        needs_comma = true;
    }
    os << "]";
    return os;
}

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
                    cout << "update " << update << " is invalid because " << update[i] << " does not come before " << update[j] << endl;
                    return false;
                }
            }
        }

        return true;
    }

    void print_requirements()
    {
        cout << "direct requirements:" << endl;
        for (const auto& [page, reqs] : requirements_by_page)
        {
            cout << page << ": [";
            bool needs_comma = false;
            for (Page p : reqs)
            {
                if (needs_comma)
                {
                    cout << ", ";
                }
                cout << p;
                needs_comma = true;
            }
            cout << "]" << endl;
        }
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

    rules.print_requirements();

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
    return "TODO";
}

} // namespace day05
