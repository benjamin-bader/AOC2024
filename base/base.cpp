#include "base.h"

#include <iostream>

using namespace std;

bool g_test_input = false;
bool g_verbose = false;

ostream& dbg()
{
    if (g_verbose)
    {
        return cout;
    }
    else
    {
        // no-op stream
        static ostream null_stream{nullptr};
        return null_stream;
    }
}
