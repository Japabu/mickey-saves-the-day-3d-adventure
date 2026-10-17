// MBPoint.h - a 2D point.
#ifndef MBENGINE_MBPOINT_H
#define MBENGINE_MBPOINT_H

#include "MBPointStruct.h"

class MBPoint : public MBPointStruct {
public:
    MBPoint(void) { x = 0; y = 0; }
    MBPoint(long ax, long ay) { x = ax; y = ay; }
    MBPoint operator+(const MBPoint &p) const { return MBPoint(x + p.x, y + p.y); }
    MBPoint operator-(const MBPoint &p) const { return MBPoint(x - p.x, y - p.y); }
    bool operator==(const MBPoint &p) const {
        if (x == p.x && y == p.y)
            return true;
        return false;
    }
};

#endif
