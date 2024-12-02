#include "base/base.h"
#include "day01/day01.h"

#include <functional>
#include <iostream>
#include <map>
#include <string>
#include <utility>

using namespace std;

map<pair<int, int>, Problem*> solutions;
map<pair<int, int>, string> expected_answers;

template <typename P>
void register_solution(int day, int part, P* solution)
{
    static_assert(std::derived_from<P, Problem>, "This is no problem");

    solutions[{day, part}] = solution;
    if constexpr (HasSolution<P>)
    {
        expected_answers[{day, part}] = P::expected;
    }
}

int main(int argc, char** argv)
{
    register_solution(1, 1, new day01::PartOne());
    register_solution(1, 2, new day01::PartTwo());

    if (argc == 3)
    {
        int day = std::stoi(argv[1]);
        int part = std::stoi(argv[2]);
        std::cout << solutions[{day, part}]->solve() << std::endl;
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
