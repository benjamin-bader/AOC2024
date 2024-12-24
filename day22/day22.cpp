#include "day22.h"

#include "parsers.h"

#include <algorithm>
#include <exception>
#include <execution>
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

namespace day22
{

namespace
{

constexpr const char* kInputFile = "day22/day22.input";
constexpr const char* kTestInput = R"(
1
2
3
2024)";

// 14,273,043,166

class Buffer
{
    uint32_t packed_;

public:
    constexpr Buffer()
        : packed_{0}
    {}

    constexpr Buffer(int a, int b, int c, int d)
        : packed_{0}
    {
        push(a);
        push(b);
        push(c);
        push(d);
    }

    int operator[](size_t i) const
    {
        return this->at(i);
    }

    constexpr int at(size_t i) const
    {
        assert(i < 4);
        int shift = 8 * (3 - static_cast<int>(i));
        return static_cast<int8_t>((packed_ >> shift) & 0xFF);
        // return data_[(pos_ + i) & 0x3];
    }

    constexpr void push(int val)
    {
        packed_ = ((packed_ << 8) | (static_cast<uint32_t>(static_cast<int8_t>(val)) & 0xFF));
    }

    constexpr uint32_t pack() const
    {
        return packed_;
    }

    const array<int8_t, 4> unpack() const
    {
        return {
            static_cast<int8_t>((packed_ >> 24) & 0xFF),
            static_cast<int8_t>((packed_ >> 16) & 0xFF),
            static_cast<int8_t>((packed_ >> 8) & 0xFF),
            static_cast<int8_t>(packed_ & 0xFF)
        };
    }

    constexpr size_t to_index() const
    {
        return static_cast<size_t>(
                ((at(0) + 9) << 15) +
                ((at(1) + 9) << 10) +
                ((at(2) + 9) << 5) +
                 (at(3) + 9)
            );
    }
};

constexpr const Buffer kMaxBuffer(9, 9, 9, 9);
constexpr const size_t kMaxIndex = kMaxBuffer.to_index() + 1;

using SeenArray = std::array<bool, kMaxIndex>;
using PriceArray = std::array<int, kMaxIndex>;

class Buyer
{
    size_t secret_;

public:
    Buyer(size_t secret)
        : secret_(secret)
    {
    }

    size_t iter(size_t num_iters) const
    {
        size_t s = secret_;
        for (size_t i = 0; i < num_iters; ++i)
        {
            mix(s);
        }
        return s;
    }

    [[maybe_unused]]
    void find_best_prefixes(size_t max_iters, SeenArray& seen, PriceArray& prices) const
    {
        Buffer buf;
        size_t s = secret_;
        int price = static_cast<int>(s % 10);

        fill(seen.begin(), seen.end(), false);

        for (size_t i = 0; i < max_iters; ++i)
        {
            mix(s);
            int new_price = static_cast<int>(s % 10);

            int delta = new_price - price;
            buf.push(delta);
            price = new_price;

            if (i < 3)
            {
                // Dont' have enough deltas to make a prefix yet
                continue;
            }

            if (!seen[buf.to_index()])
            {
                seen[buf.to_index()] = true;
                prices[buf.to_index()] += price;
            }
        }
    }

private:
    static constexpr void mix(size_t& s)
    {
        s = (s ^ (s <<  6)) & 0xFFFFFF;
        s = (s ^ (s >>  5)) & 0xFFFFFF;
        s = (s ^ (s << 11)) & 0xFFFFFF;
    }
};

/*

M = S << 6
M = M ^ S
S = M & 16777215

D = S >> 5
D = D ^ S
S = D & 16777215

T = S << 11
T = T ^ S
S = T & 16777215

*/

[[maybe_unused]]
unique_ptr<istream> get_input()
{
    if (g_test_input)
    {
        return make_unique<stringstream>(kTestInput);
    }

    return make_unique<ifstream>(kInputFile);
}

vector<Buyer> read_input()
{
    auto input = get_input();
    vector<Buyer> buyers;

    for (size_t line : parsers::Lines(*input, stoz))
    {
        buyers.emplace_back(line);
    }

    return buyers;
}

} // namespace

string PartOne::solve()
{
    auto buyers = read_input();
    auto sum = transform_reduce(
        //execution::par_unseq,
        buyers.begin(),
        buyers.end(),
        0_um,
        plus{},
        [](const Buyer& buyer) {
            return static_cast<uintmax_t>(buyer.iter(2000));
        }
    );
    return to_string(sum);
}

string PartTwo::solve()
{
    size_t num_iters = 2000;
    auto buyers = read_input();

    unique_ptr<SeenArray> seen_prefixes = make_unique<SeenArray>();
    unique_ptr<PriceArray> prices_by_prefix = make_unique<PriceArray>();

    fill(prices_by_prefix->begin(), prices_by_prefix->end(), 0);

    for_each(
        execution::seq,
        buyers.begin(), buyers.end(),
        [&seen_prefixes, &prices_by_prefix, num_iters](const Buyer& buyer) {
            buyer.find_best_prefixes(num_iters, *seen_prefixes, *prices_by_prefix);
        }
    );

    auto best_price = *max_element(prices_by_prefix->begin(), prices_by_prefix->end());

    dbg() << "best price: " << best_price << endl;

    return to_string(best_price);
}

} // namespace day22
