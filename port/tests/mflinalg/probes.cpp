// Differential probes of mflinalg (the cases: port/difftest/cases.py).
// port-test-sources: MfTransform3.cpp MfRotation3.cpp MfVector3.cpp
#include "../test.h"
#include "MfTransform3.h"

using port_test::get;
using port_test::put;

static MfTransform3 transform(const std::vector<uint8_t> &in, size_t off)
{
    MfTransform3 t;
    t.translation() = MfVector3(get<float>(in, off), get<float>(in, off + 4), get<float>(in, off + 8));
    t.rotation().setValue(get<float>(in, off + 12), get<float>(in, off + 16), get<float>(in, off + 20),
                          get<float>(in, off + 24));
    return t;
}

static void put_vector(std::vector<uint8_t> &out, const MfVector3 &v)
{
    put(out, v.x());
    put(out, v.y());
    put(out, v.z());
}

// in: a transform (translation, w x y z) and a point; out: the transformed point
PROBE(MfTransform3_times_vector)
{
    MfTransform3 t = transform(in, 0);
    MfVector3 p(get<float>(in, 28), get<float>(in, 32), get<float>(in, 36));
    put_vector(out, t * p);
}

// in: a transform; out: its inverse
PROBE(MfTransform3_inverse)
{
    MfTransform3 inv = ~transform(in, 0);
    put_vector(out, inv.translation());
    float w, x, y, z;
    inv.rotation().getValue(w, x, y, z);
    put(out, w);
    put(out, x);
    put(out, y);
    put(out, z);
}

// in: a rotation (w x y z) and a vector; out: the rotated vector
PROBE(MfRotation3_times_vector)
{
    MfRotation3 r;
    r.setValue(get<float>(in, 0), get<float>(in, 4), get<float>(in, 8), get<float>(in, 12));
    MfVector3 p(get<float>(in, 16), get<float>(in, 20), get<float>(in, 24));
    put_vector(out, r * p);
}
