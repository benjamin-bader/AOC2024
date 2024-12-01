#include <algorithm>
#include <fstream>
#include <iostream>
#include <string>
#include <utility>
#include <vector>

using namespace std;

int main(int argc, char** argv)
{
    string line;
    vector<int> left, right;
    ifstream input("day01\\part1.input");

    while (getline(input, line))
    {
        int number1, number2;
        sscanf(line.c_str(), "%d   %d", &number1, &number2);
        left.push_back(number1);
        right.push_back(number2);
    }

    sort(left.begin(), left.end());
    sort(right.begin(), right.end());

    long sum = 0;
    for (size_t i = 0; i < left.size(); i++)
    {
        int x = left[i];
        int y = right[i];
        int diff = abs(x - y);
        sum += diff;
    }

    cout << sum << endl;

    return 0;
}