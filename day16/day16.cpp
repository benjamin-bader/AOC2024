#include "day16.h"

#include "board.h"
#include "point.h"
#include "turtle.h"

#include <algorithm>
#include <deque>
#include <exception>
#include <fstream>
#include <iostream>
#include <memory>
#include <numeric>
#include <queue>
#include <ranges>
#include <sstream>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

using namespace std;

namespace day16
{

namespace
{

constexpr const char* kInputFile = "day16/day16.input";

[[maybe_unused]]
constexpr const char* kTestInput = R"(
###############
#.......#....E#
#.#.###.#.###.#
#.....#.#...#.#
#.###.#####.#.#
#.#.#.......#.#
#.#.#####.###.#
#...........#.#
###.#.#####.#.#
#...#.....#.#.#
#.#.#.###.#.#.#
#.....#...#.#.#
#.###.#.#.#.#.#
#S..#.....#...#
###############
)";

[[maybe_unused]]
constexpr const char* kSecondExample = R"(
#################
#...#...#...#..E#
#.#.#.#.#.#.#.#.#
#.#.#.#...#...#.#
#.#.#.#.###.#.#.#
#...#.#.#.....#.#
#.#.#.#.#.#####.#
#.#...#.#.#.....#
#.#.#####.#.###.#
#.#.#.......#...#
#.#.###.#####.###
#.#.#...#.....#.#
#.#.#.#####.###.#
#.#.#.........#.#
#.#.#.#########.#
#S#.............#
#################
)";

[[maybe_unused]]
unique_ptr<istream> get_input()
{
    if (g_test_input)
    {
        return make_unique<stringstream>(kSecondExample);
    }

    return make_unique<ifstream>(kInputFile);
}

Board read_board()
{
    auto input = get_input();

    Board b;

    *input >> b;

    return b;
}

struct Edge
{
    Point from;
    Point to;
    Point dir;
    int length;
};

class Graph
{
    Point start;
    Point end;
    unordered_map<Point, vector<Edge>> edges;
    Board board;

    unordered_map<Turtle, size_t> dist;
    unordered_map<Turtle, vector<Turtle>> prev;
    Turtle end_turtle;

public:
    Graph(const Board& board)
        : start()
        , end()
        , edges()
        , board(board)
    {
        for (auto p : board.all_points())
        {
            if (board[p] == 'S')
            {
                start = p;
            }
            else if (board[p] == 'E')
            {
                end = p;
            }
        }

        auto can_move_to_side = [&board](Point p, Point d) {
            Point left = p + Dir::turn_left(d);
            Point right = p + Dir::turn_right(d);

            return (board.in_bounds(left) && board[left] != '#') || (board.in_bounds(right) && board[right] != '#');
        };

        auto can_go_straight = [&board](Point p, Point d) {
            Point next = p + d;
            return board.in_bounds(next) && board[next] != '#';
        };

        // BFS to build the graph
        deque<Point> q{start};
        unordered_set<Point> visited;
        while (!q.empty())
        {
            Point cur = q.front();
            q.pop_front();

            if (const auto [_, inserted] = visited.insert(cur); !inserted)
            {
                continue;
            }

            for (const auto& d : Dir::CARDINALS)
            {
                auto n = cur + d;
                if (!board.in_bounds(n) || board[n] == '#')
                {
                    continue;
                }

                int weight = 1;

                Point new_p = n;
                while (can_go_straight(new_p, d) && !can_move_to_side(new_p, d))
                {
                    weight++;
                    new_p += d;
                }

                edges[cur].push_back(Edge{cur, new_p, d, weight});
                q.push_back(new_p);
            }
        }
    }

    void draw(ostream& out, const unordered_set<Point>& path)
    {
        for (const auto& p : board.all_points())
        {
            if (path.contains(p))
            {
                out << "\033[1;33m" << "@" << "\033[0m";
            }
            else
            {
                out << board[p];
            }

            if (p.x() == board.num_cols() - 1)
            {
                out << '\n';
            }
        }
    }

