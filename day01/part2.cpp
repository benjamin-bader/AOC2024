#include <fstream>
#include <iostream>
#include <map>
#include <string>
#include <vector>

using namespace std;

int main(int argc, char** argv)
{
    vector<int> left;
    map<int, int> right_column_counts;
    ifstream input("day01/part2.input");

    int x, y;
    while (input >> x >> y)
    {
        left.push_back(x);
        right_column_counts[y]++;
    }

    long score = 0;
    for (int n : left)
    {
        score += (n * right_column_counts[n]);
    }

    cout << score << endl;

    return 0;
}