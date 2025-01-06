#include "base/base.h"

#include "solutions.h"

#include <exception>
#include <regex>
#include <string>
#include <utility>

#include <fmt/color.h>
#include <fmt/format.h>

using namespace std;

regex verbose_arg("-(v+)");

int main(int argc, char** argv)
{
    register_solutions();

    while (argc > 1 && argv[1][0] == '-')
    {
        smatch m;
        string arg(argv[1]);
        if (regex_match(arg, m, verbose_arg))
        {
            // count the number of 'v's in the argument
            g_verbose += static_cast<int>(m[1].length());
        }
        else if (arg == "-t")
        {
            g_test_input = true;
        }
        else
        {
            fmt::println(stderr, "Unknown option: {}", argv[1]);
            return 1;
        }

        argc--;
        argv++;
    }

    if (argc == 3)
    {
        int day = stoi(argv[1]);
        int part = stoi(argv[2]);
        pair<int, int> key = {day, part};
        auto it = solutions.find(key);
        if (it == solutions.end())
        {
            fmt::println(stderr, "No problem found for day {} part {}", day, part);
            return 1;
        }

        try
        {
            fmt::println("{}", it->second->solve());
        }
        catch (const std::exception& e)
        {
            // use fmt to print to stderr
            fmt::println(stderr, "Caught exception: {}\n", e.what());
        }
    }
    else
    {
        int fails = 0;
        for (const auto& [key, problem] : solutions)
        {
            auto it = expected_answers.find(key);
            if (it == expected_answers.end() && !g_test_input)
            {
                continue;
            }

            auto expected = g_test_input ? "" : it->second;

            bool did_throw = false;
            string actual;
            try
            {
                actual = problem->solve();
            }
            catch (const std::exception& e)
            {
                actual = e.what();
                did_throw = true;
            }

            bool passed = !did_throw && actual == expected;

            string message = passed ? "[PASS]" : "[FAIL]";
            string clear = "\033[0m";
            string expected_message = " (expected " + expected + ")";

            if (g_test_input)
            {
                message = "[TEST]";
                expected_message = "";
            }

            if (actual != expected)
            {
                fails++;
            }

            auto color = g_test_input ? fmt::color::yellow : passed ? fmt::color::green : fmt::color::red;

            fmt::println(
                "{}: Day {} Part {}: {} {}",
                fmt::styled(message, fmt::fg(color)),
                key.first,
                key.second,
                actual,
                expected_message
            );
        }

        return fails;
    }

    return 0;
}
