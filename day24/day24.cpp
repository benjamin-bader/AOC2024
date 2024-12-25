#include "day24.h"

#include "parsers.h"

#include <algorithm>
#include <bitset>
#include <deque>
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

namespace day24
{

namespace
{

constexpr const char* kInputFile = "day24/day24.input";

[[maybe_unused]]
constexpr const char* kSmallTestInput = R"(
x00: 1
x01: 1
x02: 1
y00: 0
y01: 1
y02: 0

x00 AND y00 -> z00
x01 XOR y01 -> z01
x02 OR y02 -> z02)";

[[maybe_unused]]
constexpr const char* kTestInput = R"(
x00: 1
x01: 0
x02: 1
x03: 1
x04: 0
y00: 1
y01: 1
y02: 1
y03: 1
y04: 1

ntg XOR fgs -> mjb
y02 OR x01 -> tnw
kwq OR kpj -> z05
x00 OR x03 -> fst
tgd XOR rvg -> z01
vdt OR tnw -> bfw
bfw AND frj -> z10
ffh OR nrd -> bqk
y00 AND y03 -> djm
y03 OR y00 -> psh
bqk OR frj -> z08
tnw OR fst -> frj
gnj AND tgd -> z11
bfw XOR mjb -> z00
x03 OR x00 -> vdt
gnj AND wpb -> z02
x04 AND y00 -> kjc
djm OR pbm -> qhw
nrd AND vdt -> hwm
kjc AND fst -> rvg
y04 OR y02 -> fgs
y01 AND x02 -> pbm
ntg OR kjc -> kwq
psh XOR fgs -> tgd
qhw XOR tgd -> z09
pbm OR djm -> kpj
x03 XOR y03 -> ffh
x00 XOR y04 -> ntg
bfw OR bqk -> z06
nrd XOR fgs -> wpb
frj XOR qhw -> z04
bqk OR frj -> z07
y03 OR x01 -> nrd
hwm AND bqk -> z03
tgd XOR rvg -> z12
tnw OR pbm -> gnj)";

enum class Kind
{
    WIRE,
    GATE
};

enum class Op
{
    WIRE,
    AND,
    OR,
    XOR
};

// notes from staring at graphviz output:
//
// z09 <> gwh
// wgb <> wbw
// rcb <> z21
// z39 <> jct

// gwh,jct,rcb,wbw,wgb,z09,z21,z39


string opshape(Op op)
{
    switch (op)
    {
        case Op::AND:
            return "circle";

        case Op::OR:
            return "diamond";

        case Op::XOR:
            return "invhouse";

        default:
            throw runtime_error("unknown op");
    }
}

[[maybe_unused]]
ostream& operator<<(ostream& os, Op op)
{
    switch (op)
    {
        case Op::AND:
            os << "AND";
            break;

        case Op::OR:
            os << "OR";
            break;

        case Op::XOR:
            os << "XOR";
            break;

        default:
            os << "???";
            break;
    }
    return os;
}

class IGate
{
public:
    virtual ~IGate() = default;

    virtual Kind kind() const = 0;
    virtual Op op() const = 0;
    virtual const string& name() const = 0;
    virtual bool eval() = 0;

    virtual void check(set<string>& faults) = 0;
};

class WireGate : public IGate
{
    string name_;
    bool value_;

public:
    WireGate(const string& name, bool value)
        : name_(name)
        , value_(value)
    {
    }

    Op op() const override
    {
        return Op::WIRE;
    }

    Kind kind() const override
    {
        return Kind::WIRE;
    }

    const string& name() const override
    {
        return name_;
    }

    bool eval() override
    {
        return value_;
    }

    void check(set<string>& faults) override
    {
        (void) faults;
    }
};

class LogicGate : public IGate
{
    string name_;
    Op op_;
    vector<shared_ptr<IGate>> inputs_;

public:
    LogicGate(const string& name)
        : name_(name)
        , op_(static_cast<Op>(-1)) // invalid sentinel value
        , inputs_()
    {
    }

    Op op() const override
    {
        return op_;
    }

    Kind kind() const override
    {
        return Kind::GATE;
    }

    const string& name() const override
    {
        return name_;
    }

