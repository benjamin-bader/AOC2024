#include "day11.h"

#include "hash.h"

#include <algorithm>
#include <cmath>
#include <fstream>
#include <iostream>
#include <memory>
#include <numeric>
#include <ranges>
#include <sstream>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

using namespace std;

namespace day11
{

namespace
{

constexpr const char* kInputFile = "day11/day11.input";
constexpr const char* kTestInput = "125 17";

using Stone = uintmax_t;

int count_digits(Stone num)
{
    int digits = 0;
    while (num > 0)
    {
        num /= 10;
        ++digits;
    }
    return digits;
}

struct Hash
{
    size_t operator()(const tuple<Stone, uintmax_t>& t) const
    {
        size_t seed = 1;
        hash_combine(seed, get<0>(t));
        hash_combine(seed, get<1>(t));
        return seed;
    }
};

using BlinkCache = unordered_map<tuple<Stone, uintmax_t>, uintmax_t, Hash>;

uintmax_t blink(Stone s, uintmax_t num_blinks_remaining, BlinkCache& cache)
{
    tuple<Stone, uintmax_t> key{s, num_blinks_remaining};
    if (auto it = cache.find(key); it != cache.end())
    {
        return it->second;
    }

    if (num_blinks_remaining == 0)
    {
        cache[key] = 1;
        return 1;
    }

    if (s == 0)
    {
        uintmax_t result = blink(1, num_blinks_remaining - 1, cache);
        return cache[key] = result;
    }

    int digits = count_digits(s);
    if (digits % 2 == 0)
    {
        Stone divisor = static_cast<Stone>(pow(10, digits / 2));
        Stone first = s / divisor;
        Stone second = s % divisor;
        return cache[key] =
            blink(first, num_blinks_remaining - 1, cache) +
            blink(second, num_blinks_remaining - 1, cache);
    }

    return cache[key] = blink(s * 2024, num_blinks_remaining - 1, cache);
}

[[maybe_unused]]
unique_ptr<istream> get_input()
{
    if (g_test_input)
    {
        return make_unique<stringstream>(kTestInput);
    }

    return make_unique<ifstream>(kInputFile);
}

vector<Stone> read_stones()
{
    auto input = get_input();
    vector<Stone> stones;
    Stone stone;
    while (*input >> stone)
    {
        stones.push_back(stone);
    }
    return stones;
}

} // namespace

string PartOne::solve()
{
    auto stones = read_stones();

    uintmax_t num_blinks = 25;
    BlinkCache cache;

    uintmax_t num_stones = transform_reduce(
        stones.begin(), stones.end(),
        static_cast<uintmax_t>(0),
        plus<>{},
        [&](Stone s) {
            return blink(s, num_blinks, cache);
        }
    );

    return to_string(num_stones);
}

string PartTwo::solve()
{
    auto stones = read_stones();

    uintmax_t num_blinks = 75;
    BlinkCache cache;

    uintmax_t num_stones = transform_reduce(
        stones.begin(), stones.end(),
        static_cast<uintmax_t>(0),
        plus<>{},
        [&](Stone s) {
            return blink(s, num_blinks, cache);
        }
    );

    return to_string(num_stones);
}

} // namespace day11
