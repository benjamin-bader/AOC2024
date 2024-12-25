#include "day21.h"

#include "parsers.h"
#include "point.h"

#include <algorithm>
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

namespace day21
{

namespace
{

constexpr const char* kInputFile = "day21/day21.input";
constexpr const char* kTestInput = R"(
029A
980A
179A
456A
379A)";

const unordered_map<char, Point> keypad_points_by_char{
    { '7', Point(0, 0) },
    { '8', Point(1, 0) },
    { '9', Point(2, 0) },
    { '4', Point(0, 1) },
    { '5', Point(1, 1) },
    { '6', Point(2, 1) },
    { '1', Point(0, 2) },
    { '2', Point(1, 2) },
    { '3', Point(2, 2) },
    { ' ', Point(0, 3) },
    { '0', Point(1, 3) },
    { 'A', Point(2, 3) },
};

class Robot
{
    unordered_map<char, unordered_map<char, size_t>> costs_;
    Robot* derived_;

public:
    Robot() = default;
    Robot(initializer_list<pair<char, initializer_list<pair<char, size_t>>>> costs)
        : costs_{}
        , derived_{nullptr}
    {
        for (const auto& row : costs)
        {
            unordered_map<char, size_t> m;
            for (const auto& [c,cost] : row.second)
            {
                m[c] = cost;
            }
            costs_[row.first] = std::move(m);
        }
    }

    ~Robot()
    {
        if (derived_ != nullptr)
        {
            delete derived_;
        }
    }

    size_t cost_to_move(char from, char to) const
    {
        return costs_.at(from).at(to);
    }

    Robot* derive()
    {
        if (derived_ == nullptr)
        {
            derived_ = new Robot{{
                { 'A', {
                    { 'A', cost_of("") },
                    { '^', cost_of("<") },
                    { '>', cost_of("v") },
                    { 'v', min(cost_of("<v"), cost_of("v<")) },
                    { '<', min(cost_of("v<<"), cost_of("<v<")) }, // no <<v, blank space
                } },
                { '^', {
                    { 'A', cost_of(">") },
                    { '^', cost_of("") },
                    { '>', min(cost_of("v>"), cost_of(">v")) },
                    { 'v', cost_of("v") },
                    { '<', cost_of("v<") }, // no <v, blank space
                } },
                { '>', {
                    { 'A', cost_of("^") },
                    { '^', min(cost_of("^<"), cost_of("<^")) },
                    { '>', cost_of("") },
                    { 'v', cost_of("<") },
                    { '<', cost_of("<<") },
                } },
                { 'v', {
                    { 'A', min(cost_of(">^"), cost_of("^>")) },
                    { '^', cost_of("^") },
                    { '>', cost_of(">") },
                    { 'v', cost_of("") },
                    { '<', cost_of("<") },
                } },
                { '<', {
                    { 'A', min(cost_of(">>^"), cost_of(">^>")) }, // no ^>>, blank space
                    { '^', cost_of(">^") }, // no ^>
                    { '>', cost_of(">>") },
                    { 'v', cost_of(">") },
                    { '<', cost_of("") },
                } },
            }};
        }

        return derived_;
    }

private:
    size_t cost_of(string_view str) const
    {
        char cur = 'A';
        size_t cost = 0;
        for (char c : str)
        {
            cost += cost_to_move(cur, c) + 1;
            cur = c;
        }
        cost += cost_to_move(cur, 'A'); // gotta move back to A
        return cost;
    }
};

class Puzzle
{
    vector<string> codes_;
    Robot* base_robot_;

public:
    Puzzle(vector<string>&& codes)
        : codes_(std::move(codes))
        , base_robot_(nullptr)
    {
        base_robot_ = new Robot{{
            { 'A', { { 'A', 0 }, {'^', 0}, {'>', 0}, {'v', 0}, {'<', 0} } },
            { '^', { { 'A', 0 }, {'^', 0}, {'>', 0}, {'v', 0}, {'<', 0} } },
            { '>', { { 'A', 0 }, {'^', 0}, {'>', 0}, {'v', 0}, {'<', 0} } },
            { 'v', { { 'A', 0 }, {'^', 0}, {'>', 0}, {'v', 0}, {'<', 0} } },
            { '<', { { 'A', 0 }, {'^', 0}, {'>', 0}, {'v', 0}, {'<', 0} } },
        }};
    }

    ~Puzzle()
    {
        delete base_robot_;
    }