    pair<size_t, vector<Edge>> find_shortest_path()
    {
        auto end_turtle = dijkstra();

        vector<Edge> path;
        Turtle to = end_turtle;
        while (to.position() != start)
        {
            Turtle from = prev[to].front(); // just pick any old turtle, they're all the same cost
            for (const auto& edge : edges.at(from.position()))
            {
                if (edge.to == to.position())
                {
                    path.push_back(edge);
                    to = from;
                    break;
                }
            }
        }

        reverse(path.begin(), path.end());

        for (const auto& edge : path)
        {
            dbg() << edge.from << " -> " << edge.to << " (" << dist[{edge.to, edge.dir}] << ")" << endl;
        }

        return {dist[end_turtle], path};
    }

    pair<size_t, unordered_set<Point>> find_all_shortest_path_points()
    {
        auto end_turtle = dijkstra();

        unordered_set<Point> result;
        deque<Turtle> q{end_turtle};

        vector<pair<Point, Point>> edges;
        while (!q.empty())
        {
            Turtle cur = q.front();
            q.pop_front();

            result.insert(cur.position());
            for (auto& t : prev[cur])
            {
                edges.push_back({cur.position(), t.position()});
                q.push_back(t);
            }
        }

        for (const auto& [lhs, rhs] : edges)
        {
            Point dir{clamp(rhs.x() - lhs.x(), -1, 1), clamp(rhs.y() - lhs.y(), -1, 1)};

            Point p = lhs;
            do
            {
                result.insert(p);
                p += dir;
            } while (p != rhs);
        }

        return {dist[end_turtle], result};
    }

private:
    Turtle dijkstra()
    {
        if (dist.size() > 0)
        {
            return end_turtle;
        }

        // pq is a min-queue - smallest element first.
        // pairs compare lexicographically, so weight comes first.
        priority_queue<pair<size_t, Turtle>, vector<pair<size_t, Turtle>>, greater<>> pq;

        dist.reserve(edges.size() * 4);
        for (const auto& [p, _] : edges)
        {
            for (const auto& d : Dir::CARDINALS)
            {
                dist[{p, d}] = numeric_limits<size_t>::max();
            }
        }

        pq.push({0, {start, Dir::RIGHT}});
        dist[{start, Dir::RIGHT}] = 0;

        while (!pq.empty())
        {
            auto [w, p] = pq.top();
            pq.pop();

            for (const auto& edge : edges.at(p.position()))
            {
                int turns = 0;
                bool dx = p.direction().x() != edge.dir.x();
                bool dy = p.direction().y() != edge.dir.y();
                if (dx && dy)
                {
                    turns = 1;
                }
                else if (dx || dy)
                {
                    turns = 2;
                }

                size_t alt = dist[p] + edge.length + (turns * 1000);
                if (alt == dist[{edge.to, edge.dir}])
                {
                    prev[{edge.to, edge.dir}].push_back(p);
                }
                else if (alt < dist[{edge.to, edge.dir}])
                {
                    dist[{edge.to, edge.dir}] = alt;
                    prev[{edge.to, edge.dir}] = {p};
                    pq.push({alt, {edge.to, edge.dir}});
                }
            }
        }

        size_t cost = numeric_limits<size_t>::max();
        for (const auto& [d, c] : dist)
        {
            if (d.position() != end)
            {
                continue;
            }

            if (c < cost)
            {
                end_turtle = d;
                cost = c;
            }
        }

        if (cost == numeric_limits<size_t>::max())
        {
            throw logic_error{"no path found"};
        }

        return end_turtle;
    }
};

} // namespace

string PartOne::solve()
{
    auto board = read_board();
    Graph graph(board);

    auto dij = graph.find_shortest_path();

    unordered_set<Point> path;
    for (auto edge : dij.second)
    {
        path.insert(edge.from);
        path.insert(edge.to);
    }

    graph.draw(dbg(), path);
    dbg() << "path has " << path.size() << " points" << endl;

    return to_string(dij.first);
}

string PartTwo::solve()
{
    auto board = read_board();
    Graph graph(board);

    auto [cost, points] = graph.find_all_shortest_path_points();

    graph.draw(dbg(), points);

    dbg() << "I have a cost of " << cost << " and " << points.size() << " points" << endl;

    return to_string(points.size());
}

} // namespace day16
