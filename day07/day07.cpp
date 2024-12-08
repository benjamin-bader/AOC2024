#include "day07.h"

#include "parsers.h"

#include <algorithm>
#include <execution>
#include <fstream>
#include <iostream>
#include <numeric>
#include <ranges>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

using namespace std;

namespace day07
{

namespace
{

enum class OpType
{
    Add = 0,
    Mul = 1,
    Concat = 2
};

uintmax_t eval_op(uintmax_t lhs, uintmax_t rhs, OpType op)
{
    switch (op)
    {
    case OpType::Add:
        return lhs + rhs;
    case OpType::Mul:
        return lhs * rhs;
    case OpType::Concat:
    {
        uintmax_t tmp = rhs;
        while (tmp > 0)
        {
            lhs *= 10;
            tmp /= 10;
        }
        return lhs + rhs;
    }
    default:
        throw "Invalid op";
    }
}

struct Calibration
{
    uintmax_t expected;
    vector<uintmax_t> values;

    Calibration(uintmax_t expected, vector<uintmax_t> values)
        : expected(expected), values(move(values))
    {
    }

    bool find_valid_ops(int num_ops) const
    {
        vector<OpType> possible;
        possible.resize(values.size() - 1, OpType::Add);

        // early exit: any of the numbers are bigger than the expected result
        if (ranges::any_of(values, [this](uintmax_t value) { return value > expected; }))
        {
            return false;
        }

        // constraint: the last op can be concat iff the expected result, as a string,
        //             ends with the last number.
        bool last_op_concat = to_string(expected).ends_with(to_string(values.back()));

        do
        {
            uintmax_t result = values[0];
            for (size_t i = 1; i < values.size(); ++i)
            {
                result = eval_op(result, values[i], possible[i - 1]);
                if (result > expected)
                {
                    break;
                }
            }

            if (result == expected)
            {
                return true;
            }
        }
        while (permute(possible, num_ops, last_op_concat));

        return false;
    }

    bool permute(vector<OpType>& possible, int ceiling, bool last_op_concat) const
    {
        for (size_t i = 0; i < possible.size(); ++i)
        {
            possible[i] = static_cast<OpType>((static_cast<int>(possible[i]) + 1) % ceiling);
            if (i == possible.size() - 1 && !last_op_concat && possible.back() == OpType::Concat)
            {
                possible[i] = OpType::Add;
            }
            else if (possible[i] != OpType::Add)
            {
                return true;
            }
        }

        return ranges::any_of(possible, [](OpType op) { return op != OpType::Add; });
    }
};

constexpr const char* kInputFile = "day07/day07.input";

constexpr const char* kTestInput = R"(190: 10 19
3267: 81 40 27
83: 17 5
156: 15 6
7290: 6 8 6 15
161011: 16 10 13
192: 17 8 14
21037: 9 7 18 13
292: 11 6 16 20)";

Calibration parse_calibration(const string& line)
{
    size_t pos = line.find(":");
    uintmax_t expected = stoull(line.substr(0, pos));

    vector<uintmax_t> values;
    string value_string = line.substr(pos + 2);
    stringstream ss{value_string};

    uintmax_t value;
    while (ss >> value)
    {
        values.push_back(value);
    }

    return Calibration(expected, move(values));
}

vector<Calibration> read_input()
{
    //return parsers::Lines(kTestInput, parse_calibration);
    ifstream input(kInputFile);
    return parsers::Lines(input, parse_calibration);
}

} // namespace

string PartOne::solve()
{
    auto valid_calibrations = read_input()
        | views::filter([](const Calibration& c) { return c.find_valid_ops(2); })
        | views::transform(&Calibration::expected);

    uintmax_t sum = 0;
    for (auto calibration : valid_calibrations)
    {
        sum += calibration;
        if (sum < calibration)
        {
            throw "Overflow";
        }
    }

    return to_string(ranges::fold_left(valid_calibrations, 0ULL, plus<>{}));
}

string PartTwo::solve()
{
    auto all_calibrations = read_input();

    uintmax_t sum = transform_reduce(
        std::execution::par_unseq,
        all_calibrations.cbegin(), all_calibrations.cend(),
        0ULL,
        std::plus<>(),
        [](const Calibration& c) {
            return c.find_valid_ops(3) ? c.expected : 0ULL;
        }
    );

    return to_string(sum);
}

} // namespace day07