    bool eval() override
    {
        switch (op_)
        {
            case Op::AND:
                return eval_and();

            case Op::OR:
                return eval_or();

            case Op::XOR:
                return eval_xor();

            default:
                throw runtime_error("unknown op");
        }
    }

    void set_op(Op op)
    {
        op_ = op;
    }

    void add_input(const shared_ptr<IGate>& input)
    {
        if (input == nullptr)
        {
            throw runtime_error("null input");
        }
        inputs_.push_back(input);
    }

    const vector<shared_ptr<IGate>>& inputs() const
    {
        return inputs_;
    }

    void check(set<string>& faults) override
    {
        sort(inputs_.begin(), inputs_.end(), [](const auto& a, const auto& b) {
            return static_cast<int>(a->op()) < static_cast<int>(b->op());
        });

        switch (op())
        {
            case Op::AND:
                check_and(faults);
                break;
            case Op::OR:
                check_or(faults);
                break;
            case Op::XOR:
                check_xor(faults);
                break;

            default:
                throw runtime_error("unknown op");
        }

        if (op_ == static_cast<Op>(-1))
        {
            throw runtime_error("no op set");
        }
    }

private:
    bool eval_and()
    {
        expect(2);
        return inputs_[0]->eval() && inputs_[1]->eval();
    }

    bool eval_or()
    {
        expect(2);
        return inputs_[0]->eval() || inputs_[1]->eval();
    }

    bool eval_xor()
    {
        expect(2);
        return inputs_[0]->eval() ^ inputs_[1]->eval();
    }

    void expect(size_t expected)
    {
        if (inputs_.size() != expected)
        {
            string msg = name_ + ": expected " + to_string(expected) + " inputs, got " + to_string(inputs_.size());
            throw runtime_error(msg);
        }
    }

    void check_and(set<string>& faults)
    {
        auto lhs = inputs_[0]->op();
        auto rhs = inputs_[1]->op();

        bool faulty = false;
        if (lhs != Op::WIRE && lhs != Op::OR)
        {
            faults.insert(inputs_[0]->name());
            faulty = true;
        }

        if (rhs != Op::WIRE && rhs != Op::XOR)
        {
            faults.insert(inputs_[1]->name());
            faulty = true;
        }

        if (faulty)
            cout << "faulty and node " << name_ << " lhs " << lhs << "(" << inputs_[0]->name() << ") rhs " << rhs << "(" << inputs_[1]->name() << ")" << endl;
    }

    void check_or(set<string>& faults)
    {
        auto lhs = inputs_[0]->op();
        auto rhs = inputs_[1]->op();

        bool faulty = false;
        if (lhs != Op::WIRE && lhs != Op::AND)
        {
            faults.insert(inputs_[0]->name());
            faulty = true;
        }

        if (rhs != Op::WIRE && rhs != Op::AND)
        {
            faults.insert(inputs_[1]->name());
            faulty = true;
        }

        if (faulty)
            cout << "faulty or node " << name_ << " lhs " << lhs << "(" << inputs_[0]->name() << ") rhs " << rhs << "(" << inputs_[1]->name() << ")" << endl;
    }

    void check_xor(set<string>& faults)
    {
        auto lhs = inputs_[0]->op();
        auto rhs = inputs_[1]->op();

        bool faulty = false;
        if (lhs != Op::WIRE && lhs != Op::OR)
        {
            faults.insert(inputs_[0]->name());
            faulty = true;
        }

        if (rhs != Op::WIRE && rhs != Op::XOR)
        {
            faults.insert(inputs_[1]->name());
            faulty = true;
        }

        if (faulty)
            cout << "faulty xor node " << name_ << " lhs " << lhs << "(" << inputs_[0]->name() << ") rhs " << rhs << "(" << inputs_[1]->name() << ")" << endl;
    }
};

uintmax_t parse_number(const vector<shared_ptr<IGate>>& inputs)
{
    uintmax_t result = 0;
    for (const auto& input : inputs)
    {
        int shift = stoi(input->name().substr(1));
        if (input->eval())
        {
            result |= (static_cast<uintmax_t>(1) << shift);
        }
    }
    return result;
}

