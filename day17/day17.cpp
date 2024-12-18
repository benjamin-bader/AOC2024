#include "day17.h"

#include <algorithm>
#include <cmath>
#include <exception>
#include <fstream>
#include <iostream>
#include <memory>
#include <numeric>
#include <ranges>
#include <set>
#include <sstream>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

using namespace std;

namespace day17
{

namespace
{

constexpr const char* kInputFile = "day17/day17.input";

[[maybe_unused]]
constexpr const char* kTestInput = R"(
Register A: 729
Register B: 0
Register C: 0

Program: 0,1,5,4,3,0)";

[[maybe_unused]]
constexpr const char* kTestInput2 = R"(
Register A: 2024
Register B: 0
Register C: 0

Program: 0,3,5,4,3,0)";

enum Opcode
{
    kOpcodeAdv = 0,
    kOpcodeBxl = 1,
    kOpcodeBst = 2,
    kOpcodeJnz = 3,
    kOpcodeBxc = 4,
    kOpcodeOut = 5,
    kOpcodeBdv = 6,
    kOpcodeCdv = 7,
};

class Computer
{
    int64_t a_, b_, c_;
    vector<int> instructions_;
    ostream& console_;
    size_t ip_;
    bool needs_comma_;

    friend ostream& operator<<(ostream& os, const Computer& computer);

public:
    Computer(int64_t a, int64_t b, int64_t c, vector<int> instructions, ostream& console)
        : a_(a)
        , b_(b)
        , c_(c)
        , instructions_(instructions)
        , console_(console)
        , ip_(0)
        , needs_comma_(false)
    {
    }

    void reset()
    {
        a_ = 0;
        b_ = 0;
        c_ = 0;
        ip_ = 0;
        needs_comma_ = false;
    }

    int64_t a() const
    {
        return a_;
    }

    int64_t b() const
    {
        return b_;
    }

    int64_t c() const
    {
        return c_;
    }

    size_t ip() const
    {
        return ip_;
    }

    const vector<int>& instructions() const
    {
        return instructions_;
    }

    void set_a(int64_t a)
    {
        this->a_ = a;
    }

    void set_b(int64_t b)
    {
        this->b_ = b;
    }

    void set_c(int64_t c)
    {
        this->c_ = c;
    }

    void run()
    {
        //size_t ip = 0;

        while (ip_ < instructions_.size())
        {
            dbg(LogLevel::DEBUG) << "IP: " << ip_ << ", op=" << instructions_[ip_] << endl;
            switch (instructions_[ip_])
            {
            case kOpcodeAdv: adv(ip_); break;
            case kOpcodeBxl: bxl(ip_); break;
            case kOpcodeBst: bst(ip_); break;
            case kOpcodeJnz: jnz(ip_); break;
            case kOpcodeBxc: bxc(ip_); break;
            case kOpcodeOut: out(ip_); break;
            case kOpcodeBdv: bdv(ip_); break;
            case kOpcodeCdv: cdv(ip_); break;
            default:
                throw runtime_error("Invalid opcode");
            }
        }
    }

    /*
    2,4
    1,6
    7,5
    4,4
    1,7
    0,3
    5,5
    3,0

    bst 4     ; bst %a
    bxl 6     ; bxl 6
    cdv 5     ; cdv %b
    bxc 4     ; bxc _
    bxl 7     ; bxl 7
    adv 3     ; adv 3
    out 5     ; out %b
    jnz 0     ; jnz 0

    void loop()
    {
        // b = (a & 0x7) ^ 6
        // b = (((a & 0x7) ^ 6) ^ (a >> ((a & 0x7) ^ 6))) ^ 7
        do
        {
            b = a & 0b111;
            b = b ^ 6;  // b ^ 0b110
            c = a / pow(2, b);
            b = b ^ c;
            b = b ^ 7;
            a = a / 8;
            cout << b;

        } while (a != 0);
    }

    */

private:
    void adv(size_t& ip)
    {
        ip++;
        a_ = a_ >> get_combo(ip++);
    }

    void bxl(size_t& ip)
    {
        ip++;
        b_ ^= get_literal(ip++);
    }

    void bst(size_t& ip)
    {
        ip++;
        auto operand = get_combo(ip++);
        b_ = (operand & 0x7); // keep the lowest three bits
    }

    void jnz(size_t& ip)
    {
        if (a_ == 0)
        {
            ip += 2;
        }
        else
        {
            ip = get_literal(ip + 1);
        }
    }

    void bxc(size_t& ip)
    {
        ip += 2; // ignores operand
        b_ ^= c_;
    }

    void out(size_t& ip)
    {
        ip++;
        if (needs_comma_)
        {
            console_ << ',';
        }
        needs_comma_ = true;

        console_ << (get_combo(ip++) & 0x7); // combo operand, modulo 8
    }

    void bdv(size_t& ip)
    {
        ip++;
        b_ = a_ >> get_combo(ip++);
    }

