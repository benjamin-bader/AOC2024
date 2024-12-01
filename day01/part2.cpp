#include <fstream>
#include <iostream>
#include <map>
#include <string>
#include <vector>

using namespace std;

int main(int argc, char** argv)
{
    string line;
    vector<int> left;
    map<int, int> right_column_counts;
    ifstream input("day01\\part2.input");

    while (getline(input, line))
    {
        int number1, number2;
        sscanf(line.c_str(), "%d %d", &number1, &number2);

        left.push_back(number1);

        if (right_column_counts.find(number2) == right_column_counts.end())
        {
            right_column_counts[number2] = 1;
        }
        else
        {
            right_column_counts[number2]++;
        }
    }

    long score = 0;
    for (int n : left)
    {
        auto it = right_column_counts.find(n);
        if (it != right_column_counts.end())
        {
            score += (n * it->second);
        }
    }

    cout << score << endl;

    return 0;
}