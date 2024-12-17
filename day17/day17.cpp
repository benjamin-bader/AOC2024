#include "day17.h"

#include <algorithm>
#include <cmath>
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

namespace day17
{

namespace
{

constexpr const char* kInputFile = "day17/day17.input";
constexpr const char* kTestInput = R"(
Register A: 729
Register B: 0
Register C: 0

Program: 0,1,5,4,3,0)";

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
    int64_t a, b, c;
    vector<int> instructions;
    ostream& console;
    size_t ip;
    bool needs_comma;

    friend ostream& operator<<(ostream& os, const Computer& computer);

public:
    Computer(int64_t a, int64_t b, int64_t c, vector<int> instructions, ostream& console)
        : a(a)
        , b(b)
        , c(c)
        , instructions(instructions)
        , console(console)
        , ip(0)
        , needs_comma(false)
    {
    }

    void run()
    {
        size_t ip = 0;

        while (ip < instructions.size())
        {
            dbg() << "IP: " << ip << ", op=" << instructions[ip] << endl;
            switch (instructions[ip])
            {
            case kOpcodeAdv: adv(ip); break;
            case kOpcodeBxl: bxl(ip); break;
            case kOpcodeBst: bst(ip); break;
            case kOpcodeJnz: jnz(ip); break;
            case kOpcodeBxc: bxc(ip); break;
            case kOpcodeOut: out(ip); break;
            case kOpcodeBdv: bdv(ip); break;
            case kOpcodeCdv: cdv(ip); break;
            default:
                throw runtime_error("Invalid opcode");
            }
        }
    }

private:
    void adv(size_t& ip)
    {
        ip++;
        int64_t denom = 1 << get_combo(ip++);
        a = a / denom;
    }

    void bxl(size_t& ip)
    {
        ip++;
        b ^= get_literal(ip++);
    }

    void bst(size_t& ip)
    {
        ip++;
        auto lit = get_combo(ip++);
        b = (lit & 0x7); // keep the lowest three bits
    }

    void jnz(size_t& ip)
    {
        if (a == 0)
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
        b ^= c;
    }

    void out(size_t& ip)
    {
        ip++;
        if (needs_comma)
        {
            console << ',';
        }
        needs_comma = true;

        console << (get_combo(ip++) & 0x7); // combo operand, modulo 8
    }

    void bdv(size_t& ip)
    {
        ip++;
        int64_t denom = 1 << get_combo(ip++);
        b = a / denom;
    }

    void cdv(size_t& ip)
    {
        ip++;
        int64_t denom = 1 << get_combo(ip++);
        c = a / denom;
    }

    int64_t get_literal(size_t operand_index)
    {
        return static_cast<int64_t>(instructions[operand_index]);
    }

    int64_t get_combo(size_t operand_index)
    {
        switch (instructions[operand_index] % 8)
        {
        case 0: return 0;
        case 1: return 1;
        case 2: return 2;
        case 3: return 3;
        case 4: return a;
        case 5: return b;
        case 6: return c;
        case 7: // fallthrough
        default: throw runtime_error("Invalid operand");
        }
    }
};

ostream& operator<<(ostream& os, const Computer& computer)
{
    os << "Register A: " << computer.a << endl;
    os << "Register B: " << computer.b << endl;
    os << "Register C: " << computer.c << endl;
    os << "Instructions: ";
    for (size_t i = 0; i < computer.instructions.size(); i++)
    {
        if (i > 0)
        {
            os << ", ";
        }
        os << computer.instructions[i];
    }
    os << endl;

    return os;
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
    dbg() << comp << endl;
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
    return "TODO";
}

} // namespace day17
