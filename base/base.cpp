#include "base.h"

#include <iostream>

using namespace std;

bool g_test_input = false;
int g_verbose = 0;

ostream& dbg(LogLevel level)
{
    if (g_verbose >= static_cast<int>(level))
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
