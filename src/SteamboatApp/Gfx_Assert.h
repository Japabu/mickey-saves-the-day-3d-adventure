#pragma once
#include <iostream.h>

// The graphics library's assertion: reports the failed condition to cerr and
// leaves the function. The original passes the build machine's __FILE__ and
// the __LINE__; the units give them as literals.
#define GFX_ASSERT(condition, file, line)                                                                   \
    if (!(condition)) {                                                                                     \
        cerr << "Assertion failed: (" << #condition << ") File:" << file << " Line:" << line << endl;     \
        return;                                                                                             \
    }
