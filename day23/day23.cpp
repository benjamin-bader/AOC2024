#include "day23.h"

#include "parsers.h"

#include <algorithm>
#include <exception>
#include <fstream>
#include <iostream>
#include <memory>
#include <numeric>
#include <ranges>
#include <set>
#include <sstream>
#include <stack>
#include <string>
#include <string_view>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

using namespace std;

namespace day23
{

namespace
{

constexpr const char* kInputFile = "day23/day23.input";
constexpr const char* kTestInput = R"(
kh-tc
qp-kh
de-cg
ka-co
yn-aq
qp-ub
cg-tb
vc-aq
tb-ka
wh-tc
yn-cg
kh-ub
ta-co
de-co
tc-td
tb-wq
wh-td
ta-ka
td-qp
aq-cg
wq-ub
ub-vc
de-ta
wq-aq
wq-vc
wh-yn
ka-de
kh-ta
co-tc
wh-qp
tb-vc
td-yn)";

[[maybe_unused]]
unique_ptr<istream> get_input()
{
    if (g_test_input)
    {
        return make_unique<stringstream>(kTestInput);
    }

    return make_unique<ifstream>(kInputFile);
}

vector<pair<string, string>> read_input()
{
    auto in = get_input();
    vector<pair<string, string>> connections;

    for (string line : parsers::Lines(*in))
    {
        size_t ix = line.find('-');
        if (ix == string::npos)
        {
            throw runtime_error("Invalid input");
        }

        string x = line.substr(0, ix);
        string y = line.substr(ix + 1);
        connections.emplace_back(x, y);
    }

    return connections;
}

} // namespace

string PartOne::solve()
{
    auto connections = read_input();
    unordered_set<string> computers;
    for (const auto& [a, b] : connections)
    {
        computers.insert(a);
        computers.insert(b);
    }

    unordered_map<string, unordered_set<string>> connections_by_node;
    for (const auto& [a, b] : connections)
    {
        connections_by_node[a].insert(b);
        connections_by_node[b].insert(a);
    }

    vector<string> nodes(computers.begin(), computers.end());

    size_t num_t_groups = 0;
    for (size_t x = 0; x < nodes.size() - 2; x++)
    {
        for (size_t y = x + 1; y < nodes.size() - 1; ++y)
        {
            for (size_t z = y + 1; z < nodes.size(); ++z)
            {
                if (nodes[x][0] == 't' || nodes[y][0] == 't' || nodes[z][0] == 't')
                {
                    const string& a = nodes[x];
                    const string& b = nodes[y];
                    const string& c = nodes[z];

                    if (auto it = connections_by_node[a].find(b); it == connections_by_node[a].end())
                    {
                        continue;
                    }

                    if (auto it = connections_by_node[a].find(c); it == connections_by_node[a].end())
                    {
                        continue;
                    }

                    if (auto it = connections_by_node[b].find(c); it == connections_by_node[b].end())
                    {
                        continue;
                    }

                    ++num_t_groups;
                }
            }
        }
    }

    return to_string(num_t_groups);
}

using StringSet = unordered_set<string_view>;

void find_cliques(
    StringSet&& cur,
    StringSet&& candidates,
    StringSet&& excluded,
    unordered_map<string_view, StringSet>& connections_by_node,
    vector<StringSet>& cliques
) {
    if (candidates.empty() && excluded.empty())
    {
        cliques.push_back(cur);
        return;
    }

    while (!candidates.empty())
    {
        auto candidate = *candidates.begin();

        StringSet new_cur = cur;
        new_cur.insert(candidate);

        StringSet new_candidates;
        for (const auto c : candidates)
        {
            if (connections_by_node[candidate].find(c) != connections_by_node[candidate].end())
            {
                new_candidates.insert(c);
            }
        }

        StringSet new_excluded;
        for (const auto x : excluded)
        {
            if (connections_by_node[candidate].find(x) != connections_by_node[candidate].end())
            {
                new_excluded.insert(x);
            }
        }

        find_cliques(std::move(new_cur), std::move(new_candidates), std::move(new_excluded), connections_by_node, cliques);

        candidates.erase(candidate);
        excluded.insert(candidate);
    }
}

string PartTwo::solve()
{
    auto connections = read_input();
    unordered_set<string> computers;
    unordered_map<string_view, StringSet> cxs;

    for (const auto& [a, b] : connections)
    {
        auto it_a = computers.insert(a);
        auto it_b = computers.insert(b);

        cxs[*it_a.first].insert(*it_b.first);
        cxs[*it_b.first].insert(*it_a.first);
    }

    vector<StringSet> cliques;
    StringSet vertices(computers.begin(), computers.end());
    find_cliques({}, std::move(vertices), {}, cxs, cliques);

    auto it = max_element(cliques.begin(), cliques.end(), [](const auto& a, const auto& b) {
        return a.size() < b.size();
    });

    vector<string> max_clique(it->begin(), it->end());
    sort(max_clique.begin(), max_clique.end());

    stringstream out;
    for (size_t i = 0; i < max_clique.size(); ++i)
    {
        if (i > 0)
        {
            out << ',';
        }
        out << max_clique[i];
    }

    return out.str();
}

} // namespace day23
