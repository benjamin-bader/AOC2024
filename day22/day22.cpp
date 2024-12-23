#include "day22.h"

#include "parsers.h"

#include <algorithm>
#include <exception>
#include <fstream>
#include <generator>
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
    Buffer()
        : packed_{0}
    {}

    int operator[](size_t i) const
    {
        return this->at(i);
    }

    int at(size_t i) const
    {
        assert(i < 4);
        int shift = 8 * (3 - static_cast<int>(i));
        return static_cast<int8_t>((packed_ >> shift) & 0xFF);
        // return data_[(pos_ + i) & 0x3];
    }

    void push(int val)
    {
        packed_ = ((packed_ << 8) | (static_cast<uint32_t>(static_cast<int8_t>(val)) & 0xFF));
    }

    uint32_t pack() const
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
};

[[maybe_unused]]
ostream& operator<<(ostream& os, const Buffer& buffer)
{
    os << buffer.at(0) << ", " << buffer.at(1) << ", " << buffer.at(2) << ", " << buffer.at(3);
    os << " (packed: " << std::hex << buffer.pack() << std::dec << ")";
    return os;
}

bool operator==(const Buffer& lhs, const Buffer& rhs)
{
    return lhs.pack() == rhs.pack();
}

struct BufferHash
{
    size_t operator()(const Buffer& buf) const
    {
        return static_cast<size_t>(buf.pack());
    }
};

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
            s = (s ^ (s << 6)) & 16777215;
            s = (s ^ (s >> 5)) & 16777215;
            s = (s ^ (s << 11)) & 16777215;
        }
        return s;
    }

    generator<int> sequence(size_t num_iters) const
    {
        size_t s = secret_;

        for (size_t i = 0; i < num_iters; ++i)
        {
            s = (s ^ (s << 6)) & 16777215;
            s = (s ^ (s >> 5)) & 16777215;
            s = (s ^ (s << 11)) & 16777215;

            co_yield static_cast<int>(s % 10);
        }
    }

    void find_best_price(size_t max_iters, unordered_map<Buffer, long long, BufferHash>& prices_by_prefix) const
    {
        unordered_set<Buffer, BufferHash> seen_prefixes;

        Buffer buf;
        size_t i = 0;
        int price = initial_price();
        for (int new_price : sequence(max_iters))
        {
            int delta = new_price - price;
            buf.push(delta);
            price = new_price;

            if (i++ < 4)
            {
                // Dont' have enough deltas to make a prefix yet
                continue;
            }

            if (auto [_, inserted] = seen_prefixes.insert(buf); inserted)
            {
                // only the first time we see this prefix counts
                prices_by_prefix[buf] += new_price;
            }
        }
    }

private:
    int initial_price() const
    {
        return static_cast<int>(secret_ % 10);
    }
};

ostream& operator<<(ostream& os, const Buyer& buyer)
{
    os << buyer.iter(0);
    return os;
}

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

    for (size_t line : parsers::Lines(*input, [](const auto& str) { return stoull(str); } ))
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
        buyers.begin(),
        buyers.end(),
        0ull,
        plus{},
        [](const Buyer& buyer) {
            auto sz = buyer.iter(2000);
            dbg() << "buyer=" << buyer << "; sz=" << sz << endl;
            return sz;
        }
    );
    return to_string(sum);
}

string PartTwo::solve()
{
    size_t num_iters = 2000;
    auto buyers = read_input();

    unordered_map<Buffer, long long, BufferHash> sums_by_prefix;
    sums_by_prefix.reserve(buyers.size() * num_iters);

    for (const auto& buyer : buyers)
    {
        buyer.find_best_price(num_iters, sums_by_prefix);
    }

    long long best_price = 0;
    for (auto& [prefix, price] : sums_by_prefix)
    {
        best_price = max(price, best_price);
    }

    dbg() << "best price: " << best_price << endl;

    return to_string(best_price);
}

} // namespace day22
