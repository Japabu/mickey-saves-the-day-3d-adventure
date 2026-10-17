// Differential probes of RuntimeLib functions (the cases: port/difftest/cases.py).
#include "../test.h"
#include "MfPath2.h"

using port_test::get;
using port_test::put;

// in: the point count, then x, y of each point; out: each point's heading after Update
PROBE(MfPath2_Update)
{
    int n = get<int>(in, 0);
    MfPath2 path;
    path.SetNumPoints(n);
    for (int i = 0; i < n; i++)
        path.Point(i).translation() = MfVector2(get<float>(in, 4 + 8 * i), get<float>(in, 8 + 8 * i));
    path.Update();
    for (int i = 0; i < n; i++)
        put(out, path.Point(i).rotation().angle());
}
