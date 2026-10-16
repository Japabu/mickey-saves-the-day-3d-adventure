// Differential probes of SteamboatApp functions (the cases: port/difftest/cases.py).
#include "../test.h"
#include "EAIRefFrame.h"

using port_test::get;
using port_test::put;

static Vec3<float> vec3(const std::vector<uint8_t> &in, size_t off)
{
    return Vec3<float>(get<float>(in, off), get<float>(in, off + 4), get<float>(in, off + 8));
}

static void put3(std::vector<uint8_t> &out, const Vec3<float> &v)
{
    for (int i = 0; i < 3; i++)
        put(out, v.e[i]);
}

// in: At, From, Up, the new up (12 floats); out: At, From, Up after setUpVec
PROBE(EAIRefFrame_validateNewUpVec)
{
    EAIRefFrame frame(vec3(in, 0), vec3(in, 12), vec3(in, 24));
    frame.setUpVec(vec3(in, 36));
    put3(out, frame.getAtPoint());
    put3(out, frame.getFromPoint());
    put3(out, frame.getUpVec());
}
