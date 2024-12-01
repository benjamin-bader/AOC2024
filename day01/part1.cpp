#include <algorithm>
#include <fstream>
#include <iostream>
#include <numeric>
#include <ranges>
#include <vector>

using namespace std;

int main(int argc, char** argv)
{
    vector<int> left, right;
    ifstream input("day01/part1.input");

    int x, y;
    while (input >> x >> y)
    {
        left.push_back(x);
        right.push_back(y);
    }

    sort(left.begin(), left.end());
    sort(right.begin(), right.end());

    auto diff = [](int x, int y) { return abs(x - y); };
    auto xs = views::zip_transform(diff, left, right);
    cout << accumulate(xs.begin(), xs.end(), 0L) << endl;

    return 0;
}