    void cdv(size_t& ip)
    {
        ip++;
        c_ = a_ >> get_combo(ip++);
    }

    int64_t get_literal(size_t operand_index)
    {
        return static_cast<int64_t>(instructions_[operand_index]);
    }

    int64_t get_combo(size_t operand_index)
    {
        switch (instructions_[operand_index] % 8)
        {
        case 0: return 0;
        case 1: return 1;
        case 2: return 2;
        case 3: return 3;
        case 4: return a_;
        case 5: return b_;
        case 6: return c_;
        case 7: // fallthrough
        default: throw runtime_error("Invalid operand");
        }
    }
};

ostream& operator<<(ostream& os, const Computer& computer)
{
    os << "Register A: " << computer.a() << endl;
    os << "Register B: " << computer.b() << endl;
    os << "Register C: " << computer.c() << endl;
    os << "Instructions: ";
    for (size_t i = 0; i < computer.instructions().size(); i++)
    {
        if (i > 0)
        {
            os << ", ";
        }
        os << computer.instructions()[i];
    }
    os << endl;

    return os;
}

[[maybe_unused]]
unique_ptr<istream> get_input()
{
    if (g_test_input)
    {
        return make_unique<stringstream>(kTestInput2);
    }

    return make_unique<ifstream>(kInputFile);
}

Computer read_input(ostream& console)
{
    auto in = get_input();
    *in >> ws;

    int64_t a, b, c;
    vector<int> instructions;

    string line;
    if (!getline(*in, line))
    {
        throw runtime_error("Unexpected EOF");
    }

    size_t ix = line.find(": ");
    if (ix == string::npos)
    {
        throw runtime_error("Invalid input");
    }

    a = stoll(line.substr(ix + 2));

    if (!getline(*in, line))
    {
        throw runtime_error("Unexpected EOF");
    }

    ix = line.find(": ");
    if (ix == string::npos)
    {
        throw runtime_error("Invalid input");
    }

    b = stoll(line.substr(ix + 2));

    if (!getline(*in, line))
    {
        throw runtime_error("Unexpected EOF");
    }

    ix = line.find(": ");
    if (ix == string::npos)
    {
        throw runtime_error("Invalid input");
    }

    c = stoll(line.substr(ix + 2));

    if (!getline(*in, line) || !line.empty())
    {
        throw runtime_error("Invalid input");
    }

    if (!getline(*in, line))
    {
        throw runtime_error("Unexpected EOF - missing instructions");
    }

    ix = line.find(": ");
    line = line.substr(ix + 2);

    stringstream ss(line);
    string tok;
    while (getline(ss, tok, ','))
    {
        instructions.push_back(stoi(tok));
    }

    auto comp = Computer(a, b, c, instructions, console);
    dbg(LogLevel::DEBUG) << comp << endl;
    return comp;
}

} // namespace

string PartOne::solve()
{
    stringstream ss;
    Computer computer = read_input(ss);
    computer.run();
    return ss.str();
}

string PartTwo::solve()
{
    stringstream ss;
    Computer computer = read_input(ss);

    auto expected = computer.instructions();
    set<int64_t> candidates{0};

    // from analyzing the input instruction, each loop's output depends only
    // on the value of the 'a' register - 'b' and 'c' are exclusively derived
    // from it.  further, the only modification to 'a' is an unconditional
    // right shift by 3 bits.
    //
    // We can inductively solve this, but this solution will only work for
    // my own test input.  I've got no Earthly idea how to solve this generally.
    //
    // The solution starts by finding values of a that produce the last output
    // number.  Then for each candidate, shift it left three bits, and for each
    // of the 8 possible values, check if it produces the next output number.
    // so on, so forth.
    for (auto it = expected.rbegin(); it != expected.rend(); ++it)
    {
        dbg() << "next=" << *it << "; candidates: ";
        for (const int64_t& candidate : candidates)
        {
            dbg() << candidate << ' ';
        }
        dbg() << endl;

        int64_t out = *it;
        vector<int64_t> tmp;
        for (const int64_t& candidate : candidates)
        {
            int64_t maybe = candidate << 3;
            for (auto i = 0; i < 8; ++i)
            {
                auto a = maybe + i;

                // This is the part that's specific to my input.
                // Algebra FTW!
                auto b = (a & 0x7) ^ 6;
                b = ((b ^ (a >> b)) ^ 7) & 0x7;

                if (b == out)
                {
                    tmp.push_back(a);
                }
            }
        }

        candidates.clear();
        candidates.insert(tmp.begin(), tmp.end());
    }

    if (candidates.empty())
    {
        return "no answer";
    }

    auto min_candidate = *candidates.begin();

    ss.str("");
    ss.clear();
    computer.reset();
    computer.set_a(min_candidate);
    computer.run();

    dbg() << "min_candidate=" << min_candidate << "; output=" << ss.str() << endl;

    return to_string(*candidates.begin());
}

} // namespace day17
