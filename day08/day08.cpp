#include "day08.h"

#include "parsers.h"
#include "point.h"

#include <algorithm>
#include <fstream>
#include <iostream>
#include <memory>
#include <ranges>
#include <sstream>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

using namespace std;

namespace day08
{

namespace
{

constexpr const char* kInputFile = "day08/day08.input";

constexpr const char* kTestInput = R"(............
........0...
.....0......
.......0....
....0.......
......A.....
............
............
........A...
.........A..
............
............)";

unique_ptr<istream> get_input()
{
    if (g_test_input)
    {
        return make_unique<stringstream>(kTestInput);
    }

    return make_unique<ifstream>(kInputFile);
}

struct Puzzle
{
    int width;
    int height;
    unordered_map<char, vector<Point>> stations;
    unordered_set<Point> antinodes;

    bool contains(const Point& p) const
    {
        return p.x() >= 0 && p.x() < width &&
               p.y() >= 0 && p.y() < height;
    }

    void find_antinodes(bool resonant = false)
    {
        for (const auto& [station, _] : stations)
        {
            find_antinodes_for_station(station, resonant);
        }
    }

    void find_antinodes_for_station(char station, bool resonant = false)
    {
        // PRECONDITION: station vector is sorted in ascending order

        auto it = stations.find(station);
        if (it == stations.end())
        {
            return;
        }

        const auto& points = it->second;
        for (size_t i = 0; i < points.size() - 1; ++i)
        {
            for (size_t j = i + 1; j < points.size(); ++j)
            {
                const auto& p1 = points[i];
                const auto& p2 = points[j];

                if (resonant)
                {
                    add_antinode_resonant(p1, p2);
                }
                else
                {
                    Point delta = p2 - p1;

                    add_antinode(p1 - delta);
                    add_antinode(p2 + delta);
                }
            }
        }
    }

    void add_antinode(const Point& p)
    {
        if (contains(p))
        {
            antinodes.insert(p);
        }
    }

    void add_antinode_resonant(const Point& p1, const Point& p2)
    {
        Point delta = p2 - p1;

        Point a = p1;
        while (contains(a))
        {
            antinodes.insert(a);
            a = a - delta;
        }

        a = p2;
        while (contains(a))
        {
            antinodes.insert(a);
            a = a + delta;
        }
    }
};

ostream& operator<<(ostream& out, const Puzzle& p)
{
    for (int y = 0; y < p.height; ++y)
    {
        for (int x = 0; x < p.width; ++x)
        {
            Point point{x, y};
            if (p.antinodes.contains(point))
            {
                out << "X";
            }
            else
            {
                out << ".";
            }
        }
        out << '\n';
    }
    return out;
}

Puzzle read_input()
{
    auto input = get_input();
    
    Puzzle puzzle;

    int num_lines = 0;
    size_t width = 0;
    for (const auto& line : parsers::Lines(*input))
    {
        width = std::max(width, line.size());
        for (size_t i = 0; i < line.size(); i++)
        {
            char c = line[i];
            if (c == '.')
            {
                continue;
            }

            Point p{static_cast<int>(i), num_lines};
            puzzle.stations[c].push_back(p);
        }

        num_lines++;
    }

    puzzle.width = static_cast<int>(width);
    puzzle.height = num_lines;

    return puzzle;
}

} // namespace

string PartOne::solve()
{
    Puzzle p = read_input();
    p.find_antinodes();

    dbg() << p << endl;

    return to_string(p.antinodes.size());
}

string PartTwo::solve()
{
    Puzzle p = read_input();
    p.find_antinodes(true);

    dbg() << p << endl;

    return to_string(p.antinodes.size());
}

} // namespace day08
