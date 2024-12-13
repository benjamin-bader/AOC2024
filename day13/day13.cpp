#include "day13.h"

#include <algorithm>
#include <exception>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <memory>
#include <numeric>
#include <ranges>
#include <regex>
#include <sstream>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

using namespace std;

namespace day13
{

namespace
{

/*

94M + 22N = 8400
34M + 67N = 5400

*/

constexpr const char* kInputFile = "day13/day13.input";
constexpr const char* kTestInput = R"(
Button A: X+94, Y+34
Button B: X+22, Y+67
Prize: X=8400, Y=5400

Button A: X+26, Y+66
Button B: X+67, Y+21
Prize: X=12748, Y=12176

Button A: X+17, Y+86
Button B: X+84, Y+37
Prize: X=7870, Y=6450

Button A: X+69, Y+23
Button B: X+27, Y+71
Prize: X=18641, Y=10279)";

constexpr const char* kShortTestInput = R"(
Button A: X+94, Y+34
Button B: X+22, Y+67
Prize: X=8400, Y=5400)";

using Point = pair<long long, long long>;

struct System
{
    Point button_a;
    Point button_b;
    Point prize;
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

vector<System> read_input()
{
    string line;
    vector<System> systems;
    auto input = get_input();

    regex expr_a{R"(Button A: X\+(\d+), Y\+(\d+))"};
    regex expr_b{R"(Button B: X\+(\d+), Y\+(\d+))"};
    regex expr_prize{R"(Prize: X=(\d+), Y=(\d+))"};

    while (true)
    {
        System system;

        smatch sm;

        // chew up whitespace
        while (true)
        {
            int ch = input->peek();
            if (ch == '\r' || ch == '\n' || ch == ' ' || ch == '\t')
            {
                input->ignore(1, ch);
            }
            else
            {
                break;
            }
        }

        // First button
        if (!getline(*input, line))
        {
            throw invalid_argument{"Unexpected end of input"};;
        }

        if (!regex_match(line, sm, expr_a))
        {
            throw invalid_argument{"Unexpected line-a input"};;
        }
        system.button_a = {stoi(sm[1]), stoi(sm[2])};

        // Second button
        if (!getline(*input, line))
        {
            throw invalid_argument{"Unexpected end of input"};;
        }

        if (!regex_match(line, sm, expr_b))
        {
            throw invalid_argument{"Unexpected line-b input"};;
        }
        system.button_b = {stoi(sm[1]), stoi(sm[2])};

        // Prize
        if (!getline(*input, line))
        {
            throw invalid_argument{"Unexpected end of input"};;
        }

        if (!regex_match(line, sm, expr_prize))
        {
            throw invalid_argument{"Unexpected line-prize input"};;
        }
        system.prize = {stoi(sm[1]), stoi(sm[2])};

        systems.push_back(system);

        if (!getline(*input, line))
        {
            break;
        }
    }

    return systems;
}

size_t count_min_tokens(long long xy_offset = 0)
{
    auto systems = read_input();
    if (xy_offset > 0)
    {
        for (auto& system : systems)
        {
            system.prize = {system.prize.first + xy_offset, system.prize.second + xy_offset};
        }
    }

    size_t sum = 0;
    for (const auto& system : systems) {
        const Point& a = system.button_a;
        const Point& b = system.button_b;
        const Point& p = system.prize;

        // we've got a system of equations here:
        // M*button_a.x + N*button_b.x = prize.x
        // M*button_a.y + N*button_b.y = prize.y

        // Cramer's rule for 2x2 systems
        //
        // given a matrix rep
        // [ MXa  MYa ] [ Pa ]
        // [ NXb  NYb ] [ Pb ]
        //
        // then the determinant is
        // ( MXa - NYb ) * ( NXb - MYa )
        //
        // and the solutions are
        // x = Dx / D, y = Dy / D
        //
        // where Dx is
        // [ Pa  MYa ]
        // [ Pb  NYb ]
        //
        // and Dy is
        // [ MXa  Pa ]
        // [ NXb  Pb ]
        auto det = (a.first * b.second) - (a.second * b.first);
        if (det == 0)
        {
            dbg() << "whoopsie daisy" << endl;
            throw invalid_argument{"Determinant is zero"};
        }

        auto det_m = (p.first * b.second) - (p.second * b.first);
        auto det_n = (a.first * p.second) - (a.second * p.first);

        // integer truncation could happen, we'll check for it by multiplying
        // and validating that we get the expected prize coordinate value.
        auto m = det_m / det;
        auto n = det_n / det;

        bool valid_solution = (m * a.first + n * b.first == p.first) && (m * a.second + n * b.second == p.second);

        dbg() << "m: " << m << ", n: " << n << " valid: " << boolalpha << valid_solution << endl;

        if (valid_solution)
        {
            // Per the puzzle statement, pressing A costs three tokens and B costs one.
            size_t cost_a = 3;
            size_t cost_b = 1;
            sum += (m * cost_a) + (n * cost_b);
        }
    }
    return sum;
}

} // namespace

string PartOne::solve()
{
    return to_string(count_min_tokens());
}

string PartTwo::solve()
{
    return to_string(count_min_tokens(10000000000000ULL));
}

} // namespace day13