    const vector<string>& codes() const
    {
        return codes_;
    }

    size_t cost_of_code(const string& code, size_t num_dirpads) const
    {
        char cur = 'A';
        size_t cost = 0;
        for (char c : code)
        {
            cost += cost_to_move(cur, c, base_robot_, num_dirpads);
            cur = c;
        }
        return cost;
    }

private:
    size_t cost_to_move(char from, char to, Robot* robot, size_t num_remaining_robots) const
    {
        if (num_remaining_robots > 1)
        {
            // This part of the recursive call is a robot controlling a dpad
            Robot* derived = robot->derive();
            return cost_to_move(from, to, derived, num_remaining_robots - 1);
        }

        // This is the innermost dirpad - we must translate the keypad 'from'
        // and 'to' chars into dpad moves, and calculate the cost of those moves
        // using the derived robot.
        auto pFrom = keypad_points_by_char.at(from);
        auto pTo = keypad_points_by_char.at(to);

        auto dx = pTo.x() - pFrom.x();
        auto dy = pTo.y() - pFrom.y();

        auto dy_char = dy < 0 ? '^' : 'v';
        auto dx_char = dx < 0 ? '<' : '>';

        if (dx == 0 && dy == 0)
        {
            // already there!  just press the button.
            return 1;
        }
        else if (dx == 0)
        {
            // only vertical moves - known safe.
            return robot->cost_to_move('A', dy_char) + abs(dy) + robot->cost_to_move(dy_char, 'A') + 1;
        }
        else if (dy == 0)
        {
            // only horizontal moves - known safe.
            return robot->cost_to_move('A', dx_char) + abs(dx) + robot->cost_to_move(dx_char, 'A') + 1;
        }
        else if (pFrom.x() == 0 && pTo.y() == 3)
        {
            // we're in danger of crossing the blank space -
            // do horizontal moves first.
            return robot->cost_to_move('A', dx_char) + abs(dx)
                + robot->cost_to_move(dx_char, dy_char) + abs(dy)
                + robot->cost_to_move(dy_char, 'A') + 1;
        }
        else if (pFrom.y() == 3 && pTo.x() == 0)
        {
            // we're in danger of crossing the blank space -
            // do vertical moves first.
            return robot->cost_to_move('A', dy_char) + abs(dy)
                + robot->cost_to_move(dy_char, dx_char) + abs(dx)
                + robot->cost_to_move(dx_char, 'A') + 1;
        }
        else
        {
            // no danger of crossing the blank - could do horizontals
            // or verticals first; pick whichever is cheaper.
            return min(
                robot->cost_to_move('A', dx_char) + abs(dx)
                + robot->cost_to_move(dx_char, dy_char) + abs(dy)
                + robot->cost_to_move(dy_char, 'A') + 1,

                robot->cost_to_move('A', dy_char) + abs(dy)
                + robot->cost_to_move(dy_char, dx_char) + abs(dx)
                + robot->cost_to_move(dx_char, 'A') + 1
            );
        }
    }
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

unique_ptr<Puzzle> read_input()
{
    auto in = get_input();
    auto codes = parsers::Lines(*in);

    return make_unique<Puzzle>(std::move(codes));
}

} // namespace

string PartOne::solve()
{
    constexpr const size_t num_dirpads = 3;

    auto puzzle = read_input();
    size_t total_cost = 0;
    for (const auto& code : puzzle->codes())
    {
        string code_copy = code.substr(0, code.size() - 1); // remove final 'A'
        size_t code_number = stoz(code_copy);
        size_t code_cost = puzzle->cost_of_code(code, num_dirpads);
        dbg() << "code=" << code << "; code_number=" << code_number << "; code_cost=" << code_cost << endl;
        total_cost += code_cost * code_number;
    }
    return to_string(total_cost);
}

string PartTwo::solve()
{
    constexpr const size_t num_dirpads = 26;

    auto puzzle = read_input();
    size_t total_cost = 0;
    for (const auto& code : puzzle->codes())
    {
        string code_copy = code.substr(0, code.size() - 1); // remove final 'A'
        size_t code_number = stoz(code_copy);
        size_t code_cost = puzzle->cost_of_code(code, num_dirpads);
        dbg() << "code=" << code << "; code_number=" << code_number << "; code_cost=" << code_cost << endl;
        total_cost += code_cost * code_number;
    }
    return to_string(total_cost);
}

} // namespace day21