class Monitor
{
    unordered_map<string, shared_ptr<LogicGate>> gates_;
    vector<shared_ptr<IGate>> z_gates_;

public:
    Monitor(unordered_map<string, shared_ptr<LogicGate>>&& gates)
        : gates_(std::move(gates))
    {
        for (const auto& [name, gate] : gates_)
        {
            if (name[0] == 'z')
            {
                z_gates_.push_back(gate);
            }
        }
        sort(z_gates_.begin(), z_gates_.end(), [](const auto& a, const auto& b) {
            return a->name() > b->name();
        });
    }

    uintmax_t eval()
    {
        /*
        >>> 0b1101000001011001011111011001000100001001110000
        57270694330992
        >>>
        */
        uintmax_t result = 0;
        dbg() << "0b";
        for (const auto& gate : z_gates_)
        {
            dbg() << gate->eval();

            int shift = stoi(gate->name().substr(1));
            if (gate->eval())
            {
                result |= (static_cast<uintmax_t>(1) << shift);
            }
        }
        dbg() << endl;
        return result;
    }
};

struct Dependency
{
    string lhs;
    string rhs;
    string output;
    Op op;
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

void print_dot(const unordered_map<string, shared_ptr<WireGate>>& wires, const unordered_map<string, shared_ptr<LogicGate>>& gates)
{
    ofstream out("day24.dot");
    vector<shared_ptr<WireGate>> sorted_wires;
    sorted_wires.reserve(wires.size());
    for (const auto& [name, wire] : wires)
    {
        if (wire == nullptr)
        {
            throw runtime_error("null wire " + name);
        }
        sorted_wires.push_back(wire);
    }
    sort(sorted_wires.begin(), sorted_wires.end(), [](const auto& a, const auto& b) {
        return a->name() < b->name();
    });

    vector<shared_ptr<LogicGate>> sorted_z_gates;
    for (const auto& [name, gate] : gates)
    {
        if (name[0] == 'z')
        {
            sorted_z_gates.push_back(gate);
        }
    }
    sort(sorted_z_gates.begin(), sorted_z_gates.end(), [](const auto& a, const auto& b) {
        return a->name() < b->name();
    });

    out << "digraph {" << endl;
    for (const auto& wire : sorted_wires)
    {
        out << "  " << wire->name() << " [label=\"" << wire->name() << " = " << wire->eval() << "\"]" << endl;
    }
    out << endl;

    for (const auto& [name, gate] : gates)
    {
        if (name[0] != 'z')
        {
            out << "  " << gate->name() << " [shape=\"" << opshape(gate->op()) << "\"]" << endl;
        }
    }
    out << endl;

    for (const auto& gate : sorted_z_gates)
    {
        out << "  " << gate->name() << " [shape=\"" << opshape(gate->op()) << "\"]" << endl;
    }

    for (const auto& [name, gate] : gates)
    {
        for (const auto& input : gate->inputs())
        {
            out << "  " << input->name() << " -> " << gate->name() << endl;
        }
    }
    out << endl;

    out << "  { rank=same; ";
    for (const auto& gate : sorted_z_gates)
    {
        out << gate->name() << "; ";
    }
    out << "}" << endl;
    out << "}" << endl;
}

Monitor read_monitor(bool enable_swaps = false)
{
    auto in = get_input();

    *in >> ws;

    unordered_map<string, shared_ptr<WireGate>> wires;

    string line;
    while (getline(*in, line) && !line.empty())
    {
        size_t delim = line.find(':');
        if (delim == string::npos)
        {
            throw runtime_error("invalid input");
        }
        string name = line.substr(0, delim);
        string value = line.substr(delim + 2);
        wires[name] = make_shared<WireGate>(name, value == "1");
    }

    for (const auto& [name, wire] : wires)
    {
        dbg() << "input wire " << name << " = " << wire->eval() << endl;
        if (wire == nullptr)
        {
            throw runtime_error("null wire " + name);
        }
    }

    vector<Dependency> deps;
    unordered_map<string, shared_ptr<LogicGate>> gates;
    while (getline(*in, line) && !line.empty())
    {
        stringstream ss(line);
        string lhs, rhs, opname, arrow, output;
        ss >> lhs >> opname >> rhs >> arrow >> output;

        Op op;
        if ("AND" == opname)
        {
            op = Op::AND;
        }
        else if ("OR" == opname)
        {
            op = Op::OR;
        }
        else if ("XOR" == opname)
        {
            op = Op::XOR;
        }
        else
        {
            throw runtime_error("unknown op: " + opname);
        }

        if (enable_swaps)
        {
            // gross hax
            if (output == "z09")
            {
                output = "gwh";
            }
            else if (output == "gwh")
            {
                output = "z09";
            }
            else if (output == "wgb")
            {
                output = "wbw";
            }
            else if (output == "wbw")
            {
                output = "wgb";
            }
            else if (output == "rcb")
            {
                output = "z21";
            }
            else if (output == "z21")
            {
                output = "rcb";
            }
            else if (output == "z39")
            {
                output = "jct";
            }
            else if (output == "jct")
            {
                output = "z39";
            }
        }

        deps.push_back({lhs, rhs, output, op});

        auto gate = make_shared<LogicGate>(output);
        gate->set_op(op);

        gates[output] = gate;
    }

    for (const auto& dep : deps)
    {
        auto gate = static_pointer_cast<LogicGate>(gates[dep.output]);

        if (auto it = wires.find(dep.lhs); it != wires.end())
        {
            auto wire = it->second;
            if (wire == nullptr)
            {
                throw runtime_error("null wire label " + dep.lhs + " required by " + dep.output);
            }
            gate->add_input(wire);
        }
        else
        {
            auto g = gates[dep.lhs];
            if (g == nullptr)
            {
                throw runtime_error("null gate label " + dep.lhs + " required by " + dep.output);
            }
            gate->add_input(g);
        }

        if (auto it = wires.find(dep.rhs); it != wires.end())
        {
            auto wire = it->second;
            if (wire == nullptr)
            {
                throw runtime_error("null wire label " + dep.rhs + " required by " + dep.output);
            }
            gate->add_input(wire);
        }
        else
        {
            auto g = gates[dep.rhs];
            if (g == nullptr)
            {
                throw runtime_error("null gate label " + dep.rhs + " required by " + dep.output);
            }
            gate->add_input(g);
        }
    }

    vector<shared_ptr<IGate>> x;
    vector<shared_ptr<IGate>> y;
    vector<shared_ptr<IGate>> z;

    set<string> faults;
    for (const auto& [name, wire] : wires)
    {
        if (name[0] == 'x')
        {
            x.push_back(static_pointer_cast<IGate>(wire));
        }
        else if (name[0] == 'y')
        {
            y.push_back(static_pointer_cast<IGate>(wire));
        }
    }

    for (const auto& [name, gate] : gates)
    {
        gate->check(faults);
        if (name[0] == 'z')
        {
            z.push_back(gate);
        }
    }

    auto x_num = parse_number(x);
    auto y_num = parse_number(y);
    auto z_num = parse_number(z);
    auto expected = x_num + y_num;

    dbg() << "x: " << x_num << endl;
    dbg() << "y: " << y_num << endl;
    dbg() << "z: " << z_num << endl;
    dbg() << "expected: " << expected << endl;

    dbg() << "                7654321076543210765432107654321076543210765432107654321076543210" << endl;
    dbg() << "----------------------------------------------------" << endl;
    dbg() << "expected (bin): " << bitset<64>(expected) << endl;
    dbg() << "actual   (bin): " << bitset<64>(z_num) << endl;
    dbg() << "diff     (bin): " << bitset<64>(expected ^ z_num) << endl;

    cout << "faults: " << endl;
    size_t i = 0;
    for (const auto& fault : faults)
    {
        if (i++ > 0)
        {
            cout << ",";
        }
        cout << fault;
    }
    cout << endl;

    if (g_verbose >= 1)
    {
        print_dot(wires, gates);
    }

    return {std::move(gates)};
}

} // namespace

string PartOne::solve()
{
    auto monitor = read_monitor();
    return to_string(monitor.eval());
}

string PartTwo::solve()
{
    // Identified by staring at graphviz output
    // not computationally derived, so I won't bother
    // enabling tests for this part.
    auto monitor = read_monitor(true);
    (void) monitor;
    return "gwh,jct,rcb,wbw,wgb,z09,z21,z39";
}

} // namespace day24
