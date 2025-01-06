#include "day25.h"

#include "hash.h"

#include <algorithm>
#include <exception>
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

namespace day25
{

namespace
{

constexpr const char* kInputFile = "day25/day25.input";
constexpr const char* kTestInput = R"(
#####
.####
.####
.####
.#.#.
.#...
.....

#####
##.##
.#.##
...##
...#.
...#.
.....

.....
#....
#....
#...#
#.#.#
#.###
#####

.....
.....
#.#..
###..
###.#
###.#
#####

.....
.....
.....
#....
#.#..
#.#.#
#####)";

class LockAndKey
{
    size_t line_;
    array<uint8_t, 5> levels_;
    size_t hash_;

public:
    LockAndKey(size_t line, uint8_t a, uint8_t b, uint8_t c, uint8_t d, uint8_t e)
        : line_(line)
        , levels_{a, b, c, d, e}
        , hash_{0}
    {
        // let's assume that the levels are in the range [0, 5] and
        // that the hash is unique for each set of levels - three
        // bits per level.
        hash_ = (static_cast<size_t>(a) << 12)
            |   (static_cast<size_t>(b) << 9)
            |   (static_cast<size_t>(c) << 6)
            |   (static_cast<size_t>(d) << 3)
            |    static_cast<size_t>(e);
    }

    size_t line() const
    {
        return line_;
    }

    bool can_fit(const LockAndKey& other) const
    {
        for (size_t i = 0; i < levels_.size(); ++i)
        {
            if (levels_[i] + other.levels_[i] > 5)
            {
                return false;
            }
        }

        return true;
    }

    bool operator==(const LockAndKey& other) const
    {
        return levels_ == other.levels_;
    }

    size_t hash() const
    {
        return hash_;
    }
};

struct LKHash
{
    size_t operator()(const pair<LockAndKey, LockAndKey>& lk) const
    {
        // abusing the known size of the hash value
        return (lk.first.hash() << 16) | lk.second.hash();
    }
};

class Puzzle
{
    vector<LockAndKey> locks_;
    vector<LockAndKey> keys_;

public:
    Puzzle(vector<LockAndKey>&& locks, vector<LockAndKey>&& keys)
        : locks_(std::move(locks))
        , keys_(std::move(keys))
    {}

    size_t count_fitting_pairs() const
    {
        dbg("locks: {} keys: {}", locks_.size(), keys_.size());
        unordered_set<pair<LockAndKey, LockAndKey>, LKHash> lock_counts;

        for (const auto& lock : locks_)
        {
            for (const auto& key : keys_)
            {
                if (lock.can_fit(key))
                {
                    dbg("lock {} fits key {}", lock.line(), key.line());
                    lock_counts.emplace(lock, key);
                }
            }
        }

        return lock_counts.size();
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

Puzzle read_puzzle()
{
    auto in = get_input();
    *in >> ws;

    vector<LockAndKey> locks;
    vector<LockAndKey> keys;

    string line;
    size_t line_number = 1;
    while (getline(*in, line))
    {
        auto start = line_number++;
        bool is_lock = line[0] == '#';
        uint8_t a{}, b{}, c{}, d{}, e{};
        while (getline(*in, line))
        {
            line_number++;
            if (line.empty())
            {
                break;
            }

            a += line[0] == '#';
            b += line[1] == '#';
            c += line[2] == '#';
            d += line[3] == '#';
            e += line[4] == '#';
        }

        if (is_lock)
        {
            locks.emplace_back(start, a, b, c, d, e);
        }
        else
        {
            a--; b--; c--; d--; e--; // bottom row of a key is all hashes
            keys.emplace_back(start, a, b, c, d, e);
        }
    }

    return {std::move(locks), std::move(keys)};
}

} // namespace

string PartOne::solve()
{
    return to_string(read_puzzle().count_fitting_pairs());
}

string PartTwo::solve()
{
    return "TODO";
}

} // namespace day25
