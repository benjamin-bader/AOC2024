#include "base/base.h"

#include "day01/day01.h"

#include <functional>
#include <iostream>
#include <map>
#include <memory>
#include <string>
#include <utility>

using namespace std;

map<pair<int, int>, shared_ptr<Problem>> solutions;
map<pair<int, int>, string> expected_answers;

template <typename P>
void register_solution(int day, int part)
{
    static_assert(derived_from<P, Problem>, "This is no problem");

    solutions[{day, part}] = make_shared<P>();
    if constexpr (HasSolution<P>)
    {
        expected_answers[{day, part}] = P::expected;
    }
}

int main(int argc, char** argv)
{
    register_solution<day01::PartOne>(1, 1);
    register_solution<day01::PartTwo>(1, 2);

    if (argc == 3)
    {
        int day = stoi(argv[1]);
        int part = stoi(argv[2]);
        pair<int, int> key = {day, part};
        auto it = solutions.find(key);
        if (it == solutions.end())
        {
            cerr << "No problem found for day " << day << " part " << part << endl;
            return 1;
        }
        cout << it->second->solve() << endl;
    }
    else
    {
        int fails = 0;
        for (const auto& [key, problem] : solutions)
        {
            auto it = expected_answers.find(key);
            if (it == expected_answers.end())
            {
                continue;
            }

            auto& expected = it->second;
            auto actual = problem->solve();

            string color = actual == expected ? "\033[32m" : "\033[31m";
            string message = actual == expected ? "[PASS]" : "[FAIL]";
            string clear = "\033[0m";

            if (actual != expected)
            {
                fails++;
            }

            cout
                << color << message << clear
                << ": Day " << key.first << " Part " << key.second << ": "
                << actual << " (expected " << expected << ")" << endl;
        }

        return fails;
    }

    return 0;
}
