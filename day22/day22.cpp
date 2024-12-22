#include "day22.h"

#include "parsers.h"

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

namespace day22
{

namespace
{

constexpr const char* kInputFile = "day22/day22.input";
constexpr const char* kTestInput = R"(
1
10
100
2024)";

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
    return "TODO";
}

} // namespace day22
