// MBPoint3.h - a 3D point.
#ifndef MBENGINE_MBPOINT3_H
#define MBENGINE_MBPOINT3_H

#include "MBPointStruct3.h"

class MBPoint3 : public MBPointStruct3 {
public:
    MBPoint3(long ax, long ay, long az) { x = ax; y = ay; z = az; }
    MBPoint3(void) { x = y = z = 0; }
    MBPoint3 operator+(const MBPoint3 &p) const { return MBPoint3(x + p.x, y + p.y, z + p.z); }
    MBPoint3 &operator+=(const MBPoint3 &p) { x += p.x; y += p.y; z += p.z; return *this; }
};

#endif
