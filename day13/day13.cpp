#include "day13.h"

#include "point.h"

#include <algorithm>
#include <cassert>
#include <exception>
#include <fstream>
#include <iomanip>
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

#include <boost/fusion/adapted.hpp>
#include <boost/phoenix/core.hpp>
#include <boost/phoenix/object.hpp>
#include <boost/phoenix/operator.hpp>
#include <boost/spirit/include/qi.hpp>

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

using Point64 = BasicPoint<int64_t>;

struct System
{
    Point64 button_a;
    Point64 button_b;
    Point64 prize;
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

namespace phx = boost::phoenix;
namespace qi = boost::spirit::qi;

template <typename Iter, typename Skipper = qi::blank_type>
class SystemGrammar : public qi::grammar<Iter, System(), Skipper>
{

public:
    SystemGrammar() : SystemGrammar::base_type(start)
    {
        using namespace boost::spirit;
        using qi::int_;
        using qi::lit;
        using qi::_val;

        button = (lit("Button ") >> qi::char_ >> lit(": X+") >> int_ >> lit(", Y+") >> int_)[_val = phx::construct<Point64>(qi::_2, qi::_3)];
        prize = (lit("Prize: X=") >> int_ >> lit(", Y=") >> int_)[_val = phx::construct<Point64>(qi::_1, qi::_2)];
        start = qi::omit[*qi::space] >> button >> lit("\n") >> button >> lit("\n") >> prize;
    }

private:
    qi::rule<Iter, System(), Skipper> start;
    qi::rule<Iter, Point64(), Skipper> button;
    qi::rule<Iter, Point64(), Skipper> prize;
};

vector<System> read_input()
{
    string line;
    vector<System> systems;
    auto input = get_input();

    *input >> ws;

    boost::spirit::istream_iterator begin{*input >> noskipws};
    boost::spirit::istream_iterator end;

    SystemGrammar<decltype(begin)> grammar{};
    System system;
    while (boost::spirit::qi::phrase_parse(begin, end, grammar, boost::spirit::qi::blank, system))
    {
        systems.push_back(system);
    }

    return systems;
}

uintmax_t count_min_tokens(int64_t xy_offset = 0)
{
    auto systems = read_input();
    if (xy_offset > 0)
    {
        Point64 off{xy_offset, xy_offset};
        for (auto& system : systems)
        {
            system.prize = system.prize + off;
        }
    }

    uintmax_t sum = 0;
    for (const auto& system : systems) {
        const Point64& a = system.button_a;
        const Point64& b = system.button_b;
        const Point64& p = system.prize;

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
        auto det = (a.x() * b.y()) - (a.y() * b.x());
        if (det == 0)
        {
            dbg("whoopsie daisy");
            throw invalid_argument{"Determinant is zero"};
        }

        auto det_m = (p.x() * b.y()) - (p.y() * b.x());
        auto det_n = (a.x() * p.y()) - (a.y() * p.x());

        // integer truncation could happen, we'll check for it by multiplying
        // and validating that we get the expected prize coordinate value.
        auto m = det_m / det;
        auto n = det_n / det;

        bool valid_solution =
            (m * a.x() + n * b.x() == p.x()) &&
            (m * a.y() + n * b.y() == p.y());

        dbg("m: {}, n: {}, valid: {}", m, n, valid_solution);

        if (valid_solution)
        {
            // Per the puzzle statement, pressing A costs three tokens and B costs one.
            uintmax_t cost_a = 3;
            uintmax_t cost_b = 1;

            assert(m >= 0);
            assert(n >= 0);
            sum += (static_cast<uintmax_t>(m) * cost_a) + (static_cast<uintmax_t>(n) * cost_b);
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

BOOST_FUSION_ADAPT_STRUCT(
    day13::System,
    (day13::Point64, button_a)
    (day13::Point64, button_b)
    (day13::Point64, prize)
);